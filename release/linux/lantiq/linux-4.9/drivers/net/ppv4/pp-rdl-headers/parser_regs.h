/**
 * parser_regs.h
 * Description: ppv4_parser_s HW registers definitions
 * Auto-generated file by RDL Parser, DO NOT CHANGE
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2019-2020 Intel Corporation
 */

#ifndef _PP_PRSR_H_
#define _PP_PRSR_H_

#define PP_PRSR_GEN_DATE_STR                  "Sunday May 17, 2020 [5:26:11 pm]"
#define PP_PRSR_BASE                                             (0xF0100000ULL)

/**
 * SW_REG_NAME : PP_PRSR_OUTPUT_SWAP_REG
 * HW_REG_NAME : output_swap
 * DESCRIPTION : Swap output data
 *
 *  Register Fields :
 *   [31: 3][RO] - Reserved
 *   [ 2: 2][RW] - Swap Output FV, 0: first send the LSB of
 *                 field_vector (after swap if 'swap_fv' is
 *                 asserted), 1: first send the MSB of field_vector
 *   [ 1: 1][RW] - Swap FV mask, 0: compatible to big endian FV, 1:
 *                 compatible to little endian FV
 *   [ 0: 0][RW] - Swap FV, 0: big endian (legacy), 1: little endian
 *
 */
#define PP_PRSR_OUTPUT_SWAP_REG                         ((PRSR_BASE_ADDR) + 0x0)
#define PP_PRSR_OUTPUT_SWAP_RSVD0_OFF                               (3)
#define PP_PRSR_OUTPUT_SWAP_RSVD0_LEN                               (29)
#define PP_PRSR_OUTPUT_SWAP_RSVD0_MSK                               (0xFFFFFFF8)
#define PP_PRSR_OUTPUT_SWAP_RSVD0_RST                               (0x0)
#define PP_PRSR_OUTPUT_SWAP_SWAP_OP_FV_OFF                          (2)
#define PP_PRSR_OUTPUT_SWAP_SWAP_OP_FV_LEN                          (1)
#define PP_PRSR_OUTPUT_SWAP_SWAP_OP_FV_MSK                          (0x00000004)
#define PP_PRSR_OUTPUT_SWAP_SWAP_OP_FV_RST                          (0x0)
#define PP_PRSR_OUTPUT_SWAP_SWAP_FVM_OFF                            (1)
#define PP_PRSR_OUTPUT_SWAP_SWAP_FVM_LEN                            (1)
#define PP_PRSR_OUTPUT_SWAP_SWAP_FVM_MSK                            (0x00000002)
#define PP_PRSR_OUTPUT_SWAP_SWAP_FVM_RST                            (0x1)
#define PP_PRSR_OUTPUT_SWAP_SWAP_FV_OFF                             (0)
#define PP_PRSR_OUTPUT_SWAP_SWAP_FV_LEN                             (1)
#define PP_PRSR_OUTPUT_SWAP_SWAP_FV_MSK                             (0x00000001)
#define PP_PRSR_OUTPUT_SWAP_SWAP_FV_RST                             (0x1)

/**
 * SW_REG_NAME : PP_PRSR_PRE_LY2_TYPE0_1_REG
 * HW_REG_NAME : PRE_LY2_TYPE0_1
 * DESCRIPTION : Pre ly2 types
 *
 *  Register Fields :
 *   [31:16][RW] - Pre ly2 type 0
 *   [15: 0][RW] - Pre ly2 type 1
 *
 */
#define PP_PRSR_PRE_LY2_TYPE0_1_REG                     ((PRSR_BASE_ADDR) + 0x4)
#define PP_PRSR_PRE_LY2_TYPE0_1_TYPE_1_OFF                          (16)
#define PP_PRSR_PRE_LY2_TYPE0_1_TYPE_1_LEN                          (16)
#define PP_PRSR_PRE_LY2_TYPE0_1_TYPE_1_MSK                          (0xFFFF0000)
#define PP_PRSR_PRE_LY2_TYPE0_1_TYPE_1_RST                          (0x0)
#define PP_PRSR_PRE_LY2_TYPE0_1_TYPE_0_OFF                          (0)
#define PP_PRSR_PRE_LY2_TYPE0_1_TYPE_0_LEN                          (16)
#define PP_PRSR_PRE_LY2_TYPE0_1_TYPE_0_MSK                          (0x0000FFFF)
#define PP_PRSR_PRE_LY2_TYPE0_1_TYPE_0_RST                          (0x0)

/**
 * SW_REG_NAME : PP_PRSR_PRE_LY2_VALID0_1_REG
 * HW_REG_NAME : PRE_LY2_VALID0_1
 * DESCRIPTION : Is pre-ly2 valid
 *
 *  Register Fields :
 *   [31:10][RO] - Reserved
 *   [ 9: 8][RW] - Pre-LY2 number of pit-table entries
 *   [ 7: 2][RW] - Pre-LY2 pit-table index
 *   [ 1: 0][RW] - Pre-LY2 valid
 *
 */
#define PP_PRSR_PRE_LY2_VALID0_1_REG                    ((PRSR_BASE_ADDR) + 0x8)
#define PP_PRSR_PRE_LY2_VALID0_1_RSVD0_OFF                          (10)
#define PP_PRSR_PRE_LY2_VALID0_1_RSVD0_LEN                          (22)
#define PP_PRSR_PRE_LY2_VALID0_1_RSVD0_MSK                          (0xFFFFFC00)
#define PP_PRSR_PRE_LY2_VALID0_1_RSVD0_RST                          (0x0)
#define PP_PRSR_PRE_LY2_VALID0_1_PIT_CNT_OFF                        (8)
#define PP_PRSR_PRE_LY2_VALID0_1_PIT_CNT_LEN                        (2)
#define PP_PRSR_PRE_LY2_VALID0_1_PIT_CNT_MSK                        (0x00000300)
#define PP_PRSR_PRE_LY2_VALID0_1_PIT_CNT_RST                        (0x0)
#define PP_PRSR_PRE_LY2_VALID0_1_PIT_IDX_OFF                        (2)
#define PP_PRSR_PRE_LY2_VALID0_1_PIT_IDX_LEN                        (6)
#define PP_PRSR_PRE_LY2_VALID0_1_PIT_IDX_MSK                        (0x000000FC)
#define PP_PRSR_PRE_LY2_VALID0_1_PIT_IDX_RST                        (0x0)
#define PP_PRSR_PRE_LY2_VALID0_1_VALID_0_1_OFF                      (0)
#define PP_PRSR_PRE_LY2_VALID0_1_VALID_0_1_LEN                      (2)
#define PP_PRSR_PRE_LY2_VALID0_1_VALID_0_1_MSK                      (0x00000003)
#define PP_PRSR_PRE_LY2_VALID0_1_VALID_0_1_RST                      (0x0)

/**
 * SW_REG_NAME : PP_PRSR_MAC_CFG_REG
 * HW_REG_NAME : mac_cfg
 * DESCRIPTION : MAC configuration
 *
 *  Register Fields :
 *   [31:15][RO] - Reserved
 *   [14:13][RW] - Mac pit-table number of pit-table entries
 *   [12: 7][RW] - Mac pit-table index
 *   [ 6: 0][RO] - Mac Length
 *
 */
#define PP_PRSR_MAC_CFG_REG                            ((PRSR_BASE_ADDR) + 0x0C)
#define PP_PRSR_MAC_CFG_RSVD0_OFF                                   (15)
#define PP_PRSR_MAC_CFG_RSVD0_LEN                                   (17)
#define PP_PRSR_MAC_CFG_RSVD0_MSK                                   (0xFFFF8000)
#define PP_PRSR_MAC_CFG_RSVD0_RST                                   (0x0)
#define PP_PRSR_MAC_CFG_PIT_CNT_OFF                                 (13)
#define PP_PRSR_MAC_CFG_PIT_CNT_LEN                                 (2)
#define PP_PRSR_MAC_CFG_PIT_CNT_MSK                                 (0x00006000)
#define PP_PRSR_MAC_CFG_PIT_CNT_RST                                 (0x1)
#define PP_PRSR_MAC_CFG_PIT_IDX_OFF                                 (7)
#define PP_PRSR_MAC_CFG_PIT_IDX_LEN                                 (6)
#define PP_PRSR_MAC_CFG_PIT_IDX_MSK                                 (0x00001F80)
#define PP_PRSR_MAC_CFG_PIT_IDX_RST                                 (0x0)
#define PP_PRSR_MAC_CFG_MAC_LENGTH_OFF                              (0)
#define PP_PRSR_MAC_CFG_MAC_LENGTH_LEN                              (7)
#define PP_PRSR_MAC_CFG_MAC_LENGTH_MSK                              (0x0000007F)
#define PP_PRSR_MAC_CFG_MAC_LENGTH_RST                              (0xc)

/**
 * SW_REG_NAME : PP_PRSR_LY2_VALID_LEN_REG
 * HW_REG_NAME : LY2_VALID_LEN_PRIO_0_7
 * DESCRIPTION : TBD
 *
 *  Register Fields :
 *   [31:24][RO] - Reserved
 *   [23:16][RO] - Ly2 ethertype
 *   [15: 8][RW] - Ly2 type length
 *   [ 7: 0][RW] - Ly2 unit valid to compare
 *
 */
#define PP_PRSR_LY2_VALID_LEN_REG                      ((PRSR_BASE_ADDR) + 0x10)
#define PP_PRSR_LY2_VALID_LEN_RSVD0_OFF                             (24)
#define PP_PRSR_LY2_VALID_LEN_RSVD0_LEN                             (8)
#define PP_PRSR_LY2_VALID_LEN_RSVD0_MSK                             (0xFF000000)
#define PP_PRSR_LY2_VALID_LEN_RSVD0_RST                             (0x0)
#define PP_PRSR_LY2_VALID_LEN_LY2_IS_PRIO0_7_OFF                    (16)
#define PP_PRSR_LY2_VALID_LEN_LY2_IS_PRIO0_7_LEN                    (8)
#define PP_PRSR_LY2_VALID_LEN_LY2_IS_PRIO0_7_MSK                    (0x00FF0000)
#define PP_PRSR_LY2_VALID_LEN_LY2_IS_PRIO0_7_RST                    (0x0)
#define PP_PRSR_LY2_VALID_LEN_LY2_IS_LEN_OFF                        (8)
#define PP_PRSR_LY2_VALID_LEN_LY2_IS_LEN_LEN                        (8)
#define PP_PRSR_LY2_VALID_LEN_LY2_IS_LEN_MSK                        (0x0000FF00)
#define PP_PRSR_LY2_VALID_LEN_LY2_IS_LEN_RST                        (0x10)
#define PP_PRSR_LY2_VALID_LEN_LY2_IS_VALID_OFF                      (0)
#define PP_PRSR_LY2_VALID_LEN_LY2_IS_VALID_LEN                      (8)
#define PP_PRSR_LY2_VALID_LEN_LY2_IS_VALID_MSK                      (0x000000FF)
#define PP_PRSR_LY2_VALID_LEN_LY2_IS_VALID_RST                      (0x1f)

/**
 * SW_REG_NAME : PP_PRSR_LAST_ETHERTYPE_MAP_REG
 * HW_REG_NAME : last_ethertype_map
 * DESCRIPTION : Last etherType mapping
 *
 *  Register Fields :
 *   [31:29][RO] - Reserved
 *   [28:19][RW] - Fallback to UC according to ly2 ethertype that
 *                 were reached
 *   [18:13][RW] - PC for malformed packet - reached compare but nh
 *                 offset will be after EOF
 *   [12: 7][RW] - PC in case no reconized last eth-type is found
 *   [ 6: 6][RW] - The pit-table entries number for last ethertype
 *                 (ly3 unit under ly2)
 *   [ 5: 0][RW] - The pit-table index for last ethertype (ly3 unit
 *                 under ly2)
 *
 */
#define PP_PRSR_LAST_ETHERTYPE_MAP_REG                 ((PRSR_BASE_ADDR) + 0x14)
#define PP_PRSR_LAST_ETHERTYPE_MAP_RSVD0_OFF                        (29)
#define PP_PRSR_LAST_ETHERTYPE_MAP_RSVD0_LEN                        (3)
#define PP_PRSR_LAST_ETHERTYPE_MAP_RSVD0_MSK                        (0xE0000000)
#define PP_PRSR_LAST_ETHERTYPE_MAP_RSVD0_RST                        (0x0)
#define PP_PRSR_LAST_ETHERTYPE_MAP_LY2_FALLBACK_OFF                 (19)
#define PP_PRSR_LAST_ETHERTYPE_MAP_LY2_FALLBACK_LEN                 (10)
#define PP_PRSR_LAST_ETHERTYPE_MAP_LY2_FALLBACK_MSK                 (0x1FF80000)
#define PP_PRSR_LAST_ETHERTYPE_MAP_LY2_FALLBACK_RST                 (0x0)
#define PP_PRSR_LAST_ETHERTYPE_MAP_LY3_UNIT_CHOPPED_PC_OFF          (13)
#define PP_PRSR_LAST_ETHERTYPE_MAP_LY3_UNIT_CHOPPED_PC_LEN          (6)
#define PP_PRSR_LAST_ETHERTYPE_MAP_LY3_UNIT_CHOPPED_PC_MSK          (0x0007E000)
#define PP_PRSR_LAST_ETHERTYPE_MAP_LY3_UNIT_CHOPPED_PC_RST          (0x30)
#define PP_PRSR_LAST_ETHERTYPE_MAP_DEFAULT_PC_OFF                   (7)
#define PP_PRSR_LAST_ETHERTYPE_MAP_DEFAULT_PC_LEN                   (6)
#define PP_PRSR_LAST_ETHERTYPE_MAP_DEFAULT_PC_MSK                   (0x00001F80)
#define PP_PRSR_LAST_ETHERTYPE_MAP_DEFAULT_PC_RST                   (0x30)
#define PP_PRSR_LAST_ETHERTYPE_MAP_PIT_VLD_OFF                      (6)
#define PP_PRSR_LAST_ETHERTYPE_MAP_PIT_VLD_LEN                      (1)
#define PP_PRSR_LAST_ETHERTYPE_MAP_PIT_VLD_MSK                      (0x00000040)
#define PP_PRSR_LAST_ETHERTYPE_MAP_PIT_VLD_RST                      (0x1)
#define PP_PRSR_LAST_ETHERTYPE_MAP_PIT_IDX_OFF                      (0)
#define PP_PRSR_LAST_ETHERTYPE_MAP_PIT_IDX_LEN                      (6)
#define PP_PRSR_LAST_ETHERTYPE_MAP_PIT_IDX_MSK                      (0x0000003F)
#define PP_PRSR_LAST_ETHERTYPE_MAP_PIT_IDX_RST                      (0x3)

/**
 * SW_REG_NAME : PP_PRSR_FV_MASK_DIS_REG
 * HW_REG_NAME : fv_mask_disable
 * DESCRIPTION : Disable mask of FV
 *
 *  Register Fields :
 *   [31: 1][RO] - Reserved
 *   [ 0: 0][RW] - disable FV mask
 *
 */
#define PP_PRSR_FV_MASK_DIS_REG                        ((PRSR_BASE_ADDR) + 0x18)
#define PP_PRSR_FV_MASK_DIS_RSVD0_OFF                               (1)
#define PP_PRSR_FV_MASK_DIS_RSVD0_LEN                               (31)
#define PP_PRSR_FV_MASK_DIS_RSVD0_MSK                               (0xFFFFFFFE)
#define PP_PRSR_FV_MASK_DIS_RSVD0_RST                               (0x0)
#define PP_PRSR_FV_MASK_DIS_OFF                                     (0)
#define PP_PRSR_FV_MASK_DIS_LEN                                     (1)
#define PP_PRSR_FV_MASK_DIS_MSK                                     (0x00000001)
#define PP_PRSR_FV_MASK_DIS_RST                                     (0x0)

/**
 * SW_REG_NAME : PP_PRSR_FV_OOB_CFG_REG
 * HW_REG_NAME : fv_oob_cfg
 * DESCRIPTION : OOB configuration
 *
 *  Register Fields :
 *   [31:10][RO] - Reserved
 *   [ 9: 8][RW] - LRO flag rule, 0=Any, 1=Only if found in first
 *                 header, 2=Only if found in second header, 3=only
 *                 if found in last header
 *   [ 7: 2][RW] - classified index
 *   [ 1: 0][RW] - classified mask
 *
 */
#define PP_PRSR_FV_OOB_CFG_REG                         ((PRSR_BASE_ADDR) + 0x1C)
#define PP_PRSR_FV_OOB_CFG_RSVD0_OFF                                (10)
#define PP_PRSR_FV_OOB_CFG_RSVD0_LEN                                (22)
#define PP_PRSR_FV_OOB_CFG_RSVD0_MSK                                (0xFFFFFC00)
#define PP_PRSR_FV_OOB_CFG_RSVD0_RST                                (0x0)
#define PP_PRSR_FV_OOB_CFG_LRO_RULE_OFF                             (8)
#define PP_PRSR_FV_OOB_CFG_LRO_RULE_LEN                             (2)
#define PP_PRSR_FV_OOB_CFG_LRO_RULE_MSK                             (0x00000300)
#define PP_PRSR_FV_OOB_CFG_LRO_RULE_RST                             (0x0)
#define PP_PRSR_FV_OOB_CFG_CLASSIFIED_FV_IDX_OFF                    (2)
#define PP_PRSR_FV_OOB_CFG_CLASSIFIED_FV_IDX_LEN                    (6)
#define PP_PRSR_FV_OOB_CFG_CLASSIFIED_FV_IDX_MSK                    (0x000000FC)
#define PP_PRSR_FV_OOB_CFG_CLASSIFIED_FV_IDX_RST                    (0x0)
#define PP_PRSR_FV_OOB_CFG_CLASSIFIED_MASK_OFF                      (0)
#define PP_PRSR_FV_OOB_CFG_CLASSIFIED_MASK_LEN                      (2)
#define PP_PRSR_FV_OOB_CFG_CLASSIFIED_MASK_MSK                      (0x00000003)
#define PP_PRSR_FV_OOB_CFG_CLASSIFIED_MASK_RST                      (0x0)

/**
 * SW_REG_NAME : PP_PRSR_SET_PC_REG
 * HW_REG_NAME : set_PC
 * DESCRIPTION : Some specail PCs
 *
 *  Register Fields :
 *   [31:24][RO] - Reserved
 *   [23:18][RW] - PC when ana filter stop sends PC because the
 *                 second phase ends
 *   [17:12][RW] - PC when forced to stop analyzing
 *   [11: 6][RW] - NH-PC in case middle or last frag is detected
 *   [ 5: 0][RW] - Reset PC
 *
 */
