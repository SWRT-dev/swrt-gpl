/**
 * modifier_regs.h
 * Description: modifier_s HW registers definitions
 * Auto-generated file by RDL Parser, DO NOT CHANGE
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2019-2020 Intel Corporation
 */

#ifndef _PP_MOD_H_
#define _PP_MOD_H_

#define PP_MOD_GEN_DATE_STR                   "Sunday May 17, 2020 [5:26:11 pm]"
#define PP_MOD_BASE                                              (0xF2000000ULL)

/**
 * SW_REG_NAME : PP_MOD_CTRL_REG
 * HW_REG_NAME : modifier_ctrl
 * DESCRIPTION : Modifier Control
 *
 *  Register Fields :
 *   [31: 9][RO] - Reserved
 *   [ 8: 8][RW] - Set to 1 to enable auto-lock on first stw error.
 *                 Write zeros to STW reigster to release, NOTE, B
 *                 Step ONLY field
 *   [ 7: 7][RW] - Set to 1 to lock the STW Status register for read.
 *                 Unset to continue recording.
 *   [ 6: 6][RW] - Client SCE Endianity.[br]0 - no swap[br]1 - swap
 *   [ 5: 5][RW] - Client BCE Endianity.[br]0 - no swap[br]1 - swap
 *   [ 4: 4][RW] - Client Endianity.[br]0 - swap[br]1 - no swap
 *   [ 3: 3][RW] - JOBM Client Arbitration scheme [br]0 - Strict
 *                 [br]1 - Round Robin
 *                 [br] Arbitrating between Classifier and uC.
 *   [ 2: 2][RW] - JOBM DPU Distribution scheme [br]0 - Strict [br]1
 *                 - Round Robin
 *                 [br]The distribution of jobs for DPU0/1.
 *   [ 1: 0][RW] - Modifier Client Enable.[br]Classifier -
 *                 bit[0][br]uC - bit[1]
 *
 */
#define PP_MOD_CTRL_REG                                  ((MOD_BASE_ADDR) + 0x0)
#define PP_MOD_CTRL_RSVD0_OFF                                       (9)
#define PP_MOD_CTRL_RSVD0_LEN                                       (23)
#define PP_MOD_CTRL_RSVD0_MSK                                       (0xFFFFFE00)
#define PP_MOD_CTRL_RSVD0_RST                                       (0x0)
#define PP_MOD_CTRL_LOCK_STW_ERR_STATUS_OFF                         (8)
#define PP_MOD_CTRL_LOCK_STW_ERR_STATUS_LEN                         (1)
#define PP_MOD_CTRL_LOCK_STW_ERR_STATUS_MSK                         (0x00000100)
#define PP_MOD_CTRL_LOCK_STW_ERR_STATUS_RST                         (0x0)
#define PP_MOD_CTRL_LOCK_STW_STATUS_OFF                             (7)
#define PP_MOD_CTRL_LOCK_STW_STATUS_LEN                             (1)
#define PP_MOD_CTRL_LOCK_STW_STATUS_MSK                             (0x00000080)
#define PP_MOD_CTRL_LOCK_STW_STATUS_RST                             (0x0)
#define PP_MOD_CTRL_JOBM_SCE_ENDIAN_OFF                             (6)
#define PP_MOD_CTRL_JOBM_SCE_ENDIAN_LEN                             (1)
#define PP_MOD_CTRL_JOBM_SCE_ENDIAN_MSK                             (0x00000040)
#define PP_MOD_CTRL_JOBM_SCE_ENDIAN_RST                             (0x1)
#define PP_MOD_CTRL_JOBM_BCE_ENDIAN_OFF                             (5)
#define PP_MOD_CTRL_JOBM_BCE_ENDIAN_LEN                             (1)
#define PP_MOD_CTRL_JOBM_BCE_ENDIAN_MSK                             (0x00000020)
#define PP_MOD_CTRL_JOBM_BCE_ENDIAN_RST                             (0x1)
#define PP_MOD_CTRL_CLNT_ENDIAN_OFF                                 (4)
#define PP_MOD_CTRL_CLNT_ENDIAN_LEN                                 (1)
#define PP_MOD_CTRL_CLNT_ENDIAN_MSK                                 (0x00000010)
#define PP_MOD_CTRL_CLNT_ENDIAN_RST                                 (0x0)
#define PP_MOD_CTRL_JM_CLNT_ARB_SCHEME_OFF                          (3)
#define PP_MOD_CTRL_JM_CLNT_ARB_SCHEME_LEN                          (1)
#define PP_MOD_CTRL_JM_CLNT_ARB_SCHEME_MSK                          (0x00000008)
#define PP_MOD_CTRL_JM_CLNT_ARB_SCHEME_RST                          (0x1)
#define PP_MOD_CTRL_JM_DPU_ARB_SCHEME_OFF                           (2)
#define PP_MOD_CTRL_JM_DPU_ARB_SCHEME_LEN                           (1)
#define PP_MOD_CTRL_JM_DPU_ARB_SCHEME_MSK                           (0x00000004)
#define PP_MOD_CTRL_JM_DPU_ARB_SCHEME_RST                           (0x1)
#define PP_MOD_CTRL_CLIENT_EN_OFF                                   (0)
#define PP_MOD_CTRL_CLIENT_EN_LEN                                   (2)
#define PP_MOD_CTRL_CLIENT_EN_MSK                                   (0x00000003)
#define PP_MOD_CTRL_CLIENT_EN_RST                                   (0x0)

