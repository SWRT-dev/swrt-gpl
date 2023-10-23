/**
 * qos_uc_misc_regs.h
 * Description: qos_uc_misc_regs_s HW registers definitions
 * Auto-generated file by RDL Parser, DO NOT CHANGE
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2019-2020 Intel Corporation
 */

#ifndef _PP_QOS_MISC_H_
#define _PP_QOS_MISC_H_

#define PP_QOS_MISC_GEN_DATE_STR              "Sunday May 17, 2020 [5:26:11 pm]"
#define PP_QOS_MISC_BASE                                         (0xF1051000ULL)

/**
 * SW_REG_NAME : PP_QOS_MISC_UC_HW_REVISION_REG
 * HW_REG_NAME : qos_uc_hw_revision
 * DESCRIPTION : HW Revision Register
 *
 *  Register Fields :
 *   [31:30][RO] - Scheme
 *   [29:16][RO] - Module ID
 *   [15:11][RO] - RTL Revision
 *   [10: 8][RO] - Major Version; 0-Falcon; 1-PPV4
 *   [ 7: 0][RO] - Minor Version
 *
 */
#define PP_QOS_MISC_UC_HW_REVISION_REG              ((QOS_MISC_BASE_ADDR) + 0x0)
#define PP_QOS_MISC_UC_HW_REVISION_SCHEME_OFF                       (30)
#define PP_QOS_MISC_UC_HW_REVISION_SCHEME_LEN                       (2)
#define PP_QOS_MISC_UC_HW_REVISION_SCHEME_MSK                       (0xC0000000)
#define PP_QOS_MISC_UC_HW_REVISION_SCHEME_RST                       (0x0)
#define PP_QOS_MISC_UC_HW_REVISION_MODULE_ID_OFF                    (16)
#define PP_QOS_MISC_UC_HW_REVISION_MODULE_ID_LEN                    (14)
#define PP_QOS_MISC_UC_HW_REVISION_MODULE_ID_MSK                    (0x3FFF0000)
#define PP_QOS_MISC_UC_HW_REVISION_MODULE_ID_RST                    (0x0)
#define PP_QOS_MISC_UC_HW_REVISION_RTL_REV_OFF                      (11)
#define PP_QOS_MISC_UC_HW_REVISION_RTL_REV_LEN                      (5)
#define PP_QOS_MISC_UC_HW_REVISION_RTL_REV_MSK                      (0x0000F800)
#define PP_QOS_MISC_UC_HW_REVISION_RTL_REV_RST                      (0x2)
#define PP_QOS_MISC_UC_HW_REVISION_MAJOR_VER_OFF                    (8)
#define PP_QOS_MISC_UC_HW_REVISION_MAJOR_VER_LEN                    (3)
#define PP_QOS_MISC_UC_HW_REVISION_MAJOR_VER_MSK                    (0x00000700)
#define PP_QOS_MISC_UC_HW_REVISION_MAJOR_VER_RST                    (0x1)
#define PP_QOS_MISC_UC_HW_REVISION_MINOR_VER_OFF                    (0)
#define PP_QOS_MISC_UC_HW_REVISION_MINOR_VER_LEN                    (8)
#define PP_QOS_MISC_UC_HW_REVISION_MINOR_VER_MSK                    (0x000000FF)
#define PP_QOS_MISC_UC_HW_REVISION_MINOR_VER_RST                    (0xa)

/**
 * SW_REG_NAME : PP_QOS_MISC_UC_CLOCK_EN_REG
 * HW_REG_NAME : qos_uc_clock_en
 * DESCRIPTION : Clock Enable Control
 *
 *  Register Fields :
 *   [31: 6][RO] - Reserved
 *   [ 5: 5][RW] - ARC Core     Clock Enable (Enabled (1) by default)
 *   [ 4: 4][RW] - BMGR  Module Clock Enable (Enabled (1) by default)
 *   [ 3: 3][RW] - TXMGR Module Clock Enable (Enabled (1) by default)
 *   [ 2: 2][RW] - QMGR  Module Clock Enable (Enabled (1) by default)
 *   [ 1: 1][RW] - TSCD  Module Clock Enable (Enabled (1) by default)
 *   [ 0: 0][RW] - WRED  Module Clock Enable (Enabled (1) by default)
 *
 */
#define PP_QOS_MISC_UC_CLOCK_EN_REG                 ((QOS_MISC_BASE_ADDR) + 0x4)
#define PP_QOS_MISC_UC_CLOCK_EN_RSVD0_OFF                           (6)
#define PP_QOS_MISC_UC_CLOCK_EN_RSVD0_LEN                           (26)
#define PP_QOS_MISC_UC_CLOCK_EN_RSVD0_MSK                           (0xFFFFFFC0)
#define PP_QOS_MISC_UC_CLOCK_EN_RSVD0_RST                           (0x0)
#define PP_QOS_MISC_UC_CLOCK_EN_CLOCK_EN5_OFF                       (5)
#define PP_QOS_MISC_UC_CLOCK_EN_CLOCK_EN5_LEN                       (1)
#define PP_QOS_MISC_UC_CLOCK_EN_CLOCK_EN5_MSK                       (0x00000020)
#define PP_QOS_MISC_UC_CLOCK_EN_CLOCK_EN5_RST                       (0x1)
#define PP_QOS_MISC_UC_CLOCK_EN_CLOCK_EN4_OFF                       (4)
#define PP_QOS_MISC_UC_CLOCK_EN_CLOCK_EN4_LEN                       (1)
#define PP_QOS_MISC_UC_CLOCK_EN_CLOCK_EN4_MSK                       (0x00000010)
#define PP_QOS_MISC_UC_CLOCK_EN_CLOCK_EN4_RST                       (0x1)
#define PP_QOS_MISC_UC_CLOCK_EN_CLOCK_EN3_OFF                       (3)
#define PP_QOS_MISC_UC_CLOCK_EN_CLOCK_EN3_LEN                       (1)
#define PP_QOS_MISC_UC_CLOCK_EN_CLOCK_EN3_MSK                       (0x00000008)
#define PP_QOS_MISC_UC_CLOCK_EN_CLOCK_EN3_RST                       (0x1)
#define PP_QOS_MISC_UC_CLOCK_EN_CLOCK_EN2_OFF                       (2)
#define PP_QOS_MISC_UC_CLOCK_EN_CLOCK_EN2_LEN                       (1)
#define PP_QOS_MISC_UC_CLOCK_EN_CLOCK_EN2_MSK                       (0x00000004)
#define PP_QOS_MISC_UC_CLOCK_EN_CLOCK_EN2_RST                       (0x1)
#define PP_QOS_MISC_UC_CLOCK_EN_CLOCK_EN1_OFF                       (1)
#define PP_QOS_MISC_UC_CLOCK_EN_CLOCK_EN1_LEN                       (1)
#define PP_QOS_MISC_UC_CLOCK_EN_CLOCK_EN1_MSK                       (0x00000002)
#define PP_QOS_MISC_UC_CLOCK_EN_CLOCK_EN1_RST                       (0x1)
#define PP_QOS_MISC_UC_CLOCK_EN_CLOCK_EN0_OFF                       (0)
#define PP_QOS_MISC_UC_CLOCK_EN_CLOCK_EN0_LEN                       (1)
#define PP_QOS_MISC_UC_CLOCK_EN_CLOCK_EN0_MSK                       (0x00000001)
#define PP_QOS_MISC_UC_CLOCK_EN_CLOCK_EN0_RST                       (0x1)