#define PP_PRSR_SET_PC_REG                             ((PRSR_BASE_ADDR) + 0x20)
#define PP_PRSR_SET_PC_RSVD0_OFF                                    (24)
#define PP_PRSR_SET_PC_RSVD0_LEN                                    (8)
#define PP_PRSR_SET_PC_RSVD0_MSK                                    (0xFF000000)
#define PP_PRSR_SET_PC_RSVD0_RST                                    (0x0)
#define PP_PRSR_SET_PC_FILTER_PAYLOAD_PC_OFF                        (18)
#define PP_PRSR_SET_PC_FILTER_PAYLOAD_PC_LEN                        (6)
#define PP_PRSR_SET_PC_FILTER_PAYLOAD_PC_MSK                        (0x00FC0000)
#define PP_PRSR_SET_PC_FILTER_PAYLOAD_PC_RST                        (0x30)
#define PP_PRSR_SET_PC_ANA_STOP_ANALYZING_PC_OFF                    (12)
#define PP_PRSR_SET_PC_ANA_STOP_ANALYZING_PC_LEN                    (6)
#define PP_PRSR_SET_PC_ANA_STOP_ANALYZING_PC_MSK                    (0x0003F000)
#define PP_PRSR_SET_PC_ANA_STOP_ANALYZING_PC_RST                    (0x30)
#define PP_PRSR_SET_PC_LY3_FRAG_NH_PC_OFF                           (6)
#define PP_PRSR_SET_PC_LY3_FRAG_NH_PC_LEN                           (6)
#define PP_PRSR_SET_PC_LY3_FRAG_NH_PC_MSK                           (0x00000FC0)
#define PP_PRSR_SET_PC_LY3_FRAG_NH_PC_RST                           (0x30)
#define PP_PRSR_SET_PC_IDLE_PC_OFF                                  (0)
#define PP_PRSR_SET_PC_IDLE_PC_LEN                                  (6)
#define PP_PRSR_SET_PC_IDLE_PC_MSK                                  (0x0000003F)
#define PP_PRSR_SET_PC_IDLE_PC_RST                                  (0x31)

/**
 * SW_REG_NAME : PP_PRSR_RXA_REG
 * HW_REG_NAME : rxa_reg
 * DESCRIPTION : RXA Configure Register
 *
 *  Register Fields :
 *   [31:22][RO] - Reserved
 *   [21:15][RW] - max hdr lines, the max supported value is 64B, in
 *                 case the packet size is bigger than this field the
 *                 parser will read only the max_hdr_lines bytes
 *   [14: 5][RO] - Reserved
 *   [ 4: 4][RW] - arb l2 sp
 *   [ 3: 0][RW] - arb l1 sp bus
 *
 */
#define PP_PRSR_RXA_REG                                ((PRSR_BASE_ADDR) + 0x24)
#define PP_PRSR_RXA_RSVD0_OFF                                       (22)
#define PP_PRSR_RXA_RSVD0_LEN                                       (10)
#define PP_PRSR_RXA_RSVD0_MSK                                       (0xFFC00000)
#define PP_PRSR_RXA_RSVD0_RST                                       (0x0)
#define PP_PRSR_RXA_MAX_HDR_LINES_OFF                               (15)
#define PP_PRSR_RXA_MAX_HDR_LINES_LEN                               (7)
#define PP_PRSR_RXA_MAX_HDR_LINES_MSK                               (0x003F8000)
#define PP_PRSR_RXA_MAX_HDR_LINES_RST                               (0xa)
#define PP_PRSR_RXA_RSVD1_OFF                                       (5)
#define PP_PRSR_RXA_RSVD1_LEN                                       (10)
#define PP_PRSR_RXA_RSVD1_MSK                                       (0x00007FE0)
#define PP_PRSR_RXA_RSVD1_RST                                       (0x0)
#define PP_PRSR_RXA_ARB_L2_SP_OFF                                   (4)
#define PP_PRSR_RXA_ARB_L2_SP_LEN                                   (1)
#define PP_PRSR_RXA_ARB_L2_SP_MSK                                   (0x00000010)
#define PP_PRSR_RXA_ARB_L2_SP_RST                                   (0x0)
#define PP_PRSR_RXA_ARB_L1_SP_BUS_OFF                               (0)
#define PP_PRSR_RXA_ARB_L1_SP_BUS_LEN                               (4)
#define PP_PRSR_RXA_ARB_L1_SP_BUS_MSK                               (0x0000000F)
#define PP_PRSR_RXA_ARB_L1_SP_BUS_RST                               (0x0)

/**
 * SW_REG_NAME : PP_PRSR_LY3_CFG_REG
 * HW_REG_NAME : ly3_config
 * DESCRIPTION : Configuration for LY3
 *
 *  Register Fields :
 *   [31:31][RO] - Reserved
 *   [30:30][RW] - Whether to assert LRO exception bit if TCP PSH bit
 *                 != 0, NOTE, B Step ONLY field
 *   [29:25][RW] - Abort counter threshold
 *   [24:15][RW] - Maximum header length
 *   [14: 8][RW] - Abort counter threshold
 *   [ 7: 0][RW] - Maximum protocol length for each IMEM protocol
 *
 */
#define PP_PRSR_LY3_CFG_REG                            ((PRSR_BASE_ADDR) + 0x28)
#define PP_PRSR_LY3_CFG_RSVD0_OFF                                   (31)
#define PP_PRSR_LY3_CFG_RSVD0_LEN                                   (1)
#define PP_PRSR_LY3_CFG_RSVD0_MSK                                   (0x80000000)
#define PP_PRSR_LY3_CFG_RSVD0_RST                                   (0x0)
#define PP_PRSR_LY3_CFG_CHECK_TCP_PSH_LRO_OFF                       (30)
#define PP_PRSR_LY3_CFG_CHECK_TCP_PSH_LRO_LEN                       (1)
#define PP_PRSR_LY3_CFG_CHECK_TCP_PSH_LRO_MSK                       (0x40000000)
#define PP_PRSR_LY3_CFG_CHECK_TCP_PSH_LRO_RST                       (0x0)
#define PP_PRSR_LY3_CFG_EOF_CNT_THR_OFF                             (25)
#define PP_PRSR_LY3_CFG_EOF_CNT_THR_LEN                             (5)
#define PP_PRSR_LY3_CFG_EOF_CNT_THR_MSK                             (0x3E000000)
#define PP_PRSR_LY3_CFG_EOF_CNT_THR_RST                             (0x7)
#define PP_PRSR_LY3_CFG_MAX_HEADER_LEN_OFF                          (15)
#define PP_PRSR_LY3_CFG_MAX_HEADER_LEN_LEN                          (10)
#define PP_PRSR_LY3_CFG_MAX_HEADER_LEN_MSK                          (0x01FF8000)
#define PP_PRSR_LY3_CFG_MAX_HEADER_LEN_RST                          (0x100)
#define PP_PRSR_LY3_CFG_ABORT_CNT_THR_OFF                           (8)
#define PP_PRSR_LY3_CFG_ABORT_CNT_THR_LEN                           (7)
#define PP_PRSR_LY3_CFG_ABORT_CNT_THR_MSK                           (0x00007F00)
#define PP_PRSR_LY3_CFG_ABORT_CNT_THR_RST                           (0x2d)
#define PP_PRSR_LY3_CFG_MAX_PROT_LEN_OFF                            (0)
#define PP_PRSR_LY3_CFG_MAX_PROT_LEN_LEN                            (8)
#define PP_PRSR_LY3_CFG_MAX_PROT_LEN_MSK                            (0x000000FF)
#define PP_PRSR_LY3_CFG_MAX_PROT_LEN_RST                            (0xff)

/**
 * SW_REG_NAME : PP_PRSR_ABORT_CFG_REG
 * HW_REG_NAME : abort_config
 * DESCRIPTION : Configuration for LY3
 *
 *  Register Fields :
 *   [31:31][RW] - Whether to allow fallback to UC if error bit is
 *                 asserted
 *   [30:29][RO] - Reserved
 *   [28:28][RW] - Whether to drop packet if it recieved with discard
 *                 bit asserted in status
 *   [27:27][RW] - Whether to drop packet it recieved with error bit
 *                 asserted in status
 *   [26:26][RW] - Whether to drop packet if its length (in rpb
 *                 status) is zero
 *   [25:25][RW] - Whether to drop if no last eth-type was discovered
 *   [24:24][RW] - Whether to drop if ly3 protocol is bigger than its
 *                 max length or smaller than its min length
 *   [23:23][RW] - Whether to drop packet if that ly3 header is
 *                 longer than configurable length
 *   [22:22][RW] - Whether to drop packet if max header length for
 *                 each IMEM protocol
 *   [21:21][RW] - Whether to drop packet if the packet length in the
 *                 ipv4/ipv6 protocols is identical to the packet
 *                 size inserted to Parser by RPB, if it's asserted
 *                 and the checker failed the packet is treated as
 *                 aborted packet
 *   [20:20][RW] - Whether to drop packet if ly3 doesn't exceed after
 *                 certein time from ly3 start
 *   [19:19][RW] - Whether to drop packet if payload wasn't
 *                 identified after certein time from ly3 start to
 *                 eop
 *   [18:18][RW] - Whether to assert error-bit if it recieved with
 *                 discard bit asserted in status
 *   [17:17][RW] - Whether to assert errror-bit if it recieved with
 *                 error bit asserted in status
 *   [16:16][RW] - Whether to assert error bit if its length (in rpb
 *                 status) is zero
 *   [15:15][RW] - Whether to assert error-bit if no last eth-type
 *                 was discovered
 *   [14:14][RW] - Whether to assert error-bit if ly3 protocol is
 *                 bigger than its max length or smaller than its min
 *                 length
 *   [13:13][RW] - Whether to assert error-bit if ly3 header is
 *                 longer than configurable length
 *   [12:12][RW] - Whether to assert error-bit if max header length
 *                 for each IMEM protocol
 *   [11:11][RW] - Whether to assert error-bit if the packet length
 *                 in the ipv4/ipv6 protocols is identical to the
 *                 packet size inserted to Parser by RPB, if it's
 *                 asserted and the checker failed the packet is
 *                 treated as aborted packet
 *   [10:10][RW] - Whether to assert error-bit if ly3 doesn't exceed
 *                 after certein time from ly3 start
 *   [ 9: 9][RW] - Whether to assert error-bit if payload wasn't
 *                 identified after certein time from ly3 start to
 *                 eop
 *   [ 8: 6][RO] - Reserved
 *   [ 5: 5][RW] - Whether to abort parsing if no last eth-type was
 *                 discovered
 *   [ 4: 4][RW] - Whether to abort parsing if ly3 header is longer
 *                 than configurable length
 *   [ 3: 3][RW] - Whether to abort parsing if IMEM protocol overflow
 *   [ 2: 2][RW] - Whether to abort parsing if the packet length in
 *                 the ipv4/ipv6 protocols is identical to the packet
 *                 size inserted to Parser by RPB, if it's asserted
 *                 and the checker failed the packet is treated as
 *                 aborted packet
 *   [ 1: 1][RW] - Whether to abort parsing if ly3 doesn't end after
 *                 certein time from ly3 start
 *   [ 0: 0][RW] - Whether to abort parsing if payload wasn't
 *                 identified after certein time from eop
 *
 */
#define PP_PRSR_ABORT_CFG_REG                          ((PRSR_BASE_ADDR) + 0x2C)
#define PP_PRSR_ABORT_CFG_EN_ERR_FALLBACK_OFF                       (31)
#define PP_PRSR_ABORT_CFG_EN_ERR_FALLBACK_LEN                       (1)
#define PP_PRSR_ABORT_CFG_EN_ERR_FALLBACK_MSK                       (0x80000000)
#define PP_PRSR_ABORT_CFG_EN_ERR_FALLBACK_RST                       (0x0)
#define PP_PRSR_ABORT_CFG_RSVD0_OFF                                 (29)
#define PP_PRSR_ABORT_CFG_RSVD0_LEN                                 (2)
#define PP_PRSR_ABORT_CFG_RSVD0_MSK                                 (0x60000000)
#define PP_PRSR_ABORT_CFG_RSVD0_RST                                 (0x0)
#define PP_PRSR_ABORT_CFG_DROP_RPB_DISCARD_OFF                      (28)
#define PP_PRSR_ABORT_CFG_DROP_RPB_DISCARD_LEN                      (1)
#define PP_PRSR_ABORT_CFG_DROP_RPB_DISCARD_MSK                      (0x10000000)
#define PP_PRSR_ABORT_CFG_DROP_RPB_DISCARD_RST                      (0x1)
#define PP_PRSR_ABORT_CFG_DROP_RPB_ERROR_OFF                        (27)
#define PP_PRSR_ABORT_CFG_DROP_RPB_ERROR_LEN                        (1)
#define PP_PRSR_ABORT_CFG_DROP_RPB_ERROR_MSK                        (0x08000000)
#define PP_PRSR_ABORT_CFG_DROP_RPB_ERROR_RST                        (0x0)
#define PP_PRSR_ABORT_CFG_DROP_ZERO_LEN_OFF                         (26)
#define PP_PRSR_ABORT_CFG_DROP_ZERO_LEN_LEN                         (1)
#define PP_PRSR_ABORT_CFG_DROP_ZERO_LEN_MSK                         (0x04000000)
#define PP_PRSR_ABORT_CFG_DROP_ZERO_LEN_RST                         (0x1)
#define PP_PRSR_ABORT_CFG_DROP_NO_LAST_ETH_TYPE_ERR_OFF             (25)
#define PP_PRSR_ABORT_CFG_DROP_NO_LAST_ETH_TYPE_ERR_LEN             (1)
#define PP_PRSR_ABORT_CFG_DROP_NO_LAST_ETH_TYPE_ERR_MSK             (0x02000000)
#define PP_PRSR_ABORT_CFG_DROP_NO_LAST_ETH_TYPE_ERR_RST             (0x1)
#define PP_PRSR_ABORT_CFG_DROP_PROTO_LENGTH_ERR_OFF                 (24)
#define PP_PRSR_ABORT_CFG_DROP_PROTO_LENGTH_ERR_LEN                 (1)
#define PP_PRSR_ABORT_CFG_DROP_PROTO_LENGTH_ERR_MSK                 (0x01000000)
#define PP_PRSR_ABORT_CFG_DROP_PROTO_LENGTH_ERR_RST                 (0x1)
#define PP_PRSR_ABORT_CFG_DROP_MAX_HEADER_LEN_ERR_OFF               (23)
#define PP_PRSR_ABORT_CFG_DROP_MAX_HEADER_LEN_ERR_LEN               (1)
#define PP_PRSR_ABORT_CFG_DROP_MAX_HEADER_LEN_ERR_MSK               (0x00800000)
#define PP_PRSR_ABORT_CFG_DROP_MAX_HEADER_LEN_ERR_RST               (0x1)
#define PP_PRSR_ABORT_CFG_DROP_PROTO_OVERFLOW_ERR_OFF               (22)
#define PP_PRSR_ABORT_CFG_DROP_PROTO_OVERFLOW_ERR_LEN               (1)
#define PP_PRSR_ABORT_CFG_DROP_PROTO_OVERFLOW_ERR_MSK               (0x00400000)
#define PP_PRSR_ABORT_CFG_DROP_PROTO_OVERFLOW_ERR_RST               (0x1)
#define PP_PRSR_ABORT_CFG_DROP_IP_LEN_ERR_OFF                       (21)
#define PP_PRSR_ABORT_CFG_DROP_IP_LEN_ERR_LEN                       (1)
#define PP_PRSR_ABORT_CFG_DROP_IP_LEN_ERR_MSK                       (0x00200000)
#define PP_PRSR_ABORT_CFG_DROP_IP_LEN_ERR_RST                       (0x0)
#define PP_PRSR_ABORT_CFG_DROP_ABORT_CNT_ERR_OFF                    (20)
#define PP_PRSR_ABORT_CFG_DROP_ABORT_CNT_ERR_LEN                    (1)
#define PP_PRSR_ABORT_CFG_DROP_ABORT_CNT_ERR_MSK                    (0x00100000)
#define PP_PRSR_ABORT_CFG_DROP_ABORT_CNT_ERR_RST                    (0x1)
#define PP_PRSR_ABORT_CFG_DROP_EOF_NO_PAYLOAD_ERR_OFF               (19)
#define PP_PRSR_ABORT_CFG_DROP_EOF_NO_PAYLOAD_ERR_LEN               (1)
#define PP_PRSR_ABORT_CFG_DROP_EOF_NO_PAYLOAD_ERR_MSK               (0x00080000)
#define PP_PRSR_ABORT_CFG_DROP_EOF_NO_PAYLOAD_ERR_RST               (0x1)
#define PP_PRSR_ABORT_CFG_ERROR_RPB_DISCARD_OFF                     (18)
#define PP_PRSR_ABORT_CFG_ERROR_RPB_DISCARD_LEN                     (1)
#define PP_PRSR_ABORT_CFG_ERROR_RPB_DISCARD_MSK                     (0x00040000)
#define PP_PRSR_ABORT_CFG_ERROR_RPB_DISCARD_RST                     (0x1)
#define PP_PRSR_ABORT_CFG_ERROR_RPB_ERROR_OFF                       (17)
#define PP_PRSR_ABORT_CFG_ERROR_RPB_ERROR_LEN                       (1)
#define PP_PRSR_ABORT_CFG_ERROR_RPB_ERROR_MSK                       (0x00020000)
#define PP_PRSR_ABORT_CFG_ERROR_RPB_ERROR_RST                       (0x1)
#define PP_PRSR_ABORT_CFG_ERROR_ZERO_LEN_OFF                        (16)
#define PP_PRSR_ABORT_CFG_ERROR_ZERO_LEN_LEN                        (1)
#define PP_PRSR_ABORT_CFG_ERROR_ZERO_LEN_MSK                        (0x00010000)
#define PP_PRSR_ABORT_CFG_ERROR_ZERO_LEN_RST                        (0x1)
#define PP_PRSR_ABORT_CFG_ERROR_NO_LAST_ETH_TYPE_ERR_OFF            (15)
#define PP_PRSR_ABORT_CFG_ERROR_NO_LAST_ETH_TYPE_ERR_LEN            (1)
#define PP_PRSR_ABORT_CFG_ERROR_NO_LAST_ETH_TYPE_ERR_MSK            (0x00008000)
#define PP_PRSR_ABORT_CFG_ERROR_NO_LAST_ETH_TYPE_ERR_RST            (0x1)
#define PP_PRSR_ABORT_CFG_ERROR_PROTO_LENGTH_ERR_OFF                (14)
#define PP_PRSR_ABORT_CFG_ERROR_PROTO_LENGTH_ERR_LEN                (1)
#define PP_PRSR_ABORT_CFG_ERROR_PROTO_LENGTH_ERR_MSK                (0x00004000)
#define PP_PRSR_ABORT_CFG_ERROR_PROTO_LENGTH_ERR_RST                (0x1)
#define PP_PRSR_ABORT_CFG_ERROR_MAX_HEADER_LEN_ERR_OFF              (13)
#define PP_PRSR_ABORT_CFG_ERROR_MAX_HEADER_LEN_ERR_LEN              (1)
#define PP_PRSR_ABORT_CFG_ERROR_MAX_HEADER_LEN_ERR_MSK              (0x00002000)
#define PP_PRSR_ABORT_CFG_ERROR_MAX_HEADER_LEN_ERR_RST              (0x1)
#define PP_PRSR_ABORT_CFG_ERROR_PROTO_OVERFLOW_ERR_OFF              (12)
#define PP_PRSR_ABORT_CFG_ERROR_PROTO_OVERFLOW_ERR_LEN              (1)
#define PP_PRSR_ABORT_CFG_ERROR_PROTO_OVERFLOW_ERR_MSK              (0x00001000)
#define PP_PRSR_ABORT_CFG_ERROR_PROTO_OVERFLOW_ERR_RST              (0x1)
#define PP_PRSR_ABORT_CFG_ERROR_IP_LEN_ERR_OFF                      (11)
#define PP_PRSR_ABORT_CFG_ERROR_IP_LEN_ERR_LEN                      (1)
#define PP_PRSR_ABORT_CFG_ERROR_IP_LEN_ERR_MSK                      (0x00000800)
#define PP_PRSR_ABORT_CFG_ERROR_IP_LEN_ERR_RST                      (0x1)
#define PP_PRSR_ABORT_CFG_ERROR_ABORT_CNT_ERR_OFF                   (10)
#define PP_PRSR_ABORT_CFG_ERROR_ABORT_CNT_ERR_LEN                   (1)
#define PP_PRSR_ABORT_CFG_ERROR_ABORT_CNT_ERR_MSK                   (0x00000400)
#define PP_PRSR_ABORT_CFG_ERROR_ABORT_CNT_ERR_RST                   (0x1)
#define PP_PRSR_ABORT_CFG_ERROR_EOF_NO_PAYLOAD_ERR_OFF              (9)
#define PP_PRSR_ABORT_CFG_ERROR_EOF_NO_PAYLOAD_ERR_LEN              (1)
#define PP_PRSR_ABORT_CFG_ERROR_EOF_NO_PAYLOAD_ERR_MSK              (0x00000200)
#define PP_PRSR_ABORT_CFG_ERROR_EOF_NO_PAYLOAD_ERR_RST              (0x1)
#define PP_PRSR_ABORT_CFG_RSVD1_OFF                                 (6)
#define PP_PRSR_ABORT_CFG_RSVD1_LEN                                 (3)
#define PP_PRSR_ABORT_CFG_RSVD1_MSK                                 (0x000001C0)
#define PP_PRSR_ABORT_CFG_RSVD1_RST                                 (0x0)
#define PP_PRSR_ABORT_CFG_NO_LAST_ETH_TYPE_ABORT_EN_OFF             (5)
#define PP_PRSR_ABORT_CFG_NO_LAST_ETH_TYPE_ABORT_EN_LEN             (1)
#define PP_PRSR_ABORT_CFG_NO_LAST_ETH_TYPE_ABORT_EN_MSK             (0x00000020)
#define PP_PRSR_ABORT_CFG_NO_LAST_ETH_TYPE_ABORT_EN_RST             (0x1)
#define PP_PRSR_ABORT_CFG_MAX_HEADER_LEN_ABORT_EN_OFF               (4)
#define PP_PRSR_ABORT_CFG_MAX_HEADER_LEN_ABORT_EN_LEN               (1)
#define PP_PRSR_ABORT_CFG_MAX_HEADER_LEN_ABORT_EN_MSK               (0x00000010)
#define PP_PRSR_ABORT_CFG_MAX_HEADER_LEN_ABORT_EN_RST               (0x1)
#define PP_PRSR_ABORT_CFG_PROTO_OVERFLOW_ABORT_EN_OFF               (3)
#define PP_PRSR_ABORT_CFG_PROTO_OVERFLOW_ABORT_EN_LEN               (1)
#define PP_PRSR_ABORT_CFG_PROTO_OVERFLOW_ABORT_EN_MSK               (0x00000008)
#define PP_PRSR_ABORT_CFG_PROTO_OVERFLOW_ABORT_EN_RST               (0x1)
#define PP_PRSR_ABORT_CFG_IP_LEN_ABORT_EN_OFF                       (2)
#define PP_PRSR_ABORT_CFG_IP_LEN_ABORT_EN_LEN                       (1)
#define PP_PRSR_ABORT_CFG_IP_LEN_ABORT_EN_MSK                       (0x00000004)
#define PP_PRSR_ABORT_CFG_IP_LEN_ABORT_EN_RST                       (0x1)
#define PP_PRSR_ABORT_CFG_ABORT_CNT_ABORT_EN_OFF                    (1)
#define PP_PRSR_ABORT_CFG_ABORT_CNT_ABORT_EN_LEN                    (1)
#define PP_PRSR_ABORT_CFG_ABORT_CNT_ABORT_EN_MSK                    (0x00000002)
#define PP_PRSR_ABORT_CFG_ABORT_CNT_ABORT_EN_RST                    (0x1)
#define PP_PRSR_ABORT_CFG_EOF_NO_PAYLOAD_ABORT_EN_OFF               (0)
#define PP_PRSR_ABORT_CFG_EOF_NO_PAYLOAD_ABORT_EN_LEN               (1)
#define PP_PRSR_ABORT_CFG_EOF_NO_PAYLOAD_ABORT_EN_MSK               (0x00000001)
#define PP_PRSR_ABORT_CFG_EOF_NO_PAYLOAD_ABORT_EN_RST               (0x1)

