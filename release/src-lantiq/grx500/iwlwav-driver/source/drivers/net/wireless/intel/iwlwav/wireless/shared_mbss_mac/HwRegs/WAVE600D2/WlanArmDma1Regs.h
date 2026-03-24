
/***********************************************************************************
File:				WlanArmDma1Regs.h
Module:				wlanArmDma1
SOC Revision:		
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _WLAN_ARM_DMA1_REGS_H_
#define _WLAN_ARM_DMA1_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define WLAN_ARM_DMA1_BASE_ADDRESS                             MEMORY_MAP_UNIT_55_BASE_ADDRESS
#define	REG_WLAN_ARM_DMA1_STATUS                         (WLAN_ARM_DMA1_BASE_ADDRESS + 0x0)
#define	REG_WLAN_ARM_DMA1_PC                             (WLAN_ARM_DMA1_BASE_ADDRESS + 0x4)
#define	REG_WLAN_ARM_DMA1_INT_EN                         (WLAN_ARM_DMA1_BASE_ADDRESS + 0x20)
#define	REG_WLAN_ARM_DMA1_INT_EVENT_RIS                  (WLAN_ARM_DMA1_BASE_ADDRESS + 0x24)
#define	REG_WLAN_ARM_DMA1_INT_MIS                        (WLAN_ARM_DMA1_BASE_ADDRESS + 0x28)
#define	REG_WLAN_ARM_DMA1_INT_CLR                        (WLAN_ARM_DMA1_BASE_ADDRESS + 0x2C)
#define	REG_WLAN_ARM_DMA1_FAULT_STATUS_MGR_THREAD        (WLAN_ARM_DMA1_BASE_ADDRESS + 0x30)
#define	REG_WLAN_ARM_DMA1_FAULT_STATUS_CHANNEL_THREAD    (WLAN_ARM_DMA1_BASE_ADDRESS + 0x34)
#define	REG_WLAN_ARM_DMA1_FAULT_TYPE_MGR_THREAD          (WLAN_ARM_DMA1_BASE_ADDRESS + 0x38)
#define	REG_WLAN_ARM_DMA1_FTR0                           (WLAN_ARM_DMA1_BASE_ADDRESS + 0x40)
#define	REG_WLAN_ARM_DMA1_FTR1                           (WLAN_ARM_DMA1_BASE_ADDRESS + 0x44)
#define	REG_WLAN_ARM_DMA1_FTR2                           (WLAN_ARM_DMA1_BASE_ADDRESS + 0x48)
#define	REG_WLAN_ARM_DMA1_FTR3                           (WLAN_ARM_DMA1_BASE_ADDRESS + 0x4C)
#define	REG_WLAN_ARM_DMA1_CSR0                           (WLAN_ARM_DMA1_BASE_ADDRESS + 0x100)
#define	REG_WLAN_ARM_DMA1_CPC0                           (WLAN_ARM_DMA1_BASE_ADDRESS + 0x104)
#define	REG_WLAN_ARM_DMA1_CSR1                           (WLAN_ARM_DMA1_BASE_ADDRESS + 0x108)
#define	REG_WLAN_ARM_DMA1_CPC1                           (WLAN_ARM_DMA1_BASE_ADDRESS + 0x10C)
#define	REG_WLAN_ARM_DMA1_CSR2                           (WLAN_ARM_DMA1_BASE_ADDRESS + 0x110)
#define	REG_WLAN_ARM_DMA1_CPC2                           (WLAN_ARM_DMA1_BASE_ADDRESS + 0x114)
#define	REG_WLAN_ARM_DMA1_CSR3                           (WLAN_ARM_DMA1_BASE_ADDRESS + 0x118)
#define	REG_WLAN_ARM_DMA1_CPC3                           (WLAN_ARM_DMA1_BASE_ADDRESS + 0x11C)
#define	REG_WLAN_ARM_DMA1_SAR0                           (WLAN_ARM_DMA1_BASE_ADDRESS + 0x400)
#define	REG_WLAN_ARM_DMA1_DAR0                           (WLAN_ARM_DMA1_BASE_ADDRESS + 0x404)
#define	REG_WLAN_ARM_DMA1_CCR0                           (WLAN_ARM_DMA1_BASE_ADDRESS + 0x408)
#define	REG_WLAN_ARM_DMA1_LC0_0                          (WLAN_ARM_DMA1_BASE_ADDRESS + 0x40C)
#define	REG_WLAN_ARM_DMA1_LC1_0                          (WLAN_ARM_DMA1_BASE_ADDRESS + 0x410)
#define	REG_WLAN_ARM_DMA1_SAR1                           (WLAN_ARM_DMA1_BASE_ADDRESS + 0x420)
#define	REG_WLAN_ARM_DMA1_DAR1                           (WLAN_ARM_DMA1_BASE_ADDRESS + 0x424)
#define	REG_WLAN_ARM_DMA1_CCR1                           (WLAN_ARM_DMA1_BASE_ADDRESS + 0x428)
#define	REG_WLAN_ARM_DMA1_LC0_1                          (WLAN_ARM_DMA1_BASE_ADDRESS + 0x42C)
#define	REG_WLAN_ARM_DMA1_LC1_1                          (WLAN_ARM_DMA1_BASE_ADDRESS + 0x430)
#define	REG_WLAN_ARM_DMA1_SAR2                           (WLAN_ARM_DMA1_BASE_ADDRESS + 0x440)
#define	REG_WLAN_ARM_DMA1_DAR2                           (WLAN_ARM_DMA1_BASE_ADDRESS + 0x444)
#define	REG_WLAN_ARM_DMA1_CCR2                           (WLAN_ARM_DMA1_BASE_ADDRESS + 0x448)
#define	REG_WLAN_ARM_DMA1_LC0_2                          (WLAN_ARM_DMA1_BASE_ADDRESS + 0x44C)
#define	REG_WLAN_ARM_DMA1_LC1_2                          (WLAN_ARM_DMA1_BASE_ADDRESS + 0x450)
#define	REG_WLAN_ARM_DMA1_SAR3                           (WLAN_ARM_DMA1_BASE_ADDRESS + 0x460)
#define	REG_WLAN_ARM_DMA1_DAR3                           (WLAN_ARM_DMA1_BASE_ADDRESS + 0x464)
#define	REG_WLAN_ARM_DMA1_CCR3                           (WLAN_ARM_DMA1_BASE_ADDRESS + 0x468)
#define	REG_WLAN_ARM_DMA1_LC0_3                          (WLAN_ARM_DMA1_BASE_ADDRESS + 0x46C)
#define	REG_WLAN_ARM_DMA1_LC1_3                          (WLAN_ARM_DMA1_BASE_ADDRESS + 0x470)
#define	REG_WLAN_ARM_DMA1_DBGSTATUS                      (WLAN_ARM_DMA1_BASE_ADDRESS + 0xD00)
#define	REG_WLAN_ARM_DMA1_DBGCMD                         (WLAN_ARM_DMA1_BASE_ADDRESS + 0xD04)
#define	REG_WLAN_ARM_DMA1_DBGINST0                       (WLAN_ARM_DMA1_BASE_ADDRESS + 0xD08)
#define	REG_WLAN_ARM_DMA1_DBGINST1                       (WLAN_ARM_DMA1_BASE_ADDRESS + 0xD0C)
#define	REG_WLAN_ARM_DMA1_CR0                            (WLAN_ARM_DMA1_BASE_ADDRESS + 0xE00)
#define	REG_WLAN_ARM_DMA1_CR1                            (WLAN_ARM_DMA1_BASE_ADDRESS + 0xE04)
#define	REG_WLAN_ARM_DMA1_CR2                            (WLAN_ARM_DMA1_BASE_ADDRESS + 0xE08)
#define	REG_WLAN_ARM_DMA1_CR3                            (WLAN_ARM_DMA1_BASE_ADDRESS + 0xE0C)
#define	REG_WLAN_ARM_DMA1_CR4                            (WLAN_ARM_DMA1_BASE_ADDRESS + 0xE10)
#define	REG_WLAN_ARM_DMA1_CRD                            (WLAN_ARM_DMA1_BASE_ADDRESS + 0xE14)
#define	REG_WLAN_ARM_DMA1_WD                             (WLAN_ARM_DMA1_BASE_ADDRESS + 0xE80)
#define	REG_WLAN_ARM_DMA1_PERIPH_ID_0                    (WLAN_ARM_DMA1_BASE_ADDRESS + 0xFE0)
#define	REG_WLAN_ARM_DMA1_PERIPH_ID_1                    (WLAN_ARM_DMA1_BASE_ADDRESS + 0xFE4)
#define	REG_WLAN_ARM_DMA1_PERIPH_ID_2                    (WLAN_ARM_DMA1_BASE_ADDRESS + 0xFE8)
#define	REG_WLAN_ARM_DMA1_PERIPH_ID_3                    (WLAN_ARM_DMA1_BASE_ADDRESS + 0xFEC)
#define	REG_WLAN_ARM_DMA1_PCELL_ID_0                     (WLAN_ARM_DMA1_BASE_ADDRESS + 0xFF0)
#define	REG_WLAN_ARM_DMA1_PCELL_ID_1                     (WLAN_ARM_DMA1_BASE_ADDRESS + 0xFF4)
#define	REG_WLAN_ARM_DMA1_PCELL_ID_2                     (WLAN_ARM_DMA1_BASE_ADDRESS + 0xFF8)
#define	REG_WLAN_ARM_DMA1_PCELL_ID_3                     (WLAN_ARM_DMA1_BASE_ADDRESS + 0xFFC)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_WLAN_ARM_DMA1_STATUS 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dmaStatus : 4; //DMA manager operating state, reset value: 0x0, access type: RO
		uint32 wakeupEvent : 5; //Wakeup event, reset value: 0x0, access type: RO
		uint32 dns : 1; //Secure state of DMA manager thread, reset value: 0x1, access type: RO
		uint32 reserved0 : 22;
	} bitFields;
} RegWlanArmDma1Status_u;

/*REG_WLAN_ARM_DMA1_PC 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dmaProgramCnt : 32; //Program counter of DMA manager thread, reset value: 0x0, access type: RO
	} bitFields;
} RegWlanArmDma1Pc_u;

/*REG_WLAN_ARM_DMA1_INT_EN 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dmaIntEn : 16; //DMA interrupt enable, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegWlanArmDma1IntEn_u;

/*REG_WLAN_ARM_DMA1_INT_EVENT_RIS 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rawInterruptStatus : 16; //Event interrupt raw status, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegWlanArmDma1IntEventRis_u;

/*REG_WLAN_ARM_DMA1_INT_MIS 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 interruptStatus : 16; //Interrupt status register, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegWlanArmDma1IntMis_u;

/*REG_WLAN_ARM_DMA1_INT_CLR 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 interruptClear : 16; //Interrupt clear, reset value: 0x0, access type: WO
		uint32 reserved0 : 16;
	} bitFields;
} RegWlanArmDma1IntClr_u;

/*REG_WLAN_ARM_DMA1_FAULT_STATUS_MGR_THREAD 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 16;
		uint32 fsMgr : 16; //MGR thread fault status, reset value: 0x0, access type: RO
	} bitFields;
} RegWlanArmDma1FaultStatusMgrThread_u;

/*REG_WLAN_ARM_DMA1_FAULT_STATUS_CHANNEL_THREAD 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fsChan0 : 1; //Channel 0 fault status, reset value: 0x0, access type: RO
		uint32 fsChan1 : 1; //Channel 1 fault status, reset value: 0x0, access type: RO
		uint32 fsChan2 : 1; //Channel 2 fault status, reset value: 0x0, access type: RO
		uint32 fsChan3 : 1; //Channel 3 fault status, reset value: 0x0, access type: RO
		uint32 reserved0 : 28;
	} bitFields;
} RegWlanArmDma1FaultStatusChannelThread_u;

/*REG_WLAN_ARM_DMA1_FAULT_TYPE_MGR_THREAD 0x38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 undefInstr : 1; //Undefined instruction, reset value: 0x0, access type: RO
		uint32 operandInvalid : 1; //Invalid operand, reset value: 0x0, access type: RO
		uint32 reserved0 : 2;
		uint32 dmagoErr : 1; //DMA Go error, reset value: 0x0, access type: RO
		uint32 mgrEventErr : 1; //Manager event error, reset value: 0x0, access type: RO
		uint32 reserved1 : 10;
		uint32 instrFetchErr : 1; //Instruction fetch error, reset value: 0x0, access type: RO
		uint32 reserved2 : 13;
		uint32 dgbInstr : 1; //Debug instruction, reset value: 0x0, access type: RO
		uint32 reserved3 : 1;
	} bitFields;
} RegWlanArmDma1FaultTypeMgrThread_u;

/*REG_WLAN_ARM_DMA1_FTR0 0x40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 undefInstr : 1; //Undefined instruction, reset value: 0x0, access type: RO
		uint32 operandInvalid : 1; //Invalid operand, reset value: 0x0, access type: RO
		uint32 reserved0 : 3;
		uint32 chEventErr : 1; //Event related instruction with inappropriate security setting, reset value: 0x0, access type: RO
		uint32 chPeriphErr : 1; //Peripheral related instruction with inappropriate security setting, reset value: 0x0, access type: RO
		uint32 chRdwrErr : 1; //Inappropriate CCR instruction of non-secured thread, reset value: 0x0, access type: RO
		uint32 reserved1 : 4;
		uint32 mfifoErr : 1; //MFIFO prevented DMALD or DMAST during operation, reset value: 0x0, access type: RO
		uint32 stDataUnavailable : 1; //DMAST cannot proceed due to empty MFIFO, reset value: 0x0, access type: RO
		uint32 reserved2 : 2;
		uint32 instrFetchErr : 1; //Error response from AXI bus during instruction fetch, reset value: 0x0, access type: RO
		uint32 dataWriteErr : 1; //Error response from AXI bus during data write, reset value: 0x0, access type: RO
		uint32 dataReadErr : 1; //Error response from AXI bus during data read, reset value: 0x0, access type: RO
		uint32 reserved3 : 11;
		uint32 dbgInstr : 1; //Error indication generated from debug interface, reset value: 0x0, access type: RO
		uint32 lockupErr : 1; //Lockup due to resource starvation, reset value: 0x0, access type: RO
	} bitFields;
} RegWlanArmDma1Ftr0_u;

/*REG_WLAN_ARM_DMA1_FTR1 0x44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 undefInstr : 1; //Undefined instruction, reset value: 0x0, access type: RO
		uint32 operandInvalid : 1; //Invalid operand, reset value: 0x0, access type: RO
		uint32 reserved0 : 3;
		uint32 chEventErr : 1; //Event related instruction with inappropriate security setting, reset value: 0x0, access type: RO
		uint32 chPeriphErr : 1; //Peripheral related instruction with inappropriate security setting, reset value: 0x0, access type: RO
		uint32 chRdwrErr : 1; //Inappropriate CCR instruction of non-secured thread, reset value: 0x0, access type: RO
		uint32 reserved1 : 4;
		uint32 mfifoErr : 1; //MFIFO prevented DMALD or DMAST during operation, reset value: 0x0, access type: RO
		uint32 stDataUnavailable : 1; //DMAST cannot proceed due to empty MFIFO, reset value: 0x0, access type: RO
		uint32 reserved2 : 2;
		uint32 instrFetchErr : 1; //Error response from AXI bus during instruction fetch, reset value: 0x0, access type: RO
		uint32 dataWriteErr : 1; //Error response from AXI bus during data write, reset value: 0x0, access type: RO
		uint32 dataReadErr : 1; //Error response from AXI bus during data read, reset value: 0x0, access type: RO
		uint32 reserved3 : 11;
		uint32 dbgInstr : 1; //Error indication generated from debug interface, reset value: 0x0, access type: RO
		uint32 lockupErr : 1; //Lockup due to resource starvation, reset value: 0x0, access type: RO
	} bitFields;
} RegWlanArmDma1Ftr1_u;

/*REG_WLAN_ARM_DMA1_FTR2 0x48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 undefInstr : 1; //Undefined instruction, reset value: 0x0, access type: RO
		uint32 operandInvalid : 1; //Invalid operand, reset value: 0x0, access type: RO
		uint32 reserved0 : 3;
		uint32 chEventErr : 1; //Event related instruction with inappropriate security setting, reset value: 0x0, access type: RO
		uint32 chPeriphErr : 1; //Peripheral related instruction with inappropriate security setting, reset value: 0x0, access type: RO
		uint32 chRdwrErr : 1; //Inappropriate CCR instruction of non-secured thread, reset value: 0x0, access type: RO
		uint32 reserved1 : 4;
		uint32 mfifoErr : 1; //MFIFO prevented DMALD or DMAST during operation, reset value: 0x0, access type: RO
		uint32 stDataUnavailable : 1; //DMAST cannot proceed due to empty MFIFO, reset value: 0x0, access type: RO
		uint32 reserved2 : 2;
		uint32 instrFetchErr : 1; //Error response from AXI bus during instruction fetch, reset value: 0x0, access type: RO
		uint32 dataWriteErr : 1; //Error response from AXI bus during data write, reset value: 0x0, access type: RO
		uint32 dataReadErr : 1; //Error response from AXI bus during data read, reset value: 0x0, access type: RO
		uint32 reserved3 : 11;
		uint32 dbgInstr : 1; //Error indication generated from debug interface, reset value: 0x0, access type: RO
		uint32 lockupErr : 1; //Lockup due to resource starvation, reset value: 0x0, access type: RO
	} bitFields;
} RegWlanArmDma1Ftr2_u;

/*REG_WLAN_ARM_DMA1_FTR3 0x4C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 undefInstr : 1; //Undefined instruction, reset value: 0x0, access type: RO
		uint32 operandInvalid : 1; //Invalid operand, reset value: 0x0, access type: RO
		uint32 reserved0 : 3;
		uint32 chEventErr : 1; //Event related instruction with inappropriate security setting, reset value: 0x0, access type: RO
		uint32 chPeriphErr : 1; //Peripheral related instruction with inappropriate security setting, reset value: 0x0, access type: RO
		uint32 chRdwrErr : 1; //Inappropriate CCR instruction of non-secured thread, reset value: 0x0, access type: RO
		uint32 reserved1 : 4;
		uint32 mfifoErr : 1; //MFIFO prevented DMALD or DMAST during operation, reset value: 0x0, access type: RO
		uint32 stDataUnavailable : 1; //DMAST cannot proceed due to empty MFIFO, reset value: 0x0, access type: RO
		uint32 reserved2 : 2;
		uint32 instrFetchErr : 1; //Error response from AXI bus during instruction fetch, reset value: 0x0, access type: RO
		uint32 dataWriteErr : 1; //Error response from AXI bus during data write, reset value: 0x0, access type: RO
		uint32 dataReadErr : 1; //Error response from AXI bus during data read, reset value: 0x0, access type: RO
		uint32 reserved3 : 11;
		uint32 dbgInstr : 1; //Error indication generated from debug interface, reset value: 0x0, access type: RO
		uint32 lockupErr : 1; //Lockup due to resource starvation, reset value: 0x0, access type: RO
	} bitFields;
} RegWlanArmDma1Ftr3_u;

/*REG_WLAN_ARM_DMA1_CSR0 0x100 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 channelStatus : 4; //Channel status encoding, reset value: 0x0, access type: RO
		uint32 wakeupNumber : 5; //Event/Peripheral # the channel is waiting for, reset value: 0x0, access type: RO
		uint32 reserved0 : 5;
		uint32 dmawfpBNs : 1; //Burst/signle indication DMA is waiting for during DMAWFP, reset value: 0x0, access type: RO
		uint32 dmawfpPeriph : 1; // DMAWFP executed with the periph operand set, reset value: 0x0, access type: RO
		uint32 reserved1 : 5;
		uint32 cns : 1; //Channel operating in non-secure mode, reset value: 0x0, access type: RO
		uint32 reserved2 : 10;
	} bitFields;
} RegWlanArmDma1Csr0_u;

/*REG_WLAN_ARM_DMA1_CPC0 0x104 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pcChnl : 32; //Program counter for Channel0, reset value: 0x0, access type: RO
	} bitFields;
} RegWlanArmDma1Cpc0_u;

/*REG_WLAN_ARM_DMA1_CSR1 0x108 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 channelStatus : 4; //Channel status encoding, reset value: 0x0, access type: RO
		uint32 wakeupNumber : 5; //Event/Peripheral # the channel is waiting for, reset value: 0x0, access type: RO
		uint32 reserved0 : 5;
		uint32 dmawfpBNs : 1; //Burst/signle indication DMA is waiting for during DMAWFP, reset value: 0x0, access type: RO
		uint32 dmawfpPeriph : 1; // DMAWFP executed with the periph operand set, reset value: 0x0, access type: RO
		uint32 reserved1 : 5;
		uint32 cns : 1; //Channel operating in non-secure mode, reset value: 0x0, access type: RO
		uint32 reserved2 : 10;
	} bitFields;
} RegWlanArmDma1Csr1_u;

/*REG_WLAN_ARM_DMA1_CPC1 0x10C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pcChnl : 32; //Program counter for Channel1, reset value: 0x0, access type: RO
	} bitFields;
} RegWlanArmDma1Cpc1_u;

/*REG_WLAN_ARM_DMA1_CSR2 0x110 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 channelStatus : 4; //Channel status encoding, reset value: 0x0, access type: RO
		uint32 wakeupNumber : 5; //Event/Peripheral # the channel is waiting for, reset value: 0x0, access type: RO
		uint32 reserved0 : 5;
		uint32 dmawfpBNs : 1; //Burst/signle indication DMA is waiting for during DMAWFP, reset value: 0x0, access type: RO
		uint32 dmawfpPeriph : 1; // DMAWFP executed with the periph operand set, reset value: 0x0, access type: RO
		uint32 reserved1 : 5;
		uint32 cns : 1; //Channel operating in non-secure mode, reset value: 0x0, access type: RO
		uint32 reserved2 : 10;
	} bitFields;
} RegWlanArmDma1Csr2_u;

/*REG_WLAN_ARM_DMA1_CPC2 0x114 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pcChnl : 32; //Program counter for Channel2, reset value: 0x0, access type: RO
	} bitFields;
} RegWlanArmDma1Cpc2_u;

/*REG_WLAN_ARM_DMA1_CSR3 0x118 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 channelStatus : 4; //Channel status encoding, reset value: 0x0, access type: RO
		uint32 wakeupNumber : 5; //Event/Peripheral # the channel is waiting for, reset value: 0x0, access type: RO
		uint32 reserved0 : 5;
		uint32 dmawfpBNs : 1; //Burst/signle indication DMA is waiting for during DMAWFP, reset value: 0x0, access type: RO
		uint32 dmawfpPeriph : 1; // DMAWFP executed with the periph operand set, reset value: 0x0, access type: RO
		uint32 reserved1 : 5;
		uint32 cns : 1; //Channel operating in non-secure mode, reset value: 0x0, access type: RO
		uint32 reserved2 : 10;
	} bitFields;
} RegWlanArmDma1Csr3_u;

/*REG_WLAN_ARM_DMA1_CPC3 0x11C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pcChnl : 32; //Program counter for Channel3, reset value: 0x0, access type: RO
	} bitFields;
} RegWlanArmDma1Cpc3_u;

/*REG_WLAN_ARM_DMA1_SAR0 0x400 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 srcAddr : 32; //Source address for channel0, reset value: 0x0, access type: RO
	} bitFields;
} RegWlanArmDma1Sar0_u;

/*REG_WLAN_ARM_DMA1_DAR0 0x404 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dstAddr : 32; //Destination address for channel 0, reset value: 0x0, access type: RO
	} bitFields;
} RegWlanArmDma1Dar0_u;

/*REG_WLAN_ARM_DMA1_CCR0 0x408 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 srcInc : 1; //Source address increment, reset value: 0x0, access type: RO
		uint32 srcBurstSize : 3; //Source burst size, reset value: 0x0, access type: RO
		uint32 srcBurstLen : 4; //Source burst length, reset value: 0x0, access type: RO
		uint32 srcProtCtrl : 3; //Source protection control, reset value: 0x0, access type: RO
		uint32 srcCacheCtrl : 3; //Source cache control, reset value: 0x0, access type: RO
		uint32 dstInc : 1; //Destination address increment, reset value: 0x0, access type: RO
		uint32 dstBurstSize : 3; //Destination burst size, reset value: 0x0, access type: RO
		uint32 dstBurstLen : 4; //Destination burst length, reset value: 0x0, access type: RO
		uint32 dstProtCtrl : 3; //Destination protection control, reset value: 0x0, access type: RO
		uint32 dstCacheCtrl : 3; //Destination cache control, reset value: 0x0, access type: RO
		uint32 endianSwapSize : 3; //Endian swap size, reset value: 0x0, access type: RO
		uint32 reserved0 : 1;
	} bitFields;
} RegWlanArmDma1Ccr0_u;

/*REG_WLAN_ARM_DMA1_LC0_0 0x40C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 loopCounterInterations : 8; //Loop counter 0 iteration count, reset value: 0x0, access type: RO
		uint32 reserved0 : 24;
	} bitFields;
} RegWlanArmDma1Lc00_u;

/*REG_WLAN_ARM_DMA1_LC1_0 0x410 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 loopCounterInterations : 8; //Loop counter 1 iteration count, reset value: 0x0, access type: RO
		uint32 reserved0 : 24;
	} bitFields;
} RegWlanArmDma1Lc10_u;

/*REG_WLAN_ARM_DMA1_SAR1 0x420 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 srcAddr : 32; //Source address for channel1, reset value: 0x0, access type: RO
	} bitFields;
} RegWlanArmDma1Sar1_u;

/*REG_WLAN_ARM_DMA1_DAR1 0x424 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dstAddr : 32; //Destination address for channel 1, reset value: 0x0, access type: RO
	} bitFields;
} RegWlanArmDma1Dar1_u;

/*REG_WLAN_ARM_DMA1_CCR1 0x428 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 srcInc : 1; //Source address increment, reset value: 0x0, access type: RO
		uint32 srcBurstSize : 3; //Source burst size, reset value: 0x0, access type: RO
		uint32 srcBurstLen : 4; //Source burst length, reset value: 0x0, access type: RO
		uint32 srcProtCtrl : 3; //Source protection control, reset value: 0x0, access type: RO
		uint32 srcCacheCtrl : 3; //Source cache control, reset value: 0x0, access type: RO
		uint32 dstInc : 1; //Destination address increment, reset value: 0x0, access type: RO
		uint32 dstBurstSize : 3; //Destination burst size, reset value: 0x0, access type: RO
		uint32 dstBurstLen : 4; //Destination burst length, reset value: 0x0, access type: RO
		uint32 dstProtCtrl : 3; //Destination protection control, reset value: 0x0, access type: RO
		uint32 dstCacheCtrl : 3; //Destination cache control, reset value: 0x0, access type: RO
		uint32 endianSwapSize : 3; //Endian swap size, reset value: 0x0, access type: RO
		uint32 reserved0 : 1;
	} bitFields;
} RegWlanArmDma1Ccr1_u;

/*REG_WLAN_ARM_DMA1_LC0_1 0x42C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 loopCounterInterations : 8; //Loop counter 0 iteration count, reset value: 0x0, access type: RO
		uint32 reserved0 : 24;
	} bitFields;
} RegWlanArmDma1Lc01_u;

/*REG_WLAN_ARM_DMA1_LC1_1 0x430 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 loopCounterInterations : 8; //Loop counter 1 iteration count, reset value: 0x0, access type: RO
		uint32 reserved0 : 24;
	} bitFields;
} RegWlanArmDma1Lc11_u;

/*REG_WLAN_ARM_DMA1_SAR2 0x440 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 srcAddr : 32; //Source address for channel2, reset value: 0x0, access type: RO
	} bitFields;
} RegWlanArmDma1Sar2_u;

/*REG_WLAN_ARM_DMA1_DAR2 0x444 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dstAddr : 32; //Destination address for channel 2, reset value: 0x0, access type: RO
	} bitFields;
} RegWlanArmDma1Dar2_u;

/*REG_WLAN_ARM_DMA1_CCR2 0x448 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 1;
		uint32 srcBurstSize : 3; //Source burst size, reset value: 0x0, access type: RO
		uint32 srcBurstLen : 4; //Source burst length, reset value: 0x0, access type: RO
		uint32 srcProtCtrl : 3; //Source protection control, reset value: 0x0, access type: RO
		uint32 srcCacheCtrl : 3; //Source cache control, reset value: 0x0, access type: RO
		uint32 dstInc : 1; //Destination address increment, reset value: 0x0, access type: RO
		uint32 dstBurstSize : 3; //Destination burst size, reset value: 0x0, access type: RO
		uint32 dstBurstLen : 4; //Destination burst length, reset value: 0x0, access type: RO
		uint32 dstProtCtrl : 3; //Destination protection control, reset value: 0x0, access type: RO
		uint32 dstCacheCtrl : 3; //Destination cache control, reset value: 0x0, access type: RO
		uint32 endianSwapSize : 3; //Endian swap size, reset value: 0x0, access type: RO
		uint32 reserved1 : 1;
	} bitFields;
} RegWlanArmDma1Ccr2_u;

/*REG_WLAN_ARM_DMA1_LC0_2 0x44C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 loopCounterInterations : 8; //Loop counter 0 iteration count, reset value: 0x0, access type: RO
		uint32 reserved0 : 24;
	} bitFields;
} RegWlanArmDma1Lc02_u;

/*REG_WLAN_ARM_DMA1_LC1_2 0x450 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 loopCounterInterations : 8; //Loop counter 1 iteration count, reset value: 0x0, access type: RO
		uint32 reserved0 : 24;
	} bitFields;
} RegWlanArmDma1Lc12_u;

/*REG_WLAN_ARM_DMA1_SAR3 0x460 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 srcAddr : 32; //Source address for channel3, reset value: 0x0, access type: RO
	} bitFields;
} RegWlanArmDma1Sar3_u;

/*REG_WLAN_ARM_DMA1_DAR3 0x464 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dstAddr : 32; //Destination address for channel 3, reset value: 0x0, access type: RO
	} bitFields;
} RegWlanArmDma1Dar3_u;

/*REG_WLAN_ARM_DMA1_CCR3 0x468 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 1;
		uint32 srcBurstSize : 3; //Source burst size, reset value: 0x0, access type: RO
		uint32 srcBurstLen : 4; //Source burst length, reset value: 0x0, access type: RO
		uint32 srcProtCtrl : 3; //Source protection control, reset value: 0x0, access type: RO
		uint32 srcCacheCtrl : 3; //Source cache control, reset value: 0x0, access type: RO
		uint32 dstInc : 1; //Destination address increment, reset value: 0x0, access type: RO
		uint32 dstBurstSize : 3; //Destination burst size, reset value: 0x0, access type: RO
		uint32 dstBurstLen : 4; //Destination burst length, reset value: 0x0, access type: RO
		uint32 dstProtCtrl : 3; //Destination protection control, reset value: 0x0, access type: RO
		uint32 dstCacheCtrl : 3; //Destination cache control, reset value: 0x0, access type: RO
		uint32 endianSwapSize : 3; //Endian swap size, reset value: 0x0, access type: RO
		uint32 reserved1 : 1;
	} bitFields;
} RegWlanArmDma1Ccr3_u;

/*REG_WLAN_ARM_DMA1_LC0_3 0x46C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 loopCounterInterations : 8; //Loop counter 0 iteration count, reset value: 0x0, access type: RO
		uint32 reserved0 : 24;
	} bitFields;
} RegWlanArmDma1Lc03_u;

/*REG_WLAN_ARM_DMA1_LC1_3 0x470 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 loopCounterInterations : 8; //Loop counter 1 iteration count, reset value: 0x0, access type: RO
		uint32 reserved0 : 24;
	} bitFields;
} RegWlanArmDma1Lc13_u;

/*REG_WLAN_ARM_DMA1_DBGSTATUS 0xD00 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 debugStatus : 1; //Debug interface is busy, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegWlanArmDma1Dbgstatus_u;

/*REG_WLAN_ARM_DMA1_DBGCMD 0xD04 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 debugCommand : 8; //Debug command trigger, reset value: 0x0, access type: WO
		uint32 reserved0 : 24;
	} bitFields;
} RegWlanArmDma1Dbgcmd_u;

/*REG_WLAN_ARM_DMA1_DBGINST0 0xD08 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 debugThread : 1; //Debug thread, reset value: 0x0, access type: WO
		uint32 reserved0 : 7;
		uint32 channelNumber : 3; //Channel number, reset value: 0x0, access type: WO
		uint32 reserved1 : 5;
		uint32 instrByte0 : 8; //Insturction byte 0, reset value: 0x0, access type: WO
		uint32 instrByte1 : 8; //Instruction byte 1, reset value: 0x0, access type: WO
	} bitFields;
} RegWlanArmDma1Dbginst0_u;

/*REG_WLAN_ARM_DMA1_DBGINST1 0xD0C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 instrByte2 : 8; //Instruction byte 2, reset value: 0x0, access type: WO
		uint32 instrByte3 : 8; //Instruction byte 3, reset value: 0x0, access type: WO
		uint32 instrByte4 : 8; //Instruction byte 4, reset value: 0x0, access type: WO
		uint32 instrByte5 : 8; //Instruction byte 5, reset value: 0x0, access type: WO
	} bitFields;
} RegWlanArmDma1Dbginst1_u;

/*REG_WLAN_ARM_DMA1_CR0 0xE00 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 periphRequest : 1; //DMA support peripheral request, reset value: 0x1, access type: RO
		uint32 bootEn : 1; //Manager thread boot enable, reset value: 0x0, access type: RO
		uint32 mgrNsAtRst : 1; //Manager configured to non-secure mode at reset, reset value: 0x1, access type: RO
		uint32 reserved0 : 1;
		uint32 numChnls : 3; //Number of channnels, reset value: 0x3, access type: RO
		uint32 reserved1 : 5;
		uint32 numPeriphReq : 5; //Number of peripheral request interfaces that the DMAC provides, reset value: 0x3, access type: RO
		uint32 numEvents : 5; //Number of interrupt outputs that the DMAC provides, reset value: 0xf, access type: RO
		uint32 reserved2 : 10;
	} bitFields;
} RegWlanArmDma1Cr0_u;

/*REG_WLAN_ARM_DMA1_CR1 0xE04 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 icacheLen : 3; //Encoded length of icache line, reset value: 0x5, access type: RO
		uint32 reserved0 : 1;
		uint32 icacheLines : 3; //Number of icache lines, reset value: 0x5, access type: RO
		uint32 reserved1 : 25;
	} bitFields;
} RegWlanArmDma1Cr1_u;

/*REG_WLAN_ARM_DMA1_CR2 0xE08 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bootAddr : 32; //Boot address when DMAC exists reset, reset value: 0x0, access type: RO
	} bitFields;
} RegWlanArmDma1Cr2_u;

/*REG_WLAN_ARM_DMA1_CR3 0xE0C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ins : 16; //Security state of an event-interrupt resource, reset value: 0xffff, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegWlanArmDma1Cr3_u;

/*REG_WLAN_ARM_DMA1_CR4 0xE10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pns : 16; //Security state of the peripheral request interfaces, reset value: 0xf, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegWlanArmDma1Cr4_u;

/*REG_WLAN_ARM_DMA1_CRD 0xE14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dataWidth : 3; //Data width encoding, reset value: 0x4, access type: RO
		uint32 reserved0 : 1;
		uint32 wrCap : 3; //Write issuing capability, reset value: 0x3, access type: RO
		uint32 reserved1 : 1;
		uint32 wrQDep : 4; //Depth of write queue, reset value: 0xb, access type: RO
		uint32 rdCap : 3; //Read issuing capability, reset value: 0x7, access type: RO
		uint32 reserved2 : 1;
		uint32 rdQDep : 4; //Depth of read queue, reset value: 0xb, access type: RO
		uint32 dataBufferDep : 10; //Number of lines that the data buffer contains, reset value: 0xff, access type: RO
		uint32 reserved3 : 2;
	} bitFields;
} RegWlanArmDma1Crd_u;

/*REG_WLAN_ARM_DMA1_WD 0xE80 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 wdIrqOnly : 1; //Set IRQ abort when DMAC detects lock, reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegWlanArmDma1Wd_u;

/*REG_WLAN_ARM_DMA1_PERIPH_ID_0 0xFE0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 partNumber0 : 8; //DMA part number (lower 8 bits), reset value: 0x30, access type: RO
		uint32 reserved0 : 24;
	} bitFields;
} RegWlanArmDma1PeriphId0_u;

/*REG_WLAN_ARM_DMA1_PERIPH_ID_1 0xFE4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 partNumber1 : 4; //DMA part number (upper 4 bits), reset value: 0x3, access type: RO
		uint32 designer0 : 4; //Designer code (lower 4 bits), reset value: 0x1, access type: RO
		uint32 reserved0 : 24;
	} bitFields;
} RegWlanArmDma1PeriphId1_u;

/*REG_WLAN_ARM_DMA1_PERIPH_ID_2 0xFE8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 designer1 : 4; //Designer code (upper 4 bits), reset value: 0x4, access type: RO
		uint32 revision : 4; //DMA revision code, reset value: 0x3, access type: RO
		uint32 reserved0 : 24;
	} bitFields;
} RegWlanArmDma1PeriphId2_u;

/*REG_WLAN_ARM_DMA1_PERIPH_ID_3 0xFEC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 integrationCfg : 1; //Integration test logic indication, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegWlanArmDma1PeriphId3_u;

/*REG_WLAN_ARM_DMA1_PCELL_ID_0 0xFF0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 id0 : 8; //PCELL ID 1, reset value: 0x0d, access type: RO
		uint32 reserved0 : 24;
	} bitFields;
} RegWlanArmDma1PcellId0_u;

/*REG_WLAN_ARM_DMA1_PCELL_ID_1 0xFF4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 id1 : 8; //no description, reset value: 0xf0, access type: RO
		uint32 reserved0 : 24;
	} bitFields;
} RegWlanArmDma1PcellId1_u;

/*REG_WLAN_ARM_DMA1_PCELL_ID_2 0xFF8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 id2 : 8; //no description, reset value: 0x05, access type: RO
		uint32 reserved0 : 24;
	} bitFields;
} RegWlanArmDma1PcellId2_u;

/*REG_WLAN_ARM_DMA1_PCELL_ID_3 0xFFC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 id3 : 8; //no description, reset value: 0xb1, access type: RO
		uint32 reserved0 : 24;
	} bitFields;
} RegWlanArmDma1PcellId3_u;



#endif // _WLAN_ARM_DMA1_REGS_H_
