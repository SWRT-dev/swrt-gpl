/**
 * clk_ctrl_regs.h
 * Description: ppv4_clk_ctrl_s HW registers definitions
 * Auto-generated file by RDL Parser, DO NOT CHANGE
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2019-2020 Intel Corporation
 */

#ifndef _PP_CLK_CTRL_H_
#define _PP_CLK_CTRL_H_

#define PP_CLK_CTRL_GEN_DATE_STR              "Sunday May 17, 2020 [5:26:11 pm]"
#define PP_CLK_CTRL_BASE                                         (0xF0001000ULL)

/**
 * SW_REG_NAME : PP_CLK_CTRL_INFRA_EN_REG
 * HW_REG_NAME : infra_enable
 * DESCRIPTION : TBD
 *
 *  Register Fields :
 *   [31: 2][RO] - Reserved
 *   [ 1: 0][RW] - Configured Module state : 00 - Disable ; 01 - Sync
 *                 Rst ; 10 - Clk Disable ; 11 - Enable
 *
 */
#define PP_CLK_CTRL_INFRA_EN_REG                    ((CLK_CTRL_BASE_ADDR) + 0x0)
#define PP_CLK_CTRL_INFRA_EN_RSVD0_OFF                              (2)
#define PP_CLK_CTRL_INFRA_EN_RSVD0_LEN                              (30)
#define PP_CLK_CTRL_INFRA_EN_RSVD0_MSK                              (0xFFFFFFFC)
#define PP_CLK_CTRL_INFRA_EN_RSVD0_RST                              (0x0)
#define PP_CLK_CTRL_INFRA_EN_MOD_EN_OFF                             (0)
#define PP_CLK_CTRL_INFRA_EN_MOD_EN_LEN                             (2)
#define PP_CLK_CTRL_INFRA_EN_MOD_EN_MSK                             (0x00000003)
#define PP_CLK_CTRL_INFRA_EN_MOD_EN_RST                             (0x0)

/**
 * SW_REG_NAME : PP_CLK_CTRL_INFRA_STATUS_REG
 * HW_REG_NAME : infra_status
 * DESCRIPTION : TBD
 *
 *  Register Fields :
 *   [31: 2][RO] - Reserved
 *   [ 1: 0][RO] - CRU SM state : 00 - Disable ; 01 - Sync Rst ; 10 -
 *                 Clk Disable ; 11 - Enable
 *
 */
#define PP_CLK_CTRL_INFRA_STATUS_REG                ((CLK_CTRL_BASE_ADDR) + 0x4)
#define PP_CLK_CTRL_INFRA_STATUS_RSVD0_OFF                          (2)
#define PP_CLK_CTRL_INFRA_STATUS_RSVD0_LEN                          (30)
#define PP_CLK_CTRL_INFRA_STATUS_RSVD0_MSK                          (0xFFFFFFFC)
#define PP_CLK_CTRL_INFRA_STATUS_RSVD0_RST                          (0x0)
#define PP_CLK_CTRL_INFRA_STATUS_MOD_STATUS_OFF                     (0)
#define PP_CLK_CTRL_INFRA_STATUS_MOD_STATUS_LEN                     (2)
#define PP_CLK_CTRL_INFRA_STATUS_MOD_STATUS_MSK                     (0x00000003)
#define PP_CLK_CTRL_INFRA_STATUS_MOD_STATUS_RST                     (0x0)

/**
 * SW_REG_NAME : PP_CLK_CTRL_INFRA_FORCE_REG
 * HW_REG_NAME : infra_force
 * DESCRIPTION : TBD
 *
 *  Register Fields :
 *   [31: 1][RO] - Reserved
 *   [ 0: 0][RW] - Force module reset and opens the clock gater
 *
 */
#define PP_CLK_CTRL_INFRA_FORCE_REG                 ((CLK_CTRL_BASE_ADDR) + 0x8)
#define PP_CLK_CTRL_INFRA_FORCE_RSVD0_OFF                           (1)
#define PP_CLK_CTRL_INFRA_FORCE_RSVD0_LEN                           (31)
#define PP_CLK_CTRL_INFRA_FORCE_RSVD0_MSK                           (0xFFFFFFFE)
#define PP_CLK_CTRL_INFRA_FORCE_RSVD0_RST                           (0x0)
#define PP_CLK_CTRL_INFRA_FORCE_MOD_FORCE_OFF                       (0)
#define PP_CLK_CTRL_INFRA_FORCE_MOD_FORCE_LEN                       (1)
#define PP_CLK_CTRL_INFRA_FORCE_MOD_FORCE_MSK                       (0x00000001)
#define PP_CLK_CTRL_INFRA_FORCE_MOD_FORCE_RST                       (0x0)

/**
 * SW_REG_NAME : PP_CLK_CTRL_INGRESS_SS_EN_REG
 * HW_REG_NAME : ingress_ss_enable
 * DESCRIPTION : TBD
 *
 *  Register Fields :
 *   [31: 2][RO] - Reserved
 *   [ 1: 0][RW] - Configured Module state : 00 - Disable ; 01 - Sync
 *                 Rst ; 10 - Clk Disable ; 11 - Enable
 *
 */
