/**
 * checker_regs.h
 * Description: checker_s HW registers definitions
 * Auto-generated file by RDL Parser, DO NOT CHANGE
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2019-2020 Intel Corporation
 */

#ifndef _PP_CHK_H_
#define _PP_CHK_H_

#define PP_CHK_GEN_DATE_STR                   "Sunday May 17, 2020 [5:26:11 pm]"
#define PP_CHK_BASE                                              (0xF1800000ULL)

/**
 * SW_REG_NAME : PP_CHK_TBM_TIMER_REG
 * HW_REG_NAME : tbm_timer
 * DESCRIPTION : TBM Timer CFG Register.             [br]The TBM
 *               Timer is the local time counter for TBM
 *               functionallity. Transmission rate shaping is based
 *               on credits accumulated as time passes.
 *               [br]The timer increments are +1 each clock cycle
 *               with a 32b counter.[br]For more information see
 *               TBM specificaion Doc.
 *
 *  Register Fields :
 *   [31:31][RW] - Timer Enable Bit  - Default is On
 *   [30:30][RW] - CRWLR Enable Bit  - Default is On
 *                 [br]The crawler is an internal mechanism
 *                 responsible for refreshing the TBM entry credits.
 *                 [br]This mechanism replaces the EPOC function
 *                 described in the TBM spec to prevent timer wrap
 *                 around.
 *   [29:11][RO] - Reserved
 *   [10: 8][RW] - Crawler Bit. Crawler triggered when this bit in
 *                 the TBM Timer toggles
 *   [ 7: 3][RO] - Reserved
 *   [ 2: 0][RW] - Timer MSB. The Local time is 24bit resolution and
 *                 this bit determins the time scaling. See spec.
 *
 */
#define PP_CHK_TBM_TIMER_REG                             ((CHK_BASE_ADDR) + 0x0)
#define PP_CHK_TBM_TIMER_TIMER_EN_OFF                               (31)
#define PP_CHK_TBM_TIMER_TIMER_EN_LEN                               (1)
#define PP_CHK_TBM_TIMER_TIMER_EN_MSK                               (0x80000000)
#define PP_CHK_TBM_TIMER_TIMER_EN_RST                               (0x0)
#define PP_CHK_TBM_TIMER_CRLWR_EN_OFF                               (30)
#define PP_CHK_TBM_TIMER_CRLWR_EN_LEN                               (1)
#define PP_CHK_TBM_TIMER_CRLWR_EN_MSK                               (0x40000000)
#define PP_CHK_TBM_TIMER_CRLWR_EN_RST                               (0x0)
#define PP_CHK_TBM_TIMER_RSVD0_OFF                                  (11)
#define PP_CHK_TBM_TIMER_RSVD0_LEN                                  (19)
#define PP_CHK_TBM_TIMER_RSVD0_MSK                                  (0x3FFFF800)
#define PP_CHK_TBM_TIMER_RSVD0_RST                                  (0x0)
#define PP_CHK_TBM_TIMER_CRLWR_MSB_OFF                              (8)
#define PP_CHK_TBM_TIMER_CRLWR_MSB_LEN                              (3)
#define PP_CHK_TBM_TIMER_CRLWR_MSB_MSK                              (0x00000700)
#define PP_CHK_TBM_TIMER_CRLWR_MSB_RST                              (0x0)
#define PP_CHK_TBM_TIMER_RSVD1_OFF                                  (3)
#define PP_CHK_TBM_TIMER_RSVD1_LEN                                  (5)
#define PP_CHK_TBM_TIMER_RSVD1_MSK                                  (0x000000F8)
#define PP_CHK_TBM_TIMER_RSVD1_RST                                  (0x0)
#define PP_CHK_TBM_TIMER_TIMER_MSB_OFF                              (0)
#define PP_CHK_TBM_TIMER_TIMER_MSB_LEN                              (3)
#define PP_CHK_TBM_TIMER_TIMER_MSB_MSK                              (0x00000007)
#define PP_CHK_TBM_TIMER_TIMER_MSB_RST                              (0x0)

/**
 * SW_REG_NAME : PP_CHK_STAGE2_MATCH_FALLBACK_REG
 * HW_REG_NAME : stage2_match_fallback
 * DESCRIPTION : Match Fallback Register. See Checker Developer's
 *               Guide for functionallity.
 *
 *  Register Fields :
 *   [31:14][RO] - Reserved
 *   [13:13][RW] - '1' - Disable STW Error becoming exception ALT SI
 *                 #63
 *   [12: 0][RW] - Fallback Register:
 *                 [br]bit[0] - Mask send to uC flag
 *                 [br]bit[1] - Mask Drop Session flag
 *                 [br]bit[2] - Mask Drop Dynamic flag
 *                 [br]bit[3] - Mask IPvX OPT flag
 *                 [br]bit[4] - Mask TTL flag
 *                 [br]bit[5] - Mask TCP Control flag
 *                 [br]bit[6] - Mask Fragmentation flag
 *                 [br]bit[7] - Mask MTU check flag
 *                 [br]bit[8] - Mask TCP Acknowledge flag
 *                 [br]bit[9] - Mask Divert Multicast Acknowledge
 *                 flag
 *                 [br]bit[12:10] - not used
 *
 */
#define PP_CHK_STAGE2_MATCH_FALLBACK_REG                 ((CHK_BASE_ADDR) + 0x4)
#define PP_CHK_STAGE2_MATCH_FALLBACK_RSVD0_OFF                      (14)
#define PP_CHK_STAGE2_MATCH_FALLBACK_RSVD0_LEN                      (18)
#define PP_CHK_STAGE2_MATCH_FALLBACK_RSVD0_MSK                      (0xFFFFC000)
#define PP_CHK_STAGE2_MATCH_FALLBACK_RSVD0_RST                      (0x0)
#define PP_CHK_STAGE2_MATCH_FALLBACK_DIS_SW_ERR_ALT_SI_OFF          (13)
#define PP_CHK_STAGE2_MATCH_FALLBACK_DIS_SW_ERR_ALT_SI_LEN          (1)
#define PP_CHK_STAGE2_MATCH_FALLBACK_DIS_SW_ERR_ALT_SI_MSK          (0x00002000)
#define PP_CHK_STAGE2_MATCH_FALLBACK_DIS_SW_ERR_ALT_SI_RST          (0x0)
#define PP_CHK_STAGE2_MATCH_FALLBACK_OFF                            (0)
#define PP_CHK_STAGE2_MATCH_FALLBACK_LEN                            (13)
#define PP_CHK_STAGE2_MATCH_FALLBACK_MSK                            (0x00001FFF)
#define PP_CHK_STAGE2_MATCH_FALLBACK_RST                            (0x0)

/**
 * SW_REG_NAME : PP_CHK_NF_Q0_REG
 * HW_REG_NAME : nf_queue_reg0
 * DESCRIPTION : NF Queue Register0. See Checker Developer's Guide
 *               for NF_QUEUEs functionallity.
 *
 *  Register Fields :
 *   [31:28][RO] - Reserved
 *   [27:16][RW] - NF Queue1: TurboDOX Queue
 *   [15:12][RO] - Reserved
 *   [11: 0][RW] - NF Queue0: Reassembly Queue
 *
 */
#define PP_CHK_NF_Q0_REG                                 ((CHK_BASE_ADDR) + 0x8)
#define PP_CHK_NF_Q0_RSVD0_OFF                                      (28)
#define PP_CHK_NF_Q0_RSVD0_LEN                                      (4)
#define PP_CHK_NF_Q0_RSVD0_MSK                                      (0xF0000000)
#define PP_CHK_NF_Q0_RSVD0_RST                                      (0x0)
#define PP_CHK_NF_Q0_NF_Q1_OFF                                      (16)
#define PP_CHK_NF_Q0_NF_Q1_LEN                                      (12)
#define PP_CHK_NF_Q0_NF_Q1_MSK                                      (0x0FFF0000)
#define PP_CHK_NF_Q0_NF_Q1_RST                                      (0x0)
#define PP_CHK_NF_Q0_RSVD1_OFF                                      (12)
#define PP_CHK_NF_Q0_RSVD1_LEN                                      (4)
#define PP_CHK_NF_Q0_RSVD1_MSK                                      (0x0000F000)
#define PP_CHK_NF_Q0_RSVD1_RST                                      (0x0)
#define PP_CHK_NF_Q0_OFF                                            (0)
#define PP_CHK_NF_Q0_LEN                                            (12)
#define PP_CHK_NF_Q0_MSK                                            (0x00000FFF)
#define PP_CHK_NF_Q0_RST                                            (0x0)

/**
 * SW_REG_NAME : PP_CHK_NF_Q1_REG
 * HW_REG_NAME : nf_queue_reg1
 * DESCRIPTION : NF Queue Register1
 *
 *  Register Fields :
 *   [31:28][RO] - Reserved
 *   [27:16][RW] - NF Queue3: MultiCast Queue
 *   [15:12][RO] - Reserved
 *   [11: 0][RW] - NF Queue2: Fragmentation Queue
 *
 */
