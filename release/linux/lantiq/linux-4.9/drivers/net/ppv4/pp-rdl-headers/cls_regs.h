/**
 * cls_regs.h
 * Description: cls_s HW registers definitions
 * Auto-generated file by RDL Parser, DO NOT CHANGE
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2019-2020 Intel Corporation
 */

#ifndef _PP_CLS_H_
#define _PP_CLS_H_

#define PP_CLS_GEN_DATE_STR                   "Sunday May 17, 2020 [5:26:11 pm]"
#define PP_CLS_BASE                                              (0xF1C00000ULL)

/**
 * SW_REG_NAME : PP_CLS_CLS_VER_REG
 * HW_REG_NAME : cls_ver
 * DESCRIPTION : cls version
 *
 *  Register Fields :
 *   [31: 4][RO] - Reserved
 *   [ 3: 0][RO] - cls ver
 *
 */
#define PP_CLS_CLS_VER_REG                               ((CLS_BASE_ADDR) + 0x0)
#define PP_CLS_CLS_VER_RSVD0_OFF                                    (4)
#define PP_CLS_CLS_VER_RSVD0_LEN                                    (28)
#define PP_CLS_CLS_VER_RSVD0_MSK                                    (0xFFFFFFF0)
#define PP_CLS_CLS_VER_RSVD0_RST                                    (0x0)
#define PP_CLS_CLS_VER_OFF                                          (0)
#define PP_CLS_CLS_VER_LEN                                          (4)
#define PP_CLS_CLS_VER_MSK                                          (0x0000000F)
#define PP_CLS_CLS_VER_RST                                          (0x1)

/**
 * SW_REG_NAME : PP_CLS_THREAD_CFG_REG
 * HW_REG_NAME : thread_config
 * DESCRIPTION : thread_config_reg
 *
 *  Register Fields :
 *   [31:31][RW] - dbg FV : [0] no mask ; [1] with mask (inclide mask
 *                 from FV size)
 *   [30:30][RW] - Disable L1 cache : [0] enable ; [1] disable
 *   [29:25][RO] - Reserved
 *   [24:24][RW] - Error indication disable
 *   [23:17][RO] - Reserved
 *   [16:16][RW] - Select drop disable. 0 - drop flow enable, 1- drop
 *                 flow disable
 *   [15: 9][RO] - Reserved
 *   [ 8: 8][RW] - Search mode ;  0 -) normal operation , search
 *                 while sig hit and non FM on the other (non hit)
 *                 sig table ; 1 -) search on sig hit amd not FM on
 *                 both tables
 *   [ 7: 1][RO] - Reserved
 *   [ 0: 0][RW] - Select update signature cache flow. 0 - after
 *                 signature matched, 1- after full matched
 *
 */
#define PP_CLS_THREAD_CFG_REG                            ((CLS_BASE_ADDR) + 0x4)
#define PP_CLS_THREAD_CFG_DBG_FV_SEL_OFF                            (31)
#define PP_CLS_THREAD_CFG_DBG_FV_SEL_LEN                            (1)
#define PP_CLS_THREAD_CFG_DBG_FV_SEL_MSK                            (0x80000000)
#define PP_CLS_THREAD_CFG_DBG_FV_SEL_RST                            (0x0)
#define PP_CLS_THREAD_CFG_L1_DIS_OFF                                (30)
#define PP_CLS_THREAD_CFG_L1_DIS_LEN                                (1)
#define PP_CLS_THREAD_CFG_L1_DIS_MSK                                (0x40000000)
#define PP_CLS_THREAD_CFG_L1_DIS_RST                                (0x0)
#define PP_CLS_THREAD_CFG_RSVD0_OFF                                 (25)
#define PP_CLS_THREAD_CFG_RSVD0_LEN                                 (5)
#define PP_CLS_THREAD_CFG_RSVD0_MSK                                 (0x3E000000)
#define PP_CLS_THREAD_CFG_RSVD0_RST                                 (0x0)
#define PP_CLS_THREAD_CFG_ERR_INDICATE_DIS_OFF                      (24)
#define PP_CLS_THREAD_CFG_ERR_INDICATE_DIS_LEN                      (1)
#define PP_CLS_THREAD_CFG_ERR_INDICATE_DIS_MSK                      (0x01000000)
#define PP_CLS_THREAD_CFG_ERR_INDICATE_DIS_RST                      (0x0)
#define PP_CLS_THREAD_CFG_RSVD1_OFF                                 (17)
#define PP_CLS_THREAD_CFG_RSVD1_LEN                                 (7)
#define PP_CLS_THREAD_CFG_RSVD1_MSK                                 (0x00FE0000)
#define PP_CLS_THREAD_CFG_RSVD1_RST                                 (0x0)
#define PP_CLS_THREAD_CFG_DROP_FLOW_OFF                             (16)
#define PP_CLS_THREAD_CFG_DROP_FLOW_LEN                             (1)
#define PP_CLS_THREAD_CFG_DROP_FLOW_MSK                             (0x00010000)
#define PP_CLS_THREAD_CFG_DROP_FLOW_RST                             (0x0)
#define PP_CLS_THREAD_CFG_RSVD2_OFF                                 (9)
#define PP_CLS_THREAD_CFG_RSVD2_LEN                                 (7)
#define PP_CLS_THREAD_CFG_RSVD2_MSK                                 (0x0000FE00)
#define PP_CLS_THREAD_CFG_RSVD2_RST                                 (0x0)
#define PP_CLS_THREAD_CFG_SEARCH_MODE_OFF                           (8)
#define PP_CLS_THREAD_CFG_SEARCH_MODE_LEN                           (1)
#define PP_CLS_THREAD_CFG_SEARCH_MODE_MSK                           (0x00000100)
#define PP_CLS_THREAD_CFG_SEARCH_MODE_RST                           (0x0)
#define PP_CLS_THREAD_CFG_RSVD3_OFF                                 (1)
#define PP_CLS_THREAD_CFG_RSVD3_LEN                                 (7)
#define PP_CLS_THREAD_CFG_RSVD3_MSK                                 (0x000000FE)
#define PP_CLS_THREAD_CFG_RSVD3_RST                                 (0x0)
#define PP_CLS_THREAD_CFG_FM_FLOW_OFF                               (0)
#define PP_CLS_THREAD_CFG_FM_FLOW_LEN                               (1)
#define PP_CLS_THREAD_CFG_FM_FLOW_MSK                               (0x00000001)
#define PP_CLS_THREAD_CFG_FM_FLOW_RST                               (0x0)

/**
 * SW_REG_NAME : PP_CLS_HASH_SEL_REG
 * HW_REG_NAME : hash_sel
 * DESCRIPTION : Select Hash functions to H1, H2, Key
 *
 *  Register Fields :
 *   [31: 6][RO] - Reserved
 *   [ 5: 4][RW] - Select Sbox control
 *   [ 3: 0][RW] - Select Hash control
 *
 */
