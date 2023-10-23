/**
 * ingress_ila_regs.h
 * Description: ingress_ila_s HW registers definitions
 * Auto-generated file by RDL Parser, DO NOT CHANGE
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2019-2020 Intel Corporation
 */

#ifndef _PP_INGR_ILA_H_
#define _PP_INGR_ILA_H_

#define PP_INGR_ILA_GEN_DATE_STR              "Sunday May 17, 2020 [5:26:11 pm]"
#define PP_INGR_ILA_BASE                                         (0xF1003000ULL)

/**
 * SW_REG_NAME : PP_INGR_ILA_SRAM_SELECT_REG
 * HW_REG_NAME : sram_select
 * DESCRIPTION : SRAM input Select
 *
 *  Register Fields :
 *   [ 7: 7][RW] - SRAM Arbiter HOST/Internal
 *                 Logic.[br]Host=0[br]Logic=1
 *   [ 6: 0][RW] - ILA Logic Select. Selects Which ILA_LOGIC Instance
 *                 is Main (Recorder)
 *
 */
#define PP_INGR_ILA_SRAM_SELECT_REG                 ((INGR_ILA_BASE_ADDR) + 0x0)
#define PP_INGR_ILA_SRAM_SELECT_CLIENT_EN_OFF                       (7)
#define PP_INGR_ILA_SRAM_SELECT_CLIENT_EN_LEN                       (1)
#define PP_INGR_ILA_SRAM_SELECT_CLIENT_EN_MSK                       (0x00000080)
#define PP_INGR_ILA_SRAM_SELECT_CLIENT_EN_RST                       (0x0)
#define PP_INGR_ILA_SRAM_SELECT_ILA_LOGIC_SELECT_OFF                (0)
#define PP_INGR_ILA_SRAM_SELECT_ILA_LOGIC_SELECT_LEN                (7)
#define PP_INGR_ILA_SRAM_SELECT_ILA_LOGIC_SELECT_MSK                (0x0000007F)
#define PP_INGR_ILA_SRAM_SELECT_ILA_LOGIC_SELECT_RST                (0x0)

/**
 * SW_REG_NAME : PP_INGR_ILA_GEN_CTRL_REG
 * HW_REG_NAME : gen_ctrl
 * DESCRIPTION : General Control
 *
 *  Register Fields :
 *   [31:16][RW] - Client MUX Enable (16 clients)
 *   [15:14][RO] - Reserved
 *   [13:12][RW] - Client Clock Select
 *   [11:10][RO] - Reserved
 *   [ 9: 8][RW] - 0 - 1Kx36b [br]1 - 2Kx18b [br]2 - 4Kx9b
 *   [ 7: 4][RW] - Abort Recording
 *   [ 3: 0][RW] - Arming ILA to start catupe
 *
 */
#define PP_INGR_ILA_GEN_CTRL_REG                    ((INGR_ILA_BASE_ADDR) + 0x4)
#define PP_INGR_ILA_GEN_CTRL_CLNT_MUX_EN_OFF                        (16)
#define PP_INGR_ILA_GEN_CTRL_CLNT_MUX_EN_LEN                        (16)
#define PP_INGR_ILA_GEN_CTRL_CLNT_MUX_EN_MSK                        (0xFFFF0000)
#define PP_INGR_ILA_GEN_CTRL_CLNT_MUX_EN_RST                        (0x0)
#define PP_INGR_ILA_GEN_CTRL_RSVD0_OFF                              (14)
#define PP_INGR_ILA_GEN_CTRL_RSVD0_LEN                              (2)
#define PP_INGR_ILA_GEN_CTRL_RSVD0_MSK                              (0x0000C000)
#define PP_INGR_ILA_GEN_CTRL_RSVD0_RST                              (0x0)
#define PP_INGR_ILA_GEN_CTRL_CLK_SEL_OFF                            (12)
#define PP_INGR_ILA_GEN_CTRL_CLK_SEL_LEN                            (2)
#define PP_INGR_ILA_GEN_CTRL_CLK_SEL_MSK                            (0x00003000)
#define PP_INGR_ILA_GEN_CTRL_CLK_SEL_RST                            (0x0)
#define PP_INGR_ILA_GEN_CTRL_RSVD1_OFF                              (10)
#define PP_INGR_ILA_GEN_CTRL_RSVD1_LEN                              (2)
#define PP_INGR_ILA_GEN_CTRL_RSVD1_MSK                              (0x00000C00)
#define PP_INGR_ILA_GEN_CTRL_RSVD1_RST                              (0x0)
#define PP_INGR_ILA_GEN_CTRL_PORT_MODE_OFF                          (8)
#define PP_INGR_ILA_GEN_CTRL_PORT_MODE_LEN                          (2)
#define PP_INGR_ILA_GEN_CTRL_PORT_MODE_MSK                          (0x00000300)
#define PP_INGR_ILA_GEN_CTRL_PORT_MODE_RST                          (0x0)
#define PP_INGR_ILA_GEN_CTRL_ILA_ABORT_OFF                          (4)
#define PP_INGR_ILA_GEN_CTRL_ILA_ABORT_LEN                          (4)
#define PP_INGR_ILA_GEN_CTRL_ILA_ABORT_MSK                          (0x000000F0)
#define PP_INGR_ILA_GEN_CTRL_ILA_ABORT_RST                          (0x0)
#define PP_INGR_ILA_GEN_CTRL_ILA_ARM_OFF                            (0)
#define PP_INGR_ILA_GEN_CTRL_ILA_ARM_LEN                            (4)
#define PP_INGR_ILA_GEN_CTRL_ILA_ARM_MSK                            (0x0000000F)
#define PP_INGR_ILA_GEN_CTRL_ILA_ARM_RST                            (0x0)