/**
 * SW_REG_NAME : PP_MOD_DPU0_EU_CFG_REG
 * HW_REG_NAME : dpu0_eu_cfg
 * DESCRIPTION : EU Control
 *
 *  Register Fields :
 *   [31:16][RO] - Reserved
 *   [15: 0][RW] - [15:1] - Reserved[br][0] - OOR CMD Flush Enable
 *
 */
#define PP_MOD_DPU0_EU_CFG_REG                           ((MOD_BASE_ADDR) + 0x4)
#define PP_MOD_DPU0_EU_CFG_RSVD0_OFF                                (16)
#define PP_MOD_DPU0_EU_CFG_RSVD0_LEN                                (16)
#define PP_MOD_DPU0_EU_CFG_RSVD0_MSK                                (0xFFFF0000)
#define PP_MOD_DPU0_EU_CFG_RSVD0_RST                                (0x0)
#define PP_MOD_DPU0_EU_CFG_OFF                                      (0)
#define PP_MOD_DPU0_EU_CFG_LEN                                      (16)
#define PP_MOD_DPU0_EU_CFG_MSK                                      (0x0000FFFF)
#define PP_MOD_DPU0_EU_CFG_RST                                      (0x0)
/**
 * REG_IDX_ACCESS   : PP_MOD_DPU0_EU_CFG_REG_IDX
 * NUM OF REGISTERS : 9
 */
