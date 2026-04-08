
/***********************************************************************************
File:				TxSelectorRegs.h
Module:				txSelector
SOC Revision:		843
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _TX_SELECTOR_REGS_H_
#define _TX_SELECTOR_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define TX_SELECTOR_BASE_ADDRESS                             MEMORY_MAP_UNIT_29_BASE_ADDRESS
#define	REG_TX_SELECTOR_TX_SEL_INSTR                             (TX_SELECTOR_BASE_ADDRESS + 0x0)
#define	REG_TX_SELECTOR_TX_SEL_BITMAP_UPDATE                     (TX_SELECTOR_BASE_ADDRESS + 0x4)
#define	REG_TX_SELECTOR_TX_SEL_STA_PS_WRR_LOW                    (TX_SELECTOR_BASE_ADDRESS + 0x8)
#define	REG_TX_SELECTOR_TX_SEL_STA_PS_WRR_HIGH                   (TX_SELECTOR_BASE_ADDRESS + 0xC)
#define	REG_TX_SELECTOR_TX_SEL_STA_ACTIVE_WRR_LOW                (TX_SELECTOR_BASE_ADDRESS + 0x10)
#define	REG_TX_SELECTOR_TX_SEL_STA_ACTIVE_WRR_HIGH               (TX_SELECTOR_BASE_ADDRESS + 0x14)
#define	REG_TX_SELECTOR_TX_SEL_STRICT_PRIO                       (TX_SELECTOR_BASE_ADDRESS + 0x18)
#define	REG_TX_SELECTOR_TX_SEL_SELECTION                         (TX_SELECTOR_BASE_ADDRESS + 0x1C)
#define	REG_TX_SELECTOR_TX_SEL_SELECTED_BITMAP                   (TX_SELECTOR_BASE_ADDRESS + 0x20)
#define	REG_TX_SELECTOR_TX_SEL_SRC0_SELECTION                    (TX_SELECTOR_BASE_ADDRESS + 0x24)
#define	REG_TX_SELECTOR_TX_SEL_SELECTED_SRC0_BITMAP              (TX_SELECTOR_BASE_ADDRESS + 0x28)
#define	REG_TX_SELECTOR_TX_SEL_SRC1_SELECTION                    (TX_SELECTOR_BASE_ADDRESS + 0x2C)
#define	REG_TX_SELECTOR_TX_SEL_SELECTED_SRC1_BITMAP              (TX_SELECTOR_BASE_ADDRESS + 0x30)
#define	REG_TX_SELECTOR_TX_SEL_SRC2_SELECTION                    (TX_SELECTOR_BASE_ADDRESS + 0x34)
#define	REG_TX_SELECTOR_TX_SEL_SELECTED_SRC2_BITMAP              (TX_SELECTOR_BASE_ADDRESS + 0x38)
#define	REG_TX_SELECTOR_TX_SEL_SRC3_SELECTION                    (TX_SELECTOR_BASE_ADDRESS + 0x3C)
#define	REG_TX_SELECTOR_TX_SEL_SELECTED_SRC3_BITMAP              (TX_SELECTOR_BASE_ADDRESS + 0x40)
#define	REG_TX_SELECTOR_TX_SEL_SRC4_SELECTION                    (TX_SELECTOR_BASE_ADDRESS + 0x44)
#define	REG_TX_SELECTOR_TX_SEL_SELECTED_SRC4_BITMAP              (TX_SELECTOR_BASE_ADDRESS + 0x48)
#define	REG_TX_SELECTOR_TX_SEL_SRC5_SELECTION                    (TX_SELECTOR_BASE_ADDRESS + 0x4C)
#define	REG_TX_SELECTOR_TX_SEL_SELECTED_SRC5_BITMAP              (TX_SELECTOR_BASE_ADDRESS + 0x50)
#define	REG_TX_SELECTOR_TX_SEL_SRC6_SELECTION                    (TX_SELECTOR_BASE_ADDRESS + 0x54)
#define	REG_TX_SELECTOR_TX_SEL_SELECTED_SRC6_BITMAP              (TX_SELECTOR_BASE_ADDRESS + 0x58)
#define	REG_TX_SELECTOR_TX_SEL_STATUS                            (TX_SELECTOR_BASE_ADDRESS + 0x5C)
#define	REG_TX_SELECTOR_TX_SEL_ERR                               (TX_SELECTOR_BASE_ADDRESS + 0x60)
#define	REG_TX_SELECTOR_TX_SEL_ERR_CLR                           (TX_SELECTOR_BASE_ADDRESS + 0x64)
#define	REG_TX_SELECTOR_TX_SEL_VAP_AC_MATRIX_0_3                 (TX_SELECTOR_BASE_ADDRESS + 0x68)
#define	REG_TX_SELECTOR_TX_SEL_VAP_AC_MATRIX_4_7                 (TX_SELECTOR_BASE_ADDRESS + 0x6C)
#define	REG_TX_SELECTOR_TX_SEL_VAP_AC_MATRIX_8_11                (TX_SELECTOR_BASE_ADDRESS + 0x70)
#define	REG_TX_SELECTOR_TX_SEL_VAP_AC_MATRIX_12_15               (TX_SELECTOR_BASE_ADDRESS + 0x74)
#define	REG_TX_SELECTOR_TX_SEL_STA_ASSOCIATE                     (TX_SELECTOR_BASE_ADDRESS + 0x78)
#define	REG_TX_SELECTOR_TX_SEL_VAP_AC_BYPASS                     (TX_SELECTOR_BASE_ADDRESS + 0x90)
#define	REG_TX_SELECTOR_TX_SEL_VAP_PS_STATE                      (TX_SELECTOR_BASE_ADDRESS + 0x94)
#define	REG_TX_SELECTOR_TX_SEL_VAP_PS_NEW_IND                    (TX_SELECTOR_BASE_ADDRESS + 0x98)
#define	REG_TX_SELECTOR_TX_SEL_TIM_BITMAP0                       (TX_SELECTOR_BASE_ADDRESS + 0x9C)
#define	REG_TX_SELECTOR_TX_SEL_TIM_BITMAP1                       (TX_SELECTOR_BASE_ADDRESS + 0xA0)
#define	REG_TX_SELECTOR_TX_SEL_TIM_BITMAP2                       (TX_SELECTOR_BASE_ADDRESS + 0xA4)
#define	REG_TX_SELECTOR_TX_SEL_TIM_BITMAP3                       (TX_SELECTOR_BASE_ADDRESS + 0xA8)
#define	REG_TX_SELECTOR_TX_SEL_TIM_BITMAP4                       (TX_SELECTOR_BASE_ADDRESS + 0xAC)
#define	REG_TX_SELECTOR_TX_SEL_TIM_HEADER                        (TX_SELECTOR_BASE_ADDRESS + 0xB0)
#define	REG_TX_SELECTOR_TX_SEL_TIM_ELEMENT_ID                    (TX_SELECTOR_BASE_ADDRESS + 0xB4)
#define	REG_TX_SELECTOR_TX_SEL_TIM_HOB_POINTERS                  (TX_SELECTOR_BASE_ADDRESS + 0xB8)
#define	REG_TX_SELECTOR_TX_SEL_STA_ACTIVE_SEL_COUNT              (TX_SELECTOR_BASE_ADDRESS + 0xBC)
#define	REG_TX_SELECTOR_TX_SEL_STA_PS_SEL_COUNT                  (TX_SELECTOR_BASE_ADDRESS + 0xC0)
#define	REG_TX_SELECTOR_TX_SEL_VAP_ACTIVE_SEL_COUNT              (TX_SELECTOR_BASE_ADDRESS + 0xC4)
#define	REG_TX_SELECTOR_TX_SEL_VAP_PS_SEL_COUNT                  (TX_SELECTOR_BASE_ADDRESS + 0xC8)
#define	REG_TX_SELECTOR_TX_SEL_GLOBAL_SEL_COUNT                  (TX_SELECTOR_BASE_ADDRESS + 0xCC)
#define	REG_TX_SELECTOR_TX_SEL_GPLP_SEL_COUNT                    (TX_SELECTOR_BASE_ADDRESS + 0xD0)
#define	REG_TX_SELECTOR_TX_SEL_GPHP_SEL_COUNT                    (TX_SELECTOR_BASE_ADDRESS + 0xD4)
#define	REG_TX_SELECTOR_TX_SEL_BEACON_SEL_COUNT                  (TX_SELECTOR_BASE_ADDRESS + 0xD8)
#define	REG_TX_SELECTOR_TX_SEL_EMPTY_SEL_COUNT                   (TX_SELECTOR_BASE_ADDRESS + 0xDC)
#define	REG_TX_SELECTOR_TX_SEL_COUNT_CLR                         (TX_SELECTOR_BASE_ADDRESS + 0xE0)
#define	REG_TX_SELECTOR_TX_SEL_LOCKER_EN                         (TX_SELECTOR_BASE_ADDRESS + 0xE4)
#define	REG_TX_SELECTOR_TX_SEL_SW_MASTER0_LOCKER                 (TX_SELECTOR_BASE_ADDRESS + 0xE8)
#define	REG_TX_SELECTOR_TX_SEL_SW_MASTER1_LOCKER                 (TX_SELECTOR_BASE_ADDRESS + 0xEC)
#define	REG_TX_SELECTOR_TX_SEL_SW_MASTER2_LOCKER                 (TX_SELECTOR_BASE_ADDRESS + 0xF0)
#define	REG_TX_SELECTOR_TX_SEL_SW_MASTER3_LOCKER                 (TX_SELECTOR_BASE_ADDRESS + 0xF4)
#define	REG_TX_SELECTOR_TX_SEL_SW_MASTER0_LOCKER_STATUS          (TX_SELECTOR_BASE_ADDRESS + 0xF8)
#define	REG_TX_SELECTOR_TX_SEL_SW_MASTER1_LOCKER_STATUS          (TX_SELECTOR_BASE_ADDRESS + 0xFC)
#define	REG_TX_SELECTOR_TX_SEL_SW_MASTER2_LOCKER_STATUS          (TX_SELECTOR_BASE_ADDRESS + 0x100)
#define	REG_TX_SELECTOR_TX_SEL_SW_MASTER3_LOCKER_STATUS          (TX_SELECTOR_BASE_ADDRESS + 0x104)
#define	REG_TX_SELECTOR_TX_SEL_LOCKER_INT_CLR                    (TX_SELECTOR_BASE_ADDRESS + 0x108)
#define	REG_TX_SELECTOR_TX_SEL_LOCKER_PUSH_WHILE_BUSY            (TX_SELECTOR_BASE_ADDRESS + 0x10C)
#define	REG_TX_SELECTOR_TX_SEL_LOCKER_PUSH_WHILE_BUSY_CLR        (TX_SELECTOR_BASE_ADDRESS + 0x110)
#define	REG_TX_SELECTOR_TX_SELECTOR_SPARE                        (TX_SELECTOR_BASE_ADDRESS + 0x114)
#define	REG_TX_SELECTOR_TX_SEL_LOGGER_CFG                        (TX_SELECTOR_BASE_ADDRESS + 0x118)
#define	REG_TX_SELECTOR_TX_SEL_LOGGER_BUSY                       (TX_SELECTOR_BASE_ADDRESS + 0x11C)
#define	REG_TX_SELECTOR_TX_SEL_MU_SELECT_CFG                     (TX_SELECTOR_BASE_ADDRESS + 0x200)
#define	REG_TX_SELECTOR_TX_SEL_MU_BITMAP_UPDATE                  (TX_SELECTOR_BASE_ADDRESS + 0x204)
#define	REG_TX_SELECTOR_TX_SEL_MU_BITMAP_UPDATE_ALL_STA_WDATA    (TX_SELECTOR_BASE_ADDRESS + 0x208)
#define	REG_TX_SELECTOR_TX_SEL_MU_PRIMARY_THRES                  (TX_SELECTOR_BASE_ADDRESS + 0x20C)
#define	REG_TX_SELECTOR_TX_SEL_MU_SECONDARY_THRES                (TX_SELECTOR_BASE_ADDRESS + 0x210)
#define	REG_TX_SELECTOR_TX_SEL_MU_ERR                            (TX_SELECTOR_BASE_ADDRESS + 0x214)
#define	REG_TX_SELECTOR_TX_SEL_MU_ERR_CLR                        (TX_SELECTOR_BASE_ADDRESS + 0x218)
#define	REG_TX_SELECTOR_TX_SEL_MU_LOCKER_EN                      (TX_SELECTOR_BASE_ADDRESS + 0x220)
#define	REG_TX_SELECTOR_TX_SEL_MU_SW_MASTER0_LOCKER              (TX_SELECTOR_BASE_ADDRESS + 0x224)
#define	REG_TX_SELECTOR_TX_SEL_MU_SW_MASTER1_LOCKER              (TX_SELECTOR_BASE_ADDRESS + 0x228)
#define	REG_TX_SELECTOR_TX_SEL_MU_SW_MASTER2_LOCKER              (TX_SELECTOR_BASE_ADDRESS + 0x22C)
#define	REG_TX_SELECTOR_TX_SEL_MU_SW_MASTER3_LOCKER              (TX_SELECTOR_BASE_ADDRESS + 0x230)
#define	REG_TX_SELECTOR_TX_SEL_MU_SW_MASTER0_LOCKER_STATUS       (TX_SELECTOR_BASE_ADDRESS + 0x234)
#define	REG_TX_SELECTOR_TX_SEL_MU_SW_MASTER1_LOCKER_STATUS       (TX_SELECTOR_BASE_ADDRESS + 0x238)
#define	REG_TX_SELECTOR_TX_SEL_MU_SW_MASTER2_LOCKER_STATUS       (TX_SELECTOR_BASE_ADDRESS + 0x23C)
#define	REG_TX_SELECTOR_TX_SEL_MU_SW_MASTER3_LOCKER_STATUS       (TX_SELECTOR_BASE_ADDRESS + 0x240)
#define	REG_TX_SELECTOR_TX_SEL_MU_LOCKER_INT_CLR                 (TX_SELECTOR_BASE_ADDRESS + 0x244)
#define	REG_TX_SELECTOR_TX_SEL_MU_LOCKER_PUSH_WHILE_BUSY         (TX_SELECTOR_BASE_ADDRESS + 0x248)
#define	REG_TX_SELECTOR_TX_SEL_MU_LOCKER_PUSH_WHILE_BUSY_CLR     (TX_SELECTOR_BASE_ADDRESS + 0x24C)
#define	REG_TX_SELECTOR_TX_SEL_MU_UP0_RESULT                     (TX_SELECTOR_BASE_ADDRESS + 0x250)
#define	REG_TX_SELECTOR_TX_SEL_MU_UP1_RESULT                     (TX_SELECTOR_BASE_ADDRESS + 0x254)
#define	REG_TX_SELECTOR_TX_SEL_MU_UP2_RESULT                     (TX_SELECTOR_BASE_ADDRESS + 0x258)
#define	REG_TX_SELECTOR_TX_SEL_MU_UP3_RESULT                     (TX_SELECTOR_BASE_ADDRESS + 0x25C)
#define	REG_TX_SELECTOR_TX_SEL_MU_UP0_BITMAP                     (TX_SELECTOR_BASE_ADDRESS + 0x260)
#define	REG_TX_SELECTOR_TX_SEL_MU_UP1_BITMAP                     (TX_SELECTOR_BASE_ADDRESS + 0x264)
#define	REG_TX_SELECTOR_TX_SEL_MU_UP2_BITMAP                     (TX_SELECTOR_BASE_ADDRESS + 0x268)
#define	REG_TX_SELECTOR_TX_SEL_MU_UP3_BITMAP                     (TX_SELECTOR_BASE_ADDRESS + 0x26C)
#define	REG_TX_SELECTOR_TX_SEL_MU_GRP_BITMAP                     (TX_SELECTOR_BASE_ADDRESS + 0x270)
#define	REG_TX_SELECTOR_TX_SEL_MU_SM_STATUS                      (TX_SELECTOR_BASE_ADDRESS + 0x274)
#define	REG_TX_SELECTOR_TX_SEL_MU_SU_REPORT_STATUS               (TX_SELECTOR_BASE_ADDRESS + 0x278)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_TX_SELECTOR_TX_SEL_INSTR 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 instr:2;	// CPU instruction to TX Selector
		uint32 reserved0:30;
	} bitFields;
} RegTxSelectorTxSelInstr_u;

/*REG_TX_SELECTOR_TX_SEL_BITMAP_UPDATE 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 staSel:8;	// Station number
		uint32 tidSel:3;	// TID number
		uint32 reserved0:5;
		uint32 bitmapSel:3;	// Bitmap Queue selection
		uint32 reserved1:5;
		uint32 bitTypeSel:4;	// Information bit type selector
		uint32 bitValue:1;	// Information bit value
		uint32 reserved2:2;
		uint32 allTids:1;	// Inidication whether the updte is for single TID or the STA/VAP TIDs
	} bitFields;
} RegTxSelectorTxSelBitmapUpdate_u;

/*REG_TX_SELECTOR_TX_SEL_STA_PS_WRR_LOW 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 psLowVap0Prio:3;	// VAP0 priority
		uint32 psLowVap1Prio:3;	// VAP1 priority
		uint32 psLowVap2Prio:3;	// VAP2 priority
		uint32 psLowVap3Prio:3;	// VAP3 priority
		uint32 psLowVap4Prio:3;	// VAP4 priority
		uint32 psLowVap5Prio:3;	// VAP5 priority
		uint32 psLowVap6Prio:3;	// VAP6 priority
		uint32 psLowVap7Prio:3;	// VAP7 priority
		uint32 reserved0:8;
	} bitFields;
} RegTxSelectorTxSelStaPsWrrLow_u;

/*REG_TX_SELECTOR_TX_SEL_STA_PS_WRR_HIGH 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 psHighVap8Prio:3;	// VAP8 priority
		uint32 psHighVap9Prio:3;	// VAP9 priority
		uint32 psHighVap10Prio:3;	// VAP10 priority
		uint32 psHighVap11Prio:3;	// VAP11 priority
		uint32 psHighVap12Prio:3;	// VAP12 priority
		uint32 psHighVap13Prio:3;	// VAP13 priority
		uint32 psHighVap14Prio:3;	// VAP14 priority
		uint32 psHighVap15Prio:3;	// VAP15 priority
		uint32 reserved0:8;
	} bitFields;
} RegTxSelectorTxSelStaPsWrrHigh_u;

/*REG_TX_SELECTOR_TX_SEL_STA_ACTIVE_WRR_LOW 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 activeLowVap0Prio:3;	// VAP0 priority
		uint32 activeLowVap1Prio:3;	// VAP1 priority
		uint32 activeLowVap2Prio:3;	// VAP2 priority
		uint32 activeLowVap3Prio:3;	// VAP3 priority
		uint32 activeLowVap4Prio:3;	// VAP4 priority
		uint32 activeLowVap5Prio:3;	// VAP5 priority
		uint32 activeLowVap6Prio:3;	// VAP6 priority
		uint32 activeLowVap7Prio:3;	// VAP7 priority
		uint32 reserved0:8;
	} bitFields;
} RegTxSelectorTxSelStaActiveWrrLow_u;

/*REG_TX_SELECTOR_TX_SEL_STA_ACTIVE_WRR_HIGH 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 activeHighVap8Prio:3;	// VAP8 priority
		uint32 activeHighVap9Prio:3;	// VAP9 priority
		uint32 activeHighVap10Prio:3;	// VAP10 priority
		uint32 activeHighVap11Prio:3;	// VAP11 priority
		uint32 activeHighVap12Prio:3;	// VAP12 priority
		uint32 activeHighVap13Prio:3;	// VAP13 priority
		uint32 activeHighVap14Prio:3;	// VAP14 priority
		uint32 activeHighVap15Prio:3;	// VAP15 priority
		uint32 reserved0:8;
	} bitFields;
} RegTxSelectorTxSelStaActiveWrrHigh_u;

/*REG_TX_SELECTOR_TX_SEL_STRICT_PRIO 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 strictPrioGroupSelEn:6;	// Enable selection groups
		uint32 strictPrioWrrPrio:2;	// Priority of active selection
		uint32 strictPrioVapMpsPrio:2;	// Priority of VAP TID PS selection
		uint32 strictPrioGlobalPrio:2;	// Priority of Global selection
		uint32 strictPrioSelectorLockEn:1;	// Selector lock enable
		uint32 reserved0:19;
	} bitFields;
} RegTxSelectorTxSelStrictPrio_u;

/*REG_TX_SELECTOR_TX_SEL_SELECTION 0x1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 selectionTidNum:3;	// TID number
		uint32 selectionStaNum:8;	// STA number
		uint32 selectionVapNum:4;	// VAP number
		uint32 selectionTxqId:3;	// Selected Queue index
		uint32 selectionAcNum:2;	// AC number
		uint32 selectionBf:2;	// Beam Forming bits
		uint32 reserved0:2;
		uint32 selectionSrcNum:3;	// Source from which the selected result came from
		uint32 selectionEmpty:1;	// No selection exist bit
		uint32 selectionValid:1;	// Selection valid bit
		uint32 reserved1:3;
	} bitFields;
} RegTxSelectorTxSelSelection_u;

/*REG_TX_SELECTOR_TX_SEL_SELECTED_BITMAP 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 selectedBitmapReq0:1;	// Request bit
		uint32 selectedBitmapPsType0:1;	// PS type
		uint32 selectedBitmapPsMode0:1;	// PS mode
		uint32 selectedBitmapDataInQ0:1;	// Data in Q
		uint32 selectedBitmapReq1:1;	// Request bit
		uint32 selectedBitmapPsType1:1;	// PS type
		uint32 selectedBitmapPsMode1:1;	// PS mode
		uint32 selectedBitmapDataInQ1:1;	// Data in Q
		uint32 selectedBitmapReq2:1;	// Request bit
		uint32 selectedBitmapPsType2:1;	// PS type
		uint32 selectedBitmapPsMode2:1;	// PS mode
		uint32 selectedBitmapDataInQ2:1;	// Data in Q
		uint32 selectedBitmapReq3:1;	// Request bit
		uint32 selectedBitmapPsType3:1;	// PS type
		uint32 selectedBitmapPsMode3:1;	// PS mode
		uint32 selectedBitmapDataInQ3:1;	// Data in Q
		uint32 selectedBitmapReq4:1;	// Request bit
		uint32 selectedBitmapPsType4:1;	// PS type
		uint32 selectedBitmapPsMode4:1;	// PS mode
		uint32 selectedBitmapDataInQ4:1;	// Data in Q
		uint32 selectedBitmapReq5:1;	// Request bit
		uint32 selectedBitmapPsType5:1;	// PS type
		uint32 selectedBitmapPsMode5:1;	// PS mode
		uint32 selectedBitmapDataInQ5:1;	// Data in Q
		uint32 selectedBitmapReq6:1;	// Request bit
		uint32 selectedBitmapPsType6:1;	// PS type
		uint32 selectedBitmapPsMode6:1;	// PS mode
		uint32 selectedBitmapDataInQ6:1;	// Data in Q
		uint32 selectedBitmapReq7:1;	// Request bit
		uint32 selectedBitmapPsType7:1;	// PS type
		uint32 selectedBitmapPsMode7:1;	// PS mode
		uint32 selectedBitmapDataInQ7:1;	// Data in Q
	} bitFields;
} RegTxSelectorTxSelSelectedBitmap_u;

/*REG_TX_SELECTOR_TX_SEL_SRC0_SELECTION 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 src0SelectionTidNum:3;	// TID number
		uint32 src0SelectionStaNum:8;	// STA number
		uint32 src0SelectionVapNum:4;	// VAP number
		uint32 src0SelectionTxqId:3;	// Selected Queue index
		uint32 src0SelectionAcNum:2;	// AC number
		uint32 src0SelectionBf:2;	// Beam Forming bits
		uint32 reserved0:2;
		uint32 src0SelectionSrcNum:3;	// Source from which the selected result came from
		uint32 src0SelectionEmpty:1;	// No selection exist bit
		uint32 src0SelectionValid:1;	// Selection valid bit
		uint32 reserved1:3;
	} bitFields;
} RegTxSelectorTxSelSrc0Selection_u;

/*REG_TX_SELECTOR_TX_SEL_SELECTED_SRC0_BITMAP 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:3;
		uint32 selectedSrc0BitmapDataInQ0:1;	// Data in Q
		uint32 reserved1:3;
		uint32 selectedSrc0BitmapDataInQ1:1;	// Data in Q
		uint32 reserved2:3;
		uint32 selectedSrc0BitmapDataInQ2:1;	// Data in Q
		uint32 reserved3:3;
		uint32 selectedSrc0BitmapDataInQ3:1;	// Data in Q
		uint32 reserved4:3;
		uint32 selectedSrc0BitmapDataInQ4:1;	// Data in Q
		uint32 reserved5:3;
		uint32 selectedSrc0BitmapDataInQ5:1;	// Data in Q
		uint32 reserved6:3;
		uint32 selectedSrc0BitmapDataInQ6:1;	// Data in Q
		uint32 reserved7:3;
		uint32 selectedSrc0BitmapDataInQ7:1;	// Data in Q
	} bitFields;
} RegTxSelectorTxSelSelectedSrc0Bitmap_u;

/*REG_TX_SELECTOR_TX_SEL_SRC1_SELECTION 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 src1SelectionTidNum:3;	// TID number
		uint32 src1SelectionStaNum:8;	// STA number
		uint32 src1SelectionVapNum:4;	// VAP number
		uint32 src1SelectionTxqId:3;	// Selected Queue index
		uint32 src1SelectionAcNum:2;	// AC number
		uint32 src1SelectionBf:2;	// Beam Forming bits
		uint32 reserved0:2;
		uint32 src1SelectionSrcNum:3;	// Source from which the selected result came from
		uint32 src1SelectionEmpty:1;	// No selection exist bit
		uint32 src1SelectionValid:1;	// Selection valid bit
		uint32 reserved1:3;
	} bitFields;
} RegTxSelectorTxSelSrc1Selection_u;

/*REG_TX_SELECTOR_TX_SEL_SELECTED_SRC1_BITMAP 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 selectedSrc1BitmapReq0:1;	// Request bit
		uint32 selectedSrc1BitmapPsType0:1;	// PS type
		uint32 selectedSrc1BitmapPsMode0:1;	// PS mode
		uint32 selectedSrc1BitmapDataInQ0:1;	// Data in Q
		uint32 selectedSrc1BitmapReq1:1;	// Request bit
		uint32 selectedSrc1BitmapPsType1:1;	// PS type
		uint32 selectedSrc1BitmapPsMode1:1;	// PS mode
		uint32 selectedSrc1BitmapDataInQ1:1;	// Data in Q
		uint32 selectedSrc1BitmapReq2:1;	// Request bit
		uint32 selectedSrc1BitmapPsType2:1;	// PS type
		uint32 selectedSrc1BitmapPsMode2:1;	// PS mode
		uint32 selectedSrc1BitmapDataInQ2:1;	// Data in Q
		uint32 selectedSrc1BitmapReq3:1;	// Request bit
		uint32 selectedSrc1BitmapPsType3:1;	// PS type
		uint32 selectedSrc1BitmapPsMode3:1;	// PS mode
		uint32 selectedSrc1BitmapDataInQ3:1;	// Data in Q
		uint32 selectedSrc1BitmapReq4:1;	// Request bit
		uint32 selectedSrc1BitmapPsType4:1;	// PS type
		uint32 selectedSrc1BitmapPsMode4:1;	// PS mode
		uint32 selectedSrc1BitmapDataInQ4:1;	// Data in Q
		uint32 selectedSrc1BitmapReq5:1;	// Request bit
		uint32 selectedSrc1BitmapPsType5:1;	// PS type
		uint32 selectedSrc1BitmapPsMode5:1;	// PS mode
		uint32 selectedSrc1BitmapDataInQ5:1;	// Data in Q
		uint32 selectedSrc1BitmapReq6:1;	// Request bit
		uint32 selectedSrc1BitmapPsType6:1;	// PS type
		uint32 selectedSrc1BitmapPsMode6:1;	// PS mode
		uint32 selectedSrc1BitmapDataInQ6:1;	// Data in Q
		uint32 selectedSrc1BitmapReq7:1;	// Request bit
		uint32 selectedSrc1BitmapPsType7:1;	// PS type
		uint32 selectedSrc1BitmapPsMode7:1;	// PS mode
		uint32 selectedSrc1BitmapDataInQ7:1;	// Data in Q
	} bitFields;
} RegTxSelectorTxSelSelectedSrc1Bitmap_u;

/*REG_TX_SELECTOR_TX_SEL_SRC2_SELECTION 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 src2SelectionTidNum:3;	// TID number
		uint32 src2SelectionStaNum:8;	// STA number
		uint32 src2SelectionVapNum:4;	// VAP number
		uint32 src2SelectionTxqId:3;	// Selected Queue index
		uint32 src2SelectionAcNum:2;	// AC number
		uint32 src2SelectionBf:2;	// Beam Forming bits
		uint32 reserved0:2;
		uint32 src2SelectionSrcNum:3;	// Source from which the selected result came from
		uint32 src2SelectionEmpty:1;	// No selection exist bit
		uint32 src2SelectionValid:1;	// Selection valid bit
		uint32 reserved1:3;
	} bitFields;
} RegTxSelectorTxSelSrc2Selection_u;

/*REG_TX_SELECTOR_TX_SEL_SELECTED_SRC2_BITMAP 0x38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 selectedSrc2BitmapReq0:1;	// Request bit
		uint32 selectedSrc2BitmapPsType0:1;	// PS type
		uint32 selectedSrc2BitmapPsMode0:1;	// PS mode
		uint32 selectedSrc2BitmapDataInQ0:1;	// Data in Q
		uint32 selectedSrc2BitmapReq1:1;	// Request bit
		uint32 selectedSrc2BitmapPsType1:1;	// PS type
		uint32 selectedSrc2BitmapPsMode1:1;	// PS mode
		uint32 selectedSrc2BitmapDataInQ1:1;	// Data in Q
		uint32 selectedSrc2BitmapReq2:1;	// Request bit
		uint32 selectedSrc2BitmapPsType2:1;	// PS type
		uint32 selectedSrc2BitmapPsMode2:1;	// PS mode
		uint32 selectedSrc2BitmapDataInQ2:1;	// Data in Q
		uint32 selectedSrc2BitmapReq3:1;	// Request bit
		uint32 selectedSrc2BitmapPsType3:1;	// PS type
		uint32 selectedSrc2BitmapPsMode3:1;	// PS mode
		uint32 selectedSrc2BitmapDataInQ3:1;	// Data in Q
		uint32 selectedSrc2BitmapReq4:1;	// Request bit
		uint32 selectedSrc2BitmapPsType4:1;	// PS type
		uint32 selectedSrc2BitmapPsMode4:1;	// PS mode
		uint32 selectedSrc2BitmapDataInQ4:1;	// Data in Q
		uint32 selectedSrc2BitmapReq5:1;	// Request bit
		uint32 selectedSrc2BitmapPsType5:1;	// PS type
		uint32 selectedSrc2BitmapPsMode5:1;	// PS mode
		uint32 selectedSrc2BitmapDataInQ5:1;	// Data in Q
		uint32 selectedSrc2BitmapReq6:1;	// Request bit
		uint32 selectedSrc2BitmapPsType6:1;	// PS type
		uint32 selectedSrc2BitmapPsMode6:1;	// PS mode
		uint32 selectedSrc2BitmapDataInQ6:1;	// Data in Q
		uint32 selectedSrc2BitmapReq7:1;	// Request bit
		uint32 selectedSrc2BitmapPsType7:1;	// PS type
		uint32 selectedSrc2BitmapPsMode7:1;	// PS mode
		uint32 selectedSrc2BitmapDataInQ7:1;	// Data in Q
	} bitFields;
} RegTxSelectorTxSelSelectedSrc2Bitmap_u;

/*REG_TX_SELECTOR_TX_SEL_SRC3_SELECTION 0x3C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 src3SelectionTidNum:3;	// TID number
		uint32 src3SelectionStaNum:8;	// STA number
		uint32 src3SelectionVapNum:4;	// VAP number
		uint32 src3SelectionTxqId:3;	// Selected Queue index
		uint32 src3SelectionAcNum:2;	// AC number
		uint32 src3SelectionBf:2;	// Beam Forming bits
		uint32 reserved0:2;
		uint32 src3SelectionSrcNum:3;	// Source from which the selected result came from
		uint32 src3SelectionEmpty:1;	// No selection exist bit
		uint32 src3SelectionValid:1;	// Selection valid bit
		uint32 reserved1:3;
	} bitFields;
} RegTxSelectorTxSelSrc3Selection_u;

/*REG_TX_SELECTOR_TX_SEL_SELECTED_SRC3_BITMAP 0x40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 selectedSrc3BitmapReq0:1;	// Request bit
		uint32 selectedSrc3BitmapPsType0:1;	// PS type
		uint32 selectedSrc3BitmapPsMode0:1;	// PS mode
		uint32 selectedSrc3BitmapDataInQ0:1;	// Data in Q
		uint32 selectedSrc3BitmapReq1:1;	// Request bit
		uint32 selectedSrc3BitmapPsType1:1;	// PS type
		uint32 selectedSrc3BitmapPsMode1:1;	// PS mode
		uint32 selectedSrc3BitmapDataInQ1:1;	// Data in Q
		uint32 selectedSrc3BitmapReq2:1;	// Request bit
		uint32 selectedSrc3BitmapPsType2:1;	// PS type
		uint32 selectedSrc3BitmapPsMode2:1;	// PS mode
		uint32 selectedSrc3BitmapDataInQ2:1;	// Data in Q
		uint32 selectedSrc3BitmapReq3:1;	// Request bit
		uint32 selectedSrc3BitmapPsType3:1;	// PS type
		uint32 selectedSrc3BitmapPsMode3:1;	// PS mode
		uint32 selectedSrc3BitmapDataInQ3:1;	// Data in Q
		uint32 reserved0:16;
	} bitFields;
} RegTxSelectorTxSelSelectedSrc3Bitmap_u;

/*REG_TX_SELECTOR_TX_SEL_SRC4_SELECTION 0x44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 src4SelectionTidNum:3;	// TID number
		uint32 src4SelectionStaNum:8;	// STA number
		uint32 src4SelectionVapNum:4;	// VAP number
		uint32 src4SelectionTxqId:3;	// Selected Queue index
		uint32 src4SelectionAcNum:2;	// AC number
		uint32 src4SelectionBf:2;	// Beam Forming bits
		uint32 reserved0:2;
		uint32 src4SelectionSrcNum:3;	// Source from which the selected result came from
		uint32 src4SelectionEmpty:1;	// No selection exist bit
		uint32 src4SelectionValid:1;	// Selection valid bit
		uint32 reserved1:3;
	} bitFields;
} RegTxSelectorTxSelSrc4Selection_u;

/*REG_TX_SELECTOR_TX_SEL_SELECTED_SRC4_BITMAP 0x48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:3;
		uint32 selectedSrc4BitmapDataInQ:1;	// Data in Q
		uint32 reserved1:28;
	} bitFields;
} RegTxSelectorTxSelSelectedSrc4Bitmap_u;

/*REG_TX_SELECTOR_TX_SEL_SRC5_SELECTION 0x4C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 src5SelectionTidNum:3;	// TID number
		uint32 src5SelectionStaNum:8;	// STA number
		uint32 src5SelectionVapNum:4;	// VAP number
		uint32 src5SelectionTxqId:3;	// Selected Queue index
		uint32 src5SelectionAcNum:2;	// AC number
		uint32 src5SelectionBf:2;	// Beam Forming bits
		uint32 reserved0:2;
		uint32 src5SelectionSrcNum:3;	// Source from which the selected result came from
		uint32 src5SelectionEmpty:1;	// No selection exist bit
		uint32 src5SelectionValid:1;	// Selection valid bit
		uint32 reserved1:3;
	} bitFields;
} RegTxSelectorTxSelSrc5Selection_u;

/*REG_TX_SELECTOR_TX_SEL_SELECTED_SRC5_BITMAP 0x50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:3;
		uint32 selectedSrc5BitmapDataInQ:1;	// Data in Q
		uint32 reserved1:28;
	} bitFields;
} RegTxSelectorTxSelSelectedSrc5Bitmap_u;

/*REG_TX_SELECTOR_TX_SEL_SRC6_SELECTION 0x54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 src6SelectionTidNum:3;	// TID number
		uint32 src6SelectionStaNum:8;	// STA number
		uint32 src6SelectionVapNum:4;	// VAP number
		uint32 src6SelectionTxqId:3;	// Selected Queue index
		uint32 src6SelectionAcNum:2;	// AC number
		uint32 src6SelectionBf:2;	// Beam Forming bits
		uint32 reserved0:2;
		uint32 src6SelectionSrcNum:3;	// Source from which the selected result came from
		uint32 src6SelectionEmpty:1;	// No selection exist bit
		uint32 src6SelectionValid:1;	// Selection valid bit
		uint32 reserved1:3;
	} bitFields;
} RegTxSelectorTxSelSrc6Selection_u;

/*REG_TX_SELECTOR_TX_SEL_SELECTED_SRC6_BITMAP 0x58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 selectedSrc6BitmapReq0:1;	// Request bit
		uint32 reserved0:2;
		uint32 selectedSrc6BitmapDataInQ0:1;	// Data in Q
		uint32 reserved1:28;
	} bitFields;
} RegTxSelectorTxSelSelectedSrc6Bitmap_u;

/*REG_TX_SELECTOR_TX_SEL_STATUS 0x5C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 statusSwFifoPopWhileEmpty:1;	// SW FIFO underflow 
		uint32 statusSwFifoPushWhileFull:1;	// SW FIFO overflow 
		uint32 statusSwFifoEmpty:1;	// SW FIFO empty 
		uint32 statusSwFifoFull:1;	// SW FIFO full 
		uint32 statusSwFifoHfull:1;	// SW FIFO half full 
		uint32 statusDataInQFifoEmpty:1;	// Data in Q FIFO empty 
		uint32 statusDataInQFifoFull:1;	// Data in Q FIFO full 
		uint32 dataInQFifoWasFull:1;
		uint32 statusInstruction:2;	// Last instruction recieved
		uint32 statusInstrSrc:1;	// Source of instruction
		uint32 reserved0:21;
	} bitFields;
} RegTxSelectorTxSelStatus_u;

/*REG_TX_SELECTOR_TX_SEL_ERR 0x60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 errSwInstrWhileBusy:1;	// New SW instruction to while selection is being done 
		uint32 errHwInstrWhileBusy:1;	// New HW instruction to while selection is being done 
		uint32 errDataLost:1;	// Data insertion overflow/underflow
		uint32 timWrongVapSel:16;	// Selection of VAP without Beacon enabled
		uint32 reserved0:1;
		uint32 errDataInQLost:1;
		uint32 reserved1:11;
	} bitFields;
} RegTxSelectorTxSelErr_u;

/*REG_TX_SELECTOR_TX_SEL_ERR_CLR 0x64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 errClrSwInstrWhileBusyClr:1;	// SW instruction error clear 
		uint32 errClrHwInstrWhileBusyClr:1;	// HW instruction error clear 
		uint32 errClrDataLostClr:1;	// Data insertion overflow/underflow error clear
		uint32 timWrongVapSelClr:16;	// Selection of VAP without Beacon enabled clear
		uint32 reserved0:1;
		uint32 errClrDataInQLostClr:1;
		uint32 reserved1:11;
	} bitFields;
} RegTxSelectorTxSelErrClr_u;

/*REG_TX_SELECTOR_TX_SEL_VAP_AC_MATRIX_0_3 0x68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vap0Ac:7;	// VAP0-AC vector 
		uint32 reserved0:1;
		uint32 vap1Ac:7;	// VAP1-AC vector 
		uint32 reserved1:1;
		uint32 vap2Ac:7;	// VAP2-AC vector 
		uint32 reserved2:1;
		uint32 vap3Ac:7;	// VAP3-AC vector 
		uint32 reserved3:1;
	} bitFields;
} RegTxSelectorTxSelVapAcMatrix03_u;

/*REG_TX_SELECTOR_TX_SEL_VAP_AC_MATRIX_4_7 0x6C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vap4Ac:7;	// VAP4-AC vector 
		uint32 reserved0:1;
		uint32 vap5Ac:7;	// VAP5-AC vector 
		uint32 reserved1:1;
		uint32 vap6Ac:7;	// VAP6-AC vector 
		uint32 reserved2:1;
		uint32 vap7Ac:7;	// VAP7-AC vector 
		uint32 reserved3:1;
	} bitFields;
} RegTxSelectorTxSelVapAcMatrix47_u;

/*REG_TX_SELECTOR_TX_SEL_VAP_AC_MATRIX_8_11 0x70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vap8Ac:7;	// VAP8-AC vector 
		uint32 reserved0:1;
		uint32 vap9Ac:7;	// VAP9-AC vector 
		uint32 reserved1:1;
		uint32 vap10Ac:7;	// VAP10-AC vector 
		uint32 reserved2:1;
		uint32 vap11Ac:7;	// VAP11-AC vector 
		uint32 reserved3:1;
	} bitFields;
} RegTxSelectorTxSelVapAcMatrix811_u;

/*REG_TX_SELECTOR_TX_SEL_VAP_AC_MATRIX_12_15 0x74 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vap12Ac:7;	// VAP12-AC vector 
		uint32 reserved0:1;
		uint32 vap13Ac:7;	// VAP13-AC vector 
		uint32 reserved1:1;
		uint32 vap14Ac:7;	// VAP14-AC vector 
		uint32 reserved2:1;
		uint32 vap15Ac:7;	// VAP15-AC vector 
		uint32 reserved3:1;
	} bitFields;
} RegTxSelectorTxSelVapAcMatrix1215_u;

/*REG_TX_SELECTOR_TX_SEL_STA_ASSOCIATE 0x78 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 staAssociateStation:8;	// Station to be associated with the selected VAP
		uint32 staAssociateVap:4;	// Selected VAP
		uint32 reserved0:4;
		uint32 staAssociateVapStaBitValue:1;	// Selected VAP
		uint32 reserved1:7;
		uint32 vapZero:1;	// When asserted, disconnect all stations of all VAPs
		uint32 reserved2:7;
	} bitFields;
} RegTxSelectorTxSelStaAssociate_u;

/*REG_TX_SELECTOR_TX_SEL_VAP_AC_BYPASS 0x90 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vapAcBypassAcSel:3;	// TX Handler AC sel bypass
		uint32 reserved0:5;
		uint32 vapAcBypassVapSel:4;	// TX Handler VAP sel bypass
		uint32 reserved1:4;
		uint32 vapAcBypassVaild:1;	// TX Handler bypass valid
		uint32 reserved2:15;
	} bitFields;
} RegTxSelectorTxSelVapAcBypass_u;

/*REG_TX_SELECTOR_TX_SEL_VAP_PS_STATE 0x94 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vapPsStateVapPsState:16;	// Each bit indicates its VAP PS state
		uint32 vapPsStateVapInPsAutoEn:1;	// Enables VAP in PS bit automatic update by TX Selector
		uint32 reserved0:15;
	} bitFields;
} RegTxSelectorTxSelVapPsState_u;

/*REG_TX_SELECTOR_TX_SEL_VAP_PS_NEW_IND 0x98 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vapPsNewIndPs:16;	// Each bit indicates its VAP PS state
		uint32 vapPsNewIndActive:16;	// Each bit indicates its VAP PS state
	} bitFields;
} RegTxSelectorTxSelVapPsNewInd_u;

/*REG_TX_SELECTOR_TX_SEL_TIM_BITMAP0 0x9C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 virtualBitmap0:32;	// Virtual Bitmap value. Bytes 0-3 of bitmap.
	} bitFields;
} RegTxSelectorTxSelTimBitmap0_u;

/*REG_TX_SELECTOR_TX_SEL_TIM_BITMAP1 0xA0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 virtualBitmap1:32;	// Virtual Bitmap value. Bytes 4-7 of bitmap.
	} bitFields;
} RegTxSelectorTxSelTimBitmap1_u;

/*REG_TX_SELECTOR_TX_SEL_TIM_BITMAP2 0xA4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 virtualBitmap2:32;	// Virtual Bitmap value. Bytes 8-11 of bitmap.
	} bitFields;
} RegTxSelectorTxSelTimBitmap2_u;

/*REG_TX_SELECTOR_TX_SEL_TIM_BITMAP3 0xA8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 virtualBitmap3:32;	// Virtual Bitmap value. Bytes 12-15 of bitmap.
	} bitFields;
} RegTxSelectorTxSelTimBitmap3_u;

/*REG_TX_SELECTOR_TX_SEL_TIM_BITMAP4 0xAC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 virtualBitmap4:8;	// Virtual Bitmap value. Bytes 16 of bitmap.
		uint32 reserved0:24;
	} bitFields;
} RegTxSelectorTxSelTimBitmap4_u;

/*REG_TX_SELECTOR_TX_SEL_TIM_HEADER 0xB0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dtimMcPs:1;	// DTIM with MC PS to broadcast
		uint32 control:7;	// Control field value
		uint32 dtimPeriod:8;	// DTIM period value
		uint32 dtimCount:8;	// DTIM counter value
		uint32 length:8;	// TIM IE Byte length
	} bitFields;
} RegTxSelectorTxSelTimHeader_u;

/*REG_TX_SELECTOR_TX_SEL_TIM_ELEMENT_ID 0xB4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 elementId:8;	// Element ID value
		uint32 reserved0:24;
	} bitFields;
} RegTxSelectorTxSelTimElementId_u;

/*REG_TX_SELECTOR_TX_SEL_TIM_HOB_POINTERS 0xB8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hobReadLength:6;	// Transmit length value read by the HOB
		uint32 reserved0:2;
		uint32 hobReadOffset:6;	// Transmit offset value read by the HOB
		uint32 reserved1:2;
		uint32 timEmpty:1;	// TIM IE empty indication read by HOB
		uint32 reserved2:7;
		uint32 timReady:1;	// TIM IE ready to be transmitted indication
		uint32 reserved3:7;
	} bitFields;
} RegTxSelectorTxSelTimHobPointers_u;

/*REG_TX_SELECTOR_TX_SEL_STA_ACTIVE_SEL_COUNT 0xBC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 staActiveCount:12;	// Counts Selection of STA ACTIVE 
		uint32 reserved0:20;
	} bitFields;
} RegTxSelectorTxSelStaActiveSelCount_u;

/*REG_TX_SELECTOR_TX_SEL_STA_PS_SEL_COUNT 0xC0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 staPsCount:12;	// Counts Selection of sta ps 
		uint32 reserved0:20;
	} bitFields;
} RegTxSelectorTxSelStaPsSelCount_u;

/*REG_TX_SELECTOR_TX_SEL_VAP_ACTIVE_SEL_COUNT 0xC4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vapActiveCount:12;	// Counts Selection of vap ACTIVE 
		uint32 reserved0:20;
	} bitFields;
} RegTxSelectorTxSelVapActiveSelCount_u;

/*REG_TX_SELECTOR_TX_SEL_VAP_PS_SEL_COUNT 0xC8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vapPsCount:12;	// Counts Selection of vap ps 
		uint32 reserved0:20;
	} bitFields;
} RegTxSelectorTxSelVapPsSelCount_u;

/*REG_TX_SELECTOR_TX_SEL_GLOBAL_SEL_COUNT 0xCC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 globalCount:12;	// Counts Selection of global 
		uint32 reserved0:20;
	} bitFields;
} RegTxSelectorTxSelGlobalSelCount_u;

/*REG_TX_SELECTOR_TX_SEL_GPLP_SEL_COUNT 0xD0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gplpCount:12;	// Counts Selection of gplp 
		uint32 reserved0:20;
	} bitFields;
} RegTxSelectorTxSelGplpSelCount_u;

/*REG_TX_SELECTOR_TX_SEL_GPHP_SEL_COUNT 0xD4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gphpCount:12;	// Counts Selection of gphp 
		uint32 reserved0:20;
	} bitFields;
} RegTxSelectorTxSelGphpSelCount_u;

/*REG_TX_SELECTOR_TX_SEL_BEACON_SEL_COUNT 0xD8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 beaconCount:12;	// Counts Selection of beacon 
		uint32 reserved0:20;
	} bitFields;
} RegTxSelectorTxSelBeaconSelCount_u;

/*REG_TX_SELECTOR_TX_SEL_EMPTY_SEL_COUNT 0xDC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 emptyCount:12;	// Counts Empty Selections
		uint32 reserved0:20;
	} bitFields;
} RegTxSelectorTxSelEmptySelCount_u;

/*REG_TX_SELECTOR_TX_SEL_COUNT_CLR 0xE0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 countersClr:1;	// Clears selection counters
		uint32 reserved0:31;
	} bitFields;
} RegTxSelectorTxSelCountClr_u;

/*REG_TX_SELECTOR_TX_SEL_LOCKER_EN 0xE4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lockerEn:1;
		uint32 reserved0:7;
		uint32 lockerSm:2;
		uint32 reserved1:22;
	} bitFields;
} RegTxSelectorTxSelLockerEn_u;

/*REG_TX_SELECTOR_TX_SEL_SW_MASTER0_LOCKER 0xE8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 master0LockerStaSel:8;	// Station number
		uint32 master0LockerTidSel:3;	// TID number
		uint32 reserved0:5;
		uint32 master0LockerBitmapSel:3;	// Bitmap Queue selection
		uint32 reserved1:5;
		uint32 master0LockerBitTypeSel:2;	// Information bit type selector
		uint32 reserved2:5;
		uint32 master0LockerAllTids:1;
	} bitFields;
} RegTxSelectorTxSelSwMaster0Locker_u;

/*REG_TX_SELECTOR_TX_SEL_SW_MASTER1_LOCKER 0xEC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 master1LockerStaSel:8;	// Station number
		uint32 master1LockerTidSel:3;	// TID number
		uint32 reserved0:5;
		uint32 master1LockerBitmapSel:3;	// Bitmap Queue selection
		uint32 reserved1:5;
		uint32 master1LockerBitTypeSel:2;	// Information bit type selector
		uint32 reserved2:5;
		uint32 master1LockerAllTids:1;
	} bitFields;
} RegTxSelectorTxSelSwMaster1Locker_u;

/*REG_TX_SELECTOR_TX_SEL_SW_MASTER2_LOCKER 0xF0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 master2LockerStaSel:8;	// Station number
		uint32 master2LockerTidSel:3;	// TID number
		uint32 reserved0:5;
		uint32 master2LockerBitmapSel:3;	// Bitmap Queue selection
		uint32 reserved1:5;
		uint32 master2LockerBitTypeSel:2;	// Information bit type selector
		uint32 reserved2:5;
		uint32 master2LockerAllTids:1;
	} bitFields;
} RegTxSelectorTxSelSwMaster2Locker_u;

/*REG_TX_SELECTOR_TX_SEL_SW_MASTER3_LOCKER 0xF4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 master3LockerStaSel:8;	// Station number
		uint32 master3LockerTidSel:3;	// TID number
		uint32 reserved0:5;
		uint32 master3LockerBitmapSel:3;	// Bitmap Queue selection
		uint32 reserved1:5;
		uint32 master3LockerBitTypeSel:2;	// Information bit type selector
		uint32 reserved2:5;
		uint32 master3LockerAllTids:1;
	} bitFields;
} RegTxSelectorTxSelSwMaster3Locker_u;

/*REG_TX_SELECTOR_TX_SEL_SW_MASTER0_LOCKER_STATUS 0xF8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swMaster0ReqStatus:5;
		uint32 reserved0:3;
		uint32 master0LockerDoneInt:1;
		uint32 reserved1:23;
	} bitFields;
} RegTxSelectorTxSelSwMaster0LockerStatus_u;

/*REG_TX_SELECTOR_TX_SEL_SW_MASTER1_LOCKER_STATUS 0xFC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swMaster1ReqStatus:5;
		uint32 reserved0:3;
		uint32 master1LockerDoneInt:1;
		uint32 reserved1:23;
	} bitFields;
} RegTxSelectorTxSelSwMaster1LockerStatus_u;

/*REG_TX_SELECTOR_TX_SEL_SW_MASTER2_LOCKER_STATUS 0x100 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swMaster2ReqStatus:5;
		uint32 reserved0:3;
		uint32 master2LockerDoneInt:1;
		uint32 reserved1:23;
	} bitFields;
} RegTxSelectorTxSelSwMaster2LockerStatus_u;

/*REG_TX_SELECTOR_TX_SEL_SW_MASTER3_LOCKER_STATUS 0x104 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swMaster3ReqStatus:5;
		uint32 reserved0:3;
		uint32 master3LockerDoneInt:1;
		uint32 reserved1:23;
	} bitFields;
} RegTxSelectorTxSelSwMaster3LockerStatus_u;

/*REG_TX_SELECTOR_TX_SEL_LOCKER_INT_CLR 0x108 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swMaster0LockerIntClr:1;
		uint32 swMaster1LockerIntClr:1;
		uint32 swMaster2LockerIntClr:1;
		uint32 swMaster3LockerIntClr:1;
		uint32 reserved0:28;
	} bitFields;
} RegTxSelectorTxSelLockerIntClr_u;

/*REG_TX_SELECTOR_TX_SEL_LOCKER_PUSH_WHILE_BUSY 0x10C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 master0PushWhileBusy:1;
		uint32 master1PushWhileBusy:1;
		uint32 master2PushWhileBusy:1;
		uint32 master3PushWhileBusy:1;
		uint32 reserved0:28;
	} bitFields;
} RegTxSelectorTxSelLockerPushWhileBusy_u;

/*REG_TX_SELECTOR_TX_SEL_LOCKER_PUSH_WHILE_BUSY_CLR 0x110 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 master0PushWhileBusyClr:1;
		uint32 master1PushWhileBusyClr:1;
		uint32 master2PushWhileBusyClr:1;
		uint32 master3PushWhileBusyClr:1;
		uint32 reserved0:28;
	} bitFields;
} RegTxSelectorTxSelLockerPushWhileBusyClr_u;

/*REG_TX_SELECTOR_TX_SELECTOR_SPARE 0x114 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSelectorSpare:32;
	} bitFields;
} RegTxSelectorTxSelectorSpare_u;

/*REG_TX_SELECTOR_TX_SEL_LOGGER_CFG 0x118 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSelLoggerSelectionLockerPriority:2;
		uint32 txSelLoggerSelectionLockerEn:1;
		uint32 txSelLoggerSelectionLockerSel:1;	// 0x0: Selection logger , 0x1: Locker logger
		uint32 txSelLoggerBitmapPriority:2;
		uint32 txSelLoggerBitmapUpdateEn:1;
		uint32 reserved0:1;
		uint32 txSelLoggerMuLockerPriority:2;
		uint32 txSelLoggerMuLockerEn:1;
		uint32 reserved1:1;
		uint32 txSelLoggerMuBitmapPriority:2;
		uint32 txSelLoggerMuBitmapUpdateEn:1;
		uint32 reserved2:1;
		uint32 txSelLoggerMuSelectionEn:1;
		uint32 reserved3:15;
	} bitFields;
} RegTxSelectorTxSelLoggerCfg_u;

/*REG_TX_SELECTOR_TX_SEL_LOGGER_BUSY 0x11C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSelLoggerSelectionLockerBusy:1;
		uint32 reserved0:3;
		uint32 txSelLoggerMuLockerBusy:1;
		uint32 reserved1:3;
		uint32 txSelLoggerBitmapUpdateBusy:1;
		uint32 reserved2:3;
		uint32 txSelLoggerMuBitmapUpdateBusy:1;
		uint32 reserved3:19;
	} bitFields;
} RegTxSelectorTxSelLoggerBusy_u;

/*REG_TX_SELECTOR_TX_SEL_MU_SELECT_CFG 0x200 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muEn:1;	// MU enable of the TX Selector
		uint32 reserved0:3;
		uint32 muSecondarySel:1;	// MU secondary selection algorithm: , 0: MU Secondary TID equal to Primary TID , 1: MU Secondary TID can be any TID , 
		uint32 reserved1:3;
		uint32 maxGroupNum:8;	// Maximum number of supported groups. Number of groups is equal to max_group_num+1.
		uint32 selectorMuLockEn:1;	// Enables/Disables the TX Selector HW lock: , 0x0: Don't lock after selection , 0x1: Lock after selection
		uint32 reserved2:15;
	} bitFields;
} RegTxSelectorTxSelMuSelectCfg_u;

/*REG_TX_SELECTOR_TX_SEL_MU_BITMAP_UPDATE 0x204 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 memLineNum:7;	// MU Group index number. Can be 0-15.
		uint32 reserved0:1;
		uint32 userPositionIdx:2;	// User position index number in a group
		uint32 reserved1:2;
		uint32 muBitTypeSel:3;	// MU bit type: , 0x0: Group User position STA , 0x1: Group lock , 0x2: Group Tx mode , 0x3: STA group mapping , 0x4: STA MU tid Enable , 
		uint32 reserved2:1;
		uint32 muBitmapUpdateValue:8;	// Write data value - For each update there is a different data width. Data decoding is according to mu_bit_type_sel: , 0x0: 7 LSB bits are STA num, MSB is MU STA enable bit. , 0x1: LSB bit is MU lock bit value. , 0x2: 2 LSB bits are TX mode value. , 0x3: 7 LSB bits are Group index, MSB is STA Primary enable bit. , 0x4: 8 bits for 8 TIDs enable/disable MU.
		uint32 allGrp:1;	// Indication that the lower 32 bits of the group are updated with the adjacent register data.
		uint32 reserved3:3;
		uint32 muBitmapUpdateFree:1;	// Bit inidcating that the register is free for update instruction
		uint32 reserved4:3;
	} bitFields;
} RegTxSelectorTxSelMuBitmapUpdate_u;

/*REG_TX_SELECTOR_TX_SEL_MU_BITMAP_UPDATE_ALL_STA_WDATA 0x208 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 allGrpWdata:32;
	} bitFields;
} RegTxSelectorTxSelMuBitmapUpdateAllStaWdata_u;

/*REG_TX_SELECTOR_TX_SEL_MU_PRIMARY_THRES 0x20C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pdPrimaryThres:16;
		uint32 reserved0:16;
	} bitFields;
} RegTxSelectorTxSelMuPrimaryThres_u;

/*REG_TX_SELECTOR_TX_SEL_MU_SECONDARY_THRES 0x210 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pdSecondaryThres:16;
		uint32 reserved0:16;
	} bitFields;
} RegTxSelectorTxSelMuSecondaryThres_u;

/*REG_TX_SELECTOR_TX_SEL_MU_ERR 0x214 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 primaryNotValidErr:1;
		uint32 noPrimaryInGrpErr:1;
		uint32 primaryGrpThresErr:1;
		uint32 reserved0:29;
	} bitFields;
} RegTxSelectorTxSelMuErr_u;

/*REG_TX_SELECTOR_TX_SEL_MU_ERR_CLR 0x218 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 primaryNotValidErrClr:1;
		uint32 noPrimaryInGrpErrClr:1;
		uint32 primaryGrpThresErrClr:1;
		uint32 reserved0:29;
	} bitFields;
} RegTxSelectorTxSelMuErrClr_u;

/*REG_TX_SELECTOR_TX_SEL_MU_LOCKER_EN 0x220 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muLockerEn:1;
		uint32 reserved0:31;
	} bitFields;
} RegTxSelectorTxSelMuLockerEn_u;

/*REG_TX_SELECTOR_TX_SEL_MU_SW_MASTER0_LOCKER 0x224 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 master0MuLockerGrpSel:8;	// Group index number
		uint32 reserved0:16;
		uint32 master0MuLockerBitTypeSel:2;	// Information bit type selector
		uint32 reserved1:6;
	} bitFields;
} RegTxSelectorTxSelMuSwMaster0Locker_u;

/*REG_TX_SELECTOR_TX_SEL_MU_SW_MASTER1_LOCKER 0x228 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 master1MuLockerGrpSel:8;	// Group index number
		uint32 reserved0:16;
		uint32 master1MuLockerBitTypeSel:2;	// Information bit type selector
		uint32 reserved1:6;
	} bitFields;
} RegTxSelectorTxSelMuSwMaster1Locker_u;

/*REG_TX_SELECTOR_TX_SEL_MU_SW_MASTER2_LOCKER 0x22C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 master2MuLockerGrpSel:8;	// Group index number
		uint32 reserved0:16;
		uint32 master2MuLockerBitTypeSel:2;	// Information bit type selector
		uint32 reserved1:6;
	} bitFields;
} RegTxSelectorTxSelMuSwMaster2Locker_u;

/*REG_TX_SELECTOR_TX_SEL_MU_SW_MASTER3_LOCKER 0x230 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 master3MuLockerGrpSel:8;	// Group index number
		uint32 reserved0:16;
		uint32 master3MuLockerBitTypeSel:2;	// Information bit type selector
		uint32 reserved1:6;
	} bitFields;
} RegTxSelectorTxSelMuSwMaster3Locker_u;

/*REG_TX_SELECTOR_TX_SEL_MU_SW_MASTER0_LOCKER_STATUS 0x234 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swMaster0MuReqStatus:5;
		uint32 reserved0:3;
		uint32 master0MuLockerDoneInt:1;
		uint32 reserved1:23;
	} bitFields;
} RegTxSelectorTxSelMuSwMaster0LockerStatus_u;

/*REG_TX_SELECTOR_TX_SEL_MU_SW_MASTER1_LOCKER_STATUS 0x238 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swMaster1MuReqStatus:5;
		uint32 reserved0:3;
		uint32 master1MuLockerDoneInt:1;
		uint32 reserved1:23;
	} bitFields;
} RegTxSelectorTxSelMuSwMaster1LockerStatus_u;

/*REG_TX_SELECTOR_TX_SEL_MU_SW_MASTER2_LOCKER_STATUS 0x23C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swMaster2MuReqStatus:5;
		uint32 reserved0:3;
		uint32 master2MuLockerDoneInt:1;
		uint32 reserved1:23;
	} bitFields;
} RegTxSelectorTxSelMuSwMaster2LockerStatus_u;

/*REG_TX_SELECTOR_TX_SEL_MU_SW_MASTER3_LOCKER_STATUS 0x240 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swMaster3MuReqStatus:5;
		uint32 reserved0:3;
		uint32 master3MuLockerDoneInt:1;
		uint32 reserved1:23;
	} bitFields;
} RegTxSelectorTxSelMuSwMaster3LockerStatus_u;

/*REG_TX_SELECTOR_TX_SEL_MU_LOCKER_INT_CLR 0x244 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swMaster0MuLockerIntClr:1;
		uint32 swMaster1MuLockerIntClr:1;
		uint32 swMaster2MuLockerIntClr:1;
		uint32 swMaster3MuLockerIntClr:1;
		uint32 reserved0:28;
	} bitFields;
} RegTxSelectorTxSelMuLockerIntClr_u;

/*REG_TX_SELECTOR_TX_SEL_MU_LOCKER_PUSH_WHILE_BUSY 0x248 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 master0MuPushWhileBusy:1;
		uint32 master1MuPushWhileBusy:1;
		uint32 master2MuPushWhileBusy:1;
		uint32 master3MuPushWhileBusy:1;
		uint32 reserved0:28;
	} bitFields;
} RegTxSelectorTxSelMuLockerPushWhileBusy_u;

/*REG_TX_SELECTOR_TX_SEL_MU_LOCKER_PUSH_WHILE_BUSY_CLR 0x24C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 master0MuPushWhileBusyClr:1;
		uint32 master1MuPushWhileBusyClr:1;
		uint32 master2MuPushWhileBusyClr:1;
		uint32 master3MuPushWhileBusyClr:1;
		uint32 reserved0:28;
	} bitFields;
} RegTxSelectorTxSelMuLockerPushWhileBusyClr_u;

/*REG_TX_SELECTOR_TX_SEL_MU_UP0_RESULT 0x250 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muUp0Tid:3;
		uint32 muUp0Sta:7;
		uint32 reserved0:1;
		uint32 muUp0Vap:4;
		uint32 muUp0TxqId:3;
		uint32 muUp0AcId:2;
		uint32 muUp0BfReq:2;
		uint32 reserved1:6;
		uint32 muUp0Valid:1;
		uint32 reserved2:3;
	} bitFields;
} RegTxSelectorTxSelMuUp0Result_u;

/*REG_TX_SELECTOR_TX_SEL_MU_UP1_RESULT 0x254 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muUp1Tid:3;
		uint32 muUp1Sta:7;
		uint32 reserved0:1;
		uint32 muUp1Vap:4;
		uint32 muUp1TxqId:3;
		uint32 muUp1AcId:2;
		uint32 muUp1BfReq:2;
		uint32 reserved1:6;
		uint32 muUp1Valid:1;
		uint32 reserved2:3;
	} bitFields;
} RegTxSelectorTxSelMuUp1Result_u;

/*REG_TX_SELECTOR_TX_SEL_MU_UP2_RESULT 0x258 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muUp2Tid:3;
		uint32 muUp2Sta:7;
		uint32 reserved0:1;
		uint32 muUp2Vap:4;
		uint32 muUp2TxqId:3;
		uint32 muUp2AcId:2;
		uint32 muUp2BfReq:2;
		uint32 reserved1:6;
		uint32 muUp2Valid:1;
		uint32 reserved2:3;
	} bitFields;
} RegTxSelectorTxSelMuUp2Result_u;

/*REG_TX_SELECTOR_TX_SEL_MU_UP3_RESULT 0x25C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muUp3Tid:3;
		uint32 muUp3Sta:7;
		uint32 reserved0:1;
		uint32 muUp3Vap:4;
		uint32 muUp3TxqId:3;
		uint32 muUp3AcId:2;
		uint32 muUp3BfReq:2;
		uint32 reserved1:6;
		uint32 muUp3Valid:1;
		uint32 reserved2:3;
	} bitFields;
} RegTxSelectorTxSelMuUp3Result_u;

/*REG_TX_SELECTOR_TX_SEL_MU_UP0_BITMAP 0x260 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muUp0BitmapReq0:1;	// Request bit
		uint32 muUp0BitmapPsType0:1;	// PS type
		uint32 muUp0BitmapPsMode0:1;	// PS mode
		uint32 muUp0BitmapDataInQ0:1;	// Data in Q
		uint32 muUp0BitmapReq1:1;	// Request bit
		uint32 muUp0BitmapPsType1:1;	// PS type
		uint32 muUp0BitmapPsMode1:1;	// PS mode
		uint32 muUp0BitmapDataInQ1:1;	// Data in Q
		uint32 muUp0BitmapReq2:1;	// Request bit
		uint32 muUp0BitmapPsType2:1;	// PS type
		uint32 muUp0BitmapPsMode2:1;	// PS mode
		uint32 muUp0BitmapDataInQ2:1;	// Data in Q
		uint32 muUp0BitmapReq3:1;	// Request bit
		uint32 muUp0BitmapPsType3:1;	// PS type
		uint32 muUp0BitmapPsMode3:1;	// PS mode
		uint32 muUp0BitmapDataInQ3:1;	// Data in Q
		uint32 muUp0BitmapReq4:1;	// Request bit
		uint32 muUp0BitmapPsType4:1;	// PS type
		uint32 muUp0BitmapPsMode4:1;	// PS mode
		uint32 muUp0BitmapDataInQ4:1;	// Data in Q
		uint32 muUp0BitmapReq5:1;	// Request bit
		uint32 muUp0BitmapPsType5:1;	// PS type
		uint32 muUp0BitmapPsMode5:1;	// PS mode
		uint32 muUp0BitmapDataInQ5:1;	// Data in Q
		uint32 muUp0BitmapReq6:1;	// Request bit
		uint32 muUp0BitmapPsType6:1;	// PS type
		uint32 muUp0BitmapPsMode6:1;	// PS mode
		uint32 muUp0BitmapDataInQ6:1;	// Data in Q
		uint32 muUp0BitmapReq7:1;	// Request bit
		uint32 muUp0BitmapPsType7:1;	// PS type
		uint32 muUp0BitmapPsMode7:1;	// PS mode
		uint32 muUp0BitmapDataInQ7:1;	// Data in Q
	} bitFields;
} RegTxSelectorTxSelMuUp0Bitmap_u;

/*REG_TX_SELECTOR_TX_SEL_MU_UP1_BITMAP 0x264 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muUp1BitmapReq0:1;	// Request bit
		uint32 muUp1BitmapPsType0:1;	// PS type
		uint32 muUp1BitmapPsMode0:1;	// PS mode
		uint32 muUp1BitmapDataInQ0:1;	// Data in Q
		uint32 muUp1BitmapReq1:1;	// Request bit
		uint32 muUp1BitmapPsType1:1;	// PS type
		uint32 muUp1BitmapPsMode1:1;	// PS mode
		uint32 muUp1BitmapDataInQ1:1;	// Data in Q
		uint32 muUp1BitmapReq2:1;	// Request bit
		uint32 muUp1BitmapPsType2:1;	// PS type
		uint32 muUp1BitmapPsMode2:1;	// PS mode
		uint32 muUp1BitmapDataInQ2:1;	// Data in Q
		uint32 muUp1BitmapReq3:1;	// Request bit
		uint32 muUp1BitmapPsType3:1;	// PS type
		uint32 muUp1BitmapPsMode3:1;	// PS mode
		uint32 muUp1BitmapDataInQ3:1;	// Data in Q
		uint32 muUp1BitmapReq4:1;	// Request bit
		uint32 muUp1BitmapPsType4:1;	// PS type
		uint32 muUp1BitmapPsMode4:1;	// PS mode
		uint32 muUp1BitmapDataInQ4:1;	// Data in Q
		uint32 muUp1BitmapReq5:1;	// Request bit
		uint32 muUp1BitmapPsType5:1;	// PS type
		uint32 muUp1BitmapPsMode5:1;	// PS mode
		uint32 muUp1BitmapDataInQ5:1;	// Data in Q
		uint32 muUp1BitmapReq6:1;	// Request bit
		uint32 muUp1BitmapPsType6:1;	// PS type
		uint32 muUp1BitmapPsMode6:1;	// PS mode
		uint32 muUp1BitmapDataInQ6:1;	// Data in Q
		uint32 muUp1BitmapReq7:1;	// Request bit
		uint32 muUp1BitmapPsType7:1;	// PS type
		uint32 muUp1BitmapPsMode7:1;	// PS mode
		uint32 muUp1BitmapDataInQ7:1;	// Data in Q
	} bitFields;
} RegTxSelectorTxSelMuUp1Bitmap_u;

/*REG_TX_SELECTOR_TX_SEL_MU_UP2_BITMAP 0x268 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muUp2BitmapReq0:1;	// Request bit
		uint32 muUp2BitmapPsType0:1;	// PS type
		uint32 muUp2BitmapPsMode0:1;	// PS mode
		uint32 muUp2BitmapDataInQ0:1;	// Data in Q
		uint32 muUp2BitmapReq1:1;	// Request bit
		uint32 muUp2BitmapPsType1:1;	// PS type
		uint32 muUp2BitmapPsMode1:1;	// PS mode
		uint32 muUp2BitmapDataInQ1:1;	// Data in Q
		uint32 muUp2BitmapReq2:1;	// Request bit
		uint32 muUp2BitmapPsType2:1;	// PS type
		uint32 muUp2BitmapPsMode2:1;	// PS mode
		uint32 muUp2BitmapDataInQ2:1;	// Data in Q
		uint32 muUp2BitmapReq3:1;	// Request bit
		uint32 muUp2BitmapPsType3:1;	// PS type
		uint32 muUp2BitmapPsMode3:1;	// PS mode
		uint32 muUp2BitmapDataInQ3:1;	// Data in Q
		uint32 muUp2BitmapReq4:1;	// Request bit
		uint32 muUp2BitmapPsType4:1;	// PS type
		uint32 muUp2BitmapPsMode4:1;	// PS mode
		uint32 muUp2BitmapDataInQ4:1;	// Data in Q
		uint32 muUp2BitmapReq5:1;	// Request bit
		uint32 muUp2BitmapPsType5:1;	// PS type
		uint32 muUp2BitmapPsMode5:1;	// PS mode
		uint32 muUp2BitmapDataInQ5:1;	// Data in Q
		uint32 muUp2BitmapReq6:1;	// Request bit
		uint32 muUp2BitmapPsType6:1;	// PS type
		uint32 muUp2BitmapPsMode6:1;	// PS mode
		uint32 muUp2BitmapDataInQ6:1;	// Data in Q
		uint32 muUp2BitmapReq7:1;	// Request bit
		uint32 muUp2BitmapPsType7:1;	// PS type
		uint32 muUp2BitmapPsMode7:1;	// PS mode
		uint32 muUp2BitmapDataInQ7:1;	// Data in Q
	} bitFields;
} RegTxSelectorTxSelMuUp2Bitmap_u;

/*REG_TX_SELECTOR_TX_SEL_MU_UP3_BITMAP 0x26C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muUp3BitmapReq0:1;	// Request bit
		uint32 muUp3BitmapPsType0:1;	// PS type
		uint32 muUp3BitmapPsMode0:1;	// PS mode
		uint32 muUp3BitmapDataInQ0:1;	// Data in Q
		uint32 muUp3BitmapReq1:1;	// Request bit
		uint32 muUp3BitmapPsType1:1;	// PS type
		uint32 muUp3BitmapPsMode1:1;	// PS mode
		uint32 muUp3BitmapDataInQ1:1;	// Data in Q
		uint32 muUp3BitmapReq2:1;	// Request bit
		uint32 muUp3BitmapPsType2:1;	// PS type
		uint32 muUp3BitmapPsMode2:1;	// PS mode
		uint32 muUp3BitmapDataInQ2:1;	// Data in Q
		uint32 muUp3BitmapReq3:1;	// Request bit
		uint32 muUp3BitmapPsType3:1;	// PS type
		uint32 muUp3BitmapPsMode3:1;	// PS mode
		uint32 muUp3BitmapDataInQ3:1;	// Data in Q
		uint32 muUp3BitmapReq4:1;	// Request bit
		uint32 muUp3BitmapPsType4:1;	// PS type
		uint32 muUp3BitmapPsMode4:1;	// PS mode
		uint32 muUp3BitmapDataInQ4:1;	// Data in Q
		uint32 muUp3BitmapReq5:1;	// Request bit
		uint32 muUp3BitmapPsType5:1;	// PS type
		uint32 muUp3BitmapPsMode5:1;	// PS mode
		uint32 muUp3BitmapDataInQ5:1;	// Data in Q
		uint32 muUp3BitmapReq6:1;	// Request bit
		uint32 muUp3BitmapPsType6:1;	// PS type
		uint32 muUp3BitmapPsMode6:1;	// PS mode
		uint32 muUp3BitmapDataInQ6:1;	// Data in Q
		uint32 muUp3BitmapReq7:1;	// Request bit
		uint32 muUp3BitmapPsType7:1;	// PS type
		uint32 muUp3BitmapPsMode7:1;	// PS mode
		uint32 muUp3BitmapDataInQ7:1;	// Data in Q
	} bitFields;
} RegTxSelectorTxSelMuUp3Bitmap_u;

/*REG_TX_SELECTOR_TX_SEL_MU_GRP_BITMAP 0x270 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muGrpValid:1;	// MU selection valid bit
		uint32 primaryGrpPointer:7;	// Group number
		uint32 primaryIdx:2;	// Index number indicating which UP is primary at the group
		uint32 txMode:2;
		uint32 upLockDone:4;	// 4 bits, 1 bit per UP indicating which UP was locked at the SU bitmap
		uint32 grpLockDone:1;	// MU group lock done/Not done
		uint32 reserved0:15;
	} bitFields;
} RegTxSelectorTxSelMuGrpBitmap_u;

/*REG_TX_SELECTOR_TX_SEL_MU_SM_STATUS 0x274 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muSelSm:4;	// MU selection main SM
		uint32 muSelLockSm:3;	// MU lock SM
		uint32 reserved0:1;
		uint32 secPdCountRdSm:3;	// RD read from QoS RAM SM
		uint32 reserved1:1;
		uint32 muLockerSm:2;
		uint32 reserved2:18;
	} bitFields;
} RegTxSelectorTxSelMuSmStatus_u;

/*REG_TX_SELECTOR_TX_SEL_MU_SU_REPORT_STATUS 0x278 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSelMuSuSecondary0:4;	// MU secondary participated in the MU TX with reason. Only one reason will be displayed according to this Prioirty list: , 0-  Selected (i.e. there was a TX for that user position) , 1-  Selector – Secondary not valid in group , 2 - Selector – STA disabled , 3-  Selector – No data in queue , 4-  Selector – STA lock , 5-  Selector – PS without request , 6 - Selector – PD threshold fail , 7 - Selector – TID MU En bit is 0
		uint32 txSelMuSuSecondary1:4;	// MU secondary participated in the MU TX with reason. Only one reason will be displayed according to this Prioirty list: , 0-  Selected (i.e. there was a TX for that user position) , 1-  Selector – Secondary not valid in group , 2 - Selector – STA disabled , 3-  Selector – No data in queue , 4-  Selector – STA lock , 5-  Selector – PS without request , 6 - Selector – PD threshold fail , 7 - Selector – TID MU En bit is 0
		uint32 txSelMuSuSecondary2:4;	// MU secondary participated in the MU TX with reason. Only one reason will be displayed according to this Prioirty list: , 0-  Selected (i.e. there was a TX for that user position) , 1-  Selector – Secondary not valid in group , 2 - Selector – STA disabled , 3-  Selector – No data in queue , 4-  Selector – STA lock , 5-  Selector – PS without request , 6 - Selector – PD threshold fail , 7 - Selector – TID MU En bit is 0
		uint32 txSelMuSuSecondary3:4;	// MU secondary participated in the MU TX with reason. Only one reason will be displayed according to this Prioirty list: , 0-  Selected (i.e. there was a TX for that user position) , 1-  Selector – Secondary not valid in group , 2 - Selector – STA disabled , 3-  Selector – No data in queue , 4-  Selector – STA lock , 5-  Selector – PS without request , 6 - Selector – PD threshold fail , 7 - Selector – TID MU En bit is 0
		uint32 txSelMuSuPrimary:6;	// MU was changed to SU because of Primary(More than one reason possible): , Bit0 – Primary not in group , Bit1 – Primary is not valid , Bit2 – Group number is bigger than threshold , Bit3 – Group locked , Bit4 – All secondaries are not valid , Bit5 – No MU because of PS legacy selection
		uint32 reserved0:2;
		uint32 txSelMuSuValid:1;
		uint32 reserved1:7;
	} bitFields;
} RegTxSelectorTxSelMuSuReportStatus_u;



#endif // _TX_SELECTOR_REGS_H_
