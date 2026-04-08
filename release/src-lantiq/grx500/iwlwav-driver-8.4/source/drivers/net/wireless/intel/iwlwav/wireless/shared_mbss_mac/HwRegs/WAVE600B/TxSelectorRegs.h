
/***********************************************************************************
File:				TxSelectorRegs.h
Module:				TxSelector
SOC Revision:		
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
#define	REG_TX_SELECTOR_TX_SEL_INSTR                              (TX_SELECTOR_BASE_ADDRESS + 0x0)
#define	REG_TX_SELECTOR_TX_SEL_BITMAP_UPDATE_MASTER0              (TX_SELECTOR_BASE_ADDRESS + 0x4)
#define	REG_TX_SELECTOR_TX_SEL_BITMAP_UPDATE_DATA_MASTER0         (TX_SELECTOR_BASE_ADDRESS + 0x8)
#define	REG_TX_SELECTOR_TX_SEL_BITMAP_UPDATE_MASTER1              (TX_SELECTOR_BASE_ADDRESS + 0xC)
#define	REG_TX_SELECTOR_TX_SEL_BITMAP_UPDATE_DATA_MASTER1         (TX_SELECTOR_BASE_ADDRESS + 0x10)
#define	REG_TX_SELECTOR_TX_SEL_BITMAP_UPDATE_MASTER2              (TX_SELECTOR_BASE_ADDRESS + 0x14)
#define	REG_TX_SELECTOR_TX_SEL_BITMAP_UPDATE_DATA_MASTER2         (TX_SELECTOR_BASE_ADDRESS + 0x18)
#define	REG_TX_SELECTOR_TX_SEL_BITMAP_UPDATE_MASTER3              (TX_SELECTOR_BASE_ADDRESS + 0x1C)
#define	REG_TX_SELECTOR_TX_SEL_BITMAP_UPDATE_DATA_MASTER3         (TX_SELECTOR_BASE_ADDRESS + 0x20)
#define	REG_TX_SELECTOR_TX_SEL_STA_PS_WRR_LOW0                    (TX_SELECTOR_BASE_ADDRESS + 0x24)
#define	REG_TX_SELECTOR_TX_SEL_STA_PS_WRR_LOW1                    (TX_SELECTOR_BASE_ADDRESS + 0x28)
#define	REG_TX_SELECTOR_TX_SEL_STA_PS_WRR_HIGH0                   (TX_SELECTOR_BASE_ADDRESS + 0x2C)
#define	REG_TX_SELECTOR_TX_SEL_STA_PS_WRR_HIGH1                   (TX_SELECTOR_BASE_ADDRESS + 0x30)
#define	REG_TX_SELECTOR_TX_SEL_STA_ACTIVE_WRR_LOW0                (TX_SELECTOR_BASE_ADDRESS + 0x34)
#define	REG_TX_SELECTOR_TX_SEL_STA_ACTIVE_WRR_LOW1                (TX_SELECTOR_BASE_ADDRESS + 0x38)
#define	REG_TX_SELECTOR_TX_SEL_STA_ACTIVE_WRR_HIGH0               (TX_SELECTOR_BASE_ADDRESS + 0x3C)
#define	REG_TX_SELECTOR_TX_SEL_STA_ACTIVE_WRR_HIGH1               (TX_SELECTOR_BASE_ADDRESS + 0x40)
#define	REG_TX_SELECTOR_TX_SEL_STRICT_PRIO                        (TX_SELECTOR_BASE_ADDRESS + 0x44)
#define	REG_TX_SELECTOR_TX_SEL_SELECTION                          (TX_SELECTOR_BASE_ADDRESS + 0x48)
#define	REG_TX_SELECTOR_TX_SEL_SELECTED_BITMAP                    (TX_SELECTOR_BASE_ADDRESS + 0x4C)
#define	REG_TX_SELECTOR_TX_SEL_SELECTED_BITMAP_MGMT               (TX_SELECTOR_BASE_ADDRESS + 0x50)
#define	REG_TX_SELECTOR_TX_SEL_SRC0_SELECTION                     (TX_SELECTOR_BASE_ADDRESS + 0x54)
#define	REG_TX_SELECTOR_TX_SEL_SELECTED_SRC0_BITMAP               (TX_SELECTOR_BASE_ADDRESS + 0x58)
#define	REG_TX_SELECTOR_TX_SEL_SELECTED_SRC0_BITMAP_MGMT          (TX_SELECTOR_BASE_ADDRESS + 0x5C)
#define	REG_TX_SELECTOR_TX_SEL_SRC1_SELECTION                     (TX_SELECTOR_BASE_ADDRESS + 0x60)
#define	REG_TX_SELECTOR_TX_SEL_SELECTED_SRC1_BITMAP               (TX_SELECTOR_BASE_ADDRESS + 0x64)
#define	REG_TX_SELECTOR_TX_SEL_SELECTED_SRC1_BITMAP_MGMT          (TX_SELECTOR_BASE_ADDRESS + 0x68)
#define	REG_TX_SELECTOR_TX_SEL_SRC2_SELECTION                     (TX_SELECTOR_BASE_ADDRESS + 0x6C)
#define	REG_TX_SELECTOR_TX_SEL_SELECTED_SRC2_BITMAP               (TX_SELECTOR_BASE_ADDRESS + 0x70)
#define	REG_TX_SELECTOR_TX_SEL_SELECTED_SRC2_BITMAP_MGMT          (TX_SELECTOR_BASE_ADDRESS + 0x74)
#define	REG_TX_SELECTOR_TX_SEL_SRC3_SELECTION                     (TX_SELECTOR_BASE_ADDRESS + 0x78)
#define	REG_TX_SELECTOR_TX_SEL_SELECTED_SRC3_BITMAP               (TX_SELECTOR_BASE_ADDRESS + 0x7C)
#define	REG_TX_SELECTOR_TX_SEL_SRC4_SELECTION                     (TX_SELECTOR_BASE_ADDRESS + 0x80)
#define	REG_TX_SELECTOR_TX_SEL_SELECTED_SRC4_BITMAP               (TX_SELECTOR_BASE_ADDRESS + 0x84)
#define	REG_TX_SELECTOR_TX_SEL_SRC5_SELECTION                     (TX_SELECTOR_BASE_ADDRESS + 0x88)
#define	REG_TX_SELECTOR_TX_SEL_SELECTED_SRC5_BITMAP               (TX_SELECTOR_BASE_ADDRESS + 0x8C)
#define	REG_TX_SELECTOR_TX_SEL_SRC6_SELECTION                     (TX_SELECTOR_BASE_ADDRESS + 0x90)
#define	REG_TX_SELECTOR_TX_SEL_SELECTED_SRC6_BITMAP               (TX_SELECTOR_BASE_ADDRESS + 0x94)
#define	REG_TX_SELECTOR_TX_SEL_STATUS                             (TX_SELECTOR_BASE_ADDRESS + 0x98)
#define	REG_TX_SELECTOR_TX_SEL_ERR                                (TX_SELECTOR_BASE_ADDRESS + 0x9C)
#define	REG_TX_SELECTOR_TX_SEL_ERR_CLR                            (TX_SELECTOR_BASE_ADDRESS + 0xA0)
#define	REG_TX_SELECTOR_TX_SEL_DL_VAP_AC_MATRIX_0_3               (TX_SELECTOR_BASE_ADDRESS + 0xA4)
#define	REG_TX_SELECTOR_TX_SEL_DL_VAP_AC_MATRIX_4_7               (TX_SELECTOR_BASE_ADDRESS + 0xA8)
#define	REG_TX_SELECTOR_TX_SEL_DL_VAP_AC_MATRIX_8_11              (TX_SELECTOR_BASE_ADDRESS + 0xAC)
#define	REG_TX_SELECTOR_TX_SEL_DL_VAP_AC_MATRIX_12_15             (TX_SELECTOR_BASE_ADDRESS + 0xB0)
#define	REG_TX_SELECTOR_TX_SEL_DL_VAP_AC_MATRIX_16_19             (TX_SELECTOR_BASE_ADDRESS + 0xB4)
#define	REG_TX_SELECTOR_TX_SEL_DL_VAP_AC_MATRIX_20_23             (TX_SELECTOR_BASE_ADDRESS + 0xB8)
#define	REG_TX_SELECTOR_TX_SEL_DL_VAP_AC_MATRIX_24_27             (TX_SELECTOR_BASE_ADDRESS + 0xBC)
#define	REG_TX_SELECTOR_TX_SEL_DL_VAP_AC_MATRIX_28_31             (TX_SELECTOR_BASE_ADDRESS + 0xC0)
#define	REG_TX_SELECTOR_TX_SEL_UL_VAP_AC_MATRIX_0_7               (TX_SELECTOR_BASE_ADDRESS + 0xC4)
#define	REG_TX_SELECTOR_TX_SEL_UL_VAP_AC_MATRIX_8_15              (TX_SELECTOR_BASE_ADDRESS + 0xC8)
#define	REG_TX_SELECTOR_TX_SEL_UL_VAP_AC_MATRIX_16_23             (TX_SELECTOR_BASE_ADDRESS + 0xCC)
#define	REG_TX_SELECTOR_TX_SEL_UL_VAP_AC_MATRIX_24_31             (TX_SELECTOR_BASE_ADDRESS + 0xD0)
#define	REG_TX_SELECTOR_TX_SEL_STA_ASSOCIATE                      (TX_SELECTOR_BASE_ADDRESS + 0xD4)
#define	REG_TX_SELECTOR_TX_SEL_VAP_AC_BYPASS                      (TX_SELECTOR_BASE_ADDRESS + 0xD8)
#define	REG_TX_SELECTOR_TX_SEL_VAP_PS_STATE_EN                    (TX_SELECTOR_BASE_ADDRESS + 0xDC)
#define	REG_TX_SELECTOR_TX_SEL_VAP_PS_STATE                       (TX_SELECTOR_BASE_ADDRESS + 0xE0)
#define	REG_TX_SELECTOR_TX_SEL_VAP_PS_NEW_IND_PS                  (TX_SELECTOR_BASE_ADDRESS + 0xE4)
#define	REG_TX_SELECTOR_TX_SEL_VAP_PS_NEW_IND_ACTIVE              (TX_SELECTOR_BASE_ADDRESS + 0xE8)
#define	REG_TX_SELECTOR_TX_SEL_STA_ACTIVE_SEL_COUNT               (TX_SELECTOR_BASE_ADDRESS + 0xEC)
#define	REG_TX_SELECTOR_TX_SEL_STA_PS_SEL_COUNT                   (TX_SELECTOR_BASE_ADDRESS + 0xF0)
#define	REG_TX_SELECTOR_TX_SEL_VAP_ACTIVE_SEL_COUNT               (TX_SELECTOR_BASE_ADDRESS + 0xF4)
#define	REG_TX_SELECTOR_TX_SEL_VAP_PS_SEL_COUNT                   (TX_SELECTOR_BASE_ADDRESS + 0xF8)
#define	REG_TX_SELECTOR_TX_SEL_GLOBAL_SEL_COUNT                   (TX_SELECTOR_BASE_ADDRESS + 0xFC)
#define	REG_TX_SELECTOR_TX_SEL_GPLP_SEL_COUNT                     (TX_SELECTOR_BASE_ADDRESS + 0x100)
#define	REG_TX_SELECTOR_TX_SEL_GPHP_SEL_COUNT                     (TX_SELECTOR_BASE_ADDRESS + 0x104)
#define	REG_TX_SELECTOR_TX_SEL_BEACON_SEL_COUNT                   (TX_SELECTOR_BASE_ADDRESS + 0x108)
#define	REG_TX_SELECTOR_TX_SEL_EMPTY_SEL_COUNT                    (TX_SELECTOR_BASE_ADDRESS + 0x10C)
#define	REG_TX_SELECTOR_TX_SEL_COUNT_CLR                          (TX_SELECTOR_BASE_ADDRESS + 0x110)
#define	REG_TX_SELECTOR_TX_SEL_LOGGER_CFG                         (TX_SELECTOR_BASE_ADDRESS + 0x114)
#define	REG_TX_SELECTOR_TX_SEL_LOGGER_BUSY                        (TX_SELECTOR_BASE_ADDRESS + 0x118)
#define	REG_TX_SELECTOR_TX_SEL_TIM_BITMAP0                        (TX_SELECTOR_BASE_ADDRESS + 0x150)
#define	REG_TX_SELECTOR_TX_SEL_TIM_BITMAP1                        (TX_SELECTOR_BASE_ADDRESS + 0x154)
#define	REG_TX_SELECTOR_TX_SEL_TIM_BITMAP2                        (TX_SELECTOR_BASE_ADDRESS + 0x158)
#define	REG_TX_SELECTOR_TX_SEL_TIM_BITMAP3                        (TX_SELECTOR_BASE_ADDRESS + 0x15C)
#define	REG_TX_SELECTOR_TX_SEL_TIM_BITMAP4                        (TX_SELECTOR_BASE_ADDRESS + 0x160)
#define	REG_TX_SELECTOR_TX_SEL_TIM_HEADER                         (TX_SELECTOR_BASE_ADDRESS + 0x164)
#define	REG_TX_SELECTOR_TX_SEL_TIM_ELEMENT_ID                     (TX_SELECTOR_BASE_ADDRESS + 0x168)
#define	REG_TX_SELECTOR_TX_SEL_TIM_HOB_POINTERS                   (TX_SELECTOR_BASE_ADDRESS + 0x16C)
#define	REG_TX_SELECTOR_TX_SEL_TIM_ERR                            (TX_SELECTOR_BASE_ADDRESS + 0x170)
#define	REG_TX_SELECTOR_TX_SEL_TIM_ERR_CLR                        (TX_SELECTOR_BASE_ADDRESS + 0x174)
#define	REG_TX_SELECTOR_TX_SEL_TIM_MULTI_BSSID_CFG                (TX_SELECTOR_BASE_ADDRESS + 0x178)
#define	REG_TX_SELECTOR_TX_SEL_TIM_MULTI_BSSID_GROUP              (TX_SELECTOR_BASE_ADDRESS + 0x17C)
#define	REG_TX_SELECTOR_TX_SEL_TIM_MULTI_BSSID_MPS                (TX_SELECTOR_BASE_ADDRESS + 0x180)
#define	REG_TX_SELECTOR_TX_SEL_TIM_MULTI_BSSID_MPS_EN             (TX_SELECTOR_BASE_ADDRESS + 0x184)
#define	REG_TX_SELECTOR_TX_SEL_TIM_DTIM_READ_VAP_IDX              (TX_SELECTOR_BASE_ADDRESS + 0x188)
#define	REG_TX_SELECTOR_TX_SEL_TIM_DTIM_VALUE                     (TX_SELECTOR_BASE_ADDRESS + 0x18C)
#define	REG_TX_SELECTOR_TX_SEL_LOCKER_EN                          (TX_SELECTOR_BASE_ADDRESS + 0x1A0)
#define	REG_TX_SELECTOR_TX_SEL_SW_MASTER0_LOCKER                  (TX_SELECTOR_BASE_ADDRESS + 0x1A4)
#define	REG_TX_SELECTOR_TX_SEL_SW_MASTER1_LOCKER                  (TX_SELECTOR_BASE_ADDRESS + 0x1A8)
#define	REG_TX_SELECTOR_TX_SEL_SW_MASTER2_LOCKER                  (TX_SELECTOR_BASE_ADDRESS + 0x1AC)
#define	REG_TX_SELECTOR_TX_SEL_SW_MASTER3_LOCKER                  (TX_SELECTOR_BASE_ADDRESS + 0x1B0)
#define	REG_TX_SELECTOR_TX_SEL_SW_MASTER0_LOCKER_STATUS           (TX_SELECTOR_BASE_ADDRESS + 0x1B4)
#define	REG_TX_SELECTOR_TX_SEL_SW_MASTER1_LOCKER_STATUS           (TX_SELECTOR_BASE_ADDRESS + 0x1B8)
#define	REG_TX_SELECTOR_TX_SEL_SW_MASTER2_LOCKER_STATUS           (TX_SELECTOR_BASE_ADDRESS + 0x1BC)
#define	REG_TX_SELECTOR_TX_SEL_SW_MASTER3_LOCKER_STATUS           (TX_SELECTOR_BASE_ADDRESS + 0x1C0)
#define	REG_TX_SELECTOR_TX_SEL_LOCKER_INT_CLR                     (TX_SELECTOR_BASE_ADDRESS + 0x1C4)
#define	REG_TX_SELECTOR_TX_SEL_LOCKER_PUSH_WHILE_BUSY             (TX_SELECTOR_BASE_ADDRESS + 0x1C8)
#define	REG_TX_SELECTOR_TX_SEL_LOCKER_PUSH_WHILE_BUSY_CLR         (TX_SELECTOR_BASE_ADDRESS + 0x1CC)
#define	REG_TX_SELECTOR_TX_SELECTOR_SPARE                         (TX_SELECTOR_BASE_ADDRESS + 0x1D0)
#define	REG_TX_SELECTOR_TX_SEL_MU_SELECT_CFG                      (TX_SELECTOR_BASE_ADDRESS + 0x200)
#define	REG_TX_SELECTOR_TX_SEL_MU_BITMAP_UPDATE                   (TX_SELECTOR_BASE_ADDRESS + 0x204)
#define	REG_TX_SELECTOR_TX_SEL_MU_BITMAP_UPDATE_ALL_STA_WDATA     (TX_SELECTOR_BASE_ADDRESS + 0x208)
#define	REG_TX_SELECTOR_TX_SEL_MU_PRIMARY_THRES                   (TX_SELECTOR_BASE_ADDRESS + 0x20C)
#define	REG_TX_SELECTOR_TX_SEL_MU_SECONDARY_THRES                 (TX_SELECTOR_BASE_ADDRESS + 0x210)
#define	REG_TX_SELECTOR_TX_SEL_MU_ERR                             (TX_SELECTOR_BASE_ADDRESS + 0x214)
#define	REG_TX_SELECTOR_TX_SEL_MU_ERR_CLR                         (TX_SELECTOR_BASE_ADDRESS + 0x218)
#define	REG_TX_SELECTOR_TX_SEL_MU_LOCKER_EN                       (TX_SELECTOR_BASE_ADDRESS + 0x220)
#define	REG_TX_SELECTOR_TX_SEL_MU_SW_MASTER0_LOCKER               (TX_SELECTOR_BASE_ADDRESS + 0x224)
#define	REG_TX_SELECTOR_TX_SEL_MU_SW_MASTER1_LOCKER               (TX_SELECTOR_BASE_ADDRESS + 0x228)
#define	REG_TX_SELECTOR_TX_SEL_MU_SW_MASTER2_LOCKER               (TX_SELECTOR_BASE_ADDRESS + 0x22C)
#define	REG_TX_SELECTOR_TX_SEL_MU_SW_MASTER3_LOCKER               (TX_SELECTOR_BASE_ADDRESS + 0x230)
#define	REG_TX_SELECTOR_TX_SEL_MU_SW_MASTER0_LOCKER_STATUS        (TX_SELECTOR_BASE_ADDRESS + 0x234)
#define	REG_TX_SELECTOR_TX_SEL_MU_SW_MASTER1_LOCKER_STATUS        (TX_SELECTOR_BASE_ADDRESS + 0x238)
#define	REG_TX_SELECTOR_TX_SEL_MU_SW_MASTER2_LOCKER_STATUS        (TX_SELECTOR_BASE_ADDRESS + 0x23C)
#define	REG_TX_SELECTOR_TX_SEL_MU_SW_MASTER3_LOCKER_STATUS        (TX_SELECTOR_BASE_ADDRESS + 0x240)
#define	REG_TX_SELECTOR_TX_SEL_MU_LOCKER_INT_CLR                  (TX_SELECTOR_BASE_ADDRESS + 0x244)
#define	REG_TX_SELECTOR_TX_SEL_MU_LOCKER_PUSH_WHILE_BUSY          (TX_SELECTOR_BASE_ADDRESS + 0x248)
#define	REG_TX_SELECTOR_TX_SEL_MU_LOCKER_PUSH_WHILE_BUSY_CLR      (TX_SELECTOR_BASE_ADDRESS + 0x24C)
#define	REG_TX_SELECTOR_TX_SEL_MU_UP0_RESULT                      (TX_SELECTOR_BASE_ADDRESS + 0x250)
#define	REG_TX_SELECTOR_TX_SEL_MU_UP1_RESULT                      (TX_SELECTOR_BASE_ADDRESS + 0x254)
#define	REG_TX_SELECTOR_TX_SEL_MU_UP2_RESULT                      (TX_SELECTOR_BASE_ADDRESS + 0x258)
#define	REG_TX_SELECTOR_TX_SEL_MU_UP3_RESULT                      (TX_SELECTOR_BASE_ADDRESS + 0x25C)
#define	REG_TX_SELECTOR_TX_SEL_MU_UP0_BITMAP                      (TX_SELECTOR_BASE_ADDRESS + 0x260)
#define	REG_TX_SELECTOR_TX_SEL_HE_MU_UP0_BITMAP                   (TX_SELECTOR_BASE_ADDRESS + 0x264)
#define	REG_TX_SELECTOR_TX_SEL_HE_PRE_AGG_UP0_BITMAP              (TX_SELECTOR_BASE_ADDRESS + 0x268)
#define	REG_TX_SELECTOR_TX_SEL_MU_UP1_BITMAP                      (TX_SELECTOR_BASE_ADDRESS + 0x26C)
#define	REG_TX_SELECTOR_TX_SEL_HE_MU_UP1_BITMAP                   (TX_SELECTOR_BASE_ADDRESS + 0x270)
#define	REG_TX_SELECTOR_TX_SEL_HE_PRE_AGG_UP1_BITMAP              (TX_SELECTOR_BASE_ADDRESS + 0x274)
#define	REG_TX_SELECTOR_TX_SEL_MU_UP2_BITMAP                      (TX_SELECTOR_BASE_ADDRESS + 0x278)
#define	REG_TX_SELECTOR_TX_SEL_HE_MU_UP2_BITMAP                   (TX_SELECTOR_BASE_ADDRESS + 0x27C)
#define	REG_TX_SELECTOR_TX_SEL_HE_PRE_AGG_UP2_BITMAP              (TX_SELECTOR_BASE_ADDRESS + 0x280)
#define	REG_TX_SELECTOR_TX_SEL_MU_UP3_BITMAP                      (TX_SELECTOR_BASE_ADDRESS + 0x284)
#define	REG_TX_SELECTOR_TX_SEL_HE_MU_UP3_BITMAP                   (TX_SELECTOR_BASE_ADDRESS + 0x288)
#define	REG_TX_SELECTOR_TX_SEL_HE_PRE_AGG_UP3_BITMAP              (TX_SELECTOR_BASE_ADDRESS + 0x28C)
#define	REG_TX_SELECTOR_TX_SEL_MU_GRP_BITMAP                      (TX_SELECTOR_BASE_ADDRESS + 0x290)
#define	REG_TX_SELECTOR_TX_SEL_HE_GRP_BITMAP                      (TX_SELECTOR_BASE_ADDRESS + 0x294)
#define	REG_TX_SELECTOR_TX_SEL_MU_SM_STATUS                       (TX_SELECTOR_BASE_ADDRESS + 0x298)
#define	REG_TX_SELECTOR_TX_SEL_MU_SU_REPORT_STATUS                (TX_SELECTOR_BASE_ADDRESS + 0x29C)
#define	REG_TX_SELECTOR_TX_SEL_SELECT_MODE_CFG                    (TX_SELECTOR_BASE_ADDRESS + 0x2A0)
#define	REG_TX_SELECTOR_TX_SEL_PLAN_LOCKER_EN                     (TX_SELECTOR_BASE_ADDRESS + 0x2A4)
#define	REG_TX_SELECTOR_TX_SEL_PLAN_SW_MASTER0_LOCKER             (TX_SELECTOR_BASE_ADDRESS + 0x300)
#define	REG_TX_SELECTOR_TX_SEL_PLAN_SW_MASTER1_LOCKER             (TX_SELECTOR_BASE_ADDRESS + 0x304)
#define	REG_TX_SELECTOR_TX_SEL_PLAN_SW_MASTER2_LOCKER             (TX_SELECTOR_BASE_ADDRESS + 0x308)
#define	REG_TX_SELECTOR_TX_SEL_PLAN_SW_MASTER3_LOCKER             (TX_SELECTOR_BASE_ADDRESS + 0x30C)
#define	REG_TX_SELECTOR_TX_SEL_PLAN_SW_MASTER0_LOCKER_STATUS      (TX_SELECTOR_BASE_ADDRESS + 0x310)
#define	REG_TX_SELECTOR_TX_SEL_PLAN_SW_MASTER1_LOCKER_STATUS      (TX_SELECTOR_BASE_ADDRESS + 0x314)
#define	REG_TX_SELECTOR_TX_SEL_PLAN_SW_MASTER2_LOCKER_STATUS      (TX_SELECTOR_BASE_ADDRESS + 0x318)
#define	REG_TX_SELECTOR_TX_SEL_PLAN_SW_MASTER3_LOCKER_STATUS      (TX_SELECTOR_BASE_ADDRESS + 0x31C)
#define	REG_TX_SELECTOR_TX_SEL_PLAN_LOCKER_INT_CLR                (TX_SELECTOR_BASE_ADDRESS + 0x320)
#define	REG_TX_SELECTOR_TX_SEL_PLAN_LOCKER_PUSH_WHILE_BUSY        (TX_SELECTOR_BASE_ADDRESS + 0x324)
#define	REG_TX_SELECTOR_TX_SEL_PLAN_LOCKER_PUSH_WHILE_BUSY_CLR    (TX_SELECTOR_BASE_ADDRESS + 0x328)
#define	REG_TX_SELECTOR_TX_SEL_TWT_SW_HALT                        (TX_SELECTOR_BASE_ADDRESS + 0x350)
#define	REG_TX_SELECTOR_TX_SEL_TWT_CFG                            (TX_SELECTOR_BASE_ADDRESS + 0x354)
#define	REG_TX_SELECTOR_TX_SEL_TWT_SP_GROUP_UPDATE                (TX_SELECTOR_BASE_ADDRESS + 0x358)
#define	REG_TX_SELECTOR_TX_SEL_TWT_SP_GROUP_START_TSF             (TX_SELECTOR_BASE_ADDRESS + 0x35C)
#define	REG_TX_SELECTOR_TX_SEL_TWT_SP_GROUP_LOW_PHASE             (TX_SELECTOR_BASE_ADDRESS + 0x360)
#define	REG_TX_SELECTOR_TX_SEL_TWT_SP_STA_UPDATE                  (TX_SELECTOR_BASE_ADDRESS + 0x364)
#define	REG_TX_SELECTOR_TX_SEL_TWT_SP_GROUP_VALID                 (TX_SELECTOR_BASE_ADDRESS + 0x368)
#define	REG_TX_SELECTOR_TX_SEL_TWT_SP_GROUP_DEACTIVATE            (TX_SELECTOR_BASE_ADDRESS + 0x36C)
#define	REG_TX_SELECTOR_TX_SEL_PRE_AGG_SEL_REQ_BITS0              (TX_SELECTOR_BASE_ADDRESS + 0x380)
#define	REG_TX_SELECTOR_TX_SEL_PRE_AGG_SEL_REQ_BITS1              (TX_SELECTOR_BASE_ADDRESS + 0x384)
#define	REG_TX_SELECTOR_TX_SEL_PRE_AGG_SEL_RESULT_BITS0           (TX_SELECTOR_BASE_ADDRESS + 0x388)
#define	REG_TX_SELECTOR_TX_SEL_PRE_AGG_SEL_RESULT_BITS1           (TX_SELECTOR_BASE_ADDRESS + 0x38C)
#define	REG_TX_SELECTOR_TX_SEL_PRE_AGG_SEL_RESULT_BITS2           (TX_SELECTOR_BASE_ADDRESS + 0x390)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_TX_SELECTOR_TX_SEL_INSTR 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 instr : 2; //CPU instruction to TX Selector, reset value: 0x0, access type: RW
		uint32 reserved0 : 30;
	} bitFields;
} RegTxSelectorTxSelInstr_u;

/*REG_TX_SELECTOR_TX_SEL_BITMAP_UPDATE_MASTER0 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 master0StaSel : 8; //Station number or VAP number according to bitmap, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
		uint32 master0BitmapSel : 3; //Bitmap Queue selection: , 0x0: STA-TID , 0x2: VAP-MC , 0x4: GLOBAL , 0x5: GPLP , 0x6: GPHP , 0x7: BEACON, reset value: 0x0, access type: RW
		uint32 reserved1 : 1;
		uint32 master0BitTypeSel : 5; //Information bit type selector: , 0x0:    Data in Q - 1 bit Per TID bit type , 0x1:    Lock - 1 bit Per TID bit type , 0x2:    Request - 1 bit Per TID bit type , 0x3:    Power Save type - 1 bit Per TID bit type , 0x4:    Active/Power Save mode - 1 bit Per TID bit type , 0x5:    DL HE MU En - 1 bit Per TID bit type , 0x6:    UL data in Q - 1 bit Per TID bit type , 0x8:    Disable - 1 bit Per STA bit type , 0x9:    BF sequence request enable/disable - 1 bit Per STA bit type , 0xA:    BF sequence type - 1 bit Per STA bit type , 0xB:   TWT Announced type - 1 bit Per STA bit type , 0xC:   TWT SP - 1 bit Per STA bit type , 0xD:   TWT Availability - 1 bit Per STA bit type , 0xE:   DL Plan index - 5 bits Per STA bit type , 0xF:   UL Plan index - 5 bits Per STA bit type , 0x10: Plan lock - 1 bit Per STA bit type , 0x11: Insert A-control - 1 bit Per STA bit type , 0x12: MU MIMO Group lock - 1 bit Per MU MIMO Group bit type , 0x13: MU MIMO TX Mode BF req (MSB bit only) - 1 bit Per Group bit type , 0x14: OMI_VALID - 1 bit Per STA bit type , 0x15: OMI_SUPPORTED - 1 bit Per STA bit type , 0x16: 10 OMI bits: ,                     TOMI UL HE MU Dis - 1 bit Per STA bit type ,                     TOMI DL HE MU Dis - 1 bit Per STA bit type ,                     ROMI Max Nss - 3 bits Per STA bit type ,                     OMI BW - 2 bits Per STA bit type ,                     TOMI Max Nss - 3 bits Per STA bit type , 0x17: UL HE MU En - 1 bit Per STA bit type , , reset value: 0x0, access type: RW
		uint32 reserved2 : 7;
	} bitFields;
} RegTxSelectorTxSelBitmapUpdateMaster0_u;

/*REG_TX_SELECTOR_TX_SEL_BITMAP_UPDATE_DATA_MASTER0 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 master0TidWriteEn : 9; //Write enable vector - Valid for TID resolution updates only., reset value: 0x0, access type: RW
		uint32 master0TidWriteValue : 9; //Data per TID vector - Valid for TID resolution updates only., reset value: 0x0, access type: RW
		uint32 master0StaWriteData : 10; //STA data field - Valid for STA resolution updates only., reset value: 0x0, access type: RW
		uint32 reserved0 : 4;
	} bitFields;
} RegTxSelectorTxSelBitmapUpdateDataMaster0_u;

/*REG_TX_SELECTOR_TX_SEL_BITMAP_UPDATE_MASTER1 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 master1StaSel : 8; //Station number or VAP number according to bitmap, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
		uint32 master1BitmapSel : 3; //Bitmap Queue selection: , 0x0: STA-TID , 0x2: VAP-MC , 0x4: GLOBAL , 0x5: GPLP , 0x6: GPHP , 0x7: BEACON, reset value: 0x0, access type: RW
		uint32 reserved1 : 1;
		uint32 master1BitTypeSel : 5; //Information bit type selector: , 0x0:    Data in Q - 1 bit Per TID bit type , 0x1:    Lock - 1 bit Per TID bit type , 0x2:    Request - 1 bit Per TID bit type , 0x3:    Power Save type - 1 bit Per TID bit type , 0x4:    Active/Power Save mode - 1 bit Per TID bit type , 0x5:    DL HE MU En - 1 bit Per TID bit type , 0x6:    UL data in Q - 1 bit Per TID bit type , 0x8:    Disable - 1 bit Per STA bit type , 0x9:    BF sequence request enable/disable - 1 bit Per STA bit type , 0xA:    BF sequence type - 1 bit Per STA bit type , 0xB:   TWT Announced type - 1 bit Per STA bit type , 0xC:   TWT SP - 1 bit Per STA bit type , 0xD:   TWT Availability - 1 bit Per STA bit type , 0xE:   DL Plan index - 5 bits Per STA bit type , 0xF:   UL Plan index - 5 bits Per STA bit type , 0x10: Plan lock - 1 bit Per STA bit type , 0x11: Insert A-control - 1 bit Per STA bit type , 0x12: MU MIMO Group lock - 1 bit Per MU MIMO Group bit type , 0x13: MU MIMO TX Mode BF req (MSB bit only) - 1 bit Per Group bit type , 0x14: OMI_VALID - 1 bit Per STA bit type , 0x15: OMI_SUPPORTED - 1 bit Per STA bit type , 0x16: 10 OMI bits: ,                     TOMI UL HE MU Dis - 1 bit Per STA bit type ,                     TOMI DL HE MU Dis - 1 bit Per STA bit type ,                     ROMI Max Nss - 3 bits Per STA bit type ,                     OMI BW - 2 bits Per STA bit type ,                     TOMI Max Nss - 3 bits Per STA bit type , 0x17: UL HE MU En - 1 bit Per STA bit type , , reset value: 0x0, access type: RW
		uint32 reserved2 : 7;
	} bitFields;
} RegTxSelectorTxSelBitmapUpdateMaster1_u;

/*REG_TX_SELECTOR_TX_SEL_BITMAP_UPDATE_DATA_MASTER1 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 master1TidWriteEn : 9; //Write enable vector - Valid for TID resolution updates only., reset value: 0x0, access type: RW
		uint32 master1TidWriteValue : 9; //Data per TID vector - Valid for TID resolution updates only., reset value: 0x0, access type: RW
		uint32 master1StaWriteData : 10; //STA data field - Valid for STA resolution updates only., reset value: 0x0, access type: RW
		uint32 reserved0 : 4;
	} bitFields;
} RegTxSelectorTxSelBitmapUpdateDataMaster1_u;

/*REG_TX_SELECTOR_TX_SEL_BITMAP_UPDATE_MASTER2 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 master2StaSel : 8; //Station number or VAP number according to bitmap, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
		uint32 master2BitmapSel : 3; //Bitmap Queue selection: , 0x0: STA-TID , 0x2: VAP-MC , 0x4: GLOBAL , 0x5: GPLP , 0x6: GPHP , 0x7: BEACON, reset value: 0x0, access type: RW
		uint32 reserved1 : 1;
		uint32 master2BitTypeSel : 5; //Information bit type selector: , 0x0:    Data in Q - 1 bit Per TID bit type , 0x1:    Lock - 1 bit Per TID bit type , 0x2:    Request - 1 bit Per TID bit type , 0x3:    Power Save type - 1 bit Per TID bit type , 0x4:    Active/Power Save mode - 1 bit Per TID bit type , 0x5:    DL HE MU En - 1 bit Per TID bit type , 0x6:    UL data in Q - 1 bit Per TID bit type , 0x8:    Disable - 1 bit Per STA bit type , 0x9:    BF sequence request enable/disable - 1 bit Per STA bit type , 0xA:    BF sequence type - 1 bit Per STA bit type , 0xB:   TWT Announced type - 1 bit Per STA bit type , 0xC:   TWT SP - 1 bit Per STA bit type , 0xD:   TWT Availability - 1 bit Per STA bit type , 0xE:   DL Plan index - 5 bits Per STA bit type , 0xF:   UL Plan index - 5 bits Per STA bit type , 0x10: Plan lock - 1 bit Per STA bit type , 0x11: Insert A-control - 1 bit Per STA bit type , 0x12: MU MIMO Group lock - 1 bit Per MU MIMO Group bit type , 0x13: MU MIMO TX Mode BF req (MSB bit only) - 1 bit Per Group bit type , 0x14: OMI_VALID - 1 bit Per STA bit type , 0x15: OMI_SUPPORTED - 1 bit Per STA bit type , 0x16: 10 OMI bits: ,                     TOMI UL HE MU Dis - 1 bit Per STA bit type ,                     TOMI DL HE MU Dis - 1 bit Per STA bit type ,                     ROMI Max Nss - 3 bits Per STA bit type ,                     OMI BW - 2 bits Per STA bit type ,                     TOMI Max Nss - 3 bits Per STA bit type , 0x17: UL HE MU En - 1 bit Per STA bit type , , reset value: 0x0, access type: RW
		uint32 reserved2 : 7;
	} bitFields;
} RegTxSelectorTxSelBitmapUpdateMaster2_u;

/*REG_TX_SELECTOR_TX_SEL_BITMAP_UPDATE_DATA_MASTER2 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 master2TidWriteEn : 9; //Write enable vector - Valid for TID resolution updates only., reset value: 0x0, access type: RW
		uint32 master2TidWriteValue : 9; //Data per TID vector - Valid for TID resolution updates only., reset value: 0x0, access type: RW
		uint32 master2StaWriteData : 10; //STA data field - Valid for STA resolution updates only., reset value: 0x0, access type: RW
		uint32 reserved0 : 4;
	} bitFields;
} RegTxSelectorTxSelBitmapUpdateDataMaster2_u;

/*REG_TX_SELECTOR_TX_SEL_BITMAP_UPDATE_MASTER3 0x1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 master3StaSel : 8; //Station number or VAP number according to bitmap, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
		uint32 master3BitmapSel : 3; //Bitmap Queue selection: , 0x0: STA-TID , 0x2: VAP-MC , 0x4: GLOBAL , 0x5: GPLP , 0x6: GPHP , 0x7: BEACON, reset value: 0x0, access type: RW
		uint32 reserved1 : 1;
		uint32 master3BitTypeSel : 5; //Information bit type selector: , 0x0:    Data in Q - 1 bit Per TID bit type , 0x1:    Lock - 1 bit Per TID bit type , 0x2:    Request - 1 bit Per TID bit type , 0x3:    Power Save type - 1 bit Per TID bit type , 0x4:    Active/Power Save mode - 1 bit Per TID bit type , 0x5:    DL HE MU En - 1 bit Per TID bit type , 0x6:    UL data in Q - 1 bit Per TID bit type , 0x8:    Disable - 1 bit Per STA bit type , 0x9:    BF sequence request enable/disable - 1 bit Per STA bit type , 0xA:    BF sequence type - 1 bit Per STA bit type , 0xB:   TWT Announced type - 1 bit Per STA bit type , 0xC:   TWT SP - 1 bit Per STA bit type , 0xD:   TWT Availability - 1 bit Per STA bit type , 0xE:   DL Plan index - 5 bits Per STA bit type , 0xF:   UL Plan index - 5 bits Per STA bit type , 0x10: Plan lock - 1 bit Per STA bit type , 0x11: Insert A-control - 1 bit Per STA bit type , 0x12: MU MIMO Group lock - 1 bit Per MU MIMO Group bit type , 0x13: MU MIMO TX Mode BF req (MSB bit only) - 1 bit Per Group bit type , 0x14: OMI_VALID - 1 bit Per STA bit type , 0x15: OMI_SUPPORTED - 1 bit Per STA bit type , 0x16: 10 OMI bits: ,                     TOMI UL HE MU Dis - 1 bit Per STA bit type ,                     TOMI DL HE MU Dis - 1 bit Per STA bit type ,                     ROMI Max Nss - 3 bits Per STA bit type ,                     OMI BW - 2 bits Per STA bit type ,                     TOMI Max Nss - 3 bits Per STA bit type , 0x17: UL HE MU En - 1 bit Per STA bit type , , reset value: 0x0, access type: RW
		uint32 reserved2 : 7;
	} bitFields;
} RegTxSelectorTxSelBitmapUpdateMaster3_u;

/*REG_TX_SELECTOR_TX_SEL_BITMAP_UPDATE_DATA_MASTER3 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 master3TidWriteEn : 9; //Write enable vector - Valid for TID resolution updates only., reset value: 0x0, access type: RW
		uint32 master3TidWriteValue : 9; //Data per TID vector - Valid for TID resolution updates only., reset value: 0x0, access type: RW
		uint32 master3StaWriteData : 10; //STA data field - Valid for STA resolution updates only., reset value: 0x0, access type: RW
		uint32 reserved0 : 4;
	} bitFields;
} RegTxSelectorTxSelBitmapUpdateDataMaster3_u;

/*REG_TX_SELECTOR_TX_SEL_STA_PS_WRR_LOW0 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 psLowVap0Prio : 3; //VAP0 priority, reset value: 0x0, access type: RW
		uint32 psLowVap1Prio : 3; //VAP1 priority, reset value: 0x0, access type: RW
		uint32 psLowVap2Prio : 3; //VAP2 priority, reset value: 0x0, access type: RW
		uint32 psLowVap3Prio : 3; //VAP3 priority, reset value: 0x0, access type: RW
		uint32 psLowVap4Prio : 3; //VAP4 priority, reset value: 0x0, access type: RW
		uint32 psLowVap5Prio : 3; //VAP5 priority, reset value: 0x0, access type: RW
		uint32 psLowVap6Prio : 3; //VAP6 priority, reset value: 0x0, access type: RW
		uint32 psLowVap7Prio : 3; //VAP7 priority, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegTxSelectorTxSelStaPsWrrLow0_u;

/*REG_TX_SELECTOR_TX_SEL_STA_PS_WRR_LOW1 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 psLowVap8Prio : 3; //VAP8 priority, reset value: 0x0, access type: RW
		uint32 psLowVap9Prio : 3; //VAP9 priority, reset value: 0x0, access type: RW
		uint32 psLowVap10Prio : 3; //VAP10 priority, reset value: 0x0, access type: RW
		uint32 psLowVap11Prio : 3; //VAP11 priority, reset value: 0x0, access type: RW
		uint32 psLowVap12Prio : 3; //VAP12 priority, reset value: 0x0, access type: RW
		uint32 psLowVap13Prio : 3; //VAP13 priority, reset value: 0x0, access type: RW
		uint32 psLowVap14Prio : 3; //VAP14 priority, reset value: 0x0, access type: RW
		uint32 psLowVap15Prio : 3; //VAP15 priority, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegTxSelectorTxSelStaPsWrrLow1_u;

/*REG_TX_SELECTOR_TX_SEL_STA_PS_WRR_HIGH0 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 psHighVap16Prio : 3; //VAP16 priority, reset value: 0x0, access type: RW
		uint32 psHighVap17Prio : 3; //VAP17 priority, reset value: 0x0, access type: RW
		uint32 psHighVap18Prio : 3; //VAP18 priority, reset value: 0x0, access type: RW
		uint32 psHighVap19Prio : 3; //VAP19 priority, reset value: 0x0, access type: RW
		uint32 psHighVap20Prio : 3; //VAP20 priority, reset value: 0x0, access type: RW
		uint32 psHighVap21Prio : 3; //VAP21 priority, reset value: 0x0, access type: RW
		uint32 psHighVap22Prio : 3; //VAP22 priority, reset value: 0x0, access type: RW
		uint32 psHighVap23Prio : 3; //VAP23 priority, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegTxSelectorTxSelStaPsWrrHigh0_u;

/*REG_TX_SELECTOR_TX_SEL_STA_PS_WRR_HIGH1 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 psHighVap24Prio : 3; //VAP24 priority, reset value: 0x0, access type: RW
		uint32 psHighVap25Prio : 3; //VAP25 priority, reset value: 0x0, access type: RW
		uint32 psHighVap26Prio : 3; //VAP26 priority, reset value: 0x0, access type: RW
		uint32 psHighVap27Prio : 3; //VAP27 priority, reset value: 0x0, access type: RW
		uint32 psHighVap28Prio : 3; //VAP28 priority, reset value: 0x0, access type: RW
		uint32 psHighVap29Prio : 3; //VAP29 priority, reset value: 0x0, access type: RW
		uint32 psHighVap30Prio : 3; //VAP30 priority, reset value: 0x0, access type: RW
		uint32 psHighVap31Prio : 3; //VAP31 priority, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegTxSelectorTxSelStaPsWrrHigh1_u;

/*REG_TX_SELECTOR_TX_SEL_STA_ACTIVE_WRR_LOW0 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 activeLowVap0Prio : 3; //VAP0 priority, reset value: 0x0, access type: RW
		uint32 activeLowVap1Prio : 3; //VAP1 priority, reset value: 0x0, access type: RW
		uint32 activeLowVap2Prio : 3; //VAP2 priority, reset value: 0x0, access type: RW
		uint32 activeLowVap3Prio : 3; //VAP3 priority, reset value: 0x0, access type: RW
		uint32 activeLowVap4Prio : 3; //VAP4 priority, reset value: 0x0, access type: RW
		uint32 activeLowVap5Prio : 3; //VAP5 priority, reset value: 0x0, access type: RW
		uint32 activeLowVap6Prio : 3; //VAP6 priority, reset value: 0x0, access type: RW
		uint32 activeLowVap7Prio : 3; //VAP7 priority, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegTxSelectorTxSelStaActiveWrrLow0_u;

/*REG_TX_SELECTOR_TX_SEL_STA_ACTIVE_WRR_LOW1 0x38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 activeLowVap8Prio : 3; //VAP8 priority, reset value: 0x0, access type: RW
		uint32 activeLowVap9Prio : 3; //VAP9 priority, reset value: 0x0, access type: RW
		uint32 activeLowVap10Prio : 3; //VAP10 priority, reset value: 0x0, access type: RW
		uint32 activeLowVap11Prio : 3; //VAP11 priority, reset value: 0x0, access type: RW
		uint32 activeLowVap12Prio : 3; //VAP12 priority, reset value: 0x0, access type: RW
		uint32 activeLowVap13Prio : 3; //VAP13 priority, reset value: 0x0, access type: RW
		uint32 activeLowVap14Prio : 3; //VAP14 priority, reset value: 0x0, access type: RW
		uint32 activeLowVap15Prio : 3; //VAP15 priority, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegTxSelectorTxSelStaActiveWrrLow1_u;

/*REG_TX_SELECTOR_TX_SEL_STA_ACTIVE_WRR_HIGH0 0x3C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 activeHighVap16Prio : 3; //VAP16 priority, reset value: 0x0, access type: RW
		uint32 activeHighVap17Prio : 3; //VAP17 priority, reset value: 0x0, access type: RW
		uint32 activeHighVap18Prio : 3; //VAP18 priority, reset value: 0x0, access type: RW
		uint32 activeHighVap19Prio : 3; //VAP19 priority, reset value: 0x0, access type: RW
		uint32 activeHighVap20Prio : 3; //VAP20 priority, reset value: 0x0, access type: RW
		uint32 activeHighVap21Prio : 3; //VAP21 priority, reset value: 0x0, access type: RW
		uint32 activeHighVap22Prio : 3; //VAP22 priority, reset value: 0x0, access type: RW
		uint32 activeHighVap23Prio : 3; //VAP23 priority, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegTxSelectorTxSelStaActiveWrrHigh0_u;

/*REG_TX_SELECTOR_TX_SEL_STA_ACTIVE_WRR_HIGH1 0x40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 activeHighVap24Prio : 3; //VAP24 priority, reset value: 0x0, access type: RW
		uint32 activeHighVap25Prio : 3; //VAP25 priority, reset value: 0x0, access type: RW
		uint32 activeHighVap26Prio : 3; //VAP26 priority, reset value: 0x0, access type: RW
		uint32 activeHighVap27Prio : 3; //VAP27 priority, reset value: 0x0, access type: RW
		uint32 activeHighVap28Prio : 3; //VAP28 priority, reset value: 0x0, access type: RW
		uint32 activeHighVap29Prio : 3; //VAP29 priority, reset value: 0x0, access type: RW
		uint32 activeHighVap30Prio : 3; //VAP30 priority, reset value: 0x0, access type: RW
		uint32 activeHighVap31Prio : 3; //VAP31 priority, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegTxSelectorTxSelStaActiveWrrHigh1_u;

/*REG_TX_SELECTOR_TX_SEL_STRICT_PRIO 0x44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 strictPrioGroupSelEn : 6; //Enable selection groups, reset value: 0x3F, access type: RW
		uint32 strictPrioWrrPrio : 2; //Priority of active selection, reset value: 0x3, access type: RW
		uint32 strictPrioVapMpsPrio : 2; //Priority of VAP TID PS selection, reset value: 0x2, access type: RW
		uint32 strictPrioGlobalPrio : 2; //Priority of Global selection, reset value: 0x1, access type: RW
		uint32 strictPrioSelectorLockEn : 1; //Selector lock enable, reset value: 0x1, access type: RW
		uint32 reserved0 : 19;
	} bitFields;
} RegTxSelectorTxSelStrictPrio_u;

/*REG_TX_SELECTOR_TX_SEL_SELECTION 0x48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 selectionTidNum : 4; //TID number, reset value: 0x0, access type: RO
		uint32 selectionStaNum : 8; //STA number, reset value: 0x0, access type: RO
		uint32 selectionVapNum : 5; //VAP number, reset value: 0x0, access type: RO
		uint32 reserved0 : 1;
		uint32 selectionAcNum : 2; //AC number, reset value: 0x0, access type: RO
		uint32 selectionTxqId : 3; //Selected Queue index, reset value: 0x0, access type: RO
		uint32 reserved1 : 1;
		uint32 selectionSrcNum : 3; //Source from which the selected result came from, reset value: 0x0, access type: RO
		uint32 reserved2 : 1;
		uint32 selectionBf : 2; //Beam Forming bits, reset value: 0x0, access type: RO
		uint32 selectionEmpty : 1; //No selection exist bit, reset value: 0x0, access type: RO
		uint32 selectionValid : 1; //Selection valid bit, reset value: 0x0, access type: RO
	} bitFields;
} RegTxSelectorTxSelSelection_u;

/*REG_TX_SELECTOR_TX_SEL_SELECTED_BITMAP 0x4C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 selectedBitmapReq0 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 selectedBitmapPsType0 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 selectedBitmapPsMode0 : 1; //PS mode, reset value: 0x0, access type: RO
		uint32 selectedBitmapDataInQ0 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 selectedBitmapReq1 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 selectedBitmapPsType1 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 selectedBitmapPsMode1 : 1; //PS mode, reset value: 0x0, access type: RO
		uint32 selectedBitmapDataInQ1 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 selectedBitmapReq2 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 selectedBitmapPsType2 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 selectedBitmapPsMode2 : 1; //PS mode, reset value: 0x0, access type: RO
		uint32 selectedBitmapDataInQ2 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 selectedBitmapReq3 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 selectedBitmapPsType3 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 selectedBitmapPsMode3 : 1; //PS mode, reset value: 0x0, access type: RO
		uint32 selectedBitmapDataInQ3 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 selectedBitmapReq4 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 selectedBitmapPsType4 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 selectedBitmapPsMode4 : 1; //PS mode, reset value: 0x0, access type: RO
		uint32 selectedBitmapDataInQ4 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 selectedBitmapReq5 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 selectedBitmapPsType5 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 selectedBitmapPsMode5 : 1; //PS mode, reset value: 0x0, access type: RO
		uint32 selectedBitmapDataInQ5 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 selectedBitmapReq6 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 selectedBitmapPsType6 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 selectedBitmapPsMode6 : 1; //PS mode, reset value: 0x0, access type: RO
		uint32 selectedBitmapDataInQ6 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 selectedBitmapReq7 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 selectedBitmapPsType7 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 selectedBitmapPsMode7 : 1; //PS mode, reset value: 0x0, access type: RO
		uint32 selectedBitmapDataInQ7 : 1; //Data in Q, reset value: 0x0, access type: RO
	} bitFields;
} RegTxSelectorTxSelSelectedBitmap_u;

/*REG_TX_SELECTOR_TX_SEL_SELECTED_BITMAP_MGMT 0x50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 selectedBitmapReq8 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 selectedBitmapPsType8 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 selectedBitmapPsMode8 : 1; //PS mode, reset value: 0x0, access type: RO
		uint32 selectedBitmapDataInQ8 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 reserved0 : 28;
	} bitFields;
} RegTxSelectorTxSelSelectedBitmapMgmt_u;

/*REG_TX_SELECTOR_TX_SEL_SRC0_SELECTION 0x54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 src0SelectionTidNum : 4; //TID number, reset value: 0x0, access type: RO
		uint32 src0SelectionStaNum : 8; //STA number, reset value: 0x0, access type: RO
		uint32 src0SelectionVapNum : 5; //VAP number, reset value: 0x0, access type: RO
		uint32 src0SelectionTxqId : 3; //Selected Queue index, reset value: 0x0, access type: RO
		uint32 src0SelectionAcNum : 2; //AC number, reset value: 0x0, access type: RO
		uint32 reserved0 : 2;
		uint32 src0SelectionBf : 2; //Beam Forming bits, reset value: 0x0, access type: RO
		uint32 src0SelectionSrcNum : 3; //Source from which the selected result came from, reset value: 0x0, access type: RO
		uint32 reserved1 : 1;
		uint32 src0SelectionEmpty : 1; //No selection exist bit, reset value: 0x0, access type: RO
		uint32 src0SelectionValid : 1; //Selection valid bit, reset value: 0x0, access type: RO
	} bitFields;
} RegTxSelectorTxSelSrc0Selection_u;

/*REG_TX_SELECTOR_TX_SEL_SELECTED_SRC0_BITMAP 0x58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 3;
		uint32 selectedSrc0BitmapDataInQ0 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 reserved1 : 3;
		uint32 selectedSrc0BitmapDataInQ1 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 reserved2 : 3;
		uint32 selectedSrc0BitmapDataInQ2 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 reserved3 : 3;
		uint32 selectedSrc0BitmapDataInQ3 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 reserved4 : 3;
		uint32 selectedSrc0BitmapDataInQ4 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 reserved5 : 3;
		uint32 selectedSrc0BitmapDataInQ5 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 reserved6 : 3;
		uint32 selectedSrc0BitmapDataInQ6 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 reserved7 : 3;
		uint32 selectedSrc0BitmapDataInQ7 : 1; //Data in Q, reset value: 0x0, access type: RO
	} bitFields;
} RegTxSelectorTxSelSelectedSrc0Bitmap_u;

/*REG_TX_SELECTOR_TX_SEL_SELECTED_SRC0_BITMAP_MGMT 0x5C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 3;
		uint32 selectedSrc0BitmapDataInQ8 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 reserved1 : 28;
	} bitFields;
} RegTxSelectorTxSelSelectedSrc0BitmapMgmt_u;

/*REG_TX_SELECTOR_TX_SEL_SRC1_SELECTION 0x60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 src1SelectionTidNum : 4; //TID number, reset value: 0x0, access type: RO
		uint32 src1SelectionStaNum : 8; //STA number, reset value: 0x0, access type: RO
		uint32 src1SelectionVapNum : 5; //VAP number, reset value: 0x0, access type: RO
		uint32 src1SelectionTxqId : 3; //Selected Queue index, reset value: 0x0, access type: RO
		uint32 src1SelectionAcNum : 2; //AC number, reset value: 0x0, access type: RO
		uint32 reserved0 : 2;
		uint32 src1SelectionBf : 2; //Beam Forming bits, reset value: 0x0, access type: RO
		uint32 src1SelectionSrcNum : 3; //Source from which the selected result came from, reset value: 0x1, access type: RO
		uint32 reserved1 : 1;
		uint32 src1SelectionEmpty : 1; //No selection exist bit, reset value: 0x0, access type: RO
		uint32 src1SelectionValid : 1; //Selection valid bit, reset value: 0x0, access type: RO
	} bitFields;
} RegTxSelectorTxSelSrc1Selection_u;

/*REG_TX_SELECTOR_TX_SEL_SELECTED_SRC1_BITMAP 0x64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 selectedSrc1BitmapReq0 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 selectedSrc1BitmapPsType0 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 selectedSrc1BitmapPsMode0 : 1; //PS mode, reset value: 0x1, access type: RO
		uint32 selectedSrc1BitmapDataInQ0 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 selectedSrc1BitmapReq1 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 selectedSrc1BitmapPsType1 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 selectedSrc1BitmapPsMode1 : 1; //PS mode, reset value: 0x1, access type: RO
		uint32 selectedSrc1BitmapDataInQ1 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 selectedSrc1BitmapReq2 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 selectedSrc1BitmapPsType2 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 selectedSrc1BitmapPsMode2 : 1; //PS mode, reset value: 0x1, access type: RO
		uint32 selectedSrc1BitmapDataInQ2 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 selectedSrc1BitmapReq3 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 selectedSrc1BitmapPsType3 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 selectedSrc1BitmapPsMode3 : 1; //PS mode, reset value: 0x1, access type: RO
		uint32 selectedSrc1BitmapDataInQ3 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 selectedSrc1BitmapReq4 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 selectedSrc1BitmapPsType4 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 selectedSrc1BitmapPsMode4 : 1; //PS mode, reset value: 0x1, access type: RO
		uint32 selectedSrc1BitmapDataInQ4 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 selectedSrc1BitmapReq5 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 selectedSrc1BitmapPsType5 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 selectedSrc1BitmapPsMode5 : 1; //PS mode, reset value: 0x1, access type: RO
		uint32 selectedSrc1BitmapDataInQ5 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 selectedSrc1BitmapReq6 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 selectedSrc1BitmapPsType6 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 selectedSrc1BitmapPsMode6 : 1; //PS mode, reset value: 0x1, access type: RO
		uint32 selectedSrc1BitmapDataInQ6 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 selectedSrc1BitmapReq7 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 selectedSrc1BitmapPsType7 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 selectedSrc1BitmapPsMode7 : 1; //PS mode, reset value: 0x1, access type: RO
		uint32 selectedSrc1BitmapDataInQ7 : 1; //Data in Q, reset value: 0x0, access type: RO
	} bitFields;
} RegTxSelectorTxSelSelectedSrc1Bitmap_u;

/*REG_TX_SELECTOR_TX_SEL_SELECTED_SRC1_BITMAP_MGMT 0x68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 selectedSrc1BitmapReq8 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 selectedSrc1BitmapPsType8 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 selectedSrc1BitmapPsMode8 : 1; //PS mode, reset value: 0x1, access type: RO
		uint32 selectedSrc1BitmapDataInQ8 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 reserved0 : 28;
	} bitFields;
} RegTxSelectorTxSelSelectedSrc1BitmapMgmt_u;

/*REG_TX_SELECTOR_TX_SEL_SRC2_SELECTION 0x6C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 src2SelectionTidNum : 4; //TID number, reset value: 0x0, access type: RO
		uint32 src2SelectionStaNum : 8; //STA number, reset value: 0x0, access type: RO
		uint32 src2SelectionVapNum : 5; //VAP number, reset value: 0x0, access type: RO
		uint32 src2SelectionTxqId : 3; //Selected Queue index, reset value: 0x2, access type: RO
		uint32 src2SelectionAcNum : 2; //AC number, reset value: 0x0, access type: RO
		uint32 reserved0 : 2;
		uint32 src2SelectionBf : 2; //Beam Forming bits, reset value: 0x0, access type: RO
		uint32 src2SelectionSrcNum : 3; //Source from which the selected result came from, reset value: 0x3, access type: RO
		uint32 reserved1 : 1;
		uint32 src2SelectionEmpty : 1; //No selection exist bit, reset value: 0x0, access type: RO
		uint32 src2SelectionValid : 1; //Selection valid bit, reset value: 0x0, access type: RO
	} bitFields;
} RegTxSelectorTxSelSrc2Selection_u;

/*REG_TX_SELECTOR_TX_SEL_SELECTED_SRC2_BITMAP 0x70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 selectedSrc2BitmapReq0 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 selectedSrc2BitmapPsType0 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 selectedSrc2BitmapPsMode0 : 1; //PS mode, reset value: 0x1, access type: RO
		uint32 selectedSrc2BitmapDataInQ0 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 selectedSrc2BitmapReq1 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 selectedSrc2BitmapPsType1 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 selectedSrc2BitmapPsMode1 : 1; //PS mode, reset value: 0x1, access type: RO
		uint32 selectedSrc2BitmapDataInQ1 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 selectedSrc2BitmapReq2 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 selectedSrc2BitmapPsType2 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 selectedSrc2BitmapPsMode2 : 1; //PS mode, reset value: 0x1, access type: RO
		uint32 selectedSrc2BitmapDataInQ2 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 selectedSrc2BitmapReq3 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 selectedSrc2BitmapPsType3 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 selectedSrc2BitmapPsMode3 : 1; //PS mode, reset value: 0x1, access type: RO
		uint32 selectedSrc2BitmapDataInQ3 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 selectedSrc2BitmapReq4 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 selectedSrc2BitmapPsType4 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 selectedSrc2BitmapPsMode4 : 1; //PS mode, reset value: 0x1, access type: RO
		uint32 selectedSrc2BitmapDataInQ4 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 selectedSrc2BitmapReq5 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 selectedSrc2BitmapPsType5 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 selectedSrc2BitmapPsMode5 : 1; //PS mode, reset value: 0x1, access type: RO
		uint32 selectedSrc2BitmapDataInQ5 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 selectedSrc2BitmapReq6 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 selectedSrc2BitmapPsType6 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 selectedSrc2BitmapPsMode6 : 1; //PS mode, reset value: 0x1, access type: RO
		uint32 selectedSrc2BitmapDataInQ6 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 selectedSrc2BitmapReq7 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 selectedSrc2BitmapPsType7 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 selectedSrc2BitmapPsMode7 : 1; //PS mode, reset value: 0x1, access type: RO
		uint32 selectedSrc2BitmapDataInQ7 : 1; //Data in Q, reset value: 0x0, access type: RO
	} bitFields;
} RegTxSelectorTxSelSelectedSrc2Bitmap_u;

/*REG_TX_SELECTOR_TX_SEL_SELECTED_SRC2_BITMAP_MGMT 0x74 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 selectedSrc2BitmapReq8 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 selectedSrc2BitmapPsType8 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 selectedSrc2BitmapPsMode8 : 1; //PS mode, reset value: 0x1, access type: RO
		uint32 selectedSrc2BitmapDataInQ8 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 reserved0 : 28;
	} bitFields;
} RegTxSelectorTxSelSelectedSrc2BitmapMgmt_u;

/*REG_TX_SELECTOR_TX_SEL_SRC3_SELECTION 0x78 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 src3SelectionTidNum : 4; //TID number, reset value: 0x0, access type: RO
		uint32 src3SelectionStaNum : 8; //STA number, reset value: 0x0, access type: RO
		uint32 src3SelectionVapNum : 5; //VAP number, reset value: 0x0, access type: RO
		uint32 src3SelectionTxqId : 3; //Selected Queue index, reset value: 0x4, access type: RO
		uint32 src3SelectionAcNum : 2; //AC number, reset value: 0x0, access type: RO
		uint32 reserved0 : 2;
		uint32 src3SelectionBf : 2; //Beam Forming bits, reset value: 0x0, access type: RO
		uint32 src3SelectionSrcNum : 3; //Source from which the selected result came from, reset value: 0x4, access type: RO
		uint32 reserved1 : 1;
		uint32 src3SelectionEmpty : 1; //No selection exist bit, reset value: 0x0, access type: RO
		uint32 src3SelectionValid : 1; //Selection valid bit, reset value: 0x0, access type: RO
	} bitFields;
} RegTxSelectorTxSelSrc3Selection_u;

/*REG_TX_SELECTOR_TX_SEL_SELECTED_SRC3_BITMAP 0x7C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 selectedSrc3BitmapReq0 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 selectedSrc3BitmapPsType0 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 selectedSrc3BitmapPsMode0 : 1; //PS mode, reset value: 0x0, access type: RO
		uint32 selectedSrc3BitmapDataInQ0 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 selectedSrc3BitmapReq1 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 selectedSrc3BitmapPsType1 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 selectedSrc3BitmapPsMode1 : 1; //PS mode, reset value: 0x0, access type: RO
		uint32 selectedSrc3BitmapDataInQ1 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 selectedSrc3BitmapReq2 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 selectedSrc3BitmapPsType2 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 selectedSrc3BitmapPsMode2 : 1; //PS mode, reset value: 0x0, access type: RO
		uint32 selectedSrc3BitmapDataInQ2 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 selectedSrc3BitmapReq3 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 selectedSrc3BitmapPsType3 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 selectedSrc3BitmapPsMode3 : 1; //PS mode, reset value: 0x0, access type: RO
		uint32 selectedSrc3BitmapDataInQ3 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegTxSelectorTxSelSelectedSrc3Bitmap_u;

/*REG_TX_SELECTOR_TX_SEL_SRC4_SELECTION 0x80 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 src4SelectionTidNum : 4; //TID number, reset value: 0x0, access type: RO
		uint32 src4SelectionStaNum : 8; //STA number, reset value: 0x0, access type: RO
		uint32 src4SelectionVapNum : 5; //VAP number, reset value: 0x0, access type: RO
		uint32 src4SelectionTxqId : 3; //Selected Queue index, reset value: 0x5, access type: RO
		uint32 src4SelectionAcNum : 2; //AC number, reset value: 0x0, access type: RO
		uint32 reserved0 : 2;
		uint32 src4SelectionBf : 2; //Beam Forming bits, reset value: 0x0, access type: RO
		uint32 src4SelectionSrcNum : 3; //Source from which the selected result came from, reset value: 0x5, access type: RO
		uint32 reserved1 : 1;
		uint32 src4SelectionEmpty : 1; //No selection exist bit, reset value: 0x0, access type: RO
		uint32 src4SelectionValid : 1; //Selection valid bit, reset value: 0x0, access type: RO
	} bitFields;
} RegTxSelectorTxSelSrc4Selection_u;

/*REG_TX_SELECTOR_TX_SEL_SELECTED_SRC4_BITMAP 0x84 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 3;
		uint32 selectedSrc4BitmapDataInQ : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 reserved1 : 28;
	} bitFields;
} RegTxSelectorTxSelSelectedSrc4Bitmap_u;

/*REG_TX_SELECTOR_TX_SEL_SRC5_SELECTION 0x88 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 src5SelectionTidNum : 4; //TID number, reset value: 0x0, access type: RO
		uint32 src5SelectionStaNum : 8; //STA number, reset value: 0x0, access type: RO
		uint32 src5SelectionVapNum : 5; //VAP number, reset value: 0x0, access type: RO
		uint32 src5SelectionTxqId : 3; //Selected Queue index, reset value: 0x6, access type: RO
		uint32 src5SelectionAcNum : 2; //AC number, reset value: 0x0, access type: RO
		uint32 reserved0 : 2;
		uint32 src5SelectionBf : 2; //Beam Forming bits, reset value: 0x0, access type: RO
		uint32 src5SelectionSrcNum : 3; //Source from which the selected result came from, reset value: 0x6, access type: RO
		uint32 reserved1 : 1;
		uint32 src5SelectionEmpty : 1; //No selection exist bit, reset value: 0x0, access type: RO
		uint32 src5SelectionValid : 1; //Selection valid bit, reset value: 0x0, access type: RO
	} bitFields;
} RegTxSelectorTxSelSrc5Selection_u;

/*REG_TX_SELECTOR_TX_SEL_SELECTED_SRC5_BITMAP 0x8C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 3;
		uint32 selectedSrc5BitmapDataInQ : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 reserved1 : 28;
	} bitFields;
} RegTxSelectorTxSelSelectedSrc5Bitmap_u;

/*REG_TX_SELECTOR_TX_SEL_SRC6_SELECTION 0x90 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 src6SelectionTidNum : 4; //TID number, reset value: 0x0, access type: RO
		uint32 src6SelectionStaNum : 8; //STA number, reset value: 0x0, access type: RO
		uint32 src6SelectionVapNum : 5; //VAP number, reset value: 0x0, access type: RO
		uint32 src6SelectionTxqId : 3; //Selected Queue index, reset value: 0x7, access type: RO
		uint32 src6SelectionAcNum : 2; //AC number, reset value: 0x0, access type: RO
		uint32 reserved0 : 2;
		uint32 src6SelectionBf : 2; //Beam Forming bits, reset value: 0x0, access type: RO
		uint32 src6SelectionSrcNum : 3; //Source from which the selected result came from, reset value: 0x7, access type: RO
		uint32 reserved1 : 1;
		uint32 src6SelectionEmpty : 1; //No selection exist bit, reset value: 0x0, access type: RO
		uint32 src6SelectionValid : 1; //Selection valid bit, reset value: 0x0, access type: RO
	} bitFields;
} RegTxSelectorTxSelSrc6Selection_u;

/*REG_TX_SELECTOR_TX_SEL_SELECTED_SRC6_BITMAP 0x94 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 selectedSrc6BitmapReq0 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 reserved0 : 2;
		uint32 selectedSrc6BitmapDataInQ0 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 reserved1 : 28;
	} bitFields;
} RegTxSelectorTxSelSelectedSrc6Bitmap_u;

/*REG_TX_SELECTOR_TX_SEL_STATUS 0x98 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 statusSwFifoPopWhileEmpty : 1; //SW FIFO underflow , reset value: 0x0, access type: RO
		uint32 statusSwFifoPushWhileFull : 1; //SW FIFO overflow , reset value: 0x0, access type: RO
		uint32 statusSwFifoEmpty : 1; //SW FIFO empty , reset value: 0x1, access type: RO
		uint32 statusSwFifoFull : 1; //SW FIFO full , reset value: 0x0, access type: RO
		uint32 statusSwFifoHfull : 1; //SW FIFO half full , reset value: 0x0, access type: RO
		uint32 statusDataInQFifoEmpty : 1; //Data in Q FIFO empty , reset value: 0x1, access type: RO
		uint32 statusDataInQFifoFull : 1; //Data in Q FIFO full , reset value: 0x0, access type: RO
		uint32 dataInQFifoWasFull : 1; //no description, reset value: 0x0, access type: RO
		uint32 statusInstruction : 2; //Last instruction recieved, reset value: 0x0, access type: RO
		uint32 statusInstrSrc : 1; //Source of instruction, reset value: 0x0, access type: RO
		uint32 reserved0 : 21;
	} bitFields;
} RegTxSelectorTxSelStatus_u;

/*REG_TX_SELECTOR_TX_SEL_ERR 0x9C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 errSwInstrWhileBusy : 1; //New SW instruction to while selection is being done , reset value: 0x0, access type: RO
		uint32 errHwInstrWhileBusy : 1; //New HW instruction to while selection is being done , reset value: 0x0, access type: RO
		uint32 errDataLost : 1; //Data insertion overflow/underflow, reset value: 0x0, access type: RO
		uint32 reserved0 : 17;
		uint32 errDataInQLost : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved1 : 3;
		uint32 wrongBitmapAccessErr : 1; //no description, reset value: 0x0, access type: RO
		uint32 wrongBitmapAccessErrBitmap : 3; //no description, reset value: 0x0, access type: RO
		uint32 reserved2 : 4;
	} bitFields;
} RegTxSelectorTxSelErr_u;

/*REG_TX_SELECTOR_TX_SEL_ERR_CLR 0xA0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 errClrSwInstrWhileBusyClr : 1; //SW instruction error clear , reset value: 0x0, access type: WO
		uint32 errClrHwInstrWhileBusyClr : 1; //HW instruction error clear , reset value: 0x0, access type: WO
		uint32 errClrDataLostClr : 1; //Data insertion overflow/underflow error clear, reset value: 0x0, access type: WO
		uint32 reserved0 : 17;
		uint32 errClrDataInQLostClr : 1; //no description, reset value: 0x0, access type: WO
		uint32 reserved1 : 3;
		uint32 wrongBitmapAccessErrClr : 1; //no description, reset value: 0x0, access type: WO
		uint32 reserved2 : 7;
	} bitFields;
} RegTxSelectorTxSelErrClr_u;

/*REG_TX_SELECTOR_TX_SEL_DL_VAP_AC_MATRIX_0_3 0xA4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dlVap0Ac : 7; //VAP0-AC vector , reset value: 0x0, access type: RO
		uint32 reserved0 : 1;
		uint32 dlVap1Ac : 7; //VAP1-AC vector , reset value: 0x0, access type: RO
		uint32 reserved1 : 1;
		uint32 dlVap2Ac : 7; //VAP2-AC vector , reset value: 0x0, access type: RO
		uint32 reserved2 : 1;
		uint32 dlVap3Ac : 7; //VAP3-AC vector , reset value: 0x0, access type: RO
		uint32 reserved3 : 1;
	} bitFields;
} RegTxSelectorTxSelDlVapAcMatrix03_u;

/*REG_TX_SELECTOR_TX_SEL_DL_VAP_AC_MATRIX_4_7 0xA8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dlVap4Ac : 7; //VAP4-AC vector , reset value: 0x0, access type: RO
		uint32 reserved0 : 1;
		uint32 dlVap5Ac : 7; //VAP5-AC vector , reset value: 0x0, access type: RO
		uint32 reserved1 : 1;
		uint32 dlVap6Ac : 7; //VAP6-AC vector , reset value: 0x0, access type: RO
		uint32 reserved2 : 1;
		uint32 dlVap7Ac : 7; //VAP7-AC vector , reset value: 0x0, access type: RO
		uint32 reserved3 : 1;
	} bitFields;
} RegTxSelectorTxSelDlVapAcMatrix47_u;

/*REG_TX_SELECTOR_TX_SEL_DL_VAP_AC_MATRIX_8_11 0xAC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dlVap8Ac : 7; //VAP8-AC vector , reset value: 0x0, access type: RO
		uint32 reserved0 : 1;
		uint32 dlVap9Ac : 7; //VAP9-AC vector , reset value: 0x0, access type: RO
		uint32 reserved1 : 1;
		uint32 dlVap10Ac : 7; //VAP10-AC vector , reset value: 0x0, access type: RO
		uint32 reserved2 : 1;
		uint32 dlVap11Ac : 7; //VAP11-AC vector , reset value: 0x0, access type: RO
		uint32 reserved3 : 1;
	} bitFields;
} RegTxSelectorTxSelDlVapAcMatrix811_u;

/*REG_TX_SELECTOR_TX_SEL_DL_VAP_AC_MATRIX_12_15 0xB0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dlVap12Ac : 7; //VAP12-AC vector , reset value: 0x0, access type: RO
		uint32 reserved0 : 1;
		uint32 dlVap13Ac : 7; //VAP13-AC vector , reset value: 0x0, access type: RO
		uint32 reserved1 : 1;
		uint32 dlVap14Ac : 7; //VAP14-AC vector , reset value: 0x0, access type: RO
		uint32 reserved2 : 1;
		uint32 dlVap15Ac : 7; //VAP15-AC vector , reset value: 0x0, access type: RO
		uint32 reserved3 : 1;
	} bitFields;
} RegTxSelectorTxSelDlVapAcMatrix1215_u;

/*REG_TX_SELECTOR_TX_SEL_DL_VAP_AC_MATRIX_16_19 0xB4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dlVap16Ac : 7; //VAP16-AC vector , reset value: 0x0, access type: RO
		uint32 reserved0 : 1;
		uint32 dlVap17Ac : 7; //VAP17-AC vector , reset value: 0x0, access type: RO
		uint32 reserved1 : 1;
		uint32 dlVap18Ac : 7; //VAP18-AC vector , reset value: 0x0, access type: RO
		uint32 reserved2 : 1;
		uint32 dlVap19Ac : 7; //VAP19-AC vector , reset value: 0x0, access type: RO
		uint32 reserved3 : 1;
	} bitFields;
} RegTxSelectorTxSelDlVapAcMatrix1619_u;

/*REG_TX_SELECTOR_TX_SEL_DL_VAP_AC_MATRIX_20_23 0xB8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dlVap20Ac : 7; //VAP20-AC vector , reset value: 0x0, access type: RO
		uint32 reserved0 : 1;
		uint32 dlVap21Ac : 7; //VAP21-AC vector , reset value: 0x0, access type: RO
		uint32 reserved1 : 1;
		uint32 dlVap22Ac : 7; //VAP22-AC vector , reset value: 0x0, access type: RO
		uint32 reserved2 : 1;
		uint32 dlVap23Ac : 7; //VAP23-AC vector , reset value: 0x0, access type: RO
		uint32 reserved3 : 1;
	} bitFields;
} RegTxSelectorTxSelDlVapAcMatrix2023_u;

/*REG_TX_SELECTOR_TX_SEL_DL_VAP_AC_MATRIX_24_27 0xBC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dlVap24Ac : 7; //VAP24-AC vector , reset value: 0x0, access type: RO
		uint32 reserved0 : 1;
		uint32 dlVap25Ac : 7; //VAP25-AC vector , reset value: 0x0, access type: RO
		uint32 reserved1 : 1;
		uint32 dlVap26Ac : 7; //VAP26-AC vector , reset value: 0x0, access type: RO
		uint32 reserved2 : 1;
		uint32 dlVap27Ac : 7; //VAP27-AC vector , reset value: 0x0, access type: RO
		uint32 reserved3 : 1;
	} bitFields;
} RegTxSelectorTxSelDlVapAcMatrix2427_u;

/*REG_TX_SELECTOR_TX_SEL_DL_VAP_AC_MATRIX_28_31 0xC0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dlVap28Ac : 7; //VAP28-AC vector , reset value: 0x0, access type: RO
		uint32 reserved0 : 1;
		uint32 dlVap29Ac : 7; //VAP29-AC vector , reset value: 0x0, access type: RO
		uint32 reserved1 : 1;
		uint32 dlVap30Ac : 7; //VAP30-AC vector , reset value: 0x0, access type: RO
		uint32 reserved2 : 1;
		uint32 dlVap31Ac : 7; //VAP31-AC vector , reset value: 0x0, access type: RO
		uint32 reserved3 : 1;
	} bitFields;
} RegTxSelectorTxSelDlVapAcMatrix2831_u;

/*REG_TX_SELECTOR_TX_SEL_UL_VAP_AC_MATRIX_0_7 0xC4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ulVap0Ac : 4; //VAP0-AC vector , reset value: 0x0, access type: RO
		uint32 ulVap1Ac : 4; //VAP1-AC vector , reset value: 0x0, access type: RO
		uint32 ulVap2Ac : 4; //VAP2-AC vector , reset value: 0x0, access type: RO
		uint32 ulVap3Ac : 4; //VAP3-AC vector , reset value: 0x0, access type: RO
		uint32 ulVap4Ac : 4; //VAP4-AC vector , reset value: 0x0, access type: RO
		uint32 ulVap5Ac : 4; //VAP5-AC vector , reset value: 0x0, access type: RO
		uint32 ulVap6Ac : 4; //VAP6-AC vector , reset value: 0x0, access type: RO
		uint32 ulVap7Ac : 4; //VAP7-AC vector , reset value: 0x0, access type: RO
	} bitFields;
} RegTxSelectorTxSelUlVapAcMatrix07_u;

/*REG_TX_SELECTOR_TX_SEL_UL_VAP_AC_MATRIX_8_15 0xC8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ulVap8Ac : 4; //VAP8-AC vector , reset value: 0x0, access type: RO
		uint32 ulVap9Ac : 4; //VAP9-AC vector , reset value: 0x0, access type: RO
		uint32 ulVap10Ac : 4; //VAP10-AC vector , reset value: 0x0, access type: RO
		uint32 ulVap11Ac : 4; //VAP11-AC vector , reset value: 0x0, access type: RO
		uint32 ulVap12Ac : 4; //VAP12-AC vector , reset value: 0x0, access type: RO
		uint32 ulVap13Ac : 4; //VAP13-AC vector , reset value: 0x0, access type: RO
		uint32 ulVap14Ac : 4; //VAP14-AC vector , reset value: 0x0, access type: RO
		uint32 ulVap15Ac : 4; //VAP15-AC vector , reset value: 0x0, access type: RO
	} bitFields;
} RegTxSelectorTxSelUlVapAcMatrix815_u;

/*REG_TX_SELECTOR_TX_SEL_UL_VAP_AC_MATRIX_16_23 0xCC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ulVap16Ac : 4; //VAP16-AC vector , reset value: 0x0, access type: RO
		uint32 ulVap17Ac : 4; //VAP17-AC vector , reset value: 0x0, access type: RO
		uint32 ulVap18Ac : 4; //VAP18-AC vector , reset value: 0x0, access type: RO
		uint32 ulVap19Ac : 4; //VAP19-AC vector , reset value: 0x0, access type: RO
		uint32 ulVap20Ac : 4; //VAP20-AC vector , reset value: 0x0, access type: RO
		uint32 ulVap21Ac : 4; //VAP21-AC vector , reset value: 0x0, access type: RO
		uint32 ulVap22Ac : 4; //VAP22-AC vector , reset value: 0x0, access type: RO
		uint32 ulVap23Ac : 4; //VAP23-AC vector , reset value: 0x0, access type: RO
	} bitFields;
} RegTxSelectorTxSelUlVapAcMatrix1623_u;

/*REG_TX_SELECTOR_TX_SEL_UL_VAP_AC_MATRIX_24_31 0xD0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ulVap24Ac : 4; //VAP24-AC vector , reset value: 0x0, access type: RO
		uint32 ulVap25Ac : 4; //VAP25-AC vector , reset value: 0x0, access type: RO
		uint32 ulVap26Ac : 4; //VAP26-AC vector , reset value: 0x0, access type: RO
		uint32 ulVap27Ac : 4; //VAP27-AC vector , reset value: 0x0, access type: RO
		uint32 ulVap28Ac : 4; //VAP28-AC vector , reset value: 0x0, access type: RO
		uint32 ulVap29Ac : 4; //VAP29-AC vector , reset value: 0x0, access type: RO
		uint32 ulVap30Ac : 4; //VAP30-AC vector , reset value: 0x0, access type: RO
		uint32 ulVap31Ac : 4; //VAP31-AC vector , reset value: 0x0, access type: RO
	} bitFields;
} RegTxSelectorTxSelUlVapAcMatrix2431_u;

/*REG_TX_SELECTOR_TX_SEL_STA_ASSOCIATE 0xD4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 staAssociateStation : 8; //Station to be associated with the selected VAP, reset value: 0x0, access type: RW
		uint32 staAssociateVap : 5; //Selected VAP, reset value: 0x0, access type: RW
		uint32 reserved0 : 3;
		uint32 staAssociateVapStaBitValue : 1; //Selected VAP, reset value: 0x0, access type: RW
		uint32 reserved1 : 7;
		uint32 vapZero : 1; //When asserted, disconnect all stations of all VAPs, reset value: 0x0, access type: RW
		uint32 reserved2 : 7;
	} bitFields;
} RegTxSelectorTxSelStaAssociate_u;

/*REG_TX_SELECTOR_TX_SEL_VAP_AC_BYPASS 0xD8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vapAcBypassAcSel : 3; //TX Handler AC sel bypass, reset value: 0x0, access type: RW
		uint32 reserved0 : 5;
		uint32 vapAcBypassVapSel : 5; //TX Handler VAP sel bypass, reset value: 0x0, access type: RW
		uint32 reserved1 : 3;
		uint32 vapAcBypassVaild : 1; //TX Handler bypass valid, reset value: 0x0, access type: RW
		uint32 reserved2 : 15;
	} bitFields;
} RegTxSelectorTxSelVapAcBypass_u;

/*REG_TX_SELECTOR_TX_SEL_VAP_PS_STATE_EN 0xDC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 16;
		uint32 vapPsStateVapInPsAutoEn : 1; //Enables VAP in PS bit automatic update by TX Selector, reset value: 0x1, access type: RW
		uint32 reserved1 : 15;
	} bitFields;
} RegTxSelectorTxSelVapPsStateEn_u;

/*REG_TX_SELECTOR_TX_SEL_VAP_PS_STATE 0xE0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vapPsStateVapPsState : 32; //Each bit indicates its VAP PS state, reset value: 0x0, access type: RO
	} bitFields;
} RegTxSelectorTxSelVapPsState_u;

/*REG_TX_SELECTOR_TX_SEL_VAP_PS_NEW_IND_PS 0xE4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vapPsNewIndPs : 32; //Each bit indicates its VAP PS state, reset value: 0x0, access type: RO
	} bitFields;
} RegTxSelectorTxSelVapPsNewIndPs_u;

/*REG_TX_SELECTOR_TX_SEL_VAP_PS_NEW_IND_ACTIVE 0xE8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vapPsNewIndActive : 32; //Each bit indicates its VAP PS state, reset value: 0x0, access type: RO
	} bitFields;
} RegTxSelectorTxSelVapPsNewIndActive_u;

/*REG_TX_SELECTOR_TX_SEL_STA_ACTIVE_SEL_COUNT 0xEC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 staActiveCount : 12; //Counts Selection of STA ACTIVE , reset value: 0x0, access type: RO
		uint32 reserved0 : 20;
	} bitFields;
} RegTxSelectorTxSelStaActiveSelCount_u;

/*REG_TX_SELECTOR_TX_SEL_STA_PS_SEL_COUNT 0xF0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 staPsCount : 12; //Counts Selection of sta ps , reset value: 0x0, access type: RO
		uint32 reserved0 : 20;
	} bitFields;
} RegTxSelectorTxSelStaPsSelCount_u;

/*REG_TX_SELECTOR_TX_SEL_VAP_ACTIVE_SEL_COUNT 0xF4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vapActiveCount : 12; //Counts Selection of vap ACTIVE , reset value: 0x0, access type: RO
		uint32 reserved0 : 20;
	} bitFields;
} RegTxSelectorTxSelVapActiveSelCount_u;

/*REG_TX_SELECTOR_TX_SEL_VAP_PS_SEL_COUNT 0xF8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vapPsCount : 12; //Counts Selection of vap ps , reset value: 0x0, access type: RO
		uint32 reserved0 : 20;
	} bitFields;
} RegTxSelectorTxSelVapPsSelCount_u;

/*REG_TX_SELECTOR_TX_SEL_GLOBAL_SEL_COUNT 0xFC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 globalCount : 12; //Counts Selection of global , reset value: 0x0, access type: RO
		uint32 reserved0 : 20;
	} bitFields;
} RegTxSelectorTxSelGlobalSelCount_u;

/*REG_TX_SELECTOR_TX_SEL_GPLP_SEL_COUNT 0x100 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gplpCount : 12; //Counts Selection of gplp , reset value: 0x0, access type: RO
		uint32 reserved0 : 20;
	} bitFields;
} RegTxSelectorTxSelGplpSelCount_u;

/*REG_TX_SELECTOR_TX_SEL_GPHP_SEL_COUNT 0x104 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gphpCount : 12; //Counts Selection of gphp , reset value: 0x0, access type: RO
		uint32 reserved0 : 20;
	} bitFields;
} RegTxSelectorTxSelGphpSelCount_u;

/*REG_TX_SELECTOR_TX_SEL_BEACON_SEL_COUNT 0x108 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 beaconCount : 12; //Counts Selection of beacon , reset value: 0x0, access type: RO
		uint32 reserved0 : 20;
	} bitFields;
} RegTxSelectorTxSelBeaconSelCount_u;

/*REG_TX_SELECTOR_TX_SEL_EMPTY_SEL_COUNT 0x10C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 emptyCount : 12; //Counts Empty Selections, reset value: 0x0, access type: RO
		uint32 reserved0 : 20;
	} bitFields;
} RegTxSelectorTxSelEmptySelCount_u;

/*REG_TX_SELECTOR_TX_SEL_COUNT_CLR 0x110 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 countersClr : 1; //Clears selection counters, reset value: 0x0, access type: WO
		uint32 reserved0 : 31;
	} bitFields;
} RegTxSelectorTxSelCountClr_u;

/*REG_TX_SELECTOR_TX_SEL_LOGGER_CFG 0x114 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSelLoggerSelectionLockerPriority : 2; //no description, reset value: 0x0, access type: RW
		uint32 txSelLoggerSelectionLockerEn : 1; //no description, reset value: 0x0, access type: RW
		uint32 txSelLoggerSelectionLockerSel : 1; //0x0: Selection logger , 0x1: Locker logger, reset value: 0x0, access type: RW
		uint32 txSelLoggerBitmapPriority : 2; //no description, reset value: 0x0, access type: RW
		uint32 txSelLoggerBitmapUpdateEn : 1; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 1;
		uint32 txSelLoggerMuLockerPriority : 2; //no description, reset value: 0x0, access type: RW
		uint32 txSelLoggerMuLockerEn : 1; //no description, reset value: 0x0, access type: RW
		uint32 reserved1 : 1;
		uint32 txSelLoggerMuBitmapPriority : 2; //no description, reset value: 0x0, access type: RW
		uint32 txSelLoggerMuBitmapUpdateEn : 1; //no description, reset value: 0x0, access type: RW
		uint32 reserved2 : 1;
		uint32 txSelLoggerMuSelectionEn : 1; //no description, reset value: 0x0, access type: RW
		uint32 reserved3 : 3;
		uint32 txSelLoggerPlanLockerPriority : 2; //no description, reset value: 0x0, access type: RW
		uint32 txSelLoggerPlanLockerEn : 1; //no description, reset value: 0x0, access type: RW
		uint32 reserved4 : 9;
	} bitFields;
} RegTxSelectorTxSelLoggerCfg_u;

/*REG_TX_SELECTOR_TX_SEL_LOGGER_BUSY 0x118 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSelLoggerSelectionLockerBusy : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 3;
		uint32 txSelLoggerMuLockerBusy : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved1 : 3;
		uint32 txSelLoggerBitmapUpdateBusy : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved2 : 3;
		uint32 txSelLoggerMuBitmapUpdateBusy : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved3 : 3;
		uint32 txSelLoggerPlanLockerBusy : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved4 : 15;
	} bitFields;
} RegTxSelectorTxSelLoggerBusy_u;

/*REG_TX_SELECTOR_TX_SEL_TIM_BITMAP0 0x150 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 virtualBitmap0 : 32; //Virtual Bitmap value. Bytes 0-3 of bitmap., reset value: 0x0, access type: RO
	} bitFields;
} RegTxSelectorTxSelTimBitmap0_u;

/*REG_TX_SELECTOR_TX_SEL_TIM_BITMAP1 0x154 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 virtualBitmap1 : 32; //Virtual Bitmap value. Bytes 4-7 of bitmap., reset value: 0x0, access type: RO
	} bitFields;
} RegTxSelectorTxSelTimBitmap1_u;

/*REG_TX_SELECTOR_TX_SEL_TIM_BITMAP2 0x158 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 virtualBitmap2 : 32; //Virtual Bitmap value. Bytes 8-11 of bitmap., reset value: 0x0, access type: RO
	} bitFields;
} RegTxSelectorTxSelTimBitmap2_u;

/*REG_TX_SELECTOR_TX_SEL_TIM_BITMAP3 0x15C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 virtualBitmap3 : 32; //Virtual Bitmap value. Bytes 12-15 of bitmap., reset value: 0x0, access type: RO
	} bitFields;
} RegTxSelectorTxSelTimBitmap3_u;

/*REG_TX_SELECTOR_TX_SEL_TIM_BITMAP4 0x160 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 virtualBitmap4 : 8; //Virtual Bitmap value. Bytes 16 of bitmap., reset value: 0x0, access type: RO
		uint32 reserved0 : 24;
	} bitFields;
} RegTxSelectorTxSelTimBitmap4_u;

/*REG_TX_SELECTOR_TX_SEL_TIM_HEADER 0x164 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dtimMcPs : 1; //DTIM with MC PS to broadcast, reset value: 0x0, access type: RO
		uint32 control : 7; //Control field value, reset value: 0x0, access type: RO
		uint32 dtimPeriod : 8; //DTIM period value, reset value: 0x0, access type: RO
		uint32 dtimCount : 8; //DTIM counter value, reset value: 0x0, access type: RO
		uint32 length : 8; //TIM IE Byte length, reset value: 0x4, access type: RO
	} bitFields;
} RegTxSelectorTxSelTimHeader_u;

/*REG_TX_SELECTOR_TX_SEL_TIM_ELEMENT_ID 0x168 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 elementId : 8; //Element ID value, reset value: 0x5, access type: RO
		uint32 reserved0 : 24;
	} bitFields;
} RegTxSelectorTxSelTimElementId_u;

/*REG_TX_SELECTOR_TX_SEL_TIM_HOB_POINTERS 0x16C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hobReadLength : 8; //Transmit length value read by the HOB, reset value: 0x1, access type: RO
		uint32 hobReadOffset : 8; //Transmit offset value read by the HOB, reset value: 0x0, access type: RO
		uint32 timEmpty : 1; //TIM IE empty indication read by HOB, reset value: 0x1, access type: RO
		uint32 reserved0 : 7;
		uint32 timReady : 1; //TIM IE ready to be transmitted indication, reset value: 0x0, access type: RO
		uint32 reserved1 : 3;
		uint32 bitmapControlLength : 3; //Length of bitmap_control+MPS bits read by the HOB, reset value: 0x0, access type: RO
		uint32 reserved2 : 1;
	} bitFields;
} RegTxSelectorTxSelTimHobPointers_u;

/*REG_TX_SELECTOR_TX_SEL_TIM_ERR 0x170 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timWrongVapSel : 32; //Selection of VAP without Beacon enabled, reset value: 0x0, access type: RO
	} bitFields;
} RegTxSelectorTxSelTimErr_u;

/*REG_TX_SELECTOR_TX_SEL_TIM_ERR_CLR 0x174 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timWrongVapSelClr : 32; //Selection of VAP without Beacon enabled clear, reset value: 0x0, access type: WO
	} bitFields;
} RegTxSelectorTxSelTimErrClr_u;

/*REG_TX_SELECTOR_TX_SEL_TIM_MULTI_BSSID_CFG 0x178 */
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
} RegTxSelectorTxSelTimMultiBssidCfg_u;

