
/***********************************************************************************
File:				HobAutoRespRegs.h
Module:				hobAutoResp
SOC Revision:		
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _HOB_AUTO_RESP_REGS_H_
#define _HOB_AUTO_RESP_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define HOB_AUTO_RESP_BASE_ADDRESS                             MEMORY_MAP_UNIT_504_BASE_ADDRESS
#define	REG_HOB_AUTO_RESP_DEL_DESC_FRAME_LEN               (HOB_AUTO_RESP_BASE_ADDRESS + 0x0)
#define	REG_HOB_AUTO_RESP_DEL_DESC_FRAME_PTR               (HOB_AUTO_RESP_BASE_ADDRESS + 0x4)
#define	REG_HOB_AUTO_RESP_DEL_DESC_DUR_TYPE                (HOB_AUTO_RESP_BASE_ADDRESS + 0x8)
#define	REG_HOB_AUTO_RESP_DEL_TCR_FRAME_LEN                (HOB_AUTO_RESP_BASE_ADDRESS + 0xC)
#define	REG_HOB_AUTO_RESP_DEL_MAC_ADDR_RA0                 (HOB_AUTO_RESP_BASE_ADDRESS + 0x10)
#define	REG_HOB_AUTO_RESP_DEL_MAC_ADDR_RA1                 (HOB_AUTO_RESP_BASE_ADDRESS + 0x14)
#define	REG_HOB_AUTO_RESP_DEL_MAC_ADDR_TA0                 (HOB_AUTO_RESP_BASE_ADDRESS + 0x18)
#define	REG_HOB_AUTO_RESP_DEL_MAC_ADDR_TA1                 (HOB_AUTO_RESP_BASE_ADDRESS + 0x1C)
#define	REG_HOB_AUTO_RESP_AUTO_RESP_TCR_BASE_ADDR          (HOB_AUTO_RESP_BASE_ADDRESS + 0x20)
#define	REG_HOB_AUTO_RESP_AUTO_RESP_HE_MU_TCR_BASE_ADDR    (HOB_AUTO_RESP_BASE_ADDRESS + 0x24)
#define	REG_HOB_AUTO_RESP_AUTO_RESP_TX_BW                  (HOB_AUTO_RESP_BASE_ADDRESS + 0x28)
#define	REG_HOB_AUTO_RESP_FC_PM_BIT                        (HOB_AUTO_RESP_BASE_ADDRESS + 0x2C)
#define	REG_HOB_AUTO_RESP_TF_TCR_BASE_ADDR                 (HOB_AUTO_RESP_BASE_ADDRESS + 0x30)
#define	REG_HOB_AUTO_RESP_RESPONSE_TYPE_TB                 (HOB_AUTO_RESP_BASE_ADDRESS + 0x34)
#define	REG_HOB_AUTO_RESP_AUTO_RESP_TX_PUNCTURING_MAP      (HOB_AUTO_RESP_BASE_ADDRESS + 0x38)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_HOB_AUTO_RESP_DEL_DESC_FRAME_LEN 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 delDescFrameLen : 12; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 20;
	} bitFields;
} RegHobAutoRespDelDescFrameLen_u;

/*REG_HOB_AUTO_RESP_DEL_DESC_FRAME_PTR 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 delDescFramePtr : 25; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 7;
	} bitFields;
} RegHobAutoRespDelDescFramePtr_u;

/*REG_HOB_AUTO_RESP_DEL_DESC_DUR_TYPE 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 delDescDurType : 2; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 30;
	} bitFields;
} RegHobAutoRespDelDescDurType_u;

/*REG_HOB_AUTO_RESP_DEL_TCR_FRAME_LEN 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 delTcrFrameLen : 22; //In case of TF this field holds the full length of the PSDU, not just Auto Resp frame, reset value: 0x0, access type: RO
		uint32 reserved0 : 10;
	} bitFields;
} RegHobAutoRespDelTcrFrameLen_u;

/*REG_HOB_AUTO_RESP_DEL_MAC_ADDR_RA0 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 delMacAddrRa310 : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegHobAutoRespDelMacAddrRa0_u;

/*REG_HOB_AUTO_RESP_DEL_MAC_ADDR_RA1 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 delMacAddrRa4732 : 16; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegHobAutoRespDelMacAddrRa1_u;

/*REG_HOB_AUTO_RESP_DEL_MAC_ADDR_TA0 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 delMacAddrTa310 : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegHobAutoRespDelMacAddrTa0_u;

/*REG_HOB_AUTO_RESP_DEL_MAC_ADDR_TA1 0x1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 delMacAddrTa4732 : 16; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegHobAutoRespDelMacAddrTa1_u;

/*REG_HOB_AUTO_RESP_AUTO_RESP_TCR_BASE_ADDR 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoRespTcrBaseAddr : 25; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 7;
	} bitFields;
} RegHobAutoRespAutoRespTcrBaseAddr_u;

/*REG_HOB_AUTO_RESP_AUTO_RESP_HE_MU_TCR_BASE_ADDR 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoRespHeMuTcrBaseAddr : 25; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 7;
	} bitFields;
} RegHobAutoRespAutoRespHeMuTcrBaseAddr_u;

/*REG_HOB_AUTO_RESP_AUTO_RESP_TX_BW 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoRespTxBw : 3; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 29;
	} bitFields;
} RegHobAutoRespAutoRespTxBw_u;

/*REG_HOB_AUTO_RESP_FC_PM_BIT 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fcPmBit : 16; //Frame Control PM bit (bit 12) to be used by Delia in case of BF Report (OR with the frame header)., reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegHobAutoRespFcPmBit_u;

/*REG_HOB_AUTO_RESP_TF_TCR_BASE_ADDR 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tfTcrBaseAddr : 25; //Base address in TX Circ Buffer for TB response TCRs, reset value: 0x0, access type: RO
		uint32 reserved0 : 7;
	} bitFields;
} RegHobAutoRespTfTcrBaseAddr_u;

/*REG_HOB_AUTO_RESP_RESPONSE_TYPE_TB 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 responseTypeTb : 1; //1'b0 - response type is non TB , 1'b1 - response type is TB, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegHobAutoRespResponseTypeTb_u;

/*REG_HOB_AUTO_RESP_AUTO_RESP_TX_PUNCTURING_MAP 0x38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoRespTxPuncturingMap : 16; //EHT puncturing, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegHobAutoRespAutoRespTxPuncturingMap_u;



#endif // _HOB_AUTO_RESP_REGS_H_
