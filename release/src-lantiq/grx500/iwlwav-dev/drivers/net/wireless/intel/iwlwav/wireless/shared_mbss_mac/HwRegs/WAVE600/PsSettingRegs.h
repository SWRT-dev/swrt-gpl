
/***********************************************************************************
File:				PsSettingRegs.h
Module:				PsSetting
SOC Revision:		
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
#define	REG_PS_SETTING_PS_SETTING_CONTROL                         (PS_SETTING_BASE_ADDRESS + 0x0)
#define	REG_PS_SETTING_PS_VAP_DISABLE                             (PS_SETTING_BASE_ADDRESS + 0x4)
#define	REG_PS_SETTING_PD_NDP_TX_PD_LIST_IDX                      (PS_SETTING_BASE_ADDRESS + 0x8)
#define	REG_PS_SETTING_PS_STATUS_CHANGE_STA_0TO31_NOTIFY_EN       (PS_SETTING_BASE_ADDRESS + 0x10)
#define	REG_PS_SETTING_PS_STATUS_CHANGE_STA_32TO63_NOTIFY_EN      (PS_SETTING_BASE_ADDRESS + 0x14)
#define	REG_PS_SETTING_PS_STATUS_CHANGE_STA_64TO95_NOTIFY_EN      (PS_SETTING_BASE_ADDRESS + 0x18)
#define	REG_PS_SETTING_PS_STATUS_CHANGE_STA_96TO127_NOTIFY_EN     (PS_SETTING_BASE_ADDRESS + 0x1C)
#define	REG_PS_SETTING_PS_STATUS_CHANGE_STA_128TO159_NOTIFY_EN    (PS_SETTING_BASE_ADDRESS + 0x20)
#define	REG_PS_SETTING_PS_STATUS_CHANGE_STA_160TO191_NOTIFY_EN    (PS_SETTING_BASE_ADDRESS + 0x24)
#define	REG_PS_SETTING_PS_STATUS_CHANGE_STA_192TO223_NOTIFY_EN    (PS_SETTING_BASE_ADDRESS + 0x28)
#define	REG_PS_SETTING_PS_STATUS_CHANGE_STA_224TO255_NOTIFY_EN    (PS_SETTING_BASE_ADDRESS + 0x2C)
#define	REG_PS_SETTING_PS_FW_NOTIFICATION_EN                      (PS_SETTING_BASE_ADDRESS + 0x30)
#define	REG_PS_SETTING_PS_FIFO_BASE_ADDR                          (PS_SETTING_BASE_ADDRESS + 0x40)
#define	REG_PS_SETTING_PS_FIFO_DEPTH_MINUS_ONE                    (PS_SETTING_BASE_ADDRESS + 0x44)
#define	REG_PS_SETTING_PS_FIFO_CLEAR_STRB                         (PS_SETTING_BASE_ADDRESS + 0x48)
#define	REG_PS_SETTING_PS_FIFO_RD_ENTRIES_NUM                     (PS_SETTING_BASE_ADDRESS + 0x4C)
#define	REG_PS_SETTING_PS_FIFO_NUM_ENTRIES_COUNT                  (PS_SETTING_BASE_ADDRESS + 0x50)
#define	REG_PS_SETTING_PS_FIFO_DEBUG                              (PS_SETTING_BASE_ADDRESS + 0x54)
#define	REG_PS_SETTING_PS_DEBUG                                   (PS_SETTING_BASE_ADDRESS + 0x58)
#define	REG_PS_SETTING_PS_LOGGER                                  (PS_SETTING_BASE_ADDRESS + 0x5C)
#define	REG_PS_SETTING_PS_LOGGER_ACTIVE                           (PS_SETTING_BASE_ADDRESS + 0x60)
#define	REG_PS_SETTING_PS_SETTING_ERR_IRQ_EN                      (PS_SETTING_BASE_ADDRESS + 0x64)
#define	REG_PS_SETTING_PS_SETTING_ERR_IRQ_CLR                     (PS_SETTING_BASE_ADDRESS + 0x68)
#define	REG_PS_SETTING_PS_SETTING_ERR_IRQ_STATUS                  (PS_SETTING_BASE_ADDRESS + 0x6C)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_PS_SETTING_PS_SETTING_CONTROL 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 psSettingEn : 1; //Enable PS setting module, reset value: 0x1, access type: RW
		uint32 reserved0 : 1;
		uint32 psRequestMode : 1; //PS request mode: , 0 – Selective Request Mode , 1 – Full Request Mode, reset value: 0x0, access type: RW
		uint32 reserved1 : 29;
	} bitFields;
} RegPsSettingPsSettingControl_u;

/*REG_PS_SETTING_PS_VAP_DISABLE 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 psVapDisable : 32; //Enable/disable PS setting per VAP: , '1' – Disable PS setting, , '0' – Enable PS Setting., reset value: 0x0, access type: RW
	} bitFields;
} RegPsSettingPsVapDisable_u;

/*REG_PS_SETTING_PD_NDP_TX_PD_LIST_IDX 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pdNdpTxPdListIdx : 6; //List index in Q manager of NDP Tx PD poll, reset value: 0x0, access type: RW
		uint32 reserved0 : 26;
	} bitFields;
} RegPsSettingPdNdpTxPdListIdx_u;

/*REG_PS_SETTING_PS_STATUS_CHANGE_STA_0TO31_NOTIFY_EN 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 psStatusChangeSta0To31NotifyEn : 32; //Enable FW notification PS status change STA0to31, reset value: 0x0, access type: RW
	} bitFields;
} RegPsSettingPsStatusChangeSta0To31NotifyEn_u;

/*REG_PS_SETTING_PS_STATUS_CHANGE_STA_32TO63_NOTIFY_EN 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 psStatusChangeSta32To63NotifyEn : 32; //Enable FW notification PS status change STA32to63, reset value: 0x0, access type: RW
	} bitFields;
} RegPsSettingPsStatusChangeSta32To63NotifyEn_u;

/*REG_PS_SETTING_PS_STATUS_CHANGE_STA_64TO95_NOTIFY_EN 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 psStatusChangeSta64To95NotifyEn : 32; //Enable FW notification PS status change STA64to95, reset value: 0x0, access type: RW
	} bitFields;
} RegPsSettingPsStatusChangeSta64To95NotifyEn_u;

/*REG_PS_SETTING_PS_STATUS_CHANGE_STA_96TO127_NOTIFY_EN 0x1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 psStatusChangeSta96To127NotifyEn : 32; //Enable FW notification PS status change STA96to127, reset value: 0x0, access type: RW
	} bitFields;
} RegPsSettingPsStatusChangeSta96To127NotifyEn_u;

/*REG_PS_SETTING_PS_STATUS_CHANGE_STA_128TO159_NOTIFY_EN 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 psStatusChangeSta128To159NotifyEn : 32; //Enable FW notification PS status change STA128to159, reset value: 0x0, access type: RW
	} bitFields;
} RegPsSettingPsStatusChangeSta128To159NotifyEn_u;

/*REG_PS_SETTING_PS_STATUS_CHANGE_STA_160TO191_NOTIFY_EN 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 psStatusChangeSta160To191NotifyEn : 32; //Enable FW notification PS status change STA160to191, reset value: 0x0, access type: RW
	} bitFields;
} RegPsSettingPsStatusChangeSta160To191NotifyEn_u;

/*REG_PS_SETTING_PS_STATUS_CHANGE_STA_192TO223_NOTIFY_EN 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 psStatusChangeSta192To223NotifyEn : 32; //Enable FW notification PS status change STA192to223, reset value: 0x0, access type: RW
	} bitFields;
} RegPsSettingPsStatusChangeSta192To223NotifyEn_u;

/*REG_PS_SETTING_PS_STATUS_CHANGE_STA_224TO255_NOTIFY_EN 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 psStatusChangeSta224To255NotifyEn : 32; //Enable FW notification PS status change STA224to255, reset value: 0x0, access type: RW
	} bitFields;
} RegPsSettingPsStatusChangeSta224To255NotifyEn_u;

/*REG_PS_SETTING_PS_FW_NOTIFICATION_EN 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ndpTxPdEmptyNotifyEn : 1; //Enable notification to FW in case of No setting of PS request due to NDP Tx PD list empty, reset value: 0x0, access type: RW
		uint32 psPollFcPmZeroNotifyEn : 1; //Enable notification to FW in case of Receive PS-Poll with Power Management bit==0, reset value: 0x0, access type: RW
		uint32 psPollNoMatchAidNotifyEn : 1; //Enable notification to FW in case of Receive PS-Poll while AID[13:0] != (Addr2 index + 1), reset value: 0x0, access type: RW
		uint32 reserved0 : 29;
	} bitFields;
} RegPsSettingPsFwNotificationEn_u;

/*REG_PS_SETTING_PS_FIFO_BASE_ADDR 0x40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 psFifoBaseAddr : 22; //PS FIFO base address, reset value: 0x0, access type: RW
		uint32 reserved0 : 10;
	} bitFields;
} RegPsSettingPsFifoBaseAddr_u;

/*REG_PS_SETTING_PS_FIFO_DEPTH_MINUS_ONE 0x44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 psFifoDepthMinusOne : 8; //PS FIFO depth minus one, reset value: 0x0, access type: RW
		uint32 reserved0 : 24;
	} bitFields;
} RegPsSettingPsFifoDepthMinusOne_u;

/*REG_PS_SETTING_PS_FIFO_CLEAR_STRB 0x48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 psFifoClearStrb : 1; //Clear PS FIFO by write '1', reset value: 0x0, access type: WO
		uint32 psFifoClearFullDropCtrStrb : 1; //Clear PS fifo full drop counter, reset value: 0x0, access type: WO
		uint32 reserved0 : 30;
	} bitFields;
} RegPsSettingPsFifoClearStrb_u;

/*REG_PS_SETTING_PS_FIFO_RD_ENTRIES_NUM 0x4C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 psFifoRdEntriesNum : 8; //PS FIFO number of entries to decrement, reset value: 0x0, access type: RW
		uint32 reserved0 : 24;
	} bitFields;
} RegPsSettingPsFifoRdEntriesNum_u;

/*REG_PS_SETTING_PS_FIFO_NUM_ENTRIES_COUNT 0x50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 psFifoNumEntriesCount : 9; //PS FIFO number of entries count, reset value: 0x0, access type: RO
		uint32 reserved0 : 23;
	} bitFields;
} RegPsSettingPsFifoNumEntriesCount_u;

/*REG_PS_SETTING_PS_FIFO_DEBUG 0x54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 psFifoWrPtr : 8; //PS FIFO write pointer index, reset value: 0x0, access type: RO
		uint32 reserved0 : 8;
		uint32 psFifoNotEmpty : 1; //PS FIFO not empty indication, reset value: 0x0, access type: RO
		uint32 psFifoFull : 1; //PS FIFO full indication, reset value: 0x0, access type: RO
		uint32 reserved1 : 6;
		uint32 psFifoFullDropCtr : 8; //PS setting FIFO full drop counter, reset value: 0x0, access type: RO
	} bitFields;
} RegPsSettingPsFifoDebug_u;

/*REG_PS_SETTING_PS_DEBUG 0x58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 psStateMachine : 5; //PS state machine, reset value: 0x0, access type: RO
		uint32 psWrpSm : 3; //PS wrapper state machine, reset value: 0x0, access type: RO
		uint32 reserved0 : 24;
	} bitFields;
} RegPsSettingPsDebug_u;

/*REG_PS_SETTING_PS_LOGGER 0x5C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 psLoggerEn : 1; //PS Setting logger enable, reset value: 0x0, access type: RW
		uint32 reserved0 : 7;
		uint32 psLoggerPriority : 2; //PS Setting logger priority, reset value: 0x0, access type: RW
		uint32 reserved1 : 22;
	} bitFields;
} RegPsSettingPsLogger_u;

/*REG_PS_SETTING_PS_LOGGER_ACTIVE 0x60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 psLoggerActive : 1; //PS setting logger active, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegPsSettingPsLoggerActive_u;

/*REG_PS_SETTING_PS_SETTING_ERR_IRQ_EN 0x64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 startNewRxSessionWhileWaitAutoRespIrqEn : 1; //Start new Rx session, while PS setting module wait for auto response result ready, IRQ enable, reset value: 0x0, access type: RW
		uint32 startNewRxSessionWhilePsSettingActiveIrqEn : 1; //Start new Rx session, while PS setting module is still active, IRQ enable, reset value: 0x0, access type: RW
		uint32 psSettingFifoDecrementLessThanZeroIrqEn : 1; //PS setting FIFO amount has been decremented less than zero, IRQ enable, reset value: 0x0, access type: RW
		uint32 psSettingFifoFullDropIrqEn : 1; //PS setting drop entry due to FIFO full, IRQ enable, reset value: 0x0, access type: RW
		uint32 reserved0 : 28;
	} bitFields;
} RegPsSettingPsSettingErrIrqEn_u;

/*REG_PS_SETTING_PS_SETTING_ERR_IRQ_CLR 0x68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 startNewRxSessionWhileWaitAutoRespIrqClr : 1; //Start new Rx session, while PS setting module wait for auto response result ready, IRQ clear by write '1', reset value: 0x0, access type: WO
		uint32 startNewRxSessionWhilePsSettingActiveIrqClr : 1; //Start new Rx session, while PS setting module is still active, IRQ clear by write '1', reset value: 0x0, access type: WO
		uint32 psSettingFifoDecrementLessThanZeroIrqClr : 1; //PS setting FIFO amount has been decremented less than zero, IRQ clear by write '1', reset value: 0x0, access type: WO
		uint32 psSettingFifoFullDropIrqClr : 1; //PS setting drop entry due to FIFO full, IRQ clear by write '1', reset value: 0x0, access type: WO
		uint32 reserved0 : 28;
	} bitFields;
} RegPsSettingPsSettingErrIrqClr_u;

/*REG_PS_SETTING_PS_SETTING_ERR_IRQ_STATUS 0x6C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 startNewRxSessionWhileWaitAutoRespIrq : 1; //Start new Rx session, while PS setting module wait for auto response result ready, IRQ status, reset value: 0x0, access type: RO
		uint32 startNewRxSessionWhilePsSettingActiveIrq : 1; //Start new Rx session, while PS setting module is still active, IRQ status, reset value: 0x0, access type: RO
		uint32 psSettingFifoDecrementLessThanZeroIrq : 1; //PS setting FIFO amount has been decremented less than zero, IRQ status, reset value: 0x0, access type: RO
		uint32 psSettingFifoFullDropIrq : 1; //PS setting drop entry due to FIFO full, IRQ status, reset value: 0x0, access type: RO
		uint32 reserved0 : 28;
	} bitFields;
} RegPsSettingPsSettingErrIrqStatus_u;



#endif // _PS_SETTING_REGS_H_
