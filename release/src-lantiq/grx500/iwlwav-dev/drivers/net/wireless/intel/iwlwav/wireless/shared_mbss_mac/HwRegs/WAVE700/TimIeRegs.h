
/***********************************************************************************
File:				TimIeRegs.h
Module:				timIe
SOC Revision:		
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _TIM_IE_REGS_H_
#define _TIM_IE_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define TIM_IE_BASE_ADDRESS                             MEMORY_MAP_UNIT_64_BASE_ADDRESS
#define	REG_TIM_IE_TX_SEL_TIM_LENGTH                   (TIM_IE_BASE_ADDRESS + 0x0)
#define	REG_TIM_IE_TX_SEL_TIM_DEBUG                    (TIM_IE_BASE_ADDRESS + 0x4)
#define	REG_TIM_IE_TX_SEL_TIM_HEADER                   (TIM_IE_BASE_ADDRESS + 0x8)
#define	REG_TIM_IE_TX_SEL_TIM_BITMAP_CONTROL           (TIM_IE_BASE_ADDRESS + 0xC)
#define	REG_TIM_IE_TX_SEL_TIM_MPS_GROUP_LENGTH         (TIM_IE_BASE_ADDRESS + 0x10)
#define	REG_TIM_IE_TX_SEL_TIM_MPS_GROUP                (TIM_IE_BASE_ADDRESS + 0x14)
#define	REG_TIM_IE_TX_SEL_TIM_VIRTUAL_BITMAP_LENGTH    (TIM_IE_BASE_ADDRESS + 0x18)
#define	REG_TIM_IE_TX_SEL_TIM_VIRTUAL_BITMAP_SHIFT     (TIM_IE_BASE_ADDRESS + 0x1C)
#define	REG_TIM_IE_TX_SEL_TIM_VIRTUAL_BITMAP0          (TIM_IE_BASE_ADDRESS + 0x20)
#define	REG_TIM_IE_TX_SEL_TIM_VIRTUAL_BITMAP1          (TIM_IE_BASE_ADDRESS + 0x24)
#define	REG_TIM_IE_TX_SEL_TIM_VIRTUAL_BITMAP2          (TIM_IE_BASE_ADDRESS + 0x28)
#define	REG_TIM_IE_TX_SEL_TIM_VIRTUAL_BITMAP3          (TIM_IE_BASE_ADDRESS + 0x2C)
#define	REG_TIM_IE_TX_SEL_TIM_VIRTUAL_BITMAP4          (TIM_IE_BASE_ADDRESS + 0x30)
#define	REG_TIM_IE_TX_SEL_TIM_VIRTUAL_BITMAP5          (TIM_IE_BASE_ADDRESS + 0x34)
#define	REG_TIM_IE_TX_SEL_TIM_VIRTUAL_BITMAP6          (TIM_IE_BASE_ADDRESS + 0x38)
#define	REG_TIM_IE_TX_SEL_TIM_VIRTUAL_BITMAP7          (TIM_IE_BASE_ADDRESS + 0x3C)
#define	REG_TIM_IE_TX_SEL_TIM_VIRTUAL_BITMAP8          (TIM_IE_BASE_ADDRESS + 0x40)
#define	REG_TIM_IE_TX_SEL_TIM_VIRTUAL_BITMAP9          (TIM_IE_BASE_ADDRESS + 0x44)
#define	REG_TIM_IE_TX_SEL_TIM_VIRTUAL_BITMAP10         (TIM_IE_BASE_ADDRESS + 0x48)
#define	REG_TIM_IE_TX_SEL_TIM_ERR                      (TIM_IE_BASE_ADDRESS + 0x50)
#define	REG_TIM_IE_TX_SEL_TIM_ERR_CLR                  (TIM_IE_BASE_ADDRESS + 0x54)
#define	REG_TIM_IE_TX_SEL_TIM_MULTI_BSSID_CFG          (TIM_IE_BASE_ADDRESS + 0x58)
#define	REG_TIM_IE_TX_SEL_TIM_MULTI_BSSID_MPS          (TIM_IE_BASE_ADDRESS + 0x60)
#define	REG_TIM_IE_TX_SEL_TIM_MULTI_BSSID_MPS_EN       (TIM_IE_BASE_ADDRESS + 0x64)
#define	REG_TIM_IE_TX_SEL_TIM_DTIM_READ_VAP_IDX        (TIM_IE_BASE_ADDRESS + 0x68)
#define	REG_TIM_IE_TX_SEL_TIM_DTIM_VALUE               (TIM_IE_BASE_ADDRESS + 0x6C)
#define	REG_TIM_IE_TX_SEL_TIM_STA_BITMAP_SEL           (TIM_IE_BASE_ADDRESS + 0x70)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_TIM_IE_TX_SEL_TIM_LENGTH 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timTotalLength : 8; //TIM IE Total Byte length, reset value: 0x6, access type: RO
		uint32 reserved0 : 24;
	} bitFields;
} RegTimIeTxSelTimLength_u;

/*REG_TIM_IE_TX_SEL_TIM_DEBUG 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timEmpty : 1; //TIM IE empty indication read by HOB, reset value: 0x1, access type: RO
		uint32 timReady : 1; //TIM IE ready to be transmitted indication, reset value: 0x0, access type: RO
		uint32 reserved0 : 30;
	} bitFields;
} RegTimIeTxSelTimDebug_u;

/*REG_TIM_IE_TX_SEL_TIM_HEADER 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timHeaderElementId : 8; //Element ID value, reset value: 0x5, access type: RO
		uint32 timHeaderLength : 8; //TIM IE Byte length, reset value: 0x4, access type: RO
		uint32 dtimCount : 8; //DTIM counter value, reset value: 0x0, access type: RO
		uint32 dtimPeriod : 8; //DTIM period value, reset value: 0x0, access type: RO
	} bitFields;
} RegTimIeTxSelTimHeader_u;

/*REG_TIM_IE_TX_SEL_TIM_BITMAP_CONTROL 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timBitmapControl : 8; //Control field value, reset value: 0x0, access type: RO
		uint32 reserved0 : 24;
	} bitFields;
} RegTimIeTxSelTimBitmapControl_u;

/*REG_TIM_IE_TX_SEL_TIM_MPS_GROUP_LENGTH 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timMpsGroupLength : 3; //Length of MPS bits to be transmitted in TIM IE, reset value: 0x0, access type: RO
		uint32 reserved0 : 29;
	} bitFields;
} RegTimIeTxSelTimMpsGroupLength_u;

/*REG_TIM_IE_TX_SEL_TIM_MPS_GROUP 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timMpsGroup : 32; //MPS result of the VAP/VAPs activated in the TIM IE, to be transmitted in TIM IE, reset value: 0x0, access type: RO
	} bitFields;
} RegTimIeTxSelTimMpsGroup_u;

/*REG_TIM_IE_TX_SEL_TIM_VIRTUAL_BITMAP_LENGTH 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timVirtualBitmapLength : 8; //TIM Virtual bitmap length to be transmitted, reset value: 0x1, access type: RO
		uint32 timVirtualBitmapOffset : 8; //TIM Virtual bitmap offset to be transmitted from, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegTimIeTxSelTimVirtualBitmapLength_u;

/*REG_TIM_IE_TX_SEL_TIM_VIRTUAL_BITMAP_SHIFT 0x1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timVirtualBitmapShiftNum : 2; //TIM virtual bitmap shift number of bytes, reset value: 0x0, access type: RW
		uint32 reserved0 : 6;
		uint32 timVirtualBitmapShiftRight : 1; //TIM virtual bitmap shift side. 1 - right, 0- left, reset value: 0x0, access type: RW
		uint32 reserved1 : 23;
	} bitFields;
} RegTimIeTxSelTimVirtualBitmapShift_u;

/*REG_TIM_IE_TX_SEL_TIM_VIRTUAL_BITMAP0 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timVirtualBitmap0 : 32; //TIM Virtual Bitmap value. Bytes 0-3 of bitmap., reset value: 0x0, access type: RO
	} bitFields;
} RegTimIeTxSelTimVirtualBitmap0_u;

/*REG_TIM_IE_TX_SEL_TIM_VIRTUAL_BITMAP1 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timVirtualBitmap1 : 32; //TIM Virtual Bitmap value. Bytes 4-7 of bitmap., reset value: 0x0, access type: RO
	} bitFields;
} RegTimIeTxSelTimVirtualBitmap1_u;

/*REG_TIM_IE_TX_SEL_TIM_VIRTUAL_BITMAP2 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timVirtualBitmap2 : 32; //TIM Virtual Bitmap value. Bytes 8-11 of bitmap., reset value: 0x0, access type: RO
	} bitFields;
} RegTimIeTxSelTimVirtualBitmap2_u;

/*REG_TIM_IE_TX_SEL_TIM_VIRTUAL_BITMAP3 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timVirtualBitmap3 : 32; //TIM Virtual Bitmap value. Bytes 12-15 of bitmap., reset value: 0x0, access type: RO
	} bitFields;
} RegTimIeTxSelTimVirtualBitmap3_u;

/*REG_TIM_IE_TX_SEL_TIM_VIRTUAL_BITMAP4 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timVirtualBitmap4 : 32; //TIM Virtual Bitmap value. Bytes 16-19 of bitmap., reset value: 0x0, access type: RO
	} bitFields;
} RegTimIeTxSelTimVirtualBitmap4_u;

/*REG_TIM_IE_TX_SEL_TIM_VIRTUAL_BITMAP5 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timVirtualBitmap5 : 32; //TIM Virtual Bitmap value. Bytes 20-23 of bitmap., reset value: 0x0, access type: RO
	} bitFields;
} RegTimIeTxSelTimVirtualBitmap5_u;

/*REG_TIM_IE_TX_SEL_TIM_VIRTUAL_BITMAP6 0x38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timVirtualBitmap6 : 32; //TIM Virtual Bitmap value. Bytes 24-27 of bitmap., reset value: 0x0, access type: RO
	} bitFields;
} RegTimIeTxSelTimVirtualBitmap6_u;

/*REG_TIM_IE_TX_SEL_TIM_VIRTUAL_BITMAP7 0x3C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timVirtualBitmap7 : 32; //TIM Virtual Bitmap value. Bytes 28-31 of bitmap., reset value: 0x0, access type: RO
	} bitFields;
} RegTimIeTxSelTimVirtualBitmap7_u;

/*REG_TIM_IE_TX_SEL_TIM_VIRTUAL_BITMAP8 0x40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timVirtualBitmap8 : 32; //TIM Virtual Bitmap value. Bytes 32-35 of bitmap., reset value: 0x0, access type: RO
	} bitFields;
} RegTimIeTxSelTimVirtualBitmap8_u;

/*REG_TIM_IE_TX_SEL_TIM_VIRTUAL_BITMAP9 0x44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timVirtualBitmap9 : 32; //TIM Virtual Bitmap value. Bytes 36-39 of bitmap., reset value: 0x0, access type: RO
	} bitFields;
} RegTimIeTxSelTimVirtualBitmap9_u;

/*REG_TIM_IE_TX_SEL_TIM_VIRTUAL_BITMAP10 0x48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timVirtualBitmap10 : 32; //TIM Virtual Bitmap value. Bytes 40-43 of bitmap., reset value: 0x0, access type: RO
	} bitFields;
} RegTimIeTxSelTimVirtualBitmap10_u;

/*REG_TIM_IE_TX_SEL_TIM_ERR 0x50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timWrongVapSel : 32; //Selection of VAP without Beacon enabled, reset value: 0x0, access type: RO
	} bitFields;
} RegTimIeTxSelTimErr_u;

/*REG_TIM_IE_TX_SEL_TIM_ERR_CLR 0x54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timWrongVapSelClr : 32; //Selection of VAP without Beacon enabled clear, reset value: 0x0, access type: WO
	} bitFields;
} RegTimIeTxSelTimErrClr_u;

/*REG_TIM_IE_TX_SEL_TIM_MULTI_BSSID_CFG 0x58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timIeCompressed : 1; //Defines if TIM IE is compressed or not. Legacy TIM IE is compressed only., reset value: 0x1, access type: RW
		uint32 reserved0 : 1;
		uint32 primaryVapIs16 : 1; //Primary VAP cfg: , 0: Primary VAP is 0 , 1: Primary VAP is 16, reset value: 0x0, access type: RW
		uint32 reserved1 : 1;
		uint32 multiBssidGroupSize : 6; //Number of VAPs in group. Supported values are 0(Single BSSID),2,4,8,16,32, reset value: 0x0, access type: RW
		uint32 reserved2 : 22;
	} bitFields;
} RegTimIeTxSelTimMultiBssidCfg_u;

/*REG_TIM_IE_TX_SEL_TIM_MULTI_BSSID_MPS 0x60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mps : 32; //MPS result of the VAP/VAPs activated in the TIM IE, reset value: 0x0, access type: RO
	} bitFields;
} RegTimIeTxSelTimMultiBssidMps_u;

/*REG_TIM_IE_TX_SEL_TIM_MULTI_BSSID_MPS_EN 0x64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mpsEn : 32; //MPS valid VAP/VAPs in the TIM IE, reset value: 0x0, access type: RO
	} bitFields;
} RegTimIeTxSelTimMultiBssidMpsEn_u;

/*REG_TIM_IE_TX_SEL_TIM_DTIM_READ_VAP_IDX 0x68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSelTimDtimReadVapIdx : 5; //VAP index of which the DTIM values are reflected in the adjacent register., reset value: 0x0, access type: RW
		uint32 reserved0 : 27;
	} bitFields;
} RegTimIeTxSelTimDtimReadVapIdx_u;

/*REG_TIM_IE_TX_SEL_TIM_DTIM_VALUE 0x6C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dtimPeriodRead : 8; //Number of TIMs minus 1 between DTIMs, reset value: 0x0, access type: RO
		uint32 dtimCountRead : 8; //DTIM count, when equals 0 it is DTIM, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegTimIeTxSelTimDtimValue_u;

/*REG_TIM_IE_TX_SEL_TIM_STA_BITMAP_SEL 0x70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSelTimStaBitmapSel : 32; //Select which STA bitmap to be used by TIM IE,1 bit per VAP: , 0: Valid PS per STA. , 1: Data in Queue per STA., reset value: 0x0, access type: RW
	} bitFields;
} RegTimIeTxSelTimStaBitmapSel_u;



#endif // _TIM_IE_REGS_H_
