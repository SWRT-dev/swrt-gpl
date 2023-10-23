/**
 * qos_wred_regs.h
 * Description: qos_wred_s HW registers definitions
 * Auto-generated file by RDL Parser, DO NOT CHANGE
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2019-2020 Intel Corporation
 */

#ifndef _PP_QOS_WRED_H_
#define _PP_QOS_WRED_H_

#define PP_QOS_WRED_GEN_DATE_STR              "Sunday May 17, 2020 [5:26:11 pm]"
#define PP_QOS_WRED_BASE                                         (0xF1020000ULL)

/**
 * SW_REG_NAME : PP_QOS_WRED_PRIORITIZE_POP_REG
 * HW_REG_NAME : qos_wred_prioritize_pop
 * DESCRIPTION : 1 - give strict priority to pop indications over
 *               push requests. 0 - round robin between the two.
 *
 *  Register Fields :
 *   [31: 1][RO] - Reserved
 *   [ 0: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_PRIORITIZE_POP_REG              ((QOS_WRED_BASE_ADDR) + 0x0)
#define PP_QOS_WRED_PRIORITIZE_POP_RSVD0_OFF                        (1)
#define PP_QOS_WRED_PRIORITIZE_POP_RSVD0_LEN                        (31)
#define PP_QOS_WRED_PRIORITIZE_POP_RSVD0_MSK                        (0xFFFFFFFE)
#define PP_QOS_WRED_PRIORITIZE_POP_RSVD0_RST                        (0x0)
#define PP_QOS_WRED_PRIORITIZE_POP_OFF                              (0)
#define PP_QOS_WRED_PRIORITIZE_POP_LEN                              (1)
#define PP_QOS_WRED_PRIORITIZE_POP_MSK                              (0x00000001)
#define PP_QOS_WRED_PRIORITIZE_POP_RST                              (0x0)

/**
 * SW_REG_NAME : PP_QOS_WRED_TOTAL_RESOURCE_INIT_VAL_REG
 * HW_REG_NAME : qos_wred_total_resources_init_val
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_TOTAL_RESOURCE_INIT_VAL_REG     ((QOS_WRED_BASE_ADDR) + 0x4)
#define PP_QOS_WRED_TOTAL_RESOURCE_INIT_VAL_OFF                     (0)
#define PP_QOS_WRED_TOTAL_RESOURCE_INIT_VAL_LEN                     (32)
#define PP_QOS_WRED_TOTAL_RESOURCE_INIT_VAL_MSK                     (0xFFFFFFFF)
#define PP_QOS_WRED_TOTAL_RESOURCE_INIT_VAL_RST                     (0x0)

/**
 * SW_REG_NAME : PP_QOS_WRED_AVG_Q_WEIGHT_REG
 * HW_REG_NAME : qos_wred_avg_q_weight
 * DESCRIPTION : p in the calc avg = (old_avg*(1-p)) + (curr_avg*p)
 *
 *  Register Fields :
 *   [31:10][RO] - Reserved
 *   [ 9: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_AVG_Q_WEIGHT_REG                ((QOS_WRED_BASE_ADDR) + 0x8)
#define PP_QOS_WRED_AVG_Q_WEIGHT_RSVD0_OFF                          (10)
#define PP_QOS_WRED_AVG_Q_WEIGHT_RSVD0_LEN                          (22)
#define PP_QOS_WRED_AVG_Q_WEIGHT_RSVD0_MSK                          (0xFFFFFC00)
#define PP_QOS_WRED_AVG_Q_WEIGHT_RSVD0_RST                          (0x0)
#define PP_QOS_WRED_AVG_Q_WEIGHT_OFF                                (0)
#define PP_QOS_WRED_AVG_Q_WEIGHT_LEN                                (10)
#define PP_QOS_WRED_AVG_Q_WEIGHT_MSK                                (0x000003FF)
#define PP_QOS_WRED_AVG_Q_WEIGHT_RST                                (0x0)

/**
 * SW_REG_NAME : PP_QOS_WRED_SHIFT_SIZE_CNT_REG
 * HW_REG_NAME : qos_wred_shift_size_counter
 * DESCRIPTION : Q size bytes counter is 20 bits wide for
 *               calculations, with extra 5 bits. this parameter
 *               reduces the resolution of this counter, by up to
 *               11 bits.
 *
 *  Register Fields :
 *   [31: 4][RO] - Reserved
 *   [ 3: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_SHIFT_SIZE_CNT_REG             ((QOS_WRED_BASE_ADDR) + 0x0C)
#define PP_QOS_WRED_SHIFT_SIZE_CNT_RSVD0_OFF                        (4)
#define PP_QOS_WRED_SHIFT_SIZE_CNT_RSVD0_LEN                        (28)
#define PP_QOS_WRED_SHIFT_SIZE_CNT_RSVD0_MSK                        (0xFFFFFFF0)
#define PP_QOS_WRED_SHIFT_SIZE_CNT_RSVD0_RST                        (0x0)
#define PP_QOS_WRED_SHIFT_SIZE_CNT_OFF                              (0)
#define PP_QOS_WRED_SHIFT_SIZE_CNT_LEN                              (4)
#define PP_QOS_WRED_SHIFT_SIZE_CNT_MSK                              (0x0000000F)
#define PP_QOS_WRED_SHIFT_SIZE_CNT_RST                              (0x0)

/**
 * SW_REG_NAME : PP_QOS_WRED_SIGMA_UNUSED_FORCE_VAL_REG
 * HW_REG_NAME : qos_wred_sigma_unused_force_val
 * DESCRIPTION : force a value to sigma unused min guaranteed,
 *               instead of the value calculated internaly. on
 *               write.
 *
 *  Register Fields :
 *   [31: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_SIGMA_UNUSED_FORCE_VAL_REG     ((QOS_WRED_BASE_ADDR) + 0x10)
#define PP_QOS_WRED_SIGMA_UNUSED_FORCE_VAL_OFF                      (0)
#define PP_QOS_WRED_SIGMA_UNUSED_FORCE_VAL_LEN                      (32)
#define PP_QOS_WRED_SIGMA_UNUSED_FORCE_VAL_MSK                      (0xFFFFFFFF)
#define PP_QOS_WRED_SIGMA_UNUSED_FORCE_VAL_RST                      (0x0)

/**
 * SW_REG_NAME : PP_QOS_WRED_Q_ID_MAX_OCCUPANCY_REG
 * HW_REG_NAME : qos_wred_q_id_max_occupancy
 * DESCRIPTION : Q id for tracking max occupancy
 *
 *  Register Fields :
 *   [31: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_Q_ID_MAX_OCCUPANCY_REG         ((QOS_WRED_BASE_ADDR) + 0x14)
#define PP_QOS_WRED_Q_ID_MAX_OCCUPANCY_OFF                          (0)
#define PP_QOS_WRED_Q_ID_MAX_OCCUPANCY_LEN                          (32)
#define PP_QOS_WRED_Q_ID_MAX_OCCUPANCY_MSK                          (0xFFFFFFFF)
#define PP_QOS_WRED_Q_ID_MAX_OCCUPANCY_RST                          (0x0)
/**
 * REG_IDX_ACCESS   : PP_QOS_WRED_Q_ID_MAX_OCCUPANCY_REG_IDX
 * NUM OF REGISTERS : 8
 */