#define PP_CLK_CTRL_INGRESS_SS_EN_REG              ((CLK_CTRL_BASE_ADDR) + 0x10)
#define PP_CLK_CTRL_INGRESS_SS_EN_RSVD0_OFF                         (2)
#define PP_CLK_CTRL_INGRESS_SS_EN_RSVD0_LEN                         (30)
#define PP_CLK_CTRL_INGRESS_SS_EN_RSVD0_MSK                         (0xFFFFFFFC)
#define PP_CLK_CTRL_INGRESS_SS_EN_RSVD0_RST                         (0x0)
#define PP_CLK_CTRL_INGRESS_SS_EN_MOD_EN_OFF                        (0)
#define PP_CLK_CTRL_INGRESS_SS_EN_MOD_EN_LEN                        (2)
#define PP_CLK_CTRL_INGRESS_SS_EN_MOD_EN_MSK                        (0x00000003)
#define PP_CLK_CTRL_INGRESS_SS_EN_MOD_EN_RST                        (0x0)

/**
 * SW_REG_NAME : PP_CLK_CTRL_INGRESS_SS_STATUS_REG
 * HW_REG_NAME : ingress_ss_status
 * DESCRIPTION : TBD
 *
 *  Register Fields :
 *   [31: 2][RO] - Reserved
 *   [ 1: 0][RO] - CRU SM state : 00 - Disable ; 01 - Sync Rst ; 10 -
 *                 Clk Disable ; 11 - Enable
 *
 */
#define PP_CLK_CTRL_INGRESS_SS_STATUS_REG          ((CLK_CTRL_BASE_ADDR) + 0x14)
#define PP_CLK_CTRL_INGRESS_SS_STATUS_RSVD0_OFF                     (2)
#define PP_CLK_CTRL_INGRESS_SS_STATUS_RSVD0_LEN                     (30)
#define PP_CLK_CTRL_INGRESS_SS_STATUS_RSVD0_MSK                     (0xFFFFFFFC)
#define PP_CLK_CTRL_INGRESS_SS_STATUS_RSVD0_RST                     (0x0)
#define PP_CLK_CTRL_INGRESS_SS_STATUS_MOD_STATUS_OFF                (0)
#define PP_CLK_CTRL_INGRESS_SS_STATUS_MOD_STATUS_LEN                (2)
#define PP_CLK_CTRL_INGRESS_SS_STATUS_MOD_STATUS_MSK                (0x00000003)
#define PP_CLK_CTRL_INGRESS_SS_STATUS_MOD_STATUS_RST                (0x0)

/**
 * SW_REG_NAME : PP_CLK_CTRL_INGRESS_SS_FORCE_REG
 * HW_REG_NAME : ingress_ss_force
 * DESCRIPTION : TBD
 *
 *  Register Fields :
 *   [31: 1][RO] - Reserved
 *   [ 0: 0][RW] - Force module reset and opens the clock gater
 *
 */
#define PP_CLK_CTRL_INGRESS_SS_FORCE_REG           ((CLK_CTRL_BASE_ADDR) + 0x18)
#define PP_CLK_CTRL_INGRESS_SS_FORCE_RSVD0_OFF                      (1)
#define PP_CLK_CTRL_INGRESS_SS_FORCE_RSVD0_LEN                      (31)
#define PP_CLK_CTRL_INGRESS_SS_FORCE_RSVD0_MSK                      (0xFFFFFFFE)
#define PP_CLK_CTRL_INGRESS_SS_FORCE_RSVD0_RST                      (0x0)
#define PP_CLK_CTRL_INGRESS_SS_FORCE_MOD_FORCE_OFF                  (0)
#define PP_CLK_CTRL_INGRESS_SS_FORCE_MOD_FORCE_LEN                  (1)
#define PP_CLK_CTRL_INGRESS_SS_FORCE_MOD_FORCE_MSK                  (0x00000001)
#define PP_CLK_CTRL_INGRESS_SS_FORCE_MOD_FORCE_RST                  (0x0)

/**
 * SW_REG_NAME : PP_CLK_CTRL_QOS_SS_EN_REG
 * HW_REG_NAME : qos_ss_enable
 * DESCRIPTION : TBD
 *
 *  Register Fields :
 *   [31: 2][RO] - Reserved
 *   [ 1: 0][RW] - Configured Module state : 00 - Disable ; 01 - Sync
 *                 Rst ; 10 - Clk Disable ; 11 - Enable
 *
 */
#define PP_CLK_CTRL_QOS_SS_EN_REG                  ((CLK_CTRL_BASE_ADDR) + 0x20)
#define PP_CLK_CTRL_QOS_SS_EN_RSVD0_OFF                             (2)
#define PP_CLK_CTRL_QOS_SS_EN_RSVD0_LEN                             (30)
#define PP_CLK_CTRL_QOS_SS_EN_RSVD0_MSK                             (0xFFFFFFFC)
#define PP_CLK_CTRL_QOS_SS_EN_RSVD0_RST                             (0x0)
#define PP_CLK_CTRL_QOS_SS_EN_MOD_EN_OFF                            (0)
#define PP_CLK_CTRL_QOS_SS_EN_MOD_EN_LEN                            (2)
#define PP_CLK_CTRL_QOS_SS_EN_MOD_EN_MSK                            (0x00000003)
#define PP_CLK_CTRL_QOS_SS_EN_MOD_EN_RST                            (0x0)

