
/***********************************************************************************
File:				PhyTxRegs.h
Module:				PhyTx
SOC Revision:		
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _PHY_TX_REGS_H_
#define _PHY_TX_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define PHY_TX_BASE_ADDRESS                             MEMORY_MAP_UNIT_52_BASE_ADDRESS
#define	REG_PHY_TX_TX_SW_RESET                     (PHY_TX_BASE_ADDRESS + 0x0)
#define	REG_PHY_TX_TX_BLOCK_ENABLE                 (PHY_TX_BASE_ADDRESS + 0x4)
#define	REG_PHY_TX_TX_SW_RESET_GENERATE            (PHY_TX_BASE_ADDRESS + 0x8)
#define	REG_PHY_TX_TX_SW_RESET_MASK                (PHY_TX_BASE_ADDRESS + 0xC)
#define	REG_PHY_TX_TX_BE_REG_04                    (PHY_TX_BASE_ADDRESS + 0x10)
#define	REG_PHY_TX_TX_BE_REG_05                    (PHY_TX_BASE_ADDRESS + 0x14)
#define	REG_PHY_TX_TX_BE_REG_06                    (PHY_TX_BASE_ADDRESS + 0x18)
#define	REG_PHY_TX_TX_BE_REG_07                    (PHY_TX_BASE_ADDRESS + 0x1C)
#define	REG_PHY_TX_TX_BE_REG_08                    (PHY_TX_BASE_ADDRESS + 0x20)
#define	REG_PHY_TX_TX_BE_REG_09                    (PHY_TX_BASE_ADDRESS + 0x24)
#define	REG_PHY_TX_TX_BE_REG_0A                    (PHY_TX_BASE_ADDRESS + 0x28)
#define	REG_PHY_TX_TX_BE_REG_0B                    (PHY_TX_BASE_ADDRESS + 0x2C)
#define	REG_PHY_TX_TX_BE_REG_0C                    (PHY_TX_BASE_ADDRESS + 0x30)
#define	REG_PHY_TX_TX_BE_REG_0D                    (PHY_TX_BASE_ADDRESS + 0x34)
#define	REG_PHY_TX_TX_BE_REG_0E                    (PHY_TX_BASE_ADDRESS + 0x38)
#define	REG_PHY_TX_GCLK_CONTROL                    (PHY_TX_BASE_ADDRESS + 0x3C)
#define	REG_PHY_TX_TX_BE_REG_10                    (PHY_TX_BASE_ADDRESS + 0x40)
#define	REG_PHY_TX_TX_BE_REG_11                    (PHY_TX_BASE_ADDRESS + 0x44)
#define	REG_PHY_TX_TX_BE_REG_12                    (PHY_TX_BASE_ADDRESS + 0x48)
#define	REG_PHY_TX_TX_BE_REG_13                    (PHY_TX_BASE_ADDRESS + 0x4C)
#define	REG_PHY_TX_TX_BE_REG_14                    (PHY_TX_BASE_ADDRESS + 0x50)
#define	REG_PHY_TX_TX_BE_REG_16                    (PHY_TX_BASE_ADDRESS + 0x58)
#define	REG_PHY_TX_TX_BE_REG_17                    (PHY_TX_BASE_ADDRESS + 0x5C)
#define	REG_PHY_TX_TX_BE_REG_18                    (PHY_TX_BASE_ADDRESS + 0x60)
#define	REG_PHY_TX_TX_BE_REG_19                    (PHY_TX_BASE_ADDRESS + 0x64)
#define	REG_PHY_TX_TX_BE_REG_1A                    (PHY_TX_BASE_ADDRESS + 0x68)
#define	REG_PHY_TX_TX_BE_REG_1B                    (PHY_TX_BASE_ADDRESS + 0x6C)
#define	REG_PHY_TX_TX_BE_REG_1C                    (PHY_TX_BASE_ADDRESS + 0x70)
#define	REG_PHY_TX_TX_BE_REG_1D                    (PHY_TX_BASE_ADDRESS + 0x74)
#define	REG_PHY_TX_TX_BE_REG_21                    (PHY_TX_BASE_ADDRESS + 0x84)
#define	REG_PHY_TX_TX_BE_REG_29                    (PHY_TX_BASE_ADDRESS + 0xA4)
#define	REG_PHY_TX_TX_BE_REG_32                    (PHY_TX_BASE_ADDRESS + 0xC8)
#define	REG_PHY_TX_TX_BE_REG_33                    (PHY_TX_BASE_ADDRESS + 0xCC)
#define	REG_PHY_TX_TX_BE_REG_34                    (PHY_TX_BASE_ADDRESS + 0xD0)
#define	REG_PHY_TX_TX_BE_REG_35                    (PHY_TX_BASE_ADDRESS + 0xD4)
#define	REG_PHY_TX_TX_BE_REG_36                    (PHY_TX_BASE_ADDRESS + 0xD8)
#define	REG_PHY_TX_TX_BE_REG_37                    (PHY_TX_BASE_ADDRESS + 0xDC)
#define	REG_PHY_TX_TX_BE_REG_3E                    (PHY_TX_BASE_ADDRESS + 0xF8)
#define	REG_PHY_TX_TX_BE_REG_48                    (PHY_TX_BASE_ADDRESS + 0x120)
#define	REG_PHY_TX_TX_BE_REG_4B                    (PHY_TX_BASE_ADDRESS + 0x12C)
#define	REG_PHY_TX_TX_BE_REG_4C                    (PHY_TX_BASE_ADDRESS + 0x130)
#define	REG_PHY_TX_TX_BE_REG_4D                    (PHY_TX_BASE_ADDRESS + 0x134)
#define	REG_PHY_TX_TX_BE_REG_50                    (PHY_TX_BASE_ADDRESS + 0x140)
#define	REG_PHY_TX_TX_BE_REG_57                    (PHY_TX_BASE_ADDRESS + 0x15C)
#define	REG_PHY_TX_TX_BE_REG_58                    (PHY_TX_BASE_ADDRESS + 0x160)
#define	REG_PHY_TX_TX_BE_REG_59                    (PHY_TX_BASE_ADDRESS + 0x164)
#define	REG_PHY_TX_TX_BE_REG_5A                    (PHY_TX_BASE_ADDRESS + 0x168)
#define	REG_PHY_TX_TX_BE_REG_5B                    (PHY_TX_BASE_ADDRESS + 0x16C)
#define	REG_PHY_TX_TX_BE_REG_5C                    (PHY_TX_BASE_ADDRESS + 0x170)
#define	REG_PHY_TX_TX_BE_REG_5D                    (PHY_TX_BASE_ADDRESS + 0x174)
#define	REG_PHY_TX_TX_BE_REG_5E                    (PHY_TX_BASE_ADDRESS + 0x178)
#define	REG_PHY_TX_TX_BE_REG_5F                    (PHY_TX_BASE_ADDRESS + 0x17C)
#define	REG_PHY_TX_TX_BE_REG_60                    (PHY_TX_BASE_ADDRESS + 0x180)
#define	REG_PHY_TX_TX_BE_REG_61                    (PHY_TX_BASE_ADDRESS + 0x184)
#define	REG_PHY_TX_TX_BE_RAM_RM_REG                (PHY_TX_BASE_ADDRESS + 0xC28)
#define	REG_PHY_TX_TX_GLBL_RAM_RM_REG              (PHY_TX_BASE_ADDRESS + 0xC34)
#define	REG_PHY_TX_TX_SYNC_FIFO                    (PHY_TX_BASE_ADDRESS + 0xC3C)
#define	REG_PHY_TX_TX2_CONTROL                     (PHY_TX_BASE_ADDRESS + 0xC48)
#define	REG_PHY_TX_DEBUG                           (PHY_TX_BASE_ADDRESS + 0xC50)
#define	REG_PHY_TX_BEAM_FORMING0                   (PHY_TX_BASE_ADDRESS + 0xC5C)
#define	REG_PHY_TX_TX_BE_RAM_RM_REG1               (PHY_TX_BASE_ADDRESS + 0xC64)
#define	REG_PHY_TX_MEMORY_CONTROL                  (PHY_TX_BASE_ADDRESS + 0xC68)
#define	REG_PHY_TX_SIG_CRC_DEFS                    (PHY_TX_BASE_ADDRESS + 0xC6C)
#define	REG_PHY_TX_DT_SIGNAL                       (PHY_TX_BASE_ADDRESS + 0xCA8)
#define	REG_PHY_TX_DT_POWER                        (PHY_TX_BASE_ADDRESS + 0xCAC)
#define	REG_PHY_TX_BEST_DT_SIGNAL_POWER            (PHY_TX_BASE_ADDRESS + 0xCB0)
#define	REG_PHY_TX_LEGACY_TIMERS                   (PHY_TX_BASE_ADDRESS + 0xCB4)
#define	REG_PHY_TX_NON_LEGACY_BF_READY             (PHY_TX_BASE_ADDRESS + 0xCB8)
#define	REG_PHY_TX_SPARE_REGS_0                    (PHY_TX_BASE_ADDRESS + 0xCC4)
#define	REG_PHY_TX_SPARE_REGS_1                    (PHY_TX_BASE_ADDRESS + 0xCC8)
#define	REG_PHY_TX_SPARE_REGS_2                    (PHY_TX_BASE_ADDRESS + 0xCCC)
#define	REG_PHY_TX_SPARE_REGS_3                    (PHY_TX_BASE_ADDRESS + 0xCD0)
#define	REG_PHY_TX_CUSTOM1_SM_REG_0                (PHY_TX_BASE_ADDRESS + 0xCD4)
#define	REG_PHY_TX_CUSTOM1_SM_REG_1                (PHY_TX_BASE_ADDRESS + 0xCD8)
#define	REG_PHY_TX_CYCLIC_INTERPOLATION_DPD0       (PHY_TX_BASE_ADDRESS + 0xCDC)
#define	REG_PHY_TX_VHT_HT_CDD_0                    (PHY_TX_BASE_ADDRESS + 0xCE0)
#define	REG_PHY_TX_VHT_HT_CDD_1                    (PHY_TX_BASE_ADDRESS + 0xCE4)
#define	REG_PHY_TX_VHT_HT_CDD_2                    (PHY_TX_BASE_ADDRESS + 0xCE8)
#define	REG_PHY_TX_TX_11B_LUT_IQ_ADDR0             (PHY_TX_BASE_ADDRESS + 0xE10)
#define	REG_PHY_TX_TX_11B_LUT_IQ_ADDR1             (PHY_TX_BASE_ADDRESS + 0xE14)
#define	REG_PHY_TX_TX_11B_LUT_IQ_ADDR2             (PHY_TX_BASE_ADDRESS + 0xE18)
#define	REG_PHY_TX_TX_11B_LUT_IQ_ADDR3             (PHY_TX_BASE_ADDRESS + 0xE1C)
#define	REG_PHY_TX_TX_11B_LUT_IQ_ADDR4             (PHY_TX_BASE_ADDRESS + 0xE20)
#define	REG_PHY_TX_TX_11B_LUT_IQ_ADDR5             (PHY_TX_BASE_ADDRESS + 0xE24)
#define	REG_PHY_TX_TX_11B_LUT_IQ_ADDR6             (PHY_TX_BASE_ADDRESS + 0xE28)
#define	REG_PHY_TX_TX_11B_LUT_IQ_ADDR7             (PHY_TX_BASE_ADDRESS + 0xE2C)
#define	REG_PHY_TX_TX_11B_LUT_IQ_ADDR8             (PHY_TX_BASE_ADDRESS + 0xE30)
#define	REG_PHY_TX_TX_11B_LUT_IQ_ADDR9             (PHY_TX_BASE_ADDRESS + 0xE34)
#define	REG_PHY_TX_TX_11B_LUT_IQ_ADDR10            (PHY_TX_BASE_ADDRESS + 0xE38)
#define	REG_PHY_TX_TX_11B_LUT_IQ_ADDR11            (PHY_TX_BASE_ADDRESS + 0xE3C)
#define	REG_PHY_TX_TX_11B_LUT_IQ_ADDR12            (PHY_TX_BASE_ADDRESS + 0xE40)
#define	REG_PHY_TX_TX_11B_LUT_IQ_ADDR13            (PHY_TX_BASE_ADDRESS + 0xE44)
#define	REG_PHY_TX_TX_11B_LUT_IQ_ADDR14            (PHY_TX_BASE_ADDRESS + 0xE48)
#define	REG_PHY_TX_TX_11B_LUT_IQ_ADDR15            (PHY_TX_BASE_ADDRESS + 0xE4C)
#define	REG_PHY_TX_TX_11B_LUT_IQ_ADDR16            (PHY_TX_BASE_ADDRESS + 0xE50)
#define	REG_PHY_TX_TX_11B_LUT_IQ_ADDR17            (PHY_TX_BASE_ADDRESS + 0xE54)
#define	REG_PHY_TX_TX_11B_LUT_IQ_ADDR18            (PHY_TX_BASE_ADDRESS + 0xE58)
#define	REG_PHY_TX_TX_11B_LUT_IQ_ADDR19            (PHY_TX_BASE_ADDRESS + 0xE5C)
#define	REG_PHY_TX_TX_11B_LUT_IQ_ADDR20            (PHY_TX_BASE_ADDRESS + 0xE60)
#define	REG_PHY_TX_TX_11B_LUT_IQ_ADDR21            (PHY_TX_BASE_ADDRESS + 0xE64)
#define	REG_PHY_TX_TX_11B_LUT_IQ_ADDR22            (PHY_TX_BASE_ADDRESS + 0xE68)
#define	REG_PHY_TX_TX_11B_LUT_IQ_ADDR23            (PHY_TX_BASE_ADDRESS + 0xE6C)
#define	REG_PHY_TX_TX_11B_LUT_IQ_ADDR24            (PHY_TX_BASE_ADDRESS + 0xE70)
#define	REG_PHY_TX_TX_11B_LUT_IQ_ADDR25            (PHY_TX_BASE_ADDRESS + 0xE74)
#define	REG_PHY_TX_TX_11B_LUT_IQ_ADDR26            (PHY_TX_BASE_ADDRESS + 0xE78)
#define	REG_PHY_TX_TX_11B_LUT_IQ_ADDR27            (PHY_TX_BASE_ADDRESS + 0xE7C)
#define	REG_PHY_TX_TX_11B_LUT_IQ_ADDR28            (PHY_TX_BASE_ADDRESS + 0xE80)
#define	REG_PHY_TX_TX_11B_LUT_IQ_ADDR29            (PHY_TX_BASE_ADDRESS + 0xE84)
#define	REG_PHY_TX_TX_11B_LUT_IQ_ADDR30            (PHY_TX_BASE_ADDRESS + 0xE88)
#define	REG_PHY_TX_TX_11B_LUT_IQ_ADDR31            (PHY_TX_BASE_ADDRESS + 0xE8C)
#define	REG_PHY_TX_MU_PILOT_DIRECT                 (PHY_TX_BASE_ADDRESS + 0xEBC)
#define	REG_PHY_TX_TX_CONTROL_ERROR_EN_MASK        (PHY_TX_BASE_ADDRESS + 0xEC0)
#define	REG_PHY_TX_CONST_MAP_0                     (PHY_TX_BASE_ADDRESS + 0xEC4)
#define	REG_PHY_TX_CONST_MAP_1                     (PHY_TX_BASE_ADDRESS + 0xEC8)
#define	REG_PHY_TX_CONST_MAP_2                     (PHY_TX_BASE_ADDRESS + 0xECC)
#define	REG_PHY_TX_CONST_MAP_3                     (PHY_TX_BASE_ADDRESS + 0xED0)
#define	REG_PHY_TX_CONST_MAP_L_SIG                 (PHY_TX_BASE_ADDRESS + 0xED4)
#define	REG_PHY_TX_HE_STF_SCALE_0                  (PHY_TX_BASE_ADDRESS + 0xED8)
#define	REG_PHY_TX_HE_STF_SCALE_1                  (PHY_TX_BASE_ADDRESS + 0xEDC)
#define	REG_PHY_TX_HE_STF_SCALE_2                  (PHY_TX_BASE_ADDRESS + 0xEE0)
#define	REG_PHY_TX_HE_STF_SCALE_3                  (PHY_TX_BASE_ADDRESS + 0xEE4)
#define	REG_PHY_TX_TONE_GEN                        (PHY_TX_BASE_ADDRESS + 0xEF0)
#define	REG_PHY_TX_TXTD_SCALE_LEG_LSTF_0           (PHY_TX_BASE_ADDRESS + 0xEF4)
#define	REG_PHY_TX_TXTD_SCALE_LEG_LSTF_1           (PHY_TX_BASE_ADDRESS + 0xEF8)
#define	REG_PHY_TX_TXTD_SCALE_LEG_DATA_0           (PHY_TX_BASE_ADDRESS + 0xEFC)
#define	REG_PHY_TX_TXTD_SCALE_HT_DATA              (PHY_TX_BASE_ADDRESS + 0xF00)
#define	REG_PHY_TX_TXTD_SCALE_HT_HTSTF_0           (PHY_TX_BASE_ADDRESS + 0xF04)
#define	REG_PHY_TX_TXTD_SCALE_VHT_DATA             (PHY_TX_BASE_ADDRESS + 0xF08)
#define	REG_PHY_TX_TXTD_SCALE_HE_LSTF_0            (PHY_TX_BASE_ADDRESS + 0xF0C)
#define	REG_PHY_TX_TXTD_SCALE_HE_LSTF_1            (PHY_TX_BASE_ADDRESS + 0xF10)
#define	REG_PHY_TX_TXTD_SCALE_HE_LLTF              (PHY_TX_BASE_ADDRESS + 0xF14)
#define	REG_PHY_TX_TXTD_SCALE_HE_HESTF_0           (PHY_TX_BASE_ADDRESS + 0xF18)
#define	REG_PHY_TX_TXTD_SCALE_HE_HESTF_1           (PHY_TX_BASE_ADDRESS + 0xF1C)
#define	REG_PHY_TX_TXTD_SCALE_HE_DATA_0            (PHY_TX_BASE_ADDRESS + 0xF20)
#define	REG_PHY_TX_TXTD_SCALE_HE_DATA_1            (PHY_TX_BASE_ADDRESS + 0xF24)
#define	REG_PHY_TX_TXTD_SCALE_HEEXT_LSTF_DATA      (PHY_TX_BASE_ADDRESS + 0xF28)
#define	REG_PHY_TX_TXTD_SCALE_HEEXT_HESTF          (PHY_TX_BASE_ADDRESS + 0xF2C)
#define	REG_PHY_TX_TXTD_SCALE_HE_MUP_LLTF_0        (PHY_TX_BASE_ADDRESS + 0xF30)
#define	REG_PHY_TX_TXTD_SCALE_HE_MUP_LLTF_1        (PHY_TX_BASE_ADDRESS + 0xF34)
#define	REG_PHY_TX_TXTD_SCALE_HE_MUP_LLTF_2        (PHY_TX_BASE_ADDRESS + 0xF38)
#define	REG_PHY_TX_TXTD_SCALE_HE_MUP_LSTF_0        (PHY_TX_BASE_ADDRESS + 0xF3C)
#define	REG_PHY_TX_TXTD_SCALE_HE_MUP_LSTF_1        (PHY_TX_BASE_ADDRESS + 0xF40)
#define	REG_PHY_TX_TXTD_SCALE_LEG_DATA_1           (PHY_TX_BASE_ADDRESS + 0xF44)
#define	REG_PHY_TX_TXTD_SCALE_HT_HTSTF_1           (PHY_TX_BASE_ADDRESS + 0xF48)
#define	REG_PHY_TX_LEGACY_CDD_0                    (PHY_TX_BASE_ADDRESS + 0xF4C)
#define	REG_PHY_TX_LEGACY_CDD_1                    (PHY_TX_BASE_ADDRESS + 0xF50)
#define	REG_PHY_TX_CUSTOM2_SM_REG_0                (PHY_TX_BASE_ADDRESS + 0xF54)
#define	REG_PHY_TX_CUSTOM2_SM_REG_1                (PHY_TX_BASE_ADDRESS + 0xF58)
#define	REG_PHY_TX_TD_SYMBOL_DELAY_LENGTH_0_1      (PHY_TX_BASE_ADDRESS + 0xF5C)
#define	REG_PHY_TX_TD_SYMBOL_DELAY_LENGTH_2_3      (PHY_TX_BASE_ADDRESS + 0xF60)
#define	REG_PHY_TX_TD_SYMBOL_DELAY_LENGTH_4_5      (PHY_TX_BASE_ADDRESS + 0xF64)
#define	REG_PHY_TX_TSSI_OPEN_LOOP                  (PHY_TX_BASE_ADDRESS + 0xF68)
#define	REG_PHY_TX_TD_SYMBOL_DELAY_LENGTH_6_7      (PHY_TX_BASE_ADDRESS + 0xF6C)
#define	REG_PHY_TX_TD_SYMBOL_DELAY_LENGTH_8_9      (PHY_TX_BASE_ADDRESS + 0xF70)
#define	REG_PHY_TX_TD_SYMBOL_DELAY_LENGTH_10_11    (PHY_TX_BASE_ADDRESS + 0xF74)
#define	REG_PHY_TX_TD_SYMBOL_DELAY_LENGTH_12_13    (PHY_TX_BASE_ADDRESS + 0xF78)
#define	REG_PHY_TX_TD_SYMBOL_DELAY_LENGTH_14       (PHY_TX_BASE_ADDRESS + 0xF7C)
#define	REG_PHY_TX_TXTD_SCALE_11B                  (PHY_TX_BASE_ADDRESS + 0xF80)
#define	REG_PHY_TX_TONE_GEN_SCALE                  (PHY_TX_BASE_ADDRESS + 0xF84)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_PHY_TX_TX_SW_RESET 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swResetNReg : 32; //Sub blocks SW Reset ,  , [0] - tx phy mac IF , [1] - air_time_calc , [4] - tx0_long_preamble , [6] - Tx BE , [8] - tx0_tx_mapper , [9] - tx0_bin_ch , [10] - tx1_tx_mapper , [11] - tx1_bin_ch , [12] - tx2_tx_mapper , [13] - tx2_bin_ch , [14] - tx3_tx_mapper , [15] - tx3_bin_ch , [21] - tx_control , [22] - tx_signal , [30:24] - Tx TD resets , , reset value: 0x0, access type: RW
	} bitFields;
} RegPhyTxTxSwReset_u;

/*REG_PHY_TX_TX_BLOCK_ENABLE 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSublockEnable : 32; //Sub blocks Enable ,  , [0] - tx phy mac IF , [1] - air_time_calc , [4] - tx0_long_preamble , [6] - Tx BE , [8] - tx0_tx_mapper , [9] - tx0_bin_ch , [10] - tx1_tx_mapper , [11] - tx1_bin_ch , [12] - tx2_tx_mapper , [13] - tx2_bin_ch , [14] - tx3_tx_mapper , [15] - tx3_bin_ch , [21] - tx_control , [22] - tx_signal , [30:24] - Tx TD  , , reset value: 0x0, access type: RW
	} bitFields;
} RegPhyTxTxBlockEnable_u;

/*REG_PHY_TX_TX_SW_RESET_GENERATE 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swResetGenerate : 32; //SW Reset Generate, reset value: 0x0, access type: RW
	} bitFields;
} RegPhyTxTxSwResetGenerate_u;

/*REG_PHY_TX_TX_SW_RESET_MASK 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 macSwResetMask : 32; //MAC SW Reset mask, reset value: 0x0, access type: RW
	} bitFields;
} RegPhyTxTxSwResetMask_u;

/*REG_PHY_TX_TX_BE_REG_04 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 2;
		uint32 macEmuModeEn : 1; //MAC Emulator Enable, reset value: 0x0, access type: RW
		uint32 reserved1 : 2;
		uint32 analogLpbkEn : 1; //Analog loop back enable, reset value: 0x0, access type: RW
		uint32 analogLpbkClkEn : 1; //Analog loop back clock enable, reset value: 0x0, access type: RW
		uint32 reserved2 : 5;
		uint32 modem11BStb40Enable : 1; //Modem 11b strobe 40 enable, reset value: 0x1, access type: RW
		uint32 clk44GenEnable : 1; //Modem 11b clk gen enable, reset value: 0x1, access type: RW
		uint32 reserved3 : 2;
		uint32 clk44GenDiv0 : 4; //Modem 11b clk gen div 0, reset value: 0xd, access type: RW
		uint32 clk44GenDiv1 : 4; //Modem 11b clk gen div 1, reset value: 0xf, access type: RW
		uint32 clk44GenLength : 4; //Modem 11b clk gen length, reset value: 0xb, access type: RW
		uint32 reserved4 : 4;
	} bitFields;
} RegPhyTxTxBeReg04_u;

/*REG_PHY_TX_TX_BE_REG_05 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txCntrlSymbolLength8 : 16; //Symbol Length Set 0, reset value: 0x1, access type: RW
		uint32 txCntrlSymbolLength9 : 16; //Symbol Length Set 1, reset value: 0x1, access type: RW
	} bitFields;
} RegPhyTxTxBeReg05_u;

/*REG_PHY_TX_TX_BE_REG_06 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swReset1NReg : 32; //Sub blocks SW Reset, reset value: 0x0, access type: RW
	} bitFields;
} RegPhyTxTxBeReg06_u;

/*REG_PHY_TX_TX_BE_REG_07 0x1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx0Enable : 1; //TX0 enable, reset value: 0x0, access type: RW
		uint32 tx1Enable : 1; //TX1 enable, reset value: 0x0, access type: RW
		uint32 tx2Enable : 1; //TX2 enable, reset value: 0x0, access type: RW
		uint32 tx3Enable : 1; //TX3 enable, reset value: 0x0, access type: RW
		uint32 txIsOpen40Mhz : 1; //TX is open to 40 MHz, reset value: 0x0, access type: CONST
		uint32 txIsOpen80Mhz : 1; //TX is open to 80 MHz, reset value: 0x1, access type: CONST
		uint32 txIsHalfBand20160 : 1; //TX is in Half Band 20Mhz out of 80Mhz, reset value: 0x0, access type: RW
		uint32 txIsHalfBand40160 : 1; //TX is in Half Band 40Mhz out of 80Mhz, reset value: 0x0, access type: RW
		uint32 reserved0 : 3;
		uint32 r1PilotsMode : 1; //Enable Rank 1 Pilots mode, reset value: 0x0, access type: RW
		uint32 afeTxDebugModeEn : 1; //Enable AFE TX Debug mode, reset value: 0x0, access type: RW
		uint32 afeTxDebugModePStart : 1; //AFE TX Debug mode p_start, reset value: 0x0, access type: WO
		uint32 tx0FeEnable : 1; //tx0 frontend enable , reset value: 0x1, access type: RW
		uint32 tx1FeEnable : 1; //tx1 frontend enable , reset value: 0x1, access type: RW
		uint32 tx2FeEnable : 1; //tx2 frontend enable , reset value: 0x1, access type: RW
		uint32 tx3FeEnable : 1; //tx3 frontend enable , reset value: 0x1, access type: RW
		uint32 txIsHalfBand80160 : 1; //half band 20 40 mode, reset value: 0x1, access type: RW
		uint32 reserved1 : 13;
	} bitFields;
} RegPhyTxTxBeReg07_u;

/*REG_PHY_TX_TX_BE_REG_08 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txCntrlSymbolLength0 : 16; //Symbol Length Set 0, reset value: 0x1, access type: RW
		uint32 txCntrlSymbolLength1 : 16; //Symbol Length Set 1, reset value: 0x1, access type: RW
	} bitFields;
} RegPhyTxTxBeReg08_u;

/*REG_PHY_TX_TX_BE_REG_09 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txCntrlSymbolLength2 : 16; //Symbol Length Set 2, reset value: 0x1, access type: RW
		uint32 txCntrlSymbolLength3 : 16; //Symbol Length Set 3, reset value: 0x1, access type: RW
	} bitFields;
} RegPhyTxTxBeReg09_u;

/*REG_PHY_TX_TX_BE_REG_0A 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txCntrlSymbolLength4 : 16; //Symbol Length Set 4, reset value: 0x1, access type: RW
		uint32 txCntrlSymbolLength5 : 16; //Symbol Length Set 5, reset value: 0x1, access type: RW
	} bitFields;
} RegPhyTxTxBeReg0A_u;

/*REG_PHY_TX_TX_BE_REG_0B 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txCntrlSymbolLength6 : 16; //Symbol Length Set 6, reset value: 0x1, access type: RW
		uint32 txCntrlSymbolLength7 : 16; //Symbol Length Set 7, reset value: 0x1, access type: RW
	} bitFields;
} RegPhyTxTxBeReg0B_u;

/*REG_PHY_TX_TX_BE_REG_0C 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txCntrlPstartToSymStrbDelay : 16; //1st Symbol Strobe Delay, reset value: 0x640, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyTxTxBeReg0C_u;

/*REG_PHY_TX_TX_BE_REG_0D 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txRequestLength : 6; //PHY Request Length, reset value: 0x4, access type: RW
		uint32 reserved0 : 26;
	} bitFields;
} RegPhyTxTxBeReg0D_u;

/*REG_PHY_TX_TX_BE_REG_0E 0x38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 startReadDel : 16; //PHY Ready rise Delay, reset value: 0x40, access type: RW
		uint32 endDelayLen : 16; //PHY Ready drop Delay, reset value: 0x10, access type: RW
	} bitFields;
} RegPhyTxTxBeReg0E_u;

/*REG_PHY_TX_GCLK_CONTROL 0x3C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gclkEnBypass : 32; //Sub blocks Gated clock bypass ,  , [0] - tx phy mac IF , [1] - air_time_calc , [4] - tx0_long_preamble , [6] - Tx BE , [8] - tx0_tx_mapper , [9] - tx0_bin_ch , [10] - tx1_tx_mapper , [11] - tx1_bin_ch , [12] - tx2_tx_mapper , [13] - tx2_bin_ch , [14] - tx3_tx_mapper , [15] - tx3_bin_ch , [21] - tx_control , [22] - tx_signal , [31:24] - Tx TD  , , reset value: 0x0, access type: RW
	} bitFields;
} RegPhyTxGclkControl_u;

/*REG_PHY_TX_TX_BE_REG_10 0x40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 scrType : 7; //Scrambler Type, reset value: 0x0, access type: RW
		uint32 reserved0 : 25;
	} bitFields;
} RegPhyTxTxBeReg10_u;

/*REG_PHY_TX_TX_BE_REG_11 0x44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 3;
		uint32 scrInitSel : 1; //Scrambler init bypass mode, reset value: 0x0, access type: RW
		uint32 reserved1 : 3;
		uint32 prbslen : 5; //Scrambler PRBS length, reset value: 0xf, access type: RW
		uint32 reserved2 : 1;
		uint32 txEndlessMode : 1; //tx_endless_mode, reset value: 0x0, access type: RW
		uint32 reserved3 : 18;
	} bitFields;
} RegPhyTxTxBeReg11_u;

/*REG_PHY_TX_TX_BE_REG_12 0x48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swScrInit0 : 7; //Scrambler new mode init user0, reset value: 0x7f, access type: RW
		uint32 reserved0 : 1;
		uint32 swScrInit1 : 7; //Scrambler new mode init user1, reset value: 0x7f, access type: RW
		uint32 reserved1 : 1;
		uint32 swScrInit2 : 7; //Scrambler new mode init user2, reset value: 0x7f, access type: RW
		uint32 reserved2 : 1;
		uint32 swScrInit3 : 7; //Scrambler new mode init user3, reset value: 0x7f, access type: RW
		uint32 reserved3 : 1;
	} bitFields;
} RegPhyTxTxBeReg12_u;

/*REG_PHY_TX_TX_BE_REG_13 0x4C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gclkEnBypass1 : 32; //Sub blocks Gated clock bypass, reset value: 0x0, access type: RW
	} bitFields;
} RegPhyTxTxBeReg13_u;

/*REG_PHY_TX_TX_BE_REG_14 0x50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSublockEnable1 : 32; //Sub blocks Enable, reset value: 0x0, access type: RW
	} bitFields;
} RegPhyTxTxBeReg14_u;

/*REG_PHY_TX_TX_BE_REG_16 0x58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 convBypass : 1; //Convolutional Encoder Bypass, reset value: 0x0, access type: RW
		uint32 convTypeA : 9; //Convolutional Encoder Polynomial A, reset value: 0x16c, access type: CONST
		uint32 convTypeB : 9; //Convolutional Encoder Polynomial B, reset value: 0x1E4, access type: CONST
		uint32 convInit : 8; //Convolutional Encoder Init Value, reset value: 0x0, access type: CONST
		uint32 reserved0 : 5;
	} bitFields;
} RegPhyTxTxBeReg16_u;

/*REG_PHY_TX_TX_BE_REG_17 0x5C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swReset1Generate : 32; //sw_reset_1_generate, reset value: 0x0, access type: RW
	} bitFields;
} RegPhyTxTxBeReg17_u;

/*REG_PHY_TX_TX_BE_REG_18 0x60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 macSwReset1Mask : 32; //mac_sw_reset_1_mask, reset value: 0x0, access type: RW
	} bitFields;
} RegPhyTxTxBeReg18_u;

/*REG_PHY_TX_TX_BE_REG_19 0x64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txCntrlSymbolLength10 : 16; //Symbol Length Set 0, reset value: 0x1, access type: RW
		uint32 txCntrlSymbolLength11 : 16; //Symbol Length Set 1, reset value: 0x1, access type: RW
	} bitFields;
} RegPhyTxTxBeReg19_u;

/*REG_PHY_TX_TX_BE_REG_1A 0x68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txCntrlSymbolLength12 : 16; //Symbol Length Set 0, reset value: 0x1, access type: RW
		uint32 txCntrlSymbolLength13 : 16; //Symbol Length Set 1, reset value: 0x1, access type: RW
	} bitFields;
} RegPhyTxTxBeReg1A_u;

/*REG_PHY_TX_TX_BE_REG_1B 0x6C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txCntrlSymbolLength14 : 16; //Symbol Length Set 0, reset value: 0x1, access type: RW
		uint32 txCntrlSymbolLength15 : 16; //Symbol Length Set 1, reset value: 0x1, access type: RW
	} bitFields;
} RegPhyTxTxBeReg1B_u;

/*REG_PHY_TX_TX_BE_REG_1C 0x70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 signalOneValue : 10; //Signal One Value, reset value: 0x120, access type: RW
		uint32 signalZeroValue : 10; //Signal Zero Value, reset value: 0x2E0, access type: RW
		uint32 reserved0 : 12;
	} bitFields;
} RegPhyTxTxBeReg1C_u;

/*REG_PHY_TX_TX_BE_REG_1D 0x74 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 signalConvTypeA : 9; //Signal Convolutional Encoder Polynomial A, reset value: 0x0, access type: RW
		uint32 signalConvTypeB : 9; //Signal Convolutional Encoder Polynomial B, reset value: 0x0, access type: RW
		uint32 reserved0 : 14;
	} bitFields;
} RegPhyTxTxBeReg1D_u;

/*REG_PHY_TX_TX_BE_REG_21 0x84 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 9;
		uint32 txControlSm : 5; //tx_control_sm, reset value: 0x0, access type: RO
		uint32 txCtrlDataDoneLong : 1; //TX OFDM Control Data Done, reset value: 0x0, access type: RO
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxTxBeReg21_u;

/*REG_PHY_TX_TX_BE_REG_29 0xA4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swServiceData : 16; //SW Service Data, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyTxTxBeReg29_u;

/*REG_PHY_TX_TX_BE_REG_32 0xC8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pilot0Map : 10; //Pilot value for zero, reset value: 0x120, access type: RW
		uint32 pilot1Map : 10; //Pilot value for one, reset value: 0x2E0, access type: RW
		uint32 reserved0 : 12;
	} bitFields;
} RegPhyTxTxBeReg32_u;

/*REG_PHY_TX_TX_BE_REG_33 0xCC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rotateTxSelect : 1; //Rotate TX select, reset value: 0x0, access type: RW
		uint32 rotateTxBypass : 1; //Rotate TX bypass, reset value: 0x0, access type: RW
		uint32 rotHtSigEn : 1; //Rotate HT Signals, reset value: 0x0, access type: RW
		uint32 longPreCalibrateMode : 1; //Long Preamble Calibration mode, reset value: 0x0, access type: RW
		uint32 reserved0 : 1;
		uint32 sbRotateSelect : 1; //Band rotate select, reset value: 0x0, access type: RW
		uint32 reserved1 : 10;
		uint32 longPreBpskVal : 5; //Long Preamble BPSK value, reset value: 0x0, access type: RW
		uint32 longCalQpskVal : 5; //Long Preamble Calibration mode QPSK value, reset value: 0x0, access type: RW
		uint32 reserved2 : 6;
	} bitFields;
} RegPhyTxTxBeReg33_u;

/*REG_PHY_TX_TX_BE_REG_34 0xD0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pilotIniCb0R1Ists1 : 4; //HT pilot init for no CB and Rank1, reset value: 0x8, access type: RW
		uint32 pilotIniCb0R2Ists1 : 4; //HT pilot init for no CB and Rank2 for antenna 0, reset value: 0x9, access type: RW
		uint32 pilotIniCb0R2Ists2 : 4; //HT pilot init for no CB and Rank2 for antenna 1, reset value: 0x3, access type: RW
		uint32 pilotIniCb0R3Ists3 : 4; //HT pilot init for CB and Rank1, reset value: 0xc, access type: RW
		uint32 pilotIniCb0R4Ists4 : 4; //HT pilot init for CB and Rank2 for antenna 0, reset value: 0x4, access type: RW
		uint32 pilotIniCb1R1Ists1 : 6; //HT pilot init for CB and Rank1 for antenna 1, reset value: 0x3, access type: RW
		uint32 pilotIniCb1R2Ists1 : 6; //HT pilot init for CB and Rank2 for antenna 1, reset value: 0x27, access type: RW
	} bitFields;
} RegPhyTxTxBeReg34_u;

/*REG_PHY_TX_TX_BE_REG_35 0xD4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pilotSqFcb : 18; //HT Pilot sequence for Full CB, reset value: 0x1a845, access type: RW
		uint32 pilotIniCb1R2Ists2 : 6; //HT pilot init for CB and Rank2 for antenna 2, reset value: 0x2, access type: RW
		uint32 pilotIniCb1R3Ists3 : 6; //HT pilot init for CB and Rank3 for antenna 3, reset value: 0xb, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegPhyTxTxBeReg35_u;

/*REG_PHY_TX_TX_BE_REG_36 0xD8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pilotSqNcb : 8; //HT Pilot sequence for Non CB and ssb, reset value: 0x0, access type: RW
		uint32 pilotSqDup : 8; //HT Pilot sequence for Non CB and duplicate mode, reset value: 0x0, access type: RW
		uint32 pilotIniCb1R4Ists4 : 6; //HT Pilot sequence for CB and duplicate mode, reset value: 0x11, access type: RW
		uint32 pilotIniCb2 : 8; //HT Pilot sequence for CB80, reset value: 0xc, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegPhyTxTxBeReg36_u;

/*REG_PHY_TX_TX_BE_REG_37 0xDC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pilotSqFcb80 : 24; //HT Pilot sequence for Full CB, reset value: 0xe31aa3, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegPhyTxTxBeReg37_u;

/*REG_PHY_TX_TX_BE_REG_3E 0xF8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pilotInitValPStart : 7; //Pilot Init value, reset value: 0x3f, access type: RW
		uint32 reserved0 : 25;
	} bitFields;
} RegPhyTxTxBeReg3E_u;

/*REG_PHY_TX_TX_BE_REG_48 0x120 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSsb40 : 1; //TX Filter select USB/LSB 40Mhz, reset value: 0x0, access type: RW
		uint32 reserved0 : 1;
		uint32 txSsb20 : 1; //TX Filter select USB/LSB 20Mhz, reset value: 0x0, access type: RW
		uint32 txLongPreambleRamEn : 1; //long preamble from ram and not rom. For tone generator, reset value: 0x0, access type: RW
		uint32 reserved1 : 1;
		uint32 txSsb80 : 1; //tx_ssb_80, reset value: 0x0, access type: RW
		uint32 reserved2 : 26;
	} bitFields;
} RegPhyTxTxBeReg48_u;

/*REG_PHY_TX_TX_BE_REG_4B 0x12C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hypTxGpr : 32; //Spare BackEnd Register, reset value: 0x0, access type: RW
	} bitFields;
} RegPhyTxTxBeReg4B_u;

/*REG_PHY_TX_TX_BE_REG_4C 0x130 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hypTxGpi : 16; //Spare general purpose inputs, reset value: 0xa020, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyTxTxBeReg4C_u;

/*REG_PHY_TX_TX_BE_REG_4D 0x134 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hypTxGpi2 : 16; //Spare general purpose inputs, reset value: 0xa020, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyTxTxBeReg4D_u;

/*REG_PHY_TX_TX_BE_REG_50 0x140 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 1;
		uint32 pilotIniCb0R3Ists1 : 4; //rank3 ists1 pilots, reset value: 0x9, access type: RW
		uint32 pilotIniCb0R3Ists2 : 4; //rank3 ists2 pilots, reset value: 0xa, access type: RW
		uint32 pilotIniCb0R4Ists1 : 4; //rank4 ists1 pilots, reset value: 0x8, access type: RW
		uint32 pilotIniCb0R4Ists2 : 4; //rank4 ists1 pilots, reset value: 0x1, access type: RW
		uint32 pilotIniCb0R4Ists3 : 4; //rank4 ists1 pilots, reset value: 0x2, access type: RW
		uint32 reserved1 : 11;
	} bitFields;
} RegPhyTxTxBeReg50_u;

/*REG_PHY_TX_TX_BE_REG_57 0x15C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 modem11BPhyDisDly : 12; //Modem 802.11B PHY TX Disable Delay, reset value: 0x0, access type: RW
		uint32 reserved0 : 4;
		uint32 modem11BPhyEnaDly : 12; //Modem 802.11B PHY TX enable Delay, reset value: 0x0, access type: RW
		uint32 reserved1 : 4;
	} bitFields;
} RegPhyTxTxBeReg57_u;

/*REG_PHY_TX_TX_BE_REG_58 0x160 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 modem11BServiceData : 8; //Modem 802.11B Sevice data, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
		uint32 modem11BPhyEndDly : 4; //Modem 802.11B PHY RX end Delay, reset value: 0x2, access type: RW
		uint32 modem11BRasTxAntEn : 3; //Bitmap indication which 2 TX antennas are connected , reset value: 0x3, access type: RW
		uint32 reserved1 : 1;
		uint32 modem11BForceTransmitAnt : 1; //Modem 802.11B Force transmit on ant0,1,2, reset value: 0x0, access type: RW
		uint32 modem11BForceTransmitAntMask : 4; //Modem 802.11B Force transmit antenna mask, reset value: 0xf, access type: RW
		uint32 reserved2 : 3;
	} bitFields;
} RegPhyTxTxBeReg58_u;

/*REG_PHY_TX_TX_BE_REG_59 0x164 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 modem11BTpMode : 4; //Modem 802.11B test plugs mode control, reset value: 0x0, access type: RW
		uint32 modem11BRasPowerTh : 22; //Modem 802.11B RAS power threshold , reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 modem11BBypassStbBalancer : 1; //Modem 802.11B bypass strobe balancer, reset value: 0x0, access type: RW
		uint32 reserved1 : 3;
	} bitFields;
} RegPhyTxTxBeReg59_u;

/*REG_PHY_TX_TX_BE_REG_5A 0x168 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 testPlugLocalEn00 : 1; //Test Plug 00 Enable, reset value: 0x0, access type: RW
		uint32 testPlugLocalEn01 : 1; //Test Plug 00 Enable, reset value: 0x0, access type: RW
		uint32 testPlugLocalEn02 : 1; //Test Plug 00 Enable, reset value: 0x0, access type: RW
		uint32 testPlugLocalEn03 : 1; //Test Plug 00 Enable, reset value: 0x0, access type: RW
		uint32 testPlugLocalEn04 : 1; //Test Plug 00 Enable, reset value: 0x0, access type: RW
		uint32 testPlugLocalEn05 : 1; //Test Plug 00 Enable, reset value: 0x0, access type: RW
		uint32 testPlugLocalEn06 : 1; //Test Plug 00 Enable, reset value: 0x0, access type: RW
		uint32 testPlugLocalEn07 : 1; //Test Plug 00 Enable, reset value: 0x0, access type: RW
		uint32 testPlugLocalEn08 : 1; //Test Plug 08 Enable, reset value: 0x0, access type: RW
		uint32 testPlugLocalEn09 : 1; //test_plug_local_en_09, reset value: 0x0, access type: RW
		uint32 testPlugLocalEn10 : 1; //test_plug_local_en_10, reset value: 0x0, access type: RW
		uint32 testPlugLocalEn11 : 1; //test_plug_local_en_11, reset value: 0x0, access type: RW
		uint32 testPlugLocalEn12 : 1; //test_plug_local_en_12, reset value: 0x0, access type: RW
		uint32 reserved0 : 7;
		uint32 testPlugStb4080En : 1; //Test Plug strobe control, reset value: 0x0, access type: RW
		uint32 testPlugStb4080Sel : 1; //Test Plug strobe select, reset value: 0x1, access type: RW
		uint32 reserved1 : 6;
		uint32 txBfSm2TbRate : 4; //tx_bf_sm_2_tb_rate, reset value: 0x0, access type: RW
	} bitFields;
} RegPhyTxTxBeReg5A_u;

/*REG_PHY_TX_TX_BE_REG_5B 0x16C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 testBusLowOutSelect : 2; //Test Plug Mux Out Low Selector, reset value: 0x0, access type: RW
		uint32 testBusMidOutSelect : 2; //Test Plug Mux Out Mid Selector, reset value: 0x1, access type: RW
		uint32 testBusHighOutSelect : 2; //Test Plug Mux Out High Selector, reset value: 0x2, access type: RW
		uint32 testBusFourOutSelect : 2; //test_bus_four_out_select, reset value: 0x3, access type: RW
		uint32 reserved0 : 24;
	} bitFields;
} RegPhyTxTxBeReg5B_u;

/*REG_PHY_TX_TX_BE_REG_5C 0x170 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 modem11BDtCoeffK0 : 6; //Modem 802.11B Detectors K coefficent 0, reset value: 0x16, access type: CONST
		uint32 modem11BDtCoeffK1 : 6; //Modem 802.11B Detectors K coefficent 1, reset value: 0x9, access type: CONST
		uint32 modem11BDtCoeffK2 : 6; //Modem 802.11B Detectors K coefficent 2, reset value: 0x6, access type: CONST
		uint32 modem11BDtCoeffK3 : 6; //Modem 802.11B Detectors K coefficent 3, reset value: 0x6, access type: CONST
		uint32 modem11BDtCoeffK4 : 6; //Modem 802.11B Detectors K coefficent 4, reset value: 0x6, access type: CONST
		uint32 reserved0 : 2;
	} bitFields;
} RegPhyTxTxBeReg5C_u;

/*REG_PHY_TX_TX_BE_REG_5D 0x174 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 modem11BDtCoeffK5 : 6; //Modem 802.11B Detectors K coefficent 5, reset value: 0x6, access type: CONST
		uint32 modem11BDtCoeffK6 : 6; //Modem 802.11B Detectors K coefficent 6, reset value: 0x6, access type: CONST
		uint32 modem11BDtCoeffK7 : 6; //Modem 802.11B Detectors K coefficent 7, reset value: 0x6, access type: CONST
		uint32 modem11BDtCoeffK8 : 6; //Modem 802.11B Detectors K coefficent 8, reset value: 0x6, access type: CONST
		uint32 modem11BDtCoeffK9 : 6; //Modem 802.11B Detectors K coefficent 9, reset value: 0x6, access type: CONST
		uint32 reserved0 : 2;
	} bitFields;
} RegPhyTxTxBeReg5D_u;

/*REG_PHY_TX_TX_BE_REG_5E 0x178 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 modem11BDtCoeffG0 : 6; //Modem 802.11B Detectors G coefficent 0, reset value: 0x10, access type: CONST
		uint32 modem11BDtCoeffG1 : 6; //Modem 802.11B Detectors G coefficent 1, reset value: 0x7, access type: CONST
		uint32 modem11BDtCoeffG2 : 6; //Modem 802.11B Detectors G coefficent 2, reset value: 0x4, access type: CONST
		uint32 modem11BDtCoeffG3 : 6; //Modem 802.11B Detectors G coefficent 3, reset value: 0x4, access type: CONST
		uint32 modem11BDtCoeffG4 : 6; //Modem 802.11B Detectors G coefficent 4, reset value: 0x4, access type: CONST
		uint32 reserved0 : 2;
	} bitFields;
} RegPhyTxTxBeReg5E_u;

/*REG_PHY_TX_TX_BE_REG_5F 0x17C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 modem11BDtCoeffG5 : 6; //Modem 802.11B Detectors G coefficent 5, reset value: 0x4, access type: CONST
		uint32 modem11BDtCoeffG6 : 6; //Modem 802.11B Detectors G coefficent 6, reset value: 0x4, access type: CONST
		uint32 modem11BDtCoeffG7 : 6; //Modem 802.11B Detectors G coefficent 7, reset value: 0x4, access type: CONST
		uint32 modem11BDtCoeffG8 : 6; //Modem 802.11B Detectors G coefficent 8, reset value: 0x4, access type: CONST
		uint32 modem11BDtCoeffG9 : 6; //Modem 802.11B Detectors G coefficent 9, reset value: 0x4, access type: CONST
		uint32 reserved0 : 2;
	} bitFields;
} RegPhyTxTxBeReg5F_u;

/*REG_PHY_TX_TX_BE_REG_60 0x180 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 modem11BDtCoeffCorrel0 : 5; //Modem 802.11B Detectors corelator coefficent 0, reset value: 0x3, access type: CONST
		uint32 modem11BDtCoeffCorrel1 : 5; //Modem 802.11B Detectors corelator coefficent 1, reset value: 0x1D, access type: CONST
		uint32 reserved0 : 22;
	} bitFields;
} RegPhyTxTxBeReg60_u;

/*REG_PHY_TX_TX_BE_REG_61 0x184 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 r1PilotsLongNcbTbl : 2; //Map table for R1 pilots Long Pre Non CB, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 r1PilotsLongFcbTbl : 2; //Map table for R1 pilots Long Pre Full CB, reset value: 0x0, access type: RW
		uint32 reserved1 : 26;
	} bitFields;
} RegPhyTxTxBeReg61_u;

/*REG_PHY_TX_TX_BE_RAM_RM_REG 0xC28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tpcRamRm : 3; //LDPC PRAM rm reg, reset value: 0x3, access type: RW
		uint32 reserved0 : 1;
		uint32 intlvRam0RmValue : 3; //Interleaver 0 RAM 0 rm reg, reset value: 0x3, access type: RW
		uint32 reserved1 : 1;
		uint32 intlvRam1RmValue : 3; //Interleaver 0 RAM 1 rm reg, reset value: 0x3, access type: RW
		uint32 reserved2 : 1;
		uint32 intlvRam2RmValue : 3; //Interleaver 0 RAM 2 rm reg, reset value: 0x3, access type: RW
		uint32 reserved3 : 1;
		uint32 intlvRam3RmValue : 3; //Interleaver 0 RAM 3 rm reg, reset value: 0x3, access type: RW
		uint32 tx0BinChTpmRm : 3; //tx0_bin_ch_tpm_rm, reset value: 0x3, access type: RW
		uint32 tx0BinChRuIdRm : 3; //tx0_bin_ch_ru_id_rm, reset value: 0x3, access type: RW
		uint32 reserved4 : 7;
	} bitFields;
} RegPhyTxTxBeRamRmReg_u;

/*REG_PHY_TX_TX_GLBL_RAM_RM_REG 0xC34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 longPreambleRamRm : 3; //Long preamble ram rm reg, reset value: 0x3, access type: RW
		uint32 reserved0 : 29;
	} bitFields;
} RegPhyTxTxGlblRamRmReg_u;

/*REG_PHY_TX_TX_SYNC_FIFO 0xC3C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 3;
		uint32 gen11BEvenStrbEn : 1; //generate even strobes for 11b (should be enable only when no interpolation filter is used) , reset value: 0x0, access type: RW
		uint32 reserved1 : 28;
	} bitFields;
} RegPhyTxTxSyncFifo_u;

/*REG_PHY_TX_TX2_CONTROL 0xC48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 flatBypass : 1; //flat bypass in 3ss and 3antennas or 2ss and 2antenna using default matrix, reset value: 0x0, access type: RW
		uint32 csdBypass : 1; //csd bypass, reset value: 0x0, access type: RW
		uint32 antennaSelectionEn : 1; //antenna_selection_en, reset value: 0x0, access type: RW
		uint32 antennaSelection11BEn : 1; //antenna_selection_11b_en, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
		uint32 htShortEqualizer : 12; //ht short energy equalizer, reset value: 0x516, access type: RW
		uint32 reserved1 : 8;
	} bitFields;
} RegPhyTxTx2Control_u;

/*REG_PHY_TX_DEBUG 0xC50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 4;
		uint32 afeTxStreamerMode : 2; //use streamer for 1/2/3 ants, reset value: 0x0, access type: RW
		uint32 afeTxStreamerDuplicate : 1; //afe_tx_streamer_duplicate, reset value: 0x0, access type: RW
		uint32 reserved1 : 25;
	} bitFields;
} RegPhyTxDebug_u;

/*REG_PHY_TX_BEAM_FORMING0 0xC5C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 1;
		uint32 txPowerLoadGainReady : 1; //tx_power_load_gain_ready, reset value: 0x0, access type: RW
		uint32 reserved1 : 20;
		uint32 txSpCalibEn : 1; //calibration enable, reset value: 0x0, access type: RW
		uint32 reserved2 : 9;
	} bitFields;
} RegPhyTxBeamForming0_u;

/*REG_PHY_TX_TX_BE_RAM_RM_REG1 0xC64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 qRamRmValue : 3; //Standard LDPC ERAM1 rm reg, reset value: 0x3, access type: RW
		uint32 reserved0 : 13;
		uint32 txCalRamAnt0Rm : 3; //tx_cal_ram_ant_0_rm, reset value: 0x3, access type: RW
		uint32 reserved1 : 1;
		uint32 txCalRamAnt1Rm : 3; //tx_cal_ram_ant_1_rm, reset value: 0x3, access type: RW
		uint32 reserved2 : 1;
		uint32 txCalRamAnt2Rm : 3; //tx_cal_ram_ant_2_rm, reset value: 0x3, access type: RW
		uint32 reserved3 : 1;
		uint32 txCalRamAnt3Rm : 3; //tx_cal_ram_ant_3_rm, reset value: 0x3, access type: RW
		uint32 reserved4 : 1;
	} bitFields;
} RegPhyTxTxBeRamRmReg1_u;

/*REG_PHY_TX_MEMORY_CONTROL 0xC68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 memGlobalRm : 2; //Global RM value for rams, reset value: 0x0, access type: RW
		uint32 memGlobalTestMode : 1; //Global test mode for rams, reset value: 0x0, access type: RW
		uint32 reserved0 : 29;
	} bitFields;
} RegPhyTxMemoryControl_u;

/*REG_PHY_TX_SIG_CRC_DEFS 0xC6C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sigCrcType : 8; //sig_crc_type, reset value: 0x60, access type: RW
		uint32 sigCrcInit : 8; //sig_crc_init, reset value: 0xff, access type: RW
		uint32 sigCrcLen : 4; //sig_crc_len, reset value: 0x7, access type: RW
		uint32 sigCrcResultInvert : 1; //sig_crc_result_invert, reset value: 0x1, access type: RW
		uint32 reserved0 : 11;
	} bitFields;
} RegPhyTxSigCrcDefs_u;

/*REG_PHY_TX_DT_SIGNAL 0xCA8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dtSignal : 22; //antenna 0 signal quality, reset value: 0x0, access type: RO
		uint32 reserved0 : 10;
	} bitFields;
} RegPhyTxDtSignal_u;

/*REG_PHY_TX_DT_POWER 0xCAC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dtPower : 22; //antenna 0 detected power high bits, reset value: 0x0, access type: RO
		uint32 reserved0 : 10;
	} bitFields;
} RegPhyTxDtPower_u;

/*REG_PHY_TX_BEST_DT_SIGNAL_POWER 0xCB0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bestDtSignalQ : 9; //Best antenna signal quality (log), reset value: 0x0, access type: RO
		uint32 reserved0 : 3;
		uint32 bestDtPower : 9; //Best antenna detected power(log), reset value: 0x0, access type: RO
		uint32 reserved1 : 11;
	} bitFields;
} RegPhyTxBestDtSignalPower_u;

/*REG_PHY_TX_LEGACY_TIMERS 0xCB4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 legacyDataReadyLimit : 16; //legacy_data_ready_limit, reset value: 0xcd8, access type: RW
		uint32 legacyBfReadyLimit : 16; //legacy_bf_ready_limit, reset value: 0x20b, access type: RW
	} bitFields;
} RegPhyTxLegacyTimers_u;

/*REG_PHY_TX_NON_LEGACY_BF_READY 0xCB8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 nonLegacyBfReadyLimit : 16; //non_legacy_bf_ready_limit, reset value: 0xff0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyTxNonLegacyBfReady_u;

/*REG_PHY_TX_SPARE_REGS_0 0xCC4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 spareGpr0 : 1; //Spare registers, reset value: 0x0, access type: WO
		uint32 spareGpr131 : 31; //no description, reset value: 0x0, access type: RW
	} bitFields;
} RegPhyTxSpareRegs0_u;

/*REG_PHY_TX_SPARE_REGS_1 0xCC8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 spareGpr1 : 32; //Spare registers, reset value: 0x0, access type: RW
	} bitFields;
} RegPhyTxSpareRegs1_u;

/*REG_PHY_TX_SPARE_REGS_2 0xCCC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 spareGpr2 : 32; //Spare registers, reset value: 0x0, access type: RW
	} bitFields;
} RegPhyTxSpareRegs2_u;

/*REG_PHY_TX_SPARE_REGS_3 0xCD0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 spareGpr3 : 32; //Spare registers, reset value: 0x0, access type: RW
	} bitFields;
} RegPhyTxSpareRegs3_u;

/*REG_PHY_TX_CUSTOM1_SM_REG_0 0xCD4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cddCustom12SsAnt1 : 8; //custom1 cdd register for 2ss , reset value: 0x0, access type: RW
		uint32 cddCustom12SsAnt2 : 8; //custom1 cdd register for 2ss , reset value: 0x0, access type: RW
		uint32 cddCustom12SsAnt3 : 8; //custom1 cdd register for 2ss , reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegPhyTxCustom1SmReg0_u;

/*REG_PHY_TX_CUSTOM1_SM_REG_1 0xCD8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cddCustom13SsAnt1 : 8; //custom1 cdd register for 3ss , reset value: 0x0, access type: RW
		uint32 cddCustom13SsAnt2 : 8; //custom1 cdd register for 3ss , reset value: 0x0, access type: RW
		uint32 cddCustom13SsAnt3 : 8; //custom1 cdd register for 3ss , reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegPhyTxCustom1SmReg1_u;

/*REG_PHY_TX_CYCLIC_INTERPOLATION_DPD0 0xCDC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 1;
		uint32 calibrationInExplicit : 1; //The rules to operate the Calibration unit are: , not((Explicit or NDP) and calibrationInExplicit==0), reset value: 0x0, access type: RW
		uint32 reserved1 : 30;
	} bitFields;
} RegPhyTxCyclicInterpolationDpd0_u;

/*REG_PHY_TX_VHT_HT_CDD_0 0xCE0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cdd2StsAnt1 : 8; //cdd for flat mode, reset value: 0x0, access type: RW
		uint32 cdd2StsAnt2 : 8; //cdd for flat mode, reset value: 0x0, access type: RW
		uint32 cdd2StsAnt3 : 8; //cdd for flat mode, reset value: 0x0, access type: RW
		uint32 cdd3StsAnt1 : 8; //cdd for flat mode, reset value: 0x0, access type: RW
	} bitFields;
} RegPhyTxVhtHtCdd0_u;

/*REG_PHY_TX_VHT_HT_CDD_1 0xCE4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cdd3StsAnt2 : 8; //cdd for flat mode, reset value: 0x0, access type: RW
		uint32 cdd3StsAnt3 : 8; //cdd for flat mode, reset value: 0x0, access type: RW
		uint32 cdd4StsAnt1 : 8; //cdd for flat mode, reset value: 0x0, access type: RW
		uint32 cdd4StsAnt2 : 8; //cdd for flat mode, reset value: 0x0, access type: RW
	} bitFields;
} RegPhyTxVhtHtCdd1_u;

/*REG_PHY_TX_VHT_HT_CDD_2 0xCE8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cdd4StsAnt3 : 8; //cdd for flat mode, reset value: 0x0, access type: RW
		uint32 reserved0 : 24;
	} bitFields;
} RegPhyTxVhtHtCdd2_u;

/*REG_PHY_TX_TX_11B_LUT_IQ_ADDR0 0xE10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr0 : 28; //tx_11b_lut_iq_addr0, reset value: 0xEFDFBF7, access type: RW
		uint32 reserved0 : 4;
	} bitFields;
} RegPhyTxTx11BLutIqAddr0_u;

/*REG_PHY_TX_TX_11B_LUT_IQ_ADDR1 0xE14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr1 : 28; //tx_11b_lut_iq_addr1, reset value: 0xF7EBCF8, access type: RW
		uint32 reserved0 : 4;
	} bitFields;
} RegPhyTxTx11BLutIqAddr1_u;

/*REG_PHY_TX_TX_11B_LUT_IQ_ADDR2 0xE18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr2 : 28; //tx_11b_lut_iq_addr2, reset value: 0x7957576, access type: RW
		uint32 reserved0 : 4;
	} bitFields;
} RegPhyTxTx11BLutIqAddr2_u;

/*REG_PHY_TX_TX_11B_LUT_IQ_ADDR3 0xE1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr3 : 28; //tx_11b_lut_iq_addr3, reset value: 0x7F635F7, access type: RW
		uint32 reserved0 : 4;
	} bitFields;
} RegPhyTxTx11BLutIqAddr3_u;

/*REG_PHY_TX_TX_11B_LUT_IQ_ADDR4 0xE20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr4 : 28; //tx_11b_lut_iq_addr4, reset value: 0x789CF27, access type: RW
		uint32 reserved0 : 4;
	} bitFields;
} RegPhyTxTx11BLutIqAddr4_u;

/*REG_PHY_TX_TX_11B_LUT_IQ_ADDR5 0xE24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr5 : 28; //tx_11b_lut_iq_addr5, reset value: 0x7EA5027, access type: RW
		uint32 reserved0 : 4;
	} bitFields;
} RegPhyTxTx11BLutIqAddr5_u;

/*REG_PHY_TX_TX_11B_LUT_IQ_ADDR6 0xE28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr6 : 28; //tx_11b_lut_iq_addr6, reset value: 0x108A6, access type: RW
		uint32 reserved0 : 4;
	} bitFields;
} RegPhyTxTx11BLutIqAddr6_u;

/*REG_PHY_TX_TX_11B_LUT_IQ_ADDR7 0xE2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr7 : 28; //tx_11b_lut_iq_addr7, reset value: 0x61C9A6, access type: RW
		uint32 reserved0 : 4;
	} bitFields;
} RegPhyTxTx11BLutIqAddr7_u;

/*REG_PHY_TX_TX_11B_LUT_IQ_ADDR8 0xE30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr8 : 28; //tx_11b_lut_iq_addr8, reset value: 0xF7DB555, access type: RW
		uint32 reserved0 : 4;
	} bitFields;
} RegPhyTxTx11BLutIqAddr8_u;

/*REG_PHY_TX_TX_11B_LUT_IQ_ADDR9 0xE34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr9 : 28; //tx_11b_lut_iq_addr9, reset value: 0xFDE75D5, access type: RW
		uint32 reserved0 : 4;
	} bitFields;
} RegPhyTxTx11BLutIqAddr9_u;

/*REG_PHY_TX_TX_11B_LUT_IQ_ADDR10 0xE38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr10 : 28; //tx_11b_lut_iq_addr10, reset value: 0x7F52ED4, access type: RW
		uint32 reserved0 : 4;
	} bitFields;
} RegPhyTxTx11BLutIqAddr10_u;

/*REG_PHY_TX_TX_11B_LUT_IQ_ADDR11 0xE3C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr11 : 28; //tx_11b_lut_iq_addr11, reset value: 0x855EF55, access type: RW
		uint32 reserved0 : 4;
	} bitFields;
} RegPhyTxTx11BLutIqAddr11_u;

/*REG_PHY_TX_TX_11B_LUT_IQ_ADDR12 0xE40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr12 : 28; //tx_11b_lut_iq_addr12, reset value: 0x7e98884, access type: RW
		uint32 reserved0 : 4;
	} bitFields;
} RegPhyTxTx11BLutIqAddr12_u;

/*REG_PHY_TX_TX_11B_LUT_IQ_ADDR13 0xE44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr13 : 28; //tx_11b_lut_iq_addr13, reset value: 0x84A4985, access type: RW
		uint32 reserved0 : 4;
	} bitFields;
} RegPhyTxTx11BLutIqAddr13_u;

/*REG_PHY_TX_TX_11B_LUT_IQ_ADDR14 0xE48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr14 : 28; //tx_11b_lut_iq_addr14, reset value: 0x60C203, access type: RW
		uint32 reserved0 : 4;
	} bitFields;
} RegPhyTxTx11BLutIqAddr14_u;

/*REG_PHY_TX_TX_11B_LUT_IQ_ADDR15 0xE4C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr15 : 28; //tx_11b_lut_iq_addr15, reset value: 0xE18284, access type: RW
		uint32 reserved0 : 4;
	} bitFields;
} RegPhyTxTx11BLutIqAddr15_u;

/*REG_PHY_TX_TX_11B_LUT_IQ_ADDR16 0xE50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr16 : 28; //tx_11b_lut_iq_addr16, reset value: 0xEFE3CFA, access type: RW
		uint32 reserved0 : 4;
	} bitFields;
} RegPhyTxTx11BLutIqAddr16_u;

/*REG_PHY_TX_TX_11B_LUT_IQ_ADDR17 0xE54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr17 : 28; //tx_11b_lut_iq_addr17, reset value: 0xF7EFD7B, access type: RW
		uint32 reserved0 : 4;
	} bitFields;
} RegPhyTxTx11BLutIqAddr17_u;

/*REG_PHY_TX_TX_11B_LUT_IQ_ADDR18 0xE58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr18 : 28; //tx_11b_lut_iq_addr18, reset value: 0x79575F9, access type: RW
		uint32 reserved0 : 4;
	} bitFields;
} RegPhyTxTx11BLutIqAddr18_u;

/*REG_PHY_TX_TX_11B_LUT_IQ_ADDR19 0xE5C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr19 : 28; //tx_11b_lut_iq_addr19, reset value: 0x7F636FA, access type: RW
		uint32 reserved0 : 4;
	} bitFields;
} RegPhyTxTx11BLutIqAddr19_u;

/*REG_PHY_TX_TX_11B_LUT_IQ_ADDR20 0xE60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr20 : 28; //tx_11b_lut_iq_addr20, reset value: 0x789D029, access type: RW
		uint32 reserved0 : 4;
	} bitFields;
} RegPhyTxTx11BLutIqAddr20_u;

/*REG_PHY_TX_TX_11B_LUT_IQ_ADDR21 0xE64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr21 : 28; //tx_11b_lut_iq_addr21, reset value: 0x7EA90AA, access type: RW
		uint32 reserved0 : 4;
	} bitFields;
} RegPhyTxTx11BLutIqAddr21_u;

/*REG_PHY_TX_TX_11B_LUT_IQ_ADDR22 0xE68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr22 : 28; //tx_11b_lut_iq_addr22, reset value: 0x149A9, access type: RW
		uint32 reserved0 : 4;
	} bitFields;
} RegPhyTxTx11BLutIqAddr22_u;

/*REG_PHY_TX_TX_11B_LUT_IQ_ADDR23 0xE6C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr23 : 28; //tx_11b_lut_iq_addr23, reset value: 0x620A29, access type: RW
		uint32 reserved0 : 4;
	} bitFields;
} RegPhyTxTx11BLutIqAddr23_u;

/*REG_PHY_TX_TX_11B_LUT_IQ_ADDR24 0xE70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr24 : 28; //tx_11b_lut_iq_addr24, reset value: 0xF7DF5D8, access type: RW
		uint32 reserved0 : 4;
	} bitFields;
} RegPhyTxTx11BLutIqAddr24_u;

/*REG_PHY_TX_TX_11B_LUT_IQ_ADDR25 0xE74 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr25 : 28; //tx_11b_lut_iq_addr25, reset value: 0xFDEB6D8, access type: RW
		uint32 reserved0 : 4;
	} bitFields;
} RegPhyTxTx11BLutIqAddr25_u;

/*REG_PHY_TX_TX_11B_LUT_IQ_ADDR26 0xE78 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr26 : 28; //tx_11b_lut_iq_addr26, reset value: 0x7F56F57, access type: RW
		uint32 reserved0 : 4;
	} bitFields;
} RegPhyTxTx11BLutIqAddr26_u;

/*REG_PHY_TX_TX_11B_LUT_IQ_ADDR27 0xE7C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr27 : 28; //tx_11b_lut_iq_addr27, reset value: 0x855F057, access type: RW
		uint32 reserved0 : 4;
	} bitFields;
} RegPhyTxTx11BLutIqAddr27_u;

/*REG_PHY_TX_TX_11B_LUT_IQ_ADDR28 0xE80 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr28 : 28; //tx_11b_lut_iq_addr28, reset value: 0x7e98987, access type: RW
		uint32 reserved0 : 4;
	} bitFields;
} RegPhyTxTx11BLutIqAddr28_u;

/*REG_PHY_TX_TX_11B_LUT_IQ_ADDR29 0xE84 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr29 : 28; //tx_11b_lut_iq_addr29, reset value: 0x84A4A08, access type: RW
		uint32 reserved0 : 4;
	} bitFields;
} RegPhyTxTx11BLutIqAddr29_u;

/*REG_PHY_TX_TX_11B_LUT_IQ_ADDR30 0xE88 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr30 : 28; //tx_11b_lut_iq_addr30, reset value: 0x610286, access type: RW
		uint32 reserved0 : 4;
	} bitFields;
} RegPhyTxTx11BLutIqAddr30_u;

/*REG_PHY_TX_TX_11B_LUT_IQ_ADDR31 0xE8C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr31 : 28; //tx_11b_lut_iq_addr31, reset value: 0xE1C387, access type: RW
		uint32 reserved0 : 4;
	} bitFields;
} RegPhyTxTx11BLutIqAddr31_u;

/*REG_PHY_TX_MU_PILOT_DIRECT 0xEBC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muPilotDirect2SsEn : 1; //mu_pilot_direct_2ss_en, reset value: 0x0, access type: RW
		uint32 muPilotDirect3SsEn : 1; //mu_pilot_direct_3ss_en, reset value: 0x0, access type: RW
		uint32 muPilotDirect4SsEn : 1; //mu_pilot_direct_4ss_en, reset value: 0x0, access type: RW
		uint32 createMuAsDirectMap : 1; //create_mu_as_direct_map, reset value: 0x0, access type: RW
		uint32 reserved0 : 28;
	} bitFields;
} RegPhyTxMuPilotDirect_u;

/*REG_PHY_TX_TX_CONTROL_ERROR_EN_MASK 0xEC0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txControlErrorEnMask : 3; //[0]-mu packet , [1]-legacy bf , [2]-tx_enable while rx_ready, reset value: 0x0, access type: RW
		uint32 reserved0 : 29;
	} bitFields;
} RegPhyTxTxControlErrorEnMask_u;

/*REG_PHY_TX_CONST_MAP_0 0xEC4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dataEquSet0 : 12; //BPSK constellation mapping, reset value: 0x0, access type: RW
		uint32 reserved0 : 4;
		uint32 dataEquSet1 : 12; //QPSK constellation mapping, reset value: 0x0, access type: RW
		uint32 reserved1 : 4;
	} bitFields;
} RegPhyTxConstMap0_u;

/*REG_PHY_TX_CONST_MAP_1 0xEC8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dataEquSet2 : 12; //constellation mapping QAM16, reset value: 0x0, access type: RW
		uint32 reserved0 : 4;
		uint32 dataEquSet3 : 12; //constellation mapping QAM64, reset value: 0x0, access type: RW
		uint32 reserved1 : 4;
	} bitFields;
} RegPhyTxConstMap1_u;

/*REG_PHY_TX_CONST_MAP_2 0xECC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dataEquSet4 : 12; //constellation mapping QAM256, reset value: 0x0, access type: RW
		uint32 reserved0 : 4;
		uint32 dataEquSet5 : 12; //constellation mapping QAM1024, reset value: 0x0, access type: RW
		uint32 reserved1 : 4;
	} bitFields;
} RegPhyTxConstMap2_u;

/*REG_PHY_TX_CONST_MAP_3 0xED0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dataEquSet6 : 12; //constellation mapping QAM4096, reset value: 0x0, access type: RW
		uint32 reserved0 : 20;
	} bitFields;
} RegPhyTxConstMap3_u;

/*REG_PHY_TX_CONST_MAP_L_SIG 0xED4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lSigBpskVal : 12; //SU/MU BSLK, reset value: 0x0, access type: RW
		uint32 reserved0 : 4;
		uint32 lSigBpskBoostVal : 12; //l_sig_bpsk_boost_val, reset value: 0x0, access type: RW
		uint32 reserved1 : 4;
	} bitFields;
} RegPhyTxConstMapLSig_u;

/*REG_PHY_TX_HE_STF_SCALE_0 0xED8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 heStfScaleVal0 : 11; //he_stf_scale_val0, reset value: 0x0, access type: RW
		uint32 reserved0 : 5;
		uint32 heStfScaleVal1 : 11; //he_stf_scale_val1, reset value: 0x0, access type: RW
		uint32 reserved1 : 5;
	} bitFields;
} RegPhyTxHeStfScale0_u;

/*REG_PHY_TX_HE_STF_SCALE_1 0xEDC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 heStfScaleVal2 : 11; //he_stf_scale_val2, reset value: 0x0, access type: RW
		uint32 reserved0 : 5;
		uint32 heStfScaleVal3 : 11; //he_stf_scale_val3, reset value: 0x0, access type: RW
		uint32 reserved1 : 5;
	} bitFields;
} RegPhyTxHeStfScale1_u;

/*REG_PHY_TX_HE_STF_SCALE_2 0xEE0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 heStfScaleVal4 : 11; //he_stf_scale_val4, reset value: 0x0, access type: RW
		uint32 reserved0 : 5;
		uint32 heStfScaleVal5 : 11; //he_stf_scale_val5, reset value: 0x0, access type: RW
		uint32 reserved1 : 5;
	} bitFields;
} RegPhyTxHeStfScale2_u;

/*REG_PHY_TX_HE_STF_SCALE_3 0xEE4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 heStfScaleVal6 : 11; //he_stf_scale_val6, reset value: 0x0, access type: RW
		uint32 reserved0 : 5;
		uint32 heStfScaleVal7 : 11; //he_stf_scale_val7, reset value: 0x0, access type: RW
		uint32 reserved1 : 5;
	} bitFields;
} RegPhyTxHeStfScale3_u;

/*REG_PHY_TX_TONE_GEN 0xEF0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 toneGenStart : 1; //tone_gen_start, reset value: 0x0, access type: RW
		uint32 toneGenOn : 1; //tone_gen_on, reset value: 0x0, access type: RW
		uint32 toneGenLstfMode : 1; //tone_gen_lstf_mode, reset value: 0x0, access type: RW
		uint32 toneGenCycles : 27; //tone_gen_cycles, reset value: 0x0, access type: RW
		uint32 toneGenActive : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 1;
	} bitFields;
} RegPhyTxToneGen_u;

/*REG_PHY_TX_TXTD_SCALE_LEG_LSTF_0 0xEF4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txtdScaleLegLstf20 : 12; //txtd_scale_leg_lstf_20, reset value: 0x0, access type: RW
		uint32 reserved0 : 4;
		uint32 txtdScaleLegLstf40 : 12; //txtd_scale_leg_lstf_40, reset value: 0x0, access type: RW
		uint32 reserved1 : 4;
	} bitFields;
} RegPhyTxTxtdScaleLegLstf0_u;

/*REG_PHY_TX_TXTD_SCALE_LEG_LSTF_1 0xEF8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txtdScaleLegLstf80 : 12; //txtd_scale_leg_lstf_80, reset value: 0x0, access type: RW
		uint32 reserved0 : 4;
		uint32 txtdScaleLegLstf160 : 12; //txtd_scale_leg_lstf_160, reset value: 0x0, access type: RW
		uint32 reserved1 : 4;
	} bitFields;
} RegPhyTxTxtdScaleLegLstf1_u;

/*REG_PHY_TX_TXTD_SCALE_LEG_DATA_0 0xEFC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txtdScaleLegData20 : 12; //txtd_scale_leg_data_20, reset value: 0x0, access type: RW
		uint32 reserved0 : 4;
		uint32 txtdScaleLegData40160 : 12; //txtd_scale_leg_data_40_160, reset value: 0x0, access type: RW
		uint32 reserved1 : 4;
	} bitFields;
} RegPhyTxTxtdScaleLegData0_u;

/*REG_PHY_TX_TXTD_SCALE_HT_DATA 0xF00 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txtdScaleHtData20 : 12; //txtd_scale_ht_data_20, reset value: 0x0, access type: RW
		uint32 reserved0 : 4;
		uint32 txtdScaleHtData40 : 12; //txtd_scale_ht_data_40, reset value: 0x0, access type: RW
		uint32 reserved1 : 4;
	} bitFields;
} RegPhyTxTxtdScaleHtData_u;

/*REG_PHY_TX_TXTD_SCALE_HT_HTSTF_0 0xF04 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txtdScaleHtHtstf20 : 12; //txtd_scale_ht_htstf_20, reset value: 0x0, access type: RW
		uint32 reserved0 : 4;
		uint32 txtdScaleHtHtstf40 : 12; //txtd_scale_ht_htstf_40, reset value: 0x0, access type: RW
		uint32 reserved1 : 4;
	} bitFields;
} RegPhyTxTxtdScaleHtHtstf0_u;

/*REG_PHY_TX_TXTD_SCALE_VHT_DATA 0xF08 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txtdScaleVhtData80 : 12; //txtd_scale_vht_data_80, reset value: 0x0, access type: RW
		uint32 reserved0 : 4;
		uint32 txtdScaleVhtData160 : 12; //txtd_scale_vht_data_160, reset value: 0x0, access type: RW
		uint32 reserved1 : 4;
	} bitFields;
} RegPhyTxTxtdScaleVhtData_u;

/*REG_PHY_TX_TXTD_SCALE_HE_LSTF_0 0xF0C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txtdScaleHeLstf20 : 12; //txtd_scale_he_lstf_20, reset value: 0x0, access type: RW
		uint32 reserved0 : 4;
		uint32 txtdScaleHeLstf40 : 12; //txtd_scale_he_lstf_40, reset value: 0x0, access type: RW
		uint32 reserved1 : 4;
	} bitFields;
} RegPhyTxTxtdScaleHeLstf0_u;

/*REG_PHY_TX_TXTD_SCALE_HE_LSTF_1 0xF10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txtdScaleHeLstf80 : 12; //txtd_scale_he_lstf_80, reset value: 0x0, access type: RW
		uint32 reserved0 : 4;
		uint32 txtdScaleHeLstf160 : 12; //txtd_scale_he_lstf_160, reset value: 0x0, access type: RW
		uint32 reserved1 : 4;
	} bitFields;
} RegPhyTxTxtdScaleHeLstf1_u;

/*REG_PHY_TX_TXTD_SCALE_HE_LLTF 0xF14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txtdScaleHeLltf2080 : 12; //txtd_scale_he_lltf_20_80, reset value: 0x0, access type: RW
		uint32 reserved0 : 4;
		uint32 txtdScaleHeLltf40160 : 12; //txtd_scale_he_lltf_40_160, reset value: 0x0, access type: RW
		uint32 reserved1 : 4;
	} bitFields;
} RegPhyTxTxtdScaleHeLltf_u;

/*REG_PHY_TX_TXTD_SCALE_HE_HESTF_0 0xF18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txtdScaleHeHestf20 : 12; //txtd_scale_he_hestf_20, reset value: 0x0, access type: RW
		uint32 reserved0 : 4;
		uint32 txtdScaleHeHestf40 : 12; //txtd_scale_he_hestf_40, reset value: 0x0, access type: RW
		uint32 reserved1 : 4;
	} bitFields;
} RegPhyTxTxtdScaleHeHestf0_u;

/*REG_PHY_TX_TXTD_SCALE_HE_HESTF_1 0xF1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txtdScaleHeHestf80 : 12; //txtd_scale_he_hestf_80, reset value: 0x0, access type: RW
		uint32 reserved0 : 4;
		uint32 txtdScaleHeHestf160 : 12; //txtd_scale_he_hestf_160, reset value: 0x0, access type: RW
		uint32 reserved1 : 4;
	} bitFields;
} RegPhyTxTxtdScaleHeHestf1_u;

/*REG_PHY_TX_TXTD_SCALE_HE_DATA_0 0xF20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txtdScaleHeData20 : 12; //txtd_scale_he_data_20, reset value: 0x0, access type: RW
		uint32 reserved0 : 4;
		uint32 txtdScaleHeData40 : 12; //txtd_scale_he_data_40, reset value: 0x0, access type: RW
		uint32 reserved1 : 4;
	} bitFields;
} RegPhyTxTxtdScaleHeData0_u;

/*REG_PHY_TX_TXTD_SCALE_HE_DATA_1 0xF24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txtdScaleHeData80 : 12; //txtd_scale_he_data_80, reset value: 0x0, access type: RW
		uint32 reserved0 : 4;
		uint32 txtdScaleHeData160 : 12; //txtd_scale_he_data_160, reset value: 0x0, access type: RW
		uint32 reserved1 : 4;
	} bitFields;
} RegPhyTxTxtdScaleHeData1_u;

/*REG_PHY_TX_TXTD_SCALE_HEEXT_LSTF_DATA 0xF28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txtdScaleHeextLstf20 : 12; //txtd_scale_heext_lstf_20, reset value: 0x0, access type: RW
		uint32 reserved0 : 4;
		uint32 txtdScaleHeextData10 : 12; //txtd_scale_heext_data_10, reset value: 0x0, access type: RW
		uint32 reserved1 : 4;
	} bitFields;
} RegPhyTxTxtdScaleHeextLstfData_u;

/*REG_PHY_TX_TXTD_SCALE_HEEXT_HESTF 0xF2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txtdScaleHeextHestf10 : 12; //txtd_scale_heext_hestf_10, reset value: 0x0, access type: RW
		uint32 reserved0 : 4;
		uint32 txtdScaleHeextHestf20 : 12; //txtd_scale_heext_hestf_20, reset value: 0x0, access type: RW
		uint32 reserved1 : 4;
	} bitFields;
} RegPhyTxTxtdScaleHeextHestf_u;

/*REG_PHY_TX_TXTD_SCALE_HE_MUP_LLTF_0 0xF30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txtdScaleHeMupLltf801602 : 12; //txtd_scale_he_mup_lltf_80_160_2, reset value: 0x0, access type: RW
		uint32 reserved0 : 4;
		uint32 txtdScaleHeMupLltf801603 : 12; //txtd_scale_he_mup_lltf_80_160_3, reset value: 0x0, access type: RW
		uint32 reserved1 : 4;
	} bitFields;
} RegPhyTxTxtdScaleHeMupLltf0_u;

/*REG_PHY_TX_TXTD_SCALE_HE_MUP_LLTF_1 0xF34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txtdScaleHeMupLltf1605 : 12; //txtd_scale_he_mup_lltf_160_5, reset value: 0x0, access type: RW
		uint32 reserved0 : 4;
		uint32 txtdScaleHeMupLltf1606 : 12; //txtd_scale_he_mup_lltf_160_6, reset value: 0x0, access type: RW
		uint32 reserved1 : 4;
	} bitFields;
} RegPhyTxTxtdScaleHeMupLltf1_u;

/*REG_PHY_TX_TXTD_SCALE_HE_MUP_LLTF_2 0xF38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txtdScaleHeMupLltf1607 : 12; //txtd_scale_he_mup_lltf_160_7, reset value: 0x0, access type: RW
		uint32 reserved0 : 20;
	} bitFields;
} RegPhyTxTxtdScaleHeMupLltf2_u;

/*REG_PHY_TX_TXTD_SCALE_HE_MUP_LSTF_0 0xF3C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txtdScaleHeMupLstf801603 : 12; //txtd_scale_he_mup_lstf_80_160_3, reset value: 0x0, access type: RW
		uint32 reserved0 : 4;
		uint32 txtdScaleHeMupLstf1605 : 12; //txtd_scale_he_mup_lstf_160_5, reset value: 0x0, access type: RW
		uint32 reserved1 : 4;
	} bitFields;
} RegPhyTxTxtdScaleHeMupLstf0_u;

/*REG_PHY_TX_TXTD_SCALE_HE_MUP_LSTF_1 0xF40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txtdScaleHeMupLstf1606 : 12; //txtd_scale_he_mup_lstf_160_6, reset value: 0x0, access type: RW
		uint32 reserved0 : 4;
		uint32 txtdScaleHeMupLstf1607 : 12; //txtd_scale_he_mup_lstf_160_7, reset value: 0x0, access type: RW
		uint32 reserved1 : 4;
	} bitFields;
} RegPhyTxTxtdScaleHeMupLstf1_u;

/*REG_PHY_TX_TXTD_SCALE_LEG_DATA_1 0xF44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txtdScaleLegData80 : 12; //txtd_scale_leg_data_80, reset value: 0x0, access type: RW
		uint32 reserved0 : 20;
	} bitFields;
} RegPhyTxTxtdScaleLegData1_u;

/*REG_PHY_TX_TXTD_SCALE_HT_HTSTF_1 0xF48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txtdScaleHtHtstf80 : 12; //txtd_scale_ht_htstf_80, reset value: 0x0, access type: RW
		uint32 reserved0 : 20;
	} bitFields;
} RegPhyTxTxtdScaleHtHtstf1_u;

/*REG_PHY_TX_LEGACY_CDD_0 0xF4C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phyCdd2AntsOfst1 : 8; //phy_cdd_2_ants_ofst_1, reset value: 0x0, access type: RW
		uint32 phyCdd3AntsOfst1 : 8; //phy_cdd_3_ants_ofst_1, reset value: 0x0, access type: RW
		uint32 phyCdd3AntsOfst2 : 8; //phy_cdd_3_ants_ofst_2, reset value: 0x0, access type: RW
		uint32 phyCdd4AntsOfst1 : 8; //phy_cdd_4_ants_ofst_1, reset value: 0x0, access type: RW
	} bitFields;
} RegPhyTxLegacyCdd0_u;

/*REG_PHY_TX_LEGACY_CDD_1 0xF50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phyCdd4AntsOfst2 : 8; //phy_cdd_4_ants_ofst_2, reset value: 0x0, access type: RW
		uint32 phyCdd4AntsOfst3 : 8; //phy_cdd_4_ants_ofst_3, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyTxLegacyCdd1_u;

/*REG_PHY_TX_CUSTOM2_SM_REG_0 0xF54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cddCustom22SsAnt1 : 8; //custom2 cdd register for 2ss , reset value: 0x0, access type: RW
		uint32 cddCustom22SsAnt2 : 8; //custom2 cdd register for 2ss , reset value: 0x0, access type: RW
		uint32 cddCustom22SsAnt3 : 8; //custom2 cdd register for 2ss , reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegPhyTxCustom2SmReg0_u;

/*REG_PHY_TX_CUSTOM2_SM_REG_1 0xF58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cddCustom23SsAnt1 : 8; //custom2 cdd register for 3ss , reset value: 0x0, access type: RW
		uint32 cddCustom23SsAnt2 : 8; //custom2 cdd register for 3ss , reset value: 0x0, access type: RW
		uint32 cddCustom23SsAnt3 : 8; //custom2 cdd register for 3ss , reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegPhyTxCustom2SmReg1_u;

/*REG_PHY_TX_TD_SYMBOL_DELAY_LENGTH_0_1 0xF5C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txCntrlTdSymbolDelayLength0 : 16; //tx_cntrl_td_symbol_delay_length_0, reset value: 0x9ff, access type: RW
		uint32 txCntrlTdSymbolDelayLength1 : 16; //tx_cntrl_td_symbol_delay_length_1, reset value: 0x1dff, access type: RW
	} bitFields;
} RegPhyTxTdSymbolDelayLength01_u;

/*REG_PHY_TX_TD_SYMBOL_DELAY_LENGTH_2_3 0xF60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txCntrlTdSymbolDelayLength2 : 16; //tx_cntrl_td_symbol_delay_length_2, reset value: 0x31FF, access type: RW
		uint32 txCntrlTdSymbolDelayLength3 : 16; //tx_cntrl_td_symbol_delay_length_3, reset value: 0x3BFF, access type: RW
	} bitFields;
} RegPhyTxTdSymbolDelayLength23_u;

/*REG_PHY_TX_TD_SYMBOL_DELAY_LENGTH_4_5 0xF64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txCntrlTdSymbolDelayLength4 : 16; //tx_cntrl_td_symbol_delay_length_4, reset value: 0x59FF, access type: RW
		uint32 txCntrlTdSymbolDelayLength5 : 16; //tx_cntrl_td_symbol_delay_length_5, reset value: 0x67FF, access type: RW
	} bitFields;
} RegPhyTxTdSymbolDelayLength45_u;

/*REG_PHY_TX_TSSI_OPEN_LOOP 0xF68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tssiOpenLoop : 1; //tssi_open_loop, reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyTxTssiOpenLoop_u;

/*REG_PHY_TX_TD_SYMBOL_DELAY_LENGTH_6_7 0xF6C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txCntrlTdSymbolDelayLength6 : 16; //tx_cntrl_td_symbol_delay_length_6, reset value: 0x2BFF, access type: RW
		uint32 txCntrlTdSymbolDelayLength7 : 16; //tx_cntrl_td_symbol_delay_length_7, reset value: 0x3FFF, access type: RW
	} bitFields;
} RegPhyTxTdSymbolDelayLength67_u;

/*REG_PHY_TX_TD_SYMBOL_DELAY_LENGTH_8_9 0xF70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txCntrlTdSymbolDelayLength8 : 16; //tx_cntrl_td_symbol_delay_length_8, reset value: 0x67FF, access type: RW
		uint32 txCntrlTdSymbolDelayLength9 : 16; //tx_cntrl_td_symbol_delay_length_9, reset value: 0x3BFF, access type: RW
	} bitFields;
} RegPhyTxTdSymbolDelayLength89_u;

/*REG_PHY_TX_TD_SYMBOL_DELAY_LENGTH_10_11 0xF74 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txCntrlTdSymbolDelayLength10 : 16; //tx_cntrl_td_symbol_delay_length_10, reset value: 0x5DFF, access type: RW
		uint32 txCntrlTdSymbolDelayLength11 : 16; //tx_cntrl_td_symbol_delay_length_11, reset value: 0xA1FF, access type: RW
	} bitFields;
} RegPhyTxTdSymbolDelayLength1011_u;

/*REG_PHY_TX_TD_SYMBOL_DELAY_LENGTH_12_13 0xF78 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txCntrlTdSymbolDelayLength12 : 16; //tx_cntrl_td_symbol_delay_length_12, reset value: 0x3BFF, access type: RW
		uint32 txCntrlTdSymbolDelayLength13 : 16; //tx_cntrl_td_symbol_delay_length_13, reset value: 0x63FF, access type: RW
	} bitFields;
} RegPhyTxTdSymbolDelayLength1213_u;

/*REG_PHY_TX_TD_SYMBOL_DELAY_LENGTH_14 0xF7C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txCntrlTdSymbolDelayLength14 : 16; //tx_cntrl_td_symbol_delay_length_14, reset value: 0xB3FF, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyTxTdSymbolDelayLength14_u;

/*REG_PHY_TX_TXTD_SCALE_11B 0xF80 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txtdScale11B : 12; //txtd_scale_11b, reset value: 0x0, access type: RW
		uint32 reserved0 : 20;
	} bitFields;
} RegPhyTxTxtdScale11B_u;

/*REG_PHY_TX_TONE_GEN_SCALE 0xF84 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 toneGenBpskVal : 12; //no description, reset value: 0x400, access type: RW
		uint32 reserved0 : 4;
		uint32 fdScaleShift : 4; //no description, reset value: 0x0, access type: RW
		uint32 reserved1 : 12;
	} bitFields;
} RegPhyTxToneGenScale_u;



#endif // _PHY_TX_REGS_H_