/**
 * SW_REG_NAME : PP_PRSR_OOB_FV_HACK_REC_VAL2_REG
 * HW_REG_NAME : oob_fv_hack_rec_val2
 * DESCRIPTION : the FV hack recipe control register
 *               port_number[7:0] + command[9:8] - write = 0x2,
 *               read = 0x1
 *
 *  Register Fields :
 *   [31:11][RW] - reserved
 *   [10:10][RW] - the FV hack recipe control register read valid
 *   [ 9: 8][RW] - the FV hack recipe control register command: write
 *                 = 0x2, read = 0x1
 *   [ 7: 0][RW] - the FV hack recipe control register port_number
 *                 range 0-255
 *
 */
#define PP_PRSR_OOB_FV_HACK_REC_VAL2_REG               ((PRSR_BASE_ADDR) + 0x30)
#define PP_PRSR_OOB_FV_HACK_REC_VAL2_FV_HACK_RSRVD_OFF              (11)
#define PP_PRSR_OOB_FV_HACK_REC_VAL2_FV_HACK_RSRVD_LEN              (21)
#define PP_PRSR_OOB_FV_HACK_REC_VAL2_FV_HACK_RSRVD_MSK              (0xFFFFF800)
#define PP_PRSR_OOB_FV_HACK_REC_VAL2_FV_HACK_RSRVD_RST              (0x0)
#define PP_PRSR_OOB_FV_HACK_REC_VAL2_FV_HACK_RD_STAT_OFF            (10)
#define PP_PRSR_OOB_FV_HACK_REC_VAL2_FV_HACK_RD_STAT_LEN            (1)
#define PP_PRSR_OOB_FV_HACK_REC_VAL2_FV_HACK_RD_STAT_MSK            (0x00000400)
#define PP_PRSR_OOB_FV_HACK_REC_VAL2_FV_HACK_RD_STAT_RST            (0x0)
#define PP_PRSR_OOB_FV_HACK_REC_VAL2_FV_HACK_CMD_OFF                (8)
#define PP_PRSR_OOB_FV_HACK_REC_VAL2_FV_HACK_CMD_LEN                (2)
#define PP_PRSR_OOB_FV_HACK_REC_VAL2_FV_HACK_CMD_MSK                (0x00000300)
#define PP_PRSR_OOB_FV_HACK_REC_VAL2_FV_HACK_CMD_RST                (0x0)
#define PP_PRSR_OOB_FV_HACK_REC_VAL2_FV_HACK_PORT_NUM_OFF           (0)
#define PP_PRSR_OOB_FV_HACK_REC_VAL2_FV_HACK_PORT_NUM_LEN           (8)
#define PP_PRSR_OOB_FV_HACK_REC_VAL2_FV_HACK_PORT_NUM_MSK           (0x000000FF)
#define PP_PRSR_OOB_FV_HACK_REC_VAL2_FV_HACK_PORT_NUM_RST           (0x0)

/**
 * SW_REG_NAME : PP_PRSR_OOB_FV_HACK_REC_VAL0_REG
 * HW_REG_NAME : oob_fv_hack_rec_val0
 * DESCRIPTION : the FV hack recipe register 0
 *
 *  Register Fields :
 *   [31:28][RW] - out of band fv copy length
 *   [27:23][RW] - out of band fv fields target bit pointer
 *   [22:16][RW] - out of band fv status source bit pointer
 *   [15:12][RW] - out of band fv copy length
 *   [11: 7][RW] - out of band fv fields target bit pointer
 *   [ 6: 0][RW] - out of band fv status source bit pointer
 *
 */
#define PP_PRSR_OOB_FV_HACK_REC_VAL0_REG               ((PRSR_BASE_ADDR) + 0x34)
#define PP_PRSR_OOB_FV_HACK_REC_VAL0_HACK_LEN_1_OFF                 (28)
#define PP_PRSR_OOB_FV_HACK_REC_VAL0_HACK_LEN_1_LEN                 (4)
#define PP_PRSR_OOB_FV_HACK_REC_VAL0_HACK_LEN_1_MSK                 (0xF0000000)
#define PP_PRSR_OOB_FV_HACK_REC_VAL0_HACK_LEN_1_RST                 (0x0)
#define PP_PRSR_OOB_FV_HACK_REC_VAL0_HACK_FIELDS_PTR_1_OFF          (23)
#define PP_PRSR_OOB_FV_HACK_REC_VAL0_HACK_FIELDS_PTR_1_LEN          (5)
#define PP_PRSR_OOB_FV_HACK_REC_VAL0_HACK_FIELDS_PTR_1_MSK          (0x0F800000)
#define PP_PRSR_OOB_FV_HACK_REC_VAL0_HACK_FIELDS_PTR_1_RST          (0x0)
#define PP_PRSR_OOB_FV_HACK_REC_VAL0_HACK_STATUS_PTR_1_OFF          (16)
#define PP_PRSR_OOB_FV_HACK_REC_VAL0_HACK_STATUS_PTR_1_LEN          (7)
#define PP_PRSR_OOB_FV_HACK_REC_VAL0_HACK_STATUS_PTR_1_MSK          (0x007F0000)
#define PP_PRSR_OOB_FV_HACK_REC_VAL0_HACK_STATUS_PTR_1_RST          (0x0)
#define PP_PRSR_OOB_FV_HACK_REC_VAL0_HACK_LEN_0_OFF                 (12)
#define PP_PRSR_OOB_FV_HACK_REC_VAL0_HACK_LEN_0_LEN                 (4)
#define PP_PRSR_OOB_FV_HACK_REC_VAL0_HACK_LEN_0_MSK                 (0x0000F000)
#define PP_PRSR_OOB_FV_HACK_REC_VAL0_HACK_LEN_0_RST                 (0x0)
#define PP_PRSR_OOB_FV_HACK_REC_VAL0_HACK_FIELDS_PTR_0_OFF          (7)
#define PP_PRSR_OOB_FV_HACK_REC_VAL0_HACK_FIELDS_PTR_0_LEN          (5)
#define PP_PRSR_OOB_FV_HACK_REC_VAL0_HACK_FIELDS_PTR_0_MSK          (0x00000F80)
#define PP_PRSR_OOB_FV_HACK_REC_VAL0_HACK_FIELDS_PTR_0_RST          (0x0)
#define PP_PRSR_OOB_FV_HACK_REC_VAL0_HACK_STATUS_PTR_0_OFF          (0)
#define PP_PRSR_OOB_FV_HACK_REC_VAL0_HACK_STATUS_PTR_0_LEN          (7)
#define PP_PRSR_OOB_FV_HACK_REC_VAL0_HACK_STATUS_PTR_0_MSK          (0x0000007F)
#define PP_PRSR_OOB_FV_HACK_REC_VAL0_HACK_STATUS_PTR_0_RST          (0x0)

/**
 * SW_REG_NAME : PP_PRSR_OOB_FV_HACK_REC_VAL1_REG
 * HW_REG_NAME : oob_fv_hack_rec_val1
 * DESCRIPTION : the FV hack recipe register 1
 *
 *  Register Fields :
 *   [31:28][RW] - out of band fv copy length
 *   [27:23][RW] - out of band fv fields target bit pointer
 *   [22:16][RW] - out of band fv status source bit pointer
 *   [15:12][RW] - out of band fv copy length
 *   [11: 7][RW] - out of band fv fields target bit pointer
 *   [ 6: 0][RW] - out of band fv status source bit pointer
 *
 */
#define PP_PRSR_OOB_FV_HACK_REC_VAL1_REG               ((PRSR_BASE_ADDR) + 0x38)
#define PP_PRSR_OOB_FV_HACK_REC_VAL1_HACK_LEN_3_OFF                 (28)
#define PP_PRSR_OOB_FV_HACK_REC_VAL1_HACK_LEN_3_LEN                 (4)
#define PP_PRSR_OOB_FV_HACK_REC_VAL1_HACK_LEN_3_MSK                 (0xF0000000)
#define PP_PRSR_OOB_FV_HACK_REC_VAL1_HACK_LEN_3_RST                 (0x0)
#define PP_PRSR_OOB_FV_HACK_REC_VAL1_HACK_FIELDS_PTR_3_OFF          (23)
#define PP_PRSR_OOB_FV_HACK_REC_VAL1_HACK_FIELDS_PTR_3_LEN          (5)
#define PP_PRSR_OOB_FV_HACK_REC_VAL1_HACK_FIELDS_PTR_3_MSK          (0x0F800000)
#define PP_PRSR_OOB_FV_HACK_REC_VAL1_HACK_FIELDS_PTR_3_RST          (0x0)
#define PP_PRSR_OOB_FV_HACK_REC_VAL1_HACK_STATUS_PTR_3_OFF          (16)
#define PP_PRSR_OOB_FV_HACK_REC_VAL1_HACK_STATUS_PTR_3_LEN          (7)
#define PP_PRSR_OOB_FV_HACK_REC_VAL1_HACK_STATUS_PTR_3_MSK          (0x007F0000)
#define PP_PRSR_OOB_FV_HACK_REC_VAL1_HACK_STATUS_PTR_3_RST          (0x0)
#define PP_PRSR_OOB_FV_HACK_REC_VAL1_HACK_LEN_2_OFF                 (12)
#define PP_PRSR_OOB_FV_HACK_REC_VAL1_HACK_LEN_2_LEN                 (4)
#define PP_PRSR_OOB_FV_HACK_REC_VAL1_HACK_LEN_2_MSK                 (0x0000F000)
#define PP_PRSR_OOB_FV_HACK_REC_VAL1_HACK_LEN_2_RST                 (0x0)
#define PP_PRSR_OOB_FV_HACK_REC_VAL1_HACK_FIELDS_PTR_2_OFF          (7)
#define PP_PRSR_OOB_FV_HACK_REC_VAL1_HACK_FIELDS_PTR_2_LEN          (5)
#define PP_PRSR_OOB_FV_HACK_REC_VAL1_HACK_FIELDS_PTR_2_MSK          (0x00000F80)
#define PP_PRSR_OOB_FV_HACK_REC_VAL1_HACK_FIELDS_PTR_2_RST          (0x0)
#define PP_PRSR_OOB_FV_HACK_REC_VAL1_HACK_STATUS_PTR_2_OFF          (0)
#define PP_PRSR_OOB_FV_HACK_REC_VAL1_HACK_STATUS_PTR_2_LEN          (7)
#define PP_PRSR_OOB_FV_HACK_REC_VAL1_HACK_STATUS_PTR_2_MSK          (0x0000007F)
#define PP_PRSR_OOB_FV_HACK_REC_VAL1_HACK_STATUS_PTR_2_RST          (0x0)

/**
 * SW_REG_NAME : PP_PRSR_MIN_IP_LEN_CHECK_REG
 * HW_REG_NAME : min_ip_len_check
 * DESCRIPTION : Minimum value of paket size to compare with
 *               ip_len, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31:14][RO] - Reserved
 *   [13: 0][RW] - Minimum value of paket size to compare with ip_len
 *
 */
#define PP_PRSR_MIN_IP_LEN_CHECK_REG                   ((PRSR_BASE_ADDR) + 0x3C)
#define PP_PRSR_MIN_IP_LEN_CHECK_RSVD0_OFF                          (14)
#define PP_PRSR_MIN_IP_LEN_CHECK_RSVD0_LEN                          (18)
#define PP_PRSR_MIN_IP_LEN_CHECK_RSVD0_MSK                          (0xFFFFC000)
#define PP_PRSR_MIN_IP_LEN_CHECK_RSVD0_RST                          (0x0)
#define PP_PRSR_MIN_IP_LEN_CHECK_MIN_CHK_OFF                        (0)
#define PP_PRSR_MIN_IP_LEN_CHECK_MIN_CHK_LEN                        (14)
#define PP_PRSR_MIN_IP_LEN_CHECK_MIN_CHK_MSK                        (0x00003FFF)
#define PP_PRSR_MIN_IP_LEN_CHECK_MIN_CHK_RST                        (0x3d)

/**
 * SW_REG_NAME : PP_PRSR_STATUS_USEDW_REG
 * HW_REG_NAME : status_usedw
 * DESCRIPTION : Output FIFOs usedw, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31:30][RO] - Reserved
 *   [29:24][RO] - ana   FIFO usedw
 *   [23:22][RO] - Reserved
 *   [21:16][RO] - oob   FIFO usedw
 *   [15:14][RO] - Reserved
 *   [13: 8][RO] - stat  FIFO usedw
 *   [ 7: 6][RO] - Reserved
 *   [ 5: 0][RO] - empty FIFO usedw
 *
 */
