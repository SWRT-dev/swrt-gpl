
/***********************************************************************************
File:				HostIfAccRegs.h
Module:				hostIfAcc
SOC Revision:		843
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _HOST_IF_ACC_REGS_H_
#define _HOST_IF_ACC_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define HOST_IF_ACC_BASE_ADDRESS                             MEMORY_MAP_UNIT_39_BASE_ADDRESS
#define	REG_HOST_IF_ACC_SW_CLEAR_INSTRUCTION                   (HOST_IF_ACC_BASE_ADDRESS + 0x0)
#define	REG_HOST_IF_ACC_TX_IN_SW_INSTRUCTION                   (HOST_IF_ACC_BASE_ADDRESS + 0x4)
#define	REG_HOST_IF_ACC_TX_OUT_SW_INSTRUCTION                  (HOST_IF_ACC_BASE_ADDRESS + 0x8)
#define	REG_HOST_IF_ACC_RX_IN_SW_INSTRUCTION                   (HOST_IF_ACC_BASE_ADDRESS + 0xC)
#define	REG_HOST_IF_ACC_RX_OUT_SW_INSTRUCTION                  (HOST_IF_ACC_BASE_ADDRESS + 0x10)
#define	REG_HOST_IF_ACC_TX_IN_CONV_CFG                         (HOST_IF_ACC_BASE_ADDRESS + 0x14)
#define	REG_HOST_IF_ACC_TX_OUT_CONV_CFG                        (HOST_IF_ACC_BASE_ADDRESS + 0x18)
#define	REG_HOST_IF_ACC_RX_IN_CONV_CFG                         (HOST_IF_ACC_BASE_ADDRESS + 0x1C)
#define	REG_HOST_IF_ACC_RX_OUT_CONV_CFG                        (HOST_IF_ACC_BASE_ADDRESS + 0x20)
#define	REG_HOST_IF_ACC_TX_IN_HD_BUF0_BASE_ADDR                (HOST_IF_ACC_BASE_ADDRESS + 0x24)
#define	REG_HOST_IF_ACC_TX_IN_HD_BUF1_BASE_ADDR                (HOST_IF_ACC_BASE_ADDRESS + 0x28)
#define	REG_HOST_IF_ACC_TX_OUT_HD_BUF0_BASE_ADDR               (HOST_IF_ACC_BASE_ADDRESS + 0x2C)
#define	REG_HOST_IF_ACC_TX_OUT_HD_BUF1_BASE_ADDR               (HOST_IF_ACC_BASE_ADDRESS + 0x30)
#define	REG_HOST_IF_ACC_RX_IN_HD_BUF0_BASE_ADDR                (HOST_IF_ACC_BASE_ADDRESS + 0x34)
#define	REG_HOST_IF_ACC_RX_IN_HD_BUF1_BASE_ADDR                (HOST_IF_ACC_BASE_ADDRESS + 0x38)
#define	REG_HOST_IF_ACC_RX_OUT_HD_BUF0_BASE_ADDR               (HOST_IF_ACC_BASE_ADDRESS + 0x3C)
#define	REG_HOST_IF_ACC_RX_OUT_HD_BUF1_BASE_ADDR               (HOST_IF_ACC_BASE_ADDRESS + 0x40)
#define	REG_HOST_IF_ACC_TX_IN_COUNTERS                         (HOST_IF_ACC_BASE_ADDRESS + 0x44)
#define	REG_HOST_IF_ACC_TX_OUT_COUNTERS                        (HOST_IF_ACC_BASE_ADDRESS + 0x48)
#define	REG_HOST_IF_ACC_RX_IN_COUNTERS                         (HOST_IF_ACC_BASE_ADDRESS + 0x4C)
#define	REG_HOST_IF_ACC_RX_OUT_COUNTERS                        (HOST_IF_ACC_BASE_ADDRESS + 0x50)
#define	REG_HOST_IF_ACC_TX_IN_HD_COUNT_BUF0                    (HOST_IF_ACC_BASE_ADDRESS + 0x54)
#define	REG_HOST_IF_ACC_TX_IN_HD_COUNT_BUF1                    (HOST_IF_ACC_BASE_ADDRESS + 0x58)
#define	REG_HOST_IF_ACC_TX_OUT_HD_COUNT_BUF0                   (HOST_IF_ACC_BASE_ADDRESS + 0x5C)
#define	REG_HOST_IF_ACC_TX_OUT_HD_COUNT_BUF1                   (HOST_IF_ACC_BASE_ADDRESS + 0x60)
#define	REG_HOST_IF_ACC_RX_IN_HD_COUNT_BUF0                    (HOST_IF_ACC_BASE_ADDRESS + 0x64)
#define	REG_HOST_IF_ACC_RX_IN_HD_COUNT_BUF1                    (HOST_IF_ACC_BASE_ADDRESS + 0x68)
#define	REG_HOST_IF_ACC_RX_OUT_HD_COUNT_BUF0                   (HOST_IF_ACC_BASE_ADDRESS + 0x6C)
#define	REG_HOST_IF_ACC_RX_OUT_HD_COUNT_BUF1                   (HOST_IF_ACC_BASE_ADDRESS + 0x70)
#define	REG_HOST_IF_ACC_TX_IN_LIST_IDX                         (HOST_IF_ACC_BASE_ADDRESS + 0x74)
#define	REG_HOST_IF_ACC_TX_OUT_LIST_IDX                        (HOST_IF_ACC_BASE_ADDRESS + 0x78)
#define	REG_HOST_IF_ACC_RX_IN_LIST_IDX                         (HOST_IF_ACC_BASE_ADDRESS + 0x7C)
#define	REG_HOST_IF_ACC_RX_OUT_LIST_IDX                        (HOST_IF_ACC_BASE_ADDRESS + 0x80)
#define	REG_HOST_IF_ACC_MULTICAST_PD_DISTRIB_CFG               (HOST_IF_ACC_BASE_ADDRESS + 0x84)
#define	REG_HOST_IF_ACC_PD_DISTRIB_UPDATE                      (HOST_IF_ACC_BASE_ADDRESS + 0x88)
#define	REG_HOST_IF_ACC_FREE_PD_INIT                           (HOST_IF_ACC_BASE_ADDRESS + 0x8C)
#define	REG_HOST_IF_ACC_PD_THRESHOLD                           (HOST_IF_ACC_BASE_ADDRESS + 0x90)
#define	REG_HOST_IF_ACC_FREE_PD_COUNT                          (HOST_IF_ACC_BASE_ADDRESS + 0x94)
#define	REG_HOST_IF_ACC_TX_RING_CFG                            (HOST_IF_ACC_BASE_ADDRESS + 0x98)
#define	REG_HOST_IF_ACC_RX_RING_CFG                            (HOST_IF_ACC_BASE_ADDRESS + 0x9C)
#define	REG_HOST_IF_ACC_HOST_TYPE                              (HOST_IF_ACC_BASE_ADDRESS + 0xA0)
#define	REG_HOST_IF_ACC_OWN_BIT_VALUE                          (HOST_IF_ACC_BASE_ADDRESS + 0xA4)
#define	REG_HOST_IF_ACC_TX_IN_COUNTERS_UPDATE                  (HOST_IF_ACC_BASE_ADDRESS + 0xA8)
#define	REG_HOST_IF_ACC_TX_OUT_COUNTERS_UPDATE                 (HOST_IF_ACC_BASE_ADDRESS + 0xAC)
#define	REG_HOST_IF_ACC_RX_IN_COUNTERS_UPDATE                  (HOST_IF_ACC_BASE_ADDRESS + 0xB0)
#define	REG_HOST_IF_ACC_RX_OUT_COUNTERS_UPDATE                 (HOST_IF_ACC_BASE_ADDRESS + 0xB4)
#define	REG_HOST_IF_ACC_TX_IN_HOST_IF_STATUS                   (HOST_IF_ACC_BASE_ADDRESS + 0xB8)
#define	REG_HOST_IF_ACC_TX_OUT_HOST_IF_STATUS                  (HOST_IF_ACC_BASE_ADDRESS + 0xBC)
#define	REG_HOST_IF_ACC_RX_IN_HOST_IF_STATUS                   (HOST_IF_ACC_BASE_ADDRESS + 0xC0)
#define	REG_HOST_IF_ACC_RX_OUT_HOST_IF_STATUS                  (HOST_IF_ACC_BASE_ADDRESS + 0xC4)
#define	REG_HOST_IF_ACC_RX_IN_HD_ACCUM_ADD                     (HOST_IF_ACC_BASE_ADDRESS + 0xC8)
#define	REG_HOST_IF_ACC_TX_IN_HD_ACCUM_ADD                     (HOST_IF_ACC_BASE_ADDRESS + 0xCC)
#define	REG_HOST_IF_ACC_RX_OUT_HD_ACCUM_ADD                    (HOST_IF_ACC_BASE_ADDRESS + 0xD0)
#define	REG_HOST_IF_ACC_TX_OUT_HD_ACCUM_ADD                    (HOST_IF_ACC_BASE_ADDRESS + 0xD4)
#define	REG_HOST_IF_ACC_RX_IN_MGMT_ACCUM_ADD                   (HOST_IF_ACC_BASE_ADDRESS + 0xD8)
#define	REG_HOST_IF_ACC_TX_IN_MGMT_ACCUM_ADD                   (HOST_IF_ACC_BASE_ADDRESS + 0xDC)
#define	REG_HOST_IF_ACC_RX_OUT_MGMT_ACCUM_ADD                  (HOST_IF_ACC_BASE_ADDRESS + 0xE0)
#define	REG_HOST_IF_ACC_TX_OUT_MGMT_ACCUM_ADD                  (HOST_IF_ACC_BASE_ADDRESS + 0xE4)
#define	REG_HOST_IF_ACC_HOST_MAC_MAILBOX_ACCUM_ADD             (HOST_IF_ACC_BASE_ADDRESS + 0xE8)
#define	REG_HOST_IF_ACC_MAC_HOST_MAILBOX_ACCUM_ADD             (HOST_IF_ACC_BASE_ADDRESS + 0xEC)
#define	REG_HOST_IF_ACC_RX_IN_HD_ACCUM_SUB                     (HOST_IF_ACC_BASE_ADDRESS + 0xF0)
#define	REG_HOST_IF_ACC_TX_IN_HD_ACCUM_SUB                     (HOST_IF_ACC_BASE_ADDRESS + 0xF4)
#define	REG_HOST_IF_ACC_RX_OUT_HD_ACCUM_SUB                    (HOST_IF_ACC_BASE_ADDRESS + 0xF8)
#define	REG_HOST_IF_ACC_TX_OUT_HD_ACCUM_SUB                    (HOST_IF_ACC_BASE_ADDRESS + 0xFC)
#define	REG_HOST_IF_ACC_RX_IN_MGMT_ACCUM_SUB                   (HOST_IF_ACC_BASE_ADDRESS + 0x100)
#define	REG_HOST_IF_ACC_TX_IN_MGMT_ACCUM_SUB                   (HOST_IF_ACC_BASE_ADDRESS + 0x104)
#define	REG_HOST_IF_ACC_RX_OUT_MGMT_ACCUM_SUB                  (HOST_IF_ACC_BASE_ADDRESS + 0x108)
#define	REG_HOST_IF_ACC_TX_OUT_MGMT_ACCUM_SUB                  (HOST_IF_ACC_BASE_ADDRESS + 0x10C)
#define	REG_HOST_IF_ACC_HOST_MAC_MAILBOX_ACCUM_SUB             (HOST_IF_ACC_BASE_ADDRESS + 0x110)
#define	REG_HOST_IF_ACC_MAC_HOST_MAILBOX_ACCUM_SUB             (HOST_IF_ACC_BASE_ADDRESS + 0x114)
#define	REG_HOST_IF_ACC_RX_IN_HD_ACCUM_COUNT                   (HOST_IF_ACC_BASE_ADDRESS + 0x118)
#define	REG_HOST_IF_ACC_TX_IN_HD_ACCUM_COUNT                   (HOST_IF_ACC_BASE_ADDRESS + 0x11C)
#define	REG_HOST_IF_ACC_RX_OUT_HD_ACCUM_COUNT                  (HOST_IF_ACC_BASE_ADDRESS + 0x120)
#define	REG_HOST_IF_ACC_TX_OUT_HD_ACCUM_COUNT                  (HOST_IF_ACC_BASE_ADDRESS + 0x124)
#define	REG_HOST_IF_ACC_RX_IN_MGMT_ACCUM_COUNT                 (HOST_IF_ACC_BASE_ADDRESS + 0x128)
#define	REG_HOST_IF_ACC_TX_IN_MGMT_ACCUM_COUNT                 (HOST_IF_ACC_BASE_ADDRESS + 0x12C)
#define	REG_HOST_IF_ACC_RX_OUT_MGMT_ACCUM_COUNT                (HOST_IF_ACC_BASE_ADDRESS + 0x130)
#define	REG_HOST_IF_ACC_TX_OUT_MGMT_ACCUM_COUNT                (HOST_IF_ACC_BASE_ADDRESS + 0x134)
#define	REG_HOST_IF_ACC_HOST_MAC_MAILBOX_ACCUM_COUNT           (HOST_IF_ACC_BASE_ADDRESS + 0x138)
#define	REG_HOST_IF_ACC_MAC_HOST_MAILBOX_ACCUM_COUNT           (HOST_IF_ACC_BASE_ADDRESS + 0x13C)
#define	REG_HOST_IF_ACC_SW_COUNTERS_CLR                        (HOST_IF_ACC_BASE_ADDRESS + 0x140)
#define	REG_HOST_IF_ACC_TX_IN_CORE_INT                         (HOST_IF_ACC_BASE_ADDRESS + 0x144)
#define	REG_HOST_IF_ACC_TX_OUT_CORE_INT                        (HOST_IF_ACC_BASE_ADDRESS + 0x148)
#define	REG_HOST_IF_ACC_RX_IN_CORE_INT                         (HOST_IF_ACC_BASE_ADDRESS + 0x14C)
#define	REG_HOST_IF_ACC_RX_OUT_CORE_INT                        (HOST_IF_ACC_BASE_ADDRESS + 0x150)
#define	REG_HOST_IF_ACC_THRESHOLD_CLR                          (HOST_IF_ACC_BASE_ADDRESS + 0x154)
#define	REG_HOST_IF_ACC_HOST_IF_HD_COUNT_INT                   (HOST_IF_ACC_BASE_ADDRESS + 0x158)
#define	REG_HOST_IF_ACC_HOST_IF_ACC_ERR_INT                    (HOST_IF_ACC_BASE_ADDRESS + 0x15C)
#define	REG_HOST_IF_ACC_HOST_IF_ACC_ERR_INT_CLR                (HOST_IF_ACC_BASE_ADDRESS + 0x160)
#define	REG_HOST_IF_ACC_GENRISC_MASK_A                         (HOST_IF_ACC_BASE_ADDRESS + 0x164)
#define	REG_HOST_IF_ACC_GENRISC_MASK_B                         (HOST_IF_ACC_BASE_ADDRESS + 0x168)
#define	REG_HOST_IF_ACC_GENRISC_MASK_C                         (HOST_IF_ACC_BASE_ADDRESS + 0x16C)
#define	REG_HOST_IF_ACC_GENRISC_MASK_D                         (HOST_IF_ACC_BASE_ADDRESS + 0x170)
#define	REG_HOST_IF_ACC_GENRISC_MASK_A_INT_STATUS              (HOST_IF_ACC_BASE_ADDRESS + 0x174)
#define	REG_HOST_IF_ACC_GENRISC_MASK_B_INT_STATUS              (HOST_IF_ACC_BASE_ADDRESS + 0x178)
#define	REG_HOST_IF_ACC_GENRISC_MASK_C_INT_STATUS              (HOST_IF_ACC_BASE_ADDRESS + 0x17C)
#define	REG_HOST_IF_ACC_GENRISC_MASK_D_INT_STATUS              (HOST_IF_ACC_BASE_ADDRESS + 0x180)
#define	REG_HOST_IF_ACC_DEBUG_REG                              (HOST_IF_ACC_BASE_ADDRESS + 0x184)
#define	REG_HOST_IF_ACC_GCLK_BYPASS_EN                         (HOST_IF_ACC_BASE_ADDRESS + 0x188)
#define	REG_HOST_IF_ACC_TTL_SCALE_SET                          (HOST_IF_ACC_BASE_ADDRESS + 0x18C)
#define	REG_HOST_IF_ACC_GP0_DMA_JOBS_COUNT_SUB                 (HOST_IF_ACC_BASE_ADDRESS + 0x190)
#define	REG_HOST_IF_ACC_GP1_DMA_JOBS_COUNT_SUB                 (HOST_IF_ACC_BASE_ADDRESS + 0x194)
#define	REG_HOST_IF_ACC_GP2_DMA_JOBS_COUNT_SUB                 (HOST_IF_ACC_BASE_ADDRESS + 0x198)
#define	REG_HOST_IF_ACC_GP3_DMA_JOBS_COUNT_SUB                 (HOST_IF_ACC_BASE_ADDRESS + 0x19C)
#define	REG_HOST_IF_ACC_GP0_DMA_JOBS_COUNT                     (HOST_IF_ACC_BASE_ADDRESS + 0x200)
#define	REG_HOST_IF_ACC_GP1_DMA_JOBS_COUNT                     (HOST_IF_ACC_BASE_ADDRESS + 0x204)
#define	REG_HOST_IF_ACC_GP2_DMA_JOBS_COUNT                     (HOST_IF_ACC_BASE_ADDRESS + 0x208)
#define	REG_HOST_IF_ACC_GP3_DMA_JOBS_COUNT                     (HOST_IF_ACC_BASE_ADDRESS + 0x20C)
#define	REG_HOST_IF_ACC_HOST_COUNTERS_CFG                      (HOST_IF_ACC_BASE_ADDRESS + 0x210)
#define	REG_HOST_IF_ACC_TX_IN_MULTICAST_CFG                    (HOST_IF_ACC_BASE_ADDRESS + 0x214)
#define	REG_HOST_IF_ACC_RX_OUT_MULTICAST_CFG                   (HOST_IF_ACC_BASE_ADDRESS + 0x218)
#define	REG_HOST_IF_ACC_FREE_RD_INIT                           (HOST_IF_ACC_BASE_ADDRESS + 0x21C)
#define	REG_HOST_IF_ACC_RD_THRESHOLD                           (HOST_IF_ACC_BASE_ADDRESS + 0x220)
#define	REG_HOST_IF_ACC_FREE_RD_SUB                            (HOST_IF_ACC_BASE_ADDRESS + 0x224)
#define	REG_HOST_IF_ACC_FREE_RD_COUNT                          (HOST_IF_ACC_BASE_ADDRESS + 0x228)
#define	REG_HOST_IF_ACC_TX_HD_BUSY_COUNT                       (HOST_IF_ACC_BASE_ADDRESS + 0x22C)
#define	REG_HOST_IF_ACC_RX_HD_BUSY_COUNT                       (HOST_IF_ACC_BASE_ADDRESS + 0x230)
#define	REG_HOST_IF_ACC_UMT_ORDER_CFG                          (HOST_IF_ACC_BASE_ADDRESS + 0x234)
#define	REG_HOST_IF_ACC_HOST_LOGGER_CFG                        (HOST_IF_ACC_BASE_ADDRESS + 0x238)
#define	REG_HOST_IF_ACC_HOST_LOGGER_BUSY                       (HOST_IF_ACC_BASE_ADDRESS + 0x23C)
#define	REG_HOST_IF_ACC_TX_OUT_MAX_HD                          (HOST_IF_ACC_BASE_ADDRESS + 0x240)
#define	REG_HOST_IF_ACC_RX_OUT_MAX_HD                          (HOST_IF_ACC_BASE_ADDRESS + 0x244)
#define	REG_HOST_IF_ACC_TX_OUT_CYCLIC_COUNTER_INIT             (HOST_IF_ACC_BASE_ADDRESS + 0x248)
#define	REG_HOST_IF_ACC_RX_OUT_CYCLIC_COUNTER_INIT             (HOST_IF_ACC_BASE_ADDRESS + 0x24C)
#define	REG_HOST_IF_ACC_RX_IN_HD_ACCUM_ADD_BIG_END             (HOST_IF_ACC_BASE_ADDRESS + 0x250)
#define	REG_HOST_IF_ACC_TX_IN_HD_ACCUM_ADD_BIG_END             (HOST_IF_ACC_BASE_ADDRESS + 0x254)
#define	REG_HOST_IF_ACC_RX_OUT_HD_ACCUM_ADD_BIG_END            (HOST_IF_ACC_BASE_ADDRESS + 0x258)
#define	REG_HOST_IF_ACC_TX_OUT_HD_ACCUM_ADD_BIG_END            (HOST_IF_ACC_BASE_ADDRESS + 0x25C)
#define	REG_HOST_IF_ACC_RX_IN_HD_ACCUM_SUB_BIG_END             (HOST_IF_ACC_BASE_ADDRESS + 0x260)
#define	REG_HOST_IF_ACC_TX_IN_HD_ACCUM_SUB_BIG_END             (HOST_IF_ACC_BASE_ADDRESS + 0x264)
#define	REG_HOST_IF_ACC_RX_OUT_HD_ACCUM_SUB_BIG_END            (HOST_IF_ACC_BASE_ADDRESS + 0x268)
#define	REG_HOST_IF_ACC_TX_OUT_HD_ACCUM_SUB_BIG_END            (HOST_IF_ACC_BASE_ADDRESS + 0x26C)
#define	REG_HOST_IF_ACC_RX_IN_HD_ACCUM_COUNT_BIG_END           (HOST_IF_ACC_BASE_ADDRESS + 0x270)
#define	REG_HOST_IF_ACC_TX_IN_HD_ACCUM_COUNT_BIG_END           (HOST_IF_ACC_BASE_ADDRESS + 0x274)
#define	REG_HOST_IF_ACC_RX_OUT_HD_ACCUM_COUNT_BIG_END          (HOST_IF_ACC_BASE_ADDRESS + 0x278)
#define	REG_HOST_IF_ACC_TX_OUT_HD_ACCUM_COUNT_BIG_END          (HOST_IF_ACC_BASE_ADDRESS + 0x27C)
#define	REG_HOST_IF_ACC_RX_IN_HD_ACCUM_ADD_FREED               (HOST_IF_ACC_BASE_ADDRESS + 0x280)
#define	REG_HOST_IF_ACC_TX_IN_HD_ACCUM_ADD_FREED               (HOST_IF_ACC_BASE_ADDRESS + 0x284)
#define	REG_HOST_IF_ACC_RX_OUT_HD_ACCUM_ADD_READY              (HOST_IF_ACC_BASE_ADDRESS + 0x288)
#define	REG_HOST_IF_ACC_TX_OUT_HD_ACCUM_ADD_READY              (HOST_IF_ACC_BASE_ADDRESS + 0x28C)
#define	REG_HOST_IF_ACC_RX_IN_HD_ACCUM_SUB_FREED               (HOST_IF_ACC_BASE_ADDRESS + 0x290)
#define	REG_HOST_IF_ACC_TX_IN_HD_ACCUM_SUB_FREED               (HOST_IF_ACC_BASE_ADDRESS + 0x294)
#define	REG_HOST_IF_ACC_RX_OUT_HD_ACCUM_SUB_READY              (HOST_IF_ACC_BASE_ADDRESS + 0x298)
#define	REG_HOST_IF_ACC_TX_OUT_HD_ACCUM_SUB_READY              (HOST_IF_ACC_BASE_ADDRESS + 0x29C)
#define	REG_HOST_IF_ACC_RX_IN_HD_ACCUM_COUNT_FREED             (HOST_IF_ACC_BASE_ADDRESS + 0x2A0)
#define	REG_HOST_IF_ACC_TX_IN_HD_ACCUM_COUNT_FREED             (HOST_IF_ACC_BASE_ADDRESS + 0x2A4)
#define	REG_HOST_IF_ACC_RX_OUT_HD_ACCUM_COUNT_READY            (HOST_IF_ACC_BASE_ADDRESS + 0x2A8)
#define	REG_HOST_IF_ACC_TX_OUT_HD_ACCUM_COUNT_READY            (HOST_IF_ACC_BASE_ADDRESS + 0x2AC)
#define	REG_HOST_IF_ACC_RX_IN_HD_ACCUM_ADD_FREED_BIG_END       (HOST_IF_ACC_BASE_ADDRESS + 0x2B0)
#define	REG_HOST_IF_ACC_TX_IN_HD_ACCUM_ADD_FREED_BIG_END       (HOST_IF_ACC_BASE_ADDRESS + 0x2B4)
#define	REG_HOST_IF_ACC_RX_OUT_HD_ACCUM_ADD_READY_BIG_END      (HOST_IF_ACC_BASE_ADDRESS + 0x2B8)
#define	REG_HOST_IF_ACC_TX_OUT_HD_ACCUM_ADD_READY_BIG_END      (HOST_IF_ACC_BASE_ADDRESS + 0x2BC)
#define	REG_HOST_IF_ACC_RX_IN_HD_ACCUM_SUB_FREED_BIG_END       (HOST_IF_ACC_BASE_ADDRESS + 0x2C0)
#define	REG_HOST_IF_ACC_TX_IN_HD_ACCUM_SUB_FREED_BIG_END       (HOST_IF_ACC_BASE_ADDRESS + 0x2C4)
#define	REG_HOST_IF_ACC_RX_OUT_HD_ACCUM_SUB_READY_BIG_END      (HOST_IF_ACC_BASE_ADDRESS + 0x2C8)
#define	REG_HOST_IF_ACC_TX_OUT_HD_ACCUM_SUB_READY_BIG_END      (HOST_IF_ACC_BASE_ADDRESS + 0x2CC)
#define	REG_HOST_IF_ACC_RX_IN_HD_ACCUM_COUNT_FREED_BIG_END     (HOST_IF_ACC_BASE_ADDRESS + 0x2D0)
#define	REG_HOST_IF_ACC_TX_IN_HD_ACCUM_COUNT_FREED_BIG_END     (HOST_IF_ACC_BASE_ADDRESS + 0x2D4)
#define	REG_HOST_IF_ACC_RX_OUT_HD_ACCUM_COUNT_READY_BIG_END    (HOST_IF_ACC_BASE_ADDRESS + 0x2D8)
#define	REG_HOST_IF_ACC_TX_OUT_HD_ACCUM_COUNT_READY_BIG_END    (HOST_IF_ACC_BASE_ADDRESS + 0x2DC)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_HOST_IF_ACC_SW_CLEAR_INSTRUCTION 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txInSwClear:1;	// Clears all counters and SM
		uint32 txOutSwClear:1;	// Clears all counters and SM
		uint32 rxInSwClear:1;	// Clears all counters and SM
		uint32 rxOutSwClear:1;	// Clears all counters and SM
		uint32 reserved0:28;
	} bitFields;
} RegHostIfAccSwClearInstruction_u;

/*REG_HOST_IF_ACC_TX_IN_SW_INSTRUCTION 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txInSwHalt:1;	// Halts all SM at their current state
		uint32 reserved0:31;
	} bitFields;
} RegHostIfAccTxInSwInstruction_u;

/*REG_HOST_IF_ACC_TX_OUT_SW_INSTRUCTION 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txOutSwHalt:1;	// Halts all SM at their current state
		uint32 reserved0:31;
	} bitFields;
} RegHostIfAccTxOutSwInstruction_u;

/*REG_HOST_IF_ACC_RX_IN_SW_INSTRUCTION 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxInSwHalt:1;	// Halts all SM at their current state
		uint32 reserved0:31;
	} bitFields;
} RegHostIfAccRxInSwInstruction_u;

/*REG_HOST_IF_ACC_RX_OUT_SW_INSTRUCTION 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxOutSwHalt:1;	// Halts all SM at their current state
		uint32 reserved0:31;
	} bitFields;
} RegHostIfAccRxOutSwInstruction_u;

/*REG_HOST_IF_ACC_TX_IN_CONV_CFG 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txInHdLineNum:3;	// Number of line in TX HD
		uint32 reserved0:1;
		uint32 txInPdLineNum:3;	// Number of line in TX PD
		uint32 reserved1:1;
		uint32 txInTxqIdLine:2;	// The 64 bit line the txq_id field will be written too
		uint32 txInTxqIdOffset:6;	// The offset of the field in the line
		uint32 reserved2:8;
		uint32 txInDescAllZeros:1;	// When asserted Fill entire undefined Descriptor lines with desc_init_line value 
		uint32 txInSetNullPop:1;	// Inidcate whether the pop requests sets null in PD or not
		uint32 txInSetNullPush:1;	// Inidcate whether the push requests sets null in PD or not
		uint32 txInMaskConversionEn:1;
		uint32 txInSwap:1;
		uint32 reserved3:3;
	} bitFields;
} RegHostIfAccTxInConvCfg_u;

/*REG_HOST_IF_ACC_TX_OUT_CONV_CFG 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txOutHdLineNum:3;	// Number of line in TX HD
		uint32 reserved0:1;
		uint32 txOutPdLineNum:3;	// Number of line in TX PD
		uint32 reserved1:1;
		uint32 txOutTxqIdLine:2;	// line number of the TXQ_ID field in the PD. Used for Statistic counters.
		uint32 reserved2:6;
		uint32 txOutTxqIdOffset:6;	// Offset within line of the TXQ_ID field in the PD. Used for Statistic counters.
		uint32 reserved3:2;
		uint32 txOutDescAllZeros:1;	// When asserted Fill entire undefined Descriptor lines with desc_init_line value 
		uint32 txOutSetNullPop:1;	// Inidcate whether the pop requests sets null in PD or not
		uint32 txOutSetNullPush:1;	// Inidcate whether the push requests sets null in PD or not
		uint32 txOutMaskConversionEn:1;
		uint32 txOutSwap:1;
		uint32 reserved4:3;
	} bitFields;
} RegHostIfAccTxOutConvCfg_u;

/*REG_HOST_IF_ACC_RX_IN_CONV_CFG 0x1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxInHdLineNum:3;	// Number of line in TX HD
		uint32 reserved0:1;
		uint32 rxInPdLineNum:3;	// Number of line in TX PD
		uint32 reserved1:17;
		uint32 rxInDescAllZeros:1;	// When asserted Fill entire undefined Descriptor lines with desc_init_line value 
		uint32 rxInSetNullPop:1;	// Inidcate whether the pop requests sets null in PD or not
		uint32 rxInSetNullPush:1;	// Inidcate whether the push requests sets null in PD or not
		uint32 rxInMaskConversionEn:1;
		uint32 rxInSwap:1;
		uint32 reserved2:3;
	} bitFields;
} RegHostIfAccRxInConvCfg_u;

/*REG_HOST_IF_ACC_RX_OUT_CONV_CFG 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxOutHdLineNum:3;	// Number of line in TX HD
		uint32 reserved0:1;
		uint32 rxOutPdLineNum:3;	// Number of line in TX PD
		uint32 reserved1:1;
		uint32 rxOutTxqIdLine:2;	// line number of the TXQ_ID field in the RD. Used for Statistic counters.
		uint32 rxOutTxqIdOffset:6;	// Offset within line of the TXQ_ID field in the RD. Used for Statistic counters.
		uint32 reserved2:8;
		uint32 rxOutDescAllZeros:1;	// When asserted Fill entire undefined Descriptor lines with desc_init_line value 
		uint32 rxOutSetNullPop:1;	// Inidcate whether the pop requests sets null in PD or not
		uint32 rxOutSetNullPush:1;	// Inidcate whether the push requests sets null in PD or not
		uint32 rxOutMaskConversionEn:1;
		uint32 rxOutSwap:1;
		uint32 reserved3:3;
	} bitFields;
} RegHostIfAccRxOutConvCfg_u;

/*REG_HOST_IF_ACC_TX_IN_HD_BUF0_BASE_ADDR 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txInHdBuf0BaseAddr:22;	// Base address of buffer in SHRAM. Word aligned
		uint32 reserved0:10;
	} bitFields;
} RegHostIfAccTxInHdBuf0BaseAddr_u;

/*REG_HOST_IF_ACC_TX_IN_HD_BUF1_BASE_ADDR 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txInHdBuf1BaseAddr:22;	// Base address of buffer in SHRAM. Word aligned
		uint32 reserved0:10;
	} bitFields;
} RegHostIfAccTxInHdBuf1BaseAddr_u;

/*REG_HOST_IF_ACC_TX_OUT_HD_BUF0_BASE_ADDR 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txOutHdBuf0BaseAddr:22;	// Base address of buffer in SHRAM. Word aligned
		uint32 reserved0:10;
	} bitFields;
} RegHostIfAccTxOutHdBuf0BaseAddr_u;

/*REG_HOST_IF_ACC_TX_OUT_HD_BUF1_BASE_ADDR 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txOutHdBuf1BaseAddr:22;	// Base address of buffer in SHRAM. Word aligned
		uint32 reserved0:10;
	} bitFields;
} RegHostIfAccTxOutHdBuf1BaseAddr_u;

/*REG_HOST_IF_ACC_RX_IN_HD_BUF0_BASE_ADDR 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxInHdBuf0BaseAddr:22;	// Base address of buffer in SHRAM. Word aligned
		uint32 reserved0:10;
	} bitFields;
} RegHostIfAccRxInHdBuf0BaseAddr_u;

/*REG_HOST_IF_ACC_RX_IN_HD_BUF1_BASE_ADDR 0x38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxInHdBuf1BaseAddr:22;	// Base address of buffer in SHRAM. Word aligned
		uint32 reserved0:10;
	} bitFields;
} RegHostIfAccRxInHdBuf1BaseAddr_u;

/*REG_HOST_IF_ACC_RX_OUT_HD_BUF0_BASE_ADDR 0x3C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxOutHdBuf0BaseAddr:22;	// Base address of buffer in SHRAM. Word aligned
		uint32 reserved0:10;
	} bitFields;
} RegHostIfAccRxOutHdBuf0BaseAddr_u;

/*REG_HOST_IF_ACC_RX_OUT_HD_BUF1_BASE_ADDR 0x40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxOutHdBuf1BaseAddr:22;	// Base address of buffer in SHRAM. Word aligned
		uint32 reserved0:10;
	} bitFields;
} RegHostIfAccRxOutHdBuf1BaseAddr_u;

/*REG_HOST_IF_ACC_TX_IN_COUNTERS 0x44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txInBufferPointer:1;	// Pointer to the next valid buffer to be read
		uint32 reserved0:7;
		uint32 txInPendingJobsCounter:2;	// Pending job counter
		uint32 reserved1:6;
		uint32 txInAvailableBuffersCounter:2;	// Free buffers for DMA HD Write
		uint32 reserved2:6;
		uint32 txInShadowPendingJobsCounter:2;	// Shadow Pending job counter
		uint32 reserved3:6;
	} bitFields;
} RegHostIfAccTxInCounters_u;

/*REG_HOST_IF_ACC_TX_OUT_COUNTERS 0x48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txOutBufferPointer:1;	// Pointer to the next valid buffer to be read
		uint32 reserved0:7;
		uint32 txOutPendingJobsCounter:2;	// Pending job counter
		uint32 reserved1:6;
		uint32 txOutAvailableBuffersCounter:2;	// Free buffers for DMA HD Write
		uint32 reserved2:6;
		uint32 txOutShadowAvailableBuffersCounter:2;	// Shadow Free buffers for DMA HD Write
		uint32 reserved3:6;
	} bitFields;
} RegHostIfAccTxOutCounters_u;

/*REG_HOST_IF_ACC_RX_IN_COUNTERS 0x4C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxInBufferPointer:1;	// Pointer to the next valid buffer to be read
		uint32 reserved0:7;
		uint32 rxInPendingJobsCounter:2;	// Pending job counter
		uint32 reserved1:6;
		uint32 rxInAvailableBuffersCounter:2;	// Free buffers for DMA HD Write
		uint32 reserved2:6;
		uint32 rxInShadowPendingJobsCounter:2;	// Shadow Pending job counter
		uint32 reserved3:6;
	} bitFields;
} RegHostIfAccRxInCounters_u;

/*REG_HOST_IF_ACC_RX_OUT_COUNTERS 0x50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxOutBufferPointer:1;	// Pointer to the next valid buffer to be read
		uint32 reserved0:7;
		uint32 rxOutPendingJobsCounter:2;	// Pending job counter
		uint32 reserved1:6;
		uint32 rxOutAvailableBuffersCounter:2;	// Free buffers for DMA HD Write
		uint32 reserved2:6;
		uint32 rxOutShadowAvailableBuffersCounter:2;	// Shadow Free buffers for DMA HD Write
		uint32 reserved3:6;
	} bitFields;
} RegHostIfAccRxOutCounters_u;

/*REG_HOST_IF_ACC_TX_IN_HD_COUNT_BUF0 0x54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txInHdCount0:8;	// Number of HDs pending in Buffer 
		uint32 reserved0:24;
	} bitFields;
} RegHostIfAccTxInHdCountBuf0_u;

/*REG_HOST_IF_ACC_TX_IN_HD_COUNT_BUF1 0x58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txInHdCount1:8;	// Number of HDs pending in Buffer 
		uint32 reserved0:24;
	} bitFields;
} RegHostIfAccTxInHdCountBuf1_u;

/*REG_HOST_IF_ACC_TX_OUT_HD_COUNT_BUF0 0x5C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txOutHdCount0:8;	// Number of HDs pending in Buffer 
		uint32 reserved0:24;
	} bitFields;
} RegHostIfAccTxOutHdCountBuf0_u;

/*REG_HOST_IF_ACC_TX_OUT_HD_COUNT_BUF1 0x60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txOutHdCount1:8;	// Number of HDs pending in Buffer 
		uint32 reserved0:24;
	} bitFields;
} RegHostIfAccTxOutHdCountBuf1_u;

/*REG_HOST_IF_ACC_RX_IN_HD_COUNT_BUF0 0x64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxInHdCount0:8;	// Number of HDs pending in Buffer 
		uint32 reserved0:24;
	} bitFields;
} RegHostIfAccRxInHdCountBuf0_u;

/*REG_HOST_IF_ACC_RX_IN_HD_COUNT_BUF1 0x68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxInHdCount1:8;	// Number of HDs pending in Buffer 
		uint32 reserved0:24;
	} bitFields;
} RegHostIfAccRxInHdCountBuf1_u;

/*REG_HOST_IF_ACC_RX_OUT_HD_COUNT_BUF0 0x6C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxOutHdCount0:8;	// Number of HDs pending in Buffer 
		uint32 reserved0:24;
	} bitFields;
} RegHostIfAccRxOutHdCountBuf0_u;

/*REG_HOST_IF_ACC_RX_OUT_HD_COUNT_BUF1 0x70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxOutHdCount1:8;	// Number of HDs pending in Buffer 
		uint32 reserved0:24;
	} bitFields;
} RegHostIfAccRxOutHdCountBuf1_u;

/*REG_HOST_IF_ACC_TX_IN_LIST_IDX 0x74 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txInPopListIdx:5;	// Index number of pop list at the DLM
		uint32 reserved0:3;
		uint32 umacUcPushListIdx:5;	// Index number of push list at the DLM
		uint32 reserved1:3;
		uint32 umacMcPushListIdx:5;	// Index number of push list at the DLM
		uint32 reserved2:11;
	} bitFields;
} RegHostIfAccTxInListIdx_u;

/*REG_HOST_IF_ACC_TX_OUT_LIST_IDX 0x78 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txOutPopListIdx:5;	// Index number of pop list at the DLM
		uint32 reserved0:3;
		uint32 txOutPushListIdx:5;	// Index number of push list at the DLM
		uint32 reserved1:19;
	} bitFields;
} RegHostIfAccTxOutListIdx_u;

/*REG_HOST_IF_ACC_RX_IN_LIST_IDX 0x7C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxInPopListIdx:5;	// Index number of pop list at the DLM
		uint32 reserved0:3;
		uint32 rxInPushListIdx:5;	// Index number of push list at the DLM
		uint32 reserved1:19;
	} bitFields;
} RegHostIfAccRxInListIdx_u;

/*REG_HOST_IF_ACC_RX_OUT_LIST_IDX 0x80 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxOutPopListIdx:5;	// Index number of pop list at the DLM
		uint32 reserved0:3;
		uint32 rxOutPushListIdx:5;	// Index number of push list at the DLM
		uint32 reserved1:19;
	} bitFields;
} RegHostIfAccRxOutListIdx_u;

/*REG_HOST_IF_ACC_MULTICAST_PD_DISTRIB_CFG 0x84 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 groupVapPrio:2;	// Defines VAP Group priority
		uint32 reserved0:14;
		uint32 mcBitLine:2;	// Multicast bit line at TX PD
		uint32 mcBitOffset:6;	// Multicast bit offset in line at TX PD
		uint32 groupIdxLine:2;	// Group index field line at TX PD
		uint32 groupIdxOffset:6;	// Group index field offset in line at TX PD
	} bitFields;
} RegHostIfAccMulticastPdDistribCfg_u;

/*REG_HOST_IF_ACC_PD_DISTRIB_UPDATE 0x88 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pdDistAddr:10;	// Access address
		uint32 reserved0:2;
		uint32 pdDistAllTids:1;
		uint32 reserved1:3;
		uint32 pdDistType:2;	// Update type
		uint32 reserved2:6;
		uint32 pdDistBitValue:8;	// Update value
	} bitFields;
} RegHostIfAccPdDistribUpdate_u;

/*REG_HOST_IF_ACC_FREE_PD_INIT 0x8C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 freePdInit:13;	// Initial Free PD number
		uint32 reserved0:19;
	} bitFields;
} RegHostIfAccFreePdInit_u;

/*REG_HOST_IF_ACC_PD_THRESHOLD 0x90 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pdThres:13;	// PD threshold limit
		uint32 reserved0:19;
	} bitFields;
} RegHostIfAccPdThreshold_u;

/*REG_HOST_IF_ACC_FREE_PD_COUNT 0x94 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 freePdCount:13;	// Free PD count
		uint32 reserved0:19;
	} bitFields;
} RegHostIfAccFreePdCount_u;

/*REG_HOST_IF_ACC_TX_RING_CFG 0x98 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txOutRingSize:16;	// TX OUT Ring size
		uint32 txInInOrder:1;	// TX IN is working in order with Host Ring
		uint32 txOutInOrder:1;	// TX OUT is working in order with Host Ring
		uint32 reserved0:2;
		uint32 txSameBuffer:1;	// TX Host Ring uses same buffer
		uint32 reserved1:3;
		uint32 txInOwnBitCheckEn:1;	// TX IN HD own bit check enable
		uint32 txOutOwnBitCheckEn:1;	// TX OUT HD own bit check enable
		uint32 reserved2:2;
		uint32 txInRingWraparound:1;	// TX IN Host Ring supports wrap around
		uint32 txOutRingWraparound:1;	// TX OUT Host Ring supports wrap around
		uint32 reserved3:2;
	} bitFields;
} RegHostIfAccTxRingCfg_u;

/*REG_HOST_IF_ACC_RX_RING_CFG 0x9C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxOutRingSize:16;	// RX OUT Ring size
		uint32 rxInInOrder:1;	// RX IN is working in order with Host Ring
		uint32 rxOutInOrder:1;	// RX OUT is working in order with Host Ring
		uint32 reserved0:2;
		uint32 rxSameBuffer:1;	// RX Host Ring uses same buffer
		uint32 reserved1:3;
		uint32 rxInOwnBitCheckEn:1;	// RX IN HD own bit check enable
		uint32 rxOutOwnBitCheckEn:1;	// RX OUT HD own bit check enable
		uint32 reserved2:2;
		uint32 rxInRingWraparound:1;	// RX IN Host Ring supports wrap around
		uint32 rxOutRingWraparound:1;	// RX OUT Host Ring supports wrap around
		uint32 reserved3:2;
	} bitFields;
} RegHostIfAccRxRingCfg_u;

/*REG_HOST_IF_ACC_HOST_TYPE 0xA0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txInGswip:1;	// Host type indication
		uint32 txOutGswip:1;	// Host type indication
		uint32 rxInGswip:1;	// Host type indication
		uint32 rxOutGswip:1;	// Host type indication
		uint32 reserved0:28;
	} bitFields;
} RegHostIfAccHostType_u;

/*REG_HOST_IF_ACC_OWN_BIT_VALUE 0xA4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ownBitValue:1;	// OWN bit value polarity
		uint32 reserved0:15;
		uint32 ownBitLine:3;	// Own bit line at TX HD
		uint32 ownBitOffset:5;	// Own bit offset in line at TX HD
		uint32 reserved1:8;
	} bitFields;
} RegHostIfAccOwnBitValue_u;

/*REG_HOST_IF_ACC_TX_IN_COUNTERS_UPDATE 0xA8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txInPendingPush:1;	// Add 1 to pending_jobs counter
		uint32 txInPendingPop:1;	// Remove 1 from pending_jobs counter
		uint32 txInAvailablePush:1;	// Add 1 to available_buffers counter
		uint32 txInAvailablePop:1;	// Remove 1 from available_buffers counter
		uint32 txInShadowPendingPush:1;	// Add 1 to shadow pending jobs counter
		uint32 txInShadowPendingPop:1;	// Remove 1 from shadow pending_jobs counter
		uint32 reserved0:26;
	} bitFields;
} RegHostIfAccTxInCountersUpdate_u;

/*REG_HOST_IF_ACC_TX_OUT_COUNTERS_UPDATE 0xAC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txOutPendingPush:1;	// Add 1 to pending_jobs counter
		uint32 txOutPendingPop:1;	// Remove 1 from pending_jobs counter
		uint32 txOutAvailablePush:1;	// Add 1 to available_buffers counter
		uint32 txOutAvailablePop:1;	// Remove 1 from available_buffers counter
		uint32 txOutShadowAvailablePush:1;	// Add 1 to shadow available_buffers counter
		uint32 txOutShadowAvailablePop:1;	// Remove 1 from shadow available_buffers counter
		uint32 reserved0:26;
	} bitFields;
} RegHostIfAccTxOutCountersUpdate_u;

/*REG_HOST_IF_ACC_RX_IN_COUNTERS_UPDATE 0xB0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxInPendingPush:1;	// Add 1 to pending_jobs counter
		uint32 rxInPendingPop:1;	// Remove 1 from pending_jobs counter
		uint32 rxInAvailablePush:1;	// Add 1 to available_buffers counter
		uint32 rxInAvailablePop:1;	// Remove 1 from available_buffers counter
		uint32 rxInShadowPendingPush:1;	// Add 1 to shadow pending_jobs counter
		uint32 rxInShadowPendingPop:1;	// Remove 1 from shadow pending_jobs counter
		uint32 reserved0:26;
	} bitFields;
} RegHostIfAccRxInCountersUpdate_u;

/*REG_HOST_IF_ACC_RX_OUT_COUNTERS_UPDATE 0xB4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxOutPendingPush:1;	// Add 1 to pending_jobs counter
		uint32 rxOutPendingPop:1;	// Remove 1 from pending_jobs counter
		uint32 rxOutAvailablePush:1;	// Add 1 to available_buffers counter
		uint32 rxOutAvailablePop:1;	// Remove 1 from available_buffers counter
		uint32 rxOutShadowAvailablePush:1;	// Add 1 to shadow available_buffers counter
		uint32 rxOutShadowAvailablePop:1;	// Remove 1 from shadow available_buffers counter
		uint32 reserved0:26;
	} bitFields;
} RegHostIfAccRxOutCountersUpdate_u;

/*REG_HOST_IF_ACC_TX_IN_HOST_IF_STATUS 0xB8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txInQmPopState:1;	// Queue Manger POP list State machine status
		uint32 reserved0:7;
		uint32 txInQmPushState:1;	// Queue Manger PUSH list State machine status
		uint32 reserved1:7;
		uint32 txInDecSmState:2;	// Code decoding State machine status
		uint32 reserved2:6;
		uint32 txInConverterState:3;	// Conversion State machine status
		uint32 reserved3:5;
	} bitFields;
} RegHostIfAccTxInHostIfStatus_u;

/*REG_HOST_IF_ACC_TX_OUT_HOST_IF_STATUS 0xBC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txOutQmPopState:1;	// Queue Manger POP list State machine status
		uint32 reserved0:7;
		uint32 txOutQmPushState:1;	// Queue Manger PUSH list State machine status
		uint32 reserved1:7;
		uint32 txOutDecSmState:2;	// Code decoding State machine status
		uint32 reserved2:6;
		uint32 txOutConverterState:3;	// Conversion State machine status
		uint32 reserved3:5;
	} bitFields;
} RegHostIfAccTxOutHostIfStatus_u;

/*REG_HOST_IF_ACC_RX_IN_HOST_IF_STATUS 0xC0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxInQmPopState:1;	// Queue Manger POP list State machine status
		uint32 reserved0:7;
		uint32 rxInQmPushState:1;	// Queue Manger PUSH list State machine status
		uint32 reserved1:7;
		uint32 rxInDecSmState:2;	// Code decoding State machine status
		uint32 reserved2:6;
		uint32 rxInConverterState:3;	// Conversion State machine status
		uint32 reserved3:5;
	} bitFields;
} RegHostIfAccRxInHostIfStatus_u;

/*REG_HOST_IF_ACC_RX_OUT_HOST_IF_STATUS 0xC4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxOutQmPopState:1;	// Queue Manger POP list State machine status
		uint32 reserved0:7;
		uint32 rxOutQmPushState:1;	// Queue Manger PUSH list State machine status
		uint32 reserved1:7;
		uint32 rxOutDecSmState:2;	// Code decoding State machine status
		uint32 reserved2:6;
		uint32 rxOutConverterState:3;	// Conversion State machine status
		uint32 reserved3:5;
	} bitFields;
} RegHostIfAccRxOutHostIfStatus_u;

/*REG_HOST_IF_ACC_RX_IN_HD_ACCUM_ADD 0xC8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxInHdCountAddNum:16;	// Number of Ready HDs in the Host to be added
		uint32 reserved0:16;
	} bitFields;
} RegHostIfAccRxInHdAccumAdd_u;

/*REG_HOST_IF_ACC_TX_IN_HD_ACCUM_ADD 0xCC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txInHdCountAddNum:16;	// Number of Ready HDs in the Host to be added
		uint32 reserved0:16;
	} bitFields;
} RegHostIfAccTxInHdAccumAdd_u;

/*REG_HOST_IF_ACC_RX_OUT_HD_ACCUM_ADD 0xD0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxOutHdCountAddNum:16;	// Number of Ready HDs in the Host to be added
		uint32 reserved0:16;
	} bitFields;
} RegHostIfAccRxOutHdAccumAdd_u;

/*REG_HOST_IF_ACC_TX_OUT_HD_ACCUM_ADD 0xD4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txOutHdCountAddNum:16;	// Number of Ready HDs in the Host to be added
		uint32 reserved0:16;
	} bitFields;
} RegHostIfAccTxOutHdAccumAdd_u;

/*REG_HOST_IF_ACC_RX_IN_MGMT_ACCUM_ADD 0xD8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxInMgmtCountAddNum:16;	// Number of Ready HDs in the Host to be added
		uint32 reserved0:16;
	} bitFields;
} RegHostIfAccRxInMgmtAccumAdd_u;

/*REG_HOST_IF_ACC_TX_IN_MGMT_ACCUM_ADD 0xDC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txInMgmtCountAddNum:16;	// Number of Ready HDs in the Host to be added
		uint32 reserved0:16;
	} bitFields;
} RegHostIfAccTxInMgmtAccumAdd_u;

/*REG_HOST_IF_ACC_RX_OUT_MGMT_ACCUM_ADD 0xE0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxOutMgmtCountAddNum:16;	// Number of Ready HDs in the Host to be added
		uint32 reserved0:16;
	} bitFields;
} RegHostIfAccRxOutMgmtAccumAdd_u;

/*REG_HOST_IF_ACC_TX_OUT_MGMT_ACCUM_ADD 0xE4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txOutMgmtCountAddNum:16;	// Number of Ready HDs in the Host to be added
		uint32 reserved0:16;
	} bitFields;
} RegHostIfAccTxOutMgmtAccumAdd_u;

/*REG_HOST_IF_ACC_HOST_MAC_MAILBOX_ACCUM_ADD 0xE8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hostMacMailboxCountAddNum:16;	// Number of Ready HDs in the Host to be added
		uint32 reserved0:16;
	} bitFields;
} RegHostIfAccHostMacMailboxAccumAdd_u;

/*REG_HOST_IF_ACC_MAC_HOST_MAILBOX_ACCUM_ADD 0xEC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 macHostMailboxCountAddNum:16;	// Number of Ready HDs in the Host to be added
		uint32 reserved0:16;
	} bitFields;
} RegHostIfAccMacHostMailboxAccumAdd_u;

/*REG_HOST_IF_ACC_RX_IN_HD_ACCUM_SUB 0xF0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxInHdCountSubNum:16;	// Number of Ready HDs in the Host to be removed
		uint32 reserved0:16;
	} bitFields;
} RegHostIfAccRxInHdAccumSub_u;

/*REG_HOST_IF_ACC_TX_IN_HD_ACCUM_SUB 0xF4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txInHdCountSubNum:16;	// Number of Ready HDs in the Host to be removed
		uint32 reserved0:16;
	} bitFields;
} RegHostIfAccTxInHdAccumSub_u;

/*REG_HOST_IF_ACC_RX_OUT_HD_ACCUM_SUB 0xF8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxOutHdCountSubNum:16;	// Number of Ready HDs in the Host to be removed
		uint32 reserved0:16;
	} bitFields;
} RegHostIfAccRxOutHdAccumSub_u;

/*REG_HOST_IF_ACC_TX_OUT_HD_ACCUM_SUB 0xFC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txOutHdCountSubNum:16;	// Number of Ready HDs in the Host to be removed
		uint32 reserved0:16;
	} bitFields;
} RegHostIfAccTxOutHdAccumSub_u;

/*REG_HOST_IF_ACC_RX_IN_MGMT_ACCUM_SUB 0x100 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxInMgmtCountSubNum:16;	// Number of Ready HDs in the Host to be removed
		uint32 reserved0:16;
	} bitFields;
} RegHostIfAccRxInMgmtAccumSub_u;

/*REG_HOST_IF_ACC_TX_IN_MGMT_ACCUM_SUB 0x104 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txInMgmtCountSubNum:16;	// Number of Ready HDs in the Host to be removed
		uint32 reserved0:16;
	} bitFields;
} RegHostIfAccTxInMgmtAccumSub_u;

/*REG_HOST_IF_ACC_RX_OUT_MGMT_ACCUM_SUB 0x108 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxOutMgmtCountSubNum:16;	// Number of Ready HDs in the Host to be removed
		uint32 reserved0:16;
	} bitFields;
} RegHostIfAccRxOutMgmtAccumSub_u;

/*REG_HOST_IF_ACC_TX_OUT_MGMT_ACCUM_SUB 0x10C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txOutMgmtCountSubNum:16;	// Number of Ready HDs in the Host to be removed
		uint32 reserved0:16;
	} bitFields;
} RegHostIfAccTxOutMgmtAccumSub_u;

/*REG_HOST_IF_ACC_HOST_MAC_MAILBOX_ACCUM_SUB 0x110 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hostMacMailboxCountSubNum:16;	// Number of Ready HDs in the Host to be removed
		uint32 reserved0:16;
	} bitFields;
} RegHostIfAccHostMacMailboxAccumSub_u;

/*REG_HOST_IF_ACC_MAC_HOST_MAILBOX_ACCUM_SUB 0x114 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 macHostMailboxCountSubNum:16;	// Number of Ready HDs in the Host to be removed
		uint32 reserved0:16;
	} bitFields;
} RegHostIfAccMacHostMailboxAccumSub_u;

/*REG_HOST_IF_ACC_RX_IN_HD_ACCUM_COUNT 0x118 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxInHdCount:16;	// Number of Ready HDs in the Host
		uint32 reserved0:16;
	} bitFields;
} RegHostIfAccRxInHdAccumCount_u;

/*REG_HOST_IF_ACC_TX_IN_HD_ACCUM_COUNT 0x11C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txInHdCount:16;	// Number of Ready HDs in the Host
		uint32 reserved0:16;
	} bitFields;
} RegHostIfAccTxInHdAccumCount_u;

/*REG_HOST_IF_ACC_RX_OUT_HD_ACCUM_COUNT 0x120 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxOutHdCount:16;	// Number of Ready HDs in the Host
		uint32 reserved0:16;
	} bitFields;
} RegHostIfAccRxOutHdAccumCount_u;

/*REG_HOST_IF_ACC_TX_OUT_HD_ACCUM_COUNT 0x124 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txOutHdCount:16;	// Number of Ready HDs in the Host
		uint32 reserved0:16;
	} bitFields;
} RegHostIfAccTxOutHdAccumCount_u;

/*REG_HOST_IF_ACC_RX_IN_MGMT_ACCUM_COUNT 0x128 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxInMgmtCount:16;	// Number of Ready HDs in the Host
		uint32 reserved0:16;
	} bitFields;
} RegHostIfAccRxInMgmtAccumCount_u;

/*REG_HOST_IF_ACC_TX_IN_MGMT_ACCUM_COUNT 0x12C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txInMgmtCount:16;	// Number of Ready HDs in the Host
		uint32 reserved0:16;
	} bitFields;
} RegHostIfAccTxInMgmtAccumCount_u;

/*REG_HOST_IF_ACC_RX_OUT_MGMT_ACCUM_COUNT 0x130 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxOutMgmtCount:16;	// Number of Ready HDs in the Host
		uint32 reserved0:16;
	} bitFields;
} RegHostIfAccRxOutMgmtAccumCount_u;

/*REG_HOST_IF_ACC_TX_OUT_MGMT_ACCUM_COUNT 0x134 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txOutMgmtCount:16;	// Number of Ready HDs in the Host
		uint32 reserved0:16;
	} bitFields;
} RegHostIfAccTxOutMgmtAccumCount_u;

/*REG_HOST_IF_ACC_HOST_MAC_MAILBOX_ACCUM_COUNT 0x138 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hostMacMailboxCount:16;	// Number of Ready HDs in the Host
		uint32 reserved0:16;
	} bitFields;
} RegHostIfAccHostMacMailboxAccumCount_u;

/*REG_HOST_IF_ACC_MAC_HOST_MAILBOX_ACCUM_COUNT 0x13C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 macHostMailboxCount:16;	// Number of Ready HDs in the Host
		uint32 reserved0:16;
	} bitFields;
} RegHostIfAccMacHostMailboxAccumCount_u;

/*REG_HOST_IF_ACC_SW_COUNTERS_CLR 0x140 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxOutMgmtCountClr:1;	// Clears all counters and SM
		uint32 txInMgmtCountClr:1;
		uint32 rxInMgmtCountClr:1;	// Clears all counters and SM
		uint32 txOutMgmtCountClr:1;	// Clears all counters and SM
		uint32 hostMacCountClr:1;	// Clears all counters and SM
		uint32 macHostCountClr:1;
		uint32 gp0DmaJobsCounterClr:1;
		uint32 gp1DmaJobsCounterClr:1;
		uint32 gp2DmaJobsCounterClr:1;
		uint32 gp3DmaJobsCounterClr:1;
		uint32 reserved0:22;
	} bitFields;
} RegHostIfAccSwCountersClr_u;

/*REG_HOST_IF_ACC_TX_IN_CORE_INT 0x144 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:8;
		uint32 txInHdBufferFreeInt:1;	// TX in HD Buffer is free Interrupt
		uint32 reserved1:7;
		uint32 txInShadowPendingJobsInt:1;	// There is at least one pending HD Buffer
		uint32 reserved2:7;
		uint32 pdThresReachedInt:1;	// PD threshold reached interrupt
		uint32 reserved3:7;
	} bitFields;
} RegHostIfAccTxInCoreInt_u;

/*REG_HOST_IF_ACC_TX_OUT_CORE_INT 0x148 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txOutPendingJobsInt:1;	// TX out pending jobs Interrupt
		uint32 reserved0:15;
		uint32 txOutShadowHdBufferFreeInt:1;	// There is at least one free HD Buffer
		uint32 reserved1:15;
	} bitFields;
} RegHostIfAccTxOutCoreInt_u;

/*REG_HOST_IF_ACC_RX_IN_CORE_INT 0x14C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:8;
		uint32 rxInHdBufferFreeInt:1;	// rx in HD Buffer is free Interrupt
		uint32 reserved1:7;
		uint32 rxInShadowPendingJobsInt:1;	// There is at least one pending HD Buffer
		uint32 reserved2:15;
	} bitFields;
} RegHostIfAccRxInCoreInt_u;

/*REG_HOST_IF_ACC_RX_OUT_CORE_INT 0x150 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxOutPendingJobsInt:1;	// rx out pending jobs Interrupt
		uint32 reserved0:15;
		uint32 rxOutShadowHdBufferFreeInt:1;	// There is at least one free HD Buffer
		uint32 reserved1:7;
		uint32 rdThresReachedInt:1;
		uint32 reserved2:7;
	} bitFields;
} RegHostIfAccRxOutCoreInt_u;

/*REG_HOST_IF_ACC_THRESHOLD_CLR 0x154 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pdThresReachedIntClr:1;	// PD threshold reached interrupt Clear
		uint32 reserved0:7;
		uint32 rdThresReachedIntClr:1;
		uint32 reserved1:23;
	} bitFields;
} RegHostIfAccThresholdClr_u;

/*REG_HOST_IF_ACC_HOST_IF_HD_COUNT_INT 0x158 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txInHdCountNotEmptyIrq:1;	// Number of ready HDs at the Host is bigger than 0
		uint32 txOutHdCountNotEmptyIrq:1;	// Number of done HDs at the MAC is bigger than 0
		uint32 rxInHdCountNotEmptyIrq:1;	// Number of free HDs at the RX path is bigger than 0
		uint32 rxOutHdCountNotEmptyIrq:1;	// Number of HDs at the RX path is bigger than 0
		uint32 reserved0:28;
	} bitFields;
} RegHostIfAccHostIfHdCountInt_u;

/*REG_HOST_IF_ACC_HOST_IF_ACC_ERR_INT 0x15C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 busyPdCountOverflowInt:1;	// Number of busy PDs overflow interrupt
		uint32 txInOwnBitErrInt:1;	// Own bit error interrupt
		uint32 busyRdCountOverflowInt:1;
		uint32 reserved0:29;
	} bitFields;
} RegHostIfAccHostIfAccErrInt_u;

/*REG_HOST_IF_ACC_HOST_IF_ACC_ERR_INT_CLR 0x160 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 busyPdCountOverflowIntClr:1;	// Number of busy PDs overflow interrupt Clear
		uint32 txInOwnBitErrIntClr:1;	// Own bit error interrupt Clear
		uint32 busyRdCountOverflowIntClr:1;
		uint32 reserved0:29;
	} bitFields;
} RegHostIfAccHostIfAccErrIntClr_u;

/*REG_HOST_IF_ACC_GENRISC_MASK_A 0x164 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 groupAMask:14;	// Mask of GenRISC interrupts into group A
		uint32 reserved0:18;
	} bitFields;
} RegHostIfAccGenriscMaskA_u;

/*REG_HOST_IF_ACC_GENRISC_MASK_B 0x168 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 groupBMask:14;	// Mask of GenRISC interrupts into group B
		uint32 reserved0:18;
	} bitFields;
} RegHostIfAccGenriscMaskB_u;

/*REG_HOST_IF_ACC_GENRISC_MASK_C 0x16C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 groupCMask:14;	// Mask of GenRISC interrupts into group C
		uint32 reserved0:18;
	} bitFields;
} RegHostIfAccGenriscMaskC_u;

/*REG_HOST_IF_ACC_GENRISC_MASK_D 0x170 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 groupDMask:14;	// Mask of GenRISC interrupts into group D
		uint32 reserved0:18;
	} bitFields;
} RegHostIfAccGenriscMaskD_u;

/*REG_HOST_IF_ACC_GENRISC_MASK_A_INT_STATUS 0x174 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 genriscMaskAIntStatus:14;	// Interrupt status of GenRISC group A
		uint32 reserved0:18;
	} bitFields;
} RegHostIfAccGenriscMaskAIntStatus_u;

/*REG_HOST_IF_ACC_GENRISC_MASK_B_INT_STATUS 0x178 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 genriscMaskBIntStatus:14;	// Interrupt status of GenRISC group A
		uint32 reserved0:18;
	} bitFields;
} RegHostIfAccGenriscMaskBIntStatus_u;

/*REG_HOST_IF_ACC_GENRISC_MASK_C_INT_STATUS 0x17C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 genriscMaskCIntStatus:14;	// Interrupt status of GenRISC group A
		uint32 reserved0:18;
	} bitFields;
} RegHostIfAccGenriscMaskCIntStatus_u;

/*REG_HOST_IF_ACC_GENRISC_MASK_D_INT_STATUS 0x180 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 genriscMaskDIntStatus:14;	// Interrupt status of GenRISC group A
		uint32 reserved0:18;
	} bitFields;
} RegHostIfAccGenriscMaskDIntStatus_u;

/*REG_HOST_IF_ACC_DEBUG_REG 0x184 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txInPopListEmpty:1;	// pop list empty signal indication
		uint32 txOutPopListEmpty:1;	// pop list empty signal indication
		uint32 rxInPopListEmpty:1;	// pop list empty signal indication
		uint32 rxOutPopListEmpty:1;	// pop list empty signal indication
		uint32 txInCoreBusy:1;	// core is busy indication
		uint32 txOutCoreBusy:1;	// core is busy indication
		uint32 rxInCoreBusy:1;	// core is busy indication
		uint32 rxOutCoreBusy:1;	// core is busy indication
		uint32 reserved0:24;
	} bitFields;
} RegHostIfAccDebugReg_u;

/*REG_HOST_IF_ACC_GCLK_BYPASS_EN 0x188 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txInCoreGclkBypassEn:1;	// gated clock bypass
		uint32 txOutCoreGclkBypassEn:1;	// gated clock bypass
		uint32 rxInCoreGclkBypassEn:1;	// gated clock bypass
		uint32 rxOutCoreGclkBypassEn:1;	// gated clock bypass
		uint32 regFileGclkBypassEn:1;	// gated clock bypass
		uint32 reserved0:27;
	} bitFields;
} RegHostIfAccGclkBypassEn_u;

/*REG_HOST_IF_ACC_TTL_SCALE_SET 0x18C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ttlScale:3;
		uint32 reserved0:29;
	} bitFields;
} RegHostIfAccTtlScaleSet_u;

/*REG_HOST_IF_ACC_GP0_DMA_JOBS_COUNT_SUB 0x190 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gp0DmaJobsSubNum:8;	// Number of Ready HDs in the Host to be removed
		uint32 reserved0:24;
	} bitFields;
} RegHostIfAccGp0DmaJobsCountSub_u;

/*REG_HOST_IF_ACC_GP1_DMA_JOBS_COUNT_SUB 0x194 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gp1DmaJobsSubNum:8;	// Number of Ready HDs in the Host to be removed
		uint32 reserved0:24;
	} bitFields;
} RegHostIfAccGp1DmaJobsCountSub_u;

/*REG_HOST_IF_ACC_GP2_DMA_JOBS_COUNT_SUB 0x198 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gp2DmaJobsSubNum:8;	// Number of Ready HDs in the Host to be removed
		uint32 reserved0:24;
	} bitFields;
} RegHostIfAccGp2DmaJobsCountSub_u;

/*REG_HOST_IF_ACC_GP3_DMA_JOBS_COUNT_SUB 0x19C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gp3DmaJobsSubNum:8;	// Number of Ready HDs in the Host to be removed
		uint32 reserved0:24;
	} bitFields;
} RegHostIfAccGp3DmaJobsCountSub_u;

/*REG_HOST_IF_ACC_GP0_DMA_JOBS_COUNT 0x200 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gp0DmaJobsCount:8;	// Number of Ready HDs in the Host
		uint32 reserved0:24;
	} bitFields;
} RegHostIfAccGp0DmaJobsCount_u;

/*REG_HOST_IF_ACC_GP1_DMA_JOBS_COUNT 0x204 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gp1DmaJobsCount:8;	// Number of Ready HDs in the Host
		uint32 reserved0:24;
	} bitFields;
} RegHostIfAccGp1DmaJobsCount_u;

/*REG_HOST_IF_ACC_GP2_DMA_JOBS_COUNT 0x208 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gp2DmaJobsCount:8;	// Number of Ready HDs in the Host
		uint32 reserved0:24;
	} bitFields;
} RegHostIfAccGp2DmaJobsCount_u;

/*REG_HOST_IF_ACC_GP3_DMA_JOBS_COUNT 0x20C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gp3DmaJobsCount:8;	// Number of Ready HDs in the Host
		uint32 reserved0:24;
	} bitFields;
} RegHostIfAccGp3DmaJobsCount_u;

/*REG_HOST_IF_ACC_HOST_COUNTERS_CFG 0x210 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 qosTxEn:1;
		uint32 reserved0:1;
		uint32 txInUniHdCountEn:1;
		uint32 txInMultiHdCountEn:1;
		uint32 txInBroadcastHdCountEn:1;
		uint32 txInUniBytesCountEn:1;
		uint32 txInMultiBytesCountEn:1;
		uint32 txInBroadcastBytesCountEn:1;
		uint32 rxOutUniHdCountEn:1;
		uint32 rxOutMultiHdCountEn:1;
		uint32 rxOutBroadcastHdCountEn:1;
		uint32 rxOutUniBytesCountEn:1;
		uint32 rxOutMultiBytesCountEn:1;
		uint32 rxOutBroadcastBytesCountEn:1;
		uint32 rxOutStaBytesCountEn:1;
		uint32 agerCountEn:1;
		uint32 reserved1:14;
		uint32 qosTxDecEn:1;
		uint32 txCountCleanFifo:1;
	} bitFields;
} RegHostIfAccHostCountersCfg_u;

/*REG_HOST_IF_ACC_TX_IN_MULTICAST_CFG 0x214 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txInByteCountLine:2;
		uint32 reserved0:6;
		uint32 txInByteCountOffset:6;
		uint32 reserved1:18;
	} bitFields;
} RegHostIfAccTxInMulticastCfg_u;

/*REG_HOST_IF_ACC_RX_OUT_MULTICAST_CFG 0x218 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxOutByteCountLine:2;
		uint32 reserved0:6;
		uint32 rxOutByteCountOffset:6;
		uint32 reserved1:2;
		uint32 rxOutMcBitLine:2;	// Multicast bit line at TX PD
		uint32 rxOutMcBitOffset:6;	// Multicast bit offset in line at TX PD
		uint32 rxOutBcBitLine:2;	// Group index field line at TX PD
		uint32 rxOutBcBitOffset:6;	// Group index field offset in line at TX PD
	} bitFields;
} RegHostIfAccRxOutMulticastCfg_u;

/*REG_HOST_IF_ACC_FREE_RD_INIT 0x21C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 freeRdInit:13;	// Initial Free PD number
		uint32 reserved0:19;
	} bitFields;
} RegHostIfAccFreeRdInit_u;

/*REG_HOST_IF_ACC_RD_THRESHOLD 0x220 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rdThres:13;	// PD threshold limit
		uint32 reserved0:19;
	} bitFields;
} RegHostIfAccRdThreshold_u;

/*REG_HOST_IF_ACC_FREE_RD_SUB 0x224 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 freeRdSub:13;	// Number of Ready HDs in the Host to be removed
		uint32 reserved0:19;
	} bitFields;
} RegHostIfAccFreeRdSub_u;

/*REG_HOST_IF_ACC_FREE_RD_COUNT 0x228 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 freeRdCount:13;	// Free PD count
		uint32 reserved0:19;
	} bitFields;
} RegHostIfAccFreeRdCount_u;

/*REG_HOST_IF_ACC_TX_HD_BUSY_COUNT 0x22C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txHdBusyCount:13;	// Free PD count
		uint32 reserved0:19;
	} bitFields;
} RegHostIfAccTxHdBusyCount_u;

/*REG_HOST_IF_ACC_RX_HD_BUSY_COUNT 0x230 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxHdBusyCount:13;	// Free PD count
		uint32 reserved0:19;
	} bitFields;
} RegHostIfAccRxHdBusyCount_u;

/*REG_HOST_IF_ACC_UMT_ORDER_CFG 0x234 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxInTxIn:1;	// Initial Free PD number
		uint32 reserved0:31;
	} bitFields;
} RegHostIfAccUmtOrderCfg_u;

/*REG_HOST_IF_ACC_HOST_LOGGER_CFG 0x238 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txInLoggerPriority:2;
		uint32 txInLoggerShortEn:1;
		uint32 txInLoggerLongEn:1;
		uint32 txOutLoggerPriority:2;
		uint32 txOutLoggerShortEn:1;
		uint32 txOutLoggerLongEn:1;
		uint32 rxInLoggerPriority:2;
		uint32 rxInLoggerShortEn:1;
		uint32 rxInLoggerLongEn:1;
		uint32 rxOutLoggerPriority:2;
		uint32 rxOutLoggerShortEn:1;
		uint32 rxOutLoggerLongEn:1;
		uint32 reserved0:16;
	} bitFields;
} RegHostIfAccHostLoggerCfg_u;

/*REG_HOST_IF_ACC_HOST_LOGGER_BUSY 0x23C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txInLoggerBusy:1;
		uint32 reserved0:7;
		uint32 txOutLoggerBusy:1;
		uint32 reserved1:7;
		uint32 rxInLoggerBusy:1;
		uint32 reserved2:7;
		uint32 rxOutLoggerBusy:1;
		uint32 reserved3:7;
	} bitFields;
} RegHostIfAccHostLoggerBusy_u;

/*REG_HOST_IF_ACC_TX_OUT_MAX_HD 0x240 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txOutMaxHdNum:12;
		uint32 reserved0:20;
	} bitFields;
} RegHostIfAccTxOutMaxHd_u;

/*REG_HOST_IF_ACC_RX_OUT_MAX_HD 0x244 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxOutMaxHdNum:12;
		uint32 reserved0:20;
	} bitFields;
} RegHostIfAccRxOutMaxHd_u;

/*REG_HOST_IF_ACC_TX_OUT_CYCLIC_COUNTER_INIT 0x248 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txOutCyclicInitValue:16;
		uint32 reserved0:16;
	} bitFields;
} RegHostIfAccTxOutCyclicCounterInit_u;

/*REG_HOST_IF_ACC_RX_OUT_CYCLIC_COUNTER_INIT 0x24C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxOutCyclicInitValue:16;
		uint32 reserved0:16;
	} bitFields;
} RegHostIfAccRxOutCyclicCounterInit_u;

/*REG_HOST_IF_ACC_RX_IN_HD_ACCUM_ADD_BIG_END 0x250 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:16;
		uint32 rxInHdCountAddNumBigEnd:16;	// Number of Ready HDs in the Host to be added
	} bitFields;
} RegHostIfAccRxInHdAccumAddBigEnd_u;

/*REG_HOST_IF_ACC_TX_IN_HD_ACCUM_ADD_BIG_END 0x254 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:16;
		uint32 txInHdCountAddNumBigEnd:16;	// Number of Ready HDs in the Host to be added
	} bitFields;
} RegHostIfAccTxInHdAccumAddBigEnd_u;

/*REG_HOST_IF_ACC_RX_OUT_HD_ACCUM_ADD_BIG_END 0x258 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:16;
		uint32 rxOutHdCountAddNumBigEnd:16;	// Number of Ready HDs in the Host to be added
	} bitFields;
} RegHostIfAccRxOutHdAccumAddBigEnd_u;

/*REG_HOST_IF_ACC_TX_OUT_HD_ACCUM_ADD_BIG_END 0x25C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:16;
		uint32 txOutHdCountAddNumBigEnd:16;	// Number of Ready HDs in the Host to be added
	} bitFields;
} RegHostIfAccTxOutHdAccumAddBigEnd_u;

/*REG_HOST_IF_ACC_RX_IN_HD_ACCUM_SUB_BIG_END 0x260 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:16;
		uint32 rxInHdCountSubNumBigEnd:16;	// Number of Ready HDs in the Host to be removed
	} bitFields;
} RegHostIfAccRxInHdAccumSubBigEnd_u;

/*REG_HOST_IF_ACC_TX_IN_HD_ACCUM_SUB_BIG_END 0x264 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:16;
		uint32 txInHdCountSubNumBigEnd:16;	// Number of Ready HDs in the Host to be removed
	} bitFields;
} RegHostIfAccTxInHdAccumSubBigEnd_u;

/*REG_HOST_IF_ACC_RX_OUT_HD_ACCUM_SUB_BIG_END 0x268 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:16;
		uint32 rxOutHdCountSubNumBigEnd:16;	// Number of Ready HDs in the Host to be removed
	} bitFields;
} RegHostIfAccRxOutHdAccumSubBigEnd_u;

/*REG_HOST_IF_ACC_TX_OUT_HD_ACCUM_SUB_BIG_END 0x26C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:16;
		uint32 txOutHdCountSubNumBigEnd:16;	// Number of Ready HDs in the Host to be removed
	} bitFields;
} RegHostIfAccTxOutHdAccumSubBigEnd_u;

/*REG_HOST_IF_ACC_RX_IN_HD_ACCUM_COUNT_BIG_END 0x270 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:16;
		uint32 rxInHdCountBigEnd:16;	// Number of Ready HDs in the Host
	} bitFields;
} RegHostIfAccRxInHdAccumCountBigEnd_u;

/*REG_HOST_IF_ACC_TX_IN_HD_ACCUM_COUNT_BIG_END 0x274 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:16;
		uint32 txInHdCountBigEnd:16;	// Number of Ready HDs in the Host
	} bitFields;
} RegHostIfAccTxInHdAccumCountBigEnd_u;

/*REG_HOST_IF_ACC_RX_OUT_HD_ACCUM_COUNT_BIG_END 0x278 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:16;
		uint32 rxOutHdCountBigEnd:16;	// Number of Ready HDs in the Host
	} bitFields;
} RegHostIfAccRxOutHdAccumCountBigEnd_u;

/*REG_HOST_IF_ACC_TX_OUT_HD_ACCUM_COUNT_BIG_END 0x27C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:16;
		uint32 txOutHdCountBigEnd:16;	// Number of Ready HDs in the Host
	} bitFields;
} RegHostIfAccTxOutHdAccumCountBigEnd_u;

/*REG_HOST_IF_ACC_RX_IN_HD_ACCUM_ADD_FREED 0x280 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxInHdCountAddNumFreed:16;	// Number of Ready HDs in the Host to be added
		uint32 reserved0:16;
	} bitFields;
} RegHostIfAccRxInHdAccumAddFreed_u;

/*REG_HOST_IF_ACC_TX_IN_HD_ACCUM_ADD_FREED 0x284 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txInHdCountAddNumFreed:16;	// Number of Ready HDs in the Host to be added
		uint32 reserved0:16;
	} bitFields;
} RegHostIfAccTxInHdAccumAddFreed_u;

/*REG_HOST_IF_ACC_RX_OUT_HD_ACCUM_ADD_READY 0x288 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxOutHdCountAddNumReady:16;	// Number of Ready HDs in the Host to be added
		uint32 reserved0:16;
	} bitFields;
} RegHostIfAccRxOutHdAccumAddReady_u;

/*REG_HOST_IF_ACC_TX_OUT_HD_ACCUM_ADD_READY 0x28C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txOutHdCountAddNumReady:16;	// Number of Ready HDs in the Host to be added
		uint32 reserved0:16;
	} bitFields;
} RegHostIfAccTxOutHdAccumAddReady_u;

/*REG_HOST_IF_ACC_RX_IN_HD_ACCUM_SUB_FREED 0x290 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxInHdCountSubNumFreed:16;	// Number of Ready HDs in the Host to be removed
		uint32 reserved0:16;
	} bitFields;
} RegHostIfAccRxInHdAccumSubFreed_u;

/*REG_HOST_IF_ACC_TX_IN_HD_ACCUM_SUB_FREED 0x294 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txInHdCountSubNumFreed:16;	// Number of Ready HDs in the Host to be removed
		uint32 reserved0:16;
	} bitFields;
} RegHostIfAccTxInHdAccumSubFreed_u;

/*REG_HOST_IF_ACC_RX_OUT_HD_ACCUM_SUB_READY 0x298 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxOutHdCountSubNumReady:16;	// Number of Ready HDs in the Host to be removed
		uint32 reserved0:16;
	} bitFields;
} RegHostIfAccRxOutHdAccumSubReady_u;

/*REG_HOST_IF_ACC_TX_OUT_HD_ACCUM_SUB_READY 0x29C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txOutHdCountSubNumReady:16;	// Number of Ready HDs in the Host to be removed
		uint32 reserved0:16;
	} bitFields;
} RegHostIfAccTxOutHdAccumSubReady_u;

/*REG_HOST_IF_ACC_RX_IN_HD_ACCUM_COUNT_FREED 0x2A0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxInHdCountFreed:16;	// Number of Ready HDs in the Host
		uint32 reserved0:16;
	} bitFields;
} RegHostIfAccRxInHdAccumCountFreed_u;

/*REG_HOST_IF_ACC_TX_IN_HD_ACCUM_COUNT_FREED 0x2A4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txInHdCountFreed:16;	// Number of Ready HDs in the Host
		uint32 reserved0:16;
	} bitFields;
} RegHostIfAccTxInHdAccumCountFreed_u;

/*REG_HOST_IF_ACC_RX_OUT_HD_ACCUM_COUNT_READY 0x2A8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxOutHdCountReady:16;	// Number of Ready HDs in the Host
		uint32 reserved0:16;
	} bitFields;
} RegHostIfAccRxOutHdAccumCountReady_u;

/*REG_HOST_IF_ACC_TX_OUT_HD_ACCUM_COUNT_READY 0x2AC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txOutHdCountReady:16;	// Number of Ready HDs in the Host
		uint32 reserved0:16;
	} bitFields;
} RegHostIfAccTxOutHdAccumCountReady_u;

/*REG_HOST_IF_ACC_RX_IN_HD_ACCUM_ADD_FREED_BIG_END 0x2B0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:16;
		uint32 rxInHdCountAddNumFreedBigEnd:16;	// Number of Ready HDs in the Host to be added
	} bitFields;
} RegHostIfAccRxInHdAccumAddFreedBigEnd_u;

/*REG_HOST_IF_ACC_TX_IN_HD_ACCUM_ADD_FREED_BIG_END 0x2B4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:16;
		uint32 txInHdCountAddNumFreedBigEnd:16;	// Number of Ready HDs in the Host to be added
	} bitFields;
} RegHostIfAccTxInHdAccumAddFreedBigEnd_u;

/*REG_HOST_IF_ACC_RX_OUT_HD_ACCUM_ADD_READY_BIG_END 0x2B8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:16;
		uint32 rxOutHdCountAddNumReadyBigEnd:16;	// Number of Ready HDs in the Host to be added
	} bitFields;
} RegHostIfAccRxOutHdAccumAddReadyBigEnd_u;

/*REG_HOST_IF_ACC_TX_OUT_HD_ACCUM_ADD_READY_BIG_END 0x2BC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:16;
		uint32 txOutHdCountAddNumReadyBigEnd:16;	// Number of Ready HDs in the Host to be added
	} bitFields;
} RegHostIfAccTxOutHdAccumAddReadyBigEnd_u;

/*REG_HOST_IF_ACC_RX_IN_HD_ACCUM_SUB_FREED_BIG_END 0x2C0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:16;
		uint32 rxInHdCountSubNumFreedBigEnd:16;	// Number of Ready HDs in the Host to be removed
	} bitFields;
} RegHostIfAccRxInHdAccumSubFreedBigEnd_u;

/*REG_HOST_IF_ACC_TX_IN_HD_ACCUM_SUB_FREED_BIG_END 0x2C4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:16;
		uint32 txInHdCountSubNumFreedBigEnd:16;	// Number of Ready HDs in the Host to be removed
	} bitFields;
} RegHostIfAccTxInHdAccumSubFreedBigEnd_u;

/*REG_HOST_IF_ACC_RX_OUT_HD_ACCUM_SUB_READY_BIG_END 0x2C8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:16;
		uint32 rxOutHdCountSubNumReadyBigEnd:16;	// Number of Ready HDs in the Host to be removed
	} bitFields;
} RegHostIfAccRxOutHdAccumSubReadyBigEnd_u;

/*REG_HOST_IF_ACC_TX_OUT_HD_ACCUM_SUB_READY_BIG_END 0x2CC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:16;
		uint32 txOutHdCountSubNumReadyBigEnd:16;	// Number of Ready HDs in the Host to be removed
	} bitFields;
} RegHostIfAccTxOutHdAccumSubReadyBigEnd_u;

/*REG_HOST_IF_ACC_RX_IN_HD_ACCUM_COUNT_FREED_BIG_END 0x2D0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:16;
		uint32 rxInHdCountFreedBigEnd:16;	// Number of Ready HDs in the Host
	} bitFields;
} RegHostIfAccRxInHdAccumCountFreedBigEnd_u;

/*REG_HOST_IF_ACC_TX_IN_HD_ACCUM_COUNT_FREED_BIG_END 0x2D4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:16;
		uint32 txInHdCountFreedBigEnd:16;	// Number of Ready HDs in the Host
	} bitFields;
} RegHostIfAccTxInHdAccumCountFreedBigEnd_u;

/*REG_HOST_IF_ACC_RX_OUT_HD_ACCUM_COUNT_READY_BIG_END 0x2D8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:16;
		uint32 rxOutHdCountReadyBigEnd:16;	// Number of Ready HDs in the Host
	} bitFields;
} RegHostIfAccRxOutHdAccumCountReadyBigEnd_u;

/*REG_HOST_IF_ACC_TX_OUT_HD_ACCUM_COUNT_READY_BIG_END 0x2DC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:16;
		uint32 txOutHdCountReadyBigEnd:16;	// Number of Ready HDs in the Host
	} bitFields;
} RegHostIfAccTxOutHdAccumCountReadyBigEnd_u;



#endif // _HOST_IF_ACC_REGS_H_
