/**
 * counter_cache_regs.h
 * Description: counter_cache_s HW registers definitions
 * Auto-generated file by RDL Parser, DO NOT CHANGE
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2019-2020 Intel Corporation
 */

#ifndef _PP_CNTR_CACHE_H_
#define _PP_CNTR_CACHE_H_

#define PP_CNTR_CACHE_GEN_DATE_STR            "Sunday May 17, 2020 [5:26:11 pm]"
#define PP_CNTR_CACHE_BASE                                       (0xF1E00000ULL)

/**
 * SW_REG_NAME : PP_CNTR_CACHE_VER_REG
 * HW_REG_NAME : cache_ver_reg
 * DESCRIPTION : cache_ver
 *
 *  Register Fields :
 *   [31: 0][RO] - cache_ver
 *
 */
#define PP_CNTR_CACHE_VER_REG                     ((CNTR_CACHE_BASE_ADDR) + 0x0)
#define PP_CNTR_CACHE_VER_CACHE_VER_I_OFF                           (0)
#define PP_CNTR_CACHE_VER_CACHE_VER_I_LEN                           (32)
#define PP_CNTR_CACHE_VER_CACHE_VER_I_MSK                           (0xFFFFFFFF)
#define PP_CNTR_CACHE_VER_CACHE_VER_I_RST                           (0x1)

/**
 * SW_REG_NAME : PP_CNTR_CACHE_BYPASS_REG
 * HW_REG_NAME : cache_bypass_reg
 * DESCRIPTION : cache_bypass
 *
 *  Register Fields :
 *   [31: 1][RO] - Reserved
 *   [ 0: 0][RW] - cache_bypass- bypass cache operation (all
 *                 transaction go throuth the cache pipe)
 *
 */
#define PP_CNTR_CACHE_BYPASS_REG                  ((CNTR_CACHE_BASE_ADDR) + 0x4)
#define PP_CNTR_CACHE_BYPASS_RSVD0_OFF                              (1)
#define PP_CNTR_CACHE_BYPASS_RSVD0_LEN                              (31)
#define PP_CNTR_CACHE_BYPASS_RSVD0_MSK                              (0xFFFFFFFE)
#define PP_CNTR_CACHE_BYPASS_RSVD0_RST                              (0x0)
#define PP_CNTR_CACHE_BYPASS_CACHE_BYPASS_I_OFF                     (0)
#define PP_CNTR_CACHE_BYPASS_CACHE_BYPASS_I_LEN                     (1)
#define PP_CNTR_CACHE_BYPASS_CACHE_BYPASS_I_MSK                     (0x00000001)
#define PP_CNTR_CACHE_BYPASS_CACHE_BYPASS_I_RST                     (0x1)

/**
 * SW_REG_NAME : PP_CNTR_CACHE_POLICY_REG
 * HW_REG_NAME : cache_policy_reg
 * DESCRIPTION : cache_policy
 *
 *  Register Fields :
 *   [31:17][RO] - Reserved
 *   [16:16][RW] - write_policy; 1 write through policy ; 0- write
 *                 back policy
 *   [15: 5][RO] - Reserved
 *   [ 4: 4][RW] - if replacement policy selected then random_policy
 *                 can be define as ; 1 - random update each request
 *                 ; 0- random update every cycle
 *   [ 3: 1][RO] - Reserved
 *   [ 0: 0][RW] - replacement_policy; 0- random replacement policy ;
 *                 1- LRU policy
 *
 */