#define PP_CHK_NF_Q1_REG                                ((CHK_BASE_ADDR) + 0x0C)
#define PP_CHK_NF_Q1_RSVD0_OFF                                      (28)
#define PP_CHK_NF_Q1_RSVD0_LEN                                      (4)
#define PP_CHK_NF_Q1_RSVD0_MSK                                      (0xF0000000)
#define PP_CHK_NF_Q1_RSVD0_RST                                      (0x0)
#define PP_CHK_NF_Q1_NF_Q3_OFF                                      (16)
#define PP_CHK_NF_Q1_NF_Q3_LEN                                      (12)
#define PP_CHK_NF_Q1_NF_Q3_MSK                                      (0x0FFF0000)
#define PP_CHK_NF_Q1_NF_Q3_RST                                      (0x0)
#define PP_CHK_NF_Q1_RSVD1_OFF                                      (12)
#define PP_CHK_NF_Q1_RSVD1_LEN                                      (4)
#define PP_CHK_NF_Q1_RSVD1_MSK                                      (0x0000F000)
#define PP_CHK_NF_Q1_RSVD1_RST                                      (0x0)
#define PP_CHK_NF_Q1_NF_Q2_OFF                                      (0)
#define PP_CHK_NF_Q1_NF_Q2_LEN                                      (12)
#define PP_CHK_NF_Q1_NF_Q2_MSK                                      (0x00000FFF)
#define PP_CHK_NF_Q1_NF_Q2_RST                                      (0x0)

/**
 * SW_REG_NAME : PP_CHK_SI_DDR_BASE_ADDR_REG
 * HW_REG_NAME : si_ddr_base_addr_reg
 * DESCRIPTION : DSI BASE ADDR. Base address is shifted ((4 to
 *               express the full 36b base address of DSI in the
 *               DDR.
 *
 *  Register Fields :
 *   [31: 0][RW] - DSI BASE ADDR
 *
 */
#define PP_CHK_SI_DDR_BASE_ADDR_REG                     ((CHK_BASE_ADDR) + 0x10)
#define PP_CHK_SI_DDR_BASE_ADDR_DSI_BASE_ADDR_OFF                   (0)
#define PP_CHK_SI_DDR_BASE_ADDR_DSI_BASE_ADDR_LEN                   (32)
#define PP_CHK_SI_DDR_BASE_ADDR_DSI_BASE_ADDR_MSK                   (0xFFFFFFFF)
#define PP_CHK_SI_DDR_BASE_ADDR_DSI_BASE_ADDR_RST                   (0x0)

/**
 * SW_REG_NAME : PP_CHK_ALT_SI_CNTS_RAM_W0_REG
 * HW_REG_NAME : alt_si_cnts_ram_wreg_0
 * DESCRIPTION : ALT_SI_CNTS Write Register LSbs. Writing Entry to
 *               ALT SI Counters. Same WREG Mechanism as above.
 *
 *  Register Fields :
 *   [31: 0][RW] - 32 LSbits of ALT_SI_CNTS Write Register
 *
 */
#define PP_CHK_ALT_SI_CNTS_RAM_W0_REG                   ((CHK_BASE_ADDR) + 0x14)
#define PP_CHK_ALT_SI_CNTS_RAM_W0_ALT_SI_CNTS_RAM_W0_OFF            (0)
#define PP_CHK_ALT_SI_CNTS_RAM_W0_ALT_SI_CNTS_RAM_W0_LEN            (32)
#define PP_CHK_ALT_SI_CNTS_RAM_W0_ALT_SI_CNTS_RAM_W0_MSK            (0xFFFFFFFF)
#define PP_CHK_ALT_SI_CNTS_RAM_W0_ALT_SI_CNTS_RAM_W0_RST            (0x0)

/**
 * SW_REG_NAME : PP_CHK_ALT_SI_CNTS_RAM_W1_REG
 * HW_REG_NAME : alt_si_cnts_ram_wreg_1
 * DESCRIPTION : ALT_SI_CNTS Write Register Mid Bits
 *
 *  Register Fields :
 *   [31: 0][RW] - 32 Mid bits of ALT_SI_CNTS Write Register
 *
 */
#define PP_CHK_ALT_SI_CNTS_RAM_W1_REG                   ((CHK_BASE_ADDR) + 0x18)
#define PP_CHK_ALT_SI_CNTS_RAM_W1_ALT_SI_CNTS_RAM_W1_OFF            (0)
#define PP_CHK_ALT_SI_CNTS_RAM_W1_ALT_SI_CNTS_RAM_W1_LEN            (32)
#define PP_CHK_ALT_SI_CNTS_RAM_W1_ALT_SI_CNTS_RAM_W1_MSK            (0xFFFFFFFF)
#define PP_CHK_ALT_SI_CNTS_RAM_W1_ALT_SI_CNTS_RAM_W1_RST            (0x0)

/**
 * SW_REG_NAME : PP_CHK_ALT_SI_CNTS_RAM_W2_REG
 * HW_REG_NAME : alt_si_cnts_ram_wreg_2
 * DESCRIPTION : ALT_SI_CNTS Write Register MSbs
 *
 *  Register Fields :
 *   [31: 0][RW] - 32 MSbits of ALT_SI_CNTS Write Register
 *
 */
#define PP_CHK_ALT_SI_CNTS_RAM_W2_REG                   ((CHK_BASE_ADDR) + 0x1C)
#define PP_CHK_ALT_SI_CNTS_RAM_W2_ALT_SI_CNTS_RAM_W2_OFF            (0)
#define PP_CHK_ALT_SI_CNTS_RAM_W2_ALT_SI_CNTS_RAM_W2_LEN            (32)
#define PP_CHK_ALT_SI_CNTS_RAM_W2_ALT_SI_CNTS_RAM_W2_MSK            (0xFFFFFFFF)
#define PP_CHK_ALT_SI_CNTS_RAM_W2_ALT_SI_CNTS_RAM_W2_RST            (0x0)

/**
 * SW_REG_NAME : PP_CHK_ALT_SI_CNTS_RAM_R0_REG
 * HW_REG_NAME : alt_si_cnts_ram_rreg_0
 * DESCRIPTION : ALT_SI_CNTS Read Register LSbs
 *
 *  Register Fields :
 *   [31: 0][RW] - 32 LSbits of ALT_SI_CNTS Read Register
 *
 */
#define PP_CHK_ALT_SI_CNTS_RAM_R0_REG                   ((CHK_BASE_ADDR) + 0x20)
#define PP_CHK_ALT_SI_CNTS_RAM_R0_ALT_SI_CNTS_RAM_R0_OFF            (0)
#define PP_CHK_ALT_SI_CNTS_RAM_R0_ALT_SI_CNTS_RAM_R0_LEN            (32)
#define PP_CHK_ALT_SI_CNTS_RAM_R0_ALT_SI_CNTS_RAM_R0_MSK            (0xFFFFFFFF)
#define PP_CHK_ALT_SI_CNTS_RAM_R0_ALT_SI_CNTS_RAM_R0_RST            (0x0)

/**
 * SW_REG_NAME : PP_CHK_ALT_SI_CNTS_RAM_R1_REG
 * HW_REG_NAME : alt_si_cnts_ram_rreg_1
 * DESCRIPTION : ALT_SI_CNTS Read Register Mid Bits
 *
 *  Register Fields :
 *   [31: 0][RW] - 32 Mid bits of ALT_SI_CNTS Read Register
 *
 */
#define PP_CHK_ALT_SI_CNTS_RAM_R1_REG                   ((CHK_BASE_ADDR) + 0x24)
#define PP_CHK_ALT_SI_CNTS_RAM_R1_ALT_SI_CNTS_RAM_R1_OFF            (0)
#define PP_CHK_ALT_SI_CNTS_RAM_R1_ALT_SI_CNTS_RAM_R1_LEN            (32)
#define PP_CHK_ALT_SI_CNTS_RAM_R1_ALT_SI_CNTS_RAM_R1_MSK            (0xFFFFFFFF)
#define PP_CHK_ALT_SI_CNTS_RAM_R1_ALT_SI_CNTS_RAM_R1_RST            (0x0)

/**
 * SW_REG_NAME : PP_CHK_ALT_SI_CNTS_RAM_R2_REG
 * HW_REG_NAME : alt_si_cnts_ram_rreg_2
 * DESCRIPTION : ALT_SI_CNTS Read Register MSbs
 *
 *  Register Fields :
 *   [31: 0][RW] - 32 MSbits of ALT_SI_CNTS Read Register
 *
 */
#define PP_CHK_ALT_SI_CNTS_RAM_R2_REG                   ((CHK_BASE_ADDR) + 0x28)
#define PP_CHK_ALT_SI_CNTS_RAM_R2_ALT_SI_CNTS_RAM_R2_OFF            (0)
#define PP_CHK_ALT_SI_CNTS_RAM_R2_ALT_SI_CNTS_RAM_R2_LEN            (32)
#define PP_CHK_ALT_SI_CNTS_RAM_R2_ALT_SI_CNTS_RAM_R2_MSK            (0xFFFFFFFF)
#define PP_CHK_ALT_SI_CNTS_RAM_R2_ALT_SI_CNTS_RAM_R2_RST            (0x0)

/**
 * SW_REG_NAME : PP_CHK_TBM_RAM_W0_REG
 * HW_REG_NAME : tbm_ram_wreg_0
 * DESCRIPTION : TBM Write Register LSbs. The TBM has 96bit
 *               entries. To update the 3 registers (WREG) hold the
 *               entry to be updated (write indirect).
 *               [br]To update an entry write the 96bit value to
 *               the 3 registers then write to the TBM entry
 *               address. The data written to the entry address
 *               [br]will be the entry in the 3 WREG registers.
 *
 *  Register Fields :
 *   [31: 0][RW] - 32 LSbits of TBM Write Register
 *
 */
