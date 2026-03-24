
/***********************************************************************************
File:				MacGeneralRegs.h
Module:				macGeneral
SOC Revision:		
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _MAC_GENERAL_REGS_H_
#define _MAC_GENERAL_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define MAC_GENERAL_BASE_ADDRESS                             MEMORY_MAP_UNIT_41_BASE_ADDRESS
#define	REG_MAC_GENERAL_CONFIG_0                             (MAC_GENERAL_BASE_ADDRESS + 0x0)
#define	REG_MAC_GENERAL_SHRAM_MON_BASE_ADDR                  (MAC_GENERAL_BASE_ADDRESS + 0x4)
#define	REG_MAC_GENERAL_SHRAM_MON_INT_MASK                   (MAC_GENERAL_BASE_ADDRESS + 0x8)
#define	REG_MAC_GENERAL_SHRAM_MON_STATUS_CLEAR               (MAC_GENERAL_BASE_ADDRESS + 0xC)
#define	REG_MAC_GENERAL_SHRAM_MON_STATUS_REG                 (MAC_GENERAL_BASE_ADDRESS + 0x10)
#define	REG_MAC_GENERAL_SHRAM_MON_CLIENT_AT_IRQ              (MAC_GENERAL_BASE_ADDRESS + 0x14)
#define	REG_MAC_GENERAL_SHRAM_RM                             (MAC_GENERAL_BASE_ADDRESS + 0x18)
#define	REG_MAC_GENERAL_RETRY_DB_BAND1_SINGLE_MODE           (MAC_GENERAL_BASE_ADDRESS + 0x20)
#define	REG_MAC_GENERAL_PHY_EXT_RESET                        (MAC_GENERAL_BASE_ADDRESS + 0x24)
#define	REG_MAC_GENERAL_LOGGER_FIFOS_SRC_SEL                 (MAC_GENERAL_BASE_ADDRESS + 0x28)
#define	REG_MAC_GENERAL_ENDIAN_SWAP_CTL                      (MAC_GENERAL_BASE_ADDRESS + 0x2C)
#define	REG_MAC_GENERAL_LINK_ADAPTATION_SPARE_REG            (MAC_GENERAL_BASE_ADDRESS + 0x30)
#define	REG_MAC_GENERAL_AHB_ARB_BBCPU_PAGE_REG               (MAC_GENERAL_BASE_ADDRESS + 0x34)
#define	REG_MAC_GENERAL_AHB_ARB_GENRISC_HOST_PAGE_REG        (MAC_GENERAL_BASE_ADDRESS + 0x38)
#define	REG_MAC_GENERAL_RAM_CTL                              (MAC_GENERAL_BASE_ADDRESS + 0x3C)
#define	REG_MAC_GENERAL_HOST_IRQ_STATUS                      (MAC_GENERAL_BASE_ADDRESS + 0x40)
#define	REG_MAC_GENERAL_HOST_IRQ_MASK                        (MAC_GENERAL_BASE_ADDRESS + 0x44)
#define	REG_MAC_GENERAL_HYP_MAC_TOP_GPO_PRE                  (MAC_GENERAL_BASE_ADDRESS + 0x48)
#define	REG_MAC_GENERAL_ACTIVITY_LOGGER_CTL                  (MAC_GENERAL_BASE_ADDRESS + 0x4C)
#define	REG_MAC_GENERAL_TIMEOUT_PARAMS_0                     (MAC_GENERAL_BASE_ADDRESS + 0x50)
#define	REG_MAC_GENERAL_TIMEOUT_PARAMS_1                     (MAC_GENERAL_BASE_ADDRESS + 0x54)
#define	REG_MAC_GENERAL_TIMEOUT_PARAMS_2                     (MAC_GENERAL_BASE_ADDRESS + 0x58)
#define	REG_MAC_GENERAL_RAMS1_RM                             (MAC_GENERAL_BASE_ADDRESS + 0x60)
#define	REG_MAC_GENERAL_RAMS2_RM                             (MAC_GENERAL_BASE_ADDRESS + 0x64)
#define	REG_MAC_GENERAL_RAMS3_RM                             (MAC_GENERAL_BASE_ADDRESS + 0x68)
#define	REG_MAC_GENERAL_RAMS4_RM                             (MAC_GENERAL_BASE_ADDRESS + 0x6C)
#define	REG_MAC_GENERAL_RAMS5_RM                             (MAC_GENERAL_BASE_ADDRESS + 0x70)
#define	REG_MAC_GENERAL_RAMS6_RM                             (MAC_GENERAL_BASE_ADDRESS + 0x74)
#define	REG_MAC_GENERAL_CLK_CONTROL_REG_FORCE_ON             (MAC_GENERAL_BASE_ADDRESS + 0x80)
#define	REG_MAC_GENERAL_CLK_CONTROL_REG_DYN_EN               (MAC_GENERAL_BASE_ADDRESS + 0x84)
#define	REG_MAC_GENERAL_ENABLE_UPI_INTERRUPT                 (MAC_GENERAL_BASE_ADDRESS + 0x90)
#define	REG_MAC_GENERAL_UPI_INTERRUPT                        (MAC_GENERAL_BASE_ADDRESS + 0x94)
#define	REG_MAC_GENERAL_UPI_INTERRUPT_SET                    (MAC_GENERAL_BASE_ADDRESS + 0x98)
#define	REG_MAC_GENERAL_UPI_INTERRUPT_CLEAR                  (MAC_GENERAL_BASE_ADDRESS + 0x9C)
#define	REG_MAC_GENERAL_ENABLE_LPI_INTERRUPT                 (MAC_GENERAL_BASE_ADDRESS + 0xA0)
#define	REG_MAC_GENERAL_LPI_INTERRUPT                        (MAC_GENERAL_BASE_ADDRESS + 0xA4)
#define	REG_MAC_GENERAL_LPI_INTERRUPT_SET                    (MAC_GENERAL_BASE_ADDRESS + 0xA8)
#define	REG_MAC_GENERAL_LPI_INTERRUPT_CLEAR                  (MAC_GENERAL_BASE_ADDRESS + 0xAC)
#define	REG_MAC_GENERAL_ENABLE_PHI_INTERRUPT                 (MAC_GENERAL_BASE_ADDRESS + 0xB0)
#define	REG_MAC_GENERAL_PHI_INTERRUPT                        (MAC_GENERAL_BASE_ADDRESS + 0xB4)
#define	REG_MAC_GENERAL_PHI_INTERRUPT_SET                    (MAC_GENERAL_BASE_ADDRESS + 0xB8)
#define	REG_MAC_GENERAL_PHI_INTERRUPT_CLEAR                  (MAC_GENERAL_BASE_ADDRESS + 0xBC)
#define	REG_MAC_GENERAL_ENABLE_NPU2UPI_INTERRUPT             (MAC_GENERAL_BASE_ADDRESS + 0xC0)
#define	REG_MAC_GENERAL_NPU2UPI_INTERRUPT                    (MAC_GENERAL_BASE_ADDRESS + 0xC4)
#define	REG_MAC_GENERAL_NPU2UPI_INTERRUPT_SET                (MAC_GENERAL_BASE_ADDRESS + 0xC8)
#define	REG_MAC_GENERAL_NPU2UPI_INTERRUPT_CLEAR              (MAC_GENERAL_BASE_ADDRESS + 0xCC)
#define	REG_MAC_GENERAL_ENABLE_LPI1_INTERRUPT                (MAC_GENERAL_BASE_ADDRESS + 0xD0)
#define	REG_MAC_GENERAL_LPI1_INTERRUPT                       (MAC_GENERAL_BASE_ADDRESS + 0xD4)
#define	REG_MAC_GENERAL_LPI1_INTERRUPT_SET                   (MAC_GENERAL_BASE_ADDRESS + 0xD8)
#define	REG_MAC_GENERAL_LPI1_INTERRUPT_CLEAR                 (MAC_GENERAL_BASE_ADDRESS + 0xDC)
#define	REG_MAC_GENERAL_HW_LOGGER_STATUS                     (MAC_GENERAL_BASE_ADDRESS + 0xEC)
#define	REG_MAC_GENERAL_WLAN_IP_BASE_ADDR                    (MAC_GENERAL_BASE_ADDRESS + 0xF0)
#define	REG_MAC_GENERAL_HYP_MAC_TOP_GPR                      (MAC_GENERAL_BASE_ADDRESS + 0xF4)
#define	REG_MAC_GENERAL_HYP_MAC_TOP_GPR1                     (MAC_GENERAL_BASE_ADDRESS + 0xF8)
#define	REG_MAC_GENERAL_HYP_MAC_TOP_GPI_INT                  (MAC_GENERAL_BASE_ADDRESS + 0xFC)
#define	REG_MAC_GENERAL_UCPU_TIMEOUT_ADDR_L                  (MAC_GENERAL_BASE_ADDRESS + 0x100)
#define	REG_MAC_GENERAL_LCPU0_TIMEOUT_ADDR_L                 (MAC_GENERAL_BASE_ADDRESS + 0x104)
#define	REG_MAC_GENERAL_LCPU1_TIMEOUT_ADDR_L                 (MAC_GENERAL_BASE_ADDRESS + 0x108)
#define	REG_MAC_GENERAL_PCIH_TIMEOUT_ADDR_L                  (MAC_GENERAL_BASE_ADDRESS + 0x10C)
#define	REG_MAC_GENERAL_HOST_GENRISC_TIMEOUT_ADDR_L          (MAC_GENERAL_BASE_ADDRESS + 0x110)
#define	REG_MAC_GENERAL_B0_RX_GENRISC_TIMEOUT_ADDR_L         (MAC_GENERAL_BASE_ADDRESS + 0x114)
#define	REG_MAC_GENERAL_B0_TX_GENRISC_TIMEOUT_ADDR_L         (MAC_GENERAL_BASE_ADDRESS + 0x118)
#define	REG_MAC_GENERAL_B1_RX_GENRISC_TIMEOUT_ADDR_L         (MAC_GENERAL_BASE_ADDRESS + 0x11C)
#define	REG_MAC_GENERAL_B1_TX_GENRISC_TIMEOUT_ADDR_L         (MAC_GENERAL_BASE_ADDRESS + 0x120)
#define	REG_MAC_GENERAL_TRAINER_MODE                         (MAC_GENERAL_BASE_ADDRESS + 0x128)
#define	REG_MAC_GENERAL_PHY_ARBITER_GCLK_BYPASS              (MAC_GENERAL_BASE_ADDRESS + 0x12C)
#define	REG_MAC_GENERAL_WL_MAC_GENERAL_IRQ_CLR               (MAC_GENERAL_BASE_ADDRESS + 0x130)
#define	REG_MAC_GENERAL_WL_MAC_GENERAL_IRQ_STATUS            (MAC_GENERAL_BASE_ADDRESS + 0x134)
#define	REG_MAC_GENERAL_EXTERNAL_IRQ_INVERT                  (MAC_GENERAL_BASE_ADDRESS + 0x138)
#define	REG_MAC_GENERAL_PAS_SW_RESET_REG_RELEASE             (MAC_GENERAL_BASE_ADDRESS + 0x140)
#define	REG_MAC_GENERAL_PAS_SW_RESET_REG2_RELEASE            (MAC_GENERAL_BASE_ADDRESS + 0x144)
#define	REG_MAC_GENERAL_PAS_SW_RESET_REG_ASSERT              (MAC_GENERAL_BASE_ADDRESS + 0x148)
#define	REG_MAC_GENERAL_PAS_SW_RESET_REG2_ASSERT             (MAC_GENERAL_BASE_ADDRESS + 0x14C)
#define	REG_MAC_GENERAL_PAS_SW_RESET_REG_RD                  (MAC_GENERAL_BASE_ADDRESS + 0x150)
#define	REG_MAC_GENERAL_PAS_SW_RESET_REG2_RD                 (MAC_GENERAL_BASE_ADDRESS + 0x154)
#define	REG_MAC_GENERAL_FCSI_CTL                             (MAC_GENERAL_BASE_ADDRESS + 0x170)
#define	REG_MAC_GENERAL_SPARE_CONFIG_A2D                     (MAC_GENERAL_BASE_ADDRESS + 0x174)
#define	REG_MAC_GENERAL_SPARE_CONFIG_D2A                     (MAC_GENERAL_BASE_ADDRESS + 0x178)
#define	REG_MAC_GENERAL_CLIENT_0_SMPHR_TOGGLE_31TO00         (MAC_GENERAL_BASE_ADDRESS + 0x180)
#define	REG_MAC_GENERAL_CLIENT_0_SMPHR_TOGGLE_63TO32         (MAC_GENERAL_BASE_ADDRESS + 0x184)
#define	REG_MAC_GENERAL_CLIENT_1_SMPHR_TOGGLE_31TO00         (MAC_GENERAL_BASE_ADDRESS + 0x188)
#define	REG_MAC_GENERAL_CLIENT_1_SMPHR_TOGGLE_63TO32         (MAC_GENERAL_BASE_ADDRESS + 0x18C)
#define	REG_MAC_GENERAL_CLIENT_2_SMPHR_TOGGLE_31TO00         (MAC_GENERAL_BASE_ADDRESS + 0x190)
#define	REG_MAC_GENERAL_CLIENT_2_SMPHR_TOGGLE_63TO32         (MAC_GENERAL_BASE_ADDRESS + 0x194)
#define	REG_MAC_GENERAL_CLIENT_0_SMPHR_STAT_15TO00           (MAC_GENERAL_BASE_ADDRESS + 0x198)
#define	REG_MAC_GENERAL_CLIENT_0_SMPHR_STAT_31TO16           (MAC_GENERAL_BASE_ADDRESS + 0x19C)
#define	REG_MAC_GENERAL_CLIENT_0_SMPHR_STAT_47TO32           (MAC_GENERAL_BASE_ADDRESS + 0x1A0)
#define	REG_MAC_GENERAL_CLIENT_0_SMPHR_STAT_63TO48           (MAC_GENERAL_BASE_ADDRESS + 0x1A4)
#define	REG_MAC_GENERAL_CLIENT_1_SMPHR_STAT_15TO00           (MAC_GENERAL_BASE_ADDRESS + 0x1A8)
#define	REG_MAC_GENERAL_CLIENT_1_SMPHR_STAT_31TO16           (MAC_GENERAL_BASE_ADDRESS + 0x1AC)
#define	REG_MAC_GENERAL_CLIENT_1_SMPHR_STAT_47TO32           (MAC_GENERAL_BASE_ADDRESS + 0x1B0)
#define	REG_MAC_GENERAL_CLIENT_1_SMPHR_STAT_63TO48           (MAC_GENERAL_BASE_ADDRESS + 0x1B4)
#define	REG_MAC_GENERAL_CLIENT_2_SMPHR_STAT_15TO00           (MAC_GENERAL_BASE_ADDRESS + 0x1B8)
#define	REG_MAC_GENERAL_CLIENT_2_SMPHR_STAT_31TO16           (MAC_GENERAL_BASE_ADDRESS + 0x1BC)
#define	REG_MAC_GENERAL_CLIENT_2_SMPHR_STAT_47TO32           (MAC_GENERAL_BASE_ADDRESS + 0x1C0)
#define	REG_MAC_GENERAL_CLIENT_2_SMPHR_STAT_63TO48           (MAC_GENERAL_BASE_ADDRESS + 0x1C4)
#define	REG_MAC_GENERAL_LIB_DPL_IDX_SOURCE0_3                (MAC_GENERAL_BASE_ADDRESS + 0x1C8)
#define	REG_MAC_GENERAL_LIB_DPL_IDX_SOURCE4_7                (MAC_GENERAL_BASE_ADDRESS + 0x1CC)
#define	REG_MAC_GENERAL_LIB_DPL_IDX_INPUT                    (MAC_GENERAL_BASE_ADDRESS + 0x1D0)
#define	REG_MAC_GENERAL_LIB_HALT                             (MAC_GENERAL_BASE_ADDRESS + 0x1D4)
#define	REG_MAC_GENERAL_RX_LIB_DPL_IDX_SOURCE0_3             (MAC_GENERAL_BASE_ADDRESS + 0x1D8)
#define	REG_MAC_GENERAL_RX_LIB_DPL_IDX_SOURCE4_7             (MAC_GENERAL_BASE_ADDRESS + 0x1DC)
#define	REG_MAC_GENERAL_RX_LIB_DPL_IDX_INPUT                 (MAC_GENERAL_BASE_ADDRESS + 0x1E0)
#define	REG_MAC_GENERAL_RX_LIB_HALT                          (MAC_GENERAL_BASE_ADDRESS + 0x1E4)
#define	REG_MAC_GENERAL_LIBERATOR_MAX_MESSAGE_TO_LOGGER      (MAC_GENERAL_BASE_ADDRESS + 0x1E8)
#define	REG_MAC_GENERAL_LIBERATOR_DELAY_VALUE_TO_START       (MAC_GENERAL_BASE_ADDRESS + 0x1EC)
#define	REG_MAC_GENERAL_RX_LIBERATOR_DELAY_VALUE_TO_START    (MAC_GENERAL_BASE_ADDRESS + 0x1F0)
#define	REG_MAC_GENERAL_LIBERATOR_DEBUG                      (MAC_GENERAL_BASE_ADDRESS + 0x1F4)
#define	REG_MAC_GENERAL_LIBERATOR_LOGGER_REG                 (MAC_GENERAL_BASE_ADDRESS + 0x1F8)
#define	REG_MAC_GENERAL_PHY_BAND_CONFIG                      (MAC_GENERAL_BASE_ADDRESS + 0x1FC)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_MAC_GENERAL_CONFIG_0 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lmac1AutoAddrMap : 1; //if this is set then hardware automatically adds 12MB to any access that targets Band0 address space, reset value: 0x0, access type: RW
		uint32 b1GenriscRxAutoAddrMap : 1; //if this is set then hardware automatically adds 12MB to any access that targets Band0 address space, reset value: 0x0, access type: RW
		uint32 b1GenriscTxAutoAddrMap : 1; //if this is set then hardware automatically adds 12MB to any access that targets Band0 address space, reset value: 0x0, access type: RW
		uint32 reserved0 : 5;
		uint32 rxcMem1Band0Usg : 1; //Selects between band0 and band1. If '1' then Band 0 supports up to 36 users., reset value: 0x1, access type: RW
		uint32 txcMem1Band0Usg : 1; //Selects between band0 and band1. If '1' then Band 0 supports up to 36 users., reset value: 0x1, access type: RW
		uint32 reserved1 : 2;
		uint32 band0MinVap : 5; //no description, reset value: 0x0, access type: RW
		uint32 reserved2 : 3;
		uint32 band0MinSta : 8; //no description, reset value: 0x0, access type: RW
		uint32 reserved3 : 4;
	} bitFields;
} RegMacGeneralConfig0_u;

/*REG_MAC_GENERAL_SHRAM_MON_BASE_ADDR 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 shramBaseAddr : 24; //Shared RAM monitor base address, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegMacGeneralShramMonBaseAddr_u;

/*REG_MAC_GENERAL_SHRAM_MON_INT_MASK 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 shramIntMask : 16; //Shared RAM monitor interrupt mask, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegMacGeneralShramMonIntMask_u;

/*REG_MAC_GENERAL_SHRAM_MON_STATUS_CLEAR 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 shramStatusClear : 16; //Shared RAM monitor status clear, reset value: 0x0, access type: WO
		uint32 reserved0 : 16;
	} bitFields;
} RegMacGeneralShramMonStatusClear_u;

/*REG_MAC_GENERAL_SHRAM_MON_STATUS_REG 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 shramStatusReg : 16; //Shared RAM monitor status register, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegMacGeneralShramMonStatusReg_u;

/*REG_MAC_GENERAL_SHRAM_MON_CLIENT_AT_IRQ 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 shramClientAtIrq : 4; //Shared RAM client at IRQ, reset value: 0x0, access type: RO
		uint32 reserved0 : 28;
	} bitFields;
} RegMacGeneralShramMonClientAtIrq_u;

/*REG_MAC_GENERAL_SHRAM_RM 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 shramRm0 : 3; //Shared RAM RM_0, reset value: 0x3, access type: RW
		uint32 reserved0 : 1;
		uint32 shramRm1 : 3; //Shared RAM RM_1, reset value: 0x3, access type: RW
		uint32 reserved1 : 1;
		uint32 shramRm2 : 3; //Shared RAM RM_2, reset value: 0x3, access type: RW
		uint32 reserved2 : 21;
	} bitFields;
} RegMacGeneralShramRm_u;

/*REG_MAC_GENERAL_RETRY_DB_BAND1_SINGLE_MODE 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 retryDbBand1SingleMode : 1; //Retry DB Band 1 mode. 0 - Band 1 is not single mode. It uses TID Indexes [18:35] in Retry DB;   1 - Band 1 is single mode. It uses TID Index 36 in Retry DB., reset value: 0x1, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegMacGeneralRetryDbBand1SingleMode_u;

/*REG_MAC_GENERAL_PHY_EXT_RESET 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 macToPhyBand0ResetN : 1; //Reset PHY for Band 0, reset value: 0x0, access type: RW
		uint32 macToPhyBand1ResetN : 1; //Reset PHY for Band 1, reset value: 0x0, access type: RW
		uint32 reserved0 : 30;
	} bitFields;
} RegMacGeneralPhyExtReset_u;

/*REG_MAC_GENERAL_LOGGER_FIFOS_SRC_SEL 0x28 */
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
		uint32 reserved3 : 17;
	} bitFields;
} RegMacGeneralLoggerFifosSrcSel_u;

