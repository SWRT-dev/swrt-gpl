
/***********************************************************************************
File:				UlPostRxRegs.h
Module:				UlPostRx
SOC Revision:		854
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _UL_POST_RX_REGS_H_
#define _UL_POST_RX_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define UL_POST_RX_BASE_ADDRESS                             MEMORY_MAP_UNIT_43_BASE_ADDRESS
#define	REG_UL_POST_RX_VAP_HE_VARIANT_SUPPORT_BITMAP             (UL_POST_RX_BASE_ADDRESS + 0x0)
#define	REG_UL_POST_RX_STA_HE_VARIANT_SUPPORT_BITMAP_31_0        (UL_POST_RX_BASE_ADDRESS + 0x4)
#define	REG_UL_POST_RX_STA_HE_VARIANT_SUPPORT_BITMAP_63_32       (UL_POST_RX_BASE_ADDRESS + 0x8)
#define	REG_UL_POST_RX_STA_HE_VARIANT_SUPPORT_BITMAP_95_64       (UL_POST_RX_BASE_ADDRESS + 0xC)
#define	REG_UL_POST_RX_STA_HE_VARIANT_SUPPORT_BITMAP_127_96      (UL_POST_RX_BASE_ADDRESS + 0x10)
#define	REG_UL_POST_RX_STA_HE_VARIANT_SUPPORT_BITMAP_159_128     (UL_POST_RX_BASE_ADDRESS + 0x14)
#define	REG_UL_POST_RX_STA_HE_VARIANT_SUPPORT_BITMAP_191_160     (UL_POST_RX_BASE_ADDRESS + 0x18)
#define	REG_UL_POST_RX_STA_HE_VARIANT_SUPPORT_BITMAP_223_192     (UL_POST_RX_BASE_ADDRESS + 0x1C)
#define	REG_UL_POST_RX_STA_HE_VARIANT_SUPPORT_BITMAP_255_224     (UL_POST_RX_BASE_ADDRESS + 0x20)
#define	REG_UL_POST_RX_CONTROL_INFORMATION_LENGTH_ARRAY_3_0      (UL_POST_RX_BASE_ADDRESS + 0x24)
#define	REG_UL_POST_RX_CONTROL_INFORMATION_LENGTH_ARRAY_7_4      (UL_POST_RX_BASE_ADDRESS + 0x28)
#define	REG_UL_POST_RX_CONTROL_INFORMATION_LENGTH_ARRAY_11_8     (UL_POST_RX_BASE_ADDRESS + 0x2C)
#define	REG_UL_POST_RX_CONTROL_INFORMATION_LENGTH_ARRAY_15_12    (UL_POST_RX_BASE_ADDRESS + 0x30)
#define	REG_UL_POST_RX_A_CONTROL_FIELD_HW_PARSING_ENABLE         (UL_POST_RX_BASE_ADDRESS + 0x34)
#define	REG_UL_POST_RX_A_CONTROL_FIELD_SW_REPORT_ENABLE          (UL_POST_RX_BASE_ADDRESS + 0x38)
#define	REG_UL_POST_RX_DL_MU_SUSPENSION_BIT_ENABLE               (UL_POST_RX_BASE_ADDRESS + 0x3C)
#define	REG_UL_POST_RX_HE_MU_TB_REPORT_ENABLE                    (UL_POST_RX_BASE_ADDRESS + 0x40)
#define	REG_UL_POST_RX_HE_SU_REPORT_ENABLE                       (UL_POST_RX_BASE_ADDRESS + 0x44)
#define	REG_UL_POST_RX_ULPR_CONTROL                              (UL_POST_RX_BASE_ADDRESS + 0x48)
#define	REG_UL_POST_RX_ULPR_COMMON_FIFO_BASE_ADDR                (UL_POST_RX_BASE_ADDRESS + 0x50)
#define	REG_UL_POST_RX_ULPR_COMMON_FIFO_DEPTH_MINUS_ONE          (UL_POST_RX_BASE_ADDRESS + 0x54)
#define	REG_UL_POST_RX_ULPR_COMMON_FIFO_CLEAR_STRB               (UL_POST_RX_BASE_ADDRESS + 0x58)
#define	REG_UL_POST_RX_ULPR_COMMON_FIFO_RD_ENTRIES_NUM           (UL_POST_RX_BASE_ADDRESS + 0x5C)
#define	REG_UL_POST_RX_ULPR_COMMON_FIFO_NUM_ENTRIES_COUNT        (UL_POST_RX_BASE_ADDRESS + 0x60)
#define	REG_UL_POST_RX_ULPR_COMMON_FIFO_DEBUG                    (UL_POST_RX_BASE_ADDRESS + 0x64)
#define	REG_UL_POST_RX_ULPR_USER_FIFO_BASE_ADDR                  (UL_POST_RX_BASE_ADDRESS + 0x68)
#define	REG_UL_POST_RX_ULPR_USER_FIFO_DEPTH_MINUS_ONE            (UL_POST_RX_BASE_ADDRESS + 0x6C)
#define	REG_UL_POST_RX_ULPR_USER_FIFO_CLEAR_STRB                 (UL_POST_RX_BASE_ADDRESS + 0x70)
#define	REG_UL_POST_RX_ULPR_USER_FIFO_RD_ENTRIES_NUM             (UL_POST_RX_BASE_ADDRESS + 0x74)
#define	REG_UL_POST_RX_ULPR_USER_FIFO_NUM_ENTRIES_COUNT          (UL_POST_RX_BASE_ADDRESS + 0x78)
#define	REG_UL_POST_RX_ULPR_USER_FIFO_DEBUG                      (UL_POST_RX_BASE_ADDRESS + 0x7C)
#define	REG_UL_POST_RX_ULPR_INT_ERROR_STATUS                     (UL_POST_RX_BASE_ADDRESS + 0x80)
#define	REG_UL_POST_RX_ULPR_INT_ERROR_EN                         (UL_POST_RX_BASE_ADDRESS + 0x84)
#define	REG_UL_POST_RX_ULPR_INT_ERROR_CLEAR                      (UL_POST_RX_BASE_ADDRESS + 0x88)
#define	REG_UL_POST_RX_ULPR_FIFO_NOT_EMPTY_STATUS                (UL_POST_RX_BASE_ADDRESS + 0x90)
#define	REG_UL_POST_RX_ULPR_FIFO_NOT_EMPTY_EN                    (UL_POST_RX_BASE_ADDRESS + 0x94)
#define	REG_UL_POST_RX_ULPR_CLEAR_BITS                           (UL_POST_RX_BASE_ADDRESS + 0x98)
#define	REG_UL_POST_RX_ULPR_STATUS_0                             (UL_POST_RX_BASE_ADDRESS + 0x9C)
#define	REG_UL_POST_RX_ULPR_SPARE_REGISTER                       (UL_POST_RX_BASE_ADDRESS + 0xB0)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_UL_POST_RX_VAP_HE_VARIANT_SUPPORT_BITMAP 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vapHeVariantSupportBitmap : 32; //This bitmap designates whether the VAP ID (to which the Rx user belongs) supports the reception of HE Variant HT Control word. A-Control analysis can take place only if the respective bit for a given user is set. There is 1b per VAP, reset value: 0x0, access type: RW
	} bitFields;
} RegUlPostRxVapHeVariantSupportBitmap_u;

/*REG_UL_POST_RX_STA_HE_VARIANT_SUPPORT_BITMAP_31_0 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 staHeVariantSupportBitmap310 : 32; //This bitmap designates whether the STA ID (with which the Rx user is affiliated) supports the reception of HE Variant HT Control word. A-Control analysis can take place only if the respective bit for a given user is set. This vector designates Station Indexes 31:0. There is 1b per STA, reset value: 0x0, access type: RW
	} bitFields;
} RegUlPostRxStaHeVariantSupportBitmap310_u;

/*REG_UL_POST_RX_STA_HE_VARIANT_SUPPORT_BITMAP_63_32 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 staHeVariantSupportBitmap6332 : 32; //This bitmap designates whether the STA ID (with which the Rx user is affiliated) supports the reception of HE Variant HT Control word. A-Control analysis can take place only if the respective bit for a given user is set. This vector designates Station Indexes 63:32. There is 1b per STA, reset value: 0x0, access type: RW
	} bitFields;
} RegUlPostRxStaHeVariantSupportBitmap6332_u;

/*REG_UL_POST_RX_STA_HE_VARIANT_SUPPORT_BITMAP_95_64 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 staHeVariantSupportBitmap9564 : 32; //This bitmap designates whether the STA ID (with which the Rx user is affiliated) supports the reception of HE Variant HT Control word. A-Control analysis can take place only if the respective bit for a given user is set. This vector designates Station Indexes 95:64. There is 1b per STA, reset value: 0x0, access type: RW
	} bitFields;
} RegUlPostRxStaHeVariantSupportBitmap9564_u;

/*REG_UL_POST_RX_STA_HE_VARIANT_SUPPORT_BITMAP_127_96 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 staHeVariantSupportBitmap12796 : 32; //This bitmap designates whether the STA ID (with which the Rx user is affiliated) supports the reception of HE Variant HT Control word. A-Control analysis can take place only if the respective bit for a given user is set. This vector designates Station Indexes 127:96. There is 1b per STA, reset value: 0x0, access type: RW
	} bitFields;
} RegUlPostRxStaHeVariantSupportBitmap12796_u;

/*REG_UL_POST_RX_STA_HE_VARIANT_SUPPORT_BITMAP_159_128 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 staHeVariantSupportBitmap159128 : 32; //This bitmap designates whether the STA ID (with which the Rx user is affiliated) supports the reception of HE Variant HT Control word. A-Control analysis can take place only if the respective bit for a given user is set. This vector designates Station Indexes 159:128. There is 1b per STA, reset value: 0x0, access type: RW
	} bitFields;
} RegUlPostRxStaHeVariantSupportBitmap159128_u;

/*REG_UL_POST_RX_STA_HE_VARIANT_SUPPORT_BITMAP_191_160 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 staHeVariantSupportBitmap191160 : 32; //This bitmap designates whether the STA ID (with which the Rx user is affiliated) supports the reception of HE Variant HT Control word. A-Control analysis can take place only if the respective bit for a given user is set. This vector designates Station Indexes 191:160. There is 1b per STA, reset value: 0x0, access type: RW
	} bitFields;
} RegUlPostRxStaHeVariantSupportBitmap191160_u;

/*REG_UL_POST_RX_STA_HE_VARIANT_SUPPORT_BITMAP_223_192 0x1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 staHeVariantSupportBitmap223192 : 32; //This bitmap designates whether the STA ID (with which the Rx user is affiliated) supports the reception of HE Variant HT Control word. A-Control analysis can take place only if the respective bit for a given user is set. This vector designates Station Indexes 223:192. There is 1b per STA, reset value: 0x0, access type: RW
	} bitFields;
} RegUlPostRxStaHeVariantSupportBitmap223192_u;

/*REG_UL_POST_RX_STA_HE_VARIANT_SUPPORT_BITMAP_255_224 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 staHeVariantSupportBitmap255224 : 32; //This bitmap designates whether the STA ID (with which the Rx user is affiliated) supports the reception of HE Variant HT Control word. A-Control analysis can take place only if the respective bit for a given user is set. This vector designates Station Indexes 255:224. There is 1b per STA, reset value: 0x0, access type: RW
	} bitFields;
} RegUlPostRxStaHeVariantSupportBitmap255224_u;

/*REG_UL_POST_RX_CONTROL_INFORMATION_LENGTH_ARRAY_3_0 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 controlInformationLength0 : 5; //Length (in bits) of the Control Information subfield (within the A-Control word) for Control ID 0., reset value: 0x1a, access type: RW
		uint32 reserved0 : 3;
		uint32 controlInformationLength1 : 5; //Length (in bits) of the Control Information subfield (within the A-Control word) for Control ID 1., reset value: 0xc, access type: RW
		uint32 reserved1 : 3;
		uint32 controlInformationLength2 : 5; //Length (in bits) of the Control Information subfield (within the A-Control word) for Control ID 2., reset value: 0x10, access type: RW
		uint32 reserved2 : 3;
		uint32 controlInformationLength3 : 5; //Length (in bits) of the Control Information subfield (within the A-Control word) for Control ID 3., reset value: 0x1a, access type: RW
		uint32 reserved3 : 3;
	} bitFields;
} RegUlPostRxControlInformationLengthArray30_u;

/*REG_UL_POST_RX_CONTROL_INFORMATION_LENGTH_ARRAY_7_4 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 controlInformationLength4 : 5; //Length (in bits) of the Control Information subfield (within the A-Control word) for Control ID 4., reset value: 0x8, access type: RW
		uint32 reserved0 : 3;
		uint32 controlInformationLength5 : 5; //Length (in bits) of the Control Information subfield (within the A-Control word) for Control ID 5., reset value: 0xa, access type: RW
		uint32 reserved1 : 3;
		uint32 controlInformationLength6 : 5; //Length (in bits) of the Control Information subfield (within the A-Control word) for Control ID 6., reset value: 0x8, access type: RW
		uint32 reserved2 : 3;
		uint32 controlInformationLength7 : 5; //Length (in bits) of the Control Information subfield (within the A-Control word) for Control ID 7., reset value: 0x1f, access type: RW
		uint32 reserved3 : 3;
	} bitFields;
} RegUlPostRxControlInformationLengthArray74_u;

/*REG_UL_POST_RX_CONTROL_INFORMATION_LENGTH_ARRAY_11_8 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 controlInformationLength8 : 5; //Length (in bits) of the Control Information subfield (within the A-Control word) for Control ID 8., reset value: 0x1f, access type: RW
		uint32 reserved0 : 3;
		uint32 controlInformationLength9 : 5; //Length (in bits) of the Control Information subfield (within the A-Control word) for Control ID 9., reset value: 0x1f, access type: RW
		uint32 reserved1 : 3;
		uint32 controlInformationLength10 : 5; //Length (in bits) of the Control Information subfield (within the A-Control word) for Control ID 10., reset value: 0x1f, access type: RW
		uint32 reserved2 : 3;
		uint32 controlInformationLength11 : 5; //Length (in bits) of the Control Information subfield (within the A-Control word) for Control ID 11., reset value: 0x1f, access type: RW
		uint32 reserved3 : 3;
	} bitFields;
} RegUlPostRxControlInformationLengthArray118_u;

/*REG_UL_POST_RX_CONTROL_INFORMATION_LENGTH_ARRAY_15_12 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 controlInformationLength12 : 5; //Length (in bits) of the Control Information subfield (within the A-Control word) for Control ID 12., reset value: 0x1f, access type: RW
		uint32 reserved0 : 3;
		uint32 controlInformationLength13 : 5; //Length (in bits) of the Control Information subfield (within the A-Control word) for Control ID 13., reset value: 0x1f, access type: RW
		uint32 reserved1 : 3;
		uint32 controlInformationLength14 : 5; //Length (in bits) of the Control Information subfield (within the A-Control word) for Control ID 14., reset value: 0x1f, access type: RW
		uint32 reserved2 : 3;
		uint32 controlInformationLength15 : 5; //Length (in bits) of the Control Information subfield (within the A-Control word) for Control ID 15., reset value: 0x1f, access type: RW
		uint32 reserved3 : 3;
	} bitFields;
} RegUlPostRxControlInformationLengthArray1512_u;

/*REG_UL_POST_RX_A_CONTROL_FIELD_HW_PARSING_ENABLE 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aControlFieldHwParsingEnable : 16; //This bitmap designates whether the HW shall process (and parse) the Control Information subfield for each Control ID. It is used for the A-Control Analysis section. There is 1b per CTRL_ID, reset value: 0x2, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegUlPostRxAControlFieldHwParsingEnable_u;

/*REG_UL_POST_RX_A_CONTROL_FIELD_SW_REPORT_ENABLE 0x38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aControlFieldSwReportEnable : 16; //This bitmap designates whether a Control ID requires a SW report to be generated for it. It is used for the UL Report section. There is 1b per CTRL_ID, reset value: 0x12, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegUlPostRxAControlFieldSwReportEnable_u;

/*REG_UL_POST_RX_DL_MU_SUSPENSION_BIT_ENABLE 0x3C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dlMuSuspensionBitEnable : 1; //This global configuration controls the value that is written to DL MU Disable field within the TX Selector, based on the UL MU Disable bit, which is extracted from the Control Information subfield in case (CTRL_ID == 1) is analyzed. 0 – DL MU Disable is set to 0;  1 - DL MU Disable is set to the value of UL MU Disable, reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegUlPostRxDlMuSuspensionBitEnable_u;

/*REG_UL_POST_RX_HE_MU_TB_REPORT_ENABLE 0x40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 heMuTbReportEnable : 1; //This global configuration controls whether a report shall be generated in case (PHY Mode == 6), reset value: 0x1, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegUlPostRxHeMuTbReportEnable_u;

/*REG_UL_POST_RX_HE_SU_REPORT_ENABLE 0x44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 heSuReportEnable : 1; //This global configuration controls (among other conditions) whether a report shall be generated in case (PHY Mode == 4,5), reset value: 0x1, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegUlPostRxHeSuReportEnable_u;

/*REG_UL_POST_RX_ULPR_CONTROL 0x48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ulprEnable : 1; //This global configuration enables the operation of the block. When disabled, the block shall remain IDLE, reset value: 0x1, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegUlPostRxUlprControl_u;

/*REG_UL_POST_RX_ULPR_COMMON_FIFO_BASE_ADDR 0x50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ulprCommonFifoBaseAddr : 22; //ULPR common FIFO base address, reset value: 0x0, access type: RW
		uint32 reserved0 : 10;
	} bitFields;
} RegUlPostRxUlprCommonFifoBaseAddr_u;

/*REG_UL_POST_RX_ULPR_COMMON_FIFO_DEPTH_MINUS_ONE 0x54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ulprCommonFifoDepthMinusOne : 9; //ULPR common FIFO depth minus one, reset value: 0x1ff, access type: RW
		uint32 reserved0 : 23;
	} bitFields;
} RegUlPostRxUlprCommonFifoDepthMinusOne_u;

/*REG_UL_POST_RX_ULPR_COMMON_FIFO_CLEAR_STRB 0x58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ulprCommonFifoClearStrb : 1; //Clear ULPR common FIFO. , By writing '1'., reset value: 0x0, access type: WO
		uint32 ulprCommonFifoClearFullDropCtrStrb : 1; //Clear ULPR common FIFO full drop counter. , By writing '1'., reset value: 0x0, access type: WO
		uint32 ulprCommonFifoClearDecLessThanZeroStrb : 1; //Clear ULPR common FIFO decrement amount less than zero indication. , By writing '1'., reset value: 0x0, access type: WO
		uint32 reserved0 : 29;
	} bitFields;
} RegUlPostRxUlprCommonFifoClearStrb_u;

/*REG_UL_POST_RX_ULPR_COMMON_FIFO_RD_ENTRIES_NUM 0x5C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ulprCommonFifoRdEntriesNum : 9; //ULPR common FIFO number of entries to decrement., reset value: 0x0, access type: RW
		uint32 reserved0 : 23;
	} bitFields;
} RegUlPostRxUlprCommonFifoRdEntriesNum_u;

/*REG_UL_POST_RX_ULPR_COMMON_FIFO_NUM_ENTRIES_COUNT 0x60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ulprCommonFifoNumEntriesCount : 10; //ULPR common FIFO number of entries count, reset value: 0x0, access type: RO
		uint32 reserved0 : 22;
	} bitFields;
} RegUlPostRxUlprCommonFifoNumEntriesCount_u;

/*REG_UL_POST_RX_ULPR_COMMON_FIFO_DEBUG 0x64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ulprCommonFifoWrPtr : 9; //ULPR common FIFO write pointer index, reset value: 0x0, access type: RO
		uint32 reserved0 : 3;
		uint32 ulprCommonFifoNotEmpty : 1; //ULPR common FIFO not empty indication, reset value: 0x0, access type: RO
		uint32 ulprCommonFifoFull : 1; //ULPR common FIFO full indication, reset value: 0x0, access type: RO
		uint32 ulprCommonFifoDecrementLessThanZero : 1; //ULPR common report setting FIFO amount has been decremented less than zero, reset value: 0x0, access type: RO
		uint32 reserved1 : 5;
		uint32 ulprCommonFifoFullDropCtr : 9; //ULPR common report setting FIFO full drop counter, reset value: 0x0, access type: RO
		uint32 reserved2 : 3;
	} bitFields;
} RegUlPostRxUlprCommonFifoDebug_u;

/*REG_UL_POST_RX_ULPR_USER_FIFO_BASE_ADDR 0x68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ulprUserFifoBaseAddr : 22; //ULPR user FIFO base address, reset value: 0x0, access type: RW
		uint32 reserved0 : 10;
	} bitFields;
} RegUlPostRxUlprUserFifoBaseAddr_u;

/*REG_UL_POST_RX_ULPR_USER_FIFO_DEPTH_MINUS_ONE 0x6C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ulprUserFifoDepthMinusOne : 9; //ULPR user FIFO depth minus one, reset value: 0x1ff, access type: RW
		uint32 reserved0 : 23;
	} bitFields;
} RegUlPostRxUlprUserFifoDepthMinusOne_u;

/*REG_UL_POST_RX_ULPR_USER_FIFO_CLEAR_STRB 0x70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ulprUserFifoClearStrb : 1; //Clear ULPR user FIFO. , By writing '1'., reset value: 0x0, access type: WO
		uint32 ulprUserFifoClearFullDropCtrStrb : 1; //Clear ULPR user FIFO full drop counter. , By writing '1'., reset value: 0x0, access type: WO
		uint32 ulprUserFifoClearDecLessThanZeroStrb : 1; //Clear ULPR user FIFO decrement amount less than zero indication. , By writing '1'., reset value: 0x0, access type: WO
		uint32 reserved0 : 29;
	} bitFields;
} RegUlPostRxUlprUserFifoClearStrb_u;

/*REG_UL_POST_RX_ULPR_USER_FIFO_RD_ENTRIES_NUM 0x74 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ulprUserFifoRdEntriesNum : 9; //ULPR user FIFO number of entries to decrement., reset value: 0x0, access type: RW
		uint32 reserved0 : 23;
	} bitFields;
} RegUlPostRxUlprUserFifoRdEntriesNum_u;

/*REG_UL_POST_RX_ULPR_USER_FIFO_NUM_ENTRIES_COUNT 0x78 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ulprUserFifoNumEntriesCount : 10; //ULPR user FIFO number of entries count, reset value: 0x0, access type: RO
		uint32 reserved0 : 22;
	} bitFields;
} RegUlPostRxUlprUserFifoNumEntriesCount_u;

/*REG_UL_POST_RX_ULPR_USER_FIFO_DEBUG 0x7C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ulprUserFifoWrPtr : 9; //ULPR user FIFO write pointer index, reset value: 0x0, access type: RO
		uint32 reserved0 : 3;
		uint32 ulprUserFifoNotEmpty : 1; //ULPR user FIFO not empty indication, reset value: 0x0, access type: RO
		uint32 ulprUserFifoFull : 1; //ULPR user FIFO full indication, reset value: 0x0, access type: RO
		uint32 ulprUserFifoDecrementLessThanZero : 1; //ULPR user report setting FIFO amount has been decremented less than zero, reset value: 0x0, access type: RO
		uint32 reserved1 : 5;
		uint32 ulprUserFifoFullDropCtr : 9; //ULPR user report setting FIFO full drop counter, reset value: 0x0, access type: RO
		uint32 reserved2 : 3;
	} bitFields;
} RegUlPostRxUlprUserFifoDebug_u;

/*REG_UL_POST_RX_ULPR_INT_ERROR_STATUS 0x80 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ulprCommonFifoDropStatus : 1; //Common FIFO dropped entries event, reset value: 0x0, access type: RO
		uint32 ulprUserFifoDropStatus : 1; //User FIFO dropped entries event, reset value: 0x0, access type: RO
		uint32 ulprPulse1CollisionStatus : 1; //Pulse_1 collision event. Set when pulse_1 is set during ULPR processing (of previous RX Window). When this even occurs, the ULPR block shall finish processing users which have started processing and abort the rest of the users, reset value: 0x0, access type: RO
		uint32 ulprPulse2CollisionStatus : 1; //Pulse_2 collision event. Set when pulse_2 is set during ULPR processing (necessarily finishing last user processing, since pulse_1 necessarily collided as well). When this event occurs, the processing due to pulse_2 (and the following pulse_3) shall not be done and pulse_2 and the following pulse_3 shall be ignored, reset value: 0x0, access type: RO
		uint32 ulprNonUniqueControlIdStatus : 1; //Non-unique Control ID event. Control ID appears more than once within A-Control word, reset value: 0x0, access type: RO
		uint32 reserved0 : 27;
	} bitFields;
} RegUlPostRxUlprIntErrorStatus_u;

/*REG_UL_POST_RX_ULPR_INT_ERROR_EN 0x84 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ulprCommonFifoDropEn : 1; //Common FIFO dropped entries interrupt enable, reset value: 0x1, access type: RW
		uint32 ulprUserFifoDropEn : 1; //User FIFO dropped entries interrupt enable, reset value: 0x1, access type: RW
		uint32 ulprPulse1CollisionEn : 1; //Pulse_1 collision interrupt enable, reset value: 0x0, access type: RW
		uint32 ulprPulse2CollisionEn : 1; //Pulse_3 collision interrupt enable, reset value: 0x0, access type: RW
		uint32 ulprNonUniqueControlIdEn : 1; //Non-unique Control ID interrupt enable, reset value: 0x0, access type: RW
		uint32 reserved0 : 27;
	} bitFields;
} RegUlPostRxUlprIntErrorEn_u;

/*REG_UL_POST_RX_ULPR_INT_ERROR_CLEAR 0x88 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ulprCommonFifoDropClr : 1; //Common FIFO dropped entries interrupt clear, reset value: 0x0, access type: WO
		uint32 ulprUserFifoDropClr : 1; //User FIFO dropped entries interrupt clear, reset value: 0x0, access type: WO
		uint32 ulprPulse1CollisionClr : 1; //Pulse_1 collision interrupt clear, reset value: 0x0, access type: WO
		uint32 ulprPulse2CollisionClr : 1; //Pulse_3 collision interrupt clear, reset value: 0x0, access type: WO
		uint32 ulprNonUniqueControlIdClr : 1; //Non-unique Control ID interrupt clear, reset value: 0x0, access type: WO
		uint32 reserved0 : 27;
	} bitFields;
} RegUlPostRxUlprIntErrorClear_u;

/*REG_UL_POST_RX_ULPR_FIFO_NOT_EMPTY_STATUS 0x90 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ulprCommonFifoNotEmptyStatus : 1; //ULPR common FIFO not empty indication, reset value: 0x0, access type: RO
		uint32 ulprUserFifoNotEmptyStatus : 1; //ULPR user FIFO not empty indication, reset value: 0x0, access type: RO
		uint32 reserved0 : 30;
	} bitFields;
} RegUlPostRxUlprFifoNotEmptyStatus_u;

/*REG_UL_POST_RX_ULPR_FIFO_NOT_EMPTY_EN 0x94 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ulprCommonFifoNotEmptyEnable : 1; //ULPR common FIFO not empty interrupt enable, reset value: 0x0, access type: RW
		uint32 ulprUserFifoNotEmptyEnable : 1; //ULPR user FIFO not empty interrupt enable, reset value: 0x1, access type: RW
		uint32 reserved0 : 30;
	} bitFields;
} RegUlPostRxUlprFifoNotEmptyEn_u;

/*REG_UL_POST_RX_ULPR_CLEAR_BITS 0x98 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ulprTransactionNumberClear : 1; //Write 1 to this field in order to zero the transaction number, reset value: 0x0, access type: WO
		uint32 reserved0 : 31;
	} bitFields;
} RegUlPostRxUlprClearBits_u;

/*REG_UL_POST_RX_ULPR_STATUS_0 0x9C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ulprCoreSmDbg : 3; //ULPR_CORE SM status, reset value: 0x0, access type: RO
		uint32 ulprRcrScanSmDbg : 2; //ULPR_RCR_SCAN SM status, reset value: 0x0, access type: RO
		uint32 ulprInitSmDbg : 2; //ULPR_INIT SM status, reset value: 0x0, access type: RO
		uint32 ulprUserMngrSmDbg : 2; //ULPR_USER_MNGR SM status, reset value: 0x0, access type: RO
		uint32 ulprUserGetDataSmDbg : 2; //ULPR_USER_GET_DATA SM status, reset value: 0x0, access type: RO
		uint32 ulprFldCtchSmDbg : 3; //ULPR_FLD_CTCH SM status, reset value: 0x0, access type: RO
		uint32 ulprPhyStatusUserSmDbg : 2; //ULPR_PHY_STATUS_USER SM status, reset value: 0x0, access type: RO
		uint32 ulprUserProcessSmDbg : 3; //ULPR_USER_PROCESS SM status, reset value: 0x0, access type: RO
		uint32 ulprTxselSmDbg : 2; //ULPR_TXSEL SM status, reset value: 0x0, access type: RO
		uint32 ulprUserReportSmDbg : 3; //ULPR_USER_REPORT SM status, reset value: 0x0, access type: RO
		uint32 ulprTransactionNumberDbg : 8; //Current Transaction Number. This number mirrors the transaction number which is used for the reports., reset value: 0x0, access type: RO
	} bitFields;
} RegUlPostRxUlprStatus0_u;

/*REG_UL_POST_RX_ULPR_SPARE_REGISTER 0xB0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 spareRegister : 16; //Spare Register, reset value: 0x8888, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegUlPostRxUlprSpareRegister_u;



#endif // _UL_POST_RX_REGS_H_