#define PP_CHK_TBM_RAM_W0_REG                           ((CHK_BASE_ADDR) + 0x2C)
#define PP_CHK_TBM_RAM_W0_TBM_RAM_W0_OFF                            (0)
#define PP_CHK_TBM_RAM_W0_TBM_RAM_W0_LEN                            (32)
#define PP_CHK_TBM_RAM_W0_TBM_RAM_W0_MSK                            (0xFFFFFFFF)
#define PP_CHK_TBM_RAM_W0_TBM_RAM_W0_RST                            (0x0)

/**
 * SW_REG_NAME : PP_CHK_TBM_RAM_W1_REG
 * HW_REG_NAME : tbm_ram_wreg_1
 * DESCRIPTION : TBM Write Register Mid Bits
 *
 *  Register Fields :
 *   [31: 0][RW] - 32 Mid bits of TBM Write Register
 *
 */
#define PP_CHK_TBM_RAM_W1_REG                           ((CHK_BASE_ADDR) + 0x30)
#define PP_CHK_TBM_RAM_W1_TBM_RAM_W1_OFF                            (0)
#define PP_CHK_TBM_RAM_W1_TBM_RAM_W1_LEN                            (32)
#define PP_CHK_TBM_RAM_W1_TBM_RAM_W1_MSK                            (0xFFFFFFFF)
#define PP_CHK_TBM_RAM_W1_TBM_RAM_W1_RST                            (0x0)

/**
 * SW_REG_NAME : PP_CHK_TBM_RAM_W2_REG
 * HW_REG_NAME : tbm_ram_wreg_2
 * DESCRIPTION : TBM Write Register MSbs
 *
 *  Register Fields :
 *   [31: 0][RW] - 32 MSbits of TBM Write Register
 *
 */
#define PP_CHK_TBM_RAM_W2_REG                           ((CHK_BASE_ADDR) + 0x34)
#define PP_CHK_TBM_RAM_W2_TBM_RAM_W2_OFF                            (0)
#define PP_CHK_TBM_RAM_W2_TBM_RAM_W2_LEN                            (32)
#define PP_CHK_TBM_RAM_W2_TBM_RAM_W2_MSK                            (0xFFFFFFFF)
#define PP_CHK_TBM_RAM_W2_TBM_RAM_W2_RST                            (0x0)

/**
 * SW_REG_NAME : PP_CHK_TBM_RAM_R0_REG
 * HW_REG_NAME : tbm_ram_rreg_0
 * DESCRIPTION : TBM Read Register LSbs. The follwoing 3 RREG
 *               register has the same function as the WREG for
 *               reading a TBM entry.             [br]Read the TBM
 *               entry with the TBM entry address, then read the 3
 *               RREG to get the entry value.
 *
 *  Register Fields :
 *   [31: 0][RW] - 32 LSbits of TBM Read Register
 *
 */
#define PP_CHK_TBM_RAM_R0_REG                           ((CHK_BASE_ADDR) + 0x38)
#define PP_CHK_TBM_RAM_R0_TBM_RAM_R0_OFF                            (0)
#define PP_CHK_TBM_RAM_R0_TBM_RAM_R0_LEN                            (32)
#define PP_CHK_TBM_RAM_R0_TBM_RAM_R0_MSK                            (0xFFFFFFFF)
#define PP_CHK_TBM_RAM_R0_TBM_RAM_R0_RST                            (0x0)

/**
 * SW_REG_NAME : PP_CHK_TBM_RAM_R1_REG
 * HW_REG_NAME : tbm_ram_rreg_1
 * DESCRIPTION : TBM Read Register Mid Bits
 *
 *  Register Fields :
 *   [31: 0][RW] - 32 Mid bits of TBM Read Register
 *
 */
#define PP_CHK_TBM_RAM_R1_REG                           ((CHK_BASE_ADDR) + 0x3C)
#define PP_CHK_TBM_RAM_R1_TBM_RAM_R1_OFF                            (0)
#define PP_CHK_TBM_RAM_R1_TBM_RAM_R1_LEN                            (32)
#define PP_CHK_TBM_RAM_R1_TBM_RAM_R1_MSK                            (0xFFFFFFFF)
#define PP_CHK_TBM_RAM_R1_TBM_RAM_R1_RST                            (0x0)

/**
 * SW_REG_NAME : PP_CHK_TBM_RAM_R2_REG
 * HW_REG_NAME : tbm_ram_rreg_2
 * DESCRIPTION : TBM Read Register MSbs
 *
 *  Register Fields :
 *   [31: 0][RW] - 32 MSbits of TBM Read Register
 *
 */
#define PP_CHK_TBM_RAM_R2_REG                           ((CHK_BASE_ADDR) + 0x40)
#define PP_CHK_TBM_RAM_R2_TBM_RAM_R2_OFF                            (0)
#define PP_CHK_TBM_RAM_R2_TBM_RAM_R2_LEN                            (32)
#define PP_CHK_TBM_RAM_R2_TBM_RAM_R2_MSK                            (0xFFFFFFFF)
#define PP_CHK_TBM_RAM_R2_TBM_RAM_R2_RST                            (0x0)

/**
 * SW_REG_NAME : PP_CHK_GCNTS_SRAM0_W0_REG
 * HW_REG_NAME : gcnts_sram0_wreg_0
 * DESCRIPTION : GCNTS Write Register LSbs. 3xWREG to write to the
 *               GCNTs. Same concept as TBM WREG registers (see
 *               above).             [br]SRAM0 Has:
 *               [br]ADDR [0000..1023] - Group1.
 *               [br]ADDR [1024..2047] - Group2.
 *               [br]ADDR [2048..2559] - Group4.
 *               [br]ADDR [2560..2815] - Group5.
 *
 *  Register Fields :
 *   [31: 0][RW] - 32 LSbits of GCNTS Write Register
 *
 */
#define PP_CHK_GCNTS_SRAM0_W0_REG                       ((CHK_BASE_ADDR) + 0x44)
#define PP_CHK_GCNTS_SRAM0_W0_GCNTS_SRAM0_W0_OFF                    (0)
#define PP_CHK_GCNTS_SRAM0_W0_GCNTS_SRAM0_W0_LEN                    (32)
#define PP_CHK_GCNTS_SRAM0_W0_GCNTS_SRAM0_W0_MSK                    (0xFFFFFFFF)
#define PP_CHK_GCNTS_SRAM0_W0_GCNTS_SRAM0_W0_RST                    (0x0)

/**
 * SW_REG_NAME : PP_CHK_GCNTS_SRAM0_W1_REG
 * HW_REG_NAME : gcnts_sram0_wreg_1
 * DESCRIPTION : GCNTS Write Register Mid Bits
 *
 *  Register Fields :
 *   [31: 0][RW] - 32 Mid bits of GCNTS Write Register
 *
 */
#define PP_CHK_GCNTS_SRAM0_W1_REG                       ((CHK_BASE_ADDR) + 0x48)
#define PP_CHK_GCNTS_SRAM0_W1_GCNTS_SRAM0_W1_OFF                    (0)
#define PP_CHK_GCNTS_SRAM0_W1_GCNTS_SRAM0_W1_LEN                    (32)
#define PP_CHK_GCNTS_SRAM0_W1_GCNTS_SRAM0_W1_MSK                    (0xFFFFFFFF)
#define PP_CHK_GCNTS_SRAM0_W1_GCNTS_SRAM0_W1_RST                    (0x0)

/**
 * SW_REG_NAME : PP_CHK_GCNTS_SRAM0_W2_REG
 * HW_REG_NAME : gcnts_sram0_wreg_2
 * DESCRIPTION : GCNTS Write Register MSbs
 *
 *  Register Fields :
 *   [31: 0][RW] - 32 MSbits of GCNTS Write Register
 *
 */
#define PP_CHK_GCNTS_SRAM0_W2_REG                       ((CHK_BASE_ADDR) + 0x4C)
#define PP_CHK_GCNTS_SRAM0_W2_GCNTS_SRAM0_W2_OFF                    (0)
#define PP_CHK_GCNTS_SRAM0_W2_GCNTS_SRAM0_W2_LEN                    (32)
#define PP_CHK_GCNTS_SRAM0_W2_GCNTS_SRAM0_W2_MSK                    (0xFFFFFFFF)
#define PP_CHK_GCNTS_SRAM0_W2_GCNTS_SRAM0_W2_RST                    (0x0)

/**
 * SW_REG_NAME : PP_CHK_GCNTS_SRAM0_R0_REG
 * HW_REG_NAME : gcnts_sram0_rreg_0
 * DESCRIPTION : GCNTS Read Register LSbs. 3xWREG to read to the
 *               GCNTs. Same concept as TBM RREG registers (see
 *               above)
 *
 *  Register Fields :
 *   [31: 0][RW] - 32 LSbits of GCNTS Read Register
 *
 */
#define PP_CHK_GCNTS_SRAM0_R0_REG                       ((CHK_BASE_ADDR) + 0x50)
#define PP_CHK_GCNTS_SRAM0_R0_GCNTS_SRAM0_R0_OFF                    (0)
#define PP_CHK_GCNTS_SRAM0_R0_GCNTS_SRAM0_R0_LEN                    (32)
#define PP_CHK_GCNTS_SRAM0_R0_GCNTS_SRAM0_R0_MSK                    (0xFFFFFFFF)
#define PP_CHK_GCNTS_SRAM0_R0_GCNTS_SRAM0_R0_RST                    (0x0)

/**
 * SW_REG_NAME : PP_CHK_GCNTS_SRAM0_R1_REG
 * HW_REG_NAME : gcnts_sram0_rreg_1
 * DESCRIPTION : GCNTS Read Register Mid Bits
 *
 *  Register Fields :
 *   [31: 0][RW] - 32 Mid bits of GCNTS Read Register
 *
 */