/*REG_TX_SELECTOR_TX_SEL_TIM_MULTI_BSSID_GROUP 0x17C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mpsGroup : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegTxSelectorTxSelTimMultiBssidGroup_u;

/*REG_TX_SELECTOR_TX_SEL_TIM_MULTI_BSSID_MPS 0x180 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mps : 32; //MPS result of the VAP/VAPs activated in the TIM IE, reset value: 0x0, access type: RO
	} bitFields;
} RegTxSelectorTxSelTimMultiBssidMps_u;

/*REG_TX_SELECTOR_TX_SEL_TIM_MULTI_BSSID_MPS_EN 0x184 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mpsEn : 32; //MPS valid VAP/VAPs in the TIM IE, reset value: 0x0, access type: RO
	} bitFields;
} RegTxSelectorTxSelTimMultiBssidMpsEn_u;

/*REG_TX_SELECTOR_TX_SEL_TIM_DTIM_READ_VAP_IDX 0x188 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSelTimDtimReadVapIdx : 5; //VAP index of which the DTIM values are reflected in the adjacent register., reset value: 0x0, access type: RW
		uint32 reserved0 : 27;
	} bitFields;
} RegTxSelectorTxSelTimDtimReadVapIdx_u;

/*REG_TX_SELECTOR_TX_SEL_TIM_DTIM_VALUE 0x18C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dtimCountRead : 8; //Number of TIMs minus 1 between DTIMs, reset value: 0x0, access type: RO
		uint32 dtimPeriodRead : 8; //TIM period, when equals 0 it is DTIM, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegTxSelectorTxSelTimDtimValue_u;

/*REG_TX_SELECTOR_TX_SEL_LOCKER_EN 0x1A0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lockerEn : 1; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 7;
		uint32 lockerSm : 2; //no description, reset value: 0x0, access type: RO
		uint32 reserved1 : 22;
	} bitFields;
} RegTxSelectorTxSelLockerEn_u;

/*REG_TX_SELECTOR_TX_SEL_SW_MASTER0_LOCKER 0x1A4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 master0LockerStaSel : 8; //Station number, reset value: 0x0, access type: RW
		uint32 master0LockerTidSel : 4; //TID number, reset value: 0x0, access type: RW
		uint32 reserved0 : 4;
		uint32 master0LockerBitmapSel : 3; //Bitmap Queue selection, reset value: 0x0, access type: RW
		uint32 reserved1 : 5;
		uint32 master0LockerBitTypeSel : 2; //Information bit type selector, reset value: 0x0, access type: RW
		uint32 reserved2 : 5;
		uint32 master0LockerAllTids : 1; //no description, reset value: 0x0, access type: RW
	} bitFields;
} RegTxSelectorTxSelSwMaster0Locker_u;

/*REG_TX_SELECTOR_TX_SEL_SW_MASTER1_LOCKER 0x1A8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 master1LockerStaSel : 8; //Station number, reset value: 0x0, access type: RW
		uint32 master1LockerTidSel : 4; //TID number, reset value: 0x0, access type: RW
		uint32 reserved0 : 4;
		uint32 master1LockerBitmapSel : 3; //Bitmap Queue selection, reset value: 0x0, access type: RW
		uint32 reserved1 : 5;
		uint32 master1LockerBitTypeSel : 2; //Information bit type selector, reset value: 0x0, access type: RW
		uint32 reserved2 : 5;
		uint32 master1LockerAllTids : 1; //no description, reset value: 0x0, access type: RW
	} bitFields;
} RegTxSelectorTxSelSwMaster1Locker_u;

/*REG_TX_SELECTOR_TX_SEL_SW_MASTER2_LOCKER 0x1AC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 master2LockerStaSel : 8; //Station number, reset value: 0x0, access type: RW
		uint32 master2LockerTidSel : 4; //TID number, reset value: 0x0, access type: RW
		uint32 reserved0 : 4;
		uint32 master2LockerBitmapSel : 3; //Bitmap Queue selection, reset value: 0x0, access type: RW
		uint32 reserved1 : 5;
		uint32 master2LockerBitTypeSel : 2; //Information bit type selector, reset value: 0x0, access type: RW
		uint32 reserved2 : 5;
		uint32 master2LockerAllTids : 1; //no description, reset value: 0x0, access type: RW
	} bitFields;
} RegTxSelectorTxSelSwMaster2Locker_u;

/*REG_TX_SELECTOR_TX_SEL_SW_MASTER3_LOCKER 0x1B0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 master3LockerStaSel : 8; //Station number, reset value: 0x0, access type: RW
		uint32 master3LockerTidSel : 4; //TID number, reset value: 0x0, access type: RW
		uint32 reserved0 : 4;
		uint32 master3LockerBitmapSel : 3; //Bitmap Queue selection, reset value: 0x0, access type: RW
		uint32 reserved1 : 5;
		uint32 master3LockerBitTypeSel : 2; //Information bit type selector, reset value: 0x0, access type: RW
		uint32 reserved2 : 5;
		uint32 master3LockerAllTids : 1; //no description, reset value: 0x0, access type: RW
	} bitFields;
} RegTxSelectorTxSelSwMaster3Locker_u;

/*REG_TX_SELECTOR_TX_SEL_SW_MASTER0_LOCKER_STATUS 0x1B4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swMaster0ReqStatus : 5; //SW Master request status: , 0x0 – IDLE. , 0x1 – Pending. Waiting for client to unlock. Possible only for Force request. , 0x2 – Off. Client is locked. , 0x4 – Disabled. Client is disabled. , 0x8 – Reject. Client is lock/Disable request is rejected. Relevant for Try request. , 0x10 – Error. Lock request for a disabled client., reset value: 0x0, access type: RO
		uint32 reserved0 : 3;
		uint32 master0LockerDoneInt : 1; //Lock request done indication, reset value: 0x0, access type: RO
		uint32 reserved1 : 23;
	} bitFields;
} RegTxSelectorTxSelSwMaster0LockerStatus_u;

/*REG_TX_SELECTOR_TX_SEL_SW_MASTER1_LOCKER_STATUS 0x1B8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swMaster1ReqStatus : 5; //SW Master request status: , 0x0 – IDLE. , 0x1 – Pending. Waiting for client to unlock. Possible only for Force request. , 0x2 – Off. Client is locked. , 0x4 – Disabled. Client is disabled. , 0x8 – Reject. Client is lock/Disable request is rejected. Relevant for Try request. , 0x10 – Error. Lock request for a disabled client., reset value: 0x0, access type: RO
		uint32 reserved0 : 3;
		uint32 master1LockerDoneInt : 1; //Lock request done indication, reset value: 0x0, access type: RO
		uint32 reserved1 : 23;
	} bitFields;
} RegTxSelectorTxSelSwMaster1LockerStatus_u;

/*REG_TX_SELECTOR_TX_SEL_SW_MASTER2_LOCKER_STATUS 0x1BC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swMaster2ReqStatus : 5; //SW Master request status: , 0x0 – IDLE. , 0x1 – Pending. Waiting for client to unlock. Possible only for Force request. , 0x2 – Off. Client is locked. , 0x4 – Disabled. Client is disabled. , 0x8 – Reject. Client is lock/Disable request is rejected. Relevant for Try request. , 0x10 – Error. Lock request for a disabled client., reset value: 0x0, access type: RO
		uint32 reserved0 : 3;
		uint32 master2LockerDoneInt : 1; //Lock request done indication, reset value: 0x0, access type: RO
		uint32 reserved1 : 23;
	} bitFields;
} RegTxSelectorTxSelSwMaster2LockerStatus_u;

/*REG_TX_SELECTOR_TX_SEL_SW_MASTER3_LOCKER_STATUS 0x1C0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swMaster3ReqStatus : 5; //SW Master request status: , 0x0 – IDLE. , 0x1 – Pending. Waiting for client to unlock. Possible only for Force request. , 0x2 – Off. Client is locked. , 0x4 – Disabled. Client is disabled. , 0x8 – Reject. Client is lock/Disable request is rejected. Relevant for Try request. , 0x10 – Error. Lock request for a disabled client., reset value: 0x0, access type: RO
		uint32 reserved0 : 3;
		uint32 master3LockerDoneInt : 1; //Lock request done indication, reset value: 0x0, access type: RO
		uint32 reserved1 : 23;
	} bitFields;
} RegTxSelectorTxSelSwMaster3LockerStatus_u;

/*REG_TX_SELECTOR_TX_SEL_LOCKER_INT_CLR 0x1C4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swMaster0LockerIntClr : 1; //no description, reset value: 0x0, access type: WO
		uint32 swMaster1LockerIntClr : 1; //no description, reset value: 0x0, access type: WO
		uint32 swMaster2LockerIntClr : 1; //no description, reset value: 0x0, access type: WO
		uint32 swMaster3LockerIntClr : 1; //no description, reset value: 0x0, access type: WO
		uint32 reserved0 : 28;
	} bitFields;
} RegTxSelectorTxSelLockerIntClr_u;

/*REG_TX_SELECTOR_TX_SEL_LOCKER_PUSH_WHILE_BUSY 0x1C8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 master0PushWhileBusy : 1; //no description, reset value: 0x0, access type: RO
		uint32 master1PushWhileBusy : 1; //no description, reset value: 0x0, access type: RO
		uint32 master2PushWhileBusy : 1; //no description, reset value: 0x0, access type: RO
		uint32 master3PushWhileBusy : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 28;
	} bitFields;
} RegTxSelectorTxSelLockerPushWhileBusy_u;

/*REG_TX_SELECTOR_TX_SEL_LOCKER_PUSH_WHILE_BUSY_CLR 0x1CC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 master0PushWhileBusyClr : 1; //no description, reset value: 0x0, access type: WO
		uint32 master1PushWhileBusyClr : 1; //no description, reset value: 0x0, access type: WO
		uint32 master2PushWhileBusyClr : 1; //no description, reset value: 0x0, access type: WO
		uint32 master3PushWhileBusyClr : 1; //no description, reset value: 0x0, access type: WO
		uint32 reserved0 : 28;
	} bitFields;
} RegTxSelectorTxSelLockerPushWhileBusyClr_u;

/*REG_TX_SELECTOR_TX_SELECTOR_SPARE 0x1D0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSelectorSpare : 32; //no description, reset value: 0x0, access type: RW
	} bitFields;
} RegTxSelectorTxSelectorSpare_u;

/*REG_TX_SELECTOR_TX_SEL_MU_SELECT_CFG 0x200 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muEn : 1; //MU enable of the TX Selector, reset value: 0x0, access type: RW
		uint32 reserved0 : 3;
		uint32 muSecondarySel : 1; //MU secondary selection algorithm: , 0: MU Secondary TID equal to Primary TID , 1: MU Secondary TID can be any TID , , reset value: 0x0, access type: RW
		uint32 reserved1 : 3;
		uint32 maxGroupNum : 8; //Maximum number of supported groups. Number of groups is equal to max_group_num+1., reset value: 0xF, access type: RW
		uint32 selectorMuLockEn : 1; //Enables/Disables the TX Selector HW lock: , 0x0: Don't lock after selection , 0x1: Lock after selection, reset value: 0x1, access type: RW
		uint32 reserved2 : 2;
		uint32 vhtMuSuSelBypass : 1; //no description, reset value: 0x1, access type: RW
		uint32 suSelEn1Sec : 1; //In case of VHT MU threshold fail - Configures if SU selection will happen, reset value: 0x1, access type: RW
		uint32 reserved3 : 1;
		uint32 muThresh1Sec : 1; //Minimum number of secondaries to be selected to enable selection. Possible values 0-1, reset value: 0x0, access type: RW
		uint32 reserved4 : 1;
		uint32 suSelEn2Sec : 1; //In case of VHT MU threshold fail - Configures if SU selection will happen, reset value: 0x1, access type: RW
		uint32 reserved5 : 1;
		uint32 muThresh2Sec : 2; //Minimum number of secondaries to be selected to enable selection. Possible values 0-2, reset value: 0x0, access type: RW
		uint32 suSelEn3Sec : 1; //In case of VHT MU threshold fail - Configures if SU selection will happen, reset value: 0x1, access type: RW
		uint32 reserved6 : 1;
		uint32 muThresh3Sec : 2; //Minimum number of secondaries to be selected to enable selection. Possible values 0-3, reset value: 0x0, access type: RW
	} bitFields;
} RegTxSelectorTxSelMuSelectCfg_u;

/*REG_TX_SELECTOR_TX_SEL_MU_BITMAP_UPDATE 0x204 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 memLineNum : 8; //MU Group index number. Can be 0-15., reset value: 0x0, access type: RW
		uint32 userPositionIdx : 2; //User position index number in a group, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 muBitTypeSel : 3; //MU bit type: , 0x0: Group User position STA , 0x1: Group lock , 0x2: Group Tx mode , 0x3: STA group mapping , 0x4: STA MU tid Enable , , reset value: 0x0, access type: RW
		uint32 allGrp : 1; //Indication that the lower 36 bits of the group are updated with the adjacent register data., reset value: 0x0, access type: RW
		uint32 muBitmapUpdateValue : 8; //Write data value - For each update there is a different data width. Data decoding is according to mu_bit_type_sel: , 0x0: 8 bits are STA num, Valid bit will be taken from "all_grp_valid_up". , 0x1: LSB bit is MU lock bit value. , 0x2: 2 LSB bits are TX mode value. , 0x3: 7 LSB bits are Group index, MSB is STA Primary enable bit. , 0x4: 8 bits for 8 TIDs enable/disable MU., reset value: 0x0, access type: RW
		uint32 allGrpValidUp : 4; //For all_grp the entire field is used to update the UP valid bits. When a single UP STA idx is updated the relevant bit is used., reset value: 0x0, access type: RW
		uint32 reserved1 : 3;
		uint32 muBitmapUpdateFree : 1; //Bit inidcating that the register is free for update instruction, reset value: 0x1, access type: RO
	} bitFields;
} RegTxSelectorTxSelMuBitmapUpdate_u;

/*REG_TX_SELECTOR_TX_SEL_MU_BITMAP_UPDATE_ALL_STA_WDATA 0x208 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 allGrpWdata : 32; //no description, reset value: 0x0, access type: RW
	} bitFields;
} RegTxSelectorTxSelMuBitmapUpdateAllStaWdata_u;

/*REG_TX_SELECTOR_TX_SEL_MU_PRIMARY_THRES 0x20C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pdPrimaryThres : 20; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 12;
	} bitFields;
} RegTxSelectorTxSelMuPrimaryThres_u;

/*REG_TX_SELECTOR_TX_SEL_MU_SECONDARY_THRES 0x210 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pdSecondaryThres : 20; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 12;
	} bitFields;
} RegTxSelectorTxSelMuSecondaryThres_u;

/*REG_TX_SELECTOR_TX_SEL_MU_ERR 0x214 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 primaryNotValidErr : 1; //no description, reset value: 0x0, access type: RO
		uint32 noPrimaryInGrpErr : 1; //no description, reset value: 0x0, access type: RO
		uint32 primaryGrpThresErr : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 29;
	} bitFields;
} RegTxSelectorTxSelMuErr_u;

/*REG_TX_SELECTOR_TX_SEL_MU_ERR_CLR 0x218 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 primaryNotValidErrClr : 1; //no description, reset value: 0x0, access type: WO
		uint32 noPrimaryInGrpErrClr : 1; //no description, reset value: 0x0, access type: WO
		uint32 primaryGrpThresErrClr : 1; //no description, reset value: 0x0, access type: WO
		uint32 reserved0 : 29;
	} bitFields;
} RegTxSelectorTxSelMuErrClr_u;

/*REG_TX_SELECTOR_TX_SEL_MU_LOCKER_EN 0x220 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muLockerEn : 1; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegTxSelectorTxSelMuLockerEn_u;

/*REG_TX_SELECTOR_TX_SEL_MU_SW_MASTER0_LOCKER 0x224 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 master0MuLockerGrpSel : 7; //Group index number, reset value: 0x0, access type: RW
		uint32 reserved0 : 17;
		uint32 master0MuLockerBitTypeSel : 2; //Information bit type selector, reset value: 0x0, access type: RW
		uint32 reserved1 : 6;
	} bitFields;
} RegTxSelectorTxSelMuSwMaster0Locker_u;

/*REG_TX_SELECTOR_TX_SEL_MU_SW_MASTER1_LOCKER 0x228 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 master1MuLockerGrpSel : 7; //Group index number, reset value: 0x0, access type: RW
		uint32 reserved0 : 17;
		uint32 master1MuLockerBitTypeSel : 2; //Information bit type selector, reset value: 0x0, access type: RW
		uint32 reserved1 : 6;
	} bitFields;
} RegTxSelectorTxSelMuSwMaster1Locker_u;

/*REG_TX_SELECTOR_TX_SEL_MU_SW_MASTER2_LOCKER 0x22C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 master2MuLockerGrpSel : 7; //Group index number, reset value: 0x0, access type: RW
		uint32 reserved0 : 17;
		uint32 master2MuLockerBitTypeSel : 2; //Information bit type selector, reset value: 0x0, access type: RW
		uint32 reserved1 : 6;
	} bitFields;
} RegTxSelectorTxSelMuSwMaster2Locker_u;

/*REG_TX_SELECTOR_TX_SEL_MU_SW_MASTER3_LOCKER 0x230 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 master3MuLockerGrpSel : 7; //Group index number, reset value: 0x0, access type: RW
		uint32 reserved0 : 17;
		uint32 master3MuLockerBitTypeSel : 2; //Information bit type selector, reset value: 0x0, access type: RW
		uint32 reserved1 : 6;
	} bitFields;
} RegTxSelectorTxSelMuSwMaster3Locker_u;

/*REG_TX_SELECTOR_TX_SEL_MU_SW_MASTER0_LOCKER_STATUS 0x234 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swMaster0MuReqStatus : 5; //SW Master request status: , 0x0 – IDLE. , 0x1 – Pending. Waiting for client to unlock. Possible only for Force request. , 0x2 – Off. Client is locked. , 0x8 – Reject. Client is lock/Disable request is rejected. Relevant for Try request., reset value: 0x0, access type: RO
		uint32 reserved0 : 3;
		uint32 master0MuLockerDoneInt : 1; //Lock request done indication, reset value: 0x0, access type: RO
		uint32 reserved1 : 23;
	} bitFields;
} RegTxSelectorTxSelMuSwMaster0LockerStatus_u;

/*REG_TX_SELECTOR_TX_SEL_MU_SW_MASTER1_LOCKER_STATUS 0x238 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swMaster1MuReqStatus : 5; //SW Master request status: , 0x0 – IDLE. , 0x1 – Pending. Waiting for client to unlock. Possible only for Force request. , 0x2 – Off. Client is locked. , 0x8 – Reject. Client is lock/Disable request is rejected. Relevant for Try request., reset value: 0x0, access type: RO
		uint32 reserved0 : 3;
		uint32 master1MuLockerDoneInt : 1; //Lock request done indication, reset value: 0x0, access type: RO
		uint32 reserved1 : 23;
	} bitFields;
} RegTxSelectorTxSelMuSwMaster1LockerStatus_u;

/*REG_TX_SELECTOR_TX_SEL_MU_SW_MASTER2_LOCKER_STATUS 0x23C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swMaster2MuReqStatus : 5; //SW Master request status: , 0x0 – IDLE. , 0x1 – Pending. Waiting for client to unlock. Possible only for Force request. , 0x2 – Off. Client is locked. , 0x8 – Reject. Client is lock/Disable request is rejected. Relevant for Try request., reset value: 0x0, access type: RO
		uint32 reserved0 : 3;
		uint32 master2MuLockerDoneInt : 1; //Lock request done indication, reset value: 0x0, access type: RO
		uint32 reserved1 : 23;
	} bitFields;
} RegTxSelectorTxSelMuSwMaster2LockerStatus_u;

/*REG_TX_SELECTOR_TX_SEL_MU_SW_MASTER3_LOCKER_STATUS 0x240 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swMaster3MuReqStatus : 5; //SW Master request status: , 0x0 – IDLE. , 0x1 – Pending. Waiting for client to unlock. Possible only for Force request. , 0x2 – Off. Client is locked. , 0x8 – Reject. Client is lock/Disable request is rejected. Relevant for Try request., reset value: 0x0, access type: RO
		uint32 reserved0 : 3;
		uint32 master3MuLockerDoneInt : 1; //Lock request done indication, reset value: 0x0, access type: RO
		uint32 reserved1 : 23;
	} bitFields;
} RegTxSelectorTxSelMuSwMaster3LockerStatus_u;

/*REG_TX_SELECTOR_TX_SEL_MU_LOCKER_INT_CLR 0x244 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swMaster0MuLockerIntClr : 1; //no description, reset value: 0x0, access type: WO
		uint32 swMaster1MuLockerIntClr : 1; //no description, reset value: 0x0, access type: WO
		uint32 swMaster2MuLockerIntClr : 1; //no description, reset value: 0x0, access type: WO
		uint32 swMaster3MuLockerIntClr : 1; //no description, reset value: 0x0, access type: WO
		uint32 reserved0 : 28;
	} bitFields;
} RegTxSelectorTxSelMuLockerIntClr_u;

/*REG_TX_SELECTOR_TX_SEL_MU_LOCKER_PUSH_WHILE_BUSY 0x248 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 master0MuPushWhileBusy : 1; //no description, reset value: 0x0, access type: RO
		uint32 master1MuPushWhileBusy : 1; //no description, reset value: 0x0, access type: RO
		uint32 master2MuPushWhileBusy : 1; //no description, reset value: 0x0, access type: RO
		uint32 master3MuPushWhileBusy : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 28;
	} bitFields;
} RegTxSelectorTxSelMuLockerPushWhileBusy_u;

/*REG_TX_SELECTOR_TX_SEL_MU_LOCKER_PUSH_WHILE_BUSY_CLR 0x24C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 master0MuPushWhileBusyClr : 1; //no description, reset value: 0x0, access type: WO
		uint32 master1MuPushWhileBusyClr : 1; //no description, reset value: 0x0, access type: WO
		uint32 master2MuPushWhileBusyClr : 1; //no description, reset value: 0x0, access type: WO
		uint32 master3MuPushWhileBusyClr : 1; //no description, reset value: 0x0, access type: WO
		uint32 reserved0 : 28;
	} bitFields;
} RegTxSelectorTxSelMuLockerPushWhileBusyClr_u;

/*REG_TX_SELECTOR_TX_SEL_MU_UP0_RESULT 0x250 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muUp0Tid : 3; //no description, reset value: 0x0, access type: RO
		uint32 muUp0Sta : 8; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 1;
		uint32 muUp0Vap : 5; //no description, reset value: 0x0, access type: RO
		uint32 muUp0TxqId : 3; //no description, reset value: 0x0, access type: RO
		uint32 muUp0AcId : 2; //no description, reset value: 0x0, access type: RO
		uint32 reserved1 : 2;
		uint32 muUp0BfReq : 2; //no description, reset value: 0x0, access type: RO
		uint32 reserved2 : 2;
		uint32 muUp0Valid : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved3 : 3;
	} bitFields;
} RegTxSelectorTxSelMuUp0Result_u;

/*REG_TX_SELECTOR_TX_SEL_MU_UP1_RESULT 0x254 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muUp1Tid : 3; //no description, reset value: 0x0, access type: RO
		uint32 muUp1Sta : 8; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 1;
		uint32 muUp1Vap : 5; //no description, reset value: 0x0, access type: RO
		uint32 muUp1TxqId : 3; //no description, reset value: 0x0, access type: RO
		uint32 muUp1AcId : 2; //no description, reset value: 0x0, access type: RO
		uint32 reserved1 : 2;
		uint32 muUp1BfReq : 2; //no description, reset value: 0x0, access type: RO
		uint32 reserved2 : 2;
		uint32 muUp1Valid : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved3 : 3;
	} bitFields;
} RegTxSelectorTxSelMuUp1Result_u;

/*REG_TX_SELECTOR_TX_SEL_MU_UP2_RESULT 0x258 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muUp2Tid : 3; //no description, reset value: 0x0, access type: RO
		uint32 muUp2Sta : 8; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 1;
		uint32 muUp2Vap : 5; //no description, reset value: 0x0, access type: RO
		uint32 muUp2TxqId : 3; //no description, reset value: 0x0, access type: RO
		uint32 muUp2AcId : 2; //no description, reset value: 0x0, access type: RO
		uint32 reserved1 : 2;
		uint32 muUp2BfReq : 2; //no description, reset value: 0x0, access type: RO
		uint32 reserved2 : 2;
		uint32 muUp2Valid : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved3 : 3;
	} bitFields;
} RegTxSelectorTxSelMuUp2Result_u;

/*REG_TX_SELECTOR_TX_SEL_MU_UP3_RESULT 0x25C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muUp3Tid : 3; //no description, reset value: 0x0, access type: RO
		uint32 muUp3Sta : 8; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 1;
		uint32 muUp3Vap : 5; //no description, reset value: 0x0, access type: RO
		uint32 muUp3TxqId : 3; //no description, reset value: 0x0, access type: RO
		uint32 muUp3AcId : 2; //no description, reset value: 0x0, access type: RO
		uint32 reserved1 : 2;
		uint32 muUp3BfReq : 2; //no description, reset value: 0x0, access type: RO
		uint32 reserved2 : 2;
		uint32 muUp3Valid : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved3 : 3;
	} bitFields;
} RegTxSelectorTxSelMuUp3Result_u;

/*REG_TX_SELECTOR_TX_SEL_MU_UP0_BITMAP 0x260 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muUp0BitmapReq0 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 muUp0BitmapPsType0 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 muUp0BitmapPsMode0 : 1; //PS mode, reset value: 0x0, access type: RO
		uint32 muUp0BitmapDataInQ0 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 muUp0BitmapReq1 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 muUp0BitmapPsType1 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 muUp0BitmapPsMode1 : 1; //PS mode, reset value: 0x0, access type: RO
		uint32 muUp0BitmapDataInQ1 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 muUp0BitmapReq2 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 muUp0BitmapPsType2 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 muUp0BitmapPsMode2 : 1; //PS mode, reset value: 0x0, access type: RO
		uint32 muUp0BitmapDataInQ2 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 muUp0BitmapReq3 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 muUp0BitmapPsType3 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 muUp0BitmapPsMode3 : 1; //PS mode, reset value: 0x0, access type: RO
		uint32 muUp0BitmapDataInQ3 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 muUp0BitmapReq4 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 muUp0BitmapPsType4 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 muUp0BitmapPsMode4 : 1; //PS mode, reset value: 0x0, access type: RO
		uint32 muUp0BitmapDataInQ4 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 muUp0BitmapReq5 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 muUp0BitmapPsType5 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 muUp0BitmapPsMode5 : 1; //PS mode, reset value: 0x0, access type: RO
		uint32 muUp0BitmapDataInQ5 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 muUp0BitmapReq6 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 muUp0BitmapPsType6 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 muUp0BitmapPsMode6 : 1; //PS mode, reset value: 0x0, access type: RO
		uint32 muUp0BitmapDataInQ6 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 muUp0BitmapReq7 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 muUp0BitmapPsType7 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 muUp0BitmapPsMode7 : 1; //PS mode, reset value: 0x0, access type: RO
		uint32 muUp0BitmapDataInQ7 : 1; //Data in Q, reset value: 0x0, access type: RO
	} bitFields;
} RegTxSelectorTxSelMuUp0Bitmap_u;

/*REG_TX_SELECTOR_TX_SEL_HE_MU_UP0_BITMAP 0x264 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 heMuUp0BitmapReq8 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 heMuUp0BitmapPsType8 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 heMuUp0BitmapPsMode8 : 1; //PS mode, reset value: 0x0, access type: RO
		uint32 heMuUp0BitmapDataInQ8 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 heMuUp0BitmapStatus : 25; //HE bitmap Status - {ul_he_mu_en,dl_he_mu_en[8:0],ul_sta_tid_data_in_q[7:0],twt_avail,twt_sp,twt_ann,omi_valid,omi_supp,tomi_ul_dis,tomi_dl_dis}, reset value: 0x0, access type: RO
		uint32 reserved0 : 3;
	} bitFields;
} RegTxSelectorTxSelHeMuUp0Bitmap_u;

/*REG_TX_SELECTOR_TX_SEL_HE_PRE_AGG_UP0_BITMAP 0x268 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 heSelUp0PreAggBitmap : 9; //Pre agg bitmap - {INSERT_A_CONTROL,TOMI_MAX_NSS[2:0],OMI_BW[1:0],ROMI_MAX_NSS[2:0]}, reset value: 0x0, access type: RO
		uint32 reserved0 : 23;
	} bitFields;
} RegTxSelectorTxSelHePreAggUp0Bitmap_u;

/*REG_TX_SELECTOR_TX_SEL_MU_UP1_BITMAP 0x26C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 9;
		uint32 muUp1BitmapPsType2 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 muUp1BitmapPsMode2 : 1; //PS mode, reset value: 0x0, access type: RO
		uint32 muUp1BitmapDataInQ2 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 muUp1BitmapReq3 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 muUp1BitmapPsType3 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 muUp1BitmapPsMode3 : 1; //PS mode, reset value: 0x0, access type: RO
		uint32 muUp1BitmapDataInQ3 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 muUp1BitmapReq4 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 muUp1BitmapPsType4 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 muUp1BitmapPsMode4 : 1; //PS mode, reset value: 0x0, access type: RO
		uint32 muUp1BitmapDataInQ4 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 muUp1BitmapReq5 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 muUp1BitmapPsType5 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 muUp1BitmapPsMode5 : 1; //PS mode, reset value: 0x0, access type: RO
		uint32 muUp1BitmapDataInQ5 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 muUp1BitmapReq6 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 muUp1BitmapPsType6 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 muUp1BitmapPsMode6 : 1; //PS mode, reset value: 0x0, access type: RO
		uint32 muUp1BitmapDataInQ6 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 muUp1BitmapReq7 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 muUp1BitmapPsType7 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 muUp1BitmapPsMode7 : 1; //PS mode, reset value: 0x0, access type: RO
		uint32 muUp1BitmapDataInQ7 : 1; //Data in Q, reset value: 0x0, access type: RO
	} bitFields;
} RegTxSelectorTxSelMuUp1Bitmap_u;

/*REG_TX_SELECTOR_TX_SEL_HE_MU_UP1_BITMAP 0x270 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 heMuUp1BitmapReq8 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 heMuUp1BitmapPsType8 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 heMuUp1BitmapPsMode8 : 1; //PS mode, reset value: 0x0, access type: RO
		uint32 heMuUp1BitmapDataInQ8 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 heMuUp1BitmapStatus : 25; //HE bitmap Status - {ul_he_mu_en,dl_he_mu_en[8:0],ul_sta_tid_data_in_q[7:0],twt_avail,twt_sp,twt_ann,omi_valid,omi_supp,tomi_ul_dis,tomi_dl_dis}, reset value: 0x0, access type: RO
		uint32 reserved0 : 3;
	} bitFields;
} RegTxSelectorTxSelHeMuUp1Bitmap_u;

/*REG_TX_SELECTOR_TX_SEL_HE_PRE_AGG_UP1_BITMAP 0x274 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 heSelUp1PreAggBitmap : 9; //Pre agg bitmap - {INSERT_A_CONTROL,TOMI_MAX_NSS[2:0],OMI_BW[1:0],ROMI_MAX_NSS[2:0]}, reset value: 0x0, access type: RO
		uint32 reserved0 : 23;
	} bitFields;
} RegTxSelectorTxSelHePreAggUp1Bitmap_u;

/*REG_TX_SELECTOR_TX_SEL_MU_UP2_BITMAP 0x278 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 7;
		uint32 muUp2BitmapDataInQ1 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 muUp2BitmapReq2 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 muUp2BitmapPsType2 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 muUp2BitmapPsMode2 : 1; //PS mode, reset value: 0x0, access type: RO
		uint32 muUp2BitmapDataInQ2 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 muUp2BitmapReq3 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 muUp2BitmapPsType3 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 muUp2BitmapPsMode3 : 1; //PS mode, reset value: 0x0, access type: RO
		uint32 muUp2BitmapDataInQ3 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 muUp2BitmapReq4 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 muUp2BitmapPsType4 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 muUp2BitmapPsMode4 : 1; //PS mode, reset value: 0x0, access type: RO
		uint32 muUp2BitmapDataInQ4 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 muUp2BitmapReq5 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 muUp2BitmapPsType5 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 muUp2BitmapPsMode5 : 1; //PS mode, reset value: 0x0, access type: RO
		uint32 muUp2BitmapDataInQ5 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 muUp2BitmapReq6 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 muUp2BitmapPsType6 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 muUp2BitmapPsMode6 : 1; //PS mode, reset value: 0x0, access type: RO
		uint32 muUp2BitmapDataInQ6 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 muUp2BitmapReq7 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 muUp2BitmapPsType7 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 muUp2BitmapPsMode7 : 1; //PS mode, reset value: 0x0, access type: RO
		uint32 muUp2BitmapDataInQ7 : 1; //Data in Q, reset value: 0x0, access type: RO
	} bitFields;
} RegTxSelectorTxSelMuUp2Bitmap_u;

/*REG_TX_SELECTOR_TX_SEL_HE_MU_UP2_BITMAP 0x27C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 heMuUp2BitmapReq8 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 heMuUp2BitmapPsType8 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 heMuUp2BitmapPsMode8 : 1; //PS mode, reset value: 0x0, access type: RO
		uint32 heMuUp2BitmapDataInQ8 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 heMuUp2BitmapStatus : 25; //HE bitmap Status - {ul_he_mu_en,dl_he_mu_en[8:0],ul_sta_tid_data_in_q[7:0],twt_avail,twt_sp,twt_ann,omi_valid,omi_supp,tomi_ul_dis,tomi_dl_dis}, reset value: 0x0, access type: RO
		uint32 reserved0 : 3;
	} bitFields;
} RegTxSelectorTxSelHeMuUp2Bitmap_u;

/*REG_TX_SELECTOR_TX_SEL_HE_PRE_AGG_UP2_BITMAP 0x280 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 heSelUp2PreAggBitmap : 9; //Pre agg bitmap - {INSERT_A_CONTROL,TOMI_MAX_NSS[2:0],OMI_BW[1:0],ROMI_MAX_NSS[2:0]}, reset value: 0x0, access type: RO
		uint32 reserved0 : 23;
	} bitFields;
} RegTxSelectorTxSelHePreAggUp2Bitmap_u;

/*REG_TX_SELECTOR_TX_SEL_MU_UP3_BITMAP 0x284 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muUp3BitmapReq0 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 muUp3BitmapPsType0 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 muUp3BitmapPsMode0 : 1; //PS mode, reset value: 0x0, access type: RO
		uint32 muUp3BitmapDataInQ0 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 muUp3BitmapReq1 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 muUp3BitmapPsType1 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 muUp3BitmapPsMode1 : 1; //PS mode, reset value: 0x0, access type: RO
		uint32 muUp3BitmapDataInQ1 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 muUp3BitmapReq2 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 muUp3BitmapPsType2 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 muUp3BitmapPsMode2 : 1; //PS mode, reset value: 0x0, access type: RO
		uint32 muUp3BitmapDataInQ2 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 muUp3BitmapReq3 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 muUp3BitmapPsType3 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 muUp3BitmapPsMode3 : 1; //PS mode, reset value: 0x0, access type: RO
		uint32 muUp3BitmapDataInQ3 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 muUp3BitmapReq4 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 muUp3BitmapPsType4 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 muUp3BitmapPsMode4 : 1; //PS mode, reset value: 0x0, access type: RO
		uint32 muUp3BitmapDataInQ4 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 muUp3BitmapReq5 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 muUp3BitmapPsType5 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 muUp3BitmapPsMode5 : 1; //PS mode, reset value: 0x0, access type: RO
		uint32 muUp3BitmapDataInQ5 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 muUp3BitmapReq6 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 muUp3BitmapPsType6 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 muUp3BitmapPsMode6 : 1; //PS mode, reset value: 0x0, access type: RO
		uint32 muUp3BitmapDataInQ6 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 muUp3BitmapReq7 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 muUp3BitmapPsType7 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 muUp3BitmapPsMode7 : 1; //PS mode, reset value: 0x0, access type: RO
		uint32 muUp3BitmapDataInQ7 : 1; //Data in Q, reset value: 0x0, access type: RO
	} bitFields;
} RegTxSelectorTxSelMuUp3Bitmap_u;

/*REG_TX_SELECTOR_TX_SEL_HE_MU_UP3_BITMAP 0x288 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 heMuUp3BitmapReq8 : 1; //Request bit, reset value: 0x0, access type: RO
		uint32 heMuUp3BitmapPsType8 : 1; //PS type, reset value: 0x0, access type: RO
		uint32 heMuUp3BitmapPsMode8 : 1; //PS mode, reset value: 0x0, access type: RO
		uint32 heMuUp3BitmapDataInQ8 : 1; //Data in Q, reset value: 0x0, access type: RO
		uint32 heMuUp3BitmapStatus : 25; //HE bitmap Status - {ul_he_mu_en,dl_he_mu_en[8:0],ul_sta_tid_data_in_q[7:0],twt_avail,twt_sp,twt_ann,omi_valid,omi_supp,tomi_ul_dis,tomi_dl_dis}, reset value: 0x0, access type: RO
		uint32 reserved0 : 3;
	} bitFields;
} RegTxSelectorTxSelHeMuUp3Bitmap_u;

/*REG_TX_SELECTOR_TX_SEL_HE_PRE_AGG_UP3_BITMAP 0x28C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 heSelUp3PreAggBitmap : 9; //Pre agg bitmap - {INSERT_A_CONTROL,TOMI_MAX_NSS[2:0],OMI_BW[1:0],ROMI_MAX_NSS[2:0]}, reset value: 0x0, access type: RO
		uint32 reserved0 : 23;
	} bitFields;
} RegTxSelectorTxSelHePreAggUp3Bitmap_u;

/*REG_TX_SELECTOR_TX_SEL_MU_GRP_BITMAP 0x290 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muGrpValid : 1; //MU selection valid bit, reset value: 0x0, access type: RO
		uint32 primaryGrpPointer : 7; //Group number, reset value: 0x0, access type: RO
		uint32 primaryIdx : 2; //Index number indicating which UP is primary at the group, reset value: 0x0, access type: RO
		uint32 txMode : 2; //no description, reset value: 0x0, access type: RO
		uint32 upLockDone : 4; //4 bits, 1 bit per UP indicating which UP was locked at the SU bitmap, reset value: 0x0, access type: RO
		uint32 grpLockDone : 1; //MU group lock done/Not done, reset value: 0x0, access type: RO
		uint32 reserved0 : 15;
	} bitFields;
} RegTxSelectorTxSelMuGrpBitmap_u;

/*REG_TX_SELECTOR_TX_SEL_HE_GRP_BITMAP 0x294 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 selDlSel : 1; //DL/UL selection: , 0: UL , 1: DL, reset value: 0x0, access type: RO
		uint32 reserved0 : 3;
		uint32 selHeMuSu : 2; //HE/MU/SU Selection: , 0: Single User , 1: VHT MU , 2: HE, reset value: 0x0, access type: RO
		uint32 reserved1 : 2;
		uint32 selectionType : 6; //Selection Type, reset value: 0x0, access type: RO
		uint32 reserved2 : 2;
		uint32 selHePlanEntry : 7; //no description, reset value: 0x0, access type: RO
		uint32 reserved3 : 1;
		uint32 selHePlanLockDone : 1; //HE lock done, reset value: 0x0, access type: RO
		uint32 reserved4 : 7;
	} bitFields;
} RegTxSelectorTxSelHeGrpBitmap_u;

/*REG_TX_SELECTOR_TX_SEL_MU_SM_STATUS 0x298 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muSelSm : 4; //MU selection main SM, reset value: 0x0, access type: RO
		uint32 muSelLockSm : 3; //MU lock SM, reset value: 0x0, access type: RO
		uint32 reserved0 : 1;
		uint32 secPdCountRdSm : 3; //RD read from QoS RAM SM, reset value: 0x0, access type: RO
		uint32 reserved1 : 1;
		uint32 muLockerSm : 2; //no description, reset value: 0x0, access type: RO
		uint32 reserved2 : 18;
	} bitFields;
} RegTxSelectorTxSelMuSmStatus_u;

/*REG_TX_SELECTOR_TX_SEL_MU_SU_REPORT_STATUS 0x29C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSelMuSuSecondary0 : 4; //MU secondary participated in the MU TX with reason. Only one reason will be displayed according to this Prioirty list: , 0-  Selected (i.e. there was a TX for that user position) , 1-  Selector â Secondary not valid in group , 2 - Selector â STA disabled , 3-  Selector â No data in queue , 4-  Selector â STA lock , 5-  Selector â PS without request , 6 - Selector â PD threshold fail , 7 - Selector â TID MU En bit is 0, reset value: 0x0, access type: RO
		uint32 txSelMuSuSecondary1 : 4; //MU secondary participated in the MU TX with reason. Only one reason will be displayed according to this Prioirty list: , 0-  Selected (i.e. there was a TX for that user position) , 1-  Selector â Secondary not valid in group , 2 - Selector â STA disabled , 3-  Selector â No data in queue , 4-  Selector â STA lock , 5-  Selector â PS without request , 6 - Selector â PD threshold fail , 7 - Selector â TID MU En bit is 0, reset value: 0x0, access type: RO
		uint32 txSelMuSuSecondary2 : 4; //MU secondary participated in the MU TX with reason. Only one reason will be displayed according to this Prioirty list: , 0-  Selected (i.e. there was a TX for that user position) , 1-  Selector â Secondary not valid in group , 2 - Selector â STA disabled , 3-  Selector â No data in queue , 4-  Selector â STA lock , 5-  Selector â PS without request , 6 - Selector â PD threshold fail , 7 - Selector â TID MU En bit is 0, reset value: 0x0, access type: RO
		uint32 txSelMuSuSecondary3 : 4; //MU secondary participated in the MU TX with reason. Only one reason will be displayed according to this Prioirty list: , 0-  Selected (i.e. there was a TX for that user position) , 1-  Selector â Secondary not valid in group , 2 - Selector â STA disabled , 3-  Selector â No data in queue , 4-  Selector â STA lock , 5-  Selector â PS without request , 6 - Selector â PD threshold fail , 7 - Selector â TID MU En bit is 0, reset value: 0x0, access type: RO
		uint32 txSelMuSuPrimary : 7; //MU was changed to SU because of Primary(More than one reason possible): , Bit0 â Primary not in group , Bit1 â Primary is not valid , Bit2 â Group number is bigger than threshold , Bit3 â Group locked , Bit4 â All secondaries are not valid , Bit5 â No MU because of PS legacy selection , Bit6 â No MU because of FW threshold fail, reset value: 0x0, access type: RO
		uint32 reserved0 : 1;
		uint32 txSelMuSuValid : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved1 : 7;
	} bitFields;
} RegTxSelectorTxSelMuSuReportStatus_u;

/*REG_TX_SELECTOR_TX_SEL_SELECT_MODE_CFG 0x2A0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dlWithUlEn : 1; //no description, reset value: 0x1, access type: RW
		uint32 reserved0 : 3;
		uint32 mgmtBypassEn : 1; //Enables MGMT selection over other TIDs feature, reset value: 0x0, access type: RW
		uint32 reserved1 : 3;
		uint32 mgmtAcNum : 2; //no description, reset value: 0x3, access type: RW
		uint32 reserved2 : 2;
		uint32 heMuFallbackSu : 1; //Enables selection fallback from HE MU to SU in case of plan lock., reset value: 0x0, access type: RW
		uint32 reserved3 : 3;
		uint32 dlHePsWithoutReqEn : 1; //Allowing selection of DL HE MU PS with TWT==111 without PS req., reset value: 0x0, access type: RW
		uint32 reserved4 : 15;
	} bitFields;
} RegTxSelectorTxSelSelectModeCfg_u;

/*REG_TX_SELECTOR_TX_SEL_PLAN_LOCKER_EN 0x2A4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 planLockerEn : 1; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 7;
		uint32 planLockerSm : 2; //no description, reset value: 0x0, access type: RO
		uint32 reserved1 : 22;
	} bitFields;
} RegTxSelectorTxSelPlanLockerEn_u;

/*REG_TX_SELECTOR_TX_SEL_PLAN_SW_MASTER0_LOCKER 0x300 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 master0PlanLockerIdxSel : 7; //plan index number, reset value: 0x0, access type: RW
		uint32 reserved0 : 17;
		uint32 master0PlanLockerBitTypeSel : 2; //Information bit type selector, reset value: 0x0, access type: RW
		uint32 reserved1 : 6;
	} bitFields;
} RegTxSelectorTxSelPlanSwMaster0Locker_u;

/*REG_TX_SELECTOR_TX_SEL_PLAN_SW_MASTER1_LOCKER 0x304 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 master1PlanLockerIdxSel : 7; //plan index number, reset value: 0x0, access type: RW
		uint32 reserved0 : 17;
		uint32 master1PlanLockerBitTypeSel : 2; //Information bit type selector, reset value: 0x0, access type: RW
		uint32 reserved1 : 6;
	} bitFields;
} RegTxSelectorTxSelPlanSwMaster1Locker_u;

/*REG_TX_SELECTOR_TX_SEL_PLAN_SW_MASTER2_LOCKER 0x308 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 master2PlanLockerIdxSel : 7; //plan index number, reset value: 0x0, access type: RW
		uint32 reserved0 : 17;
		uint32 master2PlanLockerBitTypeSel : 2; //Information bit type selector, reset value: 0x0, access type: RW
		uint32 reserved1 : 6;
	} bitFields;
} RegTxSelectorTxSelPlanSwMaster2Locker_u;

/*REG_TX_SELECTOR_TX_SEL_PLAN_SW_MASTER3_LOCKER 0x30C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 master3PlanLockerIdxSel : 7; //plan index number, reset value: 0x0, access type: RW
		uint32 reserved0 : 17;
		uint32 master3PlanLockerBitTypeSel : 2; //Information bit type selector, reset value: 0x0, access type: RW
		uint32 reserved1 : 6;
	} bitFields;
} RegTxSelectorTxSelPlanSwMaster3Locker_u;

/*REG_TX_SELECTOR_TX_SEL_PLAN_SW_MASTER0_LOCKER_STATUS 0x310 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swMaster0PlanReqStatus : 5; //SW Master request status: , 0x0 – IDLE. , 0x1 – Pending. Waiting for client to unlock. Possible only for Force request. , 0x2 – Off. Client is locked. , 0x8 – Reject. Client is lock/Disable request is rejected. Relevant for Try request., reset value: 0x0, access type: RO
		uint32 reserved0 : 3;
		uint32 master0PlanLockerDoneInt : 1; //Lock request done indication, reset value: 0x0, access type: RO
		uint32 reserved1 : 23;
	} bitFields;
} RegTxSelectorTxSelPlanSwMaster0LockerStatus_u;

/*REG_TX_SELECTOR_TX_SEL_PLAN_SW_MASTER1_LOCKER_STATUS 0x314 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swMaster1PlanReqStatus : 5; //SW Master request status: , 0x0 – IDLE. , 0x1 – Pending. Waiting for client to unlock. Possible only for Force request. , 0x2 – Off. Client is locked. , 0x8 – Reject. Client is lock/Disable request is rejected. Relevant for Try request., reset value: 0x0, access type: RO
		uint32 reserved0 : 3;
		uint32 master1PlanLockerDoneInt : 1; //Lock request done indication, reset value: 0x0, access type: RO
		uint32 reserved1 : 23;
	} bitFields;
} RegTxSelectorTxSelPlanSwMaster1LockerStatus_u;

/*REG_TX_SELECTOR_TX_SEL_PLAN_SW_MASTER2_LOCKER_STATUS 0x318 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swMaster2PlanReqStatus : 5; //SW Master request status: , 0x0 – IDLE. , 0x1 – Pending. Waiting for client to unlock. Possible only for Force request. , 0x2 – Off. Client is locked. , 0x8 – Reject. Client is lock/Disable request is rejected. Relevant for Try request., reset value: 0x0, access type: RO
		uint32 reserved0 : 3;
		uint32 master2PlanLockerDoneInt : 1; //Lock request done indication, reset value: 0x0, access type: RO
		uint32 reserved1 : 23;
	} bitFields;
} RegTxSelectorTxSelPlanSwMaster2LockerStatus_u;

/*REG_TX_SELECTOR_TX_SEL_PLAN_SW_MASTER3_LOCKER_STATUS 0x31C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swMaster3PlanReqStatus : 5; //SW Master request status: , 0x0 – IDLE. , 0x1 – Pending. Waiting for client to unlock. Possible only for Force request. , 0x2 – Off. Client is locked. , 0x8 – Reject. Client is lock/Disable request is rejected. Relevant for Try request., reset value: 0x0, access type: RO
		uint32 reserved0 : 3;
		uint32 master3PlanLockerDoneInt : 1; //Lock request done indication, reset value: 0x0, access type: RO
		uint32 reserved1 : 23;
	} bitFields;
} RegTxSelectorTxSelPlanSwMaster3LockerStatus_u;

/*REG_TX_SELECTOR_TX_SEL_PLAN_LOCKER_INT_CLR 0x320 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swMaster0PlanLockerIntClr : 1; //no description, reset value: 0x0, access type: WO
		uint32 swMaster1PlanLockerIntClr : 1; //no description, reset value: 0x0, access type: WO
		uint32 swMaster2PlanLockerIntClr : 1; //no description, reset value: 0x0, access type: WO
		uint32 swMaster3PlanLockerIntClr : 1; //no description, reset value: 0x0, access type: WO
		uint32 reserved0 : 28;
	} bitFields;
} RegTxSelectorTxSelPlanLockerIntClr_u;

/*REG_TX_SELECTOR_TX_SEL_PLAN_LOCKER_PUSH_WHILE_BUSY 0x324 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 master0PlanPushWhileBusy : 1; //no description, reset value: 0x0, access type: RO
		uint32 master1PlanPushWhileBusy : 1; //no description, reset value: 0x0, access type: RO
		uint32 master2PlanPushWhileBusy : 1; //no description, reset value: 0x0, access type: RO
		uint32 master3PlanPushWhileBusy : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 28;
	} bitFields;
} RegTxSelectorTxSelPlanLockerPushWhileBusy_u;

/*REG_TX_SELECTOR_TX_SEL_PLAN_LOCKER_PUSH_WHILE_BUSY_CLR 0x328 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 master0PlanPushWhileBusyClr : 1; //no description, reset value: 0x0, access type: WO
		uint32 master1PlanPushWhileBusyClr : 1; //no description, reset value: 0x0, access type: WO
		uint32 master2PlanPushWhileBusyClr : 1; //no description, reset value: 0x0, access type: WO
		uint32 master3PlanPushWhileBusyClr : 1; //no description, reset value: 0x0, access type: WO
		uint32 reserved0 : 28;
	} bitFields;
} RegTxSelectorTxSelPlanLockerPushWhileBusyClr_u;

/*REG_TX_SELECTOR_TX_SEL_TWT_SW_HALT 0x350 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 twtSwHalt : 1; //Enables update of twt_avail field, reset value: 0x1, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegTxSelectorTxSelTwtSwHalt_u;

/*REG_TX_SELECTOR_TX_SEL_TWT_CFG 0x354 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 twtAvailUpdateEn : 1; //Enables update of twt_avail field, reset value: 0x1, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegTxSelectorTxSelTwtCfg_u;

/*REG_TX_SELECTOR_TX_SEL_TWT_SP_GROUP_UPDATE 0x358 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 twtSpHighLimit : 19; //High phase of the SP. Values are in ~32uS resolution., reset value: 0x0, access type: RW
		uint32 reserved0 : 5;
		uint32 twtSpGroupIdx : 5; //SP Group index, reset value: 0x0, access type: RW
		uint32 reserved1 : 2;
		uint32 twtSpUpdateDone : 1; //Instruction done indication, reset value: 0x1, access type: RO
	} bitFields;
} RegTxSelectorTxSelTwtSpGroupUpdate_u;

/*REG_TX_SELECTOR_TX_SEL_TWT_SP_GROUP_START_TSF 0x35C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 twtSpStartTsf : 32; //Start TSF value. 32 LSB bit only (Out of the full 64 bits), reset value: 0x0, access type: RW
	} bitFields;
} RegTxSelectorTxSelTwtSpGroupStartTsf_u;

/*REG_TX_SELECTOR_TX_SEL_TWT_SP_GROUP_LOW_PHASE 0x360 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 twtSpLowLimit : 19; //Low phase of the SP. Values are in ~32uS resolution., reset value: 0x0, access type: RW
		uint32 reserved0 : 1;
		uint32 twtAvailUpdateValue : 1; //twt_avail field write value during workflow., reset value: 0x0, access type: RW
		uint32 reserved1 : 11;
	} bitFields;
} RegTxSelectorTxSelTwtSpGroupLowPhase_u;

/*REG_TX_SELECTOR_TX_SEL_TWT_SP_STA_UPDATE 0x364 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 twtSpStaGroupIdx : 5; //SP Group index, reset value: 0x0, access type: RW
		uint32 reserved0 : 3;
		uint32 twtSpStaIdx : 8; //STA index, reset value: 0x0, access type: RW
		uint32 twtSpStaValue : 1; //Connectd/Disconnect STA from group, reset value: 0x0, access type: RW
		uint32 reserved1 : 14;
		uint32 twtSpStaUpdateDone : 1; //Instruction done indication, reset value: 0x1, access type: RO
	} bitFields;
} RegTxSelectorTxSelTwtSpStaUpdate_u;

/*REG_TX_SELECTOR_TX_SEL_TWT_SP_GROUP_VALID 0x368 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 twtSpGroupValid : 32; //Enabled SP groups, reset value: 0x0, access type: RO
	} bitFields;
} RegTxSelectorTxSelTwtSpGroupValid_u;

/*REG_TX_SELECTOR_TX_SEL_TWT_SP_GROUP_DEACTIVATE 0x36C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 twtSpDeactivateGroupIdx : 5; //SP group index, reset value: 0x0, access type: RW
		uint32 reserved0 : 26;
		uint32 twtSpDeactivateDone : 1; //Instruction done indication, reset value: 0x1, access type: RO
	} bitFields;
} RegTxSelectorTxSelTwtSpGroupDeactivate_u;

/*REG_TX_SELECTOR_TX_SEL_PRE_AGG_SEL_REQ_BITS0 0x380 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 preAggDl : 1; //no description, reset value: 0x0, access type: RO
		uint32 preAggMgmtOnly : 1; //no description, reset value: 0x0, access type: RO
		uint32 preAggPolicy : 2; //no description, reset value: 0x0, access type: RO
		uint32 preAggLegacyPsEn : 1; //no description, reset value: 0x0, access type: RO
		uint32 preAggHeMuOnly : 1; //no description, reset value: 0x0, access type: RO
		uint32 preAggOmiControl : 2; //no description, reset value: 0x0, access type: RO
		uint32 preAggStaNum : 8; //no description, reset value: 0x0, access type: RO
		uint32 preAggTidEn : 9; //no description, reset value: 0x0, access type: RO
		uint32 preAggMaxSelTidNum : 4; //no description, reset value: 0x0, access type: RO
		uint32 preAggPrimaryTid : 3; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegTxSelectorTxSelPreAggSelReqBits0_u;

/*REG_TX_SELECTOR_TX_SEL_PRE_AGG_SEL_REQ_BITS1 0x384 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 preAggTidInRetry : 9; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 3;
		uint32 preAggHwLockedTid : 9; //no description, reset value: 0x0, access type: RO
		uint32 reserved1 : 1;
		uint32 validateDlHeMuEn : 1; //no description, reset value: 0x0, access type: RO
		uint32 preAggDlHePsWithoutReqEn : 1; //no description, reset value: 0x0, access type: RO
		uint32 preAggLockSelectedMode : 2; //no description, reset value: 0x0, access type: RO
		uint32 reserved2 : 1;
		uint32 preAggVapIdx : 5; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegTxSelectorTxSelPreAggSelReqBits1_u;

/*REG_TX_SELECTOR_TX_SEL_PRE_AGG_SEL_RESULT_BITS0 0x388 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 preAggResultSelectedTids : 9; //no description, reset value: 0x0, access type: RO
		uint32 preAggResultSelectorLockedTids : 9; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 2;
		uint32 preAggStaNotSelectedReason : 11; //no description, reset value: 0x0, access type: RO
		uint32 reserved1 : 1;
	} bitFields;
} RegTxSelectorTxSelPreAggSelResultBits0_u;

/*REG_TX_SELECTOR_TX_SEL_PRE_AGG_SEL_RESULT_BITS1 0x38C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 preAggSelStatus031 : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegTxSelectorTxSelPreAggSelResultBits1_u;

/*REG_TX_SELECTOR_TX_SEL_PRE_AGG_SEL_RESULT_BITS2 0x390 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 preAggUlStaTidDataInQ : 8; //no description, reset value: 0x0, access type: RO
		uint32 preAggTwtAvail : 1; //no description, reset value: 0x0, access type: RO
		uint32 preAggTwtSp : 1; //no description, reset value: 0x0, access type: RO
		uint32 preAggTwtAnn : 1; //no description, reset value: 0x0, access type: RO
		uint32 preAggOmiValid : 1; //no description, reset value: 0x0, access type: RO
		uint32 preAggOmiSupp : 1; //no description, reset value: 0x0, access type: RO
		uint32 preAggTomiDis : 1; //no description, reset value: 0x0, access type: RO
		uint32 preAggOmiBw : 2; //no description, reset value: 0x0, access type: RO
		uint32 preAggTomiMaxNss : 3; //no description, reset value: 0x0, access type: RO
		uint32 preAggRomiMaxNss : 3; //no description, reset value: 0x0, access type: RO
		uint32 preAggInsertAControl : 1; //no description, reset value: 0x0, access type: RO
		uint32 preAggUlHeMuEn : 1; //no description, reset value: 0x0, access type: RO
		uint32 preAggSelStatus3235 : 4; //no description, reset value: 0x0, access type: RO
		uint32 preAggSpValidInVap : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 3;
	} bitFields;
} RegTxSelectorTxSelPreAggSelResultBits2_u;



#endif // _TX_SELECTOR_REGS_H_