#define PP_CNTR_CACHE_POLICY_REG                  ((CNTR_CACHE_BASE_ADDR) + 0x8)
#define PP_CNTR_CACHE_POLICY_RSVD0_OFF                              (17)
#define PP_CNTR_CACHE_POLICY_RSVD0_LEN                              (15)
#define PP_CNTR_CACHE_POLICY_RSVD0_MSK                              (0xFFFE0000)
#define PP_CNTR_CACHE_POLICY_RSVD0_RST                              (0x0)
#define PP_CNTR_CACHE_POLICY_WRITE_POLICY_OFF                       (16)
#define PP_CNTR_CACHE_POLICY_WRITE_POLICY_LEN                       (1)
#define PP_CNTR_CACHE_POLICY_WRITE_POLICY_MSK                       (0x00010000)
#define PP_CNTR_CACHE_POLICY_WRITE_POLICY_RST                       (0x0)
#define PP_CNTR_CACHE_POLICY_RSVD1_OFF                              (5)
#define PP_CNTR_CACHE_POLICY_RSVD1_LEN                              (11)
#define PP_CNTR_CACHE_POLICY_RSVD1_MSK                              (0x0000FFE0)
#define PP_CNTR_CACHE_POLICY_RSVD1_RST                              (0x0)
#define PP_CNTR_CACHE_POLICY_RANDOM_POLICY_OFF                      (4)
#define PP_CNTR_CACHE_POLICY_RANDOM_POLICY_LEN                      (1)
#define PP_CNTR_CACHE_POLICY_RANDOM_POLICY_MSK                      (0x00000010)
#define PP_CNTR_CACHE_POLICY_RANDOM_POLICY_RST                      (0x0)
#define PP_CNTR_CACHE_POLICY_RSVD2_OFF                              (1)
#define PP_CNTR_CACHE_POLICY_RSVD2_LEN                              (3)
#define PP_CNTR_CACHE_POLICY_RSVD2_MSK                              (0x0000000E)
#define PP_CNTR_CACHE_POLICY_RSVD2_RST                              (0x0)
#define PP_CNTR_CACHE_POLICY_REPLACEMENT_POLICY_OFF                 (0)
#define PP_CNTR_CACHE_POLICY_REPLACEMENT_POLICY_LEN                 (1)
#define PP_CNTR_CACHE_POLICY_REPLACEMENT_POLICY_MSK                 (0x00000001)
#define PP_CNTR_CACHE_POLICY_REPLACEMENT_POLICY_RST                 (0x0)

/**
 * SW_REG_NAME : PP_CNTR_CACHE_OP_REG
 * HW_REG_NAME : cache_operation_reg
 * DESCRIPTION : cache_operation
 *
 *  Register Fields :
 *   [31:25][RO] - Reserved
 *   [24:24][RW] - hold transaction while invalid or evict all cache
 *   [23:21][RO] - Reserved
 *   [20:20][W1] - status of invalidate all cache
 *   [19:17][RO] - Reserved
 *   [16:16][RW] - invalidate all cache
 *   [15: 5][RO] - Reserved
 *   [ 4: 4][W1] - status of evict all cache
 *   [ 3: 1][RO] - Reserved
 *   [ 0: 0][RW] - Evict all cache
 *
 */