#define PP_CHK_GCNTS_SRAM0_R1_REG                       ((CHK_BASE_ADDR) + 0x54)
#define PP_CHK_GCNTS_SRAM0_R1_GCNTS_SRAM0_R1_OFF                    (0)
#define PP_CHK_GCNTS_SRAM0_R1_GCNTS_SRAM0_R1_LEN                    (32)
#define PP_CHK_GCNTS_SRAM0_R1_GCNTS_SRAM0_R1_MSK                    (0xFFFFFFFF)
#define PP_CHK_GCNTS_SRAM0_R1_GCNTS_SRAM0_R1_RST                    (0x0)

/**
 * SW_REG_NAME : PP_CHK_GCNTS_SRAM0_R2_REG
 * HW_REG_NAME : gcnts_sram0_rreg_2
 * DESCRIPTION : GCNTS Read Register MSbs
 *
 *  Register Fields :
 *   [31: 0][RW] - 32 MSbits of GCNTS Read Register
 *
 */
#define PP_CHK_GCNTS_SRAM0_R2_REG                       ((CHK_BASE_ADDR) + 0x58)
#define PP_CHK_GCNTS_SRAM0_R2_GCNTS_SRAM0_R2_OFF                    (0)
#define PP_CHK_GCNTS_SRAM0_R2_GCNTS_SRAM0_R2_LEN                    (32)
#define PP_CHK_GCNTS_SRAM0_R2_GCNTS_SRAM0_R2_MSK                    (0xFFFFFFFF)
#define PP_CHK_GCNTS_SRAM0_R2_GCNTS_SRAM0_R2_RST                    (0x0)

/**
 * SW_REG_NAME : PP_CHK_GCNTS_SRAM1_W0_REG
 * HW_REG_NAME : gcnts_sram1_wreg_0
 * DESCRIPTION : GCNTS Write Register LSbs.             [br]SRAM1
 *               Has:             [br]ADDR [000..063] - Group3.
 *               [br]ADDR [064..127] - Group6.             [br]ADDR
 *               [128..143] - Group7.             [br]ADDR
 *               [144..151] - Group8.
 *
 *  Register Fields :
 *   [31: 0][RW] - 32 LSbits of GCNTS Write Register
 *
 */
#define PP_CHK_GCNTS_SRAM1_W0_REG                       ((CHK_BASE_ADDR) + 0x5C)
#define PP_CHK_GCNTS_SRAM1_W0_GCNTS_SRAM1_W0_OFF                    (0)
#define PP_CHK_GCNTS_SRAM1_W0_GCNTS_SRAM1_W0_LEN                    (32)
#define PP_CHK_GCNTS_SRAM1_W0_GCNTS_SRAM1_W0_MSK                    (0xFFFFFFFF)
#define PP_CHK_GCNTS_SRAM1_W0_GCNTS_SRAM1_W0_RST                    (0x0)

/**
 * SW_REG_NAME : PP_CHK_GCNTS_SRAM1_W1_REG
 * HW_REG_NAME : gcnts_sram1_wreg_1
 * DESCRIPTION : GCNTS Write Register Mid Bits
 *
 *  Register Fields :
 *   [31: 0][RW] - 32 Mid bits of GCNTS Write Register
 *
 */
#define PP_CHK_GCNTS_SRAM1_W1_REG                       ((CHK_BASE_ADDR) + 0x60)
#define PP_CHK_GCNTS_SRAM1_W1_GCNTS_SRAM1_W1_OFF                    (0)
#define PP_CHK_GCNTS_SRAM1_W1_GCNTS_SRAM1_W1_LEN                    (32)
#define PP_CHK_GCNTS_SRAM1_W1_GCNTS_SRAM1_W1_MSK                    (0xFFFFFFFF)
#define PP_CHK_GCNTS_SRAM1_W1_GCNTS_SRAM1_W1_RST                    (0x0)

/**
 * SW_REG_NAME : PP_CHK_GCNTS_SRAM1_W2_REG
 * HW_REG_NAME : gcnts_sram1_wreg_2
 * DESCRIPTION : GCNTS Write Register MSbs
 *
 *  Register Fields :
 *   [31: 0][RW] - 32 MSbits of GCNTS Write Register
 *
 */
#define PP_CHK_GCNTS_SRAM1_W2_REG                       ((CHK_BASE_ADDR) + 0x64)
#define PP_CHK_GCNTS_SRAM1_W2_GCNTS_SRAM1_W2_OFF                    (0)
#define PP_CHK_GCNTS_SRAM1_W2_GCNTS_SRAM1_W2_LEN                    (32)
#define PP_CHK_GCNTS_SRAM1_W2_GCNTS_SRAM1_W2_MSK                    (0xFFFFFFFF)
#define PP_CHK_GCNTS_SRAM1_W2_GCNTS_SRAM1_W2_RST                    (0x0)

/**
 * SW_REG_NAME : PP_CHK_GCNTS_SRAM1_R0_REG
 * HW_REG_NAME : gcnts_sram1_rreg_0
 * DESCRIPTION : GCNTS Read Register LSbs
 *
 *  Register Fields :
 *   [31: 0][RW] - 32 LSbits of GCNTS Read Register
 *
 */
#define PP_CHK_GCNTS_SRAM1_R0_REG                       ((CHK_BASE_ADDR) + 0x68)
#define PP_CHK_GCNTS_SRAM1_R0_GCNTS_SRAM1_R0_OFF                    (0)
#define PP_CHK_GCNTS_SRAM1_R0_GCNTS_SRAM1_R0_LEN                    (32)
#define PP_CHK_GCNTS_SRAM1_R0_GCNTS_SRAM1_R0_MSK                    (0xFFFFFFFF)
#define PP_CHK_GCNTS_SRAM1_R0_GCNTS_SRAM1_R0_RST                    (0x0)

/**
 * SW_REG_NAME : PP_CHK_GCNTS_SRAM1_R1_REG
 * HW_REG_NAME : gcnts_sram1_rreg_1
 * DESCRIPTION : GCNTS Read Register Mid Bits
 *
 *  Register Fields :
 *   [31: 0][RW] - 32 Mid bits of GCNTS Read Register
 *
 */
#define PP_CHK_GCNTS_SRAM1_R1_REG                       ((CHK_BASE_ADDR) + 0x6C)
#define PP_CHK_GCNTS_SRAM1_R1_GCNTS_SRAM1_R1_OFF                    (0)
#define PP_CHK_GCNTS_SRAM1_R1_GCNTS_SRAM1_R1_LEN                    (32)
#define PP_CHK_GCNTS_SRAM1_R1_GCNTS_SRAM1_R1_MSK                    (0xFFFFFFFF)
#define PP_CHK_GCNTS_SRAM1_R1_GCNTS_SRAM1_R1_RST                    (0x0)

/**
 * SW_REG_NAME : PP_CHK_GCNTS_SRAM1_R2_REG
 * HW_REG_NAME : gcnts_sram1_rreg_2
 * DESCRIPTION : GCNTS Read Register MSbs
 *
 *  Register Fields :
 *   [31: 0][RW] - 32 MSbits of GCNTS Read Register
 *
 */
#define PP_CHK_GCNTS_SRAM1_R2_REG                       ((CHK_BASE_ADDR) + 0x70)
#define PP_CHK_GCNTS_SRAM1_R2_GCNTS_SRAM1_R2_OFF                    (0)
#define PP_CHK_GCNTS_SRAM1_R2_GCNTS_SRAM1_R2_LEN                    (32)
#define PP_CHK_GCNTS_SRAM1_R2_GCNTS_SRAM1_R2_MSK                    (0xFFFFFFFF)
#define PP_CHK_GCNTS_SRAM1_R2_GCNTS_SRAM1_R2_RST                    (0x0)

/**
 * SW_REG_NAME : PP_CHK_MBX_SESSION_ID_REG
 * HW_REG_NAME : mbx_session_id
 * DESCRIPTION : Mailbox Session ID. Host has mailbox to access
 *               DSI. This register holds the Session ID.
 *
 *  Register Fields :
 *   [31:24][RO] - Reserved
 *   [23: 0][RW] - Mailbox Session ID
 *
 */
#define PP_CHK_MBX_SESSION_ID_REG                       ((CHK_BASE_ADDR) + 0x74)
#define PP_CHK_MBX_SESSION_ID_RSVD0_OFF                             (24)
#define PP_CHK_MBX_SESSION_ID_RSVD0_LEN                             (8)
#define PP_CHK_MBX_SESSION_ID_RSVD0_MSK                             (0xFF000000)
#define PP_CHK_MBX_SESSION_ID_RSVD0_RST                             (0x0)
#define PP_CHK_MBX_SESSION_ID_OFF                                   (0)
#define PP_CHK_MBX_SESSION_ID_LEN                                   (24)
#define PP_CHK_MBX_SESSION_ID_MSK                                   (0x00FFFFFF)
#define PP_CHK_MBX_SESSION_ID_RST                                   (0x0)

/**
 * SW_REG_NAME : PP_CHK_MBX_COMMAND_REG
 * HW_REG_NAME : mbx_command
 * DESCRIPTION : Mailbox command.Host has mailbox to access DSI.
 *               This register holds the Session ID.
 *
 *  Register Fields :
 *   [31: 0][RW] - Mailbox Information 32b command.
 *                 [br]For full description please advice 'Checker
 *                 Mailbox' section in the Dev. guide
 *
 */
