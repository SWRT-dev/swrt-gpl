
/***********************************************************************************
File:				PsSettingRegs.h
Module:				psSetting
SOC Revision:		843
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _PS_SETTING_REGS_H_
#define _PS_SETTING_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define PS_SETTING_BASE_ADDRESS                             MEMORY_MAP_UNIT_58_BASE_ADDRESS
#define	REG_PS_SETTING_PS_SETTING_CONTROL                        (PS_SETTING_BASE_ADDRESS + 0x0)
#define	REG_PS_SETTING_PS_VAP_DISABLE                            (PS_SETTING_BASE_ADDRESS + 0x4)
#define	REG_PS_SETTING_PS_ACK_REPLY_RXD_LINES_A                  (PS_SETTING_BASE_ADDRESS + 0x8)
#define	REG_PS_SETTING_PS_ACK_REPLY_RXD_LINES_B                  (PS_SETTING_BASE_ADDRESS + 0xC)
#define	REG_PS_SETTING_PD_NDP_TX_PD_LIST_IDX                     (PS_SETTING_BASE_ADDRESS + 0x10)
#define	REG_PS_SETTING_PS_STATUS_CHANGE_STA_0TO31_NOTIFY_EN      (PS_SETTING_BASE_ADDRESS + 0x20)
#define	REG_PS_SETTING_PS_STATUS_CHANGE_STA_32TO63_NOTIFY_EN     (PS_SETTING_BASE_ADDRESS + 0x24)
#define	REG_PS_SETTING_PS_STATUS_CHANGE_STA_64TO95_NOTIFY_EN     (PS_SETTING_BASE_ADDRESS + 0x28)
#define	REG_PS_SETTING_PS_STATUS_CHANGE_STA_96TO127_NOTIFY_EN    (PS_SETTING_BASE_ADDRESS + 0x2C)
#define	REG_PS_SETTING_PS_FW_NOTIFICATION_EN                     (PS_SETTING_BASE_ADDRESS + 0x30)
#define	REG_PS_SETTING_PS_FIFO_BASE_ADDR                         (PS_SETTING_BASE_ADDRESS + 0x40)
#define	REG_PS_SETTING_PS_FIFO_DEPTH_MINUS_ONE                   (PS_SETTING_BASE_ADDRESS + 0x44)
#define	REG_PS_SETTING_PS_FIFO_CLEAR_STRB                        (PS_SETTING_BASE_ADDRESS + 0x48)
#define	REG_PS_SETTING_PS_FIFO_RD_ENTRIES_NUM                    (PS_SETTING_BASE_ADDRESS + 0x4C)
#define	REG_PS_SETTING_PS_FIFO_NUM_ENTRIES_COUNT                 (PS_SETTING_BASE_ADDRESS + 0x50)
#define	REG_PS_SETTING_PS_FIFO_DEBUG                             (PS_SETTING_BASE_ADDRESS + 0x54)
#define	REG_PS_SETTING_PS_DEBUG                                  (PS_SETTING_BASE_ADDRESS + 0x58)
#define	REG_PS_SETTING_PS_LOGGER                                 (PS_SETTING_BASE_ADDRESS + 0x5C)
#define	REG_PS_SETTING_PS_LOGGER_ACTIVE                          (PS_SETTING_BASE_ADDRESS + 0x60)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_PS_SETTING_PS_SETTING_CONTROL 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 psSettingEn:1;	// Enable PS setting module
		uint32 reserved0:1;
		uint32 psRequestMode:1;	// PS request mode: , 0 – Selective Request Mode , 1 – Full Request Mode
		uint32 reserved1:29;
	} bitFields;
} RegPsSettingPsSettingControl_u;

/*REG_PS_SETTING_PS_VAP_DISABLE 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 psVapDisable:16;	// Enable/disable PS setting per VAP: , '1' – Disable PS setting, , '0' – Enable PS Setting.
		uint32 reserved0:16;
	} bitFields;
} RegPsSettingPsVapDisable_u;

/*REG_PS_SETTING_PS_ACK_REPLY_RXD_LINES_A 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 psAckReplyRxdLinesA:32;	// Each bit is per RXD line (0to31): , Indicates whether auto reply of ACK or BA has been configured for this line. , Can be used also to disable PS setting for specific RXD lines (even if there is ACK/BA).
	} bitFields;
} RegPsSettingPsAckReplyRxdLinesA_u;

/*REG_PS_SETTING_PS_ACK_REPLY_RXD_LINES_B 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 psAckReplyRxdLinesB:2;	// Each bit is per RXD line (32to33): , Indicates whether auto reply of ACK or BA has been configured for this line. , Can be used also to disable PS setting for specific RXD lines (even if there is ACK/BA).
		uint32 reserved0:30;
	} bitFields;
} RegPsSettingPsAckReplyRxdLinesB_u;

/*REG_PS_SETTING_PD_NDP_TX_PD_LIST_IDX 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pdNdpTxPdListIdx:5;	// List index in Q manager of NDP Tx PD poll
		uint32 reserved0:27;
	} bitFields;
} RegPsSettingPdNdpTxPdListIdx_u;

/*REG_PS_SETTING_PS_STATUS_CHANGE_STA_0TO31_NOTIFY_EN 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 psStatusChangeSta0To31NotifyEn:32;	// Enable FW notification PS status change STA0to31
	} bitFields;
} RegPsSettingPsStatusChangeSta0To31NotifyEn_u;

/*REG_PS_SETTING_PS_STATUS_CHANGE_STA_32TO63_NOTIFY_EN 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 psStatusChangeSta32To63NotifyEn:32;	// Enable FW notification PS status change STA32to63
	} bitFields;
} RegPsSettingPsStatusChangeSta32To63NotifyEn_u;

/*REG_PS_SETTING_PS_STATUS_CHANGE_STA_64TO95_NOTIFY_EN 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 psStatusChangeSta64To95NotifyEn:32;	// Enable FW notification PS status change STA64to95
	} bitFields;
} RegPsSettingPsStatusChangeSta64To95NotifyEn_u;

/*REG_PS_SETTING_PS_STATUS_CHANGE_STA_96TO127_NOTIFY_EN 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 psStatusChangeSta96To127NotifyEn:32;	// Enable FW notification PS status change STA96to127
	} bitFields;
} RegPsSettingPsStatusChangeSta96To127NotifyEn_u;

/*REG_PS_SETTING_PS_FW_NOTIFICATION_EN 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ndpTxPdEmptyNotifyEn:1;	// Enable notification to FW in case of No setting of PS request due to NDP Tx PD list empty
		uint32 psPollFcPmZeroNotifyEn:1;	// Enable notification to FW in case of Receive PS-Poll with Power Management bit==0
		uint32 psPollNoMatchAidNotifyEn:1;	// Enable notification to FW in case of Receive PS-Poll while AID[13:0] != (Addr2 index + 1)
		uint32 reserved0:29;
	} bitFields;
} RegPsSettingPsFwNotificationEn_u;

/*REG_PS_SETTING_PS_FIFO_BASE_ADDR 0x40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 psFifoBaseAddr:24;	// PS FIFO base address
		uint32 reserved0:8;
	} bitFields;
} RegPsSettingPsFifoBaseAddr_u;

/*REG_PS_SETTING_PS_FIFO_DEPTH_MINUS_ONE 0x44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 psFifoDepthMinusOne:8;	// PS FIFO depth minus one
		uint32 reserved0:24;
	} bitFields;
} RegPsSettingPsFifoDepthMinusOne_u;

/*REG_PS_SETTING_PS_FIFO_CLEAR_STRB 0x48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 psFifoClearStrb:1;	// Clear PS FIFO by write '1'
		uint32 psFifoClearFullDropCtrStrb:1;	// Clear PS fifo full drop counter
		uint32 psFifoClearDecLessThanZeroStrb:1;	// Clear PS FIFO decrement amount less than zero indication
		uint32 reserved0:29;
	} bitFields;
} RegPsSettingPsFifoClearStrb_u;

/*REG_PS_SETTING_PS_FIFO_RD_ENTRIES_NUM 0x4C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 psFifoRdEntriesNum:8;	// PS FIFO number of entries to decrement
		uint32 reserved0:24;
	} bitFields;
} RegPsSettingPsFifoRdEntriesNum_u;

/*REG_PS_SETTING_PS_FIFO_NUM_ENTRIES_COUNT 0x50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 psFifoNumEntriesCount:9;	// PS FIFO number of entries count
		uint32 reserved0:23;
	} bitFields;
} RegPsSettingPsFifoNumEntriesCount_u;

/*REG_PS_SETTING_PS_FIFO_DEBUG 0x54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 psFifoWrPtr:8;	// PS FIFO write pointer index
		uint32 reserved0:8;
		uint32 psFifoNotEmpty:1;	// PS FIFO not empty indication
		uint32 psFifoFull:1;	// PS FIFO full indication
		uint32 psFifoDecrementLessThanZero:1;	// PS setting FIFO amount has been decremented less than zero
		uint32 reserved1:5;
		uint32 psFifoFullDropCtr:8;	// PS setting FIFO full drop counter
	} bitFields;
} RegPsSettingPsFifoDebug_u;

/*REG_PS_SETTING_PS_DEBUG 0x58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 psStateMachine:5;	// PS state machine
		uint32 reserved0:27;
	} bitFields;
} RegPsSettingPsDebug_u;

/*REG_PS_SETTING_PS_LOGGER 0x5C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 psLoggerEn:1;	// PS Setting logger enable
		uint32 reserved0:7;
		uint32 psLoggerPriority:2;	// PS Setting logger priority
		uint32 reserved1:22;
	} bitFields;
} RegPsSettingPsLogger_u;

/*REG_PS_SETTING_PS_LOGGER_ACTIVE 0x60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 psLoggerActive:1;	// PS setting logger active
		uint32 reserved0:31;
	} bitFields;
} RegPsSettingPsLoggerActive_u;



#endif // _PS_SETTING_REGS_H_
