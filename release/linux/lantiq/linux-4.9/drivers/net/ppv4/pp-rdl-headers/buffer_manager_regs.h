/**
 * buffer_manager_regs.h
 * Description: buffer_manager_s HW registers definitions
 * Auto-generated file by RDL Parser, DO NOT CHANGE
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2019-2020 Intel Corporation
 */

#ifndef _PP_BM_H_
#define _PP_BM_H_

#define PP_BM_GEN_DATE_STR                    "Sunday May 17, 2020 [5:26:11 pm]"
#define PP_BM_BASE                                               (0xF0080000ULL)

/**
 * SW_REG_NAME : PP_BM_BMNGR_CTRL_REG
 * HW_REG_NAME : bmngr_ctrl
 * DESCRIPTION : Main BMNG Control
 *
 *  Register Fields :
 *   [31: 4][RO] - Reserved
 *   [ 3: 3][RW] - '1' - Policy Allocation Arbiter Disable.
 *   [ 2: 2][RW] - '1' - Push Disable.
 *   [ 1: 1][RW] - '1' - Pop Disable.
 *   [ 0: 0][RW] - '1' - BMNG Client Enable.
 *
 */
#define PP_BM_BMNGR_CTRL_REG                              ((BM_BASE_ADDR) + 0x0)
#define PP_BM_BMNGR_CTRL_RSVD0_OFF                                  (4)
#define PP_BM_BMNGR_CTRL_RSVD0_LEN                                  (28)
#define PP_BM_BMNGR_CTRL_RSVD0_MSK                                  (0xFFFFFFF0)
#define PP_BM_BMNGR_CTRL_RSVD0_RST                                  (0x0)
#define PP_BM_BMNGR_CTRL_PM_ARB_DIS_OFF                             (3)
#define PP_BM_BMNGR_CTRL_PM_ARB_DIS_LEN                             (1)
#define PP_BM_BMNGR_CTRL_PM_ARB_DIS_MSK                             (0x00000008)
#define PP_BM_BMNGR_CTRL_PM_ARB_DIS_RST                             (0x0)
#define PP_BM_BMNGR_CTRL_PUSH_DIS_OFF                               (2)
#define PP_BM_BMNGR_CTRL_PUSH_DIS_LEN                               (1)
#define PP_BM_BMNGR_CTRL_PUSH_DIS_MSK                               (0x00000004)
#define PP_BM_BMNGR_CTRL_PUSH_DIS_RST                               (0x0)
#define PP_BM_BMNGR_CTRL_POP_DIS_OFF                                (1)
#define PP_BM_BMNGR_CTRL_POP_DIS_LEN                                (1)
#define PP_BM_BMNGR_CTRL_POP_DIS_MSK                                (0x00000002)
#define PP_BM_BMNGR_CTRL_POP_DIS_RST                                (0x0)
#define PP_BM_BMNGR_CTRL_BMNG_CLNT_EN_OFF                           (0)
#define PP_BM_BMNGR_CTRL_BMNG_CLNT_EN_LEN                           (1)
#define PP_BM_BMNGR_CTRL_BMNG_CLNT_EN_MSK                           (0x00000001)
#define PP_BM_BMNGR_CTRL_BMNG_CLNT_EN_RST                           (0x0)

/**
 * SW_REG_NAME : PP_BM_POOL_MIN_GRNT_MASK_REG
 * HW_REG_NAME : pool_min_grnt_mask
 * DESCRIPTION : Min Guaranteed Mask Per Pool
 *
 *  Register Fields :
 *   [31:16][RO] - Reserved
 *   [15: 0][RW] - Bitmask per pool which one should be
 *                 [br] Calculated in Minimum Guaranteee
 *
 */
#define PP_BM_POOL_MIN_GRNT_MASK_REG                      ((BM_BASE_ADDR) + 0x4)
#define PP_BM_POOL_MIN_GRNT_MASK_RSVD0_OFF                          (16)
#define PP_BM_POOL_MIN_GRNT_MASK_RSVD0_LEN                          (16)
#define PP_BM_POOL_MIN_GRNT_MASK_RSVD0_MSK                          (0xFFFF0000)
#define PP_BM_POOL_MIN_GRNT_MASK_RSVD0_RST                          (0x0)
#define PP_BM_POOL_MIN_GRNT_MASK_OFF                                (0)
#define PP_BM_POOL_MIN_GRNT_MASK_LEN                                (16)
#define PP_BM_POOL_MIN_GRNT_MASK_MSK                                (0x0000FFFF)
#define PP_BM_POOL_MIN_GRNT_MASK_RST                                (0x0)

/**
 * SW_REG_NAME : PP_BM_PCU_POOL_EN_REG
 * HW_REG_NAME : pcu_pool_enable
 * DESCRIPTION : PCU Pool Enable
 *
 *  Register Fields :
 *   [31:16][RO] - Reserved
 *   [15: 0][RW] - Prefetch Enable Bits per Pools[15:0]
 *
 */