#define PP_CHK_MBX_COMMAND_REG                          ((CHK_BASE_ADDR) + 0x78)
#define PP_CHK_MBX_COMMAND_MBX_CMD_OFF                              (0)
#define PP_CHK_MBX_COMMAND_MBX_CMD_LEN                              (32)
#define PP_CHK_MBX_COMMAND_MBX_CMD_MSK                              (0xFFFFFFFF)
#define PP_CHK_MBX_COMMAND_MBX_CMD_RST                              (0x0)

/**
 * SW_REG_NAME : PP_CHK_MBX_CRWLR_TIMER_INIT_REG
 * HW_REG_NAME : mbx_crwlr_timer_init
 * DESCRIPTION : Mailbox Timer Init Value
 *
 *  Register Fields :
 *   [31: 0][RW] - Mailbox Crawler Timer Init Value. When crawler is
 *                 triggered this is the counter's value.
 *
 */
#define PP_CHK_MBX_CRWLR_TIMER_INIT_REG                 ((CHK_BASE_ADDR) + 0x7C)
#define PP_CHK_MBX_CRWLR_TIMER_INIT_MBX_TIMER_INIT_VAL_OFF          (0)
#define PP_CHK_MBX_CRWLR_TIMER_INIT_MBX_TIMER_INIT_VAL_LEN          (32)
#define PP_CHK_MBX_CRWLR_TIMER_INIT_MBX_TIMER_INIT_VAL_MSK          (0xFFFFFFFF)
#define PP_CHK_MBX_CRWLR_TIMER_INIT_MBX_TIMER_INIT_VAL_RST          (0x400)

/**
 * SW_REG_NAME : PP_CHK_MBX_CRWLR_DDR_BASE_REG
 * HW_REG_NAME : mbx_crwlr_ddr_base
 * DESCRIPTION : Mailbox Crawler DDR Base
 *
 *  Register Fields :
 *   [31: 0][RW] - Mailbox Crawler DDR Stale Base (32MSbits of 36bit
 *                 address).
 *                 [br]This value is shifted ((4 to express a full
 *                 36b DDR address to write Stale Bitmap
 *
 */
#define PP_CHK_MBX_CRWLR_DDR_BASE_REG                   ((CHK_BASE_ADDR) + 0x80)
#define PP_CHK_MBX_CRWLR_DDR_BASE_MBX_CDDR_STALE_BASE_OFF           (0)
#define PP_CHK_MBX_CRWLR_DDR_BASE_MBX_CDDR_STALE_BASE_LEN           (32)
#define PP_CHK_MBX_CRWLR_DDR_BASE_MBX_CDDR_STALE_BASE_MSK           (0xFFFFFFFF)
#define PP_CHK_MBX_CRWLR_DDR_BASE_MBX_CDDR_STALE_BASE_RST           (0x400)

/**
 * SW_REG_NAME : PP_CHK_MBX_CRWLR_R0_REG
 * HW_REG_NAME : mbx_crwlr_rreg_0
 * DESCRIPTION : Mailbox Crawler Read Register0. For Mailbox Read
 *               command the read value will appear in the
 *               following 4 registers.
 *
 *  Register Fields :
 *   [31: 0][RW] - Mailbox Crawler Read Register0
 *
 */
#define PP_CHK_MBX_CRWLR_R0_REG                         ((CHK_BASE_ADDR) + 0x84)
#define PP_CHK_MBX_CRWLR_R0_MBX_CRWLR_R0_OFF                        (0)
#define PP_CHK_MBX_CRWLR_R0_MBX_CRWLR_R0_LEN                        (32)
#define PP_CHK_MBX_CRWLR_R0_MBX_CRWLR_R0_MSK                        (0xFFFFFFFF)
#define PP_CHK_MBX_CRWLR_R0_MBX_CRWLR_R0_RST                        (0x0)

/**
 * SW_REG_NAME : PP_CHK_MBX_CRWLR_R1_REG
 * HW_REG_NAME : mbx_crwlr_rreg_1
 * DESCRIPTION : Mailbox Crawler Read Register1
 *
 *  Register Fields :
 *   [31: 0][RW] - Mailbox Crawler Read Register1
 *
 */
#define PP_CHK_MBX_CRWLR_R1_REG                         ((CHK_BASE_ADDR) + 0x88)
#define PP_CHK_MBX_CRWLR_R1_MBX_CRWLR_R1_OFF                        (0)
#define PP_CHK_MBX_CRWLR_R1_MBX_CRWLR_R1_LEN                        (32)
#define PP_CHK_MBX_CRWLR_R1_MBX_CRWLR_R1_MSK                        (0xFFFFFFFF)
#define PP_CHK_MBX_CRWLR_R1_MBX_CRWLR_R1_RST                        (0x0)

/**
 * SW_REG_NAME : PP_CHK_MBX_CRWLR_R2_REG
 * HW_REG_NAME : mbx_crwlr_rreg_2
 * DESCRIPTION : Mailbox Crawler Read Register2
 *
 *  Register Fields :
 *   [31: 0][RW] - Mailbox Crawler Read Register2
 *
 */
#define PP_CHK_MBX_CRWLR_R2_REG                         ((CHK_BASE_ADDR) + 0x8C)
#define PP_CHK_MBX_CRWLR_R2_MBX_CRWLR_R2_OFF                        (0)
#define PP_CHK_MBX_CRWLR_R2_MBX_CRWLR_R2_LEN                        (32)
#define PP_CHK_MBX_CRWLR_R2_MBX_CRWLR_R2_MSK                        (0xFFFFFFFF)
#define PP_CHK_MBX_CRWLR_R2_MBX_CRWLR_R2_RST                        (0x0)

/**
 * SW_REG_NAME : PP_CHK_MBX_CRWLR_R3_REG
 * HW_REG_NAME : mbx_crwlr_rreg_3
 * DESCRIPTION : Mailbox Crawler Read Register3
 *
 *  Register Fields :
 *   [31: 0][RW] - Mailbox Crawler Read Register3
 *
 */
#define PP_CHK_MBX_CRWLR_R3_REG                         ((CHK_BASE_ADDR) + 0x90)
#define PP_CHK_MBX_CRWLR_R3_MBX_CRWLR_R3_OFF                        (0)
#define PP_CHK_MBX_CRWLR_R3_MBX_CRWLR_R3_LEN                        (32)
#define PP_CHK_MBX_CRWLR_R3_MBX_CRWLR_R3_MSK                        (0xFFFFFFFF)
#define PP_CHK_MBX_CRWLR_R3_MBX_CRWLR_R3_RST                        (0x0)

/**
 * SW_REG_NAME : PP_CHK_MBX_STALE_CNT_REG
 * HW_REG_NAME : mbx_stale_cnt_reg
 * DESCRIPTION : This register holds the stale counter value
 *
 *  Register Fields :
 *   [31: 0][RW] - Resets every new Crawler command final value
 *                 should be read when crawlers is done (idle)
 *
 */
#define PP_CHK_MBX_STALE_CNT_REG                        ((CHK_BASE_ADDR) + 0x94)
#define PP_CHK_MBX_STALE_CNT_OFF                                    (0)
#define PP_CHK_MBX_STALE_CNT_LEN                                    (32)
#define PP_CHK_MBX_STALE_CNT_MSK                                    (0xFFFFFFFF)
#define PP_CHK_MBX_STALE_CNT_RST                                    (0x0)

/**
 * SW_REG_NAME : PP_CHK_MBX_CRWLR_STATUS_REG
 * HW_REG_NAME : mbx_crwlr_status
 * DESCRIPTION : Mailbox Crawler Status
 *
 *  Register Fields :
 *   [31: 0][RW] - Mailbox Crawler Status [br]
 *                 [00:00] - Crawler is Idle [br]
 *                 [00:01] - Crawler in Wait State (Timer) [br]
 *                 [00:02] - Crawler in Waiting for Final Stale State
 *                 (from Stage2) [br]
 *                 [00:03] - Crawler DSI Sync Loss Error (sticky)
 *                 [31:16] - Command Counter
 *
 */
#define PP_CHK_MBX_CRWLR_STATUS_REG                    ((CHK_BASE_ADDR) + 0x0A0)
#define PP_CHK_MBX_CRWLR_STATUS_OFF                                 (0)
#define PP_CHK_MBX_CRWLR_STATUS_LEN                                 (32)
#define PP_CHK_MBX_CRWLR_STATUS_MSK                                 (0xFFFFFFFF)
#define PP_CHK_MBX_CRWLR_STATUS_RST                                 (0x0)

/**
 * SW_REG_NAME : PP_CHK_HOST_CNTS_UPDATE_REG
 * HW_REG_NAME : host_cnts_update
 * DESCRIPTION : Host is able to manually update a Group Counter
 *               using this Register. Register INIT value is 0.
 *               [br]Once written value)0 the GNTs HW will trigger
 *               the counters update and will reset the register
 *               value to 0 once done.         [br]Please See
 *               Checker Developers Guide for Command Format
 *
 *  Register Fields :
 *   [31: 0][RW] - Please See Checker Developers Guide for Command
 *                 Format
 *
 */
#define PP_CHK_HOST_CNTS_UPDATE_REG                    ((CHK_BASE_ADDR) + 0x0A4)
#define PP_CHK_HOST_CNTS_UPDATE_OFF                                 (0)
#define PP_CHK_HOST_CNTS_UPDATE_LEN                                 (32)
#define PP_CHK_HOST_CNTS_UPDATE_MSK                                 (0xFFFFFFFF)
#define PP_CHK_HOST_CNTS_UPDATE_RST                                 (0x0)

