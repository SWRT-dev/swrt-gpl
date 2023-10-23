/***********************************************************************************
File:				HwMemoryMap.h
Module:				Hw Headers - wave600_memory_map_ver0.29.0.xlsx
Purpose:
Description:		This File was auto generated using Excel Memory Map Parser
************************************************************************************/
#ifndef _MEMORY_MAP_H_ 
#define _MEMORY_MAP_H_ 

/* Unit Base Address Definitions*/
#define MEMORY_MAP_UNIT_1_BASE_ADDRESS                                         (SYSTEM_IF_UPPER_CPU_BASE_ADDR)
#define MEMORY_MAP_UNIT_2_BASE_ADDRESS                                         (SYSTEM_IF_LOWER_B0_CPU_BASE_ADDR)
#define MEMORY_MAP_UNIT_5_BASE_ADDRESS                                         (VAP_ADDR_REG_BASE_ADDR)
#define MEMORY_MAP_UNIT_6_BASE_ADDRESS                                         (B0_OTFA_REG_BASE_ADDR)
#define MEMORY_MAP_UNIT_7_BASE_ADDRESS                                         (B0_RX_COORDINATOR_BASE_ADDR)
#define MEMORY_MAP_UNIT_8_BASE_ADDRESS                                         (B0_PAC_DELIA_REG_BASE_ADDR)
#define MEMORY_MAP_UNIT_9_BASE_ADDRESS                                         (B0_PAC_RXC_BASE_ADDR)
#define MEMORY_MAP_UNIT_10_BASE_ADDRESS                                        (B0_PAC_DUR_BASE_ADDR)
#define MEMORY_MAP_UNIT_11_BASE_ADDRESS                                        (B0_MAC_BFM_BASE_ADDR)
#define MEMORY_MAP_UNIT_12_BASE_ADDRESS                                        (B0_BUF_STATUS_REPORT_BASE_ADDR)
#define MEMORY_MAP_UNIT_13_BASE_ADDRESS                                        (B0_PAC_TIM_BASE_ADDR)
#define MEMORY_MAP_UNIT_14_BASE_ADDRESS                                        (B0_MAC_BF_BASE_ADDR)
#define MEMORY_MAP_UNIT_15_BASE_ADDRESS                                        (B0_MAC_BEACON_BASE_ADDR)
#define MEMORY_MAP_UNIT_16_BASE_ADDRESS                                        (B0_BSRC_REGS_BASE_ADDR)
#define MEMORY_MAP_UNIT_17_BASE_ADDRESS                                        (B0_MAC_WEP_BASE_ADDR)
#define MEMORY_MAP_UNIT_18_BASE_ADDRESS                                        (B0_RAB_BASE_ADDR)
#define MEMORY_MAP_UNIT_19_BASE_ADDRESS                                        (B0_CLC_BASE_ADDR)
#define MEMORY_MAP_UNIT_20_BASE_ADDRESS                                        (B0_MAC_TX_HC_BASE_ADDR)
#define MEMORY_MAP_UNIT_21_BASE_ADDRESS                                        (B0_PRE_AGG_BASE_ADDR)
#define MEMORY_MAP_UNIT_22_BASE_ADDRESS                                        (B0_MAC_ADDR2INDEX_BASE_ADDR)
#define MEMORY_MAP_UNIT_23_BASE_ADDRESS                                        (B0_TX_HANDLER_BASE_ADDR)
#define MEMORY_MAP_UNIT_24_BASE_ADDRESS                                        (B0_MAC_HT_EXTENSIONS_BASE_ADDR)
#define MEMORY_MAP_UNIT_25_BASE_ADDRESS                                        (B0_AUTO_RESP_REGS_BASE_ADDR)
#define MEMORY_MAP_UNIT_26_BASE_ADDRESS                                        (B0_MAC_GENRISC_TX_REG_BASE_ADDR)
#define MEMORY_MAP_UNIT_27_BASE_ADDRESS                                        (B0_MAC_GENRISC_RX_REG_BASE_ADDR)
#define MEMORY_MAP_UNIT_28_BASE_ADDRESS                                        (MAC_GENRISC_HOST_REG_BASE_ADDR)
#define MEMORY_MAP_UNIT_29_BASE_ADDRESS                                        (B0_TX_SELECTOR_BASE_ADDR)
#define MEMORY_MAP_UNIT_32_BASE_ADDRESS                                        (QUEUE_MANAGER_LCPU1_BASE_ADDR)
#define MEMORY_MAP_UNIT_36_BASE_ADDRESS                                        (QUEUE_MANAGER_UCPU_BASE_ADDR)
#define MEMORY_MAP_UNIT_37_BASE_ADDRESS                                        (QUEUE_MANAGER_LCPU0_BASE_ADDR)
#define MEMORY_MAP_UNIT_38_BASE_ADDRESS                                        (ARM_DMA_WRAPPER_BASE_ADDR)
#define MEMORY_MAP_UNIT_39_BASE_ADDRESS                                        (HOST_IF_ACC_BASE_ADDR)
#define MEMORY_MAP_UNIT_40_BASE_ADDRESS                                        (DLM_BASE_ADDR)
#define MEMORY_MAP_UNIT_41_BASE_ADDRESS                                        (MAC_GEN_REGFILE_BASE_ADDR)
#define MEMORY_MAP_UNIT_42_BASE_ADDRESS                                        (B0_TX_PD_ACC_REG_BASE_ADDR)
#define MEMORY_MAP_UNIT_43_BASE_ADDRESS                                        (B0_UL_POST_RX_BASE_ADDR)
#define MEMORY_MAP_UNIT_44_BASE_ADDRESS                                        (B0_TF_GENERATOR_REG_BASE_ADDR)
#define MEMORY_MAP_UNIT_45_BASE_ADDRESS                                        (B0_BA_ANALYZER_BASE_ADDR)
#define MEMORY_MAP_UNIT_46_BASE_ADDRESS                                        (PROT_DB_BASE_ADDR)
#define MEMORY_MAP_UNIT_47_BASE_ADDRESS                                        (RX_PP_REGS_BASE_ADDR)
#define MEMORY_MAP_UNIT_48_BASE_ADDRESS                                        (WLAN_ARM_DMA0_BASE_ADDR)
#define MEMORY_MAP_UNIT_49_BASE_ADDRESS                                        (B0_PHY_RX_BE_BASE_ADDR)
#define MEMORY_MAP_UNIT_50_BASE_ADDRESS                                        (B0_PHY_RX_TD_BASE_ADDR)
#define MEMORY_MAP_UNIT_51_BASE_ADDRESS                                        (B0_PHY_RX_FD_BASE_ADDR)
#define MEMORY_MAP_UNIT_52_BASE_ADDRESS                                        (B0_PHY_TX_BASE_ADDR)
#define MEMORY_MAP_UNIT_53_BASE_ADDRESS                                        (B0_TXH_TXC_BASE_ADDR)
#define MEMORY_MAP_UNIT_54_BASE_ADDRESS                                        (B0_TXH_NTD_BASE_ADDR)
#define MEMORY_MAP_UNIT_55_BASE_ADDRESS                                        (WLAN_ARM_DMA1_BASE_ADDR)
#define MEMORY_MAP_UNIT_57_BASE_ADDRESS                                        (B0_RX_CLASSIFIER_BASE_ADDR)
#define MEMORY_MAP_UNIT_58_BASE_ADDRESS                                        (B0_PS_SETTING_BASE_ADDR)
#define MEMORY_MAP_UNIT_59_BASE_ADDRESS                                        (TOP_REGBLOCK_BASE_ADDR)
#define MEMORY_MAP_UNIT_60_BASE_ADDRESS                                        (PCIE_PHY0_REGBLOCK_BASE_ADDR)
#define MEMORY_MAP_UNIT_61_BASE_ADDRESS                                        (DATA_LOGGER_BASE_ADDR)
#define MEMORY_MAP_UNIT_62_BASE_ADDRESS                                        (B0_PAC_EXTRAPOLATOR_BASE_ADDR)
#define MEMORY_MAP_UNIT_63_BASE_ADDRESS                                        (HOST_IF_COUNTERS_BASE_ADDR)
#define MEMORY_MAP_UNIT_64_BASE_ADDRESS                                        (RX_PP_COUNTERS_BASE_ADDR)
#define MEMORY_MAP_UNIT_65_BASE_ADDRESS                                        (B0_RX_MSDU_PARSER_BASE_ADDR)
#define MEMORY_MAP_UNIT_67_BASE_ADDRESS                                        (BSRC_CNT_MEM_BASE_ADDR)
#define MEMORY_MAP_UNIT_68_BASE_ADDRESS                                        (B0_TX_SEQUENCER_BASE_ADDR)
#define MEMORY_MAP_UNIT_69_BASE_ADDRESS                                        (B0_RF_EMULATOR_BASE_ADDR)
#define MEMORY_MAP_UNIT_70_BASE_ADDRESS                                        (B0_PHY_AGC_ANT0_BASE_ADDR)
#define MEMORY_MAP_UNIT_71_BASE_ADDRESS                                        (B0_PHY_AGC_ANT1_BASE_ADDR)
#define MEMORY_MAP_UNIT_72_BASE_ADDRESS                                        (B0_PHY_AGC_ANT2_BASE_ADDR)
#define MEMORY_MAP_UNIT_73_BASE_ADDRESS                                        (B0_PHY_AGC_ANT3_BASE_ADDR)
#define MEMORY_MAP_UNIT_80_BASE_ADDRESS                                        (B0_PHY_RXTD_ANT0_BASE_ADDR)
#define MEMORY_MAP_UNIT_81_BASE_ADDRESS                                        (B0_PHY_RXTD_ANT1_BASE_ADDR)
#define MEMORY_MAP_UNIT_82_BASE_ADDRESS                                        (B0_PHY_RXTD_ANT2_BASE_ADDR)
#define MEMORY_MAP_UNIT_83_BASE_ADDRESS                                        (B0_PHY_RXTD_ANT3_BASE_ADDR)
#define MEMORY_MAP_UNIT_90_BASE_ADDRESS                                        (B0_PHY_TXTD_ANT0_BASE_ADDR)
#define MEMORY_MAP_UNIT_91_BASE_ADDRESS                                        (B0_PHY_TXTD_ANT1_BASE_ADDR)
#define MEMORY_MAP_UNIT_92_BASE_ADDRESS                                        (B0_PHY_TXTD_ANT2_BASE_ADDR)
#define MEMORY_MAP_UNIT_93_BASE_ADDRESS                                        (B0_PHY_TXTD_ANT3_BASE_ADDR)
#define MEMORY_MAP_UNIT_94_BASE_ADDRESS                                        (PCIE_DBI0_REGBLOCK_BASE_ADDR)
#define MEMORY_MAP_UNIT_1000_BASE_ADDRESS                                      (CPU_IRAM_BASE_ADDR)
#define MEMORY_MAP_UNIT_1002_BASE_ADDRESS                                      (SYSTEM_IF_LOWER_B1_CPU_BASE_ADDR)
#define MEMORY_MAP_UNIT_1004_BASE_ADDRESS                                      (MAC_GENRISC_HOST_IRAM_BASE_ADDR)
#define MEMORY_MAP_UNIT_1006_BASE_ADDRESS                                      (MAC_GENRISC_HOST_SPRAM_BASE_ADDR)
#define MEMORY_MAP_UNIT_1008_BASE_ADDRESS                                      (WLAN_ARM_LL_MEM_BASE_ADDR)
#define MEMORY_MAP_UNIT_1015_BASE_ADDRESS                                      (PROT_DB_SHADOW_MEM_BASE_ADDR)
#define MEMORY_MAP_UNIT_1017_BASE_ADDRESS                                      (BSRC_AGER_MEM_BASE_ADDR)
#define MEMORY_MAP_UNIT_1018_BASE_ADDRESS                                      (SHARED_RAM_BASE_ADDR)
#define MEMORY_MAP_UNIT_1019_BASE_ADDRESS                                      (DESCRIPTOR_RAM_BASE_ADDR)
#define MEMORY_MAP_UNIT_1020_BASE_ADDRESS                                      (B0_RX_CIRCULAR_BASE_ADDR)
#define MEMORY_MAP_UNIT_1021_BASE_ADDRESS                                      (B0_RX_PHY_STATUS_BASE_ADDR)
#define MEMORY_MAP_UNIT_1022_BASE_ADDRESS                                      (B0_TX_CIRCULAR_BASE_ADDR)
#define MEMORY_MAP_UNIT_1023_BASE_ADDRESS                                      (B0_TX_RECIPES_BASE_ADDR)
#define MEMORY_MAP_UNIT_1024_BASE_ADDRESS                                      (B0_MAC_GENRISC_TX_IRAM_BASE_ADDR)
#define MEMORY_MAP_UNIT_1026_BASE_ADDRESS                                      (B0_MAC_GENRISC_TX_SPRAM_BASE_ADDR)
#define MEMORY_MAP_UNIT_1029_BASE_ADDRESS                                      (B0_MAC_GENRISC_TX_STD_BASE_ADDR)
#define MEMORY_MAP_UNIT_1031_BASE_ADDRESS                                      (B0_MAC_GENRISC_RX_IRAM_BASE_ADDR)
#define MEMORY_MAP_UNIT_1033_BASE_ADDRESS                                      (B0_MAC_GENRISC_RX_SPRAM_BASE_ADDR)
#define MEMORY_MAP_UNIT_1036_BASE_ADDRESS                                      (B0_MAC_TRAINER_BASE_ADDR)
#define MEMORY_MAP_UNIT_1049_BASE_ADDRESS                                      (BA_ANALYZER_COUNTER_BASE_ADDR)
#define MEMORY_MAP_UNIT_1052_BASE_ADDRESS                                      (B0_RXF_MEM_BASE_ADDR)
#define MEMORY_MAP_UNIT_1053_BASE_ADDRESS                                      (B0_AUTO_RESP_MEMS_BASE_ADDR)
#define MEMORY_MAP_UNIT_1054_BASE_ADDRESS                                      (TRAFFIC_INDICATOR_BASE_ADDR)
#define MEMORY_MAP_UNIT_1055_BASE_ADDRESS                                      (B0_DUR_AUTO_RATE_MEM_BASE_ADDR)
#define MEMORY_MAP_UNIT_1056_BASE_ADDRESS                                      (RETRY_STATE_DB_BASE_ADDR)
#define MEMORY_MAP_UNIT_1058_BASE_ADDRESS                                      (B0_FRAME_CLASS_VIO_BUFFER_BASE_ADDR)
#define MEMORY_MAP_UNIT_1060_BASE_ADDRESS                                      (BAND1_INTERNAL_BASE_ADDR)
#define MEMORY_MAP_UNIT_1061_BASE_ADDRESS                                      (BAND1_RX_BASE_ADDR)
#define MEMORY_MAP_UNIT_1062_BASE_ADDRESS                                      (BAND1_TX_BASE_ADDR)
#define MEMORY_MAP_UNIT_1064_BASE_ADDRESS                                      (HOST_EX_MAP_CPU_BASE_ADDR)
#define MEMORY_MAP_UNIT_1068_BASE_ADDRESS                                      (RETRY_POINTER_DB_BASE_ADDR)
#define MEMORY_MAP_UNIT_1069_BASE_ADDRESS                                      (B0_PAC_DELIA_MEM_BASE_ADDR)
#define MEMORY_MAP_UNIT_1073_BASE_ADDRESS                                      (B0_OTFA_MEM_BASE_ADDR)
#define MEMORY_MAP_UNIT_1077_BASE_ADDRESS                                      (B0_MAC_SEC_MEM_BASE_ADDR)
#define MEMORY_MAP_UNIT_1084_BASE_ADDRESS                                      (RX_PP_BASE_ADDR)
#define MEMORY_MAP_UNIT_1085_BASE_ADDRESS                                      (B0_TF_GENERATOR_MEM_BASE_ADDR)
#define MEMORY_MAP_UNIT_1088_BASE_ADDRESS                                      (B0_MPDU_DESC_MEM_BASE_ADDR)
#define MEMORY_MAP_UNIT_1096_BASE_ADDRESS                                      (B0_PHY_TEST_BUS_BASE_ADDR)
#define MEMORY_MAP_UNIT_1097_BASE_ADDRESS                                      (B0_TX_PD_ACC_MEM_BASE_ADDR)
#define MEMORY_MAP_UNIT_1103_BASE_ADDRESS                                      (B0_DUMMY_PHY_BASE_ADDR)
#define MEMORY_MAP_UNIT_10006_BASE_ADDRESS                                     (B1_OTFA_REG_BASE_ADDR)
#define MEMORY_MAP_UNIT_10007_BASE_ADDRESS                                     (B1_RX_COORDINATOR_BASE_ADDR)
#define MEMORY_MAP_UNIT_10008_BASE_ADDRESS                                     (B1_PAC_DELIA_REG_BASE_ADDR)
#define MEMORY_MAP_UNIT_10009_BASE_ADDRESS                                     (B1_PAC_RXC_BASE_ADDR)
#define MEMORY_MAP_UNIT_10010_BASE_ADDRESS                                     (B1_PAC_DUR_BASE_ADDR)
#define MEMORY_MAP_UNIT_10011_BASE_ADDRESS                                     (B1_MAC_BFM_BASE_ADDR)
#define MEMORY_MAP_UNIT_10012_BASE_ADDRESS                                     (B1_BUF_STATUS_REPORT_BASE_ADDR)
#define MEMORY_MAP_UNIT_10013_BASE_ADDRESS                                     (B1_PAC_TIM_BASE_ADDR)
#define MEMORY_MAP_UNIT_10014_BASE_ADDRESS                                     (B1_MAC_BF_BASE_ADDR)
#define MEMORY_MAP_UNIT_10015_BASE_ADDRESS                                     (B1_MAC_BEACON_BASE_ADDR)
#define MEMORY_MAP_UNIT_10016_BASE_ADDRESS                                     (B1_BSRC_REGS_BASE_ADDR)
#define MEMORY_MAP_UNIT_10017_BASE_ADDRESS                                     (B1_MAC_WEP_BASE_ADDR)
#define MEMORY_MAP_UNIT_10018_BASE_ADDRESS                                     (B1_RAB_BASE_ADDR)
#define MEMORY_MAP_UNIT_10019_BASE_ADDRESS                                     (B1_CLC_BASE_ADDR)
#define MEMORY_MAP_UNIT_10020_BASE_ADDRESS                                     (B1_MAC_TX_HC_BASE_ADDR)
#define MEMORY_MAP_UNIT_10021_BASE_ADDRESS                                     (B1_PRE_AGG_BASE_ADDR)
#define MEMORY_MAP_UNIT_10022_BASE_ADDRESS                                     (B1_MAC_ADDR2INDEX_BASE_ADDR)
#define MEMORY_MAP_UNIT_10023_BASE_ADDRESS                                     (B1_TX_HANDLER_BASE_ADDR)
#define MEMORY_MAP_UNIT_10024_BASE_ADDRESS                                     (B1_MAC_HT_EXTENSIONS_BASE_ADDR)
#define MEMORY_MAP_UNIT_10025_BASE_ADDRESS                                     (B1_AUTO_RESP_REGS_BASE_ADDR)
#define MEMORY_MAP_UNIT_10026_BASE_ADDRESS                                     (B1_MAC_GENRISC_TX_REG_BASE_ADDR)
#define MEMORY_MAP_UNIT_10027_BASE_ADDRESS                                     (B1_MAC_GENRISC_RX_REG_BASE_ADDR)
#define MEMORY_MAP_UNIT_10029_BASE_ADDRESS                                     (B1_TX_SELECTOR_BASE_ADDR)
#define MEMORY_MAP_UNIT_10042_BASE_ADDRESS                                     (B1_TX_PD_ACC_REG_BASE_ADDR)
#define MEMORY_MAP_UNIT_10043_BASE_ADDRESS                                     (B1_UL_POST_RX_BASE_ADDR)
#define MEMORY_MAP_UNIT_10044_BASE_ADDRESS                                     (B1_TF_GENERATOR_REG_BASE_ADDR)
#define MEMORY_MAP_UNIT_10045_BASE_ADDRESS                                     (B1_BA_ANALYZER_BASE_ADDR)
#define MEMORY_MAP_UNIT_10049_BASE_ADDRESS                                     (B1_PHY_RX_BE_BASE_ADDR)
#define MEMORY_MAP_UNIT_10050_BASE_ADDRESS                                     (B1_PHY_RX_TD_BASE_ADDR)
#define MEMORY_MAP_UNIT_10051_BASE_ADDRESS                                     (B1_PHY_RX_FD_BASE_ADDR)
#define MEMORY_MAP_UNIT_10052_BASE_ADDRESS                                     (B1_PHY_TX_BASE_ADDR)
#define MEMORY_MAP_UNIT_10053_BASE_ADDRESS                                     (B1_TXH_TXC_BASE_ADDR)
#define MEMORY_MAP_UNIT_10054_BASE_ADDRESS                                     (B1_TXH_NTD_BASE_ADDR)
#define MEMORY_MAP_UNIT_10057_BASE_ADDRESS                                     (B1_RX_CLASSIFIER_BASE_ADDR)
#define MEMORY_MAP_UNIT_10058_BASE_ADDRESS                                     (B1_PS_SETTING_BASE_ADDR)
#define MEMORY_MAP_UNIT_10062_BASE_ADDRESS                                     (B1_PAC_EXTRAPOLATOR_BASE_ADDR)
#define MEMORY_MAP_UNIT_10065_BASE_ADDRESS                                     (B1_RX_MSDU_PARSER_BASE_ADDR)
#define MEMORY_MAP_UNIT_10068_BASE_ADDRESS                                     (B1_TX_SEQUENCER_BASE_ADDR)
#define MEMORY_MAP_UNIT_10069_BASE_ADDRESS                                     (B1_RF_EMULATOR_BASE_ADDR)
#define MEMORY_MAP_UNIT_10070_BASE_ADDRESS                                     (B1_PHY_AGC_ANT0_BASE_ADDR)
#define MEMORY_MAP_UNIT_10071_BASE_ADDRESS                                     (B1_PHY_AGC_ANT1_BASE_ADDR)
#define MEMORY_MAP_UNIT_10072_BASE_ADDRESS                                     (B1_PHY_AGC_ANT2_BASE_ADDR)
#define MEMORY_MAP_UNIT_10073_BASE_ADDRESS                                     (B1_PHY_AGC_ANT3_BASE_ADDR)
#define MEMORY_MAP_UNIT_10080_BASE_ADDRESS                                     (B1_PHY_RXTD_ANT0_BASE_ADDR)
#define MEMORY_MAP_UNIT_10081_BASE_ADDRESS                                     (B1_PHY_RXTD_ANT1_BASE_ADDR)
#define MEMORY_MAP_UNIT_10082_BASE_ADDRESS                                     (B1_PHY_RXTD_ANT2_BASE_ADDR)
#define MEMORY_MAP_UNIT_10083_BASE_ADDRESS                                     (B1_PHY_RXTD_ANT3_BASE_ADDR)
#define MEMORY_MAP_UNIT_10090_BASE_ADDRESS                                     (B1_PHY_TXTD_ANT0_BASE_ADDR)
#define MEMORY_MAP_UNIT_10091_BASE_ADDRESS                                     (B1_PHY_TXTD_ANT1_BASE_ADDR)
#define MEMORY_MAP_UNIT_10092_BASE_ADDRESS                                     (B1_PHY_TXTD_ANT2_BASE_ADDR)
#define MEMORY_MAP_UNIT_10093_BASE_ADDRESS                                     (B1_PHY_TXTD_ANT3_BASE_ADDR)
#define MEMORY_MAP_UNIT_11020_BASE_ADDRESS                                     (B1_RX_CIRCULAR_BASE_ADDR)
#define MEMORY_MAP_UNIT_11021_BASE_ADDRESS                                     (B1_RX_PHY_STATUS_BASE_ADDR)
#define MEMORY_MAP_UNIT_11022_BASE_ADDRESS                                     (B1_TX_CIRCULAR_BASE_ADDR)
#define MEMORY_MAP_UNIT_11023_BASE_ADDRESS                                     (B1_TX_RECIPES_BASE_ADDR)
#define MEMORY_MAP_UNIT_11024_BASE_ADDRESS                                     (B1_MAC_GENRISC_TX_IRAM_BASE_ADDR)
#define MEMORY_MAP_UNIT_11026_BASE_ADDRESS                                     (B1_MAC_GENRISC_TX_SPRAM_BASE_ADDR)
#define MEMORY_MAP_UNIT_11029_BASE_ADDRESS                                     (B1_MAC_GENRISC_TX_STD_BASE_ADDR)
#define MEMORY_MAP_UNIT_11031_BASE_ADDRESS                                     (B1_MAC_GENRISC_RX_IRAM_BASE_ADDR)
#define MEMORY_MAP_UNIT_11033_BASE_ADDRESS                                     (B1_MAC_GENRISC_RX_SPRAM_BASE_ADDR)
#define MEMORY_MAP_UNIT_11036_BASE_ADDRESS                                     (B1_MAC_TRAINER_BASE_ADDR)
#define MEMORY_MAP_UNIT_11052_BASE_ADDRESS                                     (B1_RXF_MEM_BASE_ADDR)
#define MEMORY_MAP_UNIT_11053_BASE_ADDRESS                                     (B1_AUTO_RESP_MEMS_BASE_ADDR)
#define MEMORY_MAP_UNIT_11055_BASE_ADDRESS                                     (B1_DUR_AUTO_RATE_MEM_BASE_ADDR)
#define MEMORY_MAP_UNIT_11058_BASE_ADDRESS                                     (B1_FRAME_CLASS_VIO_BUFFER_BASE_ADDR)
#define MEMORY_MAP_UNIT_11069_BASE_ADDRESS                                     (B1_PAC_DELIA_MEM_BASE_ADDR)
#define MEMORY_MAP_UNIT_11073_BASE_ADDRESS                                     (B1_OTFA_MEM_BASE_ADDR)
#define MEMORY_MAP_UNIT_11077_BASE_ADDRESS                                     (B1_MAC_SEC_MEM_BASE_ADDR)
#define MEMORY_MAP_UNIT_11085_BASE_ADDRESS                                     (B1_TF_GENERATOR_MEM_BASE_ADDR)
#define MEMORY_MAP_UNIT_11088_BASE_ADDRESS                                     (B1_MPDU_DESC_MEM_BASE_ADDR)
#define MEMORY_MAP_UNIT_11096_BASE_ADDRESS                                     (B1_PHY_TEST_BUS_BASE_ADDR)
#define MEMORY_MAP_UNIT_11097_BASE_ADDRESS                                     (B1_TX_PD_ACC_MEM_BASE_ADDR)
#define MEMORY_MAP_UNIT_11103_BASE_ADDRESS                                     (B1_DUMMY_PHY_BASE_ADDR)
#define MEMORY_MAP_UNIT_9900000_BASE_ADDRESS                                   (0x0)