/**
 * SW_REG_NAME : PP_CLK_CTRL_QOS_SS_STATUS_REG
 * HW_REG_NAME : qos_ss_status
 * DESCRIPTION : TBD
 *
 *  Register Fields :
 *   [31: 2][RO] - Reserved
 *   [ 1: 0][RO] - CRU SM state : 00 - Disable ; 01 - Sync Rst ; 10 -
 *                 Clk Disable ; 11 - Enable
 *
 */
#define PP_CLK_CTRL_QOS_SS_STATUS_REG              ((CLK_CTRL_BASE_ADDR) + 0x24)
#define PP_CLK_CTRL_QOS_SS_STATUS_RSVD0_OFF                         (2)
#define PP_CLK_CTRL_QOS_SS_STATUS_RSVD0_LEN                         (30)
#define PP_CLK_CTRL_QOS_SS_STATUS_RSVD0_MSK                         (0xFFFFFFFC)
#define PP_CLK_CTRL_QOS_SS_STATUS_RSVD0_RST                         (0x0)
#define PP_CLK_CTRL_QOS_SS_STATUS_MOD_STATUS_OFF                    (0)
#define PP_CLK_CTRL_QOS_SS_STATUS_MOD_STATUS_LEN                    (2)
#define PP_CLK_CTRL_QOS_SS_STATUS_MOD_STATUS_MSK                    (0x00000003)
#define PP_CLK_CTRL_QOS_SS_STATUS_MOD_STATUS_RST                    (0x0)

/**
 * SW_REG_NAME : PP_CLK_CTRL_QOS_SS_FORCE_REG
 * HW_REG_NAME : qos_ss_force
 * DESCRIPTION : TBD
 *
 *  Register Fields :
 *   [31: 1][RO] - Reserved
 *   [ 0: 0][RW] - Force module reset and opens the clock gater
 *
 */
#define PP_CLK_CTRL_QOS_SS_FORCE_REG               ((CLK_CTRL_BASE_ADDR) + 0x28)
#define PP_CLK_CTRL_QOS_SS_FORCE_RSVD0_OFF                          (1)
#define PP_CLK_CTRL_QOS_SS_FORCE_RSVD0_LEN                          (31)
#define PP_CLK_CTRL_QOS_SS_FORCE_RSVD0_MSK                          (0xFFFFFFFE)
#define PP_CLK_CTRL_QOS_SS_FORCE_RSVD0_RST                          (0x0)
#define PP_CLK_CTRL_QOS_SS_FORCE_MOD_FORCE_OFF                      (0)
#define PP_CLK_CTRL_QOS_SS_FORCE_MOD_FORCE_LEN                      (1)
#define PP_CLK_CTRL_QOS_SS_FORCE_MOD_FORCE_MSK                      (0x00000001)
#define PP_CLK_CTRL_QOS_SS_FORCE_MOD_FORCE_RST                      (0x0)

/**
 * SW_REG_NAME : PP_CLK_CTRL_UC_INGRESS_EN_REG
 * HW_REG_NAME : uc_ingress_enable
 * DESCRIPTION : TBD
 *
 *  Register Fields :
 *   [31: 2][RO] - Reserved
 *   [ 1: 0][RW] - Configured Module state : 00 - Disable ; 01 - Sync
 *                 Rst ; 10 - Clk Disable ; 11 - Enable
 *
 */
#define PP_CLK_CTRL_UC_INGRESS_EN_REG              ((CLK_CTRL_BASE_ADDR) + 0x30)
#define PP_CLK_CTRL_UC_INGRESS_EN_RSVD0_OFF                         (2)
#define PP_CLK_CTRL_UC_INGRESS_EN_RSVD0_LEN                         (30)
#define PP_CLK_CTRL_UC_INGRESS_EN_RSVD0_MSK                         (0xFFFFFFFC)
#define PP_CLK_CTRL_UC_INGRESS_EN_RSVD0_RST                         (0x0)
#define PP_CLK_CTRL_UC_INGRESS_EN_MOD_EN_OFF                        (0)
#define PP_CLK_CTRL_UC_INGRESS_EN_MOD_EN_LEN                        (2)
#define PP_CLK_CTRL_UC_INGRESS_EN_MOD_EN_MSK                        (0x00000003)
#define PP_CLK_CTRL_UC_INGRESS_EN_MOD_EN_RST                        (0x0)

