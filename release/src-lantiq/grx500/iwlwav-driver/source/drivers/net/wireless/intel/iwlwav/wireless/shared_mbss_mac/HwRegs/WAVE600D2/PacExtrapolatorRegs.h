
/***********************************************************************************
File:				PacExtrapolatorRegs.h
Module:				pacExtrapolator
SOC Revision:		
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _PAC_EXTRAPOLATOR_REGS_H_
#define _PAC_EXTRAPOLATOR_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define PAC_EXTRAPOLATOR_BASE_ADDRESS                             MEMORY_MAP_UNIT_62_BASE_ADDRESS
#define	REG_PAC_EXTRAPOLATOR_EXTRAP_CONTROL_CFG                  (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0x0)
#define	REG_PAC_EXTRAPOLATOR_SU_STA_DB_CFG                       (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0x4)
#define	REG_PAC_EXTRAPOLATOR_MU_DB_CFG                           (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0x8)
#define	REG_PAC_EXTRAPOLATOR_MU_USER_REINIT_VEC                  (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0xC)
#define	REG_PAC_EXTRAPOLATOR_LN_BETA_LUT_BASE_ADDR               (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0x10)
#define	REG_PAC_EXTRAPOLATOR_WORD_UPD_EN_VEC                     (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0x14)
#define	REG_PAC_EXTRAPOLATOR_WORD_METHOD_VEC                     (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0x18)
#define	REG_PAC_EXTRAPOLATOR_TSF_RESOLUTION_CFG                  (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0x1C)
#define	REG_PAC_EXTRAPOLATOR_OFFSET_SU_WORD_0_3                  (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0x24)
#define	REG_PAC_EXTRAPOLATOR_OFFSET_SU_WORD_4_7                  (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0x28)
#define	REG_PAC_EXTRAPOLATOR_OFFSET_SU_WORD_8_11                 (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0x2C)
#define	REG_PAC_EXTRAPOLATOR_OFFSET_SU_WORD_12_15                (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0x30)
#define	REG_PAC_EXTRAPOLATOR_OFFSET_MU_WORD_0_3                  (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0x34)
#define	REG_PAC_EXTRAPOLATOR_OFFSET_MU_WORD_4_7                  (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0x38)
#define	REG_PAC_EXTRAPOLATOR_OFFSET_MU_WORD_8_11                 (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0x3C)
#define	REG_PAC_EXTRAPOLATOR_OFFSET_MU_WORD_12_15                (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0x40)
#define	REG_PAC_EXTRAPOLATOR_SIGN_EXT_WORD_0_5                   (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0x44)
#define	REG_PAC_EXTRAPOLATOR_SIGN_EXT_WORD_6_11                  (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0x48)
#define	REG_PAC_EXTRAPOLATOR_STA_EN_0_31                         (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0x4C)
#define	REG_PAC_EXTRAPOLATOR_STA_EN_32_63                        (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0x50)
#define	REG_PAC_EXTRAPOLATOR_STA_EN_64_95                        (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0x54)
#define	REG_PAC_EXTRAPOLATOR_STA_EN_96_127                       (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0x58)
#define	REG_PAC_EXTRAPOLATOR_STA_EN_128_159                      (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0x5C)
#define	REG_PAC_EXTRAPOLATOR_STA_EN_160_191                      (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0x60)
#define	REG_PAC_EXTRAPOLATOR_STA_EN_192_223                      (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0x64)
#define	REG_PAC_EXTRAPOLATOR_STA_EN_224_255                      (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0x68)
#define	REG_PAC_EXTRAPOLATOR_SW_MAC_MU_AF_REQ                    (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0x6C)
#define	REG_PAC_EXTRAPOLATOR_SW_MAC_MU_AF_CFG                    (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0x70)
#define	REG_PAC_EXTRAPOLATOR_SW_MAC_USER_VALID_VEC               (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0x74)
#define	REG_PAC_EXTRAPOLATOR_SW_API_ENTRY_UPDATE_REQ             (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0x78)
#define	REG_PAC_EXTRAPOLATOR_SW_API_STATUS                       (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0x7C)
#define	REG_PAC_EXTRAPOLATOR_SW_API_ENTRY_UPDATE_CFG             (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0x80)
#define	REG_PAC_EXTRAPOLATOR_SW_API_MIRROR_ADDR                  (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0x84)
#define	REG_PAC_EXTRAPOLATOR_SW_API_DB_ADDR                      (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0x88)
#define	REG_PAC_EXTRAPOLATOR_SW_API_WORD_UPD_ENA_VEC             (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0x8C)
#define	REG_PAC_EXTRAPOLATOR_SW_IMM_REQ                          (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0x90)
#define	REG_PAC_EXTRAPOLATOR_SW_IMM_CURRENT_ALFA                 (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0x94)
#define	REG_PAC_EXTRAPOLATOR_SW_IMM_CURRENT_PARAMETER            (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0x98)
#define	REG_PAC_EXTRAPOLATOR_SW_IMM_PREVIOUS_AVG_WEIGHTED        (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0x9C)
#define	REG_PAC_EXTRAPOLATOR_SW_IMM_CURRENT_TSF                  (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0xA0)
#define	REG_PAC_EXTRAPOLATOR_SW_IMM_PREVIOUS_TSF                 (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0xA4)
#define	REG_PAC_EXTRAPOLATOR_SW_IMM_ABS_LN_BETA                  (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0xA8)
#define	REG_PAC_EXTRAPOLATOR_SW_IMM_AVG_DELTA_T                  (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0xAC)
#define	REG_PAC_EXTRAPOLATOR_SW_IMM_REQ_CFG                      (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0xB0)
#define	REG_PAC_EXTRAPOLATOR_SW_IMM_NEW_ALFA                     (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0xB4)
#define	REG_PAC_EXTRAPOLATOR_SW_IMM_NEW_AVG_WEIGHTED             (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0xB8)
#define	REG_PAC_EXTRAPOLATOR_EXTRAP_LOGGER_CTRL                  (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0xBC)
#define	REG_PAC_EXTRAPOLATOR_DEBUG_REG                           (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0xC0)
#define	REG_PAC_EXTRAPOLATOR_DEBUG_CURRENT_TSF                   (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0xC4)
#define	REG_PAC_EXTRAPOLATOR_DEBUG_DATA_IN                       (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0xC8)
#define	REG_PAC_EXTRAPOLATOR_DEBUG_NEW_ALFA                      (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0xCC)
#define	REG_PAC_EXTRAPOLATOR_DEBUG_DATA_NEW                      (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0xE0)
#define	REG_PAC_EXTRAPOLATOR_DEBUG_LAST_SMC_ADDR                 (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0xE4)
#define	REG_PAC_EXTRAPOLATOR_MIN_RSSI_DBG                        (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0xE8)
#define	REG_PAC_EXTRAPOLATOR_MIN_RSSI_DB_BASE                    (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0xEC)
#define	REG_PAC_EXTRAPOLATOR_MIN_RSSI_CFG                        (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0xF0)
#define	REG_PAC_EXTRAPOLATOR_MIN_RSSI_IDLE_DUR                   (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0xF4)
#define	REG_PAC_EXTRAPOLATOR_DBG_INFO_MIN_RSSI                   (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0xF8)
#define	REG_PAC_EXTRAPOLATOR_DBG_INFO_AF_FETCH_PREP              (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0xFC)
#define	REG_PAC_EXTRAPOLATOR_DBG_INFO_MAC_AF                     (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0x100)
#define	REG_PAC_EXTRAPOLATOR_DBG_INFO_MAC_USERS_DONE             (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0x104)
#define	REG_PAC_EXTRAPOLATOR_DBG_INFO_MAC_REINIT_DONE            (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0x108)
#define	REG_PAC_EXTRAPOLATOR_DBG_INFO_UPD_API                    (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0x10C)
#define	REG_PAC_EXTRAPOLATOR_SW_MAC_MU_AF_TSF                    (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0x120)
#define	REG_PAC_EXTRAPOLATOR_SW_MAC_USER_REINIT_VEC              (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0x124)
#define	REG_PAC_EXTRAPOLATOR_SW_MAC_INPUT_BUFFER                 (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0x128)
#define	REG_PAC_EXTRAPOLATOR_SW_MAC_MU_DB_PTR                    (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0x12C)
#define	REG_PAC_EXTRAPOLATOR_MU_DB_ENTRY_SIZE                    (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0x130)
#define	REG_PAC_EXTRAPOLATOR_SIGN_EXT_WORD_12_15                 (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0x134)
#define	REG_PAC_EXTRAPOLATOR_SW_IMM_CURRENT_PARAMETER_5TH        (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0x138)
#define	REG_PAC_EXTRAPOLATOR_SW_IMM_PREVIOUS_AVG_WEIGHTED_5TH    (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0x13C)
#define	REG_PAC_EXTRAPOLATOR_SW_IMM_NEW_AVG_WEIGHTED_5TH         (PAC_EXTRAPOLATOR_BASE_ADDRESS + 0x140)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_PAC_EXTRAPOLATOR_EXTRAP_CONTROL_CFG 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 extrapEn : 1; //Phy AF Block enable bit, Default is Disabled , When disabled @run-time, the block will finish current process and ignore next HW triggers., reset value: 0x0, access type: RW
		uint32 extrapBusy : 1; //Phy AF Block busy bit, Default is Free, reset value: 0x0, access type: RO
		uint32 reserved0 : 30;
	} bitFields;
} RegPacExtrapolatorExtrapControlCfg_u;

/*REG_PAC_EXTRAPOLATOR_SU_STA_DB_CFG 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 suStaDbBaseAddr : 22; //Alfa factors wlan DB base address at SHRAM, word aligned , reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 suDbEntrySize : 6; //in units of 32b words; max: 63 words., reset value: 0x0, access type: RW
		uint32 reserved1 : 2;
	} bitFields;
} RegPacExtrapolatorSuStaDbCfg_u;

/*REG_PAC_EXTRAPOLATOR_MU_DB_CFG 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muDbBaseAddr : 22; //no description, reset value: 0x0, access type: RW
		uint32 muDbUpdateEna : 1; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 9;
	} bitFields;
} RegPacExtrapolatorMuDbCfg_u;

/*REG_PAC_EXTRAPOLATOR_MU_USER_REINIT_VEC 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muUserReinitVec : 32; //Bit per user. "1" means treat/overwrite old DB value as "0". Active even if the user isn't "valid"., reset value: 0x0, access type: RW
	} bitFields;
} RegPacExtrapolatorMuUserReinitVec_u;

/*REG_PAC_EXTRAPOLATOR_LN_BETA_LUT_BASE_ADDR 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lnBetaLutBaseAddr : 22; //Ln Beta LUT base address at SHRAM, word aligned, reset value: 0x0, access type: RW
		uint32 reserved0 : 10;
	} bitFields;
} RegPacExtrapolatorLnBetaLutBaseAddr_u;

/*REG_PAC_EXTRAPOLATOR_WORD_UPD_EN_VEC 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 wordUpdEnVec : 16; //bit N is an enable of WordN update, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPacExtrapolatorWordUpdEnVec_u;

/*REG_PAC_EXTRAPOLATOR_WORD_METHOD_VEC 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 wordMethodVec : 32; //each two bits are method of the corresponding word. [1:0]->word0, [3:2]-> word1, etc., reset value: 0x0, access type: RW
	} bitFields;
} RegPacExtrapolatorWordMethodVec_u;

/*REG_PAC_EXTRAPOLATOR_TSF_RESOLUTION_CFG 0x1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 suTsfResolution : 4; //TSF resolution to determine time units, reset value: 0x7, access type: RW
		uint32 muTsfResolution : 4; //TSF resolution to determine time units, reset value: 0x7, access type: RW
		uint32 reserved0 : 24;
	} bitFields;
} RegPacExtrapolatorTsfResolutionCfg_u;

/*REG_PAC_EXTRAPOLATOR_OFFSET_SU_WORD_0_3 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 offsetSuWord0 : 5; //Word0 offset in the SU entry, reset value: 0x0, access type: RW
		uint32 reserved0 : 3;
		uint32 offsetSuWord1 : 5; //Word1 offset in the SU entry, reset value: 0x0, access type: RW
		uint32 reserved1 : 3;
		uint32 offsetSuWord2 : 5; //Word2 offset in the SU entry, reset value: 0x0, access type: RW
		uint32 reserved2 : 3;
		uint32 offsetSuWord3 : 5; //Word3 offset in the SU entry, reset value: 0x0, access type: RW
		uint32 reserved3 : 3;
	} bitFields;
} RegPacExtrapolatorOffsetSuWord03_u;

/*REG_PAC_EXTRAPOLATOR_OFFSET_SU_WORD_4_7 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 offsetSuWord4 : 5; //Word4 offset in the SU entry, reset value: 0x0, access type: RW
		uint32 reserved0 : 3;
		uint32 offsetSuWord5 : 5; //Word5 offset in the SU entry, reset value: 0x0, access type: RW
		uint32 reserved1 : 3;
		uint32 offsetSuWord6 : 5; //Word6 offset in the SU entry, reset value: 0x0, access type: RW
		uint32 reserved2 : 3;
		uint32 offsetSuWord7 : 5; //Word7 offset in the SU entry, reset value: 0x0, access type: RW
		uint32 reserved3 : 3;
	} bitFields;
} RegPacExtrapolatorOffsetSuWord47_u;

/*REG_PAC_EXTRAPOLATOR_OFFSET_SU_WORD_8_11 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 offsetSuWord8 : 5; //Word8 offset in the SU entry, reset value: 0x0, access type: RW
		uint32 reserved0 : 3;
		uint32 offsetSuWord9 : 5; //Word9 offset in the SU entry, reset value: 0x0, access type: RW
		uint32 reserved1 : 3;
		uint32 offsetSuWord10 : 5; //Word10 offset in the SU entry, reset value: 0x0, access type: RW
		uint32 reserved2 : 3;
		uint32 offsetSuWord11 : 5; //Word11 offset in the SU entry, reset value: 0x0, access type: RW
		uint32 reserved3 : 3;
	} bitFields;
} RegPacExtrapolatorOffsetSuWord811_u;

/*REG_PAC_EXTRAPOLATOR_OFFSET_SU_WORD_12_15 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 offsetSuWord12 : 5; //Word12 offset in the SU entry, reset value: 0x0, access type: RW
		uint32 reserved0 : 3;
		uint32 offsetSuWord13 : 5; //Word13 offset in the SU entry, reset value: 0x0, access type: RW
		uint32 reserved1 : 3;
		uint32 offsetSuWord14 : 5; //Word14 offset in the SU entry, reset value: 0x0, access type: RW
		uint32 reserved2 : 3;
		uint32 offsetSuWord15 : 5; //Word15 offset in the SU entry, reset value: 0x0, access type: RW
		uint32 reserved3 : 3;
	} bitFields;
} RegPacExtrapolatorOffsetSuWord1215_u;

/*REG_PAC_EXTRAPOLATOR_OFFSET_MU_WORD_0_3 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 offsetMuWord0 : 5; //Word0 offset in the MU entry, reset value: 0x0, access type: RW
		uint32 reserved0 : 3;
		uint32 offsetMuWord1 : 5; //Word1 offset in the MU entry, reset value: 0x0, access type: RW
		uint32 reserved1 : 3;
		uint32 offsetMuWord2 : 5; //Word2 offset in the MU entry, reset value: 0x0, access type: RW
		uint32 reserved2 : 3;
		uint32 offsetMuWord3 : 5; //Word3 offset in the MU entry, reset value: 0x0, access type: RW
		uint32 reserved3 : 3;
	} bitFields;
} RegPacExtrapolatorOffsetMuWord03_u;

/*REG_PAC_EXTRAPOLATOR_OFFSET_MU_WORD_4_7 0x38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 offsetMuWord4 : 5; //Word4 offset in the MU entry, reset value: 0x0, access type: RW
		uint32 reserved0 : 3;
		uint32 offsetMuWord5 : 5; //Word5 offset in the MU entry, reset value: 0x0, access type: RW
		uint32 reserved1 : 3;
		uint32 offsetMuWord6 : 5; //Word6 offset in the MU entry, reset value: 0x0, access type: RW
		uint32 reserved2 : 3;
		uint32 offsetMuWord7 : 5; //Word7 offset in the MU entry, reset value: 0x0, access type: RW
		uint32 reserved3 : 3;
	} bitFields;
} RegPacExtrapolatorOffsetMuWord47_u;

/*REG_PAC_EXTRAPOLATOR_OFFSET_MU_WORD_8_11 0x3C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 offsetMuWord8 : 5; //Word8 offset in the MU entry, reset value: 0x0, access type: RW
		uint32 reserved0 : 3;
		uint32 offsetMuWord9 : 5; //Word9 offset in the MU entry, reset value: 0x0, access type: RW
		uint32 reserved1 : 3;
		uint32 offsetMuWord10 : 5; //Word10 offset in the MU entry, reset value: 0x0, access type: RW
		uint32 reserved2 : 3;
		uint32 offsetMuWord11 : 5; //Word11 offset in the MU entry, reset value: 0x0, access type: RW
		uint32 reserved3 : 3;
	} bitFields;
} RegPacExtrapolatorOffsetMuWord811_u;

/*REG_PAC_EXTRAPOLATOR_OFFSET_MU_WORD_12_15 0x40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 offsetMuWord12 : 5; //Word12 offset in the MU entry, reset value: 0x0, access type: RW
		uint32 reserved0 : 3;
		uint32 offsetMuWord13 : 5; //Word13 offset in the MU entry, reset value: 0x0, access type: RW
		uint32 reserved1 : 3;
		uint32 offsetMuWord14 : 5; //Word14 offset in the MU entry, reset value: 0x0, access type: RW
		uint32 reserved2 : 3;
		uint32 offsetMuWord15 : 5; //Word15 offset in the MU entry, reset value: 0x0, access type: RW
		uint32 reserved3 : 3;
	} bitFields;
} RegPacExtrapolatorOffsetMuWord1215_u;

/*REG_PAC_EXTRAPOLATOR_SIGN_EXT_WORD_0_5 0x44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 signExtWord0 : 5; //each bit controls sign-extension of the corresponding field of the relevant word., reset value: 0x1f, access type: RW
		uint32 signExtWord1 : 5; //each bit controls sign-extension of the corresponding field of the relevant word., reset value: 0x1f, access type: RW
		uint32 signExtWord2 : 5; //each bit controls sign-extension of the corresponding field of the relevant word., reset value: 0x1f, access type: RW
		uint32 signExtWord3 : 5; //each bit controls sign-extension of the corresponding field of the relevant word., reset value: 0x1f, access type: RW
		uint32 signExtWord4 : 5; //no description, reset value: 0x0, access type: RW
		uint32 signExtWord5 : 5; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegPacExtrapolatorSignExtWord05_u;

/*REG_PAC_EXTRAPOLATOR_SIGN_EXT_WORD_6_11 0x48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 signExtWord6 : 5; //no description, reset value: 0x0, access type: RW
		uint32 signExtWord7 : 5; //no description, reset value: 0x0, access type: RW
		uint32 signExtWord8 : 5; //no description, reset value: 0x0, access type: RW
		uint32 signExtWord9 : 5; //no description, reset value: 0x0, access type: RW
		uint32 signExtWord10 : 5; //no description, reset value: 0x0, access type: RW
		uint32 signExtWord11 : 5; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegPacExtrapolatorSignExtWord611_u;

/*REG_PAC_EXTRAPOLATOR_STA_EN_0_31 0x4C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 staEn031 : 32; //STA enable bits 0 to 31. Default is Disabled, reset value: 0x0, access type: RW
	} bitFields;
} RegPacExtrapolatorStaEn031_u;

/*REG_PAC_EXTRAPOLATOR_STA_EN_32_63 0x50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 staEn3263 : 32; //STA enable bits 32 to 63. Default is Disabled, reset value: 0x0, access type: RW
	} bitFields;
} RegPacExtrapolatorStaEn3263_u;

/*REG_PAC_EXTRAPOLATOR_STA_EN_64_95 0x54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 staEn6495 : 32; //STA enable bits 64 to 95. Default is Disabled, reset value: 0x0, access type: RW
	} bitFields;
} RegPacExtrapolatorStaEn6495_u;

/*REG_PAC_EXTRAPOLATOR_STA_EN_96_127 0x58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 staEn96127 : 32; //STA enable bits 96 to 127. Default is Disabled, reset value: 0x0, access type: RW
	} bitFields;
} RegPacExtrapolatorStaEn96127_u;

/*REG_PAC_EXTRAPOLATOR_STA_EN_128_159 0x5C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 staEn128159 : 32; //STA enable bits 128 to 159. Default is Disabled, reset value: 0x0, access type: RW
	} bitFields;
} RegPacExtrapolatorStaEn128159_u;

/*REG_PAC_EXTRAPOLATOR_STA_EN_160_191 0x60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 staEn160191 : 32; //STA enable bits 160 to 191. Default is Disabled, reset value: 0x0, access type: RW
	} bitFields;
} RegPacExtrapolatorStaEn160191_u;

/*REG_PAC_EXTRAPOLATOR_STA_EN_192_223 0x64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 staEn192223 : 32; //STA enable bits 192 to 223. Default is Disabled, reset value: 0x0, access type: RW
	} bitFields;
} RegPacExtrapolatorStaEn192223_u;

/*REG_PAC_EXTRAPOLATOR_STA_EN_224_255 0x68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 staEn224255 : 32; //STA enable bits 224 to 255. Default is Disabled, reset value: 0x0, access type: RW
	} bitFields;
} RegPacExtrapolatorStaEn224255_u;

/*REG_PAC_EXTRAPOLATOR_SW_MAC_MU_AF_REQ 0x6C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swMacMuAfReq : 1; //Send MAC AF start request, reset value: 0x0, access type: WO
		uint32 reserved0 : 31;
	} bitFields;
} RegPacExtrapolatorSwMacMuAfReq_u;

/*REG_PAC_EXTRAPOLATOR_SW_MAC_MU_AF_CFG 0x70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swMacInpEntrySize : 2; //number of 32bit words per Input entry (1 or 2), reset value: 0x0, access type: RW
		uint32 swMacTsfRes : 4; //MAC AF tsf resolution , reset value: 0x0, access type: RW
		uint32 swMacAfDone : 1; //Processing done indication, reset value: 0x0, access type: RO
		uint32 swMacAfReqError : 1; //Entry access conflict - collides with Entry Update API. Error IRQ is also asserted., reset value: 0x0, access type: RO
		uint32 swMacErrIrqClear : 1; //clear error IRQ, reset value: 0x0, access type: WO
		uint32 swMacErrIrqEna : 1; //Enable Error IRQ assertion is case of entry access conflict., reset value: 0x0, access type: RW
		uint32 reserved0 : 22;
	} bitFields;
} RegPacExtrapolatorSwMacMuAfCfg_u;

/*REG_PAC_EXTRAPOLATOR_SW_MAC_USER_VALID_VEC 0x74 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swMacUserValidVec : 32; //Enable bit per user for MAC AF., reset value: 0x0, access type: RW
	} bitFields;
} RegPacExtrapolatorSwMacUserValidVec_u;

/*REG_PAC_EXTRAPOLATOR_SW_API_ENTRY_UPDATE_REQ 0x78 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swEntryUpdateApiReq : 1; //Set entry update API request., reset value: 0x0, access type: WO
		uint32 reserved0 : 31;
	} bitFields;
} RegPacExtrapolatorSwApiEntryUpdateReq_u;

/*REG_PAC_EXTRAPOLATOR_SW_API_STATUS 0x7C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swApiUpdActive : 1; //Update is ongoing, reset value: 0x0, access type: RO
		uint32 swApiUpdPending : 1; //The API waits for Phy AF to finish, reset value: 0x0, access type: RO
		uint32 swApiDbUpdError : 1; //When asserted = the requested entry is corrupted due to collision. IRQ is also asserted., reset value: 0x0, access type: RO
		uint32 reserved0 : 29;
	} bitFields;
} RegPacExtrapolatorSwApiStatus_u;

/*REG_PAC_EXTRAPOLATOR_SW_API_ENTRY_UPDATE_CFG 0x80 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swApiEntryId : 8; //Entry ID can be either STA_ID or USER_ID., reset value: 0x0, access type: RW
		uint32 swApiEntrySize : 6; //Entry size of the requested DB to update., reset value: 0x0, access type: RW
		uint32 swApiLastWordNum : 5; //Last word in mirror buffer = last word of an array to copy from., reset value: 0x0, access type: RW
		uint32 swApiErrIrqClear : 1; //Clear API error IRQ., reset value: 0x0, access type: WO
		uint32 swApiErrIrqEna : 1; //Enable API Error IRQ (entry access conflict)., reset value: 0x0, access type: RW
		uint32 reserved0 : 11;
	} bitFields;
} RegPacExtrapolatorSwApiEntryUpdateCfg_u;

/*REG_PAC_EXTRAPOLATOR_SW_API_MIRROR_ADDR 0x84 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swApiMirrorAddr : 22; //Address of 32 words array (or less) to copy words from., reset value: 0x0, access type: RW
		uint32 reserved0 : 10;
	} bitFields;
} RegPacExtrapolatorSwApiMirrorAddr_u;

/*REG_PAC_EXTRAPOLATOR_SW_API_DB_ADDR 0x88 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swApiDbAddr : 22; //Target DB pointer to copy there words from mirror buffer., reset value: 0x0, access type: RW
		uint32 reserved0 : 10;
	} bitFields;
} RegPacExtrapolatorSwApiDbAddr_u;

/*REG_PAC_EXTRAPOLATOR_SW_API_WORD_UPD_ENA_VEC 0x8C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swApiWordUpdEnaVec : 32; //Each bit enables overwriting the word at the corresponding position in the target DB entry by the relevant word of mirror buffer., reset value: 0x0, access type: RW
	} bitFields;
} RegPacExtrapolatorSwApiWordUpdEnaVec_u;

/*REG_PAC_EXTRAPOLATOR_SW_IMM_REQ 0x90 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swImmReqSet : 1; //Set Immediate calc SW request, reset value: 0x0, access type: WO
		uint32 swImmBusy : 1; //Clear interrupt indication, reset value: 0x0, access type: RO
		uint32 reserved0 : 30;
	} bitFields;
} RegPacExtrapolatorSwImmReq_u;

/*REG_PAC_EXTRAPOLATOR_SW_IMM_CURRENT_ALFA 0x94 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swImmCurrentAlfa : 21; //Set SW current alfa, reset value: 0x0, access type: RW
		uint32 reserved0 : 11;
	} bitFields;
} RegPacExtrapolatorSwImmCurrentAlfa_u;

/*REG_PAC_EXTRAPOLATOR_SW_IMM_CURRENT_PARAMETER 0x98 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swImmCurrentParameter : 32; //Set SW current parameter, reset value: 0x0, access type: RW
	} bitFields;
} RegPacExtrapolatorSwImmCurrentParameter_u;

/*REG_PAC_EXTRAPOLATOR_SW_IMM_PREVIOUS_AVG_WEIGHTED 0x9C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swImmPreviousAvgWeighted : 32; //Set SW previous avg weighted, reset value: 0x0, access type: RW
	} bitFields;
} RegPacExtrapolatorSwImmPreviousAvgWeighted_u;

/*REG_PAC_EXTRAPOLATOR_SW_IMM_CURRENT_TSF 0xA0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swImmCurrentTsf : 32; //Set SW current TSF, reset value: 0x0, access type: RW
	} bitFields;
} RegPacExtrapolatorSwImmCurrentTsf_u;

/*REG_PAC_EXTRAPOLATOR_SW_IMM_PREVIOUS_TSF 0xA4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swImmPreviousTsf : 32; //Set SW previous TSF, reset value: 0x0, access type: RW
	} bitFields;
} RegPacExtrapolatorSwImmPreviousTsf_u;

/*REG_PAC_EXTRAPOLATOR_SW_IMM_ABS_LN_BETA 0xA8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swImmAbsLnBeta : 12; //Set SW abs ln beta, reset value: 0x0, access type: RW
		uint32 reserved0 : 20;
	} bitFields;
} RegPacExtrapolatorSwImmAbsLnBeta_u;

/*REG_PAC_EXTRAPOLATOR_SW_IMM_AVG_DELTA_T 0xAC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swImmAvgDeltaT : 15; //Set SW average delta T, reset value: 0x0, access type: RW
		uint32 reserved0 : 17;
	} bitFields;
} RegPacExtrapolatorSwImmAvgDeltaT_u;

/*REG_PAC_EXTRAPOLATOR_SW_IMM_REQ_CFG 0xB0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swImmNumOfValidFields : 3; //Set SW number of valid fields: , 0x0: one field (1 Byte) , 0x1: two fields (2 Byte) , 0x2: three fields (3 Byte) , 0x3: four fields (4 Byte) , 0x4: five fields (5 Byte), reset value: 0x0, access type: RW
		uint32 swImmHalfWordEn : 1; //Set SW half word enable, if enable number of fields ignored, reset value: 0x0, access type: RW
		uint32 swImmSignExt : 5; //SW sign extension indication per word field, reset value: 0x0, access type: RW
		uint32 swImmTsfRes : 4; //Set SW tsf resolution, reset value: 0x7, access type: RW
		uint32 swImmReqAlfaMode : 1; //Set SW request alfa mode: , 0x0: Basic alfa mode , 0x1: Alfa Beta mode, reset value: 0x0, access type: RW
		uint32 reserved0 : 18;
	} bitFields;
} RegPacExtrapolatorSwImmReqCfg_u;

/*REG_PAC_EXTRAPOLATOR_SW_IMM_NEW_ALFA 0xB4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swImmNewAlfa : 21; //SW calculated new alfa parameter, reset value: 0x0, access type: RO
		uint32 reserved0 : 11;
	} bitFields;
} RegPacExtrapolatorSwImmNewAlfa_u;

/*REG_PAC_EXTRAPOLATOR_SW_IMM_NEW_AVG_WEIGHTED 0xB8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swImmNewAvgWeighted : 32; //SW new avg weighted, reset value: 0x0, access type: RO
	} bitFields;
} RegPacExtrapolatorSwImmNewAvgWeighted_u;

/*REG_PAC_EXTRAPOLATOR_EXTRAP_LOGGER_CTRL 0xBC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 loggerPhyEn : 1; //Phy AF Logger enable bit, Default is Disabled, reset value: 0x0, access type: RW
		uint32 loggerMacEn : 1; //MAC AF Logger enable, default: Disabled, reset value: 0x0, access type: RW
		uint32 loggerPhyActive : 1; //Phy AF Logger active indication, reset value: 0x0, access type: RO
		uint32 loggerMacActive : 1; //MAC AF Logger active indication, reset value: 0x0, access type: RO
		uint32 loggerPhyMsgEna : 8; //One enable bit per Phy AF message type. Upto 8 diff. msg_ids., reset value: 0x0, access type: RW
		uint32 loggerMacMsgEna : 8; //One enable bit per MAC AF message type. Upto 8 diff. msg_ids., reset value: 0x0, access type: RW
		uint32 loggerPhyPriority : 2; //no description, reset value: 0x0, access type: RW
		uint32 loggerMacPriority : 2; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegPacExtrapolatorExtrapLoggerCtrl_u;

/*REG_PAC_EXTRAPOLATOR_DEBUG_REG 0xC0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 debugExtrapHwActive : 1; //no description, reset value: 0x0, access type: RO
		uint32 debugExtrapSwImmActive : 1; //no description, reset value: 0x0, access type: RO
		uint32 debugExtrapSwMacActive : 1; //no description, reset value: 0x0, access type: RO
		uint32 debugWordIdx : 4; //no description, reset value: 0x0, access type: RO
		uint32 debugFieldIdx : 3; //no description, reset value: 0x0, access type: RO
		uint32 debugExtrapMainSm : 4; //no description, reset value: 0x0, access type: RO
		uint32 debugPhyModeSmp : 3; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 15;
	} bitFields;
} RegPacExtrapolatorDebugReg_u;

/*REG_PAC_EXTRAPOLATOR_DEBUG_CURRENT_TSF 0xC4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 debugCurrentTsf : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegPacExtrapolatorDebugCurrentTsf_u;

/*REG_PAC_EXTRAPOLATOR_DEBUG_DATA_IN 0xC8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 debugDataInField0 : 8; //no description, reset value: 0x0, access type: RO
		uint32 debugDataInField1 : 8; //no description, reset value: 0x0, access type: RO
		uint32 debugDataInField2 : 8; //no description, reset value: 0x0, access type: RO
		uint32 debugDataInField3 : 8; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegPacExtrapolatorDebugDataIn_u;

/*REG_PAC_EXTRAPOLATOR_DEBUG_NEW_ALFA 0xCC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 debugNewAlfa : 21; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 11;
	} bitFields;
} RegPacExtrapolatorDebugNewAlfa_u;

/*REG_PAC_EXTRAPOLATOR_DEBUG_DATA_NEW 0xE0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 debugDataNewField0 : 8; //no description, reset value: 0x0, access type: RO
		uint32 debugDataNewField1 : 8; //no description, reset value: 0x0, access type: RO
		uint32 debugDataNewField2 : 8; //no description, reset value: 0x0, access type: RO
		uint32 debugDataNewField3 : 8; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegPacExtrapolatorDebugDataNew_u;

/*REG_PAC_EXTRAPOLATOR_DEBUG_LAST_SMC_ADDR 0xE4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 debugLastSmcAddr : 22; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 10;
	} bitFields;
} RegPacExtrapolatorDebugLastSmcAddr_u;

/*REG_PAC_EXTRAPOLATOR_MIN_RSSI_DBG 0xE8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 minRssiDbgValue : 8; //no description, reset value: 0x81, access type: RW
		uint32 minRssiDbgStb : 1; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 23;
	} bitFields;
} RegPacExtrapolatorMinRssiDbg_u;

/*REG_PAC_EXTRAPOLATOR_MIN_RSSI_DB_BASE 0xEC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 minRssiDbBase : 22; //Base address of "Minimal RSSI per STA" database, reset value: 0x0, access type: RW
		uint32 reserved0 : 10;
	} bitFields;
} RegPacExtrapolatorMinRssiDbBase_u;

/*REG_PAC_EXTRAPOLATOR_MIN_RSSI_CFG 0xF0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 minRssiTrig : 1; //Trigger immediate RSSI DB update when sta dropped + set 80h value to the dropped entry., reset value: 0x0, access type: RW
		uint32 minRssiCdb : 1; //Single Band (0) or CDB mode (1), reset value: 0x0, access type: RW
		uint32 minRssiEna : 1; //Enables automatic min_rssi_search triggering. , Disable only at idle state of both engines!!! (min_rssi & Phy AF)., reset value: 0x0, access type: RW
		uint32 reserved0 : 5;
		uint32 minRssiStaId : 8; //STA ID to overwrite with 80h (=dropped/invalid) @ min_rssi_trig, reset value: 0x0, access type: RW
		uint32 minRssiTrsh : 8; //Min RSSI threshold - value to provide to Phy when all STA's are 80h, reset value: 0x81, access type: RW
		uint32 reserved1 : 8;
	} bitFields;
} RegPacExtrapolatorMinRssiCfg_u;

/*REG_PAC_EXTRAPOLATOR_MIN_RSSI_IDLE_DUR 0xF4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 minRssiIdleDur : 32; //Number of microseconds at idle state - when the internal timer reaches this value, the min_RSSI DB is auto-initialized (set 80h to all STAs entries)., reset value: 0xFFFFFFFF, access type: RW
	} bitFields;
} RegPacExtrapolatorMinRssiIdleDur_u;

/*REG_PAC_EXTRAPOLATOR_DBG_INFO_MIN_RSSI 0xF8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 minRssiByteCnt : 4; //no description, reset value: 0x0, access type: RO
		uint32 minRssiAddrCnt : 4; //no description, reset value: 0x0, access type: RO
		uint32 minRssiSm : 3; //no description, reset value: 0x0, access type: RO
		uint32 minRssiStaIdx : 9; //no description, reset value: 0x1ff, access type: RO
		uint32 minRssiSwUpdPend : 1; //no description, reset value: 0x0, access type: RO
		uint32 minRssiHwUpdPend : 1; //no description, reset value: 0x0, access type: RO
		uint32 minRssiReinitPend : 1; //no description, reset value: 0x0, access type: RO
		uint32 minRssiHwSrchPend : 1; //no description, reset value: 0x0, access type: RO
		uint32 minRssiSelMaster : 2; //0: HW_UPD, 1: SW_UPD, 2: HW_SRCH, 3: RE_INIT, reset value: 0x0, access type: RO
		uint32 reserved0 : 6;
	} bitFields;
} RegPacExtrapolatorDbgInfoMinRssi_u;

/*REG_PAC_EXTRAPOLATOR_DBG_INFO_AF_FETCH_PREP 0xFC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fetchSm : 3; //no description, reset value: 0x0, access type: RO
		uint32 prepCurrSta : 8; //no description, reset value: 0x0, access type: RO
		uint32 prepCurrUser : 6; //no description, reset value: 0x0, access type: RO
		uint32 prepNoMoreUsers : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 14;
	} bitFields;
} RegPacExtrapolatorDbgInfoAfFetchPrep_u;

/*REG_PAC_EXTRAPOLATOR_DBG_INFO_MAC_AF 0x100 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 macAfSm : 4; //no description, reset value: 0x0, access type: RO
		uint32 macAfWordCnt : 4; //no description, reset value: 0x0, access type: RO
		uint32 macAfCurrUser : 5; //no description, reset value: 0x0, access type: RO
		uint32 macAfWordsReady : 1; //no description, reset value: 0x0, access type: RO
		uint32 macAfNoReinit : 1; //no description, reset value: 0x1, access type: RO
		uint32 macAfNoUsers : 1; //no description, reset value: 0x1, access type: RO
		uint32 macAfGatorEna : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 15;
	} bitFields;
} RegPacExtrapolatorDbgInfoMacAf_u;

/*REG_PAC_EXTRAPOLATOR_DBG_INFO_MAC_USERS_DONE 0x104 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 macAfUserDoneVec : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegPacExtrapolatorDbgInfoMacUsersDone_u;

/*REG_PAC_EXTRAPOLATOR_DBG_INFO_MAC_REINIT_DONE 0x108 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 macAfUserReinitDoneVec : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegPacExtrapolatorDbgInfoMacReinitDone_u;

/*REG_PAC_EXTRAPOLATOR_DBG_INFO_UPD_API 0x10C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 updApiSm : 2; //no description, reset value: 0x0, access type: RO
		uint32 updApiWordCnt : 5; //no description, reset value: 0x0, access type: RO
		uint32 updApiGatorEna : 1; //no description, reset value: 0x0, access type: RO
		uint32 updApiRdAddr : 8; //no description, reset value: 0x0, access type: RO
		uint32 updApiWrAddr : 8; //no description, reset value: 0x0, access type: RO
		uint32 updApiPhyAfCollision : 1; //High when Phy AF works on the same user group (API will be pending), reset value: 0x0, access type: RO
		uint32 reserved0 : 7;
	} bitFields;
} RegPacExtrapolatorDbgInfoUpdApi_u;

/*REG_PAC_EXTRAPOLATOR_SW_MAC_MU_AF_TSF 0x120 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swMacMuAfTsf : 32; //TSF value of the MAC words sampling moment., reset value: 0x0, access type: RW
	} bitFields;
} RegPacExtrapolatorSwMacMuAfTsf_u;

/*REG_PAC_EXTRAPOLATOR_SW_MAC_USER_REINIT_VEC 0x124 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swMacUserReinitVec : 32; //Bit per user. "1" means treat/overwrite old DB value as "0".  , Active even if the user isn't "valid"., reset value: 0x0, access type: RW
	} bitFields;
} RegPacExtrapolatorSwMacUserReinitVec_u;

/*REG_PAC_EXTRAPOLATOR_SW_MAC_INPUT_BUFFER 0x128 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swMacInputBuffer : 22; //MAC words input buffer pointer (ShRAM), reset value: 0x0, access type: RW
		uint32 reserved0 : 10;
	} bitFields;
} RegPacExtrapolatorSwMacInputBuffer_u;

/*REG_PAC_EXTRAPOLATOR_SW_MAC_MU_DB_PTR 0x12C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swMacMuDbPtr : 22; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 10;
	} bitFields;
} RegPacExtrapolatorSwMacMuDbPtr_u;

/*REG_PAC_EXTRAPOLATOR_MU_DB_ENTRY_SIZE 0x130 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muDbEntrySize : 6; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 26;
	} bitFields;
} RegPacExtrapolatorMuDbEntrySize_u;

/*REG_PAC_EXTRAPOLATOR_SIGN_EXT_WORD_12_15 0x134 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 signExtWord12 : 5; //no description, reset value: 0x0, access type: RW
		uint32 signExtWord13 : 5; //no description, reset value: 0x0, access type: RW
		uint32 signExtWord14 : 5; //no description, reset value: 0x0, access type: RW
		uint32 signExtWord15 : 5; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 12;
	} bitFields;
} RegPacExtrapolatorSignExtWord1215_u;

/*REG_PAC_EXTRAPOLATOR_SW_IMM_CURRENT_PARAMETER_5TH 0x138 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swImmCurrentParameter5Th : 8; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 24;
	} bitFields;
} RegPacExtrapolatorSwImmCurrentParameter5Th_u;

/*REG_PAC_EXTRAPOLATOR_SW_IMM_PREVIOUS_AVG_WEIGHTED_5TH 0x13C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swImmPreviousAvgWeighted5Th : 8; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 24;
	} bitFields;
} RegPacExtrapolatorSwImmPreviousAvgWeighted5Th_u;

/*REG_PAC_EXTRAPOLATOR_SW_IMM_NEW_AVG_WEIGHTED_5TH 0x140 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swImmNewAvgWeighted5Th : 8; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 24;
	} bitFields;
} RegPacExtrapolatorSwImmNewAvgWeighted5Th_u;



#endif // _PAC_EXTRAPOLATOR_REGS_H_