/**
 * SW_REG_NAME : PP_INGR_ILA_CLIENT_MUX_CTRL_REG
 * HW_REG_NAME : client_mux_ctrl
 * DESCRIPTION : Timer Value
 *
 *  Register Fields :
 *   [31:24][RW] - Port[35:27] Select
 *   [23:16][RW] - Port[26:18] Select
 *   [15: 8][RW] - Port[17:9]  Select
 *   [ 7: 0][RW] - Port[8:0]   Select
 *
 */
#define PP_INGR_ILA_CLIENT_MUX_CTRL_REG             ((INGR_ILA_BASE_ADDR) + 0x8)
#define PP_INGR_ILA_CLIENT_MUX_CTRL_SEL3_OFF                        (24)
#define PP_INGR_ILA_CLIENT_MUX_CTRL_SEL3_LEN                        (8)
#define PP_INGR_ILA_CLIENT_MUX_CTRL_SEL3_MSK                        (0xFF000000)
#define PP_INGR_ILA_CLIENT_MUX_CTRL_SEL3_RST                        (0x0)
#define PP_INGR_ILA_CLIENT_MUX_CTRL_SEL2_OFF                        (16)
#define PP_INGR_ILA_CLIENT_MUX_CTRL_SEL2_LEN                        (8)
#define PP_INGR_ILA_CLIENT_MUX_CTRL_SEL2_MSK                        (0x00FF0000)
#define PP_INGR_ILA_CLIENT_MUX_CTRL_SEL2_RST                        (0x0)
#define PP_INGR_ILA_CLIENT_MUX_CTRL_SEL1_OFF                        (8)
#define PP_INGR_ILA_CLIENT_MUX_CTRL_SEL1_LEN                        (8)
#define PP_INGR_ILA_CLIENT_MUX_CTRL_SEL1_MSK                        (0x0000FF00)
#define PP_INGR_ILA_CLIENT_MUX_CTRL_SEL1_RST                        (0x0)
#define PP_INGR_ILA_CLIENT_MUX_CTRL_SEL0_OFF                        (0)
#define PP_INGR_ILA_CLIENT_MUX_CTRL_SEL0_LEN                        (8)
#define PP_INGR_ILA_CLIENT_MUX_CTRL_SEL0_MSK                        (0x000000FF)
#define PP_INGR_ILA_CLIENT_MUX_CTRL_SEL0_RST                        (0x0)
/**
 * REG_IDX_ACCESS   : PP_INGR_ILA_CLIENT_MUX_CTRL_REG_IDX
 * NUM OF REGISTERS : 11
 */
