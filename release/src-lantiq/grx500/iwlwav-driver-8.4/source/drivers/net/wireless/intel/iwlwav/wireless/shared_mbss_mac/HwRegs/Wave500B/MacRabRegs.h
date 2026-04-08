
/***********************************************************************************
File:				MacRabRegs.h
Module:				macRab
SOC Revision:		843
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _MAC_RAB_REGS_H_
#define _MAC_RAB_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define MAC_RAB_BASE_ADDRESS                             MEMORY_MAP_UNIT_18_BASE_ADDRESS
#define	REG_MAC_RAB_ENABLE_UPI_INTERRUPT                    (MAC_RAB_BASE_ADDRESS + 0x0)
#define	REG_MAC_RAB_UPI_INTERRUPT                           (MAC_RAB_BASE_ADDRESS + 0x4)
#define	REG_MAC_RAB_UPI_INTERRUPT_SET                       (MAC_RAB_BASE_ADDRESS + 0x8)
#define	REG_MAC_RAB_UPI_INTERRUPT_CLEAR                     (MAC_RAB_BASE_ADDRESS + 0xC)
#define	REG_MAC_RAB_ENABLE_LPI_INTERRUPT                    (MAC_RAB_BASE_ADDRESS + 0x10)
#define	REG_MAC_RAB_LPI_INTERRUPT                           (MAC_RAB_BASE_ADDRESS + 0x14)
#define	REG_MAC_RAB_LPI_INTERRUPT_SET                       (MAC_RAB_BASE_ADDRESS + 0x18)
#define	REG_MAC_RAB_LPI_INTERRUPT_CLEAR                     (MAC_RAB_BASE_ADDRESS + 0x1C)
#define	REG_MAC_RAB_ENABLE_PHI_INTERRUPT                    (MAC_RAB_BASE_ADDRESS + 0x20)
#define	REG_MAC_RAB_PHI_INTERRUPT                           (MAC_RAB_BASE_ADDRESS + 0x24)
#define	REG_MAC_RAB_PHI_INTERRUPT_SET                       (MAC_RAB_BASE_ADDRESS + 0x28)
#define	REG_MAC_RAB_PHI_INTERRUPT_CLEAR                     (MAC_RAB_BASE_ADDRESS + 0x2C)
#define	REG_MAC_RAB_ENABLE_NPU2UPI_INTERRUPT                (MAC_RAB_BASE_ADDRESS + 0x30)
#define	REG_MAC_RAB_NPU2UPI_INTERRUPT                       (MAC_RAB_BASE_ADDRESS + 0x34)
#define	REG_MAC_RAB_NPU2UPI_INTERRUPT_SET                   (MAC_RAB_BASE_ADDRESS + 0x38)
#define	REG_MAC_RAB_NPU2UPI_INTERRUPT_CLEAR                 (MAC_RAB_BASE_ADDRESS + 0x3C)
#define	REG_MAC_RAB_REVISION                                (MAC_RAB_BASE_ADDRESS + 0x40)
#define	REG_MAC_RAB_PAS_SW_RESET_REG_RELEASE                (MAC_RAB_BASE_ADDRESS + 0x48)
#define	REG_MAC_RAB_PAS_SW_RESET_REG2_RELEASE               (MAC_RAB_BASE_ADDRESS + 0x4C)
#define	REG_MAC_RAB_PAS_SW_RESET_REG_ASSERT                 (MAC_RAB_BASE_ADDRESS + 0x50)
#define	REG_MAC_RAB_PAS_SW_RESET_REG2_ASSERT                (MAC_RAB_BASE_ADDRESS + 0x54)
#define	REG_MAC_RAB_PAS_SW_RESET_REG_RD                     (MAC_RAB_BASE_ADDRESS + 0x58)
#define	REG_MAC_RAB_PAS_SW_RESET_REG2_RD                    (MAC_RAB_BASE_ADDRESS + 0x5C)
#define	REG_MAC_RAB_SHRAM_MON_BASE_ADDR                     (MAC_RAB_BASE_ADDRESS + 0x6C)
#define	REG_MAC_RAB_SHRAM_MON_INT_MASK                      (MAC_RAB_BASE_ADDRESS + 0x70)
#define	REG_MAC_RAB_SHRAM_MON_STATUS_CLEAR                  (MAC_RAB_BASE_ADDRESS + 0x74)
#define	REG_MAC_RAB_SHRAM_MON_STATUS_REG                    (MAC_RAB_BASE_ADDRESS + 0x78)
#define	REG_MAC_RAB_SHRAM_MON_CLIENT_AT_IRQ                 (MAC_RAB_BASE_ADDRESS + 0x7C)
#define	REG_MAC_RAB_USR0_TX_FLOW_CONTROL_EN                 (MAC_RAB_BASE_ADDRESS + 0xB4)
#define	REG_MAC_RAB_USR1_TX_FLOW_CONTROL_EN                 (MAC_RAB_BASE_ADDRESS + 0xB8)
#define	REG_MAC_RAB_USR2_TX_FLOW_CONTROL_EN                 (MAC_RAB_BASE_ADDRESS + 0xBC)
#define	REG_MAC_RAB_USR3_TX_FLOW_CONTROL_EN                 (MAC_RAB_BASE_ADDRESS + 0xC0)
#define	REG_MAC_RAB_TX_FLOW_CONTROL_RD_ROOF_ADDR            (MAC_RAB_BASE_ADDRESS + 0xC8)
#define	REG_MAC_RAB_TX_DMA_MON_ACC_DEC_NUM                  (MAC_RAB_BASE_ADDRESS + 0xCC)
#define	REG_MAC_RAB_TX_DMA_MON_CLEAR                        (MAC_RAB_BASE_ADDRESS + 0xD0)
#define	REG_MAC_RAB_TX_DMA_MON_DEBUG                        (MAC_RAB_BASE_ADDRESS + 0xD4)
#define	REG_MAC_RAB_TX_DMA_ALIGNER_FIFO_CLEAR               (MAC_RAB_BASE_ADDRESS + 0xD8)
#define	REG_MAC_RAB_TX_DMA_ALIGNER_FIFO_DEBUG               (MAC_RAB_BASE_ADDRESS + 0xDC)
#define	REG_MAC_RAB_TX_DMA_ALIGNER_DEBUG1                   (MAC_RAB_BASE_ADDRESS + 0xE0)
#define	REG_MAC_RAB_TX_DMA_ALIGNER_DEBUG2                   (MAC_RAB_BASE_ADDRESS + 0xE4)
#define	REG_MAC_RAB_RX_DMA_FLOW_CONTROL_EN                  (MAC_RAB_BASE_ADDRESS + 0xF0)
#define	REG_MAC_RAB_RX_DMA_FLOW_CONTROL_WR_ADDR_CLR_STRB    (MAC_RAB_BASE_ADDRESS + 0xF4)
#define	REG_MAC_RAB_RX_DMA_FLOW_CONTROL_WR_DBG              (MAC_RAB_BASE_ADDRESS + 0xF8)
#define	REG_MAC_RAB_RX_DMA_FLOW_CONTROL_RD_DBG              (MAC_RAB_BASE_ADDRESS + 0xFC)
#define	REG_MAC_RAB_RX_SECURITY_MON_ADDR                    (MAC_RAB_BASE_ADDRESS + 0x100)
#define	REG_MAC_RAB_RX_SECURITY_MON_ADDR_CLR_STRB           (MAC_RAB_BASE_ADDRESS + 0x104)
#define	REG_MAC_RAB_RX_SECURITY_MON_ADDR_DBG                (MAC_RAB_BASE_ADDRESS + 0x108)
#define	REG_MAC_RAB_USR0_TX_SEC_FLOW_CONTROL_WR_DBG         (MAC_RAB_BASE_ADDRESS + 0x110)
#define	REG_MAC_RAB_USR0_TX_SEC_FLOW_CONTROL_RD_DBG         (MAC_RAB_BASE_ADDRESS + 0x114)
#define	REG_MAC_RAB_USR0_TX_DEL_FLOW_CONTROL_WR_DBG         (MAC_RAB_BASE_ADDRESS + 0x118)
#define	REG_MAC_RAB_USR0_TX_DEL_FLOW_CONTROL_RD_DBG         (MAC_RAB_BASE_ADDRESS + 0x11C)
#define	REG_MAC_RAB_USR1_TX_SEC_FLOW_CONTROL_WR_DBG         (MAC_RAB_BASE_ADDRESS + 0x120)
#define	REG_MAC_RAB_USR1_TX_SEC_FLOW_CONTROL_RD_DBG         (MAC_RAB_BASE_ADDRESS + 0x124)
#define	REG_MAC_RAB_USR1_TX_DEL_FLOW_CONTROL_WR_DBG         (MAC_RAB_BASE_ADDRESS + 0x128)
#define	REG_MAC_RAB_USR1_TX_DEL_FLOW_CONTROL_RD_DBG         (MAC_RAB_BASE_ADDRESS + 0x12C)
#define	REG_MAC_RAB_USR2_TX_SEC_FLOW_CONTROL_WR_DBG         (MAC_RAB_BASE_ADDRESS + 0x130)
#define	REG_MAC_RAB_USR2_TX_SEC_FLOW_CONTROL_RD_DBG         (MAC_RAB_BASE_ADDRESS + 0x134)
#define	REG_MAC_RAB_USR2_TX_DEL_FLOW_CONTROL_WR_DBG         (MAC_RAB_BASE_ADDRESS + 0x138)
#define	REG_MAC_RAB_USR2_TX_DEL_FLOW_CONTROL_RD_DBG         (MAC_RAB_BASE_ADDRESS + 0x13C)
#define	REG_MAC_RAB_USR3_TX_SEC_FLOW_CONTROL_WR_DBG         (MAC_RAB_BASE_ADDRESS + 0x140)
#define	REG_MAC_RAB_USR3_TX_SEC_FLOW_CONTROL_RD_DBG         (MAC_RAB_BASE_ADDRESS + 0x144)
#define	REG_MAC_RAB_USR3_TX_DEL_FLOW_CONTROL_WR_DBG         (MAC_RAB_BASE_ADDRESS + 0x148)
#define	REG_MAC_RAB_USR3_TX_DEL_FLOW_CONTROL_RD_DBG         (MAC_RAB_BASE_ADDRESS + 0x14C)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_MAC_RAB_ENABLE_UPI_INTERRUPT 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 enableUpiInterrupt:24;	// Enable Upper CPU RAB IRQ
		uint32 reserved0:8;
	} bitFields;
} RegMacRabEnableUpiInterrupt_u;

/*REG_MAC_RAB_UPI_INTERRUPT 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 upiInterrupt:24;	// RAB  upi interrupt
		uint32 reserved0:8;
	} bitFields;
} RegMacRabUpiInterrupt_u;

/*REG_MAC_RAB_UPI_INTERRUPT_SET 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 upiInterruptSet:24;	// RAB  upi interrupt set
		uint32 reserved0:8;
	} bitFields;
} RegMacRabUpiInterruptSet_u;

/*REG_MAC_RAB_UPI_INTERRUPT_CLEAR 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 upiInterruptClear:24;	// RAB  upi interrupt clear
		uint32 reserved0:8;
	} bitFields;
} RegMacRabUpiInterruptClear_u;

/*REG_MAC_RAB_ENABLE_LPI_INTERRUPT 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 enableLpiInterrupt:24;	// Enable Lower CPU RAB IRQ
		uint32 reserved0:8;
	} bitFields;
} RegMacRabEnableLpiInterrupt_u;

/*REG_MAC_RAB_LPI_INTERRUPT 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lpiInterrupt:24;	// RAB  lpi interrupt
		uint32 reserved0:8;
	} bitFields;
} RegMacRabLpiInterrupt_u;

/*REG_MAC_RAB_LPI_INTERRUPT_SET 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lpiInterruptSet:24;	// RAB  lpi interrupt set
		uint32 reserved0:8;
	} bitFields;
} RegMacRabLpiInterruptSet_u;

/*REG_MAC_RAB_LPI_INTERRUPT_CLEAR 0x1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lpiInterruptClear:24;	// RAB  lpi interrupt clear
		uint32 reserved0:8;
	} bitFields;
} RegMacRabLpiInterruptClear_u;

/*REG_MAC_RAB_ENABLE_PHI_INTERRUPT 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 enablePhiInterrupt:24;	// Enable PHI host RAB IRQ
		uint32 reserved0:8;
	} bitFields;
} RegMacRabEnablePhiInterrupt_u;

/*REG_MAC_RAB_PHI_INTERRUPT 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phiInterrupt:24;	// RAB  phi interrupt
		uint32 reserved0:8;
	} bitFields;
} RegMacRabPhiInterrupt_u;

/*REG_MAC_RAB_PHI_INTERRUPT_SET 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phiInterruptSet:24;	// RAB  phi interrupt set
		uint32 reserved0:8;
	} bitFields;
} RegMacRabPhiInterruptSet_u;

/*REG_MAC_RAB_PHI_INTERRUPT_CLEAR 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phiInterruptClear:24;	// RAB  phi interrupt clear
		uint32 reserved0:8;
	} bitFields;
} RegMacRabPhiInterruptClear_u;

/*REG_MAC_RAB_ENABLE_NPU2UPI_INTERRUPT 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 enableNpu2UpiInterrupt:24;	// Enable NPU to UPI RAB IRQ
		uint32 reserved0:8;
	} bitFields;
} RegMacRabEnableNpu2UpiInterrupt_u;

/*REG_MAC_RAB_NPU2UPI_INTERRUPT 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 npu2UpiInterrupt:24;	// RAB NPU to UPI interrupt
		uint32 reserved0:8;
	} bitFields;
} RegMacRabNpu2UpiInterrupt_u;

/*REG_MAC_RAB_NPU2UPI_INTERRUPT_SET 0x38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 npu2UpiInterruptSet:24;	// RAB NPU to UPI interrupt set
		uint32 reserved0:8;
	} bitFields;
} RegMacRabNpu2UpiInterruptSet_u;

/*REG_MAC_RAB_NPU2UPI_INTERRUPT_CLEAR 0x3C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 npu2UpiInterruptClear:24;	// RAB NPU to UPI interrupt clear
		uint32 reserved0:8;
	} bitFields;
} RegMacRabNpu2UpiInterruptClear_u;

/*REG_MAC_RAB_REVISION 0x40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 revision:12;	// RAB  revision
		uint32 reserved0:20;
	} bitFields;
} RegMacRabRevision_u;

/*REG_MAC_RAB_PAS_SW_RESET_REG_RELEASE 0x48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swResetReleaseReserved0:1;	// SW enable reserved 0 Release, by write '1'
		uint32 swResetReleaseReserved1:1;	// SW enable reserved 1 Release, by write '1'
		uint32 swResetReleaseReserved2:1;	// SW enable reserved 2 Release, by write '1'
		uint32 swResetReleaseReserved3:1;	// SW enable reserved 3 Release, by write '1'
		uint32 swResetReleaseTim:1;	// TIM SW enable Release, by write '1'
		uint32 swResetReleaseTimReg:1;	// TIM Reg block SW enable Release, by write '1'
		uint32 swResetReleaseDel:1;	// Delia SW enable Release, by write '1'
		uint32 swResetReleaseDelReg:1;	// Deliea Reg block SW enable Release, by write '1'
		uint32 swResetReleaseTcc:1;	// TCC SW enable Release, by write '1'
		uint32 swResetReleaseTxDmaMon:1;	// Tx DMA monitor SW enable  Release, by write '1'
		uint32 swResetReleaseRxc:1;	// RxC SW enable Release, by write '1'
		uint32 swResetReleaseRxcReg:1;	// RxC Reg block SW enable Release, by write '1'
		uint32 swResetReleaseRxf:1;	// RxFC SW enable Release, by write '1'
		uint32 swResetReleaseRxfReg:1;	// RxF Reg block SW enable Release, by write '1'
		uint32 swResetReleaseRcc:1;	// RCC SW enable Release, by write '1'
		uint32 swResetReleaseBeaconTimers:1;	// Beacon Timers SW registers enable Release, by write '1'
		uint32 swResetReleaseRxd:1;	// RxC SW enable Release, by write '1'
		uint32 swResetReleaseRxdReg:1;	// RxC Reg block SW enable Release, by write '1'
		uint32 swResetReleaseDur:1;	// DUR SW enable Release, by write '1'
		uint32 swResetReleaseDurReg:1;	// DUR Reg block SW enable Release, by write '1'
		uint32 swResetReleaseRta:1;	// RTA SW enable Release, by write '1'
		uint32 swResetReleaseBf:1;	// BF SW enable Release, by write '1'
		uint32 swResetReleaseWep:1;	// WEP SW enable Release, by write '1'
		uint32 swResetReleaseAggBuilder:1;	// AGG builder SW enable Release, by write '1'
		uint32 swResetReleaseAggBuilderReg:1;	// AGG builder registers SW enable Release, by write '1'
		uint32 swResetReleaseAddr2Index:1;	// Addr2index SW enable Release, by write '1'
		uint32 swResetReleaseAddr2IndexReg:1;	// Addr2index registers SW enable Release, by write '1'
		uint32 swResetReleaseTxHandler:1;	// Tx handler SW enable Release, by write '1'
		uint32 swResetReleaseTxHandlerReg:1;	// Tx handler registers SW enable Release, by write '1'
		uint32 swResetReleaseTxSelector:1;	// Tx Selector SW enable Release, by write '1'
		uint32 swResetReleaseTxSelectorReg:1;	// Tx Selector registers SW enable Release, by write '1'
		uint32 swResetReleaseBeaconTimersReg:1;	// Beacon Timers SW enable Release, by write '1'
	} bitFields;
} RegMacRabPasSwResetRegRelease_u;

/*REG_MAC_RAB_PAS_SW_RESET_REG2_RELEASE 0x4C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swResetReleaseTxhTxc:1;	// TXH TXC SW enable Release, by write '1'
		uint32 swResetReleaseTxhTxcReg:1;	// TXH TXC registers SW enable Release, by write '1'
		uint32 swResetReleaseTxhNtd:1;	// TXH NTD SW enable Release, by write '1'
		uint32 swResetReleaseTxhNtdReg:1;	// TXH NTD registers SW enable Release, by write '1'
		uint32 swResetReleaseTxDmaAligner:1;	// Tx DMA aligner SW enable  Release, by write '1'
		uint32 swResetReleaseBaAnalizer:1;	// ba_analizer  SW enable  Release, by write '1'
		uint32 swResetReleaseProtDb:1;	// Protected DB SMC lock SW enable  Release, by write '1'
		uint32 swResetReleaseProtDbReg:1;	// Protected DB SMC lock registers SW enable  Release, by write '1'
		uint32 swResetReleaseLiberatorReg:1;	// sw_reRelease_liberator_reg Release, by write '1'
		uint32 swResetReleaseRxClas:1;	// Rx Classifier SW enable  Release, by write '1'
		uint32 swResetReleaseRxClasReg:1;	// Rx Classifier registers SW enable  Release, by write '1'
		uint32 swResetReleasePsSetting:1;	// PS Setting SW enable  Release, by write '1'
		uint32 swResetReleasePsSettingReg:1;	// PS Setting registers SW enable  Release, by write '1'
		uint32 swResetReleaseRxPp:1;	// Rx post-processing SW enable  Release, by write '1'
		uint32 swResetReleaseTxSequencer:1;	// Tx Sequencer SW enable Release, by write '1'
		uint32 swResetReleasePacExtrap:1;	// PAC extrapolator SW enable  Release, by write '1'
		uint32 swResetReleasePacExtrapReg:1;	// PAC extrapolator registers SW enable  Release, by write '1'
		uint32 swResetReleaseTxPdAcc:1;	// Tx PD accelrator registers SW enable  Release, by write '1'
		uint32 swResetReleaseReserved:14;	// Reserved SW enable Release, by write '1'
	} bitFields;
} RegMacRabPasSwResetReg2Release_u;

/*REG_MAC_RAB_PAS_SW_RESET_REG_ASSERT 0x50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swResetAssertReserved0:1;	// SW enable reserved 0 Assert, by write '1'
		uint32 swResetAssertReserved1:1;	// SW enable reserved 1 Assert, by write '1'
		uint32 swResetAssertReserved2:1;	// SW enable reserved 2 Assert, by write '1'
		uint32 swResetAssertReserved3:1;	// SW enable reserved 3 Assert, by write '1'
		uint32 swResetAssertTim:1;	// TIM SW enable Assert, by write '1'
		uint32 swResetAssertTimReg:1;	// TIM Reg block SW enable Assert, by write '1'
		uint32 swResetAssertDel:1;	// Delia SW enable Assert, by write '1'
		uint32 swResetAssertDelReg:1;	// Deliea Reg block SW enable Assert, by write '1'
		uint32 swResetAssertTcc:1;	// TCC SW enable Assert, by write '1'
		uint32 swResetAssertTxDmaMon:1;	// Tx DMA monitor SW enable  Assert, by write '1'
		uint32 swResetAssertRxc:1;	// RxC SW enable Assert, by write '1'
		uint32 swResetAssertRxcReg:1;	// RxC Reg block SW enable Assert, by write '1'
		uint32 swResetAssertRxf:1;	// RxFC SW enable Assert, by write '1'
		uint32 swResetAssertRxfReg:1;	// RxF Reg block SW enable Assert, by write '1'
		uint32 swResetAssertRcc:1;	// RCC SW enable Assert, by write '1'
		uint32 swResetAssertBeaconTimers:1;	// Beacon Timers SW registers enable Assert, by write '1'
		uint32 swResetAssertRxd:1;	// RxC SW enable Assert, by write '1'
		uint32 swResetAssertRxdReg:1;	// RxC Reg block SW enable Assert, by write '1'
		uint32 swResetAssertDur:1;	// DUR SW enable Assert, by write '1'
		uint32 swResetAssertDurReg:1;	// DUR Reg block SW enable Assert, by write '1'
		uint32 swResetAssertRta:1;	// RTA SW enable Assert, by write '1'
		uint32 swResetAssertBf:1;	// BF SW enable Assert, by write '1'
		uint32 swResetAssertWep:1;	// WEP SW enable Assert, by write '1'
		uint32 swResetAssertAggBuilder:1;	// AGG builder SW enable Assert, by write '1'
		uint32 swResetAssertAggBuilderReg:1;	// AGG builder registers SW enable Assert, by write '1'
		uint32 swResetAssertAddr2Index:1;	// Addr2index SW enable Assert, by write '1'
		uint32 swResetAssertAddr2IndexReg:1;	// Addr2index registers SW enable Assert, by write '1'
		uint32 swResetAssertTxHandler:1;	// Tx handler SW enable Assert, by write '1'
		uint32 swResetAssertTxHandlerReg:1;	// Tx handler registers SW enable Assert, by write '1'
		uint32 swResetAssertTxSelector:1;	// Tx Selector SW enable Assert, by write '1'
		uint32 swResetAssertTxSelectorReg:1;	// Tx Selector registers SW enable Assert, by write '1'
		uint32 swResetAssertBeaconTimersReg:1;	// Beacon Timers SW enable Assert, by write '1'
	} bitFields;
} RegMacRabPasSwResetRegAssert_u;

/*REG_MAC_RAB_PAS_SW_RESET_REG2_ASSERT 0x54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swResetAssertTxhTxc:1;	// TXH TXC SW enable Assert, by write '1'
		uint32 swResetAssertTxhTxcReg:1;	// TXH TXC registers SW enable Assert, by write '1'
		uint32 swResetAssertTxhNtd:1;	// TXH NTD SW enable Assert, by write '1'
		uint32 swResetAssertTxhNtdReg:1;	// TXH NTD registers SW enable Assert, by write '1'
		uint32 swResetAssertTxDmaAligner:1;	// Tx DMA aligner SW enable  Assert, by write '1'
		uint32 swResetAssertBaAnalizer:1;	// ba_analizer  SW enable  Assert, by write '1'
		uint32 swResetAssertProtDb:1;	// Protected DB SMC lock SW enable  Assert, by write '1'
		uint32 swResetAssertProtDbReg:1;	// Protected DB SMC lock registers SW enable  Assert, by write '1'
		uint32 swResetAssertLiberatorReg:1;	// sw_reAssert_liberator_reg Assert, by write '1'
		uint32 swResetAssertRxClas:1;	// Rx Classifier SW enable  Assert, by write '1'
		uint32 swResetAssertRxClasReg:1;	// Rx Classifier registers SW enable  Assert, by write '1'
		uint32 swResetAssertPsSetting:1;	// PS Setting SW enable  Assert, by write '1'
		uint32 swResetAssertPsSettingReg:1;	// PS Setting registers SW enable  Assert, by write '1'
		uint32 swResetAssertRxPp:1;	// Rx post-processing SW enable  Assert, by write '1'
		uint32 swResetAssertTxSequencer:1;	// Tx Sequencer SW enable Assert, by write '1'
		uint32 swResetAssertPacExtrap:1;	// PAC extrapolator SW enable  Assert, by write '1'
		uint32 swResetAssertPacExtrapReg:1;	// PAC extrapolator registers SW enable  Assert, by write '1'
		uint32 swResetAssertTxPdAcc:1;	// Tx PD accelerator SW enable  Assert, by write '1'
		uint32 swResetAssertReserved:14;	// Reserved SW enable Assert, by write '1'
	} bitFields;
} RegMacRabPasSwResetReg2Assert_u;

/*REG_MAC_RAB_PAS_SW_RESET_REG_RD 0x58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swResetRdReserved0:1;	// SW enable reserved 0 
		uint32 swResetRdReserved1:1;	// SW enable reserved 1 
		uint32 swResetRdReserved2:1;	// SW enable reserved 2 
		uint32 swResetRdReserved3:1;	// SW enable reserved 3 
		uint32 swResetRdTim:1;	// TIM SW enable 
		uint32 swResetRdTimReg:1;	// TIM Reg block SW enable 
		uint32 swResetRdDel:1;	// Delia SW enable 
		uint32 swResetRdDelReg:1;	// Deliea Reg block SW enable 
		uint32 swResetRdTcc:1;	// TCC SW enable 
		uint32 swResetRdTxDmaMon:1;	// Tx DMA monitor SW enable  
		uint32 swResetRdRxc:1;	// RxC SW enable 
		uint32 swResetRdRxcReg:1;	// RxC Reg block SW enable 
		uint32 swResetRdRxf:1;	// RxFC SW enable 
		uint32 swResetRdRxfReg:1;	// RxF Reg block SW enable 
		uint32 swResetRdRcc:1;	// RCC SW enable 
		uint32 swResetRdBeaconTimers:1;	// Beacon Timers SW registers enable 
		uint32 swResetRdRxd:1;	// RxC SW enable 
		uint32 swResetRdRxdReg:1;	// RxC Reg block SW enable 
		uint32 swResetRdDur:1;	// DUR SW enable 
		uint32 swResetRdDurReg:1;	// DUR Reg block SW enable 
		uint32 swResetRdRta:1;	// RTA SW enable 
		uint32 swResetRdBf:1;	// BF SW enable 
		uint32 swResetRdWep:1;	// WEP SW enable 
		uint32 swResetRdAggBuilder:1;	// AGG builder SW enable 
		uint32 swResetRdAggBuilderReg:1;	// AGG builder registers SW enable 
		uint32 swResetRdAddr2Index:1;	// Addr2index SW enable 
		uint32 swResetRdAddr2IndexReg:1;	// Addr2index registers SW enable 
		uint32 swResetRdTxHandler:1;	// Tx handler SW enable 
		uint32 swResetRdTxHandlerReg:1;	// Tx handler registers SW enable 
		uint32 swResetRdTxSelector:1;	// Tx Selector SW enable 
		uint32 swResetRdTxSelectorReg:1;	// Tx Selector registers SW enable 
		uint32 swResetRdBeaconTimersReg:1;	// Beacon Timers SW enable 
	} bitFields;
} RegMacRabPasSwResetRegRd_u;

/*REG_MAC_RAB_PAS_SW_RESET_REG2_RD 0x5C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swResetRdTxhTxc:1;	// TXH TXC SW enable 
		uint32 swResetRdTxhTxcReg:1;	// TXH TXC registers SW enable 
		uint32 swResetRdTxhNtd:1;	// TXH NTD SW enable 
		uint32 swResetRdTxhNtdReg:1;	// TXH NTD registers SW enable 
		uint32 swResetRdTxDmaAligner:1;	// Tx DMA aligner SW enable  
		uint32 swResetRdBaAnalizer:1;	// ba_analizer  SW enable  
		uint32 swResetRdProtDb:1;	// Protected DB SMC lock SW enable  
		uint32 swResetRdProtDbReg:1;	// Protected DB SMC lock registers SW enable  
		uint32 swResetRdLiberatorReg:1;	// sw_reset_liberator_reg 
		uint32 swResetRdRxClas:1;	// Rx Classifier SW enable  
		uint32 swResetRdRxClasReg:1;	// Rx Classifier registers SW enable  
		uint32 swResetRdPsSetting:1;	// PS Setting SW enable  
		uint32 swResetRdPsSettingReg:1;	// PS Setting registers SW enable  
		uint32 swResetRdRxPp:1;	// Rx post-processing SW enable  
		uint32 swResetRdTxSequencer:1;	// Tx Sequencer SW enable 
		uint32 swResetRdPacExtrap:1;	// PAC extrapolator SW enable  
		uint32 swResetRdPacExtrapReg:1;	// PAC extrapolator registers SW enable  
		uint32 swResetRdTxPdAcc:1;	// Tx PD accelerator SW enable  
		uint32 swResetRdReserved:14;	// Reserved SW enable 
	} bitFields;
} RegMacRabPasSwResetReg2Rd_u;

/*REG_MAC_RAB_SHRAM_MON_BASE_ADDR 0x6C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 shramMonBaseAddr:24;	// Shared RAM monitor base address
		uint32 reserved0:8;
	} bitFields;
} RegMacRabShramMonBaseAddr_u;

/*REG_MAC_RAB_SHRAM_MON_INT_MASK 0x70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 shramMonIntMask:16;	// Shared RAM monitor interrupt mask
		uint32 reserved0:16;
	} bitFields;
} RegMacRabShramMonIntMask_u;

/*REG_MAC_RAB_SHRAM_MON_STATUS_CLEAR 0x74 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 shramMonStatusClear:16;	// Shared RAM monitor status clear
		uint32 reserved0:16;
	} bitFields;
} RegMacRabShramMonStatusClear_u;

/*REG_MAC_RAB_SHRAM_MON_STATUS_REG 0x78 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 shramMonStatusReg:16;	// Shared RAM monitor status register
		uint32 reserved0:16;
	} bitFields;
} RegMacRabShramMonStatusReg_u;

/*REG_MAC_RAB_SHRAM_MON_CLIENT_AT_IRQ 0x7C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 shramMonClientAtIrq:4;	// Shared RAM client at IRQ
		uint32 reserved0:28;
	} bitFields;
} RegMacRabShramMonClientAtIrq_u;

/*REG_MAC_RAB_USR0_TX_FLOW_CONTROL_EN 0xB4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 usr0TxDelFlowControlEn:2;	// Delia flow control enable mode: , 00: Disable/bypass , 01: WR client – WLAN DMA. , 11: WR client – Security engine.
		uint32 usr0TxSecFlowControlEn:1;	// Security engine flow control enable mode: , 0: Disable/bypass , 1: WR client – WLAN DMA
		uint32 reserved0:1;
		uint32 usr0TxFlowControlRdRoofAddrEn:1;	// flow control read roof addr en
		uint32 usr0SecDoneClrDelFlowCntrlEn:1;	// Enable release flow control between Security to Delia on Security done event
		uint32 usr0DmaDoneClrSecDelFlowCntrlEn:1;	// Enable release flow control between DMA to Security/Delia on DMA done event
		uint32 reserved1:1;
		uint32 usr0TxDelFlowControlEnFinal:2;	// Final Delia flow control enable mode: , 00: Disable/bypass , 01: WR client – WLAN DMA. , 11: WR client – Security engine.
		uint32 usr0TxSecFlowControlEnFinal:1;	// Final Security engine flow control enable mode: , 0: Disable/bypass , 1: WR client – WLAN DMA
		uint32 reserved2:21;
	} bitFields;
} RegMacRabUsr0TxFlowControlEn_u;

/*REG_MAC_RAB_USR1_TX_FLOW_CONTROL_EN 0xB8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 usr1TxDelFlowControlEn:2;	// Delia flow control enable mode: , 00: Disable/bypass , 01: WR client – WLAN DMA. , 11: WR client – Security engine.
		uint32 usr1TxSecFlowControlEn:1;	// Security engine flow control enable mode: , 0: Disable/bypass , 1: WR client – WLAN DMA
		uint32 reserved0:1;
		uint32 usr1TxFlowControlRdRoofAddrEn:1;	// flow control read roof addr en
		uint32 usr1SecDoneClrDelFlowCntrlEn:1;	// Enable release flow control between Security to Delia on Security done event
		uint32 usr1DmaDoneClrSecDelFlowCntrlEn:1;	// Enable release flow control between DMA to Security/Delia on DMA done event
		uint32 reserved1:1;
		uint32 usr1TxDelFlowControlEnFinal:2;	// Final Delia flow control enable mode: , 00: Disable/bypass , 01: WR client – WLAN DMA. , 11: WR client – Security engine.
		uint32 usr1TxSecFlowControlEnFinal:1;	// Final Security engine flow control enable mode: , 0: Disable/bypass , 1: WR client – WLAN DMA
		uint32 reserved2:21;
	} bitFields;
} RegMacRabUsr1TxFlowControlEn_u;

/*REG_MAC_RAB_USR2_TX_FLOW_CONTROL_EN 0xBC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 usr2TxDelFlowControlEn:2;	// Delia flow control enable mode: , 00: Disable/bypass , 01: WR client – WLAN DMA. , 11: WR client – Security engine.
		uint32 usr2TxSecFlowControlEn:1;	// Security engine flow control enable mode: , 0: Disable/bypass , 1: WR client – WLAN DMA
		uint32 reserved0:1;
		uint32 usr2TxFlowControlRdRoofAddrEn:1;	// flow control read roof addr en
		uint32 usr2SecDoneClrDelFlowCntrlEn:1;	// Enable release flow control between Security to Delia on Security done event
		uint32 usr2DmaDoneClrSecDelFlowCntrlEn:1;	// Enable release flow control between DMA to Security/Delia on DMA done event
		uint32 reserved1:1;
		uint32 usr2TxDelFlowControlEnFinal:2;	// Final Delia flow control enable mode: , 00: Disable/bypass , 01: WR client – WLAN DMA. , 11: WR client – Security engine.
		uint32 usr2TxSecFlowControlEnFinal:1;	// Final Security engine flow control enable mode: , 0: Disable/bypass , 1: WR client – WLAN DMA
		uint32 reserved2:21;
	} bitFields;
} RegMacRabUsr2TxFlowControlEn_u;

/*REG_MAC_RAB_USR3_TX_FLOW_CONTROL_EN 0xC0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 usr3TxDelFlowControlEn:2;	// Delia flow control enable mode: , 00: Disable/bypass , 01: WR client – WLAN DMA. , 11: WR client – Security engine.
		uint32 usr3TxSecFlowControlEn:1;	// Security engine flow control enable mode: , 0: Disable/bypass , 1: WR client – WLAN DMA
		uint32 reserved0:1;
		uint32 usr3TxFlowControlRdRoofAddrEn:1;	// flow control read roof addr en
		uint32 usr3SecDoneClrDelFlowCntrlEn:1;	// Enable release flow control between Security to Delia on Security done event
		uint32 usr3DmaDoneClrSecDelFlowCntrlEn:1;	// Enable release flow control between DMA to Security/Delia on DMA done event
		uint32 reserved1:1;
		uint32 usr3TxDelFlowControlEnFinal:2;	// Final Delia flow control enable mode: , 00: Disable/bypass , 01: WR client – WLAN DMA. , 11: WR client – Security engine.
		uint32 usr3TxSecFlowControlEnFinal:1;	// Final Security engine flow control enable mode: , 0: Disable/bypass , 1: WR client – WLAN DMA
		uint32 reserved2:21;
	} bitFields;
} RegMacRabUsr3TxFlowControlEn_u;

/*REG_MAC_RAB_TX_FLOW_CONTROL_RD_ROOF_ADDR 0xC8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txFlowControlRdRoofAddr:19;	// flow contro read roof addr
		uint32 reserved0:12;
		uint32 txFlowCntrlRdRoofLastClrFc:1;	// Last DMA job to clear the flow control based on Read roof address
	} bitFields;
} RegMacRabTxFlowControlRdRoofAddr_u;

/*REG_MAC_RAB_TX_DMA_MON_ACC_DEC_NUM 0xCC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txDmaMonAccDecNum:8;	// Tx DMA monitor accumulator decrement number
		uint32 reserved0:24;
	} bitFields;
} RegMacRabTxDmaMonAccDecNum_u;

/*REG_MAC_RAB_TX_DMA_MON_CLEAR 0xD0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txDmaMonFifoClear:1;	// Tx DMA monitor FIFO clear
		uint32 txDmaMonAccClear:1;	// Tx DMA monitor accumulator clear
		uint32 reserved0:30;
	} bitFields;
} RegMacRabTxDmaMonClear_u;

/*REG_MAC_RAB_TX_DMA_MON_DEBUG 0xD4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txDmaMonFifoFirstAddr:14;	// Tx DMA monitor FIFO first address
		uint32 txDmaMonFifoEmpty:1;	// Tx DMA monitor FIFO empty
		uint32 txDmaMonFifoFull:1;	// Tx DMA monitor FIFO full
		uint32 txDmaMonAccCount:8;	// Tx DMA monitor accumulator count
		uint32 txDmaMonFifoNumWords:5;	// Tx DMA monitor FIFO number words
		uint32 txDmaMonFifoPushWhileFull:1;	// Tx DMA monitor FIFO push while full
		uint32 txDmaMonAccExceedMaxWidth:1;	// Tx DMA monitor accumulator exceed max width
		uint32 reserved0:1;
	} bitFields;
} RegMacRabTxDmaMonDebug_u;

/*REG_MAC_RAB_TX_DMA_ALIGNER_FIFO_CLEAR 0xD8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txDmaAlignerFifoClear:1;	// Tx DMA aligner FIFO clear
		uint32 reserved0:31;
	} bitFields;
} RegMacRabTxDmaAlignerFifoClear_u;

/*REG_MAC_RAB_TX_DMA_ALIGNER_FIFO_DEBUG 0xDC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txDmaAlignerFifoNumWords:5;	// Tx DMA aligner FIFO number words
		uint32 txDmaAlignerFifoEmpty:1;	// Tx DMA aligner FIFO empty
		uint32 txDmaAlignerFifoFull:1;	// Tx DMA aligner FIFO full
		uint32 txDmaAlignerFifoPushWhileFull:1;	// Tx DMA aligner FIFO push while full
		uint32 reserved0:24;
	} bitFields;
} RegMacRabTxDmaAlignerFifoDebug_u;

/*REG_MAC_RAB_TX_DMA_ALIGNER_DEBUG1 0xE0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 xbarToAlignerAccessCounter:13;	// xbar_to_aligner access counter
		uint32 reserved0:3;
		uint32 alignerToSmcAccessCounter:13;	// aligner_to_smc access counter
		uint32 reserved1:3;
	} bitFields;
} RegMacRabTxDmaAlignerDebug1_u;

/*REG_MAC_RAB_TX_DMA_ALIGNER_DEBUG2 0xE4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 xbarToAlignerTotalTransactions:13;	// xbar_to_aligner total transactions
		uint32 reserved0:3;
		uint32 alignerToSmcTotalTransactions:13;	// aligner_to_smc total transactions
		uint32 reserved1:3;
	} bitFields;
} RegMacRabTxDmaAlignerDebug2_u;

/*REG_MAC_RAB_RX_DMA_FLOW_CONTROL_EN 0xF0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxDmaFlowControlEn:1;	// Rx DMA flow control enable
		uint32 reserved0:31;
	} bitFields;
} RegMacRabRxDmaFlowControlEn_u;

/*REG_MAC_RAB_RX_DMA_FLOW_CONTROL_WR_ADDR_CLR_STRB 0xF4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxDmaFlowControlWrAddrClrStrb:1;	// Rx DMA flow control clear wr address
		uint32 reserved0:31;
	} bitFields;
} RegMacRabRxDmaFlowControlWrAddrClrStrb_u;

/*REG_MAC_RAB_RX_DMA_FLOW_CONTROL_WR_DBG 0xF8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxDmaFlowControlWrAddr:19;	// Rx DMA flow control last wr addr
		uint32 reserved0:13;
	} bitFields;
} RegMacRabRxDmaFlowControlWrDbg_u;

/*REG_MAC_RAB_RX_DMA_FLOW_CONTROL_RD_DBG 0xFC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxDmaFlowControlRdAddr:24;	// Rx DMA flow control last rd addr
		uint32 reserved0:7;
		uint32 rxDmaFlowControlReqPending:1;	// Rx DMA flow control rd request pending
	} bitFields;
} RegMacRabRxDmaFlowControlRdDbg_u;

/*REG_MAC_RAB_RX_SECURITY_MON_ADDR 0x100 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxSecurityMonAddr:19;	// Rx Security monitor address
		uint32 reserved0:13;
	} bitFields;
} RegMacRabRxSecurityMonAddr_u;

/*REG_MAC_RAB_RX_SECURITY_MON_ADDR_CLR_STRB 0x104 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxSecurityMonAddrClrStrb:1;	// Rx Security monitor address clear
		uint32 reserved0:31;
	} bitFields;
} RegMacRabRxSecurityMonAddrClrStrb_u;

/*REG_MAC_RAB_RX_SECURITY_MON_ADDR_DBG 0x108 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxLastWrSecurityMonAddr:19;	// Rx last write Security monitor address
		uint32 reserved0:11;
		uint32 rxSecurityMonAddrEn:1;	// Rx Security monitor address enable
		uint32 rxSecurityMonAddrCmpResult:1;	// Rx Security monitor address compare result
	} bitFields;
} RegMacRabRxSecurityMonAddrDbg_u;

/*REG_MAC_RAB_USR0_TX_SEC_FLOW_CONTROL_WR_DBG 0x110 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 usr0TxSecFlowControlWrAddr:19;	// SEC flow control last wr addr
		uint32 reserved0:13;
	} bitFields;
} RegMacRabUsr0TxSecFlowControlWrDbg_u;

/*REG_MAC_RAB_USR0_TX_SEC_FLOW_CONTROL_RD_DBG 0x114 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 usr0TxSecFlowControlRdAddr:24;	// SEC flow control last rd addr
		uint32 reserved0:7;
		uint32 usr0TxSecFlowControlReqPending:1;	// SEC flow control rd request pending
	} bitFields;
} RegMacRabUsr0TxSecFlowControlRdDbg_u;

/*REG_MAC_RAB_USR0_TX_DEL_FLOW_CONTROL_WR_DBG 0x118 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 usr0TxDelFlowControlWrAddr:19;	// Delia flow control last wr addr
		uint32 reserved0:13;
	} bitFields;
} RegMacRabUsr0TxDelFlowControlWrDbg_u;

/*REG_MAC_RAB_USR0_TX_DEL_FLOW_CONTROL_RD_DBG 0x11C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 usr0TxDelFlowControlRdAddr:24;	// Delia flow control last rd addr 
		uint32 reserved0:7;
		uint32 usr0TxDelFlowControlReqPending:1;	// Delia flow control rd request pending
	} bitFields;
} RegMacRabUsr0TxDelFlowControlRdDbg_u;

/*REG_MAC_RAB_USR1_TX_SEC_FLOW_CONTROL_WR_DBG 0x120 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 usr1TxSecFlowControlWrAddr:19;	// SEC flow control last wr addr
		uint32 reserved0:13;
	} bitFields;
} RegMacRabUsr1TxSecFlowControlWrDbg_u;

/*REG_MAC_RAB_USR1_TX_SEC_FLOW_CONTROL_RD_DBG 0x124 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 usr1TxSecFlowControlRdAddr:24;	// SEC flow control last rd addr
		uint32 reserved0:7;
		uint32 usr1TxSecFlowControlReqPending:1;	// SEC flow control rd request pending
	} bitFields;
} RegMacRabUsr1TxSecFlowControlRdDbg_u;

/*REG_MAC_RAB_USR1_TX_DEL_FLOW_CONTROL_WR_DBG 0x128 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 usr1TxDelFlowControlWrAddr:19;	// Delia flow control last wr addr
		uint32 reserved0:13;
	} bitFields;
} RegMacRabUsr1TxDelFlowControlWrDbg_u;

/*REG_MAC_RAB_USR1_TX_DEL_FLOW_CONTROL_RD_DBG 0x12C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 usr1TxDelFlowControlRdAddr:24;	// Delia flow control last rd addr 
		uint32 reserved0:7;
		uint32 usr1TxDelFlowControlReqPending:1;	// Delia flow control rd request pending
	} bitFields;
} RegMacRabUsr1TxDelFlowControlRdDbg_u;

/*REG_MAC_RAB_USR2_TX_SEC_FLOW_CONTROL_WR_DBG 0x130 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 usr2TxSecFlowControlWrAddr:19;	// SEC flow control last wr addr
		uint32 reserved0:13;
	} bitFields;
} RegMacRabUsr2TxSecFlowControlWrDbg_u;

/*REG_MAC_RAB_USR2_TX_SEC_FLOW_CONTROL_RD_DBG 0x134 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 usr2TxSecFlowControlRdAddr:24;	// SEC flow control last rd addr
		uint32 reserved0:7;
		uint32 usr2TxSecFlowControlReqPending:1;	// SEC flow control rd request pending
	} bitFields;
} RegMacRabUsr2TxSecFlowControlRdDbg_u;

/*REG_MAC_RAB_USR2_TX_DEL_FLOW_CONTROL_WR_DBG 0x138 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 usr2TxDelFlowControlWrAddr:19;	// Delia flow control last wr addr
		uint32 reserved0:13;
	} bitFields;
} RegMacRabUsr2TxDelFlowControlWrDbg_u;

/*REG_MAC_RAB_USR2_TX_DEL_FLOW_CONTROL_RD_DBG 0x13C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 usr2TxDelFlowControlRdAddr:24;	// Delia flow control last rd addr 
		uint32 reserved0:7;
		uint32 usr2TxDelFlowControlReqPending:1;	// Delia flow control rd request pending
	} bitFields;
} RegMacRabUsr2TxDelFlowControlRdDbg_u;

/*REG_MAC_RAB_USR3_TX_SEC_FLOW_CONTROL_WR_DBG 0x140 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 usr3TxSecFlowControlWrAddr:19;	// SEC flow control last wr addr
		uint32 reserved0:13;
	} bitFields;
} RegMacRabUsr3TxSecFlowControlWrDbg_u;

/*REG_MAC_RAB_USR3_TX_SEC_FLOW_CONTROL_RD_DBG 0x144 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 usr3TxSecFlowControlRdAddr:24;	// SEC flow control last rd addr
		uint32 reserved0:7;
		uint32 usr3TxSecFlowControlReqPending:1;	// SEC flow control rd request pending
	} bitFields;
} RegMacRabUsr3TxSecFlowControlRdDbg_u;

/*REG_MAC_RAB_USR3_TX_DEL_FLOW_CONTROL_WR_DBG 0x148 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 usr3TxDelFlowControlWrAddr:19;	// Delia flow control last wr addr
		uint32 reserved0:13;
	} bitFields;
} RegMacRabUsr3TxDelFlowControlWrDbg_u;

/*REG_MAC_RAB_USR3_TX_DEL_FLOW_CONTROL_RD_DBG 0x14C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 usr3TxDelFlowControlRdAddr:24;	// Delia flow control last rd addr 
		uint32 reserved0:7;
		uint32 usr3TxDelFlowControlReqPending:1;	// Delia flow control rd request pending
	} bitFields;
} RegMacRabUsr3TxDelFlowControlRdDbg_u;



#endif // _MAC_RAB_REGS_H_
