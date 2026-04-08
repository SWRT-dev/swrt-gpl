
/***********************************************************************************
File:				BeaconRegs.h
Module:				beacon
SOC Revision:		843
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _BEACON_REGS_H_
#define _BEACON_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define BEACON_BASE_ADDRESS                             MEMORY_MAP_UNIT_15_BASE_ADDRESS
#define	REG_BEACON_BEACON_LCM               (BEACON_BASE_ADDRESS + 0x0)
#define	REG_BEACON_VAP0_BEACON_ACTIVATE     (BEACON_BASE_ADDRESS + 0x4)
#define	REG_BEACON_VAP0_BEACON_OFFSET       (BEACON_BASE_ADDRESS + 0x8)
#define	REG_BEACON_VAP0_BEACON_CSA          (BEACON_BASE_ADDRESS + 0xC)
#define	REG_BEACON_VAP1_BEACON_ACTIVATE     (BEACON_BASE_ADDRESS + 0x10)
#define	REG_BEACON_VAP1_BEACON_OFFSET       (BEACON_BASE_ADDRESS + 0x14)
#define	REG_BEACON_VAP1_BEACON_CSA          (BEACON_BASE_ADDRESS + 0x18)
#define	REG_BEACON_VAP2_BEACON_ACTIVATE     (BEACON_BASE_ADDRESS + 0x1C)
#define	REG_BEACON_VAP2_BEACON_OFFSET       (BEACON_BASE_ADDRESS + 0x20)
#define	REG_BEACON_VAP2_BEACON_CSA          (BEACON_BASE_ADDRESS + 0x24)
#define	REG_BEACON_VAP3_BEACON_ACTIVATE     (BEACON_BASE_ADDRESS + 0x28)
#define	REG_BEACON_VAP3_BEACON_OFFSET       (BEACON_BASE_ADDRESS + 0x2C)
#define	REG_BEACON_VAP3_BEACON_CSA          (BEACON_BASE_ADDRESS + 0x30)
#define	REG_BEACON_VAP4_BEACON_ACTIVATE     (BEACON_BASE_ADDRESS + 0x34)
#define	REG_BEACON_VAP4_BEACON_OFFSET       (BEACON_BASE_ADDRESS + 0x38)
#define	REG_BEACON_VAP4_BEACON_CSA          (BEACON_BASE_ADDRESS + 0x3C)
#define	REG_BEACON_VAP5_BEACON_ACTIVATE     (BEACON_BASE_ADDRESS + 0x40)
#define	REG_BEACON_VAP5_BEACON_OFFSET       (BEACON_BASE_ADDRESS + 0x44)
#define	REG_BEACON_VAP5_BEACON_CSA          (BEACON_BASE_ADDRESS + 0x48)
#define	REG_BEACON_VAP6_BEACON_ACTIVATE     (BEACON_BASE_ADDRESS + 0x4C)
#define	REG_BEACON_VAP6_BEACON_OFFSET       (BEACON_BASE_ADDRESS + 0x50)
#define	REG_BEACON_VAP6_BEACON_CSA          (BEACON_BASE_ADDRESS + 0x54)
#define	REG_BEACON_VAP7_BEACON_ACTIVATE     (BEACON_BASE_ADDRESS + 0x58)
#define	REG_BEACON_VAP7_BEACON_OFFSET       (BEACON_BASE_ADDRESS + 0x5C)
#define	REG_BEACON_VAP7_BEACON_CSA          (BEACON_BASE_ADDRESS + 0x60)
#define	REG_BEACON_VAP8_BEACON_ACTIVATE     (BEACON_BASE_ADDRESS + 0x64)
#define	REG_BEACON_VAP8_BEACON_OFFSET       (BEACON_BASE_ADDRESS + 0x68)
#define	REG_BEACON_VAP8_BEACON_CSA          (BEACON_BASE_ADDRESS + 0x6C)
#define	REG_BEACON_VAP9_BEACON_ACTIVATE     (BEACON_BASE_ADDRESS + 0x70)
#define	REG_BEACON_VAP9_BEACON_OFFSET       (BEACON_BASE_ADDRESS + 0x74)
#define	REG_BEACON_VAP9_BEACON_CSA          (BEACON_BASE_ADDRESS + 0x78)
#define	REG_BEACON_VAP10_BEACON_ACTIVATE    (BEACON_BASE_ADDRESS + 0x7C)
#define	REG_BEACON_VAP10_BEACON_OFFSET      (BEACON_BASE_ADDRESS + 0x80)
#define	REG_BEACON_VAP10_BEACON_CSA         (BEACON_BASE_ADDRESS + 0x84)
#define	REG_BEACON_VAP11_BEACON_ACTIVATE    (BEACON_BASE_ADDRESS + 0x88)
#define	REG_BEACON_VAP11_BEACON_OFFSET      (BEACON_BASE_ADDRESS + 0x8C)
#define	REG_BEACON_VAP11_BEACON_CSA         (BEACON_BASE_ADDRESS + 0x90)
#define	REG_BEACON_VAP12_BEACON_ACTIVATE    (BEACON_BASE_ADDRESS + 0x94)
#define	REG_BEACON_VAP12_BEACON_OFFSET      (BEACON_BASE_ADDRESS + 0x98)
#define	REG_BEACON_VAP12_BEACON_CSA         (BEACON_BASE_ADDRESS + 0x9C)
#define	REG_BEACON_VAP13_BEACON_ACTIVATE    (BEACON_BASE_ADDRESS + 0xA0)
#define	REG_BEACON_VAP13_BEACON_OFFSET      (BEACON_BASE_ADDRESS + 0xA4)
#define	REG_BEACON_VAP13_BEACON_CSA         (BEACON_BASE_ADDRESS + 0xA8)
#define	REG_BEACON_VAP14_BEACON_ACTIVATE    (BEACON_BASE_ADDRESS + 0xAC)
#define	REG_BEACON_VAP14_BEACON_OFFSET      (BEACON_BASE_ADDRESS + 0xB0)
#define	REG_BEACON_VAP14_BEACON_CSA         (BEACON_BASE_ADDRESS + 0xB4)
#define	REG_BEACON_VAP15_BEACON_ACTIVATE    (BEACON_BASE_ADDRESS + 0xB8)
#define	REG_BEACON_VAP15_BEACON_OFFSET      (BEACON_BASE_ADDRESS + 0xBC)
#define	REG_BEACON_VAP15_BEACON_CSA         (BEACON_BASE_ADDRESS + 0xC0)
#define	REG_BEACON_BEACON_CSA_GO            (BEACON_BASE_ADDRESS + 0xC4)
#define	REG_BEACON_VAP_ACTIVE_STATUS        (BEACON_BASE_ADDRESS + 0xC8)
#define	REG_BEACON_BEACON_COUNTER           (BEACON_BASE_ADDRESS + 0xCC)
#define	REG_BEACON_VERIFICATION_MODE        (BEACON_BASE_ADDRESS + 0xD0)
#define	REG_BEACON_BEACON_LOGGER_CFG        (BEACON_BASE_ADDRESS + 0xD4)
#define	REG_BEACON_BEACON_LOGGER_BUSY       (BEACON_BASE_ADDRESS + 0xD8)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_BEACON_BEACON_LCM 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lcm:16;	// Greatest common division
		uint32 reserved0:16;
	} bitFields;
} RegBeaconBeaconLcm_u;

/*REG_BEACON_VAP0_BEACON_ACTIVATE 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vap0BeaconEn:1;	// Beacon enable bit
		uint32 reserved0:7;
		uint32 vap0BeaconInterval:16;	// Beacon interval in TUs 
		uint32 vap0DtimInterval:8;	// DTIM interval in TIM units
	} bitFields;
} RegBeaconVap0BeaconActivate_u;

/*REG_BEACON_VAP0_BEACON_OFFSET 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vap0BeaconOffset:16;	// Beacon offset relative to virtual zero point
		uint32 reserved0:16;
	} bitFields;
} RegBeaconVap0BeaconOffset_u;

/*REG_BEACON_VAP0_BEACON_CSA 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vap0CsaCountLimit:8;	// Channel Switch Announcement Beacon count down init value
		uint32 vap0CsaEn:1;	// Counter enable
		uint32 reserved0:7;
		uint32 vap0CsaCounter:8;	// Current Channel Switch Announcement count
		uint32 reserved1:8;
	} bitFields;
} RegBeaconVap0BeaconCsa_u;

/*REG_BEACON_VAP1_BEACON_ACTIVATE 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vap1BeaconEn:1;	// Beacon enable bit
		uint32 reserved0:7;
		uint32 vap1BeaconInterval:16;	// Beacon interval in TUs 
		uint32 vap1DtimInterval:8;	// DTIM interval in TIM units
	} bitFields;
} RegBeaconVap1BeaconActivate_u;

/*REG_BEACON_VAP1_BEACON_OFFSET 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vap1BeaconOffset:16;	// Beacon offset relative to virtual zero point
		uint32 reserved0:16;
	} bitFields;
} RegBeaconVap1BeaconOffset_u;

/*REG_BEACON_VAP1_BEACON_CSA 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vap1CsaCountLimit:8;	// Channel Switch Announcement Beacon count down init value
		uint32 vap1CsaEn:1;	// Counter enable
		uint32 reserved0:7;
		uint32 vap1CsaCounter:8;	// Current Channel Switch Announcement count
		uint32 reserved1:8;
	} bitFields;
} RegBeaconVap1BeaconCsa_u;

/*REG_BEACON_VAP2_BEACON_ACTIVATE 0x1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vap2BeaconEn:1;	// Beacon enable bit
		uint32 reserved0:7;
		uint32 vap2BeaconInterval:16;	// Beacon interval in TUs 
		uint32 vap2DtimInterval:8;	// DTIM interval in TIM units
	} bitFields;
} RegBeaconVap2BeaconActivate_u;

/*REG_BEACON_VAP2_BEACON_OFFSET 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vap2BeaconOffset:16;	// Beacon offset relative to virtual zero point
		uint32 reserved0:16;
	} bitFields;
} RegBeaconVap2BeaconOffset_u;

/*REG_BEACON_VAP2_BEACON_CSA 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vap2CsaCountLimit:8;	// Channel Switch Announcement Beacon count down init value
		uint32 vap2CsaEn:1;	// Counter enable
		uint32 reserved0:7;
		uint32 vap2CsaCounter:8;	// Current Channel Switch Announcement count
		uint32 reserved1:8;
	} bitFields;
} RegBeaconVap2BeaconCsa_u;

/*REG_BEACON_VAP3_BEACON_ACTIVATE 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vap3BeaconEn:1;	// Beacon enable bit
		uint32 reserved0:7;
		uint32 vap3BeaconInterval:16;	// Beacon interval in TUs 
		uint32 vap3DtimInterval:8;	// DTIM interval in TIM units
	} bitFields;
} RegBeaconVap3BeaconActivate_u;

/*REG_BEACON_VAP3_BEACON_OFFSET 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vap3BeaconOffset:16;	// Beacon offset relative to virtual zero point
		uint32 reserved0:16;
	} bitFields;
} RegBeaconVap3BeaconOffset_u;

/*REG_BEACON_VAP3_BEACON_CSA 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vap3CsaCountLimit:8;	// Channel Switch Announcement Beacon count down init value
		uint32 vap3CsaEn:1;	// Counter enable
		uint32 reserved0:7;
		uint32 vap3CsaCounter:8;	// Current Channel Switch Announcement count
		uint32 reserved1:8;
	} bitFields;
} RegBeaconVap3BeaconCsa_u;

/*REG_BEACON_VAP4_BEACON_ACTIVATE 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vap4BeaconEn:1;	// Beacon enable bit
		uint32 reserved0:7;
		uint32 vap4BeaconInterval:16;	// Beacon interval in TUs 
		uint32 vap4DtimInterval:8;	// DTIM interval in TIM units
	} bitFields;
} RegBeaconVap4BeaconActivate_u;

/*REG_BEACON_VAP4_BEACON_OFFSET 0x38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vap4BeaconOffset:16;	// Beacon offset relative to virtual zero point
		uint32 reserved0:16;
	} bitFields;
} RegBeaconVap4BeaconOffset_u;

/*REG_BEACON_VAP4_BEACON_CSA 0x3C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vap4CsaCountLimit:8;	// Channel Switch Announcement Beacon count down init value
		uint32 vap4CsaEn:1;	// Counter enable
		uint32 reserved0:7;
		uint32 vap4CsaCounter:8;	// Current Channel Switch Announcement count
		uint32 reserved1:8;
	} bitFields;
} RegBeaconVap4BeaconCsa_u;

/*REG_BEACON_VAP5_BEACON_ACTIVATE 0x40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vap5BeaconEn:1;	// Beacon enable bit
		uint32 reserved0:7;
		uint32 vap5BeaconInterval:16;	// Beacon interval in TUs 
		uint32 vap5DtimInterval:8;	// DTIM interval in TIM units
	} bitFields;
} RegBeaconVap5BeaconActivate_u;

/*REG_BEACON_VAP5_BEACON_OFFSET 0x44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vap5BeaconOffset:16;	// Beacon offset relative to virtual zero point
		uint32 reserved0:16;
	} bitFields;
} RegBeaconVap5BeaconOffset_u;

/*REG_BEACON_VAP5_BEACON_CSA 0x48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vap5CsaCountLimit:8;	// Channel Switch Announcement Beacon count down init value
		uint32 vap5CsaEn:1;	// Counter enable
		uint32 reserved0:7;
		uint32 vap5CsaCounter:8;	// Current Channel Switch Announcement count
		uint32 reserved1:8;
	} bitFields;
} RegBeaconVap5BeaconCsa_u;

/*REG_BEACON_VAP6_BEACON_ACTIVATE 0x4C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vap6BeaconEn:1;	// Beacon enable bit
		uint32 reserved0:7;
		uint32 vap6BeaconInterval:16;	// Beacon interval in TUs 
		uint32 vap6DtimInterval:8;	// DTIM interval in TIM units
	} bitFields;
} RegBeaconVap6BeaconActivate_u;

/*REG_BEACON_VAP6_BEACON_OFFSET 0x50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vap6BeaconOffset:16;	// Beacon offset relative to virtual zero point
		uint32 reserved0:16;
	} bitFields;
} RegBeaconVap6BeaconOffset_u;

/*REG_BEACON_VAP6_BEACON_CSA 0x54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vap6CsaCountLimit:8;	// Channel Switch Announcement Beacon count down init value
		uint32 vap6CsaEn:1;	// Counter enable
		uint32 reserved0:7;
		uint32 vap6CsaCounter:8;	// Current Channel Switch Announcement count
		uint32 reserved1:8;
	} bitFields;
} RegBeaconVap6BeaconCsa_u;

/*REG_BEACON_VAP7_BEACON_ACTIVATE 0x58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vap7BeaconEn:1;	// Beacon enable bit
		uint32 reserved0:7;
		uint32 vap7BeaconInterval:16;	// Beacon interval in TUs 
		uint32 vap7DtimInterval:8;	// DTIM interval in TIM units
	} bitFields;
} RegBeaconVap7BeaconActivate_u;

/*REG_BEACON_VAP7_BEACON_OFFSET 0x5C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vap7BeaconOffset:16;	// Beacon offset relative to virtual zero point
		uint32 reserved0:16;
	} bitFields;
} RegBeaconVap7BeaconOffset_u;

/*REG_BEACON_VAP7_BEACON_CSA 0x60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vap7CsaCountLimit:8;	// Channel Switch Announcement Beacon count down init value
		uint32 vap7CsaEn:1;	// Counter enable
		uint32 reserved0:7;
		uint32 vap7CsaCounter:8;	// Current Channel Switch Announcement count
		uint32 reserved1:8;
	} bitFields;
} RegBeaconVap7BeaconCsa_u;

/*REG_BEACON_VAP8_BEACON_ACTIVATE 0x64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vap8BeaconEn:1;	// Beacon enable bit
		uint32 reserved0:7;
		uint32 vap8BeaconInterval:16;	// Beacon interval in TUs 
		uint32 vap8DtimInterval:8;	// DTIM interval in TIM units
	} bitFields;
} RegBeaconVap8BeaconActivate_u;

/*REG_BEACON_VAP8_BEACON_OFFSET 0x68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vap8BeaconOffset:16;	// Beacon offset relative to virtual zero point
		uint32 reserved0:16;
	} bitFields;
} RegBeaconVap8BeaconOffset_u;

/*REG_BEACON_VAP8_BEACON_CSA 0x6C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vap8CsaCountLimit:8;	// Channel Switch Announcement Beacon count down init value
		uint32 vap8CsaEn:1;	// Counter enable
		uint32 reserved0:7;
		uint32 vap8CsaCounter:8;	// Current Channel Switch Announcement count
		uint32 reserved1:8;
	} bitFields;
} RegBeaconVap8BeaconCsa_u;

/*REG_BEACON_VAP9_BEACON_ACTIVATE 0x70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vap9BeaconEn:1;	// Beacon enable bit
		uint32 reserved0:7;
		uint32 vap9BeaconInterval:16;	// Beacon interval in TUs 
		uint32 vap9DtimInterval:8;	// DTIM interval in TIM units
	} bitFields;
} RegBeaconVap9BeaconActivate_u;

/*REG_BEACON_VAP9_BEACON_OFFSET 0x74 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vap9BeaconOffset:16;	// Beacon offset relative to virtual zero point
		uint32 reserved0:16;
	} bitFields;
} RegBeaconVap9BeaconOffset_u;

/*REG_BEACON_VAP9_BEACON_CSA 0x78 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vap9CsaCountLimit:8;	// Channel Switch Announcement Beacon count down init value
		uint32 vap9CsaEn:1;	// Counter enable
		uint32 reserved0:7;
		uint32 vap9CsaCounter:8;	// Current Channel Switch Announcement count
		uint32 reserved1:8;
	} bitFields;
} RegBeaconVap9BeaconCsa_u;

/*REG_BEACON_VAP10_BEACON_ACTIVATE 0x7C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vap10BeaconEn:1;	// Beacon enable bit
		uint32 reserved0:7;
		uint32 vap10BeaconInterval:16;	// Beacon interval in TUs 
		uint32 vap10DtimInterval:8;	// DTIM interval in TIM units
	} bitFields;
} RegBeaconVap10BeaconActivate_u;

/*REG_BEACON_VAP10_BEACON_OFFSET 0x80 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vap10BeaconOffset:16;	// Beacon offset relative to virtual zero point
		uint32 reserved0:16;
	} bitFields;
} RegBeaconVap10BeaconOffset_u;

/*REG_BEACON_VAP10_BEACON_CSA 0x84 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vap10CsaCountLimit:8;	// Channel Switch Announcement Beacon count down init value
		uint32 vap10CsaEn:1;	// Counter enable
		uint32 reserved0:7;
		uint32 vap10CsaCounter:8;	// Current Channel Switch Announcement count
		uint32 reserved1:8;
	} bitFields;
} RegBeaconVap10BeaconCsa_u;

/*REG_BEACON_VAP11_BEACON_ACTIVATE 0x88 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vap11BeaconEn:1;	// Beacon enable bit
		uint32 reserved0:7;
		uint32 vap11BeaconInterval:16;	// Beacon interval in TUs 
		uint32 vap11DtimInterval:8;	// DTIM interval in TIM units
	} bitFields;
} RegBeaconVap11BeaconActivate_u;

/*REG_BEACON_VAP11_BEACON_OFFSET 0x8C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vap11BeaconOffset:16;	// Beacon offset relative to virtual zero point
		uint32 reserved0:16;
	} bitFields;
} RegBeaconVap11BeaconOffset_u;

/*REG_BEACON_VAP11_BEACON_CSA 0x90 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vap11CsaCountLimit:8;	// Channel Switch Announcement Beacon count down init value
		uint32 vap11CsaEn:1;	// Counter enable
		uint32 reserved0:7;
		uint32 vap11CsaCounter:8;	// Current Channel Switch Announcement count
		uint32 reserved1:8;
	} bitFields;
} RegBeaconVap11BeaconCsa_u;

/*REG_BEACON_VAP12_BEACON_ACTIVATE 0x94 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vap12BeaconEn:1;	// Beacon enable bit
		uint32 reserved0:7;
		uint32 vap12BeaconInterval:16;	// Beacon interval in TUs 
		uint32 vap12DtimInterval:8;	// DTIM interval in TIM units
	} bitFields;
} RegBeaconVap12BeaconActivate_u;

/*REG_BEACON_VAP12_BEACON_OFFSET 0x98 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vap12BeaconOffset:16;	// Beacon offset relative to virtual zero point
		uint32 reserved0:16;
	} bitFields;
} RegBeaconVap12BeaconOffset_u;

/*REG_BEACON_VAP12_BEACON_CSA 0x9C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vap12CsaCountLimit:8;	// Channel Switch Announcement Beacon count down init value
		uint32 vap12CsaEn:1;	// Counter enable
		uint32 reserved0:7;
		uint32 vap12CsaCounter:8;	// Current Channel Switch Announcement count
		uint32 reserved1:8;
	} bitFields;
} RegBeaconVap12BeaconCsa_u;

/*REG_BEACON_VAP13_BEACON_ACTIVATE 0xA0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vap13BeaconEn:1;	// Beacon enable bit
		uint32 reserved0:7;
		uint32 vap13BeaconInterval:16;	// Beacon interval in TUs 
		uint32 vap13DtimInterval:8;	// DTIM interval in TIM units
	} bitFields;
} RegBeaconVap13BeaconActivate_u;

/*REG_BEACON_VAP13_BEACON_OFFSET 0xA4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vap13BeaconOffset:16;	// Beacon offset relative to virtual zero point
		uint32 reserved0:16;
	} bitFields;
} RegBeaconVap13BeaconOffset_u;

/*REG_BEACON_VAP13_BEACON_CSA 0xA8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vap13CsaCountLimit:8;	// Channel Switch Announcement Beacon count down init value
		uint32 vap13CsaEn:1;	// Counter enable
		uint32 reserved0:7;
		uint32 vap13CsaCounter:8;	// Current Channel Switch Announcement count
		uint32 reserved1:8;
	} bitFields;
} RegBeaconVap13BeaconCsa_u;

/*REG_BEACON_VAP14_BEACON_ACTIVATE 0xAC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vap14BeaconEn:1;	// Beacon enable bit
		uint32 reserved0:7;
		uint32 vap14BeaconInterval:16;	// Beacon interval in TUs 
		uint32 vap14DtimInterval:8;	// DTIM interval in TIM units
	} bitFields;
} RegBeaconVap14BeaconActivate_u;

/*REG_BEACON_VAP14_BEACON_OFFSET 0xB0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vap14BeaconOffset:16;	// Beacon offset relative to virtual zero point
		uint32 reserved0:16;
	} bitFields;
} RegBeaconVap14BeaconOffset_u;

/*REG_BEACON_VAP14_BEACON_CSA 0xB4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vap14CsaCountLimit:8;	// Channel Switch Announcement Beacon count down init value
		uint32 vap14CsaEn:1;	// Counter enable
		uint32 reserved0:7;
		uint32 vap14CsaCounter:8;	// Current Channel Switch Announcement count
		uint32 reserved1:8;
	} bitFields;
} RegBeaconVap14BeaconCsa_u;

/*REG_BEACON_VAP15_BEACON_ACTIVATE 0xB8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vap15BeaconEn:1;	// Beacon enable bit
		uint32 reserved0:7;
		uint32 vap15BeaconInterval:16;	// Beacon interval in TUs 
		uint32 vap15DtimInterval:8;	// DTIM interval in TIM units
	} bitFields;
} RegBeaconVap15BeaconActivate_u;

/*REG_BEACON_VAP15_BEACON_OFFSET 0xBC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vap15BeaconOffset:16;	// Beacon offset relative to virtual zero point
		uint32 reserved0:16;
	} bitFields;
} RegBeaconVap15BeaconOffset_u;

/*REG_BEACON_VAP15_BEACON_CSA 0xC0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vap15CsaCountLimit:8;	// Channel Switch Announcement Beacon count down init value
		uint32 vap15CsaEn:1;	// Counter enable
		uint32 reserved0:7;
		uint32 vap15CsaCounter:8;	// Current Channel Switch Announcement count
		uint32 reserved1:8;
	} bitFields;
} RegBeaconVap15BeaconCsa_u;

/*REG_BEACON_BEACON_CSA_GO 0xC4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 csaBeaconCountGo:1;	// Activate all enabled CSA counters
		uint32 reserved0:31;
	} bitFields;
} RegBeaconBeaconCsaGo_u;

/*REG_BEACON_VAP_ACTIVE_STATUS 0xC8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vapActiveStatus:16;	// List of active VAPs
		uint32 reserved0:16;
	} bitFields;
} RegBeaconVapActiveStatus_u;

/*REG_BEACON_BEACON_COUNTER 0xCC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 beaconReqCounter:16;	// number of beacons transmitted in the system
		uint32 reserved0:15;
		uint32 beaconReqCounterClr:1;	// Clears Beacon counter
	} bitFields;
} RegBeaconBeaconCounter_u;

/*REG_BEACON_VERIFICATION_MODE 0xD0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 verificationMode:1;	// Verification mode to shift tsf counter
		uint32 reserved0:31;
	} bitFields;
} RegBeaconVerificationMode_u;

/*REG_BEACON_BEACON_LOGGER_CFG 0xD4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 beaconTimersLoggerPriority:2;
		uint32 beaconTimersLoggerEn:1;
		uint32 reserved0:29;
	} bitFields;
} RegBeaconBeaconLoggerCfg_u;

/*REG_BEACON_BEACON_LOGGER_BUSY 0xD8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 beaconTimersLoggerBusy:1;
		uint32 reserved0:31;
	} bitFields;
} RegBeaconBeaconLoggerBusy_u;



#endif // _BEACON_REGS_H_
