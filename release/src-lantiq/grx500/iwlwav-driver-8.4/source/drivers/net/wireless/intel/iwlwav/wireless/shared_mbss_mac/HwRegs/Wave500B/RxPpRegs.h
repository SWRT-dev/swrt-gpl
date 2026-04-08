
/***********************************************************************************
File:				RxPpRegs.h
Module:				rxPp
SOC Revision:		843
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
#define	REG_RX_PP_SW_INST_CTL                (RX_PP_BASE_ADDRESS + 0x4800)
#define	REG_RX_PP_SW_INST_ADDR               (RX_PP_BASE_ADDRESS + 0x4804)
#define	REG_RX_PP_SW_HALT                    (RX_PP_BASE_ADDRESS + 0x4808)
#define	REG_RX_PP_MPDU_IN_LIST               (RX_PP_BASE_ADDRESS + 0x480C)
#define	REG_RX_PP_ERR_LIST                   (RX_PP_BASE_ADDRESS + 0x4810)
#define	REG_RX_PP_UMAC_DONE_LIST             (RX_PP_BASE_ADDRESS + 0x4814)
#define	REG_RX_PP_LIBERATOR_LIST             (RX_PP_BASE_ADDRESS + 0x4818)
#define	REG_RX_PP_RD_TYPE0_FORWARD_LIST      (RX_PP_BASE_ADDRESS + 0x481C)
#define	REG_RX_PP_RD_TYPE1_FORWARD_LIST      (RX_PP_BASE_ADDRESS + 0x4820)
#define	REG_RX_PP_RD_TYPE2_FORWARD_LIST      (RX_PP_BASE_ADDRESS + 0x4824)
#define	REG_RX_PP_RD_TYPE3_FORWARD_LIST      (RX_PP_BASE_ADDRESS + 0x4828)
#define	REG_RX_PP_RD_TYPE4_FORWARD_LIST      (RX_PP_BASE_ADDRESS + 0x482C)
#define	REG_RX_PP_RD_TYPE5_FORWARD_LIST      (RX_PP_BASE_ADDRESS + 0x4830)
#define	REG_RX_PP_RD_TYPE6_FORWARD_LIST      (RX_PP_BASE_ADDRESS + 0x4834)
#define	REG_RX_PP_RD_TYPE7_FORWARD_LIST      (RX_PP_BASE_ADDRESS + 0x4838)
#define	REG_RX_PP_RD_TYPE8_FORWARD_LIST      (RX_PP_BASE_ADDRESS + 0x483C)
#define	REG_RX_PP_RD_TYPE9_FORWARD_LIST      (RX_PP_BASE_ADDRESS + 0x4840)
#define	REG_RX_PP_RD_TYPE10_FORWARD_LIST     (RX_PP_BASE_ADDRESS + 0x4844)
#define	REG_RX_PP_RD_TYPE11_FORWARD_LIST     (RX_PP_BASE_ADDRESS + 0x4848)
#define	REG_RX_PP_RD_TYPE12_FORWARD_LIST     (RX_PP_BASE_ADDRESS + 0x484C)
#define	REG_RX_PP_RD_TYPE13_FORWARD_LIST     (RX_PP_BASE_ADDRESS + 0x4850)
#define	REG_RX_PP_RD_TYPE14_FORWARD_LIST     (RX_PP_BASE_ADDRESS + 0x4854)
#define	REG_RX_PP_RD_TYPE15_FORWARD_LIST     (RX_PP_BASE_ADDRESS + 0x4858)
#define	REG_RX_PP_SM_STATUS                  (RX_PP_BASE_ADDRESS + 0x485C)
#define	REG_RX_PP_RD_TYPE_SKIP_UMAC_CFG      (RX_PP_BASE_ADDRESS + 0x4860)
#define	REG_RX_PP_SHRAM_FIFO_CFG             (RX_PP_BASE_ADDRESS + 0x4864)
#define	REG_RX_PP_SHRAM_FIFO_SUB             (RX_PP_BASE_ADDRESS + 0x4868)
#define	REG_RX_PP_SHRAM_FIFO_STATUS          (RX_PP_BASE_ADDRESS + 0x486C)
#define	REG_RX_PP_SHRAM_FIFO_CLR             (RX_PP_BASE_ADDRESS + 0x4870)
#define	REG_RX_PP_CONTROL_BACKDOOR_UPDATE    (RX_PP_BASE_ADDRESS + 0x4874)
#define	REG_RX_PP_RX_PP_SPARE                (RX_PP_BASE_ADDRESS + 0x4878)
#define	REG_RX_PP_RX_COUNTERS_CFG            (RX_PP_BASE_ADDRESS + 0x487C)
#define	REG_RX_PP_RX_LOGGER_CFG              (RX_PP_BASE_ADDRESS + 0x4880)
#define	REG_RX_PP_RX_LOGGER_BUSY             (RX_PP_BASE_ADDRESS + 0x4884)
#define	REG_RX_PP_RX_PP_CHICKEN_BITS         (RX_PP_BASE_ADDRESS + 0x4888)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_RX_PP_SW_INST_CTL 0x4800 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 baWindowSize:7;	// BA window size
		uint32 reserved0:1;
		uint32 newSn:12;	// New SN value. Valid when sn_update is asserted
		uint32 reserved1:4;
		uint32 clearDest:1;	// Deciding where to send SNs in list when clearing it 
		uint32 noBaForce:1;
		uint32 statusBitValue:1;
		uint32 mulTids:1;
		uint32 snUpdate:1;	// New SN update
		uint32 winSizeUpdate:1;	// Window size update
		uint32 flush:1;	// Flushing all SNs from list
		uint32 statusBitUpdate:1;
	} bitFields;
} RegRxPpSwInstCtl_u;

/*REG_RX_PP_SW_INST_ADDR 0x4804 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tid:4;	// TID address. Valid values are 0-8
		uint32 sta:7;	// STA address. Valid values are 0-127
		uint32 reserved0:1;
		uint32 swInstDone:1;
		uint32 reserved1:1;
		uint32 mulTidsEn:9;
		uint32 mulTidsStatusBitValue:9;
	} bitFields;
} RegRxPpSwInstAddr_u;

/*REG_RX_PP_SW_HALT 0x4808 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swHalt:1;	// Halts the block from moving to the next MPDU
		uint32 reserved0:31;
	} bitFields;
} RegRxPpSwHalt_u;

/*REG_RX_PP_MPDU_IN_LIST 0x480C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mpduInListIdx:5;	// MPDU list index number at the RX lists DLM
		uint32 reserved0:27;
	} bitFields;
} RegRxPpMpduInList_u;

/*REG_RX_PP_ERR_LIST 0x4810 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 errListIdx:5;	// Host IF list index number at the RX lists DLM
		uint32 reserved0:27;
	} bitFields;
} RegRxPpErrList_u;

/*REG_RX_PP_UMAC_DONE_LIST 0x4814 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 umacDoneListIdx:5;	// UMAC done list index number at the RX lists DLM
		uint32 reserved0:27;
	} bitFields;
} RegRxPpUmacDoneList_u;

/*REG_RX_PP_LIBERATOR_LIST 0x4818 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 liberatorListIdx:5;	// Drop src0 Host IF list index number at the RX lists DLM
		uint32 reserved0:27;
	} bitFields;
} RegRxPpLiberatorList_u;

/*REG_RX_PP_RD_TYPE0_FORWARD_LIST 0x481C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rdType0ListIdx:5;
		uint32 reserved0:27;
	} bitFields;
} RegRxPpRdType0ForwardList_u;

/*REG_RX_PP_RD_TYPE1_FORWARD_LIST 0x4820 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rdType1ListIdx:5;
		uint32 reserved0:27;
	} bitFields;
} RegRxPpRdType1ForwardList_u;

/*REG_RX_PP_RD_TYPE2_FORWARD_LIST 0x4824 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rdType2ListIdx:5;
		uint32 reserved0:27;
	} bitFields;
} RegRxPpRdType2ForwardList_u;

/*REG_RX_PP_RD_TYPE3_FORWARD_LIST 0x4828 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rdType3ListIdx:5;
		uint32 reserved0:27;
	} bitFields;
} RegRxPpRdType3ForwardList_u;

/*REG_RX_PP_RD_TYPE4_FORWARD_LIST 0x482C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rdType4ListIdx:5;
		uint32 reserved0:27;
	} bitFields;
} RegRxPpRdType4ForwardList_u;

/*REG_RX_PP_RD_TYPE5_FORWARD_LIST 0x4830 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rdType5ListIdx:5;
		uint32 reserved0:27;
	} bitFields;
} RegRxPpRdType5ForwardList_u;

/*REG_RX_PP_RD_TYPE6_FORWARD_LIST 0x4834 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rdType6ListIdx:5;
		uint32 reserved0:27;
	} bitFields;
} RegRxPpRdType6ForwardList_u;

/*REG_RX_PP_RD_TYPE7_FORWARD_LIST 0x4838 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rdType7ListIdx:5;
		uint32 reserved0:27;
	} bitFields;
} RegRxPpRdType7ForwardList_u;

/*REG_RX_PP_RD_TYPE8_FORWARD_LIST 0x483C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rdType8ListIdx:5;
		uint32 reserved0:27;
	} bitFields;
} RegRxPpRdType8ForwardList_u;

/*REG_RX_PP_RD_TYPE9_FORWARD_LIST 0x4840 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rdType9ListIdx:5;
		uint32 reserved0:27;
	} bitFields;
} RegRxPpRdType9ForwardList_u;

/*REG_RX_PP_RD_TYPE10_FORWARD_LIST 0x4844 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rdType10ListIdx:5;
		uint32 reserved0:27;
	} bitFields;
} RegRxPpRdType10ForwardList_u;

/*REG_RX_PP_RD_TYPE11_FORWARD_LIST 0x4848 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rdType11ListIdx:5;
		uint32 reserved0:27;
	} bitFields;
} RegRxPpRdType11ForwardList_u;

/*REG_RX_PP_RD_TYPE12_FORWARD_LIST 0x484C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rdType12ListIdx:5;
		uint32 reserved0:27;
	} bitFields;
} RegRxPpRdType12ForwardList_u;

/*REG_RX_PP_RD_TYPE13_FORWARD_LIST 0x4850 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rdType13ListIdx:5;
		uint32 reserved0:27;
	} bitFields;
} RegRxPpRdType13ForwardList_u;

/*REG_RX_PP_RD_TYPE14_FORWARD_LIST 0x4854 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rdType14ListIdx:5;
		uint32 reserved0:27;
	} bitFields;
} RegRxPpRdType14ForwardList_u;

/*REG_RX_PP_RD_TYPE15_FORWARD_LIST 0x4858 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rdType15ListIdx:5;
		uint32 reserved0:27;
	} bitFields;
} RegRxPpRdType15ForwardList_u;

/*REG_RX_PP_SM_STATUS 0x485C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rdAnalyzeSm:4;
		uint32 rdDigSm:3;
		uint32 reserved0:1;
		uint32 rdClearSm:4;
		uint32 swSnUpdateSm:4;
		uint32 swWinUpdateSm:3;
		uint32 reserved1:1;
		uint32 swFlushSm:3;
		uint32 reserved2:1;
		uint32 statusUpdateSm:2;
		uint32 reserved3:2;
		uint32 impClearSm:4;
	} bitFields;
} RegRxPpSmStatus_u;

/*REG_RX_PP_RD_TYPE_SKIP_UMAC_CFG 0x4860 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 skipBit:16;
		uint32 umacBit:16;
	} bitFields;
} RegRxPpRdTypeSkipUmacCfg_u;

/*REG_RX_PP_SHRAM_FIFO_CFG 0x4864 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fifoLineSize:10;
		uint32 shramFifoBaseAddr:21;
		uint32 shramFifoEn:1;
	} bitFields;
} RegRxPpShramFifoCfg_u;

/*REG_RX_PP_SHRAM_FIFO_SUB 0x4868 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 shramFifoSub:10;
		uint32 reserved0:22;
	} bitFields;
} RegRxPpShramFifoSub_u;

/*REG_RX_PP_SHRAM_FIFO_STATUS 0x486C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 shramFifoLinesTaken:10;
		uint32 reserved0:2;
		uint32 shramFifoFull:1;
		uint32 shramFifoEmpty:1;
		uint32 shramFifoOverflow:1;
		uint32 shramFifoUnderflow:1;
		uint32 shramSynFifoBytesInFifo:2;
		uint32 reserved1:2;
		uint32 shramSynFifoFull:1;
		uint32 shramSynFifoEmpty:1;
		uint32 shramSynFifoPopWhileEmpty:1;
		uint32 shramSynFifoPushWhileFull:1;
		uint32 reserved2:8;
	} bitFields;
} RegRxPpShramFifoStatus_u;

/*REG_RX_PP_SHRAM_FIFO_CLR 0x4870 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 shramFifoClear:1;
		uint32 reserved0:31;
	} bitFields;
} RegRxPpShramFifoClr_u;

/*REG_RX_PP_CONTROL_BACKDOOR_UPDATE 0x4874 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 snNumValid:1;	// BA window size
		uint32 pnNumValid:1;	// New SN value. Valid when sn_update is asserted
		uint32 reserved0:1;
		uint32 inFrag:1;	// Deciding where to send SNs in list when clearing it 
		uint32 controlBe:4;
		uint32 controlUpdateDone:1;
		uint32 reserved1:23;
	} bitFields;
} RegRxPpControlBackdoorUpdate_u;

/*REG_RX_PP_RX_PP_SPARE 0x4878 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxPpSpare:32;
	} bitFields;
} RegRxPpRxPpSpare_u;

/*REG_RX_PP_RX_COUNTERS_CFG 0x487C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 qosRxEn:1;
		uint32 rdCountEn:1;
		uint32 rdDelayedCountEn:1;
		uint32 rdSwDropCountEn:1;
		uint32 rdDuplicateDropCountEn:1;
		uint32 missingSnSwUpdateCountEn:1;
		uint32 mpduUniOrMgmtEn:1;
		uint32 mpduRetryEn:1;
		uint32 amsduCountEn:1;
		uint32 dropMpduEn:1;
		uint32 mpduTypeNotSupportedEn:1;
		uint32 barMpduCountEn:1;
		uint32 replayEn:1;
		uint32 replayMgmtEn:1;
		uint32 tkipCountEn:1;
		uint32 failureCountEn:1;
		uint32 rxClassAmsduBytesStbEn:1;
		uint32 rxcAmpduStbEn:1;
		uint32 rxClassMpduInAmpduStbEn:1;
		uint32 rxClassOctetMpduStbEn:1;
		uint32 rxClassDropStbEn:1;
		uint32 rxClassSecurMisStbEn:1;
		uint32 rxcCrcErrorStbEn:1;
		uint32 delineatorCrcErrorStbEn:1;
		uint32 bcMcCountEn:1;
		uint32 reserved0:5;
		uint32 qosRxDecEn:1;
		uint32 rxCountCleanFifo:1;
	} bitFields;
} RegRxPpRxCountersCfg_u;

/*REG_RX_PP_RX_LOGGER_CFG 0x4880 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxPpLoggerPriority:2;
		uint32 rxPpLoggerEn:1;
		uint32 reserved0:29;
	} bitFields;
} RegRxPpRxLoggerCfg_u;

/*REG_RX_PP_RX_LOGGER_BUSY 0x4884 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxPpLoggerBusy:1;
		uint32 reserved0:31;
	} bitFields;
} RegRxPpRxLoggerBusy_u;

/*REG_RX_PP_RX_PP_CHICKEN_BITS 0x4888 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pnFixChickenBit:1;	// In case there are clear of more than 1 SN the PN shouldn't update during the RD_DROP phase, only for SEC phase. I placed chicken bit for this fix.
		uint32 reserved0:31;
	} bitFields;
} RegRxPpRxPpChickenBits_u;



#endif // _RX_PP_REGS_H_