#define PP_BM_PCU_POOL_EN_REG                             ((BM_BASE_ADDR) + 0x8)
#define PP_BM_PCU_POOL_EN_RSVD0_OFF                                 (16)
#define PP_BM_PCU_POOL_EN_RSVD0_LEN                                 (16)
#define PP_BM_PCU_POOL_EN_RSVD0_MSK                                 (0xFFFF0000)
#define PP_BM_PCU_POOL_EN_RSVD0_RST                                 (0x0)
#define PP_BM_PCU_POOL_EN_PFTCH_POOL_EN_OFF                         (0)
#define PP_BM_PCU_POOL_EN_PFTCH_POOL_EN_LEN                         (16)
#define PP_BM_PCU_POOL_EN_PFTCH_POOL_EN_MSK                         (0x0000FFFF)
#define PP_BM_PCU_POOL_EN_PFTCH_POOL_EN_RST                         (0x0)

/**
 * SW_REG_NAME : PP_BM_POOL_RESET_REG
 * HW_REG_NAME : pool_reset
 * DESCRIPTION : Pool FIFO Reset
 *
 *  Register Fields :
 *   [31:16][RO] - Reserved
 *   [15: 0][RW] - Reset FIFOs (PU/EXT) Bits[15:0] per Pools[15:0]
 *                 active high
 *
 */
#define PP_BM_POOL_RESET_REG                             ((BM_BASE_ADDR) + 0x0C)
#define PP_BM_POOL_RESET_RSVD0_OFF                                  (16)
#define PP_BM_POOL_RESET_RSVD0_LEN                                  (16)
#define PP_BM_POOL_RESET_RSVD0_MSK                                  (0xFFFF0000)
#define PP_BM_POOL_RESET_RSVD0_RST                                  (0x0)
#define PP_BM_POOL_RESET_POOL_RST_OFF                               (0)
#define PP_BM_POOL_RESET_POOL_RST_LEN                               (16)
#define PP_BM_POOL_RESET_POOL_RST_MSK                               (0x0000FFFF)
#define PP_BM_POOL_RESET_POOL_RST_RST                               (0xffff)

/**
 * SW_REG_NAME : PP_BM_OCPM_BURST_SIZE_REG
 * HW_REG_NAME : ocpm_burst_size
 * DESCRIPTION : OCP Master Burst Size
 *
 *  Register Fields :
 *   [31: 0][RW] - Burst Size on Ext. Memory access.[br] 2 bits per
 *                 pool pool[0]=bits[1:0] etc.
 *                 [br] 0 - 64B Burst
 *                 [br] 1 - 128B Burst
 *                 [br] 2 - 256B Burst
 *                 [br] 3 - 512B Burst
 *
 */
#define PP_BM_OCPM_BURST_SIZE_REG                        ((BM_BASE_ADDR) + 0x10)
#define PP_BM_OCPM_BURST_SIZE_OCPM_BRST_SIZE_OFF                    (0)
#define PP_BM_OCPM_BURST_SIZE_OCPM_BRST_SIZE_LEN                    (32)
#define PP_BM_OCPM_BURST_SIZE_OCPM_BRST_SIZE_MSK                    (0xFFFFFFFF)
#define PP_BM_OCPM_BURST_SIZE_OCPM_BRST_SIZE_RST                    (0x0)

/**
 * SW_REG_NAME : PP_BM_OCPM_NUM_OF_BURSTS_REG
 * HW_REG_NAME : ocpm_num_of_bursts
 * DESCRIPTION : OCP Master Number Of Bursts
 *
 *  Register Fields :
 *   [31: 0][RW] - Number of Bursts to Ext.Memory.[br] 2 bits per
 *                 pool pool[0]=bits[1:0] etc.
 *                 [br] 0 - 1 Burst
 *                 [br] 1 - 2 Bursts
 *                 [br] 2 - 3 Bursts
 *                 [br] 3 - 4 Bursts
 *
 */
#define PP_BM_OCPM_NUM_OF_BURSTS_REG                     ((BM_BASE_ADDR) + 0x14)
#define PP_BM_OCPM_NUM_OF_BURSTS_OCPM_NUM_OF_BRST_OFF               (0)
#define PP_BM_OCPM_NUM_OF_BURSTS_OCPM_NUM_OF_BRST_LEN               (32)
#define PP_BM_OCPM_NUM_OF_BURSTS_OCPM_NUM_OF_BRST_MSK               (0xFFFFFFFF)
#define PP_BM_OCPM_NUM_OF_BURSTS_OCPM_NUM_OF_BRST_RST               (0x0)