#define PP_INGR_ILA_CLIENT_MUX_CTRL_REG_IDX(idx) \
	(PP_INGR_ILA_CLIENT_MUX_CTRL_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_INGR_ILA_CTRL0_REG
 * HW_REG_NAME : ila_ctrl0
 * DESCRIPTION : ILA LOGIC Control
 *
 *  Register Fields :
 *   [31:16][RW] - Pre record window Value in Samples.
 *                 [br]This value will record samples before the
 *                 first
 *                 [br]trigger state that is begining to record
 *   [15: 8][RO] - Reserved
 *   [ 7: 4][RW] - Timer Enable Bit per state. Only 1 state can use
 *                 Timer
 *   [ 3: 0][RW] - Client MUX Select. Selecting 1 of Input ILA Ports
 *
 */
#define PP_INGR_ILA_CTRL0_REG                     ((INGR_ILA_BASE_ADDR) + 0x100)
#define PP_INGR_ILA_CTRL0_WINDOW_VAL_OFF                            (16)
#define PP_INGR_ILA_CTRL0_WINDOW_VAL_LEN                            (16)
#define PP_INGR_ILA_CTRL0_WINDOW_VAL_MSK                            (0xFFFF0000)
#define PP_INGR_ILA_CTRL0_WINDOW_VAL_RST                            (0x0)
#define PP_INGR_ILA_CTRL0_RSVD0_OFF                                 (8)
#define PP_INGR_ILA_CTRL0_RSVD0_LEN                                 (8)
#define PP_INGR_ILA_CTRL0_RSVD0_MSK                                 (0x0000FF00)
#define PP_INGR_ILA_CTRL0_RSVD0_RST                                 (0x0)
#define PP_INGR_ILA_CTRL0_TIMER_EN_OFF                              (4)
#define PP_INGR_ILA_CTRL0_TIMER_EN_LEN                              (4)
#define PP_INGR_ILA_CTRL0_TIMER_EN_MSK                              (0x000000F0)
#define PP_INGR_ILA_CTRL0_TIMER_EN_RST                              (0x0)
#define PP_INGR_ILA_CTRL0_CLNT_SEL_OFF                              (0)
#define PP_INGR_ILA_CTRL0_CLNT_SEL_LEN                              (4)
#define PP_INGR_ILA_CTRL0_CLNT_SEL_MSK                              (0x0000000F)
#define PP_INGR_ILA_CTRL0_CLNT_SEL_RST                              (0x0)
/**
 * REG_IDX_ACCESS   : PP_INGR_ILA_CTRL0_REG_IDX
 * NUM OF REGISTERS : 2
 */
#define PP_INGR_ILA_CTRL0_REG_IDX(idx) \
	(PP_INGR_ILA_CTRL0_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_INGR_ILA_CTRL1_REG
 * HW_REG_NAME : ila_ctrl1
 * DESCRIPTION : Timer Value
 *
 *  Register Fields :
 *   [31: 0][RW] - Timer Set Value
 *
 */
#define PP_INGR_ILA_CTRL1_REG                     ((INGR_ILA_BASE_ADDR) + 0x108)
#define PP_INGR_ILA_CTRL1_TIMER_VAL_OFF                             (0)
#define PP_INGR_ILA_CTRL1_TIMER_VAL_LEN                             (32)
#define PP_INGR_ILA_CTRL1_TIMER_VAL_MSK                             (0xFFFFFFFF)
#define PP_INGR_ILA_CTRL1_TIMER_VAL_RST                             (0x0)
/**
 * REG_IDX_ACCESS   : PP_INGR_ILA_CTRL1_REG_IDX
 * NUM OF REGISTERS : 2
 */
#define PP_INGR_ILA_CTRL1_REG_IDX(idx) \
	(PP_INGR_ILA_CTRL1_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_INGR_ILA_MASKS_L_ILA0_REG
 * HW_REG_NAME : ila_masks_l_ila0
 * DESCRIPTION : Masks Low For ILA0
 *
 *  Register Fields :
 *   [31:18][RO] - Reserved
 *   [17: 9][RW] - Mask1 bits[17:09]
 *   [ 8: 0][RW] - Mask0 bits[08:00]. Mask[3..0] beloew should create
 *                 up to a 32bit sub Vector for OP
 *
 */
#define PP_INGR_ILA_MASKS_L_ILA0_REG              ((INGR_ILA_BASE_ADDR) + 0x110)
#define PP_INGR_ILA_MASKS_L_ILA0_RSVD0_OFF                          (18)
#define PP_INGR_ILA_MASKS_L_ILA0_RSVD0_LEN                          (14)
#define PP_INGR_ILA_MASKS_L_ILA0_RSVD0_MSK                          (0xFFFC0000)
#define PP_INGR_ILA_MASKS_L_ILA0_RSVD0_RST                          (0x0)
#define PP_INGR_ILA_MASKS_L_ILA0_MASK1_OFF                          (9)
#define PP_INGR_ILA_MASKS_L_ILA0_MASK1_LEN                          (9)
#define PP_INGR_ILA_MASKS_L_ILA0_MASK1_MSK                          (0x0003FE00)
#define PP_INGR_ILA_MASKS_L_ILA0_MASK1_RST                          (0x0)
#define PP_INGR_ILA_MASKS_L_ILA0_MASK0_OFF                          (0)
#define PP_INGR_ILA_MASKS_L_ILA0_MASK0_LEN                          (9)
#define PP_INGR_ILA_MASKS_L_ILA0_MASK0_MSK                          (0x000001FF)
#define PP_INGR_ILA_MASKS_L_ILA0_MASK0_RST                          (0x0)
/**
 * REG_IDX_ACCESS   : PP_INGR_ILA_MASKS_L_ILA0_REG_IDX
 * NUM OF REGISTERS : 4
 */
#define PP_INGR_ILA_MASKS_L_ILA0_REG_IDX(idx) \
	(PP_INGR_ILA_MASKS_L_ILA0_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_INGR_ILA_MASKS_H_ILA0_REG
 * HW_REG_NAME : ila_masks_h_ila0
 * DESCRIPTION : Masks High For ILA0
 *
 *  Register Fields :
 *   [31:18][RO] - Reserved
 *   [17: 9][RW] - Mask3 bits[35:27]
 *   [ 8: 0][RW] - Mask2 bits[26:18]
 *
 */
#define PP_INGR_ILA_MASKS_H_ILA0_REG              ((INGR_ILA_BASE_ADDR) + 0x120)
#define PP_INGR_ILA_MASKS_H_ILA0_RSVD0_OFF                          (18)
#define PP_INGR_ILA_MASKS_H_ILA0_RSVD0_LEN                          (14)
#define PP_INGR_ILA_MASKS_H_ILA0_RSVD0_MSK                          (0xFFFC0000)
#define PP_INGR_ILA_MASKS_H_ILA0_RSVD0_RST                          (0x0)
#define PP_INGR_ILA_MASKS_H_ILA0_MASK3_OFF                          (9)
#define PP_INGR_ILA_MASKS_H_ILA0_MASK3_LEN                          (9)
#define PP_INGR_ILA_MASKS_H_ILA0_MASK3_MSK                          (0x0003FE00)
#define PP_INGR_ILA_MASKS_H_ILA0_MASK3_RST                          (0x0)
#define PP_INGR_ILA_MASKS_H_ILA0_MASK2_OFF                          (0)
#define PP_INGR_ILA_MASKS_H_ILA0_MASK2_LEN                          (9)
#define PP_INGR_ILA_MASKS_H_ILA0_MASK2_MSK                          (0x000001FF)
#define PP_INGR_ILA_MASKS_H_ILA0_MASK2_RST                          (0x0)
/**
 * REG_IDX_ACCESS   : PP_INGR_ILA_MASKS_H_ILA0_REG_IDX
 * NUM OF REGISTERS : 4
 */
#define PP_INGR_ILA_MASKS_H_ILA0_REG_IDX(idx) \
	(PP_INGR_ILA_MASKS_H_ILA0_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_INGR_ILA_MASKS_L_ILA1_REG
 * HW_REG_NAME : ila_masks_l_ila1
 * DESCRIPTION : Masks Low For ILA1
 *
 *  Register Fields :
 *   [31:18][RO] - Reserved
 *   [17: 9][RW] - Mask1 bits[17:09]
 *   [ 8: 0][RW] - Mask0 bits[08:00]. Mask[3..0] beloew should create
 *                 up to a 32bit sub Vector for OP
 *
 */
#define PP_INGR_ILA_MASKS_L_ILA1_REG              ((INGR_ILA_BASE_ADDR) + 0x130)
#define PP_INGR_ILA_MASKS_L_ILA1_RSVD0_OFF                          (18)
#define PP_INGR_ILA_MASKS_L_ILA1_RSVD0_LEN                          (14)
#define PP_INGR_ILA_MASKS_L_ILA1_RSVD0_MSK                          (0xFFFC0000)
#define PP_INGR_ILA_MASKS_L_ILA1_RSVD0_RST                          (0x0)
#define PP_INGR_ILA_MASKS_L_ILA1_MASK1_OFF                          (9)
#define PP_INGR_ILA_MASKS_L_ILA1_MASK1_LEN                          (9)
#define PP_INGR_ILA_MASKS_L_ILA1_MASK1_MSK                          (0x0003FE00)
#define PP_INGR_ILA_MASKS_L_ILA1_MASK1_RST                          (0x0)
#define PP_INGR_ILA_MASKS_L_ILA1_MASK0_OFF                          (0)
#define PP_INGR_ILA_MASKS_L_ILA1_MASK0_LEN                          (9)
#define PP_INGR_ILA_MASKS_L_ILA1_MASK0_MSK                          (0x000001FF)
#define PP_INGR_ILA_MASKS_L_ILA1_MASK0_RST                          (0x0)
/**
 * REG_IDX_ACCESS   : PP_INGR_ILA_MASKS_L_ILA1_REG_IDX
 * NUM OF REGISTERS : 4
 */
#define PP_INGR_ILA_MASKS_L_ILA1_REG_IDX(idx) \
	(PP_INGR_ILA_MASKS_L_ILA1_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_INGR_ILA_MASKS_H_ILA1_REG
 * HW_REG_NAME : ila_masks_h_ila1
 * DESCRIPTION : Masks High For ILA1
 *
 *  Register Fields :
 *   [31:18][RO] - Reserved
 *   [17: 9][RW] - Mask3 bits[35:27]
 *   [ 8: 0][RW] - Mask2 bits[26:18]
 *
 */
#define PP_INGR_ILA_MASKS_H_ILA1_REG              ((INGR_ILA_BASE_ADDR) + 0x140)
#define PP_INGR_ILA_MASKS_H_ILA1_RSVD0_OFF                          (18)
#define PP_INGR_ILA_MASKS_H_ILA1_RSVD0_LEN                          (14)
#define PP_INGR_ILA_MASKS_H_ILA1_RSVD0_MSK                          (0xFFFC0000)
#define PP_INGR_ILA_MASKS_H_ILA1_RSVD0_RST                          (0x0)
#define PP_INGR_ILA_MASKS_H_ILA1_MASK3_OFF                          (9)
#define PP_INGR_ILA_MASKS_H_ILA1_MASK3_LEN                          (9)
#define PP_INGR_ILA_MASKS_H_ILA1_MASK3_MSK                          (0x0003FE00)
#define PP_INGR_ILA_MASKS_H_ILA1_MASK3_RST                          (0x0)
#define PP_INGR_ILA_MASKS_H_ILA1_MASK2_OFF                          (0)
#define PP_INGR_ILA_MASKS_H_ILA1_MASK2_LEN                          (9)
#define PP_INGR_ILA_MASKS_H_ILA1_MASK2_MSK                          (0x000001FF)
#define PP_INGR_ILA_MASKS_H_ILA1_MASK2_RST                          (0x0)
/**
 * REG_IDX_ACCESS   : PP_INGR_ILA_MASKS_H_ILA1_REG_IDX
 * NUM OF REGISTERS : 4
 */
#define PP_INGR_ILA_MASKS_H_ILA1_REG_IDX(idx) \
	(PP_INGR_ILA_MASKS_H_ILA1_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_INGR_ILA_OPER_REG
 * HW_REG_NAME : ila_oper
 * DESCRIPTION : ILA Operation Select
 *
 *  Register Fields :
 *   [31:24][RW] - OP3 Select
 *   [23:16][RW] - OP2 Select
 *   [15: 8][RW] - OP1 Select
 *   [ 7: 0][RW] - OP0 Select. For Operation select see MAS
 *
 */
#define PP_INGR_ILA_OPER_REG                      ((INGR_ILA_BASE_ADDR) + 0x200)
#define PP_INGR_ILA_OPER_OPER_SEL3_OFF                              (24)
#define PP_INGR_ILA_OPER_OPER_SEL3_LEN                              (8)
#define PP_INGR_ILA_OPER_OPER_SEL3_MSK                              (0xFF000000)
#define PP_INGR_ILA_OPER_OPER_SEL3_RST                              (0x0)
#define PP_INGR_ILA_OPER_OPER_SEL2_OFF                              (16)
#define PP_INGR_ILA_OPER_OPER_SEL2_LEN                              (8)
#define PP_INGR_ILA_OPER_OPER_SEL2_MSK                              (0x00FF0000)
#define PP_INGR_ILA_OPER_OPER_SEL2_RST                              (0x0)
#define PP_INGR_ILA_OPER_OPER_SEL1_OFF                              (8)
#define PP_INGR_ILA_OPER_OPER_SEL1_LEN                              (8)
#define PP_INGR_ILA_OPER_OPER_SEL1_MSK                              (0x0000FF00)
#define PP_INGR_ILA_OPER_OPER_SEL1_RST                              (0x0)
#define PP_INGR_ILA_OPER_OPER_SEL0_OFF                              (0)
#define PP_INGR_ILA_OPER_OPER_SEL0_LEN                              (8)
#define PP_INGR_ILA_OPER_OPER_SEL0_MSK                              (0x000000FF)
#define PP_INGR_ILA_OPER_OPER_SEL0_RST                              (0x0)
/**
 * REG_IDX_ACCESS   : PP_INGR_ILA_OPER_REG_IDX
 * NUM OF REGISTERS : 2
 */
#define PP_INGR_ILA_OPER_REG_IDX(idx) \
	(PP_INGR_ILA_OPER_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_INGR_ILA_COND_REG
 * HW_REG_NAME : ila_cond
 * DESCRIPTION : ILA Condition Select
 *
 *  Register Fields :
 *   [31:24][RW] - Condition3 Select
 *   [23:16][RW] - Condition2 Select
 *   [15: 8][RW] - Condition1 Select
 *   [ 7: 0][RW] - Condition0 Select. For Condition select see MAS
 *
 */
#define PP_INGR_ILA_COND_REG                      ((INGR_ILA_BASE_ADDR) + 0x208)
#define PP_INGR_ILA_COND_COND_SEL3_OFF                              (24)
#define PP_INGR_ILA_COND_COND_SEL3_LEN                              (8)
#define PP_INGR_ILA_COND_COND_SEL3_MSK                              (0xFF000000)
#define PP_INGR_ILA_COND_COND_SEL3_RST                              (0x0)
#define PP_INGR_ILA_COND_COND_SEL2_OFF                              (16)
#define PP_INGR_ILA_COND_COND_SEL2_LEN                              (8)
#define PP_INGR_ILA_COND_COND_SEL2_MSK                              (0x00FF0000)
#define PP_INGR_ILA_COND_COND_SEL2_RST                              (0x0)
#define PP_INGR_ILA_COND_COND_SEL1_OFF                              (8)
#define PP_INGR_ILA_COND_COND_SEL1_LEN                              (8)
#define PP_INGR_ILA_COND_COND_SEL1_MSK                              (0x0000FF00)
#define PP_INGR_ILA_COND_COND_SEL1_RST                              (0x0)
#define PP_INGR_ILA_COND_COND_SEL0_OFF                              (0)
#define PP_INGR_ILA_COND_COND_SEL0_LEN                              (8)
#define PP_INGR_ILA_COND_COND_SEL0_MSK                              (0x000000FF)
#define PP_INGR_ILA_COND_COND_SEL0_RST                              (0x0)
/**
 * REG_IDX_ACCESS   : PP_INGR_ILA_COND_REG_IDX
 * NUM OF REGISTERS : 2
 */
#define PP_INGR_ILA_COND_REG_IDX(idx) \
	(PP_INGR_ILA_COND_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_INGR_ILA_TRIG_REG
 * HW_REG_NAME : ila_trig
 * DESCRIPTION : ILA Trigger Select
 *
 *  Register Fields :
 *   [31:24][RW] - Trigger3 Select
 *   [23:16][RW] - Trigger2 Select
 *   [15: 8][RW] - Trigger1 Select
 *   [ 7: 0][RW] - Trigger0 Select. For Trigger select see MAS.
 *
 */
#define PP_INGR_ILA_TRIG_REG                      ((INGR_ILA_BASE_ADDR) + 0x210)
#define PP_INGR_ILA_TRIG_TRIG_SEL3_OFF                              (24)
#define PP_INGR_ILA_TRIG_TRIG_SEL3_LEN                              (8)
#define PP_INGR_ILA_TRIG_TRIG_SEL3_MSK                              (0xFF000000)
#define PP_INGR_ILA_TRIG_TRIG_SEL3_RST                              (0x0)
#define PP_INGR_ILA_TRIG_TRIG_SEL2_OFF                              (16)
#define PP_INGR_ILA_TRIG_TRIG_SEL2_LEN                              (8)
#define PP_INGR_ILA_TRIG_TRIG_SEL2_MSK                              (0x00FF0000)
#define PP_INGR_ILA_TRIG_TRIG_SEL2_RST                              (0x0)
#define PP_INGR_ILA_TRIG_TRIG_SEL1_OFF                              (8)
#define PP_INGR_ILA_TRIG_TRIG_SEL1_LEN                              (8)
#define PP_INGR_ILA_TRIG_TRIG_SEL1_MSK                              (0x0000FF00)
#define PP_INGR_ILA_TRIG_TRIG_SEL1_RST                              (0x0)
#define PP_INGR_ILA_TRIG_TRIG_SEL0_OFF                              (0)
#define PP_INGR_ILA_TRIG_TRIG_SEL0_LEN                              (8)
#define PP_INGR_ILA_TRIG_TRIG_SEL0_MSK                              (0x000000FF)
#define PP_INGR_ILA_TRIG_TRIG_SEL0_RST                              (0x0)
/**
 * REG_IDX_ACCESS   : PP_INGR_ILA_TRIG_REG_IDX
 * NUM OF REGISTERS : 2
 */
#define PP_INGR_ILA_TRIG_REG_IDX(idx) \
	(PP_INGR_ILA_TRIG_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_INGR_ILA_WREN_REG
 * HW_REG_NAME : ila_wren
 * DESCRIPTION : ILA Write Mem Select
 *
 *  Register Fields :
 *   [31:24][RW] - MemoryWrite3 Select
 *   [23:16][RW] - MemoryWrite2 Select
 *   [15: 8][RW] - MemoryWrite1 Select
 *   [ 7: 0][RW] - MemoryWrite0 Select. (See MAS)
 *
 */
#define PP_INGR_ILA_WREN_REG                      ((INGR_ILA_BASE_ADDR) + 0x218)
#define PP_INGR_ILA_WREN_WREN_SEL3_OFF                              (24)
#define PP_INGR_ILA_WREN_WREN_SEL3_LEN                              (8)
#define PP_INGR_ILA_WREN_WREN_SEL3_MSK                              (0xFF000000)
#define PP_INGR_ILA_WREN_WREN_SEL3_RST                              (0x0)
#define PP_INGR_ILA_WREN_WREN_SEL2_OFF                              (16)
#define PP_INGR_ILA_WREN_WREN_SEL2_LEN                              (8)
#define PP_INGR_ILA_WREN_WREN_SEL2_MSK                              (0x00FF0000)
#define PP_INGR_ILA_WREN_WREN_SEL2_RST                              (0x0)
#define PP_INGR_ILA_WREN_WREN_SEL1_OFF                              (8)
#define PP_INGR_ILA_WREN_WREN_SEL1_LEN                              (8)
#define PP_INGR_ILA_WREN_WREN_SEL1_MSK                              (0x0000FF00)
#define PP_INGR_ILA_WREN_WREN_SEL1_RST                              (0x0)
#define PP_INGR_ILA_WREN_WREN_SEL0_OFF                              (0)
#define PP_INGR_ILA_WREN_WREN_SEL0_LEN                              (8)
#define PP_INGR_ILA_WREN_WREN_SEL0_MSK                              (0x000000FF)
#define PP_INGR_ILA_WREN_WREN_SEL0_RST                              (0x0)
/**
 * REG_IDX_ACCESS   : PP_INGR_ILA_WREN_REG_IDX
 * NUM OF REGISTERS : 2
 */
#define PP_INGR_ILA_WREN_REG_IDX(idx) \
	(PP_INGR_ILA_WREN_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_INGR_ILA_OP_VAL0_REG
 * HW_REG_NAME : ila_op_val0
 * DESCRIPTION : Operation Value
 *
 *  Register Fields :
 *   [31: 0][RW] - Operand Value. (See OP Select)
 *
 */
#define PP_INGR_ILA_OP_VAL0_REG                   ((INGR_ILA_BASE_ADDR) + 0x220)
#define PP_INGR_ILA_OP_VAL0_OP_VAL_OFF                              (0)
#define PP_INGR_ILA_OP_VAL0_OP_VAL_LEN                              (32)
#define PP_INGR_ILA_OP_VAL0_OP_VAL_MSK                              (0xFFFFFFFF)
#define PP_INGR_ILA_OP_VAL0_OP_VAL_RST                              (0x0)
/**
 * REG_IDX_ACCESS   : PP_INGR_ILA_OP_VAL0_REG_IDX
 * NUM OF REGISTERS : 2
 */
#define PP_INGR_ILA_OP_VAL0_REG_IDX(idx) \
	(PP_INGR_ILA_OP_VAL0_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_INGR_ILA_OP_VAL1_REG
 * HW_REG_NAME : ila_op_val1
 * DESCRIPTION : Operation Value
 *
 *  Register Fields :
 *   [31: 0][RW] - Operand Value. (See OP Select)
 *
 */
#define PP_INGR_ILA_OP_VAL1_REG                   ((INGR_ILA_BASE_ADDR) + 0x228)
#define PP_INGR_ILA_OP_VAL1_OP_VAL_OFF                              (0)
#define PP_INGR_ILA_OP_VAL1_OP_VAL_LEN                              (32)
#define PP_INGR_ILA_OP_VAL1_OP_VAL_MSK                              (0xFFFFFFFF)
#define PP_INGR_ILA_OP_VAL1_OP_VAL_RST                              (0x0)
/**
 * REG_IDX_ACCESS   : PP_INGR_ILA_OP_VAL1_REG_IDX
 * NUM OF REGISTERS : 2
 */
#define PP_INGR_ILA_OP_VAL1_REG_IDX(idx) \
	(PP_INGR_ILA_OP_VAL1_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_INGR_ILA_OP_VAL2_REG
 * HW_REG_NAME : ila_op_val2
 * DESCRIPTION : Operation Value
 *
 *  Register Fields :
 *   [31: 0][RW] - Operand Value. (See OP Select)
 *
 */
#define PP_INGR_ILA_OP_VAL2_REG                   ((INGR_ILA_BASE_ADDR) + 0x230)
#define PP_INGR_ILA_OP_VAL2_OP_VAL_OFF                              (0)
#define PP_INGR_ILA_OP_VAL2_OP_VAL_LEN                              (32)
#define PP_INGR_ILA_OP_VAL2_OP_VAL_MSK                              (0xFFFFFFFF)
#define PP_INGR_ILA_OP_VAL2_OP_VAL_RST                              (0x0)
/**
 * REG_IDX_ACCESS   : PP_INGR_ILA_OP_VAL2_REG_IDX
 * NUM OF REGISTERS : 2
 */
#define PP_INGR_ILA_OP_VAL2_REG_IDX(idx) \
	(PP_INGR_ILA_OP_VAL2_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_INGR_ILA_OP_VAL3_REG
 * HW_REG_NAME : ila_op_val3
 * DESCRIPTION : Operation Value
 *
 *  Register Fields :
 *   [31: 0][RW] - Operand Value. (See OP Select)
 *
 */
#define PP_INGR_ILA_OP_VAL3_REG                   ((INGR_ILA_BASE_ADDR) + 0x238)
#define PP_INGR_ILA_OP_VAL3_OP_VAL_OFF                              (0)
#define PP_INGR_ILA_OP_VAL3_OP_VAL_LEN                              (32)
#define PP_INGR_ILA_OP_VAL3_OP_VAL_MSK                              (0xFFFFFFFF)
#define PP_INGR_ILA_OP_VAL3_OP_VAL_RST                              (0x0)
/**
 * REG_IDX_ACCESS   : PP_INGR_ILA_OP_VAL3_REG_IDX
 * NUM OF REGISTERS : 2
 */
#define PP_INGR_ILA_OP_VAL3_REG_IDX(idx) \
	(PP_INGR_ILA_OP_VAL3_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_INGR_ILA_STATUS_REG
 * HW_REG_NAME : status
 * DESCRIPTION : Timer Value
 *
 *  Register Fields :
 *   [31:31][RW] - ILA Done (Write Clear)
 *   [30:28][RW] - ILA Last active state), NOTE, B Step ONLY field
 *   [27:16][RW] - Read Value. The Address of the Last sample (Write
 *                 Clear), NOTE, B Step ONLY field
 *   [15:12][RO] - Reserved
 *   [11: 0][RW] - Read Value. The Address of the First sample (Write
 *                 Clear)
 *
 */
#define PP_INGR_ILA_STATUS_REG                    ((INGR_ILA_BASE_ADDR) + 0x300)
#define PP_INGR_ILA_STATUS_ILA_DONE_OFF                             (31)
#define PP_INGR_ILA_STATUS_ILA_DONE_LEN                             (1)
#define PP_INGR_ILA_STATUS_ILA_DONE_MSK                             (0x80000000)
#define PP_INGR_ILA_STATUS_ILA_DONE_RST                             (0x0)
#define PP_INGR_ILA_STATUS_ILA_STATE_OFF                            (28)
#define PP_INGR_ILA_STATUS_ILA_STATE_LEN                            (3)
#define PP_INGR_ILA_STATUS_ILA_STATE_MSK                            (0x70000000)
#define PP_INGR_ILA_STATUS_ILA_STATE_RST                            (0x0)
#define PP_INGR_ILA_STATUS_REC_ADDR_LAST_OFF                        (16)
#define PP_INGR_ILA_STATUS_REC_ADDR_LAST_LEN                        (12)
#define PP_INGR_ILA_STATUS_REC_ADDR_LAST_MSK                        (0x0FFF0000)
#define PP_INGR_ILA_STATUS_REC_ADDR_LAST_RST                        (0x0)
#define PP_INGR_ILA_STATUS_RSVD0_OFF                                (12)
#define PP_INGR_ILA_STATUS_RSVD0_LEN                                (4)
#define PP_INGR_ILA_STATUS_RSVD0_MSK                                (0x0000F000)
#define PP_INGR_ILA_STATUS_RSVD0_RST                                (0x0)
#define PP_INGR_ILA_STATUS_REC_ADDR_1ST_OFF                         (0)
#define PP_INGR_ILA_STATUS_REC_ADDR_1ST_LEN                         (12)
#define PP_INGR_ILA_STATUS_REC_ADDR_1ST_MSK                         (0x00000FFF)
#define PP_INGR_ILA_STATUS_REC_ADDR_1ST_RST                         (0x0)
/**
 * REG_IDX_ACCESS   : PP_INGR_ILA_STATUS_REG_IDX
 * NUM OF REGISTERS : 2
 */
#define PP_INGR_ILA_STATUS_REG_IDX(idx) \
	(PP_INGR_ILA_STATUS_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_INGR_ILA_WINDOW_PRE_RECORDING_REG
 * HW_REG_NAME : ila_window_pre_recording
 * DESCRIPTION : Window Pre-Recording Size, NOTE, B Step ONLY
 *               register
 *
 *  Register Fields :
 *   [31:12][RO] - Reserved
 *   [11: 0][RO] - Amount of samples in the ram recorded before the
 *                 trigger from State0 to State1. For Window usage
 *                 Only
 *
 */
#define PP_INGR_ILA_WINDOW_PRE_RECORDING_REG      ((INGR_ILA_BASE_ADDR) + 0x400)
#define PP_INGR_ILA_WINDOW_PRE_RECORDING_RSVD0_OFF                  (12)
#define PP_INGR_ILA_WINDOW_PRE_RECORDING_RSVD0_LEN                  (20)
#define PP_INGR_ILA_WINDOW_PRE_RECORDING_RSVD0_MSK                  (0xFFFFF000)
#define PP_INGR_ILA_WINDOW_PRE_RECORDING_RSVD0_RST                  (0x0)
#define PP_INGR_ILA_WINDOW_PRE_RECORDING_WIN_PRE_REC_OFF            (0)
#define PP_INGR_ILA_WINDOW_PRE_RECORDING_WIN_PRE_REC_LEN            (12)
#define PP_INGR_ILA_WINDOW_PRE_RECORDING_WIN_PRE_REC_MSK            (0x00000FFF)
#define PP_INGR_ILA_WINDOW_PRE_RECORDING_WIN_PRE_REC_RST            (0x0)
/**
 * REG_IDX_ACCESS   : PP_INGR_ILA_WINDOW_PRE_RECORDING_REG_IDX
 * NUM OF REGISTERS : 2
 */
#define PP_INGR_ILA_WINDOW_PRE_RECORDING_REG_IDX(idx) \
	(PP_INGR_ILA_WINDOW_PRE_RECORDING_REG + ((idx) << 2))

#endif