/**
 * SW_REG_NAME : PP_CLK_CTRL_UC_INGRESS_STATUS_REG
 * HW_REG_NAME : uc_ingress_status
 * DESCRIPTION : TBD
 *
 *  Register Fields :
 *   [31: 2][RO] - Reserved
 *   [ 1: 0][RO] - CRU SM state : 00 - Disable ; 01 - Sync Rst ; 10 -
 *                 Clk Disable ; 11 - Enable
 *
 */
#define PP_CLK_CTRL_UC_INGRESS_STATUS_REG          ((CLK_CTRL_BASE_ADDR) + 0x34)
#define PP_CLK_CTRL_UC_INGRESS_STATUS_RSVD0_OFF                     (2)
#define PP_CLK_CTRL_UC_INGRESS_STATUS_RSVD0_LEN                     (30)
#define PP_CLK_CTRL_UC_INGRESS_STATUS_RSVD0_MSK                     (0xFFFFFFFC)
#define PP_CLK_CTRL_UC_INGRESS_STATUS_RSVD0_RST                     (0x0)
#define PP_CLK_CTRL_UC_INGRESS_STATUS_MOD_STATUS_OFF                (0)
#define PP_CLK_CTRL_UC_INGRESS_STATUS_MOD_STATUS_LEN                (2)
#define PP_CLK_CTRL_UC_INGRESS_STATUS_MOD_STATUS_MSK                (0x00000003)
#define PP_CLK_CTRL_UC_INGRESS_STATUS_MOD_STATUS_RST                (0x0)

/**
 * SW_REG_NAME : PP_CLK_CTRL_UC_INGRESS_FORCE_REG
 * HW_REG_NAME : uc_ingress_force
 * DESCRIPTION : TBD
 *
 *  Register Fields :
 *   [31: 1][RO] - Reserved
 *   [ 0: 0][RW] - Force module reset and opens the clock gater
 *
 */
#define PP_CLK_CTRL_UC_INGRESS_FORCE_REG           ((CLK_CTRL_BASE_ADDR) + 0x38)
#define PP_CLK_CTRL_UC_INGRESS_FORCE_RSVD0_OFF                      (1)
#define PP_CLK_CTRL_UC_INGRESS_FORCE_RSVD0_LEN                      (31)
#define PP_CLK_CTRL_UC_INGRESS_FORCE_RSVD0_MSK                      (0xFFFFFFFE)
#define PP_CLK_CTRL_UC_INGRESS_FORCE_RSVD0_RST                      (0x0)
#define PP_CLK_CTRL_UC_INGRESS_FORCE_MOD_FORCE_OFF                  (0)
#define PP_CLK_CTRL_UC_INGRESS_FORCE_MOD_FORCE_LEN                  (1)
#define PP_CLK_CTRL_UC_INGRESS_FORCE_MOD_FORCE_MSK                  (0x00000001)
#define PP_CLK_CTRL_UC_INGRESS_FORCE_MOD_FORCE_RST                  (0x0)

/**
 * SW_REG_NAME : PP_CLK_CTRL_UC_EGRESS_EN_REG
 * HW_REG_NAME : uc_egress_enable
 * DESCRIPTION : TBD
 *
 *  Register Fields :
 *   [31: 2][RO] - Reserved
 *   [ 1: 0][RW] - Configured Module state : 00 - Disable ; 01 - Sync
 *                 Rst ; 10 - Clk Disable ; 11 - Enable
 *
 */
#define PP_CLK_CTRL_UC_EGRESS_EN_REG               ((CLK_CTRL_BASE_ADDR) + 0x40)
#define PP_CLK_CTRL_UC_EGRESS_EN_RSVD0_OFF                          (2)
#define PP_CLK_CTRL_UC_EGRESS_EN_RSVD0_LEN                          (30)
#define PP_CLK_CTRL_UC_EGRESS_EN_RSVD0_MSK                          (0xFFFFFFFC)
#define PP_CLK_CTRL_UC_EGRESS_EN_RSVD0_RST                          (0x0)
#define PP_CLK_CTRL_UC_EGRESS_EN_MOD_EN_OFF                         (0)
#define PP_CLK_CTRL_UC_EGRESS_EN_MOD_EN_LEN                         (2)
#define PP_CLK_CTRL_UC_EGRESS_EN_MOD_EN_MSK                         (0x00000003)
#define PP_CLK_CTRL_UC_EGRESS_EN_MOD_EN_RST                         (0x0)

/**
 * SW_REG_NAME : PP_CLK_CTRL_UC_EGRESS_STATUS_REG
 * HW_REG_NAME : uc_egress_status
 * DESCRIPTION : TBD
 *
 *  Register Fields :
 *   [31: 2][RO] - Reserved
 *   [ 1: 0][RO] - CRU SM state : 00 - Disable ; 01 - Sync Rst ; 10 -
 *                 Clk Disable ; 11 - Enable
 *
 */
