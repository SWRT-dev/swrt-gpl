
/***********************************************************************************
File:				MacHtExtensionsRegs.h
Module:				macHtExtensions
SOC Revision:		843
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
#define	REG_MAC_HT_EXTENSIONS_HT_GCLK_ENABLE                     (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x0)
#define	REG_MAC_HT_EXTENSIONS_SW_RESETS_RELEASE                  (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x4)
#define REG_MAC_HT_EXTENSIONS_HW_SEMAPHORE						 (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x8)
#define	REG_MAC_HT_EXTENSIONS_WL_MAC_IRQ                         (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0xC)
#define	REG_MAC_HT_EXTENSIONS_WLAN_IP_BASE_ADDR                  (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x10)
#define	REG_MAC_HT_EXTENSIONS_LS_MODE                            (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x14)
#define	REG_MAC_HT_EXTENSIONS_HT_GCLK_DISABLE                    (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x18)
#define	REG_MAC_HT_EXTENSIONS_SW_RESETS_ASSERT                   (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x1C)
#define	REG_MAC_HT_EXTENSIONS_DELINEATOR_CRC_ERR_CNT             (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x20)
#define	REG_MAC_HT_EXTENSIONS_DELINEATOR_AGG_ERR_CNT             (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x24)
#define	REG_MAC_HT_EXTENSIONS_FC2_CRC_CALC_REQ                   (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x2C)
#define	REG_MAC_HT_EXTENSIONS_FC2_CRC_CALC_RESULTS               (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x30)
#define	REG_MAC_HT_EXTENSIONS_HT_GCLK_STATUS                     (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x34)
#define	REG_MAC_HT_EXTENSIONS_SW_RESETS_STATUS                   (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x38)
#define	REG_MAC_HT_EXTENSIONS_RX_STAT0_LO                        (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x40)
#define	REG_MAC_HT_EXTENSIONS_RX_STAT0_HI                        (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x44)
#define	REG_MAC_HT_EXTENSIONS_RX_STAT1_LO                        (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x48)
#define	REG_MAC_HT_EXTENSIONS_RX_STAT1_HI                        (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x4C)
#define	REG_MAC_HT_EXTENSIONS_RX_STAT2_LO                        (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x50)
#define	REG_MAC_HT_EXTENSIONS_RX_STAT2_HI                        (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x54)
#define	REG_MAC_HT_EXTENSIONS_RX_METRICS_LO                      (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x58)
#define	REG_MAC_HT_EXTENSIONS_RX_METRICS_HI                      (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x5C)
#define	REG_MAC_HT_EXTENSIONS_DELINEATOR_LOGGER_CTL              (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x60)
#define	REG_MAC_HT_EXTENSIONS_HW_LOGGER_STATUS                   (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x64)
#define	REG_MAC_HT_EXTENSIONS_ACTIVITY_LOGGER_CTL                (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x68)
#define	REG_MAC_HT_EXTENSIONS_GP_IF_TIMER_PRESCALER              (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x6C)
#define	REG_MAC_HT_EXTENSIONS_GP_IF_TIMEOUT_VAL                  (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x70)
#define	REG_MAC_HT_EXTENSIONS_GP_IF_SW_REQ                       (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x74)
#define	REG_MAC_HT_EXTENSIONS_GP_IF_TIMEOUT_TIME                 (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x78)
#define	REG_MAC_HT_EXTENSIONS_GP_IF_RESULT                       (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x7C)
#define	REG_MAC_HT_EXTENSIONS_BACK_CONFIG                        (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0xC0)
#define	REG_MAC_HT_EXTENSIONS_BACK_LAST_TA_LOW                   (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0xC4)
#define	REG_MAC_HT_EXTENSIONS_BACK_LAST_TA_HIGH                  (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0xC8)
#define	REG_MAC_HT_EXTENSIONS_BACK_LAST_TID                      (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0xCC)
#define	REG_MAC_HT_EXTENSIONS_BACK_EN_VECTOR                     (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0xD0)
#define	REG_MAC_HT_EXTENSIONS_BACK_ERR_CTR                       (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0xD4)
#define	REG_MAC_HT_EXTENSIONS_BACK_IN_WORK_VALUES                (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0xD8)
#define	REG_MAC_HT_EXTENSIONS_LINK_ADAPTATION_SPARE_REGISTER     (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0xDC)
#define	REG_MAC_HT_EXTENSIONS_BACK_REG_UPDATE_REQ                (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0xE0)
#define	REG_MAC_HT_EXTENSIONS_NPU_ARB_STM                        (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0xE4)
#define	REG_MAC_HT_EXTENSIONS_PHY_ARBITER_GCLK_BYPASS            (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0xE8)
#define	REG_MAC_HT_EXTENSIONS_REDUCED_FREQ_MODE                  (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0xEC)
#define	REG_MAC_HT_EXTENSIONS_ACTIVITY_LOGGER_SW_EVENT           (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0xF0)
#define	REG_MAC_HT_EXTENSIONS_TX_STREAMER_MODE                   (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0xF4)
#define	REG_MAC_HT_EXTENSIONS_TX_STREAMER_STATUS                 (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0xF8)
#define	REG_MAC_HT_EXTENSIONS_TX_STREAMER_ADDR                   (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0xFC)
#define	REG_MAC_HT_EXTENSIONS_DEL_PHY_RX_ERR_CTR                 (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x100)
#define	REG_MAC_HT_EXTENSIONS_DEL_WR_WHILE_FULL_CTR              (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x104)
#define	REG_MAC_HT_EXTENSIONS_DELINEATOR_LENGTH_VIOLATION_CTR    (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x108)
#define	REG_MAC_HT_EXTENSIONS_ZLD_CTR                            (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x10C)
#define	REG_MAC_HT_EXTENSIONS_GOOD_DEL_CTR                       (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x110)
#define	REG_MAC_HT_EXTENSIONS_DEL_DEBUG                          (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x114)
#define	REG_MAC_HT_EXTENSIONS_DEL_DEBUG2                         (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x118)
#define	REG_MAC_HT_EXTENSIONS_DEL_DEBUG3                         (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x11C)
#define	REG_MAC_HT_EXTENSIONS_DEL_DEBUG4                         (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x120)
#define	REG_MAC_HT_EXTENSIONS_GENRISC_RX_FREE_LIST_IDX           (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x124)
#define	REG_MAC_HT_EXTENSIONS_DEL_RX_HALT_REQUEST                (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x128)
#define	REG_MAC_HT_EXTENSIONS_DEL_RX_IN_HALT                     (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x12C)
#define	REG_MAC_HT_EXTENSIONS_HYP_MAC_TOP_GPR                    (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x130)
#define	REG_MAC_HT_EXTENSIONS_HYP_MAC_TOP_GPO                    (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x134)
#define	REG_MAC_HT_EXTENSIONS_HYP_MAC_TOP_GPI                    (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x138)
#define	REG_MAC_HT_EXTENSIONS_HYP_MAC_TOP_GPI2                   (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x13C)
#define	REG_MAC_HT_EXTENSIONS_MAC_GENRISC_TX_END_IRQ             (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x144)
#define	REG_MAC_HT_EXTENSIONS_MAC_GENRISC_TX_END_IRQ_MASK        (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x148)
#define	REG_MAC_HT_EXTENSIONS_MAC_GPR1                           (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x14C)
#define	REG_MAC_HT_EXTENSIONS_MAC_GENRISC_TX_LEVEL_IRQ_CONF      (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x160)
#define	REG_MAC_HT_EXTENSIONS_MAC_GENRISC_TX_IRQ_CONF_MASK0      (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x164)
#define	REG_MAC_HT_EXTENSIONS_MAC_GENRISC_TX_IRQ_CONF_MASK1      (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x168)
#define	REG_MAC_HT_EXTENSIONS_ENDIAN_SWAP_CONTROL                (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x16C)
#define	REG_MAC_HT_EXTENSIONS_RAM_CTL                            (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x174)
#define	REG_MAC_HT_EXTENSIONS_MAC_GENRISC_TX_IRQ_CONF_MASK2      (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x178)
#define	REG_MAC_HT_EXTENSIONS_MAC_GENRISC_TX_IRQ_CONF_MASK3      (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x17C)
#define	REG_MAC_HT_EXTENSIONS_MAC_GENRISC_TX_IRQ_CONF_MASK4      (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x180)
#define	REG_MAC_HT_EXTENSIONS_SPARE_GC                           (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x198)
#define	REG_MAC_HT_EXTENSIONS_FC2_MODE                           (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x1A0)
#define	REG_MAC_HT_EXTENSIONS_DEL_A_N_LEN_LIMIT                  (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x1A8)
#define	REG_MAC_HT_EXTENSIONS_DEL_B_AC_LEN_LIMIT                 (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x1AC)
#define	REG_MAC_HT_EXTENSIONS_DEL_MINMAX_LENGTH                  (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x1B0)
#define	REG_MAC_HT_EXTENSIONS_DELINEATOR_TIMING                  (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x1B4)
#define	REG_MAC_HT_EXTENSIONS_TRAINER_MODE                       (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x1B8)
#define	REG_MAC_HT_EXTENSIONS_AHB_ARB_BBCPU_PAGE_REG             (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x1C0)
#define	REG_MAC_HT_EXTENSIONS_AHB_ARB_SLSPI_PAGE                 (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x1C4)
#define	REG_MAC_HT_EXTENSIONS_AHB_ARB_GENRISC_HOST_PAGE_REG      (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x1C8)
#define	REG_MAC_HT_EXTENSIONS_HOST_IRQ_STATUS_REG                (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x1D0)
#define	REG_MAC_HT_EXTENSIONS_HOST_IRQ_MASK_REG                  (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x1D4)
#define	REG_MAC_HT_EXTENSIONS_SHRAM_RM                           (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x1F8)
#define	REG_MAC_HT_EXTENSIONS_RAMS_RM                            (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x1FC)
#define	REG_MAC_HT_EXTENSIONS_RX_TX_RAMS_RM                      (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x208)
#define	REG_MAC_HT_EXTENSIONS_SHRAM_EXT_RM                       (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x20C)
#define	REG_MAC_HT_EXTENSIONS_RAMS2_RM                           (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x210)
#define	REG_MAC_HT_EXTENSIONS_RAMS3_RM                           (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x214)
#define	REG_MAC_HT_EXTENSIONS_RAMS4_RM                           (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x218)
#define	REG_MAC_HT_EXTENSIONS_RAMS5_RM                           (MAC_HT_EXTENSIONS_BASE_ADDRESS + 0x21C)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_MAC_HT_EXTENSIONS_HT_GCLK_ENABLE 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:1;
		uint32 statisticsGclkEna:1;	// Statistics gated clock
		uint32 semaphoreGclkEna:1;	// Semaphore gated clock
		uint32 reserved1:2;
		uint32 ddrDmaGclkEna:1;	// DDR DMA gated clock
		uint32 macGenriscTxGclkEna:1;	// MAC GenRisc Tx gated clk
		uint32 delineatorGclkEna:1;	// Delineator gated clk
		uint32 macBlockAckGclkEna:1;	// MAC block ack gated clk
		uint32 txcTestFifoGclkEna:1;	// TxC test FIFO gated clk
		uint32 macTrainerGclkEna:1;	// mac_trainer_gclk
		uint32 reserved2:1;
		uint32 macTxStreamerGclkEna:1;	// mac_tx_streamer_gclk
		uint32 macGenriscRxGclkEna:1;	// MAC GenRisc Rx gated clk
		uint32 macGenriscHostGclkEna:1;	// MAC GenRisc Host gated clk
		uint32 reserved3:17;
	} bitFields;
} RegMacHtExtensionsHtGclkEnable_u;

/*REG_MAC_HT_EXTENSIONS_SW_RESETS_RELEASE 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swResetReserved0NcRel:1;
		uint32 statisticsSwResetRel:1;	// Statistics SW reset
		uint32 semaphoreSwResetRel:1;	// Semaphore SW reset
		uint32 swResetReserved1NcRel:1;
		uint32 loggerSwResetRel:1;
		uint32 ddrDmaSwResetRel:1;	// DDR DMA SW reset
		uint32 macGenriscTxSwResetRel:1;	// MAC GenRisc Tx SW reset
		uint32 delineatorSwResetRel:1;	// Delineator SW reset
		uint32 macBlockAckSwResetRel:1;	// MAC block ack SW reset
		uint32 txcTestFifoSwResetRel:1;	// TxC test FIFO SW reset
		uint32 macTrainerSwResetRel:1;	// mac trainer SW reset
		uint32 swResetReserved11NcRel:1;	// SW reset reserved 11
		uint32 macTxStreamerSwResetRel:1;	// mac tx streamer SW reset
		uint32 macGenriscRxSwResetRel:1;	// MAC GenRisc Rx SW reset
		uint32 macGenriscHostSwResetRel:1;	// MAC GenRisc Host SW reset
		uint32 qManagerLmacRegSwResetRel:1;	// Q Manager LMAC resgisters SW reset
		uint32 qManagerUmacRegSwResetRel:1;	// Q Manager UMAC resgisters SW reset
		uint32 qManagerRxRegSwResetRel:1;	// Q Manager RX GenRISC resgisters SW reset
		uint32 qManagerHostRegSwResetRel:1;	// Q Manager Host GenRISC resgisters SW reset
		uint32 qManagerTxRegSwResetRel:1;	// Q Manager TX GenRISC resgisters SW reset
		uint32 qManagerSwResetRel:1;	// Q Manager SW reset
		uint32 rxListsDlmSwResetRel:1;	// RX Lists DLM SW reset
		uint32 rxDataDlmSwResetRel:1;	// RX Data DLM SW reset
		uint32 txListsDlmSwResetRel:1;	// TX Lists DLM SW reset
		uint32 txDataDlmSwResetRel:1;	// TX Data DLM SW reset
		uint32 dlmRegSwResetRel:1;	// DLM resgisters SW reset
		uint32 rxOutHostIfSwResetRel:1;
		uint32 rxInHostIfSwResetRel:1;
		uint32 txOutHostIfSwResetRel:1;
		uint32 txInHostIfSwResetRel:1;
		uint32 hostIfRegFileSwResetRel:1;
		uint32 qmAgerSwResetRel:1;
	} bitFields;
} RegMacHtExtensionsSwResetsRelease_u;

/*REG_MAC_HT_EXTENSIONS_WL_MAC_IRQ 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:2;
		uint32 pasPac1TxError:1;	// PAS PAC 1 TX Error
		uint32 pasPac2TxError:1;	// PAS PAC 2 TX Error
		uint32 pasPac3TxError:1;	// PAS PAC 3 TX Error
		uint32 del1AutoFillCtrLimitIrq:1;	// Delia 1 auto fill ctr limit IRQ
		uint32 del2AutoFillCtrLimitIrq:1;	// Delia 2 auto fill ctr limit IRQ
		uint32 del3AutoFillCtrLimitIrq:1;	// Delia 3 auto fill ctr limit IRQ
		uint32 del2Genrisc0TxEndIrq:1;	// Delia 0 to genrisc Tx end IRQ
		uint32 del2Genrisc1TxEndIrq:1;	// Delia 1 to genrisc Tx end IRQ
		uint32 del2Genrisc2TxEndIrq:1;	// Delia 2 to genrisc Tx end IRQ
		uint32 del2Genrisc3TxEndIrq:1;	// Delia 3 to genrisc Tx end IRQ
		uint32 reserved1:2;
		uint32 del2GenriscAllTxEndIrq:1;	// All Delias to genrisc Tx end IRQ
		uint32 del0AutoFillEndIrq:1;	// Delia 0 auto fil end IRQ
		uint32 reserved2:2;
		uint32 pasPacTickTimer:1;	// PAS PAC Tick timer
		uint32 macPhyGpIfIrq:1;
		uint32 pasPac0TxError:1;	// PAS PAC 0 TX Error
		uint32 rxAggError:1;	// Rx Aggregation error indication
		uint32 rxEofError:1;	// Rx EOF error indication
		uint32 blockAckErrIrq:1;	// Block ack error indication
		uint32 externalIrqReg:1;	// external_irq_reg
		uint32 delLimitTimerExpiredIrq:1;	// del limit timer expired IRQ
		uint32 reserved3:1;
		uint32 delRxHaltIrq:1;	// Delineator Rx Halt interrupt
		uint32 npuArbBusErrIrq:1;	// NPU arb bus err IRQ
		uint32 reducedFreqIrq:1;	// reduced freq IRQ
		uint32 del0AutoFillCtrLimitIrq:1;	// Delia 0 auto fill ctr limit IRQ
		uint32 delineatorFifoFullIrq:1;	// Delineator FIFO Full IRQ
	} bitFields;
} RegMacHtExtensionsWlMacIrq_u;

/*REG_MAC_HT_EXTENSIONS_WLAN_IP_BASE_ADDR 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 wlanIpBaseAddr:32;	// WLAN IP Base address
	} bitFields;
} RegMacHtExtensionsWlanIpBaseAddr_u;

/*REG_MAC_HT_EXTENSIONS_LS_MODE 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lsMode:1;	// Main Shared RAM light sleep mode
		uint32 reserved0:31;
	} bitFields;
} RegMacHtExtensionsLsMode_u;

/*REG_MAC_HT_EXTENSIONS_HT_GCLK_DISABLE 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:1;
		uint32 statisticsGclkDis:1;	// Statistics gated clock
		uint32 semaphoreGclkDis:1;	// Semaphore gated clock
		uint32 reserved1:2;
		uint32 ddrDmaGclkDis:1;	// DDR DMA gated clock
		uint32 macGenriscTxGclkDis:1;	// MAC GenRisc Tx gated clk
		uint32 delineatorGclkDis:1;	// Delineator gated clk
		uint32 macBlockAckGclkDis:1;	// MAC block ack gated clk
		uint32 txcTestFifoGclkDis:1;	// TxC test FIFO gated clk
		uint32 macTrainerGclkDis:1;	// mac_trainer_gclk
		uint32 reserved2:1;
		uint32 macTxStreamerGclkDis:1;	// mac_tx_streamer_gclk
		uint32 macGenriscRxGclkDis:1;	// MAC GenRisc Rx gated clk
		uint32 macGenriscHostGclkDis:1;	// MAC GenRisc Host gated clk
		uint32 reserved3:17;
	} bitFields;
} RegMacHtExtensionsHtGclkDisable_u;

/*REG_MAC_HT_EXTENSIONS_SW_RESETS_ASSERT 0x1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swResetReserved0NcAssert:1;
		uint32 statisticsSwResetAssert:1;	// Statistics SW reset
		uint32 semaphoreSwResetAssert:1;	// Semaphore SW reset
		uint32 swResetReserved1NcAssert:1;
		uint32 loggerSwResetAssert:1;
		uint32 ddrDmaSwResetAssert:1;	// DDR DMA SW reset
		uint32 macGenriscTxSwResetAssert:1;	// MAC GenRisc Tx SW reset
		uint32 delineatorSwResetAssert:1;	// Delineator SW reset
		uint32 macBlockAckSwResetAssert:1;	// MAC block ack SW reset
		uint32 txcTestFifoSwResetAssert:1;	// TxC test FIFO SW reset
		uint32 macTrainerSwResetAssert:1;	// mac trainer SW reset
		uint32 swResetReserved11NcAssert:1;	// SW reset reserved 11
		uint32 macTxStreamerSwResetAssert:1;	// mac tx streamer SW reset
		uint32 macGenriscRxSwResetAssert:1;	// MAC GenRisc Rx SW reset
		uint32 macGenriscHostSwResetAssert:1;	// MAC GenRisc Host SW reset
		uint32 qManagerLmacRegSwResetAssert:1;	// Q Manager LMAC resgisters SW reset
		uint32 qManagerUmacRegSwResetAssert:1;	// Q Manager UMAC resgisters SW reset
		uint32 qManagerRxRegSwResetAssert:1;	// Q Manager RX GenRISC resgisters SW reset
		uint32 qManagerHostRegSwResetAssert:1;	// Q Manager Host GenRISC resgisters SW reset
		uint32 qManagerTxRegSwResetAssert:1;	// Q Manager TX GenRISC resgisters SW reset
		uint32 qManagerSwResetAssert:1;	// Q Manager SW reset
		uint32 rxListsDlmSwResetAssert:1;	// RX Lists DLM SW reset
		uint32 rxDataDlmSwResetAssert:1;	// RX Data DLM SW reset
		uint32 txListsDlmSwResetAssert:1;	// TX Lists DLM SW reset
		uint32 txDataDlmSwResetAssert:1;	// TX Data DLM SW reset
		uint32 dlmRegSwResetAssert:1;	// DLM resgisters SW reset
		uint32 rxOutHostIfSwResetAssert:1;
		uint32 rxInHostIfSwResetAssert:1;
		uint32 txOutHostIfSwResetAssert:1;
		uint32 txInHostIfSwResetAssert:1;
		uint32 hostIfRegFileSwResetAssert:1;
		uint32 qmAgerSwResetAssert:1;
	} bitFields;
} RegMacHtExtensionsSwResetsAssert_u;

/*REG_MAC_HT_EXTENSIONS_DELINEATOR_CRC_ERR_CNT 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 delineatorCrcErrCnt:32;	// Delineator CRC error count
	} bitFields;
} RegMacHtExtensionsDelineatorCrcErrCnt_u;

/*REG_MAC_HT_EXTENSIONS_DELINEATOR_AGG_ERR_CNT 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 delineatorAggErrCnt:16;	// Delineator Aggregate error count
		uint32 reserved0:16;
	} bitFields;
} RegMacHtExtensionsDelineatorAggErrCnt_u;

/*REG_MAC_HT_EXTENSIONS_FC2_CRC_CALC_REQ 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reservedBits:4;	// Packet reserved bits
		uint32 packetLength:12;	// Packet length
		uint32 reserved0:16;
	} bitFields;
} RegMacHtExtensionsFc2CrcCalcReq_u;

/*REG_MAC_HT_EXTENSIONS_FC2_CRC_CALC_RESULTS 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reservedBitsResult:4;	// Packet reserved bits
		uint32 packetLengthResult:12;	// Packet length
		uint32 delimiterCrcResult:8;	// Delimiter CRC result
		uint32 uniquePatternResult:8;	// Delimiter's unique pattern
	} bitFields;
} RegMacHtExtensionsFc2CrcCalcResults_u;

/*REG_MAC_HT_EXTENSIONS_HT_GCLK_STATUS 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:1;
		uint32 statisticsGclkStat:1;	// Statistics gated clock
		uint32 semaphoreGclkStat:1;	// Semaphore gated clock
		uint32 reserved1:2;
		uint32 ddrDmaGclkStat:1;	// DDR DMA gated clock
		uint32 macGenriscTxGclkStat:1;	// MAC GenRisc Tx gated clk
		uint32 delineatorGclkStat:1;	// Delineator gated clk
		uint32 macBlockAckGclkStat:1;	// MAC block ack gated clk
		uint32 txcTestFifoGclkStat:1;	// TxC test FIFO gated clk
		uint32 macTrainerGclkStat:1;	// mac_trainer_gclk
		uint32 reserved2:1;
		uint32 macTxStreamerGclkStat:1;	// mac_tx_streamer_gclk
		uint32 macGenriscRxGclkStat:1;	// MAC GenRisc Rx gated clk
		uint32 macGenriscHostGclkStat:1;	// MAC GenRisc Host gated clk
		uint32 reserved3:17;
	} bitFields;
} RegMacHtExtensionsHtGclkStatus_u;

/*REG_MAC_HT_EXTENSIONS_SW_RESETS_STATUS 0x38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swResetReserved0NcStat:1;
		uint32 statisticsSwResetStat:1;	// Statistics SW reset
		uint32 semaphoreSwResetStat:1;	// Semaphore SW reset
		uint32 swResetReserved1NcStat:1;
		uint32 loggerSwResetStat:1;
		uint32 ddrDmaSwResetStat:1;	// DDR DMA SW reset
		uint32 macGenriscTxSwResetStat:1;	// MAC GenRisc Tx SW reset
		uint32 delineatorSwResetStat:1;	// Delineator SW reset
		uint32 macBlockAckSwResetStat:1;	// MAC block ack SW reset
		uint32 txcTestFifoSwResetStat:1;	// TxC test FIFO SW reset
		uint32 macTrainerSwResetStat:1;	// mac trainer SW reset
		uint32 swResetReserved11NcStat:1;	// SW reset reserved 11
		uint32 macTxStreamerSwResetStat:1;	// mac tx streamer SW reset
		uint32 macGenriscRxSwResetStat:1;	// MAC GenRisc Rx SW reset
		uint32 macGenriscHostSwResetStat:1;	// MAC GenRisc Host SW reset
		uint32 qManagerLmacRegSwResetStat:1;	// Q Manager LMAC resgisters SW reset
		uint32 qManagerUmacRegSwResetStat:1;	// Q Manager UMAC resgisters SW reset
		uint32 qManagerRxRegSwResetStat:1;	// Q Manager RX GenRISC resgisters SW reset
		uint32 qManagerHostRegSwResetStat:1;	// Q Manager Host GenRISC resgisters SW reset
		uint32 qManagerTxRegSwResetStat:1;	// Q Manager TX GenRISC resgisters SW reset
		uint32 qManagerSwResetStat:1;	// Q Manager SW reset
		uint32 rxListsDlmSwResetStat:1;	// RX Lists DLM SW reset
		uint32 rxDataDlmSwResetStat:1;	// RX Data DLM SW reset
		uint32 txListsDlmSwResetStat:1;	// TX Lists DLM SW reset
		uint32 txDataDlmSwResetStat:1;	// TX Data DLM SW reset
		uint32 dlmRegSwResetStat:1;	// DLM resgisters SW reset
		uint32 rxOutHostIfSwResetStat:1;
		uint32 rxInHostIfSwResetStat:1;
		uint32 txOutHostIfSwResetStat:1;
		uint32 txInHostIfSwResetStat:1;
		uint32 hostIfRegFileSwResetStat:1;
		uint32 qmAgerSwResetStat:1;
	} bitFields;
} RegMacHtExtensionsSwResetsStatus_u;

/*REG_MAC_HT_EXTENSIONS_RX_STAT0_LO 0x40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxStat0Lo:32;
	} bitFields;
} RegMacHtExtensionsRxStat0Lo_u;

/*REG_MAC_HT_EXTENSIONS_RX_STAT0_HI 0x44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxStat0Hi:32;
	} bitFields;
} RegMacHtExtensionsRxStat0Hi_u;

/*REG_MAC_HT_EXTENSIONS_RX_STAT1_LO 0x48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxStat1Lo:32;
	} bitFields;
} RegMacHtExtensionsRxStat1Lo_u;

/*REG_MAC_HT_EXTENSIONS_RX_STAT1_HI 0x4C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxStat1Hi:32;
	} bitFields;
} RegMacHtExtensionsRxStat1Hi_u;

/*REG_MAC_HT_EXTENSIONS_RX_STAT2_LO 0x50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxStat2Lo:32;
	} bitFields;
} RegMacHtExtensionsRxStat2Lo_u;

/*REG_MAC_HT_EXTENSIONS_RX_STAT2_HI 0x54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxStat2Hi:32;
	} bitFields;
} RegMacHtExtensionsRxStat2Hi_u;

/*REG_MAC_HT_EXTENSIONS_RX_METRICS_LO 0x58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxMetricsLo:32;
	} bitFields;
} RegMacHtExtensionsRxMetricsLo_u;

/*REG_MAC_HT_EXTENSIONS_RX_METRICS_HI 0x5C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxMetricsHi:32;
	} bitFields;
} RegMacHtExtensionsRxMetricsHi_u;

/*REG_MAC_HT_EXTENSIONS_DELINEATOR_LOGGER_CTL 0x60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 delLoggerMode:1;	// "0" = Short report (only status and metrics) , "1" = Elaborated report (status, {ZLD count,delimiter error,MPDU length}, metrics)
		uint32 delLoggerEna:1;	// Logger enable. Set in order to enable delineator to output reports to its FIFO
		uint32 delLoggerPrio:2;
		uint32 reserved0:28;
	} bitFields;
} RegMacHtExtensionsDelineatorLoggerCtl_u;

/*REG_MAC_HT_EXTENSIONS_HW_LOGGER_STATUS 0x64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 delineatorLoggerActive:1;
		uint32 actLoggerRunning:1;
		uint32 reserved0:30;
	} bitFields;
} RegMacHtExtensionsHwLoggerStatus_u;

/*REG_MAC_HT_EXTENSIONS_ACTIVITY_LOGGER_CTL 0x68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 actLoggerPeriod:18;	// Sets period between samples taken by the logger. Given in system clock cycles
		uint32 actLoggerReportLength:6;	// Report length given in activity samples (each is 64 bit)
		uint32 actLoggerEna:1;
		uint32 actLoggerPrio:2;
		uint32 reserved0:5;
	} bitFields;
} RegMacHtExtensionsActivityLoggerCtl_u;

/*REG_MAC_HT_EXTENSIONS_GP_IF_TIMER_PRESCALER 0x6C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gpIfTimerPrescaler:9;	// MAC Phy General purpose I/F timer prescaler
		uint32 reserved0:23;
	} bitFields;
} RegMacHtExtensionsGpIfTimerPrescaler_u;

/*REG_MAC_HT_EXTENSIONS_GP_IF_TIMEOUT_VAL 0x70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gpIfTimeoutVal:20;	// MAC Phy General purpose interface timeout value; given in prescaler ticks
		uint32 reserved0:12;
	} bitFields;
} RegMacHtExtensionsGpIfTimeoutVal_u;

/*REG_MAC_HT_EXTENSIONS_GP_IF_SW_REQ 0x74 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gpIfSwReqStb:1;
		uint32 pacPhyRxGpType:1;
		uint32 reserved0:30;
	} bitFields;
} RegMacHtExtensionsGpIfSwReq_u;

/*REG_MAC_HT_EXTENSIONS_GP_IF_TIMEOUT_TIME 0x78 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gpIfTimeoutTime:20;	// GP interface timer current value
		uint32 reserved0:12;
	} bitFields;
} RegMacHtExtensionsGpIfTimeoutTime_u;

/*REG_MAC_HT_EXTENSIONS_GP_IF_RESULT 0x7C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gpIfTimeoutEvent:1;
		uint32 reserved0:31;
	} bitFields;
} RegMacHtExtensionsGpIfResult_u;

/*REG_MAC_HT_EXTENSIONS_BACK_CONFIG 0xC0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 barCompressed:1;	// BAR compressed mode
		uint32 barAckPolicy:1;	// BAR Ack policy
		uint32 useAckPolicyOnBar:1;	// Use ACK policy on BAR
		uint32 flushOnErrorNegative:1;	// Flush block ack database on error
		uint32 backPartialCaseOnly:1;	// Enable partial case only in module (do not keep history)
		uint32 reserved0:27;
	} bitFields;
} RegMacHtExtensionsBackConfig_u;

/*REG_MAC_HT_EXTENSIONS_BACK_LAST_TA_LOW 0xC4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 backLastTaLow:32;	// Block ack last transmitted address [31:0]
	} bitFields;
} RegMacHtExtensionsBackLastTaLow_u;

/*REG_MAC_HT_EXTENSIONS_BACK_LAST_TA_HIGH 0xC8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 backLastTaHigh:16;	// Block ack last transmitted address [47:32]
		uint32 reserved0:16;
	} bitFields;
} RegMacHtExtensionsBackLastTaHigh_u;

/*REG_MAC_HT_EXTENSIONS_BACK_LAST_TID 0xCC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 backLastTid:4;	// Block ack last TID
		uint32 reserved0:28;
	} bitFields;
} RegMacHtExtensionsBackLastTid_u;

/*REG_MAC_HT_EXTENSIONS_BACK_EN_VECTOR 0xD0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 backEnVector:16;	// Block ack enable vector
		uint32 reserved0:16;
	} bitFields;
} RegMacHtExtensionsBackEnVector_u;

/*REG_MAC_HT_EXTENSIONS_BACK_ERR_CTR 0xD4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 backErrCtr:16;	// Block ack error counter
		uint32 reserved0:16;
	} bitFields;
} RegMacHtExtensionsBackErrCtr_u;

/*REG_MAC_HT_EXTENSIONS_BACK_IN_WORK_VALUES 0xD8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 toFromDsInWork:2;	// To DS/From DS in work
		uint32 ackPolicyInWork:2;	// Ack policy in work
		uint32 subtypeInWork:4;	// Subtype in work
		uint32 typeInWork:2;	// Type in work
		uint32 baMainSm:4;	// Block ack FS< state
		uint32 reserved0:18;
	} bitFields;
} RegMacHtExtensionsBackInWorkValues_u;

/*REG_MAC_HT_EXTENSIONS_LINK_ADAPTATION_SPARE_REGISTER 0xDC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 linkAdaptationSpareRegister:32;	// Link adaptation spare register
	} bitFields;
} RegMacHtExtensionsLinkAdaptationSpareRegister_u;

/*REG_MAC_HT_EXTENSIONS_BACK_REG_UPDATE_REQ 0xE0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 backRegUpdateReq:32;	// back_reg_update_req
	} bitFields;
} RegMacHtExtensionsBackRegUpdateReq_u;

/*REG_MAC_HT_EXTENSIONS_NPU_ARB_STM 0xE4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 npuArbStm:3;	// npu_arb_stm
		uint32 reserved0:29;
	} bitFields;
} RegMacHtExtensionsNpuArbStm_u;

/*REG_MAC_HT_EXTENSIONS_PHY_ARBITER_GCLK_BYPASS 0xE8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phyArbiterGclkBypass:1;	// phy_arbiter_gclk_bypass
		uint32 reserved0:31;
	} bitFields;
} RegMacHtExtensionsPhyArbiterGclkBypass_u;

/*REG_MAC_HT_EXTENSIONS_REDUCED_FREQ_MODE 0xEC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reducedFreqMode:1;	// reduced_freq_mode
		uint32 reserved0:31;
	} bitFields;
} RegMacHtExtensionsReducedFreqMode_u;

/*REG_MAC_HT_EXTENSIONS_ACTIVITY_LOGGER_SW_EVENT 0xF0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 actLoggerSwEvent0:1;
		uint32 actLoggerSwEvent1:1;
		uint32 actLoggerSwEvent2:1;
		uint32 actLoggerSwEvent3:1;
		uint32 actLoggerSwEvent4:1;
		uint32 actLoggerSwEvent5:1;
		uint32 actLoggerSwEvent6:1;
		uint32 actLoggerSwEvent7:1;
		uint32 reserved0:24;
	} bitFields;
} RegMacHtExtensionsActivityLoggerSwEvent_u;

/*REG_MAC_HT_EXTENSIONS_TX_STREAMER_MODE 0xF4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txStreamerCdivr:8;	// Tx streamer clock divison ratio
		uint32 txStreamerModeReg:2;	// Tx streamer mode register
		uint32 reserved0:2;
		uint32 txStreamerCmultr:8;	// tx_streamer_cmultr
		uint32 reserved1:12;
	} bitFields;
} RegMacHtExtensionsTxStreamerMode_u;

/*REG_MAC_HT_EXTENSIONS_TX_STREAMER_STATUS 0xF8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txStreamerActiveIndication:1;	// Tx streamer active indication
		uint32 reserved0:31;
	} bitFields;
} RegMacHtExtensionsTxStreamerStatus_u;

/*REG_MAC_HT_EXTENSIONS_TX_STREAMER_ADDR 0xFC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txStreamerStartAddr:16;	// Tx streamer start address
		uint32 txStreamerEndAddr:16;	// Tx streamer end address
	} bitFields;
} RegMacHtExtensionsTxStreamerAddr_u;

/*REG_MAC_HT_EXTENSIONS_DEL_PHY_RX_ERR_CTR 0x100 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phyRxErrCtr:16;
		uint32 reserved0:16;
	} bitFields;
} RegMacHtExtensionsDelPhyRxErrCtr_u;

/*REG_MAC_HT_EXTENSIONS_DEL_WR_WHILE_FULL_CTR 0x104 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 delWrWhileFullCtr:16;
		uint32 reserved0:16;
	} bitFields;
} RegMacHtExtensionsDelWrWhileFullCtr_u;

/*REG_MAC_HT_EXTENSIONS_DELINEATOR_LENGTH_VIOLATION_CTR 0x108 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 delineaorLengthViolationCtr:16;
		uint32 reserved0:16;
	} bitFields;
} RegMacHtExtensionsDelineatorLengthViolationCtr_u;

/*REG_MAC_HT_EXTENSIONS_ZLD_CTR 0x10C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 zldCtr:16;	// Zero length delimiters counter
		uint32 reserved0:16;
	} bitFields;
} RegMacHtExtensionsZldCtr_u;

/*REG_MAC_HT_EXTENSIONS_GOOD_DEL_CTR 0x110 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 goodDelCtr:32;	// Good delimiters counter
	} bitFields;
} RegMacHtExtensionsGoodDelCtr_u;

/*REG_MAC_HT_EXTENSIONS_DEL_DEBUG 0x114 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lastAggLengthInWords:19;	// Last aggregate length
		uint32 delLastSubframeLengthInWords:13;	// Last subframe length
	} bitFields;
} RegMacHtExtensionsDelDebug_u;

/*REG_MAC_HT_EXTENSIONS_DEL_DEBUG2 0x118 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 delMainStm:5;	// Delineator FSM state
		uint32 delMtMetricsCtr:2;	// Delineator Metrics data counter
		uint32 delLastFrameLengthInBytes:14;	// Incoming frame length in words
		uint32 lastFrameInAgg:1;	// Delineator agg. Word counter
		uint32 reserved0:10;
	} bitFields;
} RegMacHtExtensionsDelDebug2_u;

/*REG_MAC_HT_EXTENSIONS_DEL_DEBUG3 0x11C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lastBadDelimiterLocation:18;	// Last bad delimiter location
		uint32 subframeCountInAggregate:7;	// Subframe count in current aggregate
		uint32 reserved0:7;
	} bitFields;
} RegMacHtExtensionsDelDebug3_u;

/*REG_MAC_HT_EXTENSIONS_DEL_DEBUG4 0x120 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lastBadDelimiter:32;	// del_debug4
	} bitFields;
} RegMacHtExtensionsDelDebug4_u;

/*REG_MAC_HT_EXTENSIONS_GENRISC_RX_FREE_LIST_IDX 0x124 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 genriscRxFreeListIdx:5;	// Free Q manager list index which used as an interrupt to Rx GenRisc
		uint32 reserved0:27;
	} bitFields;
} RegMacHtExtensionsGenriscRxFreeListIdx_u;

/*REG_MAC_HT_EXTENSIONS_DEL_RX_HALT_REQUEST 0x128 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 delRxHaltRequest:1;
		uint32 reserved0:31;
	} bitFields;
} RegMacHtExtensionsDelRxHaltRequest_u;

/*REG_MAC_HT_EXTENSIONS_DEL_RX_IN_HALT 0x12C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 delRxInHalt:1;
		uint32 reserved0:31;
	} bitFields;
} RegMacHtExtensionsDelRxInHalt_u;

/*REG_MAC_HT_EXTENSIONS_HYP_MAC_TOP_GPR 0x130 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hypMacTopGpr:32;	// hyp_mac_top_gpr
	} bitFields;
} RegMacHtExtensionsHypMacTopGpr_u;

/*REG_MAC_HT_EXTENSIONS_HYP_MAC_TOP_GPO 0x134 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hypMacTopGpo:16;	// hyp_mac_top_gpo
		uint32 reserved0:16;
	} bitFields;
} RegMacHtExtensionsHypMacTopGpo_u;

/*REG_MAC_HT_EXTENSIONS_HYP_MAC_TOP_GPI 0x138 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hypMacTopGpi:16;	// hyp_mac_top_gpi
		uint32 reserved0:16;
	} bitFields;
} RegMacHtExtensionsHypMacTopGpi_u;

/*REG_MAC_HT_EXTENSIONS_HYP_MAC_TOP_GPI2 0x13C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hypMacTopGpi2:16;	// hyp_mac_top_gpi2
		uint32 reserved0:16;
	} bitFields;
} RegMacHtExtensionsHypMacTopGpi2_u;

/*REG_MAC_HT_EXTENSIONS_MAC_GENRISC_TX_END_IRQ 0x144 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 senderPasPacTxError:4;	// Delia Tx error IRQ, 1 bit per Delia
		uint32 senderDel2GenriscTxEndIrq:4;	// Delia Tx end IRQ,1 bit per Delia
		uint32 senderDel2GenriscAllTxEndIrq:1;	// All the Delia machines have been completed IRQ
		uint32 reserved0:23;
	} bitFields;
} RegMacHtExtensionsMacGenriscTxEndIrq_u;

/*REG_MAC_HT_EXTENSIONS_MAC_GENRISC_TX_END_IRQ_MASK 0x148 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 senderPasPacTxErrorMask:4;	// Delia Tx error IRQ enable, 1 bit per Delia
		uint32 senderDel2GenriscTxEndIrqMask:4;	// Delia Tx end IRQ enable, 1 bit per Delia
		uint32 senderDel2GenriscAllTxEndIrqMask:1;	// All the Delia machines have been completed IRQ enable
		uint32 reserved0:23;
	} bitFields;
} RegMacHtExtensionsMacGenriscTxEndIrqMask_u;

/*REG_MAC_HT_EXTENSIONS_MAC_GPR1 0x14C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 macGpr1:32;	// MAC GP register 1
	} bitFields;
} RegMacHtExtensionsMacGpr1_u;

/*REG_MAC_HT_EXTENSIONS_MAC_GENRISC_TX_LEVEL_IRQ_CONF 0x160 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txPdAccIdle:1;	// Tx PD accelerator in IDLE
		uint32 dmaCh0Idle:1;	// DMA Channel0 idle 
		uint32 senderBaaIrq:1;	// Sender BAA IRQ 
		uint32 txhNtdTxGenriscSwReqIrq:1;	// TxH NTD genrisc_tx SW req IRQ 
		uint32 delLimitTimerExpiredIrq:1;	// Delia limit timer expired IRQ 
		uint32 mips2MacGenriscTxIrq:2;	// mips2mac genrisc_tx IRQ 
		uint32 sequencerIrq:1;	// Sequencer IRQ 
		uint32 txSecurityFailIrq:4;	// Tx Security fail IRQ , , 1 bit per security engine
		uint32 txSecurityDoneIrq:4;	// Tx Security done IRQ , , 1 bit per security engine
		uint32 delAutoFillCtrLimitIrq:4;	// Delia auto fill counter limit IRQ , , 1 bit per Delia
		uint32 txPdAccPendingPtdNotEmpty:1;	// Tx PD accelerator Pending PTD accumulator is not empty
		uint32 txSecurityArbSmIdle:1;	// Tx Security engine arbiter state is not in Tx
		uint32 sequencerEndIrq:1;	// Sequencer End IRQ 
		uint32 macGenriscTxIrqConfRsvd:1;	// Reserved
		uint32 reserved0:8;
	} bitFields;
} RegMacHtExtensionsMacGenriscTxLevelIrqConf_u;

/*REG_MAC_HT_EXTENSIONS_MAC_GENRISC_TX_IRQ_CONF_MASK0 0x164 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txPdAccIdleMask0:1;	// Tx PD accelerator idle mask
		uint32 dmaCh0IdleMask0:1;	// DMA Channel0 idle enable
		uint32 senderBaaIrqMask0:1;	// Sender BAA IRQ enable
		uint32 txhNtdTxGenriscSwReqIrqMask0:1;	// TxH NTD genrisc_tx SW req IRQ enable
		uint32 delLimitTimerExpiredIrqMask0:1;	// Delia limit timer expired IRQ enable
		uint32 mips2MacGenriscTxIrqMask0:2;	// mips2mac genrisc_tx IRQ enable
		uint32 sequencerIrqMask0:1;	// Sequencer IRQ enable
		uint32 txSecurityFailIrqMask0:4;	// Tx Security fail IRQ enable, , 1 bit per security engine
		uint32 txSecurityDoneIrqMask0:4;	// Tx Security done IRQ enable, , 1 bit per security engine
		uint32 delAutoFillCtrLimitIrqMask0:4;	// Delia auto fill counter limit IRQ enable, , 1 bit per Delia
		uint32 txPdAccPendingPtdNotEmptyMask0:1;	// Tx PTD acclerartor pending PTD not empty enable
		uint32 txSecurityArbSmIdleMask0:1;	// Tx Security engine arbiter state is not in Tx enable
		uint32 sequencerEndIrqMask0:1;	// Sequencer End IRQ enable
		uint32 macGenriscTxIrqConfRsvdMask0:1;	// Reserved
		uint32 reserved0:8;
	} bitFields;
} RegMacHtExtensionsMacGenriscTxIrqConfMask0_u;

/*REG_MAC_HT_EXTENSIONS_MAC_GENRISC_TX_IRQ_CONF_MASK1 0x168 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txPdAccIdleMask1:1;	// Tx PD accelerator idle mask
		uint32 dmaCh0IdleMask1:1;	// DMA Channel0 idle enable
		uint32 senderBaaIrqMask1:1;	// Sender BAA IRQ enable
		uint32 txhNtdTxGenriscSwReqIrqMask1:1;	// TxH NTD genrisc_tx SW req IRQ enable
		uint32 delLimitTimerExpiredIrqMask1:1;	// Delia limit timer expired IRQ enable
		uint32 mips2MacGenriscTxIrqMask1:2;	// mips2mac genrisc_tx IRQ enable
		uint32 sequencerIrqMask1:1;	// Sequencer IRQ enable
		uint32 txSecurityFailIrqMask1:4;	// Tx Security fail IRQ enable, , 1 bit per security engine
		uint32 txSecurityDoneIrqMask1:4;	// Tx Security done IRQ enable, , 1 bit per security engine
		uint32 delAutoFillCtrLimitIrqMask1:4;	// Delia auto fill counter limit IRQ enable, , 1 bit per Delia
		uint32 txPdAccPendingPtdNotEmptyMask1:1;	// Tx PTD acclerartor pending PTD not empty enable
		uint32 txSecurityArbSmIdleMask1:1;	// Tx Security engine arbiter state is not in Tx enable
		uint32 sequencerEndIrqMask1:1;	// Sequencer End IRQ enable
		uint32 macGenriscTxIrqConfRsvdMask1:1;	// Reserved
		uint32 reserved0:8;
	} bitFields;
} RegMacHtExtensionsMacGenriscTxIrqConfMask1_u;

/*REG_MAC_HT_EXTENSIONS_ENDIAN_SWAP_CONTROL 0x16C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 wlanScdAhbMasterWriteSwapEn:1;	// enable WLAN SCD's AHB master swap for Write data bus based on the AHB size
		uint32 wlanScdAhbMasterWriteFullByteSwapEn:1;	// enable WLAN SCD's AHB master full byte swap for Write data bus
		uint32 wlanScdAhbMasterReadSwapEn:1;	// enable WLAN SCD's AHB master swap for Read data bus based on the AHB size
		uint32 wlanScdAhbMasterReadFullByteswapEn:1;	// enable WLAN SCD's AHB master full byte swap for Read data bus
		uint32 wlanDmaAhbMasterWriteSwapEn:1;	// enable WLAN DMA's AHB master swap for Write data bus based on the AHB size
		uint32 wlanDmaAhbMasterWriteFullByteSwapEn:1;	// enable WLAN DMA's AHB master full byte swap for Write data bus
		uint32 wlanDmaAhbMasterReadSwapEn:1;	// enable WLAN DMA's AHB master swap for Read data bus based on the AHB size
		uint32 wlanDmaAhbMasterReadFullByteswapEn:1;	// enable WLAN DMA's AHB master full byte swap for Read data bus
		uint32 wlanShramAhbSlaveWriteSwapEn:1;	// enable WLAN SHRAM's AHB slave swap for Write data bus based on the AHB size
		uint32 wlanShramAhbSlaveWriteFullByteSwapEn:1;	// enable WLAN SHRAM's AHB slave full byte swap for Write data bus
		uint32 wlanShramAhbSlaveReadSwapEn:1;	// enable WLAN SHRAM's AHB slave swap for Read data bus based on the AHB size
		uint32 wlanShramAhbSlaveReadFullByteswapEn:1;	// enable WLAN SHRAM's AHB slave full byte swap for Read data bus
		uint32 wlanPcihAhbSlaveWriteSwapEn:1;	// enable WLAN PCIH Bridge's AHB slave swap for Write data bus based on the AHB size
		uint32 wlanPcihAhbSlaveWriteFullByteSwapEn:1;	// enable WLAN PCIH Bridge's AHB slave full byte swap for Write data bus
		uint32 wlanPcihAhbSlaveReadSwapEn:1;	// enable WLAN PCIH Bridge's AHB slave swap for Read data bus based on the AHB size
		uint32 wlanPcihAhbSlaveReadFullByteswapEn:1;	// enable WLAN PCIH Bridge's AHB slave full byte swap for Read data bus
		uint32 aximWdataSwapperMode:2;	// AXI master write data swap mode
		uint32 aximRdataSwapperMode:2;	// AXI master read data swap mode
		uint32 axisWdataSwapperMode:2;	// AXI slave write data swap mode
		uint32 axisRdataSwapperMode:2;	// AXI slave read data swap mode
		uint32 reserved0:8;
	} bitFields;
} RegMacHtExtensionsEndianSwapControl_u;

/*REG_MAC_HT_EXTENSIONS_RAM_CTL 0x174 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:1;
		uint32 ramTestMode:1;	// RAM test mode
		uint32 memGlobalRm:2;	// RAM global RM value
		uint32 reserved1:28;
	} bitFields;
} RegMacHtExtensionsRamCtl_u;

/*REG_MAC_HT_EXTENSIONS_MAC_GENRISC_TX_IRQ_CONF_MASK2 0x178 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txPdAccIdleMask2:1;	// Tx PD accelerator idle mask
		uint32 dmaCh0IdleMask2:1;	// DMA Channel0 idle enable
		uint32 senderBaaIrqMask2:1;	// Sender BAA IRQ enable
		uint32 txhNtdTxGenriscSwReqIrqMask2:1;	// TxH NTD genrisc_tx SW req IRQ enable
		uint32 delLimitTimerExpiredIrqMask2:1;	// Delia limit timer expired IRQ enable
		uint32 mips2MacGenriscTxIrqMask2:2;	// mips2mac genrisc_tx IRQ enable
		uint32 sequencerIrqMask2:1;	// Sequencer IRQ enable
		uint32 txSecurityFailIrqMask2:4;	// Tx Security fail IRQ enable, , 1 bit per security engine
		uint32 txSecurityDoneIrqMask2:4;	// Tx Security done IRQ enable, , 1 bit per security engine
		uint32 delAutoFillCtrLimitIrqMask2:4;	// Delia auto fill counter limit IRQ enable, , 1 bit per Delia
		uint32 txPdAccPendingPtdNotEmptyMask2:1;	// Tx PTD acclerartor pending PTD not empty enable
		uint32 txSecurityArbSmIdleMask2:1;	// Tx Security engine arbiter state is not in Tx enable
		uint32 sequencerEndIrqMask2:1;	// Sequencer End IRQ enable
		uint32 macGenriscTxIrqConfRsvdMask2:1;	// Reserved
		uint32 reserved0:8;
	} bitFields;
} RegMacHtExtensionsMacGenriscTxIrqConfMask2_u;

/*REG_MAC_HT_EXTENSIONS_MAC_GENRISC_TX_IRQ_CONF_MASK3 0x17C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txPdAccIdleMask3:1;	// Tx PD accelerator idle mask
		uint32 dmaCh0IdleMask3:1;	// DMA Channel0 idle enable
		uint32 senderBaaIrqMask3:1;	// Sender BAA IRQ enable
		uint32 txhNtdTxGenriscSwReqIrqMask3:1;	// TxH NTD genrisc_tx SW req IRQ enable
		uint32 delLimitTimerExpiredIrqMask3:1;	// Delia limit timer expired IRQ enable
		uint32 mips2MacGenriscTxIrqMask3:2;	// mips2mac genrisc_tx IRQ enable
		uint32 sequencerIrqMask3:1;	// Sequencer IRQ enable
		uint32 txSecurityFailIrqMask3:4;	// Tx Security fail IRQ enable, , 1 bit per security engine
		uint32 txSecurityDoneIrqMask3:4;	// Tx Security done IRQ enable, , 1 bit per security engine
		uint32 delAutoFillCtrLimitIrqMask3:4;	// Delia auto fill counter limit IRQ enable, , 1 bit per Delia
		uint32 txPdAccPendingPtdNotEmptyMask3:1;	// Tx PTD acclerartor pending PTD not empty enable
		uint32 txSecurityArbSmIdleMask3:1;	// Tx Security engine arbiter state is not in Tx enable
		uint32 sequencerEndIrqMask3:1;	// Sequencer End IRQ enable
		uint32 macGenriscTxIrqConfRsvdMask3:1;	// Reserved
		uint32 reserved0:8;
	} bitFields;
} RegMacHtExtensionsMacGenriscTxIrqConfMask3_u;

/*REG_MAC_HT_EXTENSIONS_MAC_GENRISC_TX_IRQ_CONF_MASK4 0x180 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txPdAccIdleMask4:1;	// Tx PD accelerator idle mask
		uint32 dmaCh0IdleMask4:1;	// DMA Channel0 idle enable
		uint32 senderBaaIrqMask4:1;	// Sender BAA IRQ enable
		uint32 txhNtdTxGenriscSwReqIrqMask4:1;	// TxH NTD genrisc_tx SW req IRQ enable
		uint32 delLimitTimerExpiredIrqMask4:1;	// Delia limit timer expired IRQ enable
		uint32 mips2MacGenriscTxIrqMask4:2;	// mips2mac genrisc_tx IRQ enable
		uint32 sequencerIrqMask4:1;	// Sequencer IRQ enable
		uint32 txSecurityFailIrqMask4:4;	// Tx Security fail IRQ enable, , 1 bit per security engine
		uint32 txSecurityDoneIrqMask4:4;	// Tx Security done IRQ enable, , 1 bit per security engine
		uint32 delAutoFillCtrLimitIrqMask4:4;	// Delia auto fill counter limit IRQ enable, , 1 bit per Delia
		uint32 txPdAccPendingPtdNotEmptyMask4:1;	// Tx PTD acclerartor pending PTD not empty enable
		uint32 txSecurityArbSmIdleMask4:1;	// Tx Security engine arbiter state is not in Tx enable
		uint32 sequencerEndIrqMask4:1;	// Sequencer End IRQ enable
		uint32 macGenriscTxIrqConfRsvdMask4:1;	// Reserved
		uint32 reserved0:8;
	} bitFields;
} RegMacHtExtensionsMacGenriscTxIrqConfMask4_u;

/*REG_MAC_HT_EXTENSIONS_SPARE_GC 0x198 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 spareGcBypass:9;	// Spages gated clock bypass
		uint32 spareGcEn:9;	// Spares gated clock enable
		uint32 reserved0:14;
	} bitFields;
} RegMacHtExtensionsSpareGc_u;

/*REG_MAC_HT_EXTENSIONS_FC2_MODE 0x1A0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 acceptIllegalSubframeLength:1;	// Accept illegal subframe length in delimiter
		uint32 ignoreNonzeroReservedFieldInDelimiter:1;	// Ignore nonzero value in delimiter's reserved field
		uint32 checkMinimalSubframeLengthInDelimiter:1;	// Check minimal subframe length in delimiter
		uint32 checkMaximalSubframeLengthInDelimiter:1;	// Check maximal subframe length in delimiter
		uint32 deliniatorVhtDebugMode:1;	// VHT phy controled aggregate mode
		uint32 delEnablePpduMaxLenCheck:1;
		uint32 reserved0:26;
	} bitFields;
} RegMacHtExtensionsFc2Mode_u;

/*REG_MAC_HT_EXTENSIONS_DEL_A_N_LEN_LIMIT 0x1A8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 delMaxNLen:16;
		uint32 delMaxALen:16;
	} bitFields;
} RegMacHtExtensionsDelANLenLimit_u;

/*REG_MAC_HT_EXTENSIONS_DEL_B_AC_LEN_LIMIT 0x1AC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 delMaxAcLen:20;
		uint32 delMaxBLen:12;
	} bitFields;
} RegMacHtExtensionsDelBAcLenLimit_u;

/*REG_MAC_HT_EXTENSIONS_DEL_MINMAX_LENGTH 0x1B0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 delMinSubframeLength:14;	// Delineator minimal subframe length
		uint32 delMaxSubframeLength:14;	// Delineator maximal subframe length
		uint32 reserved0:4;
	} bitFields;
} RegMacHtExtensionsDelMinmaxLength_u;

/*REG_MAC_HT_EXTENSIONS_DELINEATOR_TIMING 0x1B4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 delDataToReadyFall:3;
		uint32 reserved0:1;
		uint32 delReadyRiseToData:4;
		uint32 delEofSpacingDelay:3;
		uint32 reserved1:1;
		uint32 delEoaSpacingDelay:4;
		uint32 reserved2:16;
	} bitFields;
} RegMacHtExtensionsDelineatorTiming_u;

/*REG_MAC_HT_EXTENSIONS_TRAINER_MODE 0x1B8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 trainerMode:1;	// Trainer mode
		uint32 reserved0:31;
	} bitFields;
} RegMacHtExtensionsTrainerMode_u;

/*REG_MAC_HT_EXTENSIONS_AHB_ARB_BBCPU_PAGE_REG 0x1C0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbcpuPageReg:16;	// BBCPU page register
		uint32 bbcpuPageMask:16;	// BBCPU page mask
	} bitFields;
} RegMacHtExtensionsAhbArbBbcpuPageReg_u;

/*REG_MAC_HT_EXTENSIONS_AHB_ARB_SLSPI_PAGE 0x1C4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 slaveSpiPageReg:16;	// Slave SPI page register
		uint32 slaveSpiPageMask:16;	// Slave SPI page mask
	} bitFields;
} RegMacHtExtensionsAhbArbSlspiPage_u;

/*REG_MAC_HT_EXTENSIONS_AHB_ARB_GENRISC_HOST_PAGE_REG 0x1C8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 genriscHostPageReg:16;	// MAC GenRIsc Host page register
		uint32 genriscHostPageMask:16;	// MAC GenRisc Host page mask
	} bitFields;
} RegMacHtExtensionsAhbArbGenriscHostPageReg_u;

/*REG_MAC_HT_EXTENSIONS_HOST_IRQ_STATUS_REG 0x1D0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txOutHdCountNotEmptyIrq:1;	// Tx data HD count not empty IRQ
		uint32 rxOutHdCountNotEmptyIrq:1;	// Rx data HD count not empty IRQ
		uint32 txOutMgmtCountNotEmptyIrq:1;	// Tx management HD count not empty IRQ
		uint32 rxOutMgmtCountNotEmptyIrq:1;	// Tx management HD count not empty IRQ
		uint32 macHostMailboxCountNotEmptyIrq:1;	// Host mailbox message count not empty IRQ
		uint32 rabPhiInterrupt:1;	// RAB PHI interrupt
		uint32 wlanMacXbarHostErrorIrq:1;	// MAC crossbar error to host IRQ
		uint32 reserved0:25;
	} bitFields;
} RegMacHtExtensionsHostIrqStatusReg_u;

/*REG_MAC_HT_EXTENSIONS_HOST_IRQ_MASK_REG 0x1D4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hostIrqMask:8;	// Host IRQ mask
		uint32 reserved0:24;
	} bitFields;
} RegMacHtExtensionsHostIrqMaskReg_u;

/*REG_MAC_HT_EXTENSIONS_SHRAM_RM 0x1F8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 shramRm:30;	// Shared RAM RM
		uint32 reserved0:2;
	} bitFields;
} RegMacHtExtensionsShramRm_u;

/*REG_MAC_HT_EXTENSIONS_RAMS_RM 0x1FC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ramsRm:30;	// Other MAC RAMs RM
		uint32 reserved0:2;
	} bitFields;
} RegMacHtExtensionsRamsRm_u;

/*REG_MAC_HT_EXTENSIONS_RX_TX_RAMS_RM 0x208 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxTxRamsRm:15;	// Rx and Tx circular buffers RAM RM
		uint32 reserved0:17;
	} bitFields;
} RegMacHtExtensionsRxTxRamsRm_u;

/*REG_MAC_HT_EXTENSIONS_SHRAM_EXT_RM 0x20C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 shramExtRm:15;	// Shared RAM RM
		uint32 reserved0:17;
	} bitFields;
} RegMacHtExtensionsShramExtRm_u;

/*REG_MAC_HT_EXTENSIONS_RAMS2_RM 0x210 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rams2Rm:30;	// Other MAC RAMs RM
		uint32 reserved0:2;
	} bitFields;
} RegMacHtExtensionsRams2Rm_u;

/*REG_MAC_HT_EXTENSIONS_RAMS3_RM 0x214 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rams3Rm:30;	// Other MAC RAMs RM
		uint32 reserved0:2;
	} bitFields;
} RegMacHtExtensionsRams3Rm_u;

/*REG_MAC_HT_EXTENSIONS_RAMS4_RM 0x218 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rams4Rm:30;	// Other MAC RAMs RM
		uint32 reserved0:2;
	} bitFields;
} RegMacHtExtensionsRams4Rm_u;

/*REG_MAC_HT_EXTENSIONS_RAMS5_RM 0x21C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rams5Rm:30;	// Other MAC RAMs RM
		uint32 reserved0:2;
	} bitFields;
} RegMacHtExtensionsRams5Rm_u;



#endif // _MAC_HT_EXTENSIONS_REGS_H_