#define PP_PRSR_STATUS_USEDW_REG                       ((PRSR_BASE_ADDR) + 0x40)
#define PP_PRSR_STATUS_USEDW_RSVD0_OFF                              (30)
#define PP_PRSR_STATUS_USEDW_RSVD0_LEN                              (2)
#define PP_PRSR_STATUS_USEDW_RSVD0_MSK                              (0xC0000000)
#define PP_PRSR_STATUS_USEDW_RSVD0_RST                              (0x0)
#define PP_PRSR_STATUS_USEDW_ANA_USEDW_OFF                          (24)
#define PP_PRSR_STATUS_USEDW_ANA_USEDW_LEN                          (6)
#define PP_PRSR_STATUS_USEDW_ANA_USEDW_MSK                          (0x3F000000)
#define PP_PRSR_STATUS_USEDW_ANA_USEDW_RST                          (0x0)
#define PP_PRSR_STATUS_USEDW_RSVD1_OFF                              (22)
#define PP_PRSR_STATUS_USEDW_RSVD1_LEN                              (2)
#define PP_PRSR_STATUS_USEDW_RSVD1_MSK                              (0x00C00000)
#define PP_PRSR_STATUS_USEDW_RSVD1_RST                              (0x0)
#define PP_PRSR_STATUS_USEDW_OOB_USEDW_OFF                          (16)
#define PP_PRSR_STATUS_USEDW_OOB_USEDW_LEN                          (6)
#define PP_PRSR_STATUS_USEDW_OOB_USEDW_MSK                          (0x003F0000)
#define PP_PRSR_STATUS_USEDW_OOB_USEDW_RST                          (0x0)
#define PP_PRSR_STATUS_USEDW_RSVD2_OFF                              (14)
#define PP_PRSR_STATUS_USEDW_RSVD2_LEN                              (2)
#define PP_PRSR_STATUS_USEDW_RSVD2_MSK                              (0x0000C000)
#define PP_PRSR_STATUS_USEDW_RSVD2_RST                              (0x0)
#define PP_PRSR_STATUS_USEDW_STAT_USEDW_OFF                         (8)
#define PP_PRSR_STATUS_USEDW_STAT_USEDW_LEN                         (6)
#define PP_PRSR_STATUS_USEDW_STAT_USEDW_MSK                         (0x00003F00)
#define PP_PRSR_STATUS_USEDW_STAT_USEDW_RST                         (0x0)
#define PP_PRSR_STATUS_USEDW_RSVD3_OFF                              (6)
#define PP_PRSR_STATUS_USEDW_RSVD3_LEN                              (2)
#define PP_PRSR_STATUS_USEDW_RSVD3_MSK                              (0x000000C0)
#define PP_PRSR_STATUS_USEDW_RSVD3_RST                              (0x0)
#define PP_PRSR_STATUS_USEDW_EMPTY_USEDW_OFF                        (0)
#define PP_PRSR_STATUS_USEDW_EMPTY_USEDW_LEN                        (6)
#define PP_PRSR_STATUS_USEDW_EMPTY_USEDW_MSK                        (0x0000003F)
#define PP_PRSR_STATUS_USEDW_EMPTY_USEDW_RST                        (0x0)

/**
 * SW_REG_NAME : PP_PRSR_DATA_USEDW_REG
 * HW_REG_NAME : data_usedw
 * DESCRIPTION : Output FIFOs usedw, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31:26][RO] - Reserved
 *   [25:24][RO] - output FIFO usedw
 *   [23:21][RO] - Reserved
 *   [20:16][RO] - field1 FIFO usedw
 *   [15:13][RO] - Reserved
 *   [12: 8][RO] - field0 FIFO usedw
 *   [ 7: 5][RO] - Reserved
 *   [ 4: 0][RO] - mask   FIFO usedw
 *
 */
#define PP_PRSR_DATA_USEDW_REG                         ((PRSR_BASE_ADDR) + 0x44)
#define PP_PRSR_DATA_USEDW_RSVD0_OFF                                (26)
#define PP_PRSR_DATA_USEDW_RSVD0_LEN                                (6)
#define PP_PRSR_DATA_USEDW_RSVD0_MSK                                (0xFC000000)
#define PP_PRSR_DATA_USEDW_RSVD0_RST                                (0x0)
#define PP_PRSR_DATA_USEDW_OPF_USEDW_OFF                            (24)
#define PP_PRSR_DATA_USEDW_OPF_USEDW_LEN                            (2)
#define PP_PRSR_DATA_USEDW_OPF_USEDW_MSK                            (0x03000000)
#define PP_PRSR_DATA_USEDW_OPF_USEDW_RST                            (0x0)
#define PP_PRSR_DATA_USEDW_RSVD1_OFF                                (21)
#define PP_PRSR_DATA_USEDW_RSVD1_LEN                                (3)
#define PP_PRSR_DATA_USEDW_RSVD1_MSK                                (0x00E00000)
#define PP_PRSR_DATA_USEDW_RSVD1_RST                                (0x0)
#define PP_PRSR_DATA_USEDW_FIELD1_USEDW_OFF                         (16)
#define PP_PRSR_DATA_USEDW_FIELD1_USEDW_LEN                         (5)
#define PP_PRSR_DATA_USEDW_FIELD1_USEDW_MSK                         (0x001F0000)
#define PP_PRSR_DATA_USEDW_FIELD1_USEDW_RST                         (0x0)
#define PP_PRSR_DATA_USEDW_RSVD2_OFF                                (13)
#define PP_PRSR_DATA_USEDW_RSVD2_LEN                                (3)
#define PP_PRSR_DATA_USEDW_RSVD2_MSK                                (0x0000E000)
#define PP_PRSR_DATA_USEDW_RSVD2_RST                                (0x0)
#define PP_PRSR_DATA_USEDW_FIELD0_USEDW_OFF                         (8)
#define PP_PRSR_DATA_USEDW_FIELD0_USEDW_LEN                         (5)
#define PP_PRSR_DATA_USEDW_FIELD0_USEDW_MSK                         (0x00001F00)
#define PP_PRSR_DATA_USEDW_FIELD0_USEDW_RST                         (0x0)
#define PP_PRSR_DATA_USEDW_RSVD3_OFF                                (5)
#define PP_PRSR_DATA_USEDW_RSVD3_LEN                                (3)
#define PP_PRSR_DATA_USEDW_RSVD3_MSK                                (0x000000E0)
#define PP_PRSR_DATA_USEDW_RSVD3_RST                                (0x0)
#define PP_PRSR_DATA_USEDW_MASK_USEDW_OFF                           (0)
#define PP_PRSR_DATA_USEDW_MASK_USEDW_LEN                           (5)
#define PP_PRSR_DATA_USEDW_MASK_USEDW_MSK                           (0x0000001F)
#define PP_PRSR_DATA_USEDW_MASK_USEDW_RST                           (0x0)

/**
 * SW_REG_NAME : PP_PRSR_IF_SAMPLE_REG
 * HW_REG_NAME : if_sample
 * DESCRIPTION : Current IFs state for debug, NOTE, B Step ONLY
 *               register
 *
 *  Register Fields :
 *   [31:25][RO] - ppv4_parser inner stop bus
 *   [24: 6][RO] - RPB IF
 *   [ 5: 0][RO] - prs2cls_ready,cls2prs_accept,prs2uc_ready,uc2prs_a
 *                 ccept,prs2cls_sop,prs2cls_eop
 *
 */
#define PP_PRSR_IF_SAMPLE_REG                          ((PRSR_BASE_ADDR) + 0x48)
#define PP_PRSR_IF_SAMPLE_STOP_BUS_OFF                              (25)
#define PP_PRSR_IF_SAMPLE_STOP_BUS_LEN                              (7)
#define PP_PRSR_IF_SAMPLE_STOP_BUS_MSK                              (0xFE000000)
#define PP_PRSR_IF_SAMPLE_STOP_BUS_RST                              (0x0)
#define PP_PRSR_IF_SAMPLE_RPB_IF_OFF                                (6)
#define PP_PRSR_IF_SAMPLE_RPB_IF_LEN                                (19)
#define PP_PRSR_IF_SAMPLE_RPB_IF_MSK                                (0x01FFFFC0)
#define PP_PRSR_IF_SAMPLE_RPB_IF_RST                                (0x0)
#define PP_PRSR_IF_SAMPLE_CLS_US_IF_OFF                             (0)
#define PP_PRSR_IF_SAMPLE_CLS_US_IF_LEN                             (6)
#define PP_PRSR_IF_SAMPLE_CLS_US_IF_MSK                             (0x0000003F)
#define PP_PRSR_IF_SAMPLE_CLS_US_IF_RST                             (0x0)

/**
 * SW_REG_NAME : PP_PRSR_FV_CNT_REG
 * HW_REG_NAME : FV_counter_reg
 * DESCRIPTION : FV counter register, counts the numbers of FV
 *               driven out from the parser
 *
 *  Register Fields :
 *   [31: 0][RO] - FV counter register
 *
 */
#define PP_PRSR_FV_CNT_REG                             ((PRSR_BASE_ADDR) + 0x50)
#define PP_PRSR_FV_CNT_OFF                                          (0)
#define PP_PRSR_FV_CNT_LEN                                          (32)
#define PP_PRSR_FV_CNT_MSK                                          (0xFFFFFFFF)
#define PP_PRSR_FV_CNT_RST                                          (0x0)

/**
 * SW_REG_NAME : PP_PRSR_PARSER_RPB_POP_CNT_REG
 * HW_REG_NAME : parser_rpb_pop_counter_reg
 * DESCRIPTION : parser_rpb_pop_counter register, counts the
 *               numbers of POPs from RPB
 *
 *  Register Fields :
 *   [31: 0][RO] - parser_rpb_pop_counter register
 *
 */
#define PP_PRSR_PARSER_RPB_POP_CNT_REG                 ((PRSR_BASE_ADDR) + 0x54)
#define PP_PRSR_PARSER_RPB_POP_CNT_OFF                              (0)
#define PP_PRSR_PARSER_RPB_POP_CNT_LEN                              (32)
#define PP_PRSR_PARSER_RPB_POP_CNT_MSK                              (0xFFFFFFFF)
#define PP_PRSR_PARSER_RPB_POP_CNT_RST                              (0x0)

/**
 * SW_REG_NAME : PP_PRSR_RPB_PARSER_POP_VLD_CNT_REG
 * HW_REG_NAME : rpb_parser_pop_vld_counter_reg
 * DESCRIPTION : rpb_parser_pop_vld_counter register, counts the
 *               numbers of POPs from RPB
 *
 *  Register Fields :
 *   [31: 0][RO] - rpb_parser_pop_vld_counter register
 *
 */
#define PP_PRSR_RPB_PARSER_POP_VLD_CNT_REG             ((PRSR_BASE_ADDR) + 0x58)
#define PP_PRSR_RPB_PARSER_POP_VLD_CNT_OFF                          (0)
#define PP_PRSR_RPB_PARSER_POP_VLD_CNT_LEN                          (32)
#define PP_PRSR_RPB_PARSER_POP_VLD_CNT_MSK                          (0xFFFFFFFF)
#define PP_PRSR_RPB_PARSER_POP_VLD_CNT_RST                          (0x0)

/**
 * SW_REG_NAME : PP_PRSR_FV2CLASS_CNT_REG
 * HW_REG_NAME : fv2class_counter_reg
 * DESCRIPTION : fv2class_counter register, counts the numbers of
 *               FV send to class
 *
 *  Register Fields :
 *   [31: 0][RO] - fv2class_counter register
 *
 */
#define PP_PRSR_FV2CLASS_CNT_REG                       ((PRSR_BASE_ADDR) + 0x5C)
#define PP_PRSR_FV2CLASS_CNT_OFF                                    (0)
#define PP_PRSR_FV2CLASS_CNT_LEN                                    (32)
#define PP_PRSR_FV2CLASS_CNT_MSK                                    (0xFFFFFFFF)
#define PP_PRSR_FV2CLASS_CNT_RST                                    (0x0)

/**
 * SW_REG_NAME : PP_PRSR_FV2UC_CNT_REG
 * HW_REG_NAME : fv2uc_counter_reg
 * DESCRIPTION : fv2uc_counter register, counts the numbers of FV
 *               send to uC
 *
 *  Register Fields :
 *   [31: 0][RO] - fv2uc_counter register
 *
 */
#define PP_PRSR_FV2UC_CNT_REG                          ((PRSR_BASE_ADDR) + 0x60)
#define PP_PRSR_FV2UC_CNT_OFF                                       (0)
#define PP_PRSR_FV2UC_CNT_LEN                                       (32)
#define PP_PRSR_FV2UC_CNT_MSK                                       (0xFFFFFFFF)
#define PP_PRSR_FV2UC_CNT_RST                                       (0x0)

/**
 * SW_REG_NAME : PP_PRSR_LY2_ETYPE_HIT_LAST_REG
 * HW_REG_NAME : ly2_etype_hit_last
 * DESCRIPTION : which ly2 etype were hit
 *
 *  Register Fields :
 *   [31:10][RO] - Reserved
 *   [ 9: 0][RO] - ly2_etype_hit
 *
 */
#define PP_PRSR_LY2_ETYPE_HIT_LAST_REG                 ((PRSR_BASE_ADDR) + 0x64)
#define PP_PRSR_LY2_ETYPE_HIT_LAST_RSVD0_OFF                        (10)
#define PP_PRSR_LY2_ETYPE_HIT_LAST_RSVD0_LEN                        (22)
#define PP_PRSR_LY2_ETYPE_HIT_LAST_RSVD0_MSK                        (0xFFFFFC00)
#define PP_PRSR_LY2_ETYPE_HIT_LAST_RSVD0_RST                        (0x0)
#define PP_PRSR_LY2_ETYPE_HIT_LAST_OFF                              (0)
#define PP_PRSR_LY2_ETYPE_HIT_LAST_LEN                              (10)
#define PP_PRSR_LY2_ETYPE_HIT_LAST_MSK                              (0x000003FF)
#define PP_PRSR_LY2_ETYPE_HIT_LAST_RST                              (0x0)

/**
 * SW_REG_NAME : PP_PRSR_LY2_ETYPE_HIT_STICKY_REG
 * HW_REG_NAME : ly2_etype_hit_sticky
 * DESCRIPTION : which ly2 etype were hit
 *
 *  Register Fields :
 *   [31:10][RO] - Reserved
 *   [ 9: 0][RO] - ly2_etype_hit
 *
 */
#define PP_PRSR_LY2_ETYPE_HIT_STICKY_REG               ((PRSR_BASE_ADDR) + 0x68)
#define PP_PRSR_LY2_ETYPE_HIT_STICKY_RSVD0_OFF                      (10)
#define PP_PRSR_LY2_ETYPE_HIT_STICKY_RSVD0_LEN                      (22)
#define PP_PRSR_LY2_ETYPE_HIT_STICKY_RSVD0_MSK                      (0xFFFFFC00)
#define PP_PRSR_LY2_ETYPE_HIT_STICKY_RSVD0_RST                      (0x0)
#define PP_PRSR_LY2_ETYPE_HIT_STICKY_OFF                            (0)
#define PP_PRSR_LY2_ETYPE_HIT_STICKY_LEN                            (10)
#define PP_PRSR_LY2_ETYPE_HIT_STICKY_MSK                            (0x000003FF)
#define PP_PRSR_LY2_ETYPE_HIT_STICKY_RST                            (0x0)

/**
 * SW_REG_NAME : PP_PRSR_LY3_PROTO_HIT_STICKY_0_31_REG
 * HW_REG_NAME : ly3_proto_hit_sticky_0_31
 * DESCRIPTION : which ly3 protocols were hit
 *
 *  Register Fields :
 *   [31: 0][RO] - ly3_proto_hit
 *
 */
#define PP_PRSR_LY3_PROTO_HIT_STICKY_0_31_REG          ((PRSR_BASE_ADDR) + 0x6C)
#define PP_PRSR_LY3_PROTO_HIT_STICKY_0_31_OFF                       (0)
#define PP_PRSR_LY3_PROTO_HIT_STICKY_0_31_LEN                       (32)
#define PP_PRSR_LY3_PROTO_HIT_STICKY_0_31_MSK                       (0xFFFFFFFF)
#define PP_PRSR_LY3_PROTO_HIT_STICKY_0_31_RST                       (0x0)

/**
 * SW_REG_NAME : PP_PRSR_LY3_PROTO_HIT_STICKY_32_63_REG
 * HW_REG_NAME : ly3_proto_hit_sticky_32_63
 * DESCRIPTION : which ly3 protocols were hit
 *
 *  Register Fields :
 *   [31: 0][RO] - ly3_proto_hit
 *
 */
#define PP_PRSR_LY3_PROTO_HIT_STICKY_32_63_REG         ((PRSR_BASE_ADDR) + 0x70)
#define PP_PRSR_LY3_PROTO_HIT_STICKY_32_63_OFF                      (0)
#define PP_PRSR_LY3_PROTO_HIT_STICKY_32_63_LEN                      (32)
#define PP_PRSR_LY3_PROTO_HIT_STICKY_32_63_MSK                      (0xFFFFFFFF)
#define PP_PRSR_LY3_PROTO_HIT_STICKY_32_63_RST                      (0x0)

/**
 * SW_REG_NAME : PP_PRSR_LY3_PROTO_SEQ_0_LAST_REG
 * HW_REG_NAME : ly3_proto_seq_0_last
 * DESCRIPTION : which ly3 protocols were hit
 *
 *  Register Fields :
 *   [31:30][RO] - Reserved
 *   [29:24][RO] - ly3_proto_seq_4
 *   [23:18][RO] - ly3_proto_seq_3
 *   [17:12][RO] - ly3_proto_seq_2
 *   [11: 6][RO] - ly3_proto_seq_1
 *   [ 5: 0][RO] - ly3_proto_seq_0
 *
 */
#define PP_PRSR_LY3_PROTO_SEQ_0_LAST_REG               ((PRSR_BASE_ADDR) + 0x74)
#define PP_PRSR_LY3_PROTO_SEQ_0_LAST_RSVD0_OFF                      (30)
#define PP_PRSR_LY3_PROTO_SEQ_0_LAST_RSVD0_LEN                      (2)
#define PP_PRSR_LY3_PROTO_SEQ_0_LAST_RSVD0_MSK                      (0xC0000000)
#define PP_PRSR_LY3_PROTO_SEQ_0_LAST_RSVD0_RST                      (0x0)
#define PP_PRSR_LY3_PROTO_SEQ_0_LAST_LY3_PROTO_SEQ_4_OFF            (24)
#define PP_PRSR_LY3_PROTO_SEQ_0_LAST_LY3_PROTO_SEQ_4_LEN            (6)
#define PP_PRSR_LY3_PROTO_SEQ_0_LAST_LY3_PROTO_SEQ_4_MSK            (0x3F000000)
#define PP_PRSR_LY3_PROTO_SEQ_0_LAST_LY3_PROTO_SEQ_4_RST            (0x0)
#define PP_PRSR_LY3_PROTO_SEQ_0_LAST_LY3_PROTO_SEQ_3_OFF            (18)
#define PP_PRSR_LY3_PROTO_SEQ_0_LAST_LY3_PROTO_SEQ_3_LEN            (6)
#define PP_PRSR_LY3_PROTO_SEQ_0_LAST_LY3_PROTO_SEQ_3_MSK            (0x00FC0000)
#define PP_PRSR_LY3_PROTO_SEQ_0_LAST_LY3_PROTO_SEQ_3_RST            (0x0)
#define PP_PRSR_LY3_PROTO_SEQ_0_LAST_LY3_PROTO_SEQ_2_OFF            (12)
#define PP_PRSR_LY3_PROTO_SEQ_0_LAST_LY3_PROTO_SEQ_2_LEN            (6)
#define PP_PRSR_LY3_PROTO_SEQ_0_LAST_LY3_PROTO_SEQ_2_MSK            (0x0003F000)
#define PP_PRSR_LY3_PROTO_SEQ_0_LAST_LY3_PROTO_SEQ_2_RST            (0x0)
#define PP_PRSR_LY3_PROTO_SEQ_0_LAST_LY3_PROTO_SEQ_1_OFF            (6)
#define PP_PRSR_LY3_PROTO_SEQ_0_LAST_LY3_PROTO_SEQ_1_LEN            (6)
#define PP_PRSR_LY3_PROTO_SEQ_0_LAST_LY3_PROTO_SEQ_1_MSK            (0x00000FC0)
#define PP_PRSR_LY3_PROTO_SEQ_0_LAST_LY3_PROTO_SEQ_1_RST            (0x0)
#define PP_PRSR_LY3_PROTO_SEQ_0_LAST_OFF                            (0)
#define PP_PRSR_LY3_PROTO_SEQ_0_LAST_LEN                            (6)
#define PP_PRSR_LY3_PROTO_SEQ_0_LAST_MSK                            (0x0000003F)
#define PP_PRSR_LY3_PROTO_SEQ_0_LAST_RST                            (0x0)