#define PP_CLK_CTRL_UC_EGRESS_STATUS_REG           ((CLK_CTRL_BASE_ADDR) + 0x44)
#define PP_CLK_CTRL_UC_EGRESS_STATUS_RSVD0_OFF                      (2)
#define PP_CLK_CTRL_UC_EGRESS_STATUS_RSVD0_LEN                      (30)
#define PP_CLK_CTRL_UC_EGRESS_STATUS_RSVD0_MSK                      (0xFFFFFFFC)
#define PP_CLK_CTRL_UC_EGRESS_STATUS_RSVD0_RST                      (0x0)
#define PP_CLK_CTRL_UC_EGRESS_STATUS_MOD_STATUS_OFF                 (0)
#define PP_CLK_CTRL_UC_EGRESS_STATUS_MOD_STATUS_LEN                 (2)
#define PP_CLK_CTRL_UC_EGRESS_STATUS_MOD_STATUS_MSK                 (0x00000003)
#define PP_CLK_CTRL_UC_EGRESS_STATUS_MOD_STATUS_RST                 (0x0)

/**
 * SW_REG_NAME : PP_CLK_CTRL_UC_EGRESS_FORCE_REG
 * HW_REG_NAME : uc_egress_force
 * DESCRIPTION : TBD
 *
 *  Register Fields :
 *   [31: 1][RO] - Reserved
 *   [ 0: 0][RW] - Force module reset and opens the clock gater
 *
 */
#define PP_CLK_CTRL_UC_EGRESS_FORCE_REG            ((CLK_CTRL_BASE_ADDR) + 0x48)
#define PP_CLK_CTRL_UC_EGRESS_FORCE_RSVD0_OFF                       (1)
#define PP_CLK_CTRL_UC_EGRESS_FORCE_RSVD0_LEN                       (31)
#define PP_CLK_CTRL_UC_EGRESS_FORCE_RSVD0_MSK                       (0xFFFFFFFE)
#define PP_CLK_CTRL_UC_EGRESS_FORCE_RSVD0_RST                       (0x0)
#define PP_CLK_CTRL_UC_EGRESS_FORCE_MOD_FORCE_OFF                   (0)
#define PP_CLK_CTRL_UC_EGRESS_FORCE_MOD_FORCE_LEN                   (1)
#define PP_CLK_CTRL_UC_EGRESS_FORCE_MOD_FORCE_MSK                   (0x00000001)
#define PP_CLK_CTRL_UC_EGRESS_FORCE_MOD_FORCE_RST                   (0x0)

/**
 * SW_REG_NAME : PP_CLK_CTRL_CHK_TBM_EN_REG
 * HW_REG_NAME : chk_tbm_enable
 * DESCRIPTION : TBD
 *
 *  Register Fields :
 *   [31: 2][RO] - Reserved
 *   [ 1: 0][RW] - Configured Module state : 00 - Disable ; 01 - Sync
 *                 Rst ; 10 - Clk Disable ; 11 - Enable
 *
 */
#define PP_CLK_CTRL_CHK_TBM_EN_REG                 ((CLK_CTRL_BASE_ADDR) + 0x50)
#define PP_CLK_CTRL_CHK_TBM_EN_RSVD0_OFF                            (2)
#define PP_CLK_CTRL_CHK_TBM_EN_RSVD0_LEN                            (30)
#define PP_CLK_CTRL_CHK_TBM_EN_RSVD0_MSK                            (0xFFFFFFFC)
#define PP_CLK_CTRL_CHK_TBM_EN_RSVD0_RST                            (0x0)
#define PP_CLK_CTRL_CHK_TBM_EN_MOD_EN_OFF                           (0)
#define PP_CLK_CTRL_CHK_TBM_EN_MOD_EN_LEN                           (2)
#define PP_CLK_CTRL_CHK_TBM_EN_MOD_EN_MSK                           (0x00000003)
#define PP_CLK_CTRL_CHK_TBM_EN_MOD_EN_RST                           (0x0)

/**
 * SW_REG_NAME : PP_CLK_CTRL_CHK_TBM_STATUS_REG
 * HW_REG_NAME : chk_tbm_status
 * DESCRIPTION : TBD
 *
 *  Register Fields :
 *   [31: 2][RO] - Reserved
 *   [ 1: 0][RO] - CRU SM state : 00 - Disable ; 01 - Sync Rst ; 10 -
 *                 Clk Disable ; 11 - Enable
 *
 */
#define PP_CLK_CTRL_CHK_TBM_STATUS_REG             ((CLK_CTRL_BASE_ADDR) + 0x54)
#define PP_CLK_CTRL_CHK_TBM_STATUS_RSVD0_OFF                        (2)
#define PP_CLK_CTRL_CHK_TBM_STATUS_RSVD0_LEN                        (30)
#define PP_CLK_CTRL_CHK_TBM_STATUS_RSVD0_MSK                        (0xFFFFFFFC)
#define PP_CLK_CTRL_CHK_TBM_STATUS_RSVD0_RST                        (0x0)
#define PP_CLK_CTRL_CHK_TBM_STATUS_MOD_STATUS_OFF                   (0)
#define PP_CLK_CTRL_CHK_TBM_STATUS_MOD_STATUS_LEN                   (2)
#define PP_CLK_CTRL_CHK_TBM_STATUS_MOD_STATUS_MSK                   (0x00000003)
#define PP_CLK_CTRL_CHK_TBM_STATUS_MOD_STATUS_RST                   (0x0)