#define PP_CNTR_CACHE_OP_REG                     ((CNTR_CACHE_BASE_ADDR) + 0x0C)
#define PP_CNTR_CACHE_OP_RSVD0_OFF                                  (25)
#define PP_CNTR_CACHE_OP_RSVD0_LEN                                  (7)
#define PP_CNTR_CACHE_OP_RSVD0_MSK                                  (0xFE000000)
#define PP_CNTR_CACHE_OP_RSVD0_RST                                  (0x0)
#define PP_CNTR_CACHE_OP_CACHE_HOLD_OFF                             (24)
#define PP_CNTR_CACHE_OP_CACHE_HOLD_LEN                             (1)
#define PP_CNTR_CACHE_OP_CACHE_HOLD_MSK                             (0x01000000)
#define PP_CNTR_CACHE_OP_CACHE_HOLD_RST                             (0x0)
#define PP_CNTR_CACHE_OP_RSVD1_OFF                                  (21)
#define PP_CNTR_CACHE_OP_RSVD1_LEN                                  (3)
#define PP_CNTR_CACHE_OP_RSVD1_MSK                                  (0x00E00000)
#define PP_CNTR_CACHE_OP_RSVD1_RST                                  (0x0)
#define PP_CNTR_CACHE_OP_CACHE_INVALIDATE_STATUS_OFF                (20)
#define PP_CNTR_CACHE_OP_CACHE_INVALIDATE_STATUS_LEN                (1)
#define PP_CNTR_CACHE_OP_CACHE_INVALIDATE_STATUS_MSK                (0x00100000)
#define PP_CNTR_CACHE_OP_CACHE_INVALIDATE_STATUS_RST                (0x0)
#define PP_CNTR_CACHE_OP_RSVD2_OFF                                  (17)
#define PP_CNTR_CACHE_OP_RSVD2_LEN                                  (3)
#define PP_CNTR_CACHE_OP_RSVD2_MSK                                  (0x000E0000)
#define PP_CNTR_CACHE_OP_RSVD2_RST                                  (0x0)
#define PP_CNTR_CACHE_OP_CACHE_INVALIDATE_OFF                       (16)
#define PP_CNTR_CACHE_OP_CACHE_INVALIDATE_LEN                       (1)
#define PP_CNTR_CACHE_OP_CACHE_INVALIDATE_MSK                       (0x00010000)
#define PP_CNTR_CACHE_OP_CACHE_INVALIDATE_RST                       (0x1)
#define PP_CNTR_CACHE_OP_RSVD3_OFF                                  (5)
#define PP_CNTR_CACHE_OP_RSVD3_LEN                                  (11)
#define PP_CNTR_CACHE_OP_RSVD3_MSK                                  (0x0000FFE0)
#define PP_CNTR_CACHE_OP_RSVD3_RST                                  (0x0)
#define PP_CNTR_CACHE_OP_CACHE_EVICT_STATUS_OFF                     (4)
#define PP_CNTR_CACHE_OP_CACHE_EVICT_STATUS_LEN                     (1)
#define PP_CNTR_CACHE_OP_CACHE_EVICT_STATUS_MSK                     (0x00000010)
#define PP_CNTR_CACHE_OP_CACHE_EVICT_STATUS_RST                     (0x0)
#define PP_CNTR_CACHE_OP_RSVD4_OFF                                  (1)
#define PP_CNTR_CACHE_OP_RSVD4_LEN                                  (3)
#define PP_CNTR_CACHE_OP_RSVD4_MSK                                  (0x0000000E)
#define PP_CNTR_CACHE_OP_RSVD4_RST                                  (0x0)
#define PP_CNTR_CACHE_OP_CACHE_EVICT_OFF                            (0)
#define PP_CNTR_CACHE_OP_CACHE_EVICT_LEN                            (1)
#define PP_CNTR_CACHE_OP_CACHE_EVICT_MSK                            (0x00000001)
#define PP_CNTR_CACHE_OP_CACHE_EVICT_RST                            (0x0)

/**
 * SW_REG_NAME : PP_CNTR_CACHE_EVICT_ADDR_REG
 * HW_REG_NAME : cache_evict_address_reg
 * DESCRIPTION : cache_evict_address
 *
 *  Register Fields :
 *   [31: 0][RW] - single operation cache line evict address; evict
 *                 this address while detect write to this register;
 *                 please cache if the cache_single_operation_status
 *                 bit is not in progress (zero)
 *
 */
#define PP_CNTR_CACHE_EVICT_ADDR_REG             ((CNTR_CACHE_BASE_ADDR) + 0x10)
#define PP_CNTR_CACHE_EVICT_ADDR_OFF                                (0)
#define PP_CNTR_CACHE_EVICT_ADDR_LEN                                (32)
#define PP_CNTR_CACHE_EVICT_ADDR_MSK                                (0xFFFFFFFF)
#define PP_CNTR_CACHE_EVICT_ADDR_RST                                (0x0)

