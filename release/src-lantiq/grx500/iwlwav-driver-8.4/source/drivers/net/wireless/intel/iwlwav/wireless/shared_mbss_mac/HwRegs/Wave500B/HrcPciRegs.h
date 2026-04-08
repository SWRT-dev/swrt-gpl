
/***********************************************************************************
File:				HrcPciRegs.h
Module:				hrcPci
SOC Revision:		843
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _HRC_PCI_REGS_H_
#define _HRC_PCI_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define HRC_PCI_BASE_ADDRESS                             MEMORY_MAP_UNIT_8_BASE_ADDRESS
#define	REG_HRC_PCI_HOST_INTERRUPT_STATUS                    (HRC_PCI_BASE_ADDRESS + 0x0)
#define	REG_HRC_PCI_DEVICE_INFO                              (HRC_PCI_BASE_ADDRESS + 0x0)
#define	REG_HRC_PCI_CMDSTAT                                  (HRC_PCI_BASE_ADDRESS + 0x4)
#define	REG_HRC_PCI_HOST_INTERRUPT_ENABLE                    (HRC_PCI_BASE_ADDRESS + 0x4)
#define	REG_HRC_PCI_HOST_INTERRUPT_ACTIVE                    (HRC_PCI_BASE_ADDRESS + 0x8)
#define	REG_HRC_PCI_REVCLASS                                 (HRC_PCI_BASE_ADDRESS + 0x8)
#define	REG_HRC_PCI_LATEBIST                                 (HRC_PCI_BASE_ADDRESS + 0xC)
#define	REG_HRC_PCI_HOST_TO_LOCAL_DOORBELL_INTERRUPT         (HRC_PCI_BASE_ADDRESS + 0xC)
#define	REG_HRC_PCI_HRC_RUNTIME_BASE_ADDRESS_REGISTER        (HRC_PCI_BASE_ADDRESS + 0x10)
#define	REG_HRC_PCI_GENERAL_PURPOSE_CONTROL                  (HRC_PCI_BASE_ADDRESS + 0x10)
#define	REG_HRC_PCI_GENERAL_PURPOSE_STATUS                   (HRC_PCI_BASE_ADDRESS + 0x14)
#define	REG_HRC_PCI_LOCAL1_BASE_ADDRESS_REGISTER             (HRC_PCI_BASE_ADDRESS + 0x14)
#define	REG_HRC_PCI_LOCAL2_BASE_ADDRESS_REGISTER_RESERVED    (HRC_PCI_BASE_ADDRESS + 0x18)
#define	REG_HRC_PCI_CPU_CONTROL                              (HRC_PCI_BASE_ADDRESS + 0x18)
#define	REG_HRC_PCI_LOCAL3_BASE_ADDRESS_REGISTER_RESERVED    (HRC_PCI_BASE_ADDRESS + 0x1C)
#define	REG_HRC_PCI_LOCAL4_BASE_ADDRESS_REGISTER_RESERVED    (HRC_PCI_BASE_ADDRESS + 0x20)
#define	REG_HRC_PCI_LOCAL5_BASE_ADDRESS_REGISTER_RESERVED    (HRC_PCI_BASE_ADDRESS + 0x24)
#define	REG_HRC_PCI_CARDBUS_CIS_POINTER                      (HRC_PCI_BASE_ADDRESS + 0x28)
#define	REG_HRC_PCI_SUBSYSTEM_INFO                           (HRC_PCI_BASE_ADDRESS + 0x2C)
#define	REG_HRC_PCI_BASE_ADDRESS_REGISTER_EXPANSION_ROM      (HRC_PCI_BASE_ADDRESS + 0x30)
#define	REG_HRC_PCI_CAPABILITIES_POINTER                     (HRC_PCI_BASE_ADDRESS + 0x34)
#define	REG_HRC_PCI_RESERVED_38                              (HRC_PCI_BASE_ADDRESS + 0x38)
#define	REG_HRC_PCI_INTGNTLAT                                (HRC_PCI_BASE_ADDRESS + 0x3C)
#define	REG_HRC_PCI_RESERVED_64                              (HRC_PCI_BASE_ADDRESS + 0x40)
#define	REG_HRC_PCI_HOST_EEPROM_OVERRIDE                     (HRC_PCI_BASE_ADDRESS + 0x44)
#define	REG_HRC_PCI_REVISION_NUMBER                          (HRC_PCI_BASE_ADDRESS + 0x50)
#define	REG_HRC_PCI_CAP_OFF_0                                (HRC_PCI_BASE_ADDRESS + 0x60)
#define	REG_HRC_PCI_CAP_OFF_1                                (HRC_PCI_BASE_ADDRESS + 0x64)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_HRC_PCI_HOST_INTERRUPT_STATUS 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hostInterruptStatus:1;	// HOST  host interrupt status
		uint32 localHrtToHostInterruptStatus:1;	// HOST  local hrt to host
		uint32 hostInitiatedDmaCompleteInterruptStatus:1;	// HOST  host initiated dma complete
		uint32 hostInitiatedDmaHstErrorInterruptStatus:1;	// HOST  host initiated dma hst error
		uint32 reserved0:28;
	} bitFields;
} RegHrcPciHostInterruptStatus_u;

/*REG_HRC_PCI_DEVICE_INFO 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 deviceInfo:16;	// CFG_HEADER  device info
		uint32 deviceIdInfo:16;	// CFG_HEADER  device id
	} bitFields;
} RegHrcPciDeviceInfo_u;

/*REG_HRC_PCI_CMDSTAT 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cmdstat:1;	// CFG_HEADER  cmdstat
		uint32 commandMemorySpace:1;	// CFG_HEADER  command memory space
		uint32 commandBusMaster:1;	// CFG_HEADER  command bus master
		uint32 commandSpecialCycle:1;	// CFG_HEADER  command special cycle
		uint32 commandMemWrInvalidateEnable:1;	// CFG_HEADER  command mem wr invalidate enable
		uint32 commandVgaPaletteSnoop:1;	// CFG_HEADER  command vga palette snoop
		uint32 commandParityErrorResponse:1;	// CFG_HEADER  command parity error response
		uint32 commandWaitCycleControl:1;	// CFG_HEADER  command wait cycle control
		uint32 commandSerrNEnable:1;	// CFG_HEADER  command serr n enable
		uint32 commandFastBackToBackEnable:1;	// CFG_HEADER  command fast back to back enable
		uint32 commandInterruptDisable:1;	// CFG_HEADER  command interrupt disable
		uint32 commandReserved:5;	// CFG_HEADER  command reserved
		uint32 statusReserved:3;	// CFG_HEADER  status reserved
		uint32 statusInterruptStatus:1;	// CFG_HEADER  status interrupt status
		uint32 statusCapabilitiesList:1;	// CFG_HEADER  status capabilities list
		uint32 status66MhzCapableCmdstat:1;	// CFG_HEADER  status 66 mhz capable
		uint32 statusUdfSupportCmdstat:1;	// CFG_HEADER  status udf support
		uint32 statusFastBackToBackCapableCmdstat:1;	// CFG_HEADER  status fast back to back capable
		uint32 statusMasterDataParityErrorDetected:1;	// CFG_HEADER  status master data parity error detected
		uint32 statusDevselTiming:2;	// CFG_HEADER  status devsel timing
		uint32 statusSignaledTargetAbort:1;	// CFG_HEADER  status signaled target abort
		uint32 statusReceivedTargetAbort:1;	// CFG_HEADER  status received target abort
		uint32 statusReceivedMasterAbort:1;	// CFG_HEADER  status received master abort
		uint32 statusSignaledSystemError:1;	// CFG_HEADER  status signaled system error
		uint32 statusDetectedParityError:1;	// CFG_HEADER  status detected parity error
	} bitFields;
} RegHrcPciCmdstat_u;

/*REG_HRC_PCI_HOST_INTERRUPT_ENABLE 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hostInterruptEnable:1;	// HOST  host interrupt enable
		uint32 localHrtToHostInterruptEnable:1;	// HOST  local hrt to host
		uint32 hostInitiatedDmaCompleteInterruptEnable:1;	// HOST  host initiated dma complete
		uint32 hostInitiatedDmaHstErrorInterruptEnable:1;	// HOST  host initiated dma hst error
		uint32 reserved0:4;
		uint32 global_interruptEnable:1;	// HOST  global
		uint32 reserved1:23;
	} bitFields;
} RegHrcPciHostInterruptEnable_u;

/*REG_HRC_PCI_HOST_INTERRUPT_ACTIVE 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hostInterruptActive:1;	// HOST  host interrupt active
		uint32 localHrtToHostInterruptActive:1;	// HOST  local hrt to host
		uint32 hostInitiatedDmaCompleteInterruptActive:1;	// HOST  host initiated dma complete
		uint32 hostInitiatedDmaHstErrorInterruptActive:1;	// HOST  host initiated dma hst error
		uint32 reserved0:4;
		uint32 globalInterruptActive:1;	// HOST  global
		uint32 reserved1:23;
	} bitFields;
} RegHrcPciHostInterruptActive_u;

/*REG_HRC_PCI_REVCLASS 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 revclass:8;	// CFG_HEADER  revclass
		uint32 classCodeRevclass:24;	// CFG_HEADER  class code
	} bitFields;
} RegHrcPciRevclass_u;

/*REG_HRC_PCI_LATEBIST 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 latebist:8;	// CFG_HEADER  latebist
		uint32 latencyTimer:8;	// CFG_HEADER  latency timer
		uint32 headerType:8;	// CFG_HEADER  header type
		uint32 bistCompletionCodeLatebist:4;	// CFG_HEADER  bist completion code
		uint32 bistReserved:2;	// CFG_HEADER  bist reserved
		uint32 bistStart:1;	// CFG_HEADER  bist start
		uint32 bistCapableLatebist:1;	// CFG_HEADER  bist capable
	} bitFields;
} RegHrcPciLatebist_u;

/*REG_HRC_PCI_HOST_TO_LOCAL_DOORBELL_INTERRUPT 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hostToLocalDoorbellInterrupt:4;	// HOST  host to local doorbell interrupt
		uint32 reserved0:28;
	} bitFields;
} RegHrcPciHostToLocalDoorbellInterrupt_u;

/*REG_HRC_PCI_HRC_RUNTIME_BASE_ADDRESS_REGISTER 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hrcRuntimeBaseAddressRegister:32;	// CFG_HEADER  hrc runtime base address register
	} bitFields;
} RegHrcPciHrcRuntimeBaseAddressRegister_u;

/*REG_HRC_PCI_GENERAL_PURPOSE_CONTROL 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 generalPurposeControl:8;	// HOST  general purpose control
		uint32 reserved0:24;
	} bitFields;
} RegHrcPciGeneralPurposeControl_u;

/*REG_HRC_PCI_GENERAL_PURPOSE_STATUS 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 generalPurposeStatus:8;	// HOST  general purpose status
		uint32 reserved0:24;
	} bitFields;
} RegHrcPciGeneralPurposeStatus_u;

/*REG_HRC_PCI_LOCAL1_BASE_ADDRESS_REGISTER 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 local1BaseAddressRegister:32;	// CFG_HEADER  local1 base address register
	} bitFields;
} RegHrcPciLocal1BaseAddressRegister_u;

/*REG_HRC_PCI_LOCAL2_BASE_ADDRESS_REGISTER_RESERVED 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 local2BaseAddressRegisterReserved:32;	// CFG_HEADER  local2 base address register reserved
	} bitFields;
} RegHrcPciLocal2BaseAddressRegisterReserved_u;

/*REG_HRC_PCI_CPU_CONTROL 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cpuControl:1;	// HOST  cpu control
		uint32 upperCpuReset:1;	// HOST  upper cpu reset
		uint32 lowerCpuReset:1;	// HOST  lower cpu reset
		uint32 upperCpuBootLocation:1;	// HOST  upper cpu boot location
		uint32 lowerCpuBootLocation:1;	// HOST  lower cpu boot location
		uint32 disableInterCpuReset:1;	// HOST  disable inter cpu reset
		uint32 reserved0:26;
	} bitFields;
} RegHrcPciCpuControl_u;

/*REG_HRC_PCI_LOCAL3_BASE_ADDRESS_REGISTER_RESERVED 0x1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 local3BaseAddressRegisterReserved:32;	// CFG_HEADER  local3 base address register reserved
	} bitFields;
} RegHrcPciLocal3BaseAddressRegisterReserved_u;

/*REG_HRC_PCI_LOCAL4_BASE_ADDRESS_REGISTER_RESERVED 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 local4BaseAddressRegisterReserved:32;	// CFG_HEADER  local4 base address register reserved
	} bitFields;
} RegHrcPciLocal4BaseAddressRegisterReserved_u;

/*REG_HRC_PCI_LOCAL5_BASE_ADDRESS_REGISTER_RESERVED 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 local5BaseAddressRegisterReserved:32;	// CFG_HEADER  local5 base address register reserved
	} bitFields;
} RegHrcPciLocal5BaseAddressRegisterReserved_u;

/*REG_HRC_PCI_CARDBUS_CIS_POINTER 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cardbusCisPointer:3;	// CFG_HEADER  cardbus cis pointer
		uint32 addressSpaceOffsetCardbusCisPointer:29;	// CFG_HEADER  address space offset
	} bitFields;
} RegHrcPciCardbusCisPointer_u;

/*REG_HRC_PCI_SUBSYSTEM_INFO 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 subsystemInfo:16;	// CFG_HEADER  subsystem info
		uint32 subsystemIdSubsystemInfo:16;	// CFG_HEADER  subsystem id
	} bitFields;
} RegHrcPciSubsystemInfo_u;

/*REG_HRC_PCI_BASE_ADDRESS_REGISTER_EXPANSION_ROM 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 baseAddressRegisterExpansionRom:32;	// CFG_HEADER  base address register expansion rom
	} bitFields;
} RegHrcPciBaseAddressRegisterExpansionRom_u;

/*REG_HRC_PCI_CAPABILITIES_POINTER 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 capabilitiesPointer:8;	// CFG_HEADER  capabilities pointer
		uint32 reserved0:24;
	} bitFields;
} RegHrcPciCapabilitiesPointer_u;

/*REG_HRC_PCI_RESERVED_38 0x38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved38:32;	// CFG_HEADER  reserved 38
	} bitFields;
} RegHrcPciReserved38_u;

/*REG_HRC_PCI_INTGNTLAT 0x3C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 intgntlat:8;	// CFG_HEADER  intgntlat
		uint32 interruptPin:8;	// CFG_HEADER  interrupt pin
		uint32 minGnt:8;	// CFG_HEADER  min gnt
		uint32 maxLatIntgntlat:8;	// CFG_HEADER  max lat
	} bitFields;
} RegHrcPciIntgntlat_u;

/*REG_HRC_PCI_RESERVED_64 0x40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved64:1;	// DEVDEP_CFG  reserved 64
		uint32 reserved0:31;
	} bitFields;
} RegHrcPciReserved64_u;

/*REG_HRC_PCI_HOST_EEPROM_OVERRIDE 0x44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hostEepromOverride:1;	// DEVDEP_CFG  host eeprom override
		uint32 reserved0:3;
		uint32 clockInputHostEepromOverride:1;	// DEVDEP_CFG  clock input
		uint32 clockOutputHostEepromOverride:1;	// DEVDEP_CFG  clock output
		uint32 clockOutputEnableHostEepromOverride:1;	// DEVDEP_CFG  clock output enable
		uint32 reserved1:1;
		uint32 dataInputHostEepromOverride:1;	// DEVDEP_CFG  data input
		uint32 dataOutputHostEepromOverride:1;	// DEVDEP_CFG  data output
		uint32 dataOutputEnable_eepromOverride:1;	// DEVDEP_CFG  data output enable
		uint32 reserved2:1;
		uint32 selectInputEepromOverride:1;	// DEVDEP_CFG  select input
		uint32 selectOutputEepromOverride:1;	// DEVDEP_CFG  select output
		uint32 selectOutputEnableEepromOverride:1;	// DEVDEP_CFG  select output enable
		uint32 reserved3:17;
	} bitFields;
} RegHrcPciHostEepromOverride_u;

/*REG_HRC_PCI_REVISION_NUMBER 0x50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 revisionNumberCfg:12;	// DEVDEP_CFG  revision number
		uint32 reserved0:20;
	} bitFields;
} RegHrcPciRevisionNumber_u;

/*REG_HRC_PCI_CAP_OFF_0 0x60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 capOff0:8;	// DEVDEP_CFG  cap off 0
		uint32 nextItemPtr:8;	// DEVDEP_CFG  next item ptr
		uint32 powerManagementCapabilities:16;	// DEVDEP_CFG  power management capabilities
	} bitFields;
} RegHrcPciCapOff0_u;

/*REG_HRC_PCI_CAP_OFF_1 0x64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 capOff1:2;	// DEVDEP_CFG  cap off 1
		uint32 reserved0702:6;	// DEVDEP_CFG  reserved0702
		uint32 pmeEn:1;	// DEVDEP_CFG  pme en
		uint32 dataSelect:4;	// DEVDEP_CFG  data select
		uint32 dataScale:2;	// DEVDEP_CFG  data scale
		uint32 pmeStatusCapOff1:1;	// DEVDEP_CFG  pme status
		uint32 pmcsrBse:8;	// DEVDEP_CFG  pmcsr bse
		uint32 data:8;	// DEVDEP_CFG  data
	} bitFields;
} RegHrcPciCapOff1_u;



#endif // _HRC_PCI_REGS_H_