/**
 * SW_REG_NAME : PP_PRSR_LY3_PROTO_SEQ_1_LAST_REG
 * HW_REG_NAME : ly3_proto_seq_1_last
 * DESCRIPTION : which ly3 protocols were hit
 *
 *  Register Fields :
 *   [31:30][RO] - Reserved
 *   [29:24][RO] - ly3_proto_seq_9
 *   [23:18][RO] - ly3_proto_seq_8
 *   [17:12][RO] - ly3_proto_seq_7
 *   [11: 6][RO] - ly3_proto_seq_6
 *   [ 5: 0][RO] - ly3_proto_seq_5
 *
 */
#define PP_PRSR_LY3_PROTO_SEQ_1_LAST_REG               ((PRSR_BASE_ADDR) + 0x78)
#define PP_PRSR_LY3_PROTO_SEQ_1_LAST_RSVD0_OFF                      (30)
#define PP_PRSR_LY3_PROTO_SEQ_1_LAST_RSVD0_LEN                      (2)
#define PP_PRSR_LY3_PROTO_SEQ_1_LAST_RSVD0_MSK                      (0xC0000000)
#define PP_PRSR_LY3_PROTO_SEQ_1_LAST_RSVD0_RST                      (0x0)
#define PP_PRSR_LY3_PROTO_SEQ_1_LAST_LY3_PROTO_SEQ_9_OFF            (24)
#define PP_PRSR_LY3_PROTO_SEQ_1_LAST_LY3_PROTO_SEQ_9_LEN            (6)
#define PP_PRSR_LY3_PROTO_SEQ_1_LAST_LY3_PROTO_SEQ_9_MSK            (0x3F000000)
#define PP_PRSR_LY3_PROTO_SEQ_1_LAST_LY3_PROTO_SEQ_9_RST            (0x0)
#define PP_PRSR_LY3_PROTO_SEQ_1_LAST_LY3_PROTO_SEQ_8_OFF            (18)
#define PP_PRSR_LY3_PROTO_SEQ_1_LAST_LY3_PROTO_SEQ_8_LEN            (6)
#define PP_PRSR_LY3_PROTO_SEQ_1_LAST_LY3_PROTO_SEQ_8_MSK            (0x00FC0000)
#define PP_PRSR_LY3_PROTO_SEQ_1_LAST_LY3_PROTO_SEQ_8_RST            (0x0)
#define PP_PRSR_LY3_PROTO_SEQ_1_LAST_LY3_PROTO_SEQ_7_OFF            (12)
#define PP_PRSR_LY3_PROTO_SEQ_1_LAST_LY3_PROTO_SEQ_7_LEN            (6)
#define PP_PRSR_LY3_PROTO_SEQ_1_LAST_LY3_PROTO_SEQ_7_MSK            (0x0003F000)
#define PP_PRSR_LY3_PROTO_SEQ_1_LAST_LY3_PROTO_SEQ_7_RST            (0x0)
#define PP_PRSR_LY3_PROTO_SEQ_1_LAST_LY3_PROTO_SEQ_6_OFF            (6)
#define PP_PRSR_LY3_PROTO_SEQ_1_LAST_LY3_PROTO_SEQ_6_LEN            (6)
#define PP_PRSR_LY3_PROTO_SEQ_1_LAST_LY3_PROTO_SEQ_6_MSK            (0x00000FC0)
#define PP_PRSR_LY3_PROTO_SEQ_1_LAST_LY3_PROTO_SEQ_6_RST            (0x0)
#define PP_PRSR_LY3_PROTO_SEQ_1_LAST_LY3_PROTO_SEQ_5_OFF            (0)
#define PP_PRSR_LY3_PROTO_SEQ_1_LAST_LY3_PROTO_SEQ_5_LEN            (6)
#define PP_PRSR_LY3_PROTO_SEQ_1_LAST_LY3_PROTO_SEQ_5_MSK            (0x0000003F)
#define PP_PRSR_LY3_PROTO_SEQ_1_LAST_LY3_PROTO_SEQ_5_RST            (0x0)

/**
 * SW_REG_NAME : PP_PRSR_LY3_PROTO_SEQ_2_LAST_REG
 * HW_REG_NAME : ly3_proto_seq_2_last
 * DESCRIPTION : which ly3 protocols were hit
 *
 *  Register Fields :
 *   [31:30][RO] - Reserved
 *   [29:24][RO] - ly3_proto_seq_14
 *   [23:18][RO] - ly3_proto_seq_13
 *   [17:12][RO] - ly3_proto_seq_12
 *   [11: 6][RO] - ly3_proto_seq_11
 *   [ 5: 0][RO] - ly3_proto_seq_10
 *
 */
#define PP_PRSR_LY3_PROTO_SEQ_2_LAST_REG               ((PRSR_BASE_ADDR) + 0x7C)
#define PP_PRSR_LY3_PROTO_SEQ_2_LAST_RSVD0_OFF                      (30)
#define PP_PRSR_LY3_PROTO_SEQ_2_LAST_RSVD0_LEN                      (2)
#define PP_PRSR_LY3_PROTO_SEQ_2_LAST_RSVD0_MSK                      (0xC0000000)
#define PP_PRSR_LY3_PROTO_SEQ_2_LAST_RSVD0_RST                      (0x0)
#define PP_PRSR_LY3_PROTO_SEQ_2_LAST_LY3_PROTO_SEQ_14_OFF           (24)
#define PP_PRSR_LY3_PROTO_SEQ_2_LAST_LY3_PROTO_SEQ_14_LEN           (6)
#define PP_PRSR_LY3_PROTO_SEQ_2_LAST_LY3_PROTO_SEQ_14_MSK           (0x3F000000)
#define PP_PRSR_LY3_PROTO_SEQ_2_LAST_LY3_PROTO_SEQ_14_RST           (0x0)
#define PP_PRSR_LY3_PROTO_SEQ_2_LAST_LY3_PROTO_SEQ_13_OFF           (18)
#define PP_PRSR_LY3_PROTO_SEQ_2_LAST_LY3_PROTO_SEQ_13_LEN           (6)
#define PP_PRSR_LY3_PROTO_SEQ_2_LAST_LY3_PROTO_SEQ_13_MSK           (0x00FC0000)
#define PP_PRSR_LY3_PROTO_SEQ_2_LAST_LY3_PROTO_SEQ_13_RST           (0x0)
#define PP_PRSR_LY3_PROTO_SEQ_2_LAST_LY3_PROTO_SEQ_12_OFF           (12)
#define PP_PRSR_LY3_PROTO_SEQ_2_LAST_LY3_PROTO_SEQ_12_LEN           (6)
#define PP_PRSR_LY3_PROTO_SEQ_2_LAST_LY3_PROTO_SEQ_12_MSK           (0x0003F000)
#define PP_PRSR_LY3_PROTO_SEQ_2_LAST_LY3_PROTO_SEQ_12_RST           (0x0)
#define PP_PRSR_LY3_PROTO_SEQ_2_LAST_LY3_PROTO_SEQ_11_OFF           (6)
#define PP_PRSR_LY3_PROTO_SEQ_2_LAST_LY3_PROTO_SEQ_11_LEN           (6)
#define PP_PRSR_LY3_PROTO_SEQ_2_LAST_LY3_PROTO_SEQ_11_MSK           (0x00000FC0)
#define PP_PRSR_LY3_PROTO_SEQ_2_LAST_LY3_PROTO_SEQ_11_RST           (0x0)
#define PP_PRSR_LY3_PROTO_SEQ_2_LAST_LY3_PROTO_SEQ_10_OFF           (0)
#define PP_PRSR_LY3_PROTO_SEQ_2_LAST_LY3_PROTO_SEQ_10_LEN           (6)
#define PP_PRSR_LY3_PROTO_SEQ_2_LAST_LY3_PROTO_SEQ_10_MSK           (0x0000003F)
#define PP_PRSR_LY3_PROTO_SEQ_2_LAST_LY3_PROTO_SEQ_10_RST           (0x0)

/**
 * SW_REG_NAME : PP_PRSR_DROP_STRATEGY_REG
 * HW_REG_NAME : drop_strategy_reg
 * DESCRIPTION : drop_strategy_reg
 *
 *  Register Fields :
 *   [31:17][RO] - Reserved
 *   [16:15][RW] - Drop will be Sw selectable from a couple of
 *                 strategies: any[1] , both[2], and none[0]
 *   [14: 9][RO] - Reserved
 *   [ 8: 8][RW] - Drop will be Sw selectable from a couple of
 *                 strategies: RPB threshold enable
 *   [ 7: 1][RO] - Reserved
 *   [ 0: 0][RW] - Drop will be Sw selectable from a couple of
 *                 strategies: Rate enable
 *
 */
#define PP_PRSR_DROP_STRATEGY_REG                      ((PRSR_BASE_ADDR) + 0x80)
#define PP_PRSR_DROP_STRATEGY_RSVD0_OFF                             (17)
#define PP_PRSR_DROP_STRATEGY_RSVD0_LEN                             (15)
#define PP_PRSR_DROP_STRATEGY_RSVD0_MSK                             (0xFFFE0000)
#define PP_PRSR_DROP_STRATEGY_RSVD0_RST                             (0x0)
#define PP_PRSR_DROP_STRATEGY_STRATEGIES_OFF                        (15)
#define PP_PRSR_DROP_STRATEGY_STRATEGIES_LEN                        (2)
#define PP_PRSR_DROP_STRATEGY_STRATEGIES_MSK                        (0x00018000)
#define PP_PRSR_DROP_STRATEGY_STRATEGIES_RST                        (0x0)
#define PP_PRSR_DROP_STRATEGY_RSVD1_OFF                             (9)
#define PP_PRSR_DROP_STRATEGY_RSVD1_LEN                             (6)
#define PP_PRSR_DROP_STRATEGY_RSVD1_MSK                             (0x00007E00)
#define PP_PRSR_DROP_STRATEGY_RSVD1_RST                             (0x0)
#define PP_PRSR_DROP_STRATEGY_RPB_TH_EN_OFF                         (8)
#define PP_PRSR_DROP_STRATEGY_RPB_TH_EN_LEN                         (1)
#define PP_PRSR_DROP_STRATEGY_RPB_TH_EN_MSK                         (0x00000100)
#define PP_PRSR_DROP_STRATEGY_RPB_TH_EN_RST                         (0x0)
#define PP_PRSR_DROP_STRATEGY_RSVD2_OFF                             (1)
#define PP_PRSR_DROP_STRATEGY_RSVD2_LEN                             (7)
#define PP_PRSR_DROP_STRATEGY_RSVD2_MSK                             (0x000000FE)
#define PP_PRSR_DROP_STRATEGY_RSVD2_RST                             (0x0)
#define PP_PRSR_DROP_STRATEGY_PKT_RATE_EN_OFF                       (0)
#define PP_PRSR_DROP_STRATEGY_PKT_RATE_EN_LEN                       (1)
#define PP_PRSR_DROP_STRATEGY_PKT_RATE_EN_MSK                       (0x00000001)
#define PP_PRSR_DROP_STRATEGY_PKT_RATE_EN_RST                       (0x0)

/**
 * SW_REG_NAME : PP_PRSR_RPB_FC_MASK_REG
 * HW_REG_NAME : rpb_fc_mask_reg
 * DESCRIPTION : rpb_fc_mask
 *
 *  Register Fields :
 *   [31:28][RO] - Reserved
 *   [27:24][RW] - RPB fc 3 threshold mask
 *   [23:20][RO] - Reserved
 *   [19:16][RW] - RPB fc 2 threshold mask
 *   [15:12][RO] - Reserved
 *   [11: 8][RW] - RPB fc 1 threshold mask
 *   [ 7: 4][RO] - Reserved
 *   [ 3: 0][RW] - RPB fc 0 threshold mask
 *
 */
#define PP_PRSR_RPB_FC_MASK_REG                        ((PRSR_BASE_ADDR) + 0x84)
#define PP_PRSR_RPB_FC_MASK_RSVD0_OFF                               (28)
#define PP_PRSR_RPB_FC_MASK_RSVD0_LEN                               (4)
#define PP_PRSR_RPB_FC_MASK_RSVD0_MSK                               (0xF0000000)
#define PP_PRSR_RPB_FC_MASK_RSVD0_RST                               (0x0)
#define PP_PRSR_RPB_FC_MASK_FC_3_MASK_OFF                           (24)
#define PP_PRSR_RPB_FC_MASK_FC_3_MASK_LEN                           (4)
#define PP_PRSR_RPB_FC_MASK_FC_3_MASK_MSK                           (0x0F000000)
#define PP_PRSR_RPB_FC_MASK_FC_3_MASK_RST                           (0x0)
#define PP_PRSR_RPB_FC_MASK_RSVD1_OFF                               (20)
#define PP_PRSR_RPB_FC_MASK_RSVD1_LEN                               (4)
#define PP_PRSR_RPB_FC_MASK_RSVD1_MSK                               (0x00F00000)
#define PP_PRSR_RPB_FC_MASK_RSVD1_RST                               (0x0)
#define PP_PRSR_RPB_FC_MASK_FC_2_MASK_OFF                           (16)
#define PP_PRSR_RPB_FC_MASK_FC_2_MASK_LEN                           (4)
#define PP_PRSR_RPB_FC_MASK_FC_2_MASK_MSK                           (0x000F0000)
#define PP_PRSR_RPB_FC_MASK_FC_2_MASK_RST                           (0x0)
#define PP_PRSR_RPB_FC_MASK_RSVD2_OFF                               (12)
#define PP_PRSR_RPB_FC_MASK_RSVD2_LEN                               (4)
#define PP_PRSR_RPB_FC_MASK_RSVD2_MSK                               (0x0000F000)
#define PP_PRSR_RPB_FC_MASK_RSVD2_RST                               (0x0)
#define PP_PRSR_RPB_FC_MASK_FC_1_MASK_OFF                           (8)
#define PP_PRSR_RPB_FC_MASK_FC_1_MASK_LEN                           (4)
#define PP_PRSR_RPB_FC_MASK_FC_1_MASK_MSK                           (0x00000F00)
#define PP_PRSR_RPB_FC_MASK_FC_1_MASK_RST                           (0x0)
#define PP_PRSR_RPB_FC_MASK_RSVD3_OFF                               (4)
#define PP_PRSR_RPB_FC_MASK_RSVD3_LEN                               (4)
#define PP_PRSR_RPB_FC_MASK_RSVD3_MSK                               (0x000000F0)
#define PP_PRSR_RPB_FC_MASK_RSVD3_RST                               (0x0)
#define PP_PRSR_RPB_FC_MASK_FC_0_MASK_OFF                           (0)
#define PP_PRSR_RPB_FC_MASK_FC_0_MASK_LEN                           (4)
#define PP_PRSR_RPB_FC_MASK_FC_0_MASK_MSK                           (0x0000000F)
#define PP_PRSR_RPB_FC_MASK_FC_0_MASK_RST                           (0x0)

/**
 * SW_REG_NAME : PP_PRSR_PKT_RATE_MON_INC_REG
 * HW_REG_NAME : packet_rate_mon_increment_reg
 * DESCRIPTION : packet rate monitor increment value
 *
 *  Register Fields :
 *   [31:14][RO] - Reserved
 *   [13: 0][RW] - Increment a packet rate counter by configurable
 *                 amount (up to 8192)
 *
 */
#define PP_PRSR_PKT_RATE_MON_INC_REG                   ((PRSR_BASE_ADDR) + 0x88)
#define PP_PRSR_PKT_RATE_MON_INC_RSVD0_OFF                          (14)
#define PP_PRSR_PKT_RATE_MON_INC_RSVD0_LEN                          (18)
#define PP_PRSR_PKT_RATE_MON_INC_RSVD0_MSK                          (0xFFFFC000)
#define PP_PRSR_PKT_RATE_MON_INC_RSVD0_RST                          (0x0)
#define PP_PRSR_PKT_RATE_MON_INC_OFF                                (0)
#define PP_PRSR_PKT_RATE_MON_INC_LEN                                (14)
#define PP_PRSR_PKT_RATE_MON_INC_MSK                                (0x00003FFF)
#define PP_PRSR_PKT_RATE_MON_INC_RST                                (0x0)

/**
 * SW_REG_NAME : PP_PRSR_PKT_RATE_MON_SATURATE_REG
 * HW_REG_NAME : packet_rate_mon_saturate_reg
 * DESCRIPTION : packet rate monitor high saturate
 *
 *  Register Fields :
 *   [31:19][RO] - Reserved
 *   [18: 0][RW] - packet rate monitor counter high saturate
 *                 configurable - up to 128k
 *
 */
#define PP_PRSR_PKT_RATE_MON_SATURATE_REG              ((PRSR_BASE_ADDR) + 0x8C)
#define PP_PRSR_PKT_RATE_MON_SATURATE_RSVD0_OFF                     (19)
#define PP_PRSR_PKT_RATE_MON_SATURATE_RSVD0_LEN                     (13)
#define PP_PRSR_PKT_RATE_MON_SATURATE_RSVD0_MSK                     (0xFFF80000)
#define PP_PRSR_PKT_RATE_MON_SATURATE_RSVD0_RST                     (0x0)
#define PP_PRSR_PKT_RATE_MON_SATURATE_HIGH_SATURATE_OFF             (0)
#define PP_PRSR_PKT_RATE_MON_SATURATE_HIGH_SATURATE_LEN             (19)
#define PP_PRSR_PKT_RATE_MON_SATURATE_HIGH_SATURATE_MSK             (0x0007FFFF)
#define PP_PRSR_PKT_RATE_MON_SATURATE_HIGH_SATURATE_RST             (0x0)