#define PP_MOD_DPU0_EU_CFG_REG_IDX(idx) \
	(PP_MOD_DPU0_EU_CFG_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_MOD_DPU1_EU_CFG_REG
 * HW_REG_NAME : dpu1_eu_cfg
 * DESCRIPTION : EU Control
 *
 *  Register Fields :
 *   [31:16][RO] - Reserved
 *   [15: 0][RW] - [15:1] - Reserved[br][0] - OOR CMD Flush Enable
 *
 */
#define PP_MOD_DPU1_EU_CFG_REG                          ((MOD_BASE_ADDR) + 0x28)
#define PP_MOD_DPU1_EU_CFG_RSVD0_OFF                                (16)
#define PP_MOD_DPU1_EU_CFG_RSVD0_LEN                                (16)
#define PP_MOD_DPU1_EU_CFG_RSVD0_MSK                                (0xFFFF0000)
#define PP_MOD_DPU1_EU_CFG_RSVD0_RST                                (0x0)
#define PP_MOD_DPU1_EU_CFG_OFF                                      (0)
#define PP_MOD_DPU1_EU_CFG_LEN                                      (16)
#define PP_MOD_DPU1_EU_CFG_MSK                                      (0x0000FFFF)
#define PP_MOD_DPU1_EU_CFG_RST                                      (0x0)
/**
 * REG_IDX_ACCESS   : PP_MOD_DPU1_EU_CFG_REG_IDX
 * NUM OF REGISTERS : 9
 */
#define PP_MOD_DPU1_EU_CFG_REG_IDX(idx) \
	(PP_MOD_DPU1_EU_CFG_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_MOD_STATUS_REG
 * HW_REG_NAME : mod_status
 * DESCRIPTION : Modifier Status
 *
 *  Register Fields :
 *   [31:16][RO] - Reserved
 *   [15:15][RW] - Egress DPU1 Overflow
 *   [14:14][RW] - Egress DPU0 Overflow
 *   [13:13][RW] - Match Internal Err
 *   [12:12][RW] - Meta FIFO Error
 *   [11: 9][RO] - Reserved
 *   [ 8: 8][RW] - rpb_dma1 sync loss
 *   [ 7: 5][RO] - Reserved
 *   [ 4: 4][RW] - rpb_dma0 sync loss
 *   [ 3: 0][RW] - bit[0] High then JM is in INIT
 *
 */
#define PP_MOD_STATUS_REG                              ((MOD_BASE_ADDR) + 0x200)
#define PP_MOD_STATUS_RSVD0_OFF                                     (16)
#define PP_MOD_STATUS_RSVD0_LEN                                     (16)
#define PP_MOD_STATUS_RSVD0_MSK                                     (0xFFFF0000)
#define PP_MOD_STATUS_RSVD0_RST                                     (0x0)
#define PP_MOD_STATUS_EGRESS_DPU1_BUFF_OVF_OFF                      (15)
#define PP_MOD_STATUS_EGRESS_DPU1_BUFF_OVF_LEN                      (1)
#define PP_MOD_STATUS_EGRESS_DPU1_BUFF_OVF_MSK                      (0x00008000)
#define PP_MOD_STATUS_EGRESS_DPU1_BUFF_OVF_RST                      (0x0)
#define PP_MOD_STATUS_EGRESS_DPU0_BUFF_OVF_OFF                      (14)
#define PP_MOD_STATUS_EGRESS_DPU0_BUFF_OVF_LEN                      (1)
#define PP_MOD_STATUS_EGRESS_DPU0_BUFF_OVF_MSK                      (0x00004000)
#define PP_MOD_STATUS_EGRESS_DPU0_BUFF_OVF_RST                      (0x0)
#define PP_MOD_STATUS_MATCH_ERROR_OFF                               (13)
#define PP_MOD_STATUS_MATCH_ERROR_LEN                               (1)
#define PP_MOD_STATUS_MATCH_ERROR_MSK                               (0x00002000)
#define PP_MOD_STATUS_MATCH_ERROR_RST                               (0x0)
#define PP_MOD_STATUS_META_FIFO_ERR_OFF                             (12)
#define PP_MOD_STATUS_META_FIFO_ERR_LEN                             (1)
#define PP_MOD_STATUS_META_FIFO_ERR_MSK                             (0x00001000)
#define PP_MOD_STATUS_META_FIFO_ERR_RST                             (0x0)
#define PP_MOD_STATUS_RSVD1_OFF                                     (9)
#define PP_MOD_STATUS_RSVD1_LEN                                     (3)
#define PP_MOD_STATUS_RSVD1_MSK                                     (0x00000E00)
#define PP_MOD_STATUS_RSVD1_RST                                     (0x0)
#define PP_MOD_STATUS_RPB_DMA1_SYNC_LOSS_OFF                        (8)
#define PP_MOD_STATUS_RPB_DMA1_SYNC_LOSS_LEN                        (1)
#define PP_MOD_STATUS_RPB_DMA1_SYNC_LOSS_MSK                        (0x00000100)
#define PP_MOD_STATUS_RPB_DMA1_SYNC_LOSS_RST                        (0x0)
#define PP_MOD_STATUS_RSVD2_OFF                                     (5)
#define PP_MOD_STATUS_RSVD2_LEN                                     (3)
#define PP_MOD_STATUS_RSVD2_MSK                                     (0x000000E0)
#define PP_MOD_STATUS_RSVD2_RST                                     (0x0)
#define PP_MOD_STATUS_RPB_DMA0_SYNC_LOSS_OFF                        (4)
#define PP_MOD_STATUS_RPB_DMA0_SYNC_LOSS_LEN                        (1)
#define PP_MOD_STATUS_RPB_DMA0_SYNC_LOSS_MSK                        (0x00000010)
#define PP_MOD_STATUS_RPB_DMA0_SYNC_LOSS_RST                        (0x0)
#define PP_MOD_STATUS_JOBM_STATUS_OFF                               (0)
#define PP_MOD_STATUS_JOBM_STATUS_LEN                               (4)
#define PP_MOD_STATUS_JOBM_STATUS_MSK                               (0x0000000F)
#define PP_MOD_STATUS_JOBM_STATUS_RST                               (0x1)

/**
 * SW_REG_NAME : PP_MOD_DPU0_EU_STATUS_REG
 * HW_REG_NAME : dpu0_eu_status
 * DESCRIPTION : EU Status
 *
 *  Register Fields :
 *   [31:16][RO] - Reserved
 *   [15: 0][RW] - [9] Outputs Valid         [br] [8] EU Outputs
 *                 Valid         [br] [7] CMD CTRL Flush         [br]
 *                 [6] CMD CTRL Busy         [br] [5] CMD CTRL Input
 *                 Ready         [br] [4] CMD CTRL Output Valid
 *                 [br] [3] CMD CTRL CMD FIFO Over         [br] [2]
 *                 CMD CTRL CMD FIFO Under         [br] [1] CMD CTRL
 *                 Recipe Size Error         [br] [0] CMD CTRL Stream
 *                 Pointer Error
 *
 */
#define PP_MOD_DPU0_EU_STATUS_REG                      ((MOD_BASE_ADDR) + 0x204)
#define PP_MOD_DPU0_EU_STATUS_RSVD0_OFF                             (16)
#define PP_MOD_DPU0_EU_STATUS_RSVD0_LEN                             (16)
#define PP_MOD_DPU0_EU_STATUS_RSVD0_MSK                             (0xFFFF0000)
#define PP_MOD_DPU0_EU_STATUS_RSVD0_RST                             (0x0)
#define PP_MOD_DPU0_EU_STATUS_OFF                                   (0)
#define PP_MOD_DPU0_EU_STATUS_LEN                                   (16)
#define PP_MOD_DPU0_EU_STATUS_MSK                                   (0x0000FFFF)
#define PP_MOD_DPU0_EU_STATUS_RST                                   (0x0)
/**
 * REG_IDX_ACCESS   : PP_MOD_DPU0_EU_STATUS_REG_IDX
 * NUM OF REGISTERS : 9
 */
#define PP_MOD_DPU0_EU_STATUS_REG_IDX(idx) \
	(PP_MOD_DPU0_EU_STATUS_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_MOD_DPU1_EU_STATUS_REG
 * HW_REG_NAME : dpu1_eu_status
 * DESCRIPTION : EU Status
 *
 *  Register Fields :
 *   [31:16][RO] - Reserved
 *   [15: 0][RW] - [9] Outputs Valid         [br] [8] EU Outputs
 *                 Valid         [br] [7] CMD CTRL Flush         [br]
 *                 [6] CMD CTRL Busy         [br] [5] CMD CTRL Input
 *                 Ready         [br] [4] CMD CTRL Output Valid
 *                 [br] [3] CMD CTRL CMD FIFO Over         [br] [2]
 *                 CMD CTRL CMD FIFO Under         [br] [1] CMD CTRL
 *                 Recipe Size Error         [br] [0] CMD CTRL Stream
 *                 Pointer Error
 *
 */
#define PP_MOD_DPU1_EU_STATUS_REG                      ((MOD_BASE_ADDR) + 0x228)
#define PP_MOD_DPU1_EU_STATUS_RSVD0_OFF                             (16)
#define PP_MOD_DPU1_EU_STATUS_RSVD0_LEN                             (16)
#define PP_MOD_DPU1_EU_STATUS_RSVD0_MSK                             (0xFFFF0000)
#define PP_MOD_DPU1_EU_STATUS_RSVD0_RST                             (0x0)
#define PP_MOD_DPU1_EU_STATUS_OFF                                   (0)
#define PP_MOD_DPU1_EU_STATUS_LEN                                   (16)
#define PP_MOD_DPU1_EU_STATUS_MSK                                   (0x0000FFFF)
#define PP_MOD_DPU1_EU_STATUS_RST                                   (0x0)
/**
 * REG_IDX_ACCESS   : PP_MOD_DPU1_EU_STATUS_REG_IDX
 * NUM OF REGISTERS : 9
 */
#define PP_MOD_DPU1_EU_STATUS_REG_IDX(idx) \
	(PP_MOD_DPU1_EU_STATUS_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_MOD_STW_STATUS_REG
 * HW_REG_NAME : stw_status
 * DESCRIPTION : Status Word
 *
 *  Register Fields :
 *   [31: 0][RW] - Captures Status Word
 *
 */
#define PP_MOD_STW_STATUS_REG                          ((MOD_BASE_ADDR) + 0x24C)
#define PP_MOD_STW_STATUS_OFF                                       (0)
#define PP_MOD_STW_STATUS_LEN                                       (32)
#define PP_MOD_STW_STATUS_MSK                                       (0xFFFFFFFF)
#define PP_MOD_STW_STATUS_RST                                       (0x0)
/**
 * REG_IDX_ACCESS   : PP_MOD_STW_STATUS_REG_IDX
 * NUM OF REGISTERS : 10
 */
#define PP_MOD_STW_STATUS_REG_IDX(idx) \
	(PP_MOD_STW_STATUS_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_MOD_RXDMA_STALL_STATUS_CNT_WATERMARK_REG
 * HW_REG_NAME : rxdma_stall_status_counter_watermark
 * DESCRIPTION : Caputres longest stall from RXDMA
 *
 *  Register Fields :
 *   [31: 0][RW] - Caputres longest stall from RXDMA saturated. Value
 *                 divided by 8.
 *
 */
#define PP_MOD_RXDMA_STALL_STATUS_CNT_WATERMARK_REG    ((MOD_BASE_ADDR) + 0x274)
#define PP_MOD_RXDMA_STALL_STATUS_CNT_WATERMARK_OFF                 (0)
#define PP_MOD_RXDMA_STALL_STATUS_CNT_WATERMARK_LEN                 (32)
#define PP_MOD_RXDMA_STALL_STATUS_CNT_WATERMARK_MSK                 (0xFFFFFFFF)
#define PP_MOD_RXDMA_STALL_STATUS_CNT_WATERMARK_RST                 (0x0)

/**
 * SW_REG_NAME : PP_MOD_CHK_PKT_CNT_REG
 * HW_REG_NAME : chk_pkt_cnt
 * DESCRIPTION : Counts Accepted Packets from Checker
 *
 *  Register Fields :
 *   [31: 0][RW] - Counts Accepted Packets from Checker
 *
 */
#define PP_MOD_CHK_PKT_CNT_REG                         ((MOD_BASE_ADDR) + 0x278)
#define PP_MOD_CHK_PKT_CNT_OFF                                      (0)
#define PP_MOD_CHK_PKT_CNT_LEN                                      (32)
#define PP_MOD_CHK_PKT_CNT_MSK                                      (0xFFFFFFFF)
#define PP_MOD_CHK_PKT_CNT_RST                                      (0x0)

/**
 * SW_REG_NAME : PP_MOD_UC_PKT_CNT_REG
 * HW_REG_NAME : uc_pkt_cnt
 * DESCRIPTION : Counts Accepted Packets from uC
 *
 *  Register Fields :
 *   [31: 0][RW] - Counts Accepted Packets from uC
 *
 */
#define PP_MOD_UC_PKT_CNT_REG                          ((MOD_BASE_ADDR) + 0x27C)
#define PP_MOD_UC_PKT_CNT_OFF                                       (0)
#define PP_MOD_UC_PKT_CNT_LEN                                       (32)
#define PP_MOD_UC_PKT_CNT_MSK                                       (0xFFFFFFFF)
#define PP_MOD_UC_PKT_CNT_RST                                       (0x0)

/**
 * SW_REG_NAME : PP_MOD_RECIPE_CNT_REG
 * HW_REG_NAME : recipe_cnt
 * DESCRIPTION : Recipe Counter
 *
 *  Register Fields :
 *   [31: 0][RW] - Counter per recipe how many packets accesses thsi
 *                 recipe
 *
 */
#define PP_MOD_RECIPE_CNT_REG                          ((MOD_BASE_ADDR) + 0x280)
#define PP_MOD_RECIPE_CNT_OFF                                       (0)
#define PP_MOD_RECIPE_CNT_LEN                                       (32)
#define PP_MOD_RECIPE_CNT_MSK                                       (0xFFFFFFFF)
#define PP_MOD_RECIPE_CNT_RST                                       (0x0)
/**
 * REG_IDX_ACCESS   : PP_MOD_RECIPE_CNT_REG_IDX
 * NUM OF REGISTERS : 128
 */
#define PP_MOD_RECIPE_CNT_REG_IDX(idx) \
	(PP_MOD_RECIPE_CNT_REG + ((idx) << 2))

#endif