/**
 * SW_REG_NAME : PP_QOS_MISC_UC_ARC_CTRL_REG
 * HW_REG_NAME : qos_uc_arc_ctrl
 * DESCRIPTION : ARC Core Control
 *
 *  Register Fields :
 *   [31: 3][RO] - Reserved
 *   [ 2: 2][RW] - ARC Core Run
 *   [ 1: 1][RW] - ARC Core Halt
 *   [ 0: 0][RW] - ARC Core Wake
 *
 */
#define PP_QOS_MISC_UC_ARC_CTRL_REG                 ((QOS_MISC_BASE_ADDR) + 0x8)
#define PP_QOS_MISC_UC_ARC_CTRL_RSVD0_OFF                           (3)
#define PP_QOS_MISC_UC_ARC_CTRL_RSVD0_LEN                           (29)
#define PP_QOS_MISC_UC_ARC_CTRL_RSVD0_MSK                           (0xFFFFFFF8)
#define PP_QOS_MISC_UC_ARC_CTRL_RSVD0_RST                           (0x0)
#define PP_QOS_MISC_UC_ARC_CTRL_ARC_RUN_OFF                         (2)
#define PP_QOS_MISC_UC_ARC_CTRL_ARC_RUN_LEN                         (1)
#define PP_QOS_MISC_UC_ARC_CTRL_ARC_RUN_MSK                         (0x00000004)
#define PP_QOS_MISC_UC_ARC_CTRL_ARC_RUN_RST                         (0x0)
#define PP_QOS_MISC_UC_ARC_CTRL_ARC_HALT_OFF                        (1)
#define PP_QOS_MISC_UC_ARC_CTRL_ARC_HALT_LEN                        (1)
#define PP_QOS_MISC_UC_ARC_CTRL_ARC_HALT_MSK                        (0x00000002)
#define PP_QOS_MISC_UC_ARC_CTRL_ARC_HALT_RST                        (0x0)
#define PP_QOS_MISC_UC_ARC_CTRL_ARC_WAKE_OFF                        (0)
#define PP_QOS_MISC_UC_ARC_CTRL_ARC_WAKE_LEN                        (1)
#define PP_QOS_MISC_UC_ARC_CTRL_ARC_WAKE_MSK                        (0x00000001)
#define PP_QOS_MISC_UC_ARC_CTRL_ARC_WAKE_RST                        (0x0)

/**
 * SW_REG_NAME : PP_QOS_MISC_UC_BUS_CTRL_REG
 * HW_REG_NAME : qos_uc_bus_ctrl
 * DESCRIPTION : BUS Access Control
 *
 *  Register Fields :
 *   [31: 5][RO] - Reserved
 *   [ 4: 4][RW] - MCDMA1 Master IF Posted Write Enable Control
 *   [ 3: 3][RW] - MCDMA1 Master IF BIG2LITTLE Endiannes Control
 *   [ 2: 2][RW] - MCDMA0 Master IF Posted Write Enable Control
 *   [ 1: 1][RW] - MCDMA0 Master IF BIG2LITTLE Endiannes Control
 *   [ 0: 0][RW] - Host   Master IF BIG2LITTLE Endiannes Control
 *
 */
#define PP_QOS_MISC_UC_BUS_CTRL_REG                ((QOS_MISC_BASE_ADDR) + 0x0C)
#define PP_QOS_MISC_UC_BUS_CTRL_RSVD0_OFF                           (5)
#define PP_QOS_MISC_UC_BUS_CTRL_RSVD0_LEN                           (27)
#define PP_QOS_MISC_UC_BUS_CTRL_RSVD0_MSK                           (0xFFFFFFE0)
#define PP_QOS_MISC_UC_BUS_CTRL_RSVD0_RST                           (0x0)
#define PP_QOS_MISC_UC_BUS_CTRL_MCDMAM1_PWE_OFF                     (4)
#define PP_QOS_MISC_UC_BUS_CTRL_MCDMAM1_PWE_LEN                     (1)
#define PP_QOS_MISC_UC_BUS_CTRL_MCDMAM1_PWE_MSK                     (0x00000010)
#define PP_QOS_MISC_UC_BUS_CTRL_MCDMAM1_PWE_RST                     (0x0)
#define PP_QOS_MISC_UC_BUS_CTRL_MCDMAM1_B2L_OFF                     (3)
#define PP_QOS_MISC_UC_BUS_CTRL_MCDMAM1_B2L_LEN                     (1)
#define PP_QOS_MISC_UC_BUS_CTRL_MCDMAM1_B2L_MSK                     (0x00000008)
#define PP_QOS_MISC_UC_BUS_CTRL_MCDMAM1_B2L_RST                     (0x0)
#define PP_QOS_MISC_UC_BUS_CTRL_MCDMAM0_PWE_OFF                     (2)
#define PP_QOS_MISC_UC_BUS_CTRL_MCDMAM0_PWE_LEN                     (1)
#define PP_QOS_MISC_UC_BUS_CTRL_MCDMAM0_PWE_MSK                     (0x00000004)
#define PP_QOS_MISC_UC_BUS_CTRL_MCDMAM0_PWE_RST                     (0x0)
#define PP_QOS_MISC_UC_BUS_CTRL_MCDMAM0_B2L_OFF                     (1)
#define PP_QOS_MISC_UC_BUS_CTRL_MCDMAM0_B2L_LEN                     (1)
#define PP_QOS_MISC_UC_BUS_CTRL_MCDMAM0_B2L_MSK                     (0x00000002)
#define PP_QOS_MISC_UC_BUS_CTRL_MCDMAM0_B2L_RST                     (0x0)
#define PP_QOS_MISC_UC_BUS_CTRL_HOSTM_B2L_OFF                       (0)
#define PP_QOS_MISC_UC_BUS_CTRL_HOSTM_B2L_LEN                       (1)
#define PP_QOS_MISC_UC_BUS_CTRL_HOSTM_B2L_MSK                       (0x00000001)
#define PP_QOS_MISC_UC_BUS_CTRL_HOSTM_B2L_RST                       (0x0)

/**
 * SW_REG_NAME : PP_QOS_MISC_UC_MSC_CTRL_REG
 * HW_REG_NAME : qos_uc_msc_ctrl
 * DESCRIPTION : QOS uC SS Miscellaneous Control
 *
 *  Register Fields :
 *   [31: 2][RW] - Reserved
 *   [ 1: 1][RW] - QoS Interrupt Request toward Host IRQ 3
 *   [ 0: 0][RW] - QoS Interrupt Request toward Host IRQ 2
 *
 */
#define PP_QOS_MISC_UC_MSC_CTRL_REG                ((QOS_MISC_BASE_ADDR) + 0x10)
#define PP_QOS_MISC_UC_MSC_CTRL_RESERVED_OFF                        (2)
#define PP_QOS_MISC_UC_MSC_CTRL_RESERVED_LEN                        (30)
#define PP_QOS_MISC_UC_MSC_CTRL_RESERVED_MSK                        (0xFFFFFFFC)
#define PP_QOS_MISC_UC_MSC_CTRL_RESERVED_RST                        (0x0)
#define PP_QOS_MISC_UC_MSC_CTRL_QOS_IRQ_3_OFF                       (1)
#define PP_QOS_MISC_UC_MSC_CTRL_QOS_IRQ_3_LEN                       (1)
#define PP_QOS_MISC_UC_MSC_CTRL_QOS_IRQ_3_MSK                       (0x00000002)
#define PP_QOS_MISC_UC_MSC_CTRL_QOS_IRQ_3_RST                       (0x0)
#define PP_QOS_MISC_UC_MSC_CTRL_QOS_IRQ_2_OFF                       (0)
#define PP_QOS_MISC_UC_MSC_CTRL_QOS_IRQ_2_LEN                       (1)
#define PP_QOS_MISC_UC_MSC_CTRL_QOS_IRQ_2_MSK                       (0x00000001)
#define PP_QOS_MISC_UC_MSC_CTRL_QOS_IRQ_2_RST                       (0x0)

