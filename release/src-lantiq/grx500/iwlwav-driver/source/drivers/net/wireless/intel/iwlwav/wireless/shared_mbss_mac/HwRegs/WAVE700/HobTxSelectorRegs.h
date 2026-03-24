
/***********************************************************************************
File:				HobTxSelectorRegs.h
Module:				hobTxSelector
SOC Revision:		
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _HOB_TX_SELECTOR_REGS_H_
#define _HOB_TX_SELECTOR_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define HOB_TX_SELECTOR_BASE_ADDRESS                             MEMORY_MAP_UNIT_505_BASE_ADDRESS
#define	REG_HOB_TX_SELECTOR_TIM_IE_CONTROL               (HOB_TX_SELECTOR_BASE_ADDRESS + 0x0)
#define	REG_HOB_TX_SELECTOR_TIM_IE_BITMAP_CONTROL        (HOB_TX_SELECTOR_BASE_ADDRESS + 0x4)
#define	REG_HOB_TX_SELECTOR_TIM_IE_MPS_GROUP_MSB         (HOB_TX_SELECTOR_BASE_ADDRESS + 0x8)
#define	REG_HOB_TX_SELECTOR_TIM_IE_BITMAP0               (HOB_TX_SELECTOR_BASE_ADDRESS + 0xC)
#define	REG_HOB_TX_SELECTOR_TIM_IE_BITMAP1               (HOB_TX_SELECTOR_BASE_ADDRESS + 0x10)
#define	REG_HOB_TX_SELECTOR_TIM_IE_BITMAP2               (HOB_TX_SELECTOR_BASE_ADDRESS + 0x14)
#define	REG_HOB_TX_SELECTOR_TIM_IE_BITMAP3               (HOB_TX_SELECTOR_BASE_ADDRESS + 0x18)
#define	REG_HOB_TX_SELECTOR_TIM_IE_BITMAP4               (HOB_TX_SELECTOR_BASE_ADDRESS + 0x1C)
#define	REG_HOB_TX_SELECTOR_TIM_IE_BITMAP5               (HOB_TX_SELECTOR_BASE_ADDRESS + 0x20)
#define	REG_HOB_TX_SELECTOR_TIM_IE_BITMAP6               (HOB_TX_SELECTOR_BASE_ADDRESS + 0x24)
#define	REG_HOB_TX_SELECTOR_TIM_IE_BITMAP7               (HOB_TX_SELECTOR_BASE_ADDRESS + 0x28)
#define	REG_HOB_TX_SELECTOR_TIM_IE_BITMAP8               (HOB_TX_SELECTOR_BASE_ADDRESS + 0x2C)
#define	REG_HOB_TX_SELECTOR_TIM_IE_BITMAP9               (HOB_TX_SELECTOR_BASE_ADDRESS + 0x30)
#define	REG_HOB_TX_SELECTOR_TIM_IE_BITMAP10              (HOB_TX_SELECTOR_BASE_ADDRESS + 0x34)
#define	REG_HOB_TX_SELECTOR_TIM_IE_HOB_RECIPE_FIELDS0    (HOB_TX_SELECTOR_BASE_ADDRESS + 0x38)
#define	REG_HOB_TX_SELECTOR_TIM_IE_HOB_RECIPE_FIELDS1    (HOB_TX_SELECTOR_BASE_ADDRESS + 0x3C)
#define	REG_HOB_TX_SELECTOR_TIM_IE_HOB_DEBUG_FIELDS0     (HOB_TX_SELECTOR_BASE_ADDRESS + 0x40)
#define	REG_HOB_TX_SELECTOR_TIM_IE_READ_DTIM_VAP_IDX     (HOB_TX_SELECTOR_BASE_ADDRESS + 0x44)
#define	REG_HOB_TX_SELECTOR_TIM_IE_READ_DTIM_PARAMS      (HOB_TX_SELECTOR_BASE_ADDRESS + 0x48)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_HOB_TX_SELECTOR_TIM_IE_CONTROL 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 elementId : 8; //no description, reset value: 0x5, access type: RO
		uint32 length : 8; //no description, reset value: 0x0, access type: RO
		uint32 dtimCount : 8; //no description, reset value: 0x0, access type: RO
		uint32 dtimPeriod : 8; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegHobTxSelectorTimIeControl_u;

/*REG_HOB_TX_SELECTOR_TIM_IE_BITMAP_CONTROL 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timIeBitmapControl : 8; //bitmap_control, reset value: 0x0, access type: RO
		uint32 mpsGroup : 24; //MBSSID - MPS group field value. , SBSSID - 0 , reset value: 0x0, access type: RO
	} bitFields;
} RegHobTxSelectorTimIeBitmapControl_u;

/*REG_HOB_TX_SELECTOR_TIM_IE_MPS_GROUP_MSB 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mpsGroupMsbByte : 8; //MBSSID - MPS group field value. , SBSSID - 0 , reset value: 0x0, access type: RO
		uint32 reserved0 : 24;
	} bitFields;
} RegHobTxSelectorTimIeMpsGroupMsb_u;

/*REG_HOB_TX_SELECTOR_TIM_IE_BITMAP0 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timIeBitmapB3B0 : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegHobTxSelectorTimIeBitmap0_u;

/*REG_HOB_TX_SELECTOR_TIM_IE_BITMAP1 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timIeBitmapB7B4 : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegHobTxSelectorTimIeBitmap1_u;

/*REG_HOB_TX_SELECTOR_TIM_IE_BITMAP2 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timIeBitmapB11B8 : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegHobTxSelectorTimIeBitmap2_u;

/*REG_HOB_TX_SELECTOR_TIM_IE_BITMAP3 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timIeBitmapB15B12 : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegHobTxSelectorTimIeBitmap3_u;

/*REG_HOB_TX_SELECTOR_TIM_IE_BITMAP4 0x1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timIeBitmapB19B16 : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegHobTxSelectorTimIeBitmap4_u;

/*REG_HOB_TX_SELECTOR_TIM_IE_BITMAP5 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timIeBitmapB23B20 : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegHobTxSelectorTimIeBitmap5_u;

/*REG_HOB_TX_SELECTOR_TIM_IE_BITMAP6 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timIeBitmapB27B24 : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegHobTxSelectorTimIeBitmap6_u;

/*REG_HOB_TX_SELECTOR_TIM_IE_BITMAP7 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timIeBitmapB31B28 : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegHobTxSelectorTimIeBitmap7_u;

/*REG_HOB_TX_SELECTOR_TIM_IE_BITMAP8 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timIeBitmapB35B32 : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegHobTxSelectorTimIeBitmap8_u;

/*REG_HOB_TX_SELECTOR_TIM_IE_BITMAP9 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timIeBitmapB39B36 : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegHobTxSelectorTimIeBitmap9_u;

/*REG_HOB_TX_SELECTOR_TIM_IE_BITMAP10 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timIeBitmapB40 : 8; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 24;
	} bitFields;
} RegHobTxSelectorTimIeBitmap10_u;

/*REG_HOB_TX_SELECTOR_TIM_IE_HOB_RECIPE_FIELDS0 0x38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hobReadLength : 8; //no description, reset value: 0x0, access type: RO
		uint32 hobReadOffset : 8; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegHobTxSelectorTimIeHobRecipeFields0_u;

/*REG_HOB_TX_SELECTOR_TIM_IE_HOB_RECIPE_FIELDS1 0x3C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timIeBitmapControlLength : 3; //Length of bitmap_control read: , SBSSID - 1 , MBSSID - 1 + group size in bytes., reset value: 0x0, access type: RO
		uint32 reserved0 : 29;
	} bitFields;
} RegHobTxSelectorTimIeHobRecipeFields1_u;

/*REG_HOB_TX_SELECTOR_TIM_IE_HOB_DEBUG_FIELDS0 0x40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timEmpty : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 7;
		uint32 timReady : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved1 : 7;
		uint32 timIeIsLegacy : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved2 : 15;
	} bitFields;
} RegHobTxSelectorTimIeHobDebugFields0_u;

/*REG_HOB_TX_SELECTOR_TIM_IE_READ_DTIM_VAP_IDX 0x44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 deliaReadDtimVapIdx : 5; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 27;
	} bitFields;
} RegHobTxSelectorTimIeReadDtimVapIdx_u;

/*REG_HOB_TX_SELECTOR_TIM_IE_READ_DTIM_PARAMS 0x48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 deliaReadDtimPeriod : 8; //no description, reset value: 0x0, access type: RO
		uint32 deliaReadDtimCount : 8; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegHobTxSelectorTimIeReadDtimParams_u;



#endif // _HOB_TX_SELECTOR_REGS_H_