/* Base Addresses From ARC Persepective*/
#define CPU_IRAM_BASE_ADDR                                                     (0xA0000000)
#define SYSTEM_IF_LOWER_B0_CPU_BASE_ADDR                                       (0xA0200000)
#define SYSTEM_IF_LOWER_B1_CPU_BASE_ADDR                                       (0xA0210000)
#define SYSTEM_IF_UPPER_CPU_BASE_ADDR                                          (0xA0220000)
#define MAC_GENRISC_HOST_IRAM_BASE_ADDR                                        (0xA0230000)
#define MAC_GENRISC_HOST_REG_BASE_ADDR                                         (0xA0235000)
#define MAC_GENRISC_HOST_SPRAM_BASE_ADDR                                       (0xA0235400)
#define MAC_GEN_REGFILE_BASE_ADDR                                              (0xA0238000)
#define WLAN_ARM_LL_MEM_BASE_ADDR                                              (0xA0240000)
#define WLAN_ARM_DMA0_BASE_ADDR                                                (0xA0245000)
#define WLAN_ARM_DMA1_BASE_ADDR                                                (0xA0246000)
#define ARM_DMA_WRAPPER_BASE_ADDR                                              (0xA0247000)
#define HOST_IF_COUNTERS_BASE_ADDR                                             (0xA0250000)
#define HOST_IF_ACC_BASE_ADDR                                                  (0xA0255000)
#define DLM_BASE_ADDR                                                          (0xA0260000)
#define QUEUE_MANAGER_LCPU0_BASE_ADDR                                          (0xA026C000)
#define QUEUE_MANAGER_LCPU1_BASE_ADDR                                          (0xA026C000)
#define QUEUE_MANAGER_UCPU_BASE_ADDR                                           (0xA026C000)
#define TOP_REGBLOCK_BASE_ADDR                                                 (0xA0270000)
#define PCIE_PHY0_REGBLOCK_BASE_ADDR                                           (0xA0278000)
#define PCIE_DBI0_REGBLOCK_BASE_ADDR                                           (0xA027B000)
#define PROT_DB_BASE_ADDR                                                      (0xA0290000)
#define PROT_DB_SHADOW_MEM_BASE_ADDR                                           (0xA0290200)
#define TRAFFIC_INDICATOR_BASE_ADDR                                            (0xA0291800)
#define VAP_ADDR_REG_BASE_ADDR                                                 (0xA0292000)
#define BA_ANALYZER_COUNTER_BASE_ADDR                                          (0xA0294000)
#define BSRC_CNT_MEM_BASE_ADDR                                                 (0xA0298000)
#define BSRC_AGER_MEM_BASE_ADDR                                                (0xA029A000)
#define DATA_LOGGER_BASE_ADDR                                                  (0xA02A0000)
#define RX_PP_BASE_ADDR                                                        (0xA02B0000)
#define RX_PP_REGS_BASE_ADDR                                                   (0xA02B9800)
#define RX_PP_COUNTERS_BASE_ADDR                                               (0xA02BA000)
#define RETRY_STATE_DB_BASE_ADDR                                               (0xA02BC000)
#define RETRY_POINTER_DB_BASE_ADDR                                             (0xA02C0000)
#define SHARED_RAM_BASE_ADDR                                                   (0xA0300000)
#define DESCRIPTOR_RAM_BASE_ADDR                                               (0xA0400000)
#define B0_MAC_GENRISC_TX_IRAM_BASE_ADDR                                       (0xA0800000)
#define B1_MAC_GENRISC_TX_IRAM_BASE_ADDR                                       (0xA0C00000)
#define B0_MAC_GENRISC_TX_SPRAM_BASE_ADDR                                      (0xA0810000)
#define B1_MAC_GENRISC_TX_SPRAM_BASE_ADDR                                      (0xA0C10000)
#define B0_MAC_GENRISC_TX_REG_BASE_ADDR                                        (0xA0811000)
#define B1_MAC_GENRISC_TX_REG_BASE_ADDR                                        (0xA0C11000)
#define B0_MAC_GENRISC_TX_STD_BASE_ADDR                                        (0xA0812000)
#define B1_MAC_GENRISC_TX_STD_BASE_ADDR                                        (0xA0C12000)
#define B0_MAC_GENRISC_RX_IRAM_BASE_ADDR                                       (0xA0820000)
#define B1_MAC_GENRISC_RX_IRAM_BASE_ADDR                                       (0xA0C20000)
#define B0_MAC_GENRISC_RX_SPRAM_BASE_ADDR                                      (0xA082F000)
#define B1_MAC_GENRISC_RX_SPRAM_BASE_ADDR                                      (0xA0C2F000)
#define B0_MAC_GENRISC_RX_REG_BASE_ADDR                                        (0xA0830000)
#define B1_MAC_GENRISC_RX_REG_BASE_ADDR                                        (0xA0C30000)
#define B0_MAC_HT_EXTENSIONS_BASE_ADDR                                         (0xA0831000)
#define B1_MAC_HT_EXTENSIONS_BASE_ADDR                                         (0xA0C31000)
#define B0_MAC_TRAINER_BASE_ADDR                                               (0xA0834000)
#define B1_MAC_TRAINER_BASE_ADDR                                               (0xA0C34000)
#define B0_AUTO_RESP_MEMS_BASE_ADDR                                            (0xA0838000)
#define B1_AUTO_RESP_MEMS_BASE_ADDR                                            (0xA0C38000)
#define B0_AUTO_RESP_REGS_BASE_ADDR                                            (0xA083B000)
#define B1_AUTO_RESP_REGS_BASE_ADDR                                            (0xA0C3B000)
#define B0_PAC_DUR_BASE_ADDR                                                   (0xA0900400)
#define B1_PAC_DUR_BASE_ADDR                                                   (0xA0D00400)
#define B0_PAC_EXTRAPOLATOR_BASE_ADDR                                          (0xA0900600)
#define B1_PAC_EXTRAPOLATOR_BASE_ADDR                                          (0xA0D00600)
#define B0_PAC_TIM_BASE_ADDR                                                   (0xA0900700)
#define B1_PAC_TIM_BASE_ADDR                                                   (0xA0D00700)
#define B0_MAC_BF_BASE_ADDR                                                    (0xA0900800)
#define B1_MAC_BF_BASE_ADDR                                                    (0xA0D00800)
#define B0_MAC_BEACON_BASE_ADDR                                                (0xA0900A00)
#define B1_MAC_BEACON_BASE_ADDR                                                (0xA0D00A00)
#define B0_TXH_TXC_BASE_ADDR                                                   (0xA0900E00)
#define B1_TXH_TXC_BASE_ADDR                                                   (0xA0D00E00)
#define B0_TXH_NTD_BASE_ADDR                                                   (0xA0900F00)
#define B1_TXH_NTD_BASE_ADDR                                                   (0xA0D00F00)
#define B0_PS_SETTING_BASE_ADDR                                                (0xA0901000)
#define B1_PS_SETTING_BASE_ADDR                                                (0xA0D01000)
#define B0_PAC_DELIA_REG_BASE_ADDR                                             (0xA0901C00)
#define B1_PAC_DELIA_REG_BASE_ADDR                                             (0xA0D01C00)
#define B0_PAC_DELIA_MEM_BASE_ADDR                                             (0xA0901E00)
#define B1_PAC_DELIA_MEM_BASE_ADDR                                             (0xA0D01E00)
#define B0_DUR_AUTO_RATE_MEM_BASE_ADDR                                         (0xA0903800)
#define B1_DUR_AUTO_RATE_MEM_BASE_ADDR                                         (0xA0D03800)
#define B0_PAC_RXC_BASE_ADDR                                                   (0xA0904000)
#define B1_PAC_RXC_BASE_ADDR                                                   (0xA0D04000)
#define B0_RXF_MEM_BASE_ADDR                                                   (0xA0904400)
#define B1_RXF_MEM_BASE_ADDR                                                   (0xA0D04400)
#define B0_MAC_SEC_MEM_BASE_ADDR                                               (0xA0910000)
#define B1_MAC_SEC_MEM_BASE_ADDR                                               (0xA0D10000)
#define B0_MAC_WEP_BASE_ADDR                                                   (0xA0918000)
#define B1_MAC_WEP_BASE_ADDR                                                   (0xA0D18000)
#define B0_RAB_BASE_ADDR                                                       (0xA0920000)
#define B1_RAB_BASE_ADDR                                                       (0xA0D20000)
#define B0_TX_SEQUENCER_BASE_ADDR                                              (0xA092E000)
#define B1_TX_SEQUENCER_BASE_ADDR                                              (0xA0D2E000)
#define B0_TX_PD_ACC_MEM_BASE_ADDR                                             (0xA092E800)
#define B1_TX_PD_ACC_MEM_BASE_ADDR                                             (0xA0D2E800)
#define B0_TX_PD_ACC_REG_BASE_ADDR                                             (0xA092EE00)
#define B1_TX_PD_ACC_REG_BASE_ADDR                                             (0xA0D2EE00)
#define B0_TF_GENERATOR_REG_BASE_ADDR                                          (0xA092F000)
#define B1_TF_GENERATOR_REG_BASE_ADDR                                          (0xA0D2F000)
#define B0_TF_GENERATOR_MEM_BASE_ADDR                                          (0xA092F200)
#define B1_TF_GENERATOR_MEM_BASE_ADDR                                          (0xA0D2F200)
#define B0_CLC_BASE_ADDR                                                       (0xA0930000)
#define B1_CLC_BASE_ADDR                                                       (0xA0D30000)
#define B0_PRE_AGG_BASE_ADDR                                                   (0xA0932000)
#define B1_PRE_AGG_BASE_ADDR                                                   (0xA0D32000)
#define B0_MAC_ADDR2INDEX_BASE_ADDR                                            (0xA0933000)
#define B1_MAC_ADDR2INDEX_BASE_ADDR                                            (0xA0D33000)
#define B0_TX_HANDLER_BASE_ADDR                                                (0xA0934000)
#define B1_TX_HANDLER_BASE_ADDR                                                (0xA0D34000)
#define B0_BA_ANALYZER_BASE_ADDR                                               (0xA0935000)
#define B1_BA_ANALYZER_BASE_ADDR                                               (0xA0D35000)
#define B0_TX_SELECTOR_BASE_ADDR                                               (0xA0938000)
#define B1_TX_SELECTOR_BASE_ADDR                                               (0xA0D38000)
#define B0_RX_CLASSIFIER_BASE_ADDR                                             (0xA093B000)
#define B1_RX_CLASSIFIER_BASE_ADDR                                             (0xA0D3B000)
#define B0_RX_MSDU_PARSER_BASE_ADDR                                            (0xA093E000)
#define B1_RX_MSDU_PARSER_BASE_ADDR                                            (0xA0D3E000)
#define B0_RX_COORDINATOR_BASE_ADDR                                            (0xA0941000)
#define B1_RX_COORDINATOR_BASE_ADDR                                            (0xA0D41000)
#define B0_FRAME_CLASS_VIO_BUFFER_BASE_ADDR                                    (0xA0941200)
#define B1_FRAME_CLASS_VIO_BUFFER_BASE_ADDR                                    (0xA0D41200)
#define B0_OTFA_REG_BASE_ADDR                                                  (0xA0942000)
#define B1_OTFA_REG_BASE_ADDR                                                  (0xA0D42000)
#define B0_OTFA_MEM_BASE_ADDR                                                  (0xA0942200)
#define B1_OTFA_MEM_BASE_ADDR                                                  (0xA0D42200)
#define B0_MAC_TX_HC_BASE_ADDR                                                 (0xA0947800)
#define B1_MAC_TX_HC_BASE_ADDR                                                 (0xA0D47800)
#define B0_BSRC_REGS_BASE_ADDR                                                 (0xA0947C00)
#define B1_BSRC_REGS_BASE_ADDR                                                 (0xA0D47C00)
#define B0_MAC_BFM_BASE_ADDR                                                   (0xA0948000)
#define B1_MAC_BFM_BASE_ADDR                                                   (0xA0D48000)
#define B0_UL_POST_RX_BASE_ADDR                                                (0xA094A000)
#define B1_UL_POST_RX_BASE_ADDR                                                (0xA0D4A000)
#define B0_BUF_STATUS_REPORT_BASE_ADDR                                         (0xA0950000)
#define B1_BUF_STATUS_REPORT_BASE_ADDR                                         (0xA0D50000)
#define B0_DUMMY_PHY_BASE_ADDR                                                 (0xA0AF0000)
#define B1_DUMMY_PHY_BASE_ADDR                                                 (0xA0EF0000)
#define B0_PHY_RX_BE_BASE_ADDR                                                 (0xA0B00000)
#define B1_PHY_RX_BE_BASE_ADDR                                                 (0xA0F00000)
#define B0_PHY_TEST_BUS_BASE_ADDR                                              (0xA0B20000)
#define B1_PHY_TEST_BUS_BASE_ADDR                                              (0xA0F20000)
#define B0_PHY_RX_TD_BASE_ADDR                                                 (0xA0B40000)
#define B1_PHY_RX_TD_BASE_ADDR                                                 (0xA0F40000)
#define B0_PHY_RX_FD_BASE_ADDR                                                 (0xA0B80000)
#define B1_PHY_RX_FD_BASE_ADDR                                                 (0xA0F80000)
#define B0_PHY_TX_BASE_ADDR                                                    (0xA0BC0000)
#define B1_PHY_TX_BASE_ADDR                                                    (0xA0FC0000)
#define BAND1_INTERNAL_BASE_ADDR                                               (0xA0C00000)
#define B0_RX_CIRCULAR_BASE_ADDR                                               (0xA1000000)
#define B1_RX_CIRCULAR_BASE_ADDR                                               (0xA1400000)
#define B0_RX_PHY_STATUS_BASE_ADDR                                             (0xA1378000)
#define B1_RX_PHY_STATUS_BASE_ADDR                                             (0xA1778000)
#define B0_MPDU_DESC_MEM_BASE_ADDR                                             (0xA1380000)
#define B1_MPDU_DESC_MEM_BASE_ADDR                                             (0xA1780000)
#define BAND1_RX_BASE_ADDR                                                     (0xA1400000)
#define B0_TX_CIRCULAR_BASE_ADDR                                               (0xA1800000)
#define B1_TX_CIRCULAR_BASE_ADDR                                               (0xA1C00000)
#define B0_TX_RECIPES_BASE_ADDR                                                (0xA1BF8000)
#define B1_TX_RECIPES_BASE_ADDR                                                (0xA1FF8000)
#define BAND1_TX_BASE_ADDR                                                     (0xA1C00000)
#define HOST_EX_MAP_CPU_BASE_ADDR                                              (0xA4000000)
#define B0_PHY_TXTD_ANT0_BASE_ADDR                                             (0xA0BE0000)
#define B1_PHY_TXTD_ANT0_BASE_ADDR                                             (0xA0FE0000)
#define B0_PHY_TXTD_ANT1_BASE_ADDR                                             (0xA0BE4000)
#define B1_PHY_TXTD_ANT1_BASE_ADDR                                             (0xA0FE4000)
#define B0_PHY_TXTD_ANT2_BASE_ADDR                                             (0xA0BE8000)
#define B1_PHY_TXTD_ANT2_BASE_ADDR                                             (0xA0FE8000)
#define B0_PHY_TXTD_ANT3_BASE_ADDR                                             (0xA0BEC000)
#define B1_PHY_TXTD_ANT3_BASE_ADDR                                             (0xA0FEC000)
#define B0_PHY_RXTD_ANT0_BASE_ADDR                                             (0xA0B60000)
#define B1_PHY_RXTD_ANT0_BASE_ADDR                                             (0xA0F60000)
#define B0_PHY_RXTD_ANT1_BASE_ADDR                                             (0xA0B64000)
#define B1_PHY_RXTD_ANT1_BASE_ADDR                                             (0xA0F64000)
#define B0_PHY_RXTD_ANT2_BASE_ADDR                                             (0xA0B68000)
#define B1_PHY_RXTD_ANT2_BASE_ADDR                                             (0xA0F68000)
#define B0_PHY_RXTD_ANT3_BASE_ADDR                                             (0xA0B6C000)
#define B1_PHY_RXTD_ANT3_BASE_ADDR                                             (0xA0F6C000)
#define B0_PHY_AGC_ANT0_BASE_ADDR                                              (0xA0B63000)
#define B1_PHY_AGC_ANT0_BASE_ADDR                                              (0xA0F63000)
#define B0_PHY_AGC_ANT1_BASE_ADDR                                              (0xA0B67000)
#define B1_PHY_AGC_ANT1_BASE_ADDR                                              (0xA0F67000)
#define B0_PHY_AGC_ANT2_BASE_ADDR                                              (0xA0B6B000)
#define B1_PHY_AGC_ANT2_BASE_ADDR                                              (0xA0F6B000)
#define B0_PHY_AGC_ANT3_BASE_ADDR                                              (0xA0B6F000)
#define B1_PHY_AGC_ANT3_BASE_ADDR                                              (0xA0F6F000)
#define B0_RF_EMULATOR_BASE_ADDR                                               (0xA0274000)
#define B1_RF_EMULATOR_BASE_ADDR                                               (0xA0674000)

 /* Base Addresses From ARM Dma Persepective*/