/**
 * SW_REG_NAME : PP_CLK_CTRL_CHK_TBM_FORCE_REG
 * HW_REG_NAME : chk_tbm_force
 * DESCRIPTION : TBD
 *
 *  Register Fields :
 *   [31: 1][RO] - Reserved
 *   [ 0: 0][RW] - Force module reset and opens the clock gater
 *
 */
#define PP_CLK_CTRL_CHK_TBM_FORCE_REG              ((CLK_CTRL_BASE_ADDR) + 0x58)
#define PP_CLK_CTRL_CHK_TBM_FORCE_RSVD0_OFF                         (1)
#define PP_CLK_CTRL_CHK_TBM_FORCE_RSVD0_LEN                         (31)
#define PP_CLK_CTRL_CHK_TBM_FORCE_RSVD0_MSK                         (0xFFFFFFFE)
#define PP_CLK_CTRL_CHK_TBM_FORCE_RSVD0_RST                         (0x0)
#define PP_CLK_CTRL_CHK_TBM_FORCE_MOD_FORCE_OFF                     (0)
#define PP_CLK_CTRL_CHK_TBM_FORCE_MOD_FORCE_LEN                     (1)
#define PP_CLK_CTRL_CHK_TBM_FORCE_MOD_FORCE_MSK                     (0x00000001)
#define PP_CLK_CTRL_CHK_TBM_FORCE_MOD_FORCE_RST                     (0x0)

/**
 * SW_REG_NAME : PP_CLK_CTRL_ICG_FORCE_CLK_REG
 * HW_REG_NAME : icg_force_clk
 * DESCRIPTION : force clock for ingress modules
 *
 *  Register Fields :
 *   [31: 1][RO] - Reserved
 *   [ 0: 0][RW] - Force clk and enable the clock
 *
 */
#define PP_CLK_CTRL_ICG_FORCE_CLK_REG             ((CLK_CTRL_BASE_ADDR) + 0x100)
#define PP_CLK_CTRL_ICG_FORCE_CLK_RSVD0_OFF                         (1)
#define PP_CLK_CTRL_ICG_FORCE_CLK_RSVD0_LEN                         (31)
#define PP_CLK_CTRL_ICG_FORCE_CLK_RSVD0_MSK                         (0xFFFFFFFE)
#define PP_CLK_CTRL_ICG_FORCE_CLK_RSVD0_RST                         (0x0)
#define PP_CLK_CTRL_ICG_FORCE_CLK_OFF                               (0)
#define PP_CLK_CTRL_ICG_FORCE_CLK_LEN                               (1)
#define PP_CLK_CTRL_ICG_FORCE_CLK_MSK                               (0x00000001)
#define PP_CLK_CTRL_ICG_FORCE_CLK_RST                               (0x1)

/**
 * SW_REG_NAME : PP_CLK_CTRL_DYNAMIC_CLK_GATED_EN_REG
 * HW_REG_NAME : dynaimc_clk_gated_enable
 * DESCRIPTION : enable dynaimc clock gate for Ingress modules
 *
 *  Register Fields :
 *   [31:16][RO] - Reserved
 *   [15: 0][RW] - ingress dynamic clk enable ; [0]port_distrube ;
 *                 [1]RPB(sys_clk) ; [2]RPB(ppv4_clk) ; [3]parser ;
 *                 [4]cls_core ; [5] cls_session_cache ; [6]cls_chk ;
 *                 [7]cls_chk_cache ; [8]mod ; [9]rx_dma ; [10]bmgr
 *                 ;[11]-QOS_WRED ; [12]-QOS_TSCD ; [13]-QOS_QMGR
 *                 ;[14]- QOS_TXMGR  [15]-QOS_UC;
 *
 */
#define PP_CLK_CTRL_DYNAMIC_CLK_GATED_EN_REG      ((CLK_CTRL_BASE_ADDR) + 0x104)
#define PP_CLK_CTRL_DYNAMIC_CLK_GATED_EN_RSVD0_OFF                  (16)
#define PP_CLK_CTRL_DYNAMIC_CLK_GATED_EN_RSVD0_LEN                  (16)
#define PP_CLK_CTRL_DYNAMIC_CLK_GATED_EN_RSVD0_MSK                  (0xFFFF0000)
#define PP_CLK_CTRL_DYNAMIC_CLK_GATED_EN_RSVD0_RST                  (0x0)
#define PP_CLK_CTRL_DYNAMIC_CLK_GATED_EN_ICG_EN_OFF                 (0)
#define PP_CLK_CTRL_DYNAMIC_CLK_GATED_EN_ICG_EN_LEN                 (16)
#define PP_CLK_CTRL_DYNAMIC_CLK_GATED_EN_ICG_EN_MSK                 (0x0000FFFF)
#define PP_CLK_CTRL_DYNAMIC_CLK_GATED_EN_ICG_EN_RST                 (0x0)

