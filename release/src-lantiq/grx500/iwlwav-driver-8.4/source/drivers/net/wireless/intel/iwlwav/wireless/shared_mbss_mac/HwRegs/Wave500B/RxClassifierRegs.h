
/***********************************************************************************
File:				RxClassifierRegs.h
Module:				rxClassifier
SOC Revision:		843
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _RX_CLASSIFIER_REGS_H_
#define _RX_CLASSIFIER_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define RX_CLASSIFIER_BASE_ADDRESS                             MEMORY_MAP_UNIT_57_BASE_ADDRESS
#define	REG_RX_CLASSIFIER_FRAME_CLASS_FILTER_CONTROL    (RX_CLASSIFIER_BASE_ADDRESS + 0x0)
#define	REG_RX_CLASSIFIER_FRAME_DATA_SUBTYPES_CLASS     (RX_CLASSIFIER_BASE_ADDRESS + 0x4)
#define	REG_RX_CLASSIFIER_FRAME_CTRL_SUBTYPES_CLASS     (RX_CLASSIFIER_BASE_ADDRESS + 0x8)
#define	REG_RX_CLASSIFIER_FRAME_MGMT_SUBTYPES_CLASS     (RX_CLASSIFIER_BASE_ADDRESS + 0xC)
#define	REG_RX_CLASSIFIER_FRAME_MGMT_ACTION_CLASS       (RX_CLASSIFIER_BASE_ADDRESS + 0x10)
#define	REG_RX_CLASSIFIER_FRAME_CLASS_STA_0TO15         (RX_CLASSIFIER_BASE_ADDRESS + 0x20)
#define	REG_RX_CLASSIFIER_FRAME_CLASS_STA_16TO31        (RX_CLASSIFIER_BASE_ADDRESS + 0x24)
#define	REG_RX_CLASSIFIER_FRAME_CLASS_STA_32TO47        (RX_CLASSIFIER_BASE_ADDRESS + 0x28)
#define	REG_RX_CLASSIFIER_FRAME_CLASS_STA_48TO63        (RX_CLASSIFIER_BASE_ADDRESS + 0x2C)
#define	REG_RX_CLASSIFIER_FRAME_CLASS_STA_64TO79        (RX_CLASSIFIER_BASE_ADDRESS + 0x30)
#define	REG_RX_CLASSIFIER_FRAME_CLASS_STA_80TO95        (RX_CLASSIFIER_BASE_ADDRESS + 0x34)
#define	REG_RX_CLASSIFIER_FRAME_CLASS_STA_96TO111       (RX_CLASSIFIER_BASE_ADDRESS + 0x38)
#define	REG_RX_CLASSIFIER_FRAME_CLASS_STA_112TO127      (RX_CLASSIFIER_BASE_ADDRESS + 0x3C)
#define	REG_RX_CLASSIFIER_CLASS_VIOL_VALID_ADDR         (RX_CLASSIFIER_BASE_ADDRESS + 0x40)
#define	REG_RX_CLASSIFIER_CLASS_VIOL_CLR_VALID_ADD      (RX_CLASSIFIER_BASE_ADDRESS + 0x44)
#define	REG_RX_CLASSIFIER_CLASS_VIOL_IRQ                (RX_CLASSIFIER_BASE_ADDRESS + 0x48)
#define	REG_RX_CLASSIFIER_CLASS_VIOL_CLR_IRQ            (RX_CLASSIFIER_BASE_ADDRESS + 0x4C)
#define	REG_RX_CLASSIFIER_CLASS_VIOL_BUF_FULL_CNT       (RX_CLASSIFIER_BASE_ADDRESS + 0x50)
#define	REG_RX_CLASSIFIER_RXQ_ID_FILTER0                (RX_CLASSIFIER_BASE_ADDRESS + 0x60)
#define	REG_RX_CLASSIFIER_RXQ_ID_FILTER1                (RX_CLASSIFIER_BASE_ADDRESS + 0x64)
#define	REG_RX_CLASSIFIER_RXQ_ID_FILTER2                (RX_CLASSIFIER_BASE_ADDRESS + 0x68)
#define	REG_RX_CLASSIFIER_RXQ_ID_FILTER3                (RX_CLASSIFIER_BASE_ADDRESS + 0x6C)
#define	REG_RX_CLASSIFIER_RXQ_ID_FILTER4                (RX_CLASSIFIER_BASE_ADDRESS + 0x70)
#define	REG_RX_CLASSIFIER_RXQ_ID_FILTER5                (RX_CLASSIFIER_BASE_ADDRESS + 0x74)
#define	REG_RX_CLASSIFIER_RXQ_ID_FILTER6                (RX_CLASSIFIER_BASE_ADDRESS + 0x78)
#define	REG_RX_CLASSIFIER_RXQ_ID_FILTER7                (RX_CLASSIFIER_BASE_ADDRESS + 0x7C)
#define	REG_RX_CLASSIFIER_RXQ_ID_FILTER8                (RX_CLASSIFIER_BASE_ADDRESS + 0x80)
#define	REG_RX_CLASSIFIER_RXQ_ID_FILTER9                (RX_CLASSIFIER_BASE_ADDRESS + 0x84)
#define	REG_RX_CLASSIFIER_RXQ_ID_FILTER10               (RX_CLASSIFIER_BASE_ADDRESS + 0x88)
#define	REG_RX_CLASSIFIER_RXQ_ID_FILTER11               (RX_CLASSIFIER_BASE_ADDRESS + 0x8C)
#define	REG_RX_CLASSIFIER_RXQ_ID_FILTER12               (RX_CLASSIFIER_BASE_ADDRESS + 0x90)
#define	REG_RX_CLASSIFIER_RXQ_ID_FILTER13               (RX_CLASSIFIER_BASE_ADDRESS + 0x94)
#define	REG_RX_CLASSIFIER_RXQ_ID_FILTER14               (RX_CLASSIFIER_BASE_ADDRESS + 0x98)
#define	REG_RX_CLASSIFIER_RXQ_ID_FILTER15               (RX_CLASSIFIER_BASE_ADDRESS + 0x9C)
#define	REG_RX_CLASSIFIER_RXQ_ID_FILTER_CONTROL         (RX_CLASSIFIER_BASE_ADDRESS + 0xA0)
#define	REG_RX_CLASSIFIER_RX_CLASSIFIER_ENABLE          (RX_CLASSIFIER_BASE_ADDRESS + 0xB0)
#define	REG_RX_CLASSIFIER_WEP_KEY_IDX_DB_LOCATION       (RX_CLASSIFIER_BASE_ADDRESS + 0xB4)
#define	REG_RX_CLASSIFIER_MAX_SEC_DESCRIPTORS           (RX_CLASSIFIER_BASE_ADDRESS + 0xB8)
#define	REG_RX_CLASSIFIER_SEC_DESC_DEBUG                (RX_CLASSIFIER_BASE_ADDRESS + 0xBC)
#define	REG_RX_CLASSIFIER_SEC_DESC_DEBUG_CLR            (RX_CLASSIFIER_BASE_ADDRESS + 0xC0)
#define	REG_RX_CLASSIFIER_REKEY_ACT_STA_0TO31           (RX_CLASSIFIER_BASE_ADDRESS + 0xC4)
#define	REG_RX_CLASSIFIER_REKEY_ACT_STA_32TO63          (RX_CLASSIFIER_BASE_ADDRESS + 0xC8)
#define	REG_RX_CLASSIFIER_REKEY_ACT_STA_64TO95          (RX_CLASSIFIER_BASE_ADDRESS + 0xCC)
#define	REG_RX_CLASSIFIER_REKEY_ACT_STA_96TO127         (RX_CLASSIFIER_BASE_ADDRESS + 0xD0)
#define	REG_RX_CLASSIFIER_REKEY_ACT_VAP_0TO15           (RX_CLASSIFIER_BASE_ADDRESS + 0xD4)
#define	REG_RX_CLASSIFIER_REKEY_IRQ_L2_STA_0TO31        (RX_CLASSIFIER_BASE_ADDRESS + 0xD8)
#define	REG_RX_CLASSIFIER_REKEY_IRQ_L2_STA_32TO63       (RX_CLASSIFIER_BASE_ADDRESS + 0xDC)
#define	REG_RX_CLASSIFIER_REKEY_IRQ_L2_STA_64TO95       (RX_CLASSIFIER_BASE_ADDRESS + 0xE0)
#define	REG_RX_CLASSIFIER_REKEY_IRQ_L2_STA_96TO127      (RX_CLASSIFIER_BASE_ADDRESS + 0xE4)
#define	REG_RX_CLASSIFIER_REKEY_IRQ_L2_VAP_0TO15        (RX_CLASSIFIER_BASE_ADDRESS + 0xE8)
#define	REG_RX_CLASSIFIER_REKEY_IRQ_L1_STATUS           (RX_CLASSIFIER_BASE_ADDRESS + 0xEC)
#define	REG_RX_CLASSIFIER_REKEY_CLR_ALL                 (RX_CLASSIFIER_BASE_ADDRESS + 0xF0)
#define	REG_RX_CLASSIFIER_REKEY_CTRL_STA_IDX            (RX_CLASSIFIER_BASE_ADDRESS + 0xF4)
#define	REG_RX_CLASSIFIER_REKEY_CTRL_VAP_IDX            (RX_CLASSIFIER_BASE_ADDRESS + 0xF8)
#define	REG_RX_CLASSIFIER_RX_CLAS_RTD_CTRL              (RX_CLASSIFIER_BASE_ADDRESS + 0x100)
#define	REG_RX_CLASSIFIER_MAX_RD_TEMPLATES              (RX_CLASSIFIER_BASE_ADDRESS + 0x104)
#define	REG_RX_CLASSIFIER_RTD_DEBUG                     (RX_CLASSIFIER_BASE_ADDRESS + 0x108)
#define	REG_RX_CLASSIFIER_RTD_DEBUG_CLR                 (RX_CLASSIFIER_BASE_ADDRESS + 0x10C)
#define	REG_RX_CLASSIFIER_RX_CLAS_ACC_DEC               (RX_CLASSIFIER_BASE_ADDRESS + 0x110)
#define	REG_RX_CLASSIFIER_RX_CLAS_SM_DBG                (RX_CLASSIFIER_BASE_ADDRESS + 0x114)
#define	REG_RX_CLASSIFIER_RX_CLAS_CIR_BUF_PTR           (RX_CLASSIFIER_BASE_ADDRESS + 0x118)
#define	REG_RX_CLASSIFIER_RX_CLAS_CIR_BUF_PTR_CLR       (RX_CLASSIFIER_BASE_ADDRESS + 0x11C)
#define	REG_RX_CLASSIFIER_RX_CLAS_ERR_IRQ               (RX_CLASSIFIER_BASE_ADDRESS + 0x120)
#define	REG_RX_CLASSIFIER_RX_CLAS_ERR_IRQ_EN            (RX_CLASSIFIER_BASE_ADDRESS + 0x124)
#define	REG_RX_CLASSIFIER_RX_CLAS_ERR_IRQ_CLR           (RX_CLASSIFIER_BASE_ADDRESS + 0x128)
#define	REG_RX_CLASSIFIER_RX_CLAS_STALLING_MAX_TIME     (RX_CLASSIFIER_BASE_ADDRESS + 0x12C)
#define	REG_RX_CLASSIFIER_RX_CLAS_STALLING_TIMER        (RX_CLASSIFIER_BASE_ADDRESS + 0x130)
#define	REG_RX_CLASSIFIER_RX_CLAS_LOGGER_CONTROL        (RX_CLASSIFIER_BASE_ADDRESS + 0x134)
#define	REG_RX_CLASSIFIER_RX_CLAS_LOGGER_ACTIVE         (RX_CLASSIFIER_BASE_ADDRESS + 0x138)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_RX_CLASSIFIER_FRAME_CLASS_FILTER_CONTROL 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 frameClassFilterAddr2ValidDropEn:1;	// Enable frame class drop filter when Addr2 index is valid
		uint32 frameClassFilterAddr2InvalidDropEn:1;	// Enable frame class drop filter when Addr2 index is invalid
		uint32 vapStaMisFilterDropEn:1;	// Enable VAP-STA mismatch drop filter
		uint32 reserved0:1;
		uint32 addr2InvalidDropMpduClassVal:2;	// MPDU frame class to be dropped in case of addr2 index is not valid
		uint32 vapStaMisDropMpduClassVal:2;	// MPDU frame class to be dropped in case of VAP-STA mismatch
		uint32 reserved1:24;
	} bitFields;
} RegRxClassifierFrameClassFilterControl_u;

/*REG_RX_CLASSIFIER_FRAME_DATA_SUBTYPES_CLASS 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 frameDataSubtypesClass:32;
	} bitFields;
} RegRxClassifierFrameDataSubtypesClass_u;

/*REG_RX_CLASSIFIER_FRAME_CTRL_SUBTYPES_CLASS 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 frameCtrlSubtypesClass:32;
	} bitFields;
} RegRxClassifierFrameCtrlSubtypesClass_u;

/*REG_RX_CLASSIFIER_FRAME_MGMT_SUBTYPES_CLASS 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 frameMgmtSubtypesClass:32;
	} bitFields;
} RegRxClassifierFrameMgmtSubtypesClass_u;

/*REG_RX_CLASSIFIER_FRAME_MGMT_ACTION_CLASS 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 frameMgmtAction4Class:2;
		uint32 frameMgmtAction15Class:2;
		uint32 frameMgmtActionOtherClass:2;
		uint32 reserved0:26;
	} bitFields;
} RegRxClassifierFrameMgmtActionClass_u;

/*REG_RX_CLASSIFIER_FRAME_CLASS_STA_0TO15 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 frameClassSta0To15:32;
	} bitFields;
} RegRxClassifierFrameClassSta0To15_u;

/*REG_RX_CLASSIFIER_FRAME_CLASS_STA_16TO31 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 frameClassSta16To31:32;
	} bitFields;
} RegRxClassifierFrameClassSta16To31_u;

/*REG_RX_CLASSIFIER_FRAME_CLASS_STA_32TO47 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 frameClassSta32To47:32;
	} bitFields;
} RegRxClassifierFrameClassSta32To47_u;

/*REG_RX_CLASSIFIER_FRAME_CLASS_STA_48TO63 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 frameClassSta48To63:32;
	} bitFields;
} RegRxClassifierFrameClassSta48To63_u;

/*REG_RX_CLASSIFIER_FRAME_CLASS_STA_64TO79 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 frameClassSta64To79:32;
	} bitFields;
} RegRxClassifierFrameClassSta64To79_u;

/*REG_RX_CLASSIFIER_FRAME_CLASS_STA_80TO95 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 frameClassSta80To95:32;
	} bitFields;
} RegRxClassifierFrameClassSta80To95_u;

/*REG_RX_CLASSIFIER_FRAME_CLASS_STA_96TO111 0x38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 frameClassSta96To111:32;
	} bitFields;
} RegRxClassifierFrameClassSta96To111_u;

/*REG_RX_CLASSIFIER_FRAME_CLASS_STA_112TO127 0x3C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 frameClassSta112To127:32;
	} bitFields;
} RegRxClassifierFrameClassSta112To127_u;

/*REG_RX_CLASSIFIER_CLASS_VIOL_VALID_ADDR 0x40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 frameClassViolValidMacAddr:16;
		uint32 reserved0:16;
	} bitFields;
} RegRxClassifierClassViolValidAddr_u;

/*REG_RX_CLASSIFIER_CLASS_VIOL_CLR_VALID_ADD 0x44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 frameClassViolClrValidMacAddr:16;
		uint32 reserved0:16;
	} bitFields;
} RegRxClassifierClassViolClrValidAdd_u;

/*REG_RX_CLASSIFIER_CLASS_VIOL_IRQ 0x48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 frameClassViolIrq:16;
		uint32 reserved0:16;
	} bitFields;
} RegRxClassifierClassViolIrq_u;

/*REG_RX_CLASSIFIER_CLASS_VIOL_CLR_IRQ 0x4C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 frameClassViolClrIrq:16;
		uint32 reserved0:16;
	} bitFields;
} RegRxClassifierClassViolClrIrq_u;

/*REG_RX_CLASSIFIER_CLASS_VIOL_BUF_FULL_CNT 0x50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 frameClassViolBufFullCnt:16;
		uint32 reserved0:16;
	} bitFields;
} RegRxClassifierClassViolBufFullCnt_u;

/*REG_RX_CLASSIFIER_RXQ_ID_FILTER0 0x60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxqIdFilter0DropEn:1;	// Enable RXQ-ID filter drop indication
		uint32 reserved0:1;
		uint32 rxqIdFilter0FrameType:2;	// Field frame type in RXQ-ID filter
		uint32 rxqIdFilter0TypeNa:1;	// Field frame type in RXQ-ID filter is N/A
		uint32 rxqIdFilter0FrameSubtype:4;	// Field frame subtype in RXQ-ID filter
		uint32 rxqIdFilter0SubtypeNa:1;	// Field frame subtype in RXQ-ID filter is N/A
		uint32 rxqIdFilter0SubtypeEqual:1;	// Subtype compare logic: , 1: logic of equal , 0: logic of not equal
		uint32 rxqIdFilter0Addr1IdVal:1;	// Address1 index valid bit in RXQ-ID filter
		uint32 rxqIdFilter0Addr1IdValNa:1;	// Address1 index valid bit in RXQ-ID filter is N/A
		uint32 rxqIdFilter0Addr2IdVal:1;	// Address2 index valid bit in RXQ-ID filter
		uint32 rxqIdFilter0Addr2IdValNa:1;	// Address2 index valid bit in RXQ-ID filter is N/A
		uint32 rxqIdFilter0Addr1Unicast:1;	// Address1 unicast bit in RXQ-ID filter
		uint32 rxqIdFilter0Addr1UnicastNa:1;	// Address1 unicast bit in RXQ-ID filter is N/A
		uint32 reserved1:15;
	} bitFields;
} RegRxClassifierRxqIdFilter0_u;

/*REG_RX_CLASSIFIER_RXQ_ID_FILTER1 0x64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxqIdFilter1DropEn:1;	// Enable RXQ-ID filter drop indication
		uint32 reserved0:1;
		uint32 rxqIdFilter1FrameType:2;	// Field frame type in RXQ-ID filter
		uint32 rxqIdFilter1TypeNa:1;	// Field frame type in RXQ-ID filter is N/A
		uint32 rxqIdFilter1FrameSubtype:4;	// Field frame subtype in RXQ-ID filter
		uint32 rxqIdFilter1SubtypeNa:1;	// Field frame subtype in RXQ-ID filter is N/A
		uint32 rxqIdFilter1SubtypeEqual:1;	// Subtype compare logic: , 1: logic of equal , 0: logic of not equal
		uint32 rxqIdFilter1Addr1IdVal:1;	// Address1 index valid bit in RXQ-ID filter
		uint32 rxqIdFilter1Addr1IdValNa:1;	// Address1 index valid bit in RXQ-ID filter is N/A
		uint32 rxqIdFilter1Addr2IdVal:1;	// Address2 index valid bit in RXQ-ID filter
		uint32 rxqIdFilter1Addr2IdValNa:1;	// Address2 index valid bit in RXQ-ID filter is N/A
		uint32 rxqIdFilter1Addr1Unicast:1;	// Address1 unicast bit in RXQ-ID filter
		uint32 rxqIdFilter1Addr1UnicastNa:1;	// Address1 unicast bit in RXQ-ID filter is N/A
		uint32 reserved1:15;
	} bitFields;
} RegRxClassifierRxqIdFilter1_u;

/*REG_RX_CLASSIFIER_RXQ_ID_FILTER2 0x68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxqIdFilter2DropEn:1;	// Enable RXQ-ID filter drop indication
		uint32 reserved0:1;
		uint32 rxqIdFilter2FrameType:2;	// Field frame type in RXQ-ID filter
		uint32 rxqIdFilter2TypeNa:1;	// Field frame type in RXQ-ID filter is N/A
		uint32 rxqIdFilter2FrameSubtype:4;	// Field frame subtype in RXQ-ID filter
		uint32 rxqIdFilter2SubtypeNa:1;	// Field frame subtype in RXQ-ID filter is N/A
		uint32 rxqIdFilter2SubtypeEqual:1;	// Subtype compare logic: , 1: logic of equal , 0: logic of not equal
		uint32 rxqIdFilter2Addr1IdVal:1;	// Address1 index valid bit in RXQ-ID filter
		uint32 rxqIdFilter2Addr1IdValNa:1;	// Address1 index valid bit in RXQ-ID filter is N/A
		uint32 rxqIdFilter2Addr2IdVal:1;	// Address2 index valid bit in RXQ-ID filter
		uint32 rxqIdFilter2Addr2IdValNa:1;	// Address2 index valid bit in RXQ-ID filter is N/A
		uint32 rxqIdFilter2Addr1Unicast:1;	// Address1 unicast bit in RXQ-ID filter
		uint32 rxqIdFilter2Addr1UnicastNa:1;	// Address1 unicast bit in RXQ-ID filter is N/A
		uint32 reserved1:15;
	} bitFields;
} RegRxClassifierRxqIdFilter2_u;

/*REG_RX_CLASSIFIER_RXQ_ID_FILTER3 0x6C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxqIdFilter3DropEn:1;	// Enable RXQ-ID filter drop indication
		uint32 reserved0:1;
		uint32 rxqIdFilter3FrameType:2;	// Field frame type in RXQ-ID filter
		uint32 rxqIdFilter3TypeNa:1;	// Field frame type in RXQ-ID filter is N/A
		uint32 rxqIdFilter3FrameSubtype:4;	// Field frame subtype in RXQ-ID filter
		uint32 rxqIdFilter3SubtypeNa:1;	// Field frame subtype in RXQ-ID filter is N/A
		uint32 rxqIdFilter3SubtypeEqual:1;	// Subtype compare logic: , 1: logic of equal , 0: logic of not equal
		uint32 rxqIdFilter3Addr1IdVal:1;	// Address1 index valid bit in RXQ-ID filter
		uint32 rxqIdFilter3Addr1IdValNa:1;	// Address1 index valid bit in RXQ-ID filter is N/A
		uint32 rxqIdFilter3Addr2IdVal:1;	// Address2 index valid bit in RXQ-ID filter
		uint32 rxqIdFilter3Addr2IdValNa:1;	// Address2 index valid bit in RXQ-ID filter is N/A
		uint32 rxqIdFilter3Addr1Unicast:1;	// Address1 unicast bit in RXQ-ID filter
		uint32 rxqIdFilter3Addr1UnicastNa:1;	// Address1 unicast bit in RXQ-ID filter is N/A
		uint32 reserved1:15;
	} bitFields;
} RegRxClassifierRxqIdFilter3_u;

/*REG_RX_CLASSIFIER_RXQ_ID_FILTER4 0x70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxqIdFilter4DropEn:1;	// Enable RXQ-ID filter drop indication
		uint32 reserved0:1;
		uint32 rxqIdFilter4FrameType:2;	// Field frame type in RXQ-ID filter
		uint32 rxqIdFilter4TypeNa:1;	// Field frame type in RXQ-ID filter is N/A
		uint32 rxqIdFilter4FrameSubtype:4;	// Field frame subtype in RXQ-ID filter
		uint32 rxqIdFilter4SubtypeNa:1;	// Field frame subtype in RXQ-ID filter is N/A
		uint32 rxqIdFilter4SubtypeEqual:1;	// Subtype compare logic: , 1: logic of equal , 0: logic of not equal
		uint32 rxqIdFilter4Addr1IdVal:1;	// Address1 index valid bit in RXQ-ID filter
		uint32 rxqIdFilter4Addr1IdValNa:1;	// Address1 index valid bit in RXQ-ID filter is N/A
		uint32 rxqIdFilter4Addr2IdVal:1;	// Address2 index valid bit in RXQ-ID filter
		uint32 rxqIdFilter4Addr2IdValNa:1;	// Address2 index valid bit in RXQ-ID filter is N/A
		uint32 rxqIdFilter4Addr1Unicast:1;	// Address1 unicast bit in RXQ-ID filter
		uint32 rxqIdFilter4Addr1UnicastNa:1;	// Address1 unicast bit in RXQ-ID filter is N/A
		uint32 reserved1:15;
	} bitFields;
} RegRxClassifierRxqIdFilter4_u;

/*REG_RX_CLASSIFIER_RXQ_ID_FILTER5 0x74 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxqIdFilter5DropEn:1;	// Enable RXQ-ID filter drop indication
		uint32 reserved0:1;
		uint32 rxqIdFilter5FrameType:2;	// Field frame type in RXQ-ID filter
		uint32 rxqIdFilter5TypeNa:1;	// Field frame type in RXQ-ID filter is N/A
		uint32 rxqIdFilter5FrameSubtype:4;	// Field frame subtype in RXQ-ID filter
		uint32 rxqIdFilter5SubtypeNa:1;	// Field frame subtype in RXQ-ID filter is N/A
		uint32 rxqIdFilter5SubtypeEqual:1;	// Subtype compare logic: , 1: logic of equal , 0: logic of not equal
		uint32 rxqIdFilter5Addr1IdVal:1;	// Address1 index valid bit in RXQ-ID filter
		uint32 rxqIdFilter5Addr1IdValNa:1;	// Address1 index valid bit in RXQ-ID filter is N/A
		uint32 rxqIdFilter5Addr2IdVal:1;	// Address2 index valid bit in RXQ-ID filter
		uint32 rxqIdFilter5Addr2IdValNa:1;	// Address2 index valid bit in RXQ-ID filter is N/A
		uint32 rxqIdFilter5Addr1Unicast:1;	// Address1 unicast bit in RXQ-ID filter
		uint32 rxqIdFilter5Addr1UnicastNa:1;	// Address1 unicast bit in RXQ-ID filter is N/A
		uint32 reserved1:15;
	} bitFields;
} RegRxClassifierRxqIdFilter5_u;

/*REG_RX_CLASSIFIER_RXQ_ID_FILTER6 0x78 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxqIdFilter6DropEn:1;	// Enable RXQ-ID filter drop indication
		uint32 reserved0:1;
		uint32 rxqIdFilter6FrameType:2;	// Field frame type in RXQ-ID filter
		uint32 rxqIdFilter6TypeNa:1;	// Field frame type in RXQ-ID filter is N/A
		uint32 rxqIdFilter6FrameSubtype:4;	// Field frame subtype in RXQ-ID filter
		uint32 rxqIdFilter6SubtypeNa:1;	// Field frame subtype in RXQ-ID filter is N/A
		uint32 rxqIdFilter6SubtypeEqual:1;	// Subtype compare logic: , 1: logic of equal , 0: logic of not equal
		uint32 rxqIdFilter6Addr1IdVal:1;	// Address1 index valid bit in RXQ-ID filter
		uint32 rxqIdFilter6Addr1IdValNa:1;	// Address1 index valid bit in RXQ-ID filter is N/A
		uint32 rxqIdFilter6Addr2IdVal:1;	// Address2 index valid bit in RXQ-ID filter
		uint32 rxqIdFilter6Addr2IdValNa:1;	// Address2 index valid bit in RXQ-ID filter is N/A
		uint32 rxqIdFilter6Addr1Unicast:1;	// Address1 unicast bit in RXQ-ID filter
		uint32 rxqIdFilter6Addr1UnicastNa:1;	// Address1 unicast bit in RXQ-ID filter is N/A
		uint32 reserved1:15;
	} bitFields;
} RegRxClassifierRxqIdFilter6_u;

/*REG_RX_CLASSIFIER_RXQ_ID_FILTER7 0x7C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxqIdFilter7DropEn:1;	// Enable RXQ-ID filter drop indication
		uint32 reserved0:1;
		uint32 rxqIdFilter7FrameType:2;	// Field frame type in RXQ-ID filter
		uint32 rxqIdFilter7TypeNa:1;	// Field frame type in RXQ-ID filter is N/A
		uint32 rxqIdFilter7FrameSubtype:4;	// Field frame subtype in RXQ-ID filter
		uint32 rxqIdFilter7SubtypeNa:1;	// Field frame subtype in RXQ-ID filter is N/A
		uint32 rxqIdFilter7SubtypeEqual:1;	// Subtype compare logic: , 1: logic of equal , 0: logic of not equal
		uint32 rxqIdFilter7Addr1IdVal:1;	// Address1 index valid bit in RXQ-ID filter
		uint32 rxqIdFilter7Addr1IdValNa:1;	// Address1 index valid bit in RXQ-ID filter is N/A
		uint32 rxqIdFilter7Addr2IdVal:1;	// Address2 index valid bit in RXQ-ID filter
		uint32 rxqIdFilter7Addr2IdValNa:1;	// Address2 index valid bit in RXQ-ID filter is N/A
		uint32 rxqIdFilter7Addr1Unicast:1;	// Address1 unicast bit in RXQ-ID filter
		uint32 rxqIdFilter7Addr1UnicastNa:1;	// Address1 unicast bit in RXQ-ID filter is N/A
		uint32 reserved1:15;
	} bitFields;
} RegRxClassifierRxqIdFilter7_u;

/*REG_RX_CLASSIFIER_RXQ_ID_FILTER8 0x80 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxqIdFilter8DropEn:1;	// Enable RXQ-ID filter drop indication
		uint32 reserved0:1;
		uint32 rxqIdFilter8FrameType:2;	// Field frame type in RXQ-ID filter
		uint32 rxqIdFilter8TypeNa:1;	// Field frame type in RXQ-ID filter is N/A
		uint32 rxqIdFilter8FrameSubtype:4;	// Field frame subtype in RXQ-ID filter
		uint32 rxqIdFilter8SubtypeNa:1;	// Field frame subtype in RXQ-ID filter is N/A
		uint32 rxqIdFilter8SubtypeEqual:1;	// Subtype compare logic: , 1: logic of equal , 0: logic of not equal
		uint32 rxqIdFilter8Addr1IdVal:1;	// Address1 index valid bit in RXQ-ID filter
		uint32 rxqIdFilter8Addr1IdValNa:1;	// Address1 index valid bit in RXQ-ID filter is N/A
		uint32 rxqIdFilter8Addr2IdVal:1;	// Address2 index valid bit in RXQ-ID filter
		uint32 rxqIdFilter8Addr2IdValNa:1;	// Address2 index valid bit in RXQ-ID filter is N/A
		uint32 rxqIdFilter8Addr1Unicast:1;	// Address1 unicast bit in RXQ-ID filter
		uint32 rxqIdFilter8Addr1UnicastNa:1;	// Address1 unicast bit in RXQ-ID filter is N/A
		uint32 reserved1:15;
	} bitFields;
} RegRxClassifierRxqIdFilter8_u;

/*REG_RX_CLASSIFIER_RXQ_ID_FILTER9 0x84 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxqIdFilter9DropEn:1;	// Enable RXQ-ID filter drop indication
		uint32 reserved0:1;
		uint32 rxqIdFilter9FrameType:2;	// Field frame type in RXQ-ID filter
		uint32 rxqIdFilter9TypeNa:1;	// Field frame type in RXQ-ID filter is N/A
		uint32 rxqIdFilter9FrameSubtype:4;	// Field frame subtype in RXQ-ID filter
		uint32 rxqIdFilter9SubtypeNa:1;	// Field frame subtype in RXQ-ID filter is N/A
		uint32 rxqIdFilter9SubtypeEqual:1;	// Subtype compare logic: , 1: logic of equal , 0: logic of not equal
		uint32 rxqIdFilter9Addr1IdVal:1;	// Address1 index valid bit in RXQ-ID filter
		uint32 rxqIdFilter9Addr1IdValNa:1;	// Address1 index valid bit in RXQ-ID filter is N/A
		uint32 rxqIdFilter9Addr2IdVal:1;	// Address2 index valid bit in RXQ-ID filter
		uint32 rxqIdFilter9Addr2IdValNa:1;	// Address2 index valid bit in RXQ-ID filter is N/A
		uint32 rxqIdFilter9Addr1Unicast:1;	// Address1 unicast bit in RXQ-ID filter
		uint32 rxqIdFilter9Addr1UnicastNa:1;	// Address1 unicast bit in RXQ-ID filter is N/A
		uint32 reserved1:15;
	} bitFields;
} RegRxClassifierRxqIdFilter9_u;

/*REG_RX_CLASSIFIER_RXQ_ID_FILTER10 0x88 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxqIdFilter10DropEn:1;	// Enable RXQ-ID filter drop indication
		uint32 reserved0:1;
		uint32 rxqIdFilter10FrameType:2;	// Field frame type in RXQ-ID filter
		uint32 rxqIdFilter10TypeNa:1;	// Field frame type in RXQ-ID filter is N/A
		uint32 rxqIdFilter10FrameSubtype:4;	// Field frame subtype in RXQ-ID filter
		uint32 rxqIdFilter10SubtypeNa:1;	// Field frame subtype in RXQ-ID filter is N/A
		uint32 rxqIdFilter10SubtypeEqual:1;	// Subtype compare logic: , 1: logic of equal , 0: logic of not equal
		uint32 rxqIdFilter10Addr1IdVal:1;	// Address1 index valid bit in RXQ-ID filter
		uint32 rxqIdFilter10Addr1IdValNa:1;	// Address1 index valid bit in RXQ-ID filter is N/A
		uint32 rxqIdFilter10Addr2IdVal:1;	// Address2 index valid bit in RXQ-ID filter
		uint32 rxqIdFilter10Addr2IdValNa:1;	// Address2 index valid bit in RXQ-ID filter is N/A
		uint32 rxqIdFilter10Addr1Unicast:1;	// Address1 unicast bit in RXQ-ID filter
		uint32 rxqIdFilter10Addr1UnicastNa:1;	// Address1 unicast bit in RXQ-ID filter is N/A
		uint32 reserved1:15;
	} bitFields;
} RegRxClassifierRxqIdFilter10_u;

/*REG_RX_CLASSIFIER_RXQ_ID_FILTER11 0x8C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxqIdFilter11DropEn:1;	// Enable RXQ-ID filter drop indication
		uint32 reserved0:1;
		uint32 rxqIdFilter11FrameType:2;	// Field frame type in RXQ-ID filter
		uint32 rxqIdFilter11TypeNa:1;	// Field frame type in RXQ-ID filter is N/A
		uint32 rxqIdFilter11FrameSubtype:4;	// Field frame subtype in RXQ-ID filter
		uint32 rxqIdFilter11SubtypeNa:1;	// Field frame subtype in RXQ-ID filter is N/A
		uint32 rxqIdFilter11SubtypeEqual:1;	// Subtype compare logic: , 1: logic of equal , 0: logic of not equal
		uint32 rxqIdFilter11Addr1IdVal:1;	// Address1 index valid bit in RXQ-ID filter
		uint32 rxqIdFilter11Addr1IdValNa:1;	// Address1 index valid bit in RXQ-ID filter is N/A
		uint32 rxqIdFilter11Addr2IdVal:1;	// Address2 index valid bit in RXQ-ID filter
		uint32 rxqIdFilter11Addr2IdValNa:1;	// Address2 index valid bit in RXQ-ID filter is N/A
		uint32 rxqIdFilter11Addr1Unicast:1;	// Address1 unicast bit in RXQ-ID filter
		uint32 rxqIdFilter11Addr1UnicastNa:1;	// Address1 unicast bit in RXQ-ID filter is N/A
		uint32 reserved1:15;
	} bitFields;
} RegRxClassifierRxqIdFilter11_u;

/*REG_RX_CLASSIFIER_RXQ_ID_FILTER12 0x90 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxqIdFilter12DropEn:1;	// Enable RXQ-ID filter drop indication
		uint32 reserved0:1;
		uint32 rxqIdFilter12FrameType:2;	// Field frame type in RXQ-ID filter
		uint32 rxqIdFilter12TypeNa:1;	// Field frame type in RXQ-ID filter is N/A
		uint32 rxqIdFilter12FrameSubtype:4;	// Field frame subtype in RXQ-ID filter
		uint32 rxqIdFilter12SubtypeNa:1;	// Field frame subtype in RXQ-ID filter is N/A
		uint32 rxqIdFilter12SubtypeEqual:1;	// Subtype compare logic: , 1: logic of equal , 0: logic of not equal
		uint32 rxqIdFilter12Addr1IdVal:1;	// Address1 index valid bit in RXQ-ID filter
		uint32 rxqIdFilter12Addr1IdValNa:1;	// Address1 index valid bit in RXQ-ID filter is N/A
		uint32 rxqIdFilter12Addr2IdVal:1;	// Address2 index valid bit in RXQ-ID filter
		uint32 rxqIdFilter12Addr2IdValNa:1;	// Address2 index valid bit in RXQ-ID filter is N/A
		uint32 rxqIdFilter12Addr1Unicast:1;	// Address1 unicast bit in RXQ-ID filter
		uint32 rxqIdFilter12Addr1UnicastNa:1;	// Address1 unicast bit in RXQ-ID filter is N/A
		uint32 reserved1:15;
	} bitFields;
} RegRxClassifierRxqIdFilter12_u;

/*REG_RX_CLASSIFIER_RXQ_ID_FILTER13 0x94 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxqIdFilter13DropEn:1;	// Enable RXQ-ID filter drop indication
		uint32 reserved0:1;
		uint32 rxqIdFilter13FrameType:2;	// Field frame type in RXQ-ID filter
		uint32 rxqIdFilter13TypeNa:1;	// Field frame type in RXQ-ID filter is N/A
		uint32 rxqIdFilter13FrameSubtype:4;	// Field frame subtype in RXQ-ID filter
		uint32 rxqIdFilter13SubtypeNa:1;	// Field frame subtype in RXQ-ID filter is N/A
		uint32 rxqIdFilter13SubtypeEqual:1;	// Subtype compare logic: , 1: logic of equal , 0: logic of not equal
		uint32 rxqIdFilter13Addr1IdVal:1;	// Address1 index valid bit in RXQ-ID filter
		uint32 rxqIdFilter13Addr1IdValNa:1;	// Address1 index valid bit in RXQ-ID filter is N/A
		uint32 rxqIdFilter13Addr2IdVal:1;	// Address2 index valid bit in RXQ-ID filter
		uint32 rxqIdFilter13Addr2IdValNa:1;	// Address2 index valid bit in RXQ-ID filter is N/A
		uint32 rxqIdFilter13Addr1Unicast:1;	// Address1 unicast bit in RXQ-ID filter
		uint32 rxqIdFilter13Addr1UnicastNa:1;	// Address1 unicast bit in RXQ-ID filter is N/A
		uint32 reserved1:15;
	} bitFields;
} RegRxClassifierRxqIdFilter13_u;

/*REG_RX_CLASSIFIER_RXQ_ID_FILTER14 0x98 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxqIdFilter14DropEn:1;	// Enable RXQ-ID filter drop indication
		uint32 reserved0:1;
		uint32 rxqIdFilter14FrameType:2;	// Field frame type in RXQ-ID filter
		uint32 rxqIdFilter14TypeNa:1;	// Field frame type in RXQ-ID filter is N/A
		uint32 rxqIdFilter14FrameSubtype:4;	// Field frame subtype in RXQ-ID filter
		uint32 rxqIdFilter14SubtypeNa:1;	// Field frame subtype in RXQ-ID filter is N/A
		uint32 rxqIdFilter14SubtypeEqual:1;	// Subtype compare logic: , 1: logic of equal , 0: logic of not equal
		uint32 rxqIdFilter14Addr1IdVal:1;	// Address1 index valid bit in RXQ-ID filter
		uint32 rxqIdFilter14Addr1IdValNa:1;	// Address1 index valid bit in RXQ-ID filter is N/A
		uint32 rxqIdFilter14Addr2IdVal:1;	// Address2 index valid bit in RXQ-ID filter
		uint32 rxqIdFilter14Addr2IdValNa:1;	// Address2 index valid bit in RXQ-ID filter is N/A
		uint32 rxqIdFilter14Addr1Unicast:1;	// Address1 unicast bit in RXQ-ID filter
		uint32 rxqIdFilter14Addr1UnicastNa:1;	// Address1 unicast bit in RXQ-ID filter is N/A
		uint32 reserved1:15;
	} bitFields;
} RegRxClassifierRxqIdFilter14_u;

/*REG_RX_CLASSIFIER_RXQ_ID_FILTER15 0x9C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxqIdFilter15DropEn:1;	// Enable RXQ-ID filter drop indication
		uint32 reserved0:1;
		uint32 rxqIdFilter15FrameType:2;	// Field frame type in RXQ-ID filter
		uint32 rxqIdFilter15TypeNa:1;	// Field frame type in RXQ-ID filter is N/A
		uint32 rxqIdFilter15FrameSubtype:4;	// Field frame subtype in RXQ-ID filter
		uint32 rxqIdFilter15SubtypeNa:1;	// Field frame subtype in RXQ-ID filter is N/A
		uint32 rxqIdFilter15SubtypeEqual:1;	// Subtype compare logic: , 1: logic of equal , 0: logic of not equal
		uint32 rxqIdFilter15Addr1IdVal:1;	// Address1 index valid bit in RXQ-ID filter
		uint32 rxqIdFilter15Addr1IdValNa:1;	// Address1 index valid bit in RXQ-ID filter is N/A
		uint32 rxqIdFilter15Addr2IdVal:1;	// Address2 index valid bit in RXQ-ID filter
		uint32 rxqIdFilter15Addr2IdValNa:1;	// Address2 index valid bit in RXQ-ID filter is N/A
		uint32 rxqIdFilter15Addr1Unicast:1;	// Address1 unicast bit in RXQ-ID filter
		uint32 rxqIdFilter15Addr1UnicastNa:1;	// Address1 unicast bit in RXQ-ID filter is N/A
		uint32 reserved1:15;
	} bitFields;
} RegRxClassifierRxqIdFilter15_u;

/*REG_RX_CLASSIFIER_RXQ_ID_FILTER_CONTROL 0xA0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxqIdFilterGroupAndLogic:3;	// The number of filters minus one (starting from filter #0) which gathered as AND group, i.e. the result will be only if all the filters till this value indicate as DROP/Unsupported.
		uint32 reserved0:29;
	} bitFields;
} RegRxClassifierRxqIdFilterControl_u;

/*REG_RX_CLASSIFIER_RX_CLASSIFIER_ENABLE 0xB0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxClasTopEn:1;	// Rx Classifier enable
		uint32 rxClasSecurityEn:1;	// Enable Security engine initiation
		uint32 rxClasRtdGenerationEn:1;	// Enable RD template generation
		uint32 reserved0:29;
	} bitFields;
} RegRxClassifierRxClassifierEnable_u;

/*REG_RX_CLASSIFIER_WEP_KEY_IDX_DB_LOCATION 0xB4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 wepWdsKeyIdxDbLocation:1;	// WEP Key Index Location in case of unicast WDS: , 0 – STA DB , 1 – VAP DB
		uint32 wepNoWdsKeyIdxDbLocation:1;	// WEP Key Index Location in case of unicast no WDS: , 0 – STA DB , 1 – VAP DB
		uint32 reserved0:30;
	} bitFields;
} RegRxClassifierWepKeyIdxDbLocation_u;

/*REG_RX_CLASSIFIER_MAX_SEC_DESCRIPTORS 0xB8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 maxSecDescriptors:7;	// Maximum security descriptors minus one
		uint32 reserved0:25;
	} bitFields;
} RegRxClassifierMaxSecDescriptors_u;

/*REG_RX_CLASSIFIER_SEC_DESC_DEBUG 0xBC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 secDescPendingAcc:8;	// Accumulator pending Security descriptors result
		uint32 secDescIndex:7;
		uint32 reserved0:17;
	} bitFields;
} RegRxClassifierSecDescDebug_u;

/*REG_RX_CLASSIFIER_SEC_DESC_DEBUG_CLR 0xC0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 secDescPendingAccClr:1;	// Zeroed Security descriptor pending accumulator by write '1'
		uint32 secDescIndexClr:1;	// Zeroed Security descriptor index by write '1'
		uint32 reserved0:30;
	} bitFields;
} RegRxClassifierSecDescDebugClr_u;

/*REG_RX_CLASSIFIER_REKEY_ACT_STA_0TO31 0xC4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rekeyActivateStatusSta0To31:32;	// 1bit per STA: Rekey activate status
	} bitFields;
} RegRxClassifierRekeyActSta0To31_u;

/*REG_RX_CLASSIFIER_REKEY_ACT_STA_32TO63 0xC8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rekeyActivateStatusSta32To63:32;	// 1bit per STA: Rekey activate status
	} bitFields;
} RegRxClassifierRekeyActSta32To63_u;

/*REG_RX_CLASSIFIER_REKEY_ACT_STA_64TO95 0xCC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rekeyActivateStatusSta64To95:32;	// 1bit per STA: Rekey activate status
	} bitFields;
} RegRxClassifierRekeyActSta64To95_u;

/*REG_RX_CLASSIFIER_REKEY_ACT_STA_96TO127 0xD0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rekeyActivateStatusSta96To127:32;	// 1bit per STA: Rekey activate status
	} bitFields;
} RegRxClassifierRekeyActSta96To127_u;

/*REG_RX_CLASSIFIER_REKEY_ACT_VAP_0TO15 0xD4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rekeyActivateStatusVap0To15:16;	// 1bit per VAP: Rekey activate status
		uint32 reserved0:16;
	} bitFields;
} RegRxClassifierRekeyActVap0To15_u;

/*REG_RX_CLASSIFIER_REKEY_IRQ_L2_STA_0TO31 0xD8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rekeyIrqL2Sta0To31:32;	// 1bit per STA: Rekey interrupts status
	} bitFields;
} RegRxClassifierRekeyIrqL2Sta0To31_u;

/*REG_RX_CLASSIFIER_REKEY_IRQ_L2_STA_32TO63 0xDC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rekeyIrqL2Sta32To63:32;	// 1bit per STA: Rekey interrupts status , 
	} bitFields;
} RegRxClassifierRekeyIrqL2Sta32To63_u;

/*REG_RX_CLASSIFIER_REKEY_IRQ_L2_STA_64TO95 0xE0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rekeyIrqL2Sta64To95:32;	// 1bit per STA: Rekey interrupts status
	} bitFields;
} RegRxClassifierRekeyIrqL2Sta64To95_u;

/*REG_RX_CLASSIFIER_REKEY_IRQ_L2_STA_96TO127 0xE4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rekeyIrqL2Sta96To127:32;	// 1bit per STA: Rekey interrupts status
	} bitFields;
} RegRxClassifierRekeyIrqL2Sta96To127_u;

/*REG_RX_CLASSIFIER_REKEY_IRQ_L2_VAP_0TO15 0xE8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rekeyIrqL2Vap0To15:16;	// 1bit per VAP: Rekey interrupts status
		uint32 reserved0:16;
	} bitFields;
} RegRxClassifierRekeyIrqL2Vap0To15_u;

/*REG_RX_CLASSIFIER_REKEY_IRQ_L1_STATUS 0xEC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rekeyIrqL1Sta0To31:1;	// Rekey interrupt has been set for STA 31:0
		uint32 rekeyIrqL1Sta32To63:1;	// Rekey interrupt has been set for STA 63:32
		uint32 rekeyIrqL1Sta64To95:1;	// Rekey interrupt has been set for STA 95:64
		uint32 rekeyIrqL1Sta96To127:1;	// Rekey interrupt has been set for STA 127:96
		uint32 rekeyIrqL1Vap0To15:1;	// Rekey interrupt has been set for VAP 15:0
		uint32 reserved0:27;
	} bitFields;
} RegRxClassifierRekeyIrqL1Status_u;

/*REG_RX_CLASSIFIER_REKEY_CLR_ALL 0xF0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rekeyActivateClrAllStaStrb:1;	// Clear rekey activate of STA indexes 0-127, by write ‘1’ 
		uint32 rekeyActivateClrAllVapStrb:1;	// Clear rekey activate of VAP indexes 0-15, by write ‘1’
		uint32 rekeyIrqClrAllStaStrb:1;	// Clear rekey interrupts of STA indexes 0-127, by write ‘1’  
		uint32 rekeyIrqClrAllVapStrb:1;	// Clear rekey interrupts of VAP indexes 0-15, by write ‘1’
		uint32 reserved0:28;
	} bitFields;
} RegRxClassifierRekeyClrAll_u;

/*REG_RX_CLASSIFIER_REKEY_CTRL_STA_IDX 0xF4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rekeyStaIdx:7;	// STA index to update rekey activate/IRQ 
		uint32 reserved0:1;
		uint32 rekeyActivateSetStaIdxStrb:1;	// Set rekey activate of STA index
		uint32 rekeyActivateClrStaIdxStrb:1;	// Clear rekey activate of STA index
		uint32 rekeyIrqClrStaIdxStrb:1;	// Clear rekey IRQ of STA index
		uint32 reserved1:21;
	} bitFields;
} RegRxClassifierRekeyCtrlStaIdx_u;

/*REG_RX_CLASSIFIER_REKEY_CTRL_VAP_IDX 0xF8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rekeyVapIdx:4;	// VAP index to update rekey activate/IRQ 
		uint32 reserved0:4;
		uint32 rekeyActivateSetVapIdxStrb:1;	// Set rekey activate of VAP index
		uint32 rekeyActivateClrVapIdxStrb:1;	// Clear rekey activate of VAP index
		uint32 rekeyIrqClrVapIdxStrb:1;	// Clear rekey IRQ of VAP index
		uint32 reserved1:21;
	} bitFields;
} RegRxClassifierRekeyCtrlVapIdx_u;

/*REG_RX_CLASSIFIER_RX_CLAS_RTD_CTRL 0x100 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rtdRsvdSize:2;	// Number of RTD reserved words
		uint32 reserved0:2;
		uint32 rtdCtrlWrapFcSel:1;	// Determined the RD type and RTD subtype field in case of control wrapper frame: , 0 – based on the original FC field. , 1 – based on the shifted FC field. , 
		uint32 lengthCalcNegativeDropEn:1;	// Set RD type DROP in case of negative length result
		uint32 misDataProtFrameDbDropEn:1;	// Set RD type DROP in case of mismatch between frame protected bit and security enable in DB for Data Frames.
		uint32 reserved1:25;
	} bitFields;
} RegRxClassifierRxClasRtdCtrl_u;

/*REG_RX_CLASSIFIER_MAX_RD_TEMPLATES 0x104 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 maxRdTemplates:7;	// Maximum RD templates minus one
		uint32 reserved0:25;
	} bitFields;
} RegRxClassifierMaxRdTemplates_u;

/*REG_RX_CLASSIFIER_RTD_DEBUG 0x108 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pendingRtdAcc:8;	// Accumulator pending RTD result
		uint32 occupiedRtdAcc:8;	// Accumulator occupied RTD result
		uint32 rtdIndex:7;	// Current RTD index
		uint32 reserved0:9;
	} bitFields;
} RegRxClassifierRtdDebug_u;

/*REG_RX_CLASSIFIER_RTD_DEBUG_CLR 0x10C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pendingRtdAccClrStrb:1;	// Accumulator pending RTD clear strobe
		uint32 occupiedRtdAccClrStrb:1;	// Accumulator occupied RTD clear strobe
		uint32 rtdIndexClrStrb:1;	// Current RTD index clear strobe
		uint32 reserved0:29;
	} bitFields;
} RegRxClassifierRtdDebugClr_u;

/*REG_RX_CLASSIFIER_RX_CLAS_ACC_DEC 0x110 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pendingSecDescAccDecStrb:1;	// Accumulator pending Security descriptors decrement by one strobe
		uint32 pendingRtdAccDecStrb:1;	// Accumulator pending RTD decrement by one strobe
		uint32 occupiedRtdAccDecStrb:1;	// Accumulator occupied RTD decrement by one strobe
		uint32 reserved0:29;
	} bitFields;
} RegRxClassifierRxClasAccDec_u;

/*REG_RX_CLASSIFIER_RX_CLAS_SM_DBG 0x114 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxClassifierSm:4;
		uint32 rdCirBufSm:4;
		uint32 rxqIdSm:2;
		uint32 secInitSm:4;
		uint32 frameClassFilterSm:4;
		uint32 rtdFillSm:3;
		uint32 reserved0:11;
	} bitFields;
} RegRxClassifierRxClasSmDbg_u;

/*REG_RX_CLASSIFIER_RX_CLAS_CIR_BUF_PTR 0x118 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxClasCirBufOutPtr:16;
		uint32 reserved0:16;
	} bitFields;
} RegRxClassifierRxClasCirBufPtr_u;

/*REG_RX_CLASSIFIER_RX_CLAS_CIR_BUF_PTR_CLR 0x11C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxClasCirBufOutPtrClrStrb:1;	// Rx Classifier circular buffer out pointer clear strobe, , by write '1'
		uint32 reserved0:31;
	} bitFields;
} RegRxClassifierRxClasCirBufPtrClr_u;

/*REG_RX_CLASSIFIER_RX_CLAS_ERR_IRQ 0x120 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pendingSecDescAccBelowZeroErrIrq:1;	// Accumulator pending Security descriptors decrement while the amount is zero error
		uint32 pendingRtdAccBelowZeroErrIrq:1;	// Accumulator pending RTD decrement while the amount is zero error
		uint32 occupiedRtdAccBelowZeroErrIrq:1;	// Accumulator occupied RTD decrement while the amount is zero error
		uint32 stallingTimerExpiredIrq:1;	// Stalling due to no resources timer expired IRQ status
		uint32 reserved0:28;
	} bitFields;
} RegRxClassifierRxClasErrIrq_u;

/*REG_RX_CLASSIFIER_RX_CLAS_ERR_IRQ_EN 0x124 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pendingSecDescAccBelowZeroErrIrqEn:1;	// Enable IRQ Accumulator pending Security descriptors decrement while the amount is zero error
		uint32 pendingRtdAccBelowZeroErrIrqEn:1;	// Enable IRQ Accumulator pending RTD decrement while the amount is zero error
		uint32 occupiedRtdAccBelowZeroErrIrqEn:1;	// Enable IRQ Accumulator occupied RTD decrement while the amount is zero error
		uint32 stallingTimerExpiredIrqEn:1;	// Enable IRQ Stalling due to no resources timer expired IRQ status
		uint32 reserved0:28;
	} bitFields;
} RegRxClassifierRxClasErrIrqEn_u;

/*REG_RX_CLASSIFIER_RX_CLAS_ERR_IRQ_CLR 0x128 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pendingSecDescAccBelowZeroErrIrqClr:1;	// Clear IRQ Accumulator pending Security descriptors decrement while the amount is zero error
		uint32 pendingRtdAccBelowZeroErrIrqClr:1;	// Clear IRQ Accumulator pending RTD decrement while the amount is zero error
		uint32 occupiedRtdAccBelowZeroErrIrqClr:1;	// Clear IRQ Accumulator occupied RTD decrement while the amount is zero error
		uint32 stallingTimerExpiredIrqClr:1;	// Clear IRQ Stalling due to no resources timer expired IRQ status
		uint32 reserved0:28;
	} bitFields;
} RegRxClassifierRxClasErrIrqClr_u;

/*REG_RX_CLASSIFIER_RX_CLAS_STALLING_MAX_TIME 0x12C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxClasStallingMaxTime:16;	// Maximum time (1us resolution) of stalling to compare 
		uint32 reserved0:16;
	} bitFields;
} RegRxClassifierRxClasStallingMaxTime_u;

/*REG_RX_CLASSIFIER_RX_CLAS_STALLING_TIMER 0x130 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxClasStallingTimer:16;	// Rx Classifier stalling timer due to no free resources
		uint32 reserved0:16;
	} bitFields;
} RegRxClassifierRxClasStallingTimer_u;

/*REG_RX_CLASSIFIER_RX_CLAS_LOGGER_CONTROL 0x134 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxClasLoggerEn:1;	// Rx Classifier logger enable
		uint32 rxClasLoggerRtdEn:1;	// Rx Classifier RTD logger enable
		uint32 reserved0:6;
		uint32 rxClasLoggerPriority:2;	// Rx Classifier logger priority
		uint32 reserved1:22;
	} bitFields;
} RegRxClassifierRxClasLoggerControl_u;

/*REG_RX_CLASSIFIER_RX_CLAS_LOGGER_ACTIVE 0x138 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxClasLoggerActive:1;	// Rx Classifier logger active indication
		uint32 reserved0:31;
	} bitFields;
} RegRxClassifierRxClasLoggerActive_u;



#endif // _RX_CLASSIFIER_REGS_H_