#define CPU_IRAM_OFFSET_FROM_BAR0                                              (0x00000000)
#define SYSTEM_IF_LOWER_B0_CPU_OFFSET_FROM_BAR0                                (0x00200000)
#define SYSTEM_IF_LOWER_B1_CPU_OFFSET_FROM_BAR0                                (0x00210000)
#define SYSTEM_IF_UPPER_CPU_OFFSET_FROM_BAR0                                   (0x00220000)
#define MAC_GENRISC_HOST_IRAM_OFFSET_FROM_BAR0                                 (0x00230000)
#define MAC_GENRISC_HOST_REG_OFFSET_FROM_BAR0                                  (0x00235000)
#define MAC_GENRISC_HOST_SPRAM_OFFSET_FROM_BAR0                                (0x00235400)
#define MAC_GEN_REGFILE_OFFSET_FROM_BAR0                                       (0x00238000)
#define WLAN_ARM_LL_MEM_OFFSET_FROM_BAR0                                       (0x00240000)
#define WLAN_ARM_DMA0_OFFSET_FROM_BAR0                                         (0x00245000)
#define WLAN_ARM_DMA1_OFFSET_FROM_BAR0                                         (0x00246000)
#define ARM_DMA_WRAPPER_OFFSET_FROM_BAR0                                       (0x00247000)
#define HOST_IF_COUNTERS_OFFSET_FROM_BAR0                                      (0x00250000)
#define HOST_IF_ACC_OFFSET_FROM_BAR0                                           (0x00255000)
#define DLM_OFFSET_FROM_BAR0                                                   (0x00260000)
#define QUEUE_MANAGER_LCPU0_OFFSET_FROM_BAR0                                   (0x0026C000)
#define QUEUE_MANAGER_LCPU1_OFFSET_FROM_BAR0                                   (0x0026C000)
#define QUEUE_MANAGER_UCPU_OFFSET_FROM_BAR0                                    (0x0026C000)
#define TOP_REGBLOCK_OFFSET_FROM_BAR0                                          (0x00270000)
#define PCIE_PHY0_REGBLOCK_OFFSET_FROM_BAR0                                    (0x00278000)
#define PCIE_DBI0_REGBLOCK_OFFSET_FROM_BAR0                                    (0x0027B000)
#define PROT_DB_OFFSET_FROM_BAR0                                               (0x00290000)
#define PROT_DB_SHADOW_MEM_OFFSET_FROM_BAR0                                    (0x00290200)
#define TRAFFIC_INDICATOR_OFFSET_FROM_BAR0                                     (0x00291800)
#define VAP_ADDR_REG_OFFSET_FROM_BAR0                                          (0x00292000)
#define BA_ANALYZER_COUNTER_OFFSET_FROM_BAR0                                   (0x00294000)
#define BSRC_CNT_MEM_OFFSET_FROM_BAR0                                          (0x00298000)
#define BSRC_AGER_MEM_OFFSET_FROM_BAR0                                         (0x0029A000)
#define DATA_LOGGER_OFFSET_FROM_BAR0                                           (0x002A0000)
#define RX_PP_OFFSET_FROM_BAR0                                                 (0x002B0000)
#define RX_PP_REGS_OFFSET_FROM_BAR0                                            (0x002B9800)
#define RX_PP_COUNTERS_OFFSET_FROM_BAR0                                        (0x002BA000)
#define RETRY_STATE_DB_OFFSET_FROM_BAR0                                        (0x002BC000)
#define RETRY_POINTER_DB_OFFSET_FROM_BAR0                                      (0x002C0000)
#define SHARED_RAM_OFFSET_FROM_BAR0                                            (0x00300000)
#define DESCRIPTOR_RAM_OFFSET_FROM_BAR0                                        (0x00400000)
#define B0_MAC_GENRISC_TX_IRAM_OFFSET_FROM_BAR0                                (0x00800000)
#define B1_MAC_GENRISC_TX_IRAM_OFFSET_FROM_BAR0                                (0x00C00000)
#define B0_MAC_GENRISC_TX_SPRAM_OFFSET_FROM_BAR0                               (0x00810000)
#define B1_MAC_GENRISC_TX_SPRAM_OFFSET_FROM_BAR0                               (0x00C10000)
#define B0_MAC_GENRISC_TX_REG_OFFSET_FROM_BAR0                                 (0x00811000)
#define B1_MAC_GENRISC_TX_REG_OFFSET_FROM_BAR0                                 (0x00C11000)
#define B0_MAC_GENRISC_TX_STD_OFFSET_FROM_BAR0                                 (0x00812000)
#define B1_MAC_GENRISC_TX_STD_OFFSET_FROM_BAR0                                 (0x00C12000)
#define B0_MAC_GENRISC_RX_IRAM_OFFSET_FROM_BAR0                                (0x00820000)
#define B1_MAC_GENRISC_RX_IRAM_OFFSET_FROM_BAR0                                (0x00C20000)
#define B0_MAC_GENRISC_RX_SPRAM_OFFSET_FROM_BAR0                               (0x0082F000)
#define B1_MAC_GENRISC_RX_SPRAM_OFFSET_FROM_BAR0                               (0x00C2F000)
#define B0_MAC_GENRISC_RX_REG_OFFSET_FROM_BAR0                                 (0x00830000)
#define B1_MAC_GENRISC_RX_REG_OFFSET_FROM_BAR0                                 (0x00C30000)
#define B0_MAC_HT_EXTENSIONS_OFFSET_FROM_BAR0                                  (0x00831000)
#define B1_MAC_HT_EXTENSIONS_OFFSET_FROM_BAR0                                  (0x00C31000)
#define B0_MAC_TRAINER_OFFSET_FROM_BAR0                                        (0x00834000)
#define B1_MAC_TRAINER_OFFSET_FROM_BAR0                                        (0x00C34000)
#define B0_AUTO_RESP_MEMS_OFFSET_FROM_BAR0                                     (0x00838000)
#define B1_AUTO_RESP_MEMS_OFFSET_FROM_BAR0                                     (0x00C38000)
#define B0_AUTO_RESP_REGS_OFFSET_FROM_BAR0                                     (0x0083B000)
#define B1_AUTO_RESP_REGS_OFFSET_FROM_BAR0                                     (0x00C3B000)
#define B0_PAC_DUR_OFFSET_FROM_BAR0                                            (0x00900400)
#define B1_PAC_DUR_OFFSET_FROM_BAR0                                            (0x00D00400)
#define B0_PAC_EXTRAPOLATOR_OFFSET_FROM_BAR0                                   (0x00900600)
#define B1_PAC_EXTRAPOLATOR_OFFSET_FROM_BAR0                                   (0x00D00600)
#define B0_PAC_TIM_OFFSET_FROM_BAR0                                            (0x00900700)
#define B1_PAC_TIM_OFFSET_FROM_BAR0                                            (0x00D00700)
#define B0_MAC_BF_OFFSET_FROM_BAR0                                             (0x00900800)
#define B1_MAC_BF_OFFSET_FROM_BAR0                                             (0x00D00800)
#define B0_MAC_BEACON_OFFSET_FROM_BAR0                                         (0x00900A00)
#define B1_MAC_BEACON_OFFSET_FROM_BAR0                                         (0x00D00A00)
#define B0_TXH_TXC_OFFSET_FROM_BAR0                                            (0x00900E00)
#define B1_TXH_TXC_OFFSET_FROM_BAR0                                            (0x00D00E00)
#define B0_TXH_NTD_OFFSET_FROM_BAR0                                            (0x00900F00)
#define B1_TXH_NTD_OFFSET_FROM_BAR0                                            (0x00D00F00)
#define B0_PS_SETTING_OFFSET_FROM_BAR0                                         (0x00901000)
#define B1_PS_SETTING_OFFSET_FROM_BAR0                                         (0x00D01000)
#define B0_PAC_DELIA_REG_OFFSET_FROM_BAR0                                      (0x00901C00)
#define B1_PAC_DELIA_REG_OFFSET_FROM_BAR0                                      (0x00D01C00)
#define B0_PAC_DELIA_MEM_OFFSET_FROM_BAR0                                      (0x00901E00)
#define B1_PAC_DELIA_MEM_OFFSET_FROM_BAR0                                      (0x00D01E00)
#define B0_DUR_AUTO_RATE_MEM_OFFSET_FROM_BAR0                                  (0x00903800)
#define B1_DUR_AUTO_RATE_MEM_OFFSET_FROM_BAR0                                  (0x00D03800)
#define B0_PAC_RXC_OFFSET_FROM_BAR0                                            (0x00904000)
#define B1_PAC_RXC_OFFSET_FROM_BAR0                                            (0x00D04000)
#define B0_RXF_MEM_OFFSET_FROM_BAR0                                            (0x00904400)
#define B1_RXF_MEM_OFFSET_FROM_BAR0                                            (0x00D04400)
#define B0_MAC_SEC_MEM_OFFSET_FROM_BAR0                                        (0x00910000)
#define B1_MAC_SEC_MEM_OFFSET_FROM_BAR0                                        (0x00D10000)
#define B0_MAC_WEP_OFFSET_FROM_BAR0                                            (0x00918000)
#define B1_MAC_WEP_OFFSET_FROM_BAR0                                            (0x00D18000)
#define B0_RAB_OFFSET_FROM_BAR0                                                (0x00920000)
#define B1_RAB_OFFSET_FROM_BAR0                                                (0x00D20000)
#define B0_TX_SEQUENCER_OFFSET_FROM_BAR0                                       (0x0092E000)
#define B1_TX_SEQUENCER_OFFSET_FROM_BAR0                                       (0x00D2E000)
#define B0_TX_PD_ACC_MEM_OFFSET_FROM_BAR0                                      (0x0092E800)
#define B1_TX_PD_ACC_MEM_OFFSET_FROM_BAR0                                      (0x00D2E800)
#define B0_TX_PD_ACC_REG_OFFSET_FROM_BAR0                                      (0x0092EE00)
#define B1_TX_PD_ACC_REG_OFFSET_FROM_BAR0                                      (0x00D2EE00)
#define B0_TF_GENERATOR_REG_OFFSET_FROM_BAR0                                   (0x0092F000)
#define B1_TF_GENERATOR_REG_OFFSET_FROM_BAR0                                   (0x00D2F000)
#define B0_TF_GENERATOR_MEM_OFFSET_FROM_BAR0                                   (0x0092F200)
#define B1_TF_GENERATOR_MEM_OFFSET_FROM_BAR0                                   (0x00D2F200)
#define B0_CLC_OFFSET_FROM_BAR0                                                (0x00930000)
#define B1_CLC_OFFSET_FROM_BAR0                                                (0x00D30000)
#define B0_PRE_AGG_OFFSET_FROM_BAR0                                            (0x00932000)
#define B1_PRE_AGG_OFFSET_FROM_BAR0                                            (0x00D32000)
#define B0_MAC_ADDR2INDEX_OFFSET_FROM_BAR0                                     (0x00933000)
#define B1_MAC_ADDR2INDEX_OFFSET_FROM_BAR0                                     (0x00D33000)
#define B0_TX_HANDLER_OFFSET_FROM_BAR0                                         (0x00934000)
#define B1_TX_HANDLER_OFFSET_FROM_BAR0                                         (0x00D34000)
#define B0_BA_ANALYZER_OFFSET_FROM_BAR0                                        (0x00935000)
#define B1_BA_ANALYZER_OFFSET_FROM_BAR0                                        (0x00D35000)
#define B0_TX_SELECTOR_OFFSET_FROM_BAR0                                        (0x00938000)
#define B1_TX_SELECTOR_OFFSET_FROM_BAR0                                        (0x00D38000)
#define B0_RX_CLASSIFIER_OFFSET_FROM_BAR0                                      (0x0093B000)
#define B1_RX_CLASSIFIER_OFFSET_FROM_BAR0                                      (0x00D3B000)
#define B0_RX_MSDU_PARSER_OFFSET_FROM_BAR0                                     (0x0093E000)
#define B1_RX_MSDU_PARSER_OFFSET_FROM_BAR0                                     (0x00D3E000)
#define B0_RX_COORDINATOR_OFFSET_FROM_BAR0                                     (0x00941000)
#define B1_RX_COORDINATOR_OFFSET_FROM_BAR0                                     (0x00D41000)
#define B0_FRAME_CLASS_VIO_BUFFER_OFFSET_FROM_BAR0                             (0x00941200)
#define B1_FRAME_CLASS_VIO_BUFFER_OFFSET_FROM_BAR0                             (0x00D41200)
#define B0_OTFA_REG_OFFSET_FROM_BAR0                                           (0x00942000)
#define B1_OTFA_REG_OFFSET_FROM_BAR0                                           (0x00D42000)
#define B0_OTFA_MEM_OFFSET_FROM_BAR0                                           (0x00942200)
#define B1_OTFA_MEM_OFFSET_FROM_BAR0                                           (0x00D42200)
#define B0_MAC_TX_HC_OFFSET_FROM_BAR0                                          (0x00947800)
#define B1_MAC_TX_HC_OFFSET_FROM_BAR0                                          (0x00D47800)
#define B0_BSRC_REGS_OFFSET_FROM_BAR0                                          (0x00947C00)
#define B1_BSRC_REGS_OFFSET_FROM_BAR0                                          (0x00D47C00)
#define B0_MAC_BFM_OFFSET_FROM_BAR0                                            (0x00948000)
#define B1_MAC_BFM_OFFSET_FROM_BAR0                                            (0x00D48000)
#define B0_UL_POST_RX_OFFSET_FROM_BAR0                                         (0x0094A000)
#define B1_UL_POST_RX_OFFSET_FROM_BAR0                                         (0x00D4A000)
#define B0_BUF_STATUS_REPORT_OFFSET_FROM_BAR0                                  (0x00950000)
#define B1_BUF_STATUS_REPORT_OFFSET_FROM_BAR0                                  (0x00D50000)
#define B0_DUMMY_PHY_OFFSET_FROM_BAR0                                          (0x00AF0000)
#define B1_DUMMY_PHY_OFFSET_FROM_BAR0                                          (0x00EF0000)
#define B0_PHY_RX_BE_OFFSET_FROM_BAR0                                          (0x00B00000)
#define B1_PHY_RX_BE_OFFSET_FROM_BAR0                                          (0x00F00000)
#define B0_PHY_TEST_BUS_OFFSET_FROM_BAR0                                       (0x00B20000)
#define B1_PHY_TEST_BUS_OFFSET_FROM_BAR0                                       (0x00F20000)
#define B0_PHY_RX_TD_OFFSET_FROM_BAR0                                          (0x00B40000)
#define B1_PHY_RX_TD_OFFSET_FROM_BAR0                                          (0x00F40000)
#define B0_PHY_RX_FD_OFFSET_FROM_BAR0                                          (0x00B80000)
#define B1_PHY_RX_FD_OFFSET_FROM_BAR0                                          (0x00F80000)
#define B0_PHY_TX_OFFSET_FROM_BAR0                                             (0x00BC0000)
#define B1_PHY_TX_OFFSET_FROM_BAR0                                             (0x00FC0000)
#define BAND1_INTERNAL_OFFSET_FROM_BAR0                                        (0x00C00000)
#define B0_RX_CIRCULAR_OFFSET_FROM_BAR0                                        (0x01000000)
#define B1_RX_CIRCULAR_OFFSET_FROM_BAR0                                        (0x01400000)
#define B0_RX_PHY_STATUS_OFFSET_FROM_BAR0                                      (0x01378000)
#define B1_RX_PHY_STATUS_OFFSET_FROM_BAR0                                      (0x01778000)
#define B0_MPDU_DESC_MEM_OFFSET_FROM_BAR0                                      (0x01380000)
#define B1_MPDU_DESC_MEM_OFFSET_FROM_BAR0                                      (0x01780000)
#define BAND1_RX_OFFSET_FROM_BAR0                                              (0x01400000)
#define B0_TX_CIRCULAR_OFFSET_FROM_BAR0                                        (0x01800000)
#define B1_TX_CIRCULAR_OFFSET_FROM_BAR0                                        (0x01C00000)
#define B0_TX_RECIPES_OFFSET_FROM_BAR0                                         (0x01BF8000)
#define B1_TX_RECIPES_OFFSET_FROM_BAR0                                         (0x01FF8000)
#define BAND1_TX_OFFSET_FROM_BAR0                                              (0x01C00000)
#define HOST_EX_MAP_CPU_OFFSET_FROM_BAR0                                       (0x04000000)
#define B0_PHY_TXTD_ANT0_OFFSET_FROM_BAR0                                      (0x00BE0000)
#define B1_PHY_TXTD_ANT0_OFFSET_FROM_BAR0                                      (0x00FE0000)
#define B0_PHY_TXTD_ANT1_OFFSET_FROM_BAR0                                      (0x00BE4000)
#define B1_PHY_TXTD_ANT1_OFFSET_FROM_BAR0                                      (0x00FE4000)
#define B0_PHY_TXTD_ANT2_OFFSET_FROM_BAR0                                      (0x00BE8000)
#define B1_PHY_TXTD_ANT2_OFFSET_FROM_BAR0                                      (0x00FE8000)
#define B0_PHY_TXTD_ANT3_OFFSET_FROM_BAR0                                      (0x00BEC000)
#define B1_PHY_TXTD_ANT3_OFFSET_FROM_BAR0                                      (0x00FEC000)
#define B0_PHY_RXTD_ANT0_OFFSET_FROM_BAR0                                      (0x00B60000)
#define B1_PHY_RXTD_ANT0_OFFSET_FROM_BAR0                                      (0x00F60000)
#define B0_PHY_RXTD_ANT1_OFFSET_FROM_BAR0                                      (0x00B64000)
#define B1_PHY_RXTD_ANT1_OFFSET_FROM_BAR0                                      (0x00F64000)
#define B0_PHY_RXTD_ANT2_OFFSET_FROM_BAR0                                      (0x00B68000)
#define B1_PHY_RXTD_ANT2_OFFSET_FROM_BAR0                                      (0x00F68000)
#define B0_PHY_RXTD_ANT3_OFFSET_FROM_BAR0                                      (0x00B6C000)
#define B1_PHY_RXTD_ANT3_OFFSET_FROM_BAR0                                      (0x00F6C000)
#define B0_PHY_AGC_ANT0_OFFSET_FROM_BAR0                                       (0x00B63000)
#define B1_PHY_AGC_ANT0_OFFSET_FROM_BAR0                                       (0x00F63000)
#define B0_PHY_AGC_ANT1_OFFSET_FROM_BAR0                                       (0x00B67000)
#define B1_PHY_AGC_ANT1_OFFSET_FROM_BAR0                                       (0x00F67000)
#define B0_PHY_AGC_ANT2_OFFSET_FROM_BAR0                                       (0x00B6B000)
#define B1_PHY_AGC_ANT2_OFFSET_FROM_BAR0                                       (0x00F6B000)
#define B0_PHY_AGC_ANT3_OFFSET_FROM_BAR0                                       (0x00B6F000)
#define B1_PHY_AGC_ANT3_OFFSET_FROM_BAR0                                       (0x00F6F000)
#define B0_RF_EMULATOR_OFFSET_FROM_BAR0                                        (0x00274000)
#define B1_RF_EMULATOR_OFFSET_FROM_BAR0                                        (0x00674000)

 /* Modules Sizes */