/**
 * SW_REG_NAME : PP_CNTR_CACHE_INVALID_ADDR_REG
 * HW_REG_NAME : cache_invalid_address_reg
 * DESCRIPTION : cache_invalid_address
 *
 *  Register Fields :
 *   [31: 0][RW] - single operation cache line invalid address;
 *                 invalid this address while detect write to this
 *                 register;  please cache if the
 *                 cache_single_operation_status bit is not in
 *                 progress(zero)
 *
 */
#define PP_CNTR_CACHE_INVALID_ADDR_REG           ((CNTR_CACHE_BASE_ADDR) + 0x14)
#define PP_CNTR_CACHE_INVALID_ADDR_INVALIDATE_ADDR_OFF              (0)
#define PP_CNTR_CACHE_INVALID_ADDR_INVALIDATE_ADDR_LEN              (32)
#define PP_CNTR_CACHE_INVALID_ADDR_INVALIDATE_ADDR_MSK              (0xFFFFFFFF)
#define PP_CNTR_CACHE_INVALID_ADDR_INVALIDATE_ADDR_RST              (0x0)

/**
 * SW_REG_NAME : PP_CNTR_CACHE_SINGLE_OP_STATUS_REG
 * HW_REG_NAME : cache_single_operation_status_reg
 * DESCRIPTION : cache_single_operation_status
 *
 *  Register Fields :
 *   [31: 1][RO] - Reserved
 *   [ 0: 0][RO] - Single operation ready to recive; if 1 then the
 *                 transaction in-progress. check the status to be
 *                 zero before write to single operation request
 *                 (evict/invalid)
 *
 */
#define PP_CNTR_CACHE_SINGLE_OP_STATUS_REG       ((CNTR_CACHE_BASE_ADDR) + 0x18)
#define PP_CNTR_CACHE_SINGLE_OP_STATUS_RSVD0_OFF                    (1)
#define PP_CNTR_CACHE_SINGLE_OP_STATUS_RSVD0_LEN                    (31)
#define PP_CNTR_CACHE_SINGLE_OP_STATUS_RSVD0_MSK                    (0xFFFFFFFE)
#define PP_CNTR_CACHE_SINGLE_OP_STATUS_RSVD0_RST                    (0x0)
#define PP_CNTR_CACHE_SINGLE_OP_STATUS_STATUS_SINGLE_OP_OFF         (0)
#define PP_CNTR_CACHE_SINGLE_OP_STATUS_STATUS_SINGLE_OP_LEN         (1)
#define PP_CNTR_CACHE_SINGLE_OP_STATUS_STATUS_SINGLE_OP_MSK         (0x00000001)
#define PP_CNTR_CACHE_SINGLE_OP_STATUS_STATUS_SINGLE_OP_RST         (0x0)

/**
 * SW_REG_NAME : PP_CNTR_CACHE_READ_TRANS_CNT_H_REG
 * HW_REG_NAME : read_transaction_counter_h_reg
 * DESCRIPTION : read_transaction_counter_h
 *
 *  Register Fields :
 *   [31: 0][RO] - read transaction counter [MSB bits] ; read this
 *                 register release the hold and update the counter
 *
 */
#define PP_CNTR_CACHE_READ_TRANS_CNT_H_REG       ((CNTR_CACHE_BASE_ADDR) + 0x1C)
#define PP_CNTR_CACHE_READ_TRANS_CNT_H_READ_TRANS_CNT_H_I_OFF       (0)
#define PP_CNTR_CACHE_READ_TRANS_CNT_H_READ_TRANS_CNT_H_I_LEN       (32)
#define PP_CNTR_CACHE_READ_TRANS_CNT_H_READ_TRANS_CNT_H_I_MSK       (0xFFFFFFFF)
#define PP_CNTR_CACHE_READ_TRANS_CNT_H_READ_TRANS_CNT_H_I_RST       (0x0)

/**
 * SW_REG_NAME : PP_CNTR_CACHE_READ_TRANS_CNT_L_REG
 * HW_REG_NAME : read_transaction_counter_l_reg
 * DESCRIPTION : read_transaction_counter_l
 *
 *  Register Fields :
 *   [31: 0][RO] - read_transaction counter [LSB bits] ; read this
 *                 register hold the data at the MSB bits
 *
 */