/**
 * SW_REG_NAME : PP_BM_STATUS_REG
 * HW_REG_NAME : status
 * DESCRIPTION : Buffer Manager Status Register
 *
 *  Register Fields :
 *   [31:16][RW] - Disabled (per) Pool Recieved Pointer from Client
 *   [15: 2][RO] - Reserved
 *   [ 1: 1][RO] - '1' - No Pending DDR Reads
 *   [ 0: 0][RO] - '1' - Initializing
 *
 */
#define PP_BM_STATUS_REG                                 ((BM_BASE_ADDR) + 0x18)
#define PP_BM_STATUS_DIS_POOL_PTR_RCV_OFF                           (16)
#define PP_BM_STATUS_DIS_POOL_PTR_RCV_LEN                           (16)
#define PP_BM_STATUS_DIS_POOL_PTR_RCV_MSK                           (0xFFFF0000)
#define PP_BM_STATUS_DIS_POOL_PTR_RCV_RST                           (0x0)
#define PP_BM_STATUS_RSVD0_OFF                                      (2)
#define PP_BM_STATUS_RSVD0_LEN                                      (14)
#define PP_BM_STATUS_RSVD0_MSK                                      (0x0000FFFC)
#define PP_BM_STATUS_RSVD0_RST                                      (0x0)
#define PP_BM_STATUS_NO_PEND_RDS_OFF                                (1)
#define PP_BM_STATUS_NO_PEND_RDS_LEN                                (1)
#define PP_BM_STATUS_NO_PEND_RDS_MSK                                (0x00000002)
#define PP_BM_STATUS_NO_PEND_RDS_RST                                (0x1)
#define PP_BM_STATUS_INIT_OFF                                       (0)
#define PP_BM_STATUS_INIT_LEN                                       (1)
#define PP_BM_STATUS_INIT_MSK                                       (0x00000001)
#define PP_BM_STATUS_INIT_RST                                       (0x0)

/**
 * SW_REG_NAME : PP_BM_POOL_POP_EN_REG
 * HW_REG_NAME : pool_pop_enable
 * DESCRIPTION : Pool Pop Enable
 *
 *  Register Fields :
 *   [31:16][RO] - Reserved
 *   [15: 0][RW] - Pool pop Enable Bits[15:0] per Pools[15:0]
 *
 */
#define PP_BM_POOL_POP_EN_REG                            ((BM_BASE_ADDR) + 0x1C)
#define PP_BM_POOL_POP_EN_RSVD0_OFF                                 (16)
#define PP_BM_POOL_POP_EN_RSVD0_LEN                                 (16)
#define PP_BM_POOL_POP_EN_RSVD0_MSK                                 (0xFFFF0000)
#define PP_BM_POOL_POP_EN_RSVD0_RST                                 (0x0)
#define PP_BM_POOL_POP_EN_OFF                                       (0)
#define PP_BM_POOL_POP_EN_LEN                                       (16)
#define PP_BM_POOL_POP_EN_MSK                                       (0x0000FFFF)
#define PP_BM_POOL_POP_EN_RST                                       (0xffff)

/**
 * SW_REG_NAME : PP_BM_POOL_SIZE_REG
 * HW_REG_NAME : pool_size
 * DESCRIPTION : The number of Buffers in this pool
 *
 *  Register Fields :
 *   [31:24][RO] - Reserved
 *   [23: 0][RW] - The number of Buffers in this pool
 *
 */
#define PP_BM_POOL_SIZE_REG                              ((BM_BASE_ADDR) + 0x20)
#define PP_BM_POOL_SIZE_RSVD0_OFF                                   (24)
#define PP_BM_POOL_SIZE_RSVD0_LEN                                   (8)
#define PP_BM_POOL_SIZE_RSVD0_MSK                                   (0xFF000000)
#define PP_BM_POOL_SIZE_RSVD0_RST                                   (0x0)
#define PP_BM_POOL_SIZE_OFF                                         (0)
#define PP_BM_POOL_SIZE_LEN                                         (24)
#define PP_BM_POOL_SIZE_MSK                                         (0x00FFFFFF)
#define PP_BM_POOL_SIZE_RST                                         (0x0)
/**
 * REG_IDX_ACCESS   : PP_BM_POOL_SIZE_REG_IDX
 * NUM OF REGISTERS : 16
 */