#define PP_CLS_HASH_SEL_REG                              ((CLS_BASE_ADDR) + 0x8)
#define PP_CLS_HASH_SEL_RSVD0_OFF                                   (6)
#define PP_CLS_HASH_SEL_RSVD0_LEN                                   (26)
#define PP_CLS_HASH_SEL_RSVD0_MSK                                   (0xFFFFFFC0)
#define PP_CLS_HASH_SEL_RSVD0_RST                                   (0x0)
#define PP_CLS_HASH_SEL_SBOX_CTRL_OFF                               (4)
#define PP_CLS_HASH_SEL_SBOX_CTRL_LEN                               (2)
#define PP_CLS_HASH_SEL_SBOX_CTRL_MSK                               (0x00000030)
#define PP_CLS_HASH_SEL_SBOX_CTRL_RST                               (0x0)
#define PP_CLS_HASH_SEL_HASH_CTRL_OFF                               (0)
#define PP_CLS_HASH_SEL_HASH_CTRL_LEN                               (4)
#define PP_CLS_HASH_SEL_HASH_CTRL_MSK                               (0x0000000F)
#define PP_CLS_HASH_SEL_HASH_CTRL_RST                               (0x0)

/**
 * SW_REG_NAME : PP_CLS_FV_MASK_REG
 * HW_REG_NAME : fv_mask
 * DESCRIPTION : field vector mask bit ( mask for FV hash's)
 *
 *  Register Fields :
 *   [31: 0][RW] - feild vector mask bit at Hash function entry
 *
 */
#define PP_CLS_FV_MASK_REG                              ((CLS_BASE_ADDR) + 0x0C)
#define PP_CLS_FV_MASK_OFF                                          (0)
#define PP_CLS_FV_MASK_LEN                                          (32)
#define PP_CLS_FV_MASK_MSK                                          (0xFFFFFFFF)
#define PP_CLS_FV_MASK_RST                                          (0x0)
/**
 * REG_IDX_ACCESS   : PP_CLS_FV_MASK_REG_IDX
 * NUM OF REGISTERS : 32
 */