/**
 * SW_REG_NAME : PP_PRSR_PKT_RATE_STATUS_REG
 * HW_REG_NAME : packet_rate_status_reg
 * DESCRIPTION : packet_rate_drop_counter
 *
 *  Register Fields :
 *   [31: 0][RW] - packet rate drop counter
 *
 */
#define PP_PRSR_PKT_RATE_STATUS_REG                    ((PRSR_BASE_ADDR) + 0x90)
#define PP_PRSR_PKT_RATE_STATUS_DROP_CNT_OFF                        (0)
#define PP_PRSR_PKT_RATE_STATUS_DROP_CNT_LEN                        (32)
#define PP_PRSR_PKT_RATE_STATUS_DROP_CNT_MSK                        (0xFFFFFFFF)
#define PP_PRSR_PKT_RATE_STATUS_DROP_CNT_RST                        (0x0)

/**
 * SW_REG_NAME : PP_PRSR_RPB_PPRS_TH_STATUS_REG
 * HW_REG_NAME : rpb_pprs_th_status_reg
 * DESCRIPTION : rpb_pprs_th_drop_counter
 *
 *  Register Fields :
 *   [31: 0][RW] - RPB pprs threshold drop counter
 *
 */
#define PP_PRSR_RPB_PPRS_TH_STATUS_REG                 ((PRSR_BASE_ADDR) + 0x94)
#define PP_PRSR_RPB_PPRS_TH_STATUS_DROP_CNT_OFF                     (0)
#define PP_PRSR_RPB_PPRS_TH_STATUS_DROP_CNT_LEN                     (32)
#define PP_PRSR_RPB_PPRS_TH_STATUS_DROP_CNT_MSK                     (0xFFFFFFFF)
#define PP_PRSR_RPB_PPRS_TH_STATUS_DROP_CNT_RST                     (0x0)

/**
 * SW_REG_NAME : PP_PRSR_RPB_DISCARD_CNT_REG
 * HW_REG_NAME : rpb_discard_counter
 * DESCRIPTION : error_counter register, counts the numbers of
 *               errored packet
 *
 *  Register Fields :
 *   [31: 0][RO] - error_counter
 *
 */
#define PP_PRSR_RPB_DISCARD_CNT_REG                   ((PRSR_BASE_ADDR) + 0x0A0)
#define PP_PRSR_RPB_DISCARD_CNT_ERROR_CNT_OFF                       (0)
#define PP_PRSR_RPB_DISCARD_CNT_ERROR_CNT_LEN                       (32)
#define PP_PRSR_RPB_DISCARD_CNT_ERROR_CNT_MSK                       (0xFFFFFFFF)
#define PP_PRSR_RPB_DISCARD_CNT_ERROR_CNT_RST                       (0x0)

/**
 * SW_REG_NAME : PP_PRSR_RPB_ERROR_CNT_REG
 * HW_REG_NAME : rpb_error_counter
 * DESCRIPTION : error_counter register, counts the numbers of
 *               errored packet
 *
 *  Register Fields :
 *   [31: 0][RO] - error_counter
 *
 */
#define PP_PRSR_RPB_ERROR_CNT_REG                     ((PRSR_BASE_ADDR) + 0x0A4)
#define PP_PRSR_RPB_ERROR_CNT_OFF                                   (0)
#define PP_PRSR_RPB_ERROR_CNT_LEN                                   (32)
#define PP_PRSR_RPB_ERROR_CNT_MSK                                   (0xFFFFFFFF)
#define PP_PRSR_RPB_ERROR_CNT_RST                                   (0x0)

/**
 * SW_REG_NAME : PP_PRSR_ZERO_LEN_CNT_REG
 * HW_REG_NAME : zero_len_counter
 * DESCRIPTION : error_counter register, counts the numbers of
 *               errored packet
 *
 *  Register Fields :
 *   [31: 0][RO] - error_counter
 *
 */
#define PP_PRSR_ZERO_LEN_CNT_REG                      ((PRSR_BASE_ADDR) + 0x0A8)
#define PP_PRSR_ZERO_LEN_CNT_ERROR_CNT_OFF                          (0)
#define PP_PRSR_ZERO_LEN_CNT_ERROR_CNT_LEN                          (32)
#define PP_PRSR_ZERO_LEN_CNT_ERROR_CNT_MSK                          (0xFFFFFFFF)
#define PP_PRSR_ZERO_LEN_CNT_ERROR_CNT_RST                          (0x0)

/**
 * SW_REG_NAME : PP_PRSR_NO_LAST_ETH_TYPE_ERR_CNT_REG
 * HW_REG_NAME : no_last_eth_type_err_counter
 * DESCRIPTION : error_counter register, counts the numbers of
 *               errored packet
 *
 *  Register Fields :
 *   [31: 0][RO] - error_counter
 *
 */
#define PP_PRSR_NO_LAST_ETH_TYPE_ERR_CNT_REG          ((PRSR_BASE_ADDR) + 0x0AC)
#define PP_PRSR_NO_LAST_ETH_TYPE_ERR_CNT_ERROR_CNT_OFF              (0)
#define PP_PRSR_NO_LAST_ETH_TYPE_ERR_CNT_ERROR_CNT_LEN              (32)
#define PP_PRSR_NO_LAST_ETH_TYPE_ERR_CNT_ERROR_CNT_MSK              (0xFFFFFFFF)
#define PP_PRSR_NO_LAST_ETH_TYPE_ERR_CNT_ERROR_CNT_RST              (0x0)

/**
 * SW_REG_NAME : PP_PRSR_PROTO_LENGTH_ERR_CNT_REG
 * HW_REG_NAME : proto_length_err_counter
 * DESCRIPTION : error_counter register, counts the numbers of
 *               errored packet
 *
 *  Register Fields :
 *   [31: 0][RO] - error_counter
 *
 */
#define PP_PRSR_PROTO_LENGTH_ERR_CNT_REG              ((PRSR_BASE_ADDR) + 0x0B0)
#define PP_PRSR_PROTO_LENGTH_ERR_CNT_ERROR_CNT_OFF                  (0)
#define PP_PRSR_PROTO_LENGTH_ERR_CNT_ERROR_CNT_LEN                  (32)
#define PP_PRSR_PROTO_LENGTH_ERR_CNT_ERROR_CNT_MSK                  (0xFFFFFFFF)
#define PP_PRSR_PROTO_LENGTH_ERR_CNT_ERROR_CNT_RST                  (0x0)

/**
 * SW_REG_NAME : PP_PRSR_MAX_HEADER_LEN_ERR_CNT_REG
 * HW_REG_NAME : max_header_len_err_counter
 * DESCRIPTION : error_counter register, counts the numbers of
 *               errored packet
 *
 *  Register Fields :
 *   [31: 0][RO] - error_counter
 *
 */
#define PP_PRSR_MAX_HEADER_LEN_ERR_CNT_REG            ((PRSR_BASE_ADDR) + 0x0B4)
#define PP_PRSR_MAX_HEADER_LEN_ERR_CNT_ERROR_CNT_OFF                (0)
#define PP_PRSR_MAX_HEADER_LEN_ERR_CNT_ERROR_CNT_LEN                (32)
#define PP_PRSR_MAX_HEADER_LEN_ERR_CNT_ERROR_CNT_MSK                (0xFFFFFFFF)
#define PP_PRSR_MAX_HEADER_LEN_ERR_CNT_ERROR_CNT_RST                (0x0)

/**
 * SW_REG_NAME : PP_PRSR_PROTO_OVERFLOW_ERR_CNT_REG
 * HW_REG_NAME : proto_overflow_err_counter
 * DESCRIPTION : error_counter register, counts the numbers of
 *               errored packet
 *
 *  Register Fields :
 *   [31: 0][RO] - error_counter
 *
 */
#define PP_PRSR_PROTO_OVERFLOW_ERR_CNT_REG            ((PRSR_BASE_ADDR) + 0x0B8)
#define PP_PRSR_PROTO_OVERFLOW_ERR_CNT_ERROR_CNT_OFF                (0)
#define PP_PRSR_PROTO_OVERFLOW_ERR_CNT_ERROR_CNT_LEN                (32)
#define PP_PRSR_PROTO_OVERFLOW_ERR_CNT_ERROR_CNT_MSK                (0xFFFFFFFF)
#define PP_PRSR_PROTO_OVERFLOW_ERR_CNT_ERROR_CNT_RST                (0x0)

/**
 * SW_REG_NAME : PP_PRSR_IP_LEN_ERR_CNT_REG
 * HW_REG_NAME : ip_len_err_counter
 * DESCRIPTION : error_counter register, counts the numbers of
 *               errored packet
 *
 *  Register Fields :
 *   [31: 0][RO] - error_counter
 *
 */
#define PP_PRSR_IP_LEN_ERR_CNT_REG                    ((PRSR_BASE_ADDR) + 0x0BC)
#define PP_PRSR_IP_LEN_ERR_CNT_ERROR_CNT_OFF                        (0)
#define PP_PRSR_IP_LEN_ERR_CNT_ERROR_CNT_LEN                        (32)
#define PP_PRSR_IP_LEN_ERR_CNT_ERROR_CNT_MSK                        (0xFFFFFFFF)
#define PP_PRSR_IP_LEN_ERR_CNT_ERROR_CNT_RST                        (0x0)

/**
 * SW_REG_NAME : PP_PRSR_ABORT_CNT_ERR_CNT_REG
 * HW_REG_NAME : abort_cnt_err_counter
 * DESCRIPTION : error_counter register, counts the numbers of
 *               errored packet
 *
 *  Register Fields :
 *   [31: 0][RO] - error_counter
 *
 */
#define PP_PRSR_ABORT_CNT_ERR_CNT_REG                 ((PRSR_BASE_ADDR) + 0x0C0)
#define PP_PRSR_ABORT_CNT_ERR_CNT_ERROR_CNT_OFF                     (0)
#define PP_PRSR_ABORT_CNT_ERR_CNT_ERROR_CNT_LEN                     (32)
#define PP_PRSR_ABORT_CNT_ERR_CNT_ERROR_CNT_MSK                     (0xFFFFFFFF)
#define PP_PRSR_ABORT_CNT_ERR_CNT_ERROR_CNT_RST                     (0x0)

/**
 * SW_REG_NAME : PP_PRSR_EOF_NO_PAYLOAD_ERR_CNT_REG
 * HW_REG_NAME : eof_no_payload_err_counter
 * DESCRIPTION : error_counter register, counts the numbers of
 *               errored packet
 *
 *  Register Fields :
 *   [31: 0][RO] - error_counter
 *
 */
#define PP_PRSR_EOF_NO_PAYLOAD_ERR_CNT_REG            ((PRSR_BASE_ADDR) + 0x0C4)
#define PP_PRSR_EOF_NO_PAYLOAD_ERR_CNT_ERROR_CNT_OFF                (0)
#define PP_PRSR_EOF_NO_PAYLOAD_ERR_CNT_ERROR_CNT_LEN                (32)
#define PP_PRSR_EOF_NO_PAYLOAD_ERR_CNT_ERROR_CNT_MSK                (0xFFFFFFFF)
#define PP_PRSR_EOF_NO_PAYLOAD_ERR_CNT_ERROR_CNT_RST                (0x0)

/**
 * SW_REG_NAME : PP_PRSR_STATUS_RECORD_STATE_REG
 * HW_REG_NAME : status_record_state
 * DESCRIPTION : parser output status  state
 *
 *  Register Fields :
 *   [31:31][RW] - In case of error bit (in STW) is set this bit will
 *                 be set to '1' and the status will not be recorded
 *                 until it's cleared
 *   [30: 1][RO] - Reserved
 *   [ 0: 0][RO] - Is the last recorded status was sent to UC
 *
 */
#define PP_PRSR_STATUS_RECORD_STATE_REG               ((PRSR_BASE_ADDR) + 0x0C8)
#define PP_PRSR_STATUS_RECORD_STATE_STICKY_ERROR_OFF                (31)
#define PP_PRSR_STATUS_RECORD_STATE_STICKY_ERROR_LEN                (1)
#define PP_PRSR_STATUS_RECORD_STATE_STICKY_ERROR_MSK                (0x80000000)
#define PP_PRSR_STATUS_RECORD_STATE_STICKY_ERROR_RST                (0x0)
#define PP_PRSR_STATUS_RECORD_STATE_RSVD0_OFF                       (1)
#define PP_PRSR_STATUS_RECORD_STATE_RSVD0_LEN                       (30)
#define PP_PRSR_STATUS_RECORD_STATE_RSVD0_MSK                       (0x7FFFFFFE)
#define PP_PRSR_STATUS_RECORD_STATE_RSVD0_RST                       (0x0)
#define PP_PRSR_STATUS_RECORD_STATE_FALLBACK_OFF                    (0)
#define PP_PRSR_STATUS_RECORD_STATE_FALLBACK_LEN                    (1)
#define PP_PRSR_STATUS_RECORD_STATE_FALLBACK_MSK                    (0x00000001)
#define PP_PRSR_STATUS_RECORD_STATE_FALLBACK_RST                    (0x0)

/**
 * SW_REG_NAME : PP_PRSR_OP_ERROR_CNT_REG
 * HW_REG_NAME : op_error_counter
 * DESCRIPTION : error_counter register, counts the numbers of
 *               errored packet, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][RO] - error_counter
 *
 */
#define PP_PRSR_OP_ERROR_CNT_REG                      ((PRSR_BASE_ADDR) + 0x0CC)
#define PP_PRSR_OP_ERROR_CNT_OFF                                    (0)
#define PP_PRSR_OP_ERROR_CNT_LEN                                    (32)
#define PP_PRSR_OP_ERROR_CNT_MSK                                    (0xFFFFFFFF)
#define PP_PRSR_OP_ERROR_CNT_RST                                    (0x0)

/**
 * SW_REG_NAME : PP_PRSR_OP_PARSER_TO_UC_DROP_CNT_REG
 * HW_REG_NAME : op_parser_to_uc_drop_counter
 * DESCRIPTION : error_counter register, counts the numbers of
 *               errored packet, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][RO] - error_counter
 *
 */
#define PP_PRSR_OP_PARSER_TO_UC_DROP_CNT_REG          ((PRSR_BASE_ADDR) + 0x0D0)
#define PP_PRSR_OP_PARSER_TO_UC_DROP_CNT_ERROR_CNT_OFF              (0)
#define PP_PRSR_OP_PARSER_TO_UC_DROP_CNT_ERROR_CNT_LEN              (32)
#define PP_PRSR_OP_PARSER_TO_UC_DROP_CNT_ERROR_CNT_MSK              (0xFFFFFFFF)
#define PP_PRSR_OP_PARSER_TO_UC_DROP_CNT_ERROR_CNT_RST              (0x0)

/**
 * SW_REG_NAME : PP_PRSR_OP_PPRS_DROP_CNT_REG
 * HW_REG_NAME : op_pprs_drop_counter
 * DESCRIPTION : error_counter register, counts the numbers of
 *               errored packet, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][RO] - error_counter
 *
 */
#define PP_PRSR_OP_PPRS_DROP_CNT_REG                  ((PRSR_BASE_ADDR) + 0x0D4)
#define PP_PRSR_OP_PPRS_DROP_CNT_ERROR_CNT_OFF                      (0)
#define PP_PRSR_OP_PPRS_DROP_CNT_ERROR_CNT_LEN                      (32)
#define PP_PRSR_OP_PPRS_DROP_CNT_ERROR_CNT_MSK                      (0xFFFFFFFF)
#define PP_PRSR_OP_PPRS_DROP_CNT_ERROR_CNT_RST                      (0x0)

/**
 * SW_REG_NAME : PP_PRSR_OP_PARSER_DROP_CNT_REG
 * HW_REG_NAME : op_parser_drop_counter
 * DESCRIPTION : error_counter register, counts the numbers of
 *               errored packet, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][RO] - error_counter
 *
 */
#define PP_PRSR_OP_PARSER_DROP_CNT_REG                ((PRSR_BASE_ADDR) + 0x0D8)
#define PP_PRSR_OP_PARSER_DROP_CNT_ERROR_CNT_OFF                    (0)
#define PP_PRSR_OP_PARSER_DROP_CNT_ERROR_CNT_LEN                    (32)
#define PP_PRSR_OP_PARSER_DROP_CNT_ERROR_CNT_MSK                    (0xFFFFFFFF)
#define PP_PRSR_OP_PARSER_DROP_CNT_ERROR_CNT_RST                    (0x0)

/**
 * SW_REG_NAME : PP_PRSR_STATUS_RECORD_REG
 * HW_REG_NAME : status_record
 * DESCRIPTION : parser output fields, status and mask
 *
 *  Register Fields :
 *   [31: 0][RO] - parser output fields, status and mask
 *
 */
#define PP_PRSR_STATUS_RECORD_REG                     ((PRSR_BASE_ADDR) + 0x0DC)
#define PP_PRSR_STATUS_RECORD_OUTP_OFF                              (0)
#define PP_PRSR_STATUS_RECORD_OUTP_LEN                              (32)
#define PP_PRSR_STATUS_RECORD_OUTP_MSK                              (0xFFFFFFFF)
#define PP_PRSR_STATUS_RECORD_OUTP_RST                              (0x0)
/**
 * REG_IDX_ACCESS   : PP_PRSR_STATUS_RECORD_REG_IDX
 * NUM OF REGISTERS : 9
 */