/**
 * SW_REG_NAME : PP_QOS_MISC_UC_QPS_CTRL_REG
 * HW_REG_NAME : qos_uc_qps_ctrl
 * DESCRIPTION : Queue Pending Status Selector
 *
 *  Register Fields :
 *   [31: 5][RO] - Reserved
 *   [ 4: 0][RW] - Select QP octet for ARC IRC Bundle
 *
 */
#define PP_QOS_MISC_UC_QPS_CTRL_REG                ((QOS_MISC_BASE_ADDR) + 0x14)
#define PP_QOS_MISC_UC_QPS_CTRL_RSVD0_OFF                           (5)
#define PP_QOS_MISC_UC_QPS_CTRL_RSVD0_LEN                           (27)
#define PP_QOS_MISC_UC_QPS_CTRL_RSVD0_MSK                           (0xFFFFFFE0)
#define PP_QOS_MISC_UC_QPS_CTRL_RSVD0_RST                           (0x0)
#define PP_QOS_MISC_UC_QPS_CTRL_OFF                                 (0)
#define PP_QOS_MISC_UC_QPS_CTRL_LEN                                 (5)
#define PP_QOS_MISC_UC_QPS_CTRL_MSK                                 (0x0000001F)
#define PP_QOS_MISC_UC_QPS_CTRL_RST                                 (0x0)

/**
 * SW_REG_NAME : PP_QOS_MISC_UC_PL0_CTRL_REG
 * HW_REG_NAME : qos_uc_pl0_ctrl
 * DESCRIPTION : Ports 3-0 LUT Values. Each octet is a QoS port
 *               number which is assigned to uC
 *
 *  Register Fields :
 *   [31:24][RW] - uC port 3 LUT value
 *   [23:16][RW] - uC port 2 LUT value
 *   [15: 8][RW] - uC port 1 LUT value
 *   [ 7: 0][RW] - uC port 0 LUT value
 *
 */
#define PP_QOS_MISC_UC_PL0_CTRL_REG                ((QOS_MISC_BASE_ADDR) + 0x18)
#define PP_QOS_MISC_UC_PL0_CTRL_PORT3LUVAL_OFF                      (24)
#define PP_QOS_MISC_UC_PL0_CTRL_PORT3LUVAL_LEN                      (8)
#define PP_QOS_MISC_UC_PL0_CTRL_PORT3LUVAL_MSK                      (0xFF000000)
#define PP_QOS_MISC_UC_PL0_CTRL_PORT3LUVAL_RST                      (0x3)
#define PP_QOS_MISC_UC_PL0_CTRL_PORT2LUVAL_OFF                      (16)
#define PP_QOS_MISC_UC_PL0_CTRL_PORT2LUVAL_LEN                      (8)
#define PP_QOS_MISC_UC_PL0_CTRL_PORT2LUVAL_MSK                      (0x00FF0000)
#define PP_QOS_MISC_UC_PL0_CTRL_PORT2LUVAL_RST                      (0x2)
#define PP_QOS_MISC_UC_PL0_CTRL_PORT1LUVAL_OFF                      (8)
#define PP_QOS_MISC_UC_PL0_CTRL_PORT1LUVAL_LEN                      (8)
#define PP_QOS_MISC_UC_PL0_CTRL_PORT1LUVAL_MSK                      (0x0000FF00)
#define PP_QOS_MISC_UC_PL0_CTRL_PORT1LUVAL_RST                      (0x1)
#define PP_QOS_MISC_UC_PL0_CTRL_PORT0LUVAL_OFF                      (0)
#define PP_QOS_MISC_UC_PL0_CTRL_PORT0LUVAL_LEN                      (8)
#define PP_QOS_MISC_UC_PL0_CTRL_PORT0LUVAL_MSK                      (0x000000FF)
#define PP_QOS_MISC_UC_PL0_CTRL_PORT0LUVAL_RST                      (0x0)

/**
 * SW_REG_NAME : PP_QOS_MISC_UC_PL1_CTRL_REG
 * HW_REG_NAME : qos_uc_pl1_ctrl
 * DESCRIPTION : Ports 7-4 LUT Values. Each octet is a QoS port
 *               number which is assigned to uC
 *
 *  Register Fields :
 *   [31:24][RW] - uC port 7 LUT value
 *   [23:16][RW] - uC port 6 LUT value
 *   [15: 8][RW] - uC port 5 LUT value
 *   [ 7: 0][RW] - uC port 4 LUT value
 *
 */
#define PP_QOS_MISC_UC_PL1_CTRL_REG                ((QOS_MISC_BASE_ADDR) + 0x1C)
#define PP_QOS_MISC_UC_PL1_CTRL_PORT7LUVAL_OFF                      (24)
#define PP_QOS_MISC_UC_PL1_CTRL_PORT7LUVAL_LEN                      (8)
#define PP_QOS_MISC_UC_PL1_CTRL_PORT7LUVAL_MSK                      (0xFF000000)
#define PP_QOS_MISC_UC_PL1_CTRL_PORT7LUVAL_RST                      (0x7)
#define PP_QOS_MISC_UC_PL1_CTRL_PORT6LUVAL_OFF                      (16)
#define PP_QOS_MISC_UC_PL1_CTRL_PORT6LUVAL_LEN                      (8)
#define PP_QOS_MISC_UC_PL1_CTRL_PORT6LUVAL_MSK                      (0x00FF0000)
#define PP_QOS_MISC_UC_PL1_CTRL_PORT6LUVAL_RST                      (0x6)
#define PP_QOS_MISC_UC_PL1_CTRL_PORT5LUVAL_OFF                      (8)
#define PP_QOS_MISC_UC_PL1_CTRL_PORT5LUVAL_LEN                      (8)
#define PP_QOS_MISC_UC_PL1_CTRL_PORT5LUVAL_MSK                      (0x0000FF00)
#define PP_QOS_MISC_UC_PL1_CTRL_PORT5LUVAL_RST                      (0x5)
#define PP_QOS_MISC_UC_PL1_CTRL_PORT4LUVAL_OFF                      (0)
#define PP_QOS_MISC_UC_PL1_CTRL_PORT4LUVAL_LEN                      (8)
#define PP_QOS_MISC_UC_PL1_CTRL_PORT4LUVAL_MSK                      (0x000000FF)
#define PP_QOS_MISC_UC_PL1_CTRL_PORT4LUVAL_RST                      (0x4)

/**
 * SW_REG_NAME : PP_QOS_MISC_UC_AT0_CTRL_REG
 * HW_REG_NAME : qos_uc_at0_ctrl
 * DESCRIPTION : ARC uC Address Translation Table (Part0). Each
 *               nibble is a new value of UC_M_MAddr[35:28] bits
 *               goes outside the ARC SS
 *
 *  Register Fields :
 *   [31:24][RW] - New value over original UC_M_MAddr[35:28] = 3
 *   [23:16][RW] - New value over original UC_M_MAddr[35:28] = 2
 *   [15: 8][RW] - New value over original UC_M_MAddr[35:28] = 1
 *   [ 7: 0][RW] - New value over original UC_M_MAddr[35:28] = 0
 *
 */