/**
 * SW_REG_NAME : PP_CLK_CTRL_INGRESS_CG_STATUS_REG
 * HW_REG_NAME : ingress_cg_status
 * DESCRIPTION : Ingress clock gates status
 *
 *  Register Fields :
 *   [31:16][RO] - Reserved
 *   [15: 0][RO] - ingress dynamic clk status ; [0]port_distrube ;
 *                 [1]RPB(sys_clk) ; [2]RPB(ppv4_clk) ; [3]parser ;
 *                 [4]cls_core ; [5] cls_session_cache ; [6]cls_chk ;
 *                 [7]cls_chk_cache ; [8]mod ; [9]rx_dma ; [10]bmgr
 *                 ;[11]-QOS_WRED ; [12]-QOS_TSCD ; [13]-QOS_QMGR
 *                 ;[14]- QOS_TXMGR  [15]-QOS_UC;
 *
 */
#define PP_CLK_CTRL_INGRESS_CG_STATUS_REG         ((CLK_CTRL_BASE_ADDR) + 0x108)
#define PP_CLK_CTRL_INGRESS_CG_STATUS_RSVD0_OFF                     (16)
#define PP_CLK_CTRL_INGRESS_CG_STATUS_RSVD0_LEN                     (16)
#define PP_CLK_CTRL_INGRESS_CG_STATUS_RSVD0_MSK                     (0xFFFF0000)
#define PP_CLK_CTRL_INGRESS_CG_STATUS_RSVD0_RST                     (0x0)
#define PP_CLK_CTRL_INGRESS_CG_STATUS_ICG_STATUS_OFF                (0)
#define PP_CLK_CTRL_INGRESS_CG_STATUS_ICG_STATUS_LEN                (16)
#define PP_CLK_CTRL_INGRESS_CG_STATUS_ICG_STATUS_MSK                (0x0000FFFF)
#define PP_CLK_CTRL_INGRESS_CG_STATUS_ICG_STATUS_RST                (0x0)

/**
 * SW_REG_NAME : PP_CLK_CTRL_CG_GRACE_CNT_REG
 * HW_REG_NAME : cg_grace_counter
 * DESCRIPTION : clock counter set for CG grace cycles
 *
 *  Register Fields :
 *   [31:10][RO] - Reserved
 *   [ 9: 0][RW] - clock counter set for CG grace cycles
 *
 */
#define PP_CLK_CTRL_CG_GRACE_CNT_REG              ((CLK_CTRL_BASE_ADDR) + 0x10C)
#define PP_CLK_CTRL_CG_GRACE_CNT_RSVD0_OFF                          (10)
#define PP_CLK_CTRL_CG_GRACE_CNT_RSVD0_LEN                          (22)
#define PP_CLK_CTRL_CG_GRACE_CNT_RSVD0_MSK                          (0xFFFFFC00)
#define PP_CLK_CTRL_CG_GRACE_CNT_RSVD0_RST                          (0x0)
#define PP_CLK_CTRL_CG_GRACE_CNT_ICG_GRACE_CYCLE_OFF                (0)
#define PP_CLK_CTRL_CG_GRACE_CNT_ICG_GRACE_CYCLE_LEN                (10)
#define PP_CLK_CTRL_CG_GRACE_CNT_ICG_GRACE_CYCLE_MSK                (0x000003FF)
#define PP_CLK_CTRL_CG_GRACE_CNT_ICG_GRACE_CYCLE_RST                (0x20)

/**
 * SW_REG_NAME : PP_CLK_CTRL_INGRESS_CG_REQ_STATUS_REG
 * HW_REG_NAME : ingress_cg_req_status
 * DESCRIPTION : Ingress clock gates request status
 *
 *  Register Fields :
 *   [31:11][RO] - Reserved
 *   [10: 0][RO] - ingress dynamic clk status ; [0]port_distrube ;
 *                 [1]RPB(sys_clk) ; [2]RPB(ppv4_clk) ; [3]parser ;
 *                 [4]cls_core ; [5] cls_session_cache ; [6]cls_chk ;
 *                 [7]cls_chk_cache ; [8]mod ; [9]rx_dma [not_valid]
 *                 ; [10]bmgr
 *
 */
#define PP_CLK_CTRL_INGRESS_CG_REQ_STATUS_REG     ((CLK_CTRL_BASE_ADDR) + 0x110)
#define PP_CLK_CTRL_INGRESS_CG_REQ_STATUS_RSVD0_OFF                 (11)
#define PP_CLK_CTRL_INGRESS_CG_REQ_STATUS_RSVD0_LEN                 (21)
#define PP_CLK_CTRL_INGRESS_CG_REQ_STATUS_RSVD0_MSK                 (0xFFFFF800)
#define PP_CLK_CTRL_INGRESS_CG_REQ_STATUS_RSVD0_RST                 (0x0)
#define PP_CLK_CTRL_INGRESS_CG_REQ_STATUS_ICG_REQ_STATUS_OFF        (0)
#define PP_CLK_CTRL_INGRESS_CG_REQ_STATUS_ICG_REQ_STATUS_LEN        (11)
#define PP_CLK_CTRL_INGRESS_CG_REQ_STATUS_ICG_REQ_STATUS_MSK        (0x000007FF)
#define PP_CLK_CTRL_INGRESS_CG_REQ_STATUS_ICG_REQ_STATUS_RST        (0x0)

