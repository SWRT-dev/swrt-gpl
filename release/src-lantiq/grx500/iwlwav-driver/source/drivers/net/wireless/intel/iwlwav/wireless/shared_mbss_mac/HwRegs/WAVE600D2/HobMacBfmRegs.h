
/***********************************************************************************
File:				HobMacBfmRegs.h
Module:				hobMacBfm
SOC Revision:		
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _HOB_MAC_BFM_REGS_H_
#define _HOB_MAC_BFM_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define HOB_MAC_BFM_BASE_ADDRESS                             MEMORY_MAP_UNIT_501_BASE_ADDRESS
#define	REG_HOB_MAC_BFM_BFEE_STA                      (HOB_MAC_BFM_BASE_ADDRESS + 0x0)
#define	REG_HOB_MAC_BFM_LENGTH_WITH_DELIMITERS        (HOB_MAC_BFM_BASE_ADDRESS + 0x4)
#define	REG_HOB_MAC_BFM_LENGTH_WITHOUT_DELIMITERS     (HOB_MAC_BFM_BASE_ADDRESS + 0x8)
#define	REG_HOB_MAC_BFM_RD_BF_RPT_VAP_TID0_DB_ADDR    (HOB_MAC_BFM_BASE_ADDRESS + 0xC)
#define	REG_HOB_MAC_BFM_BFEE_PHY_MODE                 (HOB_MAC_BFM_BASE_ADDRESS + 0x10)
#define	REG_HOB_MAC_BFM_RXC_PHY_NDP_BW_SMP            (HOB_MAC_BFM_BASE_ADDRESS + 0x14)
#define	REG_HOB_MAC_BFM_TX_RECIPE_BASE_ADDR           (HOB_MAC_BFM_BASE_ADDRESS + 0x18)
#define	REG_HOB_MAC_BFM_BFEE_TX_NUM_OF_MPDUS_TF       (HOB_MAC_BFM_BASE_ADDRESS + 0x1C)
#define	REG_HOB_MAC_BFM_BFEE_TX_MPDUS_VECTOR_TF       (HOB_MAC_BFM_BASE_ADDRESS + 0x20)
#define	REG_HOB_MAC_BFM_HOB_REAL_AMPDU_LENGTH         (HOB_MAC_BFM_BASE_ADDRESS + 0x24)
#define	REG_HOB_MAC_BFM_TXC_SECTION_A_BYTE_LEN        (HOB_MAC_BFM_BASE_ADDRESS + 0x28)
#define	REG_HOB_MAC_BFM_TXC_SECTION_B_BYTE_LEN        (HOB_MAC_BFM_BASE_ADDRESS + 0x2C)
#define	REG_HOB_MAC_BFM_TXC_SECTION_A_POINTER         (HOB_MAC_BFM_BASE_ADDRESS + 0x30)
#define	REG_HOB_MAC_BFM_TXC_SECTION_B_POINTER         (HOB_MAC_BFM_BASE_ADDRESS + 0x34)
#define	REG_HOB_MAC_BFM_ACTION_AND_CATEGORY           (HOB_MAC_BFM_BASE_ADDRESS + 0x38)
#define	REG_HOB_MAC_BFM_BFEE_GEN_FCS_VALID            (HOB_MAC_BFM_BASE_ADDRESS + 0x3C)
#define	REG_HOB_MAC_BFM_BFEE_RESPONSE_TYPE_TF         (HOB_MAC_BFM_BASE_ADDRESS + 0x40)
#define	REG_HOB_MAC_BFM_BFEE_INVALID_TX_DATA          (HOB_MAC_BFM_BASE_ADDRESS + 0x44)
#define	REG_HOB_MAC_BFM_BFEE_SM                       (HOB_MAC_BFM_BASE_ADDRESS + 0x4C)
#define	REG_HOB_MAC_BFM_BFEE_MIMO_CONTROL_0           (HOB_MAC_BFM_BASE_ADDRESS + 0x50)
#define	REG_HOB_MAC_BFM_BFEE_MIMO_CONTROL_1           (HOB_MAC_BFM_BASE_ADDRESS + 0x54)
#define	REG_HOB_MAC_BFM_BFEE_MIMO_CONTROL_2           (HOB_MAC_BFM_BASE_ADDRESS + 0x58)
#define	REG_HOB_MAC_BFM_SECTION_C_BASE_ADDR_0         (HOB_MAC_BFM_BASE_ADDRESS + 0x60)
#define	REG_HOB_MAC_BFM_SECTION_C_LENGTH_0            (HOB_MAC_BFM_BASE_ADDRESS + 0x64)
#define	REG_HOB_MAC_BFM_SECTION_D_BASE_ADDR_0         (HOB_MAC_BFM_BASE_ADDRESS + 0x68)
#define	REG_HOB_MAC_BFM_SECTION_D_LENGTH_0            (HOB_MAC_BFM_BASE_ADDRESS + 0x6C)
#define	REG_HOB_MAC_BFM_SECTION_C_BASE_ADDR_1         (HOB_MAC_BFM_BASE_ADDRESS + 0x70)
#define	REG_HOB_MAC_BFM_SECTION_C_LENGTH_1            (HOB_MAC_BFM_BASE_ADDRESS + 0x74)
#define	REG_HOB_MAC_BFM_SECTION_D_BASE_ADDR_1         (HOB_MAC_BFM_BASE_ADDRESS + 0x78)
#define	REG_HOB_MAC_BFM_SECTION_D_LENGTH_1            (HOB_MAC_BFM_BASE_ADDRESS + 0x7C)
#define	REG_HOB_MAC_BFM_SECTION_C_BASE_ADDR_2         (HOB_MAC_BFM_BASE_ADDRESS + 0x80)
#define	REG_HOB_MAC_BFM_SECTION_C_LENGTH_2            (HOB_MAC_BFM_BASE_ADDRESS + 0x84)
#define	REG_HOB_MAC_BFM_SECTION_D_BASE_ADDR_2         (HOB_MAC_BFM_BASE_ADDRESS + 0x88)
#define	REG_HOB_MAC_BFM_SECTION_D_LENGTH_2            (HOB_MAC_BFM_BASE_ADDRESS + 0x8C)
#define	REG_HOB_MAC_BFM_BFEE_MPDU_LENGTH_0            (HOB_MAC_BFM_BASE_ADDRESS + 0xB8)
#define	REG_HOB_MAC_BFM_BFEE_MPDU_LENGTH_1            (HOB_MAC_BFM_BASE_ADDRESS + 0xBC)
#define	REG_HOB_MAC_BFM_BFEE_MPDU_LENGTH_2            (HOB_MAC_BFM_BASE_ADDRESS + 0xC0)
#define	REG_HOB_MAC_BFM_BFEE_STATIC_INDIACTIONS       (HOB_MAC_BFM_BASE_ADDRESS + 0xD0)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_HOB_MAC_BFM_BFEE_STA 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxStaIdxUsr0Smp : 9; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 7;
		uint32 rxcVapIdxUsr0Smp : 5; //no description, reset value: 0x0, access type: RO
		uint32 reserved1 : 11;
	} bitFields;
} RegHobMacBfmBfeeSta_u;

/*REG_HOB_MAC_BFM_LENGTH_WITH_DELIMITERS 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lengthWithDelimiters : 15; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 17;
	} bitFields;
} RegHobMacBfmLengthWithDelimiters_u;

/*REG_HOB_MAC_BFM_LENGTH_WITHOUT_DELIMITERS 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lengthWithoutDelimiters : 15; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 17;
	} bitFields;
} RegHobMacBfmLengthWithoutDelimiters_u;

/*REG_HOB_MAC_BFM_RD_BF_RPT_VAP_TID0_DB_ADDR 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rdBfRptVapTid0DbAddr : 25; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 7;
	} bitFields;
} RegHobMacBfmRdBfRptVapTid0DbAddr_u;

/*REG_HOB_MAC_BFM_BFEE_PHY_MODE 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mpPhyModeIsHt : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 3;
		uint32 mpPhyMode : 2; //no description, reset value: 0x0, access type: RO
		uint32 reserved1 : 2;
		uint32 txPhyMode : 3; //no description, reset value: 0x0, access type: RO
		uint32 reserved2 : 21;
	} bitFields;
} RegHobMacBfmBfeePhyMode_u;

/*REG_HOB_MAC_BFM_RXC_PHY_NDP_BW_SMP 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxcPhyNdpBwSmp : 2; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 30;
	} bitFields;
} RegHobMacBfmRxcPhyNdpBwSmp_u;

/*REG_HOB_MAC_BFM_TX_RECIPE_BASE_ADDR 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txTcrBaseAddr : 25; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 7;
	} bitFields;
} RegHobMacBfmTxRecipeBaseAddr_u;

/*REG_HOB_MAC_BFM_BFEE_TX_NUM_OF_MPDUS_TF 0x1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfeeTxNumOfMpdusTf : 2; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 30;
	} bitFields;
} RegHobMacBfmBfeeTxNumOfMpdusTf_u;

/*REG_HOB_MAC_BFM_BFEE_TX_MPDUS_VECTOR_TF 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfeeTxMpdusVectorTf : 3; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 29;
	} bitFields;
} RegHobMacBfmBfeeTxMpdusVectorTf_u;

/*REG_HOB_MAC_BFM_HOB_REAL_AMPDU_LENGTH 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hobRealAmpduLength : 15; //Length of current AMPDU, reset value: 0x0, access type: RO
		uint32 reserved0 : 17;
	} bitFields;
} RegHobMacBfmHobRealAmpduLength_u;

/*REG_HOB_MAC_BFM_TXC_SECTION_A_BYTE_LEN 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txcSectionAByteLen : 13; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 19;
	} bitFields;
} RegHobMacBfmTxcSectionAByteLen_u;

/*REG_HOB_MAC_BFM_TXC_SECTION_B_BYTE_LEN 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txcSectionBByteLen : 13; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 19;
	} bitFields;
} RegHobMacBfmTxcSectionBByteLen_u;

/*REG_HOB_MAC_BFM_TXC_SECTION_A_POINTER 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txcSectionAPointer : 25; //Pointer for section a in the TX circular RAM, reset value: 0x0, access type: RO
		uint32 reserved0 : 7;
	} bitFields;
} RegHobMacBfmTxcSectionAPointer_u;

/*REG_HOB_MAC_BFM_TXC_SECTION_B_POINTER 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txcSectionBPointer : 25; //Pointer for section a in the TX circular RAM, reset value: 0x0, access type: RO
		uint32 reserved0 : 7;
	} bitFields;
} RegHobMacBfmTxcSectionBPointer_u;

/*REG_HOB_MAC_BFM_ACTION_AND_CATEGORY 0x38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 categoryIn : 8; //no description, reset value: 0x0, access type: RO
		uint32 actionIn : 8; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegHobMacBfmActionAndCategory_u;

/*REG_HOB_MAC_BFM_BFEE_GEN_FCS_VALID 0x3C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfeeGenFcsValid : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegHobMacBfmBfeeGenFcsValid_u;

/*REG_HOB_MAC_BFM_BFEE_RESPONSE_TYPE_TF 0x40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfeeResponseTypeTf : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegHobMacBfmBfeeResponseTypeTf_u;

/*REG_HOB_MAC_BFM_BFEE_INVALID_TX_DATA 0x44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfeeInvalidTxData : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegHobMacBfmBfeeInvalidTxData_u;

/*REG_HOB_MAC_BFM_BFEE_SM 0x4C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfeeBfrSuccess : 1; //no description, reset value: 0x0, access type: RO
		uint32 bfBfeeSmIdle : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 30;
	} bitFields;
} RegHobMacBfmBfeeSm_u;

/*REG_HOB_MAC_BFM_BFEE_MIMO_CONTROL_0 0x50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfeeMimoControl0 : 16; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegHobMacBfmBfeeMimoControl0_u;

/*REG_HOB_MAC_BFM_BFEE_MIMO_CONTROL_1 0x54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfeeMimoControl1 : 16; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegHobMacBfmBfeeMimoControl1_u;

/*REG_HOB_MAC_BFM_BFEE_MIMO_CONTROL_2 0x58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfeeMimoControl2 : 16; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegHobMacBfmBfeeMimoControl2_u;

/*REG_HOB_MAC_BFM_SECTION_C_BASE_ADDR_0 0x60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sectionCBaseAddr0 : 25; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 7;
	} bitFields;
} RegHobMacBfmSectionCBaseAddr0_u;

/*REG_HOB_MAC_BFM_SECTION_C_LENGTH_0 0x64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sectionCLength0 : 14; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 18;
	} bitFields;
} RegHobMacBfmSectionCLength0_u;

/*REG_HOB_MAC_BFM_SECTION_D_BASE_ADDR_0 0x68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sectionDBaseAddr0 : 25; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 7;
	} bitFields;
} RegHobMacBfmSectionDBaseAddr0_u;

/*REG_HOB_MAC_BFM_SECTION_D_LENGTH_0 0x6C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sectionDLength0 : 10; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 22;
	} bitFields;
} RegHobMacBfmSectionDLength0_u;

/*REG_HOB_MAC_BFM_SECTION_C_BASE_ADDR_1 0x70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sectionCBaseAddr1 : 25; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 7;
	} bitFields;
} RegHobMacBfmSectionCBaseAddr1_u;

/*REG_HOB_MAC_BFM_SECTION_C_LENGTH_1 0x74 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sectionCLength1 : 14; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 18;
	} bitFields;
} RegHobMacBfmSectionCLength1_u;

/*REG_HOB_MAC_BFM_SECTION_D_BASE_ADDR_1 0x78 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sectionDBaseAddr1 : 25; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 7;
	} bitFields;
} RegHobMacBfmSectionDBaseAddr1_u;

/*REG_HOB_MAC_BFM_SECTION_D_LENGTH_1 0x7C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sectionDLength1 : 10; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 22;
	} bitFields;
} RegHobMacBfmSectionDLength1_u;

/*REG_HOB_MAC_BFM_SECTION_C_BASE_ADDR_2 0x80 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sectionCBaseAddr2 : 25; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 7;
	} bitFields;
} RegHobMacBfmSectionCBaseAddr2_u;

/*REG_HOB_MAC_BFM_SECTION_C_LENGTH_2 0x84 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sectionCLength2 : 14; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 18;
	} bitFields;
} RegHobMacBfmSectionCLength2_u;

/*REG_HOB_MAC_BFM_SECTION_D_BASE_ADDR_2 0x88 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sectionDBaseAddr2 : 25; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 7;
	} bitFields;
} RegHobMacBfmSectionDBaseAddr2_u;

/*REG_HOB_MAC_BFM_SECTION_D_LENGTH_2 0x8C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sectionDLength2 : 10; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 22;
	} bitFields;
} RegHobMacBfmSectionDLength2_u;

/*REG_HOB_MAC_BFM_BFEE_MPDU_LENGTH_0 0xB8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfeeMpduLength0 : 14; //Length in bytes for MPDU 0. This field is valid only when MPDU 0 is required for this TX BFee session, reset value: 0x0, access type: RO
		uint32 reserved0 : 18;
	} bitFields;
} RegHobMacBfmBfeeMpduLength0_u;

/*REG_HOB_MAC_BFM_BFEE_MPDU_LENGTH_1 0xBC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfeeMpduLength1 : 14; //Length in bytes for MPDU 1. This field is valid only when MPDU 1 is required for this TX BFee session, reset value: 0x0, access type: RO
		uint32 reserved0 : 18;
	} bitFields;
} RegHobMacBfmBfeeMpduLength1_u;

/*REG_HOB_MAC_BFM_BFEE_MPDU_LENGTH_2 0xC0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfeeMpduLength2 : 14; //Length in bytes for MPDU 2. This field is valid only when MPDU 2 is required for this TX BFee session, reset value: 0x0, access type: RO
		uint32 reserved0 : 18;
	} bitFields;
} RegHobMacBfmBfeeMpduLength2_u;

/*REG_HOB_MAC_BFM_BFEE_STATIC_INDIACTIONS 0xD0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfeeLengthReady : 1; //no description, reset value: 0x0, access type: RO
		uint32 txcSectionAReady : 1; //no description, reset value: 0x0, access type: RO
		uint32 txcSectionBReady : 1; //no description, reset value: 0x0, access type: RO
		uint32 txcSectionCReady : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 28;
	} bitFields;
} RegHobMacBfmBfeeStaticIndiactions_u;



#endif // _HOB_MAC_BFM_REGS_H_