#define CPU_IRAM_SIZE                                                          (0x00180000)
#define SYSTEM_IF_LOWER_B0_CPU_SIZE                                            (0x00004000)
#define SYSTEM_IF_LOWER_B1_CPU_SIZE                                            (0x00004000)
#define SYSTEM_IF_UPPER_CPU_SIZE                                               (0x00004000)
#define MAC_GENRISC_HOST_IRAM_SIZE                                             (0x00003000)
#define MAC_GENRISC_HOST_REG_SIZE                                              (0x00000400)
#define MAC_GENRISC_HOST_SPRAM_SIZE                                            (0x00000600)
#define MAC_GEN_REGFILE_SIZE                                                   (0x00001000)
#define WLAN_ARM_LL_MEM_SIZE                                                   (0x00005000)
#define WLAN_ARM_DMA0_SIZE                                                     (0x00001000)
#define WLAN_ARM_DMA1_SIZE                                                     (0x00001000)
#define ARM_DMA_WRAPPER_SIZE                                                   (0x00001000)
#define HOST_IF_COUNTERS_SIZE                                                  (0x00005000)
#define HOST_IF_ACC_SIZE                                                       (0x00001000)
#define DLM_SIZE                                                               (0x0000C000)
#define QUEUE_MANAGER_LCPU0_SIZE                                               (0x00000400)
#define QUEUE_MANAGER_LCPU1_SIZE                                               (0x00000400)
#define QUEUE_MANAGER_UCPU_SIZE                                                (0x00000400)
#define TOP_REGBLOCK_SIZE                                                      (0x00008000)
#define PCIE_PHY0_REGBLOCK_SIZE                                                (0x00003000)
#define PCIE_DBI0_REGBLOCK_SIZE                                                (0x00005000)
#define PROT_DB_SIZE                                                           (0x00000200)
#define PROT_DB_SHADOW_MEM_SIZE                                                (0x00000400)
#define TRAFFIC_INDICATOR_SIZE                                                 (0x00000800)
#define VAP_ADDR_REG_SIZE                                                      (0x00000400)
#define BA_ANALYZER_COUNTER_SIZE                                               (0x00002000)
#define BSRC_CNT_MEM_SIZE                                                      (0x00002000)
#define BSRC_AGER_MEM_SIZE                                                     (0x00001000)
#define DATA_LOGGER_SIZE                                                       (0x00001000)
#define RX_PP_SIZE                                                             (0x00009800)
#define RX_PP_REGS_SIZE                                                        (0x00000800)
#define RX_PP_COUNTERS_SIZE                                                    (0x00002000)
#define RETRY_STATE_DB_SIZE                                                    (0x00004000)
#define RETRY_POINTER_DB_SIZE                                                  (0x00020000)
#define SHARED_RAM_SIZE                                                        (0x00050000)
#define DESCRIPTOR_RAM_SIZE                                                    (0x00200000)
#define B0_MAC_GENRISC_TX_IRAM_SIZE                                            (0x00010000)
#define B1_MAC_GENRISC_TX_IRAM_SIZE                                            (0x00010000)
#define B0_MAC_GENRISC_TX_SPRAM_SIZE                                           (0x00001000)
#define B1_MAC_GENRISC_TX_SPRAM_SIZE                                           (0x00001000)
#define B0_MAC_GENRISC_TX_REG_SIZE                                             (0x00000400)
#define B1_MAC_GENRISC_TX_REG_SIZE                                             (0x00000400)
#define B0_MAC_GENRISC_TX_STD_SIZE                                             (0x00002000)
#define B1_MAC_GENRISC_TX_STD_SIZE                                             (0x00002000)
#define B0_MAC_GENRISC_RX_IRAM_SIZE                                            (0x00004000)
#define B1_MAC_GENRISC_RX_IRAM_SIZE                                            (0x00004000)
#define B0_MAC_GENRISC_RX_SPRAM_SIZE                                           (0x00001000)
#define B1_MAC_GENRISC_RX_SPRAM_SIZE                                           (0x00001000)
#define B0_MAC_GENRISC_RX_REG_SIZE                                             (0x00000400)
#define B1_MAC_GENRISC_RX_REG_SIZE                                             (0x00000400)
#define B0_MAC_HT_EXTENSIONS_SIZE                                              (0x00001000)
#define B1_MAC_HT_EXTENSIONS_SIZE                                              (0x00001000)
#define B0_MAC_TRAINER_SIZE                                                    (0x00000400)
#define B1_MAC_TRAINER_SIZE                                                    (0x00000400)
#define B0_AUTO_RESP_MEMS_SIZE                                                 (0x00003000)
#define B1_AUTO_RESP_MEMS_SIZE                                                 (0x00003000)
#define B0_AUTO_RESP_REGS_SIZE                                                 (0x00000800)
#define B1_AUTO_RESP_REGS_SIZE                                                 (0x00000800)
#define B0_PAC_DUR_SIZE                                                        (0x00000100)
#define B1_PAC_DUR_SIZE                                                        (0x00000100)
#define B0_PAC_EXTRAPOLATOR_SIZE                                               (0x00000100)
#define B1_PAC_EXTRAPOLATOR_SIZE                                               (0x00000100)
#define B0_PAC_TIM_SIZE                                                        (0x00000100)
#define B1_PAC_TIM_SIZE                                                        (0x00000100)
#define B0_MAC_BF_SIZE                                                         (0x00000200)
#define B1_MAC_BF_SIZE                                                         (0x00000200)
#define B0_MAC_BEACON_SIZE                                                     (0x00000200)
#define B1_MAC_BEACON_SIZE                                                     (0x00000200)
#define B0_TXH_TXC_SIZE                                                        (0x00000100)
#define B1_TXH_TXC_SIZE                                                        (0x00000100)
#define B0_TXH_NTD_SIZE                                                        (0x00000100)
#define B1_TXH_NTD_SIZE                                                        (0x00000100)
#define B0_PS_SETTING_SIZE                                                     (0x00000100)
#define B1_PS_SETTING_SIZE                                                     (0x00000100)
#define B0_PAC_DELIA_REG_SIZE                                                  (0x00000200)
#define B1_PAC_DELIA_REG_SIZE                                                  (0x00000200)
#define B0_PAC_DELIA_MEM_SIZE                                                  (0x00000200)
#define B1_PAC_DELIA_MEM_SIZE                                                  (0x00000200)
#define B0_DUR_AUTO_RATE_MEM_SIZE                                              (0x00000400)
#define B1_DUR_AUTO_RATE_MEM_SIZE                                              (0x00000400)
#define B0_PAC_RXC_SIZE                                                        (0x00000400)
#define B1_PAC_RXC_SIZE                                                        (0x00000400)
#define B0_RXF_MEM_SIZE                                                        (0x00000C00)
#define B1_RXF_MEM_SIZE                                                        (0x00000C00)
#define B0_MAC_SEC_MEM_SIZE                                                    (0x00004000)
#define B1_MAC_SEC_MEM_SIZE                                                    (0x00004000)
#define B0_MAC_WEP_SIZE                                                        (0x00000400)
#define B1_MAC_WEP_SIZE                                                        (0x00000400)
#define B0_RAB_SIZE                                                            (0x00001000)
#define B1_RAB_SIZE                                                            (0x00001000)
#define B0_TX_SEQUENCER_SIZE                                                   (0x00000800)
#define B1_TX_SEQUENCER_SIZE                                                   (0x00000800)
#define B0_TX_PD_ACC_MEM_SIZE                                                  (0x00000600)
#define B1_TX_PD_ACC_MEM_SIZE                                                  (0x00000600)
#define B0_TX_PD_ACC_REG_SIZE                                                  (0x00000200)
#define B1_TX_PD_ACC_REG_SIZE                                                  (0x00000200)
#define B0_TF_GENERATOR_REG_SIZE                                               (0x00000200)
#define B1_TF_GENERATOR_REG_SIZE                                               (0x00000200)
#define B0_TF_GENERATOR_MEM_SIZE                                               (0x00000600)
#define B1_TF_GENERATOR_MEM_SIZE                                               (0x00000600)
#define B0_CLC_SIZE                                                            (0x00001000)
#define B1_CLC_SIZE                                                            (0x00001000)
#define B0_PRE_AGG_SIZE                                                        (0x00001000)
#define B1_PRE_AGG_SIZE                                                        (0x00001000)
#define B0_MAC_ADDR2INDEX_SIZE                                                 (0x00001000)
#define B1_MAC_ADDR2INDEX_SIZE                                                 (0x00001000)
#define B0_TX_HANDLER_SIZE                                                     (0x00001000)
#define B1_TX_HANDLER_SIZE                                                     (0x00001000)
#define B0_BA_ANALYZER_SIZE                                                    (0x00001000)
#define B1_BA_ANALYZER_SIZE                                                    (0x00001000)
#define B0_TX_SELECTOR_SIZE                                                    (0x00003000)
#define B1_TX_SELECTOR_SIZE                                                    (0x00003000)
#define B0_RX_CLASSIFIER_SIZE                                                  (0x00001000)
#define B1_RX_CLASSIFIER_SIZE                                                  (0x00001000)
#define B0_RX_MSDU_PARSER_SIZE                                                 (0x00000200)
#define B1_RX_MSDU_PARSER_SIZE                                                 (0x00000200)
#define B0_RX_COORDINATOR_SIZE                                                 (0x00000200)
#define B1_RX_COORDINATOR_SIZE                                                 (0x00000200)
#define B0_FRAME_CLASS_VIO_BUFFER_SIZE                                         (0x00000200)
#define B1_FRAME_CLASS_VIO_BUFFER_SIZE                                         (0x00000200)
#define B0_OTFA_REG_SIZE                                                       (0x00000200)
#define B1_OTFA_REG_SIZE                                                       (0x00000200)
#define B0_OTFA_MEM_SIZE                                                       (0x00001E00)
#define B1_OTFA_MEM_SIZE                                                       (0x00001E00)
#define B0_MAC_TX_HC_SIZE                                                      (0x00000080)
#define B1_MAC_TX_HC_SIZE                                                      (0x00000080)
#define B0_BSRC_REGS_SIZE                                                      (0x00000200)
#define B1_BSRC_REGS_SIZE                                                      (0x00000200)
#define B0_MAC_BFM_SIZE                                                        (0x00001000)
#define B1_MAC_BFM_SIZE                                                        (0x00001000)
#define B0_UL_POST_RX_SIZE                                                     (0x00000100)
#define B1_UL_POST_RX_SIZE                                                     (0x00000100)
#define B0_BUF_STATUS_REPORT_SIZE                                              (0x00004000)
#define B1_BUF_STATUS_REPORT_SIZE                                              (0x00004000)
#define B0_DUMMY_PHY_SIZE                                                      (0x00010000)
#define B1_DUMMY_PHY_SIZE                                                      (0x00010000)
#define B0_PHY_RX_BE_SIZE                                                      (0x00020000)
#define B1_PHY_RX_BE_SIZE                                                      (0x00020000)
#define B0_PHY_TEST_BUS_SIZE                                                   (0x00020000)
#define B1_PHY_TEST_BUS_SIZE                                                   (0x00020000)
#define B0_PHY_RX_TD_SIZE                                                      (0x00040000)
#define B1_PHY_RX_TD_SIZE                                                      (0x00040000)
#define B0_PHY_RX_FD_SIZE                                                      (0x00040000)
#define B1_PHY_RX_FD_SIZE                                                      (0x00040000)
#define B0_PHY_TX_SIZE                                                         (0x00040000)
#define B1_PHY_TX_SIZE                                                         (0x00040000)
#define BAND1_INTERNAL_SIZE                                                    (0x00400000)
#define B0_RX_CIRCULAR_SIZE                                                    (0x00378000)
#define B1_RX_CIRCULAR_SIZE                                                    (0x00378000)
#define B0_RX_PHY_STATUS_SIZE                                                  (0x00008000)
#define B1_RX_PHY_STATUS_SIZE                                                  (0x00008000)
#define B0_MPDU_DESC_MEM_SIZE                                                  (0x00080000)
#define B1_MPDU_DESC_MEM_SIZE                                                  (0x00080000)
#define BAND1_RX_SIZE                                                          (0x00400000)
#define B0_TX_CIRCULAR_SIZE                                                    (0x003F8000)
#define B1_TX_CIRCULAR_SIZE                                                    (0x003F8000)
#define B0_TX_RECIPES_SIZE                                                     (0x00008000)
#define B1_TX_RECIPES_SIZE                                                     (0x00008000)
#define BAND1_TX_SIZE                                                          (0x00400000)
#define HOST_EX_MAP_CPU_SIZE                                                   (0x04000000)
#define B0_PHY_TXTD_ANT0_SIZE                                                  (0x00000400)
#define B1_PHY_TXTD_ANT0_SIZE                                                  (0x00000400)
#define B0_PHY_TXTD_ANT1_SIZE                                                  (0x00000400)
#define B1_PHY_TXTD_ANT1_SIZE                                                  (0x00000400)
#define B0_PHY_TXTD_ANT2_SIZE                                                  (0x00000400)
#define B1_PHY_TXTD_ANT2_SIZE                                                  (0x00000400)
#define B0_PHY_TXTD_ANT3_SIZE                                                  (0x00000400)
#define B1_PHY_TXTD_ANT3_SIZE                                                  (0x00000400)
#define B0_PHY_RXTD_ANT0_SIZE                                                  (0x00000400)
#define B1_PHY_RXTD_ANT0_SIZE                                                  (0x00000400)
#define B0_PHY_RXTD_ANT1_SIZE                                                  (0x00000400)
#define B1_PHY_RXTD_ANT1_SIZE                                                  (0x00000400)
#define B0_PHY_RXTD_ANT2_SIZE                                                  (0x00000400)
#define B1_PHY_RXTD_ANT2_SIZE                                                  (0x00000400)
#define B0_PHY_RXTD_ANT3_SIZE                                                  (0x00000400)
#define B1_PHY_RXTD_ANT3_SIZE                                                  (0x00000400)
#define B0_PHY_AGC_ANT0_SIZE                                                   (0x00000400)
#define B1_PHY_AGC_ANT0_SIZE                                                   (0x00000400)
#define B0_PHY_AGC_ANT1_SIZE                                                   (0x00000400)
#define B1_PHY_AGC_ANT1_SIZE                                                   (0x00000400)
#define B0_PHY_AGC_ANT2_SIZE                                                   (0x00000400)
#define B1_PHY_AGC_ANT2_SIZE                                                   (0x00000400)
#define B0_PHY_AGC_ANT3_SIZE                                                   (0x00000400)
#define B1_PHY_AGC_ANT3_SIZE                                                   (0x00000400)
#define B0_RF_EMULATOR_SIZE                                                    (0x00004000)
#define B1_RF_EMULATOR_SIZE                                                    (0x00004000)
#define REDUCED_DESCRIPTOR_RAM_SIZE                                            (0x00080000)
#define REDUCED_CPU_IRAM_SIZE                                                  (0x00180000)

 /* auto generated from wave600_rams_by_modules_ver0.29.0.xlsm, sheet gen6_wlanip_rams_by_modules */
