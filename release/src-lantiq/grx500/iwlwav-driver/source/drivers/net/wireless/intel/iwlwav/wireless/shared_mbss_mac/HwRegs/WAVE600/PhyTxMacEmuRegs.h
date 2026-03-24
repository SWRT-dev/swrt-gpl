
/***********************************************************************************
File:				PhyTxMacEmuRegs.h
Module:				PhyTxMacEmu
SOC Revision:		
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _PHY_TX_MAC_EMU_REGS_H_
#define _PHY_TX_MAC_EMU_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define PHY_TX_MAC_EMU_BASE_ADDRESS                             MEMORY_MAP_UNIT_52_BASE_ADDRESS
#define	REG_PHY_TX_MAC_EMU_TX_MAC_EMU_REG00                           (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2000)
#define	REG_PHY_TX_MAC_EMU_TX_MAC_EMU_REG01                           (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2004)
#define	REG_PHY_TX_MAC_EMU_TX_MAC_EMU_REG02                           (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2008)
#define	REG_PHY_TX_MAC_EMU_ENDLESS_TYPE                               (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x200C)
#define	REG_PHY_TX_MAC_EMU_MAC_EMU_MODE                               (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2010)
#define	REG_PHY_TX_MAC_EMU_EMU_MAIN_STM                               (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2014)
#define	REG_PHY_TX_MAC_EMU_EMU_DEL_STM                                (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2018)
#define	REG_PHY_TX_MAC_EMU_EMU_PACKET_COUNTER                         (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x201C)
#define	REG_PHY_TX_MAC_EMU_PACKET_END_ON_AIR_TIME                     (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2020)
#define	REG_PHY_TX_MAC_EMU_HE_SIGA_BEAM_CHANGE_ACTIVE                 (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2024)
#define	REG_PHY_TX_MAC_EMU_TX_MAC_EMU_REG0A                           (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2028)
#define	REG_PHY_TX_MAC_EMU_TX_MAC_EMU_REG0B                           (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x202C)
#define	REG_PHY_TX_MAC_EMU_TX_MAC_EMU_REG0C                           (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2030)
#define	REG_PHY_TX_MAC_EMU_TX_MAC_EMU_REG0D                           (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2034)
#define	REG_PHY_TX_MAC_EMU_TX_MAC_EMU_REG0E                           (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2038)
#define	REG_PHY_TX_MAC_EMU_TX_PHY_ZLD                                 (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x203C)
#define	REG_PHY_TX_MAC_EMU_TX_MAC_PHY_IF                              (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2040)
#define	REG_PHY_TX_MAC_EMU_TX_PHY_PACKET_TIMER                        (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2044)
#define	REG_PHY_TX_MAC_EMU_TX_PHY_MAIN_STM                            (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2048)
#define	REG_PHY_TX_MAC_EMU_TX_PHY_TCR_COUNTER                         (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x204C)
#define	REG_PHY_TX_MAC_EMU_TX_PHY_RCR_COUNTER                         (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2050)
#define	REG_PHY_TX_MAC_EMU_TX_PHY_SYMBOL_COUNTER                      (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2054)
#define	REG_PHY_TX_MAC_EMU_TX_PHY_BF_LOW                              (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2058)
#define	REG_PHY_TX_MAC_EMU_TX_PHY_BF_HIGH                             (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x205C)
#define	REG_PHY_TX_MAC_EMU_SIG_CRC_DEFS                               (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2060)
#define	REG_PHY_TX_MAC_EMU_TCR_COMMON_TIMING                          (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2064)
#define	REG_PHY_TX_MAC_EMU_TCR_USER_TIMING                            (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2068)
#define	REG_PHY_TX_MAC_EMU_PACKET_START_TR_SWITCHING                  (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x206C)
#define	REG_PHY_TX_MAC_EMU_TCR_USER_SELECT                            (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2070)
#define	REG_PHY_TX_MAC_EMU_TX_PHY_MU_TYPE                             (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2074)
#define	REG_PHY_TX_MAC_EMU_EMU_PHY_TX_PHY_TCR0_0                      (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2100)
#define	REG_PHY_TX_MAC_EMU_EMU_PHY_TX_PHY_TCR0_1                      (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2104)
#define	REG_PHY_TX_MAC_EMU_EMU_PHY_TX_PHY_TCR0_2                      (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2108)
#define	REG_PHY_TX_MAC_EMU_EMU_PHY_TX_PHY_TCR0_3                      (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x210C)
#define	REG_PHY_TX_MAC_EMU_EMU_PHY_TX_PHY_TCR1_0                      (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2110)
#define	REG_PHY_TX_MAC_EMU_EMU_PHY_TX_PHY_TCR1_1                      (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2114)
#define	REG_PHY_TX_MAC_EMU_EMU_PHY_TX_PHY_TCR1_2                      (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2118)
#define	REG_PHY_TX_MAC_EMU_EMU_PHY_TX_PHY_TCR1_3                      (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x211C)
#define	REG_PHY_TX_MAC_EMU_EMU_PHY_TX_PHY_TCR_U_0                     (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2120)
#define	REG_PHY_TX_MAC_EMU_EMU_PHY_TX_PHY_TCR_U_1                     (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2124)
#define	REG_PHY_TX_MAC_EMU_EMU_PHY_TX_PHY_TCR_U_2                     (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2128)
#define	REG_PHY_TX_MAC_EMU_EMU_PHY_TX_PHY_TCR_U_3                     (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x212C)
#define	REG_PHY_TX_MAC_EMU_TCR0_DATA_0                                (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2130)
#define	REG_PHY_TX_MAC_EMU_TCR0_DATA_1                                (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2134)
#define	REG_PHY_TX_MAC_EMU_TCR0_DATA_2                                (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2138)
#define	REG_PHY_TX_MAC_EMU_TCR0_DATA_3                                (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x213C)
#define	REG_PHY_TX_MAC_EMU_TCR1_DATA_0                                (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2140)
#define	REG_PHY_TX_MAC_EMU_TCR1_DATA_1                                (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2144)
#define	REG_PHY_TX_MAC_EMU_TCR1_DATA_2                                (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2148)
#define	REG_PHY_TX_MAC_EMU_TCR1_DATA_3                                (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x214C)
#define	REG_PHY_TX_MAC_EMU_TCR_USER_SELECT_DATA_0                     (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2150)
#define	REG_PHY_TX_MAC_EMU_TCR_USER_SELECT_DATA_1                     (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2154)
#define	REG_PHY_TX_MAC_EMU_TCR_USER_SELECT_DATA_2                     (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2158)
#define	REG_PHY_TX_MAC_EMU_TCR_USER_SELECT_DATA_3                     (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x215C)
#define	REG_PHY_TX_MAC_EMU_TCR_COMMON_ANT_SELECT                      (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2160)
#define	REG_PHY_TX_MAC_EMU_TCR_COMMON_ANT_BOOST                       (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2164)
#define	REG_PHY_TX_MAC_EMU_TCR_COMMON_RF_POWER                        (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2168)
#define	REG_PHY_TX_MAC_EMU_TCR_COMMON_LOOP_MODE                       (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x216C)
#define	REG_PHY_TX_MAC_EMU_TCR_COMMON_CBW                             (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2170)
#define	REG_PHY_TX_MAC_EMU_TCR_COMMON_PHY_MODE                        (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2174)
#define	REG_PHY_TX_MAC_EMU_TCR_COMMON_HE_PUNCTURING_MAP               (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2178)
#define	REG_PHY_TX_MAC_EMU_TCR_COMMON_FORCE_TX                        (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x217C)
#define	REG_PHY_TX_MAC_EMU_TCR_COMMON_BSS_COLOR                       (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2180)
#define	REG_PHY_TX_MAC_EMU_TCR_COMMON_CUSTOM_BF                       (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2184)
#define	REG_PHY_TX_MAC_EMU_TCR_COMMON_N_HE_LTF                        (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2188)
#define	REG_PHY_TX_MAC_EMU_TCR_COMMON_UL_DL                           (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x218C)
#define	REG_PHY_TX_MAC_EMU_TCR_COMMON_TXOP_PS_NOT_ALLOWED             (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2190)
#define	REG_PHY_TX_MAC_EMU_TCR_COMMON_EXPECT_RCR                      (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2194)
#define	REG_PHY_TX_MAC_EMU_TCR_COMMON_BW_CHANGE                       (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2198)
#define	REG_PHY_TX_MAC_EMU_TCR_COMMON_TXOP_DURATION_FROM_MAC          (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x219C)
#define	REG_PHY_TX_MAC_EMU_TCR_COMMON_AGGREGATE                       (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x21A0)
#define	REG_PHY_TX_MAC_EMU_TCR_COMMON_DYN_BW                          (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x21A4)
#define	REG_PHY_TX_MAC_EMU_TCR_COMMON_MU_PHY_NDP                      (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x21A8)
#define	REG_PHY_TX_MAC_EMU_TCR_COMMON_MU_TRAINING                     (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x21AC)
#define	REG_PHY_TX_MAC_EMU_TCR_COMMON_HE_CP                           (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x21B0)
#define	REG_PHY_TX_MAC_EMU_TCR_COMMON_HE_LTF                          (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x21B4)
#define	REG_PHY_TX_MAC_EMU_TCR_COMMON_SCP                             (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x21B8)
#define	REG_PHY_TX_MAC_EMU_TCR_COMMON_SMOOTHING                       (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x21BC)
#define	REG_PHY_TX_MAC_EMU_TCR_COMMON_NOT_SOUNDING                    (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x21C0)
#define	REG_PHY_TX_MAC_EMU_TCR_COMMON_STBC                            (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x21C4)
#define	REG_PHY_TX_MAC_EMU_TCR_COMMON_SPATIAL_REUSE                   (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x21C8)
#define	REG_PHY_TX_MAC_EMU_TCR_COMMON_TRIGGER_BASED_DATA              (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x21CC)
#define	REG_PHY_TX_MAC_EMU_TCR_COMMON_GROUP_ID                        (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x21D0)
#define	REG_PHY_TX_MAC_EMU_TCR_COMMON_HE_SIGB_RATE                    (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x21D4)
#define	REG_PHY_TX_MAC_EMU_TCR_COMMON_MAC_DURATION                    (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x21D8)
#define	REG_PHY_TX_MAC_EMU_TCR_USER_SELECT_PSDU_LENGTH                (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x21DC)
#define	REG_PHY_TX_MAC_EMU_TCR_USER_SELECT_SUB_BAND                   (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x21E0)
#define	REG_PHY_TX_MAC_EMU_TCR_USER_SELECT_START_RU                   (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x21E4)
#define	REG_PHY_TX_MAC_EMU_TCR_USER_SELECT_RU_SIZE                    (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x21E8)
#define	REG_PHY_TX_MAC_EMU_TCR_USER_SELECT_LDPC                       (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x21EC)
#define	REG_PHY_TX_MAC_EMU_TCR_USER_SELECT_TXBF                       (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x21F0)
#define	REG_PHY_TX_MAC_EMU_TCR_USER_SELECT_PSDU_RATE                  (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x21F4)
#define	REG_PHY_TX_MAC_EMU_TCR_USER_SELECT_RELATIVE_POWER             (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x21F8)
#define	REG_PHY_TX_MAC_EMU_TCR_USER_SELECT_PACKET_EXTENSION           (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x21FC)
#define	REG_PHY_TX_MAC_EMU_TCR_USER_SELECT_STATION_ID                 (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2200)
#define	REG_PHY_TX_MAC_EMU_TCR_USER_SELECT_BF_INDEX                   (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2204)
#define	REG_PHY_TX_MAC_EMU_TCR_USER_SELECT_PSDU_MCS                   (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2208)
#define	REG_PHY_TX_MAC_EMU_TCR_USER_SELECT_PSDU_RANK                  (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x220C)
#define	REG_PHY_TX_MAC_EMU_TCR_USER_SELECT_PSDU_DCM                   (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2210)
#define	REG_PHY_TX_MAC_EMU_AIR_TIME_VALID                             (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2214)
#define	REG_PHY_TX_MAC_EMU_AIR_TIME_COMMON_L_LENGTH_EXCEED            (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2218)
#define	REG_PHY_TX_MAC_EMU_AIR_TIME_COMMON_NUM_SYM                    (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x221C)
#define	REG_PHY_TX_MAC_EMU_AIR_TIME_COMMON_A_FACTOR                   (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2220)
#define	REG_PHY_TX_MAC_EMU_AIR_TIME_COMMON_T_PE                       (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2224)
#define	REG_PHY_TX_MAC_EMU_AIR_TIME_COMMON_AIR_TIME_CYCLES            (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2228)
#define	REG_PHY_TX_MAC_EMU_AIR_TIME_COMMON_PREAMBLE_TIME              (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x222C)
#define	REG_PHY_TX_MAC_EMU_AIR_TIME_COMMON_LDPC_EXTRA_SYMBOL          (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2230)
#define	REG_PHY_TX_MAC_EMU_AIR_TIME_COMMON_AIR_TIME_RESULT            (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2234)
#define	REG_PHY_TX_MAC_EMU_AIR_TIME_COMMON_L_LENGTH                   (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2238)
#define	REG_PHY_TX_MAC_EMU_AIR_TIME_COMMON_VHT_SIG_A2_B1              (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x223C)
#define	REG_PHY_TX_MAC_EMU_AIR_TIME_COMMON_PE_DISAMBIGUITY            (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2240)
#define	REG_PHY_TX_MAC_EMU_AIR_TIME_USER_SELECT_CODING_RATE           (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2244)
#define	REG_PHY_TX_MAC_EMU_AIR_TIME_USER_SELECT_MODULATION            (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2248)
#define	REG_PHY_TX_MAC_EMU_AIR_TIME_USER_SELECT_LDPC_REM_NSHRT_NCW    (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x224C)
#define	REG_PHY_TX_MAC_EMU_AIR_TIME_USER_SELECT_LDPC_REM_NPUNC_NCW    (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2250)
#define	REG_PHY_TX_MAC_EMU_AIR_TIME_USER_SELECT_LDPC_REM_NREP_NCW     (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2254)
#define	REG_PHY_TX_MAC_EMU_AIR_TIME_USER_SELECT_LDPC_N_DATA           (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2258)
#define	REG_PHY_TX_MAC_EMU_AIR_TIME_USER_SELECT_LDPC_N_PARITY         (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x225C)
#define	REG_PHY_TX_MAC_EMU_AIR_TIME_USER_SELECT_LDPC_N_REPEAT         (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2260)
#define	REG_PHY_TX_MAC_EMU_AIR_TIME_USER_SELECT_LDPC_CW_TYPE          (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2264)
#define	REG_PHY_TX_MAC_EMU_AIR_TIME_USER_SELECT_LDPC_N_CW             (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2268)
#define	REG_PHY_TX_MAC_EMU_AIR_TIME_USER_SELECT_PADDING               (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x226C)
#define	REG_PHY_TX_MAC_EMU_AIR_TIME_USER_SELECT_NES                   (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2270)
#define	REG_PHY_TX_MAC_EMU_AIR_TIME_USER_SELECT_N_CBPS                (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2274)
#define	REG_PHY_TX_MAC_EMU_AIR_TIME_USER_SELECT_N_DBPS                (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2278)
#define	REG_PHY_TX_MAC_EMU_AIR_TIME_USER_SELECT_N_SD_LAST             (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x227C)
#define	REG_PHY_TX_MAC_EMU_HE_SIGB_NUM_SYM                            (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2280)
#define	REG_PHY_TX_MAC_EMU_PHY_MAC_ERROR                              (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2284)
#define	REG_PHY_TX_MAC_EMU_PHY_MAC_ERROR_CLEAR                        (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2288)
#define	REG_PHY_TX_MAC_EMU_PHY_MAC_ERROR_INT_EN                       (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x228C)
#define	REG_PHY_TX_MAC_EMU_BB_CPU_AIR_TIME_CALC_START                 (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2800)
#define	REG_PHY_TX_MAC_EMU_BB_CPU_LDPC                                (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2804)
#define	REG_PHY_TX_MAC_EMU_BB_CPU_SCP                                 (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2808)
#define	REG_PHY_TX_MAC_EMU_BB_CPU_STBC                                (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x280C)
#define	REG_PHY_TX_MAC_EMU_BB_CPU_PSDU_RATE                           (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2810)
#define	REG_PHY_TX_MAC_EMU_BB_CPU_BW                                  (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2814)
#define	REG_PHY_TX_MAC_EMU_BB_CPU_PACKET_MODE                         (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2818)
#define	REG_PHY_TX_MAC_EMU_BB_CPU_NESS                                (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x281C)
#define	REG_PHY_TX_MAC_EMU_BB_CPU_PACKET_LENGTH                       (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2820)
#define	REG_PHY_TX_MAC_EMU_BB_CPU_RX_TX_N_SELECTOR                    (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2824)
#define	REG_PHY_TX_MAC_EMU_BB_CPU_RX_L_LENGTH                         (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2828)
#define	REG_PHY_TX_MAC_EMU_BB_CPU_RX_VHT_SIG_A2_B1                    (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x282C)
#define	REG_PHY_TX_MAC_EMU_BB_CPU_RX_LDPC_EXTRA_SYMBOL                (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2830)
#define	REG_PHY_TX_MAC_EMU_BB_CPU_MU_STAGE                            (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2834)
#define	REG_PHY_TX_MAC_EMU_BB_CPU_N_SYM_INIT                          (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2838)
#define	REG_PHY_TX_MAC_EMU_BB_CPU_N_SYM                               (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x283C)
#define	REG_PHY_TX_MAC_EMU_BB_CPU_N_LTF_TOTAL                         (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2840)
#define	REG_PHY_TX_MAC_EMU_BB_CPU_HE_RU_SIZE                          (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2844)
#define	REG_PHY_TX_MAC_EMU_BB_CPU_N_SIGB_SYM                          (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2848)
#define	REG_PHY_TX_MAC_EMU_BB_CPU_HE_CP                               (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x284C)
#define	REG_PHY_TX_MAC_EMU_BB_CPU_HE_LTF                              (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2850)
#define	REG_PHY_TX_MAC_EMU_BB_CPU_PE_DISAMBIGUITY                     (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2854)
#define	REG_PHY_TX_MAC_EMU_BB_CPU_A_FACTOR                            (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2858)
#define	REG_PHY_TX_MAC_EMU_BB_CPU_A_FACTOR_INIT                       (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x285C)
#define	REG_PHY_TX_MAC_EMU_BB_CPU_MAX_PE                              (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2860)
#define	REG_PHY_TX_MAC_EMU_BB_CPU_AIR_TIME_CALC_VALID                 (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2864)
#define	REG_PHY_TX_MAC_EMU_BB_CPU_L_LENGTH_EXCEED                     (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2868)
#define	REG_PHY_TX_MAC_EMU_BB_CPU_VHT_SIG_A2_B1                       (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x286C)
#define	REG_PHY_TX_MAC_EMU_BB_CPU_LDPC_EXTRA_SYMBOL                   (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2870)
#define	REG_PHY_TX_MAC_EMU_BB_CPU_CODING_RATE                         (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2874)
#define	REG_PHY_TX_MAC_EMU_BB_CPU_MODULATION                          (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2878)
#define	REG_PHY_TX_MAC_EMU_BB_CPU_AIR_TIME_RESULT                     (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x287C)
#define	REG_PHY_TX_MAC_EMU_BB_CPU_L_LENGTH_RESULT                     (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2880)
#define	REG_PHY_TX_MAC_EMU_BB_CPU_NUM_SYM_RESULT                      (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2884)
#define	REG_PHY_TX_MAC_EMU_BB_CPU_NUM_SYM_INIT_RESULT                 (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2888)
#define	REG_PHY_TX_MAC_EMU_BB_CPU_LDPC_REM_NSHRT_NCW                  (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x288C)
#define	REG_PHY_TX_MAC_EMU_BB_CPU_LDPC_REM_NPUNC_NCW                  (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2890)
#define	REG_PHY_TX_MAC_EMU_BB_CPU_LDPC_REM_NREP_NCW                   (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2894)
#define	REG_PHY_TX_MAC_EMU_BB_CPU_LDPC_N_DATA                         (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x2898)
#define	REG_PHY_TX_MAC_EMU_BB_CPU_LDPC_N_PARITY                       (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x289C)
#define	REG_PHY_TX_MAC_EMU_BB_CPU_LDPC_N_REPEAT                       (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x28A0)
#define	REG_PHY_TX_MAC_EMU_BB_CPU_LDPC_CW_TYPE                        (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x28A4)
#define	REG_PHY_TX_MAC_EMU_BB_CPU_LDPC_N_CW                           (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x28A8)
#define	REG_PHY_TX_MAC_EMU_BB_CPU_TX_NUM_PADDING_BITS                 (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x28AC)
#define	REG_PHY_TX_MAC_EMU_BB_CPU_N_PAD_POST_FEC                      (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x28B0)
#define	REG_PHY_TX_MAC_EMU_BB_CPU_VITERBI_N_ES                        (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x28B4)
#define	REG_PHY_TX_MAC_EMU_BB_CPU_T_PE                                (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x28B8)
#define	REG_PHY_TX_MAC_EMU_BB_CPU_PE_DISAMBIGUITY_RESULT              (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x28BC)
#define	REG_PHY_TX_MAC_EMU_BB_CPU_A_FACTOR_RESULT                     (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x28C0)
#define	REG_PHY_TX_MAC_EMU_BB_CPU_A_FACTOR_INIT_RESULT                (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x28C4)
#define	REG_PHY_TX_MAC_EMU_BB_CPU_AIR_TIME_CYCLES                     (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x28C8)
#define	REG_PHY_TX_MAC_EMU_BB_CPU_PHY_RATE_RESULT                     (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x28CC)
#define	REG_PHY_TX_MAC_EMU_BB_CPU_AIR_TIME_ERROR                      (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x28D0)
#define	REG_PHY_TX_MAC_EMU_BB_CPU_N_CBPS                              (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x28D4)
#define	REG_PHY_TX_MAC_EMU_BB_CPU_N_DBPS                              (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x28D8)
#define	REG_PHY_TX_MAC_EMU_BB_CPU_N_SD_LAST                           (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x28DC)
#define	REG_PHY_TX_MAC_EMU_BB_CPU_PREAMBLE_TIME                       (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x28E0)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_PHY_TX_MAC_EMU_TX_MAC_EMU_REG00 0x2000 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 emuRun : 1; //Emulator Run, reset value: 0x0, access type: WO
		uint32 emuTxDone : 1; //Emulator Transmit Done, reset value: 0x0, access type: RO
		uint32 reserved0 : 30;
	} bitFields;
} RegPhyTxMacEmuTxMacEmuReg00_u;

/*REG_PHY_TX_MAC_EMU_TX_MAC_EMU_REG01 0x2004 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 emuNumOfPackets : 32; //Number of Packets, reset value: 0x0, access type: RW
	} bitFields;
} RegPhyTxMacEmuTxMacEmuReg01_u;

/*REG_PHY_TX_MAC_EMU_TX_MAC_EMU_REG02 0x2008 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 1;
		uint32 emuCrcInMode : 1; //Insert CRC, reset value: 0x0, access type: RW
		uint32 emuCrcInvert : 1; //Currupt CRC, reset value: 0x1, access type: RW
		uint32 emuCrcReverse : 1; //Reverse CRC, reset value: 0x1, access type: RW
		uint32 reserved1 : 28;
	} bitFields;
} RegPhyTxMacEmuTxMacEmuReg02_u;

/*REG_PHY_TX_MAC_EMU_ENDLESS_TYPE 0x200C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 endlessPackets : 1; //endless_packets, reset value: 0x0, access type: RW
		uint32 endlessData : 1; //endless_data, reset value: 0x0, access type: RW
		uint32 reserved0 : 30;
	} bitFields;
} RegPhyTxMacEmuEndlessType_u;

/*REG_PHY_TX_MAC_EMU_MAC_EMU_MODE 0x2010 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 macEmuMode : 1; //mac_emu_mode, reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyTxMacEmuMacEmuMode_u;

/*REG_PHY_TX_MAC_EMU_EMU_MAIN_STM 0x2014 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 emuMainStm : 4; //emu_main_stm, reset value: 0x0, access type: RO
		uint32 reserved0 : 28;
	} bitFields;
} RegPhyTxMacEmuEmuMainStm_u;

/*REG_PHY_TX_MAC_EMU_EMU_DEL_STM 0x2018 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 emuDelStm : 5; //emu_del_stm, reset value: 0x0, access type: RO
		uint32 reserved0 : 27;
	} bitFields;
} RegPhyTxMacEmuEmuDelStm_u;

/*REG_PHY_TX_MAC_EMU_EMU_PACKET_COUNTER 0x201C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 emuPacketCounter : 32; //emu_packet_counter, reset value: 0x0, access type: RO
	} bitFields;
} RegPhyTxMacEmuEmuPacketCounter_u;

/*REG_PHY_TX_MAC_EMU_PACKET_END_ON_AIR_TIME 0x2020 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 packetEndOnAirTimeOfdm : 16; //packet_end_on_air_time_ofdm, reset value: 0x140, access type: RW
		uint32 packetEndOnAirTime11B : 16; //packet_end_on_air_time_11b, reset value: 0x500, access type: RW
	} bitFields;
} RegPhyTxMacEmuPacketEndOnAirTime_u;

/*REG_PHY_TX_MAC_EMU_HE_SIGA_BEAM_CHANGE_ACTIVE 0x2024 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 heSigaBeamChangeActive : 1; //he_siga_beam_change_active, reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyTxMacEmuHeSigaBeamChangeActive_u;

/*REG_PHY_TX_MAC_EMU_TX_MAC_EMU_REG0A 0x2028 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 emuPatternMode : 2; //Transmit pattern mode, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 emuInsPrbsErr : 1; //Insert PRBS Errors, reset value: 0x0, access type: RW
		uint32 reserved1 : 3;
		uint32 emuInsErrNum : 9; //PRBS Error Rate, reset value: 0x0, access type: RW
		uint32 reserved2 : 15;
	} bitFields;
} RegPhyTxMacEmuTxMacEmuReg0A_u;

/*REG_PHY_TX_MAC_EMU_TX_MAC_EMU_REG0B 0x202C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 emuPrbsType : 32; //PRBS Type, reset value: 0x80000003, access type: CONST
	} bitFields;
} RegPhyTxMacEmuTxMacEmuReg0B_u;

/*REG_PHY_TX_MAC_EMU_TX_MAC_EMU_REG0C 0x2030 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 emuPrbsInit : 32; //PRBS Init Data, reset value: 0x162, access type: CONST
	} bitFields;
} RegPhyTxMacEmuTxMacEmuReg0C_u;

/*REG_PHY_TX_MAC_EMU_TX_MAC_EMU_REG0D 0x2034 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 emuFixedData : 32; //Fixed Data Word, reset value: 0x0, access type: RW
	} bitFields;
} RegPhyTxMacEmuTxMacEmuReg0D_u;

/*REG_PHY_TX_MAC_EMU_TX_MAC_EMU_REG0E 0x2038 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 emuIpgDly : 16; //IPG delay, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyTxMacEmuTxMacEmuReg0E_u;

/*REG_PHY_TX_MAC_EMU_TX_PHY_ZLD 0x203C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txPhyZld : 32; //tx_phy_zld, reset value: 0x4E790001, access type: RW
	} bitFields;
} RegPhyTxMacEmuTxPhyZld_u;

/*REG_PHY_TX_MAC_EMU_TX_MAC_PHY_IF 0x2040 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txPhyEnable : 1; //mac_phy_enable, reset value: 0x0, access type: RO
		uint32 txPhyReady : 1; //phy_mac_ready, reset value: 0x0, access type: RO
		uint32 reserved0 : 1;
		uint32 txPhyValid : 1; //mac_phy_valid, reset value: 0x0, access type: RO
		uint32 txPhyBusState : 4; //mac_phy_bus_state, reset value: 0x0, access type: RO
		uint32 txPhyAirTimeResult : 17; //phy_mac_air_time_result, reset value: 0x0, access type: RO
		uint32 reserved1 : 7;
	} bitFields;
} RegPhyTxMacEmuTxMacPhyIf_u;

/*REG_PHY_TX_MAC_EMU_TX_PHY_PACKET_TIMER 0x2044 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txPhyPacketTimer : 32; //tx_phy_packet_timer, reset value: 0x0, access type: RO
	} bitFields;
} RegPhyTxMacEmuTxPhyPacketTimer_u;

/*REG_PHY_TX_MAC_EMU_TX_PHY_MAIN_STM 0x2048 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txPhyMainStm : 4; //tx_phy_main_stm, reset value: 0x0, access type: RO
		uint32 reserved0 : 28;
	} bitFields;
} RegPhyTxMacEmuTxPhyMainStm_u;

/*REG_PHY_TX_MAC_EMU_TX_PHY_TCR_COUNTER 0x204C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txPhyTcrCounter : 6; //tx_phy_tcr_counter, reset value: 0x0, access type: RO
		uint32 reserved0 : 26;
	} bitFields;
} RegPhyTxMacEmuTxPhyTcrCounter_u;

/*REG_PHY_TX_MAC_EMU_TX_PHY_RCR_COUNTER 0x2050 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txPhyRcrCounter : 6; //tx_phy_rcr_counter, reset value: 0x0, access type: RO
		uint32 reserved0 : 26;
	} bitFields;
} RegPhyTxMacEmuTxPhyRcrCounter_u;

/*REG_PHY_TX_MAC_EMU_TX_PHY_SYMBOL_COUNTER 0x2054 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txPhySymbolCounter : 15; //tx_phy_symbol_counter, reset value: 0x0, access type: RO
		uint32 reserved0 : 17;
	} bitFields;
} RegPhyTxMacEmuTxPhySymbolCounter_u;

/*REG_PHY_TX_MAC_EMU_TX_PHY_BF_LOW 0x2058 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txPhyBfLow : 18; //tx_phy_bf_low, reset value: 0x0, access type: RO
		uint32 reserved0 : 14;
	} bitFields;
} RegPhyTxMacEmuTxPhyBfLow_u;

/*REG_PHY_TX_MAC_EMU_TX_PHY_BF_HIGH 0x205C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txPhyBfHigh : 18; //tx_phy_bf_high, reset value: 0x0, access type: RO
		uint32 reserved0 : 14;
	} bitFields;
} RegPhyTxMacEmuTxPhyBfHigh_u;

/*REG_PHY_TX_MAC_EMU_SIG_CRC_DEFS 0x2060 */
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
} RegPhyTxMacEmuSigCrcDefs_u;

