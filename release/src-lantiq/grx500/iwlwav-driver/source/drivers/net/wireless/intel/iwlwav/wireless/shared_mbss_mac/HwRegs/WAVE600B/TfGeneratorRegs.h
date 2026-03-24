
/***********************************************************************************
File:				TfGeneratorRegs.h
Module:				TfGenerator
SOC Revision:		
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _TF_GENERATOR_REGS_H_
#define _TF_GENERATOR_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define TF_GENERATOR_BASE_ADDRESS                             MEMORY_MAP_UNIT_44_BASE_ADDRESS
#define	REG_TF_GENERATOR_TF_GEN_ACTIVATION_CTRL                          (TF_GENERATOR_BASE_ADDRESS + 0x0)
#define	REG_TF_GENERATOR_TF_GEN_CONTROL_BITS                             (TF_GENERATOR_BASE_ADDRESS + 0x4)
#define	REG_TF_GENERATOR_TF_GEN_HEADER_FRAME_CONTROL                     (TF_GENERATOR_BASE_ADDRESS + 0x8)
#define	REG_TF_GENERATOR_TF_GEN_COMMON_INFO                              (TF_GENERATOR_BASE_ADDRESS + 0xC)
#define	REG_TF_GENERATOR_TF_GEN_USER_INFO                                (TF_GENERATOR_BASE_ADDRESS + 0x10)
#define	REG_TF_GENERATOR_TF_GEN_MU_BAR_INFO                              (TF_GENERATOR_BASE_ADDRESS + 0x14)
#define	REG_TF_GENERATOR_TF_PHASE_ALLOCATED_TEMPLATE                     (TF_GENERATOR_BASE_ADDRESS + 0x28)
#define	REG_TF_GENERATOR_TF_GEN_INT_ERROR_STATUS                         (TF_GENERATOR_BASE_ADDRESS + 0x60)
#define	REG_TF_GENERATOR_TF_GEN_INT_ERROR_EN                             (TF_GENERATOR_BASE_ADDRESS + 0x64)
#define	REG_TF_GENERATOR_TF_GEN_INT_ERROR_CLEAR                          (TF_GENERATOR_BASE_ADDRESS + 0x68)
#define	REG_TF_GENERATOR_TF_GEN_TRIGGER_TYPE_PARAMS                      (TF_GENERATOR_BASE_ADDRESS + 0x70)
#define	REG_TF_GENERATOR_TF_GEN_LSIG_LENGTH_PARAMS                       (TF_GENERATOR_BASE_ADDRESS + 0x74)
#define	REG_TF_GENERATOR_TF_GEN_CASCADE_BW_PARAMS                        (TF_GENERATOR_BASE_ADDRESS + 0x78)
#define	REG_TF_GENERATOR_TF_GEN_AID12_PARAMS                             (TF_GENERATOR_BASE_ADDRESS + 0x84)
#define	REG_TF_GENERATOR_TF_GEN_RU_ALLOCATION_PARAMS                     (TF_GENERATOR_BASE_ADDRESS + 0x88)
#define	REG_TF_GENERATOR_TF_GEN_MCS_DCM_NSS_PARAMS                       (TF_GENERATOR_BASE_ADDRESS + 0x8C)
#define	REG_TF_GENERATOR_TF_GEN_TARGET_RSSI_PARAMS                       (TF_GENERATOR_BASE_ADDRESS + 0x90)
#define	REG_TF_GENERATOR_TF_GEN_BASIC_TRIGGER_USER_PARAMS                (TF_GENERATOR_BASE_ADDRESS + 0x94)
#define	REG_TF_GENERATOR_TF_GEN_MU_BAR_CONTROL_PARAMS                    (TF_GENERATOR_BASE_ADDRESS + 0x98)
#define	REG_TF_GENERATOR_TF_GEN_COMPRESSED_MU_BAR_INFORMATION_PARAMS     (TF_GENERATOR_BASE_ADDRESS + 0x9C)
#define	REG_TF_GENERATOR_TF_GEN_MULTI_TID_MU_BAR_INFORMATION_PARAMS_0    (TF_GENERATOR_BASE_ADDRESS + 0xA0)
#define	REG_TF_GENERATOR_TF_GEN_MULTI_TID_MU_BAR_INFORMATION_PARAMS_1    (TF_GENERATOR_BASE_ADDRESS + 0xA4)
#define	REG_TF_GENERATOR_TF_GEN_SW_OVERRIDE_OFFSET                       (TF_GENERATOR_BASE_ADDRESS + 0xA8)
#define	REG_TF_GENERATOR_TF_GEN_COMMON_INFO_SW_OVERRIDE_VALUE            (TF_GENERATOR_BASE_ADDRESS + 0xAC)
#define	REG_TF_GENERATOR_TF_GEN_COMMON_INFO_SW_OVERRIDE_MASK             (TF_GENERATOR_BASE_ADDRESS + 0xB0)
#define	REG_TF_GENERATOR_TF_GEN_USER_INFO_SW_OVERRIDE_VALUE              (TF_GENERATOR_BASE_ADDRESS + 0xB4)
#define	REG_TF_GENERATOR_TF_GEN_USER_INFO_SW_OVERRIDE_MASK               (TF_GENERATOR_BASE_ADDRESS + 0xB8)
#define	REG_TF_GENERATOR_TF_GEN_STATS_0                                  (TF_GENERATOR_BASE_ADDRESS + 0xBC)
#define	REG_TF_GENERATOR_TF_GEN_CLEAR_BITS                               (TF_GENERATOR_BASE_ADDRESS + 0xC0)
#define	REG_TF_GENERATOR_TF_GEN_STATUS_0                                 (TF_GENERATOR_BASE_ADDRESS + 0xC4)
#define	REG_TF_GENERATOR_TF_GEN_STATUS_1                                 (TF_GENERATOR_BASE_ADDRESS + 0xC8)
#define	REG_TF_GENERATOR_TF_GEN_STATUS_2                                 (TF_GENERATOR_BASE_ADDRESS + 0xCC)
#define	REG_TF_GENERATOR_TF_GEN_STATUS_3                                 (TF_GENERATOR_BASE_ADDRESS + 0xD0)
#define	REG_TF_GENERATOR_TF_GEN_LOGGER_CONTROL                           (TF_GENERATOR_BASE_ADDRESS + 0xD4)
#define	REG_TF_GENERATOR_TF_GEN_LOGGER_STATUS                            (TF_GENERATOR_BASE_ADDRESS + 0xD8)
#define	REG_TF_GENERATOR_TF_GEN_SPARE_REGISTER                           (TF_GENERATOR_BASE_ADDRESS + 0xE0)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_TF_GENERATOR_TF_GEN_ACTIVATION_CTRL 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tfGenGo : 1; //Writing to this field a value of '1', serves as a Go command. The TF Generator shall start constructing the Trigger Frame upon a Go command , It is forbidden to issue a Go command before the previous one had ended. , , reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegTfGeneratorTfGenActivationCtrl_u;

/*REG_TF_GENERATOR_TF_GEN_CONTROL_BITS 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ruPrimarySecondary80SourceCfg : 1; //This parameter is used for setting the source of the Primary/Secondary 80 indication (part of RU Allocation field). 0 - Field is set by internal logic (based on Primary_80 indication);  1 - Field is taken From RU Index LUT (from bit 7 of the respective RU Index LUT word) , , reset value: 0x0, access type: RW
		uint32 ruPrimarySecondary80PolarityCfg : 1; //When ru_primary_secondary_80_source_cfg is 0 (i.e. RU Primary/Secondary 80 is set by intrnal logic), this signal sets the polarity of Primary/Secondary 80 indication. 0 - Primary/Secondary 80 keeps its value;  1 - Primary/Secondary 80 flips its value, reset value: 0x0, access type: RW
		uint32 ruPrimarySecondary80For160Cfg : 1; //Bit 12 value (RU Primary/Secondary 80 within RU Allocation) in case (BW==160) and (ru_size==160), reset value: 0x1, access type: RW
		uint32 reserved0 : 29;
	} bitFields;
} RegTfGeneratorTfGenControlBits_u;

/*REG_TF_GENERATOR_TF_GEN_HEADER_FRAME_CONTROL 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 frameControlProtocolVersion : 2; //Protocol Version field of the Frame Control (Trigger Frame values), reset value: 0x0, access type: RW
		uint32 frameControlType : 2; //Type field of the Frame Control (Trigger Frame values), reset value: 0x1, access type: RW
		uint32 frameControlSubtype : 4; //Subtype field of the Frame Control (Trigger Frame values), reset value: 0x2, access type: RW
		uint32 frameControlToDs : 1; //To DS field of the Frame Control (Trigger Frame values), reset value: 0x0, access type: RW
		uint32 frameControlFromDs : 1; //From DS field of the Frame Control (Trigger Frame values), reset value: 0x0, access type: RW
		uint32 frameControlMoreFragments : 1; //More Fragments field of the Frame Control (Trigger Frame values), reset value: 0x0, access type: RW
		uint32 frameControlRetry : 1; //Retry field of the Frame Control (Trigger Frame values), reset value: 0x0, access type: RW
		uint32 frameControlPowerManagement : 1; //Power Management field of the Frame Control (Trigger Frame values), reset value: 0x0, access type: RW
		uint32 frameControlMoreData : 1; //More Data field of the Frame Control (Trigger Frame values), reset value: 0x0, access type: RW
		uint32 frameControlProtectedFrame : 1; //Protected Frame field of the Frame Control (Trigger Frame values), reset value: 0x0, access type: RW
		uint32 frameControlHtcOrder : 1; //HTC/Order field of the Frame Control (Trigger Frame values), reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegTfGeneratorTfGenHeaderFrameControl_u;

/*REG_TF_GENERATOR_TF_GEN_COMMON_INFO 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 basicTriggerCommonInfoLength : 4; //Length (in bytes) of common info field in Basic TF (includes General Common Info + Trigger Dependent Common Info). This length cannot exceed Allocated Common Info Template Length configuration. , , reset value: 0x8, access type: RW
		uint32 bfrpCommonInfoLength : 4; //Length (in bytes) of common info field in BFRP (includes General Common Info + Trigger Dependent Common Info). This length cannot exceed Allocated Common Info Template Length configuration., reset value: 0x8, access type: RW
		uint32 muBarCommonInfoLength : 4; //Length (in bytes) of common info field in MU-BAR (includes General Common Info + Trigger Dependent Common Info). This length cannot exceed Allocated Common Info Template Length configuration. , , reset value: 0x8, access type: RW
		uint32 muRtsCommonInfoLength : 4; //Length (in bytes) of common info field in MU RTS (includes General Common Info + Trigger Dependent Common Info). This length cannot exceed Allocated Common Info Template Length configuration. ,  , , reset value: 0x8, access type: RW
		uint32 bsrpCommonInfoLength : 4; //Length (in bytes) for common info field in BSRP (includes General Common Info + Trigger Dependent Common Info). This length cannot exceed Allocated Common Info Template Length configuration. , , reset value: 0x8, access type: RW
		uint32 reserved0 : 12;
	} bitFields;
} RegTfGeneratorTfGenCommonInfo_u;

/*REG_TF_GENERATOR_TF_GEN_USER_INFO 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 basicTriggerUserInfoLength : 4; //Length (in bytes) of user info field in Basic TF (includes General User Info + Trigger Dependent User Info). This length cannot exceed Allocated User Info Template Length configuration. Maximal allowed length is 13B, reset value: 0x6, access type: RW
		uint32 bfrpUserInfoLength : 4; //Length (in bytes) of user info field in BFRP (includes General User Info + Trigger Dependent User Info). This length cannot exceed Allocated User Info Template Length configuration. Maximal allowed length is 13B, reset value: 0x6, access type: RW
		uint32 muBarUserInfoLength : 4; //Length (in bytes) of user info field in MU-BAR (includes General User Info + Optional spare section). This length cannot exceed Allocated User Info Template Length configuration. Maximal allowed length is 13B. Note that the Trigger Dependent User Info parameters for MU-BAR are given in another set of configurations, reset value: 0x5, access type: RW
		uint32 muRtsUserInfoLength : 4; //Length (in bytes) of user info field in MU RTS (includes General User Info + Trigger Dependent User Info). This length cannot exceed Allocated User Info Template Length configuration. Maximal allowed length is 13B, reset value: 0x5, access type: RW
		uint32 bsrpUserInfoLength : 4; //Length (in bytes) of user info field in BSRP (includes General User Info + Trigger Dependent User Info). This length cannot exceed Allocated User Info Template Length configuration. Maximal allowed length is 13B, reset value: 0x5, access type: RW
		uint32 reserved0 : 12;
	} bitFields;
} RegTfGeneratorTfGenUserInfo_u;

/*REG_TF_GENERATOR_TF_GEN_MU_BAR_INFO 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muBarControlLength : 3; //Length (in bytes) of BAR Control field. Maximal allowed length is 4B, reset value: 0x2, access type: RW
		uint32 reserved0 : 1;
		uint32 compressedMuBarInformationLength : 3; //Length (in bytes) of BAR information field in compressed BAR variant. Maximal allowed length is 4B, reset value: 0x2, access type: RW
		uint32 reserved1 : 1;
		uint32 multiTidMuBarInformationLength : 4; //Length (in bytes) of BAR information field in Multi-TID BAR variant. Maximal allowed length is 8B, reset value: 0x4, access type: RW
		uint32 reserved2 : 20;
	} bitFields;
} RegTfGeneratorTfGenMuBarInfo_u;

/*REG_TF_GENERATOR_TF_PHASE_ALLOCATED_TEMPLATE 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 allocatedCommonInfoTemplateDwLength : 3; //Allocated Length of the Common Info Template – common for all TF Types. Contains both General Common Info and Trigger Dependent Common Info. The size of this field is given in DWs. , , reset value: 0x2, access type: RW
		uint32 reserved0 : 1;
		uint32 allocatedUserInfoTemplateByteLength : 4; //Allocated Length of the User Info Template – common for all TF Types. Contains both General User Info and Trigger Dependent User Info. The size of this field is given in bytes. , , reset value: 0x6, access type: RW
		uint32 allocatedMuBarTemplateDwLength : 3; //Allocated Length of the MU-BAR Template. Contains BAR Control and BAR Information sections. BAR Information section is given for both Compressed and Multi-TID flavors. The size of this field is given in DWs., reset value: 0x2, access type: RW
		uint32 reserved1 : 21;
	} bitFields;
} RegTfGeneratorTfPhaseAllocatedTemplate_u;

/*REG_TF_GENERATOR_TF_GEN_INT_ERROR_STATUS 0x60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tfrgGoWhileActiveMismatchStatus : 1; //Go while active mismatch occurs if a Go command is issued while theTF Generator is not idle, reset value: 0x0, access type: RO
		uint32 tfrgTfModeMismatchStatus : 1; //TF Mode mismatch event is set when the TF Mode is either Reserved or No_TF, reset value: 0x0, access type: RO
		uint32 tfrgEmptyTidMismatchStatus : 1; //Empty TID mismatch event is set when the Allocated TIDs vector is empty in case of MU-BAR, reset value: 0x0, access type: RO
		uint32 tfrgRuIndexLutOorStatus : 1; //RU Index LUT OOR event is set when the LUT is accessed with Out of Range address (>= 64), reset value: 0x0, access type: RO
		uint32 reserved0 : 28;
	} bitFields;
} RegTfGeneratorTfGenIntErrorStatus_u;

/*REG_TF_GENERATOR_TF_GEN_INT_ERROR_EN 0x64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tfrgGoWhileActiveMismatchEn : 1; //Go while active mismatch interrupt enable, reset value: 0x0, access type: RW
		uint32 tfrgTfModeMismatchEn : 1; //TF Mode mismatch interrupt enable, reset value: 0x0, access type: RW
		uint32 tfrgEmptyTidMismatchEn : 1; //Empty TID Mismatch interrupt enable, reset value: 0x0, access type: RW
		uint32 tfrgRuIndexLutOorEn : 1; //RU Index LUT OOR interrupt enable, reset value: 0x0, access type: RW
		uint32 reserved0 : 28;
	} bitFields;
} RegTfGeneratorTfGenIntErrorEn_u;

/*REG_TF_GENERATOR_TF_GEN_INT_ERROR_CLEAR 0x68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tfrgGoWhileActiveMismatchClr : 1; //Go while active mismatch interrupt clear, reset value: 0x0, access type: WO
		uint32 tfrgTfModeMismatchClr : 1; //TF Mode mismatch interrupt clear, reset value: 0x0, access type: WO
		uint32 tfrgEmptyTidMismatchClr : 1; //Empty TID Mismatch interrupt clear, reset value: 0x0, access type: WO
		uint32 tfrgRuIndexLutOorClr : 1; //RU Index LUT OOR interrupt clear, reset value: 0x0, access type: WO
		uint32 reserved0 : 28;
	} bitFields;
} RegTfGeneratorTfGenIntErrorClear_u;

/*REG_TF_GENERATOR_TF_GEN_TRIGGER_TYPE_PARAMS 0x70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 triggerTypeMask : 4; //Mask for Trigger Type field, reset value: 0xf, access type: RW
		uint32 reserved0 : 4;
		uint32 triggerTypeOffset : 7; //Bit Offset for Trigger Type field. Offset is from start of Common Info section, reset value: 0x0, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegTfGeneratorTfGenTriggerTypeParams_u;

/*REG_TF_GENERATOR_TF_GEN_LSIG_LENGTH_PARAMS 0x74 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lsigLengthMask : 12; //Mask for L-SIG Length field, reset value: 0xfff, access type: RW
		uint32 reserved0 : 4;
		uint32 lsigLengthOffset : 7; //Bit Offset for L-SIG Length field. Offset is from start of Common Info section, reset value: 0x4, access type: RW
		uint32 reserved1 : 9;
	} bitFields;
} RegTfGeneratorTfGenLsigLengthParams_u;

/*REG_TF_GENERATOR_TF_GEN_CASCADE_BW_PARAMS 0x78 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cascadeIndicationMask : 1; //Mask for Cascade Indication field, reset value: 0x1, access type: RW
		uint32 reserved0 : 3;
		uint32 cascadeIndicationOffset : 7; //Bit Offset for Cascade Indication field. Offset is from start of Common Info section, reset value: 0x10, access type: RW
		uint32 reserved1 : 1;
		uint32 bwMask : 2; //Mask for BW field, reset value: 0x3, access type: RW
		uint32 reserved2 : 2;
		uint32 bwOffset : 7; //Bit Offset for BW field. Offset is from start of Common Info section, reset value: 0x12, access type: RW
		uint32 reserved3 : 9;
	} bitFields;
} RegTfGeneratorTfGenCascadeBwParams_u;

/*REG_TF_GENERATOR_TF_GEN_AID12_PARAMS 0x84 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aid12Mask : 12; //Mask for AID12 field, reset value: 0xfff, access type: RW
		uint32 reserved0 : 4;
		uint32 aid12Offset : 7; //Bit Offset for AID12 field. Offset is from start of User Info section, reset value: 0x0, access type: RW
		uint32 reserved1 : 9;
	} bitFields;
} RegTfGeneratorTfGenAid12Params_u;

/*REG_TF_GENERATOR_TF_GEN_RU_ALLOCATION_PARAMS 0x88 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ruPrimarySecondary80Mask : 1; //Mask for RU Primary/Secondary 80 field, reset value: 0x1, access type: RW
		uint32 ruPrimarySecondary80Offset : 7; //Bit Offset for RU Primary/Secondary 80 field. Offset is from start of User Info section, reset value: 0xc, access type: RW
		uint32 ruIndexMask : 7; //Mask for RU Index field, reset value: 0x7f, access type: RW
		uint32 reserved0 : 1;
		uint32 ruIndexOffset : 7; //Bit Offset for RU Index field. Offset is from start of User Info section, reset value: 0xd, access type: RW
		uint32 reserved1 : 9;
	} bitFields;
} RegTfGeneratorTfGenRuAllocationParams_u;

/*REG_TF_GENERATOR_TF_GEN_MCS_DCM_NSS_PARAMS 0x8C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mcsMask : 4; //Mask for MCS field, reset value: 0xf, access type: RW
		uint32 mcsOffset : 7; //Bit Offset for MCS field. Offset is from start of User Info section, reset value: 0x15, access type: RW
		uint32 reserved0 : 1;
		uint32 dcmMask : 1; //Mask for DCM field, reset value: 0x1, access type: RW
		uint32 dcmOffset : 7; //Bit Offset for DCM field. Offset is from start of User Info section, reset value: 0x19, access type: RW
		uint32 nssMask : 3; //Mask for NSS field, reset value: 0x7, access type: RW
		uint32 reserved1 : 1;
		uint32 nssOffset : 7; //Bit Offset for NSS field. Offset is from start of User Info section, reset value: 0x1d, access type: RW
		uint32 reserved2 : 1;
	} bitFields;
} RegTfGeneratorTfGenMcsDcmNssParams_u;

/*REG_TF_GENERATOR_TF_GEN_TARGET_RSSI_PARAMS 0x90 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 targetRssiMask : 8; //Mask for Target RSSI field, reset value: 0x7f, access type: RW
		uint32 targetRssiOffset : 7; //Bit Offset for Target RSSI field. Offset is from start of User Info section, reset value: 0x20, access type: RW
		uint32 reserved0 : 17;
	} bitFields;
} RegTfGeneratorTfGenTargetRssiParams_u;

/*REG_TF_GENERATOR_TF_GEN_BASIC_TRIGGER_USER_PARAMS 0x94 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 basicTriggerTidAggLimitMask : 3; //Mask for TID Aggregation Limit field in case of Basic Trigger, reset value: 0x7, access type: RW
		uint32 reserved0 : 1;
		uint32 basicTriggerTidAggLimitOffset : 7; //Bit Offset for TID Aggregation Limit field in case of Basic Trigger. Offset is from start of (General) User Info section, reset value: 0x2a, access type: RW
		uint32 reserved1 : 1;
		uint32 basicTriggerPreferredAcMask : 2; //Mask for Preferred AC field in case of Basic Trigger, reset value: 0x3, access type: RW
		uint32 reserved2 : 2;
		uint32 basicTriggerPreferredAcOffset : 7; //Bit Offset for Preferred AC field in case of Basic Trigger. Offset is from start of (General) User Info section, reset value: 0x2e, access type: RW
		uint32 reserved3 : 9;
	} bitFields;
} RegTfGeneratorTfGenBasicTriggerUserParams_u;

/*REG_TF_GENERATOR_TF_GEN_MU_BAR_CONTROL_PARAMS 0x98 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muBarControlMultiTidMask : 1; //Mask for Multi-TID field in case of MU-BAR Control, reset value: 0x1, access type: RW
		uint32 reserved0 : 3;
		uint32 muBarControlMultiTidOffset : 7; //Bit Offset for Multi-TID field in case of MU-BAR Control. Offset is from start of (General) User Info section, reset value: 0x29, access type: RW
		uint32 reserved1 : 1;
		uint32 muBarControlTidInfoMask : 4; //Mask for TID INFO field in case of MU-BAR Control, reset value: 0xf, access type: RW
		uint32 muBarControlTidInfoOffset : 7; //Bit Offset for TID INFO field in case of MU-BAR Control. Offset is from start of (General) User Info section, reset value: 0x34, access type: RW
		uint32 reserved2 : 9;
	} bitFields;
} RegTfGeneratorTfGenMuBarControlParams_u;

/*REG_TF_GENERATOR_TF_GEN_COMPRESSED_MU_BAR_INFORMATION_PARAMS 0x9C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 compressedMuBarInformationFnMask : 2; //Mask for FN[2:1] field in case of Compressed MU-BAR Information, reset value: 0x3, access type: RW
		uint32 reserved0 : 2;
		uint32 compressedMuBarInformationFnOffset : 5; //Bit Offset for FN[2:1] field in case of Compressed MU-BAR Information. Offset is from start of MU-BAR Information section, reset value: 0x1, access type: RW
		uint32 reserved1 : 3;
		uint32 compressedMuBarInformationSsnMask : 12; //Mask for SSN field in case of Compressed MU-BAR Information, reset value: 0xfff, access type: RW
		uint32 compressedMuBarInformationSsnOffset : 5; //Bit Offset for SSN field in case of Compressed MU-BAR Information. Offset is from start of MU-BAR Information section, reset value: 0x4, access type: RW
		uint32 reserved2 : 3;
	} bitFields;
} RegTfGeneratorTfGenCompressedMuBarInformationParams_u;

/*REG_TF_GENERATOR_TF_GEN_MULTI_TID_MU_BAR_INFORMATION_PARAMS_0 0xA0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 multiTidMuBarInformationTidMask : 4; //Mask for TID field in case of Multi-TID MU-BAR Information, reset value: 0xf, access type: RW
		uint32 multiTidMuBarInformationTidOffset : 6; //Bit Offset for TID field in case of Multi-TID MU-BAR Information. Offset is from start of MU-BAR Information section, reset value: 0xc, access type: RW
		uint32 reserved0 : 22;
	} bitFields;
} RegTfGeneratorTfGenMultiTidMuBarInformationParams0_u;

/*REG_TF_GENERATOR_TF_GEN_MULTI_TID_MU_BAR_INFORMATION_PARAMS_1 0xA4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 multiTidMuBarInformationFnMask : 2; //Mask for FN[2:1] field in case of Multi-TID MU-BAR Information, reset value: 0x3, access type: RW
		uint32 reserved0 : 2;
		uint32 multiTidMuBarInformationFnOffset : 6; //Bit Offset for FN[2:1] field in case of Multi-TID MU-BAR Information. Offset is from start of MU-BAR Information section, reset value: 0x11, access type: RW
		uint32 reserved1 : 2;
		uint32 multiTidMuBarInformationSsnMask : 12; //Mask for SSN field in case of Multi-TID MU-BAR Information, reset value: 0xfff, access type: RW
		uint32 multiTidMuBarInformationSsnOffset : 6; //Bit Offset for SSN field in case of Multi-TID MU-BAR Information. Offset is from start of MU-BAR Information section, reset value: 0x14, access type: RW
		uint32 reserved2 : 2;
	} bitFields;
} RegTfGeneratorTfGenMultiTidMuBarInformationParams1_u;

/*REG_TF_GENERATOR_TF_GEN_SW_OVERRIDE_OFFSET 0xA8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 commonInfoSwOverrideOffset : 7; //Common Info General SW Override Offset (in bits from start of Common Info section). Valid override bits must be within the Common Info Template boundary, reset value: 0x0, access type: RW
		uint32 reserved0 : 1;
		uint32 userInfoSwOverrideOffset : 7; //User Info General SW Override Offset (in bits from start of General User Info section). Valid override bits must be within the User Info Template boundary, reset value: 0x0, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegTfGeneratorTfGenSwOverrideOffset_u;

/*REG_TF_GENERATOR_TF_GEN_COMMON_INFO_SW_OVERRIDE_VALUE 0xAC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 commonInfoSwOverrideValue : 32; //Common Info General SW Override Value, reset value: 0x0, access type: RW
	} bitFields;
} RegTfGeneratorTfGenCommonInfoSwOverrideValue_u;

/*REG_TF_GENERATOR_TF_GEN_COMMON_INFO_SW_OVERRIDE_MASK 0xB0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 commonInfoSwOverrideMask : 32; //Common Info General SW Override mask, reset value: 0x0, access type: RW
	} bitFields;
} RegTfGeneratorTfGenCommonInfoSwOverrideMask_u;

/*REG_TF_GENERATOR_TF_GEN_USER_INFO_SW_OVERRIDE_VALUE 0xB4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 userInfoSwOverrideValue : 32; //User Info General SW Override Value, reset value: 0x0, access type: RW
	} bitFields;
} RegTfGeneratorTfGenUserInfoSwOverrideValue_u;

/*REG_TF_GENERATOR_TF_GEN_USER_INFO_SW_OVERRIDE_MASK 0xB8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 userInfoSwOverrideMask : 32; //User Info General SW Override mask, reset value: 0x0, access type: RW
	} bitFields;
} RegTfGeneratorTfGenUserInfoSwOverrideMask_u;

/*REG_TF_GENERATOR_TF_GEN_STATS_0 0xBC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 userForByteCount : 7; //User for which the number of generated bytes per GO command is counted, reset value: 0x0, access type: RW
		uint32 reserved0 : 25;
	} bitFields;
} RegTfGeneratorTfGenStats0_u;

/*REG_TF_GENERATOR_TF_GEN_CLEAR_BITS 0xC0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 1;
		uint32 tfGenNumOfGoCommandsClear : 1; //Write 1 to this field in order to clear num_of_go_commands counter, reset value: 0x0, access type: WO
		uint32 tfGenByteCountersClear : 1; //Write 1 to this field in order to clear num_of_bytes_user_i counters, reset value: 0x0, access type: WO
		uint32 reserved1 : 29;
	} bitFields;
} RegTfGeneratorTfGenClearBits_u;

/*REG_TF_GENERATOR_TF_GEN_STATUS_0 0xC4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tfGenIsIdle : 1; //Indication that TF Generator block is Idle, reset value: 0x1, access type: RO
		uint32 reserved0 : 11;
		uint32 tfrgCoreSmDbg : 2; //TFRG_CORE SM status, reset value: 0x0, access type: RO
		uint32 reserved1 : 2;
		uint32 tfrgStdInitSmDbg : 3; //TFRG_STD_INIT SM status, reset value: 0x0, access type: RO
		uint32 reserved2 : 1;
		uint32 tfrgRcrInitSmDbg : 1; //TFRG_RCR_INIT SM status, reset value: 0x0, access type: RO
		uint32 tfrgPhaseInitSmDbg : 3; //TFRG_PHASE_INIT SM status, reset value: 0x0, access type: RO
		uint32 tfrgCmnInfoSmDbg : 2; //TFRG_CMN_INFO SM status, reset value: 0x0, access type: RO
		uint32 reserved3 : 6;
	} bitFields;
} RegTfGeneratorTfGenStatus0_u;

/*REG_TF_GENERATOR_TF_GEN_STATUS_1 0xC8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tfrgUserMngrSmDbg : 2; //TFRG_USER_MNGR SM status, reset value: 0x0, access type: RO
		uint32 tfrgUserGetDataSmDbg : 2; //TFRG_USER_GET_DATA SM status, reset value: 0x0, access type: RO
		uint32 tfrgStdUserSmDbg : 3; //TFRG_STD_USER SM status, reset value: 0x0, access type: RO
		uint32 tfrgRcrUserSmDbg : 1; //TFRG_RCR_USER SM status, reset value: 0x0, access type: RO
		uint32 tfrgPhaseUserSmDbg : 2; //TFRG_PHASE_USER SM status, reset value: 0x0, access type: RO
		uint32 tfrgTidMngrSmDbg : 2; //TFRG_TID_MNGR SM status, reset value: 0x0, access type: RO
		uint32 tfrgTidGetDataSmDbg : 2; //TFRG_TID_GET_DATA SM status, reset value: 0x0, access type: RO
		uint32 reserved0 : 2;
		uint32 tfrgStdTidSmDbg : 3; //TFRG_STD_TID SM status, reset value: 0x0, access type: RO
		uint32 reserved1 : 1;
		uint32 tfrgTidInfoSmDbg : 2; //TFRG_TID_INFO SM status, reset value: 0x0, access type: RO
		uint32 tfrgUsrInfoSmDbg : 2; //TFRG_USR_INFO SM status, reset value: 0x0, access type: RO
		uint32 tfrgWrMngrSmDbg : 3; //TFRG_WR_MNGR SM status, reset value: 0x0, access type: RO
		uint32 reserved2 : 1;
		uint32 tfrgWrCycBufSmDbg : 2; //TFRG_WR_CYC_BUF SM status, reset value: 0x0, access type: RO
		uint32 reserved3 : 2;
	} bitFields;
} RegTfGeneratorTfGenStatus1_u;

/*REG_TF_GENERATOR_TF_GEN_STATUS_2 0xCC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 numOfGoCommands : 32; //Number of issued Go commands (Cleared by writing to tf_gen_num_of_go_commands_clear), reset value: 0x0, access type: RO
	} bitFields;
} RegTfGeneratorTfGenStatus2_u;

/*REG_TF_GENERATOR_TF_GEN_STATUS_3 0xD0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 numOfBytesUser0 : 15; //Number of bytes generated for user 0. The bytes are counted per Go command (if the user exists. Otherwise, the counter retains its value). The counter can be cleared by tf_gen_byte_counters_clear field, reset value: 0x0, access type: RO
		uint32 reserved0 : 1;
		uint32 numOfBytesUserX : 15; //Number of bytes generated for user X. X is taken from user_for_byte_count configuration. The bytes are counted per Go command (if the user exists. Otherwise, the counter retains its value). The counter can be cleared by tf_gen_byte_counters_clear field, reset value: 0x0, access type: RO
		uint32 reserved1 : 1;
	} bitFields;
} RegTfGeneratorTfGenStatus3_u;

/*REG_TF_GENERATOR_TF_GEN_LOGGER_CONTROL 0xD4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tfGenLoggerEn : 1; //Logger Enable, reset value: 0x0, access type: RW
		uint32 reserved0 : 1;
		uint32 tfGenLoggerPriority : 2; //TF_GEN Logger priority, reset value: 0x0, access type: RW
		uint32 reserved1 : 28;
	} bitFields;
} RegTfGeneratorTfGenLoggerControl_u;

/*REG_TF_GENERATOR_TF_GEN_LOGGER_STATUS 0xD8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tfGenLoggerIdle : 1; //Indication that the logger is IDLE. Use this indication after the logger is disabled, in order to verify that it has finished any work in progress., reset value: 0x1, access type: RO
		uint32 tfGenLoggerSm : 3; //State of the logger state machine, reset value: 0x0, access type: RO
		uint32 reserved0 : 28;
	} bitFields;
} RegTfGeneratorTfGenLoggerStatus_u;

/*REG_TF_GENERATOR_TF_GEN_SPARE_REGISTER 0xE0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 spareRegister : 16; //Spare Register, reset value: 0x8888, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegTfGeneratorTfGenSpareRegister_u;



#endif // _TF_GENERATOR_REGS_H_