#define PP_BM_POOL_SIZE_REG_IDX(idx) \
	(PP_BM_POOL_SIZE_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_BM_GRP_AVL_REG
 * HW_REG_NAME : grp_avl
 * DESCRIPTION : A counter of the number of currently available
 *               buffers in this Group
 *
 *  Register Fields :
 *   [31:24][RO] - Reserved
 *   [23: 0][RW] - A counter of the number of currently available
 *                 buffers in this Group
 *
 */
#define PP_BM_GRP_AVL_REG                               ((BM_BASE_ADDR) + 0x100)
#define PP_BM_GRP_AVL_RSVD0_OFF                                     (24)
#define PP_BM_GRP_AVL_RSVD0_LEN                                     (8)
#define PP_BM_GRP_AVL_RSVD0_MSK                                     (0xFF000000)
#define PP_BM_GRP_AVL_RSVD0_RST                                     (0x0)
#define PP_BM_GRP_AVL_GRP_AVAILABLE_OFF                             (0)
#define PP_BM_GRP_AVL_GRP_AVAILABLE_LEN                             (24)
#define PP_BM_GRP_AVL_GRP_AVAILABLE_MSK                             (0x00FFFFFF)
#define PP_BM_GRP_AVL_GRP_AVAILABLE_RST                             (0x0)
/**
 * REG_IDX_ACCESS   : PP_BM_GRP_AVL_REG_IDX
 * NUM OF REGISTERS : 16
 */
#define PP_BM_GRP_AVL_REG_IDX(idx) \
	(PP_BM_GRP_AVL_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_BM_GRP_RES_REG
 * HW_REG_NAME : grp_res
 * DESCRIPTION : A counter of the number of currently reserved
 *               buffers in this Group
 *
 *  Register Fields :
 *   [31:24][RO] - Reserved
 *   [23: 0][RW] - A counter of the number of currently reserved
 *                 buffers in this Group
 *
 */
#define PP_BM_GRP_RES_REG                               ((BM_BASE_ADDR) + 0x200)
#define PP_BM_GRP_RES_RSVD0_OFF                                     (24)
#define PP_BM_GRP_RES_RSVD0_LEN                                     (8)
#define PP_BM_GRP_RES_RSVD0_MSK                                     (0xFF000000)
#define PP_BM_GRP_RES_RSVD0_RST                                     (0x0)
#define PP_BM_GRP_RES_GRP_RESERVED_OFF                              (0)
#define PP_BM_GRP_RES_GRP_RESERVED_LEN                              (24)
#define PP_BM_GRP_RES_GRP_RESERVED_MSK                              (0x00FFFFFF)
#define PP_BM_GRP_RES_GRP_RESERVED_RST                              (0x0)
/**
 * REG_IDX_ACCESS   : PP_BM_GRP_RES_REG_IDX
 * NUM OF REGISTERS : 16
 */
#define PP_BM_GRP_RES_REG_IDX(idx) \
	(PP_BM_GRP_RES_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_BM_PCU_FIFO_BASE_ADDR_REG
 * HW_REG_NAME : pcu_fifo_base_addr
 * DESCRIPTION : PCU FIFO Base Addr
 *
 *  Register Fields :
 *   [31:16][RO] - Reserved
 *   [15: 0][RW] - Pool Bases Address in SRAM
 *
 */
#define PP_BM_PCU_FIFO_BASE_ADDR_REG                    ((BM_BASE_ADDR) + 0x400)
#define PP_BM_PCU_FIFO_BASE_ADDR_RSVD0_OFF                          (16)
#define PP_BM_PCU_FIFO_BASE_ADDR_RSVD0_LEN                          (16)
#define PP_BM_PCU_FIFO_BASE_ADDR_RSVD0_MSK                          (0xFFFF0000)
#define PP_BM_PCU_FIFO_BASE_ADDR_RSVD0_RST                          (0x0)
#define PP_BM_PCU_FIFO_BASE_ADDR_OFF                                (0)
#define PP_BM_PCU_FIFO_BASE_ADDR_LEN                                (16)
#define PP_BM_PCU_FIFO_BASE_ADDR_MSK                                (0x0000FFFF)
#define PP_BM_PCU_FIFO_BASE_ADDR_RST                                (0x0)
/**
 * REG_IDX_ACCESS   : PP_BM_PCU_FIFO_BASE_ADDR_REG_IDX
 * NUM OF REGISTERS : 16
 */
#define PP_BM_PCU_FIFO_BASE_ADDR_REG_IDX(idx) \
	(PP_BM_PCU_FIFO_BASE_ADDR_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_BM_PCU_FIFO_SIZE_REG
 * HW_REG_NAME : pcu_fifo_size
 * DESCRIPTION : PCU FIFO size
 *
 *  Register Fields :
 *   [31:16][RO] - Reserved
 *   [15: 0][RW] - Pool Size in SRAM
 *
 */
#define PP_BM_PCU_FIFO_SIZE_REG                         ((BM_BASE_ADDR) + 0x440)
#define PP_BM_PCU_FIFO_SIZE_RSVD0_OFF                               (16)
#define PP_BM_PCU_FIFO_SIZE_RSVD0_LEN                               (16)
#define PP_BM_PCU_FIFO_SIZE_RSVD0_MSK                               (0xFFFF0000)
#define PP_BM_PCU_FIFO_SIZE_RSVD0_RST                               (0x0)
#define PP_BM_PCU_FIFO_SIZE_OFF                                     (0)
#define PP_BM_PCU_FIFO_SIZE_LEN                                     (16)
#define PP_BM_PCU_FIFO_SIZE_MSK                                     (0x0000FFFF)
#define PP_BM_PCU_FIFO_SIZE_RST                                     (0x100)
/**
 * REG_IDX_ACCESS   : PP_BM_PCU_FIFO_SIZE_REG_IDX
 * NUM OF REGISTERS : 16
 */
#define PP_BM_PCU_FIFO_SIZE_REG_IDX(idx) \
	(PP_BM_PCU_FIFO_SIZE_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_BM_PCU_FIFO_OCC_REG
 * HW_REG_NAME : pcu_fifo_occ
 * DESCRIPTION : PCU FIFO Occupancy
 *
 *  Register Fields :
 *   [31:16][RO] - Reserved
 *   [15: 0][RW] - The Occupancy of the FIFO in the SRAM
 *
 */
#define PP_BM_PCU_FIFO_OCC_REG                          ((BM_BASE_ADDR) + 0x480)
#define PP_BM_PCU_FIFO_OCC_RSVD0_OFF                                (16)
#define PP_BM_PCU_FIFO_OCC_RSVD0_LEN                                (16)
#define PP_BM_PCU_FIFO_OCC_RSVD0_MSK                                (0xFFFF0000)
#define PP_BM_PCU_FIFO_OCC_RSVD0_RST                                (0x0)
#define PP_BM_PCU_FIFO_OCC_OFF                                      (0)
#define PP_BM_PCU_FIFO_OCC_LEN                                      (16)
#define PP_BM_PCU_FIFO_OCC_MSK                                      (0x0000FFFF)
#define PP_BM_PCU_FIFO_OCC_RST                                      (0x0)
/**
 * REG_IDX_ACCESS   : PP_BM_PCU_FIFO_OCC_REG_IDX
 * NUM OF REGISTERS : 16
 */
#define PP_BM_PCU_FIFO_OCC_REG_IDX(idx) \
	(PP_BM_PCU_FIFO_OCC_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_BM_PCU_FIFO_PROG_EMPTY_REG
 * HW_REG_NAME : pcu_fifo_prog_empty
 * DESCRIPTION : PCU FIFO Prog Empty
 *
 *  Register Fields :
 *   [31:16][RO] - Reserved
 *   [15: 0][RW] - Programmable Empty Threshold
 *
 */
#define PP_BM_PCU_FIFO_PROG_EMPTY_REG                   ((BM_BASE_ADDR) + 0x4C0)
#define PP_BM_PCU_FIFO_PROG_EMPTY_RSVD0_OFF                         (16)
#define PP_BM_PCU_FIFO_PROG_EMPTY_RSVD0_LEN                         (16)
#define PP_BM_PCU_FIFO_PROG_EMPTY_RSVD0_MSK                         (0xFFFF0000)
#define PP_BM_PCU_FIFO_PROG_EMPTY_RSVD0_RST                         (0x0)
#define PP_BM_PCU_FIFO_PROG_EMPTY_PCU_FIFO_P_EMPTY_OFF              (0)
#define PP_BM_PCU_FIFO_PROG_EMPTY_PCU_FIFO_P_EMPTY_LEN              (16)
#define PP_BM_PCU_FIFO_PROG_EMPTY_PCU_FIFO_P_EMPTY_MSK              (0x0000FFFF)
#define PP_BM_PCU_FIFO_PROG_EMPTY_PCU_FIFO_P_EMPTY_RST              (0x55)
/**
 * REG_IDX_ACCESS   : PP_BM_PCU_FIFO_PROG_EMPTY_REG_IDX
 * NUM OF REGISTERS : 16
 */
#define PP_BM_PCU_FIFO_PROG_EMPTY_REG_IDX(idx) \
	(PP_BM_PCU_FIFO_PROG_EMPTY_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_BM_PCU_FIFO_PROG_FULL_REG
 * HW_REG_NAME : pcu_fifo_prog_full
 * DESCRIPTION : PCU FIFO Prog Full
 *
 *  Register Fields :
 *   [31:16][RO] - Reserved
 *   [15: 0][RW] - Programmable Full Threshold
 *
 */
#define PP_BM_PCU_FIFO_PROG_FULL_REG                    ((BM_BASE_ADDR) + 0x500)
#define PP_BM_PCU_FIFO_PROG_FULL_RSVD0_OFF                          (16)
#define PP_BM_PCU_FIFO_PROG_FULL_RSVD0_LEN                          (16)
#define PP_BM_PCU_FIFO_PROG_FULL_RSVD0_MSK                          (0xFFFF0000)
#define PP_BM_PCU_FIFO_PROG_FULL_RSVD0_RST                          (0x0)
#define PP_BM_PCU_FIFO_PROG_FULL_PCU_FIFO_P_FULL_OFF                (0)
#define PP_BM_PCU_FIFO_PROG_FULL_PCU_FIFO_P_FULL_LEN                (16)
#define PP_BM_PCU_FIFO_PROG_FULL_PCU_FIFO_P_FULL_MSK                (0x0000FFFF)
#define PP_BM_PCU_FIFO_PROG_FULL_PCU_FIFO_P_FULL_RST                (0xaa)
/**
 * REG_IDX_ACCESS   : PP_BM_PCU_FIFO_PROG_FULL_REG_IDX
 * NUM OF REGISTERS : 16
 */
#define PP_BM_PCU_FIFO_PROG_FULL_REG_IDX(idx) \
	(PP_BM_PCU_FIFO_PROG_FULL_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_BM_EXT_FIFO_BASE_ADDR_LOW_REG
 * HW_REG_NAME : ext_fifo_base_addr_low
 * DESCRIPTION : EXT FIFO Base Addr Low
 *
 *  Register Fields :
 *   [31: 0][RW] - Pool Bases Address in External RAM (DDR/Other)
 *
 */
#define PP_BM_EXT_FIFO_BASE_ADDR_LOW_REG                ((BM_BASE_ADDR) + 0x540)
#define PP_BM_EXT_FIFO_BASE_ADDR_LOW_OFF                            (0)
#define PP_BM_EXT_FIFO_BASE_ADDR_LOW_LEN                            (32)
#define PP_BM_EXT_FIFO_BASE_ADDR_LOW_MSK                            (0xFFFFFFFF)
#define PP_BM_EXT_FIFO_BASE_ADDR_LOW_RST                            (0x0)
/**
 * REG_IDX_ACCESS   : PP_BM_EXT_FIFO_BASE_ADDR_LOW_REG_IDX
 * NUM OF REGISTERS : 16
 */
#define PP_BM_EXT_FIFO_BASE_ADDR_LOW_REG_IDX(idx) \
	(PP_BM_EXT_FIFO_BASE_ADDR_LOW_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_BM_EXT_FIFO_BASE_ADDR_HIGH_REG
 * HW_REG_NAME : ext_fifo_base_addr_high
 * DESCRIPTION : EXT FIFO Base Addr High
 *
 *  Register Fields :
 *   [31: 8][RO] - Reserved
 *   [ 7: 0][RW] - Pool Bases Address in  External RAM (DDR/Other)
 *
 */
#define PP_BM_EXT_FIFO_BASE_ADDR_HIGH_REG               ((BM_BASE_ADDR) + 0x580)
#define PP_BM_EXT_FIFO_BASE_ADDR_HIGH_RSVD0_OFF                     (8)
#define PP_BM_EXT_FIFO_BASE_ADDR_HIGH_RSVD0_LEN                     (24)
#define PP_BM_EXT_FIFO_BASE_ADDR_HIGH_RSVD0_MSK                     (0xFFFFFF00)
#define PP_BM_EXT_FIFO_BASE_ADDR_HIGH_RSVD0_RST                     (0x0)
#define PP_BM_EXT_FIFO_BASE_ADDR_HIGH_OFF                           (0)
#define PP_BM_EXT_FIFO_BASE_ADDR_HIGH_LEN                           (8)
#define PP_BM_EXT_FIFO_BASE_ADDR_HIGH_MSK                           (0x000000FF)
#define PP_BM_EXT_FIFO_BASE_ADDR_HIGH_RST                           (0x0)
/**
 * REG_IDX_ACCESS   : PP_BM_EXT_FIFO_BASE_ADDR_HIGH_REG_IDX
 * NUM OF REGISTERS : 16
 */
#define PP_BM_EXT_FIFO_BASE_ADDR_HIGH_REG_IDX(idx) \
	(PP_BM_EXT_FIFO_BASE_ADDR_HIGH_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_BM_EXT_FIFO_OCC_REG
 * HW_REG_NAME : ext_fifo_occ
 * DESCRIPTION : EXT FIFO Occupancy
 *
 *  Register Fields :
 *   [31: 0][RW] - External FIFO Occupancy
 *
 */
#define PP_BM_EXT_FIFO_OCC_REG                          ((BM_BASE_ADDR) + 0x5C0)
#define PP_BM_EXT_FIFO_OCC_OFF                                      (0)
#define PP_BM_EXT_FIFO_OCC_LEN                                      (32)
#define PP_BM_EXT_FIFO_OCC_MSK                                      (0xFFFFFFFF)
#define PP_BM_EXT_FIFO_OCC_RST                                      (0x0)
/**
 * REG_IDX_ACCESS   : PP_BM_EXT_FIFO_OCC_REG_IDX
 * NUM OF REGISTERS : 16
 */
#define PP_BM_EXT_FIFO_OCC_REG_IDX(idx) \
	(PP_BM_EXT_FIFO_OCC_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_BM_POOL_CNT_REG
 * HW_REG_NAME : pool_cnt
 * DESCRIPTION : Pool Allocated Counter
 *
 *  Register Fields :
 *   [31:24][RO] - Reserved
 *   [23: 0][RO] - Allocated pointers for this pool
 *
 */
#define PP_BM_POOL_CNT_REG                              ((BM_BASE_ADDR) + 0x600)
#define PP_BM_POOL_CNT_RSVD0_OFF                                    (24)
#define PP_BM_POOL_CNT_RSVD0_LEN                                    (8)
#define PP_BM_POOL_CNT_RSVD0_MSK                                    (0xFF000000)
#define PP_BM_POOL_CNT_RSVD0_RST                                    (0x0)
#define PP_BM_POOL_CNT_POOL_ALLOC_CNT_OFF                           (0)
#define PP_BM_POOL_CNT_POOL_ALLOC_CNT_LEN                           (24)
#define PP_BM_POOL_CNT_POOL_ALLOC_CNT_MSK                           (0x00FFFFFF)
#define PP_BM_POOL_CNT_POOL_ALLOC_CNT_RST                           (0x0)
/**
 * REG_IDX_ACCESS   : PP_BM_POOL_CNT_REG_IDX
 * NUM OF REGISTERS : 16
 */
#define PP_BM_POOL_CNT_REG_IDX(idx) \
	(PP_BM_POOL_CNT_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_BM_POP_CNT_REG
 * HW_REG_NAME : pop_cnt
 * DESCRIPTION : Pool Pop Counter
 *
 *  Register Fields :
 *   [31:24][RO] - Reserved
 *   [23: 0][RO] - Number of returned POPs for this pool
 *
 */
#define PP_BM_POP_CNT_REG                               ((BM_BASE_ADDR) + 0x640)
#define PP_BM_POP_CNT_RSVD0_OFF                                     (24)
#define PP_BM_POP_CNT_RSVD0_LEN                                     (8)
#define PP_BM_POP_CNT_RSVD0_MSK                                     (0xFF000000)
#define PP_BM_POP_CNT_RSVD0_RST                                     (0x0)
#define PP_BM_POP_CNT_OFF                                           (0)
#define PP_BM_POP_CNT_LEN                                           (24)
#define PP_BM_POP_CNT_MSK                                           (0x00FFFFFF)
#define PP_BM_POP_CNT_RST                                           (0x0)
/**
 * REG_IDX_ACCESS   : PP_BM_POP_CNT_REG_IDX
 * NUM OF REGISTERS : 16
 */
#define PP_BM_POP_CNT_REG_IDX(idx) \
	(PP_BM_POP_CNT_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_BM_PUSH_CNT_REG
 * HW_REG_NAME : push_cnt
 * DESCRIPTION : Pool Push Counter
 *
 *  Register Fields :
 *   [31:24][RO] - Reserved
 *   [23: 0][RO] - Number of returned PUSHs for this pool
 *
 */
#define PP_BM_PUSH_CNT_REG                              ((BM_BASE_ADDR) + 0x680)
#define PP_BM_PUSH_CNT_RSVD0_OFF                                    (24)
#define PP_BM_PUSH_CNT_RSVD0_LEN                                    (8)
#define PP_BM_PUSH_CNT_RSVD0_MSK                                    (0xFF000000)
#define PP_BM_PUSH_CNT_RSVD0_RST                                    (0x0)
#define PP_BM_PUSH_CNT_OFF                                          (0)
#define PP_BM_PUSH_CNT_LEN                                          (24)
#define PP_BM_PUSH_CNT_MSK                                          (0x00FFFFFF)
#define PP_BM_PUSH_CNT_RST                                          (0x0)
/**
 * REG_IDX_ACCESS   : PP_BM_PUSH_CNT_REG_IDX
 * NUM OF REGISTERS : 16
 */
#define PP_BM_PUSH_CNT_REG_IDX(idx) \
	(PP_BM_PUSH_CNT_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_BM_BURST_WR_CNT_REG
 * HW_REG_NAME : burst_wr_cnt
 * DESCRIPTION : DDR Burst Write Counter
 *
 *  Register Fields :
 *   [31: 0][RO] - Number of DDR Write bursts
 *
 */
#define PP_BM_BURST_WR_CNT_REG                          ((BM_BASE_ADDR) + 0x6C0)
#define PP_BM_BURST_WR_CNT_DDR_BRST_WR_CNT_OFF                      (0)
#define PP_BM_BURST_WR_CNT_DDR_BRST_WR_CNT_LEN                      (32)
#define PP_BM_BURST_WR_CNT_DDR_BRST_WR_CNT_MSK                      (0xFFFFFFFF)
#define PP_BM_BURST_WR_CNT_DDR_BRST_WR_CNT_RST                      (0x0)
/**
 * REG_IDX_ACCESS   : PP_BM_BURST_WR_CNT_REG_IDX
 * NUM OF REGISTERS : 16
 */
#define PP_BM_BURST_WR_CNT_REG_IDX(idx) \
	(PP_BM_BURST_WR_CNT_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_BM_BURST_RD_CNT_REG
 * HW_REG_NAME : burst_rd_cnt
 * DESCRIPTION : DDR Burst Read Counter
 *
 *  Register Fields :
 *   [31: 0][RO] - Number of DDR Read bursts
 *
 */
#define PP_BM_BURST_RD_CNT_REG                          ((BM_BASE_ADDR) + 0x700)
#define PP_BM_BURST_RD_CNT_DDR_BRST_RD_CNT_OFF                      (0)
#define PP_BM_BURST_RD_CNT_DDR_BRST_RD_CNT_LEN                      (32)
#define PP_BM_BURST_RD_CNT_DDR_BRST_RD_CNT_MSK                      (0xFFFFFFFF)
#define PP_BM_BURST_RD_CNT_DDR_BRST_RD_CNT_RST                      (0x0)
/**
 * REG_IDX_ACCESS   : PP_BM_BURST_RD_CNT_REG_IDX
 * NUM OF REGISTERS : 16
 */
#define PP_BM_BURST_RD_CNT_REG_IDX(idx) \
	(PP_BM_BURST_RD_CNT_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_BM_WATERMARK_LOW_THRSH_REG
 * HW_REG_NAME : watermark_low_thrsh
 * DESCRIPTION : WaterMark Low Threshold
 *
 *  Register Fields :
 *   [31: 0][RW] - WaterMark Low Threshold
 *
 */
#define PP_BM_WATERMARK_LOW_THRSH_REG                   ((BM_BASE_ADDR) + 0x740)
#define PP_BM_WATERMARK_LOW_THRSH_WM_LOW_TRSH_OFF                   (0)
#define PP_BM_WATERMARK_LOW_THRSH_WM_LOW_TRSH_LEN                   (32)
#define PP_BM_WATERMARK_LOW_THRSH_WM_LOW_TRSH_MSK                   (0xFFFFFFFF)
#define PP_BM_WATERMARK_LOW_THRSH_WM_LOW_TRSH_RST                   (0x0)
/**
 * REG_IDX_ACCESS   : PP_BM_WATERMARK_LOW_THRSH_REG_IDX
 * NUM OF REGISTERS : 16
 */
#define PP_BM_WATERMARK_LOW_THRSH_REG_IDX(idx) \
	(PP_BM_WATERMARK_LOW_THRSH_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_BM_WATERMARK_LOW_CNT_REG
 * HW_REG_NAME : watermark_low_cnt
 * DESCRIPTION : WaterMark Low Counter
 *
 *  Register Fields :
 *   [31: 0][RO] - WaterMark Low Counter
 *
 */
#define PP_BM_WATERMARK_LOW_CNT_REG                     ((BM_BASE_ADDR) + 0x780)
#define PP_BM_WATERMARK_LOW_CNT_WM_LOW_CNT_OFF                      (0)
#define PP_BM_WATERMARK_LOW_CNT_WM_LOW_CNT_LEN                      (32)
#define PP_BM_WATERMARK_LOW_CNT_WM_LOW_CNT_MSK                      (0xFFFFFFFF)
#define PP_BM_WATERMARK_LOW_CNT_WM_LOW_CNT_RST                      (0x0)
/**
 * REG_IDX_ACCESS   : PP_BM_WATERMARK_LOW_CNT_REG_IDX
 * NUM OF REGISTERS : 16
 */
#define PP_BM_WATERMARK_LOW_CNT_REG_IDX(idx) \
	(PP_BM_WATERMARK_LOW_CNT_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_BM_NULL_CNT_REG
 * HW_REG_NAME : null_cnt
 * DESCRIPTION : Policy Null Counter
 *
 *  Register Fields :
 *   [31:24][RO] - Reserved
 *   [23: 0][RO] - Number of returned NULLs for this Policy
 *
 */
#define PP_BM_NULL_CNT_REG                              ((BM_BASE_ADDR) + 0x800)
#define PP_BM_NULL_CNT_RSVD0_OFF                                    (24)
#define PP_BM_NULL_CNT_RSVD0_LEN                                    (8)
#define PP_BM_NULL_CNT_RSVD0_MSK                                    (0xFF000000)
#define PP_BM_NULL_CNT_RSVD0_RST                                    (0x0)
#define PP_BM_NULL_CNT_OFF                                          (0)
#define PP_BM_NULL_CNT_LEN                                          (24)
#define PP_BM_NULL_CNT_MSK                                          (0x00FFFFFF)
#define PP_BM_NULL_CNT_RST                                          (0x0)
/**
 * REG_IDX_ACCESS   : PP_BM_NULL_CNT_REG_IDX
 * NUM OF REGISTERS : 128
 */
#define PP_BM_NULL_CNT_REG_IDX(idx) \
	(PP_BM_NULL_CNT_REG + ((idx) << 2))

#endif