#define PP_QOS_MISC_UC_AT0_CTRL_REG                ((QOS_MISC_BASE_ADDR) + 0x20)
#define PP_QOS_MISC_UC_AT0_CTRL_N3LUVAL_OFF                         (24)
#define PP_QOS_MISC_UC_AT0_CTRL_N3LUVAL_LEN                         (8)
#define PP_QOS_MISC_UC_AT0_CTRL_N3LUVAL_MSK                         (0xFF000000)
#define PP_QOS_MISC_UC_AT0_CTRL_N3LUVAL_RST                         (0x3)
#define PP_QOS_MISC_UC_AT0_CTRL_N2LUVAL_OFF                         (16)
#define PP_QOS_MISC_UC_AT0_CTRL_N2LUVAL_LEN                         (8)
#define PP_QOS_MISC_UC_AT0_CTRL_N2LUVAL_MSK                         (0x00FF0000)
#define PP_QOS_MISC_UC_AT0_CTRL_N2LUVAL_RST                         (0x2)
#define PP_QOS_MISC_UC_AT0_CTRL_N1LUVAL_OFF                         (8)
#define PP_QOS_MISC_UC_AT0_CTRL_N1LUVAL_LEN                         (8)
#define PP_QOS_MISC_UC_AT0_CTRL_N1LUVAL_MSK                         (0x0000FF00)
#define PP_QOS_MISC_UC_AT0_CTRL_N1LUVAL_RST                         (0x1)
#define PP_QOS_MISC_UC_AT0_CTRL_N0LUVAL_OFF                         (0)
#define PP_QOS_MISC_UC_AT0_CTRL_N0LUVAL_LEN                         (8)
#define PP_QOS_MISC_UC_AT0_CTRL_N0LUVAL_MSK                         (0x000000FF)
#define PP_QOS_MISC_UC_AT0_CTRL_N0LUVAL_RST                         (0x0)

/**
 * SW_REG_NAME : PP_QOS_MISC_UC_AT1_CTRL_REG
 * HW_REG_NAME : qos_uc_at1_ctrl
 * DESCRIPTION : ARC uC Address Translation Table (Part1). Each
 *               nibble is a new value of UC_M_MAddr[35:28] bits
 *               goes outside the ARC SS
 *
 *  Register Fields :
 *   [31:24][RW] - New value over original UC_M_MAddr[35:28] = 7
 *   [23:16][RW] - New value over original UC_M_MAddr[35:28] = 6
 *   [15: 8][RW] - New value over original UC_M_MAddr[35:28] = 5
 *   [ 7: 0][RW] - New value over original UC_M_MAddr[35:28] = 4
 *
 */
#define PP_QOS_MISC_UC_AT1_CTRL_REG                ((QOS_MISC_BASE_ADDR) + 0x24)
#define PP_QOS_MISC_UC_AT1_CTRL_N7LUVAL_OFF                         (24)
#define PP_QOS_MISC_UC_AT1_CTRL_N7LUVAL_LEN                         (8)
#define PP_QOS_MISC_UC_AT1_CTRL_N7LUVAL_MSK                         (0xFF000000)
#define PP_QOS_MISC_UC_AT1_CTRL_N7LUVAL_RST                         (0x7)
#define PP_QOS_MISC_UC_AT1_CTRL_N6LUVAL_OFF                         (16)
#define PP_QOS_MISC_UC_AT1_CTRL_N6LUVAL_LEN                         (8)
#define PP_QOS_MISC_UC_AT1_CTRL_N6LUVAL_MSK                         (0x00FF0000)
#define PP_QOS_MISC_UC_AT1_CTRL_N6LUVAL_RST                         (0x6)
#define PP_QOS_MISC_UC_AT1_CTRL_N5LUVAL_OFF                         (8)
#define PP_QOS_MISC_UC_AT1_CTRL_N5LUVAL_LEN                         (8)
#define PP_QOS_MISC_UC_AT1_CTRL_N5LUVAL_MSK                         (0x0000FF00)
#define PP_QOS_MISC_UC_AT1_CTRL_N5LUVAL_RST                         (0x5)
#define PP_QOS_MISC_UC_AT1_CTRL_N4LUVAL_OFF                         (0)
#define PP_QOS_MISC_UC_AT1_CTRL_N4LUVAL_LEN                         (8)
#define PP_QOS_MISC_UC_AT1_CTRL_N4LUVAL_MSK                         (0x000000FF)
#define PP_QOS_MISC_UC_AT1_CTRL_N4LUVAL_RST                         (0x4)

/**
 * SW_REG_NAME : PP_QOS_MISC_UC_MSC_STAT_REG
 * HW_REG_NAME : qos_uc_msc_stat
 * DESCRIPTION : QOS uC SS Miscellaneous Status
 *
 *  Register Fields :
 *   [31:24][RO] - Reserved
 *   [23:16][RO] - Status bitmap of uC TX Port Packet no credit
 *   [15: 8][RO] - TX Ports 7-0 Push Status
 *   [ 7: 7][RO] - Reserved
 *   [ 6: 6][RO] - ARC Core Sys Halt
 *   [ 5: 5][RO] - ARC Core Sys Sleep
 *   [ 4: 2][RO] - ARC Core Sys Sleep Mode
 *   [ 1: 1][RO] - ARC Core Halt Acknowledge
 *   [ 0: 0][RO] - ARC Core Run Acknowledge
 *
 */
#define PP_QOS_MISC_UC_MSC_STAT_REG                ((QOS_MISC_BASE_ADDR) + 0x28)
#define PP_QOS_MISC_UC_MSC_STAT_RESERVED1_OFF                       (24)
#define PP_QOS_MISC_UC_MSC_STAT_RESERVED1_LEN                       (8)
#define PP_QOS_MISC_UC_MSC_STAT_RESERVED1_MSK                       (0xFF000000)
#define PP_QOS_MISC_UC_MSC_STAT_RESERVED1_RST                       (0x0)
#define PP_QOS_MISC_UC_MSC_STAT_TXMGR_PKTNOCRED_PORT_OFF            (16)
#define PP_QOS_MISC_UC_MSC_STAT_TXMGR_PKTNOCRED_PORT_LEN            (8)
#define PP_QOS_MISC_UC_MSC_STAT_TXMGR_PKTNOCRED_PORT_MSK            (0x00FF0000)
#define PP_QOS_MISC_UC_MSC_STAT_TXMGR_PKTNOCRED_PORT_RST            (0x0)
#define PP_QOS_MISC_UC_MSC_STAT_TXMGR_LU_PUSH_PORT_OFF              (8)
#define PP_QOS_MISC_UC_MSC_STAT_TXMGR_LU_PUSH_PORT_LEN              (8)
#define PP_QOS_MISC_UC_MSC_STAT_TXMGR_LU_PUSH_PORT_MSK              (0x0000FF00)
#define PP_QOS_MISC_UC_MSC_STAT_TXMGR_LU_PUSH_PORT_RST              (0x0)
#define PP_QOS_MISC_UC_MSC_STAT_RESERVED0_OFF                       (7)
#define PP_QOS_MISC_UC_MSC_STAT_RESERVED0_LEN                       (1)
#define PP_QOS_MISC_UC_MSC_STAT_RESERVED0_MSK                       (0x00000080)
#define PP_QOS_MISC_UC_MSC_STAT_RESERVED0_RST                       (0x0)
#define PP_QOS_MISC_UC_MSC_STAT_ARC_SYS_HALT_OFF                    (6)
#define PP_QOS_MISC_UC_MSC_STAT_ARC_SYS_HALT_LEN                    (1)
#define PP_QOS_MISC_UC_MSC_STAT_ARC_SYS_HALT_MSK                    (0x00000040)
#define PP_QOS_MISC_UC_MSC_STAT_ARC_SYS_HALT_RST                    (0x0)
#define PP_QOS_MISC_UC_MSC_STAT_ARC_SYS_SLEEP_OFF                   (5)
#define PP_QOS_MISC_UC_MSC_STAT_ARC_SYS_SLEEP_LEN                   (1)
#define PP_QOS_MISC_UC_MSC_STAT_ARC_SYS_SLEEP_MSK                   (0x00000020)
#define PP_QOS_MISC_UC_MSC_STAT_ARC_SYS_SLEEP_RST                   (0x0)
#define PP_QOS_MISC_UC_MSC_STAT_ARC_SYS_SLP_MOD_OFF                 (2)
#define PP_QOS_MISC_UC_MSC_STAT_ARC_SYS_SLP_MOD_LEN                 (3)
#define PP_QOS_MISC_UC_MSC_STAT_ARC_SYS_SLP_MOD_MSK                 (0x0000001C)
#define PP_QOS_MISC_UC_MSC_STAT_ARC_SYS_SLP_MOD_RST                 (0x0)
#define PP_QOS_MISC_UC_MSC_STAT_ARC_HLT_ACK_OFF                     (1)
#define PP_QOS_MISC_UC_MSC_STAT_ARC_HLT_ACK_LEN                     (1)
#define PP_QOS_MISC_UC_MSC_STAT_ARC_HLT_ACK_MSK                     (0x00000002)
#define PP_QOS_MISC_UC_MSC_STAT_ARC_HLT_ACK_RST                     (0x0)
#define PP_QOS_MISC_UC_MSC_STAT_ARC_RUN_ACK_OFF                     (0)
#define PP_QOS_MISC_UC_MSC_STAT_ARC_RUN_ACK_LEN                     (1)
#define PP_QOS_MISC_UC_MSC_STAT_ARC_RUN_ACK_MSK                     (0x00000001)
#define PP_QOS_MISC_UC_MSC_STAT_ARC_RUN_ACK_RST                     (0x0)

