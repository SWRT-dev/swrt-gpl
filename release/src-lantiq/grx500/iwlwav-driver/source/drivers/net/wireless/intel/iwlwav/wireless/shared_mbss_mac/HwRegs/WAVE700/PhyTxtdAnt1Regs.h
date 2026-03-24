
/***********************************************************************************
File:				PhyTxtdAnt1Regs.h
Module:				phyTxtdAnt1
SOC Revision:		
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _PHY_TXTD_ANT1_REGS_H_
#define _PHY_TXTD_ANT1_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define PHY_TXTD_ANT1_BASE_ADDRESS                             MEMORY_MAP_UNIT_91_BASE_ADDRESS
#define	REG_PHY_TXTD_ANT1_TX_MODE_11B_EN                (PHY_TXTD_ANT1_BASE_ADDRESS + 0x0)
#define	REG_PHY_TXTD_ANT1_TX_DC_OFFSET_0                (PHY_TXTD_ANT1_BASE_ADDRESS + 0x8)
#define	REG_PHY_TXTD_ANT1_TX_DC_OFFSET_1                (PHY_TXTD_ANT1_BASE_ADDRESS + 0xC)
#define	REG_PHY_TXTD_ANT1_TX_DC_OFFSET_2                (PHY_TXTD_ANT1_BASE_ADDRESS + 0x10)
#define	REG_PHY_TXTD_ANT1_TX_DC_OFFSET_3                (PHY_TXTD_ANT1_BASE_ADDRESS + 0x14)
#define	REG_PHY_TXTD_ANT1_TXTD_TEST_PLUG_MUX_CONTROL    (PHY_TXTD_ANT1_BASE_ADDRESS + 0x18)
#define	REG_PHY_TXTD_ANT1_TX_ANT_RAM_RM                 (PHY_TXTD_ANT1_BASE_ADDRESS + 0x1C)
#define	REG_PHY_TXTD_ANT1_TXTD_ANT_SPARE                (PHY_TXTD_ANT1_BASE_ADDRESS + 0x20)
#define	REG_PHY_TXTD_ANT1_TXTD_ANT_BYPASS_REG           (PHY_TXTD_ANT1_BASE_ADDRESS + 0x24)
#define	REG_PHY_TXTD_ANT1_TXTD_WINDOWING                (PHY_TXTD_ANT1_BASE_ADDRESS + 0x28)
#define	REG_PHY_TXTD_ANT1_TXTD_DUP_SWAP_FLIP            (PHY_TXTD_ANT1_BASE_ADDRESS + 0x2C)
#define	REG_PHY_TXTD_ANT1_TXTD_MIX11B_FREQ_OFFSET       (PHY_TXTD_ANT1_BASE_ADDRESS + 0x30)
#define	REG_PHY_TXTD_ANT1_TXTD_UPS8_COEFF_0             (PHY_TXTD_ANT1_BASE_ADDRESS + 0x34)
#define	REG_PHY_TXTD_ANT1_TXTD_UPS8_COEFF_1             (PHY_TXTD_ANT1_BASE_ADDRESS + 0x38)
#define	REG_PHY_TXTD_ANT1_TXTD_UPS8_COEFF_2             (PHY_TXTD_ANT1_BASE_ADDRESS + 0x3C)
#define	REG_PHY_TXTD_ANT1_TXTD_UPS8_COEFF_3             (PHY_TXTD_ANT1_BASE_ADDRESS + 0x40)
#define	REG_PHY_TXTD_ANT1_TXTD_UPS8_COEFF_4             (PHY_TXTD_ANT1_BASE_ADDRESS + 0x44)
#define	REG_PHY_TXTD_ANT1_TXTD_UPS8_COEFF_5             (PHY_TXTD_ANT1_BASE_ADDRESS + 0x48)
#define	REG_PHY_TXTD_ANT1_TXTD_UPS8_COEFF_6             (PHY_TXTD_ANT1_BASE_ADDRESS + 0x4C)
#define	REG_PHY_TXTD_ANT1_TXTD_UPS8_COEFF_7             (PHY_TXTD_ANT1_BASE_ADDRESS + 0x50)
#define	REG_PHY_TXTD_ANT1_TXTD_MEM_GLOBAL_RM            (PHY_TXTD_ANT1_BASE_ADDRESS + 0x54)
#define	REG_PHY_TXTD_ANT1_TXTD_MEM_GLOBAL_TEST_MODE     (PHY_TXTD_ANT1_BASE_ADDRESS + 0x58)
#define	REG_PHY_TXTD_ANT1_TXTD_SPARE                    (PHY_TXTD_ANT1_BASE_ADDRESS + 0x5C)
#define	REG_PHY_TXTD_ANT1_TXTD_AFE_IDLE_I_DATA          (PHY_TXTD_ANT1_BASE_ADDRESS + 0x60)
#define	REG_PHY_TXTD_ANT1_TXTD_AFE_IDLE_Q_DATA          (PHY_TXTD_ANT1_BASE_ADDRESS + 0x64)
#define	REG_PHY_TXTD_ANT1_TXTD_BIST                     (PHY_TXTD_ANT1_BASE_ADDRESS + 0x68)
#define	REG_PHY_TXTD_ANT1_TX_IQ_RIPPLE                  (PHY_TXTD_ANT1_BASE_ADDRESS + 0x6C)
#define	REG_PHY_TXTD_ANT1_TXTD_IFFT_IN_OUT_SHIFT        (PHY_TXTD_ANT1_BASE_ADDRESS + 0x70)
#define	REG_PHY_TXTD_ANT1_TXTD_END_ZEROS                (PHY_TXTD_ANT1_BASE_ADDRESS + 0x74)
#define	REG_PHY_TXTD_ANT1_TX_ABB_30                     (PHY_TXTD_ANT1_BASE_ADDRESS + 0x78)
#define	REG_PHY_TXTD_ANT1_TX_ANT_REG_31                 (PHY_TXTD_ANT1_BASE_ADDRESS + 0x7C)
#define	REG_PHY_TXTD_ANT1_TX_BF_CAL_GAIN_INDEX_ANT      (PHY_TXTD_ANT1_BASE_ADDRESS + 0x80)
#define	REG_PHY_TXTD_ANT1_TPC_ANT_RAM                   (PHY_TXTD_ANT1_BASE_ADDRESS + 0x84)
#define	REG_PHY_TXTD_ANT1_TPC_ACCELERATOR               (PHY_TXTD_ANT1_BASE_ADDRESS + 0x88)
#define	REG_PHY_TXTD_ANT1_TPC_TSSI_A_B                  (PHY_TXTD_ANT1_BASE_ADDRESS + 0x8C)
#define	REG_PHY_TXTD_ANT1_TPC_TSSI_C                    (PHY_TXTD_ANT1_BASE_ADDRESS + 0x90)
#define	REG_PHY_TXTD_ANT1_TPC_SHORT                     (PHY_TXTD_ANT1_BASE_ADDRESS + 0x94)
#define	REG_PHY_TXTD_ANT1_RF_POWER                      (PHY_TXTD_ANT1_BASE_ADDRESS + 0x98)
#define	REG_PHY_TXTD_ANT1_TX_DIGITAL_GAIN               (PHY_TXTD_ANT1_BASE_ADDRESS + 0x9C)
#define	REG_PHY_TXTD_ANT1_TX_PGC2_GAIN_SELECT_OUT       (PHY_TXTD_ANT1_BASE_ADDRESS + 0xA0)
#define	REG_PHY_TXTD_ANT1_RF_POWER_OFFSET               (PHY_TXTD_ANT1_BASE_ADDRESS + 0xA4)
#define	REG_PHY_TXTD_ANT1_TXTD_11B_IQ_0                 (PHY_TXTD_ANT1_BASE_ADDRESS + 0xA8)
#define	REG_PHY_TXTD_ANT1_TXTD_11B_IQ_1                 (PHY_TXTD_ANT1_BASE_ADDRESS + 0xAC)
#define	REG_PHY_TXTD_ANT1_STREAMER                      (PHY_TXTD_ANT1_BASE_ADDRESS + 0xB0)
#define	REG_PHY_TXTD_ANT1_TEST_PLUG_LEN                 (PHY_TXTD_ANT1_BASE_ADDRESS + 0xB4)
#define	REG_PHY_TXTD_ANT1_TXTD_UPS_COEFF_0              (PHY_TXTD_ANT1_BASE_ADDRESS + 0xB8)
#define	REG_PHY_TXTD_ANT1_TXTD_UPS_COEFF_2              (PHY_TXTD_ANT1_BASE_ADDRESS + 0xBC)
#define	REG_PHY_TXTD_ANT1_TXTD_UPSRF_COEFF_0            (PHY_TXTD_ANT1_BASE_ADDRESS + 0xC0)
#define	REG_PHY_TXTD_ANT1_PM_ANALOG_LPBK_EN             (PHY_TXTD_ANT1_BASE_ADDRESS + 0xC4)
#define	REG_PHY_TXTD_ANT1_TXTD_GCLK_EN                  (PHY_TXTD_ANT1_BASE_ADDRESS + 0xC8)
#define	REG_PHY_TXTD_ANT1_TONE_GEN_BW                   (PHY_TXTD_ANT1_BASE_ADDRESS + 0xCC)
#define	REG_PHY_TXTD_ANT1_TX_SC2FC_TXON                 (PHY_TXTD_ANT1_BASE_ADDRESS + 0xD0)
#define	REG_PHY_TXTD_ANT1_TXTD_BYPASS_SEL               (PHY_TXTD_ANT1_BASE_ADDRESS + 0xD4)
#define	REG_PHY_TXTD_ANT1_TX_PPM_FREQ_IN_CFG            (PHY_TXTD_ANT1_BASE_ADDRESS + 0xD8)
#define	REG_PHY_TXTD_ANT1_TX_PM_FDL_NCO_UP_LIM_CFG      (PHY_TXTD_ANT1_BASE_ADDRESS + 0xE0)
#define	REG_PHY_TXTD_ANT1_TX_PM_FDL_NCO_INCR_MAX_CFG    (PHY_TXTD_ANT1_BASE_ADDRESS + 0xE4)
#define	REG_PHY_TXTD_ANT1_TX_PM_FDL_NCO_INCR_CFG        (PHY_TXTD_ANT1_BASE_ADDRESS + 0xE8)
#define	REG_PHY_TXTD_ANT1_PM_DPD_CFG_SEL_DPD_PHY_I      (PHY_TXTD_ANT1_BASE_ADDRESS + 0xEC)
#define	REG_PHY_TXTD_ANT1_PM_DPD_STATUS_DBG_RDY_O       (PHY_TXTD_ANT1_BASE_ADDRESS + 0xF0)
#define	REG_PHY_TXTD_ANT1_PM_DPD_DBG_CTL_RUN_I          (PHY_TXTD_ANT1_BASE_ADDRESS + 0xF4)
#define	REG_PHY_TXTD_ANT1_PM_DPD_DBG_RES_O_0            (PHY_TXTD_ANT1_BASE_ADDRESS + 0xF8)
#define	REG_PHY_TXTD_ANT1_PM_DPD_DBG_RES_O_1            (PHY_TXTD_ANT1_BASE_ADDRESS + 0xFC)
#define	REG_PHY_TXTD_ANT1_PM_DPD_DBG_RES_O_2            (PHY_TXTD_ANT1_BASE_ADDRESS + 0x100)
#define	REG_PHY_TXTD_ANT1_PM_DPD_DBG_RES_O_3            (PHY_TXTD_ANT1_BASE_ADDRESS + 0x104)
#define	REG_PHY_TXTD_ANT1_PM_DPD_DBG_RES_O_4            (PHY_TXTD_ANT1_BASE_ADDRESS + 0x108)
#define	REG_PHY_TXTD_ANT1_PM_DPD_MEM_EQ_0_0_1           (PHY_TXTD_ANT1_BASE_ADDRESS + 0x10C)
#define	REG_PHY_TXTD_ANT1_PM_DPD_MEM_EQ_0_2_3           (PHY_TXTD_ANT1_BASE_ADDRESS + 0x110)
#define	REG_PHY_TXTD_ANT1_PM_DPD_MEM_EQ_0_4_5           (PHY_TXTD_ANT1_BASE_ADDRESS + 0x114)
#define	REG_PHY_TXTD_ANT1_PM_DPD_MEM_EQ_0_6_7           (PHY_TXTD_ANT1_BASE_ADDRESS + 0x118)
#define	REG_PHY_TXTD_ANT1_PM_DPD_MEM_EQ_0_8             (PHY_TXTD_ANT1_BASE_ADDRESS + 0x11C)
#define	REG_PHY_TXTD_ANT1_PM_DPD_MEM_EQ_1_0_1           (PHY_TXTD_ANT1_BASE_ADDRESS + 0x120)
#define	REG_PHY_TXTD_ANT1_PM_DPD_MEM_EQ_1_2_3           (PHY_TXTD_ANT1_BASE_ADDRESS + 0x124)
#define	REG_PHY_TXTD_ANT1_PM_DPD_MEM_EQ_1_4_5           (PHY_TXTD_ANT1_BASE_ADDRESS + 0x128)
#define	REG_PHY_TXTD_ANT1_PM_DPD_MEM_EQ_1_6_7           (PHY_TXTD_ANT1_BASE_ADDRESS + 0x12C)
#define	REG_PHY_TXTD_ANT1_PM_DPD_MEM_EQ_1_8             (PHY_TXTD_ANT1_BASE_ADDRESS + 0x130)
#define	REG_PHY_TXTD_ANT1_PM_DPD_MEM_EQ_2_0_1           (PHY_TXTD_ANT1_BASE_ADDRESS + 0x134)
#define	REG_PHY_TXTD_ANT1_PM_DPD_MEM_EQ_2_2_3           (PHY_TXTD_ANT1_BASE_ADDRESS + 0x138)
#define	REG_PHY_TXTD_ANT1_PM_DPD_MEM_EQ_2_4_5           (PHY_TXTD_ANT1_BASE_ADDRESS + 0x13C)
#define	REG_PHY_TXTD_ANT1_PM_DPD_MEM_EQ_2_6_7           (PHY_TXTD_ANT1_BASE_ADDRESS + 0x140)
#define	REG_PHY_TXTD_ANT1_PM_DPD_MEM_EQ_2_8             (PHY_TXTD_ANT1_BASE_ADDRESS + 0x144)
#define	REG_PHY_TXTD_ANT1_PM_DPD_MEM_EQ_3_0_1           (PHY_TXTD_ANT1_BASE_ADDRESS + 0x148)
#define	REG_PHY_TXTD_ANT1_PM_DPD_MEM_EQ_3_2_3           (PHY_TXTD_ANT1_BASE_ADDRESS + 0x14C)
#define	REG_PHY_TXTD_ANT1_PM_DPD_MEM_EQ_3_4_5           (PHY_TXTD_ANT1_BASE_ADDRESS + 0x150)
#define	REG_PHY_TXTD_ANT1_PM_DPD_MEM_EQ_3_6_7           (PHY_TXTD_ANT1_BASE_ADDRESS + 0x154)
#define	REG_PHY_TXTD_ANT1_PM_DPD_MEM_EQ_3_8             (PHY_TXTD_ANT1_BASE_ADDRESS + 0x158)
#define	REG_PHY_TXTD_ANT1_TXTD_ANT_RAM_POWER_SAVE       (PHY_TXTD_ANT1_BASE_ADDRESS + 0x29C)
#define	REG_PHY_TXTD_ANT1_TPC_DPD                       (PHY_TXTD_ANT1_BASE_ADDRESS + 0x2A0)
#define	REG_PHY_TXTD_ANT1_CLIPPER_CONF                  (PHY_TXTD_ANT1_BASE_ADDRESS + 0x2A4)
#define	REG_PHY_TXTD_ANT1_CLIPPER_THRESHOLD             (PHY_TXTD_ANT1_BASE_ADDRESS + 0x2A8)
#define	REG_PHY_TXTD_ANT1_LOOPBACK_PYPASS               (PHY_TXTD_ANT1_BASE_ADDRESS + 0x2AC)
#define	REG_PHY_TXTD_ANT1_TXTD_GCLK_BYPASS              (PHY_TXTD_ANT1_BASE_ADDRESS + 0x2B0)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_PHY_TXTD_ANT1_TX_MODE_11B_EN 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmMode11BEn : 1; //Enable TX 11B Antenna , reset value: 0x1, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyTxtdAnt1TxMode11BEn_u;

/*REG_PHY_TXTD_ANT1_TX_DC_OFFSET_0 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmAfeOffsetI0 : 12; //pm_afe_offset_i0, reset value: 0x0, access type: RW
		uint32 reserved0 : 4;
		uint32 pmAfeOffsetQ0 : 12; //pm_afe_offset_q0, reset value: 0x0, access type: RW
		uint32 reserved1 : 4;
	} bitFields;
} RegPhyTxtdAnt1TxDcOffset0_u;

/*REG_PHY_TXTD_ANT1_TX_DC_OFFSET_1 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmAfeOffsetQ1 : 12; //AFE DC Offset Q0, reset value: 0x0, access type: RW
		uint32 reserved0 : 4;
		uint32 pmAfeOffsetI1 : 12; //AFE DC Offset I0, reset value: 0x0, access type: RW
		uint32 reserved1 : 4;
	} bitFields;
} RegPhyTxtdAnt1TxDcOffset1_u;

/*REG_PHY_TXTD_ANT1_TX_DC_OFFSET_2 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmAfeOffsetQ2 : 12; //AFE DC Offset Q1, reset value: 0x0, access type: RW
		uint32 reserved0 : 4;
		uint32 pmAfeOffsetI2 : 12; //AFE DC Offset I1, reset value: 0x0, access type: RW
		uint32 reserved1 : 4;
	} bitFields;
} RegPhyTxtdAnt1TxDcOffset2_u;

/*REG_PHY_TXTD_ANT1_TX_DC_OFFSET_3 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmAfeOffsetQ3 : 12; //AFE DC Offset Q2, reset value: 0x0, access type: RW
		uint32 reserved0 : 4;
		uint32 pmAfeOffsetI3 : 12; //AFE DC Offset I2, reset value: 0x0, access type: RW
		uint32 reserved1 : 4;
	} bitFields;
} RegPhyTxtdAnt1TxDcOffset3_u;

/*REG_PHY_TXTD_ANT1_TXTD_TEST_PLUG_MUX_CONTROL 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmTxtdTestPlugMuxControl : 3; //Tx TD Test Plug Mux Control, reset value: 0x0, access type: RW
		uint32 reserved0 : 29;
	} bitFields;
} RegPhyTxtdAnt1TxtdTestPlugMuxControl_u;

/*REG_PHY_TXTD_ANT1_TX_ANT_RAM_RM 0x1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmTxwinRamRm : 3; //txwin_ram_rm, reset value: 0x3, access type: RW
		uint32 pmDpdRamRm : 3; //dpd_ram_rm, reset value: 0x3, access type: RW
		uint32 pmRippleRamRm : 3; //ripple_ram_rm, reset value: 0x3, access type: RW
		uint32 pmIqRamRm : 3; //iq_ram_rm, reset value: 0x3, access type: RW
		uint32 reserved0 : 20;
	} bitFields;
} RegPhyTxtdAnt1TxAntRamRm_u;

/*REG_PHY_TXTD_ANT1_TXTD_ANT_SPARE 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmTxtdAntSpare : 32; //Spare register, reset value: 0x0, access type: RW
	} bitFields;
} RegPhyTxtdAnt1TxtdAntSpare_u;

/*REG_PHY_TXTD_ANT1_TXTD_ANT_BYPASS_REG 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmUpsBypass : 1; //UPS 640MHz Up Sampler Bypass, reset value: 0x0, access type: RW
		uint32 reserved0 : 1;
		uint32 pmUpsrfBypass : 1; //pm_upsrf_bypass, reset value: 0x0, access type: RW
		uint32 pmNoishBypass : 1; //pm_noish_bypass, reset value: 0x0, access type: RW
		uint32 reserved1 : 28;
	} bitFields;
} RegPhyTxtdAnt1TxtdAntBypassReg_u;

/*REG_PHY_TXTD_ANT1_TXTD_WINDOWING 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmTxwinTailDis : 1; //Disable TX window last Roll Off, reset value: 0x0, access type: RW
		uint32 pmTxwinEn : 1; //pm_txwin_en, reset value: 0x1, access type: RW
		uint32 pmCddOffsetEn : 1; //pm_cdd_offset_en, reset value: 0x1, access type: RW
		uint32 pmTxwinCp4 : 3; //pm_txwin_cp_4, reset value: 0x1, access type: RW
		uint32 pmTxwinCp8Ss : 3; //pm_txwin_cp_8_ss, reset value: 0x1, access type: RW
		uint32 pmTxwinCp8Ls : 3; //pm_txwin_cp_8_ls, reset value: 0x1, access type: RW
		uint32 pmTxwinCp16 : 3; //pm_txwin_cp_16, reset value: 0x1, access type: RW
		uint32 pmTxwinCp32 : 3; //pm_txwin_cp_32, reset value: 0x1, access type: RW
		uint32 reserved0 : 14;
	} bitFields;
} RegPhyTxtdAnt1TxtdWindowing_u;

/*REG_PHY_TXTD_ANT1_TXTD_DUP_SWAP_FLIP 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmTxDuplicateI : 1; //Enable CDD offset, reset value: 0x0, access type: RW
		uint32 pmTxDuplicateQ : 1; //pm_tx_duplicate_q, reset value: 0x0, access type: RW
		uint32 pmTxFlipI : 1; //pm_tx_flip_i, reset value: 0x0, access type: RW
		uint32 pmTxFlipQ : 1; //pm_tx_flip_q, reset value: 0x0, access type: RW
		uint32 pmTxIqSwap : 1; //pm_tx_iq_swap, reset value: 0x0, access type: RW
		uint32 reserved0 : 27;
	} bitFields;
} RegPhyTxtdAnt1TxtdDupSwapFlip_u;

/*REG_PHY_TXTD_ANT1_TXTD_MIX11B_FREQ_OFFSET 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmMix11BFreqOffset : 3; //11B Mixer Frequency shift: , 0: 0 , 1: +10MHz , 2: +30MHz , 3: -10MHz , 4: -30MHz, reset value: 0x0, access type: RW
		uint32 reserved0 : 29;
	} bitFields;
} RegPhyTxtdAnt1TxtdMix11BFreqOffset_u;

/*REG_PHY_TXTD_ANT1_TXTD_UPS8_COEFF_0 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmUps8Coeff0 : 24; //UPS8 coeff 0 of 8 x 24 bit , reset value: 0x34697F, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegPhyTxtdAnt1TxtdUps8Coeff0_u;

/*REG_PHY_TXTD_ANT1_TXTD_UPS8_COEFF_1 0x38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmUps8Coeff1 : 24; //UPS8 coeff 1 of 8 x 24 bit , reset value: 0xB4719D, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegPhyTxtdAnt1TxtdUps8Coeff1_u;

/*REG_PHY_TXTD_ANT1_TXTD_UPS8_COEFF_2 0x3C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmUps8Coeff2 : 24; //UPS8 coeff 2 of 8 x 24 bit , reset value: 0x4F66FD, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegPhyTxtdAnt1TxtdUps8Coeff2_u;

/*REG_PHY_TXTD_ANT1_TXTD_UPS8_COEFF_3 0x40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmUps8Coeff3 : 24; //UPS8 coeff 3 of 8 x 24 bit , reset value: 0xC4218D, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegPhyTxtdAnt1TxtdUps8Coeff3_u;

/*REG_PHY_TXTD_ANT1_TXTD_UPS8_COEFF_4 0x44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmUps8Coeff4 : 24; //UPS8 coeff 4 of 8 x 24 bit , reset value: 0x6BE5FB, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegPhyTxtdAnt1TxtdUps8Coeff4_u;

/*REG_PHY_TXTD_ANT1_TXTD_UPS8_COEFF_5 0x48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmUps8Coeff5 : 24; //UPS8 coeff 5 of 8 x 24 bit , reset value: 0xD5E36E, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegPhyTxtdAnt1TxtdUps8Coeff5_u;

/*REG_PHY_TXTD_ANT1_TXTD_UPS8_COEFF_6 0x4C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmUps8Coeff6 : 24; //UPS8 coeff 6 of 8 x 24 bit , reset value: 0x886877, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegPhyTxtdAnt1TxtdUps8Coeff6_u;

/*REG_PHY_TXTD_ANT1_TXTD_UPS8_COEFF_7 0x50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmUps8Coeff7 : 24; //UPS8 coeff 7 of 8 x 24 bit , reset value: 0xE3B943, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegPhyTxtdAnt1TxtdUps8Coeff7_u;

/*REG_PHY_TXTD_ANT1_TXTD_MEM_GLOBAL_RM 0x54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmMemGlobalRm : 2; //Global RM value for rams, reset value: 0x0, access type: RW
		uint32 reserved0 : 30;
	} bitFields;
} RegPhyTxtdAnt1TxtdMemGlobalRm_u;

/*REG_PHY_TXTD_ANT1_TXTD_MEM_GLOBAL_TEST_MODE 0x58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmMemGlobalTestMode : 2; //Global test mode for rams, reset value: 0x0, access type: RW
		uint32 reserved0 : 30;
	} bitFields;
} RegPhyTxtdAnt1TxtdMemGlobalTestMode_u;

/*REG_PHY_TXTD_ANT1_TXTD_SPARE 0x5C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmTxtdSpare : 32; //Spare register, reset value: 0x0, access type: RW
	} bitFields;
} RegPhyTxtdAnt1TxtdSpare_u;

/*REG_PHY_TXTD_ANT1_TXTD_AFE_IDLE_I_DATA 0x60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmAfeIdleIData : 24; //AFE idle I data, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegPhyTxtdAnt1TxtdAfeIdleIData_u;

/*REG_PHY_TXTD_ANT1_TXTD_AFE_IDLE_Q_DATA 0x64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmAfeIdleQData : 24; //AFE idle Q data, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegPhyTxtdAnt1TxtdAfeIdleQData_u;

/*REG_PHY_TXTD_ANT1_TXTD_BIST 0x68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmSwBistStart : 1; //sw_bist_start, reset value: 0x0, access type: RW
		uint32 pmClearRamMode : 1; //clear_ram_mode, reset value: 0x0, access type: RW
		uint32 pmBistScrBypass : 1; //bist_scr_bypass, reset value: 0x0, access type: RW
		uint32 reserved0 : 29;
	} bitFields;
} RegPhyTxtdAnt1TxtdBist_u;

/*REG_PHY_TXTD_ANT1_TX_IQ_RIPPLE 0x6C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmIqEquBypass : 1; //iq_equ_bypass, reset value: 0x0, access type: RW
		uint32 pmRippleEquBypass : 1; //ripple_equ_bypass, reset value: 0x0, access type: RW
		uint32 reserved0 : 30;
	} bitFields;
} RegPhyTxtdAnt1TxIqRipple_u;

/*REG_PHY_TXTD_ANT1_TXTD_IFFT_IN_OUT_SHIFT 0x70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmIfftInShift20 : 2; //ifft shift in for bw 20, reset value: 0x2, access type: RW
		uint32 pmIfftInShift40 : 2; //ifft shift in for bw 40, reset value: 0x1, access type: RW
		uint32 pmIfftInShift80 : 2; //ifft shift in for bw 80, reset value: 0x0, access type: RW
		uint32 pmIfftInShift160 : 2; //ifft shift in for bw 160, reset value: 0x0, access type: RW
		uint32 pmIfftOutShift20 : 3; //ifft shift out for bw 20, reset value: 0x0, access type: RW
		uint32 pmIfftOutShift40 : 3; //ifft shift out for bw 40, reset value: 0x0, access type: RW
		uint32 pmIfftOutShift80 : 3; //ifft shift out for bw 80, reset value: 0x1, access type: RW
		uint32 pmIfftOutShift160 : 3; //ifft shift out for bw 160, reset value: 0x1, access type: RW
		uint32 pmIfftInShiftToneGen : 2; //pm_ifft_in_shift_tone_gen, reset value: 0x2, access type: RW
		uint32 pmIfftOutShiftToneGen : 3; //pm_ifft_out_shift_tone_gen, reset value: 0x6, access type: RW
		uint32 reserved0 : 7;
	} bitFields;
} RegPhyTxtdAnt1TxtdIfftInOutShift_u;

/*REG_PHY_TXTD_ANT1_TXTD_END_ZEROS 0x74 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmEndZeros : 8; //pm_end_zeros �, reset value: 0x96, access type: RW
		uint32 reserved0 : 24;
	} bitFields;
} RegPhyTxtdAnt1TxtdEndZeros_u;

/*REG_PHY_TXTD_ANT1_TX_ABB_30 0x78 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSc2FcPhaseRegfile : 2; //tx_sc2fc_phase_regfile, reset value: 0x0, access type: RW
		uint32 reserved0 : 30;
	} bitFields;
} RegPhyTxtdAnt1TxAbb30_u;

/*REG_PHY_TXTD_ANT1_TX_ANT_REG_31 0x7C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txRippleInSsbMode : 1; //tx_ripple_in_ssb_mode, reset value: 0x0, access type: RW
		uint32 reserved0 : 3;
		uint32 txUseSecRippleInHbMode : 1; //tx_use_sec_ripple_in_hb_mode, reset value: 0x0, access type: RW
		uint32 reserved1 : 27;
	} bitFields;
} RegPhyTxtdAnt1TxAntReg31_u;

/*REG_PHY_TXTD_ANT1_TX_BF_CAL_GAIN_INDEX_ANT 0x80 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfCalGainIndexAnt : 3; //bf_cal_gain_index_ant, reset value: 0x0, access type: RW
		uint32 reserved0 : 1;
		uint32 bfCalRm : 3; //bf_cal_rm, reset value: 0x3, access type: RW
		uint32 reserved1 : 25;
	} bitFields;
} RegPhyTxtdAnt1TxBfCalGainIndexAnt_u;

/*REG_PHY_TXTD_ANT1_TPC_ANT_RAM 0x84 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tpcRamRm : 3; //tpc_ram_rm, reset value: 0x3, access type: RW
		uint32 reserved0 : 29;
	} bitFields;
} RegPhyTxtdAnt1TpcAntRam_u;

/*REG_PHY_TXTD_ANT1_TPC_ACCELERATOR 0x88 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tpcaccenable : 1; //tpcaccenable, reset value: 0x0, access type: RW
		uint32 txTpcAccDone : 1; //tx_tpc_acc_done, reset value: 0x0, access type: RO
		uint32 pmTxPgc2GainSelect : 2; //pm_tx_pgc2_gain_select, reset value: 0x0, access type: RW
		uint32 tpcBoostEnMask : 1; //tpc_boost_en_mask, reset value: 0x0, access type: RW
		uint32 txDigitalGainOut : 9; //tx_digital_gain_out, reset value: 0x0, access type: RO
		uint32 reserved0 : 1;
		uint32 pmTxPgc2GainOverride : 1; //pm_tx_pgc2_gain_override, reset value: 0x0, access type: RW
		uint32 tpcGainIndexAnt : 8; //tpc_gain_index_ant, reset value: 0x0, access type: RO
		uint32 phyTxPowerAntLimit : 7; //phy_tx_power_ant_limit, reset value: 0x0, access type: RO
		uint32 tssiOpenLoopEn : 1; //tssi_open_loop_en, reset value: 0x0, access type: RW
	} bitFields;
} RegPhyTxtdAnt1TpcAccelerator_u;

/*REG_PHY_TXTD_ANT1_TPC_TSSI_A_B 0x8C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txTssiA : 16; //tx_tssi_A, reset value: 0x0, access type: RO
		uint32 txTssiB : 16; //tx_tssi_B, reset value: 0x0, access type: RO
	} bitFields;
} RegPhyTxtdAnt1TpcTssiAB_u;

/*REG_PHY_TXTD_ANT1_TPC_TSSI_C 0x90 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txTssiC : 16; //tx_tssi_C, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyTxtdAnt1TpcTssiC_u;

/*REG_PHY_TXTD_ANT1_TPC_SHORT 0x94 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txPgcGain : 2; //tx_pgc_gain, reset value: 0x0, access type: RO
		uint32 txPaDrv : 4; //tx_pa_drv, reset value: 0x0, access type: RO
		uint32 reserved0 : 1;
		uint32 txS2DIndex : 2; //tx_S2D_index, reset value: 0x0, access type: RO
		uint32 txFectl : 6; //tx_fectl, reset value: 0x0, access type: RW
		uint32 txRfOn : 1; //tx_rf_on, reset value: 0x0, access type: RO
		uint32 reserved1 : 16;
	} bitFields;
} RegPhyTxtdAnt1TpcShort_u;

/*REG_PHY_TXTD_ANT1_RF_POWER 0x98 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrRfPower : 8; //tcr_rf_power, reset value: 0x0, access type: RO
		uint32 tcrPhyPowerBoost : 2; //tcr_phy_power_boost, reset value: 0x0, access type: RO
		uint32 reserved0 : 6;
		uint32 phyTxPowerAntBoost : 9; //tx_power from tcr + boost factor, reset value: 0x0, access type: RO
		uint32 reserved1 : 7;
	} bitFields;
} RegPhyTxtdAnt1RfPower_u;

/*REG_PHY_TXTD_ANT1_TX_DIGITAL_GAIN 0x9C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmTxDigitalGain : 9; //pm_tx_digital_gain, reset value: 0x143, access type: RW
		uint32 pmTxGainEn : 1; //pm_tx_gain_en, reset value: 0x1, access type: RW
		uint32 reserved0 : 22;
	} bitFields;
} RegPhyTxtdAnt1TxDigitalGain_u;

/*REG_PHY_TXTD_ANT1_TX_PGC2_GAIN_SELECT_OUT 0xA0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txPgc2GainSelectOut : 2; //tx_pgc2_gain_select_out, reset value: 0x0, access type: RO
		uint32 reserved0 : 30;
	} bitFields;
} RegPhyTxtdAnt1TxPgc2GainSelectOut_u;

/*REG_PHY_TXTD_ANT1_RF_POWER_OFFSET 0xA4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rfPowerOffset20 : 7; //rf_power_offset_20, reset value: 0x0, access type: RW
		uint32 reserved0 : 1;
		uint32 rfPowerOffset40 : 7; //rf_power_offset_40, reset value: 0x0, access type: RW
		uint32 reserved1 : 1;
		uint32 rfPowerOffset80 : 7; //rf_power_offset_80, reset value: 0x0, access type: RW
		uint32 reserved2 : 1;
		uint32 rfPowerOffset16011B : 7; //rf_power_offset_160_11b, reset value: 0x0, access type: RW
		uint32 reserved3 : 1;
	} bitFields;
} RegPhyTxtdAnt1RfPowerOffset_u;

/*REG_PHY_TXTD_ANT1_TXTD_11B_IQ_0 0xA8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmTxIqTdF1 : 10; //pm_tx_iq_td_f1, reset value: 0x0, access type: RW
		uint32 pmTxIqTdF2 : 10; //pm_tx_iq_td_f2, reset value: 0x0, access type: RW
		uint32 pmTxIqTdF3 : 10; //pm_tx_iq_td_f3, reset value: 0x0, access type: RW
		uint32 pmTxIqTdEn : 1; //pm_tx_iq_td_en, reset value: 0x0, access type: RW
		uint32 reserved0 : 1;
	} bitFields;
} RegPhyTxtdAnt1Txtd11BIq0_u;

/*REG_PHY_TXTD_ANT1_TXTD_11B_IQ_1 0xAC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmTxIqTdF4 : 10; //pm_tx_iq_td_f4, reset value: 0x0, access type: RW
		uint32 reserved0 : 22;
	} bitFields;
} RegPhyTxtdAnt1Txtd11BIq1_u;

/*REG_PHY_TXTD_ANT1_STREAMER 0xB0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 streamerEnable : 1; //streamer_enable, reset value: 0x0, access type: RW
		uint32 streamerRate : 1; //0 = 640MHz , 1 = 320MHz, reset value: 0x0, access type: RW
		uint32 reserved0 : 30;
	} bitFields;
} RegPhyTxtdAnt1Streamer_u;

/*REG_PHY_TXTD_ANT1_TEST_PLUG_LEN 0xB4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 testPlugLen : 6; //test_plug_len, reset value: 0x0, access type: RW
		uint32 reserved0 : 26;
	} bitFields;
} RegPhyTxtdAnt1TestPlugLen_u;

/*REG_PHY_TXTD_ANT1_TXTD_UPS_COEFF_0 0xB8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmUpsCoef0 : 7; //UPS 640Msps coeff 0  , reset value: 0x43, access type: RW
		uint32 reserved0 : 9;
		uint32 pmUpsCoef1 : 10; //UPS 640Msps coeff 1  , reset value: 0x151, access type: RW
		uint32 reserved1 : 6;
	} bitFields;
} RegPhyTxtdAnt1TxtdUpsCoeff0_u;

/*REG_PHY_TXTD_ANT1_TXTD_UPS_COEFF_2 0xBC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmUpsCoef2 : 12; //UPS 640Msps coeff 2  , reset value: 0xB5F, access type: RW
		uint32 reserved0 : 4;
		uint32 pmUpsCoef3 : 13; //UPS 640Msps coeff 3  , reset value: 0x138B, access type: RW
		uint32 reserved1 : 3;
	} bitFields;
} RegPhyTxtdAnt1TxtdUpsCoeff2_u;

/*REG_PHY_TXTD_ANT1_TXTD_UPSRF_COEFF_0 0xC0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmUpsrfCoef0 : 2; //UPSRF 1280Msps coeff 0  , reset value: 0x2, access type: RW
		uint32 reserved0 : 14;
		uint32 pmUpsrfCoef1 : 4; //UPSRF 1280Msps coeff 1  , reset value: 0x9, access type: RW
		uint32 reserved1 : 12;
	} bitFields;
} RegPhyTxtdAnt1TxtdUpsrfCoeff0_u;

/*REG_PHY_TXTD_ANT1_PM_ANALOG_LPBK_EN 0xC4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmAnalogLpbkEn : 2; //0-no loop, 1-loop same ant, 2-loop from 5th ant (relevant only to ant0), reset value: 0x0, access type: RW
		uint32 reserved0 : 30;
	} bitFields;
} RegPhyTxtdAnt1PmAnalogLpbkEn_u;

/*REG_PHY_TXTD_ANT1_TXTD_GCLK_EN 0xC8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txtdPmGclkEn : 6; //txtd_pm_gclk_en, reset value: 0x0, access type: RO
		uint32 reserved0 : 26;
	} bitFields;
} RegPhyTxtdAnt1TxtdGclkEn_u;

/*REG_PHY_TXTD_ANT1_TONE_GEN_BW 0xCC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmToneGenBw : 2; //pm_tone_gen_bw, reset value: 0x0, access type: RW
		uint32 reserved0 : 30;
	} bitFields;
} RegPhyTxtdAnt1ToneGenBw_u;

/*REG_PHY_TXTD_ANT1_TX_SC2FC_TXON 0xD0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSc2FcTxon : 2; //tx_sc2fc_txon, reset value: 0x0, access type: RW
		uint32 reserved0 : 30;
	} bitFields;
} RegPhyTxtdAnt1TxSc2FcTxon_u;

/*REG_PHY_TXTD_ANT1_TXTD_BYPASS_SEL 0xD4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmTxtdBypassSel : 1; //pm_txtd_bypass_sel, reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyTxtdAnt1TxtdBypassSel_u;

/*REG_PHY_TXTD_ANT1_TX_PPM_FREQ_IN_CFG 0xD8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txPpmFreqInCfg : 20; //ppm frequency offset calculated in last reception. This value is read by genrisc from RXTD and written here. It used for NCO phase address calculation., reset value: 0x0, access type: RW
		uint32 reserved0 : 12;
	} bitFields;
} RegPhyTxtdAnt1TxPpmFreqInCfg_u;

/*REG_PHY_TXTD_ANT1_TX_PM_FDL_NCO_UP_LIM_CFG 0xE0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txPmFdlNcoUpLimCfg : 30; //nco counter uper limit initial value used when PPM >0, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegPhyTxtdAnt1TxPmFdlNcoUpLimCfg_u;

/*REG_PHY_TXTD_ANT1_TX_PM_FDL_NCO_INCR_MAX_CFG 0xE4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txPmFdlNcoIncrMaxCfg : 32; //fdl nco incr max value. Above it saturation bit is asserted, reset value: 0x0, access type: RW
	} bitFields;
} RegPhyTxtdAnt1TxPmFdlNcoIncrMaxCfg_u;

/*REG_PHY_TXTD_ANT1_TX_PM_FDL_NCO_INCR_CFG 0xE8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txPmFdlNcoIncrCfg : 32; //fdl nco incr value. Calculated in last reception. Genrisc read this value from Rx and writes it here., reset value: 0x0, access type: RW
	} bitFields;
} RegPhyTxtdAnt1TxPmFdlNcoIncrCfg_u;

/*REG_PHY_TXTD_ANT1_PM_DPD_CFG_SEL_DPD_PHY_I 0xEC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmDpdCfgSelDpdApiI : 6; //no description, reset value: 0x0, access type: RW
		uint32 pmDpdCfgPhyCtlI : 1; //no description, reset value: 0x0, access type: RW
		uint32 pmDpdCfgGlobalEnI : 1; //no description, reset value: 0x0, access type: RW
		uint32 pmDpdCfgEqEnI : 1; //no description, reset value: 0x0, access type: RW
		uint32 pmDpdCfgEqSelTapI : 1; //no description, reset value: 0x0, access type: RW
		uint32 pmDpdCfgEqSelMemI : 2; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 20;
	} bitFields;
} RegPhyTxtdAnt1PmDpdCfgSelDpdPhyI_u;

/*REG_PHY_TXTD_ANT1_PM_DPD_STATUS_DBG_RDY_O 0xF0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmDpdStatusDbgRdyO : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyTxtdAnt1PmDpdStatusDbgRdyO_u;

/*REG_PHY_TXTD_ANT1_PM_DPD_DBG_CTL_RUN_I 0xF4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmDpdDbgCtlRunI : 1; //no description, reset value: 0x0, access type: WO
		uint32 pmDpdDbgCtlCmdI : 3; //no description, reset value: 0x0, access type: RW
		uint32 pmDpdDbgCtlArgsI : 28; //no description, reset value: 0x0, access type: RW
	} bitFields;
} RegPhyTxtdAnt1PmDpdDbgCtlRunI_u;

/*REG_PHY_TXTD_ANT1_PM_DPD_DBG_RES_O_0 0xF8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmDpdDbgResO0 : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegPhyTxtdAnt1PmDpdDbgResO0_u;

/*REG_PHY_TXTD_ANT1_PM_DPD_DBG_RES_O_1 0xFC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmDpdDbgResO1 : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegPhyTxtdAnt1PmDpdDbgResO1_u;

/*REG_PHY_TXTD_ANT1_PM_DPD_DBG_RES_O_2 0x100 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmDpdDbgResO2 : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegPhyTxtdAnt1PmDpdDbgResO2_u;

/*REG_PHY_TXTD_ANT1_PM_DPD_DBG_RES_O_3 0x104 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmDpdDbgResO3 : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegPhyTxtdAnt1PmDpdDbgResO3_u;

/*REG_PHY_TXTD_ANT1_PM_DPD_DBG_RES_O_4 0x108 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmDpdDbgResO4 : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegPhyTxtdAnt1PmDpdDbgResO4_u;

/*REG_PHY_TXTD_ANT1_PM_DPD_MEM_EQ_0_0_1 0x10C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmDpdMemEq00 : 12; //no description, reset value: 0x0, access type: RW
		uint32 pmDpdMemEq01 : 12; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegPhyTxtdAnt1PmDpdMemEq001_u;

/*REG_PHY_TXTD_ANT1_PM_DPD_MEM_EQ_0_2_3 0x110 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmDpdMemEq02 : 12; //no description, reset value: 0x0, access type: RW
		uint32 pmDpdMemEq03 : 12; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegPhyTxtdAnt1PmDpdMemEq023_u;

/*REG_PHY_TXTD_ANT1_PM_DPD_MEM_EQ_0_4_5 0x114 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmDpdMemEq04 : 12; //no description, reset value: 0x0, access type: RW
		uint32 pmDpdMemEq05 : 12; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegPhyTxtdAnt1PmDpdMemEq045_u;

/*REG_PHY_TXTD_ANT1_PM_DPD_MEM_EQ_0_6_7 0x118 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmDpdMemEq06 : 12; //no description, reset value: 0x0, access type: RW
		uint32 pmDpdMemEq07 : 12; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegPhyTxtdAnt1PmDpdMemEq067_u;

/*REG_PHY_TXTD_ANT1_PM_DPD_MEM_EQ_0_8 0x11C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmDpdMemEq08 : 12; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 20;
	} bitFields;
} RegPhyTxtdAnt1PmDpdMemEq08_u;

/*REG_PHY_TXTD_ANT1_PM_DPD_MEM_EQ_1_0_1 0x120 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmDpdMemEq10 : 12; //no description, reset value: 0x0, access type: RW
		uint32 pmDpdMemEq11 : 12; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegPhyTxtdAnt1PmDpdMemEq101_u;

/*REG_PHY_TXTD_ANT1_PM_DPD_MEM_EQ_1_2_3 0x124 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmDpdMemEq12 : 12; //no description, reset value: 0x0, access type: RW
		uint32 pmDpdMemEq13 : 12; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegPhyTxtdAnt1PmDpdMemEq123_u;

/*REG_PHY_TXTD_ANT1_PM_DPD_MEM_EQ_1_4_5 0x128 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmDpdMemEq14 : 12; //no description, reset value: 0x0, access type: RW
		uint32 pmDpdMemEq15 : 12; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegPhyTxtdAnt1PmDpdMemEq145_u;

/*REG_PHY_TXTD_ANT1_PM_DPD_MEM_EQ_1_6_7 0x12C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmDpdMemEq16 : 12; //no description, reset value: 0x0, access type: RW
		uint32 pmDpdMemEq17 : 12; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegPhyTxtdAnt1PmDpdMemEq167_u;

/*REG_PHY_TXTD_ANT1_PM_DPD_MEM_EQ_1_8 0x130 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmDpdMemEq18 : 12; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 20;
	} bitFields;
} RegPhyTxtdAnt1PmDpdMemEq18_u;

/*REG_PHY_TXTD_ANT1_PM_DPD_MEM_EQ_2_0_1 0x134 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmDpdMemEq20 : 12; //no description, reset value: 0x0, access type: RW
		uint32 pmDpdMemEq21 : 12; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegPhyTxtdAnt1PmDpdMemEq201_u;

/*REG_PHY_TXTD_ANT1_PM_DPD_MEM_EQ_2_2_3 0x138 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmDpdMemEq22 : 12; //no description, reset value: 0x0, access type: RW
		uint32 pmDpdMemEq23 : 12; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegPhyTxtdAnt1PmDpdMemEq223_u;

/*REG_PHY_TXTD_ANT1_PM_DPD_MEM_EQ_2_4_5 0x13C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmDpdMemEq24 : 12; //no description, reset value: 0x0, access type: RW
		uint32 pmDpdMemEq25 : 12; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegPhyTxtdAnt1PmDpdMemEq245_u;

/*REG_PHY_TXTD_ANT1_PM_DPD_MEM_EQ_2_6_7 0x140 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmDpdMemEq26 : 12; //no description, reset value: 0x0, access type: RW
		uint32 pmDpdMemEq27 : 12; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegPhyTxtdAnt1PmDpdMemEq267_u;

/*REG_PHY_TXTD_ANT1_PM_DPD_MEM_EQ_2_8 0x144 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmDpdMemEq28 : 12; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 20;
	} bitFields;
} RegPhyTxtdAnt1PmDpdMemEq28_u;

/*REG_PHY_TXTD_ANT1_PM_DPD_MEM_EQ_3_0_1 0x148 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmDpdMemEq30 : 12; //no description, reset value: 0x0, access type: RW
		uint32 pmDpdMemEq31 : 12; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegPhyTxtdAnt1PmDpdMemEq301_u;

/*REG_PHY_TXTD_ANT1_PM_DPD_MEM_EQ_3_2_3 0x14C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmDpdMemEq32 : 12; //no description, reset value: 0x0, access type: RW
		uint32 pmDpdMemEq33 : 12; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegPhyTxtdAnt1PmDpdMemEq323_u;

/*REG_PHY_TXTD_ANT1_PM_DPD_MEM_EQ_3_4_5 0x150 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmDpdMemEq34 : 12; //no description, reset value: 0x0, access type: RW
		uint32 pmDpdMemEq35 : 12; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegPhyTxtdAnt1PmDpdMemEq345_u;

/*REG_PHY_TXTD_ANT1_PM_DPD_MEM_EQ_3_6_7 0x154 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmDpdMemEq36 : 12; //no description, reset value: 0x0, access type: RW
		uint32 pmDpdMemEq37 : 12; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegPhyTxtdAnt1PmDpdMemEq367_u;

/*REG_PHY_TXTD_ANT1_PM_DPD_MEM_EQ_3_8 0x158 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmDpdMemEq38 : 12; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 20;
	} bitFields;
} RegPhyTxtdAnt1PmDpdMemEq38_u;

/*REG_PHY_TXTD_ANT1_TXTD_ANT_RAM_POWER_SAVE 0x29C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmRamLs : 4; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 20;
		uint32 pmRamPsHwEn : 4; //no description, reset value: 0x0, access type: RW
		uint32 reserved1 : 4;
	} bitFields;
} RegPhyTxtdAnt1TxtdAntRamPowerSave_u;

/*REG_PHY_TXTD_ANT1_TPC_DPD 0x2A0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dpdSelIndex : 8; //dpd_sel_index -  , bit 7:6 is tx BW from TCR , bits 5:2 tx_pa_drv from tpc table , bits 1:0 tx pgc gain select. , , reset value: 0x0, access type: RO
		uint32 dpdSelPhy : 6; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 18;
	} bitFields;
} RegPhyTxtdAnt1TpcDpd_u;

/*REG_PHY_TXTD_ANT1_CLIPPER_CONF 0x2A4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmPreambleClipperThreshold : 10; //When HIGH clipper is bypasseed, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 pmPreambleClipperThresholdForce : 1; //no description, reset value: 0x0, access type: RW
		uint32 reserved1 : 3;
		uint32 pmDataClipperThreshold : 10; //no description, reset value: 0x0, access type: RW
		uint32 reserved2 : 2;
		uint32 pmDataClipperThresholdForce : 1; //no description, reset value: 0x0, access type: RW
		uint32 clipperBypass : 1; //no description, reset value: 0x1, access type: RW
		uint32 reserved3 : 2;
	} bitFields;
} RegPhyTxtdAnt1ClipperConf_u;

/*REG_PHY_TXTD_ANT1_CLIPPER_THRESHOLD 0x2A8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 preambleClipperThreshold : 10; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 2;
		uint32 dataClipperThreshold : 10; //no description, reset value: 0x0, access type: RO
		uint32 reserved1 : 2;
		uint32 txClipperThresholdIndex : 8; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegPhyTxtdAnt1ClipperThreshold_u;

/*REG_PHY_TXTD_ANT1_LOOPBACK_PYPASS 0x2AC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmLoopBandedgeBypass : 1; //pm_loop_bandedge_bypass, reset value: 0x0, access type: RW
		uint32 pmLoopFdlBypass : 1; //pm_loop_fdl_bypass, reset value: 0x0, access type: RW
		uint32 pmLoopForceRxPrefilter : 1; //pm_loop_force_rx_prefilter, reset value: 0x0, access type: RW
		uint32 pmLoopForceRxFdl : 1; //pm_loop_force_rx_fdl, reset value: 0x0, access type: RW
		uint32 reserved0 : 28;
	} bitFields;
} RegPhyTxtdAnt1LoopbackPypass_u;

/*REG_PHY_TXTD_ANT1_TXTD_GCLK_BYPASS 0x2B0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txtdGclkBypassEn : 10; //txtd_gclk_bypass_en, reset value: 0x0, access type: RW
		uint32 reserved0 : 22;
	} bitFields;
} RegPhyTxtdAnt1TxtdGclkBypass_u;



#endif // _PHY_TXTD_ANT1_REGS_H_