/**
 * SW_REG_NAME : PP_CHK_MAIN_STATUS_REG
 * HW_REG_NAME : main_status
 * DESCRIPTION : Main Status
 *
 *  Register Fields :
 *   [31:16][RW] - Counts Header Diff Err
 *   [15: 1][RO] - Reserved
 *   [ 0: 0][RW] - If High the Checker is in INIT
 *
 */
#define PP_CHK_MAIN_STATUS_REG                         ((CHK_BASE_ADDR) + 0x0A8)
#define PP_CHK_MAIN_STATUS_DIFF_ERR_CNT_OFF                         (16)
#define PP_CHK_MAIN_STATUS_DIFF_ERR_CNT_LEN                         (16)
#define PP_CHK_MAIN_STATUS_DIFF_ERR_CNT_MSK                         (0xFFFF0000)
#define PP_CHK_MAIN_STATUS_DIFF_ERR_CNT_RST                         (0x0)
#define PP_CHK_MAIN_STATUS_RSVD0_OFF                                (1)
#define PP_CHK_MAIN_STATUS_RSVD0_LEN                                (15)
#define PP_CHK_MAIN_STATUS_RSVD0_MSK                                (0x0000FFFE)
#define PP_CHK_MAIN_STATUS_RSVD0_RST                                (0x0)
#define PP_CHK_MAIN_STATUS_INIT_OFF                                 (0)
#define PP_CHK_MAIN_STATUS_INIT_LEN                                 (1)
#define PP_CHK_MAIN_STATUS_INIT_MSK                                 (0x00000001)
#define PP_CHK_MAIN_STATUS_INIT_RST                                 (0x0)

/**
 * SW_REG_NAME : PP_CHK_MAIN_CTRL_REG
 * HW_REG_NAME : main_ctrl
 * DESCRIPTION : Main Control
 *
 *  Register Fields :
 *   [31: 1][RO] - Reserved
 *   [ 0: 0][RW] - '1' Enable Header Diff Error detection
 *
 */
#define PP_CHK_MAIN_CTRL_REG                           ((CHK_BASE_ADDR) + 0x0AC)
#define PP_CHK_MAIN_CTRL_RSVD0_OFF                                  (1)
#define PP_CHK_MAIN_CTRL_RSVD0_LEN                                  (31)
#define PP_CHK_MAIN_CTRL_RSVD0_MSK                                  (0xFFFFFFFE)
#define PP_CHK_MAIN_CTRL_RSVD0_RST                                  (0x0)
#define PP_CHK_MAIN_CTRL_DIFF_ERR_EN_OFF                            (0)
#define PP_CHK_MAIN_CTRL_DIFF_ERR_EN_LEN                            (1)
#define PP_CHK_MAIN_CTRL_DIFF_ERR_EN_MSK                            (0x00000001)
#define PP_CHK_MAIN_CTRL_DIFF_ERR_EN_RST                            (0x1)

/**
 * SW_REG_NAME : PP_CHK_DROP_STRATEGY_REG
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
#define PP_CHK_DROP_STRATEGY_REG                       ((CHK_BASE_ADDR) + 0x100)
#define PP_CHK_DROP_STRATEGY_RSVD0_OFF                              (17)
#define PP_CHK_DROP_STRATEGY_RSVD0_LEN                              (15)
#define PP_CHK_DROP_STRATEGY_RSVD0_MSK                              (0xFFFE0000)
#define PP_CHK_DROP_STRATEGY_RSVD0_RST                              (0x0)
#define PP_CHK_DROP_STRATEGY_STRATEGIES_OFF                         (15)
#define PP_CHK_DROP_STRATEGY_STRATEGIES_LEN                         (2)
#define PP_CHK_DROP_STRATEGY_STRATEGIES_MSK                         (0x00018000)
#define PP_CHK_DROP_STRATEGY_STRATEGIES_RST                         (0x0)
#define PP_CHK_DROP_STRATEGY_RSVD1_OFF                              (9)
#define PP_CHK_DROP_STRATEGY_RSVD1_LEN                              (6)
#define PP_CHK_DROP_STRATEGY_RSVD1_MSK                              (0x00007E00)
#define PP_CHK_DROP_STRATEGY_RSVD1_RST                              (0x0)
#define PP_CHK_DROP_STRATEGY_RPB_TH_EN_OFF                          (8)
#define PP_CHK_DROP_STRATEGY_RPB_TH_EN_LEN                          (1)
#define PP_CHK_DROP_STRATEGY_RPB_TH_EN_MSK                          (0x00000100)
#define PP_CHK_DROP_STRATEGY_RPB_TH_EN_RST                          (0x0)
#define PP_CHK_DROP_STRATEGY_RSVD2_OFF                              (1)
#define PP_CHK_DROP_STRATEGY_RSVD2_LEN                              (7)
#define PP_CHK_DROP_STRATEGY_RSVD2_MSK                              (0x000000FE)
#define PP_CHK_DROP_STRATEGY_RSVD2_RST                              (0x0)
#define PP_CHK_DROP_STRATEGY_PKT_RATE_EN_OFF                        (0)
#define PP_CHK_DROP_STRATEGY_PKT_RATE_EN_LEN                        (1)
#define PP_CHK_DROP_STRATEGY_PKT_RATE_EN_MSK                        (0x00000001)
#define PP_CHK_DROP_STRATEGY_PKT_RATE_EN_RST                        (0x0)

/**
 * SW_REG_NAME : PP_CHK_RPB_FC_MASK_REG
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
#define PP_CHK_RPB_FC_MASK_REG                         ((CHK_BASE_ADDR) + 0x104)
#define PP_CHK_RPB_FC_MASK_RSVD0_OFF                                (28)
#define PP_CHK_RPB_FC_MASK_RSVD0_LEN                                (4)
#define PP_CHK_RPB_FC_MASK_RSVD0_MSK                                (0xF0000000)
#define PP_CHK_RPB_FC_MASK_RSVD0_RST                                (0x0)
#define PP_CHK_RPB_FC_MASK_FC_3_MASK_OFF                            (24)
#define PP_CHK_RPB_FC_MASK_FC_3_MASK_LEN                            (4)
#define PP_CHK_RPB_FC_MASK_FC_3_MASK_MSK                            (0x0F000000)
#define PP_CHK_RPB_FC_MASK_FC_3_MASK_RST                            (0x0)
#define PP_CHK_RPB_FC_MASK_RSVD1_OFF                                (20)
#define PP_CHK_RPB_FC_MASK_RSVD1_LEN                                (4)
#define PP_CHK_RPB_FC_MASK_RSVD1_MSK                                (0x00F00000)
#define PP_CHK_RPB_FC_MASK_RSVD1_RST                                (0x0)
#define PP_CHK_RPB_FC_MASK_FC_2_MASK_OFF                            (16)
#define PP_CHK_RPB_FC_MASK_FC_2_MASK_LEN                            (4)
#define PP_CHK_RPB_FC_MASK_FC_2_MASK_MSK                            (0x000F0000)
#define PP_CHK_RPB_FC_MASK_FC_2_MASK_RST                            (0x0)
#define PP_CHK_RPB_FC_MASK_RSVD2_OFF                                (12)
#define PP_CHK_RPB_FC_MASK_RSVD2_LEN                                (4)
#define PP_CHK_RPB_FC_MASK_RSVD2_MSK                                (0x0000F000)
#define PP_CHK_RPB_FC_MASK_RSVD2_RST                                (0x0)
#define PP_CHK_RPB_FC_MASK_FC_1_MASK_OFF                            (8)
#define PP_CHK_RPB_FC_MASK_FC_1_MASK_LEN                            (4)
#define PP_CHK_RPB_FC_MASK_FC_1_MASK_MSK                            (0x00000F00)
#define PP_CHK_RPB_FC_MASK_FC_1_MASK_RST                            (0x0)
#define PP_CHK_RPB_FC_MASK_RSVD3_OFF                                (4)
#define PP_CHK_RPB_FC_MASK_RSVD3_LEN                                (4)
#define PP_CHK_RPB_FC_MASK_RSVD3_MSK                                (0x000000F0)
#define PP_CHK_RPB_FC_MASK_RSVD3_RST                                (0x0)
#define PP_CHK_RPB_FC_MASK_FC_0_MASK_OFF                            (0)
#define PP_CHK_RPB_FC_MASK_FC_0_MASK_LEN                            (4)
#define PP_CHK_RPB_FC_MASK_FC_0_MASK_MSK                            (0x0000000F)
#define PP_CHK_RPB_FC_MASK_FC_0_MASK_RST                            (0x0)

/**
 * SW_REG_NAME : PP_CHK_PKT_RATE_MON_INC_REG
 * HW_REG_NAME : packet_rate_mon_increment_reg
 * DESCRIPTION : packet rate monitor increment value
 *
 *  Register Fields :
 *   [31:14][RO] - Reserved
 *   [13: 0][RW] - Increment a packet rate counter by configurable
 *                 amount (up to 8192)
 *
 */
#define PP_CHK_PKT_RATE_MON_INC_REG                    ((CHK_BASE_ADDR) + 0x108)
#define PP_CHK_PKT_RATE_MON_INC_RSVD0_OFF                           (14)
#define PP_CHK_PKT_RATE_MON_INC_RSVD0_LEN                           (18)
#define PP_CHK_PKT_RATE_MON_INC_RSVD0_MSK                           (0xFFFFC000)
#define PP_CHK_PKT_RATE_MON_INC_RSVD0_RST                           (0x0)
#define PP_CHK_PKT_RATE_MON_INC_OFF                                 (0)
#define PP_CHK_PKT_RATE_MON_INC_LEN                                 (14)
#define PP_CHK_PKT_RATE_MON_INC_MSK                                 (0x00003FFF)
#define PP_CHK_PKT_RATE_MON_INC_RST                                 (0x0)