#define PP_PRSR_STATUS_RECORD_REG_IDX(idx) \
	(PP_PRSR_STATUS_RECORD_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_PRSR_LY2_TYPE_LEN_REG
 * HW_REG_NAME : LY2_TYPE_LEN
 * DESCRIPTION : LY2 configuration
 *
 *  Register Fields :
 *   [31:31][RO] - Reserved
 *   [30:29][RW] - ly2 pit-table entries number
 *   [28:23][RW] - ly2 pit-table index
 *   [22:16][RW] - ly2 valid
 *   [15: 0][RW] - ly2 type
 *
 */
#define PP_PRSR_LY2_TYPE_LEN_REG                      ((PRSR_BASE_ADDR) + 0x100)
#define PP_PRSR_LY2_TYPE_LEN_RSVD0_OFF                              (31)
#define PP_PRSR_LY2_TYPE_LEN_RSVD0_LEN                              (1)
#define PP_PRSR_LY2_TYPE_LEN_RSVD0_MSK                              (0x80000000)
#define PP_PRSR_LY2_TYPE_LEN_RSVD0_RST                              (0x0)
#define PP_PRSR_LY2_TYPE_LEN_PIT_CNT_OFF                            (29)
#define PP_PRSR_LY2_TYPE_LEN_PIT_CNT_LEN                            (2)
#define PP_PRSR_LY2_TYPE_LEN_PIT_CNT_MSK                            (0x60000000)
#define PP_PRSR_LY2_TYPE_LEN_PIT_CNT_RST                            (0x1)
#define PP_PRSR_LY2_TYPE_LEN_PIT_IDX_OFF                            (23)
#define PP_PRSR_LY2_TYPE_LEN_PIT_IDX_LEN                            (6)
#define PP_PRSR_LY2_TYPE_LEN_PIT_IDX_MSK                            (0x1F800000)
#define PP_PRSR_LY2_TYPE_LEN_PIT_IDX_RST                            (0x1)
#define PP_PRSR_LY2_TYPE_LEN_LY2_LEN_OFF                            (16)
#define PP_PRSR_LY2_TYPE_LEN_LY2_LEN_LEN                            (7)
#define PP_PRSR_LY2_TYPE_LEN_LY2_LEN_MSK                            (0x007F0000)
#define PP_PRSR_LY2_TYPE_LEN_LY2_LEN_RST                            (0x2)
#define PP_PRSR_LY2_TYPE_LEN_OFF                                    (0)
#define PP_PRSR_LY2_TYPE_LEN_LEN                                    (16)
#define PP_PRSR_LY2_TYPE_LEN_MSK                                    (0x0000FFFF)
#define PP_PRSR_LY2_TYPE_LEN_RST                                    (0x600)
/**
 * REG_IDX_ACCESS   : PP_PRSR_LY2_TYPE_LEN_REG_IDX
 * NUM OF REGISTERS : 8
 */
#define PP_PRSR_LY2_TYPE_LEN_REG_IDX(idx) \
	(PP_PRSR_LY2_TYPE_LEN_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_PRSR_UNSUPPORTED_IPV6_NH_REG
 * HW_REG_NAME : unsupported_ipv6_nh
 * DESCRIPTION : When parsing ipv6 or ipv extension, which NH value
 *               is supported (0=supported, 1=unsupported), each
 *               bit represents a value of NH according to its
 *               index
 *
 *  Register Fields :
 *   [31: 0][RW] - Unsupported NH
 *
 */
#define PP_PRSR_UNSUPPORTED_IPV6_NH_REG               ((PRSR_BASE_ADDR) + 0x120)
#define PP_PRSR_UNSUPPORTED_IPV6_NH_UNSUPPORTED_NH_OFF              (0)
#define PP_PRSR_UNSUPPORTED_IPV6_NH_UNSUPPORTED_NH_LEN              (32)
#define PP_PRSR_UNSUPPORTED_IPV6_NH_UNSUPPORTED_NH_MSK              (0xFFFFFFFF)
#define PP_PRSR_UNSUPPORTED_IPV6_NH_UNSUPPORTED_NH_RST              (0x0)
/**
 * REG_IDX_ACCESS   : PP_PRSR_UNSUPPORTED_IPV6_NH_REG_IDX
 * NUM OF REGISTERS : 8
 */
#define PP_PRSR_UNSUPPORTED_IPV6_NH_REG_IDX(idx) \
	(PP_PRSR_UNSUPPORTED_IPV6_NH_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_PRSR_REDUCER_DONT_CARE_PC_REG
 * HW_REG_NAME : reducer_dont_care_pc
 * DESCRIPTION : PC values that represent specail protocol that has
 *               to be distinguished (for example IPV6 or
 *               IPV6-extension protocols)
 *
 *  Register Fields :
 *   [31: 0][RW] - Imem PC
 *
 */
#define PP_PRSR_REDUCER_DONT_CARE_PC_REG              ((PRSR_BASE_ADDR) + 0x150)
#define PP_PRSR_REDUCER_DONT_CARE_PC_IMEM_PC_OFF                    (0)
#define PP_PRSR_REDUCER_DONT_CARE_PC_IMEM_PC_LEN                    (32)
#define PP_PRSR_REDUCER_DONT_CARE_PC_IMEM_PC_MSK                    (0xFFFFFFFF)
#define PP_PRSR_REDUCER_DONT_CARE_PC_IMEM_PC_RST                    (0x30000000)
/**
 * REG_IDX_ACCESS   : PP_PRSR_REDUCER_DONT_CARE_PC_REG_IDX
 * NUM OF REGISTERS : 2
 */
#define PP_PRSR_REDUCER_DONT_CARE_PC_REG_IDX(idx) \
	(PP_PRSR_REDUCER_DONT_CARE_PC_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_PRSR_LAST_ETHERTYPE_CFG_REG
 * HW_REG_NAME : last_ethertype_cfg
 * DESCRIPTION : LY3 type, valid and PC
 *
 *  Register Fields :
 *   [31:23][RO] - Reserved
 *   [22:17][RW] - Ly3 protocol code for given etherType
 *   [16:16][RW] - EtherType valid
 *   [15: 0][RW] - EtherType value
 *
 */
#define PP_PRSR_LAST_ETHERTYPE_CFG_REG                ((PRSR_BASE_ADDR) + 0x180)
#define PP_PRSR_LAST_ETHERTYPE_CFG_RSVD0_OFF                        (23)
#define PP_PRSR_LAST_ETHERTYPE_CFG_RSVD0_LEN                        (9)
#define PP_PRSR_LAST_ETHERTYPE_CFG_RSVD0_MSK                        (0xFF800000)
#define PP_PRSR_LAST_ETHERTYPE_CFG_RSVD0_RST                        (0x0)
#define PP_PRSR_LAST_ETHERTYPE_CFG_LY3_PC_OFF                       (17)
#define PP_PRSR_LAST_ETHERTYPE_CFG_LY3_PC_LEN                       (6)
#define PP_PRSR_LAST_ETHERTYPE_CFG_LY3_PC_MSK                       (0x007E0000)
#define PP_PRSR_LAST_ETHERTYPE_CFG_LY3_PC_RST                       (0x0)
#define PP_PRSR_LAST_ETHERTYPE_CFG_ET_IS_VALID_OFF                  (16)
#define PP_PRSR_LAST_ETHERTYPE_CFG_ET_IS_VALID_LEN                  (1)
#define PP_PRSR_LAST_ETHERTYPE_CFG_ET_IS_VALID_MSK                  (0x00010000)
#define PP_PRSR_LAST_ETHERTYPE_CFG_ET_IS_VALID_RST                  (0x1)
#define PP_PRSR_LAST_ETHERTYPE_CFG_ET_VALUE_OFF                     (0)
#define PP_PRSR_LAST_ETHERTYPE_CFG_ET_VALUE_LEN                     (16)
#define PP_PRSR_LAST_ETHERTYPE_CFG_ET_VALUE_MSK                     (0x0000FFFF)
#define PP_PRSR_LAST_ETHERTYPE_CFG_ET_VALUE_RST                     (0x88f7)
/**
 * REG_IDX_ACCESS   : PP_PRSR_LAST_ETHERTYPE_CFG_REG_IDX
 * NUM OF REGISTERS : 16
 */
#define PP_PRSR_LAST_ETHERTYPE_CFG_REG_IDX(idx) \
	(PP_PRSR_LAST_ETHERTYPE_CFG_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_PRSR_PIT_FV_MASK_REG
 * HW_REG_NAME : pit_fv_mask
 * DESCRIPTION : Mask for pit
 *
 *  Register Fields :
 *   [31: 0][RW] - Pit field vector mask
 *
 */
#define PP_PRSR_PIT_FV_MASK_REG                       ((PRSR_BASE_ADDR) + 0x200)
#define PP_PRSR_PIT_FV_MASK_OFF                                     (0)
#define PP_PRSR_PIT_FV_MASK_LEN                                     (32)
#define PP_PRSR_PIT_FV_MASK_MSK                                     (0xFFFFFFFF)
#define PP_PRSR_PIT_FV_MASK_RST                                     (0x0)
/**
 * REG_IDX_ACCESS   : PP_PRSR_PIT_FV_MASK_REG_IDX
 * NUM OF REGISTERS : 64
 */
#define PP_PRSR_PIT_FV_MASK_REG_IDX(idx) \
	(PP_PRSR_PIT_FV_MASK_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_PRSR_PIT_REG
 * HW_REG_NAME : pit
 * DESCRIPTION : Parsing Information Table
 *
 *  Register Fields :
 *   [31:16][RO] - Reserved
 *   [15:10][RW] - Field vector target index
 *   [ 9: 5][RW] - The size of data to extract to field vector
 *   [ 4: 0][RW] - The offset from the start of protocol to start
 *                 extracting
 *
 */
#define PP_PRSR_PIT_REG                               ((PRSR_BASE_ADDR) + 0x300)
#define PP_PRSR_PIT_RSVD0_OFF                                       (16)
#define PP_PRSR_PIT_RSVD0_LEN                                       (16)
#define PP_PRSR_PIT_RSVD0_MSK                                       (0xFFFF0000)
#define PP_PRSR_PIT_RSVD0_RST                                       (0x0)
#define PP_PRSR_PIT_TARGET_IDX_OFF                                  (10)
#define PP_PRSR_PIT_TARGET_IDX_LEN                                  (6)
#define PP_PRSR_PIT_TARGET_IDX_MSK                                  (0x0000FC00)
#define PP_PRSR_PIT_TARGET_IDX_RST                                  (0x2)
#define PP_PRSR_PIT_DATA_LEN_OFF                                    (5)
#define PP_PRSR_PIT_DATA_LEN_LEN                                    (5)
#define PP_PRSR_PIT_DATA_LEN_MSK                                    (0x000003E0)
#define PP_PRSR_PIT_DATA_LEN_RST                                    (0x6)
#define PP_PRSR_PIT_DATA_OFFSET_OFF                                 (0)
#define PP_PRSR_PIT_DATA_OFFSET_LEN                                 (5)
#define PP_PRSR_PIT_DATA_OFFSET_MSK                                 (0x0000001F)
#define PP_PRSR_PIT_DATA_OFFSET_RST                                 (0x0)
/**
 * REG_IDX_ACCESS   : PP_PRSR_PIT_REG_IDX
 * NUM OF REGISTERS : 64
 */
#define PP_PRSR_PIT_REG_IDX(idx) \
	(PP_PRSR_PIT_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_PRSR_PORT_CFG_REG
 * HW_REG_NAME : port_config
 * DESCRIPTION : Configuration per port
 *
 *  Register Fields :
 *   [31:31][RW] - Port fallback, if asserted fields and status of
 *                 this port will be sent to UC instead of Classifier
 *   [30:21][RO] - Reserved
 *   [20:16][RW] - Port len_subtract, Parser compares the len in
 *                 IPV4/6 with the len it gets from RPB, the value of
 *                 this field will be subtrackted from RPB value
 *                 before comparison, NOTE, B Step ONLY field
 *   [15: 8][RO] - Reserved, NOTE, B Step ONLY field
 *   [ 7: 6][RW] - Port controlled Field Vector mask - 00: Mask as
 *                 received, b01: shared_mask_0, b10: shared_mask_1,
 *                 b11: disable mask, NOTE, B Step ONLY field
 *   [ 5: 0][RW] - Skip ly2 PC, the PC to L3 in case ly2 is skipped
 *
 */
#define PP_PRSR_PORT_CFG_REG                          ((PRSR_BASE_ADDR) + 0x400)
#define PP_PRSR_PORT_CFG_PORT_FALLBACK_OFF                          (31)
#define PP_PRSR_PORT_CFG_PORT_FALLBACK_LEN                          (1)
#define PP_PRSR_PORT_CFG_PORT_FALLBACK_MSK                          (0x80000000)
#define PP_PRSR_PORT_CFG_PORT_FALLBACK_RST                          (0x0)
#define PP_PRSR_PORT_CFG_RSVD0_OFF                                  (21)
#define PP_PRSR_PORT_CFG_RSVD0_LEN                                  (10)
#define PP_PRSR_PORT_CFG_RSVD0_MSK                                  (0x7FE00000)
#define PP_PRSR_PORT_CFG_RSVD0_RST                                  (0x0)
#define PP_PRSR_PORT_CFG_PKT_LEN_SUBTRACT_OFF                       (16)
#define PP_PRSR_PORT_CFG_PKT_LEN_SUBTRACT_LEN                       (5)
#define PP_PRSR_PORT_CFG_PKT_LEN_SUBTRACT_MSK                       (0x001F0000)
#define PP_PRSR_PORT_CFG_PKT_LEN_SUBTRACT_RST                       (0x0)
#define PP_PRSR_PORT_CFG_RSVD1_OFF                                  (8)
#define PP_PRSR_PORT_CFG_RSVD1_LEN                                  (8)
#define PP_PRSR_PORT_CFG_RSVD1_MSK                                  (0x0000FF00)
#define PP_PRSR_PORT_CFG_RSVD1_RST                                  (0x0)
#define PP_PRSR_PORT_CFG_FVM_CTRL_OFF                               (6)
#define PP_PRSR_PORT_CFG_FVM_CTRL_LEN                               (2)
#define PP_PRSR_PORT_CFG_FVM_CTRL_MSK                               (0x000000C0)
#define PP_PRSR_PORT_CFG_FVM_CTRL_RST                               (0x0)
#define PP_PRSR_PORT_CFG_SKIP_LY2_PC_OFF                            (0)
#define PP_PRSR_PORT_CFG_SKIP_LY2_PC_LEN                            (6)
#define PP_PRSR_PORT_CFG_SKIP_LY2_PC_MSK                            (0x0000003F)
#define PP_PRSR_PORT_CFG_SKIP_LY2_PC_RST                            (0x3f)
/**
 * REG_IDX_ACCESS   : PP_PRSR_PORT_CFG_REG_IDX
 * NUM OF REGISTERS : 256
 */
#define PP_PRSR_PORT_CFG_REG_IDX(idx) \
	(PP_PRSR_PORT_CFG_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_PRSR_IMEM_ENTRY_REG
 * HW_REG_NAME : imem_entry_desc
 * DESCRIPTION : description of an imem entry, used in buildin out-
 *               of-band information (2 first bytes in FV and 8
 *               bytes in status)
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19:14][RW] - Replacement for the entry, for similar protocols,
 *                 the fallback is base on the original protocol
 *                 while the analyse and the pit pointer is based on
 *                 the replacement
 *   [13:13][RW] - Is replacement for the entry valid
 *   [12:11][RW] - ly3 pit-table entries number
 *   [10: 5][RW] - ly3 pit-table index
 *   [ 4: 1][RW] - Protocol type that the entry represents, 0-NONE,
 *                 1-IPV4, 2-IPV6, 3-OTHER_L3, 4-UDP, 5-TCP,
 *                 6-RESERVED_L4, 7-OTHER_L4, 8-L2, 9-PAYLOAD,
 *                 A-IPV6_FRAG, B-IPV6_EXT (other than FRAG), C-F
 *                 reserved
 *   [ 0: 0][RW] - Asserted if hitting this entry in IMEM table
 *                 should cause the entire fields, mask and status to
 *                 be driven to UC instead of Classifier
 *
 */
#define PP_PRSR_IMEM_ENTRY_REG                        ((PRSR_BASE_ADDR) + 0x800)
#define PP_PRSR_IMEM_ENTRY_RSVD0_OFF                                (20)
#define PP_PRSR_IMEM_ENTRY_RSVD0_LEN                                (12)
#define PP_PRSR_IMEM_ENTRY_RSVD0_MSK                                (0xFFF00000)
#define PP_PRSR_IMEM_ENTRY_RSVD0_RST                                (0x0)
#define PP_PRSR_IMEM_ENTRY_REPLACEMENT_OFF                          (14)
#define PP_PRSR_IMEM_ENTRY_REPLACEMENT_LEN                          (6)
#define PP_PRSR_IMEM_ENTRY_REPLACEMENT_MSK                          (0x000FC000)
#define PP_PRSR_IMEM_ENTRY_REPLACEMENT_RST                          (0x0)
#define PP_PRSR_IMEM_ENTRY_REPLACEMENT_VLD_OFF                      (13)
#define PP_PRSR_IMEM_ENTRY_REPLACEMENT_VLD_LEN                      (1)
#define PP_PRSR_IMEM_ENTRY_REPLACEMENT_VLD_MSK                      (0x00002000)
#define PP_PRSR_IMEM_ENTRY_REPLACEMENT_VLD_RST                      (0x1)
#define PP_PRSR_IMEM_ENTRY_PIT_CNT_OFF                              (11)
#define PP_PRSR_IMEM_ENTRY_PIT_CNT_LEN                              (2)
#define PP_PRSR_IMEM_ENTRY_PIT_CNT_MSK                              (0x00001800)
#define PP_PRSR_IMEM_ENTRY_PIT_CNT_RST                              (0x0)
#define PP_PRSR_IMEM_ENTRY_PIT_IDX_OFF                              (5)
#define PP_PRSR_IMEM_ENTRY_PIT_IDX_LEN                              (6)
#define PP_PRSR_IMEM_ENTRY_PIT_IDX_MSK                              (0x000007E0)
#define PP_PRSR_IMEM_ENTRY_PIT_IDX_RST                              (0x0)
#define PP_PRSR_IMEM_ENTRY_ENTRY_TYPE_OFF                           (1)
#define PP_PRSR_IMEM_ENTRY_ENTRY_TYPE_LEN                           (4)
#define PP_PRSR_IMEM_ENTRY_ENTRY_TYPE_MSK                           (0x0000001E)
#define PP_PRSR_IMEM_ENTRY_ENTRY_TYPE_RST                           (0x0)
#define PP_PRSR_IMEM_ENTRY_FALLBACK_OFF                             (0)
#define PP_PRSR_IMEM_ENTRY_FALLBACK_LEN                             (1)
#define PP_PRSR_IMEM_ENTRY_FALLBACK_MSK                             (0x00000001)
#define PP_PRSR_IMEM_ENTRY_FALLBACK_RST                             (0x0)
/**
 * REG_IDX_ACCESS   : PP_PRSR_IMEM_ENTRY_REG_IDX
 * NUM OF REGISTERS : 64
 */
#define PP_PRSR_IMEM_ENTRY_REG_IDX(idx) \
	(PP_PRSR_IMEM_ENTRY_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_PRSR_IMEM0_REG
 * HW_REG_NAME : imem_reg0
 * DESCRIPTION : IMEM register
 *
 *  Register Fields :
 *   [31:29][RO] - Reserved
 *   [28:21][RW] - MISSING_DESCRIPTION
 *   [20:16][RW] - MISSING_DESCRIPTION
 *   [15:13][RW] - MISSING_DESCRIPTION
 *   [12: 5][RW] - MISSING_DESCRIPTION
 *   [ 4: 2][RW] - MISSING_DESCRIPTION
 *   [ 1: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_PRSR_IMEM0_REG                             ((PRSR_BASE_ADDR) + 0x900)
#define PP_PRSR_IMEM0_RSVD0_OFF                                     (29)
#define PP_PRSR_IMEM0_RSVD0_LEN                                     (3)
#define PP_PRSR_IMEM0_RSVD0_MSK                                     (0xE0000000)
#define PP_PRSR_IMEM0_RSVD0_RST                                     (0x0)
#define PP_PRSR_IMEM0_FLD1_SEL_OFF                                  (21)
#define PP_PRSR_IMEM0_FLD1_SEL_LEN                                  (8)
#define PP_PRSR_IMEM0_FLD1_SEL_MSK                                  (0x1FE00000)
#define PP_PRSR_IMEM0_FLD1_SEL_RST                                  (0x0)
#define PP_PRSR_IMEM0_FLD1_WIDTH_OFF                                (16)
#define PP_PRSR_IMEM0_FLD1_WIDTH_LEN                                (5)
#define PP_PRSR_IMEM0_FLD1_WIDTH_MSK                                (0x001F0000)
#define PP_PRSR_IMEM0_FLD1_WIDTH_RST                                (0x0)
#define PP_PRSR_IMEM0_SKIP_OPCODE_OFF                               (13)
#define PP_PRSR_IMEM0_SKIP_OPCODE_LEN                               (3)
#define PP_PRSR_IMEM0_SKIP_OPCODE_MSK                               (0x0000E000)
#define PP_PRSR_IMEM0_SKIP_OPCODE_RST                               (0x0)
#define PP_PRSR_IMEM0_IM1_OFF                                       (5)
#define PP_PRSR_IMEM0_IM1_LEN                                       (8)
#define PP_PRSR_IMEM0_IM1_MSK                                       (0x00001FE0)
#define PP_PRSR_IMEM0_IM1_RST                                       (0x0)
#define PP_PRSR_IMEM0_NP_SEL_OFF                                    (2)
#define PP_PRSR_IMEM0_NP_SEL_LEN                                    (3)
#define PP_PRSR_IMEM0_NP_SEL_MSK                                    (0x0000001C)
#define PP_PRSR_IMEM0_NP_SEL_RST                                    (0x7)
#define PP_PRSR_IMEM0_SKIP_SEL_OFF                                  (0)
#define PP_PRSR_IMEM0_SKIP_SEL_LEN                                  (2)
#define PP_PRSR_IMEM0_SKIP_SEL_MSK                                  (0x00000003)
#define PP_PRSR_IMEM0_SKIP_SEL_RST                                  (0x2)
/**
 * REG_IDX_ACCESS   : PP_PRSR_IMEM0_REG_IDX
 * NUM OF REGISTERS : 64
 */
#define PP_PRSR_IMEM0_REG_IDX(idx) \
	(PP_PRSR_IMEM0_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_PRSR_IMEM1_REG
 * HW_REG_NAME : imem_reg1
 * DESCRIPTION : IMEM register
 *
 *  Register Fields :
 *   [31:23][RO] - Reserved
 *   [22:19][RW] - MISSING_DESCRIPTION
 *   [18:11][RW] - MISSING_DESCRIPTION
 *   [10: 6][RW] - MISSING_DESCRIPTION
 *   [ 5: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_PRSR_IMEM1_REG                            ((PRSR_BASE_ADDR) + 0x0A00)
#define PP_PRSR_IMEM1_RSVD0_OFF                                     (23)
#define PP_PRSR_IMEM1_RSVD0_LEN                                     (9)
#define PP_PRSR_IMEM1_RSVD0_MSK                                     (0xFF800000)
#define PP_PRSR_IMEM1_RSVD0_RST                                     (0x0)
#define PP_PRSR_IMEM1_HEADER_LENGTH_OFF                             (19)
#define PP_PRSR_IMEM1_HEADER_LENGTH_LEN                             (4)
#define PP_PRSR_IMEM1_HEADER_LENGTH_MSK                             (0x00780000)
#define PP_PRSR_IMEM1_HEADER_LENGTH_RST                             (0x0)
#define PP_PRSR_IMEM1_FLD0_SEL_OFF                                  (11)
#define PP_PRSR_IMEM1_FLD0_SEL_LEN                                  (8)
#define PP_PRSR_IMEM1_FLD0_SEL_MSK                                  (0x0007F800)
#define PP_PRSR_IMEM1_FLD0_SEL_RST                                  (0x0)
#define PP_PRSR_IMEM1_FLD0_WIDTH_OFF                                (6)
#define PP_PRSR_IMEM1_FLD0_WIDTH_LEN                                (5)
#define PP_PRSR_IMEM1_FLD0_WIDTH_MSK                                (0x000007C0)
#define PP_PRSR_IMEM1_FLD0_WIDTH_RST                                (0x0)
#define PP_PRSR_IMEM1_IM0_OFF                                       (0)
#define PP_PRSR_IMEM1_IM0_LEN                                       (6)
#define PP_PRSR_IMEM1_IM0_MSK                                       (0x0000003F)
#define PP_PRSR_IMEM1_IM0_RST                                       (0x30)
/**
 * REG_IDX_ACCESS   : PP_PRSR_IMEM1_REG_IDX
 * NUM OF REGISTERS : 64
 */
#define PP_PRSR_IMEM1_REG_IDX(idx) \
	(PP_PRSR_IMEM1_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_PRSR_NP_SEL_REG
 * HW_REG_NAME : np_sel
 * DESCRIPTION : IMEM next protocol select register
 *
 *  Register Fields :
 *   [31:27][RO] - Reserved
 *   [26: 7][RW] - Protocol key
 *   [ 6: 1][RW] - next protocol
 *   [ 0: 0][RW] - Is using np
 *
 */
#define PP_PRSR_NP_SEL_REG                           ((PRSR_BASE_ADDR) + 0x0B00)
#define PP_PRSR_NP_SEL_RSVD0_OFF                                    (27)
#define PP_PRSR_NP_SEL_RSVD0_LEN                                    (5)
#define PP_PRSR_NP_SEL_RSVD0_MSK                                    (0xF8000000)
#define PP_PRSR_NP_SEL_RSVD0_RST                                    (0x0)
#define PP_PRSR_NP_SEL_KEY_OFF                                      (7)
#define PP_PRSR_NP_SEL_KEY_LEN                                      (20)
#define PP_PRSR_NP_SEL_KEY_MSK                                      (0x07FFFF80)
#define PP_PRSR_NP_SEL_KEY_RST                                      (0x30004)
#define PP_PRSR_NP_SEL_VALUE_OFF                                    (1)
#define PP_PRSR_NP_SEL_VALUE_LEN                                    (6)
#define PP_PRSR_NP_SEL_VALUE_MSK                                    (0x0000007E)
#define PP_PRSR_NP_SEL_VALUE_RST                                    (0x1)
#define PP_PRSR_NP_SEL_VALID_OFF                                    (0)
#define PP_PRSR_NP_SEL_VALID_LEN                                    (1)
#define PP_PRSR_NP_SEL_VALID_MSK                                    (0x00000001)
#define PP_PRSR_NP_SEL_VALID_RST                                    (0x1)
/**
 * REG_IDX_ACCESS   : PP_PRSR_NP_SEL_REG_IDX
 * NUM OF REGISTERS : 64
 */
#define PP_PRSR_NP_SEL_REG_IDX(idx) \
	(PP_PRSR_NP_SEL_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_PRSR_SKIP_LU_REG
 * HW_REG_NAME : skip_lu
 * DESCRIPTION : Imem skip lookup table register
 *
 *  Register Fields :
 *   [31:28][RO] - Reserved
 *   [27: 9][RW] - Protocol key
 *   [ 8: 1][RW] - Skip value
 *   [ 0: 0][RW] - Is valid
 *
 */
#define PP_PRSR_SKIP_LU_REG                          ((PRSR_BASE_ADDR) + 0x0C00)
#define PP_PRSR_SKIP_LU_RSVD0_OFF                                   (28)
#define PP_PRSR_SKIP_LU_RSVD0_LEN                                   (4)
#define PP_PRSR_SKIP_LU_RSVD0_MSK                                   (0xF0000000)
#define PP_PRSR_SKIP_LU_RSVD0_RST                                   (0x0)
#define PP_PRSR_SKIP_LU_KEY_OFF                                     (9)
#define PP_PRSR_SKIP_LU_KEY_LEN                                     (19)
#define PP_PRSR_SKIP_LU_KEY_MSK                                     (0x0FFFFE00)
#define PP_PRSR_SKIP_LU_KEY_RST                                     (0x588a8)
#define PP_PRSR_SKIP_LU_VALUE_OFF                                   (1)
#define PP_PRSR_SKIP_LU_VALUE_LEN                                   (8)
#define PP_PRSR_SKIP_LU_VALUE_MSK                                   (0x000001FE)
#define PP_PRSR_SKIP_LU_VALUE_RST                                   (0xc)
#define PP_PRSR_SKIP_LU_VALID_OFF                                   (0)
#define PP_PRSR_SKIP_LU_VALID_LEN                                   (1)
#define PP_PRSR_SKIP_LU_VALID_MSK                                   (0x00000001)
#define PP_PRSR_SKIP_LU_VALID_RST                                   (0x1)
/**
 * REG_IDX_ACCESS   : PP_PRSR_SKIP_LU_REG_IDX
 * NUM OF REGISTERS : 32
 */
#define PP_PRSR_SKIP_LU_REG_IDX(idx) \
	(PP_PRSR_SKIP_LU_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_PRSR_MERGE_PC_REG
 * HW_REG_NAME : merge_pc
 * DESCRIPTION : Imem merge protocol code register
 *
 *  Register Fields :
 *   [31:19][RO] - Reserved
 *   [18:13][RW] - Previous Protocol value
 *   [12: 7][RW] - Current Protocol value
 *   [ 6: 1][RW] - Merged protocol value
 *   [ 0: 0][RW] - Is valid
 *
 */
#define PP_PRSR_MERGE_PC_REG                         ((PRSR_BASE_ADDR) + 0x0C80)
#define PP_PRSR_MERGE_PC_RSVD0_OFF                                  (19)
#define PP_PRSR_MERGE_PC_RSVD0_LEN                                  (13)
#define PP_PRSR_MERGE_PC_RSVD0_MSK                                  (0xFFF80000)
#define PP_PRSR_MERGE_PC_RSVD0_RST                                  (0x0)
#define PP_PRSR_MERGE_PC_OFF                                        (13)
#define PP_PRSR_MERGE_PC_LEN                                        (6)
#define PP_PRSR_MERGE_PC_MSK                                        (0x0007E000)
#define PP_PRSR_MERGE_PC_RST                                        (0x2a)
#define PP_PRSR_MERGE_PC_CP_OFF                                     (7)
#define PP_PRSR_MERGE_PC_CP_LEN                                     (6)
#define PP_PRSR_MERGE_PC_CP_MSK                                     (0x00001F80)
#define PP_PRSR_MERGE_PC_CP_RST                                     (0x2b)
#define PP_PRSR_MERGE_PC_MP_OFF                                     (1)
#define PP_PRSR_MERGE_PC_MP_LEN                                     (6)
#define PP_PRSR_MERGE_PC_MP_MSK                                     (0x0000007E)
#define PP_PRSR_MERGE_PC_MP_RST                                     (0x2b)
#define PP_PRSR_MERGE_PC_VALID_OFF                                  (0)
#define PP_PRSR_MERGE_PC_VALID_LEN                                  (1)
#define PP_PRSR_MERGE_PC_VALID_MSK                                  (0x00000001)
#define PP_PRSR_MERGE_PC_VALID_RST                                  (0x1)
/**
 * REG_IDX_ACCESS   : PP_PRSR_MERGE_PC_REG_IDX
 * NUM OF REGISTERS : 16
 */
#define PP_PRSR_MERGE_PC_REG_IDX(idx) \
	(PP_PRSR_MERGE_PC_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_PRSR_IMEM_PROT_LEN_REG
 * HW_REG_NAME : imem_prot_len
 * DESCRIPTION : Minimum and maximum length of a protocol
 *
 *  Register Fields :
 *   [31:21][RO] - Reserved
 *   [20:13][RW] - Maximum length of the protocol represented in this
 *                 IMEM entry
 *   [12:11][RO] - Reserved
 *   [10: 3][RW] - Minimum length of the protocol represented in this
 *                 IMEM entry
 *   [ 2: 1][RO] - Reserved
 *   [ 0: 0][RW] - Is lecgth checker enable, if yes and a protocol is
 *                 failed this check the packet will be treated as
 *                 aborted packet
 *
 */
#define PP_PRSR_IMEM_PROT_LEN_REG                    ((PRSR_BASE_ADDR) + 0x0D00)
#define PP_PRSR_IMEM_PROT_LEN_RSVD0_OFF                             (21)
#define PP_PRSR_IMEM_PROT_LEN_RSVD0_LEN                             (11)
#define PP_PRSR_IMEM_PROT_LEN_RSVD0_MSK                             (0xFFE00000)
#define PP_PRSR_IMEM_PROT_LEN_RSVD0_RST                             (0x0)
#define PP_PRSR_IMEM_PROT_LEN_MAX_LEN_OFF                           (13)
#define PP_PRSR_IMEM_PROT_LEN_MAX_LEN_LEN                           (8)
#define PP_PRSR_IMEM_PROT_LEN_MAX_LEN_MSK                           (0x001FE000)
#define PP_PRSR_IMEM_PROT_LEN_MAX_LEN_RST                           (0x0)
#define PP_PRSR_IMEM_PROT_LEN_RSVD1_OFF                             (11)
#define PP_PRSR_IMEM_PROT_LEN_RSVD1_LEN                             (2)
#define PP_PRSR_IMEM_PROT_LEN_RSVD1_MSK                             (0x00001800)
#define PP_PRSR_IMEM_PROT_LEN_RSVD1_RST                             (0x0)
#define PP_PRSR_IMEM_PROT_LEN_MIN_LEN_OFF                           (3)
#define PP_PRSR_IMEM_PROT_LEN_MIN_LEN_LEN                           (8)
#define PP_PRSR_IMEM_PROT_LEN_MIN_LEN_MSK                           (0x000007F8)
#define PP_PRSR_IMEM_PROT_LEN_MIN_LEN_RST                           (0x0)
#define PP_PRSR_IMEM_PROT_LEN_RSVD2_OFF                             (1)
#define PP_PRSR_IMEM_PROT_LEN_RSVD2_LEN                             (2)
#define PP_PRSR_IMEM_PROT_LEN_RSVD2_MSK                             (0x00000006)
#define PP_PRSR_IMEM_PROT_LEN_RSVD2_RST                             (0x0)
#define PP_PRSR_IMEM_PROT_LEN_CHCK_EN_OFF                           (0)
#define PP_PRSR_IMEM_PROT_LEN_CHCK_EN_LEN                           (1)
#define PP_PRSR_IMEM_PROT_LEN_CHCK_EN_MSK                           (0x00000001)
#define PP_PRSR_IMEM_PROT_LEN_CHCK_EN_RST                           (0x0)
/**
 * REG_IDX_ACCESS   : PP_PRSR_IMEM_PROT_LEN_REG_IDX
 * NUM OF REGISTERS : 64
 */
#define PP_PRSR_IMEM_PROT_LEN_REG_IDX(idx) \
	(PP_PRSR_IMEM_PROT_LEN_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_PRSR_FIELDS_RECORD_REG
 * HW_REG_NAME : fields_record
 * DESCRIPTION : parser output fields, status and mask
 *
 *  Register Fields :
 *   [31: 0][RO] - parser output fields, status and mask
 *
 */
#define PP_PRSR_FIELDS_RECORD_REG                    ((PRSR_BASE_ADDR) + 0x0E00)
#define PP_PRSR_FIELDS_RECORD_OUTP_OFF                              (0)
#define PP_PRSR_FIELDS_RECORD_OUTP_LEN                              (32)
#define PP_PRSR_FIELDS_RECORD_OUTP_MSK                              (0xFFFFFFFF)
#define PP_PRSR_FIELDS_RECORD_OUTP_RST                              (0x0)
/**
 * REG_IDX_ACCESS   : PP_PRSR_FIELDS_RECORD_REG_IDX
 * NUM OF REGISTERS : 32
 */
#define PP_PRSR_FIELDS_RECORD_REG_IDX(idx) \
	(PP_PRSR_FIELDS_RECORD_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_PRSR_MASK_RECORD_REG
 * HW_REG_NAME : mask_record
 * DESCRIPTION : parser output fields, status and mask
 *
 *  Register Fields :
 *   [31: 0][RO] - parser output fields, status and mask
 *
 */
#define PP_PRSR_MASK_RECORD_REG                      ((PRSR_BASE_ADDR) + 0x0E80)
#define PP_PRSR_MASK_RECORD_OUTP_OFF                                (0)
#define PP_PRSR_MASK_RECORD_OUTP_LEN                                (32)
#define PP_PRSR_MASK_RECORD_OUTP_MSK                                (0xFFFFFFFF)
#define PP_PRSR_MASK_RECORD_OUTP_RST                                (0x0)
/**
 * REG_IDX_ACCESS   : PP_PRSR_MASK_RECORD_REG_IDX
 * NUM OF REGISTERS : 4
 */
#define PP_PRSR_MASK_RECORD_REG_IDX(idx) \
	(PP_PRSR_MASK_RECORD_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_PRSR_SHARED_FVM_0_REG
 * HW_REG_NAME : shared_fvm_0
 * DESCRIPTION : Shared FV Mask, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][RW] - Shared FVM for ports that are configured to have
 *                 it (list of 4 instances)
 *
 */
#define PP_PRSR_SHARED_FVM_0_REG                     ((PRSR_BASE_ADDR) + 0x0E90)
#define PP_PRSR_SHARED_FVM_0_OFF                                    (0)
#define PP_PRSR_SHARED_FVM_0_LEN                                    (32)
#define PP_PRSR_SHARED_FVM_0_MSK                                    (0xFFFFFFFF)
#define PP_PRSR_SHARED_FVM_0_RST                                    (0x0)
/**
 * REG_IDX_ACCESS   : PP_PRSR_SHARED_FVM_0_REG_IDX
 * NUM OF REGISTERS : 4
 */
#define PP_PRSR_SHARED_FVM_0_REG_IDX(idx) \
	(PP_PRSR_SHARED_FVM_0_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_PRSR_SHARED_FVM_1_REG
 * HW_REG_NAME : shared_fvm_1
 * DESCRIPTION : Shared FV Mask, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][RW] - Shared FVM for ports that are configured to have
 *                 it (list of 4 instances)
 *
 */
#define PP_PRSR_SHARED_FVM_1_REG                     ((PRSR_BASE_ADDR) + 0x0EA0)
#define PP_PRSR_SHARED_FVM_1_OFF                                    (0)
#define PP_PRSR_SHARED_FVM_1_LEN                                    (32)
#define PP_PRSR_SHARED_FVM_1_MSK                                    (0xFFFFFFFF)
#define PP_PRSR_SHARED_FVM_1_RST                                    (0x0)
/**
 * REG_IDX_ACCESS   : PP_PRSR_SHARED_FVM_1_REG_IDX
 * NUM OF REGISTERS : 4
 */
#define PP_PRSR_SHARED_FVM_1_REG_IDX(idx) \
	(PP_PRSR_SHARED_FVM_1_REG + ((idx) << 2))

#endif