/**
 * SW_REG_NAME : PP_QOS_MISC_UC_BP0_STAT_REG
 * HW_REG_NAME : qos_uc_bp0_stat
 * DESCRIPTION : TXMGR CMD IF Back Pressure Status on Ports 31-0
 *
 *  Register Fields :
 *   [31: 0][RO] - TXMGR CMD IF Back Pressure Status on Ports 31-0
 *
 */
#define PP_QOS_MISC_UC_BP0_STAT_REG                ((QOS_MISC_BASE_ADDR) + 0x2C)
#define PP_QOS_MISC_UC_BP0_STAT_BP_PORTS31_0_OFF                    (0)
#define PP_QOS_MISC_UC_BP0_STAT_BP_PORTS31_0_LEN                    (32)
#define PP_QOS_MISC_UC_BP0_STAT_BP_PORTS31_0_MSK                    (0xFFFFFFFF)
#define PP_QOS_MISC_UC_BP0_STAT_BP_PORTS31_0_RST                    (0x0)

/**
 * SW_REG_NAME : PP_QOS_MISC_UC_BP1_STAT_REG
 * HW_REG_NAME : qos_uc_bp1_stat
 * DESCRIPTION : TXMGR CMD IF Back Pressure Status on Ports 63-32
 *
 *  Register Fields :
 *   [31: 0][RO] - TXMGR CMD IF Back Pressure Status on Ports 63-32
 *
 */
#define PP_QOS_MISC_UC_BP1_STAT_REG                ((QOS_MISC_BASE_ADDR) + 0x30)
#define PP_QOS_MISC_UC_BP1_STAT_BP_PORTS63_32_OFF                   (0)
#define PP_QOS_MISC_UC_BP1_STAT_BP_PORTS63_32_LEN                   (32)
#define PP_QOS_MISC_UC_BP1_STAT_BP_PORTS63_32_MSK                   (0xFFFFFFFF)
#define PP_QOS_MISC_UC_BP1_STAT_BP_PORTS63_32_RST                   (0x0)

/**
 * SW_REG_NAME : PP_QOS_MISC_UC_BP2_STAT_REG
 * HW_REG_NAME : qos_uc_bp2_stat
 * DESCRIPTION : TXMGR CMD IF Back Pressure Status on Ports 95-64
 *
 *  Register Fields :
 *   [31: 0][RO] - TXMGR CMD IF Back Pressure Status on Ports 95-64
 *
 */
#define PP_QOS_MISC_UC_BP2_STAT_REG                ((QOS_MISC_BASE_ADDR) + 0x34)
#define PP_QOS_MISC_UC_BP2_STAT_BP_PORTS95_64_OFF                   (0)
#define PP_QOS_MISC_UC_BP2_STAT_BP_PORTS95_64_LEN                   (32)
#define PP_QOS_MISC_UC_BP2_STAT_BP_PORTS95_64_MSK                   (0xFFFFFFFF)
#define PP_QOS_MISC_UC_BP2_STAT_BP_PORTS95_64_RST                   (0x0)

/**
 * SW_REG_NAME : PP_QOS_MISC_UC_BP3_STAT_REG
 * HW_REG_NAME : qos_uc_bp3_stat
 * DESCRIPTION : TXMGR CMD IF Back Pressure Status on Ports 127-96
 *
 *  Register Fields :
 *   [31: 0][RO] - TXMGR CMD IF Back Pressure Status on Ports 127-96
 *
 */
#define PP_QOS_MISC_UC_BP3_STAT_REG                ((QOS_MISC_BASE_ADDR) + 0x38)
#define PP_QOS_MISC_UC_BP3_STAT_BP_PORT127_96_OFF                   (0)
#define PP_QOS_MISC_UC_BP3_STAT_BP_PORT127_96_LEN                   (32)
#define PP_QOS_MISC_UC_BP3_STAT_BP_PORT127_96_MSK                   (0xFFFFFFFF)
#define PP_QOS_MISC_UC_BP3_STAT_BP_PORT127_96_RST                   (0x0)

/**
 * SW_REG_NAME : PP_QOS_MISC_UC_PC0_STAT_REG
 * HW_REG_NAME : qos_uc_pc0_stat
 * DESCRIPTION : Packet Credits Port 0
 *
 *  Register Fields :
 *   [31: 0][RO] - Packet Credit Counter Port 0
 *
 */
#define PP_QOS_MISC_UC_PC0_STAT_REG                ((QOS_MISC_BASE_ADDR) + 0x3C)
#define PP_QOS_MISC_UC_PC0_STAT_PC_PORT0_OFF                        (0)
#define PP_QOS_MISC_UC_PC0_STAT_PC_PORT0_LEN                        (32)
#define PP_QOS_MISC_UC_PC0_STAT_PC_PORT0_MSK                        (0xFFFFFFFF)
#define PP_QOS_MISC_UC_PC0_STAT_PC_PORT0_RST                        (0x0)

/**
 * SW_REG_NAME : PP_QOS_MISC_UC_PC1_STAT_REG
 * HW_REG_NAME : qos_uc_pc1_stat
 * DESCRIPTION : Packet Credits Port 1
 *
 *  Register Fields :
 *   [31: 0][RO] - Packet Credit Counter Port 1
 *
 */
#define PP_QOS_MISC_UC_PC1_STAT_REG                ((QOS_MISC_BASE_ADDR) + 0x40)
#define PP_QOS_MISC_UC_PC1_STAT_PC_PORT1_OFF                        (0)
#define PP_QOS_MISC_UC_PC1_STAT_PC_PORT1_LEN                        (32)
#define PP_QOS_MISC_UC_PC1_STAT_PC_PORT1_MSK                        (0xFFFFFFFF)
#define PP_QOS_MISC_UC_PC1_STAT_PC_PORT1_RST                        (0x0)