#define PP_CNTR_CACHE_READ_TRANS_CNT_L_REG       ((CNTR_CACHE_BASE_ADDR) + 0x20)
#define PP_CNTR_CACHE_READ_TRANS_CNT_L_READ_TRANS_CNT_L_I_OFF       (0)
#define PP_CNTR_CACHE_READ_TRANS_CNT_L_READ_TRANS_CNT_L_I_LEN       (32)
#define PP_CNTR_CACHE_READ_TRANS_CNT_L_READ_TRANS_CNT_L_I_MSK       (0xFFFFFFFF)
#define PP_CNTR_CACHE_READ_TRANS_CNT_L_READ_TRANS_CNT_L_I_RST       (0x0)

/**
 * SW_REG_NAME : PP_CNTR_CACHE_READ_HIT_TRANS_CNT_H_REG
 * HW_REG_NAME : read_hit_transaction_counter_h_reg
 * DESCRIPTION : read_hit_transaction_counter_h
 *
 *  Register Fields :
 *   [31: 0][RO] - hit read  transaction counter [MSB bits] ; read
 *                 this register release the hold and update the
 *                 counter
 *
 */
#define PP_CNTR_CACHE_READ_HIT_TRANS_CNT_H_REG   ((CNTR_CACHE_BASE_ADDR) + 0x24)
#define PP_CNTR_CACHE_READ_HIT_TRANS_CNT_H_READ_HIT_TRANS_CNT_H_I_OFF        (0)
#define PP_CNTR_CACHE_READ_HIT_TRANS_CNT_H_READ_HIT_TRANS_CNT_H_I_LEN       (32)
#define PP_CNTR_CACHE_READ_HIT_TRANS_CNT_H_READ_HIT_TRANS_CNT_H_I_MSK \
	(0xFFFFFFFF)
#define PP_CNTR_CACHE_READ_HIT_TRANS_CNT_H_READ_HIT_TRANS_CNT_H_I_RST      (0x0)

/**
 * SW_REG_NAME : PP_CNTR_CACHE_READ_HIT_TRANS_CNT_L_REG
 * HW_REG_NAME : read_hit_transaction_counter_l_reg
 * DESCRIPTION : read_hit_transaction_counter_l
 *
 *  Register Fields :
 *   [31: 0][RO] - hit read transaction counter [LSB bits] ; read
 *                 this register hold the data at the MSB bits
 *
 */
#define PP_CNTR_CACHE_READ_HIT_TRANS_CNT_L_REG   ((CNTR_CACHE_BASE_ADDR) + 0x28)
#define PP_CNTR_CACHE_READ_HIT_TRANS_CNT_L_READ_HIT_TRANS_CNT_L_I_OFF        (0)
#define PP_CNTR_CACHE_READ_HIT_TRANS_CNT_L_READ_HIT_TRANS_CNT_L_I_LEN       (32)
#define PP_CNTR_CACHE_READ_HIT_TRANS_CNT_L_READ_HIT_TRANS_CNT_L_I_MSK \
	(0xFFFFFFFF)
#define PP_CNTR_CACHE_READ_HIT_TRANS_CNT_L_READ_HIT_TRANS_CNT_L_I_RST      (0x0)

/**
 * SW_REG_NAME : PP_CNTR_CACHE_BYPASS_WIN_VALID_REG
 * HW_REG_NAME : bypass_win_valid_reg
 * DESCRIPTION : bypass_win_valid
 *
 *  Register Fields :
 *   [31: 8][RO] - Reserved
 *   [ 7: 0][RW] - bypass windows valid; each bit represent windows
 *
 */
