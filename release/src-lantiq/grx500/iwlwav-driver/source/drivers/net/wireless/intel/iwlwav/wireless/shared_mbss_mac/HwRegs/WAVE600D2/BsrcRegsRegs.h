
/***********************************************************************************
File:				BsrcRegsRegs.h
Module:				bsrcRegs
SOC Revision:		
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _BSRC_REGS_REGS_H_
#define _BSRC_REGS_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define BSRC_REGS_BASE_ADDRESS                             MEMORY_MAP_UNIT_16_BASE_ADDRESS
#define	REG_BSRC_REGS_GLOBAL_REG                             (BSRC_REGS_BASE_ADDRESS + 0x0)
#define	REG_BSRC_REGS_SENDER_INT_STATUS                      (BSRC_REGS_BASE_ADDRESS + 0x4)
#define	REG_BSRC_REGS_SENDER_INT_EN                          (BSRC_REGS_BASE_ADDRESS + 0x8)
#define	REG_BSRC_REGS_SENDER_CLEAR_INT                       (BSRC_REGS_BASE_ADDRESS + 0xC)
#define	REG_BSRC_REGS_INT_STATUS                             (BSRC_REGS_BASE_ADDRESS + 0x10)
#define	REG_BSRC_REGS_INT_EN                                 (BSRC_REGS_BASE_ADDRESS + 0x14)
#define	REG_BSRC_REGS_CLEAR_INT                              (BSRC_REGS_BASE_ADDRESS + 0x18)
#define	REG_BSRC_REGS_INT_ERROR_STATUS                       (BSRC_REGS_BASE_ADDRESS + 0x1C)
#define	REG_BSRC_REGS_INT_ERROR_EN                           (BSRC_REGS_BASE_ADDRESS + 0x20)
#define	REG_BSRC_REGS_CLEAR_INT_ERROR                        (BSRC_REGS_BASE_ADDRESS + 0x24)
#define	REG_BSRC_REGS_AGER_CFG                               (BSRC_REGS_BASE_ADDRESS + 0x28)
#define	REG_BSRC_REGS_SENDER_INDIRECT_ACCESS                 (BSRC_REGS_BASE_ADDRESS + 0x2C)
#define	REG_BSRC_REGS_SENDER_AGER_PERIOD                     (BSRC_REGS_BASE_ADDRESS + 0x30)
#define	REG_BSRC_REGS_SENDER_BUF_STS_CNT_1_0                 (BSRC_REGS_BASE_ADDRESS + 0x34)
#define	REG_BSRC_REGS_SENDER_BUF_STS_CNT_3_2                 (BSRC_REGS_BASE_ADDRESS + 0x38)
#define	REG_BSRC_REGS_SENDER_BUF_STS_CNT_5_4                 (BSRC_REGS_BASE_ADDRESS + 0x3C)
#define	REG_BSRC_REGS_SENDER_BUF_STS_CNT_7_6                 (BSRC_REGS_BASE_ADDRESS + 0x40)
#define	REG_BSRC_REGS_FW_INDIRECT_ACCESS                     (BSRC_REGS_BASE_ADDRESS + 0x44)
#define	REG_BSRC_REGS_FW_AGER_PERIOD                         (BSRC_REGS_BASE_ADDRESS + 0x48)
#define	REG_BSRC_REGS_FW_BUF_STS_CNT_1_0                     (BSRC_REGS_BASE_ADDRESS + 0x4C)
#define	REG_BSRC_REGS_FW_BUF_STS_CNT_3_2                     (BSRC_REGS_BASE_ADDRESS + 0x50)
#define	REG_BSRC_REGS_FW_BUF_STS_CNT_5_4                     (BSRC_REGS_BASE_ADDRESS + 0x54)
#define	REG_BSRC_REGS_FW_BUF_STS_CNT_7_6                     (BSRC_REGS_BASE_ADDRESS + 0x58)
#define	REG_BSRC_REGS_NDPFB_CFG0                             (BSRC_REGS_BASE_ADDRESS + 0x5C)
#define	REG_BSRC_REGS_NDPFB_CFG1                             (BSRC_REGS_BASE_ADDRESS + 0x60)
#define	REG_BSRC_REGS_NDPFB_CFG2                             (BSRC_REGS_BASE_ADDRESS + 0x64)
#define	REG_BSRC_REGS_NDPFB_REPORT_FIFO_BASE_ADDR            (BSRC_REGS_BASE_ADDRESS + 0x68)
#define	REG_BSRC_REGS_NDPFB_REPORT_FIFO_DEPTH_MINUS_ONE      (BSRC_REGS_BASE_ADDRESS + 0x6C)
#define	REG_BSRC_REGS_NDPFB_REPORT_FIFO_CLEAR_STRB           (BSRC_REGS_BASE_ADDRESS + 0x70)
#define	REG_BSRC_REGS_NDPFB_REPORT_FIFO_RD_ENTRIES_NUM       (BSRC_REGS_BASE_ADDRESS + 0x74)
#define	REG_BSRC_REGS_NDPFB_REPORT_FIFO_NUM_ENTRIES_COUNT    (BSRC_REGS_BASE_ADDRESS + 0x78)
#define	REG_BSRC_REGS_NDPFB_REPORT_FIFO_DEBUG                (BSRC_REGS_BASE_ADDRESS + 0x7C)
#define	REG_BSRC_REGS_LOGGER_REG                             (BSRC_REGS_BASE_ADDRESS + 0x80)
#define	REG_BSRC_REGS_LOGGER_FILTER                          (BSRC_REGS_BASE_ADDRESS + 0x84)
#define	REG_BSRC_REGS_FIFO_STATUS0                           (BSRC_REGS_BASE_ADDRESS + 0x88)
#define	REG_BSRC_REGS_FIFO_STATUS1                           (BSRC_REGS_BASE_ADDRESS + 0x8C)
#define	REG_BSRC_REGS_FSM_STATUS                             (BSRC_REGS_BASE_ADDRESS + 0x90)
#define	REG_BSRC_REGS_SPARE_REG                              (BSRC_REGS_BASE_ADDRESS + 0xA0)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_BSRC_REGS_GLOBAL_REG 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crBlockEn : 1; //BSRC block enable., reset value: 0x1, access type: RW
		uint32 crBufStsCompensationEn : 1; //Buffer Status compensation enable., reset value: 0x1, access type: RW
		uint32 crMaxMpduHdrLength : 8; //Max MPDU header length, to be decremented from MPDU full length received from RxC, for Buffer Status counters update., reset value: 0x0, access type: RW
		uint32 reserved0 : 22;
	} bitFields;
} RegBsrcRegsGlobalReg_u;

/*REG_BSRC_REGS_SENDER_INT_STATUS 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 srSenderDoneInt : 1; //Sender indirect access has completed., reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegBsrcRegsSenderIntStatus_u;

/*REG_BSRC_REGS_SENDER_INT_EN 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crSenderDoneIntEn : 1; //Enable sender_done_int., reset value: 0x1, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegBsrcRegsSenderIntEn_u;

/*REG_BSRC_REGS_SENDER_CLEAR_INT 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crClrSenderDoneInt : 1; //Clear sender_done_int., reset value: 0x0, access type: WO
		uint32 reserved0 : 31;
	} bitFields;
} RegBsrcRegsSenderClearInt_u;

/*REG_BSRC_REGS_INT_STATUS 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 srFwDoneInt : 1; //FW indirect access has completed., reset value: 0x0, access type: RO
		uint32 srNdpfbReportFifoNotEmptyIrq : 1; //NDP feedback report FIFO not empty interrupt, reset value: 0x0, access type: RO
		uint32 reserved0 : 30;
	} bitFields;
} RegBsrcRegsIntStatus_u;

/*REG_BSRC_REGS_INT_EN 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crFwDoneIntEn : 1; //Enable fw_done_int., reset value: 0x1, access type: RW
		uint32 crNdpfbReportFifoNotEmptyIrqEn : 1; //Enable sr_ndpfb_report_fifo_not_empty_irq, reset value: 0x1, access type: RW
		uint32 reserved0 : 30;
	} bitFields;
} RegBsrcRegsIntEn_u;

/*REG_BSRC_REGS_CLEAR_INT 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crClrFwDoneInt : 1; //Clear fw_done_int., reset value: 0x0, access type: WO
		uint32 reserved0 : 31;
	} bitFields;
} RegBsrcRegsClearInt_u;

/*REG_BSRC_REGS_INT_ERROR_STATUS 0x1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 srStartWhileRxcFifoNotEmptyInt : 1; //Got bsrc_start pulse from RxC while RxC FIFO was not empty., reset value: 0x0, access type: RO
		uint32 srStartStrobeWhileNdpfbNotIdleInt : 1; //Got bsrc_start/strobe pulse from RxC while NDP feedback FSM was not in IDLE., reset value: 0x0, access type: RO
		uint32 srNdpfbRestartWhileNdpfbNotIdleInt : 1; //Got bsrc_ndpfb_start pulse from RxC while NDP feedback FSM was not in IDLE., reset value: 0x0, access type: RO
		uint32 srNdpfbReportFifoFullDropIrq : 1; //NDP feedback report dropped because FIFO was full., reset value: 0x0, access type: RO
		uint32 srNdpfbReportFifoDecrementLessThanZeroIrq : 1; //NDP feedback report FIFO underflow indication., reset value: 0x0, access type: RO
		uint32 reserved0 : 27;
	} bitFields;
} RegBsrcRegsIntErrorStatus_u;

/*REG_BSRC_REGS_INT_ERROR_EN 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crStartWhileRxcFifoNotEmptyIntEn : 1; //Enable start_while_rxc_fifo_not_empty_int., reset value: 0x1, access type: RW
		uint32 crStartStrobeWhileNdpfbNotIdleIntEn : 1; //Enable start_strobe_while_ndpfb_not_idle_int., reset value: 0x1, access type: RW
		uint32 crNdpfbRestartWhileNdpfbNotIdleIntEn : 1; //Enable ndpfb_restart_while_ndpfb_not_idle_int., reset value: 0x1, access type: RW
		uint32 crNdpfbReportFifoFullDropIrqEn : 1; //Enable sr_ndpfb_report_fifo_full_drop_irq., reset value: 0x1, access type: RW
		uint32 crNdpfbReportFifoDecrementLessThanZeroIrqEn : 1; //Enable sr_ndpfb_report_fifo_decrement_less_than_zero_irq., reset value: 0x1, access type: RW
		uint32 reserved0 : 27;
	} bitFields;
} RegBsrcRegsIntErrorEn_u;

/*REG_BSRC_REGS_CLEAR_INT_ERROR 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crClrStartWhileRxcFifoNotEmptyInt : 1; //Clear start_while_rxc_fifo_not_empty_int., reset value: 0x0, access type: WO
		uint32 crClrStartStrobeWhileNdpfbNotIdleInt : 1; //Clear start_strobe_while_ndpfb_not_idle_int., reset value: 0x0, access type: WO
		uint32 crClrNdpfbRestartWhileNdpfbNotIdleInt : 1; //Clear ndpfb_restart_while_ndpfb_not_idle_int., reset value: 0x0, access type: WO
		uint32 reserved0 : 29;
	} bitFields;
} RegBsrcRegsClearIntError_u;

/*REG_BSRC_REGS_AGER_CFG 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crAgerCntEn : 1; //Ager counters enable., reset value: 0x1, access type: RW
		uint32 crAgerStrobeCnt : 15; //Ager strobe counter value. Resolution is 1usec. Default is 9999, meaning once per 10msec Ager counters are decremented by 1., reset value: 0x270F, access type: RW
		uint32 crAgerPeriod : 16; //Ager period in 10msec resolution., reset value: 0x0, access type: RW
	} bitFields;
} RegBsrcRegsAgerCfg_u;

/*REG_BSRC_REGS_SENDER_INDIRECT_ACCESS 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 senderRxTidBitmap : 8; //Rx TID bitmap in Sender indirect access., reset value: 0x0, access type: WO
		uint32 senderRxStaid : 9; //Rx Station ID in Sender indirect access., reset value: 0x0, access type: WO
		uint32 senderCommand : 3; //Command in Sender indirect access: , 0 - Mask , 1 - Unmask , 2 - Clear , 3 - Set , 4 - Init , 5 - Mask & Set, reset value: 0x0, access type: WO
		uint32 reserved0 : 12;
	} bitFields;
} RegBsrcRegsSenderIndirectAccess_u;

/*REG_BSRC_REGS_SENDER_AGER_PERIOD 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crSenderClrAgerPeriod : 16; //Ager clear period in 10msec resolution., reset value: 0x0, access type: RW
		uint32 crSenderSetAgerPeriod : 16; //Ager set period in 10msec resolution., reset value: 0x0, access type: RW
	} bitFields;
} RegBsrcRegsSenderAgerPeriod_u;

/*REG_BSRC_REGS_SENDER_BUF_STS_CNT_1_0 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crSenderBufStsCnt0 : 16; //Buffer Status counter for Sender init command for TID #0., reset value: 0x0, access type: RW
		uint32 crSenderBufStsCnt1 : 16; //Buffer Status counter for Sender init command for TID #1., reset value: 0x0, access type: RW
	} bitFields;
} RegBsrcRegsSenderBufStsCnt10_u;

/*REG_BSRC_REGS_SENDER_BUF_STS_CNT_3_2 0x38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crSenderBufStsCnt2 : 16; //Buffer Status counter for Sender init command for TID #2., reset value: 0x0, access type: RW
		uint32 crSenderBufStsCnt3 : 16; //Buffer Status counter for Sender init command for TID #3., reset value: 0x0, access type: RW
	} bitFields;
} RegBsrcRegsSenderBufStsCnt32_u;

/*REG_BSRC_REGS_SENDER_BUF_STS_CNT_5_4 0x3C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crSenderBufStsCnt4 : 16; //Buffer Status counter for Sender init command for TID #4., reset value: 0x0, access type: RW
		uint32 crSenderBufStsCnt5 : 16; //Buffer Status counter for Sender init command for TID #5., reset value: 0x0, access type: RW
	} bitFields;
} RegBsrcRegsSenderBufStsCnt54_u;

/*REG_BSRC_REGS_SENDER_BUF_STS_CNT_7_6 0x40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crSenderBufStsCnt6 : 16; //Buffer Status counter for Sender init command for TID #6., reset value: 0x0, access type: RW
		uint32 crSenderBufStsCnt7 : 16; //Buffer Status counter for Sender init command for TID #7., reset value: 0x0, access type: RW
	} bitFields;
} RegBsrcRegsSenderBufStsCnt76_u;

/*REG_BSRC_REGS_FW_INDIRECT_ACCESS 0x44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fwRxTidBitmap : 8; //Rx TID bitmap in FW indirect access., reset value: 0x0, access type: WO
		uint32 fwRxStaid : 9; //Rx Station ID in FW indirect access., reset value: 0x0, access type: WO
		uint32 fwCommand : 3; //Command in FW indirect access: , 0 - Mask , 1 - Unmask , 2 - Clear , 3 - Set , 4 - Init , 5 - Mask & Set, reset value: 0x0, access type: WO
		uint32 reserved0 : 12;
	} bitFields;
} RegBsrcRegsFwIndirectAccess_u;

/*REG_BSRC_REGS_FW_AGER_PERIOD 0x48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crFwClrAgerPeriod : 16; //Ager clear period in 10msec resolution., reset value: 0x0, access type: RW
		uint32 crFwSetAgerPeriod : 16; //Ager set period in 10msec resolution., reset value: 0x0, access type: RW
	} bitFields;
} RegBsrcRegsFwAgerPeriod_u;

/*REG_BSRC_REGS_FW_BUF_STS_CNT_1_0 0x4C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crFwBufStsCnt0 : 16; //Buffer Status counter for FW init command for TID #0., reset value: 0x0, access type: RW
		uint32 crFwBufStsCnt1 : 16; //Buffer Status counter for FW init command for TID #1., reset value: 0x0, access type: RW
	} bitFields;
} RegBsrcRegsFwBufStsCnt10_u;

/*REG_BSRC_REGS_FW_BUF_STS_CNT_3_2 0x50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crFwBufStsCnt2 : 16; //Buffer Status counter for FW init command for TID #2., reset value: 0x0, access type: RW
		uint32 crFwBufStsCnt3 : 16; //Buffer Status counter for FW init command for TID #3., reset value: 0x0, access type: RW
	} bitFields;
} RegBsrcRegsFwBufStsCnt32_u;

/*REG_BSRC_REGS_FW_BUF_STS_CNT_5_4 0x54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crFwBufStsCnt4 : 16; //Buffer Status counter for FW init command for TID #4., reset value: 0x0, access type: RW
		uint32 crFwBufStsCnt5 : 16; //Buffer Status counter for FW init command for TID #5., reset value: 0x0, access type: RW
	} bitFields;
} RegBsrcRegsFwBufStsCnt54_u;

/*REG_BSRC_REGS_FW_BUF_STS_CNT_7_6 0x58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crFwBufStsCnt6 : 16; //Buffer Status counter for FW init command for TID #6., reset value: 0x0, access type: RW
		uint32 crFwBufStsCnt7 : 16; //Buffer Status counter for FW init command for TID #7., reset value: 0x0, access type: RW
	} bitFields;
} RegBsrcRegsFwBufStsCnt76_u;

/*REG_BSRC_REGS_NDPFB_CFG0 0x5C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crNdpfbBlockEn : 1; //NDP feedback mechanism enable, reset value: 0x1, access type: RW
		uint32 crNdpfbUpdateBufStsCntAssocAnyVap : 1; //Update Buffer Status Counters if associated to any VAP., reset value: 0x1, access type: RW
		uint32 crNdpfbIgnoreBssidCtrlFrameSupport : 1; //When set BSSID Control frame support bit is ignored in the decision if to update the Buffer Status Counters., reset value: 0x1, access type: RW
		uint32 crNdpfbIgnoreFeedbackAnticipated : 1; //When set Feedback anticipated bit is ignored in the decision if to update the Buffer Status Counters., reset value: 0x1, access type: RW
		uint32 reserved0 : 28;
	} bitFields;
} RegBsrcRegsNdpfbCfg0_u;

/*REG_BSRC_REGS_NDPFB_CFG1 0x60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crNdpfbValBelowThresh : 16; //Value to update the buffer status counter if feedback is '0' and counter is below this threshold., reset value: 0x1, access type: RW
		uint32 crNdpfbValAboveThresh : 16; //Value to update the buffer status counter if feedback is '1' and counter is below this threshold., reset value: 0x100, access type: RW
	} bitFields;
} RegBsrcRegsNdpfbCfg1_u;

/*REG_BSRC_REGS_NDPFB_CFG2 0x64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crNdpfbStartingAid : 12; //NDP feedback starting AID., reset value: 0x1, access type: RW
		uint32 crNdpfbMultiplexFlag : 1; //NDP feedback Multiplexing Flag., reset value: 0x0, access type: RW
		uint32 reserved0 : 3;
		uint32 crNdpfbVapid : 5; //The VAP ID that has initiated the NFRP transmission., reset value: 0x0, access type: RW
		uint32 reserved1 : 3;
		uint32 crNdpfbUpdateReportOnly : 1; //When set only NDP feedback report is generated when triggered, but no Buffer Status Counters update., reset value: 0x0, access type: RW
		uint32 reserved2 : 7;
	} bitFields;
} RegBsrcRegsNdpfbCfg2_u;

/*REG_BSRC_REGS_NDPFB_REPORT_FIFO_BASE_ADDR 0x68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crNdpfbReportFifoBaseAddr : 22; //NDP feedback report FIFO base address, reset value: 0x0, access type: RW
		uint32 reserved0 : 10;
	} bitFields;
} RegBsrcRegsNdpfbReportFifoBaseAddr_u;

/*REG_BSRC_REGS_NDPFB_REPORT_FIFO_DEPTH_MINUS_ONE 0x6C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crNdpfbReportFifoDepthMinusOne : 9; //NDP feedback report FIFO depth minus one, reset value: 0x1ff, access type: RW
		uint32 reserved0 : 23;
	} bitFields;
} RegBsrcRegsNdpfbReportFifoDepthMinusOne_u;

/*REG_BSRC_REGS_NDPFB_REPORT_FIFO_CLEAR_STRB 0x70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crNdpfbReportFifoClearStrb : 1; //Clear NDP feedback report FIFO. , By writing '1'., reset value: 0x0, access type: WO
		uint32 crNdpfbReportFifoClearFullDropCtrStrb : 1; //Clear NDP feedback report FIFO full drop counter. , By writing '1'., reset value: 0x0, access type: WO
		uint32 crNdpfbReportFifoClearDecLessThanZeroStrb : 1; //Clear Tx status report FIFO decrement amount less than zero indication. , By writing '1'., reset value: 0x0, access type: WO
		uint32 reserved0 : 29;
	} bitFields;
} RegBsrcRegsNdpfbReportFifoClearStrb_u;

/*REG_BSRC_REGS_NDPFB_REPORT_FIFO_RD_ENTRIES_NUM 0x74 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crNdpfbReportFifoRdEntriesNum : 10; //NDP feedback report FIFO number of entries to decrement., reset value: 0x0, access type: RW
		uint32 reserved0 : 22;
	} bitFields;
} RegBsrcRegsNdpfbReportFifoRdEntriesNum_u;

/*REG_BSRC_REGS_NDPFB_REPORT_FIFO_NUM_ENTRIES_COUNT 0x78 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 srNdpfbReportFifoNumEntriesCount : 10; //NDP feedback report FIFO number of entries count, reset value: 0x0, access type: RO
		uint32 reserved0 : 22;
	} bitFields;
} RegBsrcRegsNdpfbReportFifoNumEntriesCount_u;

/*REG_BSRC_REGS_NDPFB_REPORT_FIFO_DEBUG 0x7C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 srNdpfbReportFifoWrPtr : 9; //NDP feedback report FIFO write pointer index, reset value: 0x0, access type: RO
		uint32 reserved0 : 7;
		uint32 srNdpfbReportFifoNotEmpty : 1; //NDP feedback report FIFO not empty indication, reset value: 0x0, access type: RO
		uint32 srNdpfbReportFifoFull : 1; //NDP feedback report FIFO full indication, reset value: 0x0, access type: RO
		uint32 srNdpfbReportFifoDecrementLessThanZero : 1; //NDP feedback report setting FIFO amount has been decremented less than zero, reset value: 0x0, access type: RO
		uint32 reserved1 : 1;
		uint32 srNdpfbReportFifoFullDropCtr : 9; //NDP feedback report setting FIFO full drop counter, reset value: 0x0, access type: RO
		uint32 reserved2 : 3;
	} bitFields;
} RegBsrcRegsNdpfbReportFifoDebug_u;

/*REG_BSRC_REGS_LOGGER_REG 0x80 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bsrcLoggerRegEn : 1; //BSRC logger enable, reset value: 0x0, access type: RW
		uint32 bsrcLoggerRegPriority : 2; //BSRC logger priority, reset value: 0x0, access type: RW
		uint32 bsrcLoggerRegActive : 1; //BSRC logger active, reset value: 0x0, access type: RO
		uint32 crLogMaxNumOfMsgs : 8; //Max number of messages in logger per report, while there are two words of 32 bits per message., reset value: 0x20, access type: RW
		uint32 crLogTimeoutVal : 16; //Timeout value in 1 usec resolution. When there was no messages sent within that period logger report will be closed., reset value: 0x3ff, access type: RW
		uint32 reserved0 : 4;
	} bitFields;
} RegBsrcRegsLoggerReg_u;

/*REG_BSRC_REGS_LOGGER_FILTER 0x84 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crLogStaid : 9; //Filter only Stations with Station ID equals cr_log_staid, reset value: 0x0, access type: RW
		uint32 crLogAllStaid : 1; //Log all Station IDs, reset value: 0x1, access type: RW
		uint32 crLogTidBitmap : 9; //Filter only TIDs set in the bitmap, reset value: 0x1ff, access type: RW
		uint32 crLogNdpfbStrbOnly : 1; //Filter only Buffer status counters update origin from NDP feedback strobe, reset value: 0x1, access type: RW
		uint32 crLogRxcStrbOnly : 1; //Filter only Buffer status counters update origin from RxC, reset value: 0x1, access type: RW
		uint32 crLogSenderCmdOnly : 1; //Filter only Buffer status counters update origin from Sender, reset value: 0x1, access type: RW
		uint32 crLogFwCmdOnly : 1; //Filter only Buffer status counters update origin from FW, reset value: 0x1, access type: RW
		uint32 crLogAgerClrOnly : 1; //Filter only Buffer status counters update origin from Ager, reset value: 0x1, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegBsrcRegsLoggerFilter_u;

/*REG_BSRC_REGS_FIFO_STATUS0 0x88 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 srRxcFifoEmpty : 1; //RxC FIFO empty indication., reset value: 0x1, access type: RO
		uint32 srRxcFifoFull : 1; //RxC FIFO full indication., reset value: 0x0, access type: RO
		uint32 srRxcFifoBytes : 3; //Valid entries in RxC FIFO., reset value: 0x0, access type: RO
		uint32 srSenderFifoEmpty : 1; //Sender FIFO empty indication., reset value: 0x1, access type: RO
		uint32 srSenderFifoFull : 1; //Sender FIFO full indication., reset value: 0x0, access type: RO
		uint32 srSenderFifoBytes : 3; //Valid entries in Sender FIFO., reset value: 0x0, access type: RO
		uint32 srFwFifoEmpty : 1; //FW FIFO empty indication., reset value: 0x1, access type: RO
		uint32 srFwFifoFull : 1; //FW FIFO full indication., reset value: 0x0, access type: RO
		uint32 srFwFifoBytes : 3; //Valid entries in FW FIFO., reset value: 0x0, access type: RO
		uint32 reserved0 : 1;
		uint32 srAgerFifoEmpty : 1; //Ager FIFO empty indication., reset value: 0x1, access type: RO
		uint32 srAgerFifoFull : 1; //Ager FIFO full indication., reset value: 0x0, access type: RO
		uint32 srAgerFifoBytes : 2; //Valid entries in Ager FIFO., reset value: 0x0, access type: RO
		uint32 reserved1 : 4;
		uint32 srBitselFifoEmpty : 1; //Bit Selector FIFO empty indication., reset value: 0x1, access type: RO
		uint32 srBitselFifoFull : 1; //Bit Selector FIFO full indication., reset value: 0x0, access type: RO
		uint32 srBitselFifoBytes : 2; //Valid entries in Bit Selector FIFO., reset value: 0x0, access type: RO
		uint32 reserved2 : 4;
	} bitFields;
} RegBsrcRegsFifoStatus0_u;

/*REG_BSRC_REGS_FIFO_STATUS1 0x8C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 srNdpfbFifoEmpty : 1; //NDP feedback FIFO empty indication., reset value: 0x1, access type: RO
		uint32 srNdpfbFifoFull : 1; //NDP feedback FIFO full indication., reset value: 0x0, access type: RO
		uint32 srNdpfbFifoBytes : 3; //Valid entries in NDP feedback FIFO., reset value: 0x0, access type: RO
		uint32 reserved0 : 27;
	} bitFields;
} RegBsrcRegsFifoStatus1_u;

/*REG_BSRC_REGS_FSM_STATUS 0x90 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 srMainFsmState : 3; //Main FSM state., reset value: 0x0, access type: RO
		uint32 reserved0 : 1;
		uint32 srAgerFsmState : 2; //Ager FSM state., reset value: 0x0, access type: RO
		uint32 srNdpfbFsmState : 2; //NDP feedback FSM state., reset value: 0x0, access type: RO
		uint32 reserved1 : 24;
	} bitFields;
} RegBsrcRegsFsmStatus_u;

/*REG_BSRC_REGS_SPARE_REG 0xA0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crSpareReg : 32; //Spare register., reset value: 0x88888888, access type: RW
	} bitFields;
} RegBsrcRegsSpareReg_u;



#endif // _BSRC_REGS_REGS_H_