/**
 * SW_REG_NAME : PP_CHK_PKT_RATE_MON_SATURATE_REG
 * HW_REG_NAME : packet_rate_mon_saturate_reg
 * DESCRIPTION : packet rate monitor high saturate
 *
 *  Register Fields :
 *   [31:19][RO] - Reserved
 *   [18: 0][RW] - packet rate monitor counter high saturate
 *                 configurable - up to 128k
 *
 */
#define PP_CHK_PKT_RATE_MON_SATURATE_REG               ((CHK_BASE_ADDR) + 0x10C)
#define PP_CHK_PKT_RATE_MON_SATURATE_RSVD0_OFF                      (19)
#define PP_CHK_PKT_RATE_MON_SATURATE_RSVD0_LEN                      (13)
#define PP_CHK_PKT_RATE_MON_SATURATE_RSVD0_MSK                      (0xFFF80000)
#define PP_CHK_PKT_RATE_MON_SATURATE_RSVD0_RST                      (0x0)
#define PP_CHK_PKT_RATE_MON_SATURATE_HIGH_SATURATE_OFF              (0)
#define PP_CHK_PKT_RATE_MON_SATURATE_HIGH_SATURATE_LEN              (19)
#define PP_CHK_PKT_RATE_MON_SATURATE_HIGH_SATURATE_MSK              (0x0007FFFF)
#define PP_CHK_PKT_RATE_MON_SATURATE_HIGH_SATURATE_RST              (0x0)

/**
 * SW_REG_NAME : PP_CHK_PKT_RATE_STATUS_REG
 * HW_REG_NAME : packet_rate_status_reg
 * DESCRIPTION : packet_rate_drop_counter
 *
 *  Register Fields :
 *   [31: 0][RW] - packet rate drop counter
 *
 */
#define PP_CHK_PKT_RATE_STATUS_REG                     ((CHK_BASE_ADDR) + 0x110)
#define PP_CHK_PKT_RATE_STATUS_DROP_CNT_OFF                         (0)
#define PP_CHK_PKT_RATE_STATUS_DROP_CNT_LEN                         (32)
#define PP_CHK_PKT_RATE_STATUS_DROP_CNT_MSK                         (0xFFFFFFFF)
#define PP_CHK_PKT_RATE_STATUS_DROP_CNT_RST                         (0x0)

/**
 * SW_REG_NAME : PP_CHK_RPB_PPRS_TH_STATUS_REG
 * HW_REG_NAME : rpb_pprs_th_status_reg
 * DESCRIPTION : rpb_pprs_th_drop_counter
 *
 *  Register Fields :
 *   [31: 0][RW] - RPB pprs threshold drop counter
 *
 */
#define PP_CHK_RPB_PPRS_TH_STATUS_REG                  ((CHK_BASE_ADDR) + 0x114)
#define PP_CHK_RPB_PPRS_TH_STATUS_DROP_CNT_OFF                      (0)
#define PP_CHK_RPB_PPRS_TH_STATUS_DROP_CNT_LEN                      (32)
#define PP_CHK_RPB_PPRS_TH_STATUS_DROP_CNT_MSK                      (0xFFFFFFFF)
#define PP_CHK_RPB_PPRS_TH_STATUS_DROP_CNT_RST                      (0x0)

/**
 * SW_REG_NAME : PP_CHK_MBX_CRWLR_CTRL_REG
 * HW_REG_NAME : mbx_crwlr_ctrl
 * DESCRIPTION : Mailbox Control. DSI has a crawler which
 *               deactivaes sessions in order to close stale
 *               sessions. See Dev. Guide for more info.
 *
 *  Register Fields :
 *   [31:31][RW] - Mailbox Crawler Reset.[br]If Crawler gets stuck
 *                 this is soft reset.
 *   [30:17][RO] - Reserved
 *   [16:16][RW] - Mailbox Crawler DDR Mode
 *                 [br]The Crawler creates a bitmap of the slate
 *                 sessions and writes the bitmap to the DDR.
 *                 [br]0 - No Limitation.
 *                 [br]1 - Stale bitmap area Limited to 8K sessions,
 *                 and wraps around.
 *   [15: 0][RW] - Mailbox Crawler Timer pace. Decrement value for
 *                 Crawler internal Timer.
 *
 */
#define PP_CHK_MBX_CRWLR_CTRL_REG                      ((CHK_BASE_ADDR) + 0x118)
#define PP_CHK_MBX_CRWLR_CTRL_MBX_CRWLR_RESET_OFF                   (31)
#define PP_CHK_MBX_CRWLR_CTRL_MBX_CRWLR_RESET_LEN                   (1)
#define PP_CHK_MBX_CRWLR_CTRL_MBX_CRWLR_RESET_MSK                   (0x80000000)
#define PP_CHK_MBX_CRWLR_CTRL_MBX_CRWLR_RESET_RST                   (0x0)
#define PP_CHK_MBX_CRWLR_CTRL_RSVD0_OFF                             (17)
#define PP_CHK_MBX_CRWLR_CTRL_RSVD0_LEN                             (14)
#define PP_CHK_MBX_CRWLR_CTRL_RSVD0_MSK                             (0x7FFE0000)
#define PP_CHK_MBX_CRWLR_CTRL_RSVD0_RST                             (0x0)
#define PP_CHK_MBX_CRWLR_CTRL_MBX_CRWLR_DDR_MODE_OFF                (16)
#define PP_CHK_MBX_CRWLR_CTRL_MBX_CRWLR_DDR_MODE_LEN                (1)
#define PP_CHK_MBX_CRWLR_CTRL_MBX_CRWLR_DDR_MODE_MSK                (0x00010000)
#define PP_CHK_MBX_CRWLR_CTRL_MBX_CRWLR_DDR_MODE_RST                (0x0)
#define PP_CHK_MBX_CRWLR_CTRL_MBX_TIMER_PACE_OFF                    (0)
#define PP_CHK_MBX_CRWLR_CTRL_MBX_TIMER_PACE_LEN                    (16)
#define PP_CHK_MBX_CRWLR_CTRL_MBX_TIMER_PACE_MSK                    (0x0000FFFF)
#define PP_CHK_MBX_CRWLR_CTRL_MBX_TIMER_PACE_RST                    (0x1)

/**
 * SW_REG_NAME : PP_CHK_STALL_REG
 * HW_REG_NAME : stall_reg
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 0][RW] - Watermark for longest stall from Modifier
 *
 */
#define PP_CHK_STALL_REG                               ((CHK_BASE_ADDR) + 0x11C)
#define PP_CHK_STALL_STALL_CNT_OFF                                  (0)
#define PP_CHK_STALL_STALL_CNT_LEN                                  (32)
#define PP_CHK_STALL_STALL_CNT_MSK                                  (0xFFFFFFFF)
#define PP_CHK_STALL_STALL_CNT_RST                                  (0x0)

/**
 * SW_REG_NAME : PP_CHK_CLS_PKT_CNT_REG
 * HW_REG_NAME : cls_pkt_cnt
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 0][RW] - Counts Packets recieved on input from Classifier
 *
 */
#define PP_CHK_CLS_PKT_CNT_REG                         ((CHK_BASE_ADDR) + 0x120)
#define PP_CHK_CLS_PKT_CNT_OFF                                      (0)
#define PP_CHK_CLS_PKT_CNT_LEN                                      (32)
#define PP_CHK_CLS_PKT_CNT_MSK                                      (0xFFFFFFFF)
#define PP_CHK_CLS_PKT_CNT_RST                                      (0x0)

/**
 * SW_REG_NAME : PP_CHK_SYNC_CNT_REG
 * HW_REG_NAME : sync_cnt
 * DESCRIPTION : sync_cnt debug counter, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][RW] - sync_cnt
 *
 */
#define PP_CHK_SYNC_CNT_REG                            ((CHK_BASE_ADDR) + 0x124)
#define PP_CHK_SYNC_CNT_OFF                                         (0)
#define PP_CHK_SYNC_CNT_LEN                                         (32)
#define PP_CHK_SYNC_CNT_MSK                                         (0xFFFFFFFF)
#define PP_CHK_SYNC_CNT_RST                                         (0x0)

/**
 * SW_REG_NAME : PP_CHK_SYNC_FIRST_CNT_REG
 * HW_REG_NAME : sync_first_cnt
 * DESCRIPTION : sync_first_cnt debug counter, NOTE, B Step ONLY
 *               register
 *
 *  Register Fields :
 *   [31: 0][RW] - sync_first_cnt
 *
 */
#define PP_CHK_SYNC_FIRST_CNT_REG                      ((CHK_BASE_ADDR) + 0x128)
#define PP_CHK_SYNC_FIRST_CNT_OFF                                   (0)
#define PP_CHK_SYNC_FIRST_CNT_LEN                                   (32)
#define PP_CHK_SYNC_FIRST_CNT_MSK                                   (0xFFFFFFFF)
#define PP_CHK_SYNC_FIRST_CNT_RST                                   (0x0)

/**
 * SW_REG_NAME : PP_CHK_SEND2UC_CNT_REG
 * HW_REG_NAME : send2uc_cnt
 * DESCRIPTION : send2uc_cnt debug counter, NOTE, B Step ONLY
 *               register
 *
 *  Register Fields :
 *   [31: 0][RW] - send2uc_cnt
 *
 */
