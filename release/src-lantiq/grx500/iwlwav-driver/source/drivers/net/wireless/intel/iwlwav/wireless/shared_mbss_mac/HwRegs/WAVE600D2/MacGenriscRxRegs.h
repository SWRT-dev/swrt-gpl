
/***********************************************************************************
File:				MacGenriscRxRegs.h
Module:				macGenriscRx
SOC Revision:		
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _MAC_GENRISC_RX_REGS_H_
#define _MAC_GENRISC_RX_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define MAC_GENRISC_RX_BASE_ADDRESS                             MEMORY_MAP_UNIT_27_BASE_ADDRESS
#define	REG_MAC_GENRISC_RX_INTERNAL_REGISTER0           (MAC_GENRISC_RX_BASE_ADDRESS + 0x0)
#define	REG_MAC_GENRISC_RX_INTERNAL_REGISTER1           (MAC_GENRISC_RX_BASE_ADDRESS + 0x4)
#define	REG_MAC_GENRISC_RX_INTERNAL_REGISTER2           (MAC_GENRISC_RX_BASE_ADDRESS + 0x8)
#define	REG_MAC_GENRISC_RX_INTERNAL_REGISTER3           (MAC_GENRISC_RX_BASE_ADDRESS + 0xC)
#define	REG_MAC_GENRISC_RX_INTERNAL_REGISTER4           (MAC_GENRISC_RX_BASE_ADDRESS + 0x10)
#define	REG_MAC_GENRISC_RX_INTERNAL_REGISTER5           (MAC_GENRISC_RX_BASE_ADDRESS + 0x14)
#define	REG_MAC_GENRISC_RX_INTERNAL_REGISTER6           (MAC_GENRISC_RX_BASE_ADDRESS + 0x18)
#define	REG_MAC_GENRISC_RX_INTERNAL_REGISTER7           (MAC_GENRISC_RX_BASE_ADDRESS + 0x1C)
#define	REG_MAC_GENRISC_RX_INTERNAL_REGISTER8           (MAC_GENRISC_RX_BASE_ADDRESS + 0x20)
#define	REG_MAC_GENRISC_RX_INTERNAL_REGISTER9           (MAC_GENRISC_RX_BASE_ADDRESS + 0x24)
#define	REG_MAC_GENRISC_RX_INTERNAL_REGISTER10          (MAC_GENRISC_RX_BASE_ADDRESS + 0x28)
#define	REG_MAC_GENRISC_RX_INTERNAL_REGISTER11          (MAC_GENRISC_RX_BASE_ADDRESS + 0x2C)
#define	REG_MAC_GENRISC_RX_INTERNAL_REGISTER12          (MAC_GENRISC_RX_BASE_ADDRESS + 0x30)
#define	REG_MAC_GENRISC_RX_INTERNAL_REGISTER13          (MAC_GENRISC_RX_BASE_ADDRESS + 0x34)
#define	REG_MAC_GENRISC_RX_INTERNAL_REGISTER14          (MAC_GENRISC_RX_BASE_ADDRESS + 0x38)
#define	REG_MAC_GENRISC_RX_INTERNAL_REGISTER15          (MAC_GENRISC_RX_BASE_ADDRESS + 0x3C)
#define	REG_MAC_GENRISC_RX_STATUS_REGISTER              (MAC_GENRISC_RX_BASE_ADDRESS + 0x40)
#define	REG_MAC_GENRISC_RX_ERR_REG_0                    (MAC_GENRISC_RX_BASE_ADDRESS + 0x44)
#define	REG_MAC_GENRISC_RX_ERR_REG_1                    (MAC_GENRISC_RX_BASE_ADDRESS + 0x48)
#define	REG_MAC_GENRISC_RX_LAST_PC_EXECUTED             (MAC_GENRISC_RX_BASE_ADDRESS + 0x4C)
#define	REG_MAC_GENRISC_RX_ADD_OUT_ABORT                (MAC_GENRISC_RX_BASE_ADDRESS + 0x54)
#define	REG_MAC_GENRISC_RX_STOP_OP                      (MAC_GENRISC_RX_BASE_ADDRESS + 0x60)
#define	REG_MAC_GENRISC_RX_CONTINUE_OP                  (MAC_GENRISC_RX_BASE_ADDRESS + 0x64)
#define	REG_MAC_GENRISC_RX_SINGLE_STEP                  (MAC_GENRISC_RX_BASE_ADDRESS + 0x68)
#define	REG_MAC_GENRISC_RX_EXT_COMMAND                  (MAC_GENRISC_RX_BASE_ADDRESS + 0x6C)
#define	REG_MAC_GENRISC_RX_STEP_COMMAND                 (MAC_GENRISC_RX_BASE_ADDRESS + 0x70)
#define	REG_MAC_GENRISC_RX_BRKP_ADDRESS                 (MAC_GENRISC_RX_BASE_ADDRESS + 0x74)
#define	REG_MAC_GENRISC_RX_BRKP_ADDRESS_EN              (MAC_GENRISC_RX_BASE_ADDRESS + 0x7C)
#define	REG_MAC_GENRISC_RX_TEST_BUS_DATA                (MAC_GENRISC_RX_BASE_ADDRESS + 0x80)
#define	REG_MAC_GENRISC_RX_TEST_BUS_ENABLE              (MAC_GENRISC_RX_BASE_ADDRESS + 0x84)
#define	REG_MAC_GENRISC_RX_INTTEUPTS_SAMPLE             (MAC_GENRISC_RX_BASE_ADDRESS + 0x88)
#define	REG_MAC_GENRISC_RX_START_OP                     (MAC_GENRISC_RX_BASE_ADDRESS + 0x90)
#define	REG_MAC_GENRISC_RX_ABORT_CNT_LIMIT              (MAC_GENRISC_RX_BASE_ADDRESS + 0x94)
#define	REG_MAC_GENRISC_RX_CPU2GENRISC_DEBUG_MODE_EN    (MAC_GENRISC_RX_BASE_ADDRESS + 0x98)
#define	REG_MAC_GENRISC_RX_GENRISC_UPPER_IRQ_ENABLE     (MAC_GENRISC_RX_BASE_ADDRESS + 0x9C)
#define	REG_MAC_GENRISC_RX_GENRISC_LOWER_IRQ_ENABLE     (MAC_GENRISC_RX_BASE_ADDRESS + 0xA0)
#define	REG_MAC_GENRISC_RX_GENRISC_UPPER_IRQ_SET        (MAC_GENRISC_RX_BASE_ADDRESS + 0xA4)
#define	REG_MAC_GENRISC_RX_GENRISC_UPPER_IRQ_CLR        (MAC_GENRISC_RX_BASE_ADDRESS + 0xA8)
#define	REG_MAC_GENRISC_RX_GENRISC_UPPER_IRQ_STATUS     (MAC_GENRISC_RX_BASE_ADDRESS + 0xAC)
#define	REG_MAC_GENRISC_RX_GENRISC_LOWER_IRQ_SET        (MAC_GENRISC_RX_BASE_ADDRESS + 0xB0)
#define	REG_MAC_GENRISC_RX_GENRISC_LOWER_IRQ_CLR        (MAC_GENRISC_RX_BASE_ADDRESS + 0xB4)
#define	REG_MAC_GENRISC_RX_GENRISC_LOWER_IRQ_STATUS     (MAC_GENRISC_RX_BASE_ADDRESS + 0xB8)
#define	REG_MAC_GENRISC_RX_MIPS2GENRISC_IRQ_SET         (MAC_GENRISC_RX_BASE_ADDRESS + 0xBC)
#define	REG_MAC_GENRISC_RX_MIPS2GENRISC_IRQ_CLR         (MAC_GENRISC_RX_BASE_ADDRESS + 0xC0)
#define	REG_MAC_GENRISC_RX_MIPS2GENRISC_IRQ_STATUS      (MAC_GENRISC_RX_BASE_ADDRESS + 0xC4)
#define	REG_MAC_GENRISC_RX_INT_VECTOR                   (MAC_GENRISC_RX_BASE_ADDRESS + 0xC8)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_MAC_GENRISC_RX_INTERNAL_REGISTER0 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 internalRegister0 : 32; //Internal Register 0, reset value: 0x0, access type: RO
	} bitFields;
} RegMacGenriscRxInternalRegister0_u;

/*REG_MAC_GENRISC_RX_INTERNAL_REGISTER1 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 internalRegister1 : 32; //Internal Register 1, reset value: 0x0, access type: RO
	} bitFields;
} RegMacGenriscRxInternalRegister1_u;

/*REG_MAC_GENRISC_RX_INTERNAL_REGISTER2 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 internalRegister2 : 32; //Internal Register 2, reset value: 0x0, access type: RO
	} bitFields;
} RegMacGenriscRxInternalRegister2_u;

/*REG_MAC_GENRISC_RX_INTERNAL_REGISTER3 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 internalRegister3 : 32; //Internal Register 3, reset value: 0x0, access type: RO
	} bitFields;
} RegMacGenriscRxInternalRegister3_u;

/*REG_MAC_GENRISC_RX_INTERNAL_REGISTER4 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 internalRegister4 : 32; //Internal Register 4, reset value: 0x0, access type: RO
	} bitFields;
} RegMacGenriscRxInternalRegister4_u;

/*REG_MAC_GENRISC_RX_INTERNAL_REGISTER5 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 internalRegister5 : 32; //Internal Register 5, reset value: 0x0, access type: RO
	} bitFields;
} RegMacGenriscRxInternalRegister5_u;

/*REG_MAC_GENRISC_RX_INTERNAL_REGISTER6 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 internalRegister6 : 32; //Internal Register 6, reset value: 0x0, access type: RO
	} bitFields;
} RegMacGenriscRxInternalRegister6_u;

/*REG_MAC_GENRISC_RX_INTERNAL_REGISTER7 0x1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 internalRegister7 : 32; //Internal Register 7, reset value: 0x0, access type: RO
	} bitFields;
} RegMacGenriscRxInternalRegister7_u;

/*REG_MAC_GENRISC_RX_INTERNAL_REGISTER8 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 internalRegister8 : 32; //Internal Register 8, reset value: 0x0, access type: RO
	} bitFields;
} RegMacGenriscRxInternalRegister8_u;

/*REG_MAC_GENRISC_RX_INTERNAL_REGISTER9 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 internalRegister9 : 32; //Internal Register 9, reset value: 0x0, access type: RO
	} bitFields;
} RegMacGenriscRxInternalRegister9_u;

/*REG_MAC_GENRISC_RX_INTERNAL_REGISTER10 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 internalRegister10 : 32; //Internal Register 10, reset value: 0x0, access type: RO
	} bitFields;
} RegMacGenriscRxInternalRegister10_u;

/*REG_MAC_GENRISC_RX_INTERNAL_REGISTER11 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 internalRegister11 : 32; //Internal Register 11, reset value: 0x0, access type: RO
	} bitFields;
} RegMacGenriscRxInternalRegister11_u;

/*REG_MAC_GENRISC_RX_INTERNAL_REGISTER12 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 internalRegister12 : 32; //Internal Register 12, reset value: 0x0, access type: RO
	} bitFields;
} RegMacGenriscRxInternalRegister12_u;

/*REG_MAC_GENRISC_RX_INTERNAL_REGISTER13 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 internalRegister13 : 32; //Internal Register 13, reset value: 0x0, access type: RO
	} bitFields;
} RegMacGenriscRxInternalRegister13_u;

/*REG_MAC_GENRISC_RX_INTERNAL_REGISTER14 0x38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 internalRegister14 : 32; //Internal Register 14, reset value: 0x0, access type: RO
	} bitFields;
} RegMacGenriscRxInternalRegister14_u;

/*REG_MAC_GENRISC_RX_INTERNAL_REGISTER15 0x3C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 internalRegister15 : 32; //Internal Register 15, reset value: 0x0, access type: RO
	} bitFields;
} RegMacGenriscRxInternalRegister15_u;

/*REG_MAC_GENRISC_RX_STATUS_REGISTER 0x40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 statusEqual : 1; //status equal bit, reset value: 0x0, access type: RO
		uint32 statusBig : 1; //status big bit, reset value: 0x0, access type: RO
		uint32 statusLittle : 1; //status little bit, reset value: 0x0, access type: RO
		uint32 statusNeg : 1; //status neg bit, reset value: 0x0, access type: RO
		uint32 statusCarry : 1; //status carry bit, reset value: 0x0, access type: RO
		uint32 divValid : 1; //Divider result valid, reset value: 0x1, access type: RO
		uint32 currInterrupt : 5; //Current interrupt, reset value: 0x0, access type: RO
		uint32 intActive : 1; //interrupt active, reset value: 0x0, access type: RO
		uint32 enabled : 1; //enabled, reset value: 0x0, access type: RO
		uint32 timerExpired : 1; //timer expired, reset value: 0x1, access type: RO
		uint32 haltWaitInt : 1; //halt wait intterrupt, reset value: 0x0, access type: RO
		uint32 intEn : 1; //interrupt enable, reset value: 0x0, access type: RO
		uint32 breakStall : 1; //break stall, reset value: 0x0, access type: RO
		uint32 stackError : 1; //stack error, reset value: 0x0, access type: RO
		uint32 notLegalOpcErr : 1; //not legal opcode error, reset value: 0x0, access type: RO
		uint32 abortError : 1; //abort error, reset value: 0x0, access type: RO
		uint32 reserved0 : 12;
	} bitFields;
} RegMacGenriscRxStatusRegister_u;

/*REG_MAC_GENRISC_RX_ERR_REG_0 0x44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 errReg0 : 16; //Error reg 0, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegMacGenriscRxErrReg0_u;

/*REG_MAC_GENRISC_RX_ERR_REG_1 0x48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 errReg1 : 16; //Error reg 1, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegMacGenriscRxErrReg1_u;

/*REG_MAC_GENRISC_RX_LAST_PC_EXECUTED 0x4C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lastExecuted : 16; //Last pc executed, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegMacGenriscRxLastPcExecuted_u;

/*REG_MAC_GENRISC_RX_ADD_OUT_ABORT 0x54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 addOutAbort : 28; //Address out abort, reset value: 0x0, access type: RO
		uint32 reserved0 : 4;
	} bitFields;
} RegMacGenriscRxAddOutAbort_u;

/*REG_MAC_GENRISC_RX_STOP_OP 0x60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 stopOp : 1; //stop opcode, reset value: 0x0, access type: WO
		uint32 reserved0 : 31;
	} bitFields;
} RegMacGenriscRxStopOp_u;

/*REG_MAC_GENRISC_RX_CONTINUE_OP 0x64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 continueOp : 1; //continue opcode, reset value: 0x0, access type: WO
		uint32 reserved0 : 31;
	} bitFields;
} RegMacGenriscRxContinueOp_u;

/*REG_MAC_GENRISC_RX_SINGLE_STEP 0x68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 singleStep : 1; //single step, reset value: 0x0, access type: WO
		uint32 reserved0 : 31;
	} bitFields;
} RegMacGenriscRxSingleStep_u;

/*REG_MAC_GENRISC_RX_EXT_COMMAND 0x6C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 extCommand : 1; //ext command, reset value: 0x0, access type: WO
		uint32 reserved0 : 31;
	} bitFields;
} RegMacGenriscRxExtCommand_u;

/*REG_MAC_GENRISC_RX_STEP_COMMAND 0x70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 stepCommand : 32; //step command, reset value: 0x0, access type: RW
	} bitFields;
} RegMacGenriscRxStepCommand_u;

/*REG_MAC_GENRISC_RX_BRKP_ADDRESS 0x74 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 brkpAddress1 : 16; //brkp address1, reset value: 0x0, access type: RW
		uint32 brkpAddress2 : 16; //brkp address2, reset value: 0x0, access type: RW
	} bitFields;
} RegMacGenriscRxBrkpAddress_u;

/*REG_MAC_GENRISC_RX_BRKP_ADDRESS_EN 0x7C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 brkpAddress1En : 1; //brkp address1 enable, reset value: 0x0, access type: RW
		uint32 brkpAddress2En : 1; //brkp address2 enable, reset value: 0x0, access type: RW
		uint32 reserved0 : 30;
	} bitFields;
} RegMacGenriscRxBrkpAddressEn_u;

/*REG_MAC_GENRISC_RX_TEST_BUS_DATA 0x80 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 testBusData : 24; //test bus data, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegMacGenriscRxTestBusData_u;

/*REG_MAC_GENRISC_RX_TEST_BUS_ENABLE 0x84 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 testBusEnable : 1; //test bus enable, reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegMacGenriscRxTestBusEnable_u;

/*REG_MAC_GENRISC_RX_INTTEUPTS_SAMPLE 0x88 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 intteuptsSample : 24; //intteupts sample, reset value: 0x0, access type: RO
		uint32 reserved0 : 8;
	} bitFields;
} RegMacGenriscRxIntteuptsSample_u;

/*REG_MAC_GENRISC_RX_START_OP 0x90 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 startOp : 1; //start opcode, reset value: 0x0, access type: WO
		uint32 reserved0 : 31;
	} bitFields;
} RegMacGenriscRxStartOp_u;

/*REG_MAC_GENRISC_RX_ABORT_CNT_LIMIT 0x94 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 abortCntLimit : 16; //abort counter limit, reset value: 0x01ff, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegMacGenriscRxAbortCntLimit_u;

/*REG_MAC_GENRISC_RX_CPU2GENRISC_DEBUG_MODE_EN 0x98 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lowerCpu2GenriscDebugModeEn : 1; //lower cpu2genrisc debug mode enable, reset value: 0x0, access type: RW
		uint32 upperCpu2GenriscDebugModeEn : 1; //Upper cpu2genrisc debug mode enable, reset value: 0x0, access type: RW
		uint32 reserved0 : 30;
	} bitFields;
} RegMacGenriscRxCpu2GenriscDebugModeEn_u;

/*REG_MAC_GENRISC_RX_GENRISC_UPPER_IRQ_ENABLE 0x9C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 genriscUpperIrqEnable : 16; //GenRisc to Upper IRQ enable, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegMacGenriscRxGenriscUpperIrqEnable_u;

/*REG_MAC_GENRISC_RX_GENRISC_LOWER_IRQ_ENABLE 0xA0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 genriscLowerIrqEnable : 16; //GenRisc to lower IRQ enable, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegMacGenriscRxGenriscLowerIrqEnable_u;

/*REG_MAC_GENRISC_RX_GENRISC_UPPER_IRQ_SET 0xA4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 genriscUpperIrqSet : 16; //GenRisc to Upper IRQ set, reset value: 0x0, access type: WO
		uint32 reserved0 : 16;
	} bitFields;
} RegMacGenriscRxGenriscUpperIrqSet_u;

/*REG_MAC_GENRISC_RX_GENRISC_UPPER_IRQ_CLR 0xA8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 genriscUpperIrqClr : 16; //GenRisc to Upper IRQ clear, reset value: 0x0, access type: WO
		uint32 reserved0 : 16;
	} bitFields;
} RegMacGenriscRxGenriscUpperIrqClr_u;

/*REG_MAC_GENRISC_RX_GENRISC_UPPER_IRQ_STATUS 0xAC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 genriscUpperIrqStatus : 16; //GenRisc to Upper IRQ status, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegMacGenriscRxGenriscUpperIrqStatus_u;

/*REG_MAC_GENRISC_RX_GENRISC_LOWER_IRQ_SET 0xB0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 genriscLowerIrqSet : 16; //GenRisc to Lower IRQ set, reset value: 0x0, access type: WO
		uint32 reserved0 : 16;
	} bitFields;
} RegMacGenriscRxGenriscLowerIrqSet_u;

/*REG_MAC_GENRISC_RX_GENRISC_LOWER_IRQ_CLR 0xB4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 genriscLowerIrqClr : 16; //GenRisc to Lower IRQ clear, reset value: 0x0, access type: WO
		uint32 reserved0 : 16;
	} bitFields;
} RegMacGenriscRxGenriscLowerIrqClr_u;

/*REG_MAC_GENRISC_RX_GENRISC_LOWER_IRQ_STATUS 0xB8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 genriscLowerIrqStatus : 16; //GenRisc to Lower IRQ status, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegMacGenriscRxGenriscLowerIrqStatus_u;

/*REG_MAC_GENRISC_RX_MIPS2GENRISC_IRQ_SET 0xBC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mips2GenriscIrqSet : 2; //MIPS to GenRisc IRQ set, reset value: 0x0, access type: WO
		uint32 reserved0 : 30;
	} bitFields;
} RegMacGenriscRxMips2GenriscIrqSet_u;

/*REG_MAC_GENRISC_RX_MIPS2GENRISC_IRQ_CLR 0xC0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mips2GenriscIrqClr : 2; //MIPS to GenRisc IRQ clear, reset value: 0x0, access type: WO
		uint32 reserved0 : 30;
	} bitFields;
} RegMacGenriscRxMips2GenriscIrqClr_u;

/*REG_MAC_GENRISC_RX_MIPS2GENRISC_IRQ_STATUS 0xC4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mips2GenriscIrqStatus : 2; //MIPS to GenRisc IRQ status, reset value: 0x0, access type: RO
		uint32 reserved0 : 30;
	} bitFields;
} RegMacGenriscRxMips2GenriscIrqStatus_u;

/*REG_MAC_GENRISC_RX_INT_VECTOR 0xC8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 intVector : 20; //GenRisc IRQ vector, reset value: 0x0, access type: RO
		uint32 reserved0 : 12;
	} bitFields;
} RegMacGenriscRxIntVector_u;



#endif // _MAC_GENRISC_RX_REGS_H_
