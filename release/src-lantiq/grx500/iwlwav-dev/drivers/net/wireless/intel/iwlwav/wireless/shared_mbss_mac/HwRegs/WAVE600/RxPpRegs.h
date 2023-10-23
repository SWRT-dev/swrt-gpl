
/***********************************************************************************
File:				RxPpRegs.h
Module:				rxPp
SOC Revision:		
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _RX_PP_REGS_H_
#define _RX_PP_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define RX_PP_BASE_ADDRESS                             MEMORY_MAP_UNIT_47_BASE_ADDRESS
#define	REG_RX_PP_SW_INST_CTL                (RX_PP_BASE_ADDRESS + 0x0)
#define	REG_RX_PP_SW_INST_ADDR               (RX_PP_BASE_ADDRESS + 0x4)
#define	REG_RX_PP_SW_HALT                    (RX_PP_BASE_ADDRESS + 0x8)
#define	REG_RX_PP_MPDU_IN_LIST               (RX_PP_BASE_ADDRESS + 0xC)
#define	REG_RX_PP_ERR_LIST                   (RX_PP_BASE_ADDRESS + 0x10)
#define	REG_RX_PP_UMAC_DONE_LIST             (RX_PP_BASE_ADDRESS + 0x14)
#define	REG_RX_PP_LIBERATOR_LIST             (RX_PP_BASE_ADDRESS + 0x18)
#define	REG_RX_PP_RD_TYPE0_FORWARD_LIST      (RX_PP_BASE_ADDRESS + 0x1C)
#define	REG_RX_PP_RD_TYPE1_FORWARD_LIST      (RX_PP_BASE_ADDRESS + 0x20)
#define	REG_RX_PP_RD_TYPE2_FORWARD_LIST      (RX_PP_BASE_ADDRESS + 0x24)
#define	REG_RX_PP_RD_TYPE3_FORWARD_LIST      (RX_PP_BASE_ADDRESS + 0x28)
#define	REG_RX_PP_RD_TYPE4_FORWARD_LIST      (RX_PP_BASE_ADDRESS + 0x2C)
#define	REG_RX_PP_RD_TYPE5_FORWARD_LIST      (RX_PP_BASE_ADDRESS + 0x30)
#define	REG_RX_PP_RD_TYPE6_FORWARD_LIST      (RX_PP_BASE_ADDRESS + 0x34)
#define	REG_RX_PP_RD_TYPE7_FORWARD_LIST      (RX_PP_BASE_ADDRESS + 0x38)
#define	REG_RX_PP_RD_TYPE8_FORWARD_LIST      (RX_PP_BASE_ADDRESS + 0x3C)
#define	REG_RX_PP_RD_TYPE9_FORWARD_LIST      (RX_PP_BASE_ADDRESS + 0x40)
#define	REG_RX_PP_RD_TYPE10_FORWARD_LIST     (RX_PP_BASE_ADDRESS + 0x44)
#define	REG_RX_PP_RD_TYPE11_FORWARD_LIST     (RX_PP_BASE_ADDRESS + 0x48)
#define	REG_RX_PP_RD_TYPE12_FORWARD_LIST     (RX_PP_BASE_ADDRESS + 0x4C)
#define	REG_RX_PP_RD_TYPE13_FORWARD_LIST     (RX_PP_BASE_ADDRESS + 0x50)
#define	REG_RX_PP_RD_TYPE14_FORWARD_LIST     (RX_PP_BASE_ADDRESS + 0x54)
#define	REG_RX_PP_RD_TYPE15_FORWARD_LIST     (RX_PP_BASE_ADDRESS + 0x58)
#define	REG_RX_PP_RD_TYPE16_FORWARD_LIST     (RX_PP_BASE_ADDRESS + 0x5C)
#define	REG_RX_PP_RD_TYPE17_FORWARD_LIST     (RX_PP_BASE_ADDRESS + 0x60)
#define	REG_RX_PP_RD_TYPE18_FORWARD_LIST     (RX_PP_BASE_ADDRESS + 0x64)
#define	REG_RX_PP_RD_TYPE19_FORWARD_LIST     (RX_PP_BASE_ADDRESS + 0x68)
#define	REG_RX_PP_RD_TYPE20_FORWARD_LIST     (RX_PP_BASE_ADDRESS + 0x6C)
#define	REG_RX_PP_RD_TYPE21_FORWARD_LIST     (RX_PP_BASE_ADDRESS + 0x70)
#define	REG_RX_PP_RD_TYPE22_FORWARD_LIST     (RX_PP_BASE_ADDRESS + 0x74)
#define	REG_RX_PP_RD_TYPE23_FORWARD_LIST     (RX_PP_BASE_ADDRESS + 0x78)
#define	REG_RX_PP_RD_TYPE24_FORWARD_LIST     (RX_PP_BASE_ADDRESS + 0x7C)
#define	REG_RX_PP_RD_TYPE25_FORWARD_LIST     (RX_PP_BASE_ADDRESS + 0x80)
#define	REG_RX_PP_RD_TYPE26_FORWARD_LIST     (RX_PP_BASE_ADDRESS + 0x84)
#define	REG_RX_PP_RD_TYPE27_FORWARD_LIST     (RX_PP_BASE_ADDRESS + 0x88)
#define	REG_RX_PP_RD_TYPE28_FORWARD_LIST     (RX_PP_BASE_ADDRESS + 0x8C)
#define	REG_RX_PP_RD_TYPE29_FORWARD_LIST     (RX_PP_BASE_ADDRESS + 0x90)
#define	REG_RX_PP_RD_TYPE30_FORWARD_LIST     (RX_PP_BASE_ADDRESS + 0x94)
#define	REG_RX_PP_RD_TYPE31_FORWARD_LIST     (RX_PP_BASE_ADDRESS + 0x98)
#define	REG_RX_PP_RD_TYPE_SKIP_CFG           (RX_PP_BASE_ADDRESS + 0xA0)
#define	REG_RX_PP_RD_TYPE_SEC_SKIP_CFG       (RX_PP_BASE_ADDRESS + 0xA4)
#define	REG_RX_PP_RD_TYPE_UMAC_CFG           (RX_PP_BASE_ADDRESS + 0xA8)
#define	REG_RX_PP_SHRAM_FIFO_CFG             (RX_PP_BASE_ADDRESS + 0xAC)
#define	REG_RX_PP_SHRAM_FIFO_BASE_ADD        (RX_PP_BASE_ADDRESS + 0xB0)
#define	REG_RX_PP_SHRAM_FIFO_SUB             (RX_PP_BASE_ADDRESS + 0xB4)
#define	REG_RX_PP_SHRAM_FIFO_STATUS          (RX_PP_BASE_ADDRESS + 0xB8)
#define	REG_RX_PP_SHRAM_FIFO_CLR             (RX_PP_BASE_ADDRESS + 0xBC)
#define	REG_RX_PP_CONTROL_BACKDOOR_UPDATE    (RX_PP_BASE_ADDRESS + 0xC0)
#define	REG_RX_PP_RX_PP_SPARE                (RX_PP_BASE_ADDRESS + 0xC4)
#define	REG_RX_PP_RX_COUNTERS_CFG            (RX_PP_BASE_ADDRESS + 0xC8)
#define	REG_RX_PP_RX_LOGGER_CFG              (RX_PP_BASE_ADDRESS + 0xCC)
#define	REG_RX_PP_RX_LOGGER_BUSY             (RX_PP_BASE_ADDRESS + 0xD0)
#define	REG_RX_PP_SW_REKEY_EN                (RX_PP_BASE_ADDRESS + 0xD4)
#define	REG_RX_PP_SW_REKEY_INST_LSB          (RX_PP_BASE_ADDRESS + 0xD8)
#define	REG_RX_PP_SW_REKEY_INST_MSB          (RX_PP_BASE_ADDRESS + 0xDC)
#define	REG_RX_PP_FRAG_DIS_LIB_ERR_LIST      (RX_PP_BASE_ADDRESS + 0xE0)
#define	REG_RX_PP_SM_STATUS_PART1            (RX_PP_BASE_ADDRESS + 0xE4)
#define	REG_RX_PP_SM_STATUS_PART2            (RX_PP_BASE_ADDRESS + 0xE8)
#define	REG_RX_PP_MGMT_TID_CFG               (RX_PP_BASE_ADDRESS + 0xEC)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_RX_PP_SW_INST_CTL 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 baWindowSize : 9; //BA window size, reset value: 0x0, access type: RW
		uint32 statusBitValue : 1; //no description, reset value: 0x0, access type: RW
		uint32 fragEnValue : 1; //no description, reset value: 0x0, access type: RW
		uint32 fragEnUpdate : 1; //Frag_en bit update. Relvant for BAA only, reset value: 0x0, access type: RW
		uint32 newSn : 12; //New SN value. Valid when sn_update is asserted, reset value: 0x0, access type: RW
		uint32 clearDest : 1; //Deciding where to send SNs in list when clearing it , reset value: 0x0, access type: RW
		uint32 noBaForce : 1; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 1;
		uint32 mulTids : 1; //no description, reset value: 0x0, access type: RW
		uint32 snUpdate : 1; //New SN update, reset value: 0x0, access type: RW
		uint32 winSizeUpdate : 1; //Window size update, reset value: 0x0, access type: RW
		uint32 flush : 1; //Flushing all SNs from list, reset value: 0x0, access type: RW
		uint32 statusBitUpdate : 1; //no description, reset value: 0x0, access type: RW
	} bitFields;
} RegRxPpSwInstCtl_u;

/*REG_RX_PP_SW_INST_ADDR 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tid : 4; //TID address. Valid values are 0-8, reset value: 0x0, access type: RW
		uint32 sta : 8; //STA address. Valid values are 0-127, reset value: 0x0, access type: RW
		uint32 swInstDone : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 1;
		uint32 mulTidsEn : 9; //no description, reset value: 0x0, access type: RW
		uint32 mulTidsFragEnValue : 9; //no description, reset value: 0x0, access type: RW
	} bitFields;
} RegRxPpSwInstAddr_u;

/*REG_RX_PP_SW_HALT 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swHalt : 1; //Halts the block from moving to the next MPDU, reset value: 0x1, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegRxPpSwHalt_u;

/*REG_RX_PP_MPDU_IN_LIST 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mpduInListIdx : 6; //MPDU list index number at the RX lists DLM, reset value: 0x11, access type: RW
		uint32 reserved0 : 26;
	} bitFields;
} RegRxPpMpduInList_u;

/*REG_RX_PP_ERR_LIST 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 errListIdx : 6; //Host IF list index number at the RX lists DLM, reset value: 0x0, access type: RW
		uint32 reserved0 : 26;
	} bitFields;
} RegRxPpErrList_u;

/*REG_RX_PP_UMAC_DONE_LIST 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 umacDoneListIdx : 6; //UMAC done list index number at the RX lists DLM, reset value: 0x12, access type: RW
		uint32 reserved0 : 26;
	} bitFields;
} RegRxPpUmacDoneList_u;

/*REG_RX_PP_LIBERATOR_LIST 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 liberatorListIdx : 6; //Drop src0 Host IF list index number at the RX lists DLM, reset value: 0x0, access type: RW
		uint32 reserved0 : 26;
	} bitFields;
} RegRxPpLiberatorList_u;

/*REG_RX_PP_RD_TYPE0_FORWARD_LIST 0x1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rdType0ListIdx : 6; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 26;
	} bitFields;
} RegRxPpRdType0ForwardList_u;

/*REG_RX_PP_RD_TYPE1_FORWARD_LIST 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rdType1ListIdx : 6; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 26;
	} bitFields;
} RegRxPpRdType1ForwardList_u;

/*REG_RX_PP_RD_TYPE2_FORWARD_LIST 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rdType2ListIdx : 6; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 26;
	} bitFields;
} RegRxPpRdType2ForwardList_u;

/*REG_RX_PP_RD_TYPE3_FORWARD_LIST 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rdType3ListIdx : 6; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 26;
	} bitFields;
} RegRxPpRdType3ForwardList_u;

/*REG_RX_PP_RD_TYPE4_FORWARD_LIST 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rdType4ListIdx : 6; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 26;
	} bitFields;
} RegRxPpRdType4ForwardList_u;

/*REG_RX_PP_RD_TYPE5_FORWARD_LIST 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rdType5ListIdx : 6; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 26;
	} bitFields;
} RegRxPpRdType5ForwardList_u;

/*REG_RX_PP_RD_TYPE6_FORWARD_LIST 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rdType6ListIdx : 6; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 26;
	} bitFields;
} RegRxPpRdType6ForwardList_u;

/*REG_RX_PP_RD_TYPE7_FORWARD_LIST 0x38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rdType7ListIdx : 6; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 26;
	} bitFields;
} RegRxPpRdType7ForwardList_u;

/*REG_RX_PP_RD_TYPE8_FORWARD_LIST 0x3C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rdType8ListIdx : 6; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 26;
	} bitFields;
} RegRxPpRdType8ForwardList_u;

/*REG_RX_PP_RD_TYPE9_FORWARD_LIST 0x40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rdType9ListIdx : 6; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 26;
	} bitFields;
} RegRxPpRdType9ForwardList_u;

/*REG_RX_PP_RD_TYPE10_FORWARD_LIST 0x44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rdType10ListIdx : 6; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 26;
	} bitFields;
} RegRxPpRdType10ForwardList_u;

/*REG_RX_PP_RD_TYPE11_FORWARD_LIST 0x48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rdType11ListIdx : 6; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 26;
	} bitFields;
} RegRxPpRdType11ForwardList_u;

/*REG_RX_PP_RD_TYPE12_FORWARD_LIST 0x4C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rdType12ListIdx : 6; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 26;
	} bitFields;
} RegRxPpRdType12ForwardList_u;

/*REG_RX_PP_RD_TYPE13_FORWARD_LIST 0x50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rdType13ListIdx : 6; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 26;
	} bitFields;
} RegRxPpRdType13ForwardList_u;

/*REG_RX_PP_RD_TYPE14_FORWARD_LIST 0x54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rdType14ListIdx : 6; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 26;
	} bitFields;
} RegRxPpRdType14ForwardList_u;

/*REG_RX_PP_RD_TYPE15_FORWARD_LIST 0x58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rdType15ListIdx : 6; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 26;
	} bitFields;
} RegRxPpRdType15ForwardList_u;

/*REG_RX_PP_RD_TYPE16_FORWARD_LIST 0x5C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rdType16ListIdx : 6; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 26;
	} bitFields;
} RegRxPpRdType16ForwardList_u;

/*REG_RX_PP_RD_TYPE17_FORWARD_LIST 0x60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rdType17ListIdx : 6; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 26;
	} bitFields;
} RegRxPpRdType17ForwardList_u;

/*REG_RX_PP_RD_TYPE18_FORWARD_LIST 0x64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rdType18ListIdx : 6; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 26;
	} bitFields;
} RegRxPpRdType18ForwardList_u;

/*REG_RX_PP_RD_TYPE19_FORWARD_LIST 0x68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rdType19ListIdx : 6; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 26;
	} bitFields;
} RegRxPpRdType19ForwardList_u;

/*REG_RX_PP_RD_TYPE20_FORWARD_LIST 0x6C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rdType20ListIdx : 6; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 26;
	} bitFields;
} RegRxPpRdType20ForwardList_u;

/*REG_RX_PP_RD_TYPE21_FORWARD_LIST 0x70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rdType21ListIdx : 6; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 26;
	} bitFields;
} RegRxPpRdType21ForwardList_u;

/*REG_RX_PP_RD_TYPE22_FORWARD_LIST 0x74 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rdType22ListIdx : 6; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 26;
	} bitFields;
} RegRxPpRdType22ForwardList_u;

/*REG_RX_PP_RD_TYPE23_FORWARD_LIST 0x78 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rdType23ListIdx : 6; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 26;
	} bitFields;
} RegRxPpRdType23ForwardList_u;

/*REG_RX_PP_RD_TYPE24_FORWARD_LIST 0x7C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rdType24ListIdx : 6; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 26;
	} bitFields;
} RegRxPpRdType24ForwardList_u;

/*REG_RX_PP_RD_TYPE25_FORWARD_LIST 0x80 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rdType25ListIdx : 6; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 26;
	} bitFields;
} RegRxPpRdType25ForwardList_u;

/*REG_RX_PP_RD_TYPE26_FORWARD_LIST 0x84 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rdType26ListIdx : 6; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 26;
	} bitFields;
} RegRxPpRdType26ForwardList_u;

/*REG_RX_PP_RD_TYPE27_FORWARD_LIST 0x88 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rdType27ListIdx : 6; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 26;
	} bitFields;
} RegRxPpRdType27ForwardList_u;

/*REG_RX_PP_RD_TYPE28_FORWARD_LIST 0x8C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rdType28ListIdx : 6; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 26;
	} bitFields;
} RegRxPpRdType28ForwardList_u;

/*REG_RX_PP_RD_TYPE29_FORWARD_LIST 0x90 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rdType29ListIdx : 6; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 26;
	} bitFields;
} RegRxPpRdType29ForwardList_u;

/*REG_RX_PP_RD_TYPE30_FORWARD_LIST 0x94 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rdType30ListIdx : 6; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 26;
	} bitFields;
} RegRxPpRdType30ForwardList_u;

/*REG_RX_PP_RD_TYPE31_FORWARD_LIST 0x98 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rdType31ListIdx : 6; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 26;
	} bitFields;
} RegRxPpRdType31ForwardList_u;

/*REG_RX_PP_RD_TYPE_SKIP_CFG 0xA0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 skipBit : 32; //no description, reset value: 0xFFFFFFFF, access type: RW
	} bitFields;
} RegRxPpRdTypeSkipCfg_u;

/*REG_RX_PP_RD_TYPE_SEC_SKIP_CFG 0xA4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 secSkipBit : 32; //no description, reset value: 0xFFFFFFFF, access type: RW
	} bitFields;
} RegRxPpRdTypeSecSkipCfg_u;

/*REG_RX_PP_RD_TYPE_UMAC_CFG 0xA8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 umacBit : 32; //no description, reset value: 0xFFFFFFFF, access type: RW
	} bitFields;
} RegRxPpRdTypeUmacCfg_u;

/*REG_RX_PP_SHRAM_FIFO_CFG 0xAC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fifoLineSize : 10; //no description, reset value: 0x100, access type: RW
		uint32 reserved0 : 21;
		uint32 shramFifoEn : 1; //no description, reset value: 0x1, access type: RW
	} bitFields;
} RegRxPpShramFifoCfg_u;

/*REG_RX_PP_SHRAM_FIFO_BASE_ADD 0xB0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 shramFifoBaseAddr : 22; //SHRAM Frag FIFO base address - Byte aligned, reset value: 0x0, access type: RW
		uint32 reserved0 : 10;
	} bitFields;
} RegRxPpShramFifoBaseAdd_u;

/*REG_RX_PP_SHRAM_FIFO_SUB 0xB4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 shramFifoSub : 10; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 22;
	} bitFields;
} RegRxPpShramFifoSub_u;

/*REG_RX_PP_SHRAM_FIFO_STATUS 0xB8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 shramFifoLinesTaken : 10; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 2;
		uint32 shramFifoFull : 1; //no description, reset value: 0x0, access type: RO
		uint32 shramFifoEmpty : 1; //no description, reset value: 0x1, access type: RO
		uint32 shramFifoOverflow : 1; //no description, reset value: 0x0, access type: RO
		uint32 shramFifoUnderflow : 1; //no description, reset value: 0x0, access type: RO
		uint32 shramSynFifoBytesInFifo : 2; //no description, reset value: 0x0, access type: RO
		uint32 reserved1 : 2;
		uint32 shramSynFifoFull : 1; //no description, reset value: 0x0, access type: RO
		uint32 shramSynFifoEmpty : 1; //no description, reset value: 0x1, access type: RO
		uint32 shramSynFifoPopWhileEmpty : 1; //no description, reset value: 0x0, access type: RO
		uint32 shramSynFifoPushWhileFull : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved2 : 8;
	} bitFields;
} RegRxPpShramFifoStatus_u;

/*REG_RX_PP_SHRAM_FIFO_CLR 0xBC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 shramFifoClear : 1; //no description, reset value: 0x0, access type: WO
		uint32 reserved0 : 31;
	} bitFields;
} RegRxPpShramFifoClr_u;

/*REG_RX_PP_CONTROL_BACKDOOR_UPDATE 0xC0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 controlSnNumValid : 1; //SN num valid bit value, reset value: 0x0, access type: RW
		uint32 controlPnNumValid : 1; //PN num valid bit value, reset value: 0x0, access type: RW
		uint32 controlFragEn : 1; //Frag_en bit. Used in BAA only, reset value: 0x0, access type: RW
		uint32 controlHighestSnNumValid : 1; //Highest SN in window valid, reset value: 0x0, access type: RW
		uint32 controlBe : 4; //Valid bits be, reset value: 0x0, access type: RW
		uint32 controlUpdateDone : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 23;
	} bitFields;
} RegRxPpControlBackdoorUpdate_u;

/*REG_RX_PP_RX_PP_SPARE 0xC4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxPpSpare : 32; //no description, reset value: 0x0, access type: RW
	} bitFields;
} RegRxPpRxPpSpare_u;

/*REG_RX_PP_RX_COUNTERS_CFG 0xC8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 qosRxEn : 1; //no description, reset value: 0x0, access type: RW
		uint32 rdCountEn : 1; //no description, reset value: 0x0, access type: RW
		uint32 rdDelayedCountEn : 1; //no description, reset value: 0x0, access type: RW
		uint32 rdSwDropCountEn : 1; //no description, reset value: 0x0, access type: RW
		uint32 rdDuplicateDropCountEn : 1; //no description, reset value: 0x0, access type: RW
		uint32 missingSnSwUpdateCountEn : 1; //no description, reset value: 0x0, access type: RW
		uint32 mpduUniOrMgmtEn : 1; //no description, reset value: 0x0, access type: RW
		uint32 mpduRetryEn : 1; //no description, reset value: 0x0, access type: RW
		uint32 amsduCountEn : 1; //no description, reset value: 0x0, access type: RW
		uint32 dropMpduEn : 1; //no description, reset value: 0x0, access type: RW
		uint32 mpduTypeNotSupportedEn : 1; //no description, reset value: 0x0, access type: RW
		uint32 barMpduCountEn : 1; //no description, reset value: 0x0, access type: RW
		uint32 replayEn : 1; //no description, reset value: 0x0, access type: RW
		uint32 replayMgmtEn : 1; //no description, reset value: 0x0, access type: RW
		uint32 tkipCountEn : 1; //no description, reset value: 0x0, access type: RW
		uint32 failureCountEn : 1; //no description, reset value: 0x0, access type: RW
		uint32 rxClassAmsduBytesStbEn : 1; //no description, reset value: 0x0, access type: RW
		uint32 rxcAmpduStbEn : 1; //no description, reset value: 0x0, access type: RW
		uint32 rxClassMpduInAmpduStbEn : 1; //no description, reset value: 0x0, access type: RW
		uint32 rxClassOctetMpduStbEn : 1; //no description, reset value: 0x0, access type: RW
		uint32 rxClassDropStbEn : 1; //no description, reset value: 0x0, access type: RW
		uint32 rxClassSecurMisStbEn : 1; //no description, reset value: 0x0, access type: RW
		uint32 rxcCrcErrorStbEn : 1; //no description, reset value: 0x0, access type: RW
		uint32 delineatorCrcErrorStbEn : 1; //no description, reset value: 0x0, access type: RW
		uint32 bcMcCountEn : 1; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 5;
		uint32 qosRxDecEn : 1; //no description, reset value: 0x0, access type: RW
		uint32 rxCountCleanFifo : 1; //no description, reset value: 0x0, access type: WO
	} bitFields;
} RegRxPpRxCountersCfg_u;

/*REG_RX_PP_RX_LOGGER_CFG 0xCC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxPpLoggerPriority : 2; //no description, reset value: 0x0, access type: RW
		uint32 rxPpLoggerEn : 1; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 29;
	} bitFields;
} RegRxPpRxLoggerCfg_u;

/*REG_RX_PP_RX_LOGGER_BUSY 0xD0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxPpLoggerBusy : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegRxPpRxLoggerBusy_u;

/*REG_RX_PP_SW_REKEY_EN 0xD4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rekeyEn : 1; //ReKey feature enable, reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegRxPpSwRekeyEn_u;

/*REG_RX_PP_SW_REKEY_INST_LSB 0xD8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rekeySta : 8; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 4;
		uint32 rekeyKey : 2; //no description, reset value: 0x0, access type: RW
		uint32 rekeyMgmtTidEn : 1; //no description, reset value: 0x0, access type: RW
		uint32 rekeyDone : 1; //Done indication of ReKey sequence activation., reset value: 0x0, access type: RO
		uint32 rekeyPn015 : 16; //LSB ReKey PN, reset value: 0x0, access type: RW
	} bitFields;
} RegRxPpSwRekeyInstLsb_u;

/*REG_RX_PP_SW_REKEY_INST_MSB 0xDC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rekeyPn1647 : 32; //MSB ReKey PN , reset value: 0x0, access type: RW
	} bitFields;
} RegRxPpSwRekeyInstMsb_u;

/*REG_RX_PP_FRAG_DIS_LIB_ERR_LIST 0xE0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fragDisLibErrList : 1; //When fragment is disabled for the STA-TID it is dropped to: , 0x0: Liberator , 0x1: Error list, reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegRxPpFragDisLibErrList_u;

/*REG_RX_PP_SM_STATUS_PART1 0xE4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 clearMpduSm : 6; //no description, reset value: 0x0, access type: RO
		uint32 oooDigMpduSm : 5; //no description, reset value: 0x0, access type: RO
		uint32 ssnClearMpduClearSm : 5; //no description, reset value: 0x0, access type: RO
		uint32 impMpduClearSm : 3; //no description, reset value: 0x0, access type: RO
		uint32 noBaaClearSm : 5; //no description, reset value: 0x0, access type: RO
		uint32 mainMpduSm : 4; //no description, reset value: 0x0, access type: RO
		uint32 rekeyHitSm : 4; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegRxPpSmStatusPart1_u;

/*REG_RX_PP_SM_STATUS_PART2 0xE8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 statusUpdateSm : 2; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 2;
		uint32 setRekeySm : 2; //no description, reset value: 0x0, access type: RO
		uint32 reserved1 : 2;
		uint32 fragEnUpdateSm : 2; //no description, reset value: 0x0, access type: RO
		uint32 reserved2 : 2;
		uint32 flushSm : 4; //no description, reset value: 0x0, access type: RO
		uint32 windowSizeSm : 3; //no description, reset value: 0x0, access type: RO
		uint32 reserved3 : 1;
		uint32 snUpdateSm : 4; //no description, reset value: 0x0, access type: RO
		uint32 loggerSm : 3; //no description, reset value: 0x0, access type: RO
		uint32 reserved4 : 5;
	} bitFields;
} RegRxPpSmStatusPart2_u;

/*REG_RX_PP_MGMT_TID_CFG 0xEC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mgmtTidCfg : 32; //no description, reset value: 0xFFFBFFFE, access type: RW
	} bitFields;
} RegRxPpMgmtTidCfg_u;



#endif // _RX_PP_REGS_H_