#define B0_LINEAR_SLICER_EVM_MU                                                (B0_PHY_RX_FD_BASE_ADDR + 0x00002000) /* MEMORY_MAP_UNIT_51_BASE_ADDRESS */
#define B0_SEC_ENG_TOP_MIC_MEM                                                 (B0_MAC_SEC_MEM_BASE_ADDR + 0x00004000) /* MEMORY_MAP_UNIT_1077_BASE_ADDRESS */
#define HYP_TD_ANTENNA_0_RXTD_FIFO_0                                           (B0_PHY_RX_TD_BASE_ADDR + 0x00018000) /* MEMORY_MAP_UNIT_50_BASE_ADDRESS */
#define HYP_TD_ANTENNA_1_RXTD_FIFO_0                                           (B0_PHY_RX_TD_BASE_ADDR + 0x00018000) /* MEMORY_MAP_UNIT_50_BASE_ADDRESS */
#define HYP_TD_ANTENNA_2_RXTD_FIFO_0                                           (B0_PHY_RX_TD_BASE_ADDR + 0x00018000) /* MEMORY_MAP_UNIT_50_BASE_ADDRESS */
#define HYP_TD_ANTENNA_3_RXTD_FIFO_0                                           (B0_PHY_RX_TD_BASE_ADDR + 0x00018000) /* MEMORY_MAP_UNIT_50_BASE_ADDRESS */
#define B1_SEC_ENG_TOP_MIC_MEM                                                 (B1_MAC_SEC_MEM_BASE_ADDR + 0x00004000) /* MEMORY_MAP_UNIT_11077_BASE_ADDRESS */
#define HYP_TD_ANTENNA_0_FFT_0_RAM                                             (B0_PHY_RX_TD_BASE_ADDR + 0x0001C000) /* MEMORY_MAP_UNIT_50_BASE_ADDRESS */
#define HYP_TD_ANTENNA_0_FFT_1_RAM                                             (B0_PHY_RX_TD_BASE_ADDR + 0x0001E000) /* MEMORY_MAP_UNIT_50_BASE_ADDRESS */
#define HYP_TD_ANTENNA_1_FFT_0_RAM                                             (B0_PHY_RX_TD_BASE_ADDR + 0x0001C000) /* MEMORY_MAP_UNIT_50_BASE_ADDRESS */
#define HYP_TD_ANTENNA_1_FFT_1_RAM                                             (B0_PHY_RX_TD_BASE_ADDR + 0x0001E000) /* MEMORY_MAP_UNIT_50_BASE_ADDRESS */
#define HYP_TD_ANTENNA_2_FFT_0_RAM                                             (B0_PHY_RX_TD_BASE_ADDR + 0x0001C000) /* MEMORY_MAP_UNIT_50_BASE_ADDRESS */
#define HYP_TD_ANTENNA_2_FFT_1_RAM                                             (B0_PHY_RX_TD_BASE_ADDR + 0x0001E000) /* MEMORY_MAP_UNIT_50_BASE_ADDRESS */
#define HYP_TD_ANTENNA_3_FFT_0_RAM                                             (B0_PHY_RX_TD_BASE_ADDR + 0x0001C000) /* MEMORY_MAP_UNIT_50_BASE_ADDRESS */
#define HYP_TD_ANTENNA_3_FFT_1_RAM                                             (B0_PHY_RX_TD_BASE_ADDR + 0x0001E000) /* MEMORY_MAP_UNIT_50_BASE_ADDRESS */
#define CH_RAM_0_DATA_CH_RAM                                                   (B0_PHY_RX_FD_BASE_ADDR + 0x00010000) /* MEMORY_MAP_UNIT_51_BASE_ADDRESS */
#define CH_RAM_1_DATA_CH_RAM                                                   (B0_PHY_RX_FD_BASE_ADDR + 0x00010000) /* MEMORY_MAP_UNIT_51_BASE_ADDRESS */
#define CH_RAM_2_DATA_CH_RAM                                                   (B0_PHY_RX_FD_BASE_ADDR + 0x00010000) /* MEMORY_MAP_UNIT_51_BASE_ADDRESS */
#define CH_RAM_3_DATA_CH_RAM                                                   (B0_PHY_RX_FD_BASE_ADDR + 0x00010000) /* MEMORY_MAP_UNIT_51_BASE_ADDRESS */
#define B1_LINEAR_SLICER_EVM_MU                                                (B1_PHY_RX_FD_BASE_ADDR + 0x00002000) /* MEMORY_MAP_UNIT_10051_BASE_ADDRESS */
#define B0_MAC_GEN_RISC_RAMS_GEN_RISC_TX_IRAM                                  (B0_MAC_GENRISC_TX_IRAM_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_1024_BASE_ADDRESS */
#define B1_MAC_GEN_RISC_RAMS_GEN_RISC_TX_IRAM                                  (B1_MAC_GENRISC_TX_IRAM_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_11024_BASE_ADDRESS */
#define Q_MANAGER_DMA_DLMS_RAM                                                 (DLM_BASE_ADDR + 0x0000A480) /* MEMORY_MAP_UNIT_40_BASE_ADDRESS */
#define B0_GEN_RISC_GEN_RISC_IRAM                                              (B0_PHY_RX_TD_BASE_ADDR + 0x00008000) /* MEMORY_MAP_UNIT_50_BASE_ADDRESS */
#define B1_GEN_RISC_GEN_RISC_IRAM                                              (B1_PHY_RX_TD_BASE_ADDR + 0x00008000) /* MEMORY_MAP_UNIT_10050_BASE_ADDRESS */
#define Q_MANAGER_RX_MPDU_DESC0_DLMS_RAM                                       (DLM_BASE_ADDR + 0x0000A5C8) /* MEMORY_MAP_UNIT_40_BASE_ADDRESS */
#define Q_MANAGER_RX_MPDU_DESC1_DLMS_RAM                                       (DLM_BASE_ADDR + 0x0000A708) /* MEMORY_MAP_UNIT_40_BASE_ADDRESS */
#define B0_RX_CLASSIFIER_RX_CLASSIFIER_MEM                                     (B0_RX_CLASSIFIER_BASE_ADDR + 0x00000800) /* MEMORY_MAP_UNIT_57_BASE_ADDRESS */
#define B1_RX_CLASSIFIER_RX_CLASSIFIER_MEM                                     (B1_RX_CLASSIFIER_BASE_ADDR + 0x00000800) /* MEMORY_MAP_UNIT_10057_BASE_ADDRESS */
#define B0_AUTO_RESP_TID_BITMAP_MEM                                            (B0_AUTO_RESP_MEMS_BASE_ADDR + 0x00001B00) /* MEMORY_MAP_UNIT_1053_BASE_ADDRESS */
#define B0_SEC_ENG_TOP_KEY_MEM                                                 (B0_MAC_SEC_MEM_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_1077_BASE_ADDRESS */
#define B0_SEC_ENG_TOP_USER_ATTR_MEM                                           (B0_MAC_SEC_MEM_BASE_ADDR + 0x00002000) /* MEMORY_MAP_UNIT_1077_BASE_ADDRESS */
#define B1_TX_SELECTOR_STA_TID_RAM0                                            (B1_TX_SELECTOR_BASE_ADDR + 0x00000A00) /* MEMORY_MAP_UNIT_10029_BASE_ADDRESS */
#define B1_TX_SELECTOR_STA_TID_RAM1                                            (B1_TX_SELECTOR_BASE_ADDR + 0x00000A00) /* MEMORY_MAP_UNIT_10029_BASE_ADDRESS */
#define B1_TX_SELECTOR_TX_SEL_VHT_MU_STA_RAM                                   (B1_TX_SELECTOR_BASE_ADDR + 0x00000800) /* MEMORY_MAP_UNIT_10029_BASE_ADDRESS */
#define B0_MAC_GEN_RISC_RAMS_GEN_RISC_TX_STD                                   (B0_MAC_GENRISC_TX_STD_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_1029_BASE_ADDRESS */
#define B1_MAC_GEN_RISC_RAMS_GEN_RISC_TX_STD                                   (B1_MAC_GENRISC_TX_STD_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_11029_BASE_ADDRESS */
#define RETRY_STATE_DB_DUAL_BAND_RAM                                           (RETRY_STATE_DB_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_1056_BASE_ADDRESS */
#define B0_OTFA_TOP_LOCAL_DB_MEM_I                                             (B0_OTFA_MEM_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_1073_BASE_ADDRESS */
#define MAC_TX_SHARED_RAM_MEM0_0                                               (B0_TX_RECIPES_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_1023_BASE_ADDRESS */
#define MAC_TX_SHARED_RAM_MEM0_1                                               (B0_TX_RECIPES_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_1023_BASE_ADDRESS */
#define MAC_TX_SHARED_RAM_MEM0_2                                               (B0_TX_RECIPES_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_1023_BASE_ADDRESS */
#define MAC_TX_SHARED_RAM_MEM0_3                                               (B0_TX_RECIPES_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_1023_BASE_ADDRESS */
#define HYP_TD_ANTENNA_0_DELAY_FIFO                                            (B0_PHY_RX_TD_BASE_ADDR + 0x00021000) /* MEMORY_MAP_UNIT_50_BASE_ADDRESS */
#define HYP_TD_ANTENNA_1_DELAY_FIFO                                            (B0_PHY_RX_TD_BASE_ADDR + 0x00025000) /* MEMORY_MAP_UNIT_50_BASE_ADDRESS */
#define HYP_TD_ANTENNA_2_DELAY_FIFO                                            (B0_PHY_RX_TD_BASE_ADDR + 0x00029000) /* MEMORY_MAP_UNIT_50_BASE_ADDRESS */
#define HYP_TD_ANTENNA_3_DELAY_FIFO                                            (B0_PHY_RX_TD_BASE_ADDR + 0x0002D000) /* MEMORY_MAP_UNIT_50_BASE_ADDRESS */
#define Q_MANAGER_PD_DLMS_RAM                                                  (DLM_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_40_BASE_ADDRESS */
#define MAC_RX_SHARED_RAM_MEM2_0                                               (B0_RX_CIRCULAR_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_1020_BASE_ADDRESS */
#define MAC_RX_SHARED_RAM_MEM2_1                                               (B0_RX_CIRCULAR_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_1020_BASE_ADDRESS */
#define MAC_RX_SHARED_RAM_MEM2_2                                               (B0_RX_CIRCULAR_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_1020_BASE_ADDRESS */
#define MAC_RX_SHARED_RAM_MEM2_3                                               (B0_RX_CIRCULAR_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_1020_BASE_ADDRESS */
#define B0_TXH_MAP_HANDLER_TXH_MAP_MEM                                         (B0_TX_HANDLER_BASE_ADDR + 0x00000800) /* MEMORY_MAP_UNIT_23_BASE_ADDRESS */
#define B1_TXH_MAP_HANDLER_TXH_MAP_MEM                                         (B1_TX_HANDLER_BASE_ADDR + 0x00000800) /* MEMORY_MAP_UNIT_10023_BASE_ADDRESS */
#define HYP_TD_ANTENNA_0_IQ_RAM                                                (B0_PHY_TX_BASE_ADDR + 0x00021800) /* MEMORY_MAP_UNIT_52_BASE_ADDRESS */
#define HYP_TD_ANTENNA_1_IQ_RAM                                                (B0_PHY_TX_BASE_ADDR + 0x00025800) /* MEMORY_MAP_UNIT_52_BASE_ADDRESS */
#define HYP_TD_ANTENNA_2_IQ_RAM                                                (B0_PHY_TX_BASE_ADDR + 0x00029800) /* MEMORY_MAP_UNIT_52_BASE_ADDRESS */
#define HYP_TD_ANTENNA_3_IQ_RAM                                                (B0_PHY_TX_BASE_ADDR + 0x0002D800) /* MEMORY_MAP_UNIT_52_BASE_ADDRESS */
#define HYP_TD_ANTENNA_0_RIPPLE_RAM                                            (B0_PHY_TX_BASE_ADDR + 0x00021000) /* MEMORY_MAP_UNIT_52_BASE_ADDRESS */
#define HYP_TD_ANTENNA_1_RIPPLE_RAM                                            (B0_PHY_TX_BASE_ADDR + 0x00025000) /* MEMORY_MAP_UNIT_52_BASE_ADDRESS */
#define HYP_TD_ANTENNA_2_RIPPLE_RAM                                            (B0_PHY_TX_BASE_ADDR + 0x00029000) /* MEMORY_MAP_UNIT_52_BASE_ADDRESS */
#define HYP_TD_ANTENNA_3_RIPPLE_RAM                                            (B0_PHY_TX_BASE_ADDR + 0x0002D000) /* MEMORY_MAP_UNIT_52_BASE_ADDRESS */
#define MAC_RX_SHARED_RAM_MEM0_0                                               (B0_RX_CIRCULAR_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_1020_BASE_ADDRESS */
#define MAC_RX_SHARED_RAM_MEM0_1                                               (B0_RX_CIRCULAR_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_1020_BASE_ADDRESS */
#define MAC_RX_SHARED_RAM_MEM0_2                                               (B0_RX_CIRCULAR_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_1020_BASE_ADDRESS */
#define MAC_RX_SHARED_RAM_MEM0_3                                               (B0_RX_CIRCULAR_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_1020_BASE_ADDRESS */
#define B0_AUTO_RESP_USER_DB_MEM                                               (B0_AUTO_RESP_MEMS_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_1053_BASE_ADDRESS */
#define HOST_IF_ACC_DIST_RAM                                                   (HOST_IF_ACC_BASE_ADDR + 0x00000c00) /* MEMORY_MAP_UNIT_39_BASE_ADDRESS */
#define LOGGER_WRAPPER_UDP                                                     (DATA_LOGGER_BASE_ADDR + 0x00000400) /* MEMORY_MAP_UNIT_61_BASE_ADDRESS */
#define MAC_TX_SHARED_RAM_MEM1_0                                               (B0_TX_CIRCULAR_BASE_ADDR + 0x00090000) /* MEMORY_MAP_UNIT_1022_BASE_ADDRESS */
#define MAC_TX_SHARED_RAM_MEM1_1                                               (B0_TX_CIRCULAR_BASE_ADDR + 0x00090000) /* MEMORY_MAP_UNIT_1022_BASE_ADDRESS */
#define MAC_TX_SHARED_RAM_MEM1_2                                               (B0_TX_CIRCULAR_BASE_ADDR + 0x00090000) /* MEMORY_MAP_UNIT_1022_BASE_ADDRESS */
#define MAC_TX_SHARED_RAM_MEM1_3                                               (B0_TX_CIRCULAR_BASE_ADDR + 0x00090000) /* MEMORY_MAP_UNIT_1022_BASE_ADDRESS */
#define B0_HYP_RXFD_BIN_CHOOSER_NE_RAM                                         (B0_PHY_RX_FD_BASE_ADDR + 0x00007000) /* MEMORY_MAP_UNIT_51_BASE_ADDRESS */
#define B1_HYP_RXFD_BIN_CHOOSER_NE_RAM                                         (B1_PHY_RX_FD_BASE_ADDR + 0x00007000) /* MEMORY_MAP_UNIT_10051_BASE_ADDRESS */
#define MAC_RX_SHARED_RAM_MEM1_0                                               (B0_RX_CIRCULAR_BASE_ADDR + 0x00090000) /* MEMORY_MAP_UNIT_1020_BASE_ADDRESS */
#define MAC_RX_SHARED_RAM_MEM1_1                                               (B0_RX_CIRCULAR_BASE_ADDR + 0x00090000) /* MEMORY_MAP_UNIT_1020_BASE_ADDRESS */
#define MAC_RX_SHARED_RAM_MEM1_2                                               (B0_RX_CIRCULAR_BASE_ADDR + 0x00090000) /* MEMORY_MAP_UNIT_1020_BASE_ADDRESS */
#define MAC_RX_SHARED_RAM_MEM1_3                                               (B0_RX_CIRCULAR_BASE_ADDR + 0x00090000) /* MEMORY_MAP_UNIT_1020_BASE_ADDRESS */
#define B0_TX_PD_ACC_TXPD_LOCAL_CONTEXT_MEM                                    (B0_TX_PD_ACC_MEM_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_1097_BASE_ADDRESS */
#define B0_RX_RCR_RCR_RAM_CDB0                                                 (B0_PHY_RX_BE_BASE_ADDR + 0x00018000) /* MEMORY_MAP_UNIT_49_BASE_ADDRESS */
#define B1_RX_RCR_RCR_RAM_CDB1                                                 (B1_PHY_RX_BE_BASE_ADDR + 0x00018000) /* MEMORY_MAP_UNIT_10049_BASE_ADDRESS */
#define B0_PRE_AGG_PRE_AGG_MEM                                                 (B0_PRE_AGG_BASE_ADDR + 0x00000400) /* MEMORY_MAP_UNIT_21_BASE_ADDRESS */
#define B1_PRE_AGG_PRE_AGG_MEM                                                 (B1_PRE_AGG_BASE_ADDR + 0x00000400) /* MEMORY_MAP_UNIT_10021_BASE_ADDRESS */
#define HOST_IF_GEN_RISC_HOST_SPRAM                                            (MAC_GENRISC_HOST_SPRAM_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_1006_BASE_ADDRESS */
#define B0_CH_ENERGY_PER_RU_RSSI_PER_RU_RAM                                    (B0_PHY_RX_FD_BASE_ADDR + 0x00002E00) /* MEMORY_MAP_UNIT_51_BASE_ADDRESS */
#define B1_CH_ENERGY_PER_RU_RSSI_PER_RU_RAM                                    (B1_PHY_RX_FD_BASE_ADDR + 0x00002E00) /* MEMORY_MAP_UNIT_10051_BASE_ADDRESS */
#define B1_AUTO_RESP_TID_BITMAP_MEM                                            (B1_AUTO_RESP_MEMS_BASE_ADDR + 0x00001B00) /* MEMORY_MAP_UNIT_11053_BASE_ADDRESS */
#define B1_SEC_ENG_TOP_KEY_MEM                                                 (B1_MAC_SEC_MEM_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_11077_BASE_ADDRESS */
#define B1_SEC_ENG_TOP_USER_ATTR_MEM                                           (B1_MAC_SEC_MEM_BASE_ADDR + 0x00002000) /* MEMORY_MAP_UNIT_11077_BASE_ADDRESS */
#define B0_DC_ESTIMATION_CFO_THRESHOLD_RAM                                     (B0_PHY_RX_TD_BASE_ADDR + 0x00003000) /* MEMORY_MAP_UNIT_50_BASE_ADDRESS */
#define B1_DC_ESTIMATION_CFO_THRESHOLD_RAM                                     (B1_PHY_RX_TD_BASE_ADDR + 0x00003000) /* MEMORY_MAP_UNIT_10050_BASE_ADDRESS */
#define B0_LONG_PREAMBLE_LONG_PREAMBLE_RAM                                     (B0_PHY_TX_BASE_ADDR + 0x00004000) /* MEMORY_MAP_UNIT_52_BASE_ADDRESS */
#define B1_LONG_PREAMBLE_LONG_PREAMBLE_RAM                                     (B1_PHY_TX_BASE_ADDR + 0x00004000) /* MEMORY_MAP_UNIT_10052_BASE_ADDRESS */
#define HYP_TD_ANTENNA_0_BAND_SEL_COEF                                         (B0_PHY_RX_TD_BASE_ADDR + 0x00022000) /* MEMORY_MAP_UNIT_50_BASE_ADDRESS */
#define HYP_TD_ANTENNA_1_BAND_SEL_COEF                                         (B0_PHY_RX_TD_BASE_ADDR + 0x00026000) /* MEMORY_MAP_UNIT_50_BASE_ADDRESS */
#define HYP_TD_ANTENNA_2_BAND_SEL_COEF                                         (B0_PHY_RX_TD_BASE_ADDR + 0x0002A000) /* MEMORY_MAP_UNIT_50_BASE_ADDRESS */
#define HYP_TD_ANTENNA_3_BAND_SEL_COEF                                         (B0_PHY_RX_TD_BASE_ADDR + 0x0002E000) /* MEMORY_MAP_UNIT_50_BASE_ADDRESS */
#define B0_RX_COORDINATOR_FRAME_CLASS_VIO_MEM                                  (B0_FRAME_CLASS_VIO_BUFFER_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_1058_BASE_ADDRESS */
#define B1_RX_COORDINATOR_FRAME_CLASS_VIO_MEM                                  (B1_FRAME_CLASS_VIO_BUFFER_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_11058_BASE_ADDRESS */
#define B0_TX_SELECTOR_VAP_TID_RAM                                             (B0_TX_SELECTOR_BASE_ADDR + 0x00001A00) /* MEMORY_MAP_UNIT_29_BASE_ADDRESS */
#define B1_TX_SELECTOR_VAP_TID_RAM                                             (B1_TX_SELECTOR_BASE_ADDR + 0x00001A00) /* MEMORY_MAP_UNIT_10029_BASE_ADDRESS */
#define IRAM_OCP64_IRAM_0_IRAM                                                 (CPU_IRAM_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_1000_BASE_ADDRESS */
#define IRAM_OCP64_IRAM_1_IRAM                                                 (CPU_IRAM_BASE_ADDR + 0x00040000) /* MEMORY_MAP_UNIT_1000_BASE_ADDRESS */
#define IRAM_OCP64_IRAM_2_IRAM                                                 (CPU_IRAM_BASE_ADDR + 0x00080000) /* MEMORY_MAP_UNIT_1000_BASE_ADDRESS */
#define IRAM_OCP64_IRAM_3_IRAM                                                 (CPU_IRAM_BASE_ADDR + 0x000C0000) /* MEMORY_MAP_UNIT_1000_BASE_ADDRESS */
#define IRAM_OCP64_IRAM_4_IRAM                                                 (CPU_IRAM_BASE_ADDR + 0x00100000) /* MEMORY_MAP_UNIT_1000_BASE_ADDRESS */
#define IRAM_OCP64_IRAM_5_IRAM                                                 (CPU_IRAM_BASE_ADDR + 0x00140000) /* MEMORY_MAP_UNIT_1000_BASE_ADDRESS */
#define MAC_DESCRIPTOR_RAM_MEM0                                                (DESCRIPTOR_RAM_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_1019_BASE_ADDRESS */
#define MAC_DESCRIPTOR_RAM_MEM1                                                (DESCRIPTOR_RAM_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_1019_BASE_ADDRESS */
#define MAC_DESCRIPTOR_RAM_MEM2                                                (DESCRIPTOR_RAM_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_1019_BASE_ADDRESS */
#define MAC_DESCRIPTOR_RAM_MEM3                                                (DESCRIPTOR_RAM_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_1019_BASE_ADDRESS */
#define MAC_DESCRIPTOR_RAM_MEM4                                                (DESCRIPTOR_RAM_BASE_ADDR + 0x00080000) /* MEMORY_MAP_UNIT_1019_BASE_ADDRESS */
#define MAC_DESCRIPTOR_RAM_MEM5                                                (DESCRIPTOR_RAM_BASE_ADDR + 0x00080000) /* MEMORY_MAP_UNIT_1019_BASE_ADDRESS */
#define MAC_DESCRIPTOR_RAM_MEM6                                                (DESCRIPTOR_RAM_BASE_ADDR + 0x00080000) /* MEMORY_MAP_UNIT_1019_BASE_ADDRESS */
#define MAC_DESCRIPTOR_RAM_MEM7                                                (DESCRIPTOR_RAM_BASE_ADDR + 0x00080000) /* MEMORY_MAP_UNIT_1019_BASE_ADDRESS */
#define MAC_DESCRIPTOR_RAM_MEM8                                                (DESCRIPTOR_RAM_BASE_ADDR + 0x00100000) /* MEMORY_MAP_UNIT_1019_BASE_ADDRESS */
#define MAC_DESCRIPTOR_RAM_MEM9                                                (DESCRIPTOR_RAM_BASE_ADDR + 0x00100000) /* MEMORY_MAP_UNIT_1019_BASE_ADDRESS */
#define MAC_DESCRIPTOR_RAM_MEM10                                               (DESCRIPTOR_RAM_BASE_ADDR + 0x00100000) /* MEMORY_MAP_UNIT_1019_BASE_ADDRESS */
#define MAC_DESCRIPTOR_RAM_MEM11                                               (DESCRIPTOR_RAM_BASE_ADDR + 0x00100000) /* MEMORY_MAP_UNIT_1019_BASE_ADDRESS */
#define MAC_DESCRIPTOR_RAM_MEM12                                               (DESCRIPTOR_RAM_BASE_ADDR + 0x00180000) /* MEMORY_MAP_UNIT_1019_BASE_ADDRESS */
#define MAC_DESCRIPTOR_RAM_MEM13                                               (DESCRIPTOR_RAM_BASE_ADDR + 0x00180000) /* MEMORY_MAP_UNIT_1019_BASE_ADDRESS */
#define MAC_DESCRIPTOR_RAM_MEM14                                               (DESCRIPTOR_RAM_BASE_ADDR + 0x00180000) /* MEMORY_MAP_UNIT_1019_BASE_ADDRESS */
#define MAC_DESCRIPTOR_RAM_MEM15                                               (DESCRIPTOR_RAM_BASE_ADDR + 0x00180000) /* MEMORY_MAP_UNIT_1019_BASE_ADDRESS */
#define B1_OTFA_TOP_LOCAL_DB_MEM_I                                             (B1_OTFA_MEM_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_11073_BASE_ADDRESS */
#define HYP_RXFD_SEG_0_SU_HDR_MEMORY                                           (B0_PHY_RX_FD_BASE_ADDR + 0x00003000) /* MEMORY_MAP_UNIT_51_BASE_ADDRESS */
#define HYP_RXFD_SEG_1_SU_HDR_MEMORY                                           (B1_PHY_RX_FD_BASE_ADDR + 0x00003000) /* MEMORY_MAP_UNIT_10051_BASE_ADDRESS */
#define RX_PP_STA_BITMAP_RAM                                                   (RX_PP_BASE_ADDR + 0x00009000) /* MEMORY_MAP_UNIT_1084_BASE_ADDRESS */
#define B0_MAC_ADDR2INDEX_MEM                                                  (B0_MAC_ADDR2INDEX_BASE_ADDR + 0x00000800) /* MEMORY_MAP_UNIT_22_BASE_ADDRESS */
#define B1_AUTO_RESP_USER_DB_MEM                                               (B1_AUTO_RESP_MEMS_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_11053_BASE_ADDRESS */
#define B1_TX_PD_ACC_TXPD_LOCAL_CONTEXT_MEM                                    (B1_TX_PD_ACC_MEM_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_11097_BASE_ADDRESS */
#define RX_PP_PN_SN_RAM                                                        (RX_PP_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_1084_BASE_ADDRESS */
#define HYP_RXFD_SHARED_MU_PHASES_MEMORY_0                                     (B0_PHY_RX_FD_BASE_ADDR + 0x00018000) /* MEMORY_MAP_UNIT_51_BASE_ADDRESS */
#define HYP_RXFD_SHARED_MU_PHASES_MEMORY_1                                     (B0_PHY_RX_FD_BASE_ADDR + 0x00018000) /* MEMORY_MAP_UNIT_51_BASE_ADDRESS */
#define B0_MAC_GEN_RISC_RAMS_GEN_RISC_RX_IRAM                                  (B0_MAC_GENRISC_RX_IRAM_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_1031_BASE_ADDRESS */
#define B1_MAC_GEN_RISC_RAMS_GEN_RISC_RX_IRAM                                  (B1_MAC_GENRISC_RX_IRAM_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_11031_BASE_ADDRESS */
#define B0_GEN_RISC_GEN_RISC_SRAM                                              (B0_PHY_RX_TD_BASE_ADDR + 0x00004000) /* MEMORY_MAP_UNIT_50_BASE_ADDRESS */
#define B1_GEN_RISC_GEN_RISC_SRAM                                              (B1_PHY_RX_TD_BASE_ADDR + 0x00004000) /* MEMORY_MAP_UNIT_10050_BASE_ADDRESS */
#define MAC_SHARED_RAM_MEM0                                                    (SHARED_RAM_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_1018_BASE_ADDRESS */
#define MAC_SHARED_RAM_MEM1                                                    (SHARED_RAM_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_1018_BASE_ADDRESS */
#define MAC_SHARED_RAM_MEM2                                                    (SHARED_RAM_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_1018_BASE_ADDRESS */
#define MAC_SHARED_RAM_MEM3                                                    (SHARED_RAM_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_1018_BASE_ADDRESS */
#define RETRY_POINTER_DB_DUAL_BAND_RAM                                         (RETRY_POINTER_DB_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_1068_BASE_ADDRESS */
#define B0_RXC_RXC_FIELDS_CATCHER_MEM                                          (B0_RXF_MEM_BASE_ADDR + 0x00000400) /* MEMORY_MAP_UNIT_1052_BASE_ADDRESS */
#define B1_RXC_RXC_FIELDS_CATCHER_MEM                                          (B1_RXF_MEM_BASE_ADDR + 0x00000400) /* MEMORY_MAP_UNIT_11052_BASE_ADDRESS */
#define PROT_DB_SMC_LOCK_MEM                                                   (PROT_DB_SHADOW_MEM_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_1015_BASE_ADDRESS */
#define HYP_RXFD_SEG_0_MU_HDR_MEMORY                                           (B0_PHY_RX_FD_BASE_ADDR + 0x00002C00) /* MEMORY_MAP_UNIT_51_BASE_ADDRESS */
#define HYP_RXFD_SEG_1_MU_HDR_MEMORY                                           (B1_PHY_RX_FD_BASE_ADDR + 0x00002C00) /* MEMORY_MAP_UNIT_10051_BASE_ADDRESS */
#define HYP_RXFD_SHARED_SU_PHASES_MEMORY_LO_0                                  (B0_PHY_RX_FD_BASE_ADDR + 0x00020000) /* MEMORY_MAP_UNIT_51_BASE_ADDRESS */
#define HYP_RXFD_SHARED_SU_PHASES_MEMORY_LO_1                                  (B0_PHY_RX_FD_BASE_ADDR + 0x00020000) /* MEMORY_MAP_UNIT_51_BASE_ADDRESS */
#define DMAC_WRAPPER_U_LL_MEM                                                  (WLAN_ARM_LL_MEM_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_1008_BASE_ADDRESS */
#define HOST_IF_GEN_RISC_HOST_IRAM                                             (MAC_GENRISC_HOST_IRAM_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_1004_BASE_ADDRESS */
#define TRAFFIC_INDICATOR_MEM                                                  (TRAFFIC_INDICATOR_BASE_ADDR + 0x00000400) /* MEMORY_MAP_UNIT_1054_BASE_ADDRESS */
#define B0_TX_SELECTOR_STA_TID_RAM0                                            (B0_TX_SELECTOR_BASE_ADDR + 0x00000A00) /* MEMORY_MAP_UNIT_29_BASE_ADDRESS */
#define B0_TX_SELECTOR_STA_TID_RAM1                                            (B0_TX_SELECTOR_BASE_ADDR + 0x00000A00) /* MEMORY_MAP_UNIT_29_BASE_ADDRESS */
#define HOST_IF_ACC_FW_CODE_RAM                                                (HOST_IF_ACC_BASE_ADDR + 0x00000800) /* MEMORY_MAP_UNIT_39_BASE_ADDRESS */
#define B1_MAC_ADDR2INDEX_MEM                                                  (B1_MAC_ADDR2INDEX_BASE_ADDR + 0x00000800) /* MEMORY_MAP_UNIT_10022_BASE_ADDRESS */
#define B0_TX_SELECTOR_TX_SEL_VHT_MU_GRP_RAM                                   (B0_TX_SELECTOR_BASE_ADDR + 0x00000400) /* MEMORY_MAP_UNIT_29_BASE_ADDRESS */
#define B1_TX_SELECTOR_TX_SEL_VHT_MU_GRP_RAM                                   (B1_TX_SELECTOR_BASE_ADDR + 0x00000400) /* MEMORY_MAP_UNIT_10029_BASE_ADDRESS */
#define B0_TX_SELECTOR_TX_SEL_VHT_MU_STA_RAM                                   (B0_TX_SELECTOR_BASE_ADDR + 0x00000800) /* MEMORY_MAP_UNIT_29_BASE_ADDRESS */
#define B0_RXF_RXF_MEM                                                         (B0_RXF_MEM_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_1052_BASE_ADDRESS */
#define B1_RXF_RXF_MEM                                                         (B1_RXF_MEM_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_11052_BASE_ADDRESS */
#define B0_TF_GENERATOR_TF_GEN_RU_INDEX_MEM                                    (B0_TF_GENERATOR_MEM_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_1085_BASE_ADDRESS */
#define B1_TF_GENERATOR_TF_GEN_RU_INDEX_MEM                                    (B1_TF_GENERATOR_MEM_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_11085_BASE_ADDRESS */
#define HYP_TD_ANTENNA_0_TPC_POUT                                              (B0_PHY_TX_BASE_ADDR + 0x00022000) /* MEMORY_MAP_UNIT_52_BASE_ADDRESS */
#define HYP_TD_ANTENNA_1_TPC_POUT                                              (B0_PHY_TX_BASE_ADDR + 0x00026000) /* MEMORY_MAP_UNIT_52_BASE_ADDRESS */
#define HYP_TD_ANTENNA_2_TPC_POUT                                              (B0_PHY_TX_BASE_ADDR + 0x0002A000) /* MEMORY_MAP_UNIT_52_BASE_ADDRESS */
#define HYP_TD_ANTENNA_3_TPC_POUT                                              (B0_PHY_TX_BASE_ADDR + 0x0002E000) /* MEMORY_MAP_UNIT_52_BASE_ADDRESS */
#define HYP_TD_ANTENNA_0_TPC_GAIN                                              (B0_PHY_TX_BASE_ADDR + 0x00022400) /* MEMORY_MAP_UNIT_52_BASE_ADDRESS */
#define HYP_TD_ANTENNA_1_TPC_GAIN                                              (B0_PHY_TX_BASE_ADDR + 0x00026400) /* MEMORY_MAP_UNIT_52_BASE_ADDRESS */
#define HYP_TD_ANTENNA_2_TPC_GAIN                                              (B0_PHY_TX_BASE_ADDR + 0x0002A400) /* MEMORY_MAP_UNIT_52_BASE_ADDRESS */
#define HYP_TD_ANTENNA_3_TPC_GAIN                                              (B0_PHY_TX_BASE_ADDR + 0x0002E400) /* MEMORY_MAP_UNIT_52_BASE_ADDRESS */
#define HYP_TD_ANTENNA_0_TPC_TSSI                                              (B0_PHY_TX_BASE_ADDR + 0x00022200) /* MEMORY_MAP_UNIT_52_BASE_ADDRESS */
#define HYP_TD_ANTENNA_1_TPC_TSSI                                              (B0_PHY_TX_BASE_ADDR + 0x00026200) /* MEMORY_MAP_UNIT_52_BASE_ADDRESS */
#define HYP_TD_ANTENNA_2_TPC_TSSI                                              (B0_PHY_TX_BASE_ADDR + 0x0002A200) /* MEMORY_MAP_UNIT_52_BASE_ADDRESS */
#define HYP_TD_ANTENNA_3_TPC_TSSI                                              (B0_PHY_TX_BASE_ADDR + 0x0002E200) /* MEMORY_MAP_UNIT_52_BASE_ADDRESS */
#define HYP_TD_ANTENNA_0_AGC_GSP_TABLE_RAM                                     (B0_PHY_RX_TD_BASE_ADDR + 0x00023400) /* MEMORY_MAP_UNIT_50_BASE_ADDRESS */
#define HYP_TD_ANTENNA_1_AGC_GSP_TABLE_RAM                                     (B0_PHY_RX_TD_BASE_ADDR + 0x00027400) /* MEMORY_MAP_UNIT_50_BASE_ADDRESS */
#define HYP_TD_ANTENNA_2_AGC_GSP_TABLE_RAM                                     (B0_PHY_RX_TD_BASE_ADDR + 0x0002b400) /* MEMORY_MAP_UNIT_50_BASE_ADDRESS */
#define HYP_TD_ANTENNA_3_AGC_GSP_TABLE_RAM                                     (B0_PHY_RX_TD_BASE_ADDR + 0x0002f400) /* MEMORY_MAP_UNIT_50_BASE_ADDRESS */
#define HYP_TD_ANTENNA_0_AGC_DC_OFFSET_RAM                                     (B0_PHY_RX_TD_BASE_ADDR + 0x00023800) /* MEMORY_MAP_UNIT_50_BASE_ADDRESS */
#define HYP_TD_ANTENNA_1_AGC_DC_OFFSET_RAM                                     (B0_PHY_RX_TD_BASE_ADDR + 0x00027800) /* MEMORY_MAP_UNIT_50_BASE_ADDRESS */
#define HYP_TD_ANTENNA_2_AGC_DC_OFFSET_RAM                                     (B0_PHY_RX_TD_BASE_ADDR + 0x0002b800) /* MEMORY_MAP_UNIT_50_BASE_ADDRESS */
#define HYP_TD_ANTENNA_3_AGC_DC_OFFSET_RAM                                     (B0_PHY_RX_TD_BASE_ADDR + 0x0002f800) /* MEMORY_MAP_UNIT_50_BASE_ADDRESS */
#define HYP_TD_ANTENNA_0_AGC_RX_FILTER_RAM                                     (B0_PHY_RX_TD_BASE_ADDR + 0x00023e00) /* MEMORY_MAP_UNIT_50_BASE_ADDRESS */
#define HYP_TD_ANTENNA_1_AGC_RX_FILTER_RAM                                     (B0_PHY_RX_TD_BASE_ADDR + 0x00027e00) /* MEMORY_MAP_UNIT_50_BASE_ADDRESS */
#define HYP_TD_ANTENNA_2_AGC_RX_FILTER_RAM                                     (B0_PHY_RX_TD_BASE_ADDR + 0x0002be00) /* MEMORY_MAP_UNIT_50_BASE_ADDRESS */
#define HYP_TD_ANTENNA_3_AGC_RX_FILTER_RAM                                     (B0_PHY_RX_TD_BASE_ADDR + 0x0002fe00) /* MEMORY_MAP_UNIT_50_BASE_ADDRESS */
#define B0_DUR_AUTO_RATE_DUR_MEM                                               (B0_DUR_AUTO_RATE_MEM_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_1055_BASE_ADDRESS */
#define B1_DUR_AUTO_RATE_DUR_MEM                                               (B1_DUR_AUTO_RATE_MEM_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_11055_BASE_ADDRESS */
#define Q_MANAGER_TX_MPDU_DESC0_DLMS_RAM                                       (DLM_BASE_ADDR + 0x0000A848) /* MEMORY_MAP_UNIT_40_BASE_ADDRESS */
#define Q_MANAGER_TX_MPDU_DESC1_DLMS_RAM                                       (DLM_BASE_ADDR + 0x0000AA18) /* MEMORY_MAP_UNIT_40_BASE_ADDRESS */
#define MAC_TX_SHARED_RAM_MEM2_0                                               (B0_TX_RECIPES_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_1023_BASE_ADDRESS */
#define MAC_TX_SHARED_RAM_MEM2_1                                               (B0_TX_RECIPES_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_1023_BASE_ADDRESS */
#define MAC_TX_SHARED_RAM_MEM2_2                                               (B0_TX_RECIPES_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_1023_BASE_ADDRESS */
#define MAC_TX_SHARED_RAM_MEM2_3                                               (B0_TX_RECIPES_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_1023_BASE_ADDRESS */
#define RX_PP_RX_PP_STATISTICS_RAM                                             (RX_PP_COUNTERS_BASE_ADDR + 0x00000700) /* MEMORY_MAP_UNIT_64_BASE_ADDRESS */
#define B0_MAC_GEN_RISC_RAMS_GEN_RISC_RX_SPRAM                                 (B0_MAC_GENRISC_RX_SPRAM_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_1033_BASE_ADDRESS */
#define B0_MAC_GEN_RISC_RAMS_GEN_RISC_TX_SPRAM                                 (B0_MAC_GENRISC_TX_SPRAM_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_1026_BASE_ADDRESS */
#define B1_MAC_GEN_RISC_RAMS_GEN_RISC_RX_SPRAM                                 (B1_MAC_GENRISC_RX_SPRAM_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_11033_BASE_ADDRESS */
#define B1_MAC_GEN_RISC_RAMS_GEN_RISC_TX_SPRAM                                 (B1_MAC_GENRISC_TX_SPRAM_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_11026_BASE_ADDRESS */
#define B0_MU_EFF_RATE_DATABASE_MU_TRAIN_EFFECTIVE_RATE_DATABASE               (B0_PHY_RX_FD_BASE_ADDR + 0x00006000) /* MEMORY_MAP_UNIT_51_BASE_ADDRESS */
#define B1_MU_EFF_RATE_DATABASE_MU_TRAIN_EFFECTIVE_RATE_DATABASE               (B1_PHY_RX_FD_BASE_ADDR + 0x00006000) /* MEMORY_MAP_UNIT_10051_BASE_ADDRESS */
#define HYP_TD_ANTENNA_0_RXTD_FIFO_1                                           (B0_PHY_RX_TD_BASE_ADDR + 0x0001a000) /* MEMORY_MAP_UNIT_50_BASE_ADDRESS */
#define HYP_TD_ANTENNA_1_RXTD_FIFO_1                                           (B0_PHY_RX_TD_BASE_ADDR + 0x0001a000) /* MEMORY_MAP_UNIT_50_BASE_ADDRESS */
#define HYP_TD_ANTENNA_2_RXTD_FIFO_1                                           (B0_PHY_RX_TD_BASE_ADDR + 0x0001a000) /* MEMORY_MAP_UNIT_50_BASE_ADDRESS */
#define HYP_TD_ANTENNA_3_RXTD_FIFO_1                                           (B0_PHY_RX_TD_BASE_ADDR + 0x0001a000) /* MEMORY_MAP_UNIT_50_BASE_ADDRESS */
#define B0_MAC_BFM_MBFM_TASK_SW_DB_L                                           (B0_MAC_BFM_BASE_ADDR + 0x00000400) /* MEMORY_MAP_UNIT_11_BASE_ADDRESS */
#define B1_MAC_BFM_MBFM_TASK_SW_DB_L                                           (B0_MAC_BFM_BASE_ADDR + 0x00000400) /* MEMORY_MAP_UNIT_11_BASE_ADDRESS */
#define HYP_TD_ANTENNA_0_CAL_RAM                                               (B0_PHY_TX_BASE_ADDR + 0x00022800) /* MEMORY_MAP_UNIT_52_BASE_ADDRESS */
#define HYP_TD_ANTENNA_1_CAL_RAM                                               (B0_PHY_TX_BASE_ADDR + 0x00026800) /* MEMORY_MAP_UNIT_52_BASE_ADDRESS */
#define HYP_TD_ANTENNA_2_CAL_RAM                                               (B0_PHY_TX_BASE_ADDR + 0x0002A800) /* MEMORY_MAP_UNIT_52_BASE_ADDRESS */
#define HYP_TD_ANTENNA_3_CAL_RAM                                               (B0_PHY_TX_BASE_ADDR + 0x0002E800) /* MEMORY_MAP_UNIT_52_BASE_ADDRESS */
#define B0_FILTER_SELECT_FILTER_RAM                                            (B0_PHY_RX_FD_BASE_ADDR + 0x00004000) /* MEMORY_MAP_UNIT_51_BASE_ADDRESS */
#define B1_FILTER_SELECT_FILTER_RAM                                            (B1_PHY_RX_FD_BASE_ADDR + 0x00004000) /* MEMORY_MAP_UNIT_10051_BASE_ADDRESS */
#define B0_MCS_SNR_MCS_SNR_RAM                                                 (B0_PHY_RX_FD_BASE_ADDR + 0x00002400) /* MEMORY_MAP_UNIT_51_BASE_ADDRESS */
#define B1_MCS_SNR_MCS_SNR_RAM                                                 (B1_PHY_RX_FD_BASE_ADDR + 0x00002400) /* MEMORY_MAP_UNIT_10051_BASE_ADDRESS */
#define HOST_IF_ACC_HOST_IF_ACC_QOS_RAM                                        (HOST_IF_COUNTERS_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_63_BASE_ADDRESS */
#define HOST_IF_ACC_HOST_IF_ACC_STATISTICS_RAM                                 (HOST_IF_COUNTERS_BASE_ADDR + 0x00004200) /* MEMORY_MAP_UNIT_63_BASE_ADDRESS */
#define HYP_RXFD_SHARED_SU_PHASES_MEMORY_HI_0                                  (B0_PHY_RX_FD_BASE_ADDR + 0x00030000) /* MEMORY_MAP_UNIT_51_BASE_ADDRESS */
#define HYP_RXFD_SHARED_SU_PHASES_MEMORY_HI_1                                  (B0_PHY_RX_FD_BASE_ADDR + 0x00030000) /* MEMORY_MAP_UNIT_51_BASE_ADDRESS */
#define HYP_RXTD_SHARED_TEST_BUS_FIFO_0                                        (B0_PHY_RX_BE_BASE_ADDR + 0x00020000) /* MEMORY_MAP_UNIT_49_BASE_ADDRESS */
#define HYP_RXTD_SHARED_TEST_BUS_FIFO_1                                        (B1_PHY_RX_BE_BASE_ADDR + 0x00020000) /* MEMORY_MAP_UNIT_10049_BASE_ADDRESS */
#define B0_BAA_TOP_RA_USER_MEM                                                 (B0_BA_ANALYZER_BASE_ADDR + 0x00000800) /* MEMORY_MAP_UNIT_45_BASE_ADDRESS */
#define B1_BAA_TOP_RA_USER_MEM                                                 (B1_BA_ANALYZER_BASE_ADDR + 0x00000800) /* MEMORY_MAP_UNIT_10045_BASE_ADDRESS */
#define BSRC_CNT_MEM_COMMON_BAND_RAM                                           (BSRC_CNT_MEM_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_67_BASE_ADDRESS */
#define BSRC_AGER_MEM_COMMON_BAND_RAM                                          (BSRC_AGER_MEM_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_1017_BASE_ADDRESS */
#define BAA_STATISTICS_CNT_MEM_COMMON_BAND_RAM                                 (BA_ANALYZER_COUNTER_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_1049_BASE_ADDRESS */
#define B0_PRE_AGG_TOP_PARAMS_CALC_MEM                                         (B0_PRE_AGG_BASE_ADDR + 0x00000800) /* MEMORY_MAP_UNIT_21_BASE_ADDRESS */
#define B1_PRE_AGG_TOP_PARAMS_CALC_MEM                                         (B1_PRE_AGG_BASE_ADDR + 0x00000800) /* MEMORY_MAP_UNIT_10021_BASE_ADDRESS */
#define B0_TX_SELECTOR_TX_SEL_TWT_SP_GROUP_STA_RAM                             (B0_TX_SELECTOR_BASE_ADDR + 0x00001C90) /* MEMORY_MAP_UNIT_29_BASE_ADDRESS */
#define B0_TX_SELECTOR_TX_SEL_TWT_SP_GROUP_COUNT_RAM                           (B0_TX_SELECTOR_BASE_ADDR + 0x00002090) /* MEMORY_MAP_UNIT_29_BASE_ADDRESS */
#define B1_TX_SELECTOR_TX_SEL_TWT_SP_GROUP_STA_RAM                             (B1_TX_SELECTOR_BASE_ADDR + 0x00001C90) /* MEMORY_MAP_UNIT_10029_BASE_ADDRESS */
#define B1_TX_SELECTOR_TX_SEL_TWT_SP_GROUP_COUNT_RAM                           (B1_TX_SELECTOR_BASE_ADDR + 0x00002090) /* MEMORY_MAP_UNIT_10029_BASE_ADDRESS */
#define RX_PP_RX_PP_COOR_STATISTICS_RAM                                        (RX_PP_COUNTERS_BASE_ADDR + 0x00000000) /* MEMORY_MAP_UNIT_64_BASE_ADDRESS */
#define HYP_TD_ANTENNA_0_IQ_PARAMS_LUT                                         (B0_PHY_RX_TD_BASE_ADDR + 0x00022400) /* MEMORY_MAP_UNIT_50_BASE_ADDRESS */
#define HYP_TD_ANTENNA_0_IQ_FIREQ_LUT                                          (B0_PHY_RX_TD_BASE_ADDR + 0x00022800) /* MEMORY_MAP_UNIT_50_BASE_ADDRESS */
#define HYP_TD_ANTENNA_1_IQ_PARAMS_LUT                                         (B0_PHY_RX_TD_BASE_ADDR + 0x00026400) /* MEMORY_MAP_UNIT_50_BASE_ADDRESS */
#define HYP_TD_ANTENNA_1_IQ_FIREQ_LUT                                          (B0_PHY_RX_TD_BASE_ADDR + 0x00026800) /* MEMORY_MAP_UNIT_50_BASE_ADDRESS */
#define HYP_TD_ANTENNA_2_IQ_PARAMS_LUT                                         (B0_PHY_RX_TD_BASE_ADDR + 0x0002A400) /* MEMORY_MAP_UNIT_50_BASE_ADDRESS */
#define HYP_TD_ANTENNA_2_IQ_FIREQ_LUT                                          (B0_PHY_RX_TD_BASE_ADDR + 0x0002A800) /* MEMORY_MAP_UNIT_50_BASE_ADDRESS */
#define HYP_TD_ANTENNA_3_IQ_PARAMS_LUT                                         (B0_PHY_RX_TD_BASE_ADDR + 0x0002E400) /* MEMORY_MAP_UNIT_50_BASE_ADDRESS */
#define HYP_TD_ANTENNA_3_IQ_FIREQ_LUT                                          (B0_PHY_RX_TD_BASE_ADDR + 0x0002E800) /* MEMORY_MAP_UNIT_50_BASE_ADDRESS */
#define HYP_RXFD_SHARED_HE_STF_NOISE_GAIN_CALC_HE_STF_NOISE_GAIN_CALC_RAM0     (B0_PHY_RX_FD_BASE_ADDR + 0x00002D00) /* MEMORY_MAP_UNIT_51_BASE_ADDRESS */
#define HYP_RXFD_SHARED_HE_STF_NOISE_GAIN_CALC_HE_STF_NOISE_GAIN_CALC_RAM1     (B0_PHY_RX_FD_BASE_ADDR + 0x00002D40) /* MEMORY_MAP_UNIT_51_BASE_ADDRESS */
#define HYP_RXFD_SHARED_HE_STF_NOISE_GAIN_CALC_HE_STF_NOISE_GAIN_CALC_RAM2     (B0_PHY_RX_FD_BASE_ADDR + 0x00002D80) /* MEMORY_MAP_UNIT_51_BASE_ADDRESS */
#define HYP_RXFD_SHARED_HE_STF_NOISE_GAIN_CALC_HE_STF_NOISE_GAIN_CALC_RAM3     (B0_PHY_RX_FD_BASE_ADDR + 0x00002DC0) /* MEMORY_MAP_UNIT_51_BASE_ADDRESS */

#endif /* _MEMORY_MAP_H_*/
