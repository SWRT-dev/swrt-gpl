
/***********************************************************************************
File:				HrcMipsRegs.h
Module:				hrcMips
SOC Revision:		843
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _HRC_MIPS_REGS_H_
#define _HRC_MIPS_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define HRC_MIPS_BASE_ADDRESS                             MEMORY_MAP_UNIT_7_BASE_ADDRESS
#define	REG_HRC_MIPS_HRC_GPIO_IN                                            (HRC_MIPS_BASE_ADDRESS + 0x0)
#define	REG_HRC_MIPS_HRC_GPIO_OUT                                           (HRC_MIPS_BASE_ADDRESS + 0x4)
#define	REG_HRC_MIPS_HRC_GPIO_OUTPUT_ENABLE                                 (HRC_MIPS_BASE_ADDRESS + 0x8)
#define	REG_HRC_MIPS_HOST_SEMAPHORE                                         (HRC_MIPS_BASE_ADDRESS + 0xC)
#define	REG_HRC_MIPS_PAGING_REGISTER                                        (HRC_MIPS_BASE_ADDRESS + 0x10)
#define	REG_HRC_MIPS_FORCE_LOCAL_INTERRUPT                                  (HRC_MIPS_BASE_ADDRESS + 0x14)
#define	REG_HRC_MIPS_DMA_HST_BASE_ADDRESS                                   (HRC_MIPS_BASE_ADDRESS + 0x20)
#define	REG_HRC_MIPS_DMA_HS_CURSOR                                          (HRC_MIPS_BASE_ADDRESS + 0x24)
#define	REG_HRC_MIPS_DMA_LOCAL_ADDRESS                                      (HRC_MIPS_BASE_ADDRESS + 0x28)
#define	REG_HRC_MIPS_DMA_LENGTH                                             (HRC_MIPS_BASE_ADDRESS + 0x2C)
#define	REG_HRC_MIPS_DMA_CONFIGURATION                                      (HRC_MIPS_BASE_ADDRESS + 0x30)
#define	REG_HRC_MIPS_DMA_COMMAND                                            (HRC_MIPS_BASE_ADDRESS + 0x34)
#define	REG_HRC_MIPS_DMA_STATUS                                             (HRC_MIPS_BASE_ADDRESS + 0x38)
#define	REG_HRC_MIPS_REVISION_NUMBER                                        (HRC_MIPS_BASE_ADDRESS + 0x50)
#define	REG_HRC_MIPS_LOCAL_INTERRUPT_COMMAND                                (HRC_MIPS_BASE_ADDRESS + 0x60)
#define	REG_HRC_MIPS_LOCAL_SEMAPHORE                                        (HRC_MIPS_BASE_ADDRESS + 0x64)
#define	REG_HRC_MIPS_HEM_SRAM0_WAIT_STATES                                  (HRC_MIPS_BASE_ADDRESS + 0x68)
#define	REG_HRC_MIPS_HEM_SRAM1_WAIT_STATES                                  (HRC_MIPS_BASE_ADDRESS + 0x6C)
#define	REG_HRC_MIPS_SET_REVCLASS                                           (HRC_MIPS_BASE_ADDRESS + 0x80)
#define	REG_HRC_MIPS_SET_STATBIST                                           (HRC_MIPS_BASE_ADDRESS + 0x84)
#define	REG_HRC_MIPS_SET_CARDBUS_CIS_POINTER                                (HRC_MIPS_BASE_ADDRESS + 0x88)
#define	REG_HRC_MIPS_SET_SUBSYSTEM_INFO                                     (HRC_MIPS_BASE_ADDRESS + 0x8C)
#define	REG_HRC_MIPS_SET_INTGNTLAT                                          (HRC_MIPS_BASE_ADDRESS + 0x90)
#define	REG_HRC_MIPS_SET_POWER_MANAGEMENT_CAPABILITIES                      (HRC_MIPS_BASE_ADDRESS + 0x94)
#define	REG_HRC_MIPS_HRC_RUNTIME_BASE_ADDRESS_REGISTER_CONFIGURATION        (HRC_MIPS_BASE_ADDRESS + 0x98)
#define	REG_HRC_MIPS_LOCAL1_BASE_ADDRESS_REGISTER_CONFIGURATION             (HRC_MIPS_BASE_ADDRESS + 0x9C)
#define	REG_HRC_MIPS_LOCAL2_BASE_ADDRESS_REGISTER_CONFIGURATION_RESERVED    (HRC_MIPS_BASE_ADDRESS + 0xA0)
#define	REG_HRC_MIPS_LOCAL3_BASE_ADDRESS_REGISTER_CONFIGURATION_RESERVED    (HRC_MIPS_BASE_ADDRESS + 0xA4)
#define	REG_HRC_MIPS_LOCAL4_BASE_ADDRESS_REGISTER_CONFIGURATION_RESERVED    (HRC_MIPS_BASE_ADDRESS + 0xA8)
#define	REG_HRC_MIPS_LOCAL5_BASE_ADDRESS_REGISTER_CONFIGURATION_RESERVED    (HRC_MIPS_BASE_ADDRESS + 0xAC)
#define	REG_HRC_MIPS_TARGET_CONFIGURATION                                   (HRC_MIPS_BASE_ADDRESS + 0xB0)
#define	REG_HRC_MIPS_INITIATOR_CONFIGURATION                                (HRC_MIPS_BASE_ADDRESS + 0xB4)
#define	REG_HRC_MIPS_TARGET_FIFO_THRESHOLD                                  (HRC_MIPS_BASE_ADDRESS + 0xB8)
#define	REG_HRC_MIPS_INITIATOR_FIFO_THRESHOLD                               (HRC_MIPS_BASE_ADDRESS + 0xBC)
#define	REG_HRC_MIPS_LOCAL_EEPROM_OVERRIDE                                  (HRC_MIPS_BASE_ADDRESS + 0xC0)
#define	REG_HRC_MIPS_EEPROM_CONTROLLER_CONFIGURATION                        (HRC_MIPS_BASE_ADDRESS + 0xC4)
#define	REG_HRC_MIPS_EEPROM_EXECUTIVE_CONFIGURATION                         (HRC_MIPS_BASE_ADDRESS + 0xC8)
#define	REG_HRC_MIPS_EEPROM_EXECUTIVE_STATUS                                (HRC_MIPS_BASE_ADDRESS + 0xCC)
#define	REG_HRC_MIPS_CONFIG_INITIALISATION_STATUS                           (HRC_MIPS_BASE_ADDRESS + 0xD0)
#define	REG_HRC_MIPS_POWER_MANAGEMENT_CONFIGURATION                         (HRC_MIPS_BASE_ADDRESS + 0xD4)
#define	REG_HRC_MIPS_POWER_MANAGEMENT_CONTROL                               (HRC_MIPS_BASE_ADDRESS + 0xD8)
#define	REG_HRC_MIPS_SET_DEVICE_INFO                                        (HRC_MIPS_BASE_ADDRESS + 0xDC)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_HRC_MIPS_HRC_GPIO_IN 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hrcGpioIn:4;	// HOST  hrc gpio in
		uint32 reserved0:28;
	} bitFields;
} RegHrcMipsHrcGpioIn_u;

/*REG_HRC_MIPS_HRC_GPIO_OUT 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hrcGpioOut:4;	// HOST  hrc gpio out
		uint32 reserved0:28;
	} bitFields;
} RegHrcMipsHrcGpioOut_u;

/*REG_HRC_MIPS_HRC_GPIO_OUTPUT_ENABLE 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hrcGpioOutputEnable:4;	// HOST  hrc gpio output enable
		uint32 reserved0:28;
	} bitFields;
} RegHrcMipsHrcGpioOutputEnable_u;

/*REG_HRC_MIPS_HOST_SEMAPHORE 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hostSemaphore:8;	// HOST  host semaphore
		uint32 reserved0:24;
	} bitFields;
} RegHrcMipsHostSemaphore_u;

/*REG_HRC_MIPS_PAGING_REGISTER 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pagingRegister:32;	// HOST  paging register
	} bitFields;
} RegHrcMipsPagingRegister_u;

/*REG_HRC_MIPS_FORCE_LOCAL_INTERRUPT 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 forceLocalInterrupt:7;	// HOST  force local interrupt
		uint32 reserved0:25;
	} bitFields;
} RegHrcMipsForceLocalInterrupt_u;

/*REG_HRC_MIPS_DMA_HST_BASE_ADDRESS 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dmaHstBaseAddress:32;	// COMMON  dma hst base address
	} bitFields;
} RegHrcMipsDmaHstBaseAddress_u;

/*REG_HRC_MIPS_DMA_HS_CURSOR 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dmaHsCursor:16;	// COMMON  dma hs cursor
		uint32 hstIndex:8;	// COMMON  hst index
		uint32 reserved0:8;
	} bitFields;
} RegHrcMipsDmaHsCursor_u;

/*REG_HRC_MIPS_DMA_LOCAL_ADDRESS 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dmaLocalAddress:32;	// COMMON  dma local address
	} bitFields;
} RegHrcMipsDmaLocalAddress_u;

/*REG_HRC_MIPS_DMA_LENGTH 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dmaLength:16;	// COMMON  dma length
		uint32 reserved0:16;
	} bitFields;
} RegHrcMipsDmaLength_u;

/*REG_HRC_MIPS_DMA_CONFIGURATION 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dmaConfiguration:1;	// COMMON  dma configuration
		uint32 bypassHst:1;	// COMMON  bypass hst
		uint32 reserved0:30;
	} bitFields;
} RegHrcMipsDmaConfiguration_u;

/*REG_HRC_MIPS_DMA_COMMAND 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dmaCommand:1;	// COMMON  dma command
		uint32 localStart:1;	// COMMON  local start
		uint32 pciWnr:1;	// COMMON  pci wnr
		uint32 reserved0:29;
	} bitFields;
} RegHrcMipsDmaCommand_u;

/*REG_HRC_MIPS_DMA_STATUS 0x38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dmaStatus:1;	// COMMON  dma status
		uint32 localBusy:1;	// COMMON  local busy
		uint32 reserved0:30;
	} bitFields;
} RegHrcMipsDmaStatus_u;

/*REG_HRC_MIPS_REVISION_NUMBER 0x50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 revisionNumber:12;	// COMMON  revision number
		uint32 reserved0:20;
	} bitFields;
} RegHrcMipsRevisionNumber_u;

/*REG_HRC_MIPS_LOCAL_INTERRUPT_COMMAND 0x60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 localInterruptCommand:1;	// LOCAL  local interrupt command
		uint32 reserved0:31;
	} bitFields;
} RegHrcMipsLocalInterruptCommand_u;

/*REG_HRC_MIPS_LOCAL_SEMAPHORE 0x64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 localSemaphore:8;	// LOCAL  local semaphore
		uint32 reserved0:24;
	} bitFields;
} RegHrcMipsLocalSemaphore_u;

/*REG_HRC_MIPS_HEM_SRAM0_WAIT_STATES 0x68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 setupSram0WaitStates:4;	// LOCAL  setup
		uint32 activeSram0WaitStates:4;	// LOCAL  active
		uint32 holdSram0WaitStates:4;	// LOCAL  hold
		uint32 extWaitSram0WaitStates:1;	// LOCAL  ext wait
		uint32 reserved0:19;
	} bitFields;
} RegHrcMipsHemSram0WaitStates_u;

/*REG_HRC_MIPS_HEM_SRAM1_WAIT_STATES 0x6C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 setupSram1WaitStates:4;	// LOCAL  setup
		uint32 activeSram1WaitStates:4;	// LOCAL  active
		uint32 holdSram1WaitStates:4;	// LOCAL  hold
		uint32 extWaitSram1WaitStates:1;	// LOCAL  ext wait
		uint32 reserved0:19;
	} bitFields;
} RegHrcMipsHemSram1WaitStates_u;

/*REG_HRC_MIPS_SET_REVCLASS 0x80 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 setRevclass:8;	// PCI_CONFIGURATION  set revclass
		uint32 classCode:24;	// PCI_CONFIGURATION  class code
	} bitFields;
} RegHrcMipsSetRevclass_u;

/*REG_HRC_MIPS_SET_STATBIST 0x84 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 setStatbist:1;	// PCI_CONFIGURATION  set statbist
		uint32 status66MhzCapable:1;	// PCI_CONFIGURATION  status 66 mhz capable
		uint32 statusUdfSupport:1;	// PCI_CONFIGURATION  status udf support
		uint32 statusFastBackToBackCapable:1;	// PCI_CONFIGURATION  status fast back to back capable
		uint32 softResetEnable:1;	// PCI_CONFIGURATION  soft reset enable
		uint32 reserved0:3;
		uint32 bistCompletionCode:4;	// PCI_CONFIGURATION  bist completion code
		uint32 bistCapable:1;	// PCI_CONFIGURATION  bist capable
		uint32 bistComplete:1;	// PCI_CONFIGURATION  bist complete
		uint32 reserved1:18;
	} bitFields;
} RegHrcMipsSetStatbist_u;

/*REG_HRC_MIPS_SET_CARDBUS_CIS_POINTER 0x88 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 setCardbusCisPointer:3;	// PCI_CONFIGURATION  set cardbus cis pointer
		uint32 addressSpaceOffset:29;	// PCI_CONFIGURATION  address space offset
	} bitFields;
} RegHrcMipsSetCardbusCisPointer_u;

/*REG_HRC_MIPS_SET_SUBSYSTEM_INFO 0x8C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 setSubsystemInfo:16;	// PCI_CONFIGURATION  set subsystem info
		uint32 subsystemId:16;	// PCI_CONFIGURATION  subsystem id
	} bitFields;
} RegHrcMipsSetSubsystemInfo_u;

/*REG_HRC_MIPS_SET_INTGNTLAT 0x90 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 setIntgntlat:8;	// PCI_CONFIGURATION  set intgntlat
		uint32 maxLat:8;	// PCI_CONFIGURATION  max lat
		uint32 reserved0:16;
	} bitFields;
} RegHrcMipsSetIntgntlat_u;

/*REG_HRC_MIPS_SET_POWER_MANAGEMENT_CAPABILITIES 0x94 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 setPowerManagementCapabilities:16;	// PCI_CONFIGURATION  set power management capabilities
		uint32 reserved0:16;
	} bitFields;
} RegHrcMipsSetPowerManagementCapabilities_u;

/*REG_HRC_MIPS_HRC_RUNTIME_BASE_ADDRESS_REGISTER_CONFIGURATION 0x98 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hrcRuntimeBaseAddressRegisterConfiguration:1;	// PCI_CONFIGURATION  hrc runtime base address register configuration
		uint32 memorySpaceTypeRuntimeBase:2;	// PCI_CONFIGURATION  memory space type
		uint32 prefetchableRuntimeBase:1;	// PCI_CONFIGURATION  prefetchable
		uint32 windowSizeRuntimeBase:28;	// PCI_CONFIGURATION  window size
	} bitFields;
} RegHrcMipsHrcRuntimeBaseAddressRegisterConfiguration_u;

/*REG_HRC_MIPS_LOCAL1_BASE_ADDRESS_REGISTER_CONFIGURATION 0x9C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 local1BaseAddressRegisterConfiguration:1;	// PCI_CONFIGURATION  local1 base address register configuration
		uint32 memorySpaceTypeLocal1Base:2;	// PCI_CONFIGURATION  memory space type
		uint32 prefetchableLocal1Base:1;	// PCI_CONFIGURATION  prefetchable
		uint32 windowSizeLocal1Base:28;	// PCI_CONFIGURATION  window size
	} bitFields;
} RegHrcMipsLocal1BaseAddressRegisterConfiguration_u;

/*REG_HRC_MIPS_LOCAL2_BASE_ADDRESS_REGISTER_CONFIGURATION_RESERVED 0xA0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 local2BaseAddressRegisterConfigurationReserved:32;	// PCI_CONFIGURATION  local2 base address register configuration reserved
	} bitFields;
} RegHrcMipsLocal2BaseAddressRegisterConfigurationReserved_u;

/*REG_HRC_MIPS_LOCAL3_BASE_ADDRESS_REGISTER_CONFIGURATION_RESERVED 0xA4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 local3BaseAddressRegisterConfigurationReserved:32;	// PCI_CONFIGURATION  local3 base address register configuration reserved
	} bitFields;
} RegHrcMipsLocal3BaseAddressRegisterConfigurationReserved_u;

/*REG_HRC_MIPS_LOCAL4_BASE_ADDRESS_REGISTER_CONFIGURATION_RESERVED 0xA8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 local4BaseAddressRegisterConfigurationReserved:32;	// PCI_CONFIGURATION  local4 base address register configuration reserved
	} bitFields;
} RegHrcMipsLocal4BaseAddressRegisterConfigurationReserved_u;

/*REG_HRC_MIPS_LOCAL5_BASE_ADDRESS_REGISTER_CONFIGURATION_RESERVED 0xAC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 local5BaseAddressRegisterConfigurationReserved:32;	// PCI_CONFIGURATION  local5 base address register configuration reserved
	} bitFields;
} RegHrcMipsLocal5BaseAddressRegisterConfigurationReserved_u;

/*REG_HRC_MIPS_TARGET_CONFIGURATION 0xB0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 targetConfiguration:1;	// PCI_CONFIGURATION  target configuration
		uint32 initialLatencyRetryEnable:1;	// PCI_CONFIGURATION  initial latency retry enable
		uint32 subsequentLatencyRetryEnable:1;	// PCI_CONFIGURATION  subsequent latency retry enable
		uint32 disconnectReadOnFirstDataPhase:1;	// PCI_CONFIGURATION  disconnect read on first data phase
		uint32 disconnectWriteOnFirstDataPhase:1;	// PCI_CONFIGURATION  disconnect write on first data phase
		uint32 singleCycleReadDisable:1;	// PCI_CONFIGURATION  single cycle read disable
		uint32 singleCycleWriteDisable:1;	// PCI_CONFIGURATION  single cycle write disable
		uint32 targetLockEnable:1;	// PCI_CONFIGURATION  target lock enable
		uint32 readRetryDiscardOnTimeoutDisable:1;	// PCI_CONFIGURATION  read retry discard on timeout disable
		uint32 readRetryDiscardOnWriteDisable:1;	// PCI_CONFIGURATION  read retry discard on write disable
		uint32 writeRetryAbortEnable:1;	// PCI_CONFIGURATION  write retry abort enable
		uint32 continuousClockEnable:1;	// PCI_CONFIGURATION  continuous clock enable
		uint32 retryOnFifoFrameStallDisable:1;	// PCI_CONFIGURATION  retry on fifo frame stall disable
		uint32 prefetchDisable:1;	// PCI_CONFIGURATION  prefetch disable
		uint32 reserved0:18;
	} bitFields;
} RegHrcMipsTargetConfiguration_u;

/*REG_HRC_MIPS_INITIATOR_CONFIGURATION 0xB4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 initiatorConfiguration:4;	// PCI_CONFIGURATION  initiator configuration
		uint32 writeCommandCode:4;	// PCI_CONFIGURATION  write command code
		uint32 readDataPhaseLimit:2;	// PCI_CONFIGURATION  read data phase limit
		uint32 writeDataPhaseLimit:2;	// PCI_CONFIGURATION  write data phase limit
		uint32 readLengthDisable:1;	// PCI_CONFIGURATION  read length disable
		uint32 writeLengthDisable:1;	// PCI_CONFIGURATION  write length disable
		uint32 reserved0:18;
	} bitFields;
} RegHrcMipsInitiatorConfiguration_u;

/*REG_HRC_MIPS_TARGET_FIFO_THRESHOLD 0xB8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 targetFifoThreshold:5;	// PCI_CONFIGURATION  target fifo threshold
		uint32 outgoingPadTargetFifoThreshold:3;	// PCI_CONFIGURATION  outgoing pad
		uint32 incomingTargetFifoThreshold:5;	// PCI_CONFIGURATION  incoming
		uint32 reserved0:19;
	} bitFields;
} RegHrcMipsTargetFifoThreshold_u;

/*REG_HRC_MIPS_INITIATOR_FIFO_THRESHOLD 0xBC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 initiatorFifoThreshold:5;	// PCI_CONFIGURATION  initiator fifo threshold
		uint32 outgoingPadInitiatorFifoThreshold:3;	// PCI_CONFIGURATION  outgoing pad
		uint32 incomingInitiatorFifoThreshold:5;	// PCI_CONFIGURATION  incoming
		uint32 reserved0:19;
	} bitFields;
} RegHrcMipsInitiatorFifoThreshold_u;

/*REG_HRC_MIPS_LOCAL_EEPROM_OVERRIDE 0xC0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 localEepromOverride:1;	// PCI_CONFIGURATION  local eeprom override
		uint32 reserved0:3;
		uint32 clockInput:1;	// PCI_CONFIGURATION  clock input
		uint32 clockOutput:1;	// PCI_CONFIGURATION  clock output
		uint32 clockOutputEnable:1;	// PCI_CONFIGURATION  clock output enable
		uint32 reserved1:1;
		uint32 dataInput:1;	// PCI_CONFIGURATION  data input
		uint32 dataOutput:1;	// PCI_CONFIGURATION  data output
		uint32 dataOutputEnable:1;	// PCI_CONFIGURATION  data output enable
		uint32 reserved2:1;
		uint32 selectInput:1;	// PCI_CONFIGURATION  select input
		uint32 selectOutput:1;	// PCI_CONFIGURATION  select output
		uint32 selectOutputEnable:1;	// PCI_CONFIGURATION  select output enable
		uint32 reserved3:17;
	} bitFields;
} RegHrcMipsLocalEepromOverride_u;

/*REG_HRC_MIPS_EEPROM_CONTROLLER_CONFIGURATION 0xC4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 eepromControllerConfiguration:6;	// PCI_CONFIGURATION  eeprom controller configuration
		uint32 serialClockPrescale:3;	// PCI_CONFIGURATION  serial clock prescale
		uint32 totempoleEnableTbd:1;	// PCI_CONFIGURATION  totempole enable tbd
		uint32 reserved0:22;
	} bitFields;
} RegHrcMipsEepromControllerConfiguration_u;

/*REG_HRC_MIPS_EEPROM_EXECUTIVE_CONFIGURATION 0xC8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 eepromExecutiveConfiguration:1;	// PCI_CONFIGURATION  eeprom executive configuration
		uint32 cisResetLoadDisable:1;	// PCI_CONFIGURATION  cis reset load disable
		uint32 reloadFromEeprom:1;	// PCI_CONFIGURATION  reload from eeprom
		uint32 cfgregReloadDisable:1;	// PCI_CONFIGURATION  cfgreg reload disable
		uint32 cisReloadDisable:1;	// PCI_CONFIGURATION  cis reload disable
		uint32 reserved0:27;
	} bitFields;
} RegHrcMipsEepromExecutiveConfiguration_u;

/*REG_HRC_MIPS_EEPROM_EXECUTIVE_STATUS 0xCC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 eepromExecutiveStatus:1;	// PCI_CONFIGURATION  eeprom executive status
		uint32 sequenceUserBypass:1;	// PCI_CONFIGURATION  sequence user bypass
		uint32 eepromMissing:1;	// PCI_CONFIGURATION  eeprom missing
		uint32 eepromBlank:1;	// PCI_CONFIGURATION  eeprom blank
		uint32 cisSequenceUserBypass:1;	// PCI_CONFIGURATION  cis sequence user bypass
		uint32 reserved0:27;
	} bitFields;
} RegHrcMipsEepromExecutiveStatus_u;

/*REG_HRC_MIPS_CONFIG_INITIALISATION_STATUS 0xD0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 configInitialisationStatus:1;	// PCI_CONFIGURATION  config initialisation status
		uint32 disablePciTarget:1;	// PCI_CONFIGURATION  disable pci target
		uint32 reserved0:30;
	} bitFields;
} RegHrcMipsConfigInitialisationStatus_u;

/*REG_HRC_MIPS_POWER_MANAGEMENT_CONFIGURATION 0xD4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 powerManagementConfiguration:1;	// PCI_CONFIGURATION  power management configuration
		uint32 pmeActiveLevel:1;	// PCI_CONFIGURATION  pme active level
		uint32 pmeInactiveDriveEnable:1;	// PCI_CONFIGURATION  pme inactive drive enable
		uint32 reserved0:29;
	} bitFields;
} RegHrcMipsPowerManagementConfiguration_u;

/*REG_HRC_MIPS_POWER_MANAGEMENT_CONTROL 0xD8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 powerManagementControl:2;	// PCI_CONFIGURATION  power management control
		uint32 setPowerState:2;	// PCI_CONFIGURATION  set power state
		uint32 hrtAssertPme:1;	// PCI_CONFIGURATION  hrt assert pme
		uint32 pmeStatus:1;	// PCI_CONFIGURATION  pme status
		uint32 reserved0:26;
	} bitFields;
} RegHrcMipsPowerManagementControl_u;

/*REG_HRC_MIPS_SET_DEVICE_INFO 0xDC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 setDeviceInfo:16;	// PCI_CONFIGURATION  set device info
		uint32 deviceId:16;	// PCI_CONFIGURATION  device id
	} bitFields;
} RegHrcMipsSetDeviceInfo_u;



#endif // _HRC_MIPS_REGS_H_