#define PP_CLS_FV_MASK_REG_IDX(idx) \
	(PP_CLS_FV_MASK_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_CLS_LU_MASK_REG
 * HW_REG_NAME : lu_mask
 * DESCRIPTION : masking lookup thread. this enable to use less
 *               lookup thread
 *
 *  Register Fields :
 *   [31: 0][RW] - lookup thread masking
 *
 */
#define PP_CLS_LU_MASK_REG                              ((CLS_BASE_ADDR) + 0x8C)
#define PP_CLS_LU_MASK_OFF                                          (0)
#define PP_CLS_LU_MASK_LEN                                          (32)
#define PP_CLS_LU_MASK_MSK                                          (0xFFFFFFFF)
#define PP_CLS_LU_MASK_RST                                          (0x0)

/**
 * SW_REG_NAME : PP_CLS_LU_CNT_PC_CFG_REG
 * HW_REG_NAME : lu_counter_pc_config
 * DESCRIPTION : process counter threshold ; count each FV that
 *               have process time larger the PC_threshold
 *
 *  Register Fields :
 *   [31: 0][RW] - process threshold
 *
 */
#define PP_CLS_LU_CNT_PC_CFG_REG                        ((CLS_BASE_ADDR) + 0x90)
#define PP_CLS_LU_CNT_PC_CFG_PC_THR_OFF                             (0)
#define PP_CLS_LU_CNT_PC_CFG_PC_THR_LEN                             (32)
#define PP_CLS_LU_CNT_PC_CFG_PC_THR_MSK                             (0xFFFFFFFF)
#define PP_CLS_LU_CNT_PC_CFG_PC_THR_RST                             (0x0)

/**
 * SW_REG_NAME : PP_CLS_LU_CNT_CONFG_REG
 * HW_REG_NAME : lu_counter_confg
 * DESCRIPTION : lookup counters config
 *
 *  Register Fields :
 *   [31: 5][RO] - Reserved
 *   [ 4: 4][RW] - clear counters in lookup thread
 *   [ 3: 1][RO] - Reserved
 *   [ 0: 0][RW] - enable counters in lookup thread
 *
 */
#define PP_CLS_LU_CNT_CONFG_REG                         ((CLS_BASE_ADDR) + 0x94)
#define PP_CLS_LU_CNT_CONFG_RSVD0_OFF                               (5)
#define PP_CLS_LU_CNT_CONFG_RSVD0_LEN                               (27)
#define PP_CLS_LU_CNT_CONFG_RSVD0_MSK                               (0xFFFFFFE0)
#define PP_CLS_LU_CNT_CONFG_RSVD0_RST                               (0x0)
#define PP_CLS_LU_CNT_CONFG_CNT_CLR_OFF                             (4)
#define PP_CLS_LU_CNT_CONFG_CNT_CLR_LEN                             (1)
#define PP_CLS_LU_CNT_CONFG_CNT_CLR_MSK                             (0x00000010)
#define PP_CLS_LU_CNT_CONFG_CNT_CLR_RST                             (0x0)
#define PP_CLS_LU_CNT_CONFG_RSVD1_OFF                               (1)
#define PP_CLS_LU_CNT_CONFG_RSVD1_LEN                               (3)
#define PP_CLS_LU_CNT_CONFG_RSVD1_MSK                               (0x0000000E)
#define PP_CLS_LU_CNT_CONFG_RSVD1_RST                               (0x0)
#define PP_CLS_LU_CNT_CONFG_CNT_EN_OFF                              (0)
#define PP_CLS_LU_CNT_CONFG_CNT_EN_LEN                              (1)
#define PP_CLS_LU_CNT_CONFG_CNT_EN_MSK                              (0x00000001)
#define PP_CLS_LU_CNT_CONFG_CNT_EN_RST                              (0x0)

/**
 * SW_REG_NAME : PP_CLS_LU_CNT_SIG_STATUS_REG
 * HW_REG_NAME : lu_counter_sig_status
 * DESCRIPTION : lu sig status, rise when bucket have more then 2
 *               equal signatures, this clear only after reset. bit
 *               per lookup thread
 *
 *  Register Fields :
 *   [31: 0][RO] - hash tables database error indecation, indicate
 *                 more then 2 signature in bucket
 *
 */
#define PP_CLS_LU_CNT_SIG_STATUS_REG                    ((CLS_BASE_ADDR) + 0x98)
#define PP_CLS_LU_CNT_SIG_STATUS_MATCH_SIG_ERROR_OFF                (0)
#define PP_CLS_LU_CNT_SIG_STATUS_MATCH_SIG_ERROR_LEN                (32)
#define PP_CLS_LU_CNT_SIG_STATUS_MATCH_SIG_ERROR_MSK                (0xFFFFFFFF)
#define PP_CLS_LU_CNT_SIG_STATUS_MATCH_SIG_ERROR_RST                (0x0)

/**
 * SW_REG_NAME : PP_CLS_LU_COUNT_REG
 * HW_REG_NAME : lu_count
 * DESCRIPTION : lookup counter - count lookups requested
 *
 *  Register Fields :
 *   [31: 0][RO] - counter of lookup request in lookup thread
 *
 */
#define PP_CLS_LU_COUNT_REG                             ((CLS_BASE_ADDR) + 0x9C)
#define PP_CLS_LU_COUNT_OFF                                         (0)
#define PP_CLS_LU_COUNT_LEN                                         (32)
#define PP_CLS_LU_COUNT_MSK                                         (0xFFFFFFFF)
#define PP_CLS_LU_COUNT_RST                                         (0x0)
/**
 * REG_IDX_ACCESS   : PP_CLS_LU_COUNT_REG_IDX
 * NUM OF REGISTERS : 32
 */
#define PP_CLS_LU_COUNT_REG_IDX(idx) \
	(PP_CLS_LU_COUNT_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_CLS_LU_MATCH_COUNT_REG
 * HW_REG_NAME : lu_match_count
 * DESCRIPTION : lookup counter - count matched lookups
 *
 *  Register Fields :
 *   [31: 0][RO] - counter of lookup matched in lookup thread
 *
 */
#define PP_CLS_LU_MATCH_COUNT_REG                      ((CLS_BASE_ADDR) + 0x11C)
#define PP_CLS_LU_MATCH_COUNT_LU_COUNT_OFF                          (0)
#define PP_CLS_LU_MATCH_COUNT_LU_COUNT_LEN                          (32)
#define PP_CLS_LU_MATCH_COUNT_LU_COUNT_MSK                          (0xFFFFFFFF)
#define PP_CLS_LU_MATCH_COUNT_LU_COUNT_RST                          (0x0)
/**
 * REG_IDX_ACCESS   : PP_CLS_LU_MATCH_COUNT_REG_IDX
 * NUM OF REGISTERS : 32
 */
#define PP_CLS_LU_MATCH_COUNT_REG_IDX(idx) \
	(PP_CLS_LU_MATCH_COUNT_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_CLS_LU_CNT_PC_RESULT_REG
 * HW_REG_NAME : lu_counter_pc_result
 * DESCRIPTION : process counter result ; return the count of each
 *               FV that have process time larger the PC_threshold
 *
 *  Register Fields :
 *   [31: 0][RO] - counter of process time larger then PC_threshold
 *                 in lookup thread
 *
 */
#define PP_CLS_LU_CNT_PC_RESULT_REG                    ((CLS_BASE_ADDR) + 0x19C)
#define PP_CLS_LU_CNT_PC_RESULT_PC_NUM_RESULT_OFF                   (0)
#define PP_CLS_LU_CNT_PC_RESULT_PC_NUM_RESULT_LEN                   (32)
#define PP_CLS_LU_CNT_PC_RESULT_PC_NUM_RESULT_MSK                   (0xFFFFFFFF)
#define PP_CLS_LU_CNT_PC_RESULT_PC_NUM_RESULT_RST                   (0x0)
/**
 * REG_IDX_ACCESS   : PP_CLS_LU_CNT_PC_RESULT_REG_IDX
 * NUM OF REGISTERS : 32
 */
#define PP_CLS_LU_CNT_PC_RESULT_REG_IDX(idx) \
	(PP_CLS_LU_CNT_PC_RESULT_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_CLS_SESSION_INFO_DEFAULT_REG
 * HW_REG_NAME : session_info_default
 * DESCRIPTION : lookup counters config
 *
 *  Register Fields :
 *   [31: 0][RW] - session default value
 *
 */
#define PP_CLS_SESSION_INFO_DEFAULT_REG                ((CLS_BASE_ADDR) + 0x21C)
#define PP_CLS_SESSION_INFO_DEFAULT_SI_DEFAULT_OFF                  (0)
#define PP_CLS_SESSION_INFO_DEFAULT_SI_DEFAULT_LEN                  (32)
#define PP_CLS_SESSION_INFO_DEFAULT_SI_DEFAULT_MSK                  (0xFFFFFFFF)
#define PP_CLS_SESSION_INFO_DEFAULT_SI_DEFAULT_RST                  (0x0)
/**
 * REG_IDX_ACCESS   : PP_CLS_SESSION_INFO_DEFAULT_REG_IDX
 * NUM OF REGISTERS : 64
 */
#define PP_CLS_SESSION_INFO_DEFAULT_REG_IDX(idx) \
	(PP_CLS_SESSION_INFO_DEFAULT_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_CLS_HASH_TABLE_1_BASE_REG
 * HW_REG_NAME : hash_table_1_base
 * DESCRIPTION : hash_table base address
 *
 *  Register Fields :
 *   [31: 0][RW] - hash table base address;  addr = hash_base_addr((4
 *                 + ht_pointer and ~hash_mask,8'h0
 *
 */
#define PP_CLS_HASH_TABLE_1_BASE_REG                   ((CLS_BASE_ADDR) + 0x31C)
#define PP_CLS_HASH_TABLE_1_BASE_HASH_BASE_ADDR_OFF                 (0)
#define PP_CLS_HASH_TABLE_1_BASE_HASH_BASE_ADDR_LEN                 (32)
#define PP_CLS_HASH_TABLE_1_BASE_HASH_BASE_ADDR_MSK                 (0xFFFFFFFF)
#define PP_CLS_HASH_TABLE_1_BASE_HASH_BASE_ADDR_RST                 (0x0)

/**
 * SW_REG_NAME : PP_CLS_HASH_TABLE_2_BASE_REG
 * HW_REG_NAME : hash_table_2_base
 * DESCRIPTION : hash_table base address
 *
 *  Register Fields :
 *   [31: 0][RW] - hash table base address;  addr = hash_base_addr((4
 *                 + ht_pointer and ~hash_mask,8'h0
 *
 */
#define PP_CLS_HASH_TABLE_2_BASE_REG                   ((CLS_BASE_ADDR) + 0x320)
#define PP_CLS_HASH_TABLE_2_BASE_HASH_BASE_ADDR_OFF                 (0)
#define PP_CLS_HASH_TABLE_2_BASE_HASH_BASE_ADDR_LEN                 (32)
#define PP_CLS_HASH_TABLE_2_BASE_HASH_BASE_ADDR_MSK                 (0xFFFFFFFF)
#define PP_CLS_HASH_TABLE_2_BASE_HASH_BASE_ADDR_RST                 (0x0)

/**
 * SW_REG_NAME : PP_CLS_SI_DDR_BASE_REG
 * HW_REG_NAME : si_ddr_base
 * DESCRIPTION : si ddr base address
 *
 *  Register Fields :
 *   [31: 0][RW] - si ddr base address ; addr = si_ddr_addr((4 +
 *                 SI_ID,8'h0
 *
 */
#define PP_CLS_SI_DDR_BASE_REG                         ((CLS_BASE_ADDR) + 0x324)
#define PP_CLS_SI_DDR_BASE_SI_DDR_ADDR_OFF                          (0)
#define PP_CLS_SI_DDR_BASE_SI_DDR_ADDR_LEN                          (32)
#define PP_CLS_SI_DDR_BASE_SI_DDR_ADDR_MSK                          (0xFFFFFFFF)
#define PP_CLS_SI_DDR_BASE_SI_DDR_ADDR_RST                          (0x0)

/**
 * SW_REG_NAME : PP_CLS_HASH_MASK_REG
 * HW_REG_NAME : hash_mask
 * DESCRIPTION : hash1 and hash2 mask bit; this register can allow
 *               to reduce the hash table size
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19: 0][RW] - feild vector mask bit at Hash function entry
 *
 */
#define PP_CLS_HASH_MASK_REG                           ((CLS_BASE_ADDR) + 0x328)
#define PP_CLS_HASH_MASK_RSVD0_OFF                                  (20)
#define PP_CLS_HASH_MASK_RSVD0_LEN                                  (12)
#define PP_CLS_HASH_MASK_RSVD0_MSK                                  (0xFFF00000)
#define PP_CLS_HASH_MASK_RSVD0_RST                                  (0x0)
#define PP_CLS_HASH_MASK_OFF                                        (0)
#define PP_CLS_HASH_MASK_LEN                                        (20)
#define PP_CLS_HASH_MASK_MSK                                        (0x000FFFFF)
#define PP_CLS_HASH_MASK_RST                                        (0x0)

/**
 * SW_REG_NAME : PP_CLS_SWAP_EN_REG
 * HW_REG_NAME : swap_enable_reg
 * DESCRIPTION : swap enable
 *
 *  Register Fields :
 *   [31: 9][RO] - Reserved
 *   [ 8: 8][RW] - FV byte endianess swap at cls egress
 *   [ 7: 5][RO] - Reserved
 *   [ 4: 4][RW] - swap ingress intput
 *   [ 3: 1][RO] - Reserved
 *   [ 0: 0][RW] - FV byte endianess swap and Mask but swap at cls
 *                 ingress
 *
 */
#define PP_CLS_SWAP_EN_REG                             ((CLS_BASE_ADDR) + 0x32C)
#define PP_CLS_SWAP_EN_RSVD0_OFF                                    (9)
#define PP_CLS_SWAP_EN_RSVD0_LEN                                    (23)
#define PP_CLS_SWAP_EN_RSVD0_MSK                                    (0xFFFFFE00)
#define PP_CLS_SWAP_EN_RSVD0_RST                                    (0x0)
#define PP_CLS_SWAP_EN_ENDIANESS_EGRESS_OFF                         (8)
#define PP_CLS_SWAP_EN_ENDIANESS_EGRESS_LEN                         (1)
#define PP_CLS_SWAP_EN_ENDIANESS_EGRESS_MSK                         (0x00000100)
#define PP_CLS_SWAP_EN_ENDIANESS_EGRESS_RST                         (0x1)
#define PP_CLS_SWAP_EN_RSVD1_OFF                                    (5)
#define PP_CLS_SWAP_EN_RSVD1_LEN                                    (3)
#define PP_CLS_SWAP_EN_RSVD1_MSK                                    (0x000000E0)
#define PP_CLS_SWAP_EN_RSVD1_RST                                    (0x0)
#define PP_CLS_SWAP_EN_SWAP_INGRESS_OFF                             (4)
#define PP_CLS_SWAP_EN_SWAP_INGRESS_LEN                             (1)
#define PP_CLS_SWAP_EN_SWAP_INGRESS_MSK                             (0x00000010)
#define PP_CLS_SWAP_EN_SWAP_INGRESS_RST                             (0x0)
#define PP_CLS_SWAP_EN_RSVD2_OFF                                    (1)
#define PP_CLS_SWAP_EN_RSVD2_LEN                                    (3)
#define PP_CLS_SWAP_EN_RSVD2_MSK                                    (0x0000000E)
#define PP_CLS_SWAP_EN_RSVD2_RST                                    (0x0)
#define PP_CLS_SWAP_EN_ENDIANESS_INGRESS_OFF                        (0)
#define PP_CLS_SWAP_EN_ENDIANESS_INGRESS_LEN                        (1)
#define PP_CLS_SWAP_EN_ENDIANESS_INGRESS_MSK                        (0x00000001)
#define PP_CLS_SWAP_EN_ENDIANESS_INGRESS_RST                        (0x0)

/**
 * SW_REG_NAME : PP_CLS_CACHE_CFG_REG
 * HW_REG_NAME : cache_config
 * DESCRIPTION : signature cache config
 *
 *  Register Fields :
 *   [31:31][RW] - preform cache invalidate to both caches; preform
 *                 in 1 cycle
 *   [30:26][RO] - Reserved
 *   [25:25][RW] - stop counter at hash table signature cache 2
 *   [24:24][RW] - stop counter at hash table signature cache 1
 *   [23:18][RO] - Reserved
 *   [17:17][RW] - clear interrupt at hash table signature cache 2
 *   [16:16][RW] - clear interrupt at hash table signature cache 1
 *   [15:10][RO] - Reserved
 *   [ 9: 9][RW] - clear counter in hash table signature cache 2
 *   [ 8: 8][RW] - clear counter in hash table signature cache 1
 *   [ 7: 1][RO] - Reserved
 *   [ 0: 0][RW] - signature cache disable
 *
 */
#define PP_CLS_CACHE_CFG_REG                           ((CLS_BASE_ADDR) + 0x330)
#define PP_CLS_CACHE_CFG_CACHE_INVALIDATE_OFF                       (31)
#define PP_CLS_CACHE_CFG_CACHE_INVALIDATE_LEN                       (1)
#define PP_CLS_CACHE_CFG_CACHE_INVALIDATE_MSK                       (0x80000000)
#define PP_CLS_CACHE_CFG_CACHE_INVALIDATE_RST                       (0x0)
#define PP_CLS_CACHE_CFG_RSVD0_OFF                                  (26)
#define PP_CLS_CACHE_CFG_RSVD0_LEN                                  (5)
#define PP_CLS_CACHE_CFG_RSVD0_MSK                                  (0x7C000000)
#define PP_CLS_CACHE_CFG_RSVD0_RST                                  (0x0)
#define PP_CLS_CACHE_CFG_STAT_CNT_STOP_2_OFF                        (25)
#define PP_CLS_CACHE_CFG_STAT_CNT_STOP_2_LEN                        (1)
#define PP_CLS_CACHE_CFG_STAT_CNT_STOP_2_MSK                        (0x02000000)
#define PP_CLS_CACHE_CFG_STAT_CNT_STOP_2_RST                        (0x0)
#define PP_CLS_CACHE_CFG_STAT_CNT_STOP_1_OFF                        (24)
#define PP_CLS_CACHE_CFG_STAT_CNT_STOP_1_LEN                        (1)
#define PP_CLS_CACHE_CFG_STAT_CNT_STOP_1_MSK                        (0x01000000)
#define PP_CLS_CACHE_CFG_STAT_CNT_STOP_1_RST                        (0x0)
#define PP_CLS_CACHE_CFG_RSVD1_OFF                                  (18)
#define PP_CLS_CACHE_CFG_RSVD1_LEN                                  (6)
#define PP_CLS_CACHE_CFG_RSVD1_MSK                                  (0x00FC0000)
#define PP_CLS_CACHE_CFG_RSVD1_RST                                  (0x0)
#define PP_CLS_CACHE_CFG_CLR_INTR_2_OFF                             (17)
#define PP_CLS_CACHE_CFG_CLR_INTR_2_LEN                             (1)
#define PP_CLS_CACHE_CFG_CLR_INTR_2_MSK                             (0x00020000)
#define PP_CLS_CACHE_CFG_CLR_INTR_2_RST                             (0x0)
#define PP_CLS_CACHE_CFG_CLR_INTR_1_OFF                             (16)
#define PP_CLS_CACHE_CFG_CLR_INTR_1_LEN                             (1)
#define PP_CLS_CACHE_CFG_CLR_INTR_1_MSK                             (0x00010000)
#define PP_CLS_CACHE_CFG_CLR_INTR_1_RST                             (0x0)
#define PP_CLS_CACHE_CFG_RSVD2_OFF                                  (10)
#define PP_CLS_CACHE_CFG_RSVD2_LEN                                  (6)
#define PP_CLS_CACHE_CFG_RSVD2_MSK                                  (0x0000FC00)
#define PP_CLS_CACHE_CFG_RSVD2_RST                                  (0x0)
#define PP_CLS_CACHE_CFG_CLR_STAT_2_OFF                             (9)
#define PP_CLS_CACHE_CFG_CLR_STAT_2_LEN                             (1)
#define PP_CLS_CACHE_CFG_CLR_STAT_2_MSK                             (0x00000200)
#define PP_CLS_CACHE_CFG_CLR_STAT_2_RST                             (0x0)
#define PP_CLS_CACHE_CFG_CLR_STAT_1_OFF                             (8)
#define PP_CLS_CACHE_CFG_CLR_STAT_1_LEN                             (1)
#define PP_CLS_CACHE_CFG_CLR_STAT_1_MSK                             (0x00000100)
#define PP_CLS_CACHE_CFG_CLR_STAT_1_RST                             (0x0)
#define PP_CLS_CACHE_CFG_RSVD3_OFF                                  (1)
#define PP_CLS_CACHE_CFG_RSVD3_LEN                                  (7)
#define PP_CLS_CACHE_CFG_RSVD3_MSK                                  (0x000000FE)
#define PP_CLS_CACHE_CFG_RSVD3_RST                                  (0x0)
#define PP_CLS_CACHE_CFG_CACHE_DIS_OFF                              (0)
#define PP_CLS_CACHE_CFG_CACHE_DIS_LEN                              (1)
#define PP_CLS_CACHE_CFG_CACHE_DIS_MSK                              (0x00000001)
#define PP_CLS_CACHE_CFG_CACHE_DIS_RST                              (0x0)

/**
 * SW_REG_NAME : PP_CLS_CACHE_INTR_REG
 * HW_REG_NAME : cache_interrupt
 * DESCRIPTION : signature cache interrupt
 *
 *  Register Fields :
 *   [31:10][RO] - Reserved
 *   [ 9: 9][RO] - memory error for hash table signature cache 2
 *   [ 8: 8][RO] - memory error for hash table signature cache 1
 *   [ 7: 2][RO] - Reserved
 *   [ 1: 1][RO] - interrupt for hash table signature cache 2
 *   [ 0: 0][RO] - interrupt for hash table signature cache 1
 *
 */
#define PP_CLS_CACHE_INTR_REG                          ((CLS_BASE_ADDR) + 0x334)
#define PP_CLS_CACHE_INTR_RSVD0_OFF                                 (10)
#define PP_CLS_CACHE_INTR_RSVD0_LEN                                 (22)
#define PP_CLS_CACHE_INTR_RSVD0_MSK                                 (0xFFFFFC00)
#define PP_CLS_CACHE_INTR_RSVD0_RST                                 (0x0)
#define PP_CLS_CACHE_INTR_MEM_ERROR_2_OFF                           (9)
#define PP_CLS_CACHE_INTR_MEM_ERROR_2_LEN                           (1)
#define PP_CLS_CACHE_INTR_MEM_ERROR_2_MSK                           (0x00000200)
#define PP_CLS_CACHE_INTR_MEM_ERROR_2_RST                           (0x0)
#define PP_CLS_CACHE_INTR_MEM_ERROR_1_OFF                           (8)
#define PP_CLS_CACHE_INTR_MEM_ERROR_1_LEN                           (1)
#define PP_CLS_CACHE_INTR_MEM_ERROR_1_MSK                           (0x00000100)
#define PP_CLS_CACHE_INTR_MEM_ERROR_1_RST                           (0x0)
#define PP_CLS_CACHE_INTR_RSVD1_OFF                                 (2)
#define PP_CLS_CACHE_INTR_RSVD1_LEN                                 (6)
#define PP_CLS_CACHE_INTR_RSVD1_MSK                                 (0x000000FC)
#define PP_CLS_CACHE_INTR_RSVD1_RST                                 (0x0)
#define PP_CLS_CACHE_INTR_CACHE_INTR_2_OFF                          (1)
#define PP_CLS_CACHE_INTR_CACHE_INTR_2_LEN                          (1)
#define PP_CLS_CACHE_INTR_CACHE_INTR_2_MSK                          (0x00000002)
#define PP_CLS_CACHE_INTR_CACHE_INTR_2_RST                          (0x0)
#define PP_CLS_CACHE_INTR_CACHE_INTR_1_OFF                          (0)
#define PP_CLS_CACHE_INTR_CACHE_INTR_1_LEN                          (1)
#define PP_CLS_CACHE_INTR_CACHE_INTR_1_MSK                          (0x00000001)
#define PP_CLS_CACHE_INTR_CACHE_INTR_1_RST                          (0x0)

/**
 * SW_REG_NAME : PP_CLS_CACHE_STAT_MSB_INTERVAL_1_REG
 * HW_REG_NAME : cache_stat_msb_interval_1
 * DESCRIPTION : statistic_interval
 *
 *  Register Fields :
 *   [31: 0][RW] - set interval time (in cycles) for signature caches
 *
 */
#define PP_CLS_CACHE_STAT_MSB_INTERVAL_1_REG           ((CLS_BASE_ADDR) + 0x338)
#define PP_CLS_CACHE_STAT_MSB_INTERVAL_1_CACHE_STAT_INTERVAL_OFF    (0)
#define PP_CLS_CACHE_STAT_MSB_INTERVAL_1_CACHE_STAT_INTERVAL_LEN    (32)
#define PP_CLS_CACHE_STAT_MSB_INTERVAL_1_CACHE_STAT_INTERVAL_MSK    (0xFFFFFFFF)
#define PP_CLS_CACHE_STAT_MSB_INTERVAL_1_CACHE_STAT_INTERVAL_RST    (0x0)

/**
 * SW_REG_NAME : PP_CLS_CACHE_STAT_LSB_INTERVAL_1_REG
 * HW_REG_NAME : cache_stat_lsb_interval_1
 * DESCRIPTION : statistic_interval
 *
 *  Register Fields :
 *   [31: 0][RW] - set interval time (in cycles) for signature caches
 *
 */
#define PP_CLS_CACHE_STAT_LSB_INTERVAL_1_REG           ((CLS_BASE_ADDR) + 0x33C)
#define PP_CLS_CACHE_STAT_LSB_INTERVAL_1_CACHE_STAT_INTERVAL_OFF    (0)
#define PP_CLS_CACHE_STAT_LSB_INTERVAL_1_CACHE_STAT_INTERVAL_LEN    (32)
#define PP_CLS_CACHE_STAT_LSB_INTERVAL_1_CACHE_STAT_INTERVAL_MSK    (0xFFFFFFFF)
#define PP_CLS_CACHE_STAT_LSB_INTERVAL_1_CACHE_STAT_INTERVAL_RST    (0x0)

/**
 * SW_REG_NAME : PP_CLS_CACHE_STAT_MSB_INTERVAL_2_REG
 * HW_REG_NAME : cache_stat_msb_interval_2
 * DESCRIPTION : statistic_interval
 *
 *  Register Fields :
 *   [31: 0][RW] - set interval time (in cycles) for signature caches
 *
 */
#define PP_CLS_CACHE_STAT_MSB_INTERVAL_2_REG           ((CLS_BASE_ADDR) + 0x340)
#define PP_CLS_CACHE_STAT_MSB_INTERVAL_2_CACHE_STAT_INTERVAL_OFF    (0)
#define PP_CLS_CACHE_STAT_MSB_INTERVAL_2_CACHE_STAT_INTERVAL_LEN    (32)
#define PP_CLS_CACHE_STAT_MSB_INTERVAL_2_CACHE_STAT_INTERVAL_MSK    (0xFFFFFFFF)
#define PP_CLS_CACHE_STAT_MSB_INTERVAL_2_CACHE_STAT_INTERVAL_RST    (0x0)

/**
 * SW_REG_NAME : PP_CLS_CACHE_STAT_LSB_INTERVAL_2_REG
 * HW_REG_NAME : cache_stat_lsb_interval_2
 * DESCRIPTION : statistic_interval
 *
 *  Register Fields :
 *   [31: 0][RW] - set interval time (in cycles) for signature caches
 *
 */
#define PP_CLS_CACHE_STAT_LSB_INTERVAL_2_REG           ((CLS_BASE_ADDR) + 0x344)
#define PP_CLS_CACHE_STAT_LSB_INTERVAL_2_CACHE_STAT_INTERVAL_OFF    (0)
#define PP_CLS_CACHE_STAT_LSB_INTERVAL_2_CACHE_STAT_INTERVAL_LEN    (32)
#define PP_CLS_CACHE_STAT_LSB_INTERVAL_2_CACHE_STAT_INTERVAL_MSK    (0xFFFFFFFF)
#define PP_CLS_CACHE_STAT_LSB_INTERVAL_2_CACHE_STAT_INTERVAL_RST    (0x0)

/**
 * SW_REG_NAME : PP_CLS_CACHE_TRANS_STAT_1_REG
 * HW_REG_NAME : cache_trans_statistic_1
 * DESCRIPTION : cache trans_statistic
 *
 *  Register Fields :
 *   [31: 0][RO] - signature cache transaction counter
 *
 */
#define PP_CLS_CACHE_TRANS_STAT_1_REG                  ((CLS_BASE_ADDR) + 0x348)
#define PP_CLS_CACHE_TRANS_STAT_1_OFF                               (0)
#define PP_CLS_CACHE_TRANS_STAT_1_LEN                               (32)
#define PP_CLS_CACHE_TRANS_STAT_1_MSK                               (0xFFFFFFFF)
#define PP_CLS_CACHE_TRANS_STAT_1_RST                               (0x0)

/**
 * SW_REG_NAME : PP_CLS_CACHE_TRANS_STAT_2_REG
 * HW_REG_NAME : cache_trans_statistic_2
 * DESCRIPTION : cache trans_statistic
 *
 *  Register Fields :
 *   [31: 0][RO] - signature cache transaction counter
 *
 */
#define PP_CLS_CACHE_TRANS_STAT_2_REG                  ((CLS_BASE_ADDR) + 0x34C)
#define PP_CLS_CACHE_TRANS_STAT_2_OFF                               (0)
#define PP_CLS_CACHE_TRANS_STAT_2_LEN                               (32)
#define PP_CLS_CACHE_TRANS_STAT_2_MSK                               (0xFFFFFFFF)
#define PP_CLS_CACHE_TRANS_STAT_2_RST                               (0x0)

/**
 * SW_REG_NAME : PP_CLS_CACHE_HIT_STAT_1_REG
 * HW_REG_NAME : cache_hit_statistic_1
 * DESCRIPTION : cache hit statistic
 *
 *  Register Fields :
 *   [31: 0][RO] - signature cache hit counter
 *
 */
#define PP_CLS_CACHE_HIT_STAT_1_REG                    ((CLS_BASE_ADDR) + 0x350)
#define PP_CLS_CACHE_HIT_STAT_1_CACHE_MATCH_STAT_OFF                (0)
#define PP_CLS_CACHE_HIT_STAT_1_CACHE_MATCH_STAT_LEN                (32)
#define PP_CLS_CACHE_HIT_STAT_1_CACHE_MATCH_STAT_MSK                (0xFFFFFFFF)
#define PP_CLS_CACHE_HIT_STAT_1_CACHE_MATCH_STAT_RST                (0x0)

/**
 * SW_REG_NAME : PP_CLS_CACHE_HIT_STAT_2_REG
 * HW_REG_NAME : cache_hit_statistic_2
 * DESCRIPTION : cache hit statistic
 *
 *  Register Fields :
 *   [31: 0][RO] - signature cache hit counter
 *
 */
#define PP_CLS_CACHE_HIT_STAT_2_REG                    ((CLS_BASE_ADDR) + 0x354)
#define PP_CLS_CACHE_HIT_STAT_2_CACHE_MATCH_STAT_OFF                (0)
#define PP_CLS_CACHE_HIT_STAT_2_CACHE_MATCH_STAT_LEN                (32)
#define PP_CLS_CACHE_HIT_STAT_2_CACHE_MATCH_STAT_MSK                (0xFFFFFFFF)
#define PP_CLS_CACHE_HIT_STAT_2_CACHE_MATCH_STAT_RST                (0x0)

/**
 * SW_REG_NAME : PP_CLS_DBG_HASH_1_REG
 * HW_REG_NAME : dbg_hash_1
 * DESCRIPTION : dbg hash
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19: 0][RO] - hash1 on egress cls
 *
 */
#define PP_CLS_DBG_HASH_1_REG                          ((CLS_BASE_ADDR) + 0x358)
#define PP_CLS_DBG_HASH_1_RSVD0_OFF                                 (20)
#define PP_CLS_DBG_HASH_1_RSVD0_LEN                                 (12)
#define PP_CLS_DBG_HASH_1_RSVD0_MSK                                 (0xFFF00000)
#define PP_CLS_DBG_HASH_1_RSVD0_RST                                 (0x0)
#define PP_CLS_DBG_HASH_1_OFF                                       (0)
#define PP_CLS_DBG_HASH_1_LEN                                       (20)
#define PP_CLS_DBG_HASH_1_MSK                                       (0x000FFFFF)
#define PP_CLS_DBG_HASH_1_RST                                       (0x0)

/**
 * SW_REG_NAME : PP_CLS_DBG_HASH_2_REG
 * HW_REG_NAME : dbg_hash_2
 * DESCRIPTION : dbg hash
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19: 0][RO] - hash1 on egress cls
 *
 */
#define PP_CLS_DBG_HASH_2_REG                          ((CLS_BASE_ADDR) + 0x35C)
#define PP_CLS_DBG_HASH_2_RSVD0_OFF                                 (20)
#define PP_CLS_DBG_HASH_2_RSVD0_LEN                                 (12)
#define PP_CLS_DBG_HASH_2_RSVD0_MSK                                 (0xFFF00000)
#define PP_CLS_DBG_HASH_2_RSVD0_RST                                 (0x0)
#define PP_CLS_DBG_HASH_2_OFF                                       (0)
#define PP_CLS_DBG_HASH_2_LEN                                       (20)
#define PP_CLS_DBG_HASH_2_MSK                                       (0x000FFFFF)
#define PP_CLS_DBG_HASH_2_RST                                       (0x0)

/**
 * SW_REG_NAME : PP_CLS_DBG_HASH_SIG_REG
 * HW_REG_NAME : dbg_hash_sig
 * DESCRIPTION : dbg hash1
 *
 *  Register Fields :
 *   [31: 0][RO] - hash2 on egress cls
 *
 */
#define PP_CLS_DBG_HASH_SIG_REG                        ((CLS_BASE_ADDR) + 0x360)
#define PP_CLS_DBG_HASH_SIG_DBG_SIG_HASH_OFF                        (0)
#define PP_CLS_DBG_HASH_SIG_DBG_SIG_HASH_LEN                        (32)
#define PP_CLS_DBG_HASH_SIG_DBG_SIG_HASH_MSK                        (0xFFFFFFFF)
#define PP_CLS_DBG_HASH_SIG_DBG_SIG_HASH_RST                        (0x0)

/**
 * SW_REG_NAME : PP_CLS_DBG_FV_REG
 * HW_REG_NAME : dbg_fv
 * DESCRIPTION : dbg FV
 *
 *  Register Fields :
 *   [31: 0][RO] - FV on egress cls  (not incluse masks)
 *
 */
#define PP_CLS_DBG_FV_REG                              ((CLS_BASE_ADDR) + 0x364)
#define PP_CLS_DBG_FV_OFF                                           (0)
#define PP_CLS_DBG_FV_LEN                                           (32)
#define PP_CLS_DBG_FV_MSK                                           (0xFFFFFFFF)
#define PP_CLS_DBG_FV_RST                                           (0x0)
/**
 * REG_IDX_ACCESS   : PP_CLS_DBG_FV_REG_IDX
 * NUM OF REGISTERS : 32
 */
#define PP_CLS_DBG_FV_REG_IDX(idx) \
	(PP_CLS_DBG_FV_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_CLS_LU_THREAD_STM_STATUS_REG
 * HW_REG_NAME : lu_thread_stm_status
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:24][RO] - Reserved
 *   [23: 0][RO] - lu_therad STM status; stm_lu[n+3],
 *                 stm_lu[n+2],stm_lu[n+1] ,stm_lu[n+0]
 *
 */
#define PP_CLS_LU_THREAD_STM_STATUS_REG                ((CLS_BASE_ADDR) + 0x3E4)
#define PP_CLS_LU_THREAD_STM_STATUS_RSVD0_OFF                       (24)
#define PP_CLS_LU_THREAD_STM_STATUS_RSVD0_LEN                       (8)
#define PP_CLS_LU_THREAD_STM_STATUS_RSVD0_MSK                       (0xFF000000)
#define PP_CLS_LU_THREAD_STM_STATUS_RSVD0_RST                       (0x0)
#define PP_CLS_LU_THREAD_STM_STATUS_OFF                             (0)
#define PP_CLS_LU_THREAD_STM_STATUS_LEN                             (24)
#define PP_CLS_LU_THREAD_STM_STATUS_MSK                             (0x00FFFFFF)
#define PP_CLS_LU_THREAD_STM_STATUS_RST                             (0x41041)
/**
 * REG_IDX_ACCESS   : PP_CLS_LU_THREAD_STM_STATUS_REG_IDX
 * NUM OF REGISTERS : 4
 */
#define PP_CLS_LU_THREAD_STM_STATUS_REG_IDX(idx) \
	(PP_CLS_LU_THREAD_STM_STATUS_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_CLS_PKT_COUNT_PARSER_REG
 * HW_REG_NAME : packet_count_parser
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 0][RW] - packet_count from parser
 *
 */
#define PP_CLS_PKT_COUNT_PARSER_REG                    ((CLS_BASE_ADDR) + 0x3F4)
#define PP_CLS_PKT_COUNT_PARSER_PKT_COUNT_PARSER_I_OFF              (0)
#define PP_CLS_PKT_COUNT_PARSER_PKT_COUNT_PARSER_I_LEN              (32)
#define PP_CLS_PKT_COUNT_PARSER_PKT_COUNT_PARSER_I_MSK              (0xFFFFFFFF)
#define PP_CLS_PKT_COUNT_PARSER_PKT_COUNT_PARSER_I_RST              (0x0)

/**
 * SW_REG_NAME : PP_CLS_PKT_COUNT_UC_REG
 * HW_REG_NAME : packet_count_uc
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 0][RW] - packet_count from parser
 *
 */
#define PP_CLS_PKT_COUNT_UC_REG                        ((CLS_BASE_ADDR) + 0x3F8)
#define PP_CLS_PKT_COUNT_UC_PKT_COUNT_UC_REG_OFF                    (0)
#define PP_CLS_PKT_COUNT_UC_PKT_COUNT_UC_REG_LEN                    (32)
#define PP_CLS_PKT_COUNT_UC_PKT_COUNT_UC_REG_MSK                    (0xFFFFFFFF)
#define PP_CLS_PKT_COUNT_UC_PKT_COUNT_UC_REG_RST                    (0x0)

/**
 * SW_REG_NAME : PP_CLS_DBG_STW_REG
 * HW_REG_NAME : dbg_stw_reg
 * DESCRIPTION : dbg FV
 *
 *  Register Fields :
 *   [31: 0][RO] - STW on egress cls  (not incluse masks)
 *
 */
#define PP_CLS_DBG_STW_REG                             ((CLS_BASE_ADDR) + 0x3FC)
#define PP_CLS_DBG_STW_OFF                                          (0)
#define PP_CLS_DBG_STW_LEN                                          (32)
#define PP_CLS_DBG_STW_MSK                                          (0xFFFFFFFF)
#define PP_CLS_DBG_STW_RST                                          (0x0)
/**
 * REG_IDX_ACCESS   : PP_CLS_DBG_STW_REG_IDX
 * NUM OF REGISTERS : 9
 */
#define PP_CLS_DBG_STW_REG_IDX(idx) \
	(PP_CLS_DBG_STW_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_CLS_FV_TO_HASH_CALC_REG
 * HW_REG_NAME : fv_to_hash_calc_reg
 * DESCRIPTION : fv_to_hash_calc_reg, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][RW] - FV for hash calc
 *
 */
#define PP_CLS_FV_TO_HASH_CALC_REG                     ((CLS_BASE_ADDR) + 0x500)
#define PP_CLS_FV_TO_HASH_CALC_FV_TO_HASH_CALC_REG_OFF              (0)
#define PP_CLS_FV_TO_HASH_CALC_FV_TO_HASH_CALC_REG_LEN              (32)
#define PP_CLS_FV_TO_HASH_CALC_FV_TO_HASH_CALC_REG_MSK              (0xFFFFFFFF)
#define PP_CLS_FV_TO_HASH_CALC_FV_TO_HASH_CALC_REG_RST              (0x0)
/**
 * REG_IDX_ACCESS   : PP_CLS_FV_TO_HASH_CALC_REG_IDX
 * NUM OF REGISTERS : 32
 */
#define PP_CLS_FV_TO_HASH_CALC_REG_IDX(idx) \
	(PP_CLS_FV_TO_HASH_CALC_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_CLS_GO_CALC_REG
 * HW_REG_NAME : go_calc
 * DESCRIPTION : go_calc, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 9][RO] - Reserved
 *   [ 8: 8][RO] - hash calc ready to do hash calc
 *   [ 7: 1][RO] - Reserved
 *   [ 0: 0][RW] - write 1 to do FV for hash calc
 *
 */
#define PP_CLS_GO_CALC_REG                             ((CLS_BASE_ADDR) + 0x580)
#define PP_CLS_GO_CALC_RSVD0_OFF                                    (9)
#define PP_CLS_GO_CALC_RSVD0_LEN                                    (23)
#define PP_CLS_GO_CALC_RSVD0_MSK                                    (0xFFFFFE00)
#define PP_CLS_GO_CALC_RSVD0_RST                                    (0x0)
#define PP_CLS_GO_CALC_GO_READY_OFF                                 (8)
#define PP_CLS_GO_CALC_GO_READY_LEN                                 (1)
#define PP_CLS_GO_CALC_GO_READY_MSK                                 (0x00000100)
#define PP_CLS_GO_CALC_GO_READY_RST                                 (0x0)
#define PP_CLS_GO_CALC_RSVD1_OFF                                    (1)
#define PP_CLS_GO_CALC_RSVD1_LEN                                    (7)
#define PP_CLS_GO_CALC_RSVD1_MSK                                    (0x000000FE)
#define PP_CLS_GO_CALC_RSVD1_RST                                    (0x0)
#define PP_CLS_GO_CALC_OFF                                          (0)
#define PP_CLS_GO_CALC_LEN                                          (1)
#define PP_CLS_GO_CALC_MSK                                          (0x00000001)
#define PP_CLS_GO_CALC_RST                                          (0x0)

/**
 * SW_REG_NAME : PP_CLS_CALC_HASH_1_REG
 * HW_REG_NAME : calc_hash_1
 * DESCRIPTION : dbg hash, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19: 0][RO] - hash1 result
 *
 */
#define PP_CLS_CALC_HASH_1_REG                         ((CLS_BASE_ADDR) + 0x584)
#define PP_CLS_CALC_HASH_1_RSVD0_OFF                                (20)
#define PP_CLS_CALC_HASH_1_RSVD0_LEN                                (12)
#define PP_CLS_CALC_HASH_1_RSVD0_MSK                                (0xFFF00000)
#define PP_CLS_CALC_HASH_1_RSVD0_RST                                (0x0)
#define PP_CLS_CALC_HASH_1_OFF                                      (0)
#define PP_CLS_CALC_HASH_1_LEN                                      (20)
#define PP_CLS_CALC_HASH_1_MSK                                      (0x000FFFFF)
#define PP_CLS_CALC_HASH_1_RST                                      (0x0)

/**
 * SW_REG_NAME : PP_CLS_CALC_HASH_2_REG
 * HW_REG_NAME : calc_hash_2
 * DESCRIPTION : dbg hash, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19: 0][RO] - hash1 result
 *
 */
#define PP_CLS_CALC_HASH_2_REG                         ((CLS_BASE_ADDR) + 0x588)
#define PP_CLS_CALC_HASH_2_RSVD0_OFF                                (20)
#define PP_CLS_CALC_HASH_2_RSVD0_LEN                                (12)
#define PP_CLS_CALC_HASH_2_RSVD0_MSK                                (0xFFF00000)
#define PP_CLS_CALC_HASH_2_RSVD0_RST                                (0x0)
#define PP_CLS_CALC_HASH_2_OFF                                      (0)
#define PP_CLS_CALC_HASH_2_LEN                                      (20)
#define PP_CLS_CALC_HASH_2_MSK                                      (0x000FFFFF)
#define PP_CLS_CALC_HASH_2_RST                                      (0x0)

/**
 * SW_REG_NAME : PP_CLS_CALC_HASH_SIG_REG
 * HW_REG_NAME : calc_hash_sig
 * DESCRIPTION : dbg hash1, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][RO] - hash2 result
 *
 */
#define PP_CLS_CALC_HASH_SIG_REG                       ((CLS_BASE_ADDR) + 0x58C)
#define PP_CLS_CALC_HASH_SIG_CALC_SIG_HASH_OFF                      (0)
#define PP_CLS_CALC_HASH_SIG_CALC_SIG_HASH_LEN                      (32)
#define PP_CLS_CALC_HASH_SIG_CALC_SIG_HASH_MSK                      (0xFFFFFFFF)
#define PP_CLS_CALC_HASH_SIG_CALC_SIG_HASH_RST                      (0x0)

/**
 * SW_REG_NAME : PP_CLS_CMD_STATUS_REG
 * HW_REG_NAME : cmd_status
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:16][RO] - FW command; hold the amount of word remain
 *   [15: 0][RO] - command status; hold the amount of non-finished
 *                 commands in the FIFO and number of unused words in
 *                 FIFO
 *
 */
#define PP_CLS_CMD_STATUS_REG                          ((CLS_BASE_ADDR) + 0x700)
#define PP_CLS_CMD_STATUS_FW_REMAIN_WORD_OFF                        (16)
#define PP_CLS_CMD_STATUS_FW_REMAIN_WORD_LEN                        (16)
#define PP_CLS_CMD_STATUS_FW_REMAIN_WORD_MSK                        (0xFFFF0000)
#define PP_CLS_CMD_STATUS_FW_REMAIN_WORD_RST                        (0x90)
#define PP_CLS_CMD_STATUS_OFF                                       (0)
#define PP_CLS_CMD_STATUS_LEN                                       (16)
#define PP_CLS_CMD_STATUS_MSK                                       (0x0000FFFF)
#define PP_CLS_CMD_STATUS_RST                                       (0x0)

/**
 * SW_REG_NAME : PP_CLS_CMD_DATA_REG
 * HW_REG_NAME : cmd_data
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 0][WO] - command data; FIFO like register
 *
 */
#define PP_CLS_CMD_DATA_REG                            ((CLS_BASE_ADDR) + 0x800)
#define PP_CLS_CMD_DATA_OFF                                         (0)
#define PP_CLS_CMD_DATA_LEN                                         (32)
#define PP_CLS_CMD_DATA_MSK                                         (0xFFFFFFFF)
#define PP_CLS_CMD_DATA_RST                                         (0x0)

/**
 * SW_REG_NAME : PP_CLS_CMD_KICK_REG
 * HW_REG_NAME : cmd_kick
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 0][WO] - command kick; FIFO like register
 *
 */
#define PP_CLS_CMD_KICK_REG                            ((CLS_BASE_ADDR) + 0x804)
#define PP_CLS_CMD_KICK_OFF                                         (0)
#define PP_CLS_CMD_KICK_LEN                                         (32)
#define PP_CLS_CMD_KICK_MSK                                         (0xFFFFFFFF)
#define PP_CLS_CMD_KICK_RST                                         (0x0)

#endif