#define PP_CHK_SEND2UC_CNT_REG                         ((CHK_BASE_ADDR) + 0x12C)
#define PP_CHK_SEND2UC_CNT_OFF                                      (0)
#define PP_CHK_SEND2UC_CNT_LEN                                      (32)
#define PP_CHK_SEND2UC_CNT_MSK                                      (0xFFFFFFFF)
#define PP_CHK_SEND2UC_CNT_RST                                      (0x0)

/**
 * SW_REG_NAME : PP_CHK_DROP_FLAG_CNT_REG
 * HW_REG_NAME : drop_flag_cnt
 * DESCRIPTION : Drop Flag Counter, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][RW] - drop_flag_cnt
 *
 */
#define PP_CHK_DROP_FLAG_CNT_REG                       ((CHK_BASE_ADDR) + 0x130)
#define PP_CHK_DROP_FLAG_CNT_OFF                                    (0)
#define PP_CHK_DROP_FLAG_CNT_LEN                                    (32)
#define PP_CHK_DROP_FLAG_CNT_MSK                                    (0xFFFFFFFF)
#define PP_CHK_DROP_FLAG_CNT_RST                                    (0x0)

/**
 * SW_REG_NAME : PP_CHK_DYNAMIC_CNT_REG
 * HW_REG_NAME : dynamic_cnt
 * DESCRIPTION : dynamic_cnt debug counter, NOTE, B Step ONLY
 *               register
 *
 *  Register Fields :
 *   [31: 0][RW] - dynamic_cnt
 *
 */
#define PP_CHK_DYNAMIC_CNT_REG                         ((CHK_BASE_ADDR) + 0x134)
#define PP_CHK_DYNAMIC_CNT_OFF                                      (0)
#define PP_CHK_DYNAMIC_CNT_LEN                                      (32)
#define PP_CHK_DYNAMIC_CNT_MSK                                      (0xFFFFFFFF)
#define PP_CHK_DYNAMIC_CNT_RST                                      (0x0)

/**
 * SW_REG_NAME : PP_CHK_IP_OPT_CNT_REG
 * HW_REG_NAME : ip_opt_cnt
 * DESCRIPTION : ip_opt_cnt debug counter, NOTE, B Step ONLY
 *               register
 *
 *  Register Fields :
 *   [31: 0][RW] - ip_opt_cnt
 *
 */
#define PP_CHK_IP_OPT_CNT_REG                          ((CHK_BASE_ADDR) + 0x138)
#define PP_CHK_IP_OPT_CNT_OFF                                       (0)
#define PP_CHK_IP_OPT_CNT_LEN                                       (32)
#define PP_CHK_IP_OPT_CNT_MSK                                       (0xFFFFFFFF)
#define PP_CHK_IP_OPT_CNT_RST                                       (0x0)

/**
 * SW_REG_NAME : PP_CHK_TTL_EXP_CNT_REG
 * HW_REG_NAME : ttl_exp_cnt
 * DESCRIPTION : ttl_exp_cnt debug counter, NOTE, B Step ONLY
 *               register
 *
 *  Register Fields :
 *   [31: 0][RW] - ttl_exp_cnt
 *
 */
#define PP_CHK_TTL_EXP_CNT_REG                         ((CHK_BASE_ADDR) + 0x13C)
#define PP_CHK_TTL_EXP_CNT_OFF                                      (0)
#define PP_CHK_TTL_EXP_CNT_LEN                                      (32)
#define PP_CHK_TTL_EXP_CNT_MSK                                      (0xFFFFFFFF)
#define PP_CHK_TTL_EXP_CNT_RST                                      (0x0)

/**
 * SW_REG_NAME : PP_CHK_TCP_CTRL_CNT_REG
 * HW_REG_NAME : tcp_ctrl_cnt
 * DESCRIPTION : tcp_ctrl_cnt debug counter, NOTE, B Step ONLY
 *               register
 *
 *  Register Fields :
 *   [31: 0][RW] - tcp_ctrl_cnt
 *
 */
#define PP_CHK_TCP_CTRL_CNT_REG                        ((CHK_BASE_ADDR) + 0x140)
#define PP_CHK_TCP_CTRL_CNT_OFF                                     (0)
#define PP_CHK_TCP_CTRL_CNT_LEN                                     (32)
#define PP_CHK_TCP_CTRL_CNT_MSK                                     (0xFFFFFFFF)
#define PP_CHK_TCP_CTRL_CNT_RST                                     (0x0)

/**
 * SW_REG_NAME : PP_CHK_FRG_CNT_REG
 * HW_REG_NAME : frg_cnt
 * DESCRIPTION : frg_cnt debug counter, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][RW] - frg_cnt
 *
 */
#define PP_CHK_FRG_CNT_REG                             ((CHK_BASE_ADDR) + 0x144)
#define PP_CHK_FRG_CNT_OFF                                          (0)
#define PP_CHK_FRG_CNT_LEN                                          (32)
#define PP_CHK_FRG_CNT_MSK                                          (0xFFFFFFFF)
#define PP_CHK_FRG_CNT_RST                                          (0x0)

/**
 * SW_REG_NAME : PP_CHK_MTU_CNT_REG
 * HW_REG_NAME : mtu_cnt
 * DESCRIPTION : mtu_cnt debug counter, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][RW] - mtu_cnt
 *
 */
#define PP_CHK_MTU_CNT_REG                             ((CHK_BASE_ADDR) + 0x148)
#define PP_CHK_MTU_CNT_OFF                                          (0)
#define PP_CHK_MTU_CNT_LEN                                          (32)
#define PP_CHK_MTU_CNT_MSK                                          (0xFFFFFFFF)
#define PP_CHK_MTU_CNT_RST                                          (0x0)

/**
 * SW_REG_NAME : PP_CHK_TCP_ACK_CNT_REG
 * HW_REG_NAME : tcp_ack_cnt
 * DESCRIPTION : tcp_ack_cnt debug counter, NOTE, B Step ONLY
 *               register
 *
 *  Register Fields :
 *   [31: 0][RW] - tcp_ack_cnt
 *
 */
#define PP_CHK_TCP_ACK_CNT_REG                         ((CHK_BASE_ADDR) + 0x14C)
#define PP_CHK_TCP_ACK_CNT_OFF                                      (0)
#define PP_CHK_TCP_ACK_CNT_LEN                                      (32)
#define PP_CHK_TCP_ACK_CNT_MSK                                      (0xFFFFFFFF)
#define PP_CHK_TCP_ACK_CNT_RST                                      (0x0)

/**
 * SW_REG_NAME : PP_CHK_MULTICAST_CNT_REG
 * HW_REG_NAME : multicast_cnt
 * DESCRIPTION : multicast_cnt debug counter, NOTE, B Step ONLY
 *               register
 *
 *  Register Fields :
 *   [31: 0][RW] - multicast_cnt
 *
 */
#define PP_CHK_MULTICAST_CNT_REG                       ((CHK_BASE_ADDR) + 0x150)
#define PP_CHK_MULTICAST_CNT_OFF                                    (0)
#define PP_CHK_MULTICAST_CNT_LEN                                    (32)
#define PP_CHK_MULTICAST_CNT_MSK                                    (0xFFFFFFFF)
#define PP_CHK_MULTICAST_CNT_RST                                    (0x0)

/**
 * SW_REG_NAME : PP_CHK_MTU_STW_DF_CNT_REG
 * HW_REG_NAME : mtu_stw_df_cnt
 * DESCRIPTION : mtu_stw_df debug counter, NOTE, B Step ONLY
 *               register
 *
 *  Register Fields :
 *   [31: 0][RW] - mtu_stw_df
 *
 */
#define PP_CHK_MTU_STW_DF_CNT_REG                      ((CHK_BASE_ADDR) + 0x154)
#define PP_CHK_MTU_STW_DF_CNT_OFF                                   (0)
#define PP_CHK_MTU_STW_DF_CNT_LEN                                   (32)
#define PP_CHK_MTU_STW_DF_CNT_MSK                                   (0xFFFFFFFF)
#define PP_CHK_MTU_STW_DF_CNT_RST                                   (0x0)

/**
 * SW_REG_NAME : PP_CHK_EXCEPTION_CNT_REG
 * HW_REG_NAME : exception_cnt
 * DESCRIPTION : exception_cnt debug counter, NOTE, B Step ONLY
 *               register
 *
 *  Register Fields :
 *   [31: 0][RW] - exception_cnt
 *
 */
#define PP_CHK_EXCEPTION_CNT_REG                       ((CHK_BASE_ADDR) + 0x158)
#define PP_CHK_EXCEPTION_CNT_OFF                                    (0)
#define PP_CHK_EXCEPTION_CNT_LEN                                    (32)
#define PP_CHK_EXCEPTION_CNT_MSK                                    (0xFFFFFFFF)
#define PP_CHK_EXCEPTION_CNT_RST                                    (0x0)

/**
 * SW_REG_NAME : PP_CHK_DROP_UC_CNT_REG
 * HW_REG_NAME : drop_uc_cnt
 * DESCRIPTION : Drop uC Counter, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][RW] - Counting events of drop when packet should be sent
 *                 to uC, yet uC timedout
 *
 */
#define PP_CHK_DROP_UC_CNT_REG                         ((CHK_BASE_ADDR) + 0x15C)
#define PP_CHK_DROP_UC_CNT_OFF                                      (0)
#define PP_CHK_DROP_UC_CNT_LEN                                      (32)
#define PP_CHK_DROP_UC_CNT_MSK                                      (0xFFFFFFFF)
#define PP_CHK_DROP_UC_CNT_RST                                      (0x0)

#endif