/**
 * SW_REG_NAME : PP_QOS_MISC_UC_PC2_STAT_REG
 * HW_REG_NAME : qos_uc_pc2_stat
 * DESCRIPTION : Packet Credits Port 2
 *
 *  Register Fields :
 *   [31: 0][RO] - Packet Credit Counter Port 2
 *
 */
#define PP_QOS_MISC_UC_PC2_STAT_REG                ((QOS_MISC_BASE_ADDR) + 0x44)
#define PP_QOS_MISC_UC_PC2_STAT_PC_PORT2_OFF                        (0)
#define PP_QOS_MISC_UC_PC2_STAT_PC_PORT2_LEN                        (32)
#define PP_QOS_MISC_UC_PC2_STAT_PC_PORT2_MSK                        (0xFFFFFFFF)
#define PP_QOS_MISC_UC_PC2_STAT_PC_PORT2_RST                        (0x0)

/**
 * SW_REG_NAME : PP_QOS_MISC_UC_PC3_STAT_REG
 * HW_REG_NAME : qos_uc_pc3_stat
 * DESCRIPTION : Packet Credits Port 3
 *
 *  Register Fields :
 *   [31: 0][RO] - Packet Credit Counter Port 3
 *
 */
#define PP_QOS_MISC_UC_PC3_STAT_REG                ((QOS_MISC_BASE_ADDR) + 0x48)
#define PP_QOS_MISC_UC_PC3_STAT_PC_PORT3_OFF                        (0)
#define PP_QOS_MISC_UC_PC3_STAT_PC_PORT3_LEN                        (32)
#define PP_QOS_MISC_UC_PC3_STAT_PC_PORT3_MSK                        (0xFFFFFFFF)
#define PP_QOS_MISC_UC_PC3_STAT_PC_PORT3_RST                        (0x0)

/**
 * SW_REG_NAME : PP_QOS_MISC_UC_PC4_STAT_REG
 * HW_REG_NAME : qos_uc_pc4_stat
 * DESCRIPTION : Packet Credits Port 4
 *
 *  Register Fields :
 *   [31: 0][RO] - Packet Credit Counter Port 4
 *
 */
#define PP_QOS_MISC_UC_PC4_STAT_REG                ((QOS_MISC_BASE_ADDR) + 0x4C)
#define PP_QOS_MISC_UC_PC4_STAT_PC_PORT4_OFF                        (0)
#define PP_QOS_MISC_UC_PC4_STAT_PC_PORT4_LEN                        (32)
#define PP_QOS_MISC_UC_PC4_STAT_PC_PORT4_MSK                        (0xFFFFFFFF)
#define PP_QOS_MISC_UC_PC4_STAT_PC_PORT4_RST                        (0x0)

/**
 * SW_REG_NAME : PP_QOS_MISC_UC_PC5_STAT_REG
 * HW_REG_NAME : qos_uc_pc5_stat
 * DESCRIPTION : Packet Credits Port 5
 *
 *  Register Fields :
 *   [31: 0][RO] - Packet Credit Counter Port 5
 *
 */
#define PP_QOS_MISC_UC_PC5_STAT_REG                ((QOS_MISC_BASE_ADDR) + 0x50)
#define PP_QOS_MISC_UC_PC5_STAT_PC_PORT5_OFF                        (0)
#define PP_QOS_MISC_UC_PC5_STAT_PC_PORT5_LEN                        (32)
#define PP_QOS_MISC_UC_PC5_STAT_PC_PORT5_MSK                        (0xFFFFFFFF)
#define PP_QOS_MISC_UC_PC5_STAT_PC_PORT5_RST                        (0x0)

/**
 * SW_REG_NAME : PP_QOS_MISC_UC_PC6_STAT_REG
 * HW_REG_NAME : qos_uc_pc6_stat
 * DESCRIPTION : Packet Credits Port 6
 *
 *  Register Fields :
 *   [31: 0][RO] - Packet Credit Counter Port 6
 *
 */
#define PP_QOS_MISC_UC_PC6_STAT_REG                ((QOS_MISC_BASE_ADDR) + 0x54)
#define PP_QOS_MISC_UC_PC6_STAT_PC_PORT6_OFF                        (0)
#define PP_QOS_MISC_UC_PC6_STAT_PC_PORT6_LEN                        (32)
#define PP_QOS_MISC_UC_PC6_STAT_PC_PORT6_MSK                        (0xFFFFFFFF)
#define PP_QOS_MISC_UC_PC6_STAT_PC_PORT6_RST                        (0x0)

/**
 * SW_REG_NAME : PP_QOS_MISC_UC_PC7_STAT_REG
 * HW_REG_NAME : qos_uc_pc7_stat
 * DESCRIPTION : Packet Credits Port 7
 *
 *  Register Fields :
 *   [31: 0][RO] - Packet Credit Counter Port 7
 *
 */
#define PP_QOS_MISC_UC_PC7_STAT_REG                ((QOS_MISC_BASE_ADDR) + 0x58)
#define PP_QOS_MISC_UC_PC7_STAT_PC_PORT7_OFF                        (0)
#define PP_QOS_MISC_UC_PC7_STAT_PC_PORT7_LEN                        (32)
#define PP_QOS_MISC_UC_PC7_STAT_PC_PORT7_MSK                        (0xFFFFFFFF)
#define PP_QOS_MISC_UC_PC7_STAT_PC_PORT7_RST                        (0x0)

/**
 * SW_REG_NAME : PP_QOS_MISC_UC_RAND_VAL_REG
 * HW_REG_NAME : qos_uc_rand_val
 * DESCRIPTION : Randomizer LFSR generator. Each read returns next
 *               pseudorandom value according to polynom
 *               Initial value can be written
 *
 *  Register Fields :
 *   [31: 0][RW] - Randomizer value
 *
 */
#define PP_QOS_MISC_UC_RAND_VAL_REG                ((QOS_MISC_BASE_ADDR) + 0x5C)
#define PP_QOS_MISC_UC_RAND_VAL_RND_VAL_OFF                         (0)
#define PP_QOS_MISC_UC_RAND_VAL_RND_VAL_LEN                         (32)
#define PP_QOS_MISC_UC_RAND_VAL_RND_VAL_MSK                         (0xFFFFFFFF)
#define PP_QOS_MISC_UC_RAND_VAL_RND_VAL_RST                         (0x0)

/**
 * SW_REG_NAME : PP_QOS_MISC_UC_MSC_EVNT_REG
 * HW_REG_NAME : qos_uc_msc_evnt
 * DESCRIPTION : Miscelaneous Events Initiated by uC
 *
 *  Register Fields :
 *   [31: 8][RO] - Reserved
 *   [ 7: 0][RW] - Decrement Host Tx Credit Port Counter by writing 1
 *
 */
#define PP_QOS_MISC_UC_MSC_EVNT_REG                ((QOS_MISC_BASE_ADDR) + 0x60)
#define PP_QOS_MISC_UC_MSC_EVNT_RESERVED0_OFF                       (8)
#define PP_QOS_MISC_UC_MSC_EVNT_RESERVED0_LEN                       (24)
#define PP_QOS_MISC_UC_MSC_EVNT_RESERVED0_MSK                       (0xFFFFFF00)
#define PP_QOS_MISC_UC_MSC_EVNT_RESERVED0_RST                       (0x0)
#define PP_QOS_MISC_UC_MSC_EVNT_DEC_CNTR_OFF                        (0)
#define PP_QOS_MISC_UC_MSC_EVNT_DEC_CNTR_LEN                        (8)
#define PP_QOS_MISC_UC_MSC_EVNT_DEC_CNTR_MSK                        (0x000000FF)
#define PP_QOS_MISC_UC_MSC_EVNT_DEC_CNTR_RST                        (0x0)