#define PP_CNTR_CACHE_BYPASS_WIN_VALID_REG       ((CNTR_CACHE_BASE_ADDR) + 0x2C)
#define PP_CNTR_CACHE_BYPASS_WIN_VALID_RSVD0_OFF                    (8)
#define PP_CNTR_CACHE_BYPASS_WIN_VALID_RSVD0_LEN                    (24)
#define PP_CNTR_CACHE_BYPASS_WIN_VALID_RSVD0_MSK                    (0xFFFFFF00)
#define PP_CNTR_CACHE_BYPASS_WIN_VALID_RSVD0_RST                    (0x0)
#define PP_CNTR_CACHE_BYPASS_WIN_VALID_BYPASS_WIN_VALID_I_OFF       (0)
#define PP_CNTR_CACHE_BYPASS_WIN_VALID_BYPASS_WIN_VALID_I_LEN       (8)
#define PP_CNTR_CACHE_BYPASS_WIN_VALID_BYPASS_WIN_VALID_I_MSK       (0x000000FF)
#define PP_CNTR_CACHE_BYPASS_WIN_VALID_BYPASS_WIN_VALID_I_RST       (0x0)

/**
 * SW_REG_NAME : PP_CNTR_CACHE_BYPASS_WIN_LOW_REG
 * HW_REG_NAME : bypass_win_low_reg
 * DESCRIPTION : bypass_win_low
 *
 *  Register Fields :
 *   [31: 0][RW] - low address for configure bypass windows ; address
 *                 shift left by ADDR_W - CSR_ADDR_W
 *
 */
#define PP_CNTR_CACHE_BYPASS_WIN_LOW_REG         ((CNTR_CACHE_BASE_ADDR) + 0x30)
#define PP_CNTR_CACHE_BYPASS_WIN_LOW_BYPASS_WIN_LOW_I_OFF           (0)
#define PP_CNTR_CACHE_BYPASS_WIN_LOW_BYPASS_WIN_LOW_I_LEN           (32)
#define PP_CNTR_CACHE_BYPASS_WIN_LOW_BYPASS_WIN_LOW_I_MSK           (0xFFFFFFFF)
#define PP_CNTR_CACHE_BYPASS_WIN_LOW_BYPASS_WIN_LOW_I_RST           (0x0)
/**
 * REG_IDX_ACCESS   : PP_CNTR_CACHE_BYPASS_WIN_LOW_REG_IDX
 * NUM OF REGISTERS : 8
 */
#define PP_CNTR_CACHE_BYPASS_WIN_LOW_REG_IDX(idx) \
	(PP_CNTR_CACHE_BYPASS_WIN_LOW_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_CNTR_CACHE_BYPASS_WIN_HIGH_REG
 * HW_REG_NAME : bypass_win_high_reg
 * DESCRIPTION : bypass_win_high
 *
 *  Register Fields :
 *   [31: 0][RW] - high address for configure bypass windows ;
 *                 address shift left by ADDR_W - CSR_ADDR_W
 *
 */
#define PP_CNTR_CACHE_BYPASS_WIN_HIGH_REG        ((CNTR_CACHE_BASE_ADDR) + 0x50)
#define PP_CNTR_CACHE_BYPASS_WIN_HIGH_BYPASS_WIN_HIGH_I_OFF         (0)
#define PP_CNTR_CACHE_BYPASS_WIN_HIGH_BYPASS_WIN_HIGH_I_LEN         (32)
#define PP_CNTR_CACHE_BYPASS_WIN_HIGH_BYPASS_WIN_HIGH_I_MSK         (0xFFFFFFFF)
#define PP_CNTR_CACHE_BYPASS_WIN_HIGH_BYPASS_WIN_HIGH_I_RST         (0x0)
/**
 * REG_IDX_ACCESS   : PP_CNTR_CACHE_BYPASS_WIN_HIGH_REG_IDX
 * NUM OF REGISTERS : 8
 */
#define PP_CNTR_CACHE_BYPASS_WIN_HIGH_REG_IDX(idx) \
	(PP_CNTR_CACHE_BYPASS_WIN_HIGH_REG + ((idx) << 2))

#endif
