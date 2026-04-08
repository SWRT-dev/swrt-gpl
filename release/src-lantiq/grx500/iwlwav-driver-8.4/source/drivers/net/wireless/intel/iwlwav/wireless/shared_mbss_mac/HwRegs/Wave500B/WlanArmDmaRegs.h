
/***********************************************************************************
File:				WlanArmDmaRegs.h
Module:				wlanArmDma
SOC Revision:		843
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _WLAN_ARM_DMA_REGS_H_
#define _WLAN_ARM_DMA_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define WLAN_ARM_DMA_BASE_ADDRESS                             MEMORY_MAP_UNIT_48_BASE_ADDRESS
#define	REG_WLAN_ARM_DMA_STATUS                         (WLAN_ARM_DMA_BASE_ADDRESS + 0x0)
#define	REG_WLAN_ARM_DMA_PC                             (WLAN_ARM_DMA_BASE_ADDRESS + 0x4)
#define	REG_WLAN_ARM_DMA_INT_EN                         (WLAN_ARM_DMA_BASE_ADDRESS + 0x20)
#define	REG_WLAN_ARM_DMA_INT_EVENT_RIS                  (WLAN_ARM_DMA_BASE_ADDRESS + 0x24)
#define	REG_WLAN_ARM_DMA_INT_MIS                        (WLAN_ARM_DMA_BASE_ADDRESS + 0x28)
#define	REG_WLAN_ARM_DMA_INT_CLR                        (WLAN_ARM_DMA_BASE_ADDRESS + 0x2C)
#define	REG_WLAN_ARM_DMA_FAULT_STATUS_MGR_THREAD        (WLAN_ARM_DMA_BASE_ADDRESS + 0x30)
#define	REG_WLAN_ARM_DMA_FAULT_STATUS_CHANNEL_THREAD    (WLAN_ARM_DMA_BASE_ADDRESS + 0x34)
#define	REG_WLAN_ARM_DMA_FAULT_TYPE_MGR_THREAD          (WLAN_ARM_DMA_BASE_ADDRESS + 0x38)
#define	REG_WLAN_ARM_DMA_FTR0                           (WLAN_ARM_DMA_BASE_ADDRESS + 0x40)
#define	REG_WLAN_ARM_DMA_FTR1                           (WLAN_ARM_DMA_BASE_ADDRESS + 0x44)
#define	REG_WLAN_ARM_DMA_FTR2                           (WLAN_ARM_DMA_BASE_ADDRESS + 0x48)
#define	REG_WLAN_ARM_DMA_FTR3                           (WLAN_ARM_DMA_BASE_ADDRESS + 0x4C)
#define	REG_WLAN_ARM_DMA_CSR0                           (WLAN_ARM_DMA_BASE_ADDRESS + 0x100)
#define	REG_WLAN_ARM_DMA_CPC0                           (WLAN_ARM_DMA_BASE_ADDRESS + 0x104)
#define	REG_WLAN_ARM_DMA_CSR1                           (WLAN_ARM_DMA_BASE_ADDRESS + 0x108)
#define	REG_WLAN_ARM_DMA_CPC1                           (WLAN_ARM_DMA_BASE_ADDRESS + 0x10C)
#define	REG_WLAN_ARM_DMA_CSR2                           (WLAN_ARM_DMA_BASE_ADDRESS + 0x110)
#define	REG_WLAN_ARM_DMA_CPC2                           (WLAN_ARM_DMA_BASE_ADDRESS + 0x114)
#define	REG_WLAN_ARM_DMA_CSR3                           (WLAN_ARM_DMA_BASE_ADDRESS + 0x118)
#define	REG_WLAN_ARM_DMA_CPC3                           (WLAN_ARM_DMA_BASE_ADDRESS + 0x11C)
#define	REG_WLAN_ARM_DMA_SAR0                           (WLAN_ARM_DMA_BASE_ADDRESS + 0x400)
#define	REG_WLAN_ARM_DMA_DAR0                           (WLAN_ARM_DMA_BASE_ADDRESS + 0x404)
#define	REG_WLAN_ARM_DMA_CCR0                           (WLAN_ARM_DMA_BASE_ADDRESS + 0x408)
#define	REG_WLAN_ARM_DMA_LC0_0                          (WLAN_ARM_DMA_BASE_ADDRESS + 0x40C)
#define	REG_WLAN_ARM_DMA_LC1_0                          (WLAN_ARM_DMA_BASE_ADDRESS + 0x410)
#define	REG_WLAN_ARM_DMA_SAR1                           (WLAN_ARM_DMA_BASE_ADDRESS + 0x420)
#define	REG_WLAN_ARM_DMA_DAR1                           (WLAN_ARM_DMA_BASE_ADDRESS + 0x424)
#define	REG_WLAN_ARM_DMA_CCR1                           (WLAN_ARM_DMA_BASE_ADDRESS + 0x428)
#define	REG_WLAN_ARM_DMA_LC0_1                          (WLAN_ARM_DMA_BASE_ADDRESS + 0x42C)
#define	REG_WLAN_ARM_DMA_LC1_1                          (WLAN_ARM_DMA_BASE_ADDRESS + 0x430)
#define	REG_WLAN_ARM_DMA_SAR2                           (WLAN_ARM_DMA_BASE_ADDRESS + 0x440)
#define	REG_WLAN_ARM_DMA_DAR2                           (WLAN_ARM_DMA_BASE_ADDRESS + 0x444)
#define	REG_WLAN_ARM_DMA_CCR2                           (WLAN_ARM_DMA_BASE_ADDRESS + 0x448)
#define	REG_WLAN_ARM_DMA_LC0_2                          (WLAN_ARM_DMA_BASE_ADDRESS + 0x44C)
#define	REG_WLAN_ARM_DMA_LC1_2                          (WLAN_ARM_DMA_BASE_ADDRESS + 0x450)
#define	REG_WLAN_ARM_DMA_SAR3                           (WLAN_ARM_DMA_BASE_ADDRESS + 0x460)
#define	REG_WLAN_ARM_DMA_DAR3                           (WLAN_ARM_DMA_BASE_ADDRESS + 0x464)
#define	REG_WLAN_ARM_DMA_CCR3                           (WLAN_ARM_DMA_BASE_ADDRESS + 0x468)
#define	REG_WLAN_ARM_DMA_LC0_3                          (WLAN_ARM_DMA_BASE_ADDRESS + 0x46C)
#define	REG_WLAN_ARM_DMA_LC1_3                          (WLAN_ARM_DMA_BASE_ADDRESS + 0x470)
#define	REG_WLAN_ARM_DMA_DBGSTATUS                      (WLAN_ARM_DMA_BASE_ADDRESS + 0xD00)
#define	REG_WLAN_ARM_DMA_DBGCMD                         (WLAN_ARM_DMA_BASE_ADDRESS + 0xD04)
#define	REG_WLAN_ARM_DMA_DBGINST0                       (WLAN_ARM_DMA_BASE_ADDRESS + 0xD08)
#define	REG_WLAN_ARM_DMA_DBGINST1                       (WLAN_ARM_DMA_BASE_ADDRESS + 0xD0C)
#define	REG_WLAN_ARM_DMA_CR0                            (WLAN_ARM_DMA_BASE_ADDRESS + 0xE00)
#define	REG_WLAN_ARM_DMA_CR1                            (WLAN_ARM_DMA_BASE_ADDRESS + 0xE04)
#define	REG_WLAN_ARM_DMA_CR2                            (WLAN_ARM_DMA_BASE_ADDRESS + 0xE08)
#define	REG_WLAN_ARM_DMA_CR3                            (WLAN_ARM_DMA_BASE_ADDRESS + 0xE0C)
#define	REG_WLAN_ARM_DMA_CR4                            (WLAN_ARM_DMA_BASE_ADDRESS + 0xE10)
#define	REG_WLAN_ARM_DMA_CRD                            (WLAN_ARM_DMA_BASE_ADDRESS + 0xE14)
#define	REG_WLAN_ARM_DMA_WD                             (WLAN_ARM_DMA_BASE_ADDRESS + 0xE80)
#define	REG_WLAN_ARM_DMA_PERIPH_ID_0                    (WLAN_ARM_DMA_BASE_ADDRESS + 0xFE0)
#define	REG_WLAN_ARM_DMA_PERIPH_ID_1                    (WLAN_ARM_DMA_BASE_ADDRESS + 0xFE4)
#define	REG_WLAN_ARM_DMA_PERIPH_ID_2                    (WLAN_ARM_DMA_BASE_ADDRESS + 0xFE8)
#define	REG_WLAN_ARM_DMA_PERIPH_ID_3                    (WLAN_ARM_DMA_BASE_ADDRESS + 0xFEC)
#define	REG_WLAN_ARM_DMA_PCELL_ID_0                     (WLAN_ARM_DMA_BASE_ADDRESS + 0xFF0)
#define	REG_WLAN_ARM_DMA_PCELL_ID_1                     (WLAN_ARM_DMA_BASE_ADDRESS + 0xFF4)
#define	REG_WLAN_ARM_DMA_PCELL_ID_2                     (WLAN_ARM_DMA_BASE_ADDRESS + 0xFF8)
#define	REG_WLAN_ARM_DMA_PCELL_ID_3                     (WLAN_ARM_DMA_BASE_ADDRESS + 0xFFC)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_WLAN_ARM_DMA_STATUS 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dmaStatus:4;	// DMA manager operating state
		uint32 wakeupEvent:5;	// Wakeup event
		uint32 dns:1;	// Secure state of DMA manager thread
		uint32 reserved0:22;
	} bitFields;
} RegWlanArmDmaStatus_u;

/*REG_WLAN_ARM_DMA_PC 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dmaProgramCnt:32;	// Program counter of DMA manager thread
	} bitFields;
} RegWlanArmDmaPc_u;

/*REG_WLAN_ARM_DMA_INT_EN 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dmaIntEn:16;	// DMA interrupt enable
		uint32 reserved0:16;
	} bitFields;
} RegWlanArmDmaIntEn_u;

/*REG_WLAN_ARM_DMA_INT_EVENT_RIS 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rawInterruptStatus:16;	// Event interrupt raw status
		uint32 reserved0:16;
	} bitFields;
} RegWlanArmDmaIntEventRis_u;

/*REG_WLAN_ARM_DMA_INT_MIS 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 interruptStatus:16;	// Interrupt status register
		uint32 reserved0:16;
	} bitFields;
} RegWlanArmDmaIntMis_u;

/*REG_WLAN_ARM_DMA_INT_CLR 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 interruptClear:16;	// Interrupt clear
		uint32 reserved0:16;
	} bitFields;
} RegWlanArmDmaIntClr_u;

/*REG_WLAN_ARM_DMA_FAULT_STATUS_MGR_THREAD 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:16;
		uint32 fsMgr:16;	// MGR thread fault status
	} bitFields;
} RegWlanArmDmaFaultStatusMgrThread_u;

/*REG_WLAN_ARM_DMA_FAULT_STATUS_CHANNEL_THREAD 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fsChan0:1;	// Channel 0 fault status
		uint32 fsChan1:1;	// Channel 1 fault status
		uint32 fsChan2:1;	// Channel 2 fault status
		uint32 fsChan3:1;	// Channel 3 fault status
		uint32 reserved0:28;
	} bitFields;
} RegWlanArmDmaFaultStatusChannelThread_u;

/*REG_WLAN_ARM_DMA_FAULT_TYPE_MGR_THREAD 0x38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 undefInstr:1;	// Undefined instruction
		uint32 operandInvalid:1;	// Invalid operand
		uint32 reserved0:2;
		uint32 dmagoErr:1;	// DMA Go error
		uint32 mgrEventErr:1;	// Manager event error
		uint32 reserved1:10;
		uint32 instrFetchErr:1;	// Instruction fetch error
		uint32 reserved2:13;
		uint32 dgbInstr:1;	// Debug instruction
		uint32 reserved3:1;
	} bitFields;
} RegWlanArmDmaFaultTypeMgrThread_u;

/*REG_WLAN_ARM_DMA_FTR0 0x40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 undefInstr:1;	// Undefined instruction
		uint32 operandInvalid:1;	// Invalid operand
		uint32 reserved0:3;
		uint32 chEventErr:1;	// Event related instruction with inappropriate security setting
		uint32 chPeriphErr:1;	// Peripheral related instruction with inappropriate security setting
		uint32 chRdwrErr:1;	// Inappropriate CCR instruction of non-secured thread
		uint32 reserved1:4;
		uint32 mfifoErr:1;	// MFIFO prevented DMALD or DMAST during operation
		uint32 stDataUnavailable:1;	// DMAST cannot proceed due to empty MFIFO
		uint32 reserved2:2;
		uint32 instrFetchErr:1;	// Error response from AXI bus during instruction fetch
		uint32 dataWriteErr:1;	// Error response from AXI bus during data write
		uint32 dataReadErr:1;	// Error response from AXI bus during data read
		uint32 reserved3:11;
		uint32 dbgInstr:1;	// Error indication generated from debug interface
		uint32 lockupErr:1;	// Lockup due to resource starvation
	} bitFields;
} RegWlanArmDmaFtr0_u;

/*REG_WLAN_ARM_DMA_FTR1 0x44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 undefInstr:1;	// Undefined instruction
		uint32 operandInvalid:1;	// Invalid operand
		uint32 reserved0:3;
		uint32 chEventErr:1;	// Event related instruction with inappropriate security setting
		uint32 chPeriphErr:1;	// Peripheral related instruction with inappropriate security setting
		uint32 chRdwrErr:1;	// Inappropriate CCR instruction of non-secured thread
		uint32 reserved1:4;
		uint32 mfifoErr:1;	// MFIFO prevented DMALD or DMAST during operation
		uint32 stDataUnavailable:1;	// DMAST cannot proceed due to empty MFIFO
		uint32 reserved2:2;
		uint32 instrFetchErr:1;	// Error response from AXI bus during instruction fetch
		uint32 dataWriteErr:1;	// Error response from AXI bus during data write
		uint32 dataReadErr:1;	// Error response from AXI bus during data read
		uint32 reserved3:11;
		uint32 dbgInstr:1;	// Error indication generated from debug interface
		uint32 lockupErr:1;	// Lockup due to resource starvation
	} bitFields;
} RegWlanArmDmaFtr1_u;

/*REG_WLAN_ARM_DMA_FTR2 0x48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 undefInstr:1;	// Undefined instruction
		uint32 operandInvalid:1;	// Invalid operand
		uint32 reserved0:3;
		uint32 chEventErr:1;	// Event related instruction with inappropriate security setting
		uint32 chPeriphErr:1;	// Peripheral related instruction with inappropriate security setting
		uint32 chRdwrErr:1;	// Inappropriate CCR instruction of non-secured thread
		uint32 reserved1:4;
		uint32 mfifoErr:1;	// MFIFO prevented DMALD or DMAST during operation
		uint32 stDataUnavailable:1;	// DMAST cannot proceed due to empty MFIFO
		uint32 reserved2:2;
		uint32 instrFetchErr:1;	// Error response from AXI bus during instruction fetch
		uint32 dataWriteErr:1;	// Error response from AXI bus during data write
		uint32 dataReadErr:1;	// Error response from AXI bus during data read
		uint32 reserved3:11;
		uint32 dbgInstr:1;	// Error indication generated from debug interface
		uint32 lockupErr:1;	// Lockup due to resource starvation
	} bitFields;
} RegWlanArmDmaFtr2_u;

/*REG_WLAN_ARM_DMA_FTR3 0x4C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 undefInstr:1;	// Undefined instruction
		uint32 operandInvalid:1;	// Invalid operand
		uint32 reserved0:3;
		uint32 chEventErr:1;	// Event related instruction with inappropriate security setting
		uint32 chPeriphErr:1;	// Peripheral related instruction with inappropriate security setting
		uint32 chRdwrErr:1;	// Inappropriate CCR instruction of non-secured thread
		uint32 reserved1:4;
		uint32 mfifoErr:1;	// MFIFO prevented DMALD or DMAST during operation
		uint32 stDataUnavailable:1;	// DMAST cannot proceed due to empty MFIFO
		uint32 reserved2:2;
		uint32 instrFetchErr:1;	// Error response from AXI bus during instruction fetch
		uint32 dataWriteErr:1;	// Error response from AXI bus during data write
		uint32 dataReadErr:1;	// Error response from AXI bus during data read
		uint32 reserved3:11;
		uint32 dbgInstr:1;	// Error indication generated from debug interface
		uint32 lockupErr:1;	// Lockup due to resource starvation
	} bitFields;
} RegWlanArmDmaFtr3_u;

/*REG_WLAN_ARM_DMA_CSR0 0x100 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 channelStatus:4;	// Channel status encoding
		uint32 wakeupNumber:5;	// Event/Peripheral # the channel is waiting for
		uint32 reserved0:5;
		uint32 dmawfpBNs:1;	// Burst/signle indication DMA is waiting for during DMAWFP
		uint32 dmawfpPeriph:1;	//  DMAWFP executed with the periph operand set
		uint32 reserved1:5;
		uint32 cns:1;	// Channel operating in non-secure mode
		uint32 reserved2:10;
	} bitFields;
} RegWlanArmDmaCsr0_u;

/*REG_WLAN_ARM_DMA_CPC0 0x104 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pcChnl:32;	// Program counter for Channel0
	} bitFields;
} RegWlanArmDmaCpc0_u;

/*REG_WLAN_ARM_DMA_CSR1 0x108 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 channelStatus:4;	// Channel status encoding
		uint32 wakeupNumber:5;	// Event/Peripheral # the channel is waiting for
		uint32 reserved0:5;
		uint32 dmawfpBNs:1;	// Burst/signle indication DMA is waiting for during DMAWFP
		uint32 dmawfpPeriph:1;	//  DMAWFP executed with the periph operand set
		uint32 reserved1:5;
		uint32 cns:1;	// Channel operating in non-secure mode
		uint32 reserved2:10;
	} bitFields;
} RegWlanArmDmaCsr1_u;

/*REG_WLAN_ARM_DMA_CPC1 0x10C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pcChnl:32;	// Program counter for Channel1
	} bitFields;
} RegWlanArmDmaCpc1_u;

/*REG_WLAN_ARM_DMA_CSR2 0x110 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 channelStatus:4;	// Channel status encoding
		uint32 wakeupNumber:5;	// Event/Peripheral # the channel is waiting for
		uint32 reserved0:5;
		uint32 dmawfpBNs:1;	// Burst/signle indication DMA is waiting for during DMAWFP
		uint32 dmawfpPeriph:1;	//  DMAWFP executed with the periph operand set
		uint32 reserved1:5;
		uint32 cns:1;	// Channel operating in non-secure mode
		uint32 reserved2:10;
	} bitFields;
} RegWlanArmDmaCsr2_u;

/*REG_WLAN_ARM_DMA_CPC2 0x114 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pcChnl:32;	// Program counter for Channel2
	} bitFields;
} RegWlanArmDmaCpc2_u;

/*REG_WLAN_ARM_DMA_CSR3 0x118 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 channelStatus:4;	// Channel status encoding
		uint32 wakeupNumber:5;	// Event/Peripheral # the channel is waiting for
		uint32 reserved0:5;
		uint32 dmawfpBNs:1;	// Burst/signle indication DMA is waiting for during DMAWFP
		uint32 dmawfpPeriph:1;	//  DMAWFP executed with the periph operand set
		uint32 reserved1:5;
		uint32 cns:1;	// Channel operating in non-secure mode
		uint32 reserved2:10;
	} bitFields;
} RegWlanArmDmaCsr3_u;

/*REG_WLAN_ARM_DMA_CPC3 0x11C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pcChnl:32;	// Program counter for Channel3
	} bitFields;
} RegWlanArmDmaCpc3_u;

/*REG_WLAN_ARM_DMA_SAR0 0x400 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 srcAddr:32;	// Source address for channel0
	} bitFields;
} RegWlanArmDmaSar0_u;

/*REG_WLAN_ARM_DMA_DAR0 0x404 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dstAddr:32;	// Destination address for channel 0
	} bitFields;
} RegWlanArmDmaDar0_u;

/*REG_WLAN_ARM_DMA_CCR0 0x408 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 srcInc:1;	// Source address increment
		uint32 srcBurstSize:3;	// Source burst size
		uint32 srcBurstLen:4;	// Source burst length
		uint32 srcProtCtrl:3;	// Source protection control
		uint32 srcCacheCtrl:3;	// Source cache control
		uint32 dstInc:1;	// Destination address increment
		uint32 dstBurstSize:3;	// Destination burst size
		uint32 dstBurstLen:4;	// Destination burst length
		uint32 dstProtCtrl:3;	// Destination protection control
		uint32 dstCacheCtrl:3;	// Destination cache control
		uint32 endianSwapSize:3;	// Endian swap size
		uint32 reserved0:1;
	} bitFields;
} RegWlanArmDmaCcr0_u;

/*REG_WLAN_ARM_DMA_LC0_0 0x40C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 loopCounterInterations:8;	// Loop counter 0 iteration count
		uint32 reserved0:24;
	} bitFields;
} RegWlanArmDmaLc00_u;

/*REG_WLAN_ARM_DMA_LC1_0 0x410 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 loopCounterInterations:8;	// Loop counter 1 iteration count
		uint32 reserved0:24;
	} bitFields;
} RegWlanArmDmaLc10_u;

/*REG_WLAN_ARM_DMA_SAR1 0x420 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 srcAddr:32;	// Source address for channel1
	} bitFields;
} RegWlanArmDmaSar1_u;

/*REG_WLAN_ARM_DMA_DAR1 0x424 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dstAddr:32;	// Destination address for channel 1
	} bitFields;
} RegWlanArmDmaDar1_u;

/*REG_WLAN_ARM_DMA_CCR1 0x428 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 srcInc:1;	// Source address increment
		uint32 srcBurstSize:3;	// Source burst size
		uint32 srcBurstLen:4;	// Source burst length
		uint32 srcProtCtrl:3;	// Source protection control
		uint32 srcCacheCtrl:3;	// Source cache control
		uint32 dstInc:1;	// Destination address increment
		uint32 dstBurstSize:3;	// Destination burst size
		uint32 dstBurstLen:4;	// Destination burst length
		uint32 dstProtCtrl:3;	// Destination protection control
		uint32 dstCacheCtrl:3;	// Destination cache control
		uint32 endianSwapSize:3;	// Endian swap size
		uint32 reserved0:1;
	} bitFields;
} RegWlanArmDmaCcr1_u;

/*REG_WLAN_ARM_DMA_LC0_1 0x42C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 loopCounterInterations:8;	// Loop counter 0 iteration count
		uint32 reserved0:24;
	} bitFields;
} RegWlanArmDmaLc01_u;

/*REG_WLAN_ARM_DMA_LC1_1 0x430 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 loopCounterInterations:8;	// Loop counter 1 iteration count
		uint32 reserved0:24;
	} bitFields;
} RegWlanArmDmaLc11_u;

/*REG_WLAN_ARM_DMA_SAR2 0x440 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 srcAddr:32;	// Source address for channel2
	} bitFields;
} RegWlanArmDmaSar2_u;

/*REG_WLAN_ARM_DMA_DAR2 0x444 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dstAddr:32;	// Destination address for channel 2
	} bitFields;
} RegWlanArmDmaDar2_u;

/*REG_WLAN_ARM_DMA_CCR2 0x448 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:1;
		uint32 srcBurstSize:3;	// Source burst size
		uint32 srcBurstLen:4;	// Source burst length
		uint32 srcProtCtrl:3;	// Source protection control
		uint32 srcCacheCtrl:3;	// Source cache control
		uint32 dstInc:1;	// Destination address increment
		uint32 dstBurstSize:3;	// Destination burst size
		uint32 dstBurstLen:4;	// Destination burst length
		uint32 dstProtCtrl:3;	// Destination protection control
		uint32 dstCacheCtrl:3;	// Destination cache control
		uint32 endianSwapSize:3;	// Endian swap size
		uint32 reserved1:1;
	} bitFields;
} RegWlanArmDmaCcr2_u;

/*REG_WLAN_ARM_DMA_LC0_2 0x44C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 loopCounterInterations:8;	// Loop counter 0 iteration count
		uint32 reserved0:24;
	} bitFields;
} RegWlanArmDmaLc02_u;

/*REG_WLAN_ARM_DMA_LC1_2 0x450 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 loopCounterInterations:8;	// Loop counter 1 iteration count
		uint32 reserved0:24;
	} bitFields;
} RegWlanArmDmaLc12_u;

/*REG_WLAN_ARM_DMA_SAR3 0x460 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 srcAddr:32;	// Source address for channel3
	} bitFields;
} RegWlanArmDmaSar3_u;

/*REG_WLAN_ARM_DMA_DAR3 0x464 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dstAddr:32;	// Destination address for channel 3
	} bitFields;
} RegWlanArmDmaDar3_u;

/*REG_WLAN_ARM_DMA_CCR3 0x468 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:1;
		uint32 srcBurstSize:3;	// Source burst size
		uint32 srcBurstLen:4;	// Source burst length
		uint32 srcProtCtrl:3;	// Source protection control
		uint32 srcCacheCtrl:3;	// Source cache control
		uint32 dstInc:1;	// Destination address increment
		uint32 dstBurstSize:3;	// Destination burst size
		uint32 dstBurstLen:4;	// Destination burst length
		uint32 dstProtCtrl:3;	// Destination protection control
		uint32 dstCacheCtrl:3;	// Destination cache control
		uint32 endianSwapSize:3;	// Endian swap size
		uint32 reserved1:1;
	} bitFields;
} RegWlanArmDmaCcr3_u;

/*REG_WLAN_ARM_DMA_LC0_3 0x46C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 loopCounterInterations:8;	// Loop counter 0 iteration count
		uint32 reserved0:24;
	} bitFields;
} RegWlanArmDmaLc03_u;

/*REG_WLAN_ARM_DMA_LC1_3 0x470 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 loopCounterInterations:8;	// Loop counter 1 iteration count
		uint32 reserved0:24;
	} bitFields;
} RegWlanArmDmaLc13_u;

/*REG_WLAN_ARM_DMA_DBGSTATUS 0xD00 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 debugStatus:1;	// Debug interface is busy
		uint32 reserved0:31;
	} bitFields;
} RegWlanArmDmaDbgstatus_u;

/*REG_WLAN_ARM_DMA_DBGCMD 0xD04 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 debugCommand:8;	// Debug command trigger
		uint32 reserved0:24;
	} bitFields;
} RegWlanArmDmaDbgcmd_u;

/*REG_WLAN_ARM_DMA_DBGINST0 0xD08 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 debugThread:1;	// Debug thread
		uint32 reserved0:7;
		uint32 channelNumber:3;	// Channel number
		uint32 reserved1:5;
		uint32 instrByte0:8;	// Insturction byte 0
		uint32 instrByte1:8;	// Instruction byte 1
	} bitFields;
} RegWlanArmDmaDbginst0_u;

/*REG_WLAN_ARM_DMA_DBGINST1 0xD0C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 instrByte2:8;	// Instruction byte 2
		uint32 instrByte3:8;	// Instruction byte 3
		uint32 instrByte4:8;	// Instruction byte 4
		uint32 instrByte5:8;	// Instruction byte 5
	} bitFields;
} RegWlanArmDmaDbginst1_u;

/*REG_WLAN_ARM_DMA_CR0 0xE00 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 periphRequest:1;	// DMA support peripheral request
		uint32 bootEn:1;	// Manager thread boot enable
		uint32 mgrNsAtRst:1;	// Manager configured to non-secure mode at reset
		uint32 reserved0:1;
		uint32 numChnls:3;	// Number of channnels
		uint32 reserved1:5;
		uint32 numPeriphReq:5;	// Number of peripheral request interfaces that the DMAC provides
		uint32 numEvents:5;	// Number of interrupt outputs that the DMAC provides
		uint32 reserved2:10;
	} bitFields;
} RegWlanArmDmaCr0_u;

/*REG_WLAN_ARM_DMA_CR1 0xE04 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 icacheLen:3;	// Encoded length of icache line
		uint32 reserved0:1;
		uint32 icacheLines:3;	// Number of icache lines
		uint32 reserved1:25;
	} bitFields;
} RegWlanArmDmaCr1_u;

/*REG_WLAN_ARM_DMA_CR2 0xE08 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bootAddr:32;	// Boot address when DMAC exists reset
	} bitFields;
} RegWlanArmDmaCr2_u;

/*REG_WLAN_ARM_DMA_CR3 0xE0C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ins:16;	// Security state of an event-interrupt resource
		uint32 reserved0:16;
	} bitFields;
} RegWlanArmDmaCr3_u;

/*REG_WLAN_ARM_DMA_CR4 0xE10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pns:16;	// Security state of the peripheral request interfaces
		uint32 reserved0:16;
	} bitFields;
} RegWlanArmDmaCr4_u;

/*REG_WLAN_ARM_DMA_CRD 0xE14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dataWidth:3;	// Data width encoding
		uint32 reserved0:1;
		uint32 wrCap:3;	// Write issuing capability
		uint32 reserved1:1;
		uint32 wrQDep:4;	// Depth of write queue
		uint32 rdCap:3;	// Read issuing capability
		uint32 reserved2:1;
		uint32 rdQDep:4;	// Depth of read queue
		uint32 dataBufferDep:10;	// Number of lines that the data buffer contains
		uint32 reserved3:2;
	} bitFields;
} RegWlanArmDmaCrd_u;

/*REG_WLAN_ARM_DMA_WD 0xE80 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 wdIrqOnly:1;	// Set IRQ abort when DMAC detects lock
		uint32 reserved0:31;
	} bitFields;
} RegWlanArmDmaWd_u;

/*REG_WLAN_ARM_DMA_PERIPH_ID_0 0xFE0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 partNumber0:8;	// DMA part number (lower 8 bits)
		uint32 reserved0:24;
	} bitFields;
} RegWlanArmDmaPeriphId0_u;

/*REG_WLAN_ARM_DMA_PERIPH_ID_1 0xFE4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 partNumber1:4;	// DMA part number (upper 4 bits)
		uint32 designer0:4;	// Designer code (lower 4 bits)
		uint32 reserved0:24;
	} bitFields;
} RegWlanArmDmaPeriphId1_u;

/*REG_WLAN_ARM_DMA_PERIPH_ID_2 0xFE8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 designer1:4;	// Designer code (upper 4 bits)
		uint32 revision:4;	// DMA revision code
		uint32 reserved0:24;
	} bitFields;
} RegWlanArmDmaPeriphId2_u;

/*REG_WLAN_ARM_DMA_PERIPH_ID_3 0xFEC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 integrationCfg:1;	// Integration test logic indication
		uint32 reserved0:31;
	} bitFields;
} RegWlanArmDmaPeriphId3_u;

/*REG_WLAN_ARM_DMA_PCELL_ID_0 0xFF0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 id0:8;	// PCELL ID 1
		uint32 reserved0:24;
	} bitFields;
} RegWlanArmDmaPcellId0_u;

/*REG_WLAN_ARM_DMA_PCELL_ID_1 0xFF4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 id1:8;
		uint32 reserved0:24;
	} bitFields;
} RegWlanArmDmaPcellId1_u;

/*REG_WLAN_ARM_DMA_PCELL_ID_2 0xFF8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 id2:8;
		uint32 reserved0:24;
	} bitFields;
} RegWlanArmDmaPcellId2_u;

/*REG_WLAN_ARM_DMA_PCELL_ID_3 0xFFC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 id3:8;
		uint32 reserved0:24;
	} bitFields;
} RegWlanArmDmaPcellId3_u;



#endif // _WLAN_ARM_DMA_REGS_H_
