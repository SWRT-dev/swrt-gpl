
/***********************************************************************************
File:				MacHtExtensionsRegs.h
Module:				MacHtExtensions
SOC Revision:		
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _MAC_HT_EXTENSIONS_REGS_H_
#define _MAC_HT_EXTENSIONS_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define MAC_HT_EXTENSIONS_BASE_ADDRESS                             MEMORY_MAP_UNIT_24_BASE_ADDRESS
#define	REG_MAC_HT_EXTENSIONS_MAC_BAND_ID                       (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x0)
#define	REG_MAC_HT_EXTENSIONS_MAC_BAND_CONTROL                  (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x4)
#define	REG_MAC_HT_EXTENSIONS_WL_MAC_IRQ_CLR                    (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x10)
#define	REG_MAC_HT_EXTENSIONS_WL_MAC_IRQ_STATUS                 (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x14)
#define	REG_MAC_HT_EXTENSIONS_GP_IF_TIMER_PRESCALER             (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x20)
#define	REG_MAC_HT_EXTENSIONS_GP_IF_TIMEOUT_VAL                 (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x24)
#define	REG_MAC_HT_EXTENSIONS_GP_IF_SW_REQ                      (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x28)
#define	REG_MAC_HT_EXTENSIONS_GP_IF_TIMEOUT_TIME                (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x2C)
#define	REG_MAC_HT_EXTENSIONS_GP_IF_RESULT                      (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x30)
#define	REG_MAC_HT_EXTENSIONS_ACTIVITY_LOGGER_SW_EVENT          (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x34)
#define	REG_MAC_HT_EXTENSIONS_GENRISC_RX_FREE_LIST_IDX          (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x38)
#define	REG_MAC_HT_EXTENSIONS_LOGGER_FIFOS_SRC_SEL              (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x3C)
#define	REG_MAC_HT_EXTENSIONS_MAC_GENRISC_TX_END_IRQ            (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x40)
#define	REG_MAC_HT_EXTENSIONS_MAC_GENRISC_TX_END_IRQ_MASK       (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x44)
#define	REG_MAC_HT_EXTENSIONS_MAC_GENRISC_TX_LEVEL_IRQ_CONF     (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x48)
#define	REG_MAC_HT_EXTENSIONS_MAC_GENRISC_TX_IRQ_CONF_MASK0     (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x4C)
#define	REG_MAC_HT_EXTENSIONS_MAC_GENRISC_TX_IRQ_CONF_MASK1     (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x50)
#define	REG_MAC_HT_EXTENSIONS_MAC_GENRISC_TX_IRQ_CONF_MASK2     (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x54)
#define	REG_MAC_HT_EXTENSIONS_MAC_GENRISC_TX_IRQ_CONF_MASK3     (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x58)
#define	REG_MAC_HT_EXTENSIONS_MAC_GENRISC_TX_IRQ_CONF_MASK4     (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x5C)
#define	REG_MAC_HT_EXTENSIONS_MAC_GENRISC_TX_IRQ_CONF_MASK5     (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x60)
#define	REG_MAC_HT_EXTENSIONS_RAMS1_RM                          (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x6C)
#define	REG_MAC_HT_EXTENSIONS_RAMS2_RM                          (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x70)
#define	REG_MAC_HT_EXTENSIONS_RAMS3_RM                          (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x74)
#define	REG_MAC_HT_EXTENSIONS_RAMS4_RM                          (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x78)
#define	REG_MAC_HT_EXTENSIONS_RAMS5_RM                          (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x7C)
#define	REG_MAC_HT_EXTENSIONS_RAMS6_RM                          (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x80)
#define	REG_MAC_HT_EXTENSIONS_RAMS7_RM                          (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x84)
#define	REG_MAC_HT_EXTENSIONS_STD_CONTENTION_EVENTS             (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x90)
#define	REG_MAC_HT_EXTENSIONS_EVENT_CONTROL                     (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x94)
#define	REG_MAC_HT_EXTENSIONS_CLK_CONTROL_REG_FORCE_ON          (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0xA0)
#define	REG_MAC_HT_EXTENSIONS_CLK_CONTROL_REG_DYN_EN            (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0xA4)
#define	REG_MAC_HT_EXTENSIONS_CLK_CONTROL_REG_FORCE_ON2         (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0xA8)
#define	REG_MAC_HT_EXTENSIONS_CLK_CONTROL_REG_DYN_EN2           (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0xB0)
#define	REG_MAC_HT_EXTENSIONS_HT_EXT_SPARE_GPO                  (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0xC0)
#define	REG_MAC_HT_EXTENSIONS_HT_EXT_SPARE_GPI                  (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0xC4)
#define	REG_MAC_HT_EXTENSIONS_MAC_GENRISC_RX_LEVEL_IRQ          (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0xC8)
#define	REG_MAC_HT_EXTENSIONS_MAC_GENRISC_RX_LEVEL_IRQ_MASK0    (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0xCC)
#define	REG_MAC_HT_EXTENSIONS_MAC_GENRISC_RX_LEVEL_IRQ_MASK1    (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0xD0)
#define	REG_MAC_HT_EXTENSIONS_MAC_GENRISC_RX_LEVEL_IRQ_MASK2    (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0xD4)
#define	REG_MAC_HT_EXTENSIONS_MAC_GENRISC_RX_LEVEL_IRQ_MASK3    (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0xD8)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_MAC_HT_EXTENSIONS_MAC_BAND_ID 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 macBandId : 1; //Band ID Indication, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegMacHtExtensionsMacBandId_u;

/*REG_MAC_HT_EXTENSIONS_MAC_BAND_CONTROL 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 staAidOffset65En : 1; //AID offset from STA index: , 0: AID equals to STA index + 1. , 1: AID equals to STA index + 65., reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegMacHtExtensionsMacBandControl_u;

/*REG_MAC_HT_EXTENSIONS_WL_MAC_IRQ_CLR 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 8;
		uint32 del2GenriscTxEndIrqClr : 1; //Delia to genrisc Tx end IRQ clear (write '1' to clear), reset value: 0x0, access type: WO
		uint32 reserved1 : 6;
		uint32 delAutoFillEndIrqClr : 1; //Delia auto fill IRQ clear (write '1' to clear), reset value: 0x0, access type: WO
		uint32 reserved2 : 2;
		uint32 pasPacTickTimerClr : 1; //PAS PAC Tick timer clear  (write '1' to clear), reset value: 0x0, access type: WO
		uint32 macPhyGpIfIrqClr : 1; //MAC_PHY GP IF IRQ clear (write '1' to clear), reset value: 0x0, access type: WO
		uint32 pasPac0TxErrorClr : 1; //PAS PAC 0 TX Error clear (write '1' to clear), reset value: 0x0, access type: WO
		uint32 reserved3 : 4;
		uint32 delLimitTimerExpiredIrqClr : 1; //del limit timer expired IRQ clear (write '1' to clear), reset value: 0x0, access type: WO
		uint32 reserved4 : 6;
	} bitFields;
} RegMacHtExtensionsWlMacIrqClr_u;

/*REG_MAC_HT_EXTENSIONS_WL_MAC_IRQ_STATUS 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 8;
		uint32 del2GenriscTxEndIrqStatus : 1; //Delia 0 to genrisc Tx end IRQ status, reset value: 0x0, access type: RO
		uint32 reserved1 : 6;
		uint32 delAutoFillEndIrqStatus : 1; //Delia auto fill IRQ status, reset value: 0x0, access type: RO
		uint32 reserved2 : 2;
		uint32 pasPacTickTimerStatus : 1; //PAS PAC Tick timer status, reset value: 0x0, access type: RO
		uint32 macPhyGpIfIrqStatus : 1; //MAC_PHY GP IF IRQ status, reset value: 0x0, access type: RO
		uint32 pasPacTxErrorStatus : 1; //PAS PAC 0 TX Error status, reset value: 0x0, access type: RO
		uint32 reserved3 : 4;
		uint32 delLimitTimerExpiredIrqStatus : 1; //del limit timer expired IRQ status, reset value: 0x0, access type: RO
		uint32 reserved4 : 6;
	} bitFields;
} RegMacHtExtensionsWlMacIrqStatus_u;

/*REG_MAC_HT_EXTENSIONS_GP_IF_TIMER_PRESCALER 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gpIfTimerPrescaler : 10; //MAC Phy General purpose I/F timer prescaler, reset value: 0x27f, access type: RW
		uint32 reserved0 : 22;
	} bitFields;
} RegMacHtExtensionsGpIfTimerPrescaler_u;

/*REG_MAC_HT_EXTENSIONS_GP_IF_TIMEOUT_VAL 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gpIfTimeoutVal : 20; //MAC Phy General purpose interface timeout value; given in prescaler ticks, reset value: 0x1869f, access type: RW
		uint32 reserved0 : 12;
	} bitFields;
} RegMacHtExtensionsGpIfTimeoutVal_u;

/*REG_MAC_HT_EXTENSIONS_GP_IF_SW_REQ 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gpIfSwReqStb : 1; //no description, reset value: 0x0, access type: WO
		uint32 pacPhyRxGpType : 1; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 30;
	} bitFields;
} RegMacHtExtensionsGpIfSwReq_u;

/*REG_MAC_HT_EXTENSIONS_GP_IF_TIMEOUT_TIME 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gpIfTimeoutTime : 20; //GP interface timer current value, reset value: 0x0, access type: RO
		uint32 reserved0 : 12;
	} bitFields;
} RegMacHtExtensionsGpIfTimeoutTime_u;

/*REG_MAC_HT_EXTENSIONS_GP_IF_RESULT 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gpIfTimeoutEvent : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegMacHtExtensionsGpIfResult_u;

/*REG_MAC_HT_EXTENSIONS_ACTIVITY_LOGGER_SW_EVENT 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 actLoggerSwEvent : 8; //no description, reset value: 0x0, access type: WO
		uint32 reserved0 : 24;
	} bitFields;
} RegMacHtExtensionsActivityLoggerSwEvent_u;

/*REG_MAC_HT_EXTENSIONS_GENRISC_RX_FREE_LIST_IDX 0x38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 genriscRxFreeHostRdsListIdx : 6; //Free Host RDs DLM list index which used as an interrupt to Rx GenRisc, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 genriscRxFreeFwRdsListIdx : 6; //Free FW RDs DLM list index which used as an interrupt to Rx GenRisc, reset value: 0x0, access type: RW
		uint32 reserved1 : 2;
		uint32 genriscRxFreeDriverRdsListIdx : 6; //Free Driver RDs DLM list index which used as an interrupt to Rx GenRisc, reset value: 0x0, access type: RW
		uint32 reserved2 : 10;
	} bitFields;
} RegMacHtExtensionsGenriscRxFreeListIdx_u;

/*REG_MAC_HT_EXTENSIONS_LOGGER_FIFOS_SRC_SEL 0x3C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fifo0SrcSel : 3; //FIFO0 source select, reset value: 0x0, access type: RW
		uint32 reserved0 : 1;
		uint32 fifo1SrcSel : 3; //FIFO1 source select, reset value: 0x0, access type: RW
		uint32 reserved1 : 1;
		uint32 fifo2SrcSel : 3; //FIFO2 source select, reset value: 0x0, access type: RW
		uint32 reserved2 : 1;
		uint32 fifo3SrcSel : 3; //FIFO3 source select, reset value: 0x0, access type: RW
		uint32 reserved3 : 1;
		uint32 fifo4SrcSel : 3; //FIFO4 source select, reset value: 0x0, access type: RW
		uint32 reserved4 : 1;
		uint32 fifo5SrcSel : 3; //FIFO4 source select, reset value: 0x0, access type: RW
		uint32 reserved5 : 9;
	} bitFields;
} RegMacHtExtensionsLoggerFifosSrcSel_u;

/*REG_MAC_HT_EXTENSIONS_MAC_GENRISC_TX_END_IRQ 0x40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 senderPasPacTxError : 1; //Delia Tx error IRQ, 1 bit per Delia, reset value: 0x0, access type: RO
		uint32 reserved0 : 3;
		uint32 senderDel2GenriscTxEndIrq : 1; //Delia Tx end IRQ, reset value: 0x0, access type: RO
		uint32 reserved1 : 27;
	} bitFields;
} RegMacHtExtensionsMacGenriscTxEndIrq_u;

/*REG_MAC_HT_EXTENSIONS_MAC_GENRISC_TX_END_IRQ_MASK 0x44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 senderPasPacTxErrorMask : 1; //Delia Tx error IRQ enable, 1 bit per Delia, reset value: 0x0, access type: RW
		uint32 reserved0 : 3;
		uint32 senderDel2GenriscTxEndIrqMask : 1; //Delia Tx end IRQ enable, reset value: 0x0, access type: RW
		uint32 reserved1 : 27;
	} bitFields;
} RegMacHtExtensionsMacGenriscTxEndIrqMask_u;

/*REG_MAC_HT_EXTENSIONS_MAC_GENRISC_TX_LEVEL_IRQ_CONF 0x48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txPdAccIdleConf : 1; //Tx PD accelerator in IDLE, reset value: 0x1, access type: RO
		uint32 dma0WrapChIsIdleConf : 1; //DMA wrapper channel is IDLE, reset value: 0x1, access type: RO
		uint32 senderBaaIrqConf : 1; //Sender BAA IRQ , reset value: 0x0, access type: RO
		uint32 txhNtdTxGenriscSwReqIrqConf : 1; //TxH NTD genrisc_tx SW req IRQ , reset value: 0x0, access type: RO
		uint32 delLimitTimerExpiredIrqConf : 1; //Delia limit timer expired IRQ , reset value: 0x0, access type: RO
		uint32 arc2MacGenriscTxIrqConf : 2; //arc2mac genrisc_tx IRQ , reset value: 0x0, access type: RO
		uint32 sequencerIrqConf : 1; //Sequencer IRQ , reset value: 0x0, access type: RO
		uint32 txSecurityFailIrqConf : 1; //Tx Security fail IRQ, reset value: 0x0, access type: RO
		uint32 txSecurityDoneIrqConf : 1; //Tx Security done IRQ , reset value: 0x0, access type: RO
		uint32 txSecurityArbSmIdleConf : 1; //Tx Security engine arbiter state is not in Tx, reset value: 0x1, access type: RO
		uint32 sequencerEndIrqConf : 1; //Sequencer End IRQ , reset value: 0x0, access type: RO
		uint32 otfaIdleConf : 1; //OTFA in IDLE, reset value: 0x1, access type: RO
		uint32 baaIdleConf : 1; //BAA in IDLE, reset value: 0x1, access type: RO
		uint32 txHcIdleConf : 1; //TX_HC in IDLE, reset value: 0x1, access type: RO
		uint32 txDmaAlignerIdleConf : 1; //TX DMA Aligner in IDLE, reset value: 0x1, access type: RO
		uint32 dmaDoneConf : 1; //TX DMA Done indication, reset value: 0x0, access type: RO
		uint32 durationReadyConf : 1; //TX MPDU Duration field ready, reset value: 0x0, access type: RO
		uint32 deliaRecoveryDoneConf : 1; //Delia recovery done, reset value: 0x0, access type: RO
		uint32 tfGeneratorIdleConf : 1; //Reserved, reset value: 0x0, access type: RO
		uint32 macGenriscTxIrqConfRsvd20Conf : 1; //Reserved, reset value: 0x0, access type: RO
		uint32 macGenriscTxIrqConfRsvd21Conf : 1; //Reserved, reset value: 0x0, access type: RO
		uint32 macGenriscTxIrqConfRsvd22Conf : 1; //Reserved, reset value: 0x0, access type: RO
		uint32 macGenriscTxIrqConfRsvd23Conf : 1; //Reserved, reset value: 0x0, access type: RO
		uint32 reserved0 : 8;
	} bitFields;
} RegMacHtExtensionsMacGenriscTxLevelIrqConf_u;

/*REG_MAC_HT_EXTENSIONS_MAC_GENRISC_TX_IRQ_CONF_MASK0 0x4C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txPdAccIdleMask0 : 1; //Tx PD accelerator in IDLE mask, reset value: 0x0, access type: RW
		uint32 dma0WrapChIsIdleMask0 : 1; //DMA wrapper channel is IDLE mask, reset value: 0x0, access type: RW
		uint32 senderBaaIrqMask0 : 1; //Sender BAA IRQ mask, reset value: 0x0, access type: RW
		uint32 txhNtdTxGenriscSwReqIrqMask0 : 1; //TxH NTD genrisc_tx SW req IRQ mask, reset value: 0x0, access type: RW
		uint32 delLimitTimerExpiredIrqMask0 : 1; //Delia limit timer expired IRQ mask, reset value: 0x0, access type: RW
		uint32 arc2MacGenriscTxIrqMask0 : 2; //arc2mac genrisc_tx IRQ mask, reset value: 0x0, access type: RW
		uint32 sequencerIrqMask0 : 1; //Sequencer IRQ mask, reset value: 0x0, access type: RW
		uint32 txSecurityFailIrqMask0 : 1; //Tx Security fail IRQ mask, reset value: 0x0, access type: RW
		uint32 txSecurityDoneIrqMask0 : 1; //Tx Security done IRQ mask, reset value: 0x0, access type: RW
		uint32 txSecurityArbSmIdleMask0 : 1; //Tx Security engine arbiter state is not in Tx mask, reset value: 0x0, access type: RW
		uint32 sequencerEndIrqMask0 : 1; //Sequencer End IRQ mask, reset value: 0x0, access type: RW
		uint32 otfaIdleMask0 : 1; //OTFA in IDLE mask, reset value: 0x0, access type: RW
		uint32 baaIdleMask0 : 1; //BAA in IDLE mask, reset value: 0x0, access type: RW
		uint32 txHcIdleMask0 : 1; //TX_HC in IDLE mask, reset value: 0x0, access type: RW
		uint32 txDmaAlignerIdleMask0 : 1; //TX DMA Aligner in IDLE mask, reset value: 0x0, access type: RW
		uint32 dmaDoneMask0 : 1; //TX DMA Done indication mask, reset value: 0x0, access type: RW
		uint32 durationReadyMask0 : 1; //TX MPDU Duration field ready mask, reset value: 0x0, access type: RW
		uint32 deliaRecoveryDoneMask0 : 1; //Delia recovery done mask, reset value: 0x0, access type: RW
		uint32 tfGeneratorIdleMask0 : 1; //Reserved, reset value: 0x0, access type: RW
		uint32 macGenriscTxIrqConfRsvd20Mask0 : 1; //Reserved, reset value: 0x0, access type: RW
		uint32 macGenriscTxIrqConfRsvd21Mask0 : 1; //Reserved, reset value: 0x0, access type: RW
		uint32 macGenriscTxIrqConfRsvd22Mask0 : 1; //Reserved, reset value: 0x0, access type: RW
		uint32 macGenriscTxIrqConfRsvd23Mask0 : 1; //Reserved, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegMacHtExtensionsMacGenriscTxIrqConfMask0_u;

/*REG_MAC_HT_EXTENSIONS_MAC_GENRISC_TX_IRQ_CONF_MASK1 0x50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txPdAccIdleMask1 : 1; //Tx PD accelerator in IDLE mask, reset value: 0x0, access type: RW
		uint32 dma0WrapChIsIdleMask1 : 1; //DMA wrapper channel is IDLE mask, reset value: 0x0, access type: RW
		uint32 senderBaaIrqMask1 : 1; //Sender BAA IRQ mask, reset value: 0x0, access type: RW
		uint32 txhNtdTxGenriscSwReqIrqMask1 : 1; //TxH NTD genrisc_tx SW req IRQ mask, reset value: 0x0, access type: RW
		uint32 delLimitTimerExpiredIrqMask1 : 1; //Delia limit timer expired IRQ mask, reset value: 0x0, access type: RW
		uint32 arc2MacGenriscTxIrqMask1 : 2; //arc2mac genrisc_tx IRQ mask, reset value: 0x0, access type: RW
		uint32 sequencerIrqMask1 : 1; //Sequencer IRQ mask, reset value: 0x0, access type: RW
		uint32 txSecurityFailIrqMask1 : 1; //Tx Security fail IRQ mask, reset value: 0x0, access type: RW
		uint32 txSecurityDoneIrqMask1 : 1; //Tx Security done IRQ mask, reset value: 0x0, access type: RW
		uint32 txSecurityArbSmIdleMask1 : 1; //Tx Security engine arbiter state is not in Tx mask, reset value: 0x0, access type: RW
		uint32 sequencerEndIrqMask1 : 1; //Sequencer End IRQ mask, reset value: 0x0, access type: RW
		uint32 otfaIdleMask1 : 1; //OTFA in IDLE mask, reset value: 0x0, access type: RW
		uint32 baaIdleMask1 : 1; //BAA in IDLE mask, reset value: 0x0, access type: RW
		uint32 txHcIdleMask1 : 1; //TX_HC in IDLE mask, reset value: 0x0, access type: RW
		uint32 txDmaAlignerIdleMask1 : 1; //TX DMA Aligner in IDLE mask, reset value: 0x0, access type: RW
		uint32 dmaDoneMask1 : 1; //TX DMA Done indication mask, reset value: 0x0, access type: RW
		uint32 durationReadyMask1 : 1; //TX MPDU Duration field ready mask, reset value: 0x0, access type: RW
		uint32 deliaRecoveryDoneMask1 : 1; //Delia recovery done mask, reset value: 0x0, access type: RW
		uint32 tfGeneratorIdleMask1 : 1; //Reserved, reset value: 0x0, access type: RW
		uint32 macGenriscTxIrqConfRsvd20Mask1 : 1; //Reserved, reset value: 0x0, access type: RW
		uint32 macGenriscTxIrqConfRsvd21Mask1 : 1; //Reserved, reset value: 0x0, access type: RW
		uint32 macGenriscTxIrqConfRsvd22Mask1 : 1; //Reserved, reset value: 0x0, access type: RW
		uint32 macGenriscTxIrqConfRsvd23Mask1 : 1; //Reserved, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegMacHtExtensionsMacGenriscTxIrqConfMask1_u;

/*REG_MAC_HT_EXTENSIONS_MAC_GENRISC_TX_IRQ_CONF_MASK2 0x54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txPdAccIdleMask2 : 1; //Tx PD accelerator in IDLE mask, reset value: 0x0, access type: RW
		uint32 dma0WrapChIsIdleMask2 : 1; //DMA wrapper channel is IDLE mask, reset value: 0x0, access type: RW
		uint32 senderBaaIrqMask2 : 1; //Sender BAA IRQ mask, reset value: 0x0, access type: RW
		uint32 txhNtdTxGenriscSwReqIrqMask2 : 1; //TxH NTD genrisc_tx SW req IRQ mask, reset value: 0x0, access type: RW
		uint32 delLimitTimerExpiredIrqMask2 : 1; //Delia limit timer expired IRQ mask, reset value: 0x0, access type: RW
		uint32 arc2MacGenriscTxIrqMask2 : 2; //arc2mac genrisc_tx IRQ mask, reset value: 0x0, access type: RW
		uint32 sequencerIrqMask2 : 1; //Sequencer IRQ mask, reset value: 0x0, access type: RW
		uint32 txSecurityFailIrqMask2 : 1; //Tx Security fail IRQ mask, reset value: 0x0, access type: RW
		uint32 txSecurityDoneIrqMask2 : 1; //Tx Security done IRQ mask, reset value: 0x0, access type: RW
		uint32 txSecurityArbSmIdleMask2 : 1; //Tx Security engine arbiter state is not in Tx mask, reset value: 0x0, access type: RW
		uint32 sequencerEndIrqMask2 : 1; //Sequencer End IRQ mask, reset value: 0x0, access type: RW
		uint32 otfaIdleMask2 : 1; //OTFA in IDLE mask, reset value: 0x0, access type: RW
		uint32 baaIdleMask2 : 1; //BAA in IDLE mask, reset value: 0x0, access type: RW
		uint32 txHcIdleMask2 : 1; //TX_HC in IDLE mask, reset value: 0x0, access type: RW
		uint32 txDmaAlignerIdleMask2 : 1; //TX DMA Aligner in IDLE mask, reset value: 0x0, access type: RW
		uint32 dmaDoneMask2 : 1; //TX DMA Done indication mask, reset value: 0x0, access type: RW
		uint32 durationReadyMask2 : 1; //TX MPDU Duration field ready mask, reset value: 0x0, access type: RW
		uint32 deliaRecoveryDoneMask2 : 1; //Delia recovery done mask, reset value: 0x0, access type: RW
		uint32 tfGeneratorIdleMask2 : 1; //Reserved, reset value: 0x0, access type: RW
		uint32 macGenriscTxIrqConfRsvd20Mask2 : 1; //Reserved, reset value: 0x0, access type: RW
		uint32 macGenriscTxIrqConfRsvd21Mask2 : 1; //Reserved, reset value: 0x0, access type: RW
		uint32 macGenriscTxIrqConfRsvd22Mask2 : 1; //Reserved, reset value: 0x0, access type: RW
		uint32 macGenriscTxIrqConfRsvd23Mask2 : 1; //Reserved, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegMacHtExtensionsMacGenriscTxIrqConfMask2_u;

/*REG_MAC_HT_EXTENSIONS_MAC_GENRISC_TX_IRQ_CONF_MASK3 0x58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txPdAccIdleMask3 : 1; //Tx PD accelerator in IDLE mask, reset value: 0x0, access type: RW
		uint32 dma0WrapChIsIdleMask3 : 1; //DMA wrapper channel is IDLE mask, reset value: 0x0, access type: RW
		uint32 senderBaaIrqMask3 : 1; //Sender BAA IRQ mask, reset value: 0x0, access type: RW
		uint32 txhNtdTxGenriscSwReqIrqMask3 : 1; //TxH NTD genrisc_tx SW req IRQ mask, reset value: 0x0, access type: RW
		uint32 delLimitTimerExpiredIrqMask3 : 1; //Delia limit timer expired IRQ mask, reset value: 0x0, access type: RW
		uint32 arc2MacGenriscTxIrqMask3 : 2; //arc2mac genrisc_tx IRQ mask, reset value: 0x0, access type: RW
		uint32 sequencerIrqMask3 : 1; //Sequencer IRQ mask, reset value: 0x0, access type: RW
		uint32 txSecurityFailIrqMask3 : 1; //Tx Security fail IRQ mask, reset value: 0x0, access type: RW
		uint32 txSecurityDoneIrqMask3 : 1; //Tx Security done IRQ mask, reset value: 0x0, access type: RW
		uint32 txSecurityArbSmIdleMask3 : 1; //Tx Security engine arbiter state is not in Tx mask, reset value: 0x0, access type: RW
		uint32 sequencerEndIrqMask3 : 1; //Sequencer End IRQ mask, reset value: 0x0, access type: RW
		uint32 otfaIdleMask3 : 1; //OTFA in IDLE mask, reset value: 0x0, access type: RW
		uint32 baaIdleMask3 : 1; //BAA in IDLE mask, reset value: 0x0, access type: RW
		uint32 txHcIdleMask3 : 1; //TX_HC in IDLE mask, reset value: 0x0, access type: RW
		uint32 txDmaAlignerIdleMask3 : 1; //TX DMA Aligner in IDLE mask, reset value: 0x0, access type: RW
		uint32 dmaDoneMask3 : 1; //TX DMA Done indication mask, reset value: 0x0, access type: RW
		uint32 durationReadyMask3 : 1; //TX MPDU Duration field ready mask, reset value: 0x0, access type: RW
		uint32 deliaRecoveryDoneMask3 : 1; //Delia recovery done mask, reset value: 0x0, access type: RW
		uint32 tfGeneratorIdleMask3 : 1; //Reserved, reset value: 0x0, access type: RW
		uint32 macGenriscTxIrqConfRsvd20Mask3 : 1; //Reserved, reset value: 0x0, access type: RW
		uint32 macGenriscTxIrqConfRsvd21Mask3 : 1; //Reserved, reset value: 0x0, access type: RW
		uint32 macGenriscTxIrqConfRsvd22Mask3 : 1; //Reserved, reset value: 0x0, access type: RW
		uint32 macGenriscTxIrqConfRsvd23Mask3 : 1; //Reserved, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegMacHtExtensionsMacGenriscTxIrqConfMask3_u;

/*REG_MAC_HT_EXTENSIONS_MAC_GENRISC_TX_IRQ_CONF_MASK4 0x5C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txPdAccIdleMask4 : 1; //Tx PD accelerator in IDLE mask, reset value: 0x0, access type: RW
		uint32 dma0WrapChIsIdleMask4 : 1; //DMA wrapper channel is IDLE mask, reset value: 0x0, access type: RW
		uint32 senderBaaIrqMask4 : 1; //Sender BAA IRQ mask, reset value: 0x0, access type: RW
		uint32 txhNtdTxGenriscSwReqIrqMask4 : 1; //TxH NTD genrisc_tx SW req IRQ mask, reset value: 0x0, access type: RW
		uint32 delLimitTimerExpiredIrqMask4 : 1; //Delia limit timer expired IRQ mask, reset value: 0x0, access type: RW
		uint32 arc2MacGenriscTxIrqMask4 : 2; //arc2mac genrisc_tx IRQ mask, reset value: 0x0, access type: RW
		uint32 sequencerIrqMask4 : 1; //Sequencer IRQ mask, reset value: 0x0, access type: RW
		uint32 txSecurityFailIrqMask4 : 1; //Tx Security fail IRQ mask, reset value: 0x0, access type: RW
		uint32 txSecurityDoneIrqMask4 : 1; //Tx Security done IRQ mask, reset value: 0x0, access type: RW
		uint32 txSecurityArbSmIdleMask4 : 1; //Tx Security engine arbiter state is not in Tx mask, reset value: 0x0, access type: RW
		uint32 sequencerEndIrqMask4 : 1; //Sequencer End IRQ mask, reset value: 0x0, access type: RW
		uint32 otfaIdleMask4 : 1; //OTFA in IDLE mask, reset value: 0x0, access type: RW
		uint32 baaIdleMask4 : 1; //BAA in IDLE mask, reset value: 0x0, access type: RW
		uint32 txHcIdleMask4 : 1; //TX_HC in IDLE mask, reset value: 0x0, access type: RW
		uint32 txDmaAlignerIdleMask4 : 1; //TX DMA Aligner in IDLE mask, reset value: 0x0, access type: RW
		uint32 dmaDoneMask4 : 1; //TX DMA Done indication mask, reset value: 0x0, access type: RW
		uint32 durationReadyMask4 : 1; //TX MPDU Duration field ready mask, reset value: 0x0, access type: RW
		uint32 deliaRecoveryDoneMask4 : 1; //Delia recovery done mask, reset value: 0x0, access type: RW
		uint32 tfGeneratorIdleMask4 : 1; //Reserved, reset value: 0x0, access type: RW
		uint32 macGenriscTxIrqConfRsvd20Mask4 : 1; //Reserved, reset value: 0x0, access type: RW
		uint32 macGenriscTxIrqConfRsvd21Mask4 : 1; //Reserved, reset value: 0x0, access type: RW
		uint32 macGenriscTxIrqConfRsvd22Mask4 : 1; //Reserved, reset value: 0x0, access type: RW
		uint32 macGenriscTxIrqConfRsvd23Mask4 : 1; //Reserved, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegMacHtExtensionsMacGenriscTxIrqConfMask4_u;

/*REG_MAC_HT_EXTENSIONS_MAC_GENRISC_TX_IRQ_CONF_MASK5 0x60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txPdAccIdleMask5 : 1; //Tx PD accelerator in IDLE mask, reset value: 0x0, access type: RW
		uint32 dma0WrapChIsIdleMask5 : 1; //DMA wrapper channel is IDLE mask, reset value: 0x0, access type: RW
		uint32 senderBaaIrqMask5 : 1; //Sender BAA IRQ mask, reset value: 0x0, access type: RW
		uint32 txhNtdTxGenriscSwReqIrqMask5 : 1; //TxH NTD genrisc_tx SW req IRQ mask, reset value: 0x0, access type: RW
		uint32 delLimitTimerExpiredIrqMask5 : 1; //Delia limit timer expired IRQ mask, reset value: 0x0, access type: RW
		uint32 arc2MacGenriscTxIrqMask5 : 2; //arc2mac genrisc_tx IRQ mask, reset value: 0x0, access type: RW
		uint32 sequencerIrqMask5 : 1; //Sequencer IRQ mask, reset value: 0x0, access type: RW
		uint32 txSecurityFailIrqMask5 : 1; //Tx Security fail IRQ mask, reset value: 0x0, access type: RW
		uint32 txSecurityDoneIrqMask5 : 1; //Tx Security done IRQ mask, reset value: 0x0, access type: RW
		uint32 txSecurityArbSmIdleMask5 : 1; //Tx Security engine arbiter state is not in Tx mask, reset value: 0x0, access type: RW
		uint32 sequencerEndIrqMask5 : 1; //Sequencer End IRQ mask, reset value: 0x0, access type: RW
		uint32 otfaIdleMask5 : 1; //OTFA in IDLE mask, reset value: 0x0, access type: RW
		uint32 baaIdleMask5 : 1; //BAA in IDLE mask, reset value: 0x0, access type: RW
		uint32 txHcIdleMask5 : 1; //TX_HC in IDLE mask, reset value: 0x0, access type: RW
		uint32 txDmaAlignerIdleMask5 : 1; //TX DMA Aligner in IDLE mask, reset value: 0x0, access type: RW
		uint32 dmaDoneMask5 : 1; //TX DMA Done indication mask, reset value: 0x0, access type: RW
		uint32 durationReadyMask5 : 1; //TX MPDU Duration field ready mask, reset value: 0x0, access type: RW
		uint32 deliaRecoveryDoneMask5 : 1; //Delia recovery done mask, reset value: 0x0, access type: RW
		uint32 tfGeneratorIdleMask5 : 1; //Reserved, reset value: 0x0, access type: RW
		uint32 macGenriscTxIrqConfRsvd20Mask5 : 1; //Reserved, reset value: 0x0, access type: RW
		uint32 macGenriscTxIrqConfRsvd21Mask5 : 1; //Reserved, reset value: 0x0, access type: RW
		uint32 macGenriscTxIrqConfRsvd22Mask5 : 1; //Reserved, reset value: 0x0, access type: RW
		uint32 macGenriscTxIrqConfRsvd23Mask5 : 1; //Reserved, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegMacHtExtensionsMacGenriscTxIrqConfMask5_u;

/*REG_MAC_HT_EXTENSIONS_RAMS1_RM 0x6C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rams1Rm0 : 3; //Other MAC RAMs RM, reset value: 0x3, access type: RW
		uint32 reserved0 : 1;
		uint32 rams1Rm1 : 3; //no description, reset value: 0x3, access type: RW
		uint32 reserved1 : 1;
		uint32 rams1Rm2 : 3; //no description, reset value: 0x3, access type: RW
		uint32 reserved2 : 1;
		uint32 rams1Rm3 : 3; //no description, reset value: 0x3, access type: RW
		uint32 reserved3 : 1;
		uint32 rams1Rm4 : 3; //no description, reset value: 0x3, access type: RW
		uint32 reserved4 : 1;
		uint32 rams1Rm5 : 3; //no description, reset value: 0x3, access type: RW
		uint32 reserved5 : 1;
		uint32 rams1Rm6 : 3; //no description, reset value: 0x3, access type: RW
		uint32 reserved6 : 1;
		uint32 rams1Rm7 : 3; //no description, reset value: 0x3, access type: RW
		uint32 reserved7 : 1;
	} bitFields;
} RegMacHtExtensionsRams1Rm_u;

/*REG_MAC_HT_EXTENSIONS_RAMS2_RM 0x70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rams2Rm0 : 3; //Other MAC RAMs RM, reset value: 0x3, access type: RW
		uint32 reserved0 : 1;
		uint32 rams2Rm1 : 3; //no description, reset value: 0x3, access type: RW
		uint32 reserved1 : 1;
		uint32 rams2Rm2 : 3; //no description, reset value: 0x3, access type: RW
		uint32 reserved2 : 1;
		uint32 rams2Rm3 : 3; //no description, reset value: 0x3, access type: RW
		uint32 reserved3 : 1;
		uint32 rams2Rm4 : 3; //no description, reset value: 0x3, access type: RW
		uint32 reserved4 : 1;
		uint32 rams2Rm5 : 3; //no description, reset value: 0x3, access type: RW
		uint32 reserved5 : 1;
		uint32 rams2Rm6 : 3; //no description, reset value: 0x3, access type: RW
		uint32 reserved6 : 1;
		uint32 rams2Rm7 : 3; //no description, reset value: 0x3, access type: RW
		uint32 reserved7 : 1;
	} bitFields;
} RegMacHtExtensionsRams2Rm_u;

/*REG_MAC_HT_EXTENSIONS_RAMS3_RM 0x74 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rams3Rm0 : 3; //Other MAC RAMs RM, reset value: 0x3, access type: RW
		uint32 reserved0 : 1;
		uint32 rams3Rm1 : 3; //no description, reset value: 0x3, access type: RW
		uint32 reserved1 : 1;
		uint32 rams3Rm2 : 3; //no description, reset value: 0x3, access type: RW
		uint32 reserved2 : 1;
		uint32 rams3Rm3 : 3; //no description, reset value: 0x3, access type: RW
		uint32 reserved3 : 1;
		uint32 rams3Rm4 : 3; //no description, reset value: 0x3, access type: RW
		uint32 reserved4 : 1;
		uint32 rams3Rm5 : 3; //no description, reset value: 0x3, access type: RW
		uint32 reserved5 : 1;
		uint32 rams3Rm6 : 3; //no description, reset value: 0x3, access type: RW
		uint32 reserved6 : 1;
		uint32 rams3Rm7 : 3; //no description, reset value: 0x3, access type: RW
		uint32 reserved7 : 1;
	} bitFields;
} RegMacHtExtensionsRams3Rm_u;

/*REG_MAC_HT_EXTENSIONS_RAMS4_RM 0x78 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rams4Rm0 : 3; //Other MAC RAMs RM, reset value: 0x3, access type: RW
		uint32 reserved0 : 1;
		uint32 rams4Rm1 : 3; //no description, reset value: 0x3, access type: RW
		uint32 reserved1 : 1;
		uint32 rams4Rm2 : 3; //no description, reset value: 0x3, access type: RW
		uint32 reserved2 : 1;
		uint32 rams4Rm3 : 3; //no description, reset value: 0x3, access type: RW
		uint32 reserved3 : 1;
		uint32 rams4Rm4 : 3; //no description, reset value: 0x3, access type: RW
		uint32 reserved4 : 1;
		uint32 rams4Rm5 : 3; //no description, reset value: 0x3, access type: RW
		uint32 reserved5 : 1;
		uint32 rams4Rm6 : 3; //no description, reset value: 0x3, access type: RW
		uint32 reserved6 : 1;
		uint32 rams4Rm7 : 3; //no description, reset value: 0x3, access type: RW
		uint32 reserved7 : 1;
	} bitFields;
} RegMacHtExtensionsRams4Rm_u;

/*REG_MAC_HT_EXTENSIONS_RAMS5_RM 0x7C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rams5Rm0 : 3; //Other MAC RAMs RM, reset value: 0x3, access type: RW
		uint32 reserved0 : 1;
		uint32 rams5Rm1 : 3; //no description, reset value: 0x3, access type: RW
		uint32 reserved1 : 1;
		uint32 rams5Rm2 : 3; //no description, reset value: 0x3, access type: RW
		uint32 reserved2 : 1;
		uint32 rams5Rm3 : 3; //no description, reset value: 0x3, access type: RW
		uint32 reserved3 : 1;
		uint32 rams5Rm4 : 3; //no description, reset value: 0x3, access type: RW
		uint32 reserved4 : 1;
		uint32 rams5Rm5 : 3; //no description, reset value: 0x3, access type: RW
		uint32 reserved5 : 1;
		uint32 rams5Rm6 : 3; //no description, reset value: 0x3, access type: RW
		uint32 reserved6 : 1;
		uint32 rams5Rm7 : 3; //no description, reset value: 0x3, access type: RW
		uint32 reserved7 : 1;
	} bitFields;
} RegMacHtExtensionsRams5Rm_u;

/*REG_MAC_HT_EXTENSIONS_RAMS6_RM 0x80 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rams6Rm0 : 3; //Other MAC RAMs RM, reset value: 0x3, access type: RW
		uint32 reserved0 : 1;
		uint32 rams6Rm1 : 3; //no description, reset value: 0x3, access type: RW
		uint32 reserved1 : 1;
		uint32 rams6Rm2 : 3; //no description, reset value: 0x3, access type: RW
		uint32 reserved2 : 1;
		uint32 rams6Rm3 : 3; //no description, reset value: 0x3, access type: RW
		uint32 reserved3 : 1;
		uint32 rams6Rm4 : 3; //no description, reset value: 0x3, access type: RW
		uint32 reserved4 : 1;
		uint32 rams6Rm5 : 3; //no description, reset value: 0x3, access type: RW
		uint32 reserved5 : 1;
		uint32 rams6Rm6 : 3; //no description, reset value: 0x3, access type: RW
		uint32 reserved6 : 1;
		uint32 rams6Rm7 : 3; //no description, reset value: 0x3, access type: RW
		uint32 reserved7 : 1;
	} bitFields;
} RegMacHtExtensionsRams6Rm_u;

/*REG_MAC_HT_EXTENSIONS_RAMS7_RM 0x84 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rams7Rm0 : 3; //Other MAC RAMs RM, reset value: 0x3, access type: RW
		uint32 reserved0 : 1;
		uint32 rams7Rm1 : 3; //no description, reset value: 0x3, access type: RW
		uint32 reserved1 : 1;
		uint32 rams7Rm2 : 3; //no description, reset value: 0x3, access type: RW
		uint32 reserved2 : 1;
		uint32 rams7Rm3 : 3; //no description, reset value: 0x3, access type: RW
		uint32 reserved3 : 1;
		uint32 rams7Rm4 : 3; //no description, reset value: 0x3, access type: RW
		uint32 reserved4 : 1;
		uint32 rams7Rm5 : 3; //no description, reset value: 0x3, access type: RW
		uint32 reserved5 : 1;
		uint32 rams7Rm6 : 3; //no description, reset value: 0x3, access type: RW
		uint32 reserved6 : 1;
		uint32 rams7Rm7 : 3; //no description, reset value: 0x3, access type: RW
		uint32 reserved7 : 1;
	} bitFields;
} RegMacHtExtensionsRams7Rm_u;

/*REG_MAC_HT_EXTENSIONS_STD_CONTENTION_EVENTS 0x90 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 genriscPreAggStdContention : 1; //STD access contention between Genrisc and Pre-aggregator. Cleared by writing '1' to std_contention_clr field, reset value: 0x0, access type: RO
		uint32 genriscSequencerStdContention : 1; //STD access contention between Genrisc and Sequencer. Cleared by writing '1' to std_contention_clr field, reset value: 0x0, access type: RO
		uint32 preAggSequencerStdContention : 1; //STD access contention between Pre-aggregator and Sequencer. Cleared by writing '1' to std_contention_clr field, reset value: 0x0, access type: RO
		uint32 reserved0 : 1;
		uint32 genriscPreAggStdContentionAddr : 11; //STD contention address in case of contention between Genrisc and Pre-aggregator. Locked for the first contention event. In order to reuse, need to clear genrisc_pre_agg_std_contention, reset value: 0x0, access type: RO
		uint32 reserved1 : 1;
		uint32 genriscSequencerStdContentionAddr : 11; //STD contention address in case of contention between Genrisc and Sequencer. Locked for the first contention event. In order to reuse, need to clear genrisc_sequencer_std_contention, reset value: 0x0, access type: RO
		uint32 reserved2 : 5;
	} bitFields;
} RegMacHtExtensionsStdContentionEvents_u;

/*REG_MAC_HT_EXTENSIONS_EVENT_CONTROL 0x94 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 stdContentionClr : 1; //Clear STD Contention events., reset value: 0x0, access type: WO
		uint32 reserved0 : 31;
	} bitFields;
} RegMacHtExtensionsEventControl_u;

/*REG_MAC_HT_EXTENSIONS_CLK_CONTROL_REG_FORCE_ON 0xA0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 clkControlForceOnMacGenriscTx : 1; //Clock Force On MAC_GENRISC_TX, reset value: 0x0, access type: RW
		uint32 clkControlForceOnMacGenriscRx : 1; //Clock Force On MAC_GENRISC_RX, reset value: 0x0, access type: RW
		uint32 clkControlForceOnMacGenriscTxStm : 1; //Clock Force On MAC_GENRISC_TX_STM, reset value: 0x0, access type: RW
		uint32 clkControlForceOnMacGenriscRxStm : 1; //Clock Force On MAC_GENRISC_RX_STM, reset value: 0x0, access type: RW
		uint32 clkControlForceOnRxMsduParser : 1; //Clock Force On MSDU_PARSER, reset value: 0x0, access type: RW
		uint32 clkControlForceOnRxMsduParserLogger : 1; //Clock Force On MSDU_PARSER_LOGGER, reset value: 0x0, access type: RW
		uint32 clkControlForceOnRxClassifier : 1; //Clock Force On Rx Classifier, reset value: 0x0, access type: RW
		uint32 clkControlForceOnTxHc : 1; //Clock Force On TX_HC, reset value: 0x0, access type: RW
		uint32 clkControlForceOnMacAddr2Index : 1; //Clock Force On MAC_ADDR2INDEX, reset value: 0x0, access type: RW
		uint32 clkControlForceOnTxDmaAligner : 1; //Clock Force On TX_DMA_ALIGNER, reset value: 0x0, access type: RW
		uint32 clkControlForceOnRxCoordinatorStatusTrace : 1; //Clock Force On RX coordinator status trace unit, reset value: 0x0, access type: RW
		uint32 clkControlForceOnRxCoordinatorStatusTraceFifos : 1; //Clock Force On RX coordinator status trace unit fifos, reset value: 0x0, access type: RW
		uint32 clkControlForceOnRxCoordinatorWrapUp : 1; //Clock Force On RX coordinator wrap up, reset value: 0x0, access type: RW
		uint32 clkControlForceOnRxCoordinatorStatistics : 1; //Clock Force On RX coordinator statistics, reset value: 0x0, access type: RW
		uint32 secEngGclkBypass : 7; //Security Clock Force On, reset value: 0x0, access type: RW
		uint32 delGclkBypass : 4; //Delia Clock Force On, reset value: 0x0, access type: RW
		uint32 clkControlForceOnAutoResp : 2; //Clock bypass for Auto response. bit [0] - bypass clock gater for auto resp general logic. bit[1] - bypass clock gater frame gen and length calculation, reset value: 0x0, access type: RW
		uint32 clkControlForceOnTxPdAcc : 1; //Clock Force On TX_PD Acc, reset value: 0x0, access type: RW
		uint32 clkControlForceOnPreAgg : 1; //Clock Force On for Pre AGG. , reset value: 0x0, access type: RW
		uint32 beaconTimersGclkBypass : 1; //Clock Force On for Beacon timers, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegMacHtExtensionsClkControlRegForceOn_u;

/*REG_MAC_HT_EXTENSIONS_CLK_CONTROL_REG_DYN_EN 0xA4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 clkControlDynEnMacGenriscTx : 1; //Clock Dynamic Enable MAC_GENRISC_TX, reset value: 0x0, access type: RW
		uint32 clkControlDynEnMacGenriscRx : 1; //Clock Dynamic Enable MAC_GENRISC_RX, reset value: 0x0, access type: RW
		uint32 clkControlDynEnMacGenriscTxStm : 1; //Clock Dynamic Enable MAC_GENRISC_TX_STM, reset value: 0x0, access type: RW
		uint32 clkControlDynEnMacGenriscRxStm : 1; //Clock Dynamic Enable MAC_GENRISC_RX_STM, reset value: 0x0, access type: RW
		uint32 clkControlDynEnRxMsduParser : 1; //Clock Dynamic Enable MSDU_PARSER, reset value: 0x0, access type: RW
		uint32 clkControlDynEnRxMsduParserLogger : 1; //Clock Dynamic Enable MSDU_PARSER_LOGGER, reset value: 0x0, access type: RW
		uint32 clkControlDynEnRxClassifier : 1; //Clock Dynamic Enable Rx Classifier, reset value: 0x0, access type: RW
		uint32 clkControlDynEnTxHc : 1; //Clock Dynamic Enable TX_HC, reset value: 0x0, access type: RW
		uint32 clkControlDynEnMacAddr2Index : 1; //Clock Dynamic Enable MAC_ADDR2INDEX, reset value: 0x0, access type: RW
		uint32 clkControlDynEnTxDmaAligner : 1; //Clock Dynamic Enable TX_DMA_ALIGNER, reset value: 0x0, access type: RW
		uint32 clkControlDynEnRxCoordinatorStatusTrace : 1; //Clock Dynamic Enable RX Coordinator status trace, reset value: 0x0, access type: RW
		uint32 clkControlDynEnRxCoordinatorStatusTraceFifos : 1; //Clock Dynamic Enable RX Coordinator status trace fifos, reset value: 0x0, access type: RW
		uint32 clkControlDynEnRxCoordinatorWrapUp : 1; //Clock Dynamic Enable RX Coordinator wrap up, reset value: 0x0, access type: RW
		uint32 clkControlDynEnRxCoordinatorStatistics : 1; //Clock Dynamic Enable RX Coordinator statistics, reset value: 0x0, access type: RW
		uint32 secEngSwGclkEn : 7; //Security Clock Dynamic Enable, reset value: 0x0, access type: RW
		uint32 delSwGclkEn : 4; //Delia Clock Dynamic Enable, reset value: 0x0, access type: RW
		uint32 clkControlDynEnAutoResp : 2; //Clock Dynamic Enable Auto response. bit [0] - dynamic enable for auto resp general logic. bit[1] -  dynamic enable for frame gen and length calculation, reset value: 0x0, access type: RW
		uint32 clkControlDynEnTxPdAcc : 1; //Clock Dynamic Enable TX_PD Acc, reset value: 0x0, access type: RW
		uint32 clkControlDynEnPreAgg : 1; //Clock Dynamic Enable Pre Agg, reset value: 0x0, access type: RW
		uint32 beaconTimersSwGclkEn : 1; //Clock Dynamic Enable Beacon timers, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegMacHtExtensionsClkControlRegDynEn_u;

/*REG_MAC_HT_EXTENSIONS_CLK_CONTROL_REG_FORCE_ON2 0xA8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 otfaGclkBypass : 2; //OTFA Clock Force On, reset value: 0x0, access type: RW
		uint32 txSequencerGclkBypass : 1; //Tx Sequencer Clock Force On, reset value: 0x0, access type: RW
		uint32 txSelGclkBypass : 8; //Tx Selector Clock Force On, reset value: 0x0, access type: RW
		uint32 clkControlForceOnTfGenerator : 1; //Clock Force On TF_GENERATOR, reset value: 0x0, access type: RW
		uint32 clkControlForceOnMbfm : 1; //Clock Force On MBFM, reset value: 0x0, access type: RW
		uint32 psSettingGclkBypass : 1; //PS setting Clock Force On, reset value: 0x0, access type: RW
		uint32 bsrcGclkBypass : 3; //BSRC Clock Force On, reset value: 0x0, access type: RW
		uint32 clkControlForceOnUlPostRx : 1; //UL_POST_RX Clock Force On, reset value: 0x0, access type: RW
		uint32 rxcGclkBypass : 1; //RxC Clock Force On, reset value: 0x0, access type: RW
		uint32 baaGclkBypass : 7; //BAA Clock Force On, reset value: 0x0, access type: RW
		uint32 pacExtrapGclkBypass : 1; //PAC extrap Clock Force On, reset value: 0x0, access type: RW
		uint32 txhMapGclkBypass : 1; //TxH MAP Clock Force On, reset value: 0x0, access type: RW
		uint32 txhTxcGclkBypass : 1; //TxH TxC Clock Force On, reset value: 0x0, access type: RW
		uint32 txhNtdGclkBypass : 1; //TxH NTD Clock Force On, reset value: 0x0, access type: RW
		uint32 durGclkBypass : 1; //DUR Clock Force On, reset value: 0x0, access type: RW
		uint32 reserved0 : 1;
	} bitFields;
} RegMacHtExtensionsClkControlRegForceOn2_u;

/*REG_MAC_HT_EXTENSIONS_CLK_CONTROL_REG_DYN_EN2 0xB0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 otfaSwGclkEn : 1; //OTFA Clock Dynamic Enable, reset value: 0x0, access type: RW
		uint32 reserved0 : 1;
		uint32 txSequencerSwGclkEn : 1; //Tx Sequencer Clock Dynamic Enable, reset value: 0x0, access type: RW
		uint32 txSelSwGclkEn : 8; //Tx Selector Clock Dynamic Enable, reset value: 0x0, access type: RW
		uint32 clkControlDynEnTfGenerator : 1; //Clock Dynamic Enable TF_GENERATOR, reset value: 0x0, access type: RW
		uint32 clkControlDynEnMbfm : 1; //Clock Dynamic Enable MBFM, reset value: 0x0, access type: RW
		uint32 psSettingSwGclkEn : 1; //PS setting Clock Dynamic Enable, reset value: 0x0, access type: RW
		uint32 bsrcSwGclkEn : 3; //BSRC Clock Dynamic Enable, reset value: 0x0, access type: RW
		uint32 clkControlDynEnUlPostRx : 1; //UL_POST_RX Clock Dynamic Enable, reset value: 0x0, access type: RW
		uint32 rxcSwGclkEn : 1; //RxC Clock Dynamic Enable, reset value: 0x0, access type: RW
		uint32 baaSwGclkEn : 7; //BAA Clock Dynamic Enable, reset value: 0x0, access type: RW
		uint32 pacExtrapGclkEn : 1; //PAC extrap Clock Dynamic Enable, reset value: 0x0, access type: RW
		uint32 txhMapSwGclkEn : 1; //TxH MAP Clock Dynamic Enable, reset value: 0x0, access type: RW
		uint32 txhTxcSwGclkEn : 1; //TxH TxC Clock Dynamic Enable, reset value: 0x0, access type: RW
		uint32 txhNtdSwGclkEn : 1; //TxH NTD Clock Dynamic Enable, reset value: 0x0, access type: RW
		uint32 durSwGclkEn : 1; //DUR Clock Dynamic Enable, reset value: 0x0, access type: RW
		uint32 reserved1 : 1;
	} bitFields;
} RegMacHtExtensionsClkControlRegDynEn2_u;

/*REG_MAC_HT_EXTENSIONS_HT_EXT_SPARE_GPO 0xC0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 htExtSpareGpo : 8; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 24;
	} bitFields;
} RegMacHtExtensionsHtExtSpareGpo_u;

/*REG_MAC_HT_EXTENSIONS_HT_EXT_SPARE_GPI 0xC4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 htExtSpareGpi : 8; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 24;
	} bitFields;
} RegMacHtExtensionsHtExtSpareGpi_u;

/*REG_MAC_HT_EXTENSIONS_MAC_GENRISC_RX_LEVEL_IRQ 0xC8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mips2MacGenriscRxIrq1 : 1; //MIPS to GenRisc RxH IRQs bit 1, reset value: 0x0, access type: RO
		uint32 dmaWrapGenriscRxIrq : 1; //DMA Wrapper to RxH IRQ, reset value: 0x0, access type: RO
		uint32 rxhPhyStatusDlmInListNotEmpty : 1; //RxH PHY Status DLM MPDU list not empty, reset value: 0x0, access type: RO
		uint32 rxhPassDlmInListNotEmpty : 1; //RxH Pass DLM MPDU list not empty, reset value: 0x0, access type: RO
		uint32 rxhErrDlmInListNotEmpty : 1; //RxH error DLM MPDU list not empty, reset value: 0x0, access type: RO
		uint32 rxSecurityCompleteIrq : 1; //Rx security complete IRQ, reset value: 0x0, access type: RO
		uint32 rxSecurityFailIrq : 1; //Rx Security fail IRQ, reset value: 0x0, access type: RO
		uint32 reserved0 : 25;
	} bitFields;
} RegMacHtExtensionsMacGenriscRxLevelIrq_u;

/*REG_MAC_HT_EXTENSIONS_MAC_GENRISC_RX_LEVEL_IRQ_MASK0 0xCC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mips2MacGenriscRxIrq1Mask0 : 1; //MIPS to GenRisc RxH IRQs bit 1 mask0, reset value: 0x0, access type: RW
		uint32 dmaWrapGenriscRxIrqMask0 : 1; //DMA Wrapper to RxH IRQ mask0, reset value: 0x0, access type: RW
		uint32 rxhPhyStatusDlmInListNotEmptyMask0 : 1; //RxH PHY Status DLM MPDU list not empty mask0, reset value: 0x0, access type: RW
		uint32 rxhPassDlmInListNotEmptyMask0 : 1; //RxH Pass DLM MPDU list not empty mask0, reset value: 0x0, access type: RW
		uint32 rxhErrDlmInListNotEmptyMask0 : 1; //RxH error DLM MPDU list not empty mask0, reset value: 0x0, access type: RW
		uint32 rxSecurityCompleteIrqMask0 : 1; //Rx security complete IRQ mask0, reset value: 0x0, access type: RW
		uint32 rxSecurityFailIrqMask0 : 1; //Rx Security fail IRQ mask0, reset value: 0x0, access type: RW
		uint32 reserved0 : 25;
	} bitFields;
} RegMacHtExtensionsMacGenriscRxLevelIrqMask0_u;

/*REG_MAC_HT_EXTENSIONS_MAC_GENRISC_RX_LEVEL_IRQ_MASK1 0xD0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mips2MacGenriscRxIrq1Mask1 : 1; //MIPS to GenRisc RxH IRQs bit 1 mask1, reset value: 0x0, access type: RW
		uint32 dmaWrapGenriscRxIrqMask1 : 1; //DMA Wrapper to RxH IRQ mask1, reset value: 0x0, access type: RW
		uint32 rxhPhyStatusDlmInListNotEmptyMask1 : 1; //RxH PHY Status DLM MPDU list not empty mask1, reset value: 0x0, access type: RW
		uint32 rxhPassDlmInListNotEmptyMask1 : 1; //RxH Pass DLM MPDU list not empty mask1, reset value: 0x0, access type: RW
		uint32 rxhErrDlmInListNotEmptyMask1 : 1; //RxH error DLM MPDU list not empty mask1, reset value: 0x0, access type: RW
		uint32 rxSecurityCompleteIrqMask1 : 1; //Rx security complete IRQ mask1, reset value: 0x0, access type: RW
		uint32 rxSecurityFailIrqMask1 : 1; //Rx Security fail IRQ mask1, reset value: 0x0, access type: RW
		uint32 reserved0 : 25;
	} bitFields;
} RegMacHtExtensionsMacGenriscRxLevelIrqMask1_u;

/*REG_MAC_HT_EXTENSIONS_MAC_GENRISC_RX_LEVEL_IRQ_MASK2 0xD4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mips2MacGenriscRxIrq1Mask2 : 1; //MIPS to GenRisc RxH IRQs bit 1 mask2, reset value: 0x0, access type: RW
		uint32 dmaWrapGenriscRxIrqMask2 : 1; //DMA Wrapper to RxH IRQ mask2, reset value: 0x0, access type: RW
		uint32 rxhPhyStatusDlmInListNotEmptyMask2 : 1; //RxH PHY Status DLM MPDU list not empty mask2, reset value: 0x0, access type: RW
		uint32 rxhPassDlmInListNotEmptyMask2 : 1; //RxH Pass DLM MPDU list not empty mask2, reset value: 0x0, access type: RW
		uint32 rxhErrDlmInListNotEmptyMask2 : 1; //RxH error DLM MPDU list not empty mask2, reset value: 0x0, access type: RW
		uint32 rxSecurityCompleteIrqMask2 : 1; //Rx security complete IRQ mask2, reset value: 0x0, access type: RW
		uint32 rxSecurityFailIrqMask2 : 1; //Rx Security fail IRQ mask2, reset value: 0x0, access type: RW
		uint32 reserved0 : 25;
	} bitFields;
} RegMacHtExtensionsMacGenriscRxLevelIrqMask2_u;

/*REG_MAC_HT_EXTENSIONS_MAC_GENRISC_RX_LEVEL_IRQ_MASK3 0xD8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mips2MacGenriscRxIrq1Mask3 : 1; //MIPS to GenRisc RxH IRQs bit 1 mask3, reset value: 0x0, access type: RW
		uint32 dmaWrapGenriscRxIrqMask3 : 1; //DMA Wrapper to RxH IRQ mask3, reset value: 0x0, access type: RW
		uint32 rxhPhyStatusDlmInListNotEmptyMask3 : 1; //RxH PHY Status DLM MPDU list not empty mask3, reset value: 0x0, access type: RW
		uint32 rxhPassDlmInListNotEmptyMask3 : 1; //RxH Pass DLM MPDU list not empty mask3, reset value: 0x0, access type: RW
		uint32 rxhErrDlmInListNotEmptyMask3 : 1; //RxH error DLM MPDU list not empty mask3, reset value: 0x0, access type: RW
		uint32 rxSecurityCompleteIrqMask3 : 1; //Rx security complete IRQ mask3, reset value: 0x0, access type: RW
		uint32 rxSecurityFailIrqMask3 : 1; //Rx Security fail IRQ mask3, reset value: 0x0, access type: RW
		uint32 reserved0 : 25;
	} bitFields;
} RegMacHtExtensionsMacGenriscRxLevelIrqMask3_u;



#endif // _MAC_HT_EXTENSIONS_REGS_H_