/**
 * SW_REG_NAME : PP_CLK_CTRL_CRU_OVERRIDE_REG
 * HW_REG_NAME : cru_override
 * DESCRIPTION : cru_override_en
 *
 *  Register Fields :
 *   [31:30][RW] - cru_override_reg CRU 5;
 *   [29:28][RW] - cru_override_reg CRU 4;
 *   [27:26][RW] - cru_override_reg CRU 3;
 *   [25:24][RW] - cru_override_reg CRU 2;
 *   [23:22][RW] - cru_override_reg CRU 1;
 *   [21:20][RW] - cru_override_reg CRU 0;
 *   [19: 6][RO] - Reserved
 *   [ 5: 0][RW] - cru_override_en
 *
 */
#define PP_CLK_CTRL_CRU_OVERRIDE_REG              ((CLK_CTRL_BASE_ADDR) + 0x200)
#define PP_CLK_CTRL_CRU_OVERRIDE_OVERRIDE_VAL_5_OFF                 (30)
#define PP_CLK_CTRL_CRU_OVERRIDE_OVERRIDE_VAL_5_LEN                 (2)
#define PP_CLK_CTRL_CRU_OVERRIDE_OVERRIDE_VAL_5_MSK                 (0xC0000000)
#define PP_CLK_CTRL_CRU_OVERRIDE_OVERRIDE_VAL_5_RST                 (0x0)
#define PP_CLK_CTRL_CRU_OVERRIDE_OVERRIDE_VAL_4_OFF                 (28)
#define PP_CLK_CTRL_CRU_OVERRIDE_OVERRIDE_VAL_4_LEN                 (2)
#define PP_CLK_CTRL_CRU_OVERRIDE_OVERRIDE_VAL_4_MSK                 (0x30000000)
#define PP_CLK_CTRL_CRU_OVERRIDE_OVERRIDE_VAL_4_RST                 (0x0)
#define PP_CLK_CTRL_CRU_OVERRIDE_OVERRIDE_VAL_3_OFF                 (26)
#define PP_CLK_CTRL_CRU_OVERRIDE_OVERRIDE_VAL_3_LEN                 (2)
#define PP_CLK_CTRL_CRU_OVERRIDE_OVERRIDE_VAL_3_MSK                 (0x0C000000)
#define PP_CLK_CTRL_CRU_OVERRIDE_OVERRIDE_VAL_3_RST                 (0x0)
#define PP_CLK_CTRL_CRU_OVERRIDE_OVERRIDE_VAL_2_OFF                 (24)
#define PP_CLK_CTRL_CRU_OVERRIDE_OVERRIDE_VAL_2_LEN                 (2)
#define PP_CLK_CTRL_CRU_OVERRIDE_OVERRIDE_VAL_2_MSK                 (0x03000000)
#define PP_CLK_CTRL_CRU_OVERRIDE_OVERRIDE_VAL_2_RST                 (0x0)
#define PP_CLK_CTRL_CRU_OVERRIDE_OVERRIDE_VAL_1_OFF                 (22)
#define PP_CLK_CTRL_CRU_OVERRIDE_OVERRIDE_VAL_1_LEN                 (2)
#define PP_CLK_CTRL_CRU_OVERRIDE_OVERRIDE_VAL_1_MSK                 (0x00C00000)
#define PP_CLK_CTRL_CRU_OVERRIDE_OVERRIDE_VAL_1_RST                 (0x0)
#define PP_CLK_CTRL_CRU_OVERRIDE_OVERRIDE_VAL_0_OFF                 (20)
#define PP_CLK_CTRL_CRU_OVERRIDE_OVERRIDE_VAL_0_LEN                 (2)
#define PP_CLK_CTRL_CRU_OVERRIDE_OVERRIDE_VAL_0_MSK                 (0x00300000)
#define PP_CLK_CTRL_CRU_OVERRIDE_OVERRIDE_VAL_0_RST                 (0x0)
#define PP_CLK_CTRL_CRU_OVERRIDE_RSVD0_OFF                          (6)
#define PP_CLK_CTRL_CRU_OVERRIDE_RSVD0_LEN                          (14)
#define PP_CLK_CTRL_CRU_OVERRIDE_RSVD0_MSK                          (0x000FFFC0)
#define PP_CLK_CTRL_CRU_OVERRIDE_RSVD0_RST                          (0x0)
#define PP_CLK_CTRL_CRU_OVERRIDE_OVERRIDE_EN_OFF                    (0)
#define PP_CLK_CTRL_CRU_OVERRIDE_OVERRIDE_EN_LEN                    (6)
#define PP_CLK_CTRL_CRU_OVERRIDE_OVERRIDE_EN_MSK                    (0x0000003F)
#define PP_CLK_CTRL_CRU_OVERRIDE_OVERRIDE_EN_RST                    (0x0)

#endif