/**
 * SW_REG_NAME : PP_QOS_MISC_UC_MSC_STAT1_REG
 * HW_REG_NAME : qos_uc_msc_stat1
 * DESCRIPTION : QOS uC SS Miscellaneous Status 1
 *
 *  Register Fields :
 *   [31:21][RO] - Reserved
 *   [20:20][RO] - Clock Request Status: uC
 *   [19:19][RO] - Clock Request Status: TXMGR
 *   [18:18][RO] - Clock Request Status: QMGR
 *   [17:17][RO] - Clock Request Status: TSCD
 *   [16:16][RO] - Clock Request Status: WRED
 *   [15: 9][RO] - Reserved
 *   [ 8: 6][RO] - ppv4_hw_dfs_lvl_notify [2:0]
 *   [ 5: 3][RO] - ppv4_uc_dfs_lvl_notify [2:0]
 *   [ 2: 0][RO] - ppv4_hw_dfs_lvl_notify [2:0]
 *
 */
#define PP_QOS_MISC_UC_MSC_STAT1_REG               ((QOS_MISC_BASE_ADDR) + 0x64)
#define PP_QOS_MISC_UC_MSC_STAT1_RESERVED1_OFF                      (21)
#define PP_QOS_MISC_UC_MSC_STAT1_RESERVED1_LEN                      (11)
#define PP_QOS_MISC_UC_MSC_STAT1_RESERVED1_MSK                      (0xFFE00000)
#define PP_QOS_MISC_UC_MSC_STAT1_RESERVED1_RST                      (0x0)
#define PP_QOS_MISC_UC_MSC_STAT1_CLOCK_REQUEST_STAT4_OFF            (20)
#define PP_QOS_MISC_UC_MSC_STAT1_CLOCK_REQUEST_STAT4_LEN            (1)
#define PP_QOS_MISC_UC_MSC_STAT1_CLOCK_REQUEST_STAT4_MSK            (0x00100000)
#define PP_QOS_MISC_UC_MSC_STAT1_CLOCK_REQUEST_STAT4_RST            (0x0)
#define PP_QOS_MISC_UC_MSC_STAT1_CLOCK_REQUEST_STAT3_OFF            (19)
#define PP_QOS_MISC_UC_MSC_STAT1_CLOCK_REQUEST_STAT3_LEN            (1)
#define PP_QOS_MISC_UC_MSC_STAT1_CLOCK_REQUEST_STAT3_MSK            (0x00080000)
#define PP_QOS_MISC_UC_MSC_STAT1_CLOCK_REQUEST_STAT3_RST            (0x0)
#define PP_QOS_MISC_UC_MSC_STAT1_CLOCK_REQUEST_STAT2_OFF            (18)
#define PP_QOS_MISC_UC_MSC_STAT1_CLOCK_REQUEST_STAT2_LEN            (1)
#define PP_QOS_MISC_UC_MSC_STAT1_CLOCK_REQUEST_STAT2_MSK            (0x00040000)
#define PP_QOS_MISC_UC_MSC_STAT1_CLOCK_REQUEST_STAT2_RST            (0x0)
#define PP_QOS_MISC_UC_MSC_STAT1_CLOCK_REQUEST_STAT1_OFF            (17)
#define PP_QOS_MISC_UC_MSC_STAT1_CLOCK_REQUEST_STAT1_LEN            (1)
#define PP_QOS_MISC_UC_MSC_STAT1_CLOCK_REQUEST_STAT1_MSK            (0x00020000)
#define PP_QOS_MISC_UC_MSC_STAT1_CLOCK_REQUEST_STAT1_RST            (0x0)
#define PP_QOS_MISC_UC_MSC_STAT1_CLOCK_REQUEST_STAT0_OFF            (16)
#define PP_QOS_MISC_UC_MSC_STAT1_CLOCK_REQUEST_STAT0_LEN            (1)
#define PP_QOS_MISC_UC_MSC_STAT1_CLOCK_REQUEST_STAT0_MSK            (0x00010000)
#define PP_QOS_MISC_UC_MSC_STAT1_CLOCK_REQUEST_STAT0_RST            (0x0)
#define PP_QOS_MISC_UC_MSC_STAT1_RESERVED0_OFF                      (9)
#define PP_QOS_MISC_UC_MSC_STAT1_RESERVED0_LEN                      (7)
#define PP_QOS_MISC_UC_MSC_STAT1_RESERVED0_MSK                      (0x0000FE00)
#define PP_QOS_MISC_UC_MSC_STAT1_RESERVED0_RST                      (0x0)
#define PP_QOS_MISC_UC_MSC_STAT1_PPV4_TBM_DFS_LVL_NOTIFY_OFF        (6)
#define PP_QOS_MISC_UC_MSC_STAT1_PPV4_TBM_DFS_LVL_NOTIFY_LEN        (3)
#define PP_QOS_MISC_UC_MSC_STAT1_PPV4_TBM_DFS_LVL_NOTIFY_MSK        (0x000001C0)
#define PP_QOS_MISC_UC_MSC_STAT1_PPV4_TBM_DFS_LVL_NOTIFY_RST        (0x0)
#define PP_QOS_MISC_UC_MSC_STAT1_PPV4_UC_DFS_LVL_NOTIFY_OFF         (3)
#define PP_QOS_MISC_UC_MSC_STAT1_PPV4_UC_DFS_LVL_NOTIFY_LEN         (3)
#define PP_QOS_MISC_UC_MSC_STAT1_PPV4_UC_DFS_LVL_NOTIFY_MSK         (0x00000038)
#define PP_QOS_MISC_UC_MSC_STAT1_PPV4_UC_DFS_LVL_NOTIFY_RST         (0x0)
#define PP_QOS_MISC_UC_MSC_STAT1_PPV4_HW_DFS_LVL_NOTIFY_OFF         (0)
#define PP_QOS_MISC_UC_MSC_STAT1_PPV4_HW_DFS_LVL_NOTIFY_LEN         (3)
#define PP_QOS_MISC_UC_MSC_STAT1_PPV4_HW_DFS_LVL_NOTIFY_MSK         (0x00000007)
#define PP_QOS_MISC_UC_MSC_STAT1_PPV4_HW_DFS_LVL_NOTIFY_RST         (0x0)

/**
 * SW_REG_NAME : PP_QOS_MISC_UC_BP4_STAT_REG
 * HW_REG_NAME : qos_uc_bp4_stat
 * DESCRIPTION : TXMGR CMD IF Back Pressure Status on Ports 159-128
 *
 *  Register Fields :
 *   [31: 0][RO] - TXMGR CMD IF Back Pressure Status on Ports 159-128
 *
 */
#define PP_QOS_MISC_UC_BP4_STAT_REG                ((QOS_MISC_BASE_ADDR) + 0x68)
#define PP_QOS_MISC_UC_BP4_STAT_BP_PORT159_128_OFF                  (0)
#define PP_QOS_MISC_UC_BP4_STAT_BP_PORT159_128_LEN                  (32)
#define PP_QOS_MISC_UC_BP4_STAT_BP_PORT159_128_MSK                  (0xFFFFFFFF)
#define PP_QOS_MISC_UC_BP4_STAT_BP_PORT159_128_RST                  (0x0)

/**
 * SW_REG_NAME : PP_QOS_MISC_UC_BP5_STAT_REG
 * HW_REG_NAME : qos_uc_bp5_stat
 * DESCRIPTION : TXMGR CMD IF Back Pressure Status on Ports 191-160
 *
 *  Register Fields :
 *   [31: 0][RO] - TXMGR CMD IF Back Pressure Status on Ports 191-160
 *
 */
