
/***********************************************************************************
File:				DmacWrapperRegs.h
Module:				dmacWrapper
SOC Revision:		
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
#define	REG_DMAC_WRAPPER_DMA1_BA_REG                         (DMAC_WRAPPER_BASE_ADDRESS + 0x800)
#define	REG_DMAC_WRAPPER_DMA1_CONFIG_REG                     (DMAC_WRAPPER_BASE_ADDRESS + 0x804)
#define	REG_DMAC_WRAPPER_DMA1_EXTERNAL_REQ_REG               (DMAC_WRAPPER_BASE_ADDRESS + 0x808)
#define	REG_DMAC_WRAPPER_DMA1_ABORT_REG                      (DMAC_WRAPPER_BASE_ADDRESS + 0x80C)
#define	REG_DMAC_WRAPPER_DMA1_MISC_REG                       (DMAC_WRAPPER_BASE_ADDRESS + 0x810)
#define	REG_DMAC_WRAPPER_DMA1_JOB_PUSH_WHILE_FULL_CLR_IND    (DMAC_WRAPPER_BASE_ADDRESS + 0x814)
#define	REG_DMAC_WRAPPER_DMA1_JUNK_BASE_ADDR_REG             (DMAC_WRAPPER_BASE_ADDRESS + 0x818)
#define	REG_DMAC_WRAPPER_DMA1_STAT0_IND_REG                  (DMAC_WRAPPER_BASE_ADDRESS + 0x81C)
#define	REG_DMAC_WRAPPER_DMA1_STAT1_IND_REG                  (DMAC_WRAPPER_BASE_ADDRESS + 0x820)
#define	REG_DMAC_WRAPPER_TX1_SENDER_ALIGNER_EN               (DMAC_WRAPPER_BASE_ADDRESS + 0x824)
#define	REG_DMAC_WRAPPER_DMA0_BA_REG                         (DMAC_WRAPPER_BASE_ADDRESS + 0x900)
#define	REG_DMAC_WRAPPER_DMA0_CONFIG_REG                     (DMAC_WRAPPER_BASE_ADDRESS + 0x904)
#define	REG_DMAC_WRAPPER_DMA0_EXTERNAL_REQ_REG               (DMAC_WRAPPER_BASE_ADDRESS + 0x908)
#define	REG_DMAC_WRAPPER_DMA0_ABORT_REG                      (DMAC_WRAPPER_BASE_ADDRESS + 0x90C)
#define	REG_DMAC_WRAPPER_DMA0_MISC0_REG                      (DMAC_WRAPPER_BASE_ADDRESS + 0x910)
#define	REG_DMAC_WRAPPER_DMA0_MISC1_REG                      (DMAC_WRAPPER_BASE_ADDRESS + 0x914)
#define	REG_DMAC_WRAPPER_DMA0_JOB_PUSH_WHILE_FULL_CLR_IND    (DMAC_WRAPPER_BASE_ADDRESS + 0x918)
#define	REG_DMAC_WRAPPER_DMA0_JUNK_BASE_ADDR_REG             (DMAC_WRAPPER_BASE_ADDRESS + 0x920)
#define	REG_DMAC_WRAPPER_DMA0_STAT0_IND_REG                  (DMAC_WRAPPER_BASE_ADDRESS + 0x924)
#define	REG_DMAC_WRAPPER_DMA0_STAT1_IND_REG                  (DMAC_WRAPPER_BASE_ADDRESS + 0x928)
#define	REG_DMAC_WRAPPER_TX0_SENDER_ALIGNER_EN               (DMAC_WRAPPER_BASE_ADDRESS + 0x92C)
#define	REG_DMAC_WRAPPER_DMA0_CH0_RECOVERY_REG               (DMAC_WRAPPER_BASE_ADDRESS + 0x930)
#define	REG_DMAC_WRAPPER_DMA0_CH1_RECOVERY_REG               (DMAC_WRAPPER_BASE_ADDRESS + 0x934)
#define	REG_DMAC_WRAPPER_DMA0_STAT2_IND_REG                  (DMAC_WRAPPER_BASE_ADDRESS + 0x938)
#define	REG_DMAC_WRAPPER_AGENT_0_INT_MASK_REG                (DMAC_WRAPPER_BASE_ADDRESS + 0xA00)
#define	REG_DMAC_WRAPPER_AGENT_0_PEND_INT_REG                (DMAC_WRAPPER_BASE_ADDRESS + 0xA04)
#define	REG_DMAC_WRAPPER_AGENT_0_INT_CLEAR_REG               (DMAC_WRAPPER_BASE_ADDRESS + 0xA08)
#define	REG_DMAC_WRAPPER_AGENT_1_INT_MASK_REG                (DMAC_WRAPPER_BASE_ADDRESS + 0xA10)
#define	REG_DMAC_WRAPPER_AGENT_1_PEND_INT_REG                (DMAC_WRAPPER_BASE_ADDRESS + 0xA14)
#define	REG_DMAC_WRAPPER_AGENT_1_INT_CLEAR_REG               (DMAC_WRAPPER_BASE_ADDRESS + 0xA18)
#define	REG_DMAC_WRAPPER_AGENT_2_INT_MASK_REG                (DMAC_WRAPPER_BASE_ADDRESS + 0xA20)
#define	REG_DMAC_WRAPPER_AGENT_2_PEND_INT_REG                (DMAC_WRAPPER_BASE_ADDRESS + 0xA24)
#define	REG_DMAC_WRAPPER_AGENT_2_INT_CLEAR_REG               (DMAC_WRAPPER_BASE_ADDRESS + 0xA28)
#define	REG_DMAC_WRAPPER_AGENT_3_INT_MASK_REG                (DMAC_WRAPPER_BASE_ADDRESS + 0xA30)
#define	REG_DMAC_WRAPPER_AGENT_3_PEND_INT_REG                (DMAC_WRAPPER_BASE_ADDRESS + 0xA34)
#define	REG_DMAC_WRAPPER_AGENT_3_INT_CLEAR_REG               (DMAC_WRAPPER_BASE_ADDRESS + 0xA38)
#define	REG_DMAC_WRAPPER_AGENT_4_INT_MASK_REG                (DMAC_WRAPPER_BASE_ADDRESS + 0xA40)
#define	REG_DMAC_WRAPPER_AGENT_4_PEND_INT_REG                (DMAC_WRAPPER_BASE_ADDRESS + 0xA44)
#define	REG_DMAC_WRAPPER_AGENT_4_INT_CLEAR_REG               (DMAC_WRAPPER_BASE_ADDRESS + 0xA48)
#define	REG_DMAC_WRAPPER_AGENT_5_INT_MASK_REG                (DMAC_WRAPPER_BASE_ADDRESS + 0xA50)
#define	REG_DMAC_WRAPPER_AGENT_5_PEND_INT_REG                (DMAC_WRAPPER_BASE_ADDRESS + 0xA54)
#define	REG_DMAC_WRAPPER_AGENT_5_INT_CLEAR_REG               (DMAC_WRAPPER_BASE_ADDRESS + 0xA58)
#define	REG_DMAC_WRAPPER_AGENT_6_INT_MASK_REG                (DMAC_WRAPPER_BASE_ADDRESS + 0xA60)
#define	REG_DMAC_WRAPPER_AGENT_6_PEND_INT_REG                (DMAC_WRAPPER_BASE_ADDRESS + 0xA64)
#define	REG_DMAC_WRAPPER_AGENT_6_INT_CLEAR_REG               (DMAC_WRAPPER_BASE_ADDRESS + 0xA68)
#define	REG_DMAC_WRAPPER_AGENT_7_INT_MASK_REG                (DMAC_WRAPPER_BASE_ADDRESS + 0xA70)
#define	REG_DMAC_WRAPPER_AGENT_7_PEND_INT_REG                (DMAC_WRAPPER_BASE_ADDRESS + 0xA74)
#define	REG_DMAC_WRAPPER_AGENT_7_INT_CLEAR_REG               (DMAC_WRAPPER_BASE_ADDRESS + 0xA78)
#define	REG_DMAC_WRAPPER_RX0_JOB_DONE_LAST_ACC               (DMAC_WRAPPER_BASE_ADDRESS + 0xB00)
#define	REG_DMAC_WRAPPER_RX0_JOB_DONE_LAST_ACC_DEC           (DMAC_WRAPPER_BASE_ADDRESS + 0xB04)
#define	REG_DMAC_WRAPPER_RX0_JOB_DONE_LAST_ACC_CLR           (DMAC_WRAPPER_BASE_ADDRESS + 0xB08)
#define	REG_DMAC_WRAPPER_RX1_JOB_DONE_LAST_ACC               (DMAC_WRAPPER_BASE_ADDRESS + 0xB10)
#define	REG_DMAC_WRAPPER_RX1_JOB_DONE_LAST_ACC_DEC           (DMAC_WRAPPER_BASE_ADDRESS + 0xB14)
#define	REG_DMAC_WRAPPER_RX1_JOB_DONE_LAST_ACC_CLR           (DMAC_WRAPPER_BASE_ADDRESS + 0xB18)
#define	REG_DMAC_WRAPPER_DMA1_CH0_CONFIG_REG                 (DMAC_WRAPPER_BASE_ADDRESS + 0xC00)
#define	REG_DMAC_WRAPPER_DMA1_CH0_FIFO_PUSH_SRC_ADDR         (DMAC_WRAPPER_BASE_ADDRESS + 0xC04)
#define	REG_DMAC_WRAPPER_DMA1_CH0_FIFO_PUSH_DEST_ADDR        (DMAC_WRAPPER_BASE_ADDRESS + 0xC08)
#define	REG_DMAC_WRAPPER_DMA1_CH0_FIFO_PUSH_C_REG            (DMAC_WRAPPER_BASE_ADDRESS + 0xC0C)
#define	REG_DMAC_WRAPPER_DMA1_CH0_FIFO_PUSH_MPDUP_REG        (DMAC_WRAPPER_BASE_ADDRESS + 0xC10)
#define	REG_DMAC_WRAPPER_DMA1_CH1_CONFIG_REG                 (DMAC_WRAPPER_BASE_ADDRESS + 0xC20)
#define	REG_DMAC_WRAPPER_DMA1_CH1_FIFO_PUSH_SRC_ADDR         (DMAC_WRAPPER_BASE_ADDRESS + 0xC24)
#define	REG_DMAC_WRAPPER_DMA1_CH1_FIFO_PUSH_DEST_ADDR        (DMAC_WRAPPER_BASE_ADDRESS + 0xC28)
#define	REG_DMAC_WRAPPER_DMA1_CH1_FIFO_PUSH_C_REG            (DMAC_WRAPPER_BASE_ADDRESS + 0xC2C)
#define	REG_DMAC_WRAPPER_DMA1_CH1_FIFO_PUSH_MPDUP_REG        (DMAC_WRAPPER_BASE_ADDRESS + 0xC30)
#define	REG_DMAC_WRAPPER_DMA1_CH2_CONFIG_REG                 (DMAC_WRAPPER_BASE_ADDRESS + 0xC40)
#define	REG_DMAC_WRAPPER_DMA1_CH2_FIFO_PUSH_SRC_ADDR         (DMAC_WRAPPER_BASE_ADDRESS + 0xC44)
#define	REG_DMAC_WRAPPER_DMA1_CH2_FIFO_PUSH_DEST_ADDR        (DMAC_WRAPPER_BASE_ADDRESS + 0xC48)
#define	REG_DMAC_WRAPPER_DMA1_CH2_FIFO_PUSH_C_REG            (DMAC_WRAPPER_BASE_ADDRESS + 0xC4C)
#define	REG_DMAC_WRAPPER_DMA1_CH3_CONFIG_REG                 (DMAC_WRAPPER_BASE_ADDRESS + 0xC60)
#define	REG_DMAC_WRAPPER_DMA1_CH3_FIFO0_PUSH_SRC_ADDR        (DMAC_WRAPPER_BASE_ADDRESS + 0xC64)
#define	REG_DMAC_WRAPPER_DMA1_CH3_FIFO0_PUSH_DEST_ADDR       (DMAC_WRAPPER_BASE_ADDRESS + 0xC68)
#define	REG_DMAC_WRAPPER_DMA1_CH3_FIFO0_PUSH_C_REG           (DMAC_WRAPPER_BASE_ADDRESS + 0xC6C)
#define	REG_DMAC_WRAPPER_DMA1_CH3_FIFO1_PUSH_SRC_ADDR        (DMAC_WRAPPER_BASE_ADDRESS + 0xC80)
#define	REG_DMAC_WRAPPER_DMA1_CH3_FIFO1_PUSH_DEST_ADDR       (DMAC_WRAPPER_BASE_ADDRESS + 0xC84)
#define	REG_DMAC_WRAPPER_DMA1_CH3_FIFO1_PUSH_C_REG           (DMAC_WRAPPER_BASE_ADDRESS + 0xC88)
#define	REG_DMAC_WRAPPER_DMA1_CH3_FIFO2_PUSH_SRC_ADDR        (DMAC_WRAPPER_BASE_ADDRESS + 0xC90)
#define	REG_DMAC_WRAPPER_DMA1_CH3_FIFO2_PUSH_DEST_ADDR       (DMAC_WRAPPER_BASE_ADDRESS + 0xC94)
#define	REG_DMAC_WRAPPER_DMA1_CH3_FIFO2_PUSH_C_REG           (DMAC_WRAPPER_BASE_ADDRESS + 0xC98)
#define	REG_DMAC_WRAPPER_DMA0_CH0_CONFIG_REG                 (DMAC_WRAPPER_BASE_ADDRESS + 0xD00)
#define	REG_DMAC_WRAPPER_DMA0_CH0_FIFO_PUSH_SRC_ADDR         (DMAC_WRAPPER_BASE_ADDRESS + 0xD04)
#define	REG_DMAC_WRAPPER_DMA0_CH0_FIFO_PUSH_DEST_ADDR        (DMAC_WRAPPER_BASE_ADDRESS + 0xD08)
#define	REG_DMAC_WRAPPER_DMA0_CH0_FIFO_PUSH_C_REG            (DMAC_WRAPPER_BASE_ADDRESS + 0xD0C)
#define	REG_DMAC_WRAPPER_DMA0_CH0_FIFO_PUSH_MPDUP_REG        (DMAC_WRAPPER_BASE_ADDRESS + 0xD10)
#define	REG_DMAC_WRAPPER_DMA0_CH1_CONFIG_REG                 (DMAC_WRAPPER_BASE_ADDRESS + 0xD20)
#define	REG_DMAC_WRAPPER_DMA0_CH1_FIFO_PUSH_SRC_ADDR         (DMAC_WRAPPER_BASE_ADDRESS + 0xD24)
#define	REG_DMAC_WRAPPER_DMA0_CH1_FIFO_PUSH_DEST_ADDR        (DMAC_WRAPPER_BASE_ADDRESS + 0xD28)
#define	REG_DMAC_WRAPPER_DMA0_CH1_FIFO_PUSH_C_REG            (DMAC_WRAPPER_BASE_ADDRESS + 0xD2C)
#define	REG_DMAC_WRAPPER_DMA0_CH1_FIFO_PUSH_MPDUP_REG        (DMAC_WRAPPER_BASE_ADDRESS + 0xD30)
#define	REG_DMAC_WRAPPER_DMA1_CH0_JOB_CNT_REG                (DMAC_WRAPPER_BASE_ADDRESS + 0xF00)
#define	REG_DMAC_WRAPPER_DMA1_CH0_BATCH_CNT_REG              (DMAC_WRAPPER_BASE_ADDRESS + 0xF04)
#define	REG_DMAC_WRAPPER_DMA1_CH0_STAT_IND_REG               (DMAC_WRAPPER_BASE_ADDRESS + 0xF08)
#define	REG_DMAC_WRAPPER_DMA1_CH1_JOB_CNT_REG                (DMAC_WRAPPER_BASE_ADDRESS + 0xF10)
#define	REG_DMAC_WRAPPER_DMA1_CH1_BATCH_CNT_REG              (DMAC_WRAPPER_BASE_ADDRESS + 0xF14)
#define	REG_DMAC_WRAPPER_DMA1_CH1_STAT_IND_REG               (DMAC_WRAPPER_BASE_ADDRESS + 0xF18)
#define	REG_DMAC_WRAPPER_DMA1_CH2_JOB_CNT_REG                (DMAC_WRAPPER_BASE_ADDRESS + 0xF20)
#define	REG_DMAC_WRAPPER_DMA1_CH2_BATCH_CNT_REG              (DMAC_WRAPPER_BASE_ADDRESS + 0xF24)
#define	REG_DMAC_WRAPPER_DMA1_CH2_STAT_IND_REG               (DMAC_WRAPPER_BASE_ADDRESS + 0xF28)
#define	REG_DMAC_WRAPPER_DMA1_CH3_JOB_CNT_REG                (DMAC_WRAPPER_BASE_ADDRESS + 0xF30)
#define	REG_DMAC_WRAPPER_DMA1_CH3_BATCH_CNT_REG              (DMAC_WRAPPER_BASE_ADDRESS + 0xF34)
#define	REG_DMAC_WRAPPER_DMA1_CH3_STAT_IND_REG               (DMAC_WRAPPER_BASE_ADDRESS + 0xF38)
#define	REG_DMAC_WRAPPER_DMA0_CH0_JOB_CNT_REG                (DMAC_WRAPPER_BASE_ADDRESS + 0xF40)
#define	REG_DMAC_WRAPPER_DMA0_CH0_BATCH_CNT_REG              (DMAC_WRAPPER_BASE_ADDRESS + 0xF44)
#define	REG_DMAC_WRAPPER_DMA0_CH0_STAT_IND_REG               (DMAC_WRAPPER_BASE_ADDRESS + 0xF48)
#define	REG_DMAC_WRAPPER_DMA0_CH1_JOB_CNT_REG                (DMAC_WRAPPER_BASE_ADDRESS + 0xF50)
#define	REG_DMAC_WRAPPER_DMA0_CH1_BATCH_CNT_REG              (DMAC_WRAPPER_BASE_ADDRESS + 0xF54)
#define	REG_DMAC_WRAPPER_DMA0_CH1_STAT_IND_REG               (DMAC_WRAPPER_BASE_ADDRESS + 0xF58)
#define	REG_DMAC_WRAPPER_FIFO0_STAT_IND_REG                  (DMAC_WRAPPER_BASE_ADDRESS + 0xF60)
#define	REG_DMAC_WRAPPER_FIFO1_STAT_IND_REG                  (DMAC_WRAPPER_BASE_ADDRESS + 0xF64)
#define	REG_DMAC_WRAPPER_FIFO2_STAT_IND_REG                  (DMAC_WRAPPER_BASE_ADDRESS + 0xF68)
#define	REG_DMAC_WRAPPER_FIFO3_STAT_IND_REG                  (DMAC_WRAPPER_BASE_ADDRESS + 0xF6C)
#define	REG_DMAC_WRAPPER_FIFO4_STAT_IND_REG                  (DMAC_WRAPPER_BASE_ADDRESS + 0xF70)
#define	REG_DMAC_WRAPPER_FIFO5_STAT_IND_REG                  (DMAC_WRAPPER_BASE_ADDRESS + 0xF74)
#define	REG_DMAC_WRAPPER_FIFO6_STAT_IND_REG                  (DMAC_WRAPPER_BASE_ADDRESS + 0xF78)
#define	REG_DMAC_WRAPPER_FIFO7_STAT_IND_REG                  (DMAC_WRAPPER_BASE_ADDRESS + 0xF7C)
#define	REG_DMAC_WRAPPER_SPARE_32_BIT_REG                    (DMAC_WRAPPER_BASE_ADDRESS + 0xFF0)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_DMAC_WRAPPER_DMA1_BA_REG 0x800 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 16;
		uint32 dma1MemBa : 16; //base address bits[31:16] of ram where dmac1 wrapper's data structure reside , reset value: 0x0, access type: RW
	} bitFields;
} RegDmacWrapperDma1BaReg_u;

/*REG_DMAC_WRAPPER_DMA1_CONFIG_REG 0x804 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma1ChManipBa : 16; //offset to base of manipulation lists [units: 64b], reset value: 0x0, access type: RW
		uint32 dma1ChCodeAlloc : 8; //dmac1 microcode block size per channel [units: 64b], reset value: 0x0, access type: RW
		uint32 dma1AddrTogConf : 2; //dmac1 axi address bit 32 toggle. '00' - fixed zero, '01' - fixed one, '11' - toggle, reset value: 0x0, access type: RW
		uint32 dma1AwaddrTogFwHostEn : 1; //enable write address toggling for FW and Host channel. '0' - disabled (default) '1' -enabled., reset value: 0x0, access type: RW
		uint32 dma1AraddrTogFwHostEn : 1; //enable read address toggling for FW and Host channel. '0' - disabled (default) '1' -enabled., reset value: 0x0, access type: RW
		uint32 reserved0 : 4;
	} bitFields;
} RegDmacWrapperDma1ConfigReg_u;

/*REG_DMAC_WRAPPER_DMA1_EXTERNAL_REQ_REG 0x808 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma1ExtStartReq : 1; //Request a DMAGO, reset value: 0x0, access type: RW
		uint32 dma1ExtStopReq : 1; //Request a DMAKILL, reset value: 0x0, access type: RW
		uint32 dma1ExtSigReq : 1; //Request a DMASEV, reset value: 0x0, access type: RW
		uint32 reserved0 : 1;
		uint32 dma1ExtReqArg : 28; //External request argument (channel# or signal#), reset value: 0x0, access type: RW
	} bitFields;
} RegDmacWrapperDma1ExternalReqReg_u;

/*REG_DMAC_WRAPPER_DMA1_ABORT_REG 0x80C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 8;
		uint32 dma1FifoClearReq : 6; //fifo clear, reset value: 0x0, access type: RW
		uint32 reserved1 : 18;
	} bitFields;
} RegDmacWrapperDma1AbortReg_u;

/*REG_DMAC_WRAPPER_DMA1_MISC_REG 0x810 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 12;
		uint32 dma1LastJobIdChSel : 3; //selects which channel is connected to the "last job id" interrupts, reset value: 0x0, access type: RW
		uint32 reserved1 : 11;
		uint32 dma1WordAlign : 1; //Word align mode: 32-bit aligned transfers would be done as 32-bit transfers (not bursts of 8-bit transfers), reset value: 0x1, access type: RW
		uint32 cdbMode : 1; //cdb mode - 0 - 3x1/4x0 (36 users to band0 no/one user for band1), 1 - 2x2 (18 users for band0 18 users band1), reset value: 0x1, access type: RW
		uint32 reserved2 : 4;
	} bitFields;
} RegDmacWrapperDma1MiscReg_u;

/*REG_DMAC_WRAPPER_DMA1_JOB_PUSH_WHILE_FULL_CLR_IND 0x814 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma1ClearPushWhileFullInd : 6; //no description, reset value: 0x0, access type: WO
		uint32 reserved0 : 26;
	} bitFields;
} RegDmacWrapperDma1JobPushWhileFullClrInd_u;

/*REG_DMAC_WRAPPER_DMA1_JUNK_BASE_ADDR_REG 0x818 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma1JunkBaseAddrConf : 32; //32 bit base address of junk yard. , NOTE: need to configure carefully to avoid memory rundown, reset value: 0x0, access type: RW
	} bitFields;
} RegDmacWrapperDma1JunkBaseAddrReg_u;

/*REG_DMAC_WRAPPER_DMA1_STAT0_IND_REG 0x81C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma1Status0Manipulation0Sm : 5; //status manipulation0 state machine, reset value: 0x0, access type: RO
		uint32 dma1Status0Manipulation1Sm : 5; //status manipulation1 state machine, reset value: 0x0, access type: RO
		uint32 dma1Status0Manipulation2Sm : 5; //status manipulation2 state machine, reset value: 0x0, access type: RO
		uint32 dma1Status0Manipulation3Sm : 5; //status manipulation3 state machine, reset value: 0x0, access type: RO
		uint32 reserved0 : 12;
	} bitFields;
} RegDmacWrapperDma1Stat0IndReg_u;

/*REG_DMAC_WRAPPER_DMA1_STAT1_IND_REG 0x820 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma1Status1DmacInterrupts : 8; //status dmac interrupts, reset value: 0x0, access type: RO
		uint32 dma1Status1WrapLastJobIndicationsTog : 8; //status wrapper last job indications (toggling), reset value: 0x0, access type: RO
		uint32 dma1Status1JobFifoFullIndications : 6; //status job fifo full indications, reset value: 0x0, access type: RO
		uint32 reserved0 : 9;
		uint32 dma1Status1DmacAbortInterrupt : 1; //status dmac abort interrupt, reset value: 0x0, access type: RO
	} bitFields;
} RegDmacWrapperDma1Stat1IndReg_u;

/*REG_DMAC_WRAPPER_TX1_SENDER_ALIGNER_EN 0x824 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx1SenderAlignerEn : 1; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegDmacWrapperTx1SenderAlignerEn_u;

/*REG_DMAC_WRAPPER_DMA0_BA_REG 0x900 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 16;
		uint32 dma0MemBa : 16; //base address bits[31:16] of ram where dmac0 wrapper's data structure reside , reset value: 0x0, access type: RW
	} bitFields;
} RegDmacWrapperDma0BaReg_u;

/*REG_DMAC_WRAPPER_DMA0_CONFIG_REG 0x904 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma0ChManipBa : 16; //offset to base of manipulation lists [units: 64b], reset value: 0x0, access type: RW
		uint32 dma0ChCodeAlloc : 8; //dmac1 microcode block size per channel [units: 64b], reset value: 0x0, access type: RW
		uint32 dma0AddrTogConf : 2; //dmac0 axi address bit 32 toggle. '00' - fixed zero, '01' - fixed one, '11' - toggle, reset value: 0x0, access type: RW
		uint32 reserved0 : 6;
	} bitFields;
} RegDmacWrapperDma0ConfigReg_u;

/*REG_DMAC_WRAPPER_DMA0_EXTERNAL_REQ_REG 0x908 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma0ExtStartReq : 1; //Request a DMAGO, reset value: 0x0, access type: RW
		uint32 dma0ExtStopReq : 1; //Request a DMAKILL, reset value: 0x0, access type: RW
		uint32 dma0ExtSigReq : 1; //Request a DMASEV, reset value: 0x0, access type: RW
		uint32 reserved0 : 1;
		uint32 dma0ExtReqArg : 28; //External request argument (channel# or signal#), reset value: 0x0, access type: RW
	} bitFields;
} RegDmacWrapperDma0ExternalReqReg_u;

/*REG_DMAC_WRAPPER_DMA0_ABORT_REG 0x90C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 8;
		uint32 dma0FifoClearReq : 2; //fifo clear, reset value: 0x0, access type: RW
		uint32 reserved1 : 22;
	} bitFields;
} RegDmacWrapperDma0AbortReg_u;

/*REG_DMAC_WRAPPER_DMA0_MISC0_REG 0x910 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma0MonHdrOffset0 : 6; //header monitor: offset to last header byte, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 dma0MonChSel0 : 3; //header monitor: selection of monitorred channel, reset value: 0x0, access type: RW
		uint32 dma0MonHdrEn0 : 1; //header monitor enable, reset value: 0x0, access type: RW
		uint32 dma0LastJobIdChSel : 3; //selects which channel is connected to the "last job id" interrupts, reset value: 0x0, access type: RW
		uint32 reserved1 : 1;
		uint32 dma0Tx0BufferFullMargin : 10; //Tx0 buffer full margin, reset value: 0x80, access type: RW
		uint32 dma0WordAlign : 1; //Word align mode: 32-bit aligned transfers would be done as 32-bit transfers (not bursts of 8-bit transfers), reset value: 0x1, access type: RW
		uint32 reserved2 : 5;
	} bitFields;
} RegDmacWrapperDma0Misc0Reg_u;

/*REG_DMAC_WRAPPER_DMA0_MISC1_REG 0x914 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma0MonHdrOffset1 : 6; //header monitor: offset to last header byte, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 dma0MonChSel1 : 3; //header monitor: selection of monitorred channel, reset value: 0x0, access type: RW
		uint32 dma0MonHdrEn1 : 1; //header monitor enable, reset value: 0x0, access type: RW
		uint32 reserved1 : 4;
		uint32 dma0Tx1BufferFullMargin : 10; //Tx1 buffer full margin, reset value: 0x80, access type: RW
		uint32 reserved2 : 6;
	} bitFields;
} RegDmacWrapperDma0Misc1Reg_u;

/*REG_DMAC_WRAPPER_DMA0_JOB_PUSH_WHILE_FULL_CLR_IND 0x918 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma0ClearPushWhileFullInd : 2; //no description, reset value: 0x0, access type: WO
		uint32 reserved0 : 30;
	} bitFields;
} RegDmacWrapperDma0JobPushWhileFullClrInd_u;

/*REG_DMAC_WRAPPER_DMA0_JUNK_BASE_ADDR_REG 0x920 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma0JunkBaseAddrConf : 32; //32 bit base address of junk yard. , NOTE: need to configure carefully to avoid memory rundown, reset value: 0x0, access type: RW
	} bitFields;
} RegDmacWrapperDma0JunkBaseAddrReg_u;

/*REG_DMAC_WRAPPER_DMA0_STAT0_IND_REG 0x924 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma0Status0Manipulation0Sm : 5; //status manipulation0 state machine, reset value: 0x0, access type: RO
		uint32 dma0Status0Manipulation1Sm : 5; //status manipulation1 state machine, reset value: 0x0, access type: RO
		uint32 reserved0 : 22;
	} bitFields;
} RegDmacWrapperDma0Stat0IndReg_u;

/*REG_DMAC_WRAPPER_DMA0_STAT1_IND_REG 0x928 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma0Status1DmacInterrupts : 8; //status dmac interrupts, reset value: 0x0, access type: RO
		uint32 dma0Status1WrapLastJobIndicationsTog : 8; //status wrapper last job indications (toggling), reset value: 0x0, access type: RO
		uint32 dma0Status1JobFifoFullIndications : 2; //status job fifo full indications, reset value: 0x0, access type: RO
		uint32 reserved0 : 13;
		uint32 dma0Status1DmacAbortInterrupt : 1; //status dmac abort interrupt, reset value: 0x0, access type: RO
	} bitFields;
} RegDmacWrapperDma0Stat1IndReg_u;

/*REG_DMAC_WRAPPER_TX0_SENDER_ALIGNER_EN 0x92C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx0SenderAlignerEn : 1; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegDmacWrapperTx0SenderAlignerEn_u;

/*REG_DMAC_WRAPPER_DMA0_CH0_RECOVERY_REG 0x930 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma0Ch0ForceTxSkip : 1; //In case TX0 ch recovery mode, set this bit to 1'b1 and activate tx skip force on. This sets all the jobs from activation to skip mode in which no DMA job is actualy done only fictive job promotion to completion. It is SW responsibility to clear this bit at the end the recovery process in order to get back to normal job processing. , reset value: 0x0, access type: RW
		uint32 dma0Ch0ForceFcByp : 1; //In case TX0 ch recovery mode, set this bit to 1'b1 and activate fc byp force on. This sets all the jobs from activation to Flow Control bypass mode. This means that FC is not checked before job selection.  It is SW responsibility to clear this bit at the end the recovery process in order to get back to normal job processing., reset value: 0x0, access type: RW
		uint32 reserved0 : 30;
	} bitFields;
} RegDmacWrapperDma0Ch0RecoveryReg_u;

/*REG_DMAC_WRAPPER_DMA0_CH1_RECOVERY_REG 0x934 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma0Ch1ForceTxSkip : 1; //In case TX1 ch recovery mode, set this bit to 1'b1 and activate tx skip force on. This sets all the jobs from activation to skip mode in which no DMA job is actualy done only fictive job promotion to completion.  It is SW responsibility to clear this bit at the end the recovery process in order to get back to normal job processing., reset value: 0x0, access type: RW
		uint32 dma0Ch1ForceFcByp : 1; //In case TX1 ch recovery mode, set this bit to 1'b1 and activate fc byp force on. This sets all the jobs from activation to Flow Control bypass mode. This means that FC is not checked before job selection.  It is SW responsibility to clear this bit at the end the recovery process in order to get back to normal job processing., reset value: 0x0, access type: RW
		uint32 reserved0 : 30;
	} bitFields;
} RegDmacWrapperDma0Ch1RecoveryReg_u;

/*REG_DMAC_WRAPPER_DMA0_STAT2_IND_REG 0x938 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txJobReadFsm : 3; //status tx job read fsm, reset value: 0x0, access type: RO
		uint32 rxJobReadFsm : 3; //status rx job read fsm, reset value: 0x0, access type: RO
		uint32 hostJobReadFsm : 3; //status host job read fsm, reset value: 0x0, access type: RO
		uint32 fwJobReadFsm : 3; //status fw job read fsm, reset value: 0x0, access type: RO
		uint32 tx0JobSelFsm : 2; //status tx0 job sel fsm, reset value: 0x0, access type: RO
		uint32 tx1JobSelFsm : 2; //status tx1 job sel fsm, reset value: 0x0, access type: RO
		uint32 rx0JobSelFsm : 2; //status rx0 job sel fsm, reset value: 0x0, access type: RO
		uint32 rx1JobSelFsm : 2; //status rx1 job sel fsm, reset value: 0x0, access type: RO
		uint32 hostJobSelFsm : 2; //status host job sel fsm, reset value: 0x0, access type: RO
		uint32 fwJobSelFsm : 2; //status fw job sel fsm, reset value: 0x0, access type: RO
		uint32 reserved0 : 8;
	} bitFields;
} RegDmacWrapperDma0Stat2IndReg_u;

/*REG_DMAC_WRAPPER_AGENT_0_INT_MASK_REG 0xA00 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 agent0Dmac0Bits4To7IntEn : 4; //agent 0 (GenriscTX0) enable interrupts DMAC0 bits[7:4], reset value: 0x0, access type: RW
		uint32 agent0Dmac1Bits4To7IntEn : 4; //agent 0 (GenriscTX0) enable interrupts DMAC1 bits[7:4], reset value: 0x0, access type: RW
		uint32 agent0Dmac0AbortIntEn : 1; //agent 0 (GenriscTX0) enable interrupts DMAC0 abort, reset value: 0x0, access type: RW
		uint32 agent0Dmac1AbortIntEn : 1; //agent 0 (GenriscTX0) enable interrupts DMAC1 abort, reset value: 0x0, access type: RW
		uint32 agent0Rsvd0IntEn : 2; //agent 0 (GenriscTX0) enable interrupts Reserved, reset value: 0x0, access type: RW
		uint32 agent0HostLastJobIdIntEn : 8; //agent 0 (GenriscTX0) enable interrupts "host last job id", reset value: 0x0, access type: RW
		uint32 agent0FwLastJobIdIntEn : 3; //agent 0 (GenriscTX0) enable interrupts "fw last job id", reset value: 0x0, access type: RW
		uint32 agent0Rsvd1IntEn : 1; //agent 0 (GenriscTX0) enable interrupts Reserved, reset value: 0x0, access type: RW
		uint32 agent0Tx0LastJobDoneIntEn : 1; //agent 0 (GenriscTX0) enable interrupts "tx0 last job is done", reset value: 0x0, access type: RW
		uint32 agent0Tx1LastJobDoneIntEn : 1; //agent 0 (GenriscTX0) enable interrupts "tx1 last job is done", reset value: 0x0, access type: RW
		uint32 agent0Rsvd2IntEn : 2; //agent 0 (GenriscTX0) enable interrupts Reserved, reset value: 0x0, access type: RW
		uint32 agent0Rx0LastJobDoneIntEn : 1; //agent 0 (GenriscTX0) enable interrupts "rx0 last job is done", reset value: 0x0, access type: RW
		uint32 agent0Rx1LastJobDoneIntEn : 1; //agent 0 (GenriscTX0) enable interrupts "rx1 last job is done", reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegDmacWrapperAgent0IntMaskReg_u;

/*REG_DMAC_WRAPPER_AGENT_0_PEND_INT_REG 0xA04 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 agent0Dmac0Bits4To7PendInt : 4; //agent 0 (GenriscTX0) pending interrupts DMAC0 bits[7:4], reset value: 0x0, access type: RO
		uint32 agent0Dmac1Bits4To7PendInt : 4; //agent 0 (GenriscTX0) pending interrupts DMAC1 bits[7:4], reset value: 0x0, access type: RO
		uint32 agent0Dmac0AbortPendInt : 1; //agent 0 (GenriscTX0) pending interrupts DMAC0 abort, reset value: 0x0, access type: RO
		uint32 agent0Dmac1AbortPendInt : 1; //agent 0 (GenriscTX0) pending interrupts DMAC1 abort, reset value: 0x0, access type: RO
		uint32 agent0Rsvd0PendInt : 2; //agent 0 (GenriscTX0) pending interrupts Reserved, reset value: 0x0, access type: RO
		uint32 agent0HostLastJobIdPendInt : 8; //agent 0 (GenriscTX0) pending interrupts "host last job id", reset value: 0x0, access type: RO
		uint32 agent0FwLastJobIdPendInt : 3; //agent 0 (GenriscTX0) pending interrupts "fw last job id", reset value: 0x0, access type: RO
		uint32 agent0Rsvd1PendInt : 1; //agent 0 (GenriscTX0) pending interrupts Reserved, reset value: 0x0, access type: RO
		uint32 agent0Tx0LastJobDonePendInt : 1; //agent 0 (GenriscTX0) pending interrupts "tx0 last job is done", reset value: 0x0, access type: RO
		uint32 agent0Tx1LastJobDonePendInt : 1; //agent 0 (GenriscTX0) pending interrupts "tx1 last job is done", reset value: 0x0, access type: RO
		uint32 agent0Rsvd2PendInt : 2; //agent 0 (GenriscTX0) pending interrupts Reserved, reset value: 0x0, access type: RO
		uint32 agent0Rx0LastJobDonePendInt : 1; //agent 0 (GenriscTX0) pending interrupts "rx0 last job is done", reset value: 0x0, access type: RO
		uint32 agent0Rx1LastJobDonePendInt : 1; //agent 0 (GenriscTX0) pending interrupts "rx1 last job is done", reset value: 0x0, access type: RO
		uint32 reserved0 : 2;
	} bitFields;
} RegDmacWrapperAgent0PendIntReg_u;

/*REG_DMAC_WRAPPER_AGENT_0_INT_CLEAR_REG 0xA08 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 agent0Dmac0Bits4To7IntClear : 4; //agent 0 (GenriscTX0) interrupt clear DMAC0 bits[7:4], reset value: 0x0, access type: WO
		uint32 agent0Dmac1Bits4To7IntClear : 4; //agent 0 (GenriscTX0) interrupt clear DMAC1 bits[7:4], reset value: 0x0, access type: WO
		uint32 agent0Dmac0AbortIntClear : 1; //agent 0 (GenriscTX0) interrupt clear DMAC0 abort, reset value: 0x0, access type: WO
		uint32 agent0Dmac1AbortIntClear : 1; //agent 0 (GenriscTX0) interrupt clear DMAC1 abort, reset value: 0x0, access type: WO
		uint32 agent0Rsvd0IntClear : 2; //agent 0 (GenriscTX0) interrupt clear Reserved, reset value: 0x0, access type: WO
		uint32 agent0HostLastJobIdIntClear : 8; //agent 0 (GenriscTX0) interrupt clear "host last job id", reset value: 0x0, access type: WO
		uint32 agent0FwLastJobIdIntClear : 3; //agent 0 (GenriscTX0) interrupt clear "fw last job id", reset value: 0x0, access type: WO
		uint32 agent0Rsvd1IntClear : 1; //agent 0 (GenriscTX0) interrupt clear Reserved, reset value: 0x0, access type: WO
		uint32 agent0Tx0LastJobDoneIntClear : 1; //agent 0 (GenriscTX0) interrupt clear "tx0 last job is done", reset value: 0x0, access type: WO
		uint32 agent0Tx1LastJobDoneIntClear : 1; //agent 0 (GenriscTX0) interrupt clear "tx1 last job is done", reset value: 0x0, access type: WO
		uint32 agent0Rsvd2IntClear : 2; //agent 0 (GenriscTX0) interrupt clear Reserved, reset value: 0x0, access type: WO
		uint32 agent0Rx0LastJobDoneIntClear : 1; //agent 0 (GenriscTX0) interrupt clear "rx0 last job is done", reset value: 0x0, access type: WO
		uint32 agent0Rx1LastJobDoneIntClear : 1; //agent 0 (GenriscTX0) interrupt clear "rx1 last job is done", reset value: 0x0, access type: WO
		uint32 reserved0 : 2;
	} bitFields;
} RegDmacWrapperAgent0IntClearReg_u;

/*REG_DMAC_WRAPPER_AGENT_1_INT_MASK_REG 0xA10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 agent1Dmac0Bits4To7IntEn : 4; //agent 1 (GenriscTX1) enable interrupts DMAC0 bits[7:4], reset value: 0x0, access type: RW
		uint32 agent1Dmac1Bits4To7IntEn : 4; //agent 1 (GenriscTX1) enable interrupts DMAC1 bits[7:4], reset value: 0x0, access type: RW
		uint32 agent1Dmac0AbortIntEn : 1; //agent 1 (GenriscTX1) enable interrupts DMAC0 abort, reset value: 0x0, access type: RW
		uint32 agent1Dmac1AbortIntEn : 1; //agent 1 (GenriscTX1) enable interrupts DMAC1 abort, reset value: 0x0, access type: RW
		uint32 agent1Rsvd0IntEn : 2; //agent 1 (GenriscTX1) enable interrupts Reserved, reset value: 0x0, access type: RW
		uint32 agent1HostLastJobIdIntEn : 8; //agent 1 (GenriscTX1) enable interrupts "host last job id", reset value: 0x0, access type: RW
		uint32 agent1FwLastJobIdIntEn : 3; //agent 1 (GenriscTX1) enable interrupts "fw last job id", reset value: 0x0, access type: RW
		uint32 agent1Rsvd1IntEn : 1; //agent 1 (GenriscTX1) enable interrupts Reserved, reset value: 0x0, access type: RW
		uint32 agent1Tx0LastJobDoneIntEn : 1; //agent 1 (GenriscTX1) enable interrupts "tx0 last job is done", reset value: 0x0, access type: RW
		uint32 agent1Tx1LastJobDoneIntEn : 1; //agent 1 (GenriscTX1) enable interrupts "tx1 last job is done", reset value: 0x0, access type: RW
		uint32 agent1Rsvd2IntEn : 2; //agent 1 (GenriscTX1) enable interrupts Reserved, reset value: 0x0, access type: RW
		uint32 agent1Rx0LastJobDoneIntEn : 1; //agent 1 (GenriscTX1) enable interrupts "rx0 last job is done", reset value: 0x0, access type: RW
		uint32 agent1Rx1LastJobDoneIntEn : 1; //agent 1 (GenriscTX1) enable interrupts "rx1 last job is done", reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegDmacWrapperAgent1IntMaskReg_u;

/*REG_DMAC_WRAPPER_AGENT_1_PEND_INT_REG 0xA14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 agent1Dmac0Bits4To7PendInt : 4; //agent 1 (GenriscTX1) pending interrupts DMAC0 bits[7:4], reset value: 0x0, access type: RO
		uint32 agent1Dmac1Bits4To7PendInt : 4; //agent 1 (GenriscTX1) pending interrupts DMAC1 bits[7:4], reset value: 0x0, access type: RO
		uint32 agent1Dmac0AbortPendInt : 1; //agent 1 (GenriscTX1) pending interrupts DMAC0 abort, reset value: 0x0, access type: RO
		uint32 agent1Dmac1AbortPendInt : 1; //agent 1 (GenriscTX1) pending interrupts DMAC1 abort, reset value: 0x0, access type: RO
		uint32 agent1Rsvd0PendInt : 2; //agent 1 (GenriscTX1) pending interrupts Reserved, reset value: 0x0, access type: RO
		uint32 agent1HostLastJobIdPendInt : 8; //agent 1 (GenriscTX1) pending interrupts "host last job id", reset value: 0x0, access type: RO
		uint32 agent1FwLastJobIdPendInt : 3; //agent 1 (GenriscTX1) pending interrupts "fw last job id", reset value: 0x0, access type: RO
		uint32 agent1Rsvd1PendInt : 1; //agent 1 (GenriscTX1) pending interrupts Reserved, reset value: 0x0, access type: RO
		uint32 agent1Tx0LastJobDonePendInt : 1; //agent 1 (GenriscTX1) pending interrupts "tx0 last job is done", reset value: 0x0, access type: RO
		uint32 agent1Tx1LastJobDonePendInt : 1; //agent 1 (GenriscTX1) pending interrupts "tx1 last job is done", reset value: 0x0, access type: RO
		uint32 agent1Rsvd2PendInt : 2; //agent 1 (GenriscTX1) pending interrupts Reserved, reset value: 0x0, access type: RO
		uint32 agent1Rx0LastJobDonePendInt : 1; //agent 1 (GenriscTX1) pending interrupts "rx0 last job is done", reset value: 0x0, access type: RO
		uint32 agent1Rx1LastJobDonePendInt : 1; //agent 1 (GenriscTX1) pending interrupts "rx1 last job is done", reset value: 0x0, access type: RO
		uint32 reserved0 : 2;
	} bitFields;
} RegDmacWrapperAgent1PendIntReg_u;

/*REG_DMAC_WRAPPER_AGENT_1_INT_CLEAR_REG 0xA18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 agent1Dmac0Bits4To7IntClear : 4; //agent 1 (GenriscTX1) interrupt clear DMAC0 bits[7:4], reset value: 0x0, access type: WO
		uint32 agent1Dmac1Bits4To7IntClear : 4; //agent 1 (GenriscTX1) interrupt clear DMAC1 bits[7:4], reset value: 0x0, access type: WO
		uint32 agent1Dmac0AbortIntClear : 1; //agent 1 (GenriscTX1) interrupt clear DMAC0 abort, reset value: 0x0, access type: WO
		uint32 agent1Dmac1AbortIntClear : 1; //agent 1 (GenriscTX1) interrupt clear DMAC1 abort, reset value: 0x0, access type: WO
		uint32 agent1Rsvd0IntClear : 2; //agent 1 (GenriscTX1) interrupt clear Reserved, reset value: 0x0, access type: WO
		uint32 agent1HostLastJobIdIntClear : 8; //agent 1 (GenriscTX1) interrupt clear "host last job id", reset value: 0x0, access type: WO
		uint32 agent1FwLastJobIdIntClear : 3; //agent 1 (GenriscTX1) interrupt clear "fw last job id", reset value: 0x0, access type: WO
		uint32 agent1Rsvd1IntClear : 1; //agent 1 (GenriscTX1) interrupt clear Reserved, reset value: 0x0, access type: WO
		uint32 agent1Tx0LastJobDoneIntClear : 1; //agent 1 (GenriscTX1) interrupt clear "tx0 last job is done", reset value: 0x0, access type: WO
		uint32 agent1Tx1LastJobDoneIntClear : 1; //agent 1 (GenriscTX1) interrupt clear "tx1 last job is done", reset value: 0x0, access type: WO
		uint32 agent1Rsvd2IntClear : 2; //agent 1 (GenriscTX1) interrupt clear Reserved, reset value: 0x0, access type: WO
		uint32 agent1Rx0LastJobDoneIntClear : 1; //agent 1 (GenriscTX1) interrupt clear "rx0 last job is done", reset value: 0x0, access type: WO
		uint32 agent1Rx1LastJobDoneIntClear : 1; //agent 1 (GenriscTX1) interrupt clear "rx1 last job is done", reset value: 0x0, access type: WO
		uint32 reserved0 : 2;
	} bitFields;
} RegDmacWrapperAgent1IntClearReg_u;

/*REG_DMAC_WRAPPER_AGENT_2_INT_MASK_REG 0xA20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 agent2Dmac0Bits4To7IntEn : 4; //agent 2 (GenriscRX0) enable interrupts DMAC0 bits[7:4], reset value: 0x0, access type: RW
		uint32 agent2Dmac1Bits4To7IntEn : 4; //agent 2 (GenriscRX0) enable interrupts DMAC1 bits[7:4], reset value: 0x0, access type: RW
		uint32 agent2Dmac0AbortIntEn : 1; //agent 2 (GenriscRX0) enable interrupts DMAC0 abort, reset value: 0x0, access type: RW
		uint32 agent2Dmac1AbortIntEn : 1; //agent 2 (GenriscRX0) enable interrupts DMAC1 abort, reset value: 0x0, access type: RW
		uint32 agent2Rsvd0IntEn : 2; //agent 2 (GenriscRX0) enable interrupts Reserved, reset value: 0x0, access type: RW
		uint32 agent2HostLastJobIdIntEn : 8; //agent 2 (GenriscRX0) enable interrupts "host last job id", reset value: 0x0, access type: RW
		uint32 agent2FwLastJobIdIntEn : 3; //agent 2 (GenriscRX0) enable interrupts "fw last job id", reset value: 0x0, access type: RW
		uint32 agent2Rsvd1IntEn : 1; //agent 2 (GenriscRX0) enable interrupts Reserved, reset value: 0x0, access type: RW
		uint32 agent2Tx0LastJobDoneIntEn : 1; //agent 2 (GenriscRX0) enable interrupts "tx0 last job is done", reset value: 0x0, access type: RW
		uint32 agent2Tx1LastJobDoneIntEn : 1; //agent 2 (GenriscRX0) enable interrupts "tx1 last job is done", reset value: 0x0, access type: RW
		uint32 agent2Rsvd2IntEn : 2; //agent 2 (GenriscRX0) enable interrupts Reserved, reset value: 0x0, access type: RW
		uint32 agent2Rx0LastJobDoneIntEn : 1; //agent 2 (GenriscRX0) enable interrupts "rx0 last job is done", reset value: 0x0, access type: RW
		uint32 agent2Rx1LastJobDoneIntEn : 1; //agent 2 (GenriscRX0) enable interrupts "rx1 last job is done", reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegDmacWrapperAgent2IntMaskReg_u;

/*REG_DMAC_WRAPPER_AGENT_2_PEND_INT_REG 0xA24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 agent2Dmac0Bits4To7PendInt : 4; //agent 2 (GenriscRX0) pending interrupts DMAC0 bits[7:4], reset value: 0x0, access type: RO
		uint32 agent2Dmac1Bits4To7PendInt : 4; //agent 2 (GenriscRX0) pending interrupts DMAC1 bits[7:4], reset value: 0x0, access type: RO
		uint32 agent2Dmac0AbortPendInt : 1; //agent 2 (GenriscRX0) pending interrupts DMAC0 abort, reset value: 0x0, access type: RO
		uint32 agent2Dmac1AbortPendInt : 1; //agent 2 (GenriscRX0) pending interrupts DMAC1 abort, reset value: 0x0, access type: RO
		uint32 agent2Rsvd0PendInt : 2; //agent 2 (GenriscRX0) pending interrupts Reserved, reset value: 0x0, access type: RO
		uint32 agent2HostLastJobIdPendInt : 8; //agent 2 (GenriscRX0) pending interrupts "host last job id", reset value: 0x0, access type: RO
		uint32 agent2FwLastJobIdPendInt : 3; //agent 2 (GenriscRX0) pending interrupts "fw last job id", reset value: 0x0, access type: RO
		uint32 agent2Rsvd1PendInt : 1; //agent 2 (GenriscRX0) pending interrupts Reserved, reset value: 0x0, access type: RO
		uint32 agent2Tx0LastJobDonePendInt : 1; //agent 2 (GenriscRX0) pending interrupts "tx0 last job is done", reset value: 0x0, access type: RO
		uint32 agent2Tx1LastJobDonePendInt : 1; //agent 2 (GenriscRX0) pending interrupts "tx1 last job is done", reset value: 0x0, access type: RO
		uint32 agent2Rsvd2PendInt : 2; //agent 2 (GenriscRX0) pending interrupts Reserved, reset value: 0x0, access type: RO
		uint32 agent2Rx0LastJobDonePendInt : 1; //agent 2 (GenriscRX0) pending interrupts "rx0 last job is done", reset value: 0x0, access type: RO
		uint32 agent2Rx1LastJobDonePendInt : 1; //agent 2 (GenriscRX0) pending interrupts "rx1 last job is done", reset value: 0x0, access type: RO
		uint32 reserved0 : 2;
	} bitFields;
} RegDmacWrapperAgent2PendIntReg_u;

/*REG_DMAC_WRAPPER_AGENT_2_INT_CLEAR_REG 0xA28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 agent2Dmac0Bits4To7IntClear : 4; //agent 2 (GenriscRX0) interrupt clear DMAC0 bits[7:4], reset value: 0x0, access type: WO
		uint32 agent2Dmac1Bits4To7IntClear : 4; //agent 2 (GenriscRX0) interrupt clear DMAC1 bits[7:4], reset value: 0x0, access type: WO
		uint32 agent2Dmac0AbortIntClear : 1; //agent 2 (GenriscRX0) interrupt clear DMAC0 abort, reset value: 0x0, access type: WO
		uint32 agent2Dmac1AbortIntClear : 1; //agent 2 (GenriscRX0) interrupt clear DMAC1 abort, reset value: 0x0, access type: WO
		uint32 agent2Rsvd0IntClear : 2; //agent 2 (GenriscRX0) interrupt clear Reserved, reset value: 0x0, access type: WO
		uint32 agent2HostLastJobIdIntClear : 8; //agent 2 (GenriscRX0) interrupt clear "host last job id", reset value: 0x0, access type: WO
		uint32 agent2FwLastJobIdIntClear : 3; //agent 2 (GenriscRX0) interrupt clear "fw last job id", reset value: 0x0, access type: WO
		uint32 agent2Rsvd1IntClear : 1; //agent 2 (GenriscRX0) interrupt clear Reserved, reset value: 0x0, access type: WO
		uint32 agent2Tx0LastJobDoneIntClear : 1; //agent 2 (GenriscRX0) interrupt clear "tx0 last job is done", reset value: 0x0, access type: WO
		uint32 agent2Tx1LastJobDoneIntClear : 1; //agent 2 (GenriscRX0) interrupt clear "tx1 last job is done", reset value: 0x0, access type: WO
		uint32 agent2Rsvd2IntClear : 2; //agent 2 (GenriscRX0) interrupt clear Reserved, reset value: 0x0, access type: WO
		uint32 agent2Rx0LastJobDoneIntClear : 1; //agent 2 (GenriscRX0) interrupt clear "rx0 last job is done", reset value: 0x0, access type: WO
		uint32 agent2Rx1LastJobDoneIntClear : 1; //agent 2 (GenriscRX0) interrupt clear "rx1 last job is done", reset value: 0x0, access type: WO
		uint32 reserved0 : 2;
	} bitFields;
} RegDmacWrapperAgent2IntClearReg_u;

/*REG_DMAC_WRAPPER_AGENT_3_INT_MASK_REG 0xA30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 agent3Dmac0Bits4To7IntEn : 4; //agent 3 (GenriscRX1) enable interrupts DMAC0 bits[7:4], reset value: 0x0, access type: RW
		uint32 agent3Dmac1Bits4To7IntEn : 4; //agent 3 (GenriscRX1) enable interrupts DMAC1 bits[7:4], reset value: 0x0, access type: RW
		uint32 agent3Dmac0AbortIntEn : 1; //agent 3 (GenriscRX1) enable interrupts DMAC0 abort, reset value: 0x0, access type: RW
		uint32 agent3Dmac1AbortIntEn : 1; //agent 3 (GenriscRX1) enable interrupts DMAC1 abort, reset value: 0x0, access type: RW
		uint32 agent3Rsvd0IntEn : 2; //agent 3 (GenriscRX1) enable interrupts Reserved, reset value: 0x0, access type: RW
		uint32 agent3HostLastJobIdIntEn : 8; //agent 3 (GenriscRX1) enable interrupts "host last job id", reset value: 0x0, access type: RW
		uint32 agent3FwLastJobIdIntEn : 3; //agent 3 (GenriscRX1) enable interrupts "fw last job id", reset value: 0x0, access type: RW
		uint32 agent3Rsvd1IntEn : 1; //agent 3 (GenriscRX1) enable interrupts Reserved, reset value: 0x0, access type: RW
		uint32 agent3Tx0LastJobDoneIntEn : 1; //agent 3 (GenriscRX1) enable interrupts "tx0 last job is done", reset value: 0x0, access type: RW
		uint32 agent3Tx1LastJobDoneIntEn : 1; //agent 3 (GenriscRX1) enable interrupts "tx1 last job is done", reset value: 0x0, access type: RW
		uint32 agent3Rsvd2IntEn : 2; //agent 3 (GenriscRX1) enable interrupts Reserved, reset value: 0x0, access type: RW
		uint32 agent3Rx0LastJobDoneIntEn : 1; //agent 3 (GenriscRX1) enable interrupts "rx0 last job is done", reset value: 0x0, access type: RW
		uint32 agent3Rx1LastJobDoneIntEn : 1; //agent 3 (GenriscRX1) enable interrupts "rx1 last job is done", reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegDmacWrapperAgent3IntMaskReg_u;

/*REG_DMAC_WRAPPER_AGENT_3_PEND_INT_REG 0xA34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 agent3Dmac0Bits4To7PendInt : 4; //agent 3 (GenriscRX1) pending interrupts DMAC0 bits[7:4], reset value: 0x0, access type: RO
		uint32 agent3Dmac1Bits4To7PendInt : 4; //agent 3 (GenriscRX1) pending interrupts DMAC1 bits[7:4], reset value: 0x0, access type: RO
		uint32 agent3Dmac0AbortPendInt : 1; //agent 3 (GenriscRX1) pending interrupts DMAC0 abort, reset value: 0x0, access type: RO
		uint32 agent3Dmac1AbortPendInt : 1; //agent 3 (GenriscRX1) pending interrupts DMAC1 abort, reset value: 0x0, access type: RO
		uint32 agent3Rsvd0PendInt : 2; //agent 3 (GenriscRX1) pending interrupts Reserved, reset value: 0x0, access type: RO
		uint32 agent3HostLastJobIdPendInt : 8; //agent 3 (GenriscRX1) pending interrupts "host last job id", reset value: 0x0, access type: RO
		uint32 agent3FwLastJobIdPendInt : 3; //agent 3 (GenriscRX1) pending interrupts "fw last job id", reset value: 0x0, access type: RO
		uint32 agent3Rsvd1PendInt : 1; //agent 3 (GenriscRX1) pending interrupts Reserved, reset value: 0x0, access type: RO
		uint32 agent3Tx0LastJobDonePendInt : 1; //agent 3 (GenriscRX1) pending interrupts "tx0 last job is done", reset value: 0x0, access type: RO
		uint32 agent3Tx1LastJobDonePendInt : 1; //agent 3 (GenriscRX1) pending interrupts "tx1 last job is done", reset value: 0x0, access type: RO
		uint32 agent3Rsvd2PendInt : 2; //agent 3 (GenriscRX1) pending interrupts Reserved, reset value: 0x0, access type: RO
		uint32 agent3Rx0LastJobDonePendInt : 1; //agent 3 (GenriscRX1) pending interrupts "rx0 last job is done", reset value: 0x0, access type: RO
		uint32 agent3Rx1LastJobDonePendInt : 1; //agent 3 (GenriscRX1) pending interrupts "rx1 last job is done", reset value: 0x0, access type: RO
		uint32 reserved0 : 2;
	} bitFields;
} RegDmacWrapperAgent3PendIntReg_u;

/*REG_DMAC_WRAPPER_AGENT_3_INT_CLEAR_REG 0xA38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 agent3Dmac0Bits4To7IntClear : 4; //agent 3 (GenriscRX1) interrupt clear DMAC0 bits[7:4], reset value: 0x0, access type: WO
		uint32 agent3Dmac1Bits4To7IntClear : 4; //agent 3 (GenriscRX1) interrupt clear DMAC1 bits[7:4], reset value: 0x0, access type: WO
		uint32 agent3Dmac0AbortIntClear : 1; //agent 3 (GenriscRX1) interrupt clear DMAC0 abort, reset value: 0x0, access type: WO
		uint32 agent3Dmac1AbortIntClear : 1; //agent 3 (GenriscRX1) interrupt clear DMAC1 abort, reset value: 0x0, access type: WO
		uint32 agent3Rsvd0IntClear : 2; //agent 3 (GenriscRX1) interrupt clear Reserved, reset value: 0x0, access type: WO
		uint32 agent3HostLastJobIdIntClear : 8; //agent 3 (GenriscRX1) interrupt clear "host last job id", reset value: 0x0, access type: WO
		uint32 agent3FwLastJobIdIntClear : 3; //agent 3 (GenriscRX1) interrupt clear "fw last job id", reset value: 0x0, access type: WO
		uint32 agent3Rsvd1IntClear : 1; //agent 3 (GenriscRX1) interrupt clear Reserved, reset value: 0x0, access type: WO
		uint32 agent3Tx0LastJobDoneIntClear : 1; //agent 3 (GenriscRX1) interrupt clear "tx0 last job is done", reset value: 0x0, access type: WO
		uint32 agent3Tx1LastJobDoneIntClear : 1; //agent 3 (GenriscRX1) interrupt clear "tx1 last job is done", reset value: 0x0, access type: WO
		uint32 agent3Rsvd2IntClear : 2; //agent 3 (GenriscRX1) interrupt clear Reserved, reset value: 0x0, access type: WO
		uint32 agent3Rx0LastJobDoneIntClear : 1; //agent 3 (GenriscRX1) interrupt clear "rx0 last job is done", reset value: 0x0, access type: WO
		uint32 agent3Rx1LastJobDoneIntClear : 1; //agent 3 (GenriscRX1) interrupt clear "rx1 last job is done", reset value: 0x0, access type: WO
		uint32 reserved0 : 2;
	} bitFields;
} RegDmacWrapperAgent3IntClearReg_u;

/*REG_DMAC_WRAPPER_AGENT_4_INT_MASK_REG 0xA40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 agent4Dmac0Bits4To7IntEn : 4; //agent 4 (GenriscHOST) enable interrupts DMAC0 bits[7:4], reset value: 0x0, access type: RW
		uint32 agent4Dmac1Bits4To7IntEn : 4; //agent 4 (GenriscHOST) enable interrupts DMAC1 bits[7:4], reset value: 0x0, access type: RW
		uint32 agent4Dmac0AbortIntEn : 1; //agent 4 (GenriscHOST) enable interrupts DMAC0 abort, reset value: 0x0, access type: RW
		uint32 agent4Dmac1AbortIntEn : 1; //agent 4 (GenriscHOST) enable interrupts DMAC1 abort, reset value: 0x0, access type: RW
		uint32 agent4Rsvd0IntEn : 2; //agent 4 (GenriscHOST) enable interrupts Reserved, reset value: 0x0, access type: RW
		uint32 agent4HostLastJobIdIntEn : 8; //agent 4 (GenriscHOST) enable interrupts "host last job id", reset value: 0x0, access type: RW
		uint32 agent4FwLastJobIdIntEn : 3; //agent 4 (GenriscHOST) enable interrupts "fw last job id", reset value: 0x0, access type: RW
		uint32 agent4Rsvd1IntEn : 1; //agent 4 (GenriscHOST) enable interrupts Reserved, reset value: 0x0, access type: RW
		uint32 agent4Tx0LastJobDoneIntEn : 1; //agent 4 (GenriscHOST) enable interrupts "tx0 last job is done", reset value: 0x0, access type: RW
		uint32 agent4Tx1LastJobDoneIntEn : 1; //agent 4 (GenriscHOST) enable interrupts "tx1 last job is done", reset value: 0x0, access type: RW
		uint32 agent4Rsvd2IntEn : 2; //agent 4 (GenriscHOST) enable interrupts Reserved, reset value: 0x0, access type: RW
		uint32 agent4Rx0LastJobDoneIntEn : 1; //agent 4 (GenriscHOST) enable interrupts "rx0 last job is done", reset value: 0x0, access type: RW
		uint32 agent4Rx1LastJobDoneIntEn : 1; //agent 4 (GenriscHOST) enable interrupts "rx1 last job is done", reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegDmacWrapperAgent4IntMaskReg_u;

/*REG_DMAC_WRAPPER_AGENT_4_PEND_INT_REG 0xA44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 agent4Dmac0Bits4To7PendInt : 4; //agent 4 (GenriscHOST) pending interrupts DMAC0 bits[7:4], reset value: 0x0, access type: RO
		uint32 agent4Dmac1Bits4To7PendInt : 4; //agent 4 (GenriscHOST) pending interrupts DMAC1 bits[7:4], reset value: 0x0, access type: RO
		uint32 agent4Dmac0AbortPendInt : 1; //agent 4 (GenriscHOST) pending interrupts DMAC0 abort, reset value: 0x0, access type: RO
		uint32 agent4Dmac1AbortPendInt : 1; //agent 4 (GenriscHOST) pending interrupts DMAC1 abort, reset value: 0x0, access type: RO
		uint32 agent4Rsvd0PendInt : 2; //agent 4 (GenriscHOST) pending interrupts Reserved, reset value: 0x0, access type: RO
		uint32 agent4HostLastJobIdPendInt : 8; //agent 4 (GenriscHOST) pending interrupts "host last job id", reset value: 0x0, access type: RO
		uint32 agent4FwLastJobIdPendInt : 3; //agent 4 (GenriscHOST) pending interrupts "fw last job id", reset value: 0x0, access type: RO
		uint32 agent4Rsvd1PendInt : 1; //agent 4 (GenriscHOST) pending interrupts Reserved, reset value: 0x0, access type: RO
		uint32 agent4Tx0LastJobDonePendInt : 1; //agent 4 (GenriscHOST) pending interrupts "tx0 last job is done", reset value: 0x0, access type: RO
		uint32 agent4Tx1LastJobDonePendInt : 1; //agent 4 (GenriscHOST) pending interrupts "tx1 last job is done", reset value: 0x0, access type: RO
		uint32 agent4Rsvd2PendInt : 2; //agent 4 (GenriscHOST) pending interrupts Reserved, reset value: 0x0, access type: RO
		uint32 agent4Rx0LastJobDonePendInt : 1; //agent 4 (GenriscHOST) pending interrupts "rx0 last job is done", reset value: 0x0, access type: RO
		uint32 agent4Rx1LastJobDonePendInt : 1; //agent 4 (GenriscHOST) pending interrupts "rx1 last job is done", reset value: 0x0, access type: RO
		uint32 reserved0 : 2;
	} bitFields;
} RegDmacWrapperAgent4PendIntReg_u;

/*REG_DMAC_WRAPPER_AGENT_4_INT_CLEAR_REG 0xA48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 agent4Dmac0Bits4To7IntClear : 4; //agent 4 (GenriscHOST) interrupt clear DMAC0 bits[7:4], reset value: 0x0, access type: WO
		uint32 agent4Dmac1Bits4To7IntClear : 4; //agent 4 (GenriscHOST) interrupt clear DMAC1 bits[7:4], reset value: 0x0, access type: WO
		uint32 agent4Dmac0AbortIntClear : 1; //agent 4 (GenriscHOST) interrupt clear DMAC0 abort, reset value: 0x0, access type: WO
		uint32 agent4Dmac1AbortIntClear : 1; //agent 4 (GenriscHOST) interrupt clear DMAC1 abort, reset value: 0x0, access type: WO
		uint32 agent4Rsvd0IntClear : 2; //agent 4 (GenriscHOST) interrupt clear Reserved, reset value: 0x0, access type: WO
		uint32 agent4HostLastJobIdIntClear : 8; //agent 4 (GenriscHOST) interrupt clear "host last job id", reset value: 0x0, access type: WO
		uint32 agent4FwLastJobIdIntClear : 3; //agent 4 (GenriscHOST) interrupt clear "fw last job id", reset value: 0x0, access type: WO
		uint32 agent4Rsvd1IntClear : 1; //agent 4 (GenriscHOST) interrupt clear Reserved, reset value: 0x0, access type: WO
		uint32 agent4Tx0LastJobDoneIntClear : 1; //agent 4 (GenriscHOST) interrupt clear "tx0 last job is done", reset value: 0x0, access type: WO
		uint32 agent4Tx1LastJobDoneIntClear : 1; //agent 4 (GenriscHOST) interrupt clear "tx1 last job is done", reset value: 0x0, access type: WO
		uint32 agent4Rsvd2IntClear : 2; //agent 4 (GenriscHOST) interrupt clear Reserved, reset value: 0x0, access type: WO
		uint32 agent4Rx0LastJobDoneIntClear : 1; //agent 4 (GenriscHOST) interrupt clear "rx0 last job is done", reset value: 0x0, access type: WO
		uint32 agent4Rx1LastJobDoneIntClear : 1; //agent 4 (GenriscHOST) interrupt clear "rx1 last job is done", reset value: 0x0, access type: WO
		uint32 reserved0 : 2;
	} bitFields;
} RegDmacWrapperAgent4IntClearReg_u;

/*REG_DMAC_WRAPPER_AGENT_5_INT_MASK_REG 0xA50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 agent5Dmac0Bits4To7IntEn : 4; //agent 5 (FW0) enable interrupts DMAC0 bits[7:4], reset value: 0x0, access type: RW
		uint32 agent5Dmac1Bits4To7IntEn : 4; //agent 5 (FW0) enable interrupts DMAC1 bits[7:4], reset value: 0x0, access type: RW
		uint32 agent5Dmac0AbortIntEn : 1; //agent 5 (FW0) enable interrupts DMAC0 abort, reset value: 0x0, access type: RW
		uint32 agent5Dmac1AbortIntEn : 1; //agent 5 (FW0) enable interrupts DMAC1 abort, reset value: 0x0, access type: RW
		uint32 agent5Rsvd0IntEn : 2; //agent 5 (FW0) enable interrupts Reserved, reset value: 0x0, access type: RW
		uint32 agent5HostLastJobIdIntEn : 8; //agent 5 (FW0) enable interrupts "host last job id", reset value: 0x0, access type: RW
		uint32 agent5FwLastJobIdIntEn : 3; //agent 5 (FW0) enable interrupts "fw last job id", reset value: 0x0, access type: RW
		uint32 agent5Rsvd1IntEn : 1; //agent 5 (FW0) enable interrupts Reserved, reset value: 0x0, access type: RW
		uint32 agent5Tx0LastJobDoneIntEn : 1; //agent 5 (FW0) enable interrupts "tx0 last job is done", reset value: 0x0, access type: RW
		uint32 agent5Tx1LastJobDoneIntEn : 1; //agent 5 (FW0) enable interrupts "tx1 last job is done", reset value: 0x0, access type: RW
		uint32 agent5Rsvd2IntEn : 2; //agent 5 (FW0) enable interrupts Reserved, reset value: 0x0, access type: RW
		uint32 agent5Rx0LastJobDoneIntEn : 1; //agent 5 (FW0) enable interrupts "rx0 last job is done", reset value: 0x0, access type: RW
		uint32 agent5Rx1LastJobDoneIntEn : 1; //agent 5 (FW0) enable interrupts "rx1 last job is done", reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegDmacWrapperAgent5IntMaskReg_u;

/*REG_DMAC_WRAPPER_AGENT_5_PEND_INT_REG 0xA54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 agent5Dmac0Bits4To7PendInt : 4; //agent 5 (FW0) pending interrupts DMAC0 bits[7:4], reset value: 0x0, access type: RO
		uint32 agent5Dmac1Bits4To7PendInt : 4; //agent 5 (FW0) pending interrupts DMAC1 bits[7:4], reset value: 0x0, access type: RO
		uint32 agent5Dmac0AbortPendInt : 1; //agent 5 (FW0) pending interrupts DMAC0 abort, reset value: 0x0, access type: RO
		uint32 agent5Dmac1AbortPendInt : 1; //agent 5 (FW0) pending interrupts DMAC1 abort, reset value: 0x0, access type: RO
		uint32 agent5Rsvd0PendInt : 2; //agent 5 (FW0) pending interrupts Reserved, reset value: 0x0, access type: RO
		uint32 agent5HostLastJobIdPendInt : 8; //agent 5 (FW0) pending interrupts "host last job id", reset value: 0x0, access type: RO
		uint32 agent5FwLastJobIdPendInt : 3; //agent 5 (FW0) pending interrupts "fw last job id", reset value: 0x0, access type: RO
		uint32 agent5Rsvd1PendInt : 1; //agent 5 (FW0) pending interrupts Reserved, reset value: 0x0, access type: RO
		uint32 agent5Tx0LastJobDonePendInt : 1; //agent 5 (FW0) pending interrupts "tx0 last job is done", reset value: 0x0, access type: RO
		uint32 agent5Tx1LastJobDonePendInt : 1; //agent 5 (FW0) pending interrupts "tx1 last job is done", reset value: 0x0, access type: RO
		uint32 agent5Rsvd2PendInt : 2; //agent 5 (FW0) pending interrupts Reserved, reset value: 0x0, access type: RO
		uint32 agent5Rx0LastJobDonePendInt : 1; //agent 5 (FW0) pending interrupts "rx0 last job is done", reset value: 0x0, access type: RO
		uint32 agent5Rx1LastJobDonePendInt : 1; //agent 5 (FW0) pending interrupts "rx1 last job is done", reset value: 0x0, access type: RO
		uint32 reserved0 : 2;
	} bitFields;
} RegDmacWrapperAgent5PendIntReg_u;

/*REG_DMAC_WRAPPER_AGENT_5_INT_CLEAR_REG 0xA58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 agent5Dmac0Bits4To7IntClear : 4; //agent 5 (FW0) interrupt clear DMAC0 bits[7:4], reset value: 0x0, access type: WO
		uint32 agent5Dmac1Bits4To7IntClear : 4; //agent 5 (FW0) interrupt clear DMAC1 bits[7:4], reset value: 0x0, access type: WO
		uint32 agent5Dmac0AbortIntClear : 1; //agent 5 (FW0) interrupt clear DMAC0 abort, reset value: 0x0, access type: WO
		uint32 agent5Dmac1AbortIntClear : 1; //agent 5 (FW0) interrupt clear DMAC1 abort, reset value: 0x0, access type: WO
		uint32 agent5Rsvd0IntClear : 2; //agent 5 (FW0) interrupt clear Reserved, reset value: 0x0, access type: WO
		uint32 agent5HostLastJobIdIntClear : 8; //agent 5 (FW0) interrupt clear "host last job id", reset value: 0x0, access type: WO
		uint32 agent5FwLastJobIdIntClear : 3; //agent 5 (FW0) interrupt clear "fw last job id", reset value: 0x0, access type: WO
		uint32 agent5Rsvd1IntClear : 1; //agent 5 (FW0) interrupt clear Reserved, reset value: 0x0, access type: WO
		uint32 agent5Tx0LastJobDoneIntClear : 1; //agent 5 (FW0) interrupt clear "tx0 last job is done", reset value: 0x0, access type: WO
		uint32 agent5Tx1LastJobDoneIntClear : 1; //agent 5 (FW0) interrupt clear "tx1 last job is done", reset value: 0x0, access type: WO
		uint32 agent5Rsvd2IntClear : 2; //agent 5 (FW0) interrupt clear Reserved, reset value: 0x0, access type: WO
		uint32 agent5Rx0LastJobDoneIntClear : 1; //agent 5 (FW0) interrupt clear "rx0 last job is done", reset value: 0x0, access type: WO
		uint32 agent5Rx1LastJobDoneIntClear : 1; //agent 5 (FW0) interrupt clear "rx1 last job is done", reset value: 0x0, access type: WO
		uint32 reserved0 : 2;
	} bitFields;
} RegDmacWrapperAgent5IntClearReg_u;

/*REG_DMAC_WRAPPER_AGENT_6_INT_MASK_REG 0xA60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 agent6Dmac0Bits4To7IntEn : 4; //agent 6 (FW1) enable interrupts DMAC0 bits[7:4], reset value: 0x0, access type: RW
		uint32 agent6Dmac1Bits4To7IntEn : 4; //agent 6 (FW1) enable interrupts DMAC1 bits[7:4], reset value: 0x0, access type: RW
		uint32 agent6Dmac0AbortIntEn : 1; //agent 6 (FW1) enable interrupts DMAC0 abort, reset value: 0x0, access type: RW
		uint32 agent6Dmac1AbortIntEn : 1; //agent 6 (FW1) enable interrupts DMAC1 abort, reset value: 0x0, access type: RW
		uint32 agent6Rsvd0IntEn : 2; //agent 6 (FW1) enable interrupts Reserved, reset value: 0x0, access type: RW
		uint32 agent6HostLastJobIdIntEn : 8; //agent 6 (FW1) enable interrupts "host last job id", reset value: 0x0, access type: RW
		uint32 agent6FwLastJobIdIntEn : 3; //agent 6 (FW1) enable interrupts "fw last job id", reset value: 0x0, access type: RW
		uint32 agent6Rsvd1IntEn : 1; //agent 6 (FW1) enable interrupts Reserved, reset value: 0x0, access type: RW
		uint32 agent6Tx0LastJobDoneIntEn : 1; //agent 6 (FW1) enable interrupts "tx0 last job is done", reset value: 0x0, access type: RW
		uint32 agent6Tx1LastJobDoneIntEn : 1; //agent 6 (FW1) enable interrupts "tx1 last job is done", reset value: 0x0, access type: RW
		uint32 agent6Rsvd2IntEn : 2; //agent 6 (FW1) enable interrupts Reserved, reset value: 0x0, access type: RW
		uint32 agent6Rx0LastJobDoneIntEn : 1; //agent 6 (FW1) enable interrupts "rx0 last job is done", reset value: 0x0, access type: RW
		uint32 agent6Rx1LastJobDoneIntEn : 1; //agent 6 (FW1) enable interrupts "rx1 last job is done", reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegDmacWrapperAgent6IntMaskReg_u;

/*REG_DMAC_WRAPPER_AGENT_6_PEND_INT_REG 0xA64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 agent6Dmac0Bits4To7PendInt : 4; //agent 6 (FW1) pending interrupts DMAC0 bits[7:4], reset value: 0x0, access type: RO
		uint32 agent6Dmac1Bits4To7PendInt : 4; //agent 6 (FW1) pending interrupts DMAC1 bits[7:4], reset value: 0x0, access type: RO
		uint32 agent6Dmac0AbortPendInt : 1; //agent 6 (FW1) pending interrupts DMAC0 abort, reset value: 0x0, access type: RO
		uint32 agent6Dmac1AbortPendInt : 1; //agent 6 (FW1) pending interrupts DMAC1 abort, reset value: 0x0, access type: RO
		uint32 agent6Rsvd0PendInt : 2; //agent 6 (FW1) pending interrupts Reserved, reset value: 0x0, access type: RO
		uint32 agent6HostLastJobIdPendInt : 8; //agent 6 (FW1) pending interrupts "host last job id", reset value: 0x0, access type: RO
		uint32 agent6FwLastJobIdPendInt : 3; //agent 6 (FW1) pending interrupts "fw last job id", reset value: 0x0, access type: RO
		uint32 agent6Rsvd1PendInt : 1; //agent 6 (FW1) pending interrupts Reserved, reset value: 0x0, access type: RO
		uint32 agent6Tx0LastJobDonePendInt : 1; //agent 6 (FW1) pending interrupts "tx0 last job is done", reset value: 0x0, access type: RO
		uint32 agent6Tx1LastJobDonePendInt : 1; //agent 6 (FW1) pending interrupts "tx1 last job is done", reset value: 0x0, access type: RO
		uint32 agent6Rsvd2PendInt : 2; //agent 6 (FW1) pending interrupts Reserved, reset value: 0x0, access type: RO
		uint32 agent6Rx0LastJobDonePendInt : 1; //agent 6 (FW1) pending interrupts "rx0 last job is done", reset value: 0x0, access type: RO
		uint32 agent6Rx1LastJobDonePendInt : 1; //agent 6 (FW1) pending interrupts "rx1 last job is done", reset value: 0x0, access type: RO
		uint32 reserved0 : 2;
	} bitFields;
} RegDmacWrapperAgent6PendIntReg_u;

/*REG_DMAC_WRAPPER_AGENT_6_INT_CLEAR_REG 0xA68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 agent6Dmac0Bits4To7IntClear : 4; //agent 6 (FW1) interrupt clear DMAC0 bits[7:4], reset value: 0x0, access type: WO
		uint32 agent6Dmac1Bits4To7IntClear : 4; //agent 6 (FW1) interrupt clear DMAC1 bits[7:4], reset value: 0x0, access type: WO
		uint32 agent6Dmac0AbortIntClear : 1; //agent 6 (FW1) interrupt clear DMAC0 abort, reset value: 0x0, access type: WO
		uint32 agent6Dmac1AbortIntClear : 1; //agent 6 (FW1) interrupt clear DMAC1 abort, reset value: 0x0, access type: WO
		uint32 agent6Rsvd0IntClear : 2; //agent 6 (FW1) interrupt clear Reserved, reset value: 0x0, access type: WO
		uint32 agent6HostLastJobIdIntClear : 8; //agent 6 (FW1) interrupt clear "host last job id", reset value: 0x0, access type: WO
		uint32 agent6FwLastJobIdIntClear : 3; //agent 6 (FW1) interrupt clear "fw last job id", reset value: 0x0, access type: WO
		uint32 agent6Rsvd1IntClear : 1; //agent 6 (FW1) interrupt clear Reserved, reset value: 0x0, access type: WO
		uint32 agent6Tx0LastJobDoneIntClear : 1; //agent 6 (FW1) interrupt clear "tx0 last job is done", reset value: 0x0, access type: WO
		uint32 agent6Tx1LastJobDoneIntClear : 1; //agent 6 (FW1) interrupt clear "tx1 last job is done", reset value: 0x0, access type: WO
		uint32 agent6Rsvd2IntClear : 2; //agent 6 (FW1) interrupt clear Reserved, reset value: 0x0, access type: WO
		uint32 agent6Rx0LastJobDoneIntClear : 1; //agent 6 (FW1) interrupt clear "rx0 last job is done", reset value: 0x0, access type: WO
		uint32 agent6Rx1LastJobDoneIntClear : 1; //agent 6 (FW1) interrupt clear "rx1 last job is done", reset value: 0x0, access type: WO
		uint32 reserved0 : 2;
	} bitFields;
} RegDmacWrapperAgent6IntClearReg_u;

/*REG_DMAC_WRAPPER_AGENT_7_INT_MASK_REG 0xA70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 agent7Dmac0Bits4To7IntEn : 4; //agent 7 (FW2) enable interrupts DMAC0 bits[7:4], reset value: 0x0, access type: RW
		uint32 agent7Dmac1Bits4To7IntEn : 4; //agent 7 (FW2) enable interrupts DMAC1 bits[7:4], reset value: 0x0, access type: RW
		uint32 agent7Dmac0AbortIntEn : 1; //agent 7 (FW2) enable interrupts DMAC0 abort, reset value: 0x0, access type: RW
		uint32 agent7Dmac1AbortIntEn : 1; //agent 7 (FW2) enable interrupts DMAC1 abort, reset value: 0x0, access type: RW
		uint32 agent7Rsvd0IntEn : 2; //agent 7 (FW2) enable interrupts Reserved, reset value: 0x0, access type: RW
		uint32 agent7HostLastJobIdIntEn : 8; //agent 7 (FW2) enable interrupts "host last job id", reset value: 0x0, access type: RW
		uint32 agent7FwLastJobIdIntEn : 3; //agent 7 (FW2) enable interrupts "fw last job id", reset value: 0x0, access type: RW
		uint32 agent7Rsvd1IntEn : 1; //agent 7 (FW2) enable interrupts Reserved, reset value: 0x0, access type: RW
		uint32 agent7Tx0LastJobDoneIntEn : 1; //agent 7 (FW2) enable interrupts "tx0 last job is done", reset value: 0x0, access type: RW
		uint32 agent7Tx1LastJobDoneIntEn : 1; //agent 7 (FW2) enable interrupts "tx1 last job is done", reset value: 0x0, access type: RW
		uint32 agent7Rsvd2IntEn : 2; //agent 7 (FW2) enable interrupts Reserved, reset value: 0x0, access type: RW
		uint32 agent7Rx0LastJobDoneIntEn : 1; //agent 7 (FW2) enable interrupts "rx0 last job is done", reset value: 0x0, access type: RW
		uint32 agent7Rx1LastJobDoneIntEn : 1; //agent 7 (FW2) enable interrupts "rx1 last job is done", reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegDmacWrapperAgent7IntMaskReg_u;

/*REG_DMAC_WRAPPER_AGENT_7_PEND_INT_REG 0xA74 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 agent7Dmac0Bits4To7PendInt : 4; //agent 7 (FW2) pending interrupts DMAC0 bits[7:4], reset value: 0x0, access type: RO
		uint32 agent7Dmac1Bits4To7PendInt : 4; //agent 7 (FW2) pending interrupts DMAC1 bits[7:4], reset value: 0x0, access type: RO
		uint32 agent7Dmac0AbortPendInt : 1; //agent 7 (FW2) pending interrupts DMAC0 abort, reset value: 0x0, access type: RO
		uint32 agent7Dmac1AbortPendInt : 1; //agent 7 (FW2) pending interrupts DMAC1 abort, reset value: 0x0, access type: RO
		uint32 agent7Rsvd0PendInt : 2; //agent 7 (FW2) pending interrupts Reserved, reset value: 0x0, access type: RO
		uint32 agent7HostLastJobIdPendInt : 8; //agent 7 (FW2) pending interrupts "host last job id", reset value: 0x0, access type: RO
		uint32 agent7FwLastJobIdPendInt : 3; //agent 7 (FW2) pending interrupts "fw last job id", reset value: 0x0, access type: RO
		uint32 agent7Rsvd1PendInt : 1; //agent 7 (FW2) pending interrupts Reserved, reset value: 0x0, access type: RO
		uint32 agent7Tx0LastJobDonePendInt : 1; //agent 7 (FW2) pending interrupts "tx0 last job is done", reset value: 0x0, access type: RO
		uint32 agent7Tx1LastJobDonePendInt : 1; //agent 7 (FW2) pending interrupts "tx1 last job is done", reset value: 0x0, access type: RO
		uint32 agent7Rsvd2PendInt : 2; //agent 7 (FW2) pending interrupts Reserved, reset value: 0x0, access type: RO
		uint32 agent7Rx0LastJobDonePendInt : 1; //agent 7 (FW2) pending interrupts "rx0 last job is done", reset value: 0x0, access type: RO
		uint32 agent7Rx1LastJobDonePendInt : 1; //agent 7 (FW2) pending interrupts "rx1 last job is done", reset value: 0x0, access type: RO
		uint32 reserved0 : 2;
	} bitFields;
} RegDmacWrapperAgent7PendIntReg_u;

/*REG_DMAC_WRAPPER_AGENT_7_INT_CLEAR_REG 0xA78 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 agent7Dmac0Bits4To7IntClear : 4; //agent 7 (FW2) interrupt clear DMAC0 bits[7:4], reset value: 0x0, access type: WO
		uint32 agent7Dmac1Bits4To7IntClear : 4; //agent 7 (FW2) interrupt clear DMAC1 bits[7:4], reset value: 0x0, access type: WO
		uint32 agent7Dmac0AbortIntClear : 1; //agent 7 (FW2) interrupt clear DMAC0 abort, reset value: 0x0, access type: WO
		uint32 agent7Dmac1AbortIntClear : 1; //agent 7 (FW2) interrupt clear DMAC1 abort, reset value: 0x0, access type: WO
		uint32 agent7Rsvd0IntClear : 2; //agent 7 (FW2) interrupt clear Reserved, reset value: 0x0, access type: WO
		uint32 agent7HostLastJobIdIntClear : 8; //agent 7 (FW2) interrupt clear "host last job id", reset value: 0x0, access type: WO
		uint32 agent7FwLastJobIdIntClear : 3; //agent 7 (FW2) interrupt clear "fw last job id", reset value: 0x0, access type: WO
		uint32 agent7Rsvd1IntClear : 1; //agent 7 (FW2) interrupt clear Reserved, reset value: 0x0, access type: WO
		uint32 agent7Tx0LastJobDoneIntClear : 1; //agent 7 (FW2) interrupt clear "tx0 last job is done", reset value: 0x0, access type: WO
		uint32 agent7Tx1LastJobDoneIntClear : 1; //agent 7 (FW2) interrupt clear "tx1 last job is done", reset value: 0x0, access type: WO
		uint32 agent7Rsvd2IntClear : 2; //agent 7 (FW2) interrupt clear Reserved, reset value: 0x0, access type: WO
		uint32 agent7Rx0LastJobDoneIntClear : 1; //agent 7 (FW2) interrupt clear "rx0 last job is done", reset value: 0x0, access type: WO
		uint32 agent7Rx1LastJobDoneIntClear : 1; //agent 7 (FW2) interrupt clear "rx1 last job is done", reset value: 0x0, access type: WO
		uint32 reserved0 : 2;
	} bitFields;
} RegDmacWrapperAgent7IntClearReg_u;

/*REG_DMAC_WRAPPER_RX0_JOB_DONE_LAST_ACC 0xB00 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rx0JobDoneLastAcc : 8; //Accumulator of job done last of Channel1, reset value: 0x0, access type: RO
		uint32 reserved0 : 23;
		uint32 rx0JobDoneLastAccExceed : 1; //Accumulator of job done last of Channel1 exceed the maximum value indication (or decrement below the zero) – Clear by write to ch1_job_done_last_acc_clr, reset value: 0x0, access type: RO
	} bitFields;
} RegDmacWrapperRx0JobDoneLastAcc_u;

/*REG_DMAC_WRAPPER_RX0_JOB_DONE_LAST_ACC_DEC 0xB04 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rx0JobDoneLastAccDec : 1; //Write 0x1 - Decrement by one the accumulator of job done last of Channel1 , reset value: 0x0, access type: WO
		uint32 reserved0 : 31;
	} bitFields;
} RegDmacWrapperRx0JobDoneLastAccDec_u;

/*REG_DMAC_WRAPPER_RX0_JOB_DONE_LAST_ACC_CLR 0xB08 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rx0JobDoneLastAccClr : 1; //Write 0x1 - Clear the accumulator of job done last of Channel1 , reset value: 0x0, access type: WO
		uint32 reserved0 : 31;
	} bitFields;
} RegDmacWrapperRx0JobDoneLastAccClr_u;

/*REG_DMAC_WRAPPER_RX1_JOB_DONE_LAST_ACC 0xB10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rx1JobDoneLastAcc : 8; //Accumulator of job done last of Channel1, reset value: 0x0, access type: RO
		uint32 reserved0 : 23;
		uint32 rx1JobDoneLastAccExceed : 1; //Accumulator of job done last of Channel1 exceed the maximum value indication (or decrement below the zero) – Clear by write to ch1_job_done_last_acc_clr, reset value: 0x0, access type: RO
	} bitFields;
} RegDmacWrapperRx1JobDoneLastAcc_u;

/*REG_DMAC_WRAPPER_RX1_JOB_DONE_LAST_ACC_DEC 0xB14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rx1JobDoneLastAccDec : 1; //Write 0x1 - Decrement by one the accumulator of job done last of Channel1 , reset value: 0x0, access type: WO
		uint32 reserved0 : 31;
	} bitFields;
} RegDmacWrapperRx1JobDoneLastAccDec_u;

/*REG_DMAC_WRAPPER_RX1_JOB_DONE_LAST_ACC_CLR 0xB18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rx1JobDoneLastAccClr : 1; //Write 0x1 - Clear the accumulator of job done last of Channel1 , reset value: 0x0, access type: WO
		uint32 reserved0 : 31;
	} bitFields;
} RegDmacWrapperRx1JobDoneLastAccClr_u;

/*REG_DMAC_WRAPPER_DMA1_CH0_CONFIG_REG 0xC00 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma1Ch0BusAndTrnsfSize : 2; //[1] - ch max transfer '0' - 128 Bytes, '1' - 256 Bytes , [0] - ch dma bus size '0' - 64bit, '1' 128bit, reset value: 0x0, access type: RW
		uint32 dma1Ch0EndianSwap : 2; //ch0 endian swap (0=none;1=within shorts;2=within words;3=within doublewords), reset value: 0x0, access type: RW
		uint32 dma1Ch0ExtFcEnable : 1; //ch0 external flow control enable, reset value: 0x0, access type: RW
		uint32 dma1Ch0RxbufFcEnable : 1; //ch0 tx buf flow control enable, reset value: 0x0, access type: RW
		uint32 dma1Ch0WmbMode : 1; //ch0 wmb enable, reset value: 0x0, access type: RW
		uint32 dma1Ch0Opt : 1; //ch0 enable load optimizations, reset value: 0x0, access type: RW
		uint32 dma1Ch0PrgLen : 8; //ch0 size of each program, reset value: 0x0, access type: RW
		uint32 dma1Ch0LegacyModeEn : 1; //'0' - legacy mode not active , '1' - legacy mode active, reset value: 0x0, access type: RW
		uint32 dma1Ch0FlushpSevN : 1; //'0' - use DMASEV op, '1' - use DMAFLUSHP op, reset value: 0x0, access type: RW
		uint32 dma1Ch0CacheInvldEn : 1; //'0' - do not add invalidate cache op at program end , '1' - add invalidate cache op at program end, reset value: 0x0, access type: RW
		uint32 dma1Ch0WmbSwOverr : 1; //SW override for WMB op. if set WMB op is placed after every store op. '0' - non active, '1' - active, reset value: 0x0, access type: RW
		uint32 reserved0 : 12;
	} bitFields;
} RegDmacWrapperDma1Ch0ConfigReg_u;

/*REG_DMAC_WRAPPER_DMA1_CH0_FIFO_PUSH_SRC_ADDR 0xC04 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma1Ch0FifoPushSrcAddr : 32; //ch0 fifo push source address, reset value: 0x0, access type: RW
	} bitFields;
} RegDmacWrapperDma1Ch0FifoPushSrcAddr_u;

/*REG_DMAC_WRAPPER_DMA1_CH0_FIFO_PUSH_DEST_ADDR 0xC08 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma1Ch0FifoPushDestAddr : 32; //ch0 fifo push destination address, reset value: 0x0, access type: RW
	} bitFields;
} RegDmacWrapperDma1Ch0FifoPushDestAddr_u;

/*REG_DMAC_WRAPPER_DMA1_CH0_FIFO_PUSH_C_REG 0xC0C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma1Ch0FifoPushLength : 15; //ch0 fifo push length, reset value: 0x0, access type: RW
		uint32 reserved0 : 1;
		uint32 dma1Ch0FifoPushEnd : 1; //ch0 fifo push end, reset value: 0x0, access type: RW
		uint32 dma1Ch0FifoPushLast : 1; //ch0 fifo push last, reset value: 0x0, access type: RW
		uint32 dma1Ch0FifoPushIdentifier : 7; //ch0 fifo push identifier, reset value: 0x0, access type: RW
		uint32 dma1Ch0FifoPushSkip : 1; //ch0 fifo skip, reset value: 0x0, access type: RW
		uint32 reserved1 : 1;
		uint32 dma1Ch0FifoPushFcByp : 1; //ch0 fifo fc bypass, reset value: 0x0, access type: RW
		uint32 reserved2 : 4;
	} bitFields;
} RegDmacWrapperDma1Ch0FifoPushCReg_u;

/*REG_DMAC_WRAPPER_DMA1_CH0_FIFO_PUSH_MPDUP_REG 0xC10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma1Ch0FifoPushMpduPtr : 16; //ch0 fifo push rx mpdu pointer, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegDmacWrapperDma1Ch0FifoPushMpdupReg_u;

/*REG_DMAC_WRAPPER_DMA1_CH1_CONFIG_REG 0xC20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma1Ch1BusAndTrnsfSize : 2; //[1] - ch max transfer '0' - 128 Bytes, '1' - 256 Bytes , [0] - ch dma bus size '0' - 64bit, '1' 128bit, reset value: 0x0, access type: RW
		uint32 dma1Ch1EndianSwap : 2; //ch1 endian swap (0=none;1=within shorts;2=within words;3=within doublewords), reset value: 0x0, access type: RW
		uint32 dma1Ch1ExtFcEnable : 1; //ch1 external flow control enable, reset value: 0x0, access type: RW
		uint32 dma1Ch1RxbufFcEnable : 1; //ch1 tx buf flow control enable, reset value: 0x0, access type: RW
		uint32 dma1Ch1WmbMode : 1; //ch1 wmb enable, reset value: 0x0, access type: RW
		uint32 dma1Ch1Opt : 1; //ch1 enable load optimizations, reset value: 0x0, access type: RW
		uint32 dma1Ch1PrgLen : 8; //ch1 size of each program, reset value: 0x0, access type: RW
		uint32 dma1Ch1LegacyModeEn : 1; //'0' - legacy mode not active , '1' - legacy mode active, reset value: 0x0, access type: RW
		uint32 dma1Ch1FlushpSevN : 1; //'0' - use DMASEV op, '1' - use DMAFLUSHP op, reset value: 0x0, access type: RW
		uint32 dma1Ch1CacheInvldEn : 1; //'0' - do not add invalidate cache op at program end , '1' - add invalidate cache op at program end, reset value: 0x0, access type: RW
		uint32 dma1Ch1WmbSwOverr : 1; //SW override for WMB op. if set WMB op is placed after every store op. '0' - non active, '1' - active, reset value: 0x0, access type: RW
		uint32 reserved0 : 12;
	} bitFields;
} RegDmacWrapperDma1Ch1ConfigReg_u;

/*REG_DMAC_WRAPPER_DMA1_CH1_FIFO_PUSH_SRC_ADDR 0xC24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma1Ch1FifoPushSrcAddr : 32; //ch1 fifo push source address, reset value: 0x0, access type: RW
	} bitFields;
} RegDmacWrapperDma1Ch1FifoPushSrcAddr_u;

/*REG_DMAC_WRAPPER_DMA1_CH1_FIFO_PUSH_DEST_ADDR 0xC28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma1Ch1FifoPushDestAddr : 32; //ch1 fifo push destination address, reset value: 0x0, access type: RW
	} bitFields;
} RegDmacWrapperDma1Ch1FifoPushDestAddr_u;

/*REG_DMAC_WRAPPER_DMA1_CH1_FIFO_PUSH_C_REG 0xC2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma1Ch1FifoPushLength : 15; //ch1 fifo push length, reset value: 0x0, access type: RW
		uint32 reserved0 : 1;
		uint32 dma1Ch1FifoPushEnd : 1; //ch1 fifo push end, reset value: 0x0, access type: RW
		uint32 dma1Ch1FifoPushLast : 1; //ch1 fifo push last, reset value: 0x0, access type: RW
		uint32 dma1Ch1FifoPushIdentifier : 7; //ch1 fifo push identifier, reset value: 0x0, access type: RW
		uint32 dma1Ch1FifoPushSkip : 1; //ch1 fifo skip, reset value: 0x0, access type: RW
		uint32 reserved1 : 1;
		uint32 dma1Ch1FifoPushFcByp : 1; //ch1 fifo fc bypass, reset value: 0x0, access type: RW
		uint32 reserved2 : 4;
	} bitFields;
} RegDmacWrapperDma1Ch1FifoPushCReg_u;

/*REG_DMAC_WRAPPER_DMA1_CH1_FIFO_PUSH_MPDUP_REG 0xC30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma1Ch1FifoPushMpduPtr : 16; //ch1 fifo push rx mpdu pointer, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegDmacWrapperDma1Ch1FifoPushMpdupReg_u;

/*REG_DMAC_WRAPPER_DMA1_CH2_CONFIG_REG 0xC40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma1Ch2BusAndTrnsfSize : 2; //[1] - ch max transfer '0' - 128 Bytes, '1' - 256 Bytes , [0] - ch dma bus size '0' - 64bit, '1' 128bit, reset value: 0x0, access type: RW
		uint32 dma1Ch2EndianSwap : 2; //ch2 endian swap (0=none;1=within shorts;2=within words;3=within doublewords), reset value: 0x0, access type: RW
		uint32 dma1Ch2ExtFcEnable : 1; //ch2 external flow control enable, reset value: 0x0, access type: RW
		uint32 reserved0 : 1;
		uint32 dma1Ch2WmbMode : 1; //ch2 wmb enable, reset value: 0x0, access type: RW
		uint32 dma1Ch2Opt : 1; //ch2 enable load optimizations, reset value: 0x0, access type: RW
		uint32 dma1Ch2PrgLen : 8; //ch2 size of each program, reset value: 0x0, access type: RW
		uint32 dma1Ch2LegacyModeEn : 1; //'0' - legacy mode not active , '1' - legacy mode active, reset value: 0x0, access type: RW
		uint32 dma1Ch2FlushpSevN : 1; //'0' - use DMASEV op, '1' - use DMAFLUSHP op, reset value: 0x0, access type: RW
		uint32 dma1Ch2CacheInvldEn : 1; //'0' - do not add invalidate cache op at program end , '1' - add invalidate cache op at program end, reset value: 0x0, access type: RW
		uint32 dma1Ch2WmbSwOverr : 1; //SW override for WMB op. if set WMB op is placed after every store op. '0' - non active, '1' - active, reset value: 0x0, access type: RW
		uint32 reserved1 : 12;
	} bitFields;
} RegDmacWrapperDma1Ch2ConfigReg_u;

/*REG_DMAC_WRAPPER_DMA1_CH2_FIFO_PUSH_SRC_ADDR 0xC44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma1Ch2FifoPushSrcAddr : 32; //ch2 fifo push source address, reset value: 0x0, access type: RW
	} bitFields;
} RegDmacWrapperDma1Ch2FifoPushSrcAddr_u;

/*REG_DMAC_WRAPPER_DMA1_CH2_FIFO_PUSH_DEST_ADDR 0xC48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma1Ch2FifoPushDestAddr : 32; //ch2 fifo push destination address, reset value: 0x0, access type: RW
	} bitFields;
} RegDmacWrapperDma1Ch2FifoPushDestAddr_u;

/*REG_DMAC_WRAPPER_DMA1_CH2_FIFO_PUSH_C_REG 0xC4C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma1Ch2FifoPushLength : 15; //ch2 fifo push length, reset value: 0x0, access type: RW
		uint32 reserved0 : 1;
		uint32 dma1Ch2FifoPushEnd : 1; //ch2 fifo push end, reset value: 0x0, access type: RW
		uint32 dma1Ch2FifoPushLast : 1; //ch2 fifo push last, reset value: 0x0, access type: RW
		uint32 dma1Ch2FifoPushIdentifier : 3; //ch2 fifo push identifier, reset value: 0x0, access type: RW
		uint32 reserved1 : 11;
	} bitFields;
} RegDmacWrapperDma1Ch2FifoPushCReg_u;

/*REG_DMAC_WRAPPER_DMA1_CH3_CONFIG_REG 0xC60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma1Ch3BusAndTrnsfSize : 2; //[1] - ch max transfer '0' - 128 Bytes, '1' - 256 Bytes , [0] - ch dma bus size '0' - 64bit, '1' 128bit, reset value: 0x0, access type: RW
		uint32 dma1Ch3EndianSwap : 2; //ch3 endian swap (0=none;1=within shorts;2=within words;3=within doublewords), reset value: 0x0, access type: RW
		uint32 dma1Ch3ExtFcEnable : 1; //ch3 external flow control enable, reset value: 0x0, access type: RW
		uint32 reserved0 : 1;
		uint32 dma1Ch3WmbMode : 1; //ch3 wmb enable, reset value: 0x0, access type: RW
		uint32 dma1Ch3Opt : 1; //ch3 enable load optimizations, reset value: 0x0, access type: RW
		uint32 dma1Ch3PrgLen : 8; //ch3 size of each program, reset value: 0x0, access type: RW
		uint32 dma1Ch3LegacyModeEn : 1; //'0' - legacy mode not active , '1' - legacy mode active, reset value: 0x0, access type: RW
		uint32 dma1Ch3FlushpSevN : 1; //'0' - use DMASEV op, '1' - use DMAFLUSHP op, reset value: 0x0, access type: RW
		uint32 dma1Ch3CacheInvldEn : 1; //'0' - do not add invalidate cache op at program end , '1' - add invalidate cache op at program end, reset value: 0x0, access type: RW
		uint32 dma1Ch3WmbSwOverr : 1; //SW override for WMB op. if set WMB op is placed after every store op. '0' - non active, '1' - active, reset value: 0x0, access type: RW
		uint32 reserved1 : 12;
	} bitFields;
} RegDmacWrapperDma1Ch3ConfigReg_u;

/*REG_DMAC_WRAPPER_DMA1_CH3_FIFO0_PUSH_SRC_ADDR 0xC64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma1Ch3Fifo0PushSrcAddr : 32; //ch3 fifo0 push source address, reset value: 0x0, access type: RW
	} bitFields;
} RegDmacWrapperDma1Ch3Fifo0PushSrcAddr_u;

/*REG_DMAC_WRAPPER_DMA1_CH3_FIFO0_PUSH_DEST_ADDR 0xC68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma1Ch3Fifo0PushDestAddr : 32; //ch3 fifo0 push destination address, reset value: 0x0, access type: RW
	} bitFields;
} RegDmacWrapperDma1Ch3Fifo0PushDestAddr_u;

/*REG_DMAC_WRAPPER_DMA1_CH3_FIFO0_PUSH_C_REG 0xC6C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma1Ch3Fifo0PushLength : 15; //ch3 fifo0 push length, reset value: 0x0, access type: RW
		uint32 reserved0 : 1;
		uint32 dma1Ch3Fifo0PushEnd : 1; //ch3 fifo0 push end, reset value: 0x0, access type: RW
		uint32 dma1Ch3Fifo0PushLast : 1; //ch3 fifo0 push last, reset value: 0x0, access type: RW
		uint32 dma1Ch3Fifo0PushIdentifier : 2; //ch3 fifo0 push identifier, reset value: 0x0, access type: RW
		uint32 reserved1 : 12;
	} bitFields;
} RegDmacWrapperDma1Ch3Fifo0PushCReg_u;

/*REG_DMAC_WRAPPER_DMA1_CH3_FIFO1_PUSH_SRC_ADDR 0xC80 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma1Ch3Fifo1PushSrcAddr : 32; //ch3 fifo1 push source address, reset value: 0x0, access type: RW
	} bitFields;
} RegDmacWrapperDma1Ch3Fifo1PushSrcAddr_u;

/*REG_DMAC_WRAPPER_DMA1_CH3_FIFO1_PUSH_DEST_ADDR 0xC84 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma1Ch3Fifo1PushDestAddr : 32; //ch3 fifo1 push destination address, reset value: 0x0, access type: RW
	} bitFields;
} RegDmacWrapperDma1Ch3Fifo1PushDestAddr_u;

/*REG_DMAC_WRAPPER_DMA1_CH3_FIFO1_PUSH_C_REG 0xC88 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma1Ch3Fifo1PushLength : 15; //ch3 fifo1 push length, reset value: 0x0, access type: RW
		uint32 reserved0 : 1;
		uint32 dma1Ch3Fifo1PushEnd : 1; //ch3 fifo1 push end, reset value: 0x0, access type: RW
		uint32 dma1Ch3Fifo1PushLast : 1; //ch3 fifo1 push last, reset value: 0x0, access type: RW
		uint32 dma1Ch3Fifo1PushIdentifier : 2; //ch3 fifo1 push identifier, reset value: 0x0, access type: RW
		uint32 reserved1 : 12;
	} bitFields;
} RegDmacWrapperDma1Ch3Fifo1PushCReg_u;

/*REG_DMAC_WRAPPER_DMA1_CH3_FIFO2_PUSH_SRC_ADDR 0xC90 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma1Ch3Fifo2PushSrcAddr : 32; //ch3 fifo2 push source address, reset value: 0x0, access type: RW
	} bitFields;
} RegDmacWrapperDma1Ch3Fifo2PushSrcAddr_u;

/*REG_DMAC_WRAPPER_DMA1_CH3_FIFO2_PUSH_DEST_ADDR 0xC94 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma1Ch3Fifo2PushDestAddr : 32; //ch3 fifo2 push destination address, reset value: 0x0, access type: RW
	} bitFields;
} RegDmacWrapperDma1Ch3Fifo2PushDestAddr_u;

/*REG_DMAC_WRAPPER_DMA1_CH3_FIFO2_PUSH_C_REG 0xC98 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma1Ch3Fifo2PushLength : 15; //ch3 fifo2 push length, reset value: 0x0, access type: RW
		uint32 reserved0 : 1;
		uint32 dma1Ch3Fifo2PushEnd : 1; //ch3 fifo2 push end, reset value: 0x0, access type: RW
		uint32 dma1Ch3Fifo2PushLast : 1; //ch3 fifo2 push last, reset value: 0x0, access type: RW
		uint32 dma1Ch3Fifo2PushIdentifier : 2; //ch3 fifo2 push identifier, reset value: 0x0, access type: RW
		uint32 reserved1 : 12;
	} bitFields;
} RegDmacWrapperDma1Ch3Fifo2PushCReg_u;

/*REG_DMAC_WRAPPER_DMA0_CH0_CONFIG_REG 0xD00 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma0Ch0BusAndTrnsfSize : 2; //[1] - ch max transfer '0' - 128 Bytes, '1' - 256 Bytes , [0] - ch dma bus size '0' - 64bit, '1' 128bit, reset value: 0x0, access type: RW
		uint32 dma0Ch0EndianSwap : 2; //ch0 endian swap (0=none;1=within shorts;2=within words;3=within doublewords), reset value: 0x0, access type: RW
		uint32 dma0Ch0ExtFcEnable : 1; //ch0 external flow control enable, reset value: 0x0, access type: RW
		uint32 dma0Ch0TxbufFcEnable : 1; //ch0 tx buf flow control enable, reset value: 0x0, access type: RW
		uint32 dma0Ch0WmbMode : 1; //ch0 wmb enable, reset value: 0x0, access type: RW
		uint32 dma0Ch0Opt : 1; //ch0 enable load optimizations, reset value: 0x0, access type: RW
		uint32 dma0Ch0PrgLen : 8; //ch0 size of each program, reset value: 0x0, access type: RW
		uint32 dma0Ch0LegacyModeEn : 1; //'0' - legacy mode not active , '1' - legacy mode active, reset value: 0x0, access type: RW
		uint32 dma0Ch0FlushpSevN : 1; //'0' - use DMASEV op, '1' - use DMAFLUSHP op, reset value: 0x0, access type: RW
		uint32 dma0Ch0CacheInvldEn : 1; //'0' - do not add invalidate cache op at program end , '1' - add invalidate cache op at program end, reset value: 0x0, access type: RW
		uint32 dma0Ch0WmbSwOverr : 1; //SW override for WMB op. if set WMB op is placed after every store op. '0' - non active, '1' - active, reset value: 0x0, access type: RW
		uint32 reserved0 : 12;
	} bitFields;
} RegDmacWrapperDma0Ch0ConfigReg_u;

/*REG_DMAC_WRAPPER_DMA0_CH0_FIFO_PUSH_SRC_ADDR 0xD04 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma0Ch0FifoPushSrcAddr : 32; //ch0 fifo push source address, reset value: 0x0, access type: RW
	} bitFields;
} RegDmacWrapperDma0Ch0FifoPushSrcAddr_u;

/*REG_DMAC_WRAPPER_DMA0_CH0_FIFO_PUSH_DEST_ADDR 0xD08 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma0Ch0FifoPushDestAddr : 32; //ch0 fifo push destination address, reset value: 0x0, access type: RW
	} bitFields;
} RegDmacWrapperDma0Ch0FifoPushDestAddr_u;

/*REG_DMAC_WRAPPER_DMA0_CH0_FIFO_PUSH_C_REG 0xD0C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma0Ch0FifoPushLength : 15; //ch0 fifo push length, reset value: 0x0, access type: RW
		uint32 reserved0 : 1;
		uint32 dma0Ch0FifoPushEnd : 1; //ch0 fifo push end, reset value: 0x0, access type: RW
		uint32 dma0Ch0FifoPushLast : 1; //ch0 fifo push last, reset value: 0x0, access type: RW
		uint32 dma0Ch0FifoPushIdentifier : 7; //ch0 fifo push identifier, reset value: 0x0, access type: RW
		uint32 dma0Ch0FifoPushSkip : 1; //ch0 fifo skip, reset value: 0x0, access type: RW
		uint32 reserved1 : 1;
		uint32 dma0Ch0FifoPushFcByp : 1; //ch0 fifo push fc bypass, reset value: 0x0, access type: RW
		uint32 reserved2 : 4;
	} bitFields;
} RegDmacWrapperDma0Ch0FifoPushCReg_u;

/*REG_DMAC_WRAPPER_DMA0_CH0_FIFO_PUSH_MPDUP_REG 0xD10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma0Ch0FifoPushTxMpduPtr : 16; //ch0 fifo push tx mpdu pointer, reset value: 0x0, access type: RW
		uint32 dma0Ch0FifoPushDoMpduHdr : 1; //ch0 fifo push tx do MPDU Header bit for Header Converdion module, reset value: 0x0, access type: RW
		uint32 dma0Ch0FifoPushDoMsduHdr : 1; //ch0 fifo push tx do MSDU Header bit for Header Converdion module, reset value: 0x0, access type: RW
		uint32 dma0Ch0FifoPushAddSecurity : 1; //ch0 fifo push tx add security bit for Header Converdion module, reset value: 0x0, access type: RW
		uint32 dma0Ch0FifoPushFirstMsduInMpdu : 1; //ch0 fifo push tx first MSDU in MPDU bit for Header Converdion module, reset value: 0x0, access type: RW
		uint32 dma0Ch0FifoPushLastMsduInMpdu : 1; //ch0 fifo push tx last MSDU in MPDU bit for Header Converdion module, reset value: 0x0, access type: RW
		uint32 dma0Ch0FifoPushEtype : 2; //ch0 fifo push tx Etype bits for Header Converdion module, reset value: 0x0, access type: RW
		uint32 dma0Ch0FifoPushAmsduPadd : 2; //ch0 fifo push tx AMSDU padding bits for Header Converdion module, reset value: 0x0, access type: RW
		uint32 dma0Ch0FifoPushSpp : 1; //ch0 fifo push tx SSP bit for Header Converdion module, reset value: 0x0, access type: RW
		uint32 reserved0 : 6;
	} bitFields;
} RegDmacWrapperDma0Ch0FifoPushMpdupReg_u;

/*REG_DMAC_WRAPPER_DMA0_CH1_CONFIG_REG 0xD20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma0Ch1BusAndTrnsfSize : 2; //[1] - ch max transfer '0' - 128 Bytes, '1' - 256 Bytes , [0] - ch dma bus size '0' - 64bit, '1' 128bit, reset value: 0x0, access type: RW
		uint32 dma0Ch1EndianSwap : 2; //ch1 endian swap (0=none;1=within shorts;2=within words;3=within doublewords), reset value: 0x0, access type: RW
		uint32 dma0Ch1ExtFcEnable : 1; //ch1 external flow control enable, reset value: 0x0, access type: RW
		uint32 dma0Ch1TxbufFcEnable : 1; //ch1 tx buf flow control enable, reset value: 0x0, access type: RW
		uint32 dma0Ch1WmbMode : 1; //ch1 wmb enable, reset value: 0x0, access type: RW
		uint32 dma0Ch1Opt : 1; //ch1 enable load optimizations, reset value: 0x0, access type: RW
		uint32 dma0Ch1PrgLen : 8; //ch1 size of each program, reset value: 0x0, access type: RW
		uint32 dma0Ch1LegacyModeEn : 1; //'0' - legacy mode not active , '1' - legacy mode active, reset value: 0x0, access type: RW
		uint32 dma0Ch1FlushpSevN : 1; //'0' - use DMASEV op, '1' - use DMAFLUSHP op, reset value: 0x0, access type: RW
		uint32 dma0Ch1CacheInvldEn : 1; //'0' - do not add invalidate cache op at program end , '1' - add invalidate cache op at program end, reset value: 0x0, access type: RW
		uint32 dma0Ch1WmbSwOverr : 1; //SW override for WMB op. if set WMB op is placed after every store op. '0' - non active, '1' - active, reset value: 0x0, access type: RW
		uint32 reserved0 : 12;
	} bitFields;
} RegDmacWrapperDma0Ch1ConfigReg_u;

/*REG_DMAC_WRAPPER_DMA0_CH1_FIFO_PUSH_SRC_ADDR 0xD24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma0Ch1FifoPushSrcAddr : 32; //ch1 fifo push source address, reset value: 0x0, access type: RW
	} bitFields;
} RegDmacWrapperDma0Ch1FifoPushSrcAddr_u;

/*REG_DMAC_WRAPPER_DMA0_CH1_FIFO_PUSH_DEST_ADDR 0xD28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma0Ch1FifoPushDestAddr : 32; //ch1 fifo push destination address, reset value: 0x0, access type: RW
	} bitFields;
} RegDmacWrapperDma0Ch1FifoPushDestAddr_u;

/*REG_DMAC_WRAPPER_DMA0_CH1_FIFO_PUSH_C_REG 0xD2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma0Ch1FifoPushLength : 15; //ch1 fifo push length, reset value: 0x0, access type: RW
		uint32 reserved0 : 1;
		uint32 dma0Ch1FifoPushEnd : 1; //ch1 fifo push end, reset value: 0x0, access type: RW
		uint32 dma0Ch1FifoPushLast : 1; //ch1 fifo push last, reset value: 0x0, access type: RW
		uint32 dma0Ch1FifoPushIdentifier : 7; //ch1 fifo push identifier, reset value: 0x0, access type: RW
		uint32 dma0Ch1FifoPushSkip : 1; //ch1 fifo skip, reset value: 0x0, access type: RW
		uint32 reserved1 : 1;
		uint32 dma0Ch1FifoPushFcByp : 1; //ch1 fifo fc bypass, reset value: 0x0, access type: RW
		uint32 reserved2 : 4;
	} bitFields;
} RegDmacWrapperDma0Ch1FifoPushCReg_u;

/*REG_DMAC_WRAPPER_DMA0_CH1_FIFO_PUSH_MPDUP_REG 0xD30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma0Ch1FifoPushTxMpduPtr : 16; //ch1 fifo push tx mpdu pointer, reset value: 0x0, access type: RW
		uint32 dma0Ch1FifoPushDoMpduHdr : 1; //ch1 fifo push tx do MPDU Header bit for Header Converdion module, reset value: 0x0, access type: RW
		uint32 dma0Ch1FifoPushDoMsduHdr : 1; //ch1 fifo push tx do MSDU Header bit for Header Converdion module, reset value: 0x0, access type: RW
		uint32 dma0Ch1FifoPushAddSecurity : 1; //ch1 fifo push tx add security bit for Header Converdion module, reset value: 0x0, access type: RW
		uint32 dma0Ch1FifoPushFirstMsduInMpdu : 1; //ch1 fifo push tx first MSDU in MPDU bit for Header Converdion module, reset value: 0x0, access type: RW
		uint32 dma0Ch1FifoPushLastMsduInMpdu : 1; //ch1 fifo push tx last MSDU in MPDU bit for Header Converdion module, reset value: 0x0, access type: RW
		uint32 dma0Ch1FifoPushEtype : 2; //ch1 fifo push tx Etype bits for Header Converdion module, reset value: 0x0, access type: RW
		uint32 dma0Ch1FifoPushAmsduPadd : 2; //ch1 fifo push tx AMSDU padding bits for Header Converdion module, reset value: 0x0, access type: RW
		uint32 dma0Ch1FifoPushSpp : 1; //ch1 fifo push tx SSP bit for Header Converdion module, reset value: 0x0, access type: RW
		uint32 reserved0 : 6;
	} bitFields;
} RegDmacWrapperDma0Ch1FifoPushMpdupReg_u;

/*REG_DMAC_WRAPPER_DMA1_CH0_JOB_CNT_REG 0xF00 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma1Ch0JobCnt : 10; //ch0 jobs counter, reset value: 0x0, access type: RO
		uint32 reserved0 : 22;
	} bitFields;
} RegDmacWrapperDma1Ch0JobCntReg_u;

/*REG_DMAC_WRAPPER_DMA1_CH0_BATCH_CNT_REG 0xF04 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma1Ch0BatchCnt : 10; //ch0 batch counter (cleared on read), reset value: 0x0, access type: RO
		uint32 reserved0 : 22;
	} bitFields;
} RegDmacWrapperDma1Ch0BatchCntReg_u;

/*REG_DMAC_WRAPPER_DMA1_CH0_STAT_IND_REG 0xF08 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma1Ch0StatusLastJobIndicationTog : 1; //ch0 status last job indication (toggling), reset value: 0x0, access type: RO
		uint32 dma1Ch0StatusLastIdJob : 7; //ch0 status last id job, reset value: 0x0, access type: RO
		uint32 dma1Ch0StatusFirstProgramReady : 1; //ch0 status first program ready, reset value: 0x0, access type: RO
		uint32 dma1Ch0StatusSecondProgramReady : 1; //ch0 status second program ready, reset value: 0x0, access type: RO
		uint32 dma1Ch0StatusChannelControllerSm : 3; //ch0 status channel controller state machine, reset value: 0x0, access type: RO
		uint32 dma1Ch0StatusAbortSm : 2; //ch0 status abort state machine, reset value: 0x0, access type: RO
		uint32 dma1Ch0StatusIsIdle : 1; //ch0 status channel controller is idle indication, reset value: 0x1, access type: RO
		uint32 dma1Ch0StatusIsDead : 1; //ch0 status channel controller is dead indication, reset value: 0x1, access type: RO
		uint32 dma1Ch0StatusDmaChannelActive : 1; //ch0 status DMAC channel active indication, reset value: 0x0, access type: RO
		uint32 reserved0 : 14;
	} bitFields;
} RegDmacWrapperDma1Ch0StatIndReg_u;

/*REG_DMAC_WRAPPER_DMA1_CH1_JOB_CNT_REG 0xF10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma1Ch1JobCnt : 10; //ch1 jobs counter, reset value: 0x0, access type: RO
		uint32 reserved0 : 22;
	} bitFields;
} RegDmacWrapperDma1Ch1JobCntReg_u;

/*REG_DMAC_WRAPPER_DMA1_CH1_BATCH_CNT_REG 0xF14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma1Ch1BatchCnt : 10; //ch1 batch counter (cleared on read), reset value: 0x0, access type: RO
		uint32 reserved0 : 22;
	} bitFields;
} RegDmacWrapperDma1Ch1BatchCntReg_u;

/*REG_DMAC_WRAPPER_DMA1_CH1_STAT_IND_REG 0xF18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma1Ch1StatusLastJobIndicationTog : 1; //ch1 status last job indication (toggling), reset value: 0x0, access type: RO
		uint32 dma1Ch1StatusLastIdJob : 7; //ch1 status last id job, reset value: 0x0, access type: RO
		uint32 dma1Ch1StatusFirstProgramReady : 1; //ch1 status first program ready, reset value: 0x0, access type: RO
		uint32 dma1Ch1StatusSecondProgramReady : 1; //ch1 status second program ready, reset value: 0x0, access type: RO
		uint32 dma1Ch1StatusChannelControllerSm : 3; //ch1 status channel controller state machine, reset value: 0x0, access type: RO
		uint32 dma1Ch1StatusAbortSm : 2; //ch1 status abort state machine, reset value: 0x0, access type: RO
		uint32 dma1Ch1StatusIsIdle : 1; //ch1 status channel controller is idle indication, reset value: 0x1, access type: RO
		uint32 dma1Ch1StatusIsDead : 1; //ch1 status channel controller is dead indication, reset value: 0x1, access type: RO
		uint32 dma1Ch1StatusDmaChannelActive : 1; //ch1 status DMAC channel active indication, reset value: 0x0, access type: RO
		uint32 reserved0 : 14;
	} bitFields;
} RegDmacWrapperDma1Ch1StatIndReg_u;

/*REG_DMAC_WRAPPER_DMA1_CH2_JOB_CNT_REG 0xF20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma1Ch2JobCnt : 10; //ch2 jobs counter, reset value: 0x0, access type: RO
		uint32 reserved0 : 22;
	} bitFields;
} RegDmacWrapperDma1Ch2JobCntReg_u;

/*REG_DMAC_WRAPPER_DMA1_CH2_BATCH_CNT_REG 0xF24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma1Ch2BatchCnt : 10; //ch2 batch counter (cleared on read), reset value: 0x0, access type: RO
		uint32 reserved0 : 22;
	} bitFields;
} RegDmacWrapperDma1Ch2BatchCntReg_u;

/*REG_DMAC_WRAPPER_DMA1_CH2_STAT_IND_REG 0xF28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma1Ch2StatusLastJobIndicationTog : 1; //ch2 status last job indication (toggling), reset value: 0x0, access type: RO
		uint32 dma1Ch2StatusLastIdJob : 7; //ch2 status last id job, reset value: 0x0, access type: RO
		uint32 dma1Ch2StatusFirstProgramReady : 1; //ch2 status first program ready, reset value: 0x0, access type: RO
		uint32 dma1Ch2StatusSecondProgramReady : 1; //ch2 status second program ready, reset value: 0x0, access type: RO
		uint32 dma1Ch2StatusChannelControllerSm : 3; //ch2 status channel controller state machine, reset value: 0x0, access type: RO
		uint32 dma1Ch2StatusAbortSm : 2; //ch2 status abort state machine, reset value: 0x0, access type: RO
		uint32 dma1Ch2StatusIsIdle : 1; //ch2 status channel controller is idle indication, reset value: 0x1, access type: RO
		uint32 dma1Ch2StatusIsDead : 1; //ch2 status channel controller is dead indication, reset value: 0x1, access type: RO
		uint32 dma1Ch2StatusDmaChannelActive : 1; //ch2 status DMAC channel active indication, reset value: 0x0, access type: RO
		uint32 reserved0 : 14;
	} bitFields;
} RegDmacWrapperDma1Ch2StatIndReg_u;

/*REG_DMAC_WRAPPER_DMA1_CH3_JOB_CNT_REG 0xF30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma1Ch3JobCnt : 10; //ch3 jobs counter, reset value: 0x0, access type: RO
		uint32 reserved0 : 22;
	} bitFields;
} RegDmacWrapperDma1Ch3JobCntReg_u;

/*REG_DMAC_WRAPPER_DMA1_CH3_BATCH_CNT_REG 0xF34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma1Ch3BatchCnt : 10; //ch3 batch counter (cleared on read), reset value: 0x0, access type: RO
		uint32 reserved0 : 22;
	} bitFields;
} RegDmacWrapperDma1Ch3BatchCntReg_u;

/*REG_DMAC_WRAPPER_DMA1_CH3_STAT_IND_REG 0xF38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma1Ch3StatusLastJobIndicationTog : 1; //ch3 status last job indication (toggling), reset value: 0x0, access type: RO
		uint32 dma1Ch3StatusLastIdJob : 7; //ch3 status last id job, reset value: 0x0, access type: RO
		uint32 dma1Ch3StatusFirstProgramReady : 1; //ch3 status first program ready, reset value: 0x0, access type: RO
		uint32 dma1Ch3StatusSecondProgramReady : 1; //ch3 status second program ready, reset value: 0x0, access type: RO
		uint32 dma1Ch3StatusChannelControllerSm : 3; //ch3 status channel controller state machine, reset value: 0x0, access type: RO
		uint32 dma1Ch3StatusAbortSm : 2; //ch3 status abort state machine, reset value: 0x0, access type: RO
		uint32 dma1Ch3StatusIsIdle : 1; //ch3 status channel controller is idle indication, reset value: 0x1, access type: RO
		uint32 dma1Ch3StatusIsDead : 1; //ch3 status channel controller is dead indication, reset value: 0x1, access type: RO
		uint32 dma1Ch3StatusDmaChannelActive : 1; //ch3 status DMAC channel active indication, reset value: 0x0, access type: RO
		uint32 reserved0 : 14;
	} bitFields;
} RegDmacWrapperDma1Ch3StatIndReg_u;

/*REG_DMAC_WRAPPER_DMA0_CH0_JOB_CNT_REG 0xF40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma0Ch0JobCnt : 10; //ch0 jobs counter, reset value: 0x0, access type: RO
		uint32 reserved0 : 22;
	} bitFields;
} RegDmacWrapperDma0Ch0JobCntReg_u;

/*REG_DMAC_WRAPPER_DMA0_CH0_BATCH_CNT_REG 0xF44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma0Ch0BatchCnt : 10; //ch0 batch counter (cleared on read), reset value: 0x0, access type: RO
		uint32 reserved0 : 22;
	} bitFields;
} RegDmacWrapperDma0Ch0BatchCntReg_u;

/*REG_DMAC_WRAPPER_DMA0_CH0_STAT_IND_REG 0xF48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma0Ch0StatusLastJobIndicationTog : 1; //ch0 status last job indication (toggling), reset value: 0x0, access type: RO
		uint32 dma0Ch0StatusLastIdJob : 7; //ch0 status last id job, reset value: 0x0, access type: RO
		uint32 dma0Ch0StatusFirstProgramReady : 1; //ch0 status first program ready, reset value: 0x0, access type: RO
		uint32 dma0Ch0StatusSecondProgramReady : 1; //ch0 status second program ready, reset value: 0x0, access type: RO
		uint32 dma0Ch0StatusChannelControllerSm : 3; //ch0 status channel controller state machine, reset value: 0x0, access type: RO
		uint32 dma0Ch0StatusAbortSm : 2; //ch0 status abort state machine, reset value: 0x0, access type: RO
		uint32 dma0Ch0StatusIsIdle : 1; //ch0 status channel controller is idle indication, reset value: 0x1, access type: RO
		uint32 dma0Ch0StatusIsDead : 1; //ch0 status channel controller is dead indication, reset value: 0x1, access type: RO
		uint32 dma0Ch0StatusDmaChannelActive : 1; //ch0 status DMAC channel active indication, reset value: 0x0, access type: RO
		uint32 reserved0 : 14;
	} bitFields;
} RegDmacWrapperDma0Ch0StatIndReg_u;

/*REG_DMAC_WRAPPER_DMA0_CH1_JOB_CNT_REG 0xF50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma0Ch1JobCnt : 10; //ch1 jobs counter, reset value: 0x0, access type: RO
		uint32 reserved0 : 22;
	} bitFields;
} RegDmacWrapperDma0Ch1JobCntReg_u;

/*REG_DMAC_WRAPPER_DMA0_CH1_BATCH_CNT_REG 0xF54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma0Ch1BatchCnt : 10; //ch1 batch counter (cleared on read), reset value: 0x0, access type: RO
		uint32 reserved0 : 22;
	} bitFields;
} RegDmacWrapperDma0Ch1BatchCntReg_u;

/*REG_DMAC_WRAPPER_DMA0_CH1_STAT_IND_REG 0xF58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dma0Ch1StatusLastJobIndicationTog : 1; //ch1 status last job indication (toggling), reset value: 0x0, access type: RO
		uint32 dma0Ch1StatusLastIdJob : 7; //ch1 status last id job, reset value: 0x0, access type: RO
		uint32 dma0Ch1StatusFirstProgramReady : 1; //ch1 status first program ready, reset value: 0x0, access type: RO
		uint32 dma0Ch1StatusSecondProgramReady : 1; //ch1 status second program ready, reset value: 0x0, access type: RO
		uint32 dma0Ch1StatusChannelControllerSm : 3; //ch1 status channel controller state machine, reset value: 0x0, access type: RO
		uint32 dma0Ch1StatusAbortSm : 2; //ch1 status abort state machine, reset value: 0x0, access type: RO
		uint32 dma0Ch1StatusIsIdle : 1; //ch1 status channel controller is idle indication, reset value: 0x1, access type: RO
		uint32 dma0Ch1StatusIsDead : 1; //ch1 status channel controller is dead indication, reset value: 0x1, access type: RO
		uint32 dma0Ch1StatusDmaChannelActive : 1; //ch1 status DMAC channel active indication, reset value: 0x0, access type: RO
		uint32 reserved0 : 14;
	} bitFields;
} RegDmacWrapperDma0Ch1StatIndReg_u;

/*REG_DMAC_WRAPPER_FIFO0_STAT_IND_REG 0xF60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fifo0StatusJobFifoCount : 3; //fifo0 (TX0) status job fifo count, reset value: 0x0, access type: RO
		uint32 reserved0 : 1;
		uint32 fifo0StatusJobFifoEmpty : 1; //fifo0 (TX0) status job fifo empty, reset value: 0x1, access type: RO
		uint32 fifo0StatusJobFifoFull : 1; //fifo0 (TX0) status job fifo full, reset value: 0x0, access type: RO
		uint32 fifo0StatusLastFifoPushIgnored : 1; //fifo0 (TX0) status last fifo push ignored, reset value: 0x0, access type: RO
		uint32 reserved1 : 25;
	} bitFields;
} RegDmacWrapperFifo0StatIndReg_u;

/*REG_DMAC_WRAPPER_FIFO1_STAT_IND_REG 0xF64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fifo1StatusJobFifoCount : 3; //fifo1 (TX1) status job fifo count, reset value: 0x0, access type: RO
		uint32 reserved0 : 1;
		uint32 fifo1StatusJobFifoEmpty : 1; //fifo1 (TX1) status job fifo empty, reset value: 0x1, access type: RO
		uint32 fifo1StatusJobFifoFull : 1; //fifo1 (TX1) status job fifo full, reset value: 0x0, access type: RO
		uint32 fifo1StatusLastFifoPushIgnored : 1; //fifo1 (TX1) status last fifo push ignored, reset value: 0x0, access type: RO
		uint32 reserved1 : 25;
	} bitFields;
} RegDmacWrapperFifo1StatIndReg_u;

/*REG_DMAC_WRAPPER_FIFO2_STAT_IND_REG 0xF68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fifo2StatusJobFifoCount : 3; //fifo2 (RX0) status job fifo count, reset value: 0x0, access type: RO
		uint32 reserved0 : 1;
		uint32 fifo2StatusJobFifoEmpty : 1; //fifo2 (RX0) status job fifo empty, reset value: 0x1, access type: RO
		uint32 fifo2StatusJobFifoFull : 1; //fifo2 (RX0) status job fifo full, reset value: 0x0, access type: RO
		uint32 fifo2StatusLastFifoPushIgnored : 1; //fifo2 (RX0) status last fifo push ignored, reset value: 0x0, access type: RO
		uint32 reserved1 : 25;
	} bitFields;
} RegDmacWrapperFifo2StatIndReg_u;

/*REG_DMAC_WRAPPER_FIFO3_STAT_IND_REG 0xF6C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fifo3StatusJobFifoCount : 3; //fifo3 (RX1) status job fifo count, reset value: 0x0, access type: RO
		uint32 reserved0 : 1;
		uint32 fifo3StatusJobFifoEmpty : 1; //fifo3 (RX1) status job fifo empty, reset value: 0x1, access type: RO
		uint32 fifo3StatusJobFifoFull : 1; //fifo3 (RX1) status job fifo full, reset value: 0x0, access type: RO
		uint32 fifo3StatusLastFifoPushIgnored : 1; //fifo3 (RX1) status last fifo push ignored, reset value: 0x0, access type: RO
		uint32 reserved1 : 25;
	} bitFields;
} RegDmacWrapperFifo3StatIndReg_u;

/*REG_DMAC_WRAPPER_FIFO4_STAT_IND_REG 0xF70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fifo4StatusJobFifoCount : 3; //fifo4 (HOST) status job fifo count, reset value: 0x0, access type: RO
		uint32 reserved0 : 1;
		uint32 fifo4StatusJobFifoEmpty : 1; //fifo4 (HOST) status job fifo empty, reset value: 0x1, access type: RO
		uint32 fifo4StatusJobFifoFull : 1; //fifo4 (HOST) status job fifo full, reset value: 0x0, access type: RO
		uint32 fifo4StatusLastFifoPushIgnored : 1; //fifo4 (HOST) status last fifo push ignored, reset value: 0x0, access type: RO
		uint32 reserved1 : 25;
	} bitFields;
} RegDmacWrapperFifo4StatIndReg_u;

/*REG_DMAC_WRAPPER_FIFO5_STAT_IND_REG 0xF74 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fifo5StatusJobFifoCount : 3; //fifo5 (CPU0) status job fifo count, reset value: 0x0, access type: RO
		uint32 reserved0 : 1;
		uint32 fifo5StatusJobFifoEmpty : 1; //fifo5 (CPU0) status job fifo empty, reset value: 0x1, access type: RO
		uint32 fifo5StatusJobFifoFull : 1; //fifo5 (CPU0) status job fifo full, reset value: 0x0, access type: RO
		uint32 fifo5StatusLastFifoPushIgnored : 1; //fifo5 (CPU0) status last fifo push ignored, reset value: 0x0, access type: RO
		uint32 reserved1 : 25;
	} bitFields;
} RegDmacWrapperFifo5StatIndReg_u;

/*REG_DMAC_WRAPPER_FIFO6_STAT_IND_REG 0xF78 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fifo6StatusJobFifoCount : 3; //fifo6 (CPU1) status job fifo count, reset value: 0x0, access type: RO
		uint32 reserved0 : 1;
		uint32 fifo6StatusJobFifoEmpty : 1; //fifo6 (CPU1) status job fifo empty, reset value: 0x1, access type: RO
		uint32 fifo6StatusJobFifoFull : 1; //fifo6 (CPU1) status job fifo full, reset value: 0x0, access type: RO
		uint32 fifo6StatusLastFifoPushIgnored : 1; //fifo6 (CPU1) status last fifo push ignored, reset value: 0x0, access type: RO
		uint32 reserved1 : 25;
	} bitFields;
} RegDmacWrapperFifo6StatIndReg_u;

/*REG_DMAC_WRAPPER_FIFO7_STAT_IND_REG 0xF7C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fifo7StatusJobFifoCount : 3; //fifo7 (CPU2) status job fifo count, reset value: 0x0, access type: RO
		uint32 reserved0 : 1;
		uint32 fifo7StatusJobFifoEmpty : 1; //fifo7 (CPU2) status job fifo empty, reset value: 0x1, access type: RO
		uint32 fifo7StatusJobFifoFull : 1; //fifo7 (CPU2) status job fifo full, reset value: 0x0, access type: RO
		uint32 fifo7StatusLastFifoPushIgnored : 1; //fifo7 (CPU2) status last fifo push ignored, reset value: 0x0, access type: RO
		uint32 reserved1 : 25;
	} bitFields;
} RegDmacWrapperFifo7StatIndReg_u;

/*REG_DMAC_WRAPPER_SPARE_32_BIT_REG 0xFF0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dmacSpare32Bit : 32; //no description, reset value: 0x88888888, access type: RW
	} bitFields;
} RegDmacWrapperSpare32BitReg_u;



#endif // _DMAC_WRAPPER_REGS_H_