/*REG_MAC_GENERAL_ENDIAN_SWAP_CTL 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 endianWrCvtFullSwapEna : 1; //PCIH APB byte swapper for write data bus, reset value: 0x0, access type: RW
		uint32 endianRdCvtFullSwapEna : 1; //PCIH APB byte swapper for read data bus, reset value: 0x0, access type: RW
		uint32 reserved0 : 6;
		uint32 axiM0WrSwapMode : 3; //AXI master 0 write data swap mode, reset value: 0x0, access type: RW
		uint32 reserved1 : 1;
		uint32 axiM0RdSwapMode : 3; //AXI master 0 read data swap mode, reset value: 0x0, access type: RW
		uint32 reserved2 : 1;
		uint32 axiM1WrSwapMode : 3; //AXI master 1 write data swap mode, reset value: 0x0, access type: RW
		uint32 reserved3 : 1;
		uint32 axiM1RdSwapMode : 3; //AXI master 1 read data swap mode, reset value: 0x0, access type: RW
		uint32 reserved4 : 1;
		uint32 axiSWrSwapMode : 3; //AXI slave write data swap mode, reset value: 0x0, access type: RW
		uint32 reserved5 : 1;
		uint32 axiSRdSwapMode : 3; //AXI slave read data swap mode, reset value: 0x0, access type: RW
		uint32 reserved6 : 1;
	} bitFields;
} RegMacGeneralEndianSwapCtl_u;

/*REG_MAC_GENERAL_LINK_ADAPTATION_SPARE_REG 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 linkAdaptationSpareReg : 32; //Link adaptation spare register, reset value: 0xfe00, access type: RW
	} bitFields;
} RegMacGeneralLinkAdaptationSpareReg_u;

/*REG_MAC_GENERAL_AHB_ARB_BBCPU_PAGE_REG 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbcpuPageReg : 16; //BBCPU page register, reset value: 0x0, access type: RW
		uint32 bbcpuPageMask : 16; //BBCPU page mask, reset value: 0x0, access type: RW
	} bitFields;
} RegMacGeneralAhbArbBbcpuPageReg_u;

/*REG_MAC_GENERAL_AHB_ARB_GENRISC_HOST_PAGE_REG 0x38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 genriscHostPageReg : 16; //MAC GenRIsc Host page register, reset value: 0x0, access type: RW
		uint32 genriscHostPageMask : 16; //MAC GenRisc Host page mask, reset value: 0xffff, access type: RW
	} bitFields;
} RegMacGeneralAhbArbGenriscHostPageReg_u;

/*REG_MAC_GENERAL_RAM_CTL 0x3C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 1;
		uint32 ramTestMode : 1; //RAM test mode , , reset value: 0x0, access type: RW
		uint32 memGlobalRm : 2; //RAM global RM value , , reset value: 0x0, access type: RW
		uint32 reserved1 : 28;
	} bitFields;
} RegMacGeneralRamCtl_u;

/*REG_MAC_GENERAL_HOST_IRQ_STATUS 0x40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txOutHdCountNotEmptyIrq : 1; //Tx data HD count not empty IRQ ,  , , reset value: 0x0, access type: RO
		uint32 rxOutHdCountNotEmptyIrq : 1; //Rx data HD count not empty IRQ, reset value: 0x0, access type: RO
		uint32 txOutMgmtCountNotEmptyIrq : 1; //Tx management HD count not empty IRQ, reset value: 0x0, access type: RO
		uint32 rxOutMgmtCountNotEmptyIrq : 1; //no description, reset value: 0x0, access type: RO
		uint32 macHostMailboxCountNotEmptyIrq : 1; //no description, reset value: 0x0, access type: RO
		uint32 rabPhiInterrupt : 1; //no description, reset value: 0x0, access type: RO
		uint32 wlanMacXbarDmaErrorIrq : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 25;
	} bitFields;
} RegMacGeneralHostIrqStatus_u;

/*REG_MAC_GENERAL_HOST_IRQ_MASK 0x44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hostIrqMask : 8; //Host IRQ mask. `0` = Mask; `1` = enable , reset value: 0x0, access type: RW
		uint32 reserved0 : 24;
	} bitFields;
} RegMacGeneralHostIrqMask_u;

/*REG_MAC_GENERAL_HYP_MAC_TOP_GPO_PRE 0x48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hypMacTopGpoPre : 16; //hyp_mac_top_gpo ,  , , reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegMacGeneralHypMacTopGpoPre_u;

/*REG_MAC_GENERAL_ACTIVITY_LOGGER_CTL 0x4C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 actLoggerPeriod : 18; //Sets period between samples taken by the logger. Given in system clock cycles , , reset value: 0x27f, access type: RW
		uint32 actLoggerReportLength : 6; //Report length given in activity samples (each is 64 bit) , , reset value: 0x3d, access type: RW
		uint32 actLoggerEnaB0 : 1; //Activity logger enable - band0, reset value: 0x0, access type: RW
		uint32 actLoggerEnaB1 : 1; //Activity logger enable - band1, reset value: 0x0, access type: RW
		uint32 actLoggerPrio : 2; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 4;
	} bitFields;
} RegMacGeneralActivityLoggerCtl_u;

/*REG_MAC_GENERAL_TIMEOUT_PARAMS_0 0x50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ucpuTimeoutVal : 16; //UCPU timeout value, reset value: 0x1234, access type: RW
		uint32 lcpu0TimeoutVal : 16; //LCPU0 timeout value, reset value: 0x1234, access type: RW
	} bitFields;
} RegMacGeneralTimeoutParams0_u;

/*REG_MAC_GENERAL_TIMEOUT_PARAMS_1 0x54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lcpu1TimeoutVal : 16; //LCPU1 timeout value, reset value: 0x1234, access type: RW
		uint32 pcihTimeoutVal : 16; //PCIH timeout value, reset value: 0x1234, access type: RW
	} bitFields;
} RegMacGeneralTimeoutParams1_u;

/*REG_MAC_GENERAL_TIMEOUT_PARAMS_2 0x58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hostGenriscTimeoutVal : 16; //Host genrisc timeout value, reset value: 0x1234, access type: RW
		uint32 rxTxGenriscTimeoutVal : 16; //Rx_Tx genrisc timeout value, reset value: 0x1234, access type: RW
	} bitFields;
} RegMacGeneralTimeoutParams2_u;

/*REG_MAC_GENERAL_RAMS1_RM 0x60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rams1Rm0 : 3; //Various MAC RAMs RM_1_0, reset value: 0x3, access type: RW
		uint32 reserved0 : 1;
		uint32 rams1Rm1 : 3; //Various MAC RAMs RM_1_1, reset value: 0x3, access type: RW
		uint32 reserved1 : 1;
		uint32 rams1Rm2 : 3; //Various MAC RAMs RM_1_2, reset value: 0x3, access type: RW
		uint32 reserved2 : 1;
		uint32 rams1Rm3 : 3; //Various MAC RAMs RM_1_3, reset value: 0x3, access type: RW
		uint32 reserved3 : 1;
		uint32 rams1Rm4 : 3; //Various MAC RAMs RM_1_4, reset value: 0x3, access type: RW
		uint32 reserved4 : 1;
		uint32 rams1Rm5 : 3; //Various MAC RAMs RM_1_5, reset value: 0x3, access type: RW
		uint32 reserved5 : 1;
		uint32 rams1Rm6 : 3; //Various MAC RAMs RM_1_6, reset value: 0x3, access type: RW
		uint32 reserved6 : 1;
		uint32 rams1Rm7 : 3; //Various MAC RAMs RM_1_7, reset value: 0x3, access type: RW
		uint32 reserved7 : 1;
	} bitFields;
} RegMacGeneralRams1Rm_u;

/*REG_MAC_GENERAL_RAMS2_RM 0x64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rams2Rm0 : 3; //Various MAC RAMs RM_2_0, reset value: 0x3, access type: RW
		uint32 reserved0 : 1;
		uint32 rams2Rm1 : 3; //Various MAC RAMs RM_2_1, reset value: 0x3, access type: RW
		uint32 reserved1 : 1;
		uint32 rams2Rm2 : 3; //Various MAC RAMs RM_2_2, reset value: 0x3, access type: RW
		uint32 reserved2 : 1;
		uint32 rams2Rm3 : 3; //Various MAC RAMs RM_2_3, reset value: 0x3, access type: RW
		uint32 reserved3 : 1;
		uint32 rams2Rm4 : 3; //Various MAC RAMs RM_2_4, reset value: 0x3, access type: RW
		uint32 reserved4 : 1;
		uint32 rams2Rm5 : 3; //Various MAC RAMs RM_2_5, reset value: 0x3, access type: RW
		uint32 reserved5 : 1;
		uint32 rams2Rm6 : 3; //Various MAC RAMs RM_2_6, reset value: 0x3, access type: RW
		uint32 reserved6 : 1;
		uint32 rams2Rm7 : 3; //Various MAC RAMs RM_2_7, reset value: 0x3, access type: RW
		uint32 reserved7 : 1;
	} bitFields;
} RegMacGeneralRams2Rm_u;

/*REG_MAC_GENERAL_RAMS3_RM 0x68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rams3Rm0 : 3; //Various MAC RAMs RM_3_0, reset value: 0x3, access type: RW
		uint32 reserved0 : 1;
		uint32 rams3Rm1 : 3; //Various MAC RAMs RM_3_1, reset value: 0x3, access type: RW
		uint32 reserved1 : 1;
		uint32 rams3Rm2 : 3; //Various MAC RAMs RM_3_2, reset value: 0x3, access type: RW
		uint32 reserved2 : 1;
		uint32 rams3Rm3 : 3; //Various MAC RAMs RM_3_3, reset value: 0x3, access type: RW
		uint32 reserved3 : 1;
		uint32 rams3Rm4 : 3; //Various MAC RAMs RM_3_4, reset value: 0x3, access type: RW
		uint32 reserved4 : 1;
		uint32 rams3Rm5 : 3; //Various MAC RAMs RM_3_5, reset value: 0x3, access type: RW
		uint32 reserved5 : 1;
		uint32 rams3Rm6 : 3; //Various MAC RAMs RM_3_6, reset value: 0x3, access type: RW
		uint32 reserved6 : 1;
		uint32 rams3Rm7 : 3; //Various MAC RAMs RM_3_7, reset value: 0x3, access type: RW
		uint32 reserved7 : 1;
	} bitFields;
} RegMacGeneralRams3Rm_u;

/*REG_MAC_GENERAL_RAMS4_RM 0x6C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rams4Rm0 : 3; //Various MAC RAMs RM_4_0, reset value: 0x3, access type: RW
		uint32 reserved0 : 1;
		uint32 rams4Rm1 : 3; //Various MAC RAMs RM_4_1, reset value: 0x3, access type: RW
		uint32 reserved1 : 1;
		uint32 rams4Rm2 : 3; //Various MAC RAMs RM_4_2, reset value: 0x3, access type: RW
		uint32 reserved2 : 1;
		uint32 rams4Rm3 : 3; //Various MAC RAMs RM_4_3, reset value: 0x3, access type: RW
		uint32 reserved3 : 1;
		uint32 rams4Rm4 : 3; //Various MAC RAMs RM_4_4, reset value: 0x3, access type: RW
		uint32 reserved4 : 1;
		uint32 rams4Rm5 : 3; //Various MAC RAMs RM_4_5, reset value: 0x3, access type: RW
		uint32 reserved5 : 1;
		uint32 rams4Rm6 : 3; //Various MAC RAMs RM_4_6, reset value: 0x3, access type: RW
		uint32 reserved6 : 1;
		uint32 rams4Rm7 : 3; //Various MAC RAMs RM_4_7, reset value: 0x3, access type: RW
		uint32 reserved7 : 1;
	} bitFields;
} RegMacGeneralRams4Rm_u;

/*REG_MAC_GENERAL_RAMS5_RM 0x70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rams5Rm0 : 3; //Various MAC RAMs RM_5_0, reset value: 0x3, access type: RW
		uint32 reserved0 : 1;
		uint32 rams5Rm1 : 3; //Various MAC RAMs RM_5_1, reset value: 0x3, access type: RW
		uint32 reserved1 : 1;
		uint32 rams5Rm2 : 3; //Various MAC RAMs RM_5_2, reset value: 0x3, access type: RW
		uint32 reserved2 : 1;
		uint32 rams5Rm3 : 3; //Various MAC RAMs RM_5_3, reset value: 0x3, access type: RW
		uint32 reserved3 : 1;
		uint32 rams5Rm4 : 3; //Various MAC RAMs RM_5_4, reset value: 0x3, access type: RW
		uint32 reserved4 : 1;
		uint32 rams5Rm5 : 3; //Various MAC RAMs RM_5_5, reset value: 0x3, access type: RW
		uint32 reserved5 : 1;
		uint32 rams5Rm6 : 3; //Various MAC RAMs RM_5_6, reset value: 0x3, access type: RW
		uint32 reserved6 : 1;
		uint32 rams5Rm7 : 3; //Various MAC RAMs RM_5_7, reset value: 0x3, access type: RW
		uint32 reserved7 : 1;
	} bitFields;
} RegMacGeneralRams5Rm_u;

/*REG_MAC_GENERAL_RAMS6_RM 0x74 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rams6Rm0 : 3; //no description, reset value: 0x3, access type: RW
		uint32 reserved0 : 1;
		uint32 rams6Rm1 : 3; //no description, reset value: 0x3, access type: RW
		uint32 reserved1 : 1;
		uint32 rams6Rm2 : 3; //Various MAC RAMs RM_6_2, reset value: 0x3, access type: RW
		uint32 reserved2 : 1;
		uint32 rams6Rm3 : 3; //Various MAC RAMs RM_6_3, reset value: 0x3, access type: RW
		uint32 reserved3 : 1;
		uint32 rams6Rm4 : 3; //Various MAC RAMs RM_6_4, reset value: 0x3, access type: RW
		uint32 reserved4 : 1;
		uint32 rams6Rm5 : 3; //RM value for RX_COOR statistics counters RAM, reset value: 0x3, access type: RW
		uint32 reserved5 : 1;
		uint32 rams6Rm6 : 3; //RM value for mac_xbar reorder buffer memories (see "urfh2ec39a0d" instance name)., reset value: 0x3, access type: RW
		uint32 reserved6 : 1;
		uint32 rams6Rm7 : 3; //RM value for mac_xbar DMA buffer memories (see "urfh5fd8eed5_0", "urfh5fd8eed5_1" instance names)., reset value: 0x3, access type: RW
		uint32 reserved7 : 1;
	} bitFields;
} RegMacGeneralRams6Rm_u;

/*REG_MAC_GENERAL_CLK_CONTROL_REG_FORCE_ON 0x80 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 clkControlForceOnQManager0 : 1; //Clock Force On Q_MANAGER_0, reset value: 0x0, access type: RW
		uint32 clkControlForceOnQManager1 : 1; //Clock Force On Q_MANAGER_1, reset value: 0x0, access type: RW
		uint32 clkControlForceOnQManager2 : 1; //Clock Force On Q_MANAGER_2, reset value: 0x0, access type: RW
		uint32 clkControlForceOnQManager3 : 1; //Clock Force On Q_MANAGER_3, reset value: 0x0, access type: RW
		uint32 clkControlForceOnDlm0 : 1; //Clock Force On DLM_0, reset value: 0x0, access type: RW
		uint32 clkControlForceOnDlm1 : 1; //Clock Force On DLM_1, reset value: 0x0, access type: RW
		uint32 clkControlForceOnDlm2 : 1; //Clock Force On DLM_2, reset value: 0x0, access type: RW
		uint32 clkControlForceOnDlm3 : 1; //Clock Force On DLM_3, reset value: 0x0, access type: RW
		uint32 clkControlForceOnDlm4 : 1; //Clock Force On DLM_4, reset value: 0x0, access type: RW
		uint32 clkControlForceOnDlm5 : 1; //Clock Force On DLM_5, reset value: 0x0, access type: RW
		uint32 clkControlForceOnDlm6 : 1; //Clock Force On DLM_6, reset value: 0x0, access type: RW
		uint32 clkControlForceOnDlm7 : 1; //Clock Force On DLM_7, reset value: 0x0, access type: RW
		uint32 clkControlForceOnDlm8 : 1; //Clock Force On DLM_8, reset value: 0x0, access type: RW
		uint32 clkControlForceOnQManagerAger : 1; //Clock Force On Q_MANAGER_AGER, reset value: 0x0, access type: RW
		uint32 clkControlForceOnRxPp0 : 1; //Clock Force On Reserved 0, reset value: 0x0, access type: RW
		uint32 clkControlForceOnRxPp1 : 1; //Clock Force On Reserved 1, reset value: 0x0, access type: RW
		uint32 clkControlForceOnDma0 : 1; //Clock Force On DMA_0, reset value: 0x0, access type: RW
		uint32 clkControlForceOnDma1 : 1; //Clock Force On DMA_1, reset value: 0x0, access type: RW
		uint32 clkControlForceOnTxLiberator : 1; //Clock Force On Tx liberator, reset value: 0x0, access type: RW
		uint32 clkControlForceOnRxLiberator : 1; //Clock Force On Rx liberator, reset value: 0x0, access type: RW
		uint32 clkControlForceOnProtDb : 1; //Clock Force On PROT_DB, reset value: 0x0, access type: RW
		uint32 clkControlForceOnSemaphore : 1; //Clock Force On Semaphore, reset value: 0x0, access type: RW
		uint32 clkControlForceOnRxAddr1ToVapIdx : 1; //Clock Force On Rx address1 to VAP index, reset value: 0x0, access type: RW
		uint32 clkControlForceOnLogger : 1; //Clock Force On Reserved 6, reset value: 0x0, access type: RW
		uint32 clkControlForceOnReserved7 : 1; //Clock Force On Reserved 7, reset value: 0x0, access type: RW
		uint32 clkControlForceOnReserved8 : 1; //Clock Force On Reserved 8, reset value: 0x0, access type: RW
		uint32 clkControlForceOnReserved9 : 1; //Clock Force On Reserved 9, reset value: 0x0, access type: RW
		uint32 clkControlForceOnReserved10 : 1; //Clock Force On Reserved 10, reset value: 0x0, access type: RW
		uint32 clkControlForceOnReserved11 : 1; //Clock Force On Reserved 11, reset value: 0x0, access type: RW
		uint32 clkControlForceOnReserved12 : 1; //Clock Force On Reserved 12, reset value: 0x0, access type: RW
		uint32 clkControlForceOnReserved13 : 1; //Clock Force On Reserved 13, reset value: 0x0, access type: RW
		uint32 clkControlForceOnReserved14 : 1; //Clock Force On Reserved 14, reset value: 0x0, access type: RW
	} bitFields;
} RegMacGeneralClkControlRegForceOn_u;

/*REG_MAC_GENERAL_CLK_CONTROL_REG_DYN_EN 0x84 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 clkControlDynEnQManager0 : 1; //Clock Dynamic Enable Q_MANAGER_0, reset value: 0x0, access type: RW
		uint32 clkControlDynEnQManager1 : 1; //Clock Dynamic Enable Q_MANAGER_1, reset value: 0x0, access type: RW
		uint32 clkControlDynEnQManager2 : 1; //Clock Dynamic Enable Q_MANAGER_2, reset value: 0x0, access type: RW
		uint32 clkControlDynEnQManager3 : 1; //Clock Dynamic Enable Q_MANAGER_3, reset value: 0x0, access type: RW
		uint32 clkControlDynEnDlm0 : 1; //Clock Dynamic Enable DLM_0, reset value: 0x0, access type: RW
		uint32 clkControlDynEnDlm1 : 1; //Clock Dynamic Enable DLM_1, reset value: 0x0, access type: RW
		uint32 clkControlDynEnDlm2 : 1; //Clock Dynamic Enable DLM_2, reset value: 0x0, access type: RW
		uint32 clkControlDynEnDlm3 : 1; //Clock Dynamic Enable DLM_3, reset value: 0x0, access type: RW
		uint32 clkControlDynEnDlm4 : 1; //Clock Dynamic Enable DLM_4, reset value: 0x0, access type: RW
		uint32 clkControlDynEnDlm5 : 1; //Clock Dynamic Enable DLM_5, reset value: 0x0, access type: RW
		uint32 clkControlDynEnDlm6 : 1; //Clock Dynamic Enable DLM_6, reset value: 0x0, access type: RW
		uint32 clkControlDynEnDlm7 : 1; //Clock Dynamic Enable DLM_7, reset value: 0x0, access type: RW
		uint32 clkControlDynEnDlm8 : 1; //Clock Dynamic Enable DLM_8, reset value: 0x0, access type: RW
		uint32 clkControlDynEnQManagerAger : 1; //Clock Dynamic Enable Q_MANAGER_AGER, reset value: 0x0, access type: RW
		uint32 clkControlDynEnRxPp0 : 1; //Clock Dynamic Enable Reserved 0, reset value: 0x0, access type: RW
		uint32 clkControlDynEnRxPp1 : 1; //Clock Dynamic Enable Reserved 1, reset value: 0x0, access type: RW
		uint32 clkControlDynEnDma0 : 1; //Clock Dynamic Enable DMA_0, reset value: 0x0, access type: RW
		uint32 clkControlDynEnDma1 : 1; //Clock Dynamic Enable DMA_1, reset value: 0x0, access type: RW
		uint32 clkControlDynEnTxLiberator : 1; //Clock Dynamic Enable Tx liberator, reset value: 0x0, access type: RW
		uint32 clkControlDynEnRxLiberator : 1; //Clock Dynamic Enable Rx liberator, reset value: 0x0, access type: RW
		uint32 clkControlDynEnProtDb : 1; //Clock Dynamic Enable PROT_DB, reset value: 0x0, access type: RW
		uint32 clkControlDynEnSemaphore : 1; //Clock Dynamic Enable Semaphore, reset value: 0x0, access type: RW
		uint32 clkControlDynEnRxAddr1ToVapIdx : 1; //Clock Dynamic Enable Rx address1 to VAP index, reset value: 0x0, access type: RW
		uint32 clkControlDynEnLogger : 1; //Clock Dynamic Enable Reserved 6, reset value: 0x0, access type: RW
		uint32 clkControlDynEnReserved7 : 1; //Clock Dynamic Enable Reserved 7, reset value: 0x0, access type: RW
		uint32 clkControlDynEnReserved8 : 1; //Clock Dynamic Enable Reserved 8, reset value: 0x0, access type: RW
		uint32 clkControlDynEnReserved9 : 1; //Clock Dynamic Enable Reserved 9, reset value: 0x0, access type: RW
		uint32 clkControlDynEnReserved10 : 1; //Clock Dynamic Enable Reserved 10, reset value: 0x0, access type: RW
		uint32 clkControlDynEnReserved11 : 1; //Clock Dynamic Enable Reserved 11, reset value: 0x0, access type: RW
		uint32 clkControlDynEnReserved12 : 1; //Clock Dynamic Enable Reserved 12, reset value: 0x0, access type: RW
		uint32 clkControlDynEnReserved13 : 1; //Clock Dynamic Enable Reserved 13, reset value: 0x0, access type: RW
		uint32 clkControlDynEnReserved14 : 1; //Clock Dynamic Enable Reserved 14, reset value: 0x0, access type: RW
	} bitFields;
} RegMacGeneralClkControlRegDynEn_u;

/*REG_MAC_GENERAL_ENABLE_UPI_INTERRUPT 0x90 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 enableUpiInterrupt : 24; //Enable Upper CPU RAB IRQ, reset value: 0xffffff, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegMacGeneralEnableUpiInterrupt_u;

/*REG_MAC_GENERAL_UPI_INTERRUPT 0x94 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 upiInterrupt : 24; //RAB  upi interrupt, reset value: 0x0, access type: RO
		uint32 reserved0 : 8;
	} bitFields;
} RegMacGeneralUpiInterrupt_u;

/*REG_MAC_GENERAL_UPI_INTERRUPT_SET 0x98 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 upiInterruptSet : 24; //RAB  upi interrupt set, reset value: 0x0, access type: WO
		uint32 reserved0 : 8;
	} bitFields;
} RegMacGeneralUpiInterruptSet_u;

/*REG_MAC_GENERAL_UPI_INTERRUPT_CLEAR 0x9C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 upiInterruptClear : 24; //RAB  upi interrupt clear, reset value: 0x0, access type: WO
		uint32 reserved0 : 8;
	} bitFields;
} RegMacGeneralUpiInterruptClear_u;

/*REG_MAC_GENERAL_ENABLE_LPI_INTERRUPT 0xA0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 enableLpiInterrupt : 24; //Enable Lower CPU RAB IRQ, reset value: 0xffffff, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegMacGeneralEnableLpiInterrupt_u;

/*REG_MAC_GENERAL_LPI_INTERRUPT 0xA4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lpiInterrupt : 24; //RAB  lpi interrupt, reset value: 0x0, access type: RO
		uint32 reserved0 : 8;
	} bitFields;
} RegMacGeneralLpiInterrupt_u;

/*REG_MAC_GENERAL_LPI_INTERRUPT_SET 0xA8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lpiInterruptSet : 24; //RAB  lpi interrupt set, reset value: 0x0, access type: WO
		uint32 reserved0 : 8;
	} bitFields;
} RegMacGeneralLpiInterruptSet_u;

/*REG_MAC_GENERAL_LPI_INTERRUPT_CLEAR 0xAC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lpiInterruptClear : 24; //RAB  lpi interrupt clear, reset value: 0x0, access type: WO
		uint32 reserved0 : 8;
	} bitFields;
} RegMacGeneralLpiInterruptClear_u;

/*REG_MAC_GENERAL_ENABLE_PHI_INTERRUPT 0xB0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 enablePhiInterrupt : 24; //Enable PHI host RAB IRQ, reset value: 0xffffff, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegMacGeneralEnablePhiInterrupt_u;

/*REG_MAC_GENERAL_PHI_INTERRUPT 0xB4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phiInterrupt : 24; //RAB  phi interrupt, reset value: 0x0, access type: RO
		uint32 reserved0 : 8;
	} bitFields;
} RegMacGeneralPhiInterrupt_u;

/*REG_MAC_GENERAL_PHI_INTERRUPT_SET 0xB8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phiInterruptSet : 24; //RAB  phi interrupt set, reset value: 0x0, access type: WO
		uint32 reserved0 : 8;
	} bitFields;
} RegMacGeneralPhiInterruptSet_u;

/*REG_MAC_GENERAL_PHI_INTERRUPT_CLEAR 0xBC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phiInterruptClear : 24; //RAB  phi interrupt clear, reset value: 0x0, access type: WO
		uint32 reserved0 : 8;
	} bitFields;
} RegMacGeneralPhiInterruptClear_u;

/*REG_MAC_GENERAL_ENABLE_NPU2UPI_INTERRUPT 0xC0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 enableNpu2UpiInterrupt : 24; //Enable NPU to UPI RAB IRQ, reset value: 0xffffff, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegMacGeneralEnableNpu2UpiInterrupt_u;

/*REG_MAC_GENERAL_NPU2UPI_INTERRUPT 0xC4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 npu2UpiInterrupt : 24; //RAB NPU to UPI interrupt, reset value: 0x0, access type: RO
		uint32 reserved0 : 8;
	} bitFields;
} RegMacGeneralNpu2UpiInterrupt_u;

/*REG_MAC_GENERAL_NPU2UPI_INTERRUPT_SET 0xC8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 npu2UpiInterruptSet : 24; //RAB NPU to UPI interrupt set, reset value: 0x0, access type: WO
		uint32 reserved0 : 8;
	} bitFields;
} RegMacGeneralNpu2UpiInterruptSet_u;

/*REG_MAC_GENERAL_NPU2UPI_INTERRUPT_CLEAR 0xCC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 npu2UpiInterruptClear : 24; //RAB NPU to UPI interrupt clear, reset value: 0x0, access type: WO
		uint32 reserved0 : 8;
	} bitFields;
} RegMacGeneralNpu2UpiInterruptClear_u;

/*REG_MAC_GENERAL_ENABLE_LPI1_INTERRUPT 0xD0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 enableLpi1Interrupt : 24; //no description, reset value: 0xffffff, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegMacGeneralEnableLpi1Interrupt_u;

/*REG_MAC_GENERAL_LPI1_INTERRUPT 0xD4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lpi1Interrupt : 24; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 8;
	} bitFields;
} RegMacGeneralLpi1Interrupt_u;

/*REG_MAC_GENERAL_LPI1_INTERRUPT_SET 0xD8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lpi1InterruptSet : 24; //no description, reset value: 0x0, access type: WO
		uint32 reserved0 : 8;
	} bitFields;
} RegMacGeneralLpi1InterruptSet_u;

/*REG_MAC_GENERAL_LPI1_INTERRUPT_CLEAR 0xDC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lpi1InterruptClear : 24; //no description, reset value: 0x0, access type: WO
		uint32 reserved0 : 8;
	} bitFields;
} RegMacGeneralLpi1InterruptClear_u;

/*REG_MAC_GENERAL_HW_LOGGER_STATUS 0xEC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 actLoggerRunning0 : 1; //Activity Logger 0 running indication, reset value: 0x0, access type: RO
		uint32 actLoggerRunning1 : 1; //Activity Logger 1 running indication, reset value: 0x0, access type: RO
		uint32 reserved0 : 30;
	} bitFields;
} RegMacGeneralHwLoggerStatus_u;

/*REG_MAC_GENERAL_WLAN_IP_BASE_ADDR 0xF0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 wlanIpBaseAddr : 32; //WLAN IP Base address (The configured 7 bit field is located in bits 31:25. All other bits are constant zeros), reset value: 0x18000000, access type: RO
	} bitFields;
} RegMacGeneralWlanIpBaseAddr_u;

/*REG_MAC_GENERAL_HYP_MAC_TOP_GPR 0xF4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hypMacTopGpr : 32; //hyp_mac_top_gpr , , reset value: 0x0, access type: RW
	} bitFields;
} RegMacGeneralHypMacTopGpr_u;

/*REG_MAC_GENERAL_HYP_MAC_TOP_GPR1 0xF8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hypMacTopGpr1 : 32; //hyp_mac_top_gpr1, reset value: 0x0, access type: RW
	} bitFields;
} RegMacGeneralHypMacTopGpr1_u;

/*REG_MAC_GENERAL_HYP_MAC_TOP_GPI_INT 0xFC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hypMacTopGpiInt : 16; //hyp_mac_top_gpi , , reset value: 0x200, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegMacGeneralHypMacTopGpiInt_u;

/*REG_MAC_GENERAL_UCPU_TIMEOUT_ADDR_L 0x100 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ucpuTimeoutAddrL : 32; //UCPU timeout address, reset value: 0x0, access type: RO
	} bitFields;
} RegMacGeneralUcpuTimeoutAddrL_u;

/*REG_MAC_GENERAL_LCPU0_TIMEOUT_ADDR_L 0x104 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lcpu0TimeoutAddrL : 32; //LCPU0 timeout address, reset value: 0x0, access type: RO
	} bitFields;
} RegMacGeneralLcpu0TimeoutAddrL_u;

/*REG_MAC_GENERAL_LCPU1_TIMEOUT_ADDR_L 0x108 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lcpu1TimeoutAddrL : 32; //LCPU1 timeout address, reset value: 0x0, access type: RO
	} bitFields;
} RegMacGeneralLcpu1TimeoutAddrL_u;

/*REG_MAC_GENERAL_PCIH_TIMEOUT_ADDR_L 0x10C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pcihTimeoutAddrL : 32; //PCIH timeout address, reset value: 0x0, access type: RO
	} bitFields;
} RegMacGeneralPcihTimeoutAddrL_u;

/*REG_MAC_GENERAL_HOST_GENRISC_TIMEOUT_ADDR_L 0x110 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hostGenriscTimeoutAddrL : 32; //Host Genrisc timeout address, reset value: 0x0, access type: RO
	} bitFields;
} RegMacGeneralHostGenriscTimeoutAddrL_u;

/*REG_MAC_GENERAL_B0_RX_GENRISC_TIMEOUT_ADDR_L 0x114 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 b0RxGenriscTimeoutAddrL : 32; //Band 0 Rx Genrisc timeout address, reset value: 0x0, access type: RO
	} bitFields;
} RegMacGeneralB0RxGenriscTimeoutAddrL_u;

/*REG_MAC_GENERAL_B0_TX_GENRISC_TIMEOUT_ADDR_L 0x118 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 b0TxGenriscTimeoutAddrL : 32; //Band 0 Tx Genrisc timeout address, reset value: 0x0, access type: RO
	} bitFields;
} RegMacGeneralB0TxGenriscTimeoutAddrL_u;

/*REG_MAC_GENERAL_B1_RX_GENRISC_TIMEOUT_ADDR_L 0x11C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 b1RxGenriscTimeoutAddrL : 32; //Band 1 Rx Genrisc timeout address, reset value: 0x0, access type: RO
	} bitFields;
} RegMacGeneralB1RxGenriscTimeoutAddrL_u;

/*REG_MAC_GENERAL_B1_TX_GENRISC_TIMEOUT_ADDR_L 0x120 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 b1TxGenriscTimeoutAddrL : 32; //Band 1 Tx Genrisc timeout address, reset value: 0x0, access type: RO
	} bitFields;
} RegMacGeneralB1TxGenriscTimeoutAddrL_u;

/*REG_MAC_GENERAL_TRAINER_MODE 0x128 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 trainerMode : 1; //Trainer mode, reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegMacGeneralTrainerMode_u;

/*REG_MAC_GENERAL_PHY_ARBITER_GCLK_BYPASS 0x12C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phyArbiterGclkBypass : 1; //Phy arbiter gclk bypass, reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegMacGeneralPhyArbiterGclkBypass_u;

/*REG_MAC_GENERAL_WL_MAC_GENERAL_IRQ_CLR 0x130 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 externalIrqClr : 4; //External IRQ clear (write '1' to clear), reset value: 0x0, access type: WO
		uint32 reserved0 : 28;
	} bitFields;
} RegMacGeneralWlMacGeneralIrqClr_u;

/*REG_MAC_GENERAL_WL_MAC_GENERAL_IRQ_STATUS 0x134 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 externalIrqStatus : 4; //External IRQ status, reset value: 0x0, access type: RO
		uint32 reserved0 : 28;
	} bitFields;
} RegMacGeneralWlMacGeneralIrqStatus_u;

/*REG_MAC_GENERAL_EXTERNAL_IRQ_INVERT 0x138 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 externalIrqInvert : 4; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 28;
	} bitFields;
} RegMacGeneralExternalIrqInvert_u;

/*REG_MAC_GENERAL_PAS_SW_RESET_REG_RELEASE 0x140 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swResetReleaseDmacWrapper : 1; //DMAC_WRAPPER SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseDataLogger : 1; //DATA LOGGER SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseHostGenrisc : 1; //HOST GENRISC SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseMacBlockAck : 1; //MAC BLOCK ACK SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseMacTrainer : 1; //MAC TRAINER SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseReserved5 : 1; //Reserved SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseSemaphore : 1; //SEMAPHORE SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseHostIf0 : 1; //HOST_IF_0 SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseHostIf1 : 1; //HOST_IF_1 SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseHostIf2 : 1; //HOST_IF_2 SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseHostIf3 : 1; //HOST_IF_3 SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseHostIf4 : 1; //HOST_IF_4 SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseRxPp : 1; //RX_PP SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseProtDb : 1; //PROT_DB SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseProtDbReg : 1; //PROT_DB_REG SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseLiberator : 1; //LIBERATOR SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseRxLiberator : 1; //RX_LIBERATOR SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseTrafficIndicator : 1; //Traffic indicator SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseRxAddr1ToVapIdx : 1; //Rx address 1 to VAP index SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseReserved : 13; //Reserved SW enable Release, by write '1', reset value: 0x0, access type: WO
	} bitFields;
} RegMacGeneralPasSwResetRegRelease_u;

/*REG_MAC_GENERAL_PAS_SW_RESET_REG2_RELEASE 0x144 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swResetReleaseQManager0 : 1; //Q_MANAGER_0 SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseQManager1 : 1; //Q_MANAGER_1 SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseQManager2 : 1; //Q_MANAGER_2 SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseQManager3 : 1; //Q_MANAGER_3 SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseQManager4 : 1; //Q_MANAGER_4 SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseQManager5 : 1; //Q_MANAGER_5 SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseQManager6 : 1; //Q_MANAGER_6 SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseQManager7 : 1; //Q_MANAGER_7 SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseQManager8 : 1; //Q_MANAGER_8 SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseQManager9 : 1; //Q_MANAGER_9 SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseQManager10 : 1; //Q_MANAGER_10 SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseQManager11 : 1; //Q_MANAGER_11 SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseQManager12 : 1; //Q_MANAGER_12 SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseQManager13 : 1; //Q_MANAGER_13 SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseQManager14 : 1; //Q_MANAGER_14 SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseQManager15 : 1; //Q_MANAGER_15 SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseQManager16 : 1; //Q_MANAGER_16 SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseQManager17 : 1; //Q_MANAGER_17 SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseQManager18 : 1; //Q_MANAGER_18 SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseQManagerAger : 1; //Q_MANAGER Ager SW enable Release, by write '1', reset value: 0x0, access type: WO
		uint32 swResetReleaseReserved2 : 12; //Reserved2 SW enable Release, by write '1', reset value: 0x0, access type: WO
	} bitFields;
} RegMacGeneralPasSwResetReg2Release_u;

/*REG_MAC_GENERAL_PAS_SW_RESET_REG_ASSERT 0x148 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swResetAssertDmacWrapper : 1; //DMAC_WRAPPER SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertDataLogger : 1; //DATA LOGGER SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertHostGenrisc : 1; //HOST GENRISC SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertMacBlockAck : 1; //MAC BLOCK ACK SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertMacTrainer : 1; //MAC TRAINER SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertReserved5 : 1; //Reserved SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertSemaphore : 1; //SEMAPHORE SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertHostIf0 : 1; //HOST_IF_0 SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertHostIf1 : 1; //HOST_IF_1 SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertHostIf2 : 1; //HOST_IF_2 SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertHostIf3 : 1; //HOST_IF_3 SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertHostIf4 : 1; //HOST_IF_4 SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertRxPp : 1; //RX_PP SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertProtDb : 1; //PROT_DB SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertProtDbReg : 1; //PROT_DB_REG SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertLiberator : 1; //LIBERATOR SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertRxLiberator : 1; //RX_LIBERATOR SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertTrafficIndicator : 1; //Traffic indicator SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertRxAddr1ToVapIdx : 1; //Rx address1 to VAP index SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertReserved : 13; //Reserved SW enable Assert, by write '1', reset value: 0x0, access type: WO
	} bitFields;
} RegMacGeneralPasSwResetRegAssert_u;

/*REG_MAC_GENERAL_PAS_SW_RESET_REG2_ASSERT 0x14C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swResetAssertQManager0 : 1; //Q_MANAGER_0 SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertQManager1 : 1; //Q_MANAGER_1 SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertQManager2 : 1; //Q_MANAGER_2 SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertQManager3 : 1; //Q_MANAGER_3 SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertQManager4 : 1; //Q_MANAGER_4 SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertQManager5 : 1; //Q_MANAGER_5 SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertQManager6 : 1; //Q_MANAGER_6 SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertQManager7 : 1; //Q_MANAGER_7 SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertQManager8 : 1; //Q_MANAGER_8 SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertQManager9 : 1; //Q_MANAGER_9 SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertQManager10 : 1; //Q_MANAGER_10 SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertQManager11 : 1; //Q_MANAGER_11 SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertQManager12 : 1; //Q_MANAGER_12 SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertQManager13 : 1; //Q_MANAGER_13 SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertQManager14 : 1; //Q_MANAGER_14 SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertQManager15 : 1; //Q_MANAGER_15 SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertQManager16 : 1; //Q_MANAGER_16 SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertQManager17 : 1; //Q_MANAGER_17 SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertQManager18 : 1; //Q_MANAGER_18 SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertQManagerAger : 1; //Q_MANAGER Ager SW enable Assert, by write '1', reset value: 0x0, access type: WO
		uint32 swResetAssertReserved2 : 12; //Reserved2 SW enable Assert, by write '1', reset value: 0x0, access type: WO
	} bitFields;
} RegMacGeneralPasSwResetReg2Assert_u;

/*REG_MAC_GENERAL_PAS_SW_RESET_REG_RD 0x150 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swResetRdDmacWrapper : 1; //DMAC_WRAPPER SW enable, reset value: 0x1, access type: RO
		uint32 swResetRdDataLogger : 1; //DATA LOGGER SW enable, reset value: 0x1, access type: RO
		uint32 swResetRdHostGenrisc : 1; //HOST GENRISC SW enable, reset value: 0x1, access type: RO
		uint32 swResetRdMacBlockAck : 1; //MAC BLOCK ACK SW enable, reset value: 0x1, access type: RO
		uint32 swResetRdMacTrainer : 1; //MAC TRAINER SW enable, reset value: 0x1, access type: RO
		uint32 swResetRdReserved5 : 1; //Reserved SW enable, reset value: 0x1, access type: RO
		uint32 swResetRdSemaphore : 1; //SEMAPHORE SW enable, reset value: 0x1, access type: RO
		uint32 swResetRdHostIf0 : 1; //HOST_IF_0 SW enable, reset value: 0x1, access type: RO
		uint32 swResetRdHostIf1 : 1; //HOST_IF_1 SW enable, reset value: 0x1, access type: RO
		uint32 swResetRdHostIf2 : 1; //HOST_IF_2 SW enable, reset value: 0x1, access type: RO
		uint32 swResetRdHostIf3 : 1; //HOST_IF_3 SW enable, reset value: 0x1, access type: RO
		uint32 swResetRdHostIf4 : 1; //HOST_IF_4 SW enable, reset value: 0x1, access type: RO
		uint32 swResetRdRxPp : 1; //RX_PP SW enable, reset value: 0x1, access type: RO
		uint32 swResetRdProtDb : 1; //PROT_DB SW enable, reset value: 0x1, access type: RO
		uint32 swResetRdProtDbReg : 1; //PROT_DB_REG SW enable, reset value: 0x1, access type: RO
		uint32 swResetRdLiberator : 1; //LIBERATOR SW enable, reset value: 0x1, access type: RO
		uint32 swResetRdRxLiberator : 1; //RX_LIBERATOR SW enable, reset value: 0x1, access type: RO
		uint32 swResetRdTrafficIndicator : 1; //Traffic indicator SW enable, reset value: 0x1, access type: RO
		uint32 swResetRdRxAddr1ToVapIdx : 1; //Rx address1 to VAP index SW enable, reset value: 0x1, access type: RO
		uint32 swResetRdReserved : 13; //Reserved SW enable, reset value: 0x1fff, access type: RO
	} bitFields;
} RegMacGeneralPasSwResetRegRd_u;

/*REG_MAC_GENERAL_PAS_SW_RESET_REG2_RD 0x154 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swResetRdQManager0 : 1; //Q_MANAGER_0 SW enable , reset value: 0x1, access type: RO
		uint32 swResetRdQManager1 : 1; //Q_MANAGER_1 SW enable , reset value: 0x1, access type: RO
		uint32 swResetRdQManager2 : 1; //Q_MANAGER_2 SW enable , reset value: 0x1, access type: RO
		uint32 swResetRdQManager3 : 1; //Q_MANAGER_3 SW enable , reset value: 0x1, access type: RO
		uint32 swResetRdQManager4 : 1; //Q_MANAGER_4 SW enable  , reset value: 0x1, access type: RO
		uint32 swResetRdQManager5 : 1; //Q_MANAGER_5 SW enable  , reset value: 0x1, access type: RO
		uint32 swResetRdQManager6 : 1; //Q_MANAGER_6 SW enable  , reset value: 0x1, access type: RO
		uint32 swResetRdQManager7 : 1; //Q_MANAGER_7 SW enable  , reset value: 0x1, access type: RO
		uint32 swResetRdQManager8 : 1; //Q_MANAGER_8 SW enable , reset value: 0x1, access type: RO
		uint32 swResetRdQManager9 : 1; //Q_MANAGER_9 SW enable , reset value: 0x1, access type: RO
		uint32 swResetRdQManager10 : 1; //Q_MANAGER_10 SW enable , reset value: 0x1, access type: RO
		uint32 swResetRdQManager11 : 1; //Q_MANAGER_11 SW enable , reset value: 0x1, access type: RO
		uint32 swResetRdQManager12 : 1; //Q_MANAGER_12 SW enable , reset value: 0x1, access type: RO
		uint32 swResetRdQManager13 : 1; //Q_MANAGER_13 SW enable , reset value: 0x1, access type: RO
		uint32 swResetRdQManager14 : 1; //Q_MANAGER_14 SW enable , reset value: 0x1, access type: RO
		uint32 swResetRdQManager15 : 1; //Q_MANAGER_15 SW enable , reset value: 0x1, access type: RO
		uint32 swResetRdQManager16 : 1; //Q_MANAGER_16 SW enable , reset value: 0x1, access type: RO
		uint32 swResetRdQManager17 : 1; //Q_MANAGER_17 SW enable , reset value: 0x1, access type: RO
		uint32 swResetRdQManager18 : 1; //Q_MANAGER_18 SW enable , reset value: 0x1, access type: RO
		uint32 swResetRdQManagerAger : 1; //Q_MANAGER Ager SW enable , reset value: 0x1, access type: RO
		uint32 swResetRdReserved2 : 12; //Reserved2 SW enable , reset value: 0xfff, access type: RO
	} bitFields;
} RegMacGeneralPasSwResetReg2Rd_u;

/*REG_MAC_GENERAL_FCSI_CTL 0x170 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fcsiMsRstqcD2A : 1; //Central PLL FCSI reset, reset value: 0x0, access type: RW
		uint32 fcsiRfResetQcD2A : 1; //RF PLL FCSI reset, reset value: 0x0, access type: RW
		uint32 fcsiMsForceDefaultcD2A : 1; //Force central PLL to default values, reset value: 0x0, access type: RW
		uint32 reserved0 : 29;
	} bitFields;
} RegMacGeneralFcsiCtl_u;

/*REG_MAC_GENERAL_SPARE_CONFIG_A2D 0x174 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 spareConfigA2D : 8; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 24;
	} bitFields;
} RegMacGeneralSpareConfigA2D_u;

/*REG_MAC_GENERAL_SPARE_CONFIG_D2A 0x178 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 spareConfigD2A : 8; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 24;
	} bitFields;
} RegMacGeneralSpareConfigD2A_u;

/*REG_MAC_GENERAL_CLIENT_0_SMPHR_TOGGLE_31TO00 0x180 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 client0SmphrToggle31To00 : 32; //Client 0 Semaphore Toggle of resources 31:0, reset value: 0x0, access type: WO
	} bitFields;
} RegMacGeneralClient0SmphrToggle31To00_u;

/*REG_MAC_GENERAL_CLIENT_0_SMPHR_TOGGLE_63TO32 0x184 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 client0SmphrToggle63To32 : 32; //Client 0 Semaphore Toggle of resources 63:32, reset value: 0x0, access type: WO
	} bitFields;
} RegMacGeneralClient0SmphrToggle63To32_u;

/*REG_MAC_GENERAL_CLIENT_1_SMPHR_TOGGLE_31TO00 0x188 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 client1SmphrToggle31To00 : 32; //Client 1 Semaphore Toggle of resources 31:0, reset value: 0x0, access type: WO
	} bitFields;
} RegMacGeneralClient1SmphrToggle31To00_u;

/*REG_MAC_GENERAL_CLIENT_1_SMPHR_TOGGLE_63TO32 0x18C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 client1SmphrToggle63To32 : 32; //Client 1 Semaphore Toggle of resources 63:32, reset value: 0x0, access type: WO
	} bitFields;
} RegMacGeneralClient1SmphrToggle63To32_u;

/*REG_MAC_GENERAL_CLIENT_2_SMPHR_TOGGLE_31TO00 0x190 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 client2SmphrToggle31To00 : 32; //Client 2 Semaphore Toggle of resources 31:0, reset value: 0x0, access type: WO
	} bitFields;
} RegMacGeneralClient2SmphrToggle31To00_u;

/*REG_MAC_GENERAL_CLIENT_2_SMPHR_TOGGLE_63TO32 0x194 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 client2SmphrToggle63To32 : 32; //Client 2 Semaphore Toggle of resources 63:32, reset value: 0x0, access type: WO
	} bitFields;
} RegMacGeneralClient2SmphrToggle63To32_u;

/*REG_MAC_GENERAL_CLIENT_0_SMPHR_STAT_15TO00 0x198 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 client0SmphrStat15To00 : 32; //Client 0 {Pending, Taken} tuples of resources 15:0, reset value: 0x0, access type: RO
	} bitFields;
} RegMacGeneralClient0SmphrStat15To00_u;

/*REG_MAC_GENERAL_CLIENT_0_SMPHR_STAT_31TO16 0x19C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 client0SmphrStat31To16 : 32; //Client 0 {Pending, Taken} tuples of resources 31:16, reset value: 0x0, access type: RO
	} bitFields;
} RegMacGeneralClient0SmphrStat31To16_u;

/*REG_MAC_GENERAL_CLIENT_0_SMPHR_STAT_47TO32 0x1A0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 client0SmphrStat47To32 : 32; //Client 0 {Pending, Taken} tuples of resources 47:32, reset value: 0x0, access type: RO
	} bitFields;
} RegMacGeneralClient0SmphrStat47To32_u;

/*REG_MAC_GENERAL_CLIENT_0_SMPHR_STAT_63TO48 0x1A4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 client0SmphrStat63To48 : 32; //Client 0 {Pending, Taken} tuples of resources 63:48, reset value: 0x0, access type: RO
	} bitFields;
} RegMacGeneralClient0SmphrStat63To48_u;

/*REG_MAC_GENERAL_CLIENT_1_SMPHR_STAT_15TO00 0x1A8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 client1SmphrStat15To00 : 32; //Client 1 {Pending, Taken} tuples of resources 15:0, reset value: 0x0, access type: RO
	} bitFields;
} RegMacGeneralClient1SmphrStat15To00_u;

/*REG_MAC_GENERAL_CLIENT_1_SMPHR_STAT_31TO16 0x1AC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 client1SmphrStat31To16 : 32; //Client 1 {Pending, Taken} tuples of resources 31:16, reset value: 0x0, access type: RO
	} bitFields;
} RegMacGeneralClient1SmphrStat31To16_u;

/*REG_MAC_GENERAL_CLIENT_1_SMPHR_STAT_47TO32 0x1B0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 client1SmphrStat47To32 : 32; //Client 1 {Pending, Taken} tuples of resources 47:32, reset value: 0x0, access type: RO
	} bitFields;
} RegMacGeneralClient1SmphrStat47To32_u;

/*REG_MAC_GENERAL_CLIENT_1_SMPHR_STAT_63TO48 0x1B4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 client1SmphrStat63To48 : 32; //Client 1 {Pending, Taken} tuples of resources 63:48, reset value: 0x0, access type: RO
	} bitFields;
} RegMacGeneralClient1SmphrStat63To48_u;

/*REG_MAC_GENERAL_CLIENT_2_SMPHR_STAT_15TO00 0x1B8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 client2SmphrStat15To00 : 32; //Client 2 {Pending, Taken} tuples of resources 15:0, reset value: 0x0, access type: RO
	} bitFields;
} RegMacGeneralClient2SmphrStat15To00_u;

/*REG_MAC_GENERAL_CLIENT_2_SMPHR_STAT_31TO16 0x1BC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 client2SmphrStat31To16 : 32; //Client 2 {Pending, Taken} tuples of resources 31:16, reset value: 0x0, access type: RO
	} bitFields;
} RegMacGeneralClient2SmphrStat31To16_u;

/*REG_MAC_GENERAL_CLIENT_2_SMPHR_STAT_47TO32 0x1C0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 client2SmphrStat47To32 : 32; //Client 2 {Pending, Taken} tuples of resources 47:32, reset value: 0x0, access type: RO
	} bitFields;
} RegMacGeneralClient2SmphrStat47To32_u;

/*REG_MAC_GENERAL_CLIENT_2_SMPHR_STAT_63TO48 0x1C4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 client2SmphrStat63To48 : 32; //Client 2 {Pending, Taken} tuples of resources 63:48, reset value: 0x0, access type: RO
	} bitFields;
} RegMacGeneralClient2SmphrStat63To48_u;

/*REG_MAC_GENERAL_LIB_DPL_IDX_SOURCE0_3 0x1C8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 liberatorDplIdxSource0 : 6; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 liberatorDplIdxSource1 : 6; //no description, reset value: 0x1, access type: RW
		uint32 reserved1 : 2;
		uint32 liberatorDplIdxSource2 : 6; //no description, reset value: 0x2, access type: RW
		uint32 reserved2 : 2;
		uint32 liberatorDplIdxSource3 : 6; //no description, reset value: 0x3, access type: RW
		uint32 reserved3 : 2;
	} bitFields;
} RegMacGeneralLibDplIdxSource03_u;

/*REG_MAC_GENERAL_LIB_DPL_IDX_SOURCE4_7 0x1CC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 liberatorDplIdxSource4 : 6; //no description, reset value: 0x4, access type: RW
		uint32 reserved0 : 2;
		uint32 liberatorDplIdxSource5 : 6; //no description, reset value: 0x5, access type: RW
		uint32 reserved1 : 2;
		uint32 liberatorDplIdxSource6 : 6; //no description, reset value: 0x6, access type: RW
		uint32 reserved2 : 2;
		uint32 liberatorDplIdxSource7 : 6; //no description, reset value: 0x7, access type: RW
		uint32 reserved3 : 2;
	} bitFields;
} RegMacGeneralLibDplIdxSource47_u;

/*REG_MAC_GENERAL_LIB_DPL_IDX_INPUT 0x1D0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 liberatorDplIdxIn : 6; //no description, reset value: 0x9, access type: RW
		uint32 reserved0 : 26;
	} bitFields;
} RegMacGeneralLibDplIdxInput_u;

/*REG_MAC_GENERAL_LIB_HALT 0x1D4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 liberatorHaltWork : 1; //no description, reset value: 0x1, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegMacGeneralLibHalt_u;

/*REG_MAC_GENERAL_RX_LIB_DPL_IDX_SOURCE0_3 0x1D8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxLiberatorDplIdxSource0 : 6; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 rxLiberatorDplIdxSource1 : 6; //no description, reset value: 0x1, access type: RW
		uint32 reserved1 : 2;
		uint32 rxLiberatorDplIdxSource2 : 6; //no description, reset value: 0x2, access type: RW
		uint32 reserved2 : 2;
		uint32 rxLiberatorDplIdxSource3 : 6; //no description, reset value: 0x3, access type: RW
		uint32 reserved3 : 2;
	} bitFields;
} RegMacGeneralRxLibDplIdxSource03_u;

/*REG_MAC_GENERAL_RX_LIB_DPL_IDX_SOURCE4_7 0x1DC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxLiberatorDplIdxSource4 : 6; //no description, reset value: 0x4, access type: RW
		uint32 reserved0 : 2;
		uint32 rxLiberatorDplIdxSource5 : 6; //no description, reset value: 0x5, access type: RW
		uint32 reserved1 : 2;
		uint32 rxLiberatorDplIdxSource6 : 6; //no description, reset value: 0x6, access type: RW
		uint32 reserved2 : 2;
		uint32 rxLiberatorDplIdxSource7 : 6; //no description, reset value: 0x7, access type: RW
		uint32 reserved3 : 2;
	} bitFields;
} RegMacGeneralRxLibDplIdxSource47_u;

/*REG_MAC_GENERAL_RX_LIB_DPL_IDX_INPUT 0x1E0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxLiberatorDplIdxIn : 6; //no description, reset value: 0x9, access type: RW
		uint32 reserved0 : 2;
		uint32 rxLiberatorDplIdxToAdd : 6; //no description, reset value: 0x7, access type: RW
		uint32 reserved1 : 18;
	} bitFields;
} RegMacGeneralRxLibDplIdxInput_u;

/*REG_MAC_GENERAL_RX_LIB_HALT 0x1E4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxLiberatorHaltWork : 1; //no description, reset value: 0x1, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegMacGeneralRxLibHalt_u;

/*REG_MAC_GENERAL_LIBERATOR_MAX_MESSAGE_TO_LOGGER 0x1E8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 liberatorMaxMessageToLogger : 8; //no description, reset value: 0x20, access type: RW
		uint32 reserved0 : 24;
	} bitFields;
} RegMacGeneralLiberatorMaxMessageToLogger_u;

/*REG_MAC_GENERAL_LIBERATOR_DELAY_VALUE_TO_START 0x1EC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 liberatorDelayValueToStart : 32; //no description, reset value: 0x0, access type: RW
	} bitFields;
} RegMacGeneralLiberatorDelayValueToStart_u;

/*REG_MAC_GENERAL_RX_LIBERATOR_DELAY_VALUE_TO_START 0x1F0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxLiberatorDelayValueToStart : 32; //no description, reset value: 0x0, access type: RW
	} bitFields;
} RegMacGeneralRxLiberatorDelayValueToStart_u;

/*REG_MAC_GENERAL_LIBERATOR_DEBUG 0x1F4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txLiberatorWorking : 1; //no description, reset value: 0x0, access type: RO
		uint32 rxLiberatorWorking : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 30;
	} bitFields;
} RegMacGeneralLiberatorDebug_u;

/*REG_MAC_GENERAL_LIBERATOR_LOGGER_REG 0x1F8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 liberatorLoggerRegEn : 1; //no description, reset value: 0x0, access type: RW
		uint32 liberatorLoggerRegPriority : 2; //no description, reset value: 0x0, access type: RW
		uint32 liberatorLoggerRegActive : 1; //no description, reset value: 0x0, access type: RO
		uint32 rxLiberatorLoggerRegEn : 1; //no description, reset value: 0x0, access type: RW
		uint32 rxLiberatorLoggerRegPriority : 2; //no description, reset value: 0x0, access type: RW
		uint32 rxLiberatorLoggerRegActive : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 24;
	} bitFields;
} RegMacGeneralLiberatorLoggerReg_u;

/*REG_MAC_GENERAL_PHY_BAND_CONFIG 0x1FC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 b0PhyActive : 1; //Band0's Phy active, reset value: 0x0, access type: RW
		uint32 b1PhyActive : 1; //Band 1's phy active, reset value: 0x0, access type: RW
		uint32 fcsiCenResetN : 1; //Central FCSI reset, reset value: 0x0, access type: RW
		uint32 fcsiCenBlockEn : 1; //Central FCSI block enable, reset value: 0x0, access type: RW
		uint32 phyAntenna0Sel : 1; //Antenna 0's band assignment, reset value: 0x0, access type: RW
		uint32 phyAntenna1Sel : 1; //Antenna 1's band assignment, reset value: 0x0, access type: RW
		uint32 phyAntenna2Sel : 1; //Antenna 2's band assignment, reset value: 0x0, access type: RW
		uint32 phyAntenna3Sel : 1; //Antenna 3's band assignment, reset value: 0x0, access type: RW
		uint32 reserved0 : 24;
	} bitFields;
} RegMacGeneralPhyBandConfig_u;



#endif // _MAC_GENERAL_REGS_H_