#define PP_QOS_MISC_UC_BP5_STAT_REG                ((QOS_MISC_BASE_ADDR) + 0x6C)
#define PP_QOS_MISC_UC_BP5_STAT_BP_PORT191_160_OFF                  (0)
#define PP_QOS_MISC_UC_BP5_STAT_BP_PORT191_160_LEN                  (32)
#define PP_QOS_MISC_UC_BP5_STAT_BP_PORT191_160_MSK                  (0xFFFFFFFF)
#define PP_QOS_MISC_UC_BP5_STAT_BP_PORT191_160_RST                  (0x0)

/**
 * SW_REG_NAME : PP_QOS_MISC_UC_BP6_STAT_REG
 * HW_REG_NAME : qos_uc_bp6_stat
 * DESCRIPTION : TXMGR CMD IF Back Pressure Status on Ports 223-192
 *
 *  Register Fields :
 *   [31: 0][RO] - TXMGR CMD IF Back Pressure Status on Ports 223-192
 *
 */
#define PP_QOS_MISC_UC_BP6_STAT_REG                ((QOS_MISC_BASE_ADDR) + 0x70)
#define PP_QOS_MISC_UC_BP6_STAT_BP_PORT223_192_OFF                  (0)
#define PP_QOS_MISC_UC_BP6_STAT_BP_PORT223_192_LEN                  (32)
#define PP_QOS_MISC_UC_BP6_STAT_BP_PORT223_192_MSK                  (0xFFFFFFFF)
#define PP_QOS_MISC_UC_BP6_STAT_BP_PORT223_192_RST                  (0x0)

/**
 * SW_REG_NAME : PP_QOS_MISC_UC_BP7_STAT_REG
 * HW_REG_NAME : qos_uc_bp7_stat
 * DESCRIPTION : TXMGR CMD IF Back Pressure Status on Ports 255-224
 *
 *  Register Fields :
 *   [31: 0][RO] - TXMGR CMD IF Back Pressure Status on Ports 255-224
 *
 */
#define PP_QOS_MISC_UC_BP7_STAT_REG                ((QOS_MISC_BASE_ADDR) + 0x74)
#define PP_QOS_MISC_UC_BP7_STAT_BP_PORT255_224_OFF                  (0)
#define PP_QOS_MISC_UC_BP7_STAT_BP_PORT255_224_LEN                  (32)
#define PP_QOS_MISC_UC_BP7_STAT_BP_PORT255_224_MSK                  (0xFFFFFFFF)
#define PP_QOS_MISC_UC_BP7_STAT_BP_PORT255_224_RST                  (0x0)

/**
 * SW_REG_NAME : PP_QOS_MISC_UC_AT2_CTRL_REG
 * HW_REG_NAME : qos_uc_at2_ctrl
 * DESCRIPTION : ARC uC Address Translation Table (Part2). Each
 *               nibble is a new value of UC_M_MAddr[35:28] bits
 *               goes outside the ARC SS
 *
 *  Register Fields :
 *   [31:24][RW] - New value over original UC_M_MAddr[35:28] = B
 *   [23:16][RW] - New value over original UC_M_MAddr[35:28] = A
 *   [15: 8][RW] - New value over original UC_M_MAddr[35:28] = 9
 *   [ 7: 0][RW] - New value over original UC_M_MAddr[35:28] = 8
 *
 */
#define PP_QOS_MISC_UC_AT2_CTRL_REG                ((QOS_MISC_BASE_ADDR) + 0x78)
#define PP_QOS_MISC_UC_AT2_CTRL_NBLUVAL_OFF                         (24)
#define PP_QOS_MISC_UC_AT2_CTRL_NBLUVAL_LEN                         (8)
#define PP_QOS_MISC_UC_AT2_CTRL_NBLUVAL_MSK                         (0xFF000000)
#define PP_QOS_MISC_UC_AT2_CTRL_NBLUVAL_RST                         (0xb)
#define PP_QOS_MISC_UC_AT2_CTRL_NALUVAL_OFF                         (16)
#define PP_QOS_MISC_UC_AT2_CTRL_NALUVAL_LEN                         (8)
#define PP_QOS_MISC_UC_AT2_CTRL_NALUVAL_MSK                         (0x00FF0000)
#define PP_QOS_MISC_UC_AT2_CTRL_NALUVAL_RST                         (0xa)
#define PP_QOS_MISC_UC_AT2_CTRL_N9LUVAL_OFF                         (8)
#define PP_QOS_MISC_UC_AT2_CTRL_N9LUVAL_LEN                         (8)
#define PP_QOS_MISC_UC_AT2_CTRL_N9LUVAL_MSK                         (0x0000FF00)
#define PP_QOS_MISC_UC_AT2_CTRL_N9LUVAL_RST                         (0x9)
#define PP_QOS_MISC_UC_AT2_CTRL_N8LUVAL_OFF                         (0)
#define PP_QOS_MISC_UC_AT2_CTRL_N8LUVAL_LEN                         (8)
#define PP_QOS_MISC_UC_AT2_CTRL_N8LUVAL_MSK                         (0x000000FF)
#define PP_QOS_MISC_UC_AT2_CTRL_N8LUVAL_RST                         (0x8)

/**
 * SW_REG_NAME : PP_QOS_MISC_UC_AT3_CTRL_REG
 * HW_REG_NAME : qos_uc_at3_ctrl
 * DESCRIPTION : ARC uC Address Translation Table (Part3). Each
 *               nibble is a new value of UC_M_MAddr[35:28] bits
 *               goes outside the ARC SS
 *
 *  Register Fields :
 *   [31:24][RW] - New value over original UC_M_MAddr[35:28] = F
 *   [23:16][RW] - New value over original UC_M_MAddr[35:28] = E
 *   [15: 8][RW] - New value over original UC_M_MAddr[35:28] = D
 *   [ 7: 0][RW] - New value over original UC_M_MAddr[35:28] = C
 *
 */
#define PP_QOS_MISC_UC_AT3_CTRL_REG                ((QOS_MISC_BASE_ADDR) + 0x7C)
#define PP_QOS_MISC_UC_AT3_CTRL_NFLUVAL_OFF                         (24)
#define PP_QOS_MISC_UC_AT3_CTRL_NFLUVAL_LEN                         (8)
#define PP_QOS_MISC_UC_AT3_CTRL_NFLUVAL_MSK                         (0xFF000000)
#define PP_QOS_MISC_UC_AT3_CTRL_NFLUVAL_RST                         (0xf)
#define PP_QOS_MISC_UC_AT3_CTRL_NELUVAL_OFF                         (16)
#define PP_QOS_MISC_UC_AT3_CTRL_NELUVAL_LEN                         (8)
#define PP_QOS_MISC_UC_AT3_CTRL_NELUVAL_MSK                         (0x00FF0000)
#define PP_QOS_MISC_UC_AT3_CTRL_NELUVAL_RST                         (0xe)
#define PP_QOS_MISC_UC_AT3_CTRL_NDLUVAL_OFF                         (8)
#define PP_QOS_MISC_UC_AT3_CTRL_NDLUVAL_LEN                         (8)
#define PP_QOS_MISC_UC_AT3_CTRL_NDLUVAL_MSK                         (0x0000FF00)
#define PP_QOS_MISC_UC_AT3_CTRL_NDLUVAL_RST                         (0xd)
#define PP_QOS_MISC_UC_AT3_CTRL_NCLUVAL_OFF                         (0)
#define PP_QOS_MISC_UC_AT3_CTRL_NCLUVAL_LEN                         (8)
#define PP_QOS_MISC_UC_AT3_CTRL_NCLUVAL_MSK                         (0x000000FF)
#define PP_QOS_MISC_UC_AT3_CTRL_NCLUVAL_RST                         (0xc)

#endif
