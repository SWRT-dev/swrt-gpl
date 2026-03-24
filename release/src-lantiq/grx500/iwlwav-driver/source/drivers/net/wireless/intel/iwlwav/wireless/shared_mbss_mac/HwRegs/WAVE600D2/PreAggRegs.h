
/***********************************************************************************
File:				PreAggRegs.h
Module:				preAgg
SOC Revision:		
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _PRE_AGG_REGS_H_
#define _PRE_AGG_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define PRE_AGG_BASE_ADDRESS                             MEMORY_MAP_UNIT_21_BASE_ADDRESS
#define	REG_PRE_AGG_PRE_AGG_ENABLE                               (PRE_AGG_BASE_ADDRESS + 0x0)
#define	REG_PRE_AGG_PACKET_DENSITY_BYTE_OVERHEAD                 (PRE_AGG_BASE_ADDRESS + 0x4)
#define	REG_PRE_AGG_TAIL_BIT_LENGTH_OVERHEAD1                    (PRE_AGG_BASE_ADDRESS + 0x8)
#define	REG_PRE_AGG_TAIL_BIT_LENGTH_OVERHEAD2                    (PRE_AGG_BASE_ADDRESS + 0xC)
#define	REG_PRE_AGG_PHY_PREAMBLE_DURATION_OVERHEAD               (PRE_AGG_BASE_ADDRESS + 0x10)
#define	REG_PRE_AGG_PHY_PREAMBLE_DURATION_OVERHEAD_11B           (PRE_AGG_BASE_ADDRESS + 0x14)
#define	REG_PRE_AGG_RTS_CTS2SELF_METHOD                          (PRE_AGG_BASE_ADDRESS + 0x18)
#define	REG_PRE_AGG_MAX_PSDU_SIZE_HT                             (PRE_AGG_BASE_ADDRESS + 0x1C)
#define	REG_PRE_AGG_MAX_PSDU_SIZE_VHT                            (PRE_AGG_BASE_ADDRESS + 0x20)
#define	REG_PRE_AGG_MAX_PSDU_SIZE_HE                             (PRE_AGG_BASE_ADDRESS + 0x24)
#define	REG_PRE_AGG_MAX_PSDU_TX_TIME_LIMIT                       (PRE_AGG_BASE_ADDRESS + 0x28)
#define	REG_PRE_AGG_PRE_AGG_TCR_MAC_DUR_EN                       (PRE_AGG_BASE_ADDRESS + 0x2C)
#define	REG_PRE_AGG_PRE_AGG_DEBUG_STATUS                         (PRE_AGG_BASE_ADDRESS + 0x30)
#define	REG_PRE_AGG_MU_GRP_DB_ADDR                               (PRE_AGG_BASE_ADDRESS + 0x40)
#define	REG_PRE_AGG_AGG_CONFIG_DB                                (PRE_AGG_BASE_ADDRESS + 0x44)
#define	REG_PRE_AGG_AGG_CONFIG_DB_4B_OFFSET                      (PRE_AGG_BASE_ADDRESS + 0x48)
#define	REG_PRE_AGG_TCR_BASE_ADDR1                               (PRE_AGG_BASE_ADDRESS + 0x4C)
#define	REG_PRE_AGG_TCR_BASE_ADDR2                               (PRE_AGG_BASE_ADDRESS + 0x50)
#define	REG_PRE_AGG_RCR_BASE_ADDR                                (PRE_AGG_BASE_ADDRESS + 0x54)
#define	REG_PRE_AGG_PRE_AGG_ABORT_PULSE                          (PRE_AGG_BASE_ADDRESS + 0x60)
#define	REG_PRE_AGG_PRE_AGG_TIMER_MIN_MAX                        (PRE_AGG_BASE_ADDRESS + 0x64)
#define	REG_PRE_AGG_PRE_AGG_TIMER                                (PRE_AGG_BASE_ADDRESS + 0x68)
#define	REG_PRE_AGG_ZERO_TIMER_THRESHOLD                         (PRE_AGG_BASE_ADDRESS + 0x6C)
#define	REG_PRE_AGG_PRE_AGG_STATUS_IRQ                           (PRE_AGG_BASE_ADDRESS + 0x70)
#define	REG_PRE_AGG_PRE_AGG_STATUS_IRQ_CLR                       (PRE_AGG_BASE_ADDRESS + 0x74)
#define	REG_PRE_AGG_PRE_AGG_STATUS_IRQ_MASK                      (PRE_AGG_BASE_ADDRESS + 0x78)
#define	REG_PRE_AGG_OVERRIDE_DB_PARAMETERS                       (PRE_AGG_BASE_ADDRESS + 0x7C)
#define	REG_PRE_AGG_FORCE_RTS_CTS_VAP                            (PRE_AGG_BASE_ADDRESS + 0x84)
#define	REG_PRE_AGG_FORCE_CTS2SELF_VAP                           (PRE_AGG_BASE_ADDRESS + 0x88)
#define	REG_PRE_AGG_RTS_CTS2SELF_CONTROL                         (PRE_AGG_BASE_ADDRESS + 0x8C)
#define	REG_PRE_AGG_BF_IMPLICIT_FILLER_PHY_MODE                  (PRE_AGG_BASE_ADDRESS + 0x90)
#define	REG_PRE_AGG_PREAMBLE_11B_TYPE_CONTROL                    (PRE_AGG_BASE_ADDRESS + 0x94)
#define	REG_PRE_AGG_PREAMBLE_11B_TYPE_VAP                        (PRE_AGG_BASE_ADDRESS + 0x98)
#define	REG_PRE_AGG_USE_ONLY_CCA_20S_PWR_PHY_MODE_HT             (PRE_AGG_BASE_ADDRESS + 0x9C)
#define	REG_PRE_AGG_TX_STICKY_CCA_FREE                           (PRE_AGG_BASE_ADDRESS + 0xA0)
#define	REG_PRE_AGG_RD_DB_TCRS_DATA_802_11_PD_TYPE               (PRE_AGG_BASE_ADDRESS + 0xA4)
#define	REG_PRE_AGG_BW_LIMIT_VAP_15_0                            (PRE_AGG_BASE_ADDRESS + 0xA8)
#define	REG_PRE_AGG_BW_LIMIT_VAP_31_16                           (PRE_AGG_BASE_ADDRESS + 0xAC)
#define	REG_PRE_AGG_MU_MULT_USRS_TRAINING_CONTROL                (PRE_AGG_BASE_ADDRESS + 0xB0)
#define	REG_PRE_AGG_MU_BAR_BACK_AIR_TIME_DURATION                (PRE_AGG_BASE_ADDRESS + 0xB4)
#define	REG_PRE_AGG_MU_MAX_NSTS                                  (PRE_AGG_BASE_ADDRESS + 0xB8)
#define	REG_PRE_AGG_TXOP_AIR_TIME_DURATION                       (PRE_AGG_BASE_ADDRESS + 0x100)
#define	REG_PRE_AGG_BF_SEQ_AIR_TIME_DURATION                     (PRE_AGG_BASE_ADDRESS + 0x104)
#define	REG_PRE_AGG_TX_DUR_CALC_CONTROL                          (PRE_AGG_BASE_ADDRESS + 0x108)
#define	REG_PRE_AGG_TX_SEQ_AIR_TIME_DUR                          (PRE_AGG_BASE_ADDRESS + 0x10C)
#define	REG_PRE_AGG_NO_PSDU_TIME_LIMIT_11B_EN                    (PRE_AGG_BASE_ADDRESS + 0x110)
#define	REG_PRE_AGG_BF_RPT_ANALYZER_DELTA_MCS_NSS                (PRE_AGG_BASE_ADDRESS + 0x114)
#define	REG_PRE_AGG_BF_RPT_ANALYZER_SNR_THRESHOLD_MCS0TO3        (PRE_AGG_BASE_ADDRESS + 0x118)
#define	REG_PRE_AGG_BF_RPT_ANALYZER_SNR_THRESHOLD_MCS4TO7        (PRE_AGG_BASE_ADDRESS + 0x11C)
#define	REG_PRE_AGG_BF_RPT_ANALYZER_SNR_THRESHOLD_MCS8TO9        (PRE_AGG_BASE_ADDRESS + 0x120)
#define	REG_PRE_AGG_RX_BF_RPT_MCS_NSS_PARAMETERS                 (PRE_AGG_BASE_ADDRESS + 0x124)
#define	REG_PRE_AGG_RX_BF_RPT_SNR                                (PRE_AGG_BASE_ADDRESS + 0x128)
#define	REG_PRE_AGG_RX_BF_RPT_SNR_STREAM0                        (PRE_AGG_BASE_ADDRESS + 0x12C)
#define	REG_PRE_AGG_RX_BF_RPT_SNR_STREAM1                        (PRE_AGG_BASE_ADDRESS + 0x130)
#define	REG_PRE_AGG_RX_BF_RPT_SNR_STREAM2                        (PRE_AGG_BASE_ADDRESS + 0x134)
#define	REG_PRE_AGG_RX_BF_RPT_SNR_STREAM3                        (PRE_AGG_BASE_ADDRESS + 0x138)
#define	REG_PRE_AGG_PRE_AGG_LOGGER                               (PRE_AGG_BASE_ADDRESS + 0x140)
#define	REG_PRE_AGG_PRE_AGG_LOGGER_ACTIVE                        (PRE_AGG_BASE_ADDRESS + 0x144)
#define	REG_PRE_AGG_PRE_TX_CALIBRATION_IND_EN                    (PRE_AGG_BASE_ADDRESS + 0x148)
#define	REG_PRE_AGG_PRE_TX_CALIBRATION_IND_SET                   (PRE_AGG_BASE_ADDRESS + 0x14C)
#define	REG_PRE_AGG_FIRST_PD_POINTER                             (PRE_AGG_BASE_ADDRESS + 0x198)
#define	REG_PRE_AGG_USP_LOCK                                     (PRE_AGG_BASE_ADDRESS + 0x1C4)
#define	REG_PRE_AGG_TX_SEL_RESULT                                (PRE_AGG_BASE_ADDRESS + 0x1C8)
#define	REG_PRE_AGG_TX_SEL_USP0_RESULT1                          (PRE_AGG_BASE_ADDRESS + 0x1CC)
#define	REG_PRE_AGG_TX_SEL_USP0_RESULT2                          (PRE_AGG_BASE_ADDRESS + 0x1D0)
#define	REG_PRE_AGG_TX_SEL_USP1_RESULT1                          (PRE_AGG_BASE_ADDRESS + 0x1D4)
#define	REG_PRE_AGG_TX_SEL_USP1_RESULT2                          (PRE_AGG_BASE_ADDRESS + 0x1D8)
#define	REG_PRE_AGG_TX_SEL_USP2_RESULT1                          (PRE_AGG_BASE_ADDRESS + 0x1DC)
#define	REG_PRE_AGG_TX_SEL_USP2_RESULT2                          (PRE_AGG_BASE_ADDRESS + 0x1F0)
#define	REG_PRE_AGG_TX_SEL_USP3_RESULT1                          (PRE_AGG_BASE_ADDRESS + 0x1F4)
#define	REG_PRE_AGG_TX_SEL_USP3_RESULT2                          (PRE_AGG_BASE_ADDRESS + 0x1F8)
#define	REG_PRE_AGG_MU_USP_SECONDARY_NO_TX_REASON                (PRE_AGG_BASE_ADDRESS + 0x220)
#define	REG_PRE_AGG_PRE_AGG_DEBUG_SM                             (PRE_AGG_BASE_ADDRESS + 0x230)
#define	REG_PRE_AGG_PRE_AGG_DEBUG_SM1                            (PRE_AGG_BASE_ADDRESS + 0x234)
#define	REG_PRE_AGG_BW_2_SUB_BAND_MAPPING                        (PRE_AGG_BASE_ADDRESS + 0x238)
#define	REG_PRE_AGG_TAIL_BIT_LENGTH_OVERHEAD1_AX                 (PRE_AGG_BASE_ADDRESS + 0x23C)
#define	REG_PRE_AGG_TAIL_BIT_LENGTH_OVERHEAD2_AX                 (PRE_AGG_BASE_ADDRESS + 0x240)
#define	REG_PRE_AGG_PHY_PREAMBLE_DURATION_OVERHEAD_AX            (PRE_AGG_BASE_ADDRESS + 0x244)
#define	REG_PRE_AGG_PSDU_BYTE_LEN_CALC_PARAMS                    (PRE_AGG_BASE_ADDRESS + 0x248)
#define	REG_PRE_AGG_BAA_PROCESS_TIME                             (PRE_AGG_BASE_ADDRESS + 0x24C)
#define	REG_PRE_AGG_HE_OMI_CONFIGURATION                         (PRE_AGG_BASE_ADDRESS + 0x250)
#define	REG_PRE_AGG_SU_DURATION_UPDATE_CONFIG                    (PRE_AGG_BASE_ADDRESS + 0x254)
#define	REG_PRE_AGG_VHT_DURATION_CONFIG                          (PRE_AGG_BASE_ADDRESS + 0x258)
#define	REG_PRE_AGG_HE_SU_UNSPECIFIED_DURATION_VAP               (PRE_AGG_BASE_ADDRESS + 0x25C)
#define	REG_PRE_AGG_BAR_WIN_SIZE_CONFIG                          (PRE_AGG_BASE_ADDRESS + 0x260)
#define	REG_PRE_AGG_HE_MU_PROTECTION_SOUNDING_PHASE_DB_CONFIG    (PRE_AGG_BASE_ADDRESS + 0x264)
#define	REG_PRE_AGG_HE_MU_DL_UL_DATA_PHASE_DB_CONFIG             (PRE_AGG_BASE_ADDRESS + 0x268)
#define	REG_PRE_AGG_HE_NFRP_PHASE_DB_CONFIG                      (PRE_AGG_BASE_ADDRESS + 0x26C)
#define	REG_PRE_AGG_HE_PLAN_DB_POINTER                           (PRE_AGG_BASE_ADDRESS + 0x270)
#define	REG_PRE_AGG_MAX_ALLOWED_NSS_BW_CFG                       (PRE_AGG_BASE_ADDRESS + 0x274)
#define	REG_PRE_AGG_TOMI_MU_DISABLE_CFG                          (PRE_AGG_BASE_ADDRESS + 0x278)
#define	REG_PRE_AGG_HE_MU_TF_MU_BAR_CFG                          (PRE_AGG_BASE_ADDRESS + 0x27C)
#define	REG_PRE_AGG_HE_MU_PHASE_INFO                             (PRE_AGG_BASE_ADDRESS + 0x280)
#define	REG_PRE_AGG_HE_MU_PLAN_ACTION_BITMAP                     (PRE_AGG_BASE_ADDRESS + 0x284)
#define	REG_PRE_AGG_EMPTY_USER_TCR_REG0                          (PRE_AGG_BASE_ADDRESS + 0x28C)
#define	REG_PRE_AGG_EMPTY_USER_TCR_REG1                          (PRE_AGG_BASE_ADDRESS + 0x290)
#define	REG_PRE_AGG_EMPTY_USER_TCR_REG2                          (PRE_AGG_BASE_ADDRESS + 0x294)
#define	REG_PRE_AGG_EMPTY_USER_TCR_REG3                          (PRE_AGG_BASE_ADDRESS + 0x298)
#define	REG_PRE_AGG_EMPTY_USER_RCR_REG                           (PRE_AGG_BASE_ADDRESS + 0x29C)
#define	REG_PRE_AGG_PLAN_INDEX_TO_POINTER_LUT_BASE_ADDR          (PRE_AGG_BASE_ADDRESS + 0x2A0)
#define	REG_PRE_AGG_INV_PHY_DMA_RATIO_MU_MIMO_2USERS             (PRE_AGG_BASE_ADDRESS + 0x2A4)
#define	REG_PRE_AGG_INV_PHY_DMA_RATIO_MU_MIMO_3USERS             (PRE_AGG_BASE_ADDRESS + 0x2A8)
#define	REG_PRE_AGG_INV_PHY_DMA_RATIO_MU_MIMO_4USERS             (PRE_AGG_BASE_ADDRESS + 0x2AC)
#define	REG_PRE_AGG_INV_PHY_DMA_RATIO_SU                         (PRE_AGG_BASE_ADDRESS + 0x2B0)
#define	REG_PRE_AGG_PACKET_EXT_OVERHEAD                          (PRE_AGG_BASE_ADDRESS + 0x2B4)
#define	REG_PRE_AGG_MIN_MPDU_LENGTH_CONF                         (PRE_AGG_BASE_ADDRESS + 0x2B8)
#define	REG_PRE_AGG_AT_LEAST_ONE_STA_IN_PS_W_FORCE_ONE_NSS       (PRE_AGG_BASE_ADDRESS + 0x2BC)
#define	REG_PRE_AGG_PRE_AGG_CMN_RPT_FIFO_BASE_ADDR               (PRE_AGG_BASE_ADDRESS + 0x2C0)
#define	REG_PRE_AGG_PRE_AGG_CMN_RPT_FIFO_DEPTH_MINUS_ONE         (PRE_AGG_BASE_ADDRESS + 0x2C4)
#define	REG_PRE_AGG_PRE_AGG_CMN_RPT_FIFO_CLEAR_STRB              (PRE_AGG_BASE_ADDRESS + 0x2C8)
#define	REG_PRE_AGG_PRE_AGG_CMN_RPT_FIFO_RD_ENTRIES_NUM          (PRE_AGG_BASE_ADDRESS + 0x2CC)
#define	REG_PRE_AGG_PRE_AGG_CMN_RPT_FIFO_NUM_ENTRIES_COUNT       (PRE_AGG_BASE_ADDRESS + 0x2D0)
#define	REG_PRE_AGG_PRE_AGG_CMN_RPT_FIFO_DEBUG                   (PRE_AGG_BASE_ADDRESS + 0x2D4)
#define	REG_PRE_AGG_PRE_AGG_USR_RPT_FIFO_BASE_ADDR               (PRE_AGG_BASE_ADDRESS + 0x2D8)
#define	REG_PRE_AGG_PRE_AGG_USR_RPT_FIFO_DEPTH_MINUS_ONE         (PRE_AGG_BASE_ADDRESS + 0x2DC)
#define	REG_PRE_AGG_PRE_AGG_USR_RPT_FIFO_CLEAR_STRB              (PRE_AGG_BASE_ADDRESS + 0x2E0)
#define	REG_PRE_AGG_PRE_AGG_USR_RPT_FIFO_RD_ENTRIES_NUM          (PRE_AGG_BASE_ADDRESS + 0x2E4)
#define	REG_PRE_AGG_PRE_AGG_USR_RPT_FIFO_NUM_ENTRIES_COUNT       (PRE_AGG_BASE_ADDRESS + 0x2E8)
#define	REG_PRE_AGG_PRE_AGG_USR_RPT_FIFO_DEBUG                   (PRE_AGG_BASE_ADDRESS + 0x2EC)
#define	REG_PRE_AGG_PRE_AGG_USR_RPT_FIFO_DEBUG2                  (PRE_AGG_BASE_ADDRESS + 0x2F0)
#define	REG_PRE_AGG_MIN_BYTES_PER_USER_CFG                       (PRE_AGG_BASE_ADDRESS + 0x2F4)
#define	REG_PRE_AGG_SEL_BW_FIFO_DBG_STATUS                       (PRE_AGG_BASE_ADDRESS + 0x2F8)
#define	REG_PRE_AGG_USE_UL_INDICATION_FOR_EOSP_CFG               (PRE_AGG_BASE_ADDRESS + 0x2FC)
#define	REG_PRE_AGG_HANDLE_OMI_BW                                (PRE_AGG_BASE_ADDRESS + 0x300)
#define	REG_PRE_AGG_PRE_AGG_SKIP_PHASE_CONTROL                   (PRE_AGG_BASE_ADDRESS + 0x304)
#define	REG_PRE_AGG_PRE_AGG_DEBUG_STATUS1                        (PRE_AGG_BASE_ADDRESS + 0x308)
#define	REG_PRE_AGG_PRE_AGG_DEBUG_STATUS2                        (PRE_AGG_BASE_ADDRESS + 0x30C)
#define	REG_PRE_AGG_PRE_AGG_RPT_FIFO_EN                          (PRE_AGG_BASE_ADDRESS + 0x310)
#define	REG_PRE_AGG_TF_PADDING_MULT_FACTOR_CFG                   (PRE_AGG_BASE_ADDRESS + 0x314)
#define	REG_PRE_AGG_HE_MU_KEEP_USER_CFG                          (PRE_AGG_BASE_ADDRESS + 0x318)
#define	REG_PRE_AGG_PRIMARY_AC_MNG_MAP                           (PRE_AGG_BASE_ADDRESS + 0x31C)
#define	REG_PRE_AGG_MAX_SU_PPDU_DURATION_LIMIT_1                 (PRE_AGG_BASE_ADDRESS + 0x320)
#define	REG_PRE_AGG_MAX_SU_PPDU_DURATION_LIMIT_2                 (PRE_AGG_BASE_ADDRESS + 0x324)
#define	REG_PRE_AGG_MAX_VHT_MU_PPDU_DURATION_LIMIT_1             (PRE_AGG_BASE_ADDRESS + 0x328)
#define	REG_PRE_AGG_MAX_VHT_MU_PPDU_DURATION_LIMIT_2             (PRE_AGG_BASE_ADDRESS + 0x32C)
#define	REG_PRE_AGG_MAX_HE_MU_DL_PPDU_DURATION_LIMIT_1           (PRE_AGG_BASE_ADDRESS + 0x330)
#define	REG_PRE_AGG_MAX_HE_MU_DL_PPDU_DURATION_LIMIT_2           (PRE_AGG_BASE_ADDRESS + 0x334)
#define	REG_PRE_AGG_REG_DATA_AIR_TIME_CALC_RESULT                (PRE_AGG_BASE_ADDRESS + 0x338)
#define	REG_PRE_AGG_MIN_PSDU_LENGTH_OTF_CONF                     (PRE_AGG_BASE_ADDRESS + 0x33C)
#define	REG_PRE_AGG_MU_MIMO_UPDATE_LDPC_ALL_USERS                (PRE_AGG_BASE_ADDRESS + 0x340)
#define	REG_PRE_AGG_SU_FORCE_MORE_DATA_AND_EOSP                  (PRE_AGG_BASE_ADDRESS + 0x344)
#define	REG_PRE_AGG_HE_TB_RR_INIT_PRIMARY_TID_CFG                (PRE_AGG_BASE_ADDRESS + 0x348)
#define	REG_PRE_AGG_HE_TB_PRIMARY_TID_SELECTION_CFG              (PRE_AGG_BASE_ADDRESS + 0x34C)
#define	REG_PRE_AGG_HE_TB_MODE_CONTROL_CFG                       (PRE_AGG_BASE_ADDRESS + 0x350)
#define	REG_PRE_AGG_HE_TB_MAX_BYTE_COUNTER_VAL_CFG               (PRE_AGG_BASE_ADDRESS + 0x354)
#define	REG_PRE_AGG_HE_TB_MAX_BYTE_COUNTER_SET_CFG               (PRE_AGG_BASE_ADDRESS + 0x358)
#define	REG_PRE_AGG_HE_TB_B_FACTOR_COEF_CFG                      (PRE_AGG_BASE_ADDRESS + 0x35C)
#define	REG_PRE_AGG_HE_TB_OFFSET_TID0_FACTOR_CFG                 (PRE_AGG_BASE_ADDRESS + 0x360)
#define	REG_PRE_AGG_HE_TB_OFFSET_TID1_FACTOR_CFG                 (PRE_AGG_BASE_ADDRESS + 0x364)
#define	REG_PRE_AGG_HE_TB_OFFSET_TID2_FACTOR_CFG                 (PRE_AGG_BASE_ADDRESS + 0x368)
#define	REG_PRE_AGG_HE_TB_OFFSET_TID3_FACTOR_CFG                 (PRE_AGG_BASE_ADDRESS + 0x36C)
#define	REG_PRE_AGG_HE_TB_OFFSET_TID4_FACTOR_CFG                 (PRE_AGG_BASE_ADDRESS + 0x370)
#define	REG_PRE_AGG_HE_TB_OFFSET_TID5_FACTOR_CFG                 (PRE_AGG_BASE_ADDRESS + 0x374)
#define	REG_PRE_AGG_HE_TB_OFFSET_TID6_FACTOR_CFG                 (PRE_AGG_BASE_ADDRESS + 0x378)
#define	REG_PRE_AGG_HE_TB_OFFSET_TID7_FACTOR_CFG                 (PRE_AGG_BASE_ADDRESS + 0x37C)
#define	REG_PRE_AGG_HE_TB_UNSPECIFIED_BYTE_COUNTER_VAP           (PRE_AGG_BASE_ADDRESS + 0x380)
#define	REG_PRE_AGG_PROTECTED_BEACON_VAP                         (PRE_AGG_BASE_ADDRESS + 0x384)
#define	REG_PRE_AGG_MIN_TWT_REM_SP_DUR_THR0                      (PRE_AGG_BASE_ADDRESS + 0x388)
#define	REG_PRE_AGG_MIN_TWT_REM_SP_DUR_THR1                      (PRE_AGG_BASE_ADDRESS + 0x38C)
#define	REG_PRE_AGG_MIN_TWT_REM_SP_DUR_THR2                      (PRE_AGG_BASE_ADDRESS + 0x390)
#define	REG_PRE_AGG_DELTA_TIME_FROM_END_TWT_SP                   (PRE_AGG_BASE_ADDRESS + 0x394)
#define	REG_PRE_AGG_CLR_PLAN_IGNORE_UL_DATA_START_STRB           (PRE_AGG_BASE_ADDRESS + 0x398)
#define	REG_PRE_AGG_CLR_PLAN_IGNORE_UL_DATA_DONE                 (PRE_AGG_BASE_ADDRESS + 0x39C)
#define	REG_PRE_AGG_MAX_HE_MU_UL_PPDU_DURATION_LIMIT_1           (PRE_AGG_BASE_ADDRESS + 0x3A0)
#define	REG_PRE_AGG_MAX_HE_MU_UL_PPDU_DURATION_LIMIT_2           (PRE_AGG_BASE_ADDRESS + 0x3A4)
#define	REG_PRE_AGG_MIN_TWT_REM_SP_DUR_THR_SU                    (PRE_AGG_BASE_ADDRESS + 0x3A8)
#define	REG_PRE_AGG_SU_DELTA_TIME_FROM_END_TWT_SP                (PRE_AGG_BASE_ADDRESS + 0x3AC)
#define	REG_PRE_AGG_SU_TWT_REM_SP_DUR_GLOBAL_EN                  (PRE_AGG_BASE_ADDRESS + 0x3B0)
#define	REG_PRE_AGG_MAX_PSDU_SIZE_HE_UL                          (PRE_AGG_BASE_ADDRESS + 0x3B4)
#define	REG_PRE_AGG_PSDU_BYTE_LEN_CALC_PARAMS2                   (PRE_AGG_BASE_ADDRESS + 0x3B8)
#define	REG_PRE_AGG_MULTI_TID_CONTROL                            (PRE_AGG_BASE_ADDRESS + 0x3BC)
#define	REG_PRE_AGG_TWT_REM_SP_DUR_RESULT                        (PRE_AGG_BASE_ADDRESS + 0x3C0)
#define	REG_PRE_AGG_DELTA_TIME_FROM_END_TWT_SP2                  (PRE_AGG_BASE_ADDRESS + 0x3C4)
#define	REG_PRE_AGG_PRE_AGG_SPARE                                (PRE_AGG_BASE_ADDRESS + 0x3FC)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_PRE_AGG_PRE_AGG_ENABLE 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 preAggEnable : 1; //Pre AGG enable, reset value: 0x1, access type: RW
		uint32 preAggMuEnable : 1; //Pre AGG MU transmission enable, reset value: 0x0, access type: RW
		uint32 reserved0 : 30;
	} bitFields;
} RegPreAggPreAggEnable_u;

/*REG_PRE_AGG_PACKET_DENSITY_BYTE_OVERHEAD 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 packetDensityByteOverhead : 6; //packet density byte overhead, reset value: 0x1, access type: RW
		uint32 reserved0 : 26;
	} bitFields;
} RegPreAggPacketDensityByteOverhead_u;

/*REG_PRE_AGG_TAIL_BIT_LENGTH_OVERHEAD1 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tailBitLengthOverhead111Ag : 6; //tail bit length overhead1 11ag, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 tailBitLengthOverhead111B : 6; //tail bit length overhead1 11b, reset value: 0x0, access type: RW
		uint32 reserved1 : 2;
		uint32 tailBitLengthOverhead111N : 6; //tail bit length overhead1 11n, reset value: 0x0, access type: RW
		uint32 reserved2 : 2;
		uint32 tailBitLengthOverhead111Ac : 6; //tail bit length overhead1 11ac, reset value: 0x0, access type: RW
		uint32 reserved3 : 2;
	} bitFields;
} RegPreAggTailBitLengthOverhead1_u;

/*REG_PRE_AGG_TAIL_BIT_LENGTH_OVERHEAD2 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tailBitLengthOverhead211Ag : 6; //tail bit length overhead2 11ag, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 tailBitLengthOverhead211B : 6; //tail bit length overhead2 11b, reset value: 0x0, access type: RW
		uint32 reserved1 : 2;
		uint32 tailBitLengthOverhead211N : 6; //tail bit length overhead2 11n, reset value: 0x0, access type: RW
		uint32 reserved2 : 2;
		uint32 tailBitLengthOverhead211Ac : 6; //tail bit length overhead2 11ac, reset value: 0x0, access type: RW
		uint32 reserved3 : 2;
	} bitFields;
} RegPreAggTailBitLengthOverhead2_u;

/*REG_PRE_AGG_PHY_PREAMBLE_DURATION_OVERHEAD 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phyPreambleOverhead11Ag : 8; //phy preamble duration overhead 11ag, reset value: 0x14, access type: RW
		uint32 phyPreambleOverhead11N : 8; //phy preamble duration overhead 11n, reset value: 0x20, access type: RW
		uint32 phyPreambleOverhead11Ac : 8; //phy preamble duration overhead 11ac, reset value: 0x24, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegPreAggPhyPreambleDurationOverhead_u;

/*REG_PRE_AGG_PHY_PREAMBLE_DURATION_OVERHEAD_11B 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phyPreambleOverhead11BShort : 8; //phy preamble duration overhead 11b short, reset value: 0x60, access type: RW
		uint32 phyPreambleOverhead11BLong : 8; //phy preamble duration overhead 11b long, reset value: 0xc0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPreAggPhyPreambleDurationOverhead11B_u;

/*REG_PRE_AGG_RTS_CTS2SELF_METHOD 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rtsCtsThresholdMethod : 2; //RTS-CTS threshold method, , 00: RTS/ CTS based on PPDU size and duration thresholds (at least one occurred). , 01: RTS/ CTS based on PPDU duration thresholds.  , 10: RTS/ CTS based on PPDU size thresholds.  , , reset value: 0x0, access type: RW
		uint32 cts2SelfThresholdMethod : 2; //CTS-to-Self threshold method, , 00: CTS-to-self based on PPDU size and duration thresholds (at least one occurred). , 01: CTS-to-self based on PPDU duration thresholds. , 10: CTS-to-self based on PPDU size thresholds. , , reset value: 0x0, access type: RW
		uint32 reserved0 : 28;
	} bitFields;
} RegPreAggRtsCts2SelfMethod_u;

/*REG_PRE_AGG_MAX_PSDU_SIZE_HT 0x1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 maxPsduSizeHt : 23; //max PSDU size in HT, reset value: 0xffff, access type: RW
		uint32 reserved0 : 9;
	} bitFields;
} RegPreAggMaxPsduSizeHt_u;

/*REG_PRE_AGG_MAX_PSDU_SIZE_VHT 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 maxPsduSizeVht : 23; //max PSDU size in VHT, reset value: 0xfffff, access type: RW
		uint32 reserved0 : 9;
	} bitFields;
} RegPreAggMaxPsduSizeVht_u;

/*REG_PRE_AGG_MAX_PSDU_SIZE_HE 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 maxPsduSizeHe : 23; //max PSDU size in HE, reset value: 0x3fffff, access type: RW
		uint32 reserved0 : 9;
	} bitFields;
} RegPreAggMaxPsduSizeHe_u;

/*REG_PRE_AGG_MAX_PSDU_TX_TIME_LIMIT 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 maxPsduTxTimeLimit : 13; //Max PSDU Tx Time limit, resolution of 2[us], reset value: 0x1fff, access type: RW
		uint32 reserved0 : 19;
	} bitFields;
} RegPreAggMaxPsduTxTimeLimit_u;

/*REG_PRE_AGG_PRE_AGG_TCR_MAC_DUR_EN 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 preAggTcrMacDurEn : 1; //one bit en/disable. '1' -enable mac duration logic to TCR (default).  '0' - take DB TCR relevant fields to destination TCR , reset value: 0x1, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegPreAggPreAggTcrMacDurEn_u;

/*REG_PRE_AGG_PRE_AGG_DEBUG_STATUS 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 currUserId : 6; //current user id, reset value: 0x0, access type: RO
		uint32 lastPhaseInd : 1; //last phse indication, reset value: 0x0, access type: RO
		uint32 txSelTxopType : 3; //txop type : STD_TXOP_TYPE_LEGACY = 0; , STD_TXOP_TYPE_HE_SU_OR_HE_EXT_SU = 1; , STD_TXOP_TYPE_VHT_MU = 2; , STD_TXOP_TYPE_HE_MU = 3; STD_TXOP_TYPE_HE_TB = 4, reset value: 0x0, access type: RO
		uint32 aggOtfMode : 1; //OTF mode indication, reset value: 0x0, access type: RO
		uint32 reserved0 : 3;
		uint32 phyMode : 3; //phy mode, reset value: 0x0, access type: RO
		uint32 heSuMultiTid : 1; //inidaction for he SU multi TID , reset value: 0x0, access type: RO
		uint32 firstPdType : 5; //PD type, reset value: 0x0, access type: RO
		uint32 reserved1 : 9;
	} bitFields;
} RegPreAggPreAggDebugStatus_u;

/*REG_PRE_AGG_MU_GRP_DB_ADDR 0x40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muGrpDbBaseAddr : 22; //MU Group DB base byte address, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 muGrpDbEntry4BSize : 8; //MU Group DB entry size, reolution of 4 Bytes, reset value: 0x19, access type: RW
	} bitFields;
} RegPreAggMuGrpDbAddr_u;

/*REG_PRE_AGG_AGG_CONFIG_DB 0x44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aggConfigDbBaseAddr : 22; //AGG config DB base address, byte address, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 aggConfigDbEntry4BSize : 8; //AGG config DB word size, reset value: 0x3, access type: RW
	} bitFields;
} RegPreAggAggConfigDb_u;

/*REG_PRE_AGG_AGG_CONFIG_DB_4B_OFFSET 0x48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aggConfigDb4BOffset : 8; //AGG config DB word offset, reset value: 0x0, access type: RW
		uint32 reserved0 : 24;
	} bitFields;
} RegPreAggAggConfigDb4BOffset_u;

/*REG_PRE_AGG_TCR_BASE_ADDR1 0x4C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrBaseAddr1 : 22; //First TCR base address, byte address, reset value: 0x0, access type: RW
		uint32 reserved0 : 10;
	} bitFields;
} RegPreAggTcrBaseAddr1_u;

/*REG_PRE_AGG_TCR_BASE_ADDR2 0x50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrBaseAddr2 : 22; //Second TCR base address, byte address, reset value: 0x0, access type: RW
		uint32 reserved0 : 10;
	} bitFields;
} RegPreAggTcrBaseAddr2_u;

/*REG_PRE_AGG_RCR_BASE_ADDR 0x54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rcrBaseAddr : 22; //RCR base address, byte address, reset value: 0x0, access type: RW
		uint32 reserved0 : 10;
	} bitFields;
} RegPreAggRcrBaseAddr_u;

/*REG_PRE_AGG_PRE_AGG_ABORT_PULSE 0x60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 1;
		uint32 preAggAbortPulse : 1; //Pre AGG abort pulse - write '1', reset value: 0x0, access type: WO
		uint32 reserved1 : 30;
	} bitFields;
} RegPreAggPreAggAbortPulse_u;

/*REG_PRE_AGG_PRE_AGG_TIMER_MIN_MAX 0x64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 preAggTimerMin : 13; //Pre AGG timer minimum, reset value: 0x500, access type: RW
		uint32 reserved0 : 2;
		uint32 preAggTimerMinEn : 1; //Pre AGG timer minimum enable, reset value: 0x0, access type: RW
		uint32 preAggTimerMax : 13; //Pre AGG timer maximum, reset value: 0xac8, access type: RW
		uint32 reserved1 : 2;
		uint32 preAggTimerMaxEn : 1; //Pre AGG timer maximum enable, reset value: 0x0, access type: RW
	} bitFields;
} RegPreAggPreAggTimerMinMax_u;

/*REG_PRE_AGG_PRE_AGG_TIMER 0x68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 preAggTimer : 13; //Pre AGG timer, reset value: 0x0, access type: RO
		uint32 reserved0 : 19;
	} bitFields;
} RegPreAggPreAggTimer_u;

/*REG_PRE_AGG_ZERO_TIMER_THRESHOLD 0x6C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 zeroTimerThreshold : 9; //zero timer threshold, reset value: 0x78, access type: RW
		uint32 reserved0 : 6;
		uint32 zeroTimerThresholdEn : 1; //zero timer threshold enable, reset value: 0x1, access type: RW
		uint32 reserved1 : 16;
	} bitFields;
} RegPreAggZeroTimerThreshold_u;

/*REG_PRE_AGG_PRE_AGG_STATUS_IRQ 0x70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 doneOperationIrq : 1; //done_operation_irq, reset value: 0x0, access type: RO
		uint32 operationTimerErrorIrq : 1; //operation_timer_error_irq, reset value: 0x0, access type: RO
		uint32 txhdStartDuringOperationIrq : 1; //txhd_start_during_operation_irq, reset value: 0x0, access type: RO
		uint32 firstPdPointerNullIrq : 1; //first_pd_pointer_null_irq, reset value: 0x0, access type: RO
		uint32 txSelPrimaryNotValidErrIrq : 1; //tx_sel_primary_not_valid_err_irq, reset value: 0x0, access type: RO
		uint32 txSelNoUspValidErrIrq : 1; //tx_sel_no_usp_valid_err_irq, reset value: 0x0, access type: RO
		uint32 seqStartDuringOperationIrq : 1; //seq_start_during_operation_irq, reset value: 0x0, access type: RO
		uint32 noPhaseValidInPlanIrq : 1; //no_phase_valid_in_plan_irq, reset value: 0x0, access type: RO
		uint32 noUsersValidInPhaseIrq : 1; //no_users_valid_in_phase_irq, reset value: 0x0, access type: RO
		uint32 cmnRptFifoFullDropIrq : 1; //cmn_rpt_fifo_full_drop_irq, reset value: 0x0, access type: RO
		uint32 cmnRptFifoDecLessThanZeroIrq : 1; //cmn_rpt_fifo_dec_less_than_zero_irq, reset value: 0x0, access type: RO
		uint32 firstUsrRptFifoFullDropIrq : 1; //first_usr_rpt_fifo_full_drop_irq, reset value: 0x0, access type: RO
		uint32 usrRptFifoDecLessThanZeroIrq : 1; //usr_rpt_fifo_dec_less_than_zero_irq, reset value: 0x0, access type: RO
		uint32 nonFirstUsrRptFifoFullDropIrq : 1; //non_first_usr_rpt_fifo_full_drop_irq, reset value: 0x0, access type: RO
		uint32 numOfSelctedTidsAboveMaxIrq : 1; //num_of_selcted_tids_above_max, reset value: 0x0, access type: RO
		uint32 autoRespTbModeStartDuringOperationIrq : 1; //auto_resp_tb_mode_start_during_operation_irq, reset value: 0x0, access type: RO
		uint32 clrPlanIgnoreUlDataStartDuringOperationIrq : 1; //clr_plan_ignore_ul_data_start_during_operation_irq, reset value: 0x0, access type: RO
		uint32 aggUnlockStartDuringOperationIrq : 1; //agg_unlock_start_during_operation_irq, reset value: 0x0, access type: RO
		uint32 txhdUnlockStartDuringOperationIrq : 1; //txhd_unlock_start_during_operation_irq, reset value: 0x0, access type: RO
		uint32 reserved0 : 13;
	} bitFields;
} RegPreAggPreAggStatusIrq_u;

/*REG_PRE_AGG_PRE_AGG_STATUS_IRQ_CLR 0x74 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 doneOperationIrqClr : 1; //done_operation_irq_clr, reset value: 0x0, access type: WO
		uint32 operationTimerErrorIrqClr : 1; //operation_timer_error_irq_clr, reset value: 0x0, access type: WO
		uint32 txhdStartDuringOperationIrqClr : 1; //txhd_start_during_operation_irq_clr, reset value: 0x0, access type: WO
		uint32 firstPdPointerNullIrqClr : 1; //first_pd_pointer_null_irq_clr, reset value: 0x0, access type: WO
		uint32 txSelPrimaryNotValidErrIrqClr : 1; //tx_sel_primary_not_valid_err_irq_clr, reset value: 0x0, access type: WO
		uint32 txSelNoUspValidErrIrqClr : 1; //tx_sel_no_usp_valid_err_irq_clr, reset value: 0x0, access type: WO
		uint32 seqStartDuringOperationIrqClr : 1; //seq_start_during_operation_irq_clr, reset value: 0x0, access type: WO
		uint32 noPhaseValidInPlanIrqClr : 1; //no_phase_valid_in_plan_irq_clr, reset value: 0x0, access type: WO
		uint32 noUsersValidInPhaseIrqClr : 1; //no_users_valid_in_phase_irq_clr, reset value: 0x0, access type: WO
		uint32 reserved0 : 5;
		uint32 numOfSelctedTidsAboveMaxIrqClr : 1; //num_of_selcted_tids_above_max clear, reset value: 0x0, access type: WO
		uint32 autoRespTbModeStartDuringOperationIrqClr : 1; //auto_resp_tb_mode_start_during_operation_irq_clr, reset value: 0x0, access type: WO
		uint32 clrPlanIgnoreUlDataStartDuringOperationIrqClr : 1; //clr_plan_ignore_ul_data_start_during_operation_irq_clr, reset value: 0x0, access type: WO
		uint32 aggUnlockStartDuringOperationIrqClr : 1; //agg_unlock_start_during_operation_irq_clr, reset value: 0x0, access type: WO
		uint32 txhdUnlockStartDuringOperationIrqClr : 1; //txhd_unlock_start_during_operation_irq_clr, reset value: 0x0, access type: WO
		uint32 reserved1 : 13;
	} bitFields;
} RegPreAggPreAggStatusIrqClr_u;

/*REG_PRE_AGG_PRE_AGG_STATUS_IRQ_MASK 0x78 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 doneOperationIrqMask : 1; //done_operation_irq_mask, reset value: 0x0, access type: RW
		uint32 operationTimerErrorIrqMask : 1; //operation_timer_error_irq_mask, reset value: 0x0, access type: RW
		uint32 txhdStartDuringOperationIrqMask : 1; //txhd_start_during_operation_irq_mask, reset value: 0x0, access type: RW
		uint32 firstPdPointerNullIrqMask : 1; //tx_std_forbidden_genrisc_access_irq_mask, reset value: 0x0, access type: RW
		uint32 txSelPrimaryNotValidErrIrqMask : 1; //tx_sel_primary_not_valid_err_irq_mask, reset value: 0x0, access type: RW
		uint32 txSelNoUspValidErrIrqMask : 1; //tx_sel_no_usp_valid_err_irq_mask, reset value: 0x0, access type: RW
		uint32 seqStartDuringOperationIrqMask : 1; //seq_start_during_operation_irq_mask, reset value: 0x0, access type: RW
		uint32 noPhaseValidInPlanIrqMask : 1; //no_phase_valid_in_plan_irq_mask, reset value: 0x0, access type: RW
		uint32 noUsersValidInPhaseIrqMask : 1; //no_users_valid_in_phase_irq_mask, reset value: 0x0, access type: RW
		uint32 cmnRptFifoFullDropIrqMask : 1; //cmn_rpt_fifo_full_drop_irq_mask, reset value: 0x0, access type: RW
		uint32 cmnRptFifoDecLessThanZeroIrqMask : 1; //cmn_rpt_fifo_dec_less_than_zero_irq_mask, reset value: 0x0, access type: RW
		uint32 firstUsrRptFifoFullDropIrqMask : 1; //first_usr_rpt_fifo_full_drop_irq_mask, reset value: 0x0, access type: RW
		uint32 usrRptFifoDecLessThanZeroIrqMask : 1; //usr_rpt_fifo_dec_less_than_zero_irq_mask, reset value: 0x0, access type: RW
		uint32 nonFirstUsrRptFifoFullDropIrqMask : 1; //non_first_usr_rpt_fifo_full_drop_irq_mask, reset value: 0x0, access type: RW
		uint32 numOfSelctedTidsAboveMaxIrqMask : 1; //num_of_selcted_tids_above_max mask, reset value: 0x0, access type: RW
		uint32 autoRespTbModeStartDuringOperationIrqMask : 1; //auto_resp_tb_mode_start_during_operation_irq_mask, reset value: 0x0, access type: RW
		uint32 clrPlanIgnoreUlDataStartDuringOperationIrqMask : 1; //clr_plan_ignore_ul_data_start_during_operation_irq_mask, reset value: 0x0, access type: RW
		uint32 aggUnlockStartDuringOperationIrqMask : 1; //agg_unlock_start_during_operation_irq_mask, reset value: 0x0, access type: RW
		uint32 txhdUnlockStartDuringOperationIrqMask : 1; //txhd_unlock_start_during_operation_irq_mask, reset value: 0x0, access type: RW
		uint32 reserved0 : 13;
	} bitFields;
} RegPreAggPreAggStatusIrqMask_u;

/*REG_PRE_AGG_OVERRIDE_DB_PARAMETERS 0x7C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 enableOverrideDbProtectionParams : 1; //Enable override DB protection parameters, reset value: 0x0, access type: RW
		uint32 overrideDbRtsCtsTxMethod : 2; //override DB rts_cts_tx_method, reset value: 0x0, access type: RW
		uint32 overrideDbCts2SelfTxMethod : 2; //override DB cts2self_tx_method, reset value: 0x0, access type: RW
		uint32 overrideDbStaticBwEn : 1; //override DB static_bw_en, reset value: 0x0, access type: RW
		uint32 overrideDbDynamicBwEn : 1; //override DB dynamic_bw_en, reset value: 0x0, access type: RW
		uint32 reserved0 : 1;
		uint32 enableOverrideDbAggregationParams : 1; //enable override DB aggregation parameters, reset value: 0x0, access type: RW
		uint32 overrideDbBaEnable : 1; //override DB ba_enable, reset value: 0x0, access type: RW
		uint32 overrideDbPpduTxMode : 2; //override DB ppdu_tx_mode, reset value: 0x0, access type: RW
		uint32 reserved1 : 20;
	} bitFields;
} RegPreAggOverrideDbParameters_u;

/*REG_PRE_AGG_FORCE_RTS_CTS_VAP 0x84 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 forceRtsCtsVap : 32; //Bit per VAP: Force RTS-CTS Transmission (relevant only to STA Q_ID), reset value: 0x0, access type: RW
	} bitFields;
} RegPreAggForceRtsCtsVap_u;

/*REG_PRE_AGG_FORCE_CTS2SELF_VAP 0x88 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 forceCts2SelfVap : 32; //Bit per VAP: Force CTS2Self Transmission (relevant only to STA Q_ID), reset value: 0x0, access type: RW
	} bitFields;
} RegPreAggForceCts2SelfVap_u;

/*REG_PRE_AGG_RTS_CTS2SELF_CONTROL 0x8C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ignoreDynamicBw20Mhz : 1; //Ignore dynamic enable bit from DB In case BW == 20MHz, reset value: 0x0, access type: RW
		uint32 muPsduLengthRtsCts2SelfThrSumMaxN : 1; //The PSDU length which used for RTS-CTS/CTS2Self based on threshold: , 0: Maximum of the PSDU length of the active USPs. , 1: SUM of the PSDU length of the active USPs., reset value: 0x0, access type: RW
		uint32 reserved0 : 30;
	} bitFields;
} RegPreAggRtsCts2SelfControl_u;

/*REG_PRE_AGG_BF_IMPLICIT_FILLER_PHY_MODE 0x90 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfImplicitFillerPhyMode11B : 1; //Enable setting the BF implicit filler when PHY mode is 11b, reset value: 0x0, access type: RW
		uint32 bfImplicitFillerPhyMode11Ag : 1; //Enable setting the BF implicit filler when PHY mode is 11a/g, reset value: 0x1, access type: RW
		uint32 bfImplicitFillerPhyMode11N : 1; //Enable setting the BF implicit filler when PHY mode is 11n, reset value: 0x0, access type: RW
		uint32 bfImplicitFillerPhyMode11Ac : 1; //Enable setting the BF implicit filler when PHY mode is 11ac, reset value: 0x0, access type: RW
		uint32 reserved0 : 28;
	} bitFields;
} RegPreAggBfImplicitFillerPhyMode_u;

/*REG_PRE_AGG_PREAMBLE_11B_TYPE_CONTROL 0x94 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 preamble11BTypeEn : 1; //Enable 11B preamble long/short setting based configuration instead of DB, reset value: 0x1, access type: RW
		uint32 reserved0 : 3;
		uint32 preamble11BMcs000Replacement : 3; //MCS replacement in case of 11B MCS value of 000, i.e. short preamable 1Mb/s, reset value: 0x1, access type: RW
		uint32 reserved1 : 25;
	} bitFields;
} RegPreAggPreamble11BTypeControl_u;

/*REG_PRE_AGG_PREAMBLE_11B_TYPE_VAP 0x98 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 preamble11BTypeVap : 32; //11B Preamble type, 1 bit per VAP: , 0 - short. , 1 - long., reset value: 0x0, access type: RW
	} bitFields;
} RegPreAggPreamble11BTypeVap_u;

/*REG_PRE_AGG_USE_ONLY_CCA_20S_PWR_PHY_MODE_HT 0x9C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 useOnlyCca20SPwrPhyModeHt : 32; //Bit per VAP: Use only CCA 20MHz secondary power free for selection BW 40Mhz in PHY mdoe HT, reset value: 0xffffffff, access type: RW
	} bitFields;
} RegPreAggUseOnlyCca20SPwrPhyModeHt_u;

/*REG_PRE_AGG_TX_STICKY_CCA_FREE 0xA0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txStickyCca20PFree : 1; //Tx sticky bit of CCA 20MHz primary free, reset value: 0x1, access type: RO
		uint32 txStickyCca20SFree : 1; //Tx sticky bit of CCA 20MHz secondary free, reset value: 0x1, access type: RO
		uint32 txStickyCca40SFree : 1; //Tx sticky bit of CCA 40MHz secondary free, reset value: 0x1, access type: RO
		uint32 txStickyCca80SFree : 1; //Tx sticky bit of CCA 80MHz secondary free, reset value: 0x1, access type: RO
		uint32 txStickyCca20SPwrFree : 1; //Tx sticky bit of CCA 20MHz secondary power free, reset value: 0x1, access type: RO
		uint32 reserved0 : 3;
		uint32 txStickyCcaBandsFree : 8; //Tx sticky bits of CCA per BAND free, reset value: 0xff, access type: RO
		uint32 reserved1 : 16;
	} bitFields;
} RegPreAggTxStickyCcaFree_u;

/*REG_PRE_AGG_RD_DB_TCRS_DATA_802_11_PD_TYPE 0xA4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rdDbTcrsData80211PdType : 32; //Select read TCRs and BW limit of data or 802.11, 1 bit per PD type: , 0 - Read 802.11 TCRs. , 1 - Read Data TCRs., reset value: 0x100107, access type: RW
	} bitFields;
} RegPreAggRdDbTcrsData80211PdType_u;

/*REG_PRE_AGG_BW_LIMIT_VAP_15_0 0xA8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bwLimitVap150 : 32; //BW limit per VAP, 2bits for each VAP, VAPs 15-0. , 00: 20MHz , 01: 40MHz , 10: 80MHz , 11: 160MHz , reset value: 0xffffffff, access type: RW
	} bitFields;
} RegPreAggBwLimitVap150_u;

/*REG_PRE_AGG_BW_LIMIT_VAP_31_16 0xAC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bwLimitVap3116 : 32; //BW limit per VAP, 2bits for each VAP, VAPs 31-16. , 00: 20MHz , 01: 40MHz , 10: 80MHz , 11: 160MHz , reset value: 0xffffffff, access type: RW
	} bitFields;
} RegPreAggBwLimitVap3116_u;

/*REG_PRE_AGG_MU_MULT_USRS_TRAINING_CONTROL 0xB0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muMultUsrsTrainingAirTimeDuration : 18; //Air time duration of MU multiple users BF training sequence, [micro seconds], reset value: 0x3ffff, access type: RW
		uint32 muMultUsrsTrainingBwLimit : 2; //BW limit of MU multiple users BF training sequence: , 00: 20MHz , 01: 40MHz , 10: 80MHz, reset value: 0x2, access type: RW
		uint32 muMultUsrsTrainingFillerType : 2; //Filler type of MU multiple users BF training sequence: , 00: Filler Disabled ( NA )   , 01: Filler using CTS-to-Self  , 10: Filler using RTS-CTS to the primary STA/TID    , 11: Filler using MAC NDP+ACK to the primary STA/TID, reset value: 0x1, access type: RW
		uint32 muMultUsrsTrainingProtection : 2; //Protection setting of MU multiple users BF training sequence: , 00: None , 01: RTS-CTS , 10: CTS-to-Self, reset value: 0x0, access type: RW
		uint32 muMultUsrsTrainingRtsCtsFormat : 2; //RTS-CTS format setting of MU multiple users BF training sequence: , 00: Legacy RTS. , 10: Static BW. , 11: Dynamic BW., reset value: 0x2, access type: RW
		uint32 reserved0 : 6;
	} bitFields;
} RegPreAggMuMultUsrsTrainingControl_u;

/*REG_PRE_AGG_MU_BAR_BACK_AIR_TIME_DURATION 0xB4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muBarBackAirTimeDuration : 10; //Air time duration of SIFS+BAR+SIFS+BACK for MU sequence, [micro seconds], reset value: 0x0, access type: RW
		uint32 reserved0 : 22;
	} bitFields;
} RegPreAggMuBarBackAirTimeDuration_u;

/*REG_PRE_AGG_MU_MAX_NSTS 0xB8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txMuMaxNsts : 3; //Max number of allocated Nsts (minus one) in the transmit, for all USPs, when MU-MIMO transmission is selected., reset value: 0x3, access type: RW
		uint32 rxMuMaxNsts : 3; //Max number of allocated Nsts (minus one) in the receive, for all USPs, when MU-MIMO transmission is selected, reset value: 0x3, access type: RW
		uint32 reserved0 : 26;
	} bitFields;
} RegPreAggMuMaxNsts_u;

/*REG_PRE_AGG_TXOP_AIR_TIME_DURATION 0x100 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txopAirTimeDuration : 18; //TXOP multiple data transmissions air time duration [micro seconds], reset value: 0x3ffff, access type: RW
		uint32 reserved0 : 14;
	} bitFields;
} RegPreAggTxopAirTimeDuration_u;

/*REG_PRE_AGG_BF_SEQ_AIR_TIME_DURATION 0x104 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfSeqAirTimeDuration : 18; //Air time duration of BF explicit sequence, [micro seconds], reset value: 0x1770, access type: RW
		uint32 reserved0 : 14;
	} bitFields;
} RegPreAggBfSeqAirTimeDuration_u;

/*REG_PRE_AGG_TX_DUR_CALC_CONTROL 0x108 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txDurAutoReplyDegradationFactor : 4; //Degradation factor for the Tx duration calculation for the auto reply of the main Tx, reset value: 0x0, access type: RW
		uint32 reserved0 : 4;
		uint32 txSeqDurMainOfdmAdd6MicroEn : 1; //Enable adding 0x6 [us] when the PHY mode of the main Tx frame is OFDM (11a/g/n/ac) during Tx sequence air time calculation, reset value: 0x0, access type: RW
		uint32 txMpduDurFieldOfdmDec6MicroEn : 1; //Enable decrease 0x6 [us] when the PHY mode of the Tx frame is OFDM (11a/g/n/ac) during Tx MPDU duration field calculation, reset value: 0x0, access type: RW
		uint32 reserved1 : 22;
	} bitFields;
} RegPreAggTxDurCalcControl_u;

/*REG_PRE_AGG_TX_SEQ_AIR_TIME_DUR 0x10C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSeqAirTimeDur : 18; //Total Tx sequence air time duration [micro seconds], reset value: 0x0, access type: RO
		uint32 reserved0 : 14;
	} bitFields;
} RegPreAggTxSeqAirTimeDur_u;

/*REG_PRE_AGG_NO_PSDU_TIME_LIMIT_11B_EN 0x110 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 noPpduTimeLimit11BEn : 1; //Enable ignore Max PPDU time limit DB parameter in PHY mode 11B, reset value: 0x1, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegPreAggNoPsduTimeLimit11BEn_u;

/*REG_PRE_AGG_BF_RPT_ANALYZER_DELTA_MCS_NSS 0x114 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfRptDeltaMcsCpuRaFillerLow : 4; //Delta MCS compare value to select CPU RA filler when BF report MCS is lower than DB MCS, reset value: 0x0, access type: RW
		uint32 bfRptDeltaMcsCpuRaFillerHigh : 4; //Delta MCS compare value to select CPU RA filler when BF report MCS is higher than DB MCS, reset value: 0x0, access type: RW
		uint32 bfRptDeltaMcsDoNothingLow : 4; //Delta MCS compare value to select HW fast RA when BF report MCS is lower than DB MCS, reset value: 0x0, access type: RW
		uint32 bfRptDeltaMcsDoNothingHigh : 4; //Delta MCS compare value to select HW fast RA when BF report MCS is higher than DB MCS, reset value: 0x0, access type: RW
		uint32 bfRptDeltaNssCpuRaFillerLow : 2; //Delta Nss compare value to select CPU RA filler when BF report Nss is lower than DB Nss, reset value: 0x0, access type: RW
		uint32 bfRptDeltaNssCpuRaFillerHigh : 2; //Delta Nss compare value to select CPU RA filler when BF report Nss is higher than DB Nss, reset value: 0x0, access type: RW
		uint32 bfRptDeltaNssDoNothingLow : 2; //Delta Nss compare value to select HW fast RA when BF report Nss is lower than DB Nss, reset value: 0x0, access type: RW
		uint32 bfRptDeltaNssDoNothingHigh : 2; //Delta Nss compare value to select HW fast RA when BF report Nss is higher than DB Nss, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegPreAggBfRptAnalyzerDeltaMcsNss_u;

/*REG_PRE_AGG_BF_RPT_ANALYZER_SNR_THRESHOLD_MCS0TO3 0x118 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfRptAnalyzerSnrThresholdMcs0To3 : 32; //BF report analyzer SNR threshold, 8bits signed value per MCS, reset value: 0x0, access type: RW
	} bitFields;
} RegPreAggBfRptAnalyzerSnrThresholdMcs0To3_u;

/*REG_PRE_AGG_BF_RPT_ANALYZER_SNR_THRESHOLD_MCS4TO7 0x11C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfRptAnalyzerSnrThresholdMcs4To7 : 32; //BF report analyzer SNR threshold, 8bits signed value per MCS, reset value: 0x0, access type: RW
	} bitFields;
} RegPreAggBfRptAnalyzerSnrThresholdMcs4To7_u;

/*REG_PRE_AGG_BF_RPT_ANALYZER_SNR_THRESHOLD_MCS8TO9 0x120 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfRptAnalyzerSnrThresholdMcs8To9 : 16; //BF report analyzer SNR threshold, 8bits signed value per MCS, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPreAggBfRptAnalyzerSnrThresholdMcs8To9_u;

/*REG_PRE_AGG_RX_BF_RPT_MCS_NSS_PARAMETERS 0x124 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxBfRptPhyMode : 2; //BF report PHY mode based on Action code and category: 0-HT, 1-VHT, 2-HE, reset value: 0x0, access type: RO
		uint32 rxBfRptMimoNc : 2; //Nc from BF report MIMO control, reset value: 0x0, access type: RO
		uint32 rxBfRptMfb : 7; //BF report MCS feedback, reset value: 0x0, access type: RO
		uint32 rxBfRptMfbValid : 1; //BF report MCS feeback valid indication, reset value: 0x0, access type: RO
		uint32 reserved0 : 20;
	} bitFields;
} RegPreAggRxBfRptMcsNssParameters_u;

/*REG_PRE_AGG_RX_BF_RPT_SNR 0x128 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxBfRptSnr : 32; //BF report SNR 0-3, reset value: 0x0, access type: RO
	} bitFields;
} RegPreAggRxBfRptSnr_u;

/*REG_PRE_AGG_RX_BF_RPT_SNR_STREAM0 0x12C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxBfRptSnrStream0 : 32; //BF report SNR of stream 0, with signed extension, reset value: 0x0, access type: RO
	} bitFields;
} RegPreAggRxBfRptSnrStream0_u;

/*REG_PRE_AGG_RX_BF_RPT_SNR_STREAM1 0x130 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxBfRptSnrStream1 : 32; //BF report SNR of stream 1, with signed extension, reset value: 0x0, access type: RO
	} bitFields;
} RegPreAggRxBfRptSnrStream1_u;

/*REG_PRE_AGG_RX_BF_RPT_SNR_STREAM2 0x134 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxBfRptSnrStream2 : 32; //BF report SNR of stream 2, with signed extension, reset value: 0x0, access type: RO
	} bitFields;
} RegPreAggRxBfRptSnrStream2_u;

/*REG_PRE_AGG_RX_BF_RPT_SNR_STREAM3 0x138 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxBfRptSnrStream3 : 32; //BF report SNR of stream 3, with signed extension, reset value: 0x0, access type: RO
	} bitFields;
} RegPreAggRxBfRptSnrStream3_u;

/*REG_PRE_AGG_PRE_AGG_LOGGER 0x140 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 preAggLoggerEn : 1; //Pre AGG logger enable, reset value: 0x0, access type: RW
		uint32 preAggRptLoggerEn : 1; //Pre AGG report logger enable, reset value: 0x0, access type: RW
		uint32 reserved0 : 6;
		uint32 preAggLoggerPriority : 2; //Pre AGG logger priority, reset value: 0x0, access type: RW
		uint32 reserved1 : 22;
	} bitFields;
} RegPreAggPreAggLogger_u;

/*REG_PRE_AGG_PRE_AGG_LOGGER_ACTIVE 0x144 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 preAggLoggerActive : 1; //Pre AGG logger active, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegPreAggPreAggLoggerActive_u;

/*REG_PRE_AGG_PRE_TX_CALIBRATION_IND_EN 0x148 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 macPhyPreTxCalibrationIndEn : 1; //Enable MAC-PHY pre Tx calibration indication, reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegPreAggPreTxCalibrationIndEn_u;

/*REG_PRE_AGG_PRE_TX_CALIBRATION_IND_SET 0x14C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 preTxCalibrationIndSet : 1; //Generate the Pre Tx calibration indication, by write '1' - for Debug purpose, reset value: 0x0, access type: WO
		uint32 reserved0 : 31;
	} bitFields;
} RegPreAggPreTxCalibrationIndSet_u;

/*REG_PRE_AGG_FIRST_PD_POINTER 0x198 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 firstPdPointer : 24; //First PD pointer of Primary USP, reset value: 0x0, access type: RO
		uint32 reserved0 : 8;
	} bitFields;
} RegPreAggFirstPdPointer_u;

/*REG_PRE_AGG_USP_LOCK 0x1C4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 uspLock : 32; //User locked bitmap, reset value: 0x0, access type: RO
	} bitFields;
} RegPreAggUspLock_u;

/*REG_PRE_AGG_TX_SEL_RESULT 0x1C8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 1;
		uint32 txSelMuPrimaryUsp : 2; //Tx selector Primary USP, reset value: 0x0, access type: RO
		uint32 txSelMuGrpTxMode : 2; //Tx selector MU Group Tx mode, reset value: 0x0, access type: RO
		uint32 txSelUspValid : 4; //Tx selector USP valid results, reset value: 0x0, access type: RO
		uint32 txSelMuGrpEntry : 7; //Tx selector MU Group entry index, reset value: 0x0, access type: RO
		uint32 reserved1 : 4;
		uint32 txSelHePlanLock : 1; //Tx selector HE Plan lock, reset value: 0x0, access type: RO
		uint32 txSelMuGroupLock : 1; //Tx selector MU group lock, reset value: 0x0, access type: RO
		uint32 aggSwitchMuToSu : 1; //AGG modified Tx selector results from MU to SU, reset value: 0x0, access type: RO
		uint32 txSelHePlanEntry : 7; //Tx selector HE Plan entry index, reset value: 0x0, access type: RO
		uint32 reserved2 : 2;
	} bitFields;
} RegPreAggTxSelResult_u;

/*REG_PRE_AGG_TX_SEL_USP0_RESULT1 0x1CC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSelUsp0TxqTidId : 4; //Tx selector usp0 TXQ TID id, reset value: 0x0, access type: RO
		uint32 txSelUsp0TxqStaId : 8; //Tx selector usp0 TXQ STA id, reset value: 0x0, access type: RO
		uint32 txSelUsp0TxqVapId : 5; //Tx selector usp0 TXQ VAP id, reset value: 0x0, access type: RO
		uint32 txSelUsp0TxqQueueId : 3; //Tx selector usp0 TXQ Queue id, reset value: 0x0, access type: RO
		uint32 txSelUsp0SuBfSeq : 2; //Tx selector usp0 SU BF sequence, reset value: 0x0, access type: RO
		uint32 reserved0 : 10;
	} bitFields;
} RegPreAggTxSelUsp0Result1_u;

/*REG_PRE_AGG_TX_SEL_USP0_RESULT2 0x1D0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSelUsp0BitmapStatus : 32; //Tx selector usp0 BitMAP Status, reset value: 0x0, access type: RO
	} bitFields;
} RegPreAggTxSelUsp0Result2_u;

/*REG_PRE_AGG_TX_SEL_USP1_RESULT1 0x1D4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSelUsp1TxqTidId : 4; //Tx selector usp1 TXQ TID id, reset value: 0x0, access type: RO
		uint32 txSelUsp1TxqStaId : 8; //Tx selector usp1 TXQ STA id, reset value: 0x0, access type: RO
		uint32 txSelUsp1TxqVapId : 5; //Tx selector usp1 TXQ VAP id, reset value: 0x0, access type: RO
		uint32 txSelUsp1TxqQueueId : 3; //Tx selector usp1 TXQ Queue id, reset value: 0x0, access type: RO
		uint32 txSelUsp1SuBfSeq : 2; //Tx selector usp1 SU BF sequence, reset value: 0x0, access type: RO
		uint32 reserved0 : 10;
	} bitFields;
} RegPreAggTxSelUsp1Result1_u;

/*REG_PRE_AGG_TX_SEL_USP1_RESULT2 0x1D8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSelUsp1BitmapStatus : 32; //Tx selector usp1 BitMAP Status, reset value: 0x0, access type: RO
	} bitFields;
} RegPreAggTxSelUsp1Result2_u;

/*REG_PRE_AGG_TX_SEL_USP2_RESULT1 0x1DC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSelUsp2TxqTidId : 4; //Tx selector usp2 TXQ TID id, reset value: 0x0, access type: RO
		uint32 txSelUsp2TxqStaId : 8; //Tx selector usp2 TXQ STA id, reset value: 0x0, access type: RO
		uint32 txSelUsp2TxqVapId : 5; //Tx selector usp2 TXQ VAP id, reset value: 0x0, access type: RO
		uint32 txSelUsp2TxqQueueId : 3; //Tx selector usp2 TXQ Queue id, reset value: 0x0, access type: RO
		uint32 txSelUsp2SuBfSeq : 2; //Tx selector usp2 SU BF sequence, reset value: 0x0, access type: RO
		uint32 reserved0 : 10;
	} bitFields;
} RegPreAggTxSelUsp2Result1_u;

/*REG_PRE_AGG_TX_SEL_USP2_RESULT2 0x1F0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSelUsp2BitmapStatus : 32; //Tx selector usp2 BitMAP Status, reset value: 0x0, access type: RO
	} bitFields;
} RegPreAggTxSelUsp2Result2_u;

/*REG_PRE_AGG_TX_SEL_USP3_RESULT1 0x1F4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSelUsp3TxqTidId : 4; //Tx selector usp3 TXQ TID id, reset value: 0x0, access type: RO
		uint32 txSelUsp3TxqStaId : 8; //Tx selector usp3 TXQ STA id, reset value: 0x0, access type: RO
		uint32 txSelUsp3TxqVapId : 5; //Tx selector usp3 TXQ VAP id, reset value: 0x0, access type: RO
		uint32 txSelUsp3TxqQueueId : 3; //Tx selector usp3 TXQ Queue id, reset value: 0x0, access type: RO
		uint32 txSelUsp3SuBfSeq : 2; //Tx selector usp3 SU BF sequence, reset value: 0x0, access type: RO
		uint32 reserved0 : 10;
	} bitFields;
} RegPreAggTxSelUsp3Result1_u;

/*REG_PRE_AGG_TX_SEL_USP3_RESULT2 0x1F8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txSelUsp3BitmapStatus : 32; //Tx selector usp3 BitMAP Status, reset value: 0x0, access type: RO
	} bitFields;
} RegPreAggTxSelUsp3Result2_u;

/*REG_PRE_AGG_MU_USP_SECONDARY_NO_TX_REASON 0x220 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muUsp0SecondaryNoTxReason : 4; //MU USP Secondary no Tx reason: , 8- PD type not data,  , 9- Cannot aggregate,  , 10- Max Nsts reached, reset value: 0x0, access type: RO
		uint32 muUsp1SecondaryNoTxReason : 4; //MU USP Secondary no Tx reason: , 8- PD type not data,  , 9- Cannot aggregate,  , 10- Max Nsts reached, reset value: 0x0, access type: RO
		uint32 muUsp2SecondaryNoTxReason : 4; //MU USP Secondary no Tx reason: , 8- PD type not data,  , 9- Cannot aggregate,  , 10- Max Nsts reached, reset value: 0x0, access type: RO
		uint32 muUsp3SecondaryNoTxReason : 4; //MU USP Secondary no Tx reason: , 8- PD type not data,  , 9- Cannot aggregate,  , 10- Max Nsts reached, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegPreAggMuUspSecondaryNoTxReason_u;

/*REG_PRE_AGG_PRE_AGG_DEBUG_SM 0x230 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 preAggMainSm : 6; //Pre AGG main SM, reset value: 0x0, access type: RO
		uint32 preAggRtsCts2SelfSm : 3; //Pre AGG RTS CTS2Self SM, reset value: 0x0, access type: RO
		uint32 preAggRdDbSm : 4; //Pre AGG Read DB SM, reset value: 0x0, access type: RO
		uint32 txDurSm : 5; //Tx duration State machine, reset value: 0x0, access type: RO
		uint32 reserved0 : 3;
		uint32 ccaBwSm : 2; //BW based CCA state machine, reset value: 0x0, access type: RO
		uint32 bfRptAnalyzerSm : 3; //BF report analyzer state machine, reset value: 0x0, access type: RO
		uint32 txselUnlockIfSm : 3; //Tx Selector unlock I/F state machine, reset value: 0x0, access type: RO
		uint32 qmRdPdSm : 3; //QM peek and Read PD state machine, reset value: 0x0, access type: RO
	} bitFields;
} RegPreAggPreAggDebugSm_u;

/*REG_PRE_AGG_PRE_AGG_DEBUG_SM1 0x234 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 stdIfSm : 4; //STD IF State Machine, reset value: 0x0, access type: RO
		uint32 preAggPRdSm : 5; //Plan read State Machine, reset value: 0x0, access type: RO
		uint32 preAggRdDbCtrlSm : 3; //Read DB control state machine, reset value: 0x0, access type: RO
		uint32 preAggParamsCalcSm : 4; //Parameters calc state machine, reset value: 0x0, access type: RO
		uint32 selBwStgFsm : 3; //Selector and BW state machine, reset value: 0x0, access type: RO
		uint32 reserved0 : 13;
	} bitFields;
} RegPreAggPreAggDebugSm1_u;

/*REG_PRE_AGG_BW_2_SUB_BAND_MAPPING 0x238 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bw2SubBandMapP20Mhz : 8; //primary 20MHz BW mapping to RU sub band  , reset value: 0x0, access type: RW
		uint32 bw2SubBandMapP40Mhz : 8; //primary 40MHz BW mapping to RU sub band  , reset value: 0x0, access type: RW
		uint32 bw2SubBandMapP80Mhz : 8; //primary 80MHz BW mapping to RU sub band  , reset value: 0x0, access type: RW
		uint32 bw2SubBandMapP160Mhz : 8; //primary 160MHz BW mapping to RU sub band  , reset value: 0x0, access type: RW
	} bitFields;
} RegPreAggBw2SubBandMapping_u;

/*REG_PRE_AGG_TAIL_BIT_LENGTH_OVERHEAD1_AX 0x23C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tailBitLengthOverhead111AxSu : 6; //tail bit length overhead1 11ax su, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 tailBitLengthOverhead111AxSuEx : 6; //tail bit length overhead1 11ax su ext, reset value: 0x0, access type: RW
		uint32 reserved1 : 2;
		uint32 tailBitLengthOverhead111AxTrg : 6; //tail bit length overhead1 11ax trigger based, reset value: 0x0, access type: RW
		uint32 reserved2 : 2;
		uint32 tailBitLengthOverhead111AxMu : 6; //tail bit length overhead1 11ax mu, reset value: 0x0, access type: RW
		uint32 reserved3 : 2;
	} bitFields;
} RegPreAggTailBitLengthOverhead1Ax_u;

/*REG_PRE_AGG_TAIL_BIT_LENGTH_OVERHEAD2_AX 0x240 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tailBitLengthOverhead211AxSu : 6; //tail bit length overhead2 11ax su, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 tailBitLengthOverhead211AxSuEx : 6; //tail bit length overhead2 11ax su ext, reset value: 0x0, access type: RW
		uint32 reserved1 : 2;
		uint32 tailBitLengthOverhead211AxTrg : 6; //tail bit length overhead2 11ax trigger based, reset value: 0x0, access type: RW
		uint32 reserved2 : 2;
		uint32 tailBitLengthOverhead211AxMu : 6; //tail bit length overhead2 11ax mu, reset value: 0x0, access type: RW
		uint32 reserved3 : 2;
	} bitFields;
} RegPreAggTailBitLengthOverhead2Ax_u;

/*REG_PRE_AGG_PHY_PREAMBLE_DURATION_OVERHEAD_AX 0x244 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phyPreambleOverhead11AxSu : 8; //phy preamble duration overhead 11ax su, reset value: 0x24, access type: RW
		uint32 phyPreambleOverhead11AxSuExt : 8; //phy preamble duration overhead 11ax su ext, reset value: 0x2E, access type: RW
		uint32 phyPreambleOverhead11AxTrg : 8; //phy preamble duration overhead 11ax  trigger based, reset value: 0x28, access type: RW
		uint32 phyPreambleOverhead11AxMu : 8; //phy preamble duration overhead 11ax mu, reset value: 0x2E, access type: RW
	} bitFields;
} RegPreAggPhyPreambleDurationOverheadAx_u;

/*REG_PRE_AGG_PSDU_BYTE_LEN_CALC_PARAMS 0x248 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 management64BCounterCfg : 8; //Number of 64 bytes to add for TID=15 / Management frame , Common for all the STAs., reset value: 0x20, access type: RW
		uint32 extraBytesForMpduHdrAmsduCfg : 8; //Number of bytes to add per MPDU in case of A-MSDU , Common for all the STAs. , Default value contains: 4 (delimiter) + 26 (basic header size) + 4 (FCS) + 3 (max padding between MPDUs), reset value: 0x25, access type: RW
		uint32 extraBytesForMpduHdrSingleMsduCfg : 8; //Number of bytes to add per MPDU in case of single MSDU , Common for all the STAs. , Default value contains: 4 (delimiter) + 26 (basic header size) - 6 (PD eth. Overhead) + 4 (FCS) + 3 (max padding between MPDUs), reset value: 0x1f, access type: RW
		uint32 extraBytesForMsduInAmsduCfg : 4; //Number of bytes to add per MSDU in case of A-MSDU , Common for all the STAs. , Default value contains: 14 (AMSDU size) - 6 (PD eth. Overhead) + 2 (average padding between MSDUs), reset value: 0xa, access type: RW
		uint32 extraBytesForMpduTrailCfg : 4; //Number of bytes to add per MPDU , Common for all the STAs. , Default value: 7 byte., reset value: 0x7, access type: RW
	} bitFields;
} RegPreAggPsduByteLenCalcParams_u;

/*REG_PRE_AGG_BAA_PROCESS_TIME 0x24C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 baaProcessTimeHeSu : 10; //Time that require to send Data while the BAA process the previous results TX Data. , Relevant for HE MU DL Data , Default value: 150usec. (each bit represents 1usec), reset value: 0x96, access type: RW
		uint32 baaProcessTimeVhtMu : 10; //Time that require to send Data while the BAA process the previous results TX Data. , Relevant for HE MU DL Data , Default value: 100usec. (each bit represents 1usec), reset value: 0x64, access type: RW
		uint32 baaProcessTimeHeMu : 10; //Time that require to send Data while the BAA process the previous results TX Data. , Relevant for HE MU DL Data , Default value: 500usec. (each bit represents 1usec), reset value: 0x1F4, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegPreAggBaaProcessTime_u;

/*REG_PRE_AGG_HE_OMI_CONFIGURATION 0x250 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ignoreTomiMuDisable : 1; //0: not ignore TOMI UL/DL MU Disable i.e. in case STA set TOMI UL/DL MU Disable, this STA shall be removed from DL and UL Data plans. , 1: ignore TOMI UL/DL MU Disable., reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegPreAggHeOmiConfiguration_u;

/*REG_PRE_AGG_SU_DURATION_UPDATE_CONFIG 0x254 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 suPartialDuration : 18; //Partial Duration in [us] for SU Partial Duration Mode, reset value: 0x0, access type: RW
		uint32 reserved0 : 14;
	} bitFields;
} RegPreAggSuDurationUpdateConfig_u;

/*REG_PRE_AGG_VHT_DURATION_CONFIG 0x258 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vhtPartialDuration : 18; //Partial Duration in [us] for VHT Partial Duration Mode (set in Group DB), reset value: 0x0, access type: RW
		uint32 reserved0 : 14;
	} bitFields;
} RegPreAggVhtDurationConfig_u;

/*REG_PRE_AGG_HE_SU_UNSPECIFIED_DURATION_VAP 0x25C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 heSuUnspecifiedDurationVap : 32; //Set TCR TXOP duration unspecified for HE SU, HE SU EX. 1 bit per VAP, reset value: 0x0, access type: RW
	} bitFields;
} RegPreAggHeSuUnspecifiedDurationVap_u;

/*REG_PRE_AGG_BAR_WIN_SIZE_CONFIG 0x260 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dontCalcBarWinSize : 1; //Don't Calc BAR Window Size Configuration: , 0: Calc Bar Window Size  , 1: Don't Calc BAR Window Size, reset value: 0x1, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegPreAggBarWinSizeConfig_u;

/*REG_PRE_AGG_HE_MU_PROTECTION_SOUNDING_PHASE_DB_CONFIG 0x264 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 protectionPhasePerUser4BOffset : 8; //HE MU protection phase offset from base address of DB to first per user word in DWs (4Bytes), reset value: 0x15, access type: RW
		uint32 protectionPhasePerUserEntry4BSize : 4; //HE MU protection phase DB per user entry size in DWs (4Bytes), reset value: 0x2, access type: RW
		uint32 reserved0 : 4;
		uint32 soundingPhasePerUser4BOffset : 8; //HE MU sounding phase offset from base address of DB to first per user word in DWs (4Bytes), reset value: 0x1f, access type: RW
		uint32 soundingPhasePerUserEntry4BSize : 4; //HE MU sounding phase DB per user entry size in DWs (4Bytes), reset value: 0x3, access type: RW
		uint32 reserved1 : 4;
	} bitFields;
} RegPreAggHeMuProtectionSoundingPhaseDbConfig_u;

/*REG_PRE_AGG_HE_MU_DL_UL_DATA_PHASE_DB_CONFIG 0x268 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dlDataPhasePerUser4BOffset : 8; //HE MU DL DATA phase offset from base address of DB to first per user word in DWs (4Bytes), reset value: 0x1d, access type: RW
		uint32 dlDataPhasePerUserEntry4BSize : 4; //HE MU DL DATA phase DB per user entry size in DWs (4Bytes), reset value: 0x5, access type: RW
		uint32 reserved0 : 4;
		uint32 ulDataPhasePerUser4BOffset : 8; //HE MU UL DATA phase offset from base address of DB to first per user word in DWs (4Bytes), reset value: 0x1f, access type: RW
		uint32 ulDataPhasePerUserEntry4BSize : 4; //HE MU UL DATA phase DB per user entry size in DWs (4Bytes), reset value: 0x6, access type: RW
		uint32 reserved1 : 4;
	} bitFields;
} RegPreAggHeMuDlUlDataPhaseDbConfig_u;

/*REG_PRE_AGG_HE_NFRP_PHASE_DB_CONFIG 0x26C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 nfrpPhasePerUser4BOffset : 8; //HE NFRP phase offset from base address of DB to first per user word in DWs (4Bytes), reset value: 0x15, access type: RW
		uint32 nfrpPhasePerUserEntry4BSize : 4; //HE NFRP phase DB per user entry size in DWs (4Bytes), reset value: 0x2, access type: RW
		uint32 reserved0 : 20;
	} bitFields;
} RegPreAggHeNfrpPhaseDbConfig_u;

/*REG_PRE_AGG_HE_PLAN_DB_POINTER 0x270 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hePlanDbPointer : 22; //HE MU Plan DB pointer, reset value: 0x0, access type: RO
		uint32 reserved0 : 10;
	} bitFields;
} RegPreAggHePlanDbPointer_u;

/*REG_PRE_AGG_MAX_ALLOWED_NSS_BW_CFG 0x274 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 maxAllowedNssCfg : 3; //Max Allowed NSS (per CDB) , 0 ÃÂ¢ÃÂÃÂ 1 NSS , 1 ÃÂ¢ÃÂÃÂ 2 NSS , 2 ÃÂ¢ÃÂÃÂ 3 NSS , 3 ÃÂ¢ÃÂÃÂ 4 NSS , 4 ÃÂ¢ÃÂÃÂ 5 NSS , 5 ÃÂ¢ÃÂÃÂ 6 NSS , 6 ÃÂ¢ÃÂÃÂ 7 NSS , 7 ÃÂ¢ÃÂÃÂ 8 NSS , Default: 3 ÃÂ¢ÃÂÃÂ 4NSS , reset value: 0x3, access type: RW
		uint32 maxAllowedBwCfg : 2; //Max Allowed BW (per CDB) , 00 ÃÂ¢ÃÂÃÂ 20MHz , 01 ÃÂ¢ÃÂÃÂ 40MHz  , 10 ÃÂ¢ÃÂÃÂ 80MHz , 11 ÃÂ¢ÃÂÃÂ 160MHz , Default: 11 ÃÂ¢ÃÂÃÂ 160MHz , reset value: 0x3, access type: RW
		uint32 reserved0 : 27;
	} bitFields;
} RegPreAggMaxAllowedNssBwCfg_u;

/*REG_PRE_AGG_TOMI_MU_DISABLE_CFG 0x278 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tomiDlHeMuImmedaiteCfg : 2; //no description, reset value: 0x2, access type: RW
		uint32 tomiDlHeMuVhtAlikeCfg : 2; //no description, reset value: 0x0, access type: RW
		uint32 tomiDlHeMuMuBarCfg : 2; //no description, reset value: 0x2, access type: RW
		uint32 tomiUlHeMuDataCfg : 2; //no description, reset value: 0x3, access type: RW
		uint32 tomiUlHeMuBsrpCfg : 2; //no description, reset value: 0x2, access type: RW
		uint32 tomiSoundingCfg : 2; //no description, reset value: 0x2, access type: RW
		uint32 tomiHeMuRtsCfg : 2; //no description, reset value: 0x0, access type: RW
		uint32 tomiSuVhtCfg : 2; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPreAggTomiMuDisableCfg_u;

/*REG_PRE_AGG_HE_MU_TF_MU_BAR_CFG 0x27C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tfMuBarStSendCbarVar : 1; //The configuration defined if in case of MU-BAR with single TID need to send Compressed BAR variant or Multi-TID BAR Variant: , 0 ÃÂ¢ÃÂÃÂ send TF MU-BAR with Multi-TID BAR Variant , 1 ÃÂ¢ÃÂÃÂ send TF MU-BAR with Compressed BAR Variant     , Default Value  = 1, reset value: 0x1, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegPreAggHeMuTfMuBarCfg_u;

/*REG_PRE_AGG_HE_MU_PHASE_INFO 0x280 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 heMuPhasePointer : 22; //reflect the current HE MU phase pointer of the phase beeing processed , reset value: 0x0, access type: RO
		uint32 reserved0 : 2;
		uint32 heMuPhaseIndex : 4; //reflect the current HE MU phase index, reset value: 0x0, access type: RO
		uint32 reserved1 : 4;
	} bitFields;
} RegPreAggHeMuPhaseInfo_u;

/*REG_PRE_AGG_HE_MU_PLAN_ACTION_BITMAP 0x284 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 heMuPlanActionBitmap : 16; //reflect the current HE MU plan action bitmap from phase DB , reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegPreAggHeMuPlanActionBitmap_u;

/*REG_PRE_AGG_EMPTY_USER_TCR_REG0 0x28C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 emptyUserTcr310 : 32; //empty user TCR bits [31:0] to be written to tx cyc buff in case user is not valid in phase DB, reset value: 0x0, access type: RW
	} bitFields;
} RegPreAggEmptyUserTcrReg0_u;

/*REG_PRE_AGG_EMPTY_USER_TCR_REG1 0x290 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 emptyUserTcr6332 : 32; //empty user TCR bits [63:32] to be written to tx cyc buff in case user is not valid in phase DB, reset value: 0x0, access type: RW
	} bitFields;
} RegPreAggEmptyUserTcrReg1_u;

/*REG_PRE_AGG_EMPTY_USER_TCR_REG2 0x294 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 emptyUserTcr9564 : 32; //empty user TCR bits [95:64] to be written to tx cyc buff in case user is not valid in phase DB, reset value: 0x0, access type: RW
	} bitFields;
} RegPreAggEmptyUserTcrReg2_u;

/*REG_PRE_AGG_EMPTY_USER_TCR_REG3 0x298 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 emptyUserTcr12796 : 32; //empty user TCR bits [127:96] to be written to tx cyc buff in case user is not valid in phase DB, reset value: 0x0, access type: RW
	} bitFields;
} RegPreAggEmptyUserTcrReg3_u;

/*REG_PRE_AGG_EMPTY_USER_RCR_REG 0x29C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 emptyUserRcr310 : 32; //empty user RCR bits [31:0] to be written to tx cyc buff in case user is not valid in phase DB, reset value: 0x0, access type: RW
	} bitFields;
} RegPreAggEmptyUserRcrReg_u;

/*REG_PRE_AGG_PLAN_INDEX_TO_POINTER_LUT_BASE_ADDR 0x2A0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 planIndexToPointerLutBaseAddr : 25; //base address of LUT contianing plan DB base address according to plan index, reset value: 0x0, access type: RW
		uint32 reserved0 : 7;
	} bitFields;
} RegPreAggPlanIndexToPointerLutBaseAddr_u;

/*REG_PRE_AGG_INV_PHY_DMA_RATIO_MU_MIMO_2USERS 0x2A4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 invPhyDmaRatioMuMimo2Users : 12; //inv_phy_dma_ratio for 2 MU MIMO, reset value: 0xda, access type: RW
		uint32 reserved0 : 20;
	} bitFields;
} RegPreAggInvPhyDmaRatioMuMimo2Users_u;

/*REG_PRE_AGG_INV_PHY_DMA_RATIO_MU_MIMO_3USERS 0x2A8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 invPhyDmaRatioMuMimo3Users : 12; //inv_phy_dma_ratio for 3 MU MIMO, reset value: 0x148, access type: RW
		uint32 reserved0 : 20;
	} bitFields;
} RegPreAggInvPhyDmaRatioMuMimo3Users_u;

/*REG_PRE_AGG_INV_PHY_DMA_RATIO_MU_MIMO_4USERS 0x2AC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 invPhyDmaRatioMuMimo4Users : 12; //inv_phy_dma_ratio for 4 MU MIMO, reset value: 0x1b5, access type: RW
		uint32 reserved0 : 20;
	} bitFields;
} RegPreAggInvPhyDmaRatioMuMimo4Users_u;

/*REG_PRE_AGG_INV_PHY_DMA_RATIO_SU 0x2B0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 invPhyDmaRatioSu : 12; //inv_phy_dma_ratio for SU, reset value: 0x6d, access type: RW
		uint32 reserved0 : 20;
	} bitFields;
} RegPreAggInvPhyDmaRatioSu_u;

/*REG_PRE_AGG_PACKET_EXT_OVERHEAD 0x2B4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 packetExtOverhead : 5; //packet_ext_overhead, reset value: 0x10, access type: RW
		uint32 reserved0 : 27;
	} bitFields;
} RegPreAggPacketExtOverhead_u;

/*REG_PRE_AGG_MIN_MPDU_LENGTH_CONF 0x2B8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 minMpduLengthConf : 14; //min_mpdu_length_conf, reset value: 0x0, access type: RW
		uint32 reserved0 : 18;
	} bitFields;
} RegPreAggMinMpduLengthConf_u;

/*REG_PRE_AGG_AT_LEAST_ONE_STA_IN_PS_W_FORCE_ONE_NSS 0x2BC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 atLeastOneStaInPsWForceOneNss : 1; //at_least_one_sta_in_ps_w_force_one_nss, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegPreAggAtLeastOneStaInPsWForceOneNss_u;

/*REG_PRE_AGG_PRE_AGG_CMN_RPT_FIFO_BASE_ADDR 0x2C0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 preAggCmnRptFifoBaseAddr : 22; //PRE AGG COMMON REPORT  FIFO base address, reset value: 0x0, access type: RW
		uint32 reserved0 : 10;
	} bitFields;
} RegPreAggPreAggCmnRptFifoBaseAddr_u;

/*REG_PRE_AGG_PRE_AGG_CMN_RPT_FIFO_DEPTH_MINUS_ONE 0x2C4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 preAggCmnRptFifoDepthMinusOne : 10; //PRE AGG COMMON REPORT  FIFO depth minus one, reset value: 0x0, access type: RW
		uint32 reserved0 : 22;
	} bitFields;
} RegPreAggPreAggCmnRptFifoDepthMinusOne_u;

/*REG_PRE_AGG_PRE_AGG_CMN_RPT_FIFO_CLEAR_STRB 0x2C8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 preAggCmnRptFifoClearStrb : 1; //Clear PRE AGG COMMON REPORT  FIFO. , By write '1'., reset value: 0x0, access type: WO
		uint32 preAggCmnRptFifoClearFullDropCtrStrb : 1; //Clear PRE AGG COMMON REPORT  full drop counter. , By write '1'., reset value: 0x0, access type: WO
		uint32 preAggCmnRptFifoClearDecLessThanZeroStrb : 1; //Clear PRE AGG COMMON REPORT  FIFO decrement amount less than zero indication. , By write '1'.  , , reset value: 0x0, access type: WO
		uint32 reserved0 : 29;
	} bitFields;
} RegPreAggPreAggCmnRptFifoClearStrb_u;

/*REG_PRE_AGG_PRE_AGG_CMN_RPT_FIFO_RD_ENTRIES_NUM 0x2CC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 preAggCmnRptFifoRdEntriesNum : 11; //PRE AGG COMMON REPORT  FIFO number of entries to decrement., reset value: 0x0, access type: RW
		uint32 reserved0 : 21;
	} bitFields;
} RegPreAggPreAggCmnRptFifoRdEntriesNum_u;

/*REG_PRE_AGG_PRE_AGG_CMN_RPT_FIFO_NUM_ENTRIES_COUNT 0x2D0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 preAggCmnRptFifoNumEntriesCount : 11; //PRE AGG COMMON REPORT  FIFO number of entries count, reset value: 0x0, access type: RO
		uint32 reserved0 : 21;
	} bitFields;
} RegPreAggPreAggCmnRptFifoNumEntriesCount_u;

/*REG_PRE_AGG_PRE_AGG_CMN_RPT_FIFO_DEBUG 0x2D4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 preAggCmnRptFifoWrPtr : 10; //PRE AGG COMMON REPORT  FIFO write pointer index, reset value: 0x0, access type: RO
		uint32 reserved0 : 6;
		uint32 preAggCmnRptFifoNotEmpty : 1; //PRE AGG COMMON REPORT  FIFO not empty indication, reset value: 0x0, access type: RO
		uint32 preAggCmnRptFifoFull : 1; //PRE AGG COMMON REPORT  FIFO full indication, reset value: 0x0, access type: RO
		uint32 preAggCmnRptFifoDecrementLessThanZero : 1; //PRE AGG COMMON REPORT  FIFO amount has been decremented less than zero, reset value: 0x0, access type: RO
		uint32 reserved1 : 1;
		uint32 preAggCmnRptFifoFullDropCtr : 10; //PRE AGG COMMON REPORT  FIFO full drop counter, reset value: 0x0, access type: RO
		uint32 reserved2 : 2;
	} bitFields;
} RegPreAggPreAggCmnRptFifoDebug_u;

/*REG_PRE_AGG_PRE_AGG_USR_RPT_FIFO_BASE_ADDR 0x2D8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 preAggUsrRptFifoBaseAddr : 22; //PRE AGG USER REPORT  FIFO base address, reset value: 0x0, access type: RW
		uint32 reserved0 : 10;
	} bitFields;
} RegPreAggPreAggUsrRptFifoBaseAddr_u;

/*REG_PRE_AGG_PRE_AGG_USR_RPT_FIFO_DEPTH_MINUS_ONE 0x2DC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 preAggUsrRptFifoDepthMinusOne : 10; //PRE AGG USER REPORT  FIFO depth minus one, reset value: 0x0, access type: RW
		uint32 reserved0 : 22;
	} bitFields;
} RegPreAggPreAggUsrRptFifoDepthMinusOne_u;

/*REG_PRE_AGG_PRE_AGG_USR_RPT_FIFO_CLEAR_STRB 0x2E0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 preAggUsrRptFifoClearStrb : 1; //Clear PRE AGG USER REPORT  FIFO. , By write '1'., reset value: 0x0, access type: WO
		uint32 preAggFirstUsrRptFifoClearFullDropCtrStrb : 1; //Clear PRE AGG USER REPORT  full drop counter first user. , By write '1'., reset value: 0x0, access type: WO
		uint32 preAggUsrRptFifoClearDecLessThanZeroStrb : 1; //Clear PRE AGG USER REPORT  FIFO decrement amount less than zero indication. , By write '1'.  , , reset value: 0x0, access type: WO
		uint32 preAggNonFirstUsrRptFifoClearFullDropCtrStrb : 1; //Clear PRE AGG USER REPORT  full drop counter NON first user. , By write '1'., reset value: 0x0, access type: WO
		uint32 reserved0 : 28;
	} bitFields;
} RegPreAggPreAggUsrRptFifoClearStrb_u;

/*REG_PRE_AGG_PRE_AGG_USR_RPT_FIFO_RD_ENTRIES_NUM 0x2E4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 preAggUsrRptFifoRdEntriesNum : 11; //PRE AGG USER REPORT  FIFO number of entries to decrement., reset value: 0x0, access type: RW
		uint32 reserved0 : 21;
	} bitFields;
} RegPreAggPreAggUsrRptFifoRdEntriesNum_u;

/*REG_PRE_AGG_PRE_AGG_USR_RPT_FIFO_NUM_ENTRIES_COUNT 0x2E8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 preAggUsrRptFifoNumEntriesCount : 11; //PRE AGG USER REPORT  FIFO number of entries count, reset value: 0x0, access type: RO
		uint32 reserved0 : 21;
	} bitFields;
} RegPreAggPreAggUsrRptFifoNumEntriesCount_u;

/*REG_PRE_AGG_PRE_AGG_USR_RPT_FIFO_DEBUG 0x2EC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 preAggUsrRptFifoWrPtr : 10; //PRE AGG USER REPORT  FIFO write pointer index, reset value: 0x0, access type: RO
		uint32 reserved0 : 6;
		uint32 preAggUsrRptFifoNotEmpty : 1; //PRE AGG USER REPORT  FIFO not empty indication, reset value: 0x0, access type: RO
		uint32 preAggUsrRptFifoFull : 1; //PRE AGG USER REPORT  FIFO full indication, reset value: 0x0, access type: RO
		uint32 preAggUsrRptFifoDecrementLessThanZero : 1; //PRE AGG USER REPORT  FIFO amount has been decremented less than zero, reset value: 0x0, access type: RO
		uint32 reserved1 : 5;
		uint32 preAggFifoCommonUserSmDbg : 3; //fifo_common_user_shram_logic state machine, reset value: 0x0, access type: RO
		uint32 reserved2 : 5;
	} bitFields;
} RegPreAggPreAggUsrRptFifoDebug_u;

/*REG_PRE_AGG_PRE_AGG_USR_RPT_FIFO_DEBUG2 0x2F0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 preAggFirstUsrRptFifoFullDropCtr : 10; //PRE AGG USER REPORT  FIFO full drop counter on first user, reset value: 0x0, access type: RO
		uint32 reserved0 : 6;
		uint32 preAggNonFirstUsrRptFifoFullDropCtr : 10; //PRE AGG USER REPORT  FIFO full drop counter on NON first user, reset value: 0x0, access type: RO
		uint32 reserved1 : 6;
	} bitFields;
} RegPreAggPreAggUsrRptFifoDebug2_u;

/*REG_PRE_AGG_MIN_BYTES_PER_USER_CFG 0x2F4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 minBytesPerUserCfg : 14; //min_bytes_per_user_cfg, reset value: 0x0, access type: RW
		uint32 reserved0 : 18;
	} bitFields;
} RegPreAggMinBytesPerUserCfg_u;

/*REG_PRE_AGG_SEL_BW_FIFO_DBG_STATUS 0x2F8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 selBwNumBytesInFifo : 2; //selector BW stage syn fifo debug - num of bytes in fifo status, reset value: 0x0, access type: RO
		uint32 reserved0 : 30;
	} bitFields;
} RegPreAggSelBwFifoDbgStatus_u;

/*REG_PRE_AGG_USE_UL_INDICATION_FOR_EOSP_CFG 0x2FC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 useUlIndicationForEospCfg : 1; //no description, reset value: 0x1, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegPreAggUseUlIndicationForEospCfg_u;

/*REG_PRE_AGG_HANDLE_OMI_BW 0x300 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 omiHeMuReplaceUpdateUserToDummyCfg : 1; //omi_he_mu_replace_update_user_to_dummy_cfg, reset value: 0x1, access type: RW
		uint32 omiHe20MhzRemoveUserSpecificRu : 2; //omi_he_20mhz_remove_user_specific_ru: , 0 - disabled , 1 - remove user , 2 - remove user specific RU, reset value: 0x2, access type: RW
		uint32 omiHe40MhzRemoveUserSpecificRu : 2; //omi_he_40mhz_remove_user_specific_ru: , 0 - disabled , 1 - remove user , 2 - remove user specific RU, reset value: 0x1, access type: RW
		uint32 muMimoUpdateNssToIternalMinNssCfg : 1; //mu_mimo_update_nss_to_iternal_min_nss_cfg, reset value: 0x0, access type: RW
		uint32 reserved0 : 26;
	} bitFields;
} RegPreAggHandleOmiBw_u;

/*REG_PRE_AGG_PRE_AGG_SKIP_PHASE_CONTROL 0x304 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 skipPhaseThresholdCfg : 4; //thershhold for number of skip phase done before doing TXOP abort, reset value: 0x2, access type: RW
		uint32 reserved0 : 28;
	} bitFields;
} RegPreAggPreAggSkipPhaseControl_u;

/*REG_PRE_AGG_PRE_AGG_DEBUG_STATUS1 0x308 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txopCanceledStatusBus : 15; //bit[0] no users valid from selector , bit[1] prev user for primary user not locked , bit[2] prev TID for ptimary tid not locked , bit[3] first PD pointer Null , bit[4] sequencer drop primary user , bit[5] reactivation and primary PD not data , bit[6] TXOP aborted due to OTF mode change , bit[7] primary user empty selection , bit[8] bf_rpt_analyzer_required_ra_filler , bit[9] OTFA can't build , bit[10] empty plan , bit[11] empty phase , bit[12] he minimum user threshold , bit[13] STA compress capability not support full BW , bit[14] STA removed due to TWT SP remaining duration, reset value: 0x0, access type: RO
		uint32 reserved0 : 1;
		uint32 dlDataMimoInd : 1; //DL mimo indication, reset value: 0x0, access type: RO
		uint32 phaseType : 3; //PRE_AGG_PROTECTION_PHASE     = 3'd0; , PRE_AGG_SOUNDING_PHASE       = 3'd1; , PRE_AGG_DL_DATA_PHASE        = 3'd2; , PRE_AGG_UL_DATA_PHASE        = 3'd3;, reset value: 0x0, access type: RO
		uint32 reactivateMode : 1; //0 - activation; 1 - reactivation, reset value: 0x0, access type: RO
		uint32 dlDataTxType : 2; //PRE_AGG_DL_DATA_IMMEDIATE    = 2'd0; , PRE_AGG_DL_DATA_VHT_ALIKE    = 2'd1; , PRE_AGG_DL_DATA_MU_BAR       = 2'd2;  , reset value: 0x0, access type: RO
		uint32 tfMode : 2; //{PRE_AGG_TF_MODE_NO_TF=2'd0, PRE_AGG_TF_MODE_RESERVE=2'd1, PRE_AGG_TF_MODE_BROADCAST=2'd2, PRE_AGG_TF_MODE_UNICAST=2'd3, reset value: 0x0, access type: RO
		uint32 tfParamTriggerType : 3; //0 - basic , 1 - bf_rpt_poll , 2 - mu_bar , 3 - mu_rts , 4 - buffer_status_rpt, reset value: 0x0, access type: RO
		uint32 numTids : 4; //numer of selected TIDs (0-9), reset value: 0x0, access type: RO
	} bitFields;
} RegPreAggPreAggDebugStatus1_u;

/*REG_PRE_AGG_PRE_AGG_DEBUG_STATUS2 0x30C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 finalBw : 2; //final BW to STD, reset value: 0x0, access type: RO
		uint32 tcrAFinalBw : 2; //final BW to TCR A, reset value: 0x0, access type: RO
		uint32 tcrBFinalBw : 2; //final BW to TCR B, reset value: 0x0, access type: RO
		uint32 reserved0 : 2;
		uint32 userTcrAValid : 1; //user TCR A is valid, reset value: 0x0, access type: RO
		uint32 userTcrBValid : 1; //user TCR B is valid, reset value: 0x0, access type: RO
		uint32 userRcrValid : 1; //user RCR is valid, reset value: 0x0, access type: RO
		uint32 commonTcrAValid : 1; //common TCR A is valid, reset value: 0x0, access type: RO
		uint32 commonTcrBValid : 1; //common TCR B is valid, reset value: 0x0, access type: RO
		uint32 commonRcrValid : 1; //common RCR is valid, reset value: 0x0, access type: RO
		uint32 bcUserTcrAValid : 1; //bc user TCR A is valid, reset value: 0x0, access type: RO
		uint32 bcUserTcrBValid : 1; //bc user TCR B is valid, reset value: 0x0, access type: RO
		uint32 skipPhaseCounter : 4; //counts number of skip phases event made in TXOP, reset value: 0x0, access type: RO
		uint32 internalSkipPhaseInd : 1; //1 - last phase was skipped. 0 - last phase was not skipped, reset value: 0x0, access type: RO
		uint32 reserved1 : 11;
	} bitFields;
} RegPreAggPreAggDebugStatus2_u;

/*REG_PRE_AGG_PRE_AGG_RPT_FIFO_EN 0x310 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 preAggRptFifoEn : 1; //PRE AGG REPORT FIFO enable. This bit enables both user and common fifo reprot, reset value: 0x1, access type: RW
		uint32 preAggCmnRptFifoNotEmptyEn : 1; //PRE AGG COMMON REPORT FIFO not empty interrupt enable, reset value: 0x0, access type: RW
		uint32 preAggUsrRptFifoNotEmptyEn : 1; //PRE AGG USER REPORT FIFO not empty interrupt enable, reset value: 0x0, access type: RW
		uint32 reserved0 : 29;
	} bitFields;
} RegPreAggPreAggRptFifoEn_u;

/*REG_PRE_AGG_TF_PADDING_MULT_FACTOR_CFG 0x314 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tfPaddingMultFactorCfg : 2; //tf_padding_mult_factor_cfg for HE: , When 0: , 	if TF Padding = 8 usec then TF_factor = 1.5 , 	if TF Padding = 16 usec then TF_factor = 2.5 , when 1: , 	if TF Padding = 8 usec then TF_factor = 2 , 	if TF Padding = 16 usec then TF_factor = 3, reset value: 0x1, access type: RW
		uint32 reserved0 : 30;
	} bitFields;
} RegPreAggTfPaddingMultFactorCfg_u;

/*REG_PRE_AGG_HE_MU_KEEP_USER_CFG 0x318 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 heMuKeepUserCfg : 1; //he_mu_keep_user_cfg, reset value: 0x1, access type: RW
		uint32 heMuUpdateDlMimoBaCfg : 1; //he_mu_update_dl_mimo_ba_cfg, reset value: 0x1, access type: RW
		uint32 heMuUpdateUlMimoTfBaCfg : 1; //he_mu_update_ul_mimo_tf_ba_cfg, reset value: 0x1, access type: RW
		uint32 reserved0 : 29;
	} bitFields;
} RegPreAggHeMuKeepUserCfg_u;

/*REG_PRE_AGG_PRIMARY_AC_MNG_MAP 0x31C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 primaryAcMngMap : 2; //Primary AC Management Mapping Configuration defined to which AC the management is mapped (should be set the same as in TXH), reset value: 0x3, access type: RW
		uint32 reserved0 : 30;
	} bitFields;
} RegPreAggPrimaryAcMngMap_u;

/*REG_PRE_AGG_MAX_SU_PPDU_DURATION_LIMIT_1 0x320 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 maxSuPpduDurationLimitBe : 13; //Maximum SU PPDU Duration Limit used by Pre-AGG for calculating Maximum PSDU length Limit when Primary AC=BE is selected , Resolution is 2us , , reset value: 0x1fff, access type: RW
		uint32 reserved0 : 3;
		uint32 maxSuPpduDurationLimitBk : 13; //Maximum SU PPDU Duration Limit used by Pre-AGG for calculating Maximum PSDU length Limit when Primary AC=BK is selected , Resolution is 2us, reset value: 0x1fff, access type: RW
		uint32 reserved1 : 3;
	} bitFields;
} RegPreAggMaxSuPpduDurationLimit1_u;

/*REG_PRE_AGG_MAX_SU_PPDU_DURATION_LIMIT_2 0x324 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 maxSuPpduDurationLimitVo : 13; //Maximum SU PPDU Duration Limit used by Pre-AGG for calculating Maximum PSDU length Limit when Primary AC=VO is selected , Resolution is 2us, reset value: 0x1fff, access type: RW
		uint32 reserved0 : 3;
		uint32 maxSuPpduDurationLimitVi : 13; //Maximum SU PPDU Duration Limit used by Pre-AGG for calculating Maximum PSDU length Limit when Primary AC=VI is selected , Resolution is 2us, reset value: 0x1fff, access type: RW
		uint32 reserved1 : 3;
	} bitFields;
} RegPreAggMaxSuPpduDurationLimit2_u;

/*REG_PRE_AGG_MAX_VHT_MU_PPDU_DURATION_LIMIT_1 0x328 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 maxVhtMuPpduDurationLimitBe : 13; //Maximum VHT MU PPDU Duration Limit used by Pre-AGG for calculating Maximum PSDU length Limit when Primary AC=BE is selected , Resolution is 2us , , reset value: 0x1fff, access type: RW
		uint32 reserved0 : 3;
		uint32 maxVhtMuPpduDurationLimitBk : 13; //Maximum VHT MU PPDU Duration Limit used by Pre-AGG for calculating Maximum PSDU length Limit when Primary AC=BK is selected , Resolution is 2us, reset value: 0x1fff, access type: RW
		uint32 reserved1 : 3;
	} bitFields;
} RegPreAggMaxVhtMuPpduDurationLimit1_u;

/*REG_PRE_AGG_MAX_VHT_MU_PPDU_DURATION_LIMIT_2 0x32C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 maxVhtMuPpduDurationLimitVo : 13; //Maximum VHT MU PPDU Duration Limit used by Pre-AGG for calculating Maximum PSDU length Limit when Primary AC=VO is selected , Resolution is 2us, reset value: 0x1fff, access type: RW
		uint32 reserved0 : 3;
		uint32 maxVhtMuPpduDurationLimitVi : 13; //Maximum VHT MU PPDU Duration Limit used by Pre-AGG for calculating Maximum PSDU length Limit when Primary AC=VI is selected , Resolution is 2us, reset value: 0x1fff, access type: RW
		uint32 reserved1 : 3;
	} bitFields;
} RegPreAggMaxVhtMuPpduDurationLimit2_u;

/*REG_PRE_AGG_MAX_HE_MU_DL_PPDU_DURATION_LIMIT_1 0x330 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 maxHeMuDlPpduDurationLimitBe : 13; //Maximum HE MU DL PPDU Duration Limit used by Pre-AGG for calculating Maximum PSDU length Limit when Primary AC=BE is selected , Resolution is 2us , , reset value: 0x1fff, access type: RW
		uint32 reserved0 : 3;
		uint32 maxHeMuDlPpduDurationLimitBk : 13; //Maximum HE MU DL PPDU Duration Limit used by Pre-AGG for calculating Maximum PSDU length Limit when Primary AC=BK is selected , Resolution is 2us, reset value: 0x1fff, access type: RW
		uint32 reserved1 : 3;
	} bitFields;
} RegPreAggMaxHeMuDlPpduDurationLimit1_u;

/*REG_PRE_AGG_MAX_HE_MU_DL_PPDU_DURATION_LIMIT_2 0x334 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 maxHeMuDlPpduDurationLimitVo : 13; //Maximum HE MU DL PPDU Duration Limit used by Pre-AGG for calculating Maximum PSDU length Limit when Primary AC=VO is selected , Resolution is 2us, reset value: 0x1fff, access type: RW
		uint32 reserved0 : 3;
		uint32 maxHeMuDlPpduDurationLimitVi : 13; //Maximum HE MU DL PPDU Duration Limit used by Pre-AGG for calculating Maximum PSDU length Limit when Primary AC=VI is selected , Resolution is 2us, reset value: 0x1fff, access type: RW
		uint32 reserved1 : 3;
	} bitFields;
} RegPreAggMaxHeMuDlPpduDurationLimit2_u;

/*REG_PRE_AGG_REG_DATA_AIR_TIME_CALC_RESULT 0x338 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 regDataAirTimeCalcResult : 17; //reg_data_air_time_calc_result, reset value: 0x0, access type: RO
		uint32 reserved0 : 15;
	} bitFields;
} RegPreAggRegDataAirTimeCalcResult_u;

/*REG_PRE_AGG_MIN_PSDU_LENGTH_OTF_CONF 0x33C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 minPsduLengthOtfConf : 10; //no description, reset value: 0x10, access type: RW
		uint32 reserved0 : 22;
	} bitFields;
} RegPreAggMinPsduLengthOtfConf_u;

/*REG_PRE_AGG_MU_MIMO_UPDATE_LDPC_ALL_USERS 0x340 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muMimoUpdateLdpcAllUsers : 1; //no description, reset value: 0x1, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegPreAggMuMimoUpdateLdpcAllUsers_u;

/*REG_PRE_AGG_SU_FORCE_MORE_DATA_AND_EOSP 0x344 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 suForceMoreDataAndEosp : 2; //0 - Calc EOSP and More Data according to the current definitions , 1 - Set More Data = 1 an EOSP = 0 , 2 - Set More Data = 0 and EOSP = 1 , note that this configuration is for all SU (including HE SU) + TWT users, reset value: 0x0, access type: RW
		uint32 reserved0 : 30;
	} bitFields;
} RegPreAggSuForceMoreDataAndEosp_u;

/*REG_PRE_AGG_HE_TB_RR_INIT_PRIMARY_TID_CFG 0x348 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 heTbRrInitPrimaryTidCfg : 3; //When the he_tb_rr_init_primary_tid is configured, it sets the next Primary TID Round Robin Value In HE TB Mode that will be used as the primary tid field in selector reselection I/F   , Reset Value / Default Value = 0 , Values [0 Ã¢ÂÂ 7] for TID0, TID1, Ã¢ÂÂ¦ ,TID7, reset value: 0x0, access type: RW
		uint32 reserved0 : 29;
	} bitFields;
} RegPreAggHeTbRrInitPrimaryTidCfg_u;

/*REG_PRE_AGG_HE_TB_PRIMARY_TID_SELECTION_CFG 0x34C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 heTbPrimaryTidSelectionCfg : 2; //HE TB Mode Primary TID Selection Configuration -  define how to select the primary TID , 0 Ã¢ÂÂ Preferred AC - select primary TID based on Preferred AC received from TF , 1 Ã¢ÂÂ Round Robin - select primary TID based on Selector TID selection, using round robin  , 2 Ã¢ÂÂ highest AC Ã¢ÂÂ select primary TID based on the highest AC , Default = 1 , , reset value: 0x1, access type: RW
		uint32 reserved0 : 30;
	} bitFields;
} RegPreAggHeTbPrimaryTidSelectionCfg_u;

/*REG_PRE_AGG_HE_TB_MODE_CONTROL_CFG 0x350 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 heTbEnableSelectorAControlCfg : 1; //HE TB Mode Enable Selector A-Control Configuration -  Enable inserting A-Control from STA DB in HE TB mode into Header HT Control , reset value: 0x1, access type: RW
		uint32 heTbEnableUphControlDataCfg : 1; //Enable UPH Control in UL Data TB Mode -  Enable/Disable inserting A-Control in UL Data TB Mode  , 0 Ã¢ÂÂ Disable (not adding A-Control) , 1 Ã¢ÂÂ Enable Ã¢ÂÂ add A-Control if there is OMI or UPH control , Default = 1 ,  , , reset value: 0x1, access type: RW
		uint32 heTbEnableUphControlBsrpCfg : 1; //Enable UPH Control in UL BSRP Mode -  Enable/Disable inserting A-Control in BSRP TB Mode  , 0 Ã¢ÂÂ Disable (not adding A-Control) , 1 Ã¢ÂÂ Enable Ã¢ÂÂ add A-Control if there is OMI or UPH control , Default = 1 ,  ,  , , reset value: 0x1, access type: RW
		uint32 heTbEnableUphControlBackCfg : 1; //Enable UPH Control in UL BA Mode -  Enable/Disable inserting A-Control in BA TB Mode  , 0 Ã¢ÂÂ Disable (not adding A-Control) , 1 Ã¢ÂÂ Enable Ã¢ÂÂ add A-Control if there is OMI or UPH control , Default = 0 ,  ,  ,  , , reset value: 0x0, access type: RW
		uint32 heTbAControlFillerValCfg : 1; //HE TB Mode A-Control Filler Value Configuration -  defines how to fill the remaining A-Control bits   ,  , 0 - fill with 0 the remaining a-control bits , 1 - fill with 1 the remaining a-control bits , Default = 1, reset value: 0x1, access type: RW
		uint32 reserved0 : 27;
	} bitFields;
} RegPreAggHeTbModeControlCfg_u;

/*REG_PRE_AGG_HE_TB_MAX_BYTE_COUNTER_VAL_CFG 0x354 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 heTbMaxByteCounterValCfg : 22; //HE TB Mode Max Byte Counter Value Configuration -  when Byte Counter value > HE TB Mode Max Byte Counter Value Pre-AGG shall set the Byte Counter to Max Value (he_tb_max_byte_counter_set_cfg) ,  , Default = 2 147 328, reset value: 0x20C400, access type: RW
		uint32 reserved0 : 10;
	} bitFields;
} RegPreAggHeTbMaxByteCounterValCfg_u;

/*REG_PRE_AGG_HE_TB_MAX_BYTE_COUNTER_SET_CFG 0x358 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 heTbMaxByteCounterSetCfg : 22; //Max Byte Counter Value Configuration define the Max Value that Pre-AGG shall set when Byte Counter value > HE TB Mode Max Byte Counter Value ,  , Default = 2^22-2, reset value: 0x3FFFFE, access type: RW
		uint32 reserved0 : 10;
	} bitFields;
} RegPreAggHeTbMaxByteCounterSetCfg_u;

/*REG_PRE_AGG_HE_TB_B_FACTOR_COEF_CFG 0x35C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 heTbBFactorTid0Cfg : 3; //b Factor Coefficient per TID Configuration: , Byte Counter Factor Coefficient per TID  , b factor options are:  , 0 - 0, , 1 â 1 , 2 - 1.125 (+12.5%)  , 3 - 1.25 (+25%)  , 4 - 1.5 (+50%)  , 5 - 2(+100%) }  , 6,7 â reserved , Default = 1, reset value: 0x1, access type: RW
		uint32 reserved0 : 1;
		uint32 heTbBFactorTid1Cfg : 3; //b Factor Coefficient per TID Configuration: , Byte Counter Factor Coefficient per TID  , b factor options are:  , 0 - 0, , 1 â 1 , 2 - 1.125 (+12.5%)  , 3 - 1.25 (+25%)  , 4 - 1.5 (+50%)  , 5 - 2(+100%) }  , 6,7 â reserved , Default = 1, reset value: 0x1, access type: RW
		uint32 reserved1 : 1;
		uint32 heTbBFactorTid2Cfg : 3; //b Factor Coefficient per TID Configuration: , Byte Counter Factor Coefficient per TID  , b factor options are:  , 0 - 0, , 1 â 1 , 2 - 1.125 (+12.5%)  , 3 - 1.25 (+25%)  , 4 - 1.5 (+50%)  , 5 - 2(+100%) }  , 6,7 â reserved , Default = 1, reset value: 0x1, access type: RW
		uint32 reserved2 : 1;
		uint32 heTbBFactorTid3Cfg : 3; //b Factor Coefficient per TID Configuration: , Byte Counter Factor Coefficient per TID  , b factor options are:  , 0 - 0, , 1 â 1 , 2 - 1.125 (+12.5%)  , 3 - 1.25 (+25%)  , 4 - 1.5 (+50%)  , 5 - 2(+100%) }  , 6,7 â reserved , Default = 1, reset value: 0x1, access type: RW
		uint32 reserved3 : 1;
		uint32 heTbBFactorTid4Cfg : 3; //b Factor Coefficient per TID Configuration: , Byte Counter Factor Coefficient per TID  , b factor options are:  , 0 - 0, , 1 â 1 , 2 - 1.125 (+12.5%)  , 3 - 1.25 (+25%)  , 4 - 1.5 (+50%)  , 5 - 2(+100%) }  , 6,7 â reserved , Default = 1, reset value: 0x1, access type: RW
		uint32 reserved4 : 1;
		uint32 heTbBFactorTid5Cfg : 3; //b Factor Coefficient per TID Configuration: , Byte Counter Factor Coefficient per TID  , b factor options are:  , 0 - 0, , 1 â 1 , 2 - 1.125 (+12.5%)  , 3 - 1.25 (+25%)  , 4 - 1.5 (+50%)  , 5 - 2(+100%) }  , 6,7 â reserved , Default = 1, reset value: 0x1, access type: RW
		uint32 reserved5 : 1;
		uint32 heTbBFactorTid6Cfg : 3; //b Factor Coefficient per TID Configuration: , Byte Counter Factor Coefficient per TID  , b factor options are:  , 0 - 0, , 1 â 1 , 2 - 1.125 (+12.5%)  , 3 - 1.25 (+25%)  , 4 - 1.5 (+50%)  , 5 - 2(+100%) }  , 6,7 â reserved , Default = 1, reset value: 0x1, access type: RW
		uint32 reserved6 : 1;
		uint32 heTbBFactorTid7Cfg : 3; //b Factor Coefficient per TID Configuration: , Byte Counter Factor Coefficient per TID  , b factor options are:  , 0 - 0, , 1 â 1 , 2 - 1.125 (+12.5%)  , 3 - 1.25 (+25%)  , 4 - 1.5 (+50%)  , 5 - 2(+100%) }  , 6,7 â reserved , Default = 1, reset value: 0x1, access type: RW
		uint32 reserved7 : 1;
	} bitFields;
} RegPreAggHeTbBFactorCoefCfg_u;

/*REG_PRE_AGG_HE_TB_OFFSET_TID0_FACTOR_CFG 0x360 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 heTbOffsetTid0FactorCfg : 22; //Offset Factor Coefficient per TID Configuration: , 22-bit unsigned Offset  , Used in the buffer status report calculation  ,  Offset[TID=0] + floor(b x Byte Counter[TID=0]) , Default = 0, reset value: 0x0, access type: RW
		uint32 reserved0 : 10;
	} bitFields;
} RegPreAggHeTbOffsetTid0FactorCfg_u;

/*REG_PRE_AGG_HE_TB_OFFSET_TID1_FACTOR_CFG 0x364 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 heTbOffsetTid1FactorCfg : 22; //Offset Factor Coefficient per TID Configuration: , 22-bit unsigned Offset  , Used in the buffer status report calculation  ,  Offset[TID=1] + floor(b x Byte Counter[TID=0]) , Default = 0, reset value: 0x0, access type: RW
		uint32 reserved0 : 10;
	} bitFields;
} RegPreAggHeTbOffsetTid1FactorCfg_u;

/*REG_PRE_AGG_HE_TB_OFFSET_TID2_FACTOR_CFG 0x368 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 heTbOffsetTid2FactorCfg : 22; //Offset Factor Coefficient per TID Configuration: , 22-bit unsigned Offset  , Used in the buffer status report calculation  ,  Offset[TID=2] + floor(b x Byte Counter[TID=0]) , Default = 0, reset value: 0x0, access type: RW
		uint32 reserved0 : 10;
	} bitFields;
} RegPreAggHeTbOffsetTid2FactorCfg_u;

/*REG_PRE_AGG_HE_TB_OFFSET_TID3_FACTOR_CFG 0x36C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 heTbOffsetTid3FactorCfg : 22; //Offset Factor Coefficient per TID Configuration: , 22-bit unsigned Offset  , Used in the buffer status report calculation  ,  Offset[TID=3] + floor(b x Byte Counter[TID=0]) , Default = 0, reset value: 0x0, access type: RW
		uint32 reserved0 : 10;
	} bitFields;
} RegPreAggHeTbOffsetTid3FactorCfg_u;

/*REG_PRE_AGG_HE_TB_OFFSET_TID4_FACTOR_CFG 0x370 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 heTbOffsetTid4FactorCfg : 22; //Offset Factor Coefficient per TID Configuration: , 22-bit unsigned Offset  , Used in the buffer status report calculation  ,  Offset[TID=4] + floor(b x Byte Counter[TID=0]) , Default = 0, reset value: 0x0, access type: RW
		uint32 reserved0 : 10;
	} bitFields;
} RegPreAggHeTbOffsetTid4FactorCfg_u;

/*REG_PRE_AGG_HE_TB_OFFSET_TID5_FACTOR_CFG 0x374 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 heTbOffsetTid5FactorCfg : 22; //Offset Factor Coefficient per TID Configuration: , 22-bit unsigned Offset  , Used in the buffer status report calculation  ,  Offset[TID=5] + floor(b x Byte Counter[TID=0]) , Default = 0, reset value: 0x0, access type: RW
		uint32 reserved0 : 10;
	} bitFields;
} RegPreAggHeTbOffsetTid5FactorCfg_u;

/*REG_PRE_AGG_HE_TB_OFFSET_TID6_FACTOR_CFG 0x378 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 heTbOffsetTid6FactorCfg : 22; //Offset Factor Coefficient per TID Configuration: , 22-bit unsigned Offset  , Used in the buffer status report calculation  ,  Offset[TID=6] + floor(b x Byte Counter[TID=0]) , Default = 0, reset value: 0x0, access type: RW
		uint32 reserved0 : 10;
	} bitFields;
} RegPreAggHeTbOffsetTid6FactorCfg_u;

/*REG_PRE_AGG_HE_TB_OFFSET_TID7_FACTOR_CFG 0x37C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 heTbOffsetTid7FactorCfg : 22; //Offset Factor Coefficient per TID Configuration: , 22-bit unsigned Offset  , Used in the buffer status report calculation  ,  Offset[TID=7] + floor(b x Byte Counter[TID=0]) , Default = 0, reset value: 0x0, access type: RW
		uint32 reserved0 : 10;
	} bitFields;
} RegPreAggHeTbOffsetTid7FactorCfg_u;

/*REG_PRE_AGG_HE_TB_UNSPECIFIED_BYTE_COUNTER_VAP 0x380 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 heTbUnspecifiedByteCounterVap : 32; //HE TB Mode Unspecified Byte Counter Configuration for each value: , Bit 0 for VAP 0 , Bit 1 for VAP 1 , Bit 2 for VAP 2 , :  , Bit 31 for VAP 31  , 0 â Specified value , 1 â Unspecified Value , Default value 0 for all VAPs, reset value: 0x0, access type: RW
	} bitFields;
} RegPreAggHeTbUnspecifiedByteCounterVap_u;

/*REG_PRE_AGG_PROTECTED_BEACON_VAP 0x384 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 protectedBeaconVap : 32; //In case of PD type Beacon, enable taking beacon security parameters from VAP DB. 1 bit per VAP., reset value: 0x0, access type: RW
	} bitFields;
} RegPreAggProtectedBeaconVap_u;

/*REG_PRE_AGG_MIN_TWT_REM_SP_DUR_THR0 0x388 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 minTwtRemSpDurThrProtection : 8; //Threshold for Protection phase which defines the remaining TWT SP minimum time for running the phase , Resolution is 64us, reset value: 0x8, access type: RW
		uint32 reserved0 : 8;
		uint32 minTwtRemSpDurThrSounding : 8; //Threshold for Sounding phase which defines the remaining TWT SP minimum time for running the phase , Resolution is 64us, reset value: 0x10, access type: RW
		uint32 reserved1 : 8;
	} bitFields;
} RegPreAggMinTwtRemSpDurThr0_u;

/*REG_PRE_AGG_MIN_TWT_REM_SP_DUR_THR1 0x38C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 minTwtRemSpDurThrUlData : 8; //Threshold for UL data phase which defines the remaining TWT SP minimum time for running the phase , Resolution is 64us, reset value: 0x10, access type: RW
		uint32 reserved0 : 8;
		uint32 minTwtRemSpDurThrBsrp : 8; //Threshold for BSRP phase which defines the remaining TWT SP minimum time for running the phase , Resolution is 64us, reset value: 0x8, access type: RW
		uint32 reserved1 : 8;
	} bitFields;
} RegPreAggMinTwtRemSpDurThr1_u;

/*REG_PRE_AGG_MIN_TWT_REM_SP_DUR_THR2 0x390 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 minTwtRemSpDurThrDlData : 8; //Threshold for DL phase which defines the remaining TWT SP minimum time for running the phase , Resolution is 64us, reset value: 0x10, access type: RW
		uint32 reserved0 : 24;
	} bitFields;
} RegPreAggMinTwtRemSpDurThr2_u;

/*REG_PRE_AGG_DELTA_TIME_FROM_END_TWT_SP 0x394 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dlDeltaTimeFromEndTwtSp : 8; //In DL HE MU with TF basic, when reading the TWT SP remaining time Time, need to reduce from it time for SIFS + BA and extra. , Using this configuration its guarantee that even the BA will not exceed the TWT SP , Resolution is 64us, reset value: 0x2, access type: RW
		uint32 reserved0 : 8;
		uint32 ulDeltaTimeFromEndTwtSp : 8; //In UL HE MU, when reading the TWT SP remaining time Time, need to reduce from it time for TF + SIFS + SIFS + MBA and some extra. , Using this configuration its guarantee that even the BA will not exceed the TWT SP , Resolution is 64us, reset value: 0x3, access type: RW
		uint32 reserved1 : 8;
	} bitFields;
} RegPreAggDeltaTimeFromEndTwtSp_u;

/*REG_PRE_AGG_CLR_PLAN_IGNORE_UL_DATA_START_STRB 0x398 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 clrPlanIgnoreUlDataStartStrb : 1; //Activate operation of clear plan ignore UL data in Queue, reset value: 0x0, access type: WO
		uint32 reserved0 : 31;
	} bitFields;
} RegPreAggClrPlanIgnoreUlDataStartStrb_u;

/*REG_PRE_AGG_CLR_PLAN_IGNORE_UL_DATA_DONE 0x39C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 clrPlanIgnoreUlDataDone : 1; //Done indication of clear plan ignore UL data in Queue, reset value: 0x1, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegPreAggClrPlanIgnoreUlDataDone_u;

/*REG_PRE_AGG_MAX_HE_MU_UL_PPDU_DURATION_LIMIT_1 0x3A0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 maxHeMuUlPpduDurationLimitBe : 13; //Maximum HE MU UL PPDU Duration Limit used by Pre-AGG for calculating Maximum PSDU length Limit when Primary AC=BE is selected , Resolution is 2us , , reset value: 0x1fff, access type: RW
		uint32 reserved0 : 3;
		uint32 maxHeMuUlPpduDurationLimitBk : 13; //Maximum HE MU UL PPDU Duration Limit used by Pre-AGG for calculating Maximum PSDU length Limit when Primary AC=BK is selected , Resolution is 2us, reset value: 0x1fff, access type: RW
		uint32 reserved1 : 3;
	} bitFields;
} RegPreAggMaxHeMuUlPpduDurationLimit1_u;

/*REG_PRE_AGG_MAX_HE_MU_UL_PPDU_DURATION_LIMIT_2 0x3A4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 maxHeMuUlPpduDurationLimitVo : 13; //Maximum HE MU UL PPDU Duration Limit used by Pre-AGG for calculating Maximum PSDU length Limit when Primary AC=VO is selected , Resolution is 2us, reset value: 0x1fff, access type: RW
		uint32 reserved0 : 3;
		uint32 maxHeMuUlPpduDurationLimitVi : 13; //Maximum HE MU UL PPDU Duration Limit used by Pre-AGG for calculating Maximum PSDU length Limit when Primary AC=VI is selected , Resolution is 2us, reset value: 0x1fff, access type: RW
		uint32 reserved1 : 3;
	} bitFields;
} RegPreAggMaxHeMuUlPpduDurationLimit2_u;

/*REG_PRE_AGG_MIN_TWT_REM_SP_DUR_THR_SU 0x3A8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 minTwtRemSpDurThrSu : 8; //In txop TYPE SU defines the remaining TWT SP minimum time  , Resolution is 64us, reset value: 0x10, access type: RW
		uint32 reserved0 : 8;
		uint32 minTwtRemSpDurThrSuBfRpt : 8; //In txop TYPE SU defines the remaining TWT SP minimum time for BF sequence , Resolution is 64us, reset value: 0x20, access type: RW
		uint32 reserved1 : 8;
	} bitFields;
} RegPreAggMinTwtRemSpDurThrSu_u;

/*REG_PRE_AGG_SU_DELTA_TIME_FROM_END_TWT_SP 0x3AC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 suDeltaTimeFromEndTwtSp : 8; //In SU, when reading the TWT SP remaining time Time, need to reduce from it time for SIFS + BA and extra. , Using this configuration its guarantee that even the BA will not exceed the TWT SP , Resolution is 64us, reset value: 0x2, access type: RW
		uint32 reserved0 : 24;
	} bitFields;
} RegPreAggSuDeltaTimeFromEndTwtSp_u;

/*REG_PRE_AGG_SU_TWT_REM_SP_DUR_GLOBAL_EN 0x3B0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 suTwtRemSpDurGlobalEn : 1; //su global enable pre agg questioning twt module for twt sp remaining duration, reset value: 0x0, access type: RW
		uint32 suStdSelectedTimeBasedSeqReactDueToTwtEn : 1; //Enable setting selected_time_based_seq_react STD field when TWT remaining duration limits the PPDU duration. , This configuration is for SU. , It's used by OTFA to not generate error in case of PSDU length limit., reset value: 0x0, access type: RW
		uint32 heMuStdSelectedTimeBasedSeqReactDueToTwtEn : 1; //Enable setting selected_time_based_seq_react STD field when TWT remaining duration limits the PPDU duration. , This configuration is for HE MU. , It's used by OTFA to not generate error in case of PSDU length limit., reset value: 0x0, access type: RW
		uint32 reserved0 : 29;
	} bitFields;
} RegPreAggSuTwtRemSpDurGlobalEn_u;

/*REG_PRE_AGG_MAX_PSDU_SIZE_HE_UL 0x3B4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 maxPsduSizeHeUl : 23; //max PSDU size in HE for UL data, reset value: 0x3fffff, access type: RW
		uint32 reserved0 : 9;
	} bitFields;
} RegPreAggMaxPsduSizeHeUl_u;

/*REG_PRE_AGG_PSDU_BYTE_LEN_CALC_PARAMS2 0x3B8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 extraBytesForMpduLowNumMpdusCfg : 8; //Number of bytes to add per MPDU in case of low number of MPDUs (up to 4). Used to compensate the missing bytes due to 64bytes resolution of the byte counters. , Common for all the STAs., reset value: 0x20, access type: RW
		uint32 reserved0 : 24;
	} bitFields;
} RegPreAggPsduByteLenCalcParams2_u;

/*REG_PRE_AGG_MULTI_TID_CONTROL 0x3BC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 qosNullMultiTidAgg : 1; //Define if PD Type of QoS null can be multi-TID Aggregate , 0 – Disable – QoS Null cannot be aggregate in multi-tid (sent as single TID) , 1 – Enable – QoS Null can be aggregate in Multi-TID Aggregation , , reset value: 0x1, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegPreAggMultiTidControl_u;

/*REG_PRE_AGG_TWT_REM_SP_DUR_RESULT 0x3C0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 twtEndOfSpValue : 21; //TWT remaining end of service period result, 1[us] resolution, reset value: 0x0, access type: RO
		uint32 reserved0 : 3;
		uint32 twtIfReqWasActive : 1; //TWT IF request was active, reset value: 0x0, access type: RO
		uint32 reserved1 : 7;
	} bitFields;
} RegPreAggTwtRemSpDurResult_u;

/*REG_PRE_AGG_DELTA_TIME_FROM_END_TWT_SP2 0x3C4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dlMuBarDeltaTimeFromEndTwtSp : 8; //In DL HE MU with TF MU BAR, when reading the TWT SP remaining time Time, need to reduce from it time for SIFS + TF MU BAR + SIFS + BA and extra. , Using this configuration its guarantee that even the BA will not exceed the TWT SP , Resolution is 64us, reset value: 0x5, access type: RW
		uint32 reserved0 : 8;
		uint32 dlVhtAlikeDeltaTimeFromEndTwtSp : 8; //In DL HE MU VHT Alike, when reading the TWT SP remaining time Time, need to reduce from it time for SIFS + BARs + BA and extra. , Using this configuration its guarantee that even the BA will not exceed the TWT SP , Resolution is 64us, reset value: 0x8, access type: RW
		uint32 reserved1 : 8;
	} bitFields;
} RegPreAggDeltaTimeFromEndTwtSp2_u;

/*REG_PRE_AGG_PRE_AGG_SPARE 0x3FC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 preAggSpare : 32; //Spare registers, reset value: 0x0, access type: RW
	} bitFields;
} RegPreAggPreAggSpare_u;



#endif // _PRE_AGG_REGS_H_
