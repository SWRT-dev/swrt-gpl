
/***********************************************************************************
File:				PhyTxRegsRegs.h
Module:				phyTxRegs
SOC Revision:		843
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _PHY_TX_REGS_REGS_H_
#define _PHY_TX_REGS_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define PHY_TX_REGS_BASE_ADDRESS                             MEMORY_MAP_UNIT_52_BASE_ADDRESS
#define	REG_PHY_TX_REGS_TX_BE_REG_00                    (PHY_TX_REGS_BASE_ADDRESS + 0x0)
#define	REG_PHY_TX_REGS_TX_BE_REG_01                    (PHY_TX_REGS_BASE_ADDRESS + 0x4)
#define	REG_PHY_TX_REGS_TX_BE_REG_02                    (PHY_TX_REGS_BASE_ADDRESS + 0x8)
#define	REG_PHY_TX_REGS_TX_BE_REG_03                    (PHY_TX_REGS_BASE_ADDRESS + 0xC)
#define	REG_PHY_TX_REGS_TX_BE_REG_04                    (PHY_TX_REGS_BASE_ADDRESS + 0x10)
#define	REG_PHY_TX_REGS_TX_BE_REG_05                    (PHY_TX_REGS_BASE_ADDRESS + 0x14)
#define	REG_PHY_TX_REGS_TX_BE_REG_06                    (PHY_TX_REGS_BASE_ADDRESS + 0x18)
#define	REG_PHY_TX_REGS_TX_BE_REG_07                    (PHY_TX_REGS_BASE_ADDRESS + 0x1C)
#define	REG_PHY_TX_REGS_TX_BE_REG_08                    (PHY_TX_REGS_BASE_ADDRESS + 0x20)
#define	REG_PHY_TX_REGS_TX_BE_REG_09                    (PHY_TX_REGS_BASE_ADDRESS + 0x24)
#define	REG_PHY_TX_REGS_TX_BE_REG_0A                    (PHY_TX_REGS_BASE_ADDRESS + 0x28)
#define	REG_PHY_TX_REGS_TX_BE_REG_0B                    (PHY_TX_REGS_BASE_ADDRESS + 0x2C)
#define	REG_PHY_TX_REGS_TX_BE_REG_0C                    (PHY_TX_REGS_BASE_ADDRESS + 0x30)
#define	REG_PHY_TX_REGS_TX_BE_REG_0D                    (PHY_TX_REGS_BASE_ADDRESS + 0x34)
#define	REG_PHY_TX_REGS_TX_BE_REG_0E                    (PHY_TX_REGS_BASE_ADDRESS + 0x38)
#define	REG_PHY_TX_REGS_GCLK_CONTROL                    (PHY_TX_REGS_BASE_ADDRESS + 0x3C)
#define	REG_PHY_TX_REGS_TX_BE_REG_10                    (PHY_TX_REGS_BASE_ADDRESS + 0x40)
#define	REG_PHY_TX_REGS_TX_BE_REG_11                    (PHY_TX_REGS_BASE_ADDRESS + 0x44)
#define	REG_PHY_TX_REGS_TX_BE_REG_12                    (PHY_TX_REGS_BASE_ADDRESS + 0x48)
#define	REG_PHY_TX_REGS_TX_BE_REG_13                    (PHY_TX_REGS_BASE_ADDRESS + 0x4C)
#define	REG_PHY_TX_REGS_TX_BE_REG_14                    (PHY_TX_REGS_BASE_ADDRESS + 0x50)
#define	REG_PHY_TX_REGS_TX_BE_REG_15                    (PHY_TX_REGS_BASE_ADDRESS + 0x54)
#define	REG_PHY_TX_REGS_TX_BE_REG_16                    (PHY_TX_REGS_BASE_ADDRESS + 0x58)
#define	REG_PHY_TX_REGS_TX_BE_REG_17                    (PHY_TX_REGS_BASE_ADDRESS + 0x5C)
#define	REG_PHY_TX_REGS_TX_BE_REG_18                    (PHY_TX_REGS_BASE_ADDRESS + 0x60)
#define	REG_PHY_TX_REGS_TX_BE_REG_1C                    (PHY_TX_REGS_BASE_ADDRESS + 0x70)
#define	REG_PHY_TX_REGS_TX_BE_REG_1D                    (PHY_TX_REGS_BASE_ADDRESS + 0x74)
#define	REG_PHY_TX_REGS_TX_BE_REG_21                    (PHY_TX_REGS_BASE_ADDRESS + 0x84)
#define	REG_PHY_TX_REGS_TX_BE_REG_22                    (PHY_TX_REGS_BASE_ADDRESS + 0x88)
#define	REG_PHY_TX_REGS_TX_BE_REG_23                    (PHY_TX_REGS_BASE_ADDRESS + 0x8C)
#define	REG_PHY_TX_REGS_TX_BE_REG_29                    (PHY_TX_REGS_BASE_ADDRESS + 0xA4)
#define	REG_PHY_TX_REGS_TX_BE_REG_32                    (PHY_TX_REGS_BASE_ADDRESS + 0xC8)
#define	REG_PHY_TX_REGS_TX_BE_REG_33                    (PHY_TX_REGS_BASE_ADDRESS + 0xCC)
#define	REG_PHY_TX_REGS_TX_BE_REG_34                    (PHY_TX_REGS_BASE_ADDRESS + 0xD0)
#define	REG_PHY_TX_REGS_TX_BE_REG_35                    (PHY_TX_REGS_BASE_ADDRESS + 0xD4)
#define	REG_PHY_TX_REGS_TX_BE_REG_36                    (PHY_TX_REGS_BASE_ADDRESS + 0xD8)
#define	REG_PHY_TX_REGS_TX_BE_REG_37                    (PHY_TX_REGS_BASE_ADDRESS + 0xDC)
#define	REG_PHY_TX_REGS_TX_BE_REG_3C                    (PHY_TX_REGS_BASE_ADDRESS + 0xF0)
#define	REG_PHY_TX_REGS_TX_BE_REG_3D                    (PHY_TX_REGS_BASE_ADDRESS + 0xF4)
#define	REG_PHY_TX_REGS_TX_BE_REG_3E                    (PHY_TX_REGS_BASE_ADDRESS + 0xF8)
#define	REG_PHY_TX_REGS_TX_BE_REG_48                    (PHY_TX_REGS_BASE_ADDRESS + 0x120)
#define	REG_PHY_TX_REGS_TX_BE_REG_4B                    (PHY_TX_REGS_BASE_ADDRESS + 0x12C)
#define	REG_PHY_TX_REGS_TX_BE_REG_4C                    (PHY_TX_REGS_BASE_ADDRESS + 0x130)
#define	REG_PHY_TX_REGS_TX_BE_REG_4D                    (PHY_TX_REGS_BASE_ADDRESS + 0x134)
#define	REG_PHY_TX_REGS_TX_BE_REG_50                    (PHY_TX_REGS_BASE_ADDRESS + 0x140)
#define	REG_PHY_TX_REGS_TX_BE_REG_57                    (PHY_TX_REGS_BASE_ADDRESS + 0x15C)
#define	REG_PHY_TX_REGS_TX_BE_REG_58                    (PHY_TX_REGS_BASE_ADDRESS + 0x160)
#define	REG_PHY_TX_REGS_TX_BE_REG_59                    (PHY_TX_REGS_BASE_ADDRESS + 0x164)
#define	REG_PHY_TX_REGS_TX_BE_REG_5A                    (PHY_TX_REGS_BASE_ADDRESS + 0x168)
#define	REG_PHY_TX_REGS_TX_BE_REG_5B                    (PHY_TX_REGS_BASE_ADDRESS + 0x16C)
#define	REG_PHY_TX_REGS_TX_BE_REG_5C                    (PHY_TX_REGS_BASE_ADDRESS + 0x170)
#define	REG_PHY_TX_REGS_TX_BE_REG_5D                    (PHY_TX_REGS_BASE_ADDRESS + 0x174)
#define	REG_PHY_TX_REGS_TX_BE_REG_5E                    (PHY_TX_REGS_BASE_ADDRESS + 0x178)
#define	REG_PHY_TX_REGS_TX_BE_REG_5F                    (PHY_TX_REGS_BASE_ADDRESS + 0x17C)
#define	REG_PHY_TX_REGS_TX_BE_REG_60                    (PHY_TX_REGS_BASE_ADDRESS + 0x180)
#define	REG_PHY_TX_REGS_TX_BE_REG_61                    (PHY_TX_REGS_BASE_ADDRESS + 0x184)
#define	REG_PHY_TX_REGS_TX_FE_REG_62                    (PHY_TX_REGS_BASE_ADDRESS + 0x188)
#define	REG_PHY_TX_REGS_TX0_FE_REG_07                   (PHY_TX_REGS_BASE_ADDRESS + 0x400)
#define	REG_PHY_TX_REGS_TX0_FE_REG_0D                   (PHY_TX_REGS_BASE_ADDRESS + 0x40C)
#define	REG_PHY_TX_REGS_TX0_FE_REG_0E                   (PHY_TX_REGS_BASE_ADDRESS + 0x410)
#define	REG_PHY_TX_REGS_TX0_FE_REG_14F                  (PHY_TX_REGS_BASE_ADDRESS + 0x418)
#define	REG_PHY_TX_REGS_TX0_FE_REG_150                  (PHY_TX_REGS_BASE_ADDRESS + 0x41C)
#define	REG_PHY_TX_REGS_TX0_FE_REG_151                  (PHY_TX_REGS_BASE_ADDRESS + 0x420)
#define	REG_PHY_TX_REGS_TX0_FE_REG_152                  (PHY_TX_REGS_BASE_ADDRESS + 0x424)
#define	REG_PHY_TX_REGS_TX0_FE_REG_153                  (PHY_TX_REGS_BASE_ADDRESS + 0x428)
#define	REG_PHY_TX_REGS_TX0_FE_REG_154                  (PHY_TX_REGS_BASE_ADDRESS + 0x42C)
#define	REG_PHY_TX_REGS_TX1_FE_REG_07                   (PHY_TX_REGS_BASE_ADDRESS + 0x600)
#define	REG_PHY_TX_REGS_TX1_FE_REG_0D                   (PHY_TX_REGS_BASE_ADDRESS + 0x60C)
#define	REG_PHY_TX_REGS_TX1_FE_REG_0E                   (PHY_TX_REGS_BASE_ADDRESS + 0x610)
#define	REG_PHY_TX_REGS_TX1_FE_REG_14F                  (PHY_TX_REGS_BASE_ADDRESS + 0x618)
#define	REG_PHY_TX_REGS_TX1_FE_REG_150                  (PHY_TX_REGS_BASE_ADDRESS + 0x61C)
#define	REG_PHY_TX_REGS_TX1_FE_REG_151                  (PHY_TX_REGS_BASE_ADDRESS + 0x620)
#define	REG_PHY_TX_REGS_TX1_FE_REG_152                  (PHY_TX_REGS_BASE_ADDRESS + 0x624)
#define	REG_PHY_TX_REGS_TX1_FE_REG_153                  (PHY_TX_REGS_BASE_ADDRESS + 0x628)
#define	REG_PHY_TX_REGS_TX1_FE_REG_154                  (PHY_TX_REGS_BASE_ADDRESS + 0x62C)
#define	REG_PHY_TX_REGS_TX2_FE_REG_07                   (PHY_TX_REGS_BASE_ADDRESS + 0x800)
#define	REG_PHY_TX_REGS_TX2_FE_REG_0D                   (PHY_TX_REGS_BASE_ADDRESS + 0x80C)
#define	REG_PHY_TX_REGS_TX2_FE_REG_0E                   (PHY_TX_REGS_BASE_ADDRESS + 0x810)
#define	REG_PHY_TX_REGS_TX2_FE_REG_14F                  (PHY_TX_REGS_BASE_ADDRESS + 0x818)
#define	REG_PHY_TX_REGS_TX2_FE_REG_150                  (PHY_TX_REGS_BASE_ADDRESS + 0x81C)
#define	REG_PHY_TX_REGS_TX2_FE_REG_151                  (PHY_TX_REGS_BASE_ADDRESS + 0x820)
#define	REG_PHY_TX_REGS_TX2_FE_REG_152                  (PHY_TX_REGS_BASE_ADDRESS + 0x824)
#define	REG_PHY_TX_REGS_TX2_FE_REG_153                  (PHY_TX_REGS_BASE_ADDRESS + 0x828)
#define	REG_PHY_TX_REGS_TX2_FE_REG_154                  (PHY_TX_REGS_BASE_ADDRESS + 0x82C)
#define	REG_PHY_TX_REGS_TX3_FE_REG_07                   (PHY_TX_REGS_BASE_ADDRESS + 0xA00)
#define	REG_PHY_TX_REGS_TX3_FE_REG_0D                   (PHY_TX_REGS_BASE_ADDRESS + 0xA0C)
#define	REG_PHY_TX_REGS_TX3_FE_REG_0E                   (PHY_TX_REGS_BASE_ADDRESS + 0xA10)
#define	REG_PHY_TX_REGS_TX3_FE_REG_14F                  (PHY_TX_REGS_BASE_ADDRESS + 0xA18)
#define	REG_PHY_TX_REGS_TX3_FE_REG_150                  (PHY_TX_REGS_BASE_ADDRESS + 0xA1C)
#define	REG_PHY_TX_REGS_TX3_FE_REG_151                  (PHY_TX_REGS_BASE_ADDRESS + 0xA20)
#define	REG_PHY_TX_REGS_TX3_FE_REG_152                  (PHY_TX_REGS_BASE_ADDRESS + 0xA24)
#define	REG_PHY_TX_REGS_TX3_FE_REG_153                  (PHY_TX_REGS_BASE_ADDRESS + 0xA28)
#define	REG_PHY_TX_REGS_TX3_FE_REG_154                  (PHY_TX_REGS_BASE_ADDRESS + 0xA2C)
#define	REG_PHY_TX_REGS_TX_FE_REG_07                    (PHY_TX_REGS_BASE_ADDRESS + 0xC20)
#define	REG_PHY_TX_REGS_TX_FE_REG_14F                   (PHY_TX_REGS_BASE_ADDRESS + 0xC24)
#define	REG_PHY_TX_REGS_TX_BE_RAM_RM_REG                (PHY_TX_REGS_BASE_ADDRESS + 0xC28)
#define	REG_PHY_TX_REGS_TX0_FE_RAM_RM_REG0              (PHY_TX_REGS_BASE_ADDRESS + 0xC2C)
#define	REG_PHY_TX_REGS_TX1_FE_RAM_RM_REG0              (PHY_TX_REGS_BASE_ADDRESS + 0xC30)
#define	REG_PHY_TX_REGS_TX_GLBL_RAM_RM_REG              (PHY_TX_REGS_BASE_ADDRESS + 0xC34)
#define	REG_PHY_TX_REGS_TX_2ND_FILTER                   (PHY_TX_REGS_BASE_ADDRESS + 0xC38)
#define	REG_PHY_TX_REGS_TX_SYNC_FIFO                    (PHY_TX_REGS_BASE_ADDRESS + 0xC3C)
#define	REG_PHY_TX_REGS_TX_FFT_SHARING                  (PHY_TX_REGS_BASE_ADDRESS + 0xC40)
#define	REG_PHY_TX_REGS_TX2_CONTROL                     (PHY_TX_REGS_BASE_ADDRESS + 0xC48)
#define	REG_PHY_TX_REGS_NOISE_SHAPING                   (PHY_TX_REGS_BASE_ADDRESS + 0xC4C)
#define	REG_PHY_TX_REGS_DEBUG                           (PHY_TX_REGS_BASE_ADDRESS + 0xC50)
#define	REG_PHY_TX_REGS_BEAM_FORMING0                   (PHY_TX_REGS_BASE_ADDRESS + 0xC5C)
#define	REG_PHY_TX_REGS_TX3_FE_RAM_RM_REG0              (PHY_TX_REGS_BASE_ADDRESS + 0xC60)
#define	REG_PHY_TX_REGS_TX_BE_RAM_RM_REG1               (PHY_TX_REGS_BASE_ADDRESS + 0xC64)
#define	REG_PHY_TX_REGS_MEMORY_CONTROL                  (PHY_TX_REGS_BASE_ADDRESS + 0xC68)
#define	REG_PHY_TX_REGS_SIG_CRC_DEFS                    (PHY_TX_REGS_BASE_ADDRESS + 0xC6C)
#define	REG_PHY_TX_REGS_BB_AIR_TIME_CALC_START          (PHY_TX_REGS_BASE_ADDRESS + 0xC70)
#define	REG_PHY_TX_REGS_BB_AIR_TIME_CALC_MODE           (PHY_TX_REGS_BASE_ADDRESS + 0xC74)
#define	REG_PHY_TX_REGS_BB_AIR_TIME_CALC_LENGTHS        (PHY_TX_REGS_BASE_ADDRESS + 0xC78)
#define	REG_PHY_TX_REGS_BB_AIR_TIME_CALC_RESULTS1       (PHY_TX_REGS_BASE_ADDRESS + 0xC7C)
#define	REG_PHY_TX_REGS_BB_AIR_TIME_CALC_RESULTS2       (PHY_TX_REGS_BASE_ADDRESS + 0xC80)
#define	REG_PHY_TX_REGS_BB_AIR_TIME_CALC_RESULTS3       (PHY_TX_REGS_BASE_ADDRESS + 0xC84)
#define	REG_PHY_TX_REGS_BB_AIR_TIME_CALC_RESULTS4       (PHY_TX_REGS_BASE_ADDRESS + 0xC88)
#define	REG_PHY_TX_REGS_BB_AIR_TIME_CALC_RESULTS5       (PHY_TX_REGS_BASE_ADDRESS + 0xC8C)
#define	REG_PHY_TX_REGS_BB_AIR_TIME_CALC_RESULTS6       (PHY_TX_REGS_BASE_ADDRESS + 0xC90)
#define	REG_PHY_TX_REGS_BB_AIR_TIME_CALC_RESULTS7       (PHY_TX_REGS_BASE_ADDRESS + 0xC94)
#define	REG_PHY_TX_REGS_TX0_AFE_OFFSET_GAIN0_REG        (PHY_TX_REGS_BASE_ADDRESS + 0xC98)
#define	REG_PHY_TX_REGS_TX0_AFE_OFFSET_GAIN1_REG        (PHY_TX_REGS_BASE_ADDRESS + 0xC9C)
#define	REG_PHY_TX_REGS_TX0_TX1_AMP_GAIN_F              (PHY_TX_REGS_BASE_ADDRESS + 0xCA0)
#define	REG_PHY_TX_REGS_TX2_TX3_AMP_GAIN_F              (PHY_TX_REGS_BASE_ADDRESS + 0xCA4)
#define	REG_PHY_TX_REGS_DT_SIGNAL                       (PHY_TX_REGS_BASE_ADDRESS + 0xCA8)
#define	REG_PHY_TX_REGS_DT_POWER                        (PHY_TX_REGS_BASE_ADDRESS + 0xCAC)
#define	REG_PHY_TX_REGS_BEST_DT_SIGNAL_POWER            (PHY_TX_REGS_BASE_ADDRESS + 0xCB0)
#define	REG_PHY_TX_REGS_LEGACY_TIMERS                   (PHY_TX_REGS_BASE_ADDRESS + 0xCB4)
#define	REG_PHY_TX_REGS_NON_LEGACY_BF_READY             (PHY_TX_REGS_BASE_ADDRESS + 0xCB8)
#define	REG_PHY_TX_REGS_TX_BE_RAM1_RM_REG               (PHY_TX_REGS_BASE_ADDRESS + 0xCBC)
#define	REG_PHY_TX_REGS_RATE_FILTER_RM_REG              (PHY_TX_REGS_BASE_ADDRESS + 0xCC0)
#define	REG_PHY_TX_REGS_SPARE_REGS_0                    (PHY_TX_REGS_BASE_ADDRESS + 0xCC4)
#define	REG_PHY_TX_REGS_SPARE_REGS_1                    (PHY_TX_REGS_BASE_ADDRESS + 0xCC8)
#define	REG_PHY_TX_REGS_SPARE_REGS_2                    (PHY_TX_REGS_BASE_ADDRESS + 0xCCC)
#define	REG_PHY_TX_REGS_SPARE_REGS_3                    (PHY_TX_REGS_BASE_ADDRESS + 0xCD0)
#define	REG_PHY_TX_REGS_CUSTOM1_SM_REG                  (PHY_TX_REGS_BASE_ADDRESS + 0xCD4)
#define	REG_PHY_TX_REGS_CUSTOM2_SM_REG                  (PHY_TX_REGS_BASE_ADDRESS + 0xCD8)
#define	REG_PHY_TX_REGS_CYCLIC_INTERPOLATION_DPD0       (PHY_TX_REGS_BASE_ADDRESS + 0xCDC)
#define	REG_PHY_TX_REGS_CYCLIC_INTERPOLATION_DPD1       (PHY_TX_REGS_BASE_ADDRESS + 0xCE0)
#define	REG_PHY_TX_REGS_VHT_HT_CDD_0                    (PHY_TX_REGS_BASE_ADDRESS + 0xCE4)
#define	REG_PHY_TX_REGS_VHT_HT_CDD_1                    (PHY_TX_REGS_BASE_ADDRESS + 0xCE8)
#define	REG_PHY_TX_REGS_TCR_TIME_LIMITS                 (PHY_TX_REGS_BASE_ADDRESS + 0xCEC)
#define	REG_PHY_TX_REGS_TCR_LENGTH_TIME_LIMITS          (PHY_TX_REGS_BASE_ADDRESS + 0xCF0)
#define	REG_PHY_TX_REGS_END_OF_FRAME_DELIMETER          (PHY_TX_REGS_BASE_ADDRESS + 0xCF4)
#define	REG_PHY_TX_REGS_TX_IMPLICIT_CALIBRATION         (PHY_TX_REGS_BASE_ADDRESS + 0xCF8)
#define	REG_PHY_TX_REGS_TX1_AFE_OFFSET_GAIN0_REG        (PHY_TX_REGS_BASE_ADDRESS + 0xCFC)
#define	REG_PHY_TX_REGS_TX1_AFE_OFFSET_GAIN1_REG        (PHY_TX_REGS_BASE_ADDRESS + 0xD00)
#define	REG_PHY_TX_REGS_TX2_AFE_OFFSET_GAIN0_REG        (PHY_TX_REGS_BASE_ADDRESS + 0xD04)
#define	REG_PHY_TX_REGS_TX2_AFE_OFFSET_GAIN1_REG        (PHY_TX_REGS_BASE_ADDRESS + 0xD08)
#define	REG_PHY_TX_REGS_TX3_AFE_OFFSET_GAIN0_REG        (PHY_TX_REGS_BASE_ADDRESS + 0xD0C)
#define	REG_PHY_TX_REGS_TX3_AFE_OFFSET_GAIN1_REG        (PHY_TX_REGS_BASE_ADDRESS + 0xD10)
#define	REG_PHY_TX_REGS_TPC_ACC                         (PHY_TX_REGS_BASE_ADDRESS + 0xD14)
#define	REG_PHY_TX_REGS_TX0_TX1_DIGITAL_GAIN            (PHY_TX_REGS_BASE_ADDRESS + 0xD18)
#define	REG_PHY_TX_REGS_TX2_TX3_DIGITAL_GAIN            (PHY_TX_REGS_BASE_ADDRESS + 0xD1C)
#define	REG_PHY_TX_REGS_TX_PTARGET                      (PHY_TX_REGS_BASE_ADDRESS + 0xD20)
#define	REG_PHY_TX_REGS_TPC_POUT                        (PHY_TX_REGS_BASE_ADDRESS + 0xD24)
#define	REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR0             (PHY_TX_REGS_BASE_ADDRESS + 0xE10)
#define	REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR1             (PHY_TX_REGS_BASE_ADDRESS + 0xE14)
#define	REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR2             (PHY_TX_REGS_BASE_ADDRESS + 0xE18)
#define	REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR3             (PHY_TX_REGS_BASE_ADDRESS + 0xE1C)
#define	REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR4             (PHY_TX_REGS_BASE_ADDRESS + 0xE20)
#define	REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR5             (PHY_TX_REGS_BASE_ADDRESS + 0xE24)
#define	REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR6             (PHY_TX_REGS_BASE_ADDRESS + 0xE28)
#define	REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR7             (PHY_TX_REGS_BASE_ADDRESS + 0xE2C)
#define	REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR8             (PHY_TX_REGS_BASE_ADDRESS + 0xE30)
#define	REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR9             (PHY_TX_REGS_BASE_ADDRESS + 0xE34)
#define	REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR10            (PHY_TX_REGS_BASE_ADDRESS + 0xE38)
#define	REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR11            (PHY_TX_REGS_BASE_ADDRESS + 0xE3C)
#define	REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR12            (PHY_TX_REGS_BASE_ADDRESS + 0xE40)
#define	REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR13            (PHY_TX_REGS_BASE_ADDRESS + 0xE44)
#define	REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR14            (PHY_TX_REGS_BASE_ADDRESS + 0xE48)
#define	REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR15            (PHY_TX_REGS_BASE_ADDRESS + 0xE4C)
#define	REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR16            (PHY_TX_REGS_BASE_ADDRESS + 0xE50)
#define	REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR17            (PHY_TX_REGS_BASE_ADDRESS + 0xE54)
#define	REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR18            (PHY_TX_REGS_BASE_ADDRESS + 0xE58)
#define	REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR19            (PHY_TX_REGS_BASE_ADDRESS + 0xE5C)
#define	REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR20            (PHY_TX_REGS_BASE_ADDRESS + 0xE60)
#define	REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR21            (PHY_TX_REGS_BASE_ADDRESS + 0xE64)
#define	REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR22            (PHY_TX_REGS_BASE_ADDRESS + 0xE68)
#define	REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR23            (PHY_TX_REGS_BASE_ADDRESS + 0xE6C)
#define	REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR24            (PHY_TX_REGS_BASE_ADDRESS + 0xE70)
#define	REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR25            (PHY_TX_REGS_BASE_ADDRESS + 0xE74)
#define	REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR26            (PHY_TX_REGS_BASE_ADDRESS + 0xE78)
#define	REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR27            (PHY_TX_REGS_BASE_ADDRESS + 0xE7C)
#define	REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR28            (PHY_TX_REGS_BASE_ADDRESS + 0xE80)
#define	REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR29            (PHY_TX_REGS_BASE_ADDRESS + 0xE84)
#define	REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR30            (PHY_TX_REGS_BASE_ADDRESS + 0xE88)
#define	REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR31            (PHY_TX_REGS_BASE_ADDRESS + 0xE8C)
#define	REG_PHY_TX_REGS_TX_AIR_TIME_RESULT              (PHY_TX_REGS_BASE_ADDRESS + 0xE90)
#define	REG_PHY_TX_REGS_BB_AIR_TIME_CALC_MU_RESULTS1    (PHY_TX_REGS_BASE_ADDRESS + 0xE94)
#define	REG_PHY_TX_REGS_BB_AIR_TIME_CALC_MU_RESULTS2    (PHY_TX_REGS_BASE_ADDRESS + 0xE98)
#define	REG_PHY_TX_REGS_BB_AIR_TIME_CALC_MU_MODE        (PHY_TX_REGS_BASE_ADDRESS + 0xE9C)
#define	REG_PHY_TX_REGS_TX_BE_DEBUG1_USER1              (PHY_TX_REGS_BASE_ADDRESS + 0xEA0)
#define	REG_PHY_TX_REGS_TX_BE_DEBUG2_USER1              (PHY_TX_REGS_BASE_ADDRESS + 0xEA4)
#define	REG_PHY_TX_REGS_TX_BE_DEBUG1_USER2              (PHY_TX_REGS_BASE_ADDRESS + 0xEA8)
#define	REG_PHY_TX_REGS_TX_BE_DEBUG2_USER2              (PHY_TX_REGS_BASE_ADDRESS + 0xEAC)
#define	REG_PHY_TX_REGS_TX_BE_DEBUG1_USER3              (PHY_TX_REGS_BASE_ADDRESS + 0xEB0)
#define	REG_PHY_TX_REGS_TX_BE_DEBUG2_USER3              (PHY_TX_REGS_BASE_ADDRESS + 0xEB4)
#define	REG_PHY_TX_REGS_SPHERE_TB_CONTROL               (PHY_TX_REGS_BASE_ADDRESS + 0xEB8)
#define	REG_PHY_TX_REGS_MU_PILOT_DIRECT                 (PHY_TX_REGS_BASE_ADDRESS + 0xEBC)
#define	REG_PHY_TX_REGS_TX_CONTROL_ERROR_EN_MASK        (PHY_TX_REGS_BASE_ADDRESS + 0xEC0)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_PHY_TX_REGS_TX_BE_REG_00 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swResetNReg:32;	// Sub blocks SW Reset
	} bitFields;
} RegPhyTxRegsTxBeReg00_u;

/*REG_PHY_TX_REGS_TX_BE_REG_01 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSublockEnable:32;	// Sub blocks Enable
	} bitFields;
} RegPhyTxRegsTxBeReg01_u;

/*REG_PHY_TX_REGS_TX_BE_REG_02 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swResetGenerate:32;	// SW Reset Generate
	} bitFields;
} RegPhyTxRegsTxBeReg02_u;

/*REG_PHY_TX_REGS_TX_BE_REG_03 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 macSwResetMask:32;	// MAC SW Reset mask
	} bitFields;
} RegPhyTxRegsTxBeReg03_u;

/*REG_PHY_TX_REGS_TX_BE_REG_04 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 noInterpolationMode:1;	// Enable no interpolation mode
		uint32 reserved0:1;
		uint32 macEmuModeEn:1;	// MAC Emulator Enable
		uint32 txEndlessMode:1;	// Tuning Mode
		uint32 reserved1:1;
		uint32 analogLpbkEn:1;	// Analog loop back enable
		uint32 analogLpbkClkEn:1;	// Analog loop back clock enable
		uint32 reserved2:5;
		uint32 modem11BStb40Enable:1;	// Modem 11b strobe 40 enable
		uint32 clk44GenEnable:1;	// Modem 11b clk gen enable
		uint32 reserved3:2;
		uint32 clk44GenDiv0:4;	// Modem 11b clk gen div 0
		uint32 clk44GenDiv1:4;	// Modem 11b clk gen div 1
		uint32 clk44GenLength:4;	// Modem 11b clk gen length
		uint32 reserved4:4;
	} bitFields;
} RegPhyTxRegsTxBeReg04_u;

/*REG_PHY_TX_REGS_TX_BE_REG_05 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swBistStart:1;	// SW BIST Start
		uint32 clearRamMode:1;	// Clear RAM Mode Enable
		uint32 reserved0:30;
	} bitFields;
} RegPhyTxRegsTxBeReg05_u;

/*REG_PHY_TX_REGS_TX_BE_REG_06 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swReset1NReg:32;	// Sub blocks SW Reset
	} bitFields;
} RegPhyTxRegsTxBeReg06_u;

/*REG_PHY_TX_REGS_TX_BE_REG_07 0x1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx0Enable:1;	// TX0 enable
		uint32 tx1Enable:1;	// TX1 enable
		uint32 tx2Enable:1;	// TX2 enable
		uint32 tx3Enable:1;	// TX3 enable
		uint32 txIsOpen40Mhz:1;	// TX is open to 40 MHz
		uint32 txIsOpen80Mhz:1;	// TX is open to 80 MHz
		uint32 txIsHalfBand2080:1;	// TX is in Half Band 20Mhz out of 80Mhz
		uint32 txIsHalfBand4080:1;	// TX is in Half Band 40Mhz out of 80Mhz
		uint32 reserved0:2;
		uint32 windowingEn:1;	// Enable windowing
		uint32 r1PilotsMode:1;	// Enable Rank 1 Pilots mode
		uint32 afeTxDebugModeEn:1;	// Enable AFE TX Debug mode
		uint32 afeTxDebugModePStart:1;	// AFE TX Debug mode p_start
		uint32 tx0FeEnable:1;	// tx0 frontend enable 
		uint32 tx1FeEnable:1;	// tx1 frontend enable 
		uint32 tx2FeEnable:1;	// tx2 frontend enable 
		uint32 tx3FeEnable:1;	// tx3 frontend enable 
		uint32 txIsHalfBand2040:1;	// half band 20 40 mode
		uint32 reserved1:13;
	} bitFields;
} RegPhyTxRegsTxBeReg07_u;

/*REG_PHY_TX_REGS_TX_BE_REG_08 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txCntrlSymbolLength0:16;	// Symbol Length Set 0
		uint32 txCntrlSymbolLength1:16;	// Symbol Length Set 1
	} bitFields;
} RegPhyTxRegsTxBeReg08_u;

/*REG_PHY_TX_REGS_TX_BE_REG_09 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txCntrlSymbolLength2:16;	// Symbol Length Set 2
		uint32 txCntrlSymbolLength3:16;	// Symbol Length Set 3
	} bitFields;
} RegPhyTxRegsTxBeReg09_u;

/*REG_PHY_TX_REGS_TX_BE_REG_0A 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txCntrlSymbolLength4:16;	// Symbol Length Set 4
		uint32 txCntrlSymbolLength5:16;	// Symbol Length Set 5
	} bitFields;
} RegPhyTxRegsTxBeReg0A_u;

/*REG_PHY_TX_REGS_TX_BE_REG_0B 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txCntrlSymbolLength6:16;	// Symbol Length Set 6
		uint32 txCntrlSymbolLength7:16;	// Symbol Length Set 7
	} bitFields;
} RegPhyTxRegsTxBeReg0B_u;

/*REG_PHY_TX_REGS_TX_BE_REG_0C 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txCntrlPstartToSymStrbDelay:8;	// 1st Symbol Strobe Delay
		uint32 txCntrlPstartCycInterpToSymStrbDelay:8;
		uint32 reserved0:16;
	} bitFields;
} RegPhyTxRegsTxBeReg0C_u;

/*REG_PHY_TX_REGS_TX_BE_REG_0D 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txRequestLength:6;	// PHY Request Length
		uint32 reserved0:26;
	} bitFields;
} RegPhyTxRegsTxBeReg0D_u;

/*REG_PHY_TX_REGS_TX_BE_REG_0E 0x38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 startReadDel:16;	// PHY Ready rise Delay
		uint32 endDelayLen:16;	// PHY Ready drop Delay
	} bitFields;
} RegPhyTxRegsTxBeReg0E_u;

/*REG_PHY_TX_REGS_GCLK_CONTROL 0x3C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gclkEnBypass:32;	// Sub blocks Gated clock bypass
	} bitFields;
} RegPhyTxRegsGclkControl_u;

/*REG_PHY_TX_REGS_TX_BE_REG_10 0x40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 scrType:7;	// Scrambler Type
		uint32 reserved0:25;
	} bitFields;
} RegPhyTxRegsTxBeReg10_u;

/*REG_PHY_TX_REGS_TX_BE_REG_11 0x44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:3;
		uint32 scrInitSel:1;	// Scrambler init bypass mode
		uint32 reserved1:3;
		uint32 prbslen:5;	// Scrambler PRBS length
		uint32 reserved2:20;
	} bitFields;
} RegPhyTxRegsTxBeReg11_u;

/*REG_PHY_TX_REGS_TX_BE_REG_12 0x48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swScrInit0:7;	// Scrambler new mode init user0
		uint32 reserved0:1;
		uint32 swScrInit1:7;	// Scrambler new mode init user1
		uint32 reserved1:1;
		uint32 swScrInit2:7;	// Scrambler new mode init user2
		uint32 reserved2:1;
		uint32 swScrInit3:7;	// Scrambler new mode init user3
		uint32 reserved3:1;
	} bitFields;
} RegPhyTxRegsTxBeReg12_u;

/*REG_PHY_TX_REGS_TX_BE_REG_13 0x4C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gclkEnBypass1:32;	// Sub blocks Gated clock bypass
	} bitFields;
} RegPhyTxRegsTxBeReg13_u;

/*REG_PHY_TX_REGS_TX_BE_REG_14 0x50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSublockEnable1:32;	// Sub blocks Enable
	} bitFields;
} RegPhyTxRegsTxBeReg14_u;

/*REG_PHY_TX_REGS_TX_BE_REG_15 0x54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fftSb1:2;	// fft sb 1
		uint32 fftSb2:2;	// fft sb 2
		uint32 fftSb3:2;	// fft sb 3
		uint32 fftSb4:2;	// fft sb 4
		uint32 fftSb5:2;	// fft sb 5
		uint32 fftSb6:2;	// fft sb 6
		uint32 fftSbPp:2;	// fft sb pp
		uint32 reserved0:2;
		uint32 ifftSb1:2;	// fft sb 1
		uint32 ifftSb2:2;	// fft sb 2
		uint32 ifftSb3:2;	// fft sb 3
		uint32 ifftSb4:2;	// fft sb 4
		uint32 ifftSb5:2;	// fft sb 5
		uint32 ifftSb6:2;	// fft sb 6
		uint32 ifftSbPp:2;	// fft sb pp
		uint32 reserved1:2;
	} bitFields;
} RegPhyTxRegsTxBeReg15_u;

/*REG_PHY_TX_REGS_TX_BE_REG_16 0x58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 convBypass:1;	// Convolutional Encoder Bypass
		uint32 convTypeA:9;	// Convolutional Encoder Polynomial A
		uint32 convTypeB:9;	// Convolutional Encoder Polynomial B
		uint32 convInit:8;	// Convolutional Encoder Init Value
		uint32 reserved0:5;
	} bitFields;
} RegPhyTxRegsTxBeReg16_u;

/*REG_PHY_TX_REGS_TX_BE_REG_17 0x5C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swReset1Generate:32;
	} bitFields;
} RegPhyTxRegsTxBeReg17_u;

/*REG_PHY_TX_REGS_TX_BE_REG_18 0x60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 macSwReset1Mask:32;
	} bitFields;
} RegPhyTxRegsTxBeReg18_u;

/*REG_PHY_TX_REGS_TX_BE_REG_1C 0x70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 signalOneValue:10;	// Signal One Value
		uint32 signalZeroValue:10;	// Signal Zero Value
		uint32 reserved0:12;
	} bitFields;
} RegPhyTxRegsTxBeReg1C_u;

/*REG_PHY_TX_REGS_TX_BE_REG_1D 0x74 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 signalConvTypeA:9;	// Signal Convolutional Encoder Polynomial A
		uint32 signalConvTypeB:9;	// Signal Convolutional Encoder Polynomial B
		uint32 reserved0:14;
	} bitFields;
} RegPhyTxRegsTxBeReg1D_u;

/*REG_PHY_TX_REGS_TX_BE_REG_21 0x84 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txPhyMacIfWrStm:4;	// TX OFDM PHY-MAC I/F Write STM
		uint32 txPhyMacIfRdStm:5;	// TX OFDM PHY-MAC I/F Read STM
		uint32 txControlSm:5;
		uint32 txCtrlDataDoneLong:1;	// TX OFDM Control Data Done
		uint32 tx0AfeIfWrAfterPEnd:1;	// TX Ant0 AFE I/F Write after Packet End
		uint32 tx1AfeIfWrAfterPEnd:1;	// TX Ant1 AFE I/F Write after Packet End
		uint32 tx2AfeIfWrAfterPEnd:1;	// TX Ant2 AFE I/F Write after Packet End
		uint32 tx3AfeIfWrAfterPEnd:1;	// TX Ant3 AFE I/F Write after Packet End
		uint32 txPhyMacIfBubbleBytesInFifo:3;	// tx phy mac if bubble byte count
		uint32 txPhyMacIfOfdmRdReady:1;	// read ready indication from tx_control/11b modem
		uint32 reserved0:9;
	} bitFields;
} RegPhyTxRegsTxBeReg21_u;

/*REG_PHY_TX_REGS_TX_BE_REG_22 0x88 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txPhyMacIfByteCntRd:20;	// how many bytes were read by the phy
		uint32 reserved0:12;
	} bitFields;
} RegPhyTxRegsTxBeReg22_u;

/*REG_PHY_TX_REGS_TX_BE_REG_23 0x8C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txPhyMacIfByteCntWr:20;	// how many bytes were read from the mac
		uint32 reserved0:12;
	} bitFields;
} RegPhyTxRegsTxBeReg23_u;

/*REG_PHY_TX_REGS_TX_BE_REG_29 0xA4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swServiceData:9;	// SW Service Data
		uint32 reserved0:23;
	} bitFields;
} RegPhyTxRegsTxBeReg29_u;

/*REG_PHY_TX_REGS_TX_BE_REG_32 0xC8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pilot0Map:10;	// Pilot value for zero
		uint32 pilot1Map:10;	// Pilot value for one
		uint32 reserved0:12;
	} bitFields;
} RegPhyTxRegsTxBeReg32_u;

/*REG_PHY_TX_REGS_TX_BE_REG_33 0xCC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rotateTxSelect:1;	// Rotate TX select
		uint32 rotateTxBypass:1;	// Rotate TX bypass
		uint32 rotHtSigEn:1;	// Rotate HT Signals
		uint32 longPreCalibrateMode:1;	// Long Preamble Calibration mode
		uint32 filterCalibrateMode:1;	// TX Filter Mixer Calibration mode
		uint32 sbRotateSelect:1;	// Band rotate select
		uint32 reserved0:10;
		uint32 longPreBpskVal:5;	// Long Preamble BPSK value
		uint32 longCalQpskVal:5;	// Long Preamble Calibration mode QPSK value
		uint32 reserved1:6;
	} bitFields;
} RegPhyTxRegsTxBeReg33_u;

/*REG_PHY_TX_REGS_TX_BE_REG_34 0xD0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pilotIniCb0R1Ists1:4;	// HT pilot init for no CB and Rank1
		uint32 pilotIniCb0R2Ists1:4;	// HT pilot init for no CB and Rank2 for antenna 0
		uint32 pilotIniCb0R2Ists2:4;	// HT pilot init for no CB and Rank2 for antenna 1
		uint32 pilotIniCb0R3Ists3:4;	// HT pilot init for CB and Rank1
		uint32 pilotIniCb0R4Ists4:4;	// HT pilot init for CB and Rank2 for antenna 0
		uint32 pilotIniCb1R1Ists1:6;	// HT pilot init for CB and Rank1 for antenna 1
		uint32 pilotIniCb1R2Ists1:6;	// HT pilot init for CB and Rank2 for antenna 1
	} bitFields;
} RegPhyTxRegsTxBeReg34_u;

/*REG_PHY_TX_REGS_TX_BE_REG_35 0xD4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pilotSqFcb:18;	// HT Pilot sequence for Full CB
		uint32 pilotIniCb1R2Ists2:6;	// HT pilot init for CB and Rank2 for antenna 2
		uint32 pilotIniCb1R3Ists3:6;	// HT pilot init for CB and Rank3 for antenna 3
		uint32 reserved0:2;
	} bitFields;
} RegPhyTxRegsTxBeReg35_u;

/*REG_PHY_TX_REGS_TX_BE_REG_36 0xD8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pilotSqNcb:8;	// HT Pilot sequence for Non CB and ssb
		uint32 pilotSqDup:8;	// HT Pilot sequence for Non CB and duplicate mode
		uint32 pilotIniCb1R4Ists4:6;	// HT Pilot sequence for CB and duplicate mode
		uint32 pilotIniCb2:8;	// HT Pilot sequence for CB80
		uint32 reserved0:2;
	} bitFields;
} RegPhyTxRegsTxBeReg36_u;

/*REG_PHY_TX_REGS_TX_BE_REG_37 0xDC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pilotSqFcb80:24;	// HT Pilot sequence for Full CB
		uint32 reserved0:8;
	} bitFields;
} RegPhyTxRegsTxBeReg37_u;

/*REG_PHY_TX_REGS_TX_BE_REG_3C 0xF0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dataEqualizerSet0:8;	// Multiplication Value set 0
		uint32 dataEqualizerSet1:8;	// Multiplication Value set 1
		uint32 dataEqualizerSet2:8;	// Multiplication Value set 2
		uint32 dataEqualizerSet3:8;	// Multiplication Value set 3
	} bitFields;
} RegPhyTxRegsTxBeReg3C_u;

/*REG_PHY_TX_REGS_TX_BE_REG_3D 0xF4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pilotEqualizerSet0:9;	// Pilot multiplier set 0
		uint32 reserved0:1;
		uint32 pilotEqualizerSet1:9;	// Pilot multiplier set 1
		uint32 dataEqualizerSet4:8;	// Multiplication Value set 4 for 256QAM
		uint32 reserved1:5;
	} bitFields;
} RegPhyTxRegsTxBeReg3D_u;

/*REG_PHY_TX_REGS_TX_BE_REG_3E 0xF8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pilotInitValPStart:7;	// Pilot Init value
		uint32 reserved0:25;
	} bitFields;
} RegPhyTxRegsTxBeReg3E_u;

/*REG_PHY_TX_REGS_TX_BE_REG_48 0x120 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSsb40:1;	// TX Filter select USB/LSB 40Mhz
		uint32 fixEquBypass:1;	// Fix Equalizer Bypass
		uint32 txSsb20:1;	// TX Filter select USB/LSB 20Mhz
		uint32 txLongPreambleRamEn:1;	// long preamble from ram and not rom. For tone generator
		uint32 txRippleBypass:1;	// ripple equalizer bypass
		uint32 reserved0:27;
	} bitFields;
} RegPhyTxRegsTxBeReg48_u;

/*REG_PHY_TX_REGS_TX_BE_REG_4B 0x12C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hypTxGpr:32;	// Spare BackEnd Register
	} bitFields;
} RegPhyTxRegsTxBeReg4B_u;

/*REG_PHY_TX_REGS_TX_BE_REG_4C 0x130 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hypTxGpi:16;	// Spare general purpose inputs
		uint32 reserved0:16;
	} bitFields;
} RegPhyTxRegsTxBeReg4C_u;

/*REG_PHY_TX_REGS_TX_BE_REG_4D 0x134 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hypTxGpi2:16;	// Spare general purpose inputs
		uint32 reserved0:16;
	} bitFields;
} RegPhyTxRegsTxBeReg4D_u;

/*REG_PHY_TX_REGS_TX_BE_REG_50 0x140 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:1;
		uint32 pilotIniCb0R3Ists1:4;	// rank3 ists1 pilots
		uint32 pilotIniCb0R3Ists2:4;	// rank3 ists2 pilots
		uint32 pilotIniCb0R4Ists1:4;	// rank4 ists1 pilots
		uint32 pilotIniCb0R4Ists2:4;	// rank4 ists1 pilots
		uint32 pilotIniCb0R4Ists3:4;	// rank4 ists1 pilots
		uint32 reserved1:11;
	} bitFields;
} RegPhyTxRegsTxBeReg50_u;

/*REG_PHY_TX_REGS_TX_BE_REG_57 0x15C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 modem11BPhyDisDly:12;	// Modem 802.11B PHY TX Disable Delay
		uint32 reserved0:4;
		uint32 modem11BPhyEnaDly:12;	// Modem 802.11B PHY TX enable Delay
		uint32 reserved1:4;
	} bitFields;
} RegPhyTxRegsTxBeReg57_u;

/*REG_PHY_TX_REGS_TX_BE_REG_58 0x160 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 modem11BServiceData:8;	// Modem 802.11B Sevice data
		uint32 reserved0:8;
		uint32 modem11BPhyEndDly:4;	// Modem 802.11B PHY RX end Delay
		uint32 modem11BRasTxAntEn:3;	// Bitmap indication which 2 TX antennas are connected 
		uint32 reserved1:1;
		uint32 modem11BForceTransmitAnt:1;	// Modem 802.11B Force transmit on ant0,1,2
		uint32 modem11BForceTransmitAntMask:4;	// Modem 802.11B Force transmit antenna mask
		uint32 reserved2:3;
	} bitFields;
} RegPhyTxRegsTxBeReg58_u;

/*REG_PHY_TX_REGS_TX_BE_REG_59 0x164 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 modem11BTpMode:4;	// Modem 802.11B test plugs mode control
		uint32 modem11BRasPowerTh:22;	// Modem 802.11B RAS power threshold 
		uint32 reserved0:2;
		uint32 modem11BBypassStbBalancer:1;	// Modem 802.11B bypass strobe balancer
		uint32 reserved1:3;
	} bitFields;
} RegPhyTxRegsTxBeReg59_u;

/*REG_PHY_TX_REGS_TX_BE_REG_5A 0x168 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 testPlugLocalEn00:1;	// Test Plug 00 Enable
		uint32 testPlugLocalEn01:1;	// Test Plug 00 Enable
		uint32 testPlugLocalEn02:1;	// Test Plug 00 Enable
		uint32 testPlugLocalEn03:1;	// Test Plug 00 Enable
		uint32 testPlugLocalEn04:1;	// Test Plug 00 Enable
		uint32 testPlugLocalEn05:1;	// Test Plug 00 Enable
		uint32 testPlugLocalEn06:1;	// Test Plug 00 Enable
		uint32 testPlugLocalEn07:1;	// Test Plug 00 Enable
		uint32 testPlugLocalEn08:1;	// Test Plug 08 Enable
		uint32 testPlugLocalEn09:1;
		uint32 testPlugLocalEn10:1;
		uint32 testPlugLocalEn11:1;
		uint32 testPlugLocalEn12:1;
		uint32 reserved0:3;
		uint32 tx0FeTestPlugMuxControl:2;	// Tx0 Front End Test Plug Mux Control
		uint32 tx1FeTestPlugMuxControl:2;	// Tx1 Front End Test Plug Mux Control
		uint32 testPlugStb4080En:1;	// Test Plug strobe control
		uint32 testPlugStb4080Sel:1;	// Test Plug strobe select
		uint32 tx2FeTestPlugMuxControl:2;	// Tx2 Front End Test Plug Mux Control
		uint32 filter2Tb:2;	// choose first interpolation filter to test bus else amp_gain_scale
		uint32 tx3FeTestPlugMuxControl:2;
		uint32 txBfSm2TbRate:4;
	} bitFields;
} RegPhyTxRegsTxBeReg5A_u;

/*REG_PHY_TX_REGS_TX_BE_REG_5B 0x16C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 testBusLowOutSelect:2;	// Test Plug Mux Out Low Selector
		uint32 testBusMidOutSelect:2;	// Test Plug Mux Out Mid Selector
		uint32 testBusHighOutSelect:2;	// Test Plug Mux Out High Selector
		uint32 testBusFourOutSelect:2;
		uint32 reserved0:24;
	} bitFields;
} RegPhyTxRegsTxBeReg5B_u;

/*REG_PHY_TX_REGS_TX_BE_REG_5C 0x170 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 modem11BDtCoeffK0:6;	// Modem 802.11B Detectors K coefficent 0
		uint32 modem11BDtCoeffK1:6;	// Modem 802.11B Detectors K coefficent 1
		uint32 modem11BDtCoeffK2:6;	// Modem 802.11B Detectors K coefficent 2
		uint32 modem11BDtCoeffK3:6;	// Modem 802.11B Detectors K coefficent 3
		uint32 modem11BDtCoeffK4:6;	// Modem 802.11B Detectors K coefficent 4
		uint32 reserved0:2;
	} bitFields;
} RegPhyTxRegsTxBeReg5C_u;

/*REG_PHY_TX_REGS_TX_BE_REG_5D 0x174 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 modem11BDtCoeffK5:6;	// Modem 802.11B Detectors K coefficent 5
		uint32 modem11BDtCoeffK6:6;	// Modem 802.11B Detectors K coefficent 6
		uint32 modem11BDtCoeffK7:6;	// Modem 802.11B Detectors K coefficent 7
		uint32 modem11BDtCoeffK8:6;	// Modem 802.11B Detectors K coefficent 8
		uint32 modem11BDtCoeffK9:6;	// Modem 802.11B Detectors K coefficent 9
		uint32 reserved0:2;
	} bitFields;
} RegPhyTxRegsTxBeReg5D_u;

/*REG_PHY_TX_REGS_TX_BE_REG_5E 0x178 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 modem11BDtCoeffG0:6;	// Modem 802.11B Detectors G coefficent 0
		uint32 modem11BDtCoeffG1:6;	// Modem 802.11B Detectors G coefficent 1
		uint32 modem11BDtCoeffG2:6;	// Modem 802.11B Detectors G coefficent 2
		uint32 modem11BDtCoeffG3:6;	// Modem 802.11B Detectors G coefficent 3
		uint32 modem11BDtCoeffG4:6;	// Modem 802.11B Detectors G coefficent 4
		uint32 reserved0:2;
	} bitFields;
} RegPhyTxRegsTxBeReg5E_u;

/*REG_PHY_TX_REGS_TX_BE_REG_5F 0x17C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 modem11BDtCoeffG5:6;	// Modem 802.11B Detectors G coefficent 5
		uint32 modem11BDtCoeffG6:6;	// Modem 802.11B Detectors G coefficent 6
		uint32 modem11BDtCoeffG7:6;	// Modem 802.11B Detectors G coefficent 7
		uint32 modem11BDtCoeffG8:6;	// Modem 802.11B Detectors G coefficent 8
		uint32 modem11BDtCoeffG9:6;	// Modem 802.11B Detectors G coefficent 9
		uint32 reserved0:2;
	} bitFields;
} RegPhyTxRegsTxBeReg5F_u;

/*REG_PHY_TX_REGS_TX_BE_REG_60 0x180 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 modem11BDtCoeffCorrel0:5;	// Modem 802.11B Detectors corelator coefficent 0
		uint32 modem11BDtCoeffCorrel1:5;	// Modem 802.11B Detectors corelator coefficent 1
		uint32 reserved0:22;
	} bitFields;
} RegPhyTxRegsTxBeReg60_u;

/*REG_PHY_TX_REGS_TX_BE_REG_61 0x184 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 r1PilotsLongNcbTbl:2;	// Map table for R1 pilots Long Pre Non CB
		uint32 reserved0:2;
		uint32 r1PilotsLongFcbTbl:2;	// Map table for R1 pilots Long Pre Full CB
		uint32 reserved1:26;
	} bitFields;
} RegPhyTxRegsTxBeReg61_u;

/*REG_PHY_TX_REGS_TX_FE_REG_62 0x188 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 windowing80Tap0:3;	// tap0 for 80Mhz windowing
		uint32 reserved0:1;
		uint32 windowing80Tap1:3;	// tap1 for 80Mhz windowing
		uint32 reserved1:1;
		uint32 windowing80Tap2:3;	// tap2 for 80Mhz windowing
		uint32 reserved2:1;
		uint32 windowing80Tap3:3;	// tap0 for 40Mhz windowing
		uint32 reserved3:1;
		uint32 windowing80Tap4:3;	// tap1 for 40Mhz windowing
		uint32 reserved4:1;
		uint32 txIqTdEnable:1;	// tx iq td enable
		uint32 txIqTdBypass:1;	// tx_iq_td_bypass
		uint32 reserved5:10;
	} bitFields;
} RegPhyTxRegsTxFeReg62_u;

/*REG_PHY_TX_REGS_TX0_FE_REG_07 0x400 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx0RoundInit:4;	// Tx Interpolation Out Scale
		uint32 tx0RoundInitQuarter:4;	// Tx Interpolation Out Scale HB 20/40 or 40/80
		uint32 reserved0:24;
	} bitFields;
} RegPhyTxRegsTx0FeReg07_u;

/*REG_PHY_TX_REGS_TX0_FE_REG_0D 0x40C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx0PnGenMode:1;	// Tx PN Noise Generator Enable
		uint32 reserved0:31;
	} bitFields;
} RegPhyTxRegsTx0FeReg0D_u;

/*REG_PHY_TX_REGS_TX0_FE_REG_0E 0x410 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx0AfeIdleQData:12;	// AFE Idle_Q_data
		uint32 reserved0:4;
		uint32 tx0AfeIdleIData:12;	// AFE Idle I data
		uint32 reserved1:4;
	} bitFields;
} RegPhyTxRegsTx0FeReg0E_u;

/*REG_PHY_TX_REGS_TX0_FE_REG_14F 0x418 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx0RoundInitEight:4;	// Tx Interpolation Out Scale in filter of 20/80
		uint32 tx0IDuplicate:1;
		uint32 tx0QDuplicate:1;
		uint32 reserved0:2;
		uint32 tx0FlipIEn:1;	// invert i
		uint32 tx0FlipQEn:1;	// invert q
		uint32 tx0SwapIQEn:1;	// swap iq
		uint32 reserved1:1;
		uint32 tx0IqTdF1:10;	// iq ssb filter f1
		uint32 tx0IqTdF2:10;	// iq ssb filter f2
	} bitFields;
} RegPhyTxRegsTx0FeReg14F_u;

/*REG_PHY_TX_REGS_TX0_FE_REG_150 0x41C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx0IqTdF3:10;	// iq ssb filter f3
		uint32 reserved0:6;
		uint32 tx0IqTdF4:10;	// iq ssb filter f4
		uint32 reserved1:6;
	} bitFields;
} RegPhyTxRegsTx0FeReg150_u;

/*REG_PHY_TX_REGS_TX0_FE_REG_151 0x420 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx0IqTdF120:10;
		uint32 reserved0:6;
		uint32 tx0IqTdF220:10;
		uint32 reserved1:6;
	} bitFields;
} RegPhyTxRegsTx0FeReg151_u;

/*REG_PHY_TX_REGS_TX0_FE_REG_152 0x424 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx0IqTdF320:10;
		uint32 reserved0:6;
		uint32 tx0IqTdF420:10;
		uint32 reserved1:6;
	} bitFields;
} RegPhyTxRegsTx0FeReg152_u;

/*REG_PHY_TX_REGS_TX0_FE_REG_153 0x428 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx0IqTdF140:10;
		uint32 reserved0:6;
		uint32 tx0IqTdF240:10;
		uint32 reserved1:6;
	} bitFields;
} RegPhyTxRegsTx0FeReg153_u;

/*REG_PHY_TX_REGS_TX0_FE_REG_154 0x42C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx0IqTdF340:10;
		uint32 reserved0:6;
		uint32 tx0IqTdF440:10;
		uint32 reserved1:6;
	} bitFields;
} RegPhyTxRegsTx0FeReg154_u;

/*REG_PHY_TX_REGS_TX1_FE_REG_07 0x600 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx1RoundInit:4;	// Tx Interpolation Out Scale
		uint32 tx1RoundInitQuarter:4;	// Tx Interpolation Out Scale HB
		uint32 reserved0:24;
	} bitFields;
} RegPhyTxRegsTx1FeReg07_u;

/*REG_PHY_TX_REGS_TX1_FE_REG_0D 0x60C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx1PnGenMode:1;	// Tx PN Noise Generator Enable
		uint32 reserved0:31;
	} bitFields;
} RegPhyTxRegsTx1FeReg0D_u;

/*REG_PHY_TX_REGS_TX1_FE_REG_0E 0x610 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx1AfeIdleQData:12;	// AFE Idle_Q_data
		uint32 reserved0:4;
		uint32 tx1AfeIdleIData:12;	// AFE Idle I data
		uint32 reserved1:4;
	} bitFields;
} RegPhyTxRegsTx1FeReg0E_u;

/*REG_PHY_TX_REGS_TX1_FE_REG_14F 0x618 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx1RoundInitEight:4;	// Tx Interpolation Out Scale in filter of 20/80
		uint32 tx1IDuplicate:1;
		uint32 tx1QDuplicate:1;
		uint32 reserved0:2;
		uint32 tx1FlipIEn:1;	// invert i
		uint32 tx1FlipQEn:1;	// invert q
		uint32 tx1SwapIQEn:1;	// swap iq
		uint32 reserved1:1;
		uint32 tx1IqTdF1:10;	// iq ssb filter f1
		uint32 tx1IqTdF2:10;	// iq ssb filter f2
	} bitFields;
} RegPhyTxRegsTx1FeReg14F_u;

/*REG_PHY_TX_REGS_TX1_FE_REG_150 0x61C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx1IqTdF3:10;	// iq ssb filter f3
		uint32 reserved0:6;
		uint32 tx1IqTdF4:10;	// iq ssb filter f4
		uint32 reserved1:6;
	} bitFields;
} RegPhyTxRegsTx1FeReg150_u;

/*REG_PHY_TX_REGS_TX1_FE_REG_151 0x620 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx1IqTdF120:10;
		uint32 reserved0:6;
		uint32 tx1IqTdF220:10;
		uint32 reserved1:6;
	} bitFields;
} RegPhyTxRegsTx1FeReg151_u;

/*REG_PHY_TX_REGS_TX1_FE_REG_152 0x624 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx1IqTdF320:10;
		uint32 reserved0:6;
		uint32 tx1IqTdF420:10;
		uint32 reserved1:6;
	} bitFields;
} RegPhyTxRegsTx1FeReg152_u;

/*REG_PHY_TX_REGS_TX1_FE_REG_153 0x628 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx1IqTdF140:10;
		uint32 reserved0:6;
		uint32 tx1IqTdF240:10;
		uint32 reserved1:6;
	} bitFields;
} RegPhyTxRegsTx1FeReg153_u;

/*REG_PHY_TX_REGS_TX1_FE_REG_154 0x62C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx1IqTdF340:10;
		uint32 reserved0:6;
		uint32 tx1IqTdF440:10;
		uint32 reserved1:6;
	} bitFields;
} RegPhyTxRegsTx1FeReg154_u;

/*REG_PHY_TX_REGS_TX2_FE_REG_07 0x800 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx2RoundInit:4;	// Tx Interpolation Out Scale
		uint32 tx2RoundInitQuarter:4;	// Tx Interpolation Out Scale HB
		uint32 reserved0:24;
	} bitFields;
} RegPhyTxRegsTx2FeReg07_u;

/*REG_PHY_TX_REGS_TX2_FE_REG_0D 0x80C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx2PnGenMode:1;	// Tx PN Noise Generator Enable
		uint32 reserved0:31;
	} bitFields;
} RegPhyTxRegsTx2FeReg0D_u;

/*REG_PHY_TX_REGS_TX2_FE_REG_0E 0x810 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx2AfeIdleQData:12;	// AFE Idle_Q_data
		uint32 reserved0:4;
		uint32 tx2AfeIdleIData:12;	// AFE Idle I data
		uint32 reserved1:4;
	} bitFields;
} RegPhyTxRegsTx2FeReg0E_u;

/*REG_PHY_TX_REGS_TX2_FE_REG_14F 0x818 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx2RoundInitEight:4;	// Tx Interpolation Out Scale HB 2080
		uint32 tx2IDuplicate:1;
		uint32 tx2QDuplicate:1;
		uint32 reserved0:2;
		uint32 tx2FlipIEn:1;	// invert i
		uint32 tx2FlipQEn:1;	// invert q
		uint32 tx2SwapIQEn:1;	// swap iq
		uint32 reserved1:1;
		uint32 tx2IqTdF1:10;	// iq ssb filter f1
		uint32 tx2IqTdF2:10;	// iq ssb filter f2
	} bitFields;
} RegPhyTxRegsTx2FeReg14F_u;

/*REG_PHY_TX_REGS_TX2_FE_REG_150 0x81C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx2IqTdF3:10;	// iq ssb filter f3
		uint32 reserved0:6;
		uint32 tx2IqTdF4:10;	// iq ssb filter f4
		uint32 reserved1:6;
	} bitFields;
} RegPhyTxRegsTx2FeReg150_u;

/*REG_PHY_TX_REGS_TX2_FE_REG_151 0x820 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx2IqTdF120:10;
		uint32 reserved0:6;
		uint32 tx2IqTdF220:10;
		uint32 reserved1:6;
	} bitFields;
} RegPhyTxRegsTx2FeReg151_u;

/*REG_PHY_TX_REGS_TX2_FE_REG_152 0x824 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx2IqTdF320:10;
		uint32 reserved0:6;
		uint32 tx2IqTdF420:10;
		uint32 reserved1:6;
	} bitFields;
} RegPhyTxRegsTx2FeReg152_u;

/*REG_PHY_TX_REGS_TX2_FE_REG_153 0x828 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx2IqTdF140:10;
		uint32 reserved0:6;
		uint32 tx2IqTdF240:10;
		uint32 reserved1:6;
	} bitFields;
} RegPhyTxRegsTx2FeReg153_u;

/*REG_PHY_TX_REGS_TX2_FE_REG_154 0x82C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx2IqTdF340:10;
		uint32 reserved0:6;
		uint32 tx2IqTdF440:10;
		uint32 reserved1:6;
	} bitFields;
} RegPhyTxRegsTx2FeReg154_u;

/*REG_PHY_TX_REGS_TX3_FE_REG_07 0xA00 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx3RoundInit:4;	// Tx Interpolation Out Scale
		uint32 tx3RoundInitQuarter:4;	// Tx Interpolation Out Scale HB
		uint32 reserved0:24;
	} bitFields;
} RegPhyTxRegsTx3FeReg07_u;

/*REG_PHY_TX_REGS_TX3_FE_REG_0D 0xA0C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx3PnGenMode:1;	// Tx PN Noise Generator Enable
		uint32 reserved0:31;
	} bitFields;
} RegPhyTxRegsTx3FeReg0D_u;

/*REG_PHY_TX_REGS_TX3_FE_REG_0E 0xA10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx3AfeIdleQData:12;	// AFE Idle_Q_data
		uint32 reserved0:4;
		uint32 tx3AfeIdleIData:12;	// AFE Idle I data
		uint32 reserved1:4;
	} bitFields;
} RegPhyTxRegsTx3FeReg0E_u;

/*REG_PHY_TX_REGS_TX3_FE_REG_14F 0xA18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx3RoundInitEight:4;	// Tx Interpolation Out Scale HB 2080
		uint32 tx3IDuplicate:1;
		uint32 tx3QDuplicate:1;
		uint32 reserved0:2;
		uint32 tx3FlipIEn:1;	// invert i
		uint32 tx3FlipQEn:1;	// invert q
		uint32 tx3SwapIQEn:1;	// swap iq
		uint32 reserved1:1;
		uint32 tx3IqTdF1:10;	// iq ssb filter f1
		uint32 tx3IqTdF2:10;	// iq ssb filter f2
	} bitFields;
} RegPhyTxRegsTx3FeReg14F_u;

/*REG_PHY_TX_REGS_TX3_FE_REG_150 0xA1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx3IqTdF3:10;	// iq ssb filter f3
		uint32 reserved0:6;
		uint32 tx3IqTdF4:10;	// iq ssb filter f4
		uint32 reserved1:6;
	} bitFields;
} RegPhyTxRegsTx3FeReg150_u;

/*REG_PHY_TX_REGS_TX3_FE_REG_151 0xA20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx3IqTdF120:10;
		uint32 reserved0:6;
		uint32 tx3IqTdF220:10;
		uint32 reserved1:6;
	} bitFields;
} RegPhyTxRegsTx3FeReg151_u;

/*REG_PHY_TX_REGS_TX3_FE_REG_152 0xA24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx3IqTdF320:10;
		uint32 reserved0:6;
		uint32 tx3IqTdF420:10;
		uint32 reserved1:6;
	} bitFields;
} RegPhyTxRegsTx3FeReg152_u;

/*REG_PHY_TX_REGS_TX3_FE_REG_153 0xA28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx3IqTdF140:10;
		uint32 reserved0:6;
		uint32 tx3IqTdF240:10;
		uint32 reserved1:6;
	} bitFields;
} RegPhyTxRegsTx3FeReg153_u;

/*REG_PHY_TX_REGS_TX3_FE_REG_154 0xA2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx3IqTdF340:10;
		uint32 reserved0:6;
		uint32 tx3IqTdF440:10;
		uint32 reserved1:6;
	} bitFields;
} RegPhyTxRegsTx3FeReg154_u;

/*REG_PHY_TX_REGS_TX_FE_REG_07 0xC20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txFilter11BRoundInit:4;	// Tx Interpolation Out Scale in 802_11B mode
		uint32 txFilter11BRoundInitQuarter:4;	// Tx Interpolation Out Scale in 802_11B mode 20/40 or 40/80
		uint32 reserved0:8;
		uint32 ifftInShift:2;	// ifft right shift with sign extension
		uint32 ifftOutShift:3;	// ifft left shift with sign extension , -1 - shift left , 1-2 shift right
		uint32 ifftInShiftCb40:2;	// ifft right shift with sign extension
		uint32 ifftOutShiftCb40:3;	// ifft left shift with sign extension , -1 - shift left , 1-2 shift right
		uint32 ifftInShiftCb80:2;	// ifft right shift with sign extension
		uint32 ifftOutShiftCb80:3;	// ifft left shift with sign extension , -1 - shift left , 1-2 shift right
		uint32 reserved1:1;
	} bitFields;
} RegPhyTxRegsTxFeReg07_u;

/*REG_PHY_TX_REGS_TX_FE_REG_14F 0xC24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txFilter11BRoundInitEight:4;	// Tx Interpolation Out Scale in 802_11B mode 20/80
		uint32 reserved0:28;
	} bitFields;
} RegPhyTxRegsTxFeReg14F_u;

/*REG_PHY_TX_REGS_TX_BE_RAM_RM_REG 0xC28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tpcRamRm:3;	// LDPC PRAM rm reg
		uint32 reserved0:1;
		uint32 interRam00Rm:3;	// Interleaver 0 RAM 0 rm reg
		uint32 reserved1:1;
		uint32 interRam01Rm:3;	// Interleaver 0 RAM 1 rm reg
		uint32 reserved2:1;
		uint32 interRam02Rm:3;	// Interleaver 0 RAM 2 rm reg
		uint32 reserved3:1;
		uint32 interRam03Rm:3;	// Interleaver 0 RAM 3 rm reg
		uint32 reserved4:13;
	} bitFields;
} RegPhyTxRegsTxBeRamRmReg_u;

/*REG_PHY_TX_REGS_TX0_FE_RAM_RM_REG0 0xC2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx0CycExtPreambleRamRm:3;
		uint32 reserved0:1;
		uint32 tx0RippleRamRm:3;	// Ripple RAM rm reg
		uint32 reserved1:1;
		uint32 tx0IqRamRm:3;	// IQ RAM rm reg
		uint32 reserved2:1;
		uint32 tx0FftRam0Rm:3;	// FFT Odd 0 rm reg
		uint32 reserved3:1;
		uint32 tx0FftRam1Rm:3;	// FFT Odd 1 rm reg
		uint32 reserved4:13;
	} bitFields;
} RegPhyTxRegsTx0FeRamRmReg0_u;

/*REG_PHY_TX_REGS_TX1_FE_RAM_RM_REG0 0xC30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx1CycExtPreambleRamRm:3;
		uint32 reserved0:1;
		uint32 tx1RippleRamRm:3;	// Ripple RAM rm reg
		uint32 reserved1:1;
		uint32 tx1IqRamRm:3;	// IQ RAM rm reg
		uint32 reserved2:1;
		uint32 tx1FftRam0Rm:3;	// FFT Odd 0 rm reg
		uint32 reserved3:1;
		uint32 tx1FftRam1Rm:3;	// FFT Odd 1 rm reg
		uint32 reserved4:13;
	} bitFields;
} RegPhyTxRegsTx1FeRamRmReg0_u;

/*REG_PHY_TX_REGS_TX_GLBL_RAM_RM_REG 0xC34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 longPreambleRamRm:3;	// Long preamble ram rm reg
		uint32 reserved0:29;
	} bitFields;
} RegPhyTxRegsTxGlblRamRmReg_u;

/*REG_PHY_TX_REGS_TX_2ND_FILTER 0xC38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx2NdFilterC0:10;	// tx 2nd interpolation filter coef0
		uint32 tx2NdFilterC1:10;	// tx 2nd interpolation filter coef1
		uint32 tx2NdFilterC2:10;	// tx 2nd interpolation filter coef2
		uint32 tx2NdFilterBypass:1;	// tx 2nd interpolation filter bypass
		uint32 reserved0:1;
	} bitFields;
} RegPhyTxRegsTx2NdFilter_u;

/*REG_PHY_TX_REGS_TX_SYNC_FIFO 0xC3C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:2;
		uint32 fifoInput2Tb:1;	// 1-fifo input to test bus, 0-fifo output to test bus
		uint32 gen11BEvenStrbEn:1;	// generate even strobes for 11b (should be enable only when no interpolation filter is used) 
		uint32 reserved1:28;
	} bitFields;
} RegPhyTxRegsTxSyncFifo_u;

/*REG_PHY_TX_REGS_TX_FFT_SHARING 0xC40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fftShareOverrideControl:1;	// fft_share_override_control
		uint32 reserved0:3;
		uint32 fft0ControlRx:1;	// fft0_control_rx
		uint32 fft1ControlRx:1;	// fft1_control_rx
		uint32 fft2ControlRx:1;	// fft2_control_rx
		uint32 fft3ControlRx:1;
		uint32 fftGen3SatFixEn:1;	// fft gen3 input saturation bug fix enable (default on) 
		uint32 reserved1:23;
	} bitFields;
} RegPhyTxRegsTxFftSharing_u;

/*REG_PHY_TX_REGS_TX2_CONTROL 0xC48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 flatBypass:1;	// flat bypass in 3ss and 3antennas or 2ss and 2antenna using default matrix
		uint32 csdBypass:1;	// csd bypass
		uint32 antennaSelectionEn:1;
		uint32 antennaSelection11BEn:1;
		uint32 reserved0:8;
		uint32 htShortEqualizer:12;	// ht short energy equalizer
		uint32 reserved1:8;
	} bitFields;
} RegPhyTxRegsTx2Control_u;

/*REG_PHY_TX_REGS_NOISE_SHAPING 0xC4C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txNoiseShapingEn:1;	// noise shaping enable
		uint32 txNoiseShapingA1:12;	// noise shaping A1 parameter
		uint32 txNoiseShapingMode:2;	// noise shaping mode
		uint32 reserved0:17;
	} bitFields;
} RegPhyTxRegsNoiseShaping_u;

/*REG_PHY_TX_REGS_DEBUG 0xC50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:4;
		uint32 afeTxStreamerMode:2;	// use streamer for 1/2/3 ants
		uint32 afeTxStreamerDuplicate:1;
		uint32 reserved1:1;
		uint32 tx2RippleRamRm:3;	// tx2 ripple RAM rm
		uint32 tx2IqRamRm:3;	// tx2 iq RAM rm
		uint32 tx2FftRam0Rm:3;	// tx2 fft_0 fft4_o RAM rm
		uint32 tx2FftRam1Rm:3;	// tx2 fft_1 fft4_o RAM rm
		uint32 reserved2:6;
		uint32 tx2CycExtPreambleRamRm:3;	// tx2 cyc extention preamble RAM rm
		uint32 reserved3:3;
	} bitFields;
} RegPhyTxRegsDebug_u;

/*REG_PHY_TX_REGS_BEAM_FORMING0 0xC5C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 legacyStfBoost:1;	// STF boost for Legacy QBF: , 0: none, 1: 3dB 
		uint32 txPowerLoadGainReady:1;
		uint32 reserved0:20;
		uint32 txSpCalibEn:1;	// calibration enable
		uint32 reserved1:9;
	} bitFields;
} RegPhyTxRegsBeamForming0_u;

/*REG_PHY_TX_REGS_TX3_FE_RAM_RM_REG0 0xC60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx3CycExtPreambleRamRm:3;	// Cyc extension rm reg
		uint32 reserved0:5;
		uint32 tx3RippleRamRm:3;	// Ripple RAM rm reg
		uint32 reserved1:1;
		uint32 tx3IqRamRm:3;	// IQ RAM rm reg
		uint32 reserved2:1;
		uint32 tx3FftRam0Rm:3;	// FFT Odd 0 rm reg
		uint32 reserved3:1;
		uint32 tx3FftRam1Rm:3;	// FFT Odd 1 rm reg
		uint32 reserved4:9;
	} bitFields;
} RegPhyTxRegsTx3FeRamRmReg0_u;

/*REG_PHY_TX_REGS_TX_BE_RAM_RM_REG1 0xC64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ldpcStandardEram1Rm:3;	// Standard LDPC ERAM1 rm reg
		uint32 reserved0:1;
		uint32 ldpcStandardEram2Rm:3;	// Standard LDPC ERAM2 rm reg
		uint32 reserved1:1;
		uint32 ldpcStandardEram0Rm:3;	// Standard LDPC ERAM0 rm reg
		uint32 reserved2:5;
		uint32 txCalRamAnt0Rm:3;	// tx_cal_ram_ant_0_rm
		uint32 reserved3:1;
		uint32 txCalRamAnt1Rm:3;	// tx_cal_ram_ant_1_rm
		uint32 reserved4:1;
		uint32 txCalRamAnt2Rm:3;	// tx_cal_ram_ant_2_rm
		uint32 reserved5:1;
		uint32 txCalRamAnt3Rm:3;	// tx_cal_ram_ant_3_rm
		uint32 reserved6:1;
	} bitFields;
} RegPhyTxRegsTxBeRamRmReg1_u;

/*REG_PHY_TX_REGS_MEMORY_CONTROL 0xC68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 memGlobalRm:2;	// Global RM value for rams
		uint32 memGlobalTestMode:1;	// Global test mode for rams
		uint32 bistScrBypass:1;	// Global bist scrambler bypass for rams
		uint32 reserved0:28;
	} bitFields;
} RegPhyTxRegsMemoryControl_u;

/*REG_PHY_TX_REGS_SIG_CRC_DEFS 0xC6C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sigCrcType:8;	// sig_crc_type
		uint32 sigCrcInit:8;	// sig_crc_init
		uint32 sigCrcLen:4;	// sig_crc_len
		uint32 sigCrcResultInvert:1;	// sig_crc_result_invert
		uint32 reserved0:11;
	} bitFields;
} RegPhyTxRegsSigCrcDefs_u;

/*REG_PHY_TX_REGS_BB_AIR_TIME_CALC_START 0xC70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbAirTimeCalcStart:1;	// bb_air_time_calc_start
		uint32 reserved0:31;
	} bitFields;
} RegPhyTxRegsBbAirTimeCalcStart_u;

/*REG_PHY_TX_REGS_BB_AIR_TIME_CALC_MODE 0xC74 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbAirTimeCalcLdpc:1;	// bb_air_time_calc_ldpc
		uint32 bbAirTimeCalcScp:1;	// bb_air_time_calc_scp
		uint32 bbAirTimeCalcStbc:2;	// bb_air_time_calc_stbc
		uint32 bbAirTimeCalcMcs:6;	// bb_air_time_calc_mcs
		uint32 bbAirTimeCalcBw:2;	// bb_air_time_calc_bw
		uint32 bbAirTimeCalcPacketMode:2;	// bb_air_time_calc_packet_mode
		uint32 bbAirTimeCalcNess:2;	// bb_air_time_calc_ness
		uint32 bbAirTimeCalcNss:2;	// bb_air_time_calc_nss
		uint32 bbAirTimeCalcRxTxN:1;	// bb_air_time_calc_rx_tx_n
		uint32 bbAirTimeCalcPreamble:1;	// bb_air_time_calc_preamble
		uint32 bbAirTimeCalcRxSigA2B1:1;	// bb_air_time_calc_rx_sig_a2_b1
		uint32 bbAirTimeCalcRxSigA2B3:1;	// bb_air_time_calc_rx_sig_a2_b3
		uint32 bbAirTimeCalcMuStage:2;	// bb_air_time_calc_mu_stage
		uint32 reserved0:8;
	} bitFields;
} RegPhyTxRegsBbAirTimeCalcMode_u;

/*REG_PHY_TX_REGS_BB_AIR_TIME_CALC_LENGTHS 0xC78 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbAirTimeCalcPacketLength:20;	// bb_air_time_calc_packet_length
		uint32 bbAirTimeCalcLLength:12;	// bb_air_time_calc_l_length
	} bitFields;
} RegPhyTxRegsBbAirTimeCalcLengths_u;

/*REG_PHY_TX_REGS_BB_AIR_TIME_CALC_RESULTS1 0xC7C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbAirTimeCalcDone:1;	// bb_air_time_calc_done
		uint32 bbAirTimeCalcLLengthExceed:1;	// bb_air_time_calc_l_length_exceed
		uint32 bbAirTimeCalcSigA2B1:1;	// bb_air_time_calc_sig_a2_b1
		uint32 bbAirTimeCalcSigA2B3:1;	// bb_air_time_calc_sig_a2_b3
		uint32 bbAirTimeCalcCodingRate:2;	// bb_air_time_calc_coding_rate
		uint32 bbAirTimeCalcModulation:3;	// bb_air_time_calc_modulation
		uint32 bbAirTimeCalcViterbiNEs:2;	// bb_air_time_calc_viterbi_n_es
		uint32 reserved0:1;
		uint32 bbAirTimeCalcNumPaddingBits:14;	// bb_air_time_calc_num_padding_bits
		uint32 reserved1:6;
	} bitFields;
} RegPhyTxRegsBbAirTimeCalcResults1_u;

/*REG_PHY_TX_REGS_BB_AIR_TIME_CALC_RESULTS2 0xC80 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbAirTimeCalcPhyRateResult:15;	// bb_air_time_calc_phy_rate_result
		uint32 reserved0:1;
		uint32 bbAirTimeCalcNumSymResult:15;	// bb_air_time_calc_num_sym_result
		uint32 reserved1:1;
	} bitFields;
} RegPhyTxRegsBbAirTimeCalcResults2_u;

/*REG_PHY_TX_REGS_BB_AIR_TIME_CALC_RESULTS3 0xC84 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbAirTimeCalcAirTimeResult:17;	// bb_air_time_calc_air_time_result
		uint32 reserved0:3;
		uint32 bbAirTimeCalcLLengthResult:12;	// bb_air_time_calc_l_length_result
	} bitFields;
} RegPhyTxRegsBbAirTimeCalcResults3_u;

/*REG_PHY_TX_REGS_BB_AIR_TIME_CALC_RESULTS4 0xC88 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbAirTimeCalcLdpcRemNshrtNcw:15;	// bb_air_time_calc_ldpc_rem_nshrt_ncw
		uint32 reserved0:1;
		uint32 bbAirTimeCalcLdpcRemNpuncNcw:15;	// bb_air_time_calc_ldpc_rem_npunc_ncw
		uint32 reserved1:1;
	} bitFields;
} RegPhyTxRegsBbAirTimeCalcResults4_u;

/*REG_PHY_TX_REGS_BB_AIR_TIME_CALC_RESULTS5 0xC8C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbAirTimeCalcLdpcRemNrepNcw:15;	// bb_air_time_calc_ldpc_rem_nrep_ncw
		uint32 reserved0:1;
		uint32 bbAirTimeCalcLdpcNData:11;	// bb_air_time_calc_ldpc_n_data
		uint32 reserved1:5;
	} bitFields;
} RegPhyTxRegsBbAirTimeCalcResults5_u;

/*REG_PHY_TX_REGS_BB_AIR_TIME_CALC_RESULTS6 0xC90 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbAirTimeCalcLdpcNParity:11;	// bb_air_time_calc_ldpc_n_parity
		uint32 reserved0:1;
		uint32 bbAirTimeCalcLdpcNRepeat:13;	// bb_air_time_calc_ldpc_n_repeat
		uint32 reserved1:7;
	} bitFields;
} RegPhyTxRegsBbAirTimeCalcResults6_u;

/*REG_PHY_TX_REGS_BB_AIR_TIME_CALC_RESULTS7 0xC94 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbAirTimeCalcLdpcCwType:2;	// bb_air_time_calc_ldpc_cw_type
		uint32 reserved0:2;
		uint32 bbAirTimeCalcLdpcNCw:15;	// bb_air_time_calc_ldpc_n_cw
		uint32 reserved1:13;
	} bitFields;
} RegPhyTxRegsBbAirTimeCalcResults7_u;

/*REG_PHY_TX_REGS_TX0_AFE_OFFSET_GAIN0_REG 0xC98 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx0AfeQOffsetGain0:10;	// AFE Magnitude limit Q offset
		uint32 tx0AfeIOffsetGain0:10;	// AFE Magnitude limit I offset
		uint32 tx0AfeQOffsetGain1:10;
		uint32 reserved0:2;
	} bitFields;
} RegPhyTxRegsTx0AfeOffsetGain0Reg_u;

/*REG_PHY_TX_REGS_TX0_AFE_OFFSET_GAIN1_REG 0xC9C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx0AfeIOffsetGain1:10;	// AFE Magnitude limit Q offset
		uint32 tx0AfeQOffsetGain2:10;	// AFE Magnitude limit I offset
		uint32 tx0AfeIOffsetGain2:10;
		uint32 reserved0:2;
	} bitFields;
} RegPhyTxRegsTx0AfeOffsetGain1Reg_u;

/*REG_PHY_TX_REGS_TX0_TX1_AMP_GAIN_F 0xCA0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 filt0OutAmpGainF:9;	// Ant#0 Gain scaling factor number 16 (f)
		uint32 reserved0:3;
		uint32 filt1OutAmpGainF:9;	// Ant#1 Gain scaling factor number 16 (f)
		uint32 digitalTpcGainEn:1;
		uint32 reserved1:10;
	} bitFields;
} RegPhyTxRegsTx0Tx1AmpGainF_u;

/*REG_PHY_TX_REGS_TX2_TX3_AMP_GAIN_F 0xCA4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 filt2OutAmpGainF:9;	// Ant#2 Gain scaling factor number 16 (f)
		uint32 filt3OutAmpGainF:9;
		uint32 reserved0:14;
	} bitFields;
} RegPhyTxRegsTx2Tx3AmpGainF_u;

/*REG_PHY_TX_REGS_DT_SIGNAL 0xCA8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dtSignal:22;	// antenna 0 signal quality
		uint32 reserved0:10;
	} bitFields;
} RegPhyTxRegsDtSignal_u;

/*REG_PHY_TX_REGS_DT_POWER 0xCAC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dtPower:22;	// antenna 0 detected power high bits
		uint32 reserved0:10;
	} bitFields;
} RegPhyTxRegsDtPower_u;

/*REG_PHY_TX_REGS_BEST_DT_SIGNAL_POWER 0xCB0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bestDtSignalQ:9;	// Best antenna signal quality (log)
		uint32 reserved0:3;
		uint32 bestDtPower:9;	// Best antenna detected power(log)
		uint32 reserved1:11;
	} bitFields;
} RegPhyTxRegsBestDtSignalPower_u;

/*REG_PHY_TX_REGS_LEGACY_TIMERS 0xCB4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 legacyDataReadyLimit:16;	// legacy_data_ready_limit
		uint32 legacyBfReadyLimit:16;	// legacy_bf_ready_limit
	} bitFields;
} RegPhyTxRegsLegacyTimers_u;

/*REG_PHY_TX_REGS_NON_LEGACY_BF_READY 0xCB8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 nonLegacyBfReadyLimit:16;	// non_legacy_bf_ready_limit
		uint32 reserved0:16;
	} bitFields;
} RegPhyTxRegsNonLegacyBfReady_u;

/*REG_PHY_TX_REGS_TX_BE_RAM1_RM_REG 0xCBC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 interRam10Rm:3;	// Interleaver 1 RAM 0 rm reg
		uint32 reserved0:1;
		uint32 interRam11Rm:3;	// Interleaver 1 RAM 1 rm reg
		uint32 reserved1:1;
		uint32 interRam12Rm:3;	// Interleaver 1 RAM 2 rm reg
		uint32 reserved2:1;
		uint32 interRam13Rm:3;	// Interleaver 1 RAM 3 rm reg
		uint32 reserved3:17;
	} bitFields;
} RegPhyTxRegsTxBeRam1RmReg_u;

/*REG_PHY_TX_REGS_RATE_FILTER_RM_REG 0xCC0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rateFilterRamTxRm:3;	// rate_filter_ram_tx_rm
		uint32 reserved0:1;
		uint32 rateFilterRamRxRm:3;	// rate_filter_ram_rx_rm
		uint32 reserved1:25;
	} bitFields;
} RegPhyTxRegsRateFilterRmReg_u;

/*REG_PHY_TX_REGS_SPARE_REGS_0 0xCC4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 spareGpr0:32;	// Spare registers
	} bitFields;
} RegPhyTxRegsSpareRegs0_u;

/*REG_PHY_TX_REGS_SPARE_REGS_1 0xCC8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 spareGpr1:32;	// Spare registers
	} bitFields;
} RegPhyTxRegsSpareRegs1_u;

/*REG_PHY_TX_REGS_SPARE_REGS_2 0xCCC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 spareGpr2:32;	// Spare registers
	} bitFields;
} RegPhyTxRegsSpareRegs2_u;

/*REG_PHY_TX_REGS_SPARE_REGS_3 0xCD0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 spareGpr3:32;	// Spare registers
	} bitFields;
} RegPhyTxRegsSpareRegs3_u;

/*REG_PHY_TX_REGS_CUSTOM1_SM_REG 0xCD4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cddCustom12SsAnt1:4;	// custom1 cdd register for 2ss 
		uint32 cddCustom12SsAnt2:4;	// custom1 cdd register for 2ss 
		uint32 cddCustom12SsAnt3:4;	// custom1 cdd register for 2ss 
		uint32 reserved0:4;
		uint32 cddCustom13SsAnt1:4;	// custom1 cdd register for 3ss 
		uint32 cddCustom13SsAnt2:4;	// custom1 cdd register for 3ss 
		uint32 cddCustom13SsAnt3:4;	// custom1 cdd register for 3ss 
		uint32 reserved1:4;
	} bitFields;
} RegPhyTxRegsCustom1SmReg_u;

/*REG_PHY_TX_REGS_CUSTOM2_SM_REG 0xCD8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cddCustom22SsAnt1:4;	// custom2 cdd register for 2ss 
		uint32 cddCustom22SsAnt2:4;	// custom2 cdd register for 2ss 
		uint32 cddCustom22SsAnt3:4;	// custom2 cdd register for 2ss 
		uint32 reserved0:4;
		uint32 cddCustom23SsAnt1:4;	// custom2 cdd register for 3ss 
		uint32 cddCustom23SsAnt2:4;	// custom2 cdd register for 3ss 
		uint32 cddCustom23SsAnt3:4;	// custom2 cdd register for 3ss 
		uint32 reserved1:4;
	} bitFields;
} RegPhyTxRegsCustom2SmReg_u;

/*REG_PHY_TX_REGS_CYCLIC_INTERPOLATION_DPD0 0xCDC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cyclicInterpolatorEn:1;	// cyclic_interpolator_en
		uint32 calibrationInExplicit:1;	// The rules to operate the Calibration unit are: , not((Explicit or NDP) and calibrationInExplicit==0)
		uint32 dpdScale:2;	// 2 bit scaler for the dpd
		uint32 dpd0Bypass:1;
		uint32 dpd1Bypass:1;
		uint32 dpd2Bypass:1;
		uint32 dpd3Bypass:1;
		uint32 dpd0Gain:7;
		uint32 dpd1Gain:7;
		uint32 cycInterpRamRm:3;
		uint32 dpdRamRm:3;
		uint32 reserved0:4;
	} bitFields;
} RegPhyTxRegsCyclicInterpolationDpd0_u;

/*REG_PHY_TX_REGS_CYCLIC_INTERPOLATION_DPD1 0xCE0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dpd2Gain:7;
		uint32 dpd3Gain:7;
		uint32 reserved0:18;
	} bitFields;
} RegPhyTxRegsCyclicInterpolationDpd1_u;

/*REG_PHY_TX_REGS_VHT_HT_CDD_0 0xCE4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cdd2StsAnt1:4;	// cdd for flat mode
		uint32 cdd2StsAnt2:4;	// cdd for flat mode
		uint32 cdd2StsAnt3:4;	// cdd for flat mode
		uint32 cdd3StsAnt1:4;	// cdd for flat mode
		uint32 cdd3StsAnt2:4;	// cdd for flat mode
		uint32 cdd3StsAnt3:4;	// cdd for flat mode
		uint32 cdd4StsAnt1:4;	// cdd for flat mode
		uint32 cdd4StsAnt2:4;	// cdd for flat mode
	} bitFields;
} RegPhyTxRegsVhtHtCdd0_u;

/*REG_PHY_TX_REGS_VHT_HT_CDD_1 0xCE8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cdd4StsAnt3:4;	// cdd for flat mode
		uint32 reserved0:28;
	} bitFields;
} RegPhyTxRegsVhtHtCdd1_u;

/*REG_PHY_TX_REGS_TCR_TIME_LIMITS 0xCEC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txTcr0TimeLimit:8;	// tcr0 time limit 0.5usec
		uint32 txTcr2TimeLimit:9;	// tcr1 and tcr2 time limit 1usec
		uint32 reserved0:15;
	} bitFields;
} RegPhyTxRegsTcrTimeLimits_u;

/*REG_PHY_TX_REGS_TCR_LENGTH_TIME_LIMITS 0xCF0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txTcr4LegacyBfTimeLimit:9;	// tcr length for legacy + bf time limit 1.5 usec
		uint32 reserved0:3;
		uint32 txTcr4NonLegacyBfTimeLimit:12;	// tcr length for all othe cases (ht/vht/11b and legacy non bf) 11usec
		uint32 reserved1:8;
	} bitFields;
} RegPhyTxRegsTcrLengthTimeLimits_u;

/*REG_PHY_TX_REGS_END_OF_FRAME_DELIMETER 0xCF4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 endOfFrameDelimeter:32;
	} bitFields;
} RegPhyTxRegsEndOfFrameDelimeter_u;

/*REG_PHY_TX_REGS_TX_IMPLICIT_CALIBRATION 0xCF8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txImplicitBfCalibEn:1;
		uint32 txOnToTxReqThreshold:6;
		uint32 txRequestCntThreshold:5;
		uint32 txCalibrationJumpOccured:1;
		uint32 txCalibrationJumpVal:2;
		uint32 reserved0:17;
	} bitFields;
} RegPhyTxRegsTxImplicitCalibration_u;

/*REG_PHY_TX_REGS_TX1_AFE_OFFSET_GAIN0_REG 0xCFC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx1AfeQOffsetGain0:10;
		uint32 tx1AfeIOffsetGain0:10;
		uint32 tx1AfeQOffsetGain1:10;
		uint32 reserved0:2;
	} bitFields;
} RegPhyTxRegsTx1AfeOffsetGain0Reg_u;

/*REG_PHY_TX_REGS_TX1_AFE_OFFSET_GAIN1_REG 0xD00 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx1AfeIOffsetGain1:10;
		uint32 tx1AfeQOffsetGain2:10;
		uint32 tx1AfeIOffsetGain2:10;
		uint32 reserved0:2;
	} bitFields;
} RegPhyTxRegsTx1AfeOffsetGain1Reg_u;

/*REG_PHY_TX_REGS_TX2_AFE_OFFSET_GAIN0_REG 0xD04 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx2AfeQOffsetGain0:10;
		uint32 tx2AfeIOffsetGain0:10;
		uint32 tx2AfeQOffsetGain1:10;
		uint32 reserved0:2;
	} bitFields;
} RegPhyTxRegsTx2AfeOffsetGain0Reg_u;

/*REG_PHY_TX_REGS_TX2_AFE_OFFSET_GAIN1_REG 0xD08 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx2AfeIOffsetGain1:10;
		uint32 tx2AfeQOffsetGain2:10;
		uint32 tx2AfeIOffsetGain2:10;
		uint32 reserved0:2;
	} bitFields;
} RegPhyTxRegsTx2AfeOffsetGain1Reg_u;

/*REG_PHY_TX_REGS_TX3_AFE_OFFSET_GAIN0_REG 0xD0C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx3AfeQOffsetGain0:10;
		uint32 tx3AfeIOffsetGain0:10;
		uint32 tx3AfeQOffsetGain1:10;
		uint32 reserved0:2;
	} bitFields;
} RegPhyTxRegsTx3AfeOffsetGain0Reg_u;

/*REG_PHY_TX_REGS_TX3_AFE_OFFSET_GAIN1_REG 0xD10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx3AfeIOffsetGain1:10;
		uint32 tx3AfeQOffsetGain2:10;
		uint32 tx3AfeIOffsetGain2:10;
		uint32 reserved0:2;
	} bitFields;
} RegPhyTxRegsTx3AfeOffsetGain1Reg_u;

/*REG_PHY_TX_REGS_TPC_ACC 0xD14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tpcaccenable:1;
		uint32 txTpcAccDone:1;
		uint32 tx0Pgc2GainSelect:2;
		uint32 tx1Pgc2GainSelect:2;
		uint32 tx2Pgc2GainSelect:2;
		uint32 tx3Pgc2GainSelect:2;
		uint32 tpcBoostEnMask:4;
		uint32 tssiOpenLoopEn:1;
		uint32 reserved0:17;
	} bitFields;
} RegPhyTxRegsTpcAcc_u;

/*REG_PHY_TX_REGS_TX0_TX1_DIGITAL_GAIN 0xD18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 filt0OutAmpGainTpc:9;
		uint32 reserved0:7;
		uint32 filt1OutAmpGainTpc:9;
		uint32 reserved1:7;
	} bitFields;
} RegPhyTxRegsTx0Tx1DigitalGain_u;

/*REG_PHY_TX_REGS_TX2_TX3_DIGITAL_GAIN 0xD1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 filt2OutAmpGainTpc:9;
		uint32 reserved0:7;
		uint32 filt3OutAmpGainTpc:9;
		uint32 reserved1:7;
	} bitFields;
} RegPhyTxRegsTx2Tx3DigitalGain_u;

/*REG_PHY_TX_REGS_TX_PTARGET 0xD20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txRfPowerAnt0:6;
		uint32 reserved0:2;
		uint32 txRfPowerAnt1:6;
		uint32 reserved1:2;
		uint32 txRfPowerAnt2:6;
		uint32 reserved2:2;
		uint32 txRfPowerAnt3:6;
		uint32 reserved3:2;
	} bitFields;
} RegPhyTxRegsTxPtarget_u;

/*REG_PHY_TX_REGS_TPC_POUT 0xD24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tpcGainIndexAnt0:7;
		uint32 reserved0:1;
		uint32 tpcGainIndexAnt1:7;
		uint32 reserved1:1;
		uint32 tpcGainIndexAnt2:7;
		uint32 reserved2:1;
		uint32 tpcGainIndexAnt3:7;
		uint32 reserved3:1;
	} bitFields;
} RegPhyTxRegsTpcPout_u;

/*REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR0 0xE10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr0:28;
		uint32 reserved0:4;
	} bitFields;
} RegPhyTxRegsTx11BLutIqAddr0_u;

/*REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR1 0xE14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr1:28;
		uint32 reserved0:4;
	} bitFields;
} RegPhyTxRegsTx11BLutIqAddr1_u;

/*REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR2 0xE18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr2:28;
		uint32 reserved0:4;
	} bitFields;
} RegPhyTxRegsTx11BLutIqAddr2_u;

/*REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR3 0xE1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr3:28;
		uint32 reserved0:4;
	} bitFields;
} RegPhyTxRegsTx11BLutIqAddr3_u;

/*REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR4 0xE20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr4:28;
		uint32 reserved0:4;
	} bitFields;
} RegPhyTxRegsTx11BLutIqAddr4_u;

/*REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR5 0xE24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr5:28;
		uint32 reserved0:4;
	} bitFields;
} RegPhyTxRegsTx11BLutIqAddr5_u;

/*REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR6 0xE28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr6:28;
		uint32 reserved0:4;
	} bitFields;
} RegPhyTxRegsTx11BLutIqAddr6_u;

/*REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR7 0xE2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr7:28;
		uint32 reserved0:4;
	} bitFields;
} RegPhyTxRegsTx11BLutIqAddr7_u;

/*REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR8 0xE30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr8:28;
		uint32 reserved0:4;
	} bitFields;
} RegPhyTxRegsTx11BLutIqAddr8_u;

/*REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR9 0xE34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr9:28;
		uint32 reserved0:4;
	} bitFields;
} RegPhyTxRegsTx11BLutIqAddr9_u;

/*REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR10 0xE38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr10:28;
		uint32 reserved0:4;
	} bitFields;
} RegPhyTxRegsTx11BLutIqAddr10_u;

/*REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR11 0xE3C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr11:28;
		uint32 reserved0:4;
	} bitFields;
} RegPhyTxRegsTx11BLutIqAddr11_u;

/*REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR12 0xE40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr12:28;
		uint32 reserved0:4;
	} bitFields;
} RegPhyTxRegsTx11BLutIqAddr12_u;

/*REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR13 0xE44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr13:28;
		uint32 reserved0:4;
	} bitFields;
} RegPhyTxRegsTx11BLutIqAddr13_u;

/*REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR14 0xE48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr14:28;
		uint32 reserved0:4;
	} bitFields;
} RegPhyTxRegsTx11BLutIqAddr14_u;

/*REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR15 0xE4C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr15:28;
		uint32 reserved0:4;
	} bitFields;
} RegPhyTxRegsTx11BLutIqAddr15_u;

/*REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR16 0xE50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr16:28;
		uint32 reserved0:4;
	} bitFields;
} RegPhyTxRegsTx11BLutIqAddr16_u;

/*REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR17 0xE54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr17:28;
		uint32 reserved0:4;
	} bitFields;
} RegPhyTxRegsTx11BLutIqAddr17_u;

/*REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR18 0xE58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr18:28;
		uint32 reserved0:4;
	} bitFields;
} RegPhyTxRegsTx11BLutIqAddr18_u;

/*REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR19 0xE5C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr19:28;
		uint32 reserved0:4;
	} bitFields;
} RegPhyTxRegsTx11BLutIqAddr19_u;

/*REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR20 0xE60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr20:28;
		uint32 reserved0:4;
	} bitFields;
} RegPhyTxRegsTx11BLutIqAddr20_u;

/*REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR21 0xE64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr21:28;
		uint32 reserved0:4;
	} bitFields;
} RegPhyTxRegsTx11BLutIqAddr21_u;

/*REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR22 0xE68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr22:28;
		uint32 reserved0:4;
	} bitFields;
} RegPhyTxRegsTx11BLutIqAddr22_u;

/*REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR23 0xE6C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr23:28;
		uint32 reserved0:4;
	} bitFields;
} RegPhyTxRegsTx11BLutIqAddr23_u;

/*REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR24 0xE70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr24:28;
		uint32 reserved0:4;
	} bitFields;
} RegPhyTxRegsTx11BLutIqAddr24_u;

/*REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR25 0xE74 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr25:28;
		uint32 reserved0:4;
	} bitFields;
} RegPhyTxRegsTx11BLutIqAddr25_u;

/*REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR26 0xE78 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr26:28;
		uint32 reserved0:4;
	} bitFields;
} RegPhyTxRegsTx11BLutIqAddr26_u;

/*REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR27 0xE7C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr27:28;
		uint32 reserved0:4;
	} bitFields;
} RegPhyTxRegsTx11BLutIqAddr27_u;

/*REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR28 0xE80 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr28:28;
		uint32 reserved0:4;
	} bitFields;
} RegPhyTxRegsTx11BLutIqAddr28_u;

/*REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR29 0xE84 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr29:28;
		uint32 reserved0:4;
	} bitFields;
} RegPhyTxRegsTx11BLutIqAddr29_u;

/*REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR30 0xE88 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr30:28;
		uint32 reserved0:4;
	} bitFields;
} RegPhyTxRegsTx11BLutIqAddr30_u;

/*REG_PHY_TX_REGS_TX_11B_LUT_IQ_ADDR31 0xE8C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr31:28;
		uint32 reserved0:4;
	} bitFields;
} RegPhyTxRegsTx11BLutIqAddr31_u;

/*REG_PHY_TX_REGS_TX_AIR_TIME_RESULT 0xE90 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txAirTimeResult:17;	// air time result
		uint32 txAirTimeResultValid:1;	// air time result valid bit
		uint32 reserved0:14;
	} bitFields;
} RegPhyTxRegsTxAirTimeResult_u;

/*REG_PHY_TX_REGS_BB_AIR_TIME_CALC_MU_RESULTS1 0xE94 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbAirTimeCalcNumPaddingBitsMu:24;	// bb_air_time_calc_num_padding_bits in mu
		uint32 reserved0:8;
	} bitFields;
} RegPhyTxRegsBbAirTimeCalcMuResults1_u;

/*REG_PHY_TX_REGS_BB_AIR_TIME_CALC_MU_RESULTS2 0xE98 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbAirTimeCalcNumSymInitResult:15;	// bb_air_time_calc_num_sym_result
		uint32 reserved0:17;
	} bitFields;
} RegPhyTxRegsBbAirTimeCalcMuResults2_u;

/*REG_PHY_TX_REGS_BB_AIR_TIME_CALC_MU_MODE 0xE9C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbAirTimeCalcNSym:15;	// bb_air_time_calc_n_sym
		uint32 bbAirTimeCalcNSymInit:15;	// bb_air_time_calc_n_sym_init
		uint32 bbAirTimeCalcNssTotal:2;	// bb_air_time_calc_nss_total
	} bitFields;
} RegPhyTxRegsBbAirTimeCalcMuMode_u;

/*REG_PHY_TX_REGS_TX_BE_DEBUG1_USER1 0xEA0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txPhyMacIfUser1WrStm:4;	// tx_phy_mac_if_user1_wr_stm
		uint32 txPhyMacIfUser1RdStm:5;	// tx_phy_mac_if_user1_rd_stm
		uint32 txPhyMacIfUser1BubbleBytesInFifo:3;	// tx_phy_mac_if_user1_bubble_bytes_in_fifo
		uint32 txPhyMacIfUser1ByteCntWr:20;	// tx_phy_mac_if_user1_byte_cnt_wr
	} bitFields;
} RegPhyTxRegsTxBeDebug1User1_u;

/*REG_PHY_TX_REGS_TX_BE_DEBUG2_USER1 0xEA4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txPhyMacIfUser1ByteCntRd:20;	// tx_phy_mac_if_user1_byte_cnt_rd
		uint32 txPhyMacIfUser1OfdmRdReady:1;	// tx_phy_mac_if_user1_ofdm_rd_ready
		uint32 reserved0:11;
	} bitFields;
} RegPhyTxRegsTxBeDebug2User1_u;

/*REG_PHY_TX_REGS_TX_BE_DEBUG1_USER2 0xEA8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txPhyMacIfUser2WrStm:4;	// tx_phy_mac_if_user2_wr_stm
		uint32 txPhyMacIfUser2RdStm:5;	// tx_phy_mac_if_user2_rd_stm
		uint32 txPhyMacIfUser2BubbleBytesInFifo:3;	// tx_phy_mac_if_user2_bubble_bytes_in_fifo
		uint32 txPhyMacIfUser2ByteCntWr:20;	// tx_phy_mac_if_user2_byte_cnt_wr
	} bitFields;
} RegPhyTxRegsTxBeDebug1User2_u;

/*REG_PHY_TX_REGS_TX_BE_DEBUG2_USER2 0xEAC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txPhyMacIfUser2ByteCntRd:20;	// tx_phy_mac_if_user2_byte_cnt_rd
		uint32 txPhyMacIfUser2OfdmRdReady:1;	// tx_phy_mac_if_user2_ofdm_rd_ready
		uint32 reserved0:11;
	} bitFields;
} RegPhyTxRegsTxBeDebug2User2_u;

/*REG_PHY_TX_REGS_TX_BE_DEBUG1_USER3 0xEB0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txPhyMacIfUser3WrStm:4;	// tx_phy_mac_if_user3_wr_stm
		uint32 txPhyMacIfUser3RdStm:5;	// tx_phy_mac_if_user3_rd_stm
		uint32 txPhyMacIfUser3BubbleBytesInFifo:3;	// tx_phy_mac_if_user3_bubble_bytes_in_fifo
		uint32 txPhyMacIfUser3ByteCntWr:20;	// tx_phy_mac_if_user3_byte_cnt_wr
	} bitFields;
} RegPhyTxRegsTxBeDebug1User3_u;

/*REG_PHY_TX_REGS_TX_BE_DEBUG2_USER3 0xEB4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txPhyMacIfUser3ByteCntRd:20;	// tx_phy_mac_if_user3_byte_cnt_rd
		uint32 txPhyMacIfUser3OfdmRdReady:1;	// tx_phy_mac_if_user3_ofdm_rd_ready
		uint32 reserved0:11;
	} bitFields;
} RegPhyTxRegsTxBeDebug2User3_u;

/*REG_PHY_TX_REGS_SPHERE_TB_CONTROL 0xEB8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sphereBus0Len:1;	// sphere_bus0_len
		uint32 sphereBus1Len:1;	// sphere_bus1_len
		uint32 sphereBus2Len:1;	// sphere_bus2_len
		uint32 sphereBus3Len:1;	// sphere_bus3_len
		uint32 sphereTbMode:1;	// sphere_tb_mode
		uint32 reserved0:27;
	} bitFields;
} RegPhyTxRegsSphereTbControl_u;

/*REG_PHY_TX_REGS_MU_PILOT_DIRECT 0xEBC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muPilotDirect2SsEn:1;
		uint32 muPilotDirect3SsEn:1;
		uint32 muPilotDirect4SsEn:1;
		uint32 createMuAsDirectMap:1;
		uint32 reserved0:28;
	} bitFields;
} RegPhyTxRegsMuPilotDirect_u;

/*REG_PHY_TX_REGS_TX_CONTROL_ERROR_EN_MASK 0xEC0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txControlErrorEnMask:3;	// [0]-mu packet , [1]-legacy bf , [2]-tx_enable while rx_ready
		uint32 reserved0:29;
	} bitFields;
} RegPhyTxRegsTxControlErrorEnMask_u;



#endif // _PHY_TX_REGS_REGS_H_