/*REG_PHY_TX_MAC_EMU_TCR_COMMON_TIMING 0x2064 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrCommon0Timing : 16; //tcr_common_0_timing, reset value: 0x140, access type: RW
		uint32 tcrCommon1Timing : 16; //tcr_common_1_timing, reset value: 0x280, access type: RW
	} bitFields;
} RegPhyTxMacEmuTcrCommonTiming_u;

/*REG_PHY_TX_MAC_EMU_TCR_USER_TIMING 0x2068 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrUser0Timing : 16; //tcr_user_0_timing, reset value: 0x3C0, access type: RW
		uint32 tcrUserLastTiming : 16; //tcr_user_last_timing, reset value: 0x640, access type: RW
	} bitFields;
} RegPhyTxMacEmuTcrUserTiming_u;

/*REG_PHY_TX_MAC_EMU_PACKET_START_TR_SWITCHING 0x206C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 packetStartTrSwitching : 16; //packet_start_tr_switching, reset value: 0x640, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyTxMacEmuPacketStartTrSwitching_u;

/*REG_PHY_TX_MAC_EMU_TCR_USER_SELECT 0x2070 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrUserSelect : 6; //tcr_user_select, reset value: 0x0, access type: RW
		uint32 reserved0 : 26;
	} bitFields;
} RegPhyTxMacEmuTcrUserSelect_u;

/*REG_PHY_TX_MAC_EMU_TX_PHY_MU_TYPE 0x2074 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txPhyMuType : 2; //tx_phy_mu_type, reset value: 0x0, access type: RO
		uint32 reserved0 : 30;
	} bitFields;
} RegPhyTxMacEmuTxPhyMuType_u;

/*REG_PHY_TX_MAC_EMU_EMU_PHY_TX_PHY_TCR0_0 0x2100 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 emuPhyTxPhyTcr00 : 32; //emu_phy_tx_phy_tcr0_0, reset value: 0x0, access type: RW
	} bitFields;
} RegPhyTxMacEmuEmuPhyTxPhyTcr00_u;

/*REG_PHY_TX_MAC_EMU_EMU_PHY_TX_PHY_TCR0_1 0x2104 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 emuPhyTxPhyTcr01 : 32; //emu_phy_tx_phy_tcr0_1, reset value: 0x0, access type: RW
	} bitFields;
} RegPhyTxMacEmuEmuPhyTxPhyTcr01_u;

/*REG_PHY_TX_MAC_EMU_EMU_PHY_TX_PHY_TCR0_2 0x2108 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 emuPhyTxPhyTcr02 : 32; //emu_phy_tx_phy_tcr0_2, reset value: 0x0, access type: RW
	} bitFields;
} RegPhyTxMacEmuEmuPhyTxPhyTcr02_u;

/*REG_PHY_TX_MAC_EMU_EMU_PHY_TX_PHY_TCR0_3 0x210C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 emuPhyTxPhyTcr03 : 32; //emu_phy_tx_phy_tcr0_3, reset value: 0x0, access type: RW
	} bitFields;
} RegPhyTxMacEmuEmuPhyTxPhyTcr03_u;

/*REG_PHY_TX_MAC_EMU_EMU_PHY_TX_PHY_TCR1_0 0x2110 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 emuPhyTxPhyTcr10 : 32; //emu_phy_tx_phy_tcr1_0, reset value: 0x0, access type: RW
	} bitFields;
} RegPhyTxMacEmuEmuPhyTxPhyTcr10_u;

/*REG_PHY_TX_MAC_EMU_EMU_PHY_TX_PHY_TCR1_1 0x2114 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 emuPhyTxPhyTcr11 : 32; //emu_phy_tx_phy_tcr1_1, reset value: 0x0, access type: RW
	} bitFields;
} RegPhyTxMacEmuEmuPhyTxPhyTcr11_u;

/*REG_PHY_TX_MAC_EMU_EMU_PHY_TX_PHY_TCR1_2 0x2118 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 emuPhyTxPhyTcr12 : 32; //emu_phy_tx_phy_tcr1_2, reset value: 0x0, access type: RW
	} bitFields;
} RegPhyTxMacEmuEmuPhyTxPhyTcr12_u;

/*REG_PHY_TX_MAC_EMU_EMU_PHY_TX_PHY_TCR1_3 0x211C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 emuPhyTxPhyTcr13 : 32; //emu_phy_tx_phy_tcr1_3, reset value: 0x0, access type: RW
	} bitFields;
} RegPhyTxMacEmuEmuPhyTxPhyTcr13_u;

/*REG_PHY_TX_MAC_EMU_EMU_PHY_TX_PHY_TCR_U_0 0x2120 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 emuPhyTxPhyTcrU0 : 32; //emu_phy_tx_phy_tcr_u_0, reset value: 0x0, access type: RW
	} bitFields;
} RegPhyTxMacEmuEmuPhyTxPhyTcrU0_u;

/*REG_PHY_TX_MAC_EMU_EMU_PHY_TX_PHY_TCR_U_1 0x2124 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 emuPhyTxPhyTcrU1 : 32; //emu_phy_tx_phy_tcr_u_1, reset value: 0x0, access type: RW
	} bitFields;
} RegPhyTxMacEmuEmuPhyTxPhyTcrU1_u;

/*REG_PHY_TX_MAC_EMU_EMU_PHY_TX_PHY_TCR_U_2 0x2128 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 emuPhyTxPhyTcrU2 : 32; //emu_phy_tx_phy_tcr_u_2, reset value: 0x0, access type: RW
	} bitFields;
} RegPhyTxMacEmuEmuPhyTxPhyTcrU2_u;

/*REG_PHY_TX_MAC_EMU_EMU_PHY_TX_PHY_TCR_U_3 0x212C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 emuPhyTxPhyTcrU3 : 32; //emu_phy_tx_phy_tcr_u_3, reset value: 0x0, access type: RW
	} bitFields;
} RegPhyTxMacEmuEmuPhyTxPhyTcrU3_u;

/*REG_PHY_TX_MAC_EMU_TCR0_DATA_0 0x2130 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcr0Data0 : 32; //tcr0_data_0, reset value: 0x0, access type: RO
	} bitFields;
} RegPhyTxMacEmuTcr0Data0_u;

/*REG_PHY_TX_MAC_EMU_TCR0_DATA_1 0x2134 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcr0Data1 : 32; //tcr0_data_1, reset value: 0x0, access type: RO
	} bitFields;
} RegPhyTxMacEmuTcr0Data1_u;

/*REG_PHY_TX_MAC_EMU_TCR0_DATA_2 0x2138 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcr0Data2 : 32; //tcr0_data_2, reset value: 0x0, access type: RO
	} bitFields;
} RegPhyTxMacEmuTcr0Data2_u;

/*REG_PHY_TX_MAC_EMU_TCR0_DATA_3 0x213C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcr0Data3 : 32; //tcr0_data_3, reset value: 0x0, access type: RO
	} bitFields;
} RegPhyTxMacEmuTcr0Data3_u;

/*REG_PHY_TX_MAC_EMU_TCR1_DATA_0 0x2140 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcr1Data0 : 32; //tcr1_data_0, reset value: 0x0, access type: RO
	} bitFields;
} RegPhyTxMacEmuTcr1Data0_u;

/*REG_PHY_TX_MAC_EMU_TCR1_DATA_1 0x2144 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcr1Data1 : 32; //tcr1_data_1, reset value: 0x0, access type: RO
	} bitFields;
} RegPhyTxMacEmuTcr1Data1_u;

/*REG_PHY_TX_MAC_EMU_TCR1_DATA_2 0x2148 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcr1Data2 : 32; //tcr1_data_2, reset value: 0x0, access type: RO
	} bitFields;
} RegPhyTxMacEmuTcr1Data2_u;

/*REG_PHY_TX_MAC_EMU_TCR1_DATA_3 0x214C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcr1Data3 : 32; //tcr1_data_3, reset value: 0x0, access type: RO
	} bitFields;
} RegPhyTxMacEmuTcr1Data3_u;

/*REG_PHY_TX_MAC_EMU_TCR_USER_SELECT_DATA_0 0x2150 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrUserSelectData0 : 32; //tcr_user_select_data_0, reset value: 0x0, access type: RO
	} bitFields;
} RegPhyTxMacEmuTcrUserSelectData0_u;

/*REG_PHY_TX_MAC_EMU_TCR_USER_SELECT_DATA_1 0x2154 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrUserSelectData1 : 32; //tcr_user_select_data_1, reset value: 0x0, access type: RO
	} bitFields;
} RegPhyTxMacEmuTcrUserSelectData1_u;

/*REG_PHY_TX_MAC_EMU_TCR_USER_SELECT_DATA_2 0x2158 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrUserSelectData2 : 32; //tcr_user_select_data_2, reset value: 0x0, access type: RO
	} bitFields;
} RegPhyTxMacEmuTcrUserSelectData2_u;

/*REG_PHY_TX_MAC_EMU_TCR_USER_SELECT_DATA_3 0x215C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrUserSelectData3 : 32; //tcr_user_select_data_3, reset value: 0x0, access type: RO
	} bitFields;
} RegPhyTxMacEmuTcrUserSelectData3_u;

/*REG_PHY_TX_MAC_EMU_TCR_COMMON_ANT_SELECT 0x2160 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrCommonAntSelect : 16; //tcr_common_ant_select, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyTxMacEmuTcrCommonAntSelect_u;

/*REG_PHY_TX_MAC_EMU_TCR_COMMON_ANT_BOOST 0x2164 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrCommonAntBoost : 16; //tcr_common_ant_boost, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyTxMacEmuTcrCommonAntBoost_u;

/*REG_PHY_TX_MAC_EMU_TCR_COMMON_RF_POWER 0x2168 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrCommonRfPower : 8; //tcr_common_rf_power, reset value: 0x0, access type: RO
		uint32 reserved0 : 24;
	} bitFields;
} RegPhyTxMacEmuTcrCommonRfPower_u;

/*REG_PHY_TX_MAC_EMU_TCR_COMMON_LOOP_MODE 0x216C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrCommonLoopMode : 2; //tcr_common_loop_mode, reset value: 0x0, access type: RO
		uint32 reserved0 : 30;
	} bitFields;
} RegPhyTxMacEmuTcrCommonLoopMode_u;

/*REG_PHY_TX_MAC_EMU_TCR_COMMON_CBW 0x2170 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrCommonCbw : 2; //tcr_common_cbw, reset value: 0x0, access type: RO
		uint32 reserved0 : 30;
	} bitFields;
} RegPhyTxMacEmuTcrCommonCbw_u;

/*REG_PHY_TX_MAC_EMU_TCR_COMMON_PHY_MODE 0x2174 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrCommonPhyMode : 3; //tcr_common_phy_mode, reset value: 0x0, access type: RO
		uint32 reserved0 : 29;
	} bitFields;
} RegPhyTxMacEmuTcrCommonPhyMode_u;

/*REG_PHY_TX_MAC_EMU_TCR_COMMON_HE_PUNCTURING_MAP 0x2178 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrCommonHePuncturingMap : 8; //tcr_common_he_puncturing_map, reset value: 0x0, access type: RO
		uint32 reserved0 : 24;
	} bitFields;
} RegPhyTxMacEmuTcrCommonHePuncturingMap_u;

/*REG_PHY_TX_MAC_EMU_TCR_COMMON_FORCE_TX 0x217C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrCommonForceTx : 1; //tcr_common_force_tx, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyTxMacEmuTcrCommonForceTx_u;

/*REG_PHY_TX_MAC_EMU_TCR_COMMON_BSS_COLOR 0x2180 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrCommonBssColor : 6; //tcr_common_bss_color, reset value: 0x0, access type: RO
		uint32 reserved0 : 26;
	} bitFields;
} RegPhyTxMacEmuTcrCommonBssColor_u;

/*REG_PHY_TX_MAC_EMU_TCR_COMMON_CUSTOM_BF 0x2184 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrCommonCustomBf : 2; //tcr_common_custom_bf, reset value: 0x0, access type: RO
		uint32 reserved0 : 30;
	} bitFields;
} RegPhyTxMacEmuTcrCommonCustomBf_u;

/*REG_PHY_TX_MAC_EMU_TCR_COMMON_N_HE_LTF 0x2188 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrCommonNHeLtf : 3; //tcr_common_n_he_ltf, reset value: 0x0, access type: RO
		uint32 reserved0 : 29;
	} bitFields;
} RegPhyTxMacEmuTcrCommonNHeLtf_u;

/*REG_PHY_TX_MAC_EMU_TCR_COMMON_UL_DL 0x218C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrCommonUlDl : 1; //tcr_common_ul_dl, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyTxMacEmuTcrCommonUlDl_u;

/*REG_PHY_TX_MAC_EMU_TCR_COMMON_TXOP_PS_NOT_ALLOWED 0x2190 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrCommonTxopPsNotAllowed : 1; //tcr_common_txop_ps_not_allowed, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyTxMacEmuTcrCommonTxopPsNotAllowed_u;

/*REG_PHY_TX_MAC_EMU_TCR_COMMON_EXPECT_RCR 0x2194 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrCommonExpectRcr : 1; //tcr_common_expect_rcr, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyTxMacEmuTcrCommonExpectRcr_u;

/*REG_PHY_TX_MAC_EMU_TCR_COMMON_BW_CHANGE 0x2198 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrCommonBwChange : 1; //tcr_common_bw_change, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyTxMacEmuTcrCommonBwChange_u;

/*REG_PHY_TX_MAC_EMU_TCR_COMMON_TXOP_DURATION_FROM_MAC 0x219C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrCommonTxopDurationFromMac : 1; //tcr_common_txop_duration_from_mac, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyTxMacEmuTcrCommonTxopDurationFromMac_u;

/*REG_PHY_TX_MAC_EMU_TCR_COMMON_AGGREGATE 0x21A0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrCommonAggregate : 1; //tcr_common_aggregate, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyTxMacEmuTcrCommonAggregate_u;

/*REG_PHY_TX_MAC_EMU_TCR_COMMON_DYN_BW 0x21A4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrCommonDynBw : 1; //tcr_common_dyn_bw, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyTxMacEmuTcrCommonDynBw_u;

/*REG_PHY_TX_MAC_EMU_TCR_COMMON_MU_PHY_NDP 0x21A8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrCommonMuPhyNdp : 1; //tcr_common_mu_phy_ndp, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyTxMacEmuTcrCommonMuPhyNdp_u;

/*REG_PHY_TX_MAC_EMU_TCR_COMMON_MU_TRAINING 0x21AC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrCommonMuTraining : 1; //tcr_common_mu_training, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyTxMacEmuTcrCommonMuTraining_u;

/*REG_PHY_TX_MAC_EMU_TCR_COMMON_HE_CP 0x21B0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrCommonHeCp : 2; //tcr_common_he_cp, reset value: 0x0, access type: RO
		uint32 reserved0 : 30;
	} bitFields;
} RegPhyTxMacEmuTcrCommonHeCp_u;

/*REG_PHY_TX_MAC_EMU_TCR_COMMON_HE_LTF 0x21B4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrCommonHeLtf : 2; //tcr_common_he_ltf, reset value: 0x0, access type: RO
		uint32 reserved0 : 30;
	} bitFields;
} RegPhyTxMacEmuTcrCommonHeLtf_u;

/*REG_PHY_TX_MAC_EMU_TCR_COMMON_SCP 0x21B8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrCommonScp : 1; //tcr_common_scp, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyTxMacEmuTcrCommonScp_u;

/*REG_PHY_TX_MAC_EMU_TCR_COMMON_SMOOTHING 0x21BC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrCommonSmoothing : 1; //tcr_common_smoothing, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyTxMacEmuTcrCommonSmoothing_u;

/*REG_PHY_TX_MAC_EMU_TCR_COMMON_NOT_SOUNDING 0x21C0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrCommonNotSounding : 1; //tcr_common_not_sounding   , reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyTxMacEmuTcrCommonNotSounding_u;

/*REG_PHY_TX_MAC_EMU_TCR_COMMON_STBC 0x21C4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrCommonStbc : 1; //tcr_common_stbc, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyTxMacEmuTcrCommonStbc_u;

/*REG_PHY_TX_MAC_EMU_TCR_COMMON_SPATIAL_REUSE 0x21C8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrCommonSpatialReuse : 16; //tcr_common_spatial_reuse, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyTxMacEmuTcrCommonSpatialReuse_u;

/*REG_PHY_TX_MAC_EMU_TCR_COMMON_TRIGGER_BASED_DATA 0x21CC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrCommonTriggerBasedData : 16; //tcr_common_trigger_based_data   , reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyTxMacEmuTcrCommonTriggerBasedData_u;

/*REG_PHY_TX_MAC_EMU_TCR_COMMON_GROUP_ID 0x21D0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrCommonGroupId : 6; //tcr_common_group_id, reset value: 0x0, access type: RO
		uint32 reserved0 : 26;
	} bitFields;
} RegPhyTxMacEmuTcrCommonGroupId_u;

/*REG_PHY_TX_MAC_EMU_TCR_COMMON_HE_SIGB_RATE 0x21D4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrCommonHeSigbRate : 8; //tcr_common_he_sigb_rate, reset value: 0x0, access type: RO
		uint32 reserved0 : 24;
	} bitFields;
} RegPhyTxMacEmuTcrCommonHeSigbRate_u;

/*REG_PHY_TX_MAC_EMU_TCR_COMMON_MAC_DURATION 0x21D8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrCommonMacDuration : 15; //tcr_common_mac_duration, reset value: 0x0, access type: RO
		uint32 reserved0 : 17;
	} bitFields;
} RegPhyTxMacEmuTcrCommonMacDuration_u;

/*REG_PHY_TX_MAC_EMU_TCR_USER_SELECT_PSDU_LENGTH 0x21DC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrUserSelectPsduLength : 22; //tcr_user_select_psdu_length, reset value: 0x0, access type: RO
		uint32 reserved0 : 10;
	} bitFields;
} RegPhyTxMacEmuTcrUserSelectPsduLength_u;

/*REG_PHY_TX_MAC_EMU_TCR_USER_SELECT_SUB_BAND 0x21E0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrUserSelectSubBand : 3; //tcr_user_select_sub_band, reset value: 0x0, access type: RO
		uint32 reserved0 : 29;
	} bitFields;
} RegPhyTxMacEmuTcrUserSelectSubBand_u;

/*REG_PHY_TX_MAC_EMU_TCR_USER_SELECT_START_RU 0x21E4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrUserSelectStartRu : 4; //tcr_user_select_start_ru, reset value: 0x0, access type: RO
		uint32 reserved0 : 28;
	} bitFields;
} RegPhyTxMacEmuTcrUserSelectStartRu_u;

/*REG_PHY_TX_MAC_EMU_TCR_USER_SELECT_RU_SIZE 0x21E8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrUserSelectRuSize : 3; //tcr_user_select_ru_size, reset value: 0x0, access type: RO
		uint32 reserved0 : 29;
	} bitFields;
} RegPhyTxMacEmuTcrUserSelectRuSize_u;

/*REG_PHY_TX_MAC_EMU_TCR_USER_SELECT_LDPC 0x21EC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrUserSelectLdpc : 1; //tcr_user_select_ldpc, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyTxMacEmuTcrUserSelectLdpc_u;

/*REG_PHY_TX_MAC_EMU_TCR_USER_SELECT_TXBF 0x21F0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrUserSelectTxbf : 1; //tcr_user_select_txbf, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyTxMacEmuTcrUserSelectTxbf_u;

/*REG_PHY_TX_MAC_EMU_TCR_USER_SELECT_PSDU_RATE 0x21F4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrUserSelectPsduRate : 8; //tcr_user_select_psdu_rate, reset value: 0x0, access type: RO
		uint32 reserved0 : 24;
	} bitFields;
} RegPhyTxMacEmuTcrUserSelectPsduRate_u;

/*REG_PHY_TX_MAC_EMU_TCR_USER_SELECT_RELATIVE_POWER 0x21F8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrUserSelectRelativePower : 4; //tcr_user_select_relative_power, reset value: 0x0, access type: RO
		uint32 reserved0 : 28;
	} bitFields;
} RegPhyTxMacEmuTcrUserSelectRelativePower_u;

/*REG_PHY_TX_MAC_EMU_TCR_USER_SELECT_PACKET_EXTENSION 0x21FC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrUserSelectPacketExtension : 2; //tcr_user_select_packet_extension, reset value: 0x0, access type: RO
		uint32 reserved0 : 30;
	} bitFields;
} RegPhyTxMacEmuTcrUserSelectPacketExtension_u;

/*REG_PHY_TX_MAC_EMU_TCR_USER_SELECT_STATION_ID 0x2200 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrUserSelectStationId : 11; //tcr_user_select_station_id, reset value: 0x0, access type: RO
		uint32 reserved0 : 21;
	} bitFields;
} RegPhyTxMacEmuTcrUserSelectStationId_u;

/*REG_PHY_TX_MAC_EMU_TCR_USER_SELECT_BF_INDEX 0x2204 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrUserSelectBfIndex : 8; //tcr_user_select_bf_index, reset value: 0x0, access type: RO
		uint32 reserved0 : 24;
	} bitFields;
} RegPhyTxMacEmuTcrUserSelectBfIndex_u;

/*REG_PHY_TX_MAC_EMU_TCR_USER_SELECT_PSDU_MCS 0x2208 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrUserSelectPsduMcs : 6; //tcr_user_select_psdu_mcs, reset value: 0x0, access type: RO
		uint32 reserved0 : 26;
	} bitFields;
} RegPhyTxMacEmuTcrUserSelectPsduMcs_u;

/*REG_PHY_TX_MAC_EMU_TCR_USER_SELECT_PSDU_RANK 0x220C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrUserSelectPsduRank : 2; //tcr_user_select_psdu_rank, reset value: 0x0, access type: RO
		uint32 reserved0 : 30;
	} bitFields;
} RegPhyTxMacEmuTcrUserSelectPsduRank_u;

/*REG_PHY_TX_MAC_EMU_TCR_USER_SELECT_PSDU_DCM 0x2210 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrUserSelectPsduDcm : 1; //tcr_user_select_psdu_dcm, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyTxMacEmuTcrUserSelectPsduDcm_u;

/*REG_PHY_TX_MAC_EMU_AIR_TIME_VALID 0x2214 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 airTimeValid : 1; //air_time_valid, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyTxMacEmuAirTimeValid_u;

/*REG_PHY_TX_MAC_EMU_AIR_TIME_COMMON_L_LENGTH_EXCEED 0x2218 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 airTimeCommonLLengthExceed : 1; //air_time_common_l_length_exceed, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyTxMacEmuAirTimeCommonLLengthExceed_u;

/*REG_PHY_TX_MAC_EMU_AIR_TIME_COMMON_NUM_SYM 0x221C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 airTimeCommonNumSym : 15; //air_time_common_num_sym, reset value: 0x0, access type: RO
		uint32 reserved0 : 17;
	} bitFields;
} RegPhyTxMacEmuAirTimeCommonNumSym_u;

/*REG_PHY_TX_MAC_EMU_AIR_TIME_COMMON_A_FACTOR 0x2220 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 airTimeCommonAFactor : 2; //air_time_common_a_factor, reset value: 0x0, access type: RO
		uint32 reserved0 : 30;
	} bitFields;
} RegPhyTxMacEmuAirTimeCommonAFactor_u;

/*REG_PHY_TX_MAC_EMU_AIR_TIME_COMMON_T_PE 0x2224 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 airTimeCommonTPe : 5; //air_time_common_t_pe, reset value: 0x0, access type: RO
		uint32 reserved0 : 27;
	} bitFields;
} RegPhyTxMacEmuAirTimeCommonTPe_u;

/*REG_PHY_TX_MAC_EMU_AIR_TIME_COMMON_AIR_TIME_CYCLES 0x2228 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 airTimeCommonAirTimeCycles : 26; //air_time_common_air_time_cycles, reset value: 0x0, access type: RO
		uint32 reserved0 : 6;
	} bitFields;
} RegPhyTxMacEmuAirTimeCommonAirTimeCycles_u;

/*REG_PHY_TX_MAC_EMU_AIR_TIME_COMMON_PREAMBLE_TIME 0x222C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 airTimeCommonPreambleTime : 18; //air_time_common_preamble_time, reset value: 0x0, access type: RO
		uint32 reserved0 : 14;
	} bitFields;
} RegPhyTxMacEmuAirTimeCommonPreambleTime_u;

/*REG_PHY_TX_MAC_EMU_AIR_TIME_COMMON_LDPC_EXTRA_SYMBOL 0x2230 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 airTimeCommonLdpcExtraSymbol : 1; //air_time_common_ldpc_extra_symbol, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyTxMacEmuAirTimeCommonLdpcExtraSymbol_u;

/*REG_PHY_TX_MAC_EMU_AIR_TIME_COMMON_AIR_TIME_RESULT 0x2234 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 airTimeCommonAirTimeResult : 17; //air_time_common_air_time_result, reset value: 0x0, access type: RO
		uint32 reserved0 : 15;
	} bitFields;
} RegPhyTxMacEmuAirTimeCommonAirTimeResult_u;

/*REG_PHY_TX_MAC_EMU_AIR_TIME_COMMON_L_LENGTH 0x2238 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 airTimeCommonLLength : 12; //air_time_common_l_length, reset value: 0x0, access type: RO
		uint32 reserved0 : 20;
	} bitFields;
} RegPhyTxMacEmuAirTimeCommonLLength_u;

/*REG_PHY_TX_MAC_EMU_AIR_TIME_COMMON_VHT_SIG_A2_B1 0x223C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 airTimeCommonVhtSigA2B1 : 1; //air_time_common_vht_sig_a2_b1, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyTxMacEmuAirTimeCommonVhtSigA2B1_u;

/*REG_PHY_TX_MAC_EMU_AIR_TIME_COMMON_PE_DISAMBIGUITY 0x2240 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 airTimeCommonPeDisambiguity : 1; //air_time_common_pe_disambiguity, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyTxMacEmuAirTimeCommonPeDisambiguity_u;

/*REG_PHY_TX_MAC_EMU_AIR_TIME_USER_SELECT_CODING_RATE 0x2244 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 airTimeUserSelectCodingRate : 2; //air_time_user_select_coding_rate, reset value: 0x0, access type: RO
		uint32 reserved0 : 30;
	} bitFields;
} RegPhyTxMacEmuAirTimeUserSelectCodingRate_u;

/*REG_PHY_TX_MAC_EMU_AIR_TIME_USER_SELECT_MODULATION 0x2248 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 airTimeUserSelectModulation : 3; //air_time_user_select_modulation, reset value: 0x0, access type: RO
		uint32 reserved0 : 29;
	} bitFields;
} RegPhyTxMacEmuAirTimeUserSelectModulation_u;

/*REG_PHY_TX_MAC_EMU_AIR_TIME_USER_SELECT_LDPC_REM_NSHRT_NCW 0x224C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 airTimeUserSelectLdpcRemNshrtNcw : 15; //air_time_user_select_ldpc_rem_nshrt_ncw, reset value: 0x0, access type: RO
		uint32 reserved0 : 17;
	} bitFields;
} RegPhyTxMacEmuAirTimeUserSelectLdpcRemNshrtNcw_u;

/*REG_PHY_TX_MAC_EMU_AIR_TIME_USER_SELECT_LDPC_REM_NPUNC_NCW 0x2250 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 airTimeUserSelectLdpcRemNpuncNcw : 15; //air_time_user_select_ldpc_rem_npunc_ncw, reset value: 0x0, access type: RO
		uint32 reserved0 : 17;
	} bitFields;
} RegPhyTxMacEmuAirTimeUserSelectLdpcRemNpuncNcw_u;

/*REG_PHY_TX_MAC_EMU_AIR_TIME_USER_SELECT_LDPC_REM_NREP_NCW 0x2254 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 airTimeUserSelectLdpcRemNrepNcw : 15; //air_time_user_select_ldpc_rem_nrep_ncw, reset value: 0x0, access type: RO
		uint32 reserved0 : 17;
	} bitFields;
} RegPhyTxMacEmuAirTimeUserSelectLdpcRemNrepNcw_u;

/*REG_PHY_TX_MAC_EMU_AIR_TIME_USER_SELECT_LDPC_N_DATA 0x2258 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 airTimeUserSelectLdpcNData : 11; //air_time_user_select_ldpc_n_data, reset value: 0x0, access type: RO
		uint32 reserved0 : 21;
	} bitFields;
} RegPhyTxMacEmuAirTimeUserSelectLdpcNData_u;

/*REG_PHY_TX_MAC_EMU_AIR_TIME_USER_SELECT_LDPC_N_PARITY 0x225C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 airTimeUserSelectLdpcNParity : 11; //air_time_user_select_ldpc_n_parity, reset value: 0x0, access type: RO
		uint32 reserved0 : 21;
	} bitFields;
} RegPhyTxMacEmuAirTimeUserSelectLdpcNParity_u;

/*REG_PHY_TX_MAC_EMU_AIR_TIME_USER_SELECT_LDPC_N_REPEAT 0x2260 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 airTimeUserSelectLdpcNRepeat : 15; //air_time_user_select_ldpc_n_repeat, reset value: 0x0, access type: RO
		uint32 reserved0 : 17;
	} bitFields;
} RegPhyTxMacEmuAirTimeUserSelectLdpcNRepeat_u;

/*REG_PHY_TX_MAC_EMU_AIR_TIME_USER_SELECT_LDPC_CW_TYPE 0x2264 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 airTimeUserSelectLdpcCwType : 2; //air_time_user_select_ldpc_cw_type, reset value: 0x0, access type: RO
		uint32 reserved0 : 30;
	} bitFields;
} RegPhyTxMacEmuAirTimeUserSelectLdpcCwType_u;

/*REG_PHY_TX_MAC_EMU_AIR_TIME_USER_SELECT_LDPC_N_CW 0x2268 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 airTimeUserSelectLdpcNCw : 15; //air_time_user_select_ldpc_n_cw, reset value: 0x0, access type: RO
		uint32 reserved0 : 17;
	} bitFields;
} RegPhyTxMacEmuAirTimeUserSelectLdpcNCw_u;

/*REG_PHY_TX_MAC_EMU_AIR_TIME_USER_SELECT_PADDING 0x226C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 airTimeUserSelectPadding : 25; //air_time_user_select_padding, reset value: 0x0, access type: RO
		uint32 reserved0 : 7;
	} bitFields;
} RegPhyTxMacEmuAirTimeUserSelectPadding_u;

/*REG_PHY_TX_MAC_EMU_AIR_TIME_USER_SELECT_NES 0x2270 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 airTimeUserSelectNes : 3; //air_time_user_select_nes, reset value: 0x0, access type: RO
		uint32 reserved0 : 29;
	} bitFields;
} RegPhyTxMacEmuAirTimeUserSelectNes_u;

/*REG_PHY_TX_MAC_EMU_AIR_TIME_USER_SELECT_N_CBPS 0x2274 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 airTimeUserSelectNCbps : 17; //air_time_user_select_n_cbps, reset value: 0x0, access type: RO
		uint32 reserved0 : 15;
	} bitFields;
} RegPhyTxMacEmuAirTimeUserSelectNCbps_u;

/*REG_PHY_TX_MAC_EMU_AIR_TIME_USER_SELECT_N_DBPS 0x2278 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 airTimeUserSelectNDbps : 17; //air_time_user_select_n_dbps, reset value: 0x0, access type: RO
		uint32 reserved0 : 15;
	} bitFields;
} RegPhyTxMacEmuAirTimeUserSelectNDbps_u;

/*REG_PHY_TX_MAC_EMU_AIR_TIME_USER_SELECT_N_SD_LAST 0x227C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 airTimeUserSelectNSdLast : 11; //air_time_user_select_n_sd_last, reset value: 0x0, access type: RO
		uint32 reserved0 : 21;
	} bitFields;
} RegPhyTxMacEmuAirTimeUserSelectNSdLast_u;

/*REG_PHY_TX_MAC_EMU_HE_SIGB_NUM_SYM 0x2280 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 heSigbNumSym : 5; //he_sigb_num_sym, reset value: 0x0, access type: RO
		uint32 reserved0 : 27;
	} bitFields;
} RegPhyTxMacEmuHeSigbNumSym_u;

/*REG_PHY_TX_MAC_EMU_PHY_MAC_ERROR 0x2284 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txTcr0TimingError : 1; //tx_tcr0_timing_error, reset value: 0x0, access type: RO
		uint32 txTcr1TimingError : 1; //tx_tcr1_timing_error, reset value: 0x0, access type: RO
		uint32 txTcrUTimingError : 1; //tx_tcr_u_timing_error, reset value: 0x0, access type: RO
		uint32 txTcrMuTimingError : 1; //tx_tcr_mu_timing_error, reset value: 0x0, access type: RO
		uint32 txBfError : 1; //tx_bf_error, reset value: 0x0, access type: RO
		uint32 txLLengthExceedError : 1; //tx_l_length_exceed_error, reset value: 0x0, access type: RO
		uint32 txRcrTimingError : 1; //tx_rcr_timing_error, reset value: 0x0, access type: RO
		uint32 txHeSigbNumSymError : 1; //tx_he_sigb_num_sym_error, reset value: 0x0, access type: RO
		uint32 txEndOfAirError : 1; //tx_end_of_air_error, reset value: 0x0, access type: RO
		uint32 reserved0 : 23;
	} bitFields;
} RegPhyTxMacEmuPhyMacError_u;

/*REG_PHY_TX_MAC_EMU_PHY_MAC_ERROR_CLEAR 0x2288 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txMacErrorClear : 1; //clears all error indications, reset value: 0x0, access type: WO
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyTxMacEmuPhyMacErrorClear_u;

/*REG_PHY_TX_MAC_EMU_PHY_MAC_ERROR_INT_EN 0x228C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txTcr0TimingErrorIntEn : 1; //tx_tcr0_timing_error_int_en, reset value: 0x1, access type: RW
		uint32 txTcr1TimingErrorIntEn : 1; //tx_tcr1_timing_error_int_en, reset value: 0x1, access type: RW
		uint32 txTcrUTimingErrorIntEn : 1; //tx_tcr_u_timing_error_int_en, reset value: 0x1, access type: RW
		uint32 txTcrMuTimingErrorIntEn : 1; //tx_tcr_mu_timing_error_int_en, reset value: 0x1, access type: RW
		uint32 txBfErrorIntEn : 1; //tx_bf_error_int_en, reset value: 0x1, access type: RW
		uint32 txLLengthExceedErrorIntEn : 1; //tx_l_length_exceed_error_int_en, reset value: 0x1, access type: RW
		uint32 txRcrTimingErrorIntEn : 1; //tx_rcr_timing_error_int_en, reset value: 0x1, access type: RW
		uint32 txHeSigbNumSymErrorIntEn : 1; //tx_he_sigb_num_sym_error_int_en, reset value: 0x1, access type: RW
		uint32 txEndOfAirErrorIntEn : 1; //tx_end_of_air_error_int_en, reset value: 0x1, access type: RW
		uint32 reserved0 : 23;
	} bitFields;
} RegPhyTxMacEmuPhyMacErrorIntEn_u;

/*REG_PHY_TX_MAC_EMU_BB_CPU_AIR_TIME_CALC_START 0x2800 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbCpuAirTimeCalcStart : 1; //bb_cpu_air_time_calc_start       , reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyTxMacEmuBbCpuAirTimeCalcStart_u;

/*REG_PHY_TX_MAC_EMU_BB_CPU_LDPC 0x2804 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbCpuLdpc : 1; //bb_cpu_ldpc                      , reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyTxMacEmuBbCpuLdpc_u;

/*REG_PHY_TX_MAC_EMU_BB_CPU_SCP 0x2808 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbCpuScp : 1; //bb_cpu_scp                       , reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyTxMacEmuBbCpuScp_u;

/*REG_PHY_TX_MAC_EMU_BB_CPU_STBC 0x280C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbCpuStbc : 2; //bb_cpu_stbc                      , reset value: 0x0, access type: RW
		uint32 reserved0 : 30;
	} bitFields;
} RegPhyTxMacEmuBbCpuStbc_u;

/*REG_PHY_TX_MAC_EMU_BB_CPU_PSDU_RATE 0x2810 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbCpuPsduRate : 8; //bb_cpu_psdu_rate                 , reset value: 0x0, access type: RW
		uint32 reserved0 : 24;
	} bitFields;
} RegPhyTxMacEmuBbCpuPsduRate_u;

/*REG_PHY_TX_MAC_EMU_BB_CPU_BW 0x2814 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbCpuBw : 2; //bb_cpu_bw                        , reset value: 0x0, access type: RW
		uint32 reserved0 : 30;
	} bitFields;
} RegPhyTxMacEmuBbCpuBw_u;

/*REG_PHY_TX_MAC_EMU_BB_CPU_PACKET_MODE 0x2818 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbCpuPacketMode : 3; //bb_cpu_packet_mode               , reset value: 0x0, access type: RW
		uint32 reserved0 : 29;
	} bitFields;
} RegPhyTxMacEmuBbCpuPacketMode_u;

/*REG_PHY_TX_MAC_EMU_BB_CPU_NESS 0x281C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbCpuNess : 3; //bb_cpu_ness                      , reset value: 0x0, access type: RW
		uint32 reserved0 : 29;
	} bitFields;
} RegPhyTxMacEmuBbCpuNess_u;

/*REG_PHY_TX_MAC_EMU_BB_CPU_PACKET_LENGTH 0x2820 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbCpuPacketLength : 22; //bb_cpu_packet_length             , reset value: 0x0, access type: RW
		uint32 reserved0 : 10;
	} bitFields;
} RegPhyTxMacEmuBbCpuPacketLength_u;

/*REG_PHY_TX_MAC_EMU_BB_CPU_RX_TX_N_SELECTOR 0x2824 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbCpuRxTxNSelector : 1; //bb_cpu_rx_tx_n_selector          , reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyTxMacEmuBbCpuRxTxNSelector_u;

/*REG_PHY_TX_MAC_EMU_BB_CPU_RX_L_LENGTH 0x2828 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbCpuRxLLength : 12; //bb_cpu_rx_l_length               , reset value: 0x0, access type: RW
		uint32 reserved0 : 20;
	} bitFields;
} RegPhyTxMacEmuBbCpuRxLLength_u;

/*REG_PHY_TX_MAC_EMU_BB_CPU_RX_VHT_SIG_A2_B1 0x282C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbCpuRxVhtSigA2B1 : 1; //bb_cpu_rx_vht_sig_a2_b1          , reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyTxMacEmuBbCpuRxVhtSigA2B1_u;

/*REG_PHY_TX_MAC_EMU_BB_CPU_RX_LDPC_EXTRA_SYMBOL 0x2830 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbCpuRxLdpcExtraSymbol : 1; //bb_cpu_rx_ldpc_extra_symbol      , reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyTxMacEmuBbCpuRxLdpcExtraSymbol_u;

/*REG_PHY_TX_MAC_EMU_BB_CPU_MU_STAGE 0x2834 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbCpuMuStage : 2; //bb_cpu_mu_stage                  , reset value: 0x0, access type: RW
		uint32 reserved0 : 30;
	} bitFields;
} RegPhyTxMacEmuBbCpuMuStage_u;

/*REG_PHY_TX_MAC_EMU_BB_CPU_N_SYM_INIT 0x2838 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbCpuNSymInit : 15; //bb_cpu_n_sym_init                , reset value: 0x0, access type: RW
		uint32 reserved0 : 17;
	} bitFields;
} RegPhyTxMacEmuBbCpuNSymInit_u;

/*REG_PHY_TX_MAC_EMU_BB_CPU_N_SYM 0x283C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbCpuNSym : 15; //bb_cpu_n_sym                     , reset value: 0x0, access type: RW
		uint32 reserved0 : 17;
	} bitFields;
} RegPhyTxMacEmuBbCpuNSym_u;

/*REG_PHY_TX_MAC_EMU_BB_CPU_N_LTF_TOTAL 0x2840 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbCpuNLtfTotal : 3; //bb_cpu_n_ltf_total               , reset value: 0x0, access type: RW
		uint32 reserved0 : 29;
	} bitFields;
} RegPhyTxMacEmuBbCpuNLtfTotal_u;

/*REG_PHY_TX_MAC_EMU_BB_CPU_HE_RU_SIZE 0x2844 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbCpuHeRuSize : 3; //bb_cpu_he_ru_size                , reset value: 0x0, access type: RW
		uint32 reserved0 : 29;
	} bitFields;
} RegPhyTxMacEmuBbCpuHeRuSize_u;

/*REG_PHY_TX_MAC_EMU_BB_CPU_N_SIGB_SYM 0x2848 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbCpuNSigbSym : 4; //bb_cpu_n_sigb_sym                , reset value: 0x0, access type: RW
		uint32 reserved0 : 28;
	} bitFields;
} RegPhyTxMacEmuBbCpuNSigbSym_u;

/*REG_PHY_TX_MAC_EMU_BB_CPU_HE_CP 0x284C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbCpuHeCp : 2; //bb_cpu_he_cp                     , reset value: 0x0, access type: RW
		uint32 reserved0 : 30;
	} bitFields;
} RegPhyTxMacEmuBbCpuHeCp_u;

/*REG_PHY_TX_MAC_EMU_BB_CPU_HE_LTF 0x2850 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbCpuHeLtf : 2; //bb_cpu_he_ltf                    , reset value: 0x0, access type: RW
		uint32 reserved0 : 30;
	} bitFields;
} RegPhyTxMacEmuBbCpuHeLtf_u;

/*REG_PHY_TX_MAC_EMU_BB_CPU_PE_DISAMBIGUITY 0x2854 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbCpuPeDisambiguity : 1; //bb_cpu_pe_disambiguity           , reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyTxMacEmuBbCpuPeDisambiguity_u;

/*REG_PHY_TX_MAC_EMU_BB_CPU_A_FACTOR 0x2858 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbCpuAFactor : 2; //bb_cpu_a_factor                  , reset value: 0x0, access type: RW
		uint32 reserved0 : 30;
	} bitFields;
} RegPhyTxMacEmuBbCpuAFactor_u;

/*REG_PHY_TX_MAC_EMU_BB_CPU_A_FACTOR_INIT 0x285C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbCpuAFactorInit : 2; //bb_cpu_a_factor_init             , reset value: 0x0, access type: RW
		uint32 reserved0 : 30;
	} bitFields;
} RegPhyTxMacEmuBbCpuAFactorInit_u;

/*REG_PHY_TX_MAC_EMU_BB_CPU_MAX_PE 0x2860 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbCpuMaxPe : 2; //bb_cpu_max_pe                    , reset value: 0x0, access type: RW
		uint32 reserved0 : 30;
	} bitFields;
} RegPhyTxMacEmuBbCpuMaxPe_u;

/*REG_PHY_TX_MAC_EMU_BB_CPU_AIR_TIME_CALC_VALID 0x2864 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbCpuAirTimeCalcValid : 1; //bb_cpu_air_time_calc_valid       , reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyTxMacEmuBbCpuAirTimeCalcValid_u;

/*REG_PHY_TX_MAC_EMU_BB_CPU_L_LENGTH_EXCEED 0x2868 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbCpuLLengthExceed : 1; //bb_cpu_l_length_exceed           , reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyTxMacEmuBbCpuLLengthExceed_u;

/*REG_PHY_TX_MAC_EMU_BB_CPU_VHT_SIG_A2_B1 0x286C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbCpuVhtSigA2B1 : 1; //bb_cpu_vht_sig_a2_b1             , reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyTxMacEmuBbCpuVhtSigA2B1_u;

/*REG_PHY_TX_MAC_EMU_BB_CPU_LDPC_EXTRA_SYMBOL 0x2870 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbCpuLdpcExtraSymbol : 1; //bb_cpu_ldpc_extra_symbol         , reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyTxMacEmuBbCpuLdpcExtraSymbol_u;

/*REG_PHY_TX_MAC_EMU_BB_CPU_CODING_RATE 0x2874 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbCpuCodingRate : 2; //bb_cpu_coding_rate               , reset value: 0x0, access type: RO
		uint32 reserved0 : 30;
	} bitFields;
} RegPhyTxMacEmuBbCpuCodingRate_u;

/*REG_PHY_TX_MAC_EMU_BB_CPU_MODULATION 0x2878 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbCpuModulation : 3; //bb_cpu_modulation                , reset value: 0x0, access type: RO
		uint32 reserved0 : 29;
	} bitFields;
} RegPhyTxMacEmuBbCpuModulation_u;

/*REG_PHY_TX_MAC_EMU_BB_CPU_AIR_TIME_RESULT 0x287C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbCpuAirTimeResult : 17; //bb_cpu_air_time_result           , reset value: 0x0, access type: RO
		uint32 reserved0 : 15;
	} bitFields;
} RegPhyTxMacEmuBbCpuAirTimeResult_u;

/*REG_PHY_TX_MAC_EMU_BB_CPU_L_LENGTH_RESULT 0x2880 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbCpuLLengthResult : 12; //bb_cpu_l_length_result           , reset value: 0x0, access type: RO
		uint32 reserved0 : 20;
	} bitFields;
} RegPhyTxMacEmuBbCpuLLengthResult_u;

/*REG_PHY_TX_MAC_EMU_BB_CPU_NUM_SYM_RESULT 0x2884 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbCpuNumSymResult : 15; //bb_cpu_num_sym_result            , reset value: 0x0, access type: RO
		uint32 reserved0 : 17;
	} bitFields;
} RegPhyTxMacEmuBbCpuNumSymResult_u;

/*REG_PHY_TX_MAC_EMU_BB_CPU_NUM_SYM_INIT_RESULT 0x2888 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbCpuNumSymInitResult : 15; //bb_cpu_num_sym_init_result       , reset value: 0x0, access type: RO
		uint32 reserved0 : 17;
	} bitFields;
} RegPhyTxMacEmuBbCpuNumSymInitResult_u;

/*REG_PHY_TX_MAC_EMU_BB_CPU_LDPC_REM_NSHRT_NCW 0x288C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbCpuLdpcRemNshrtNcw : 15; //bb_cpu_ldpc_rem_nshrt_ncw        , reset value: 0x0, access type: RO
		uint32 reserved0 : 17;
	} bitFields;
} RegPhyTxMacEmuBbCpuLdpcRemNshrtNcw_u;

/*REG_PHY_TX_MAC_EMU_BB_CPU_LDPC_REM_NPUNC_NCW 0x2890 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbCpuLdpcRemNpuncNcw : 15; //bb_cpu_ldpc_rem_npunc_ncw        , reset value: 0x0, access type: RO
		uint32 reserved0 : 17;
	} bitFields;
} RegPhyTxMacEmuBbCpuLdpcRemNpuncNcw_u;

/*REG_PHY_TX_MAC_EMU_BB_CPU_LDPC_REM_NREP_NCW 0x2894 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbCpuLdpcRemNrepNcw : 15; //bb_cpu_ldpc_rem_nrep_ncw         , reset value: 0x0, access type: RO
		uint32 reserved0 : 17;
	} bitFields;
} RegPhyTxMacEmuBbCpuLdpcRemNrepNcw_u;

/*REG_PHY_TX_MAC_EMU_BB_CPU_LDPC_N_DATA 0x2898 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbCpuLdpcNData : 11; //bb_cpu_ldpc_n_data               , reset value: 0x0, access type: RO
		uint32 reserved0 : 21;
	} bitFields;
} RegPhyTxMacEmuBbCpuLdpcNData_u;

/*REG_PHY_TX_MAC_EMU_BB_CPU_LDPC_N_PARITY 0x289C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbCpuLdpcNParity : 11; //bb_cpu_ldpc_n_parity             , reset value: 0x0, access type: RO
		uint32 reserved0 : 21;
	} bitFields;
} RegPhyTxMacEmuBbCpuLdpcNParity_u;

/*REG_PHY_TX_MAC_EMU_BB_CPU_LDPC_N_REPEAT 0x28A0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbCpuLdpcNRepeat : 15; //bb_cpu_ldpc_n_repeat             , reset value: 0x0, access type: RO
		uint32 reserved0 : 17;
	} bitFields;
} RegPhyTxMacEmuBbCpuLdpcNRepeat_u;

/*REG_PHY_TX_MAC_EMU_BB_CPU_LDPC_CW_TYPE 0x28A4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbCpuLdpcCwType : 2; //bb_cpu_ldpc_cw_type              , reset value: 0x0, access type: RO
		uint32 reserved0 : 30;
	} bitFields;
} RegPhyTxMacEmuBbCpuLdpcCwType_u;

/*REG_PHY_TX_MAC_EMU_BB_CPU_LDPC_N_CW 0x28A8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbCpuLdpcNCw : 15; //bb_cpu_ldpc_n_cw                 , reset value: 0x0, access type: RO
		uint32 reserved0 : 17;
	} bitFields;
} RegPhyTxMacEmuBbCpuLdpcNCw_u;

/*REG_PHY_TX_MAC_EMU_BB_CPU_TX_NUM_PADDING_BITS 0x28AC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbCpuTxNumPaddingBits : 25; //bb_cpu_tx_num_padding_bits       , reset value: 0x0, access type: RO
		uint32 reserved0 : 7;
	} bitFields;
} RegPhyTxMacEmuBbCpuTxNumPaddingBits_u;

/*REG_PHY_TX_MAC_EMU_BB_CPU_N_PAD_POST_FEC 0x28B0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbCpuNPadPostFec : 17; //bb_cpu_n_pad_post_fec            , reset value: 0x0, access type: RO
		uint32 reserved0 : 15;
	} bitFields;
} RegPhyTxMacEmuBbCpuNPadPostFec_u;

/*REG_PHY_TX_MAC_EMU_BB_CPU_VITERBI_N_ES 0x28B4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbCpuViterbiNEs : 3; //bb_cpu_viterbi_n_es              , reset value: 0x0, access type: RO
		uint32 reserved0 : 29;
	} bitFields;
} RegPhyTxMacEmuBbCpuViterbiNEs_u;

/*REG_PHY_TX_MAC_EMU_BB_CPU_T_PE 0x28B8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbCpuTPe : 5; //bb_cpu_t_pe                      , reset value: 0x0, access type: RO
		uint32 reserved0 : 27;
	} bitFields;
} RegPhyTxMacEmuBbCpuTPe_u;

/*REG_PHY_TX_MAC_EMU_BB_CPU_PE_DISAMBIGUITY_RESULT 0x28BC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbCpuPeDisambiguityResult : 1; //bb_cpu_pe_disambiguity_result    , reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyTxMacEmuBbCpuPeDisambiguityResult_u;

/*REG_PHY_TX_MAC_EMU_BB_CPU_A_FACTOR_RESULT 0x28C0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbCpuAFactorResult : 3; //bb_cpu_a_factor_result           , reset value: 0x0, access type: RO
		uint32 reserved0 : 29;
	} bitFields;
} RegPhyTxMacEmuBbCpuAFactorResult_u;

/*REG_PHY_TX_MAC_EMU_BB_CPU_A_FACTOR_INIT_RESULT 0x28C4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbCpuAFactorInitResult : 3; //bb_cpu_a_factor_init_result      , reset value: 0x0, access type: RO
		uint32 reserved0 : 29;
	} bitFields;
} RegPhyTxMacEmuBbCpuAFactorInitResult_u;

/*REG_PHY_TX_MAC_EMU_BB_CPU_AIR_TIME_CYCLES 0x28C8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbCpuAirTimeCycles : 26; //bb_cpu_air_time_cycles           , reset value: 0x0, access type: RO
		uint32 reserved0 : 6;
	} bitFields;
} RegPhyTxMacEmuBbCpuAirTimeCycles_u;

/*REG_PHY_TX_MAC_EMU_BB_CPU_PHY_RATE_RESULT 0x28CC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbCpuPhyRateResult : 16; //bb_cpu_phy_rate_result           , reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyTxMacEmuBbCpuPhyRateResult_u;

/*REG_PHY_TX_MAC_EMU_BB_CPU_AIR_TIME_ERROR 0x28D0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbCpuAirTimeError : 1; //bb_cpu_air_time_error            , reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyTxMacEmuBbCpuAirTimeError_u;

/*REG_PHY_TX_MAC_EMU_BB_CPU_N_CBPS 0x28D4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbCpuNCbps : 17; //bb_cpu_n_cbps                    , reset value: 0x0, access type: RO
		uint32 reserved0 : 15;
	} bitFields;
} RegPhyTxMacEmuBbCpuNCbps_u;

/*REG_PHY_TX_MAC_EMU_BB_CPU_N_DBPS 0x28D8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbCpuNDbps : 17; //bb_cpu_n_dbps                    , reset value: 0x0, access type: RO
		uint32 reserved0 : 15;
	} bitFields;
} RegPhyTxMacEmuBbCpuNDbps_u;

/*REG_PHY_TX_MAC_EMU_BB_CPU_N_SD_LAST 0x28DC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbCpuNSdLast : 11; //bb_cpu_n_sd_last                 , reset value: 0x0, access type: RO
		uint32 reserved0 : 21;
	} bitFields;
} RegPhyTxMacEmuBbCpuNSdLast_u;

/*REG_PHY_TX_MAC_EMU_BB_CPU_PREAMBLE_TIME 0x28E0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bbCpuPreambleTime : 18; //bb_cpu_preamble_time , reset value: 0x0, access type: RO
		uint32 reserved0 : 14;
	} bitFields;
} RegPhyTxMacEmuBbCpuPreambleTime_u;



#endif // _PHY_TX_MAC_EMU_REGS_H_