#define PP_QOS_WRED_Q_ID_MAX_OCCUPANCY_REG_IDX(idx) \
	(PP_QOS_WRED_Q_ID_MAX_OCCUPANCY_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_QOS_WRED_MAX_OCCUPANCY_R_REG
 * HW_REG_NAME : qos_wred_max_occupancy_r
 * DESCRIPTION : max occupancy counters, read (no clear)
 *
 *  Register Fields :
 *   [31: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_MAX_OCCUPANCY_R_REG            ((QOS_WRED_BASE_ADDR) + 0x34)
#define PP_QOS_WRED_MAX_OCCUPANCY_R_OFF                             (0)
#define PP_QOS_WRED_MAX_OCCUPANCY_R_LEN                             (32)
#define PP_QOS_WRED_MAX_OCCUPANCY_R_MSK                             (0xFFFFFFFF)
#define PP_QOS_WRED_MAX_OCCUPANCY_R_RST                             (0x0)
/**
 * REG_IDX_ACCESS   : PP_QOS_WRED_MAX_OCCUPANCY_R_REG_IDX
 * NUM OF REGISTERS : 8
 */
#define PP_QOS_WRED_MAX_OCCUPANCY_R_REG_IDX(idx) \
	(PP_QOS_WRED_MAX_OCCUPANCY_R_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_QOS_WRED_MAX_OCCUPANCY_RC_REG
 * HW_REG_NAME : qos_wred_max_occupancy_rc
 * DESCRIPTION : max occupancy counters, read clear
 *
 *  Register Fields :
 *   [31: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_MAX_OCCUPANCY_RC_REG           ((QOS_WRED_BASE_ADDR) + 0x54)
#define PP_QOS_WRED_MAX_OCCUPANCY_RC_OFF                            (0)
#define PP_QOS_WRED_MAX_OCCUPANCY_RC_LEN                            (32)
#define PP_QOS_WRED_MAX_OCCUPANCY_RC_MSK                            (0xFFFFFFFF)
#define PP_QOS_WRED_MAX_OCCUPANCY_RC_RST                            (0x0)
/**
 * REG_IDX_ACCESS   : PP_QOS_WRED_MAX_OCCUPANCY_RC_REG_IDX
 * NUM OF REGISTERS : 8
 */
#define PP_QOS_WRED_MAX_OCCUPANCY_RC_REG_IDX(idx) \
	(PP_QOS_WRED_MAX_OCCUPANCY_RC_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_QOS_WRED_INACTIVE_Q_ACCESS_COUNT_REG
 * HW_REG_NAME : qos_wred_inactive_q_access_count
 * DESCRIPTION : not available
 *
 *  Register Fields :
 *   [31: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_INACTIVE_Q_ACCESS_COUNT_REG    ((QOS_WRED_BASE_ADDR) + 0x74)
#define PP_QOS_WRED_INACTIVE_Q_ACCESS_COUNT_OFF                     (0)
#define PP_QOS_WRED_INACTIVE_Q_ACCESS_COUNT_LEN                     (32)
#define PP_QOS_WRED_INACTIVE_Q_ACCESS_COUNT_MSK                     (0xFFFFFFFF)
#define PP_QOS_WRED_INACTIVE_Q_ACCESS_COUNT_RST                     (0x0)

/**
 * SW_REG_NAME : PP_QOS_WRED_POP_UNDERFLOW_COUNT_REG
 * HW_REG_NAME : qos_wred_pop_underflow_count
 * DESCRIPTION : Available three times
 *
 *  Register Fields :
 *   [31: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_POP_UNDERFLOW_COUNT_REG        ((QOS_WRED_BASE_ADDR) + 0x78)
#define PP_QOS_WRED_POP_UNDERFLOW_COUNT_OFF                         (0)
#define PP_QOS_WRED_POP_UNDERFLOW_COUNT_LEN                         (32)
#define PP_QOS_WRED_POP_UNDERFLOW_COUNT_MSK                         (0xFFFFFFFF)
#define PP_QOS_WRED_POP_UNDERFLOW_COUNT_RST                         (0x0)

/**
 * SW_REG_NAME : PP_QOS_WRED_Q_CFGS_UPDATE_REG
 * HW_REG_NAME : qos_wred_q_configs_update
 * DESCRIPTION : indicates the configurations that will be changed.
 *               (1 - change this configuration, 0 - don't change)
 *
 *  Register Fields :
 *   [31:14][RO] - Reserved
 *   [13:13][RW] - MISSING_DESCRIPTION
 *   [12:12][RW] - MISSING_DESCRIPTION
 *   [11:11][RW] - MISSING_DESCRIPTION
 *   [10:10][RW] - MISSING_DESCRIPTION
 *   [ 9: 9][RW] - MISSING_DESCRIPTION
 *   [ 8: 8][RW] - MISSING_DESCRIPTION
 *   [ 7: 7][RW] - MISSING_DESCRIPTION
 *   [ 6: 6][RW] - MISSING_DESCRIPTION
 *   [ 5: 5][RW] - MISSING_DESCRIPTION
 *   [ 4: 4][RW] - MISSING_DESCRIPTION
 *   [ 3: 3][RW] - MISSING_DESCRIPTION
 *   [ 2: 2][RW] - MISSING_DESCRIPTION
 *   [ 1: 1][RW] - MISSING_DESCRIPTION
 *   [ 0: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_Q_CFGS_UPDATE_REG              ((QOS_WRED_BASE_ADDR) + 0x7C)
#define PP_QOS_WRED_Q_CFGS_UPDATE_RSVD0_OFF                         (14)
#define PP_QOS_WRED_Q_CFGS_UPDATE_RSVD0_LEN                         (18)
#define PP_QOS_WRED_Q_CFGS_UPDATE_RSVD0_MSK                         (0xFFFFC000)
#define PP_QOS_WRED_Q_CFGS_UPDATE_RSVD0_RST                         (0x0)
#define PP_QOS_WRED_Q_CFGS_UPDATE_UPDATE_MAX_ALLOWED_OFF            (13)
#define PP_QOS_WRED_Q_CFGS_UPDATE_UPDATE_MAX_ALLOWED_LEN            (1)
#define PP_QOS_WRED_Q_CFGS_UPDATE_UPDATE_MAX_ALLOWED_MSK            (0x00002000)
#define PP_QOS_WRED_Q_CFGS_UPDATE_UPDATE_MAX_ALLOWED_RST            (0x0)
#define PP_QOS_WRED_Q_CFGS_UPDATE_UPDATE_MIN_GUARANTEED_OFF         (12)
#define PP_QOS_WRED_Q_CFGS_UPDATE_UPDATE_MIN_GUARANTEED_LEN         (1)
#define PP_QOS_WRED_Q_CFGS_UPDATE_UPDATE_MIN_GUARANTEED_MSK         (0x00001000)
#define PP_QOS_WRED_Q_CFGS_UPDATE_UPDATE_MIN_GUARANTEED_RST         (0x0)
#define PP_QOS_WRED_Q_CFGS_UPDATE_UPDATE_SHIFT_AVG_GREEN_OFF        (11)
#define PP_QOS_WRED_Q_CFGS_UPDATE_UPDATE_SHIFT_AVG_GREEN_LEN        (1)
#define PP_QOS_WRED_Q_CFGS_UPDATE_UPDATE_SHIFT_AVG_GREEN_MSK        (0x00000800)
#define PP_QOS_WRED_Q_CFGS_UPDATE_UPDATE_SHIFT_AVG_GREEN_RST        (0x0)
#define PP_QOS_WRED_Q_CFGS_UPDATE_UPDATE_SLOPE_GREEN_OFF            (10)
#define PP_QOS_WRED_Q_CFGS_UPDATE_UPDATE_SLOPE_GREEN_LEN            (1)
#define PP_QOS_WRED_Q_CFGS_UPDATE_UPDATE_SLOPE_GREEN_MSK            (0x00000400)
#define PP_QOS_WRED_Q_CFGS_UPDATE_UPDATE_SLOPE_GREEN_RST            (0x0)
#define PP_QOS_WRED_Q_CFGS_UPDATE_UPDATE_MAX_AVG_GREEN_OFF          (9)
#define PP_QOS_WRED_Q_CFGS_UPDATE_UPDATE_MAX_AVG_GREEN_LEN          (1)
#define PP_QOS_WRED_Q_CFGS_UPDATE_UPDATE_MAX_AVG_GREEN_MSK          (0x00000200)
#define PP_QOS_WRED_Q_CFGS_UPDATE_UPDATE_MAX_AVG_GREEN_RST          (0x0)
#define PP_QOS_WRED_Q_CFGS_UPDATE_UPDATE_MIN_AVG_GREEN_OFF          (8)
#define PP_QOS_WRED_Q_CFGS_UPDATE_UPDATE_MIN_AVG_GREEN_LEN          (1)
#define PP_QOS_WRED_Q_CFGS_UPDATE_UPDATE_MIN_AVG_GREEN_MSK          (0x00000100)
#define PP_QOS_WRED_Q_CFGS_UPDATE_UPDATE_MIN_AVG_GREEN_RST          (0x0)
#define PP_QOS_WRED_Q_CFGS_UPDATE_UPDATE_SHIFT_AVG_YELLOW_OFF       (7)
#define PP_QOS_WRED_Q_CFGS_UPDATE_UPDATE_SHIFT_AVG_YELLOW_LEN       (1)
#define PP_QOS_WRED_Q_CFGS_UPDATE_UPDATE_SHIFT_AVG_YELLOW_MSK       (0x00000080)
#define PP_QOS_WRED_Q_CFGS_UPDATE_UPDATE_SHIFT_AVG_YELLOW_RST       (0x0)
#define PP_QOS_WRED_Q_CFGS_UPDATE_UPDATE_SLOPE_YELLOW_OFF           (6)
#define PP_QOS_WRED_Q_CFGS_UPDATE_UPDATE_SLOPE_YELLOW_LEN           (1)
#define PP_QOS_WRED_Q_CFGS_UPDATE_UPDATE_SLOPE_YELLOW_MSK           (0x00000040)
#define PP_QOS_WRED_Q_CFGS_UPDATE_UPDATE_SLOPE_YELLOW_RST           (0x0)
#define PP_QOS_WRED_Q_CFGS_UPDATE_UPDATE_MAX_AVG_YELLOW_OFF         (5)
#define PP_QOS_WRED_Q_CFGS_UPDATE_UPDATE_MAX_AVG_YELLOW_LEN         (1)
#define PP_QOS_WRED_Q_CFGS_UPDATE_UPDATE_MAX_AVG_YELLOW_MSK         (0x00000020)
#define PP_QOS_WRED_Q_CFGS_UPDATE_UPDATE_MAX_AVG_YELLOW_RST         (0x0)
#define PP_QOS_WRED_Q_CFGS_UPDATE_UPDATE_MIN_AVG_YELLOW_OFF         (4)
#define PP_QOS_WRED_Q_CFGS_UPDATE_UPDATE_MIN_AVG_YELLOW_LEN         (1)
#define PP_QOS_WRED_Q_CFGS_UPDATE_UPDATE_MIN_AVG_YELLOW_MSK         (0x00000010)
#define PP_QOS_WRED_Q_CFGS_UPDATE_UPDATE_MIN_AVG_YELLOW_RST         (0x0)
#define PP_QOS_WRED_Q_CFGS_UPDATE_UPDATE_FIXED_YELLOW_DROP_P_OFF    (3)
#define PP_QOS_WRED_Q_CFGS_UPDATE_UPDATE_FIXED_YELLOW_DROP_P_LEN    (1)
#define PP_QOS_WRED_Q_CFGS_UPDATE_UPDATE_FIXED_YELLOW_DROP_P_MSK    (0x00000008)
#define PP_QOS_WRED_Q_CFGS_UPDATE_UPDATE_FIXED_YELLOW_DROP_P_RST    (0x0)
#define PP_QOS_WRED_Q_CFGS_UPDATE_UPDATE_FIXED_GREEN_DROP_P_OFF     (2)
#define PP_QOS_WRED_Q_CFGS_UPDATE_UPDATE_FIXED_GREEN_DROP_P_LEN     (1)
#define PP_QOS_WRED_Q_CFGS_UPDATE_UPDATE_FIXED_GREEN_DROP_P_MSK     (0x00000004)
#define PP_QOS_WRED_Q_CFGS_UPDATE_UPDATE_FIXED_GREEN_DROP_P_RST     (0x0)
#define PP_QOS_WRED_Q_CFGS_UPDATE_UPDATE_DIS_OFF                    (1)
#define PP_QOS_WRED_Q_CFGS_UPDATE_UPDATE_DIS_LEN                    (1)
#define PP_QOS_WRED_Q_CFGS_UPDATE_UPDATE_DIS_MSK                    (0x00000002)
#define PP_QOS_WRED_Q_CFGS_UPDATE_UPDATE_DIS_RST                    (0x0)
#define PP_QOS_WRED_Q_CFGS_UPDATE_UPDATE_ACTIVE_Q_OFF               (0)
#define PP_QOS_WRED_Q_CFGS_UPDATE_UPDATE_ACTIVE_Q_LEN               (1)
#define PP_QOS_WRED_Q_CFGS_UPDATE_UPDATE_ACTIVE_Q_MSK               (0x00000001)
#define PP_QOS_WRED_Q_CFGS_UPDATE_UPDATE_ACTIVE_Q_RST               (0x0)

/**
 * SW_REG_NAME : PP_QOS_WRED_ACTIVE_Q_REG
 * HW_REG_NAME : qos_wred_active_q
 * DESCRIPTION : config this queue as active/inactive queue. 1 -
 *               active, 0 - inactive.
 *
 *  Register Fields :
 *   [31: 1][RO] - Reserved
 *   [ 0: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_ACTIVE_Q_REG                   ((QOS_WRED_BASE_ADDR) + 0x80)
#define PP_QOS_WRED_ACTIVE_Q_RSVD0_OFF                              (1)
#define PP_QOS_WRED_ACTIVE_Q_RSVD0_LEN                              (31)
#define PP_QOS_WRED_ACTIVE_Q_RSVD0_MSK                              (0xFFFFFFFE)
#define PP_QOS_WRED_ACTIVE_Q_RSVD0_RST                              (0x0)
#define PP_QOS_WRED_ACTIVE_Q_OFF                                    (0)
#define PP_QOS_WRED_ACTIVE_Q_LEN                                    (1)
#define PP_QOS_WRED_ACTIVE_Q_MSK                                    (0x00000001)
#define PP_QOS_WRED_ACTIVE_Q_RST                                    (0x0)

/**
 * SW_REG_NAME : PP_QOS_WRED_DIS_REG
 * HW_REG_NAME : qos_wred_disable
 * DESCRIPTION : disable configuration for disabling one of the
 *               drop decision parameters for updating existing Q
 *               or adding a new Q. bit 0-disable wred, bit
 *               1-disable min guaranteed max allowed, 2-disable
 *               qm_full, 3-use fixed probability instead of sloped
 *               area in wred graph
 *
 *  Register Fields :
 *   [31: 4][RO] - Reserved
 *   [ 3: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_DIS_REG                        ((QOS_WRED_BASE_ADDR) + 0x84)
#define PP_QOS_WRED_DIS_RSVD0_OFF                                   (4)
#define PP_QOS_WRED_DIS_RSVD0_LEN                                   (28)
#define PP_QOS_WRED_DIS_RSVD0_MSK                                   (0xFFFFFFF0)
#define PP_QOS_WRED_DIS_RSVD0_RST                                   (0x0)
#define PP_QOS_WRED_DIS_OFF                                         (0)
#define PP_QOS_WRED_DIS_LEN                                         (4)
#define PP_QOS_WRED_DIS_MSK                                         (0x0000000F)
#define PP_QOS_WRED_DIS_RST                                         (0x0)

/**
 * SW_REG_NAME : PP_QOS_WRED_FIXED_GREEN_DROP_P_REG
 * HW_REG_NAME : qos_wred_fixed_green_drop_p
 * DESCRIPTION : in case disable bit 3 is set, in the green wred
 *               graph this value will be fixed instead of sloped
 *               area
 *
 *  Register Fields :
 *   [31:16][RO] - Reserved
 *   [15: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_FIXED_GREEN_DROP_P_REG         ((QOS_WRED_BASE_ADDR) + 0x88)
#define PP_QOS_WRED_FIXED_GREEN_DROP_P_RSVD0_OFF                    (16)
#define PP_QOS_WRED_FIXED_GREEN_DROP_P_RSVD0_LEN                    (16)
#define PP_QOS_WRED_FIXED_GREEN_DROP_P_RSVD0_MSK                    (0xFFFF0000)
#define PP_QOS_WRED_FIXED_GREEN_DROP_P_RSVD0_RST                    (0x0)
#define PP_QOS_WRED_FIXED_GREEN_DROP_P_OFF                          (0)
#define PP_QOS_WRED_FIXED_GREEN_DROP_P_LEN                          (16)
#define PP_QOS_WRED_FIXED_GREEN_DROP_P_MSK                          (0x0000FFFF)
#define PP_QOS_WRED_FIXED_GREEN_DROP_P_RST                          (0x0)

/**
 * SW_REG_NAME : PP_QOS_WRED_FIXED_YELLOW_DROP_P_REG
 * HW_REG_NAME : qos_wred_fixed_yellow_drop_p
 * DESCRIPTION : in case disable bit 3 is set, in the yellow wred
 *               graph this value will be fixed instead of sloped
 *               area
 *
 *  Register Fields :
 *   [31:16][RO] - Reserved
 *   [15: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_FIXED_YELLOW_DROP_P_REG        ((QOS_WRED_BASE_ADDR) + 0x8C)
#define PP_QOS_WRED_FIXED_YELLOW_DROP_P_RSVD0_OFF                   (16)
#define PP_QOS_WRED_FIXED_YELLOW_DROP_P_RSVD0_LEN                   (16)
#define PP_QOS_WRED_FIXED_YELLOW_DROP_P_RSVD0_MSK                   (0xFFFF0000)
#define PP_QOS_WRED_FIXED_YELLOW_DROP_P_RSVD0_RST                   (0x0)
#define PP_QOS_WRED_FIXED_YELLOW_DROP_P_OFF                         (0)
#define PP_QOS_WRED_FIXED_YELLOW_DROP_P_LEN                         (16)
#define PP_QOS_WRED_FIXED_YELLOW_DROP_P_MSK                         (0x0000FFFF)
#define PP_QOS_WRED_FIXED_YELLOW_DROP_P_RST                         (0x0)

/**
 * SW_REG_NAME : PP_QOS_WRED_MIN_AVG_YELLOW_REG
 * HW_REG_NAME : qos_wred_min_avg_yellow
 * DESCRIPTION : min average configuration for yellow packets for
 *               updating existing Q or adding a new Q
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_MIN_AVG_YELLOW_REG             ((QOS_WRED_BASE_ADDR) + 0x90)
#define PP_QOS_WRED_MIN_AVG_YELLOW_RSVD0_OFF                        (20)
#define PP_QOS_WRED_MIN_AVG_YELLOW_RSVD0_LEN                        (12)
#define PP_QOS_WRED_MIN_AVG_YELLOW_RSVD0_MSK                        (0xFFF00000)
#define PP_QOS_WRED_MIN_AVG_YELLOW_RSVD0_RST                        (0x0)
#define PP_QOS_WRED_MIN_AVG_YELLOW_OFF                              (0)
#define PP_QOS_WRED_MIN_AVG_YELLOW_LEN                              (20)
#define PP_QOS_WRED_MIN_AVG_YELLOW_MSK                              (0x000FFFFF)
#define PP_QOS_WRED_MIN_AVG_YELLOW_RST                              (0x0)

/**
 * SW_REG_NAME : PP_QOS_WRED_MAX_AVG_YELLOW_REG
 * HW_REG_NAME : qos_wred_max_avg_yellow
 * DESCRIPTION : max average for yellow packets configuration for
 *               updating existing Q or adding a new Q
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_MAX_AVG_YELLOW_REG             ((QOS_WRED_BASE_ADDR) + 0x94)
#define PP_QOS_WRED_MAX_AVG_YELLOW_RSVD0_OFF                        (20)
#define PP_QOS_WRED_MAX_AVG_YELLOW_RSVD0_LEN                        (12)
#define PP_QOS_WRED_MAX_AVG_YELLOW_RSVD0_MSK                        (0xFFF00000)
#define PP_QOS_WRED_MAX_AVG_YELLOW_RSVD0_RST                        (0x0)
#define PP_QOS_WRED_MAX_AVG_YELLOW_OFF                              (0)
#define PP_QOS_WRED_MAX_AVG_YELLOW_LEN                              (20)
#define PP_QOS_WRED_MAX_AVG_YELLOW_MSK                              (0x000FFFFF)
#define PP_QOS_WRED_MAX_AVG_YELLOW_RST                              (0x0)

/**
 * SW_REG_NAME : PP_QOS_WRED_SLOPE_YELLOW_REG
 * HW_REG_NAME : qos_wred_slope_yellow
 * DESCRIPTION : WRED graph slope for yellow packets configuration
 *               for updating existing Q or adding a new Q
 *
 *  Register Fields :
 *   [31: 5][RO] - Reserved
 *   [ 4: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_SLOPE_YELLOW_REG               ((QOS_WRED_BASE_ADDR) + 0x98)
#define PP_QOS_WRED_SLOPE_YELLOW_RSVD0_OFF                          (5)
#define PP_QOS_WRED_SLOPE_YELLOW_RSVD0_LEN                          (27)
#define PP_QOS_WRED_SLOPE_YELLOW_RSVD0_MSK                          (0xFFFFFFE0)
#define PP_QOS_WRED_SLOPE_YELLOW_RSVD0_RST                          (0x0)
#define PP_QOS_WRED_SLOPE_YELLOW_OFF                                (0)
#define PP_QOS_WRED_SLOPE_YELLOW_LEN                                (5)
#define PP_QOS_WRED_SLOPE_YELLOW_MSK                                (0x0000001F)
#define PP_QOS_WRED_SLOPE_YELLOW_RST                                (0x0)

/**
 * SW_REG_NAME : PP_QOS_WRED_SHIFT_AVG_YELLOW_REG
 * HW_REG_NAME : qos_wred_shift_avg_yellow
 * DESCRIPTION : in drop probability calculation - the result of
 *               (avg_q_size - min_avg) is shifted right to fit in
 *               11 bits.
 *
 *  Register Fields :
 *   [31: 5][RO] - Reserved
 *   [ 4: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_SHIFT_AVG_YELLOW_REG           ((QOS_WRED_BASE_ADDR) + 0x9C)
#define PP_QOS_WRED_SHIFT_AVG_YELLOW_RSVD0_OFF                      (5)
#define PP_QOS_WRED_SHIFT_AVG_YELLOW_RSVD0_LEN                      (27)
#define PP_QOS_WRED_SHIFT_AVG_YELLOW_RSVD0_MSK                      (0xFFFFFFE0)
#define PP_QOS_WRED_SHIFT_AVG_YELLOW_RSVD0_RST                      (0x0)
#define PP_QOS_WRED_SHIFT_AVG_YELLOW_OFF                            (0)
#define PP_QOS_WRED_SHIFT_AVG_YELLOW_LEN                            (5)
#define PP_QOS_WRED_SHIFT_AVG_YELLOW_MSK                            (0x0000001F)
#define PP_QOS_WRED_SHIFT_AVG_YELLOW_RST                            (0x0)

/**
 * SW_REG_NAME : PP_QOS_WRED_MIN_AVG_GREEN_REG
 * HW_REG_NAME : qos_wred_min_avg_green
 * DESCRIPTION : min average configuration for green packets for
 *               updating existing Q or adding a new Q
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_MIN_AVG_GREEN_REG             ((QOS_WRED_BASE_ADDR) + 0x0A0)
#define PP_QOS_WRED_MIN_AVG_GREEN_RSVD0_OFF                         (20)
#define PP_QOS_WRED_MIN_AVG_GREEN_RSVD0_LEN                         (12)
#define PP_QOS_WRED_MIN_AVG_GREEN_RSVD0_MSK                         (0xFFF00000)
#define PP_QOS_WRED_MIN_AVG_GREEN_RSVD0_RST                         (0x0)
#define PP_QOS_WRED_MIN_AVG_GREEN_OFF                               (0)
#define PP_QOS_WRED_MIN_AVG_GREEN_LEN                               (20)
#define PP_QOS_WRED_MIN_AVG_GREEN_MSK                               (0x000FFFFF)
#define PP_QOS_WRED_MIN_AVG_GREEN_RST                               (0x0)

/**
 * SW_REG_NAME : PP_QOS_WRED_MAX_AVG_GREEN_REG
 * HW_REG_NAME : qos_wred_max_avg_green
 * DESCRIPTION : max average for green packets configuration for
 *               updating existing Q or adding a new Q
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_MAX_AVG_GREEN_REG             ((QOS_WRED_BASE_ADDR) + 0x0A4)
#define PP_QOS_WRED_MAX_AVG_GREEN_RSVD0_OFF                         (20)
#define PP_QOS_WRED_MAX_AVG_GREEN_RSVD0_LEN                         (12)
#define PP_QOS_WRED_MAX_AVG_GREEN_RSVD0_MSK                         (0xFFF00000)
#define PP_QOS_WRED_MAX_AVG_GREEN_RSVD0_RST                         (0x0)
#define PP_QOS_WRED_MAX_AVG_GREEN_OFF                               (0)
#define PP_QOS_WRED_MAX_AVG_GREEN_LEN                               (20)
#define PP_QOS_WRED_MAX_AVG_GREEN_MSK                               (0x000FFFFF)
#define PP_QOS_WRED_MAX_AVG_GREEN_RST                               (0x0)

/**
 * SW_REG_NAME : PP_QOS_WRED_SLOPE_GREEN_REG
 * HW_REG_NAME : qos_wred_slope_green
 * DESCRIPTION : WRED graph slope for green packets configuration
 *               for updating existing Q or adding a new Q
 *
 *  Register Fields :
 *   [31: 5][RO] - Reserved
 *   [ 4: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_SLOPE_GREEN_REG               ((QOS_WRED_BASE_ADDR) + 0x0A8)
#define PP_QOS_WRED_SLOPE_GREEN_RSVD0_OFF                           (5)
#define PP_QOS_WRED_SLOPE_GREEN_RSVD0_LEN                           (27)
#define PP_QOS_WRED_SLOPE_GREEN_RSVD0_MSK                           (0xFFFFFFE0)
#define PP_QOS_WRED_SLOPE_GREEN_RSVD0_RST                           (0x0)
#define PP_QOS_WRED_SLOPE_GREEN_OFF                                 (0)
#define PP_QOS_WRED_SLOPE_GREEN_LEN                                 (5)
#define PP_QOS_WRED_SLOPE_GREEN_MSK                                 (0x0000001F)
#define PP_QOS_WRED_SLOPE_GREEN_RST                                 (0x0)

/**
 * SW_REG_NAME : PP_QOS_WRED_SHIFT_AVG_GREEN_REG
 * HW_REG_NAME : qos_wred_shift_avg_green
 * DESCRIPTION : in drop probability calculation - the result of
 *               (avg_q_size - min_avg) is shifted right to fit in
 *               11 bits.
 *
 *  Register Fields :
 *   [31: 5][RO] - Reserved
 *   [ 4: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_SHIFT_AVG_GREEN_REG           ((QOS_WRED_BASE_ADDR) + 0x0AC)
#define PP_QOS_WRED_SHIFT_AVG_GREEN_RSVD0_OFF                       (5)
#define PP_QOS_WRED_SHIFT_AVG_GREEN_RSVD0_LEN                       (27)
#define PP_QOS_WRED_SHIFT_AVG_GREEN_RSVD0_MSK                       (0xFFFFFFE0)
#define PP_QOS_WRED_SHIFT_AVG_GREEN_RSVD0_RST                       (0x0)
#define PP_QOS_WRED_SHIFT_AVG_GREEN_OFF                             (0)
#define PP_QOS_WRED_SHIFT_AVG_GREEN_LEN                             (5)
#define PP_QOS_WRED_SHIFT_AVG_GREEN_MSK                             (0x0000001F)
#define PP_QOS_WRED_SHIFT_AVG_GREEN_RST                             (0x0)

/**
 * SW_REG_NAME : PP_QOS_WRED_MIN_GUARANTEED_REG
 * HW_REG_NAME : qos_wred_min_guaranteed
 * DESCRIPTION : min guaranteed configuration for updating existing
 *               Q or adding a new Q
 *
 *  Register Fields :
 *   [31:16][RO] - Reserved
 *   [15: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_MIN_GUARANTEED_REG            ((QOS_WRED_BASE_ADDR) + 0x0B0)
#define PP_QOS_WRED_MIN_GUARANTEED_RSVD0_OFF                        (16)
#define PP_QOS_WRED_MIN_GUARANTEED_RSVD0_LEN                        (16)
#define PP_QOS_WRED_MIN_GUARANTEED_RSVD0_MSK                        (0xFFFF0000)
#define PP_QOS_WRED_MIN_GUARANTEED_RSVD0_RST                        (0x0)
#define PP_QOS_WRED_MIN_GUARANTEED_OFF                              (0)
#define PP_QOS_WRED_MIN_GUARANTEED_LEN                              (16)
#define PP_QOS_WRED_MIN_GUARANTEED_MSK                              (0x0000FFFF)
#define PP_QOS_WRED_MIN_GUARANTEED_RST                              (0x0)

/**
 * SW_REG_NAME : PP_QOS_WRED_MAX_ALLOWED_REG
 * HW_REG_NAME : qos_wred_max_allowed
 * DESCRIPTION : max allowed configuration for updating existing Q
 *               or adding a new Q
 *
 *  Register Fields :
 *   [31:16][RO] - Reserved
 *   [15: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_MAX_ALLOWED_REG               ((QOS_WRED_BASE_ADDR) + 0x0B4)
#define PP_QOS_WRED_MAX_ALLOWED_RSVD0_OFF                           (16)
#define PP_QOS_WRED_MAX_ALLOWED_RSVD0_LEN                           (16)
#define PP_QOS_WRED_MAX_ALLOWED_RSVD0_MSK                           (0xFFFF0000)
#define PP_QOS_WRED_MAX_ALLOWED_RSVD0_RST                           (0x0)
#define PP_QOS_WRED_MAX_ALLOWED_OFF                                 (0)
#define PP_QOS_WRED_MAX_ALLOWED_LEN                                 (16)
#define PP_QOS_WRED_MAX_ALLOWED_MSK                                 (0x0000FFFF)
#define PP_QOS_WRED_MAX_ALLOWED_RST                                 (0x0)

/**
 * SW_REG_NAME : PP_QOS_WRED_Q_CNTS_CLR_REG
 * HW_REG_NAME : qos_wred_q_counters_clear
 * DESCRIPTION : indicates the counters that will be cleared when
 *               using the read clear command in the mem command
 *               register. ( 1 - clear this counter, 0 - don't
 *               clear)
 *
 *  Register Fields :
 *   [31:10][RO] - Reserved
 *   [ 9: 9][RW] - MISSING_DESCRIPTION
 *   [ 8: 8][RW] - MISSING_DESCRIPTION
 *   [ 7: 7][RW] - MISSING_DESCRIPTION
 *   [ 6: 6][RW] - MISSING_DESCRIPTION
 *   [ 5: 5][RW] - MISSING_DESCRIPTION
 *   [ 4: 4][RW] - MISSING_DESCRIPTION
 *   [ 3: 3][RW] - MISSING_DESCRIPTION
 *   [ 2: 2][RW] - MISSING_DESCRIPTION
 *   [ 1: 1][RW] - MISSING_DESCRIPTION
 *   [ 0: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_Q_CNTS_CLR_REG                ((QOS_WRED_BASE_ADDR) + 0x0B8)
#define PP_QOS_WRED_Q_CNTS_CLR_RSVD0_OFF                            (10)
#define PP_QOS_WRED_Q_CNTS_CLR_RSVD0_LEN                            (22)
#define PP_QOS_WRED_Q_CNTS_CLR_RSVD0_MSK                            (0xFFFFFC00)
#define PP_QOS_WRED_Q_CNTS_CLR_RSVD0_RST                            (0x0)
#define PP_QOS_WRED_Q_CNTS_CLR_CLR_TOTAL_RED_DROPS_OFF              (9)
#define PP_QOS_WRED_Q_CNTS_CLR_CLR_TOTAL_RED_DROPS_LEN              (1)
#define PP_QOS_WRED_Q_CNTS_CLR_CLR_TOTAL_RED_DROPS_MSK              (0x00000200)
#define PP_QOS_WRED_Q_CNTS_CLR_CLR_TOTAL_RED_DROPS_RST              (0x0)
#define PP_QOS_WRED_Q_CNTS_CLR_CLR_TOTAL_DROPPED_BYTES_OFF          (8)
#define PP_QOS_WRED_Q_CNTS_CLR_CLR_TOTAL_DROPPED_BYTES_LEN          (1)
#define PP_QOS_WRED_Q_CNTS_CLR_CLR_TOTAL_DROPPED_BYTES_MSK          (0x00000100)
#define PP_QOS_WRED_Q_CNTS_CLR_CLR_TOTAL_DROPPED_BYTES_RST          (0x0)
#define PP_QOS_WRED_Q_CNTS_CLR_CLR_TOTAL_DROPS_OFF                  (7)
#define PP_QOS_WRED_Q_CNTS_CLR_CLR_TOTAL_DROPS_LEN                  (1)
#define PP_QOS_WRED_Q_CNTS_CLR_CLR_TOTAL_DROPS_MSK                  (0x00000080)
#define PP_QOS_WRED_Q_CNTS_CLR_CLR_TOTAL_DROPS_RST                  (0x0)
#define PP_QOS_WRED_Q_CNTS_CLR_CLR_TOTAL_ACCEPTS_OFF                (6)
#define PP_QOS_WRED_Q_CNTS_CLR_CLR_TOTAL_ACCEPTS_LEN                (1)
#define PP_QOS_WRED_Q_CNTS_CLR_CLR_TOTAL_ACCEPTS_MSK                (0x00000040)
#define PP_QOS_WRED_Q_CNTS_CLR_CLR_TOTAL_ACCEPTS_RST                (0x0)
#define PP_QOS_WRED_Q_CNTS_CLR_CLR_TOTAL_BYTES_ADDED_OFF            (5)
#define PP_QOS_WRED_Q_CNTS_CLR_CLR_TOTAL_BYTES_ADDED_LEN            (1)
#define PP_QOS_WRED_Q_CNTS_CLR_CLR_TOTAL_BYTES_ADDED_MSK            (0x00000020)
#define PP_QOS_WRED_Q_CNTS_CLR_CLR_TOTAL_BYTES_ADDED_RST            (0x0)
#define PP_QOS_WRED_Q_CNTS_CLR_CLR_DROP_P_GREEN_OFF                 (4)
#define PP_QOS_WRED_Q_CNTS_CLR_CLR_DROP_P_GREEN_LEN                 (1)
#define PP_QOS_WRED_Q_CNTS_CLR_CLR_DROP_P_GREEN_MSK                 (0x00000010)
#define PP_QOS_WRED_Q_CNTS_CLR_CLR_DROP_P_GREEN_RST                 (0x0)
#define PP_QOS_WRED_Q_CNTS_CLR_CLR_DROP_P_YELLOW_OFF                (3)
#define PP_QOS_WRED_Q_CNTS_CLR_CLR_DROP_P_YELLOW_LEN                (1)
#define PP_QOS_WRED_Q_CNTS_CLR_CLR_DROP_P_YELLOW_MSK                (0x00000008)
#define PP_QOS_WRED_Q_CNTS_CLR_CLR_DROP_P_YELLOW_RST                (0x0)
#define PP_QOS_WRED_Q_CNTS_CLR_CLR_Q_SIZE_ENTRIES_OFF               (2)
#define PP_QOS_WRED_Q_CNTS_CLR_CLR_Q_SIZE_ENTRIES_LEN               (1)
#define PP_QOS_WRED_Q_CNTS_CLR_CLR_Q_SIZE_ENTRIES_MSK               (0x00000004)
#define PP_QOS_WRED_Q_CNTS_CLR_CLR_Q_SIZE_ENTRIES_RST               (0x0)
#define PP_QOS_WRED_Q_CNTS_CLR_CLR_Q_AVG_SIZE_BYTES_OFF             (1)
#define PP_QOS_WRED_Q_CNTS_CLR_CLR_Q_AVG_SIZE_BYTES_LEN             (1)
#define PP_QOS_WRED_Q_CNTS_CLR_CLR_Q_AVG_SIZE_BYTES_MSK             (0x00000002)
#define PP_QOS_WRED_Q_CNTS_CLR_CLR_Q_AVG_SIZE_BYTES_RST             (0x0)
#define PP_QOS_WRED_Q_CNTS_CLR_CLR_Q_SIZE_BYTES_OFF                 (0)
#define PP_QOS_WRED_Q_CNTS_CLR_CLR_Q_SIZE_BYTES_LEN                 (1)
#define PP_QOS_WRED_Q_CNTS_CLR_CLR_Q_SIZE_BYTES_MSK                 (0x00000001)
#define PP_QOS_WRED_Q_CNTS_CLR_CLR_Q_SIZE_BYTES_RST                 (0x0)

/**
 * SW_REG_NAME : PP_QOS_WRED_Q_ADDR_REG
 * HW_REG_NAME : qos_wred_q_addr
 * DESCRIPTION : Address of the Q that needs to be read from the
 *               memory, updated, added or removed.
 *
 *  Register Fields :
 *   [31: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_Q_ADDR_REG                    ((QOS_WRED_BASE_ADDR) + 0x0BC)
#define PP_QOS_WRED_Q_ADDR_OFF                                      (0)
#define PP_QOS_WRED_Q_ADDR_LEN                                      (32)
#define PP_QOS_WRED_Q_ADDR_MSK                                      (0xFFFFFFFF)
#define PP_QOS_WRED_Q_ADDR_RST                                      (0x0)

/**
 * SW_REG_NAME : PP_QOS_WRED_MEM_CMD_REG
 * HW_REG_NAME : qos_wred_mem_cmd
 * DESCRIPTION : Command for accessing the Q counters memory. 0 -
 *               read q counters, 1 - read clear q counters, 2 -
 *               remove Q, 3 - add new Q, 4 - update existing Q.
 *               for 0, 2 - provide q address, for 1 - provide q
 *               address and counters to clear with q counters
 *               clear register, for 3 - provide q address and all
 *               Q configurations, for 4 - provide q address and
 *               all Q configurations with Q configuration update
 *               register, 5 - provide q address and aqm_to_context
 *               configuration only, 6 - read clear all Qs
 *               counters, 7 - remove all Qs.
 *
 *  Register Fields :
 *   [31: 3][RO] - Reserved
 *   [ 2: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_MEM_CMD_REG                   ((QOS_WRED_BASE_ADDR) + 0x0C0)
#define PP_QOS_WRED_MEM_CMD_RSVD0_OFF                               (3)
#define PP_QOS_WRED_MEM_CMD_RSVD0_LEN                               (29)
#define PP_QOS_WRED_MEM_CMD_RSVD0_MSK                               (0xFFFFFFF8)
#define PP_QOS_WRED_MEM_CMD_RSVD0_RST                               (0x0)
#define PP_QOS_WRED_MEM_CMD_OFF                                     (0)
#define PP_QOS_WRED_MEM_CMD_LEN                                     (3)
#define PP_QOS_WRED_MEM_CMD_MSK                                     (0x00000007)
#define PP_QOS_WRED_MEM_CMD_RST                                     (0x0)

/**
 * SW_REG_NAME : PP_QOS_WRED_MEM_ACCESS_BUSY_REG
 * HW_REG_NAME : qos_wred_mem_access_busy
 * DESCRIPTION : This register signals the host that it cant write
 *               new configurations to write to mem, otherwise it
 *               will override a command that wasn't handled yet.
 *
 *  Register Fields :
 *   [31: 1][RO] - Reserved
 *   [ 0: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_MEM_ACCESS_BUSY_REG           ((QOS_WRED_BASE_ADDR) + 0x0C4)
#define PP_QOS_WRED_MEM_ACCESS_BUSY_RSVD0_OFF                       (1)
#define PP_QOS_WRED_MEM_ACCESS_BUSY_RSVD0_LEN                       (31)
#define PP_QOS_WRED_MEM_ACCESS_BUSY_RSVD0_MSK                       (0xFFFFFFFE)
#define PP_QOS_WRED_MEM_ACCESS_BUSY_RSVD0_RST                       (0x0)
#define PP_QOS_WRED_MEM_ACCESS_BUSY_OFF                             (0)
#define PP_QOS_WRED_MEM_ACCESS_BUSY_LEN                             (1)
#define PP_QOS_WRED_MEM_ACCESS_BUSY_MSK                             (0x00000001)
#define PP_QOS_WRED_MEM_ACCESS_BUSY_RST                             (0x0)

/**
 * SW_REG_NAME : PP_QOS_WRED_Q_SIZE_BYTES_REG
 * HW_REG_NAME : qos_wred_q_size_bytes
 * DESCRIPTION : q size bytes counter value for the q specified in
 *               q address register.
 *
 *  Register Fields :
 *   [31:31][RO] - Reserved
 *   [30: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_Q_SIZE_BYTES_REG              ((QOS_WRED_BASE_ADDR) + 0x0C8)
#define PP_QOS_WRED_Q_SIZE_BYTES_RSVD0_OFF                          (31)
#define PP_QOS_WRED_Q_SIZE_BYTES_RSVD0_LEN                          (1)
#define PP_QOS_WRED_Q_SIZE_BYTES_RSVD0_MSK                          (0x80000000)
#define PP_QOS_WRED_Q_SIZE_BYTES_RSVD0_RST                          (0x0)
#define PP_QOS_WRED_Q_SIZE_BYTES_OFF                                (0)
#define PP_QOS_WRED_Q_SIZE_BYTES_LEN                                (31)
#define PP_QOS_WRED_Q_SIZE_BYTES_MSK                                (0x7FFFFFFF)
#define PP_QOS_WRED_Q_SIZE_BYTES_RST                                (0x0)

/**
 * SW_REG_NAME : PP_QOS_WRED_Q_AVG_SIZE_BYTES_REG
 * HW_REG_NAME : qos_wred_q_avg_size_bytes
 * DESCRIPTION : q average size bytes counter value for the q
 *               specified in q address register.
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_Q_AVG_SIZE_BYTES_REG          ((QOS_WRED_BASE_ADDR) + 0x0CC)
#define PP_QOS_WRED_Q_AVG_SIZE_BYTES_RSVD0_OFF                      (20)
#define PP_QOS_WRED_Q_AVG_SIZE_BYTES_RSVD0_LEN                      (12)
#define PP_QOS_WRED_Q_AVG_SIZE_BYTES_RSVD0_MSK                      (0xFFF00000)
#define PP_QOS_WRED_Q_AVG_SIZE_BYTES_RSVD0_RST                      (0x0)
#define PP_QOS_WRED_Q_AVG_SIZE_BYTES_OFF                            (0)
#define PP_QOS_WRED_Q_AVG_SIZE_BYTES_LEN                            (20)
#define PP_QOS_WRED_Q_AVG_SIZE_BYTES_MSK                            (0x000FFFFF)
#define PP_QOS_WRED_Q_AVG_SIZE_BYTES_RST                            (0x0)

/**
 * SW_REG_NAME : PP_QOS_WRED_Q_SIZE_ENTRIES_REG
 * HW_REG_NAME : qos_wred_q_size_entries
 * DESCRIPTION : q size entries counter value for the q specified
 *               in q address register.
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_Q_SIZE_ENTRIES_REG            ((QOS_WRED_BASE_ADDR) + 0x0D0)
#define PP_QOS_WRED_Q_SIZE_ENTRIES_RSVD0_OFF                        (20)
#define PP_QOS_WRED_Q_SIZE_ENTRIES_RSVD0_LEN                        (12)
#define PP_QOS_WRED_Q_SIZE_ENTRIES_RSVD0_MSK                        (0xFFF00000)
#define PP_QOS_WRED_Q_SIZE_ENTRIES_RSVD0_RST                        (0x0)
#define PP_QOS_WRED_Q_SIZE_ENTRIES_OFF                              (0)
#define PP_QOS_WRED_Q_SIZE_ENTRIES_LEN                              (20)
#define PP_QOS_WRED_Q_SIZE_ENTRIES_MSK                              (0x000FFFFF)
#define PP_QOS_WRED_Q_SIZE_ENTRIES_RST                              (0x0)

/**
 * SW_REG_NAME : PP_QOS_WRED_DROP_P_YELLOW_REG
 * HW_REG_NAME : qos_wred_drop_p_yellow
 * DESCRIPTION : drop probability for yellow packets for the q
 *               specified in q address register.
 *
 *  Register Fields :
 *   [31:16][RO] - Reserved
 *   [15: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_DROP_P_YELLOW_REG             ((QOS_WRED_BASE_ADDR) + 0x0D4)
#define PP_QOS_WRED_DROP_P_YELLOW_RSVD0_OFF                         (16)
#define PP_QOS_WRED_DROP_P_YELLOW_RSVD0_LEN                         (16)
#define PP_QOS_WRED_DROP_P_YELLOW_RSVD0_MSK                         (0xFFFF0000)
#define PP_QOS_WRED_DROP_P_YELLOW_RSVD0_RST                         (0x0)
#define PP_QOS_WRED_DROP_P_YELLOW_OFF                               (0)
#define PP_QOS_WRED_DROP_P_YELLOW_LEN                               (16)
#define PP_QOS_WRED_DROP_P_YELLOW_MSK                               (0x0000FFFF)
#define PP_QOS_WRED_DROP_P_YELLOW_RST                               (0x0)

/**
 * SW_REG_NAME : PP_QOS_WRED_DROP_P_GREEN_REG
 * HW_REG_NAME : qos_wred_drop_p_green
 * DESCRIPTION : drop probability for green packets for the q
 *               specified in q address register.
 *
 *  Register Fields :
 *   [31:16][RO] - Reserved
 *   [15: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_DROP_P_GREEN_REG              ((QOS_WRED_BASE_ADDR) + 0x0D8)
#define PP_QOS_WRED_DROP_P_GREEN_RSVD0_OFF                          (16)
#define PP_QOS_WRED_DROP_P_GREEN_RSVD0_LEN                          (16)
#define PP_QOS_WRED_DROP_P_GREEN_RSVD0_MSK                          (0xFFFF0000)
#define PP_QOS_WRED_DROP_P_GREEN_RSVD0_RST                          (0x0)
#define PP_QOS_WRED_DROP_P_GREEN_OFF                                (0)
#define PP_QOS_WRED_DROP_P_GREEN_LEN                                (16)
#define PP_QOS_WRED_DROP_P_GREEN_MSK                                (0x0000FFFF)
#define PP_QOS_WRED_DROP_P_GREEN_RST                                (0x0)

/**
 * SW_REG_NAME : PP_QOS_WRED_TOTAL_BYTES_ADDED_LO_REG
 * HW_REG_NAME : qos_wred_total_bytes_added_lo
 * DESCRIPTION : total bytes added to q counter value bits 31:0 for
 *               the q specified in q address register.
 *
 *  Register Fields :
 *   [31: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_TOTAL_BYTES_ADDED_LO_REG      ((QOS_WRED_BASE_ADDR) + 0x0DC)
#define PP_QOS_WRED_TOTAL_BYTES_ADDED_LO_OFF                        (0)
#define PP_QOS_WRED_TOTAL_BYTES_ADDED_LO_LEN                        (32)
#define PP_QOS_WRED_TOTAL_BYTES_ADDED_LO_MSK                        (0xFFFFFFFF)
#define PP_QOS_WRED_TOTAL_BYTES_ADDED_LO_RST                        (0x0)

/**
 * SW_REG_NAME : PP_QOS_WRED_TOTAL_BYTES_ADDED_HI_REG
 * HW_REG_NAME : qos_wred_total_bytes_added_hi
 * DESCRIPTION : total bytes added to q counter value bits 63:32
 *               for the q specified in q address register.
 *
 *  Register Fields :
 *   [31:28][RO] - Reserved
 *   [27: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_TOTAL_BYTES_ADDED_HI_REG      ((QOS_WRED_BASE_ADDR) + 0x0E0)
#define PP_QOS_WRED_TOTAL_BYTES_ADDED_HI_RSVD0_OFF                  (28)
#define PP_QOS_WRED_TOTAL_BYTES_ADDED_HI_RSVD0_LEN                  (4)
#define PP_QOS_WRED_TOTAL_BYTES_ADDED_HI_RSVD0_MSK                  (0xF0000000)
#define PP_QOS_WRED_TOTAL_BYTES_ADDED_HI_RSVD0_RST                  (0x0)
#define PP_QOS_WRED_TOTAL_BYTES_ADDED_HI_OFF                        (0)
#define PP_QOS_WRED_TOTAL_BYTES_ADDED_HI_LEN                        (28)
#define PP_QOS_WRED_TOTAL_BYTES_ADDED_HI_MSK                        (0x0FFFFFFF)
#define PP_QOS_WRED_TOTAL_BYTES_ADDED_HI_RST                        (0x0)

/**
 * SW_REG_NAME : PP_QOS_WRED_TOTAL_BYTES_DROPPED_LO_REG
 * HW_REG_NAME : qos_wred_total_bytes_dropped_lo
 * DESCRIPTION : total bytes dropped to q counter value bits 31:0
 *               for the q specified in q address register.
 *
 *  Register Fields :
 *   [31: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_TOTAL_BYTES_DROPPED_LO_REG    ((QOS_WRED_BASE_ADDR) + 0x0E4)
#define PP_QOS_WRED_TOTAL_BYTES_DROPPED_LO_OFF                      (0)
#define PP_QOS_WRED_TOTAL_BYTES_DROPPED_LO_LEN                      (32)
#define PP_QOS_WRED_TOTAL_BYTES_DROPPED_LO_MSK                      (0xFFFFFFFF)
#define PP_QOS_WRED_TOTAL_BYTES_DROPPED_LO_RST                      (0x0)

/**
 * SW_REG_NAME : PP_QOS_WRED_TOTAL_BYTES_DROPPED_HI_REG
 * HW_REG_NAME : qos_wred_total_bytes_dropped_hi
 * DESCRIPTION : total bytes dropped to q counter value bits 63:32
 *               for the q specified in q address register.
 *
 *  Register Fields :
 *   [31:26][RO] - Reserved
 *   [25: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_TOTAL_BYTES_DROPPED_HI_REG    ((QOS_WRED_BASE_ADDR) + 0x0E8)
#define PP_QOS_WRED_TOTAL_BYTES_DROPPED_HI_RSVD0_OFF                (26)
#define PP_QOS_WRED_TOTAL_BYTES_DROPPED_HI_RSVD0_LEN                (6)
#define PP_QOS_WRED_TOTAL_BYTES_DROPPED_HI_RSVD0_MSK                (0xFC000000)
#define PP_QOS_WRED_TOTAL_BYTES_DROPPED_HI_RSVD0_RST                (0x0)
#define PP_QOS_WRED_TOTAL_BYTES_DROPPED_HI_OFF                      (0)
#define PP_QOS_WRED_TOTAL_BYTES_DROPPED_HI_LEN                      (26)
#define PP_QOS_WRED_TOTAL_BYTES_DROPPED_HI_MSK                      (0x03FFFFFF)
#define PP_QOS_WRED_TOTAL_BYTES_DROPPED_HI_RST                      (0x0)

/**
 * SW_REG_NAME : PP_QOS_WRED_TOTAL_ACCEPTS_REG
 * HW_REG_NAME : qos_wred_total_accepts
 * DESCRIPTION : total packets accepted to q counter value for the
 *               q specified in q address register.
 *
 *  Register Fields :
 *   [31: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_TOTAL_ACCEPTS_REG             ((QOS_WRED_BASE_ADDR) + 0x0EC)
#define PP_QOS_WRED_TOTAL_ACCEPTS_OFF                               (0)
#define PP_QOS_WRED_TOTAL_ACCEPTS_LEN                               (32)
#define PP_QOS_WRED_TOTAL_ACCEPTS_MSK                               (0xFFFFFFFF)
#define PP_QOS_WRED_TOTAL_ACCEPTS_RST                               (0x0)

/**
 * SW_REG_NAME : PP_QOS_WRED_TOTAL_DROPS_REG
 * HW_REG_NAME : qos_wred_total_drops
 * DESCRIPTION : total packets droped from q counter value for the
 *               q specified in q address register.
 *
 *  Register Fields :
 *   [31: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_TOTAL_DROPS_REG               ((QOS_WRED_BASE_ADDR) + 0x0F0)
#define PP_QOS_WRED_TOTAL_DROPS_OFF                                 (0)
#define PP_QOS_WRED_TOTAL_DROPS_LEN                                 (32)
#define PP_QOS_WRED_TOTAL_DROPS_MSK                                 (0xFFFFFFFF)
#define PP_QOS_WRED_TOTAL_DROPS_RST                                 (0x0)

/**
 * SW_REG_NAME : PP_QOS_WRED_TOTAL_RED_DROPS_REG
 * HW_REG_NAME : qos_wred_total_red_drops
 * DESCRIPTION : total red packets droped from q counter value for
 *               the q specified in q address register.
 *
 *  Register Fields :
 *   [31:16][RO] - Reserved
 *   [15: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_TOTAL_RED_DROPS_REG           ((QOS_WRED_BASE_ADDR) + 0x0F4)
#define PP_QOS_WRED_TOTAL_RED_DROPS_RSVD0_OFF                       (16)
#define PP_QOS_WRED_TOTAL_RED_DROPS_RSVD0_LEN                       (16)
#define PP_QOS_WRED_TOTAL_RED_DROPS_RSVD0_MSK                       (0xFFFF0000)
#define PP_QOS_WRED_TOTAL_RED_DROPS_RSVD0_RST                       (0x0)
#define PP_QOS_WRED_TOTAL_RED_DROPS_OFF                             (0)
#define PP_QOS_WRED_TOTAL_RED_DROPS_LEN                             (16)
#define PP_QOS_WRED_TOTAL_RED_DROPS_MSK                             (0x0000FFFF)
#define PP_QOS_WRED_TOTAL_RED_DROPS_RST                             (0x0)

/**
 * SW_REG_NAME : PP_QOS_WRED_MEM_RD_VALID_REG
 * HW_REG_NAME : qos_wred_mem_rd_valid
 * DESCRIPTION : when the read value registers are written, this
 *               register is set.
 *
 *  Register Fields :
 *   [31: 1][RO] - Reserved
 *   [ 0: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_MEM_RD_VALID_REG              ((QOS_WRED_BASE_ADDR) + 0x0F8)
#define PP_QOS_WRED_MEM_RD_VALID_RSVD0_OFF                          (1)
#define PP_QOS_WRED_MEM_RD_VALID_RSVD0_LEN                          (31)
#define PP_QOS_WRED_MEM_RD_VALID_RSVD0_MSK                          (0xFFFFFFFE)
#define PP_QOS_WRED_MEM_RD_VALID_RSVD0_RST                          (0x0)
#define PP_QOS_WRED_MEM_RD_VALID_OFF                                (0)
#define PP_QOS_WRED_MEM_RD_VALID_LEN                                (1)
#define PP_QOS_WRED_MEM_RD_VALID_MSK                                (0x00000001)
#define PP_QOS_WRED_MEM_RD_VALID_RST                                (0x0)

/**
 * SW_REG_NAME : PP_QOS_WRED_TOTAL_RESOURCE_RO_REG
 * HW_REG_NAME : qos_wred_total_resources_ro
 * DESCRIPTION : current value of total available resources
 *               counter.
 *
 *  Register Fields :
 *   [31: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_TOTAL_RESOURCE_RO_REG         ((QOS_WRED_BASE_ADDR) + 0x0FC)
#define PP_QOS_WRED_TOTAL_RESOURCE_RO_OFF                           (0)
#define PP_QOS_WRED_TOTAL_RESOURCE_RO_LEN                           (32)
#define PP_QOS_WRED_TOTAL_RESOURCE_RO_MSK                           (0xFFFFFFFF)
#define PP_QOS_WRED_TOTAL_RESOURCE_RO_RST                           (0x0)

/**
 * SW_REG_NAME : PP_QOS_WRED_TOTAL_RESOURCE_ADD_SUB_REG
 * HW_REG_NAME : qos_wred_total_resources_add_sub
 * DESCRIPTION : value to add (or subtract) to total available
 *               resources counter. on write.
 *
 *  Register Fields :
 *   [31: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_TOTAL_RESOURCE_ADD_SUB_REG    ((QOS_WRED_BASE_ADDR) + 0x100)
#define PP_QOS_WRED_TOTAL_RESOURCE_ADD_SUB_OFF                      (0)
#define PP_QOS_WRED_TOTAL_RESOURCE_ADD_SUB_LEN                      (32)
#define PP_QOS_WRED_TOTAL_RESOURCE_ADD_SUB_MSK                      (0xFFFFFFFF)
#define PP_QOS_WRED_TOTAL_RESOURCE_ADD_SUB_RST                      (0x0)

/**
 * SW_REG_NAME : PP_QOS_WRED_SIGMA_UNUSED_RO_REG
 * HW_REG_NAME : qos_wred_sigma_unused_ro
 * DESCRIPTION : current value sigma unused min guaranteed counter.
 *
 *  Register Fields :
 *   [31: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_SIGMA_UNUSED_RO_REG           ((QOS_WRED_BASE_ADDR) + 0x104)
#define PP_QOS_WRED_SIGMA_UNUSED_RO_OFF                             (0)
#define PP_QOS_WRED_SIGMA_UNUSED_RO_LEN                             (32)
#define PP_QOS_WRED_SIGMA_UNUSED_RO_MSK                             (0xFFFFFFFF)
#define PP_QOS_WRED_SIGMA_UNUSED_RO_RST                             (0x0)

/**
 * SW_REG_NAME : PP_QOS_WRED_DEFAULT_COLOR_REG
 * HW_REG_NAME : qos_wred_default_color
 * DESCRIPTION : define default color (0) as green/yellow/red.
 *               1-green, 2-yellow, 3-red.
 *
 *  Register Fields :
 *   [31: 2][RO] - Reserved
 *   [ 1: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_DEFAULT_COLOR_REG             ((QOS_WRED_BASE_ADDR) + 0x108)
#define PP_QOS_WRED_DEFAULT_COLOR_RSVD0_OFF                         (2)
#define PP_QOS_WRED_DEFAULT_COLOR_RSVD0_LEN                         (30)
#define PP_QOS_WRED_DEFAULT_COLOR_RSVD0_MSK                         (0xFFFFFFFC)
#define PP_QOS_WRED_DEFAULT_COLOR_RSVD0_RST                         (0x0)
#define PP_QOS_WRED_DEFAULT_COLOR_OFF                               (0)
#define PP_QOS_WRED_DEFAULT_COLOR_LEN                               (2)
#define PP_QOS_WRED_DEFAULT_COLOR_MSK                               (0x00000003)
#define PP_QOS_WRED_DEFAULT_COLOR_RST                               (0x1)

/**
 * SW_REG_NAME : PP_QOS_WRED_MTU_REG
 * HW_REG_NAME : qos_wred_mtu
 * DESCRIPTION : used as threshold for queue size.
 *
 *  Register Fields :
 *   [31: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_MTU_REG                       ((QOS_WRED_BASE_ADDR) + 0x10C)
#define PP_QOS_WRED_MTU_OFF                                         (0)
#define PP_QOS_WRED_MTU_LEN                                         (32)
#define PP_QOS_WRED_MTU_MSK                                         (0xFFFFFFFF)
#define PP_QOS_WRED_MTU_RST                                         (0x5ea)

/**
 * SW_REG_NAME : PP_QOS_WRED_AQM_OCCUPANCY_CNT_REG
 * HW_REG_NAME : qos_wred_aqm_occupancy_cnt
 * DESCRIPTION : Per DAQM context. Increment upon accepted packet;
 *               decrement upon OCP write; readable to control path
 *               SW
 *
 *  Register Fields :
 *   [31: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_AQM_OCCUPANCY_CNT_REG         ((QOS_WRED_BASE_ADDR) + 0x110)
#define PP_QOS_WRED_AQM_OCCUPANCY_CNT_OFF                           (0)
#define PP_QOS_WRED_AQM_OCCUPANCY_CNT_LEN                           (32)
#define PP_QOS_WRED_AQM_OCCUPANCY_CNT_MSK                           (0xFFFFFFFF)
#define PP_QOS_WRED_AQM_OCCUPANCY_CNT_RST                           (0x0)
/**
 * REG_IDX_ACCESS   : PP_QOS_WRED_AQM_OCCUPANCY_CNT_REG_IDX
 * NUM OF REGISTERS : 16
 */
#define PP_QOS_WRED_AQM_OCCUPANCY_CNT_REG_IDX(idx) \
	(PP_QOS_WRED_AQM_OCCUPANCY_CNT_REG + ((idx) << 5))

/**
 * SW_REG_NAME : PP_QOS_WRED_AQM_BURST_ALLOW_REG
 * HW_REG_NAME : qos_wred_aqm_burst_allowance
 * DESCRIPTION : Per DAQM context. Countdown timer which allows up
 *               to 500ms (counts up to 5000 times of 0.1ms timer)
 *
 *  Register Fields :
 *   [31:13][RO] - Reserved
 *   [12: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_AQM_BURST_ALLOW_REG           ((QOS_WRED_BASE_ADDR) + 0x114)
#define PP_QOS_WRED_AQM_BURST_ALLOW_RSVD0_OFF                       (13)
#define PP_QOS_WRED_AQM_BURST_ALLOW_RSVD0_LEN                       (19)
#define PP_QOS_WRED_AQM_BURST_ALLOW_RSVD0_MSK                       (0xFFFFE000)
#define PP_QOS_WRED_AQM_BURST_ALLOW_RSVD0_RST                       (0x0)
#define PP_QOS_WRED_AQM_BURST_ALLOW_OFF                             (0)
#define PP_QOS_WRED_AQM_BURST_ALLOW_LEN                             (13)
#define PP_QOS_WRED_AQM_BURST_ALLOW_MSK                             (0x00001FFF)
#define PP_QOS_WRED_AQM_BURST_ALLOW_RST                             (0x0)
/**
 * REG_IDX_ACCESS   : PP_QOS_WRED_AQM_BURST_ALLOW_REG_IDX
 * NUM OF REGISTERS : 16
 */
#define PP_QOS_WRED_AQM_BURST_ALLOW_REG_IDX(idx) \
	(PP_QOS_WRED_AQM_BURST_ALLOW_REG + ((idx) << 5))

/**
 * SW_REG_NAME : PP_QOS_WRED_AQM_ACCU_PROB_REG
 * HW_REG_NAME : qos_wred_aqm_accu_prob
 * DESCRIPTION : Per DAQM context. accumulated drop prob since last
 *               drop.
 *
 *  Register Fields :
 *   [31: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_AQM_ACCU_PROB_REG             ((QOS_WRED_BASE_ADDR) + 0x118)
#define PP_QOS_WRED_AQM_ACCU_PROB_OFF                               (0)
#define PP_QOS_WRED_AQM_ACCU_PROB_LEN                               (32)
#define PP_QOS_WRED_AQM_ACCU_PROB_MSK                               (0xFFFFFFFF)
#define PP_QOS_WRED_AQM_ACCU_PROB_RST                               (0x0)
/**
 * REG_IDX_ACCESS   : PP_QOS_WRED_AQM_ACCU_PROB_REG_IDX
 * NUM OF REGISTERS : 16
 */
#define PP_QOS_WRED_AQM_ACCU_PROB_REG_IDX(idx) \
	(PP_QOS_WRED_AQM_ACCU_PROB_REG + ((idx) << 5))

/**
 * SW_REG_NAME : PP_QOS_WRED_AQM_DROP_PROB_REG
 * HW_REG_NAME : qos_wred_aqm_drop_prob
 * DESCRIPTION : Per DAQM context. Set by control path SW. The
 *               current packet drop probability. represented by 4
 *               bits before decimal point, 24 bits after.
 *
 *  Register Fields :
 *   [31:28][RO] - Reserved
 *   [27: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_AQM_DROP_PROB_REG             ((QOS_WRED_BASE_ADDR) + 0x11c)
#define PP_QOS_WRED_AQM_DROP_PROB_RSVD0_OFF                         (28)
#define PP_QOS_WRED_AQM_DROP_PROB_RSVD0_LEN                         (4)
#define PP_QOS_WRED_AQM_DROP_PROB_RSVD0_MSK                         (0xF0000000)
#define PP_QOS_WRED_AQM_DROP_PROB_RSVD0_RST                         (0x0)
#define PP_QOS_WRED_AQM_DROP_PROB_OFF                               (0)
#define PP_QOS_WRED_AQM_DROP_PROB_LEN                               (28)
#define PP_QOS_WRED_AQM_DROP_PROB_MSK                               (0x0FFFFFFF)
#define PP_QOS_WRED_AQM_DROP_PROB_RST                               (0x0)
/**
 * REG_IDX_ACCESS   : PP_QOS_WRED_AQM_DROP_PROB_REG_IDX
 * NUM OF REGISTERS : 16
 */
#define PP_QOS_WRED_AQM_DROP_PROB_REG_IDX(idx) \
	(PP_QOS_WRED_AQM_DROP_PROB_REG + ((idx) << 5))

/**
 * SW_REG_NAME : PP_QOS_WRED_AQM_BURST_STATE_REG
 * HW_REG_NAME : qos_wred_aqm_burst_state
 * DESCRIPTION : Per DAQM context. 0 - NOBURST - no burst yet. 1 -
 *               FIRST_BURST - first burst detected, no protection
 *               yet. 2 - PROTECT_BURST - first burst detected,
 *               protecting burst if burst_allowance_ ) 0
 *
 *  Register Fields :
 *   [31: 2][RO] - Reserved
 *   [ 1: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_AQM_BURST_STATE_REG           ((QOS_WRED_BASE_ADDR) + 0x120)
#define PP_QOS_WRED_AQM_BURST_STATE_RSVD0_OFF                       (2)
#define PP_QOS_WRED_AQM_BURST_STATE_RSVD0_LEN                       (30)
#define PP_QOS_WRED_AQM_BURST_STATE_RSVD0_MSK                       (0xFFFFFFFC)
#define PP_QOS_WRED_AQM_BURST_STATE_RSVD0_RST                       (0x0)
#define PP_QOS_WRED_AQM_BURST_STATE_OFF                             (0)
#define PP_QOS_WRED_AQM_BURST_STATE_LEN                             (2)
#define PP_QOS_WRED_AQM_BURST_STATE_MSK                             (0x00000003)
#define PP_QOS_WRED_AQM_BURST_STATE_RST                             (0x0)
/**
 * REG_IDX_ACCESS   : PP_QOS_WRED_AQM_BURST_STATE_REG_IDX
 * NUM OF REGISTERS : 16
 */
#define PP_QOS_WRED_AQM_BURST_STATE_REG_IDX(idx) \
	(PP_QOS_WRED_AQM_BURST_STATE_REG + ((idx) << 5))

/**
 * SW_REG_NAME : PP_QOS_WRED_AQM_QDELAY_STATUS_REG
 * HW_REG_NAME : qos_wred_aqm_qdelay_status
 * DESCRIPTION : Per DAQM context. The status of: (qdelay_old_ (
 *               0.5 * LATENCY_TARGET andand drop_prob_ ( 0.2).
 *               from SW.
 *
 *  Register Fields :
 *   [31: 1][RO] - Reserved
 *   [ 0: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_AQM_QDELAY_STATUS_REG         ((QOS_WRED_BASE_ADDR) + 0x124)
#define PP_QOS_WRED_AQM_QDELAY_STATUS_RSVD0_OFF                     (1)
#define PP_QOS_WRED_AQM_QDELAY_STATUS_RSVD0_LEN                     (31)
#define PP_QOS_WRED_AQM_QDELAY_STATUS_RSVD0_MSK                     (0xFFFFFFFE)
#define PP_QOS_WRED_AQM_QDELAY_STATUS_RSVD0_RST                     (0x0)
#define PP_QOS_WRED_AQM_QDELAY_STATUS_OFF                           (0)
#define PP_QOS_WRED_AQM_QDELAY_STATUS_LEN                           (1)
#define PP_QOS_WRED_AQM_QDELAY_STATUS_MSK                           (0x00000001)
#define PP_QOS_WRED_AQM_QDELAY_STATUS_RST                           (0x0)
/**
 * REG_IDX_ACCESS   : PP_QOS_WRED_AQM_QDELAY_STATUS_REG_IDX
 * NUM OF REGISTERS : 16
 */
#define PP_QOS_WRED_AQM_QDELAY_STATUS_REG_IDX(idx) \
	(PP_QOS_WRED_AQM_QDELAY_STATUS_REG + ((idx) << 5))

/**
 * SW_REG_NAME : PP_QOS_WRED_AQM_OCCUPANCY_DEC_REG
 * HW_REG_NAME : qos_wred_aqm_occupancy_decrement
 * DESCRIPTION : The number of bytes to decrement occupancy counter
 *               per context.
 *
 *  Register Fields :
 *   [31: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_AQM_OCCUPANCY_DEC_REG         ((QOS_WRED_BASE_ADDR) + 0x128)
#define PP_QOS_WRED_AQM_OCCUPANCY_DEC_OFF                           (0)
#define PP_QOS_WRED_AQM_OCCUPANCY_DEC_LEN                           (32)
#define PP_QOS_WRED_AQM_OCCUPANCY_DEC_MSK                           (0xFFFFFFFF)
#define PP_QOS_WRED_AQM_OCCUPANCY_DEC_RST                           (0x0)
/**
 * REG_IDX_ACCESS   : PP_QOS_WRED_AQM_OCCUPANCY_DEC_REG_IDX
 * NUM OF REGISTERS : 16
 */
#define PP_QOS_WRED_AQM_OCCUPANCY_DEC_REG_IDX(idx) \
	(PP_QOS_WRED_AQM_OCCUPANCY_DEC_REG + ((idx) << 5))

/**
 * SW_REG_NAME : PP_QOS_WRED_AQM_OCCUPANCY_INC_REG
 * HW_REG_NAME : qos_wred_aqm_occupancy_increment
 * DESCRIPTION : The number of bytes to increment occupancy counter
 *               per context.
 *
 *  Register Fields :
 *   [31: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_AQM_OCCUPANCY_INC_REG         ((QOS_WRED_BASE_ADDR) + 0x12c)
#define PP_QOS_WRED_AQM_OCCUPANCY_INC_OFF                           (0)
#define PP_QOS_WRED_AQM_OCCUPANCY_INC_LEN                           (32)
#define PP_QOS_WRED_AQM_OCCUPANCY_INC_MSK                           (0xFFFFFFFF)
#define PP_QOS_WRED_AQM_OCCUPANCY_INC_RST                           (0x0)
/**
 * REG_IDX_ACCESS   : PP_QOS_WRED_AQM_OCCUPANCY_INC_REG_IDX
 * NUM OF REGISTERS : 16
 */
#define PP_QOS_WRED_AQM_OCCUPANCY_INC_REG_IDX(idx) \
	(PP_QOS_WRED_AQM_OCCUPANCY_INC_REG + ((idx) << 5))

/**
 * SW_REG_NAME : PP_QOS_WRED_AQM_CTRL_PATH_COND_REG
 * HW_REG_NAME : qos_wred_aqm_control_path_condition
 * DESCRIPTION : bits 0-15 are for control path condition true per
 *               aqm context, bits 16-31 are for control path
 *               condition false.
 *
 *  Register Fields :
 *   [31:16][RW] - MISSING_DESCRIPTION
 *   [15: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_AQM_CTRL_PATH_COND_REG        ((QOS_WRED_BASE_ADDR) + 0x310)
#define PP_QOS_WRED_AQM_CTRL_PATH_COND_AQM_CTRL_PATH_COND_FALSE_OFF (16)
#define PP_QOS_WRED_AQM_CTRL_PATH_COND_AQM_CTRL_PATH_COND_FALSE_LEN (16)
#define PP_QOS_WRED_AQM_CTRL_PATH_COND_AQM_CTRL_PATH_COND_FALSE_MSK (0xFFFF0000)
#define PP_QOS_WRED_AQM_CTRL_PATH_COND_AQM_CTRL_PATH_COND_FALSE_RST (0x0)
#define PP_QOS_WRED_AQM_CTRL_PATH_COND_AQM_CTRL_PATH_COND_TRUE_OFF  (0)
#define PP_QOS_WRED_AQM_CTRL_PATH_COND_AQM_CTRL_PATH_COND_TRUE_LEN  (16)
#define PP_QOS_WRED_AQM_CTRL_PATH_COND_AQM_CTRL_PATH_COND_TRUE_MSK  (0x0000FFFF)
#define PP_QOS_WRED_AQM_CTRL_PATH_COND_AQM_CTRL_PATH_COND_TRUE_RST  (0x0)

/**
 * SW_REG_NAME : PP_QOS_WRED_AQM_BURST_ALLOW_LOAD_VAL_REG
 * HW_REG_NAME : qos_wred_aqm_burst_allowance_load_val
 * DESCRIPTION : value to load to burst allowance timers (up to
 *               5000)
 *
 *  Register Fields :
 *   [31:13][RO] - Reserved
 *   [12: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_AQM_BURST_ALLOW_LOAD_VAL_REG  ((QOS_WRED_BASE_ADDR) + 0x314)
#define PP_QOS_WRED_AQM_BURST_ALLOW_LOAD_VAL_RSVD0_OFF              (13)
#define PP_QOS_WRED_AQM_BURST_ALLOW_LOAD_VAL_RSVD0_LEN              (19)
#define PP_QOS_WRED_AQM_BURST_ALLOW_LOAD_VAL_RSVD0_MSK              (0xFFFFE000)
#define PP_QOS_WRED_AQM_BURST_ALLOW_LOAD_VAL_RSVD0_RST              (0x0)
#define PP_QOS_WRED_AQM_BURST_ALLOW_LOAD_VAL_OFF                    (0)
#define PP_QOS_WRED_AQM_BURST_ALLOW_LOAD_VAL_LEN                    (13)
#define PP_QOS_WRED_AQM_BURST_ALLOW_LOAD_VAL_MSK                    (0x00001FFF)
#define PP_QOS_WRED_AQM_BURST_ALLOW_LOAD_VAL_RST                    (0x5dc)

/**
 * SW_REG_NAME : PP_QOS_WRED_AQM_BURST_ALLOW_NOT_ZERO_REG
 * HW_REG_NAME : qos_wred_aqm_burst_allowance_not_zero
 * DESCRIPTION : per context 1 means counter!=0
 *
 *  Register Fields :
 *   [31:16][RO] - Reserved
 *   [15: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_AQM_BURST_ALLOW_NOT_ZERO_REG  ((QOS_WRED_BASE_ADDR) + 0x318)
#define PP_QOS_WRED_AQM_BURST_ALLOW_NOT_ZERO_RSVD0_OFF              (16)
#define PP_QOS_WRED_AQM_BURST_ALLOW_NOT_ZERO_RSVD0_LEN              (16)
#define PP_QOS_WRED_AQM_BURST_ALLOW_NOT_ZERO_RSVD0_MSK              (0xFFFF0000)
#define PP_QOS_WRED_AQM_BURST_ALLOW_NOT_ZERO_RSVD0_RST              (0x0)
#define PP_QOS_WRED_AQM_BURST_ALLOW_NOT_ZERO_BA_NOT_ZERO_OFF        (0)
#define PP_QOS_WRED_AQM_BURST_ALLOW_NOT_ZERO_BA_NOT_ZERO_LEN        (16)
#define PP_QOS_WRED_AQM_BURST_ALLOW_NOT_ZERO_BA_NOT_ZERO_MSK        (0x0000FFFF)
#define PP_QOS_WRED_AQM_BURST_ALLOW_NOT_ZERO_BA_NOT_ZERO_RST        (0x0)

/**
 * SW_REG_NAME : PP_QOS_WRED_AQM_BUFF_SIZE_DEV3_OBSOLETE_REG
 * HW_REG_NAME : qos_wred_aqm_buff_size_dev3_obsolete
 * DESCRIPTION : buffer size for aqm algorithm, devided by 3
 *               because that's what the algorithm uses., NOTE, B
 *               Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_AQM_BUFF_SIZE_DEV3_OBSOLETE_REG \
	((QOS_WRED_BASE_ADDR) + 0x31C)
#define PP_QOS_WRED_AQM_BUFF_SIZE_DEV3_OBSOLETE_OFF                 (0)
#define PP_QOS_WRED_AQM_BUFF_SIZE_DEV3_OBSOLETE_LEN                 (32)
#define PP_QOS_WRED_AQM_BUFF_SIZE_DEV3_OBSOLETE_MSK                 (0xFFFFFFFF)
#define PP_QOS_WRED_AQM_BUFF_SIZE_DEV3_OBSOLETE_RST                 (0x0)

/**
 * SW_REG_NAME : PP_QOS_WRED_AQM_INC_OVERHEAD_REG
 * HW_REG_NAME : qos_wred_aqm_increment_overhead
 * DESCRIPTION : overhead to add on every packet to occupancy
 *               counter
 *
 *  Register Fields :
 *   [31:16][RO] - Reserved
 *   [15: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_AQM_INC_OVERHEAD_REG          ((QOS_WRED_BASE_ADDR) + 0x320)
#define PP_QOS_WRED_AQM_INC_OVERHEAD_RSVD0_OFF                      (16)
#define PP_QOS_WRED_AQM_INC_OVERHEAD_RSVD0_LEN                      (16)
#define PP_QOS_WRED_AQM_INC_OVERHEAD_RSVD0_MSK                      (0xFFFF0000)
#define PP_QOS_WRED_AQM_INC_OVERHEAD_RSVD0_RST                      (0x0)
#define PP_QOS_WRED_AQM_INC_OVERHEAD_OFF                            (0)
#define PP_QOS_WRED_AQM_INC_OVERHEAD_LEN                            (16)
#define PP_QOS_WRED_AQM_INC_OVERHEAD_MSK                            (0x0000FFFF)
#define PP_QOS_WRED_AQM_INC_OVERHEAD_RST                            (0x0)

/**
 * SW_REG_NAME : PP_QOS_WRED_FAKE_POP_COUNT_REG
 * HW_REG_NAME : qos_wred_fake_pop_count_reg
 * DESCRIPTION : Fake Pop (cancel push by rxdma) Counter, NOTE, B
 *               Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_FAKE_POP_COUNT_REG            ((QOS_WRED_BASE_ADDR) + 0x324)
#define PP_QOS_WRED_FAKE_POP_COUNT_FAKE_POP_CNT_OFF                 (0)
#define PP_QOS_WRED_FAKE_POP_COUNT_FAKE_POP_CNT_LEN                 (32)
#define PP_QOS_WRED_FAKE_POP_COUNT_FAKE_POP_CNT_MSK                 (0xFFFFFFFF)
#define PP_QOS_WRED_FAKE_POP_COUNT_FAKE_POP_CNT_RST                 (0x0)

/**
 * SW_REG_NAME : PP_QOS_WRED_POP_BYTE_UNDERFLOW_COUNT_REG
 * HW_REG_NAME : qos_wred_pop_byte_underflow_count
 * DESCRIPTION : Available three times, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_POP_BYTE_UNDERFLOW_COUNT_REG  ((QOS_WRED_BASE_ADDR) + 0x328)
#define PP_QOS_WRED_POP_BYTE_UNDERFLOW_COUNT_POP_UNDERFLOW_COUNT_OFF         (0)
#define PP_QOS_WRED_POP_BYTE_UNDERFLOW_COUNT_POP_UNDERFLOW_COUNT_LEN        (32)
#define PP_QOS_WRED_POP_BYTE_UNDERFLOW_COUNT_POP_UNDERFLOW_COUNT_MSK \
	(0xFFFFFFFF)
#define PP_QOS_WRED_POP_BYTE_UNDERFLOW_COUNT_POP_UNDERFLOW_COUNT_RST       (0x0)

/**
 * SW_REG_NAME : PP_QOS_WRED_POP_BYTE_UNDERFLOW_SUM_REG
 * HW_REG_NAME : qos_wred_pop_byte_underflow_sum
 * DESCRIPTION : Available three times, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_POP_BYTE_UNDERFLOW_SUM_REG    ((QOS_WRED_BASE_ADDR) + 0x32C)
#define PP_QOS_WRED_POP_BYTE_UNDERFLOW_SUM_POP_UNDERFLOW_COUNT_OFF  (0)
#define PP_QOS_WRED_POP_BYTE_UNDERFLOW_SUM_POP_UNDERFLOW_COUNT_LEN  (32)
#define PP_QOS_WRED_POP_BYTE_UNDERFLOW_SUM_POP_UNDERFLOW_COUNT_MSK  (0xFFFFFFFF)
#define PP_QOS_WRED_POP_BYTE_UNDERFLOW_SUM_POP_UNDERFLOW_COUNT_RST  (0x0)

/**
 * SW_REG_NAME : PP_QOS_WRED_INIT_REG
 * HW_REG_NAME : qos_wred_init
 * DESCRIPTION : Writing to this reg start initialization of WRED,
 *               NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31:31][RO] - MISSING_DESCRIPTION
 *   [30: 1][RO] - Reserved
 *   [ 0: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_INIT_REG                      ((QOS_WRED_BASE_ADDR) + 0x330)
#define PP_QOS_WRED_INIT_INIT_DONE_OFF                              (31)
#define PP_QOS_WRED_INIT_INIT_DONE_LEN                              (1)
#define PP_QOS_WRED_INIT_INIT_DONE_MSK                              (0x80000000)
#define PP_QOS_WRED_INIT_INIT_DONE_RST                              (0x0)
#define PP_QOS_WRED_INIT_RSVD0_OFF                                  (1)
#define PP_QOS_WRED_INIT_RSVD0_LEN                                  (30)
#define PP_QOS_WRED_INIT_RSVD0_MSK                                  (0x7FFFFFFE)
#define PP_QOS_WRED_INIT_RSVD0_RST                                  (0x0)
#define PP_QOS_WRED_INIT_START_INIT_OFF                             (0)
#define PP_QOS_WRED_INIT_START_INIT_LEN                             (1)
#define PP_QOS_WRED_INIT_START_INIT_MSK                             (0x00000001)
#define PP_QOS_WRED_INIT_START_INIT_RST                             (0x0)

/**
 * SW_REG_NAME : PP_QOS_WRED_LAST_PUSH_H_ADDR_REG
 * HW_REG_NAME : qos_wred_last_push_h_addr
 * DESCRIPTION : Last push high address, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 8][RO] - Reserved
 *   [ 7: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_LAST_PUSH_H_ADDR_REG          ((QOS_WRED_BASE_ADDR) + 0x334)
#define PP_QOS_WRED_LAST_PUSH_H_ADDR_RSVD0_OFF                      (8)
#define PP_QOS_WRED_LAST_PUSH_H_ADDR_RSVD0_LEN                      (24)
#define PP_QOS_WRED_LAST_PUSH_H_ADDR_RSVD0_MSK                      (0xFFFFFF00)
#define PP_QOS_WRED_LAST_PUSH_H_ADDR_RSVD0_RST                      (0x0)
#define PP_QOS_WRED_LAST_PUSH_H_ADDR_OFF                            (0)
#define PP_QOS_WRED_LAST_PUSH_H_ADDR_LEN                            (8)
#define PP_QOS_WRED_LAST_PUSH_H_ADDR_MSK                            (0x000000FF)
#define PP_QOS_WRED_LAST_PUSH_H_ADDR_RST                            (0x0)

/**
 * SW_REG_NAME : PP_QOS_WRED_LAST_PUSH_L_ADDR_REG
 * HW_REG_NAME : qos_wred_last_push_l_addr
 * DESCRIPTION : Last push low address, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_LAST_PUSH_L_ADDR_REG          ((QOS_WRED_BASE_ADDR) + 0x338)
#define PP_QOS_WRED_LAST_PUSH_L_ADDR_OFF                            (0)
#define PP_QOS_WRED_LAST_PUSH_L_ADDR_LEN                            (32)
#define PP_QOS_WRED_LAST_PUSH_L_ADDR_MSK                            (0xFFFFFFFF)
#define PP_QOS_WRED_LAST_PUSH_L_ADDR_RST                            (0x0)

/**
 * SW_REG_NAME : PP_QOS_WRED_LAST_PUSH_DROP_REG
 * HW_REG_NAME : qos_wred_last_push_drop
 * DESCRIPTION : Last push drop, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 7][RO] - Reserved
 *   [ 6: 6][RO] - MISSING_DESCRIPTION
 *   [ 5: 5][RO] - MISSING_DESCRIPTION
 *   [ 4: 4][RO] - MISSING_DESCRIPTION
 *   [ 3: 3][RO] - MISSING_DESCRIPTION
 *   [ 2: 2][RO] - MISSING_DESCRIPTION
 *   [ 1: 1][RO] - MISSING_DESCRIPTION
 *   [ 0: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_LAST_PUSH_DROP_REG            ((QOS_WRED_BASE_ADDR) + 0x33C)
#define PP_QOS_WRED_LAST_PUSH_DROP_RSVD0_OFF                        (7)
#define PP_QOS_WRED_LAST_PUSH_DROP_RSVD0_LEN                        (25)
#define PP_QOS_WRED_LAST_PUSH_DROP_RSVD0_MSK                        (0xFFFFFF80)
#define PP_QOS_WRED_LAST_PUSH_DROP_RSVD0_RST                        (0x0)
#define PP_QOS_WRED_LAST_PUSH_DROP_AQM_DROP_OFF                     (6)
#define PP_QOS_WRED_LAST_PUSH_DROP_AQM_DROP_LEN                     (1)
#define PP_QOS_WRED_LAST_PUSH_DROP_AQM_DROP_MSK                     (0x00000040)
#define PP_QOS_WRED_LAST_PUSH_DROP_AQM_DROP_RST                     (0x0)
#define PP_QOS_WRED_LAST_PUSH_DROP_NACTIVE_DROP_OFF                 (5)
#define PP_QOS_WRED_LAST_PUSH_DROP_NACTIVE_DROP_LEN                 (1)
#define PP_QOS_WRED_LAST_PUSH_DROP_NACTIVE_DROP_MSK                 (0x00000020)
#define PP_QOS_WRED_LAST_PUSH_DROP_NACTIVE_DROP_RST                 (0x0)
#define PP_QOS_WRED_LAST_PUSH_DROP_RED_DROP_OFF                     (4)
#define PP_QOS_WRED_LAST_PUSH_DROP_RED_DROP_LEN                     (1)
#define PP_QOS_WRED_LAST_PUSH_DROP_RED_DROP_MSK                     (0x00000010)
#define PP_QOS_WRED_LAST_PUSH_DROP_RED_DROP_RST                     (0x0)
#define PP_QOS_WRED_LAST_PUSH_DROP_QM_DROP_OFF                      (3)
#define PP_QOS_WRED_LAST_PUSH_DROP_QM_DROP_LEN                      (1)
#define PP_QOS_WRED_LAST_PUSH_DROP_QM_DROP_MSK                      (0x00000008)
#define PP_QOS_WRED_LAST_PUSH_DROP_QM_DROP_RST                      (0x0)
#define PP_QOS_WRED_LAST_PUSH_DROP_MAX_DROP_OFF                     (2)
#define PP_QOS_WRED_LAST_PUSH_DROP_MAX_DROP_LEN                     (1)
#define PP_QOS_WRED_LAST_PUSH_DROP_MAX_DROP_MSK                     (0x00000004)
#define PP_QOS_WRED_LAST_PUSH_DROP_MAX_DROP_RST                     (0x0)
#define PP_QOS_WRED_LAST_PUSH_DROP_WRED_DROP_OFF                    (1)
#define PP_QOS_WRED_LAST_PUSH_DROP_WRED_DROP_LEN                    (1)
#define PP_QOS_WRED_LAST_PUSH_DROP_WRED_DROP_MSK                    (0x00000002)
#define PP_QOS_WRED_LAST_PUSH_DROP_WRED_DROP_RST                    (0x0)
#define PP_QOS_WRED_LAST_PUSH_DROP_OFF                              (0)
#define PP_QOS_WRED_LAST_PUSH_DROP_LEN                              (1)
#define PP_QOS_WRED_LAST_PUSH_DROP_MSK                              (0x00000001)
#define PP_QOS_WRED_LAST_PUSH_DROP_RST                              (0x0)

/**
 * SW_REG_NAME : PP_QOS_WRED_LAST_PUSH_INFO_REG
 * HW_REG_NAME : qos_wred_last_push_info
 * DESCRIPTION : Last push information, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31:23][RO] - MISSING_DESCRIPTION
 *   [22:18][RO] - Reserved
 *   [17:16][RO] - MISSING_DESCRIPTION
 *   [15: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_LAST_PUSH_INFO_REG            ((QOS_WRED_BASE_ADDR) + 0x340)
#define PP_QOS_WRED_LAST_PUSH_INFO_Q_ID_OFF                         (23)
#define PP_QOS_WRED_LAST_PUSH_INFO_Q_ID_LEN                         (9)
#define PP_QOS_WRED_LAST_PUSH_INFO_Q_ID_MSK                         (0xFF800000)
#define PP_QOS_WRED_LAST_PUSH_INFO_Q_ID_RST                         (0x0)
#define PP_QOS_WRED_LAST_PUSH_INFO_RSVD0_OFF                        (18)
#define PP_QOS_WRED_LAST_PUSH_INFO_RSVD0_LEN                        (5)
#define PP_QOS_WRED_LAST_PUSH_INFO_RSVD0_MSK                        (0x007C0000)
#define PP_QOS_WRED_LAST_PUSH_INFO_RSVD0_RST                        (0x0)
#define PP_QOS_WRED_LAST_PUSH_INFO_COLOR_OFF                        (16)
#define PP_QOS_WRED_LAST_PUSH_INFO_COLOR_LEN                        (2)
#define PP_QOS_WRED_LAST_PUSH_INFO_COLOR_MSK                        (0x00030000)
#define PP_QOS_WRED_LAST_PUSH_INFO_COLOR_RST                        (0x0)
#define PP_QOS_WRED_LAST_PUSH_INFO_PKT_SIZE_OFF                     (0)
#define PP_QOS_WRED_LAST_PUSH_INFO_PKT_SIZE_LEN                     (16)
#define PP_QOS_WRED_LAST_PUSH_INFO_PKT_SIZE_MSK                     (0x0000FFFF)
#define PP_QOS_WRED_LAST_PUSH_INFO_PKT_SIZE_RST                     (0x0)

/**
 * SW_REG_NAME : PP_QOS_WRED_LAST_POP_INFO_REG
 * HW_REG_NAME : qos_wred_last_pop_info
 * DESCRIPTION : Last pop information, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31:23][RO] - MISSING_DESCRIPTION
 *   [22:19][RO] - Reserved
 *   [18:18][RO] - MISSING_DESCRIPTION
 *   [17:16][RO] - MISSING_DESCRIPTION
 *   [15: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_LAST_POP_INFO_REG             ((QOS_WRED_BASE_ADDR) + 0x344)
#define PP_QOS_WRED_LAST_POP_INFO_Q_ID_OFF                          (23)
#define PP_QOS_WRED_LAST_POP_INFO_Q_ID_LEN                          (9)
#define PP_QOS_WRED_LAST_POP_INFO_Q_ID_MSK                          (0xFF800000)
#define PP_QOS_WRED_LAST_POP_INFO_Q_ID_RST                          (0x0)
#define PP_QOS_WRED_LAST_POP_INFO_RSVD0_OFF                         (19)
#define PP_QOS_WRED_LAST_POP_INFO_RSVD0_LEN                         (4)
#define PP_QOS_WRED_LAST_POP_INFO_RSVD0_MSK                         (0x00780000)
#define PP_QOS_WRED_LAST_POP_INFO_RSVD0_RST                         (0x0)
#define PP_QOS_WRED_LAST_POP_INFO_FAKE_OFF                          (18)
#define PP_QOS_WRED_LAST_POP_INFO_FAKE_LEN                          (1)
#define PP_QOS_WRED_LAST_POP_INFO_FAKE_MSK                          (0x00040000)
#define PP_QOS_WRED_LAST_POP_INFO_FAKE_RST                          (0x0)
#define PP_QOS_WRED_LAST_POP_INFO_COLOR_OFF                         (16)
#define PP_QOS_WRED_LAST_POP_INFO_COLOR_LEN                         (2)
#define PP_QOS_WRED_LAST_POP_INFO_COLOR_MSK                         (0x00030000)
#define PP_QOS_WRED_LAST_POP_INFO_COLOR_RST                         (0x0)
#define PP_QOS_WRED_LAST_POP_INFO_PKT_SIZE_OFF                      (0)
#define PP_QOS_WRED_LAST_POP_INFO_PKT_SIZE_LEN                      (16)
#define PP_QOS_WRED_LAST_POP_INFO_PKT_SIZE_MSK                      (0x0000FFFF)
#define PP_QOS_WRED_LAST_POP_INFO_PKT_SIZE_RST                      (0x0)

/**
 * SW_REG_NAME : PP_QOS_WRED_POP_PORT_BYTE_UNDERFLOW_COUNT_REG
 * HW_REG_NAME : qos_wred_pop_port_byte_underflow_count
 * DESCRIPTION : Available three times, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_POP_PORT_BYTE_UNDERFLOW_COUNT_REG \
	((QOS_WRED_BASE_ADDR) + 0x348)
#define PP_QOS_WRED_POP_PORT_BYTE_UNDERFLOW_COUNT_POP_UNDERFLOW_COUNT_OFF    (0)
#define PP_QOS_WRED_POP_PORT_BYTE_UNDERFLOW_COUNT_POP_UNDERFLOW_COUNT_LEN   (32)
#define PP_QOS_WRED_POP_PORT_BYTE_UNDERFLOW_COUNT_POP_UNDERFLOW_COUNT_MSK \
	(0xFFFFFFFF)
#define PP_QOS_WRED_POP_PORT_BYTE_UNDERFLOW_COUNT_POP_UNDERFLOW_COUNT_RST  (0x0)

/**
 * SW_REG_NAME : PP_QOS_WRED_POP_PORT_BYTE_UNDERFLOW_SUM_REG
 * HW_REG_NAME : qos_wred_pop_port_byte_underflow_sum
 * DESCRIPTION : Available three times, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_POP_PORT_BYTE_UNDERFLOW_SUM_REG \
	((QOS_WRED_BASE_ADDR) + 0x34C)
#define PP_QOS_WRED_POP_PORT_BYTE_UNDERFLOW_SUM_POP_UNDERFLOW_COUNT_OFF      (0)
#define PP_QOS_WRED_POP_PORT_BYTE_UNDERFLOW_SUM_POP_UNDERFLOW_COUNT_LEN     (32)
#define PP_QOS_WRED_POP_PORT_BYTE_UNDERFLOW_SUM_POP_UNDERFLOW_COUNT_MSK \
	(0xFFFFFFFF)
#define PP_QOS_WRED_POP_PORT_BYTE_UNDERFLOW_SUM_POP_UNDERFLOW_COUNT_RST    (0x0)

/**
 * SW_REG_NAME : PP_QOS_WRED_QOS_INFO_MEM_REG
 * HW_REG_NAME : qos_info_mem
 * DESCRIPTION : MISSING Description, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_QOS_INFO_MEM_REG              ((QOS_WRED_BASE_ADDR) + 0x380)
#define PP_QOS_WRED_QOS_INFO_MEM_OFF                                (0)
#define PP_QOS_WRED_QOS_INFO_MEM_LEN                                (32)
#define PP_QOS_WRED_QOS_INFO_MEM_MSK                                (0xFFFFFFFF)
#define PP_QOS_WRED_QOS_INFO_MEM_RST                                (0x0)
/**
 * REG_IDX_ACCESS   : PP_QOS_WRED_QOS_INFO_MEM_REG_IDX
 * NUM OF REGISTERS : 14
 */
#define PP_QOS_WRED_QOS_INFO_MEM_REG_IDX(idx) \
	(PP_QOS_WRED_QOS_INFO_MEM_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_QOS_WRED_QOS_INFO_MEM_CMD_REG
 * HW_REG_NAME : qos_info_mem_cmd
 * DESCRIPTION : Upon writing to this register the info of the
 *               written queue will be loaded to info_mem_regs,
 *               NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31:31][RO] - MISSING_DESCRIPTION
 *   [30: 9][RO] - Reserved
 *   [ 8: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_QOS_INFO_MEM_CMD_REG          ((QOS_WRED_BASE_ADDR) + 0x3BC)
#define PP_QOS_WRED_QOS_INFO_MEM_CMD_DATA_RDY_OFF                   (31)
#define PP_QOS_WRED_QOS_INFO_MEM_CMD_DATA_RDY_LEN                   (1)
#define PP_QOS_WRED_QOS_INFO_MEM_CMD_DATA_RDY_MSK                   (0x80000000)
#define PP_QOS_WRED_QOS_INFO_MEM_CMD_DATA_RDY_RST                   (0x0)
#define PP_QOS_WRED_QOS_INFO_MEM_CMD_RSVD0_OFF                      (9)
#define PP_QOS_WRED_QOS_INFO_MEM_CMD_RSVD0_LEN                      (22)
#define PP_QOS_WRED_QOS_INFO_MEM_CMD_RSVD0_MSK                      (0x7FFFFE00)
#define PP_QOS_WRED_QOS_INFO_MEM_CMD_RSVD0_RST                      (0x0)
#define PP_QOS_WRED_QOS_INFO_MEM_CMD_Q_OFF                          (0)
#define PP_QOS_WRED_QOS_INFO_MEM_CMD_Q_LEN                          (9)
#define PP_QOS_WRED_QOS_INFO_MEM_CMD_Q_MSK                          (0x000001FF)
#define PP_QOS_WRED_QOS_INFO_MEM_CMD_Q_RST                          (0x0)

/**
 * SW_REG_NAME : PP_QOS_WRED_AQM_BUFF_SIZE_DEV3_REG
 * HW_REG_NAME : qos_wred_aqm_buff_size_dev3
 * DESCRIPTION : buffer size for aqm algorithm, devided by 3
 *               because that's what the algorithm uses.
 *
 *  Register Fields :
 *   [31: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_AQM_BUFF_SIZE_DEV3_REG        ((QOS_WRED_BASE_ADDR) + 0x3C0)
#define PP_QOS_WRED_AQM_BUFF_SIZE_DEV3_OFF                          (0)
#define PP_QOS_WRED_AQM_BUFF_SIZE_DEV3_LEN                          (32)
#define PP_QOS_WRED_AQM_BUFF_SIZE_DEV3_MSK                          (0xFFFFFFFF)
#define PP_QOS_WRED_AQM_BUFF_SIZE_DEV3_RST                          (0x0)
/**
 * REG_IDX_ACCESS   : PP_QOS_WRED_AQM_BUFF_SIZE_DEV3_REG_IDX
 * NUM OF REGISTERS : 16
 */
#define PP_QOS_WRED_AQM_BUFF_SIZE_DEV3_REG_IDX(idx) \
	(PP_QOS_WRED_AQM_BUFF_SIZE_DEV3_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_QOS_WRED_DROP_CNT_0_REG
 * HW_REG_NAME : qos_wred_drop_counter_0
 * DESCRIPTION : WRED generic counter, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_DROP_CNT_0_REG                ((QOS_WRED_BASE_ADDR) + 0x400)
#define PP_QOS_WRED_DROP_CNT_0_CNTR_OFF                             (0)
#define PP_QOS_WRED_DROP_CNT_0_CNTR_LEN                             (32)
#define PP_QOS_WRED_DROP_CNT_0_CNTR_MSK                             (0xFFFFFFFF)
#define PP_QOS_WRED_DROP_CNT_0_CNTR_RST                             (0x0)
/**
 * REG_IDX_ACCESS   : PP_QOS_WRED_DROP_CNT_0_REG_IDX
 * NUM OF REGISTERS : 8
 */
#define PP_QOS_WRED_DROP_CNT_0_REG_IDX(idx) \
	(PP_QOS_WRED_DROP_CNT_0_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_QOS_WRED_Q_DROP_CNT_0_REG
 * HW_REG_NAME : qos_wred_queue_drop_counter_0
 * DESCRIPTION : WRED queue-related counter, NOTE, B Step ONLY
 *               register
 *
 *  Register Fields :
 *   [31:10][RO] - Reserved
 *   [ 9: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_Q_DROP_CNT_0_REG              ((QOS_WRED_BASE_ADDR) + 0x43C)
#define PP_QOS_WRED_Q_DROP_CNT_0_RSVD0_OFF                          (10)
#define PP_QOS_WRED_Q_DROP_CNT_0_RSVD0_LEN                          (22)
#define PP_QOS_WRED_Q_DROP_CNT_0_RSVD0_MSK                          (0xFFFFFC00)
#define PP_QOS_WRED_Q_DROP_CNT_0_RSVD0_RST                          (0x0)
#define PP_QOS_WRED_Q_DROP_CNT_0_OFF                                (0)
#define PP_QOS_WRED_Q_DROP_CNT_0_LEN                                (10)
#define PP_QOS_WRED_Q_DROP_CNT_0_MSK                                (0x000003FF)
#define PP_QOS_WRED_Q_DROP_CNT_0_RST                                (0x3ff)

/**
 * SW_REG_NAME : PP_QOS_WRED_DROP_CNT_1_REG
 * HW_REG_NAME : qos_wred_drop_counter_1
 * DESCRIPTION : WRED generic counter, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_DROP_CNT_1_REG                ((QOS_WRED_BASE_ADDR) + 0x440)
#define PP_QOS_WRED_DROP_CNT_1_CNTR_OFF                             (0)
#define PP_QOS_WRED_DROP_CNT_1_CNTR_LEN                             (32)
#define PP_QOS_WRED_DROP_CNT_1_CNTR_MSK                             (0xFFFFFFFF)
#define PP_QOS_WRED_DROP_CNT_1_CNTR_RST                             (0x0)
/**
 * REG_IDX_ACCESS   : PP_QOS_WRED_DROP_CNT_1_REG_IDX
 * NUM OF REGISTERS : 8
 */
#define PP_QOS_WRED_DROP_CNT_1_REG_IDX(idx) \
	(PP_QOS_WRED_DROP_CNT_1_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_QOS_WRED_Q_DROP_CNT_1_REG
 * HW_REG_NAME : qos_wred_queue_drop_counter_1
 * DESCRIPTION : WRED queue-related counter, NOTE, B Step ONLY
 *               register
 *
 *  Register Fields :
 *   [31:10][RO] - Reserved
 *   [ 9: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_Q_DROP_CNT_1_REG              ((QOS_WRED_BASE_ADDR) + 0x4FC)
#define PP_QOS_WRED_Q_DROP_CNT_1_RSVD0_OFF                          (10)
#define PP_QOS_WRED_Q_DROP_CNT_1_RSVD0_LEN                          (22)
#define PP_QOS_WRED_Q_DROP_CNT_1_RSVD0_MSK                          (0xFFFFFC00)
#define PP_QOS_WRED_Q_DROP_CNT_1_RSVD0_RST                          (0x0)
#define PP_QOS_WRED_Q_DROP_CNT_1_OFF                                (0)
#define PP_QOS_WRED_Q_DROP_CNT_1_LEN                                (10)
#define PP_QOS_WRED_Q_DROP_CNT_1_MSK                                (0x000003FF)
#define PP_QOS_WRED_Q_DROP_CNT_1_RST                                (0x3ff)

/**
 * SW_REG_NAME : PP_QOS_WRED_PORT_YELLOW_BYTES_REG
 * HW_REG_NAME : qos_wred_port_yellow_bytes
 * DESCRIPTION : per port counts yellow bytes., NOTE, B Step ONLY
 *               register
 *
 *  Register Fields :
 *   [31: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_PORT_YELLOW_BYTES_REG        ((QOS_WRED_BASE_ADDR) + 0x1000)
#define PP_QOS_WRED_PORT_YELLOW_BYTES_YELLOW_BYTES_CNT_OFF          (0)
#define PP_QOS_WRED_PORT_YELLOW_BYTES_YELLOW_BYTES_CNT_LEN          (32)
#define PP_QOS_WRED_PORT_YELLOW_BYTES_YELLOW_BYTES_CNT_MSK          (0xFFFFFFFF)
#define PP_QOS_WRED_PORT_YELLOW_BYTES_YELLOW_BYTES_CNT_RST          (0x0)
/**
 * REG_IDX_ACCESS   : PP_QOS_WRED_PORT_YELLOW_BYTES_REG_IDX
 * NUM OF REGISTERS : 256
 */
#define PP_QOS_WRED_PORT_YELLOW_BYTES_REG_IDX(idx) \
	(PP_QOS_WRED_PORT_YELLOW_BYTES_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_QOS_WRED_PORT_GREEN_BYTES_REG
 * HW_REG_NAME : qos_wred_port_green_bytes
 * DESCRIPTION : per port counts green bytes., NOTE, B Step ONLY
 *               register
 *
 *  Register Fields :
 *   [31: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_PORT_GREEN_BYTES_REG         ((QOS_WRED_BASE_ADDR) + 0x1400)
#define PP_QOS_WRED_PORT_GREEN_BYTES_GREEN_BYTES_CNT_OFF            (0)
#define PP_QOS_WRED_PORT_GREEN_BYTES_GREEN_BYTES_CNT_LEN            (32)
#define PP_QOS_WRED_PORT_GREEN_BYTES_GREEN_BYTES_CNT_MSK            (0xFFFFFFFF)
#define PP_QOS_WRED_PORT_GREEN_BYTES_GREEN_BYTES_CNT_RST            (0x0)
/**
 * REG_IDX_ACCESS   : PP_QOS_WRED_PORT_GREEN_BYTES_REG_IDX
 * NUM OF REGISTERS : 256
 */
#define PP_QOS_WRED_PORT_GREEN_BYTES_REG_IDX(idx) \
	(PP_QOS_WRED_PORT_GREEN_BYTES_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_QOS_WRED_PORT_YELLOW_THRESH_REG
 * HW_REG_NAME : qos_wred_port_yellow_thresh
 * DESCRIPTION : per port defines yellow threshold for output
 *               indication bit., NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_PORT_YELLOW_THRESH_REG       ((QOS_WRED_BASE_ADDR) + 0x2000)
#define PP_QOS_WRED_PORT_YELLOW_THRESH_OFF                          (0)
#define PP_QOS_WRED_PORT_YELLOW_THRESH_LEN                          (32)
#define PP_QOS_WRED_PORT_YELLOW_THRESH_MSK                          (0xFFFFFFFF)
#define PP_QOS_WRED_PORT_YELLOW_THRESH_RST                          (0x0)
/**
 * REG_IDX_ACCESS   : PP_QOS_WRED_PORT_YELLOW_THRESH_REG_IDX
 * NUM OF REGISTERS : 256
 */
#define PP_QOS_WRED_PORT_YELLOW_THRESH_REG_IDX(idx) \
	(PP_QOS_WRED_PORT_YELLOW_THRESH_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_QOS_WRED_PORT_GREEN_THRESH_REG
 * HW_REG_NAME : qos_wred_port_green_thresh
 * DESCRIPTION : per port defines green threshold for output
 *               indication bit., NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_PORT_GREEN_THRESH_REG        ((QOS_WRED_BASE_ADDR) + 0x2400)
#define PP_QOS_WRED_PORT_GREEN_THRESH_OFF                           (0)
#define PP_QOS_WRED_PORT_GREEN_THRESH_LEN                           (32)
#define PP_QOS_WRED_PORT_GREEN_THRESH_MSK                           (0xFFFFFFFF)
#define PP_QOS_WRED_PORT_GREEN_THRESH_RST                           (0x0)
/**
 * REG_IDX_ACCESS   : PP_QOS_WRED_PORT_GREEN_THRESH_REG_IDX
 * NUM OF REGISTERS : 256
 */
#define PP_QOS_WRED_PORT_GREEN_THRESH_REG_IDX(idx) \
	(PP_QOS_WRED_PORT_GREEN_THRESH_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_QOS_WRED_Q_TO_PORT_LOOKUP_REG
 * HW_REG_NAME : qos_wred_q_to_port_lookup
 * DESCRIPTION : matches queue to its port., NOTE, B Step ONLY
 *               register
 *
 *  Register Fields :
 *   [31: 7][RO] - Reserved
 *   [ 6: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_Q_TO_PORT_LOOKUP_REG         ((QOS_WRED_BASE_ADDR) + 0x3000)
#define PP_QOS_WRED_Q_TO_PORT_LOOKUP_RSVD0_OFF                      (7)
#define PP_QOS_WRED_Q_TO_PORT_LOOKUP_RSVD0_LEN                      (25)
#define PP_QOS_WRED_Q_TO_PORT_LOOKUP_RSVD0_MSK                      (0xFFFFFF80)
#define PP_QOS_WRED_Q_TO_PORT_LOOKUP_RSVD0_RST                      (0x0)
#define PP_QOS_WRED_Q_TO_PORT_LOOKUP_OFF                            (0)
#define PP_QOS_WRED_Q_TO_PORT_LOOKUP_LEN                            (7)
#define PP_QOS_WRED_Q_TO_PORT_LOOKUP_MSK                            (0x0000007F)
#define PP_QOS_WRED_Q_TO_PORT_LOOKUP_RST                            (0x0)
/**
 * REG_IDX_ACCESS   : PP_QOS_WRED_Q_TO_PORT_LOOKUP_REG_IDX
 * NUM OF REGISTERS : 512
 */
#define PP_QOS_WRED_Q_TO_PORT_LOOKUP_REG_IDX(idx) \
	(PP_QOS_WRED_Q_TO_PORT_LOOKUP_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_QOS_WRED_Q_SIZE_BYTES_DIRECT_REG
 * HW_REG_NAME : qos_wred_q_size_bytes_direct
 * DESCRIPTION : direct read of q size bytes counter., NOTE, B Step
 *               ONLY register
 *
 *  Register Fields :
 *   [31: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_Q_SIZE_BYTES_DIRECT_REG      ((QOS_WRED_BASE_ADDR) + 0x4000)
#define PP_QOS_WRED_Q_SIZE_BYTES_DIRECT_OFF                         (0)
#define PP_QOS_WRED_Q_SIZE_BYTES_DIRECT_LEN                         (32)
#define PP_QOS_WRED_Q_SIZE_BYTES_DIRECT_MSK                         (0xFFFFFFFF)
#define PP_QOS_WRED_Q_SIZE_BYTES_DIRECT_RST                         (0x0)
/**
 * REG_IDX_ACCESS   : PP_QOS_WRED_Q_SIZE_BYTES_DIRECT_REG_IDX
 * NUM OF REGISTERS : 512
 */
#define PP_QOS_WRED_Q_SIZE_BYTES_DIRECT_REG_IDX(idx) \
	(PP_QOS_WRED_Q_SIZE_BYTES_DIRECT_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_QOS_WRED_AQM_Q_TO_CONTEXT_REG
 * HW_REG_NAME : qos_wred_aqm_q_to_context
 * DESCRIPTION : matches queue to its aqm context. bit 4 is for
 *               disable (1-disable aqm for this q). bits 3:0 is
 *               for context - 0-15., NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 5][RO] - Reserved
 *   [ 4: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_QOS_WRED_AQM_Q_TO_CONTEXT_REG         ((QOS_WRED_BASE_ADDR) + 0x5000)
#define PP_QOS_WRED_AQM_Q_TO_CONTEXT_RSVD0_OFF                      (5)
#define PP_QOS_WRED_AQM_Q_TO_CONTEXT_RSVD0_LEN                      (27)
#define PP_QOS_WRED_AQM_Q_TO_CONTEXT_RSVD0_MSK                      (0xFFFFFFE0)
#define PP_QOS_WRED_AQM_Q_TO_CONTEXT_RSVD0_RST                      (0x0)
#define PP_QOS_WRED_AQM_Q_TO_CONTEXT_OFF                            (0)
#define PP_QOS_WRED_AQM_Q_TO_CONTEXT_LEN                            (5)
#define PP_QOS_WRED_AQM_Q_TO_CONTEXT_MSK                            (0x0000001F)
#define PP_QOS_WRED_AQM_Q_TO_CONTEXT_RST                            (0x0)
/**
 * REG_IDX_ACCESS   : PP_QOS_WRED_AQM_Q_TO_CONTEXT_REG_IDX
 * NUM OF REGISTERS : 512
 */
#define PP_QOS_WRED_AQM_Q_TO_CONTEXT_REG_IDX(idx) \
	(PP_QOS_WRED_AQM_Q_TO_CONTEXT_REG + ((idx) << 2))

#endif
