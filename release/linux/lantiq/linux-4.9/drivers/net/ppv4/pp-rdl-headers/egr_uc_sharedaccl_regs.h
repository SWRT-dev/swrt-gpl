/**
 * egr_uc_sharedaccl_regs.h
 * Description: egr_uc_sharedaccl_s HW registers definitions
 *              NOTE: B Step ONLY module
 * Auto-generated file by RDL Parser, DO NOT CHANGE
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2019-2020 Intel Corporation
 */

#ifndef _PP_UC_EGR_SHACC_H_
#define _PP_UC_EGR_SHACC_H_

#define PP_UC_EGR_SHACC_GEN_DATE_STR          "Sunday May 17, 2020 [5:26:11 pm]"
#define PP_UC_EGR_SHACC_BASE                                     (0xF0700000ULL)

/**
 * SW_REG_NAME : PP_UC_EGR_SHACC_CCU_ISR_REG
 * HW_REG_NAME : reg_ccu_isr
 * DESCRIPTION : ISR, Interrupt Status Register, NOTE, B Step ONLY
 *               register
 *
 *  Register Fields :
 *   [31: 0][RO] - ISR, Interrupt Status Register
 *
 */
#define PP_UC_EGR_SHACC_CCU_ISR_REG             ((UC_EGR_SHACC_BASE_ADDR) + 0x0)
#define PP_UC_EGR_SHACC_CCU_ISR_CCU_ISR_OFF                         (0)
#define PP_UC_EGR_SHACC_CCU_ISR_CCU_ISR_LEN                         (32)
#define PP_UC_EGR_SHACC_CCU_ISR_CCU_ISR_MSK                         (0xFFFFFFFF)
#define PP_UC_EGR_SHACC_CCU_ISR_CCU_ISR_RST                         (0x0)

/**
 * SW_REG_NAME : PP_UC_EGR_SHACC_CCU_IER_REG
 * HW_REG_NAME : reg_ccu_ier
 * DESCRIPTION : ISR, Interrupt Enable Register, NOTE, B Step ONLY
 *               register
 *
 *  Register Fields :
 *   [31: 0][RW] - ISR, Interrupt Enable Register
 *
 */
#define PP_UC_EGR_SHACC_CCU_IER_REG           ((UC_EGR_SHACC_BASE_ADDR) + 0x400)
#define PP_UC_EGR_SHACC_CCU_IER_CCU_IER_OFF                         (0)
#define PP_UC_EGR_SHACC_CCU_IER_CCU_IER_LEN                         (32)
#define PP_UC_EGR_SHACC_CCU_IER_CCU_IER_MSK                         (0xFFFFFFFF)
#define PP_UC_EGR_SHACC_CCU_IER_CCU_IER_RST                         (0x0)

/**
 * SW_REG_NAME : PP_UC_EGR_SHACC_CCU_IRR_REG
 * HW_REG_NAME : reg_ccu_irr
 * DESCRIPTION : ISR, Interrupt Request Register, NOTE, B Step ONLY
 *               register
 *
 *  Register Fields :
 *   [31: 0][RO] - ISR, Interrupt Request Register
 *
 */
#define PP_UC_EGR_SHACC_CCU_IRR_REG           ((UC_EGR_SHACC_BASE_ADDR) + 0x800)
#define PP_UC_EGR_SHACC_CCU_IRR_CCU_IRR_OFF                         (0)
#define PP_UC_EGR_SHACC_CCU_IRR_CCU_IRR_LEN                         (32)
#define PP_UC_EGR_SHACC_CCU_IRR_CCU_IRR_MSK                         (0xFFFFFFFF)
#define PP_UC_EGR_SHACC_CCU_IRR_CCU_IRR_RST                         (0x0)

/**
 * SW_REG_NAME : PP_UC_EGR_SHACC_CCU_UC_RESET_REG
 * HW_REG_NAME : reg_ccu_uc_reset
 * DESCRIPTION : UC Reset; Active low, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 1][RO] - Reserved
 *   [ 0: 0][RW] - UC Reset; Active low
 *
 */
#define PP_UC_EGR_SHACC_CCU_UC_RESET_REG     ((UC_EGR_SHACC_BASE_ADDR) + 0x0C00)
#define PP_UC_EGR_SHACC_CCU_UC_RESET_RSVD0_OFF                      (1)
#define PP_UC_EGR_SHACC_CCU_UC_RESET_RSVD0_LEN                      (31)
#define PP_UC_EGR_SHACC_CCU_UC_RESET_RSVD0_MSK                      (0xFFFFFFFE)
#define PP_UC_EGR_SHACC_CCU_UC_RESET_RSVD0_RST                      (0x0)
#define PP_UC_EGR_SHACC_CCU_UC_RESET_CCU_UC_RESET_OFF               (0)
#define PP_UC_EGR_SHACC_CCU_UC_RESET_CCU_UC_RESET_LEN               (1)
#define PP_UC_EGR_SHACC_CCU_UC_RESET_CCU_UC_RESET_MSK               (0x00000001)
#define PP_UC_EGR_SHACC_CCU_UC_RESET_CCU_UC_RESET_RST               (0x0)
/**
 * REG_IDX_ACCESS   : PP_UC_EGR_SHACC_CCU_UC_RESET_REG_IDX
 * NUM OF REGISTERS : 4
 */
#define PP_UC_EGR_SHACC_CCU_UC_RESET_REG_IDX(idx) \
	(PP_UC_EGR_SHACC_CCU_UC_RESET_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_UC_EGR_SHACC_CCU_UC_CLK_EN_REG
 * HW_REG_NAME : reg_ccu_uc_clk_en
 * DESCRIPTION : UC Clock Enable; Active High, NOTE, B Step ONLY
 *               register
 *
 *  Register Fields :
 *   [31: 1][RO] - Reserved
 *   [ 0: 0][RW] - UC Clock Enable; Active High
 *
 */
#define PP_UC_EGR_SHACC_CCU_UC_CLK_EN_REG    ((UC_EGR_SHACC_BASE_ADDR) + 0x1000)
#define PP_UC_EGR_SHACC_CCU_UC_CLK_EN_RSVD0_OFF                     (1)
#define PP_UC_EGR_SHACC_CCU_UC_CLK_EN_RSVD0_LEN                     (31)
#define PP_UC_EGR_SHACC_CCU_UC_CLK_EN_RSVD0_MSK                     (0xFFFFFFFE)
#define PP_UC_EGR_SHACC_CCU_UC_CLK_EN_RSVD0_RST                     (0x0)
#define PP_UC_EGR_SHACC_CCU_UC_CLK_EN_CCU_UC_CLK_EN_OFF             (0)
#define PP_UC_EGR_SHACC_CCU_UC_CLK_EN_CCU_UC_CLK_EN_LEN             (1)
#define PP_UC_EGR_SHACC_CCU_UC_CLK_EN_CCU_UC_CLK_EN_MSK             (0x00000001)
#define PP_UC_EGR_SHACC_CCU_UC_CLK_EN_CCU_UC_CLK_EN_RST             (0x0)
/**
 * REG_IDX_ACCESS   : PP_UC_EGR_SHACC_CCU_UC_CLK_EN_REG_IDX
 * NUM OF REGISTERS : 4
 */
#define PP_UC_EGR_SHACC_CCU_UC_CLK_EN_REG_IDX(idx) \
	(PP_UC_EGR_SHACC_CCU_UC_CLK_EN_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_UC_EGR_SHACC_CCU_GP_REG
 * HW_REG_NAME : reg_ccu_gp_reg
 * DESCRIPTION : General Purpose register, NOTE, B Step ONLY
 *               register
 *
 *  Register Fields :
 *   [31: 0][RW] - General Purpose register
 *
 */
#define PP_UC_EGR_SHACC_CCU_GP_REG           ((UC_EGR_SHACC_BASE_ADDR) + 0x1400)
#define PP_UC_EGR_SHACC_CCU_GP_CCU_GP_REG_OFF                       (0)
#define PP_UC_EGR_SHACC_CCU_GP_CCU_GP_REG_LEN                       (32)
#define PP_UC_EGR_SHACC_CCU_GP_CCU_GP_REG_MSK                       (0xFFFFFFFF)
#define PP_UC_EGR_SHACC_CCU_GP_CCU_GP_REG_RST                       (0x0)
/**
 * REG_IDX_ACCESS   : PP_UC_EGR_SHACC_CCU_GP_REG_IDX
 * NUM OF REGISTERS : 8
 */
#define PP_UC_EGR_SHACC_CCU_GP_REG_IDX(idx) \
	(PP_UC_EGR_SHACC_CCU_GP_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_UC_EGR_SHACC_CCU_MAX_CPU_REG
 * HW_REG_NAME : reg_ccu_max_cpu
 * DESCRIPTION : Max CPUs, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 8][RO] - Reserved
 *   [ 7: 0][RO] - Max CPUs
 *
 */
#define PP_UC_EGR_SHACC_CCU_MAX_CPU_REG      ((UC_EGR_SHACC_BASE_ADDR) + 0x1800)
#define PP_UC_EGR_SHACC_CCU_MAX_CPU_RSVD0_OFF                       (8)
#define PP_UC_EGR_SHACC_CCU_MAX_CPU_RSVD0_LEN                       (24)
#define PP_UC_EGR_SHACC_CCU_MAX_CPU_RSVD0_MSK                       (0xFFFFFF00)
#define PP_UC_EGR_SHACC_CCU_MAX_CPU_RSVD0_RST                       (0x0)
#define PP_UC_EGR_SHACC_CCU_MAX_CPU_CCU_MAX_CPU_OFF                 (0)
#define PP_UC_EGR_SHACC_CCU_MAX_CPU_CCU_MAX_CPU_LEN                 (8)
#define PP_UC_EGR_SHACC_CCU_MAX_CPU_CCU_MAX_CPU_MSK                 (0x000000FF)
#define PP_UC_EGR_SHACC_CCU_MAX_CPU_CCU_MAX_CPU_RST                 (0x0)

/**
 * SW_REG_NAME : PP_UC_EGR_SHACC_CCU_UC_VERSION_REG
 * HW_REG_NAME : reg_ccu_uc_version
 * DESCRIPTION : uC Version, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31:16][RO] - Reserved
 *   [15: 0][RO] - Max CPUs
 *
 */
#define PP_UC_EGR_SHACC_CCU_UC_VERSION_REG   ((UC_EGR_SHACC_BASE_ADDR) + 0x2800)
#define PP_UC_EGR_SHACC_CCU_UC_VERSION_RSVD0_OFF                    (16)
#define PP_UC_EGR_SHACC_CCU_UC_VERSION_RSVD0_LEN                    (16)
#define PP_UC_EGR_SHACC_CCU_UC_VERSION_RSVD0_MSK                    (0xFFFF0000)
#define PP_UC_EGR_SHACC_CCU_UC_VERSION_RSVD0_RST                    (0x0)
#define PP_UC_EGR_SHACC_CCU_UC_VERSION_UC_VERSION_OFF               (0)
#define PP_UC_EGR_SHACC_CCU_UC_VERSION_UC_VERSION_LEN               (16)
#define PP_UC_EGR_SHACC_CCU_UC_VERSION_UC_VERSION_MSK               (0x0000FFFF)
#define PP_UC_EGR_SHACC_CCU_UC_VERSION_UC_VERSION_RST               (0x100)

/**
 * SW_REG_NAME : PP_UC_EGR_SHACC_DYNAMIC_ICG_FORCE_CLK_REG
 * HW_REG_NAME : reg_dynamic_icg_force_clk
 * DESCRIPTION : force clock for uc egress modules, NOTE, B Step
 *               ONLY register
 *
 *  Register Fields :
 *   [31: 1][RO] - Reserved
 *   [ 0: 0][RW] - Force system clk and enable the clock
 *
 */
#define PP_UC_EGR_SHACC_DYNAMIC_ICG_FORCE_CLK_REG \
	((UC_EGR_SHACC_BASE_ADDR) + 0x2C00)
#define PP_UC_EGR_SHACC_DYNAMIC_ICG_FORCE_CLK_RSVD0_OFF             (1)
#define PP_UC_EGR_SHACC_DYNAMIC_ICG_FORCE_CLK_RSVD0_LEN             (31)
#define PP_UC_EGR_SHACC_DYNAMIC_ICG_FORCE_CLK_RSVD0_MSK             (0xFFFFFFFE)
#define PP_UC_EGR_SHACC_DYNAMIC_ICG_FORCE_CLK_RSVD0_RST             (0x0)
#define PP_UC_EGR_SHACC_DYNAMIC_ICG_FORCE_CLK_DYNAMIC_ICG_FORCE_CLK_OFF      (0)
#define PP_UC_EGR_SHACC_DYNAMIC_ICG_FORCE_CLK_DYNAMIC_ICG_FORCE_CLK_LEN      (1)
#define PP_UC_EGR_SHACC_DYNAMIC_ICG_FORCE_CLK_DYNAMIC_ICG_FORCE_CLK_MSK \
	(0x00000001)
#define PP_UC_EGR_SHACC_DYNAMIC_ICG_FORCE_CLK_DYNAMIC_ICG_FORCE_CLK_RST    (0x1)

/**
 * SW_REG_NAME : PP_UC_EGR_SHACC_DYNAMIC_CLK_GATED_EN_REG
 * HW_REG_NAME : reg_dynamic_clk_gated_enable
 * DESCRIPTION : enable dynaimc system clock gate for uC egress
 *               modules, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 2][RO] - Reserved
 *   [ 1: 0][RW] - uc egress dynamic clk enable ; [0]dmae ;
 *                 [1]writer+reader  ;
 *
 */
#define PP_UC_EGR_SHACC_DYNAMIC_CLK_GATED_EN_REG \
	((UC_EGR_SHACC_BASE_ADDR) + 0x2C04)
#define PP_UC_EGR_SHACC_DYNAMIC_CLK_GATED_EN_RSVD0_OFF              (2)
#define PP_UC_EGR_SHACC_DYNAMIC_CLK_GATED_EN_RSVD0_LEN              (30)
#define PP_UC_EGR_SHACC_DYNAMIC_CLK_GATED_EN_RSVD0_MSK              (0xFFFFFFFC)
#define PP_UC_EGR_SHACC_DYNAMIC_CLK_GATED_EN_RSVD0_RST              (0x0)
#define PP_UC_EGR_SHACC_DYNAMIC_CLK_GATED_EN_DYNAMIC_ICG_EN_OFF     (0)
#define PP_UC_EGR_SHACC_DYNAMIC_CLK_GATED_EN_DYNAMIC_ICG_EN_LEN     (2)
#define PP_UC_EGR_SHACC_DYNAMIC_CLK_GATED_EN_DYNAMIC_ICG_EN_MSK     (0x00000003)
#define PP_UC_EGR_SHACC_DYNAMIC_CLK_GATED_EN_DYNAMIC_ICG_EN_RST     (0x0)

/**
 * SW_REG_NAME : PP_UC_EGR_SHACC_DYNAMIC_EGRESS_CG_STATUS_REG
 * HW_REG_NAME : reg_dynamic_egress_cg_status
 * DESCRIPTION : uC egress clock gates status , NOTE, B Step ONLY
 *               register
 *
 *  Register Fields :
 *   [31: 2][RO] - Reserved
 *   [ 1: 0][RO] - uc egress dynamic clk status ; [0]dmae ;
 *                 [1]writer+reader  ;
 *
 */
#define PP_UC_EGR_SHACC_DYNAMIC_EGRESS_CG_STATUS_REG \
	((UC_EGR_SHACC_BASE_ADDR) + 0x2C08)
#define PP_UC_EGR_SHACC_DYNAMIC_EGRESS_CG_STATUS_RSVD0_OFF          (2)
#define PP_UC_EGR_SHACC_DYNAMIC_EGRESS_CG_STATUS_RSVD0_LEN          (30)
#define PP_UC_EGR_SHACC_DYNAMIC_EGRESS_CG_STATUS_RSVD0_MSK          (0xFFFFFFFC)
#define PP_UC_EGR_SHACC_DYNAMIC_EGRESS_CG_STATUS_RSVD0_RST          (0x0)
#define PP_UC_EGR_SHACC_DYNAMIC_EGRESS_CG_STATUS_DYNAMIC_ICG_STATUS_OFF      (0)
#define PP_UC_EGR_SHACC_DYNAMIC_EGRESS_CG_STATUS_DYNAMIC_ICG_STATUS_LEN      (2)
#define PP_UC_EGR_SHACC_DYNAMIC_EGRESS_CG_STATUS_DYNAMIC_ICG_STATUS_MSK \
	(0x00000003)
#define PP_UC_EGR_SHACC_DYNAMIC_EGRESS_CG_STATUS_DYNAMIC_ICG_STATUS_RST    (0x0)

/**
 * SW_REG_NAME : PP_UC_EGR_SHACC_DYNAMIC_CG_GRACE_CNT_REG
 * HW_REG_NAME : reg_dynamic_cg_grace_counter
 * DESCRIPTION : uc egress clock counter set for CG grace cycles,
 *               NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31:10][RO] - Reserved
 *   [ 9: 0][RW] - uc sys clock counter set for CG grace cycles
 *
 */
#define PP_UC_EGR_SHACC_DYNAMIC_CG_GRACE_CNT_REG \
	((UC_EGR_SHACC_BASE_ADDR) + 0x2C0C)
#define PP_UC_EGR_SHACC_DYNAMIC_CG_GRACE_CNT_RSVD0_OFF              (10)
#define PP_UC_EGR_SHACC_DYNAMIC_CG_GRACE_CNT_RSVD0_LEN              (22)
#define PP_UC_EGR_SHACC_DYNAMIC_CG_GRACE_CNT_RSVD0_MSK              (0xFFFFFC00)
#define PP_UC_EGR_SHACC_DYNAMIC_CG_GRACE_CNT_RSVD0_RST              (0x0)
#define PP_UC_EGR_SHACC_DYNAMIC_CG_GRACE_CNT_DYNAMIC_ICG_GRACE_CYCLE_OFF     (0)
#define PP_UC_EGR_SHACC_DYNAMIC_CG_GRACE_CNT_DYNAMIC_ICG_GRACE_CYCLE_LEN    (10)
#define PP_UC_EGR_SHACC_DYNAMIC_CG_GRACE_CNT_DYNAMIC_ICG_GRACE_CYCLE_MSK \
	(0x000003FF)
#define PP_UC_EGR_SHACC_DYNAMIC_CG_GRACE_CNT_DYNAMIC_ICG_GRACE_CYCLE_RST  (0x20)

/**
 * SW_REG_NAME : PP_UC_EGR_SHACC_DYNAMIC_EGRESS_CG_REQ_STATUS_REG
 * HW_REG_NAME : reg_dynamic_egress_cg_req_status
 * DESCRIPTION : Ingress clock gates request status , NOTE, B Step
 *               ONLY register
 *
 *  Register Fields :
 *   [31: 2][RO] - Reserved
 *   [ 1: 0][RO] - uc egress dynamic clk request status ; [0]dmae ;
 *                 [1]writer+reader  ;
 *
 */
#define PP_UC_EGR_SHACC_DYNAMIC_EGRESS_CG_REQ_STATUS_REG \
	((UC_EGR_SHACC_BASE_ADDR) + 0x2C10)
#define PP_UC_EGR_SHACC_DYNAMIC_EGRESS_CG_REQ_STATUS_RSVD0_OFF      (2)
#define PP_UC_EGR_SHACC_DYNAMIC_EGRESS_CG_REQ_STATUS_RSVD0_LEN      (30)
#define PP_UC_EGR_SHACC_DYNAMIC_EGRESS_CG_REQ_STATUS_RSVD0_MSK      (0xFFFFFFFC)
#define PP_UC_EGR_SHACC_DYNAMIC_EGRESS_CG_REQ_STATUS_RSVD0_RST      (0x0)
#define PP_UC_EGR_SHACC_DYNAMIC_EGRESS_CG_REQ_STATUS_DYNAMIC_ICG_REQ_STATUS_OFF \
	(0)
#define PP_UC_EGR_SHACC_DYNAMIC_EGRESS_CG_REQ_STATUS_DYNAMIC_ICG_REQ_STATUS_LEN \
	(2)
#define PP_UC_EGR_SHACC_DYNAMIC_EGRESS_CG_REQ_STATUS_DYNAMIC_ICG_REQ_STATUS_MSK \
	(0x00000003)
#define PP_UC_EGR_SHACC_DYNAMIC_EGRESS_CG_REQ_STATUS_DYNAMIC_ICG_REQ_STATUS_RST \
	(0x0)

/**
 * SW_REG_NAME : PP_UC_EGR_SHACC_SYNCOBJ_TYPE_REG
 * HW_REG_NAME : reg_syncobj_type
 * DESCRIPTION : Indicates that the sync object if of type
 *               Mutex/Rendezvous, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31:16][RO] - Reserved
 *   [15: 0][RW] - Indicates that the sync object if of type
 *                 Mutex/Rendezvous
 *
 */
#define PP_UC_EGR_SHACC_SYNCOBJ_TYPE_REG    ((UC_EGR_SHACC_BASE_ADDR) + 0x10000)
#define PP_UC_EGR_SHACC_SYNCOBJ_TYPE_RSVD0_OFF                      (16)
#define PP_UC_EGR_SHACC_SYNCOBJ_TYPE_RSVD0_LEN                      (16)
#define PP_UC_EGR_SHACC_SYNCOBJ_TYPE_RSVD0_MSK                      (0xFFFF0000)
#define PP_UC_EGR_SHACC_SYNCOBJ_TYPE_RSVD0_RST                      (0x0)
#define PP_UC_EGR_SHACC_SYNCOBJ_TYPE_SYNCOBJ_TYPE_OFF               (0)
#define PP_UC_EGR_SHACC_SYNCOBJ_TYPE_SYNCOBJ_TYPE_LEN               (16)
#define PP_UC_EGR_SHACC_SYNCOBJ_TYPE_SYNCOBJ_TYPE_MSK               (0x0000FFFF)
#define PP_UC_EGR_SHACC_SYNCOBJ_TYPE_SYNCOBJ_TYPE_RST               (0x0)

/**
 * SW_REG_NAME : PP_UC_EGR_SHACC_SYNCOBJ_POST_START_REG
 * HW_REG_NAME : reg_syncobj_post_start
 * DESCRIPTION : Used by POSTA to add a Poster, NOTE, B Step ONLY
 *               register
 *
 *  Register Fields :
 *   [31: 4][RO] - Reserved
 *   [ 3: 0][RW] - Used by POSTA to add a Poster
 *
 */
#define PP_UC_EGR_SHACC_SYNCOBJ_POST_START_REG \
	((UC_EGR_SHACC_BASE_ADDR) + 0x10400)
#define PP_UC_EGR_SHACC_SYNCOBJ_POST_START_RSVD0_OFF                (4)
#define PP_UC_EGR_SHACC_SYNCOBJ_POST_START_RSVD0_LEN                (28)
#define PP_UC_EGR_SHACC_SYNCOBJ_POST_START_RSVD0_MSK                (0xFFFFFFF0)
#define PP_UC_EGR_SHACC_SYNCOBJ_POST_START_RSVD0_RST                (0x0)
#define PP_UC_EGR_SHACC_SYNCOBJ_POST_START_SYNCOBJ_POST_START_OFF   (0)
#define PP_UC_EGR_SHACC_SYNCOBJ_POST_START_SYNCOBJ_POST_START_LEN   (4)
#define PP_UC_EGR_SHACC_SYNCOBJ_POST_START_SYNCOBJ_POST_START_MSK   (0x0000000F)
#define PP_UC_EGR_SHACC_SYNCOBJ_POST_START_SYNCOBJ_POST_START_RST   (0x0)

/**
 * SW_REG_NAME : PP_UC_EGR_SHACC_SYNCOBJ_ACCEPT_START_REG
 * HW_REG_NAME : reg_syncobj_accept_start
 * DESCRIPTION : Used by ACCEPTA to set an Acceptor, NOTE, B Step
 *               ONLY register
 *
 *  Register Fields :
 *   [31: 4][RO] - Reserved
 *   [ 3: 0][RW] - Used by ACCEPTA to set an Acceptor
 *
 */
#define PP_UC_EGR_SHACC_SYNCOBJ_ACCEPT_START_REG \
	((UC_EGR_SHACC_BASE_ADDR) + 0x10800)
#define PP_UC_EGR_SHACC_SYNCOBJ_ACCEPT_START_RSVD0_OFF              (4)
#define PP_UC_EGR_SHACC_SYNCOBJ_ACCEPT_START_RSVD0_LEN              (28)
#define PP_UC_EGR_SHACC_SYNCOBJ_ACCEPT_START_RSVD0_MSK              (0xFFFFFFF0)
#define PP_UC_EGR_SHACC_SYNCOBJ_ACCEPT_START_RSVD0_RST              (0x0)
#define PP_UC_EGR_SHACC_SYNCOBJ_ACCEPT_START_SYNCOBJ_ACCEPT_START_OFF        (0)
#define PP_UC_EGR_SHACC_SYNCOBJ_ACCEPT_START_SYNCOBJ_ACCEPT_START_LEN        (4)
#define PP_UC_EGR_SHACC_SYNCOBJ_ACCEPT_START_SYNCOBJ_ACCEPT_START_MSK \
	(0x0000000F)
#define PP_UC_EGR_SHACC_SYNCOBJ_ACCEPT_START_SYNCOBJ_ACCEPT_START_RST      (0x0)

/**
 * SW_REG_NAME : PP_UC_EGR_SHACC_SYNCOBJ_SYNC_DONE_REG
 * HW_REG_NAME : reg_syncobj_sync_done
 * DESCRIPTION : Used by a task to indicate end of task
 *               synchronization, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 4][RO] - Reserved
 *   [ 3: 0][RW] - Used by a task to indicate end of task
 *                 synchronization
 *
 */
#define PP_UC_EGR_SHACC_SYNCOBJ_SYNC_DONE_REG \
	((UC_EGR_SHACC_BASE_ADDR) + 0x10C00)
#define PP_UC_EGR_SHACC_SYNCOBJ_SYNC_DONE_RSVD0_OFF                 (4)
#define PP_UC_EGR_SHACC_SYNCOBJ_SYNC_DONE_RSVD0_LEN                 (28)
#define PP_UC_EGR_SHACC_SYNCOBJ_SYNC_DONE_RSVD0_MSK                 (0xFFFFFFF0)
#define PP_UC_EGR_SHACC_SYNCOBJ_SYNC_DONE_RSVD0_RST                 (0x0)
#define PP_UC_EGR_SHACC_SYNCOBJ_SYNC_DONE_SYNCOBJ_SYNC_DONE_OFF     (0)
#define PP_UC_EGR_SHACC_SYNCOBJ_SYNC_DONE_SYNCOBJ_SYNC_DONE_LEN     (4)
#define PP_UC_EGR_SHACC_SYNCOBJ_SYNC_DONE_SYNCOBJ_SYNC_DONE_MSK     (0x0000000F)
#define PP_UC_EGR_SHACC_SYNCOBJ_SYNC_DONE_SYNCOBJ_SYNC_DONE_RST     (0x0)

/**
 * SW_REG_NAME : PP_UC_EGR_SHACC_SYNCOBJ_GO_DONE_REG
 * HW_REG_NAME : reg_syncobj_go_done
 * DESCRIPTION : Used by a task to indicate end of accept
 *               synchronization, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 4][RO] - Reserved
 *   [ 3: 0][RW] - Used by a task to indicate end of accept
 *                 synchronization
 *
 */
#define PP_UC_EGR_SHACC_SYNCOBJ_GO_DONE_REG ((UC_EGR_SHACC_BASE_ADDR) + 0x11000)
#define PP_UC_EGR_SHACC_SYNCOBJ_GO_DONE_RSVD0_OFF                   (4)
#define PP_UC_EGR_SHACC_SYNCOBJ_GO_DONE_RSVD0_LEN                   (28)
#define PP_UC_EGR_SHACC_SYNCOBJ_GO_DONE_RSVD0_MSK                   (0xFFFFFFF0)
#define PP_UC_EGR_SHACC_SYNCOBJ_GO_DONE_RSVD0_RST                   (0x0)
#define PP_UC_EGR_SHACC_SYNCOBJ_GO_DONE_SYNCOBJ_GO_DONE_OFF         (0)
#define PP_UC_EGR_SHACC_SYNCOBJ_GO_DONE_SYNCOBJ_GO_DONE_LEN         (4)
#define PP_UC_EGR_SHACC_SYNCOBJ_GO_DONE_SYNCOBJ_GO_DONE_MSK         (0x0000000F)
#define PP_UC_EGR_SHACC_SYNCOBJ_GO_DONE_SYNCOBJ_GO_DONE_RST         (0x0)

/**
 * SW_REG_NAME : PP_UC_EGR_SHACC_SYNCOBJ_DEST_REG
 * HW_REG_NAME : reg_syncobj_dest
 * DESCRIPTION : Holds a destination address, NOTE, B Step ONLY
 *               register
 *
 *  Register Fields :
 *   [31: 0][RW] - Holds a destination address
 *
 */
#define PP_UC_EGR_SHACC_SYNCOBJ_DEST_REG    ((UC_EGR_SHACC_BASE_ADDR) + 0x11400)
#define PP_UC_EGR_SHACC_SYNCOBJ_DEST_SYNCOBJ_DEST_OFF               (0)
#define PP_UC_EGR_SHACC_SYNCOBJ_DEST_SYNCOBJ_DEST_LEN               (32)
#define PP_UC_EGR_SHACC_SYNCOBJ_DEST_SYNCOBJ_DEST_MSK               (0xFFFFFFFF)
#define PP_UC_EGR_SHACC_SYNCOBJ_DEST_SYNCOBJ_DEST_RST               (0x0)
/**
 * REG_IDX_ACCESS   : PP_UC_EGR_SHACC_SYNCOBJ_DEST_REG_IDX
 * NUM OF REGISTERS : 16
 */
#define PP_UC_EGR_SHACC_SYNCOBJ_DEST_REG_IDX(idx) \
	(PP_UC_EGR_SHACC_SYNCOBJ_DEST_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_UC_EGR_SHACC_HWEVENT_START_REG
 * HW_REG_NAME : reg_hwevent_start
 * DESCRIPTION : Used by task to add a request for event, NOTE, B
 *               Step ONLY register
 *
 *  Register Fields :
 *   [31: 6][RO] - Reserved
 *   [ 5: 0][RW] - Used by task to add a request for event
 *
 */
#define PP_UC_EGR_SHACC_HWEVENT_START_REG   ((UC_EGR_SHACC_BASE_ADDR) + 0x18000)
#define PP_UC_EGR_SHACC_HWEVENT_START_RSVD0_OFF                     (6)
#define PP_UC_EGR_SHACC_HWEVENT_START_RSVD0_LEN                     (26)
#define PP_UC_EGR_SHACC_HWEVENT_START_RSVD0_MSK                     (0xFFFFFFC0)
#define PP_UC_EGR_SHACC_HWEVENT_START_RSVD0_RST                     (0x0)
#define PP_UC_EGR_SHACC_HWEVENT_START_HWEVENT_START_OFF             (0)
#define PP_UC_EGR_SHACC_HWEVENT_START_HWEVENT_START_LEN             (6)
#define PP_UC_EGR_SHACC_HWEVENT_START_HWEVENT_START_MSK             (0x0000003F)
#define PP_UC_EGR_SHACC_HWEVENT_START_HWEVENT_START_RST             (0x0)

/**
 * SW_REG_NAME : PP_UC_EGR_SHACC_HWEVENT_SOFT_EVENT_F_REG
 * HW_REG_NAME : reg_hwevent_soft_event_f
 * DESCRIPTION : Indicates that soft event was issued, NOTE, B Step
 *               ONLY register
 *
 *  Register Fields :
 *   [31: 1][RO] - Reserved
 *   [ 0: 0][RW] - Indicates that HW event was issued
 *
 */
#define PP_UC_EGR_SHACC_HWEVENT_SOFT_EVENT_F_REG \
	((UC_EGR_SHACC_BASE_ADDR) + 0x18400)
#define PP_UC_EGR_SHACC_HWEVENT_SOFT_EVENT_F_RSVD0_OFF              (1)
#define PP_UC_EGR_SHACC_HWEVENT_SOFT_EVENT_F_RSVD0_LEN              (31)
#define PP_UC_EGR_SHACC_HWEVENT_SOFT_EVENT_F_RSVD0_MSK              (0xFFFFFFFE)
#define PP_UC_EGR_SHACC_HWEVENT_SOFT_EVENT_F_RSVD0_RST              (0x0)
#define PP_UC_EGR_SHACC_HWEVENT_SOFT_EVENT_F_HWEVENT_SOFT_EVENT_F_OFF        (0)
#define PP_UC_EGR_SHACC_HWEVENT_SOFT_EVENT_F_HWEVENT_SOFT_EVENT_F_LEN        (1)
#define PP_UC_EGR_SHACC_HWEVENT_SOFT_EVENT_F_HWEVENT_SOFT_EVENT_F_MSK \
	(0x00000001)
#define PP_UC_EGR_SHACC_HWEVENT_SOFT_EVENT_F_HWEVENT_SOFT_EVENT_F_RST      (0x0)
/**
 * REG_IDX_ACCESS   : PP_UC_EGR_SHACC_HWEVENT_SOFT_EVENT_F_REG_IDX
 * NUM OF REGISTERS : 32
 */
#define PP_UC_EGR_SHACC_HWEVENT_SOFT_EVENT_F_REG_IDX(idx) \
	(PP_UC_EGR_SHACC_HWEVENT_SOFT_EVENT_F_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_UC_EGR_SHACC_HWEVENT_DONE_REG
 * HW_REG_NAME : reg_hwevent_done
 * DESCRIPTION : Used by task to indicate the host that it finished
 *               serving an event, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 5][RO] - Reserved
 *   [ 4: 0][RW] - Used by task to indicate the host that it finished
 *                 serving an event
 *
 */
#define PP_UC_EGR_SHACC_HWEVENT_DONE_REG    ((UC_EGR_SHACC_BASE_ADDR) + 0x18800)
#define PP_UC_EGR_SHACC_HWEVENT_DONE_RSVD0_OFF                      (5)
#define PP_UC_EGR_SHACC_HWEVENT_DONE_RSVD0_LEN                      (27)
#define PP_UC_EGR_SHACC_HWEVENT_DONE_RSVD0_MSK                      (0xFFFFFFE0)
#define PP_UC_EGR_SHACC_HWEVENT_DONE_RSVD0_RST                      (0x0)
#define PP_UC_EGR_SHACC_HWEVENT_DONE_HWEVENT_DONE_OFF               (0)
#define PP_UC_EGR_SHACC_HWEVENT_DONE_HWEVENT_DONE_LEN               (5)
#define PP_UC_EGR_SHACC_HWEVENT_DONE_HWEVENT_DONE_MSK               (0x0000001F)
#define PP_UC_EGR_SHACC_HWEVENT_DONE_HWEVENT_DONE_RST               (0x0)

/**
 * SW_REG_NAME : PP_UC_EGR_SHACC_HWEVENT_IER_REG
 * HW_REG_NAME : reg_hwevent_ier
 * DESCRIPTION : Interrupt Enable Register, NOTE, B Step ONLY
 *               register
 *
 *  Register Fields :
 *   [31: 0][RW] - Interrupt Enable Register
 *
 */
#define PP_UC_EGR_SHACC_HWEVENT_IER_REG     ((UC_EGR_SHACC_BASE_ADDR) + 0x18C00)
#define PP_UC_EGR_SHACC_HWEVENT_IER_HWEVENT_IER_OFF                 (0)
#define PP_UC_EGR_SHACC_HWEVENT_IER_HWEVENT_IER_LEN                 (32)
#define PP_UC_EGR_SHACC_HWEVENT_IER_HWEVENT_IER_MSK                 (0xFFFFFFFF)
#define PP_UC_EGR_SHACC_HWEVENT_IER_HWEVENT_IER_RST                 (0x0)

/**
 * SW_REG_NAME : PP_UC_EGR_SHACC_HWEVENT_IRR_REG
 * HW_REG_NAME : reg_hwevent_irr
 * DESCRIPTION : Interrupt Status Register, bit per event, NOTE, B
 *               Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][RO] - Interrupt Status Register, bit per event
 *
 */
#define PP_UC_EGR_SHACC_HWEVENT_IRR_REG     ((UC_EGR_SHACC_BASE_ADDR) + 0x19000)
#define PP_UC_EGR_SHACC_HWEVENT_IRR_HWEVENT_IRR_OFF                 (0)
#define PP_UC_EGR_SHACC_HWEVENT_IRR_HWEVENT_IRR_LEN                 (32)
#define PP_UC_EGR_SHACC_HWEVENT_IRR_HWEVENT_IRR_MSK                 (0xFFFFFFFF)
#define PP_UC_EGR_SHACC_HWEVENT_IRR_HWEVENT_IRR_RST                 (0x0)

/**
 * SW_REG_NAME : PP_UC_EGR_SHACC_HWEVENT_REQUEST_REG
 * HW_REG_NAME : reg_hwevent_request
 * DESCRIPTION : Used by task to add a request for event, NOTE, B
 *               Step ONLY register
 *
 *  Register Fields :
 *   [31:12][RO] - Reserved
 *   [11: 0][RO] - Used by task to add a request for event
 *
 */
#define PP_UC_EGR_SHACC_HWEVENT_REQUEST_REG ((UC_EGR_SHACC_BASE_ADDR) + 0x19400)
#define PP_UC_EGR_SHACC_HWEVENT_REQUEST_RSVD0_OFF                   (12)
#define PP_UC_EGR_SHACC_HWEVENT_REQUEST_RSVD0_LEN                   (20)
#define PP_UC_EGR_SHACC_HWEVENT_REQUEST_RSVD0_MSK                   (0xFFFFF000)
#define PP_UC_EGR_SHACC_HWEVENT_REQUEST_RSVD0_RST                   (0x0)
#define PP_UC_EGR_SHACC_HWEVENT_REQUEST_HWEVENT_REQUEST_OFF         (0)
#define PP_UC_EGR_SHACC_HWEVENT_REQUEST_HWEVENT_REQUEST_LEN         (12)
#define PP_UC_EGR_SHACC_HWEVENT_REQUEST_HWEVENT_REQUEST_MSK         (0x00000FFF)
#define PP_UC_EGR_SHACC_HWEVENT_REQUEST_HWEVENT_REQUEST_RST         (0x0)
/**
 * REG_IDX_ACCESS   : PP_UC_EGR_SHACC_HWEVENT_REQUEST_REG_IDX
 * NUM OF REGISTERS : 64
 */
#define PP_UC_EGR_SHACC_HWEVENT_REQUEST_REG_IDX(idx) \
	(PP_UC_EGR_SHACC_HWEVENT_REQUEST_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_UC_EGR_SHACC_HWEVENT_TASK_F_REG
 * HW_REG_NAME : reg_hwevent_task_f
 * DESCRIPTION : Indicates that a task is waiting for this event,
 *               NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 1][RO] - Reserved
 *   [ 0: 0][RO] - Indicates that a task is waiting for this event
 *
 */
#define PP_UC_EGR_SHACC_HWEVENT_TASK_F_REG  ((UC_EGR_SHACC_BASE_ADDR) + 0x19800)
#define PP_UC_EGR_SHACC_HWEVENT_TASK_F_RSVD0_OFF                    (1)
#define PP_UC_EGR_SHACC_HWEVENT_TASK_F_RSVD0_LEN                    (31)
#define PP_UC_EGR_SHACC_HWEVENT_TASK_F_RSVD0_MSK                    (0xFFFFFFFE)
#define PP_UC_EGR_SHACC_HWEVENT_TASK_F_RSVD0_RST                    (0x0)
#define PP_UC_EGR_SHACC_HWEVENT_TASK_F_HWEVENT_TASK_F_OFF           (0)
#define PP_UC_EGR_SHACC_HWEVENT_TASK_F_HWEVENT_TASK_F_LEN           (1)
#define PP_UC_EGR_SHACC_HWEVENT_TASK_F_HWEVENT_TASK_F_MSK           (0x00000001)
#define PP_UC_EGR_SHACC_HWEVENT_TASK_F_HWEVENT_TASK_F_RST           (0x0)
/**
 * REG_IDX_ACCESS   : PP_UC_EGR_SHACC_HWEVENT_TASK_F_REG_IDX
 * NUM OF REGISTERS : 64
 */
#define PP_UC_EGR_SHACC_HWEVENT_TASK_F_REG_IDX(idx) \
	(PP_UC_EGR_SHACC_HWEVENT_TASK_F_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_UC_EGR_SHACC_HWEVENT_HARD_EVENT_F_REG
 * HW_REG_NAME : reg_hwevent_hard_event_f
 * DESCRIPTION : Indicates that hard event was issued, NOTE, B Step
 *               ONLY register
 *
 *  Register Fields :
 *   [31: 1][RO] - Reserved
 *   [ 0: 0][RO] - Indicates that a task is waiting for this event
 *
 */
#define PP_UC_EGR_SHACC_HWEVENT_HARD_EVENT_F_REG \
	((UC_EGR_SHACC_BASE_ADDR) + 0x19C00)
#define PP_UC_EGR_SHACC_HWEVENT_HARD_EVENT_F_RSVD0_OFF              (1)
#define PP_UC_EGR_SHACC_HWEVENT_HARD_EVENT_F_RSVD0_LEN              (31)
#define PP_UC_EGR_SHACC_HWEVENT_HARD_EVENT_F_RSVD0_MSK              (0xFFFFFFFE)
#define PP_UC_EGR_SHACC_HWEVENT_HARD_EVENT_F_RSVD0_RST              (0x0)
#define PP_UC_EGR_SHACC_HWEVENT_HARD_EVENT_F_HWEVENT_HARD_EVENT_F_OFF        (0)
#define PP_UC_EGR_SHACC_HWEVENT_HARD_EVENT_F_HWEVENT_HARD_EVENT_F_LEN        (1)
#define PP_UC_EGR_SHACC_HWEVENT_HARD_EVENT_F_HWEVENT_HARD_EVENT_F_MSK \
	(0x00000001)
#define PP_UC_EGR_SHACC_HWEVENT_HARD_EVENT_F_HWEVENT_HARD_EVENT_F_RST      (0x0)
/**
 * REG_IDX_ACCESS   : PP_UC_EGR_SHACC_HWEVENT_HARD_EVENT_F_REG_IDX
 * NUM OF REGISTERS : 32
 */
#define PP_UC_EGR_SHACC_HWEVENT_HARD_EVENT_F_REG_IDX(idx) \
	(PP_UC_EGR_SHACC_HWEVENT_HARD_EVENT_F_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_UC_EGR_SHACC_CLUSTERBRIDGE_FAT_REG
 * HW_REG_NAME : reg_clusterbridge_fat
 * DESCRIPTION : Filter and address translation, NOTE, B Step ONLY
 *               register
 *
 *  Register Fields :
 *   [31:10][RO] - Reserved
 *   [ 9: 0][RW] - Filter and address translation
 *
 */
#define PP_UC_EGR_SHACC_CLUSTERBRIDGE_FAT_REG \
	((UC_EGR_SHACC_BASE_ADDR) + 0x20400)
#define PP_UC_EGR_SHACC_CLUSTERBRIDGE_FAT_RSVD0_OFF                 (10)
#define PP_UC_EGR_SHACC_CLUSTERBRIDGE_FAT_RSVD0_LEN                 (22)
#define PP_UC_EGR_SHACC_CLUSTERBRIDGE_FAT_RSVD0_MSK                 (0xFFFFFC00)
#define PP_UC_EGR_SHACC_CLUSTERBRIDGE_FAT_RSVD0_RST                 (0x0)
#define PP_UC_EGR_SHACC_CLUSTERBRIDGE_FAT_CLUSTERBRIDGE_PERIPH_ID_OFF        (0)
#define PP_UC_EGR_SHACC_CLUSTERBRIDGE_FAT_CLUSTERBRIDGE_PERIPH_ID_LEN       (10)
#define PP_UC_EGR_SHACC_CLUSTERBRIDGE_FAT_CLUSTERBRIDGE_PERIPH_ID_MSK \
	(0x000003FF)
#define PP_UC_EGR_SHACC_CLUSTERBRIDGE_FAT_CLUSTERBRIDGE_PERIPH_ID_RST      (0x0)
/**
 * REG_IDX_ACCESS   : PP_UC_EGR_SHACC_CLUSTERBRIDGE_FAT_REG_IDX
 * NUM OF REGISTERS : 48
 */
#define PP_UC_EGR_SHACC_CLUSTERBRIDGE_FAT_REG_IDX(idx) \
	(PP_UC_EGR_SHACC_CLUSTERBRIDGE_FAT_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_UC_EGR_SHACC_CLUSTERBRIDGE_IER_REG
 * HW_REG_NAME : reg_clusterbridge_ier
 * DESCRIPTION : ISR, Interrupt Enable Register, NOTE, B Step ONLY
 *               register
 *
 *  Register Fields :
 *   [31: 1][RO] - Reserved
 *   [ 0: 0][RW] - ISR, Interrupt Enable Register
 *
 */
#define PP_UC_EGR_SHACC_CLUSTERBRIDGE_IER_REG \
	((UC_EGR_SHACC_BASE_ADDR) + 0x20800)
#define PP_UC_EGR_SHACC_CLUSTERBRIDGE_IER_RSVD0_OFF                 (1)
#define PP_UC_EGR_SHACC_CLUSTERBRIDGE_IER_RSVD0_LEN                 (31)
#define PP_UC_EGR_SHACC_CLUSTERBRIDGE_IER_RSVD0_MSK                 (0xFFFFFFFE)
#define PP_UC_EGR_SHACC_CLUSTERBRIDGE_IER_RSVD0_RST                 (0x0)
#define PP_UC_EGR_SHACC_CLUSTERBRIDGE_IER_CLUSTERBRIDGE_IER_OFF     (0)
#define PP_UC_EGR_SHACC_CLUSTERBRIDGE_IER_CLUSTERBRIDGE_IER_LEN     (1)
#define PP_UC_EGR_SHACC_CLUSTERBRIDGE_IER_CLUSTERBRIDGE_IER_MSK     (0x00000001)
#define PP_UC_EGR_SHACC_CLUSTERBRIDGE_IER_CLUSTERBRIDGE_IER_RST     (0x0)

/**
 * SW_REG_NAME : PP_UC_EGR_SHACC_CLUSTERBRIDGE_IRR_REG
 * HW_REG_NAME : reg_clusterbridge_irr
 * DESCRIPTION : ISR, Interrupt Request Register, NOTE, B Step ONLY
 *               register
 *
 *  Register Fields :
 *   [31: 1][RO] - Reserved
 *   [ 0: 0][RO] - ISR, Interrupt Request Register
 *
 */
#define PP_UC_EGR_SHACC_CLUSTERBRIDGE_IRR_REG \
	((UC_EGR_SHACC_BASE_ADDR) + 0x20C00)
#define PP_UC_EGR_SHACC_CLUSTERBRIDGE_IRR_RSVD0_OFF                 (1)
#define PP_UC_EGR_SHACC_CLUSTERBRIDGE_IRR_RSVD0_LEN                 (31)
#define PP_UC_EGR_SHACC_CLUSTERBRIDGE_IRR_RSVD0_MSK                 (0xFFFFFFFE)
#define PP_UC_EGR_SHACC_CLUSTERBRIDGE_IRR_RSVD0_RST                 (0x0)
#define PP_UC_EGR_SHACC_CLUSTERBRIDGE_IRR_CLUSTERBRIDGE_IRR_OFF     (0)
#define PP_UC_EGR_SHACC_CLUSTERBRIDGE_IRR_CLUSTERBRIDGE_IRR_LEN     (1)
#define PP_UC_EGR_SHACC_CLUSTERBRIDGE_IRR_CLUSTERBRIDGE_IRR_MSK     (0x00000001)
#define PP_UC_EGR_SHACC_CLUSTERBRIDGE_IRR_CLUSTERBRIDGE_IRR_RST     (0x0)

/**
 * SW_REG_NAME : PP_UC_EGR_SHACC_CLUSTERBRIDGE_DEST_ADDR_REG
 * HW_REG_NAME : reg_clusterbridge_dest_addr
 * DESCRIPTION : Save the last ADDR, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][RO] - Save the last ADDR
 *
 */
#define PP_UC_EGR_SHACC_CLUSTERBRIDGE_DEST_ADDR_REG \
	((UC_EGR_SHACC_BASE_ADDR) + 0x21000)
#define PP_UC_EGR_SHACC_CLUSTERBRIDGE_DEST_ADDR_CLUSTERBRIDGE_DEST_ADDR_OFF  (0)
#define PP_UC_EGR_SHACC_CLUSTERBRIDGE_DEST_ADDR_CLUSTERBRIDGE_DEST_ADDR_LEN (32)
#define PP_UC_EGR_SHACC_CLUSTERBRIDGE_DEST_ADDR_CLUSTERBRIDGE_DEST_ADDR_MSK \
	(0xFFFFFFFF)
#define PP_UC_EGR_SHACC_CLUSTERBRIDGE_DEST_ADDR_CLUSTERBRIDGE_DEST_ADDR_RST \
	(0x0)

/**
 * SW_REG_NAME : PP_UC_EGR_SHACC_CLUSTERBRIDGE_DEST_DATA_REG
 * HW_REG_NAME : reg_clusterbridge_dest_data
 * DESCRIPTION : Save the last DATA, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][RO] - Save the last DATA
 *
 */
#define PP_UC_EGR_SHACC_CLUSTERBRIDGE_DEST_DATA_REG \
	((UC_EGR_SHACC_BASE_ADDR) + 0x21400)
#define PP_UC_EGR_SHACC_CLUSTERBRIDGE_DEST_DATA_CLUSTERBRIDGE_DEST_DATA_OFF  (0)
#define PP_UC_EGR_SHACC_CLUSTERBRIDGE_DEST_DATA_CLUSTERBRIDGE_DEST_DATA_LEN (32)
#define PP_UC_EGR_SHACC_CLUSTERBRIDGE_DEST_DATA_CLUSTERBRIDGE_DEST_DATA_MSK \
	(0xFFFFFFFF)
#define PP_UC_EGR_SHACC_CLUSTERBRIDGE_DEST_DATA_CLUSTERBRIDGE_DEST_DATA_RST \
	(0x0)
/**
 * REG_IDX_ACCESS   : PP_UC_EGR_SHACC_CLUSTERBRIDGE_DEST_DATA_REG_IDX
 * NUM OF REGISTERS : 4
 */
#define PP_UC_EGR_SHACC_CLUSTERBRIDGE_DEST_DATA_REG_IDX(idx) \
	(PP_UC_EGR_SHACC_CLUSTERBRIDGE_DEST_DATA_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_UC_EGR_SHACC_CLUSTERBRIDGE_BASE_REG
 * HW_REG_NAME : reg_clusterbridge_base
 * DESCRIPTION : Filter and address translation BASE, NOTE, B Step
 *               ONLY register
 *
 *  Register Fields :
 *   [31: 0][RW] - Filter and address translation
 *
 */
#define PP_UC_EGR_SHACC_CLUSTERBRIDGE_BASE_REG \
	((UC_EGR_SHACC_BASE_ADDR) + 0x21800)
#define PP_UC_EGR_SHACC_CLUSTERBRIDGE_BASE_CLUSTERBRIDGE_PERIPH_ID_BASE_OFF  (0)
#define PP_UC_EGR_SHACC_CLUSTERBRIDGE_BASE_CLUSTERBRIDGE_PERIPH_ID_BASE_LEN (32)
#define PP_UC_EGR_SHACC_CLUSTERBRIDGE_BASE_CLUSTERBRIDGE_PERIPH_ID_BASE_MSK \
	(0xFFFFFFFF)
#define PP_UC_EGR_SHACC_CLUSTERBRIDGE_BASE_CLUSTERBRIDGE_PERIPH_ID_BASE_RST \
	(0x0)
/**
 * REG_IDX_ACCESS   : PP_UC_EGR_SHACC_CLUSTERBRIDGE_BASE_REG_IDX
 * NUM OF REGISTERS : 8
 */
#define PP_UC_EGR_SHACC_CLUSTERBRIDGE_BASE_REG_IDX(idx) \
	(PP_UC_EGR_SHACC_CLUSTERBRIDGE_BASE_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_UC_EGR_SHACC_CLUSTERBRIDGE_VALID_REG
 * HW_REG_NAME : reg_clusterbridge_valid
 * DESCRIPTION : MISSING Description, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 1][RO] - Reserved
 *   [ 0: 0][RW] - Filter and address translation Valid
 *
 */
#define PP_UC_EGR_SHACC_CLUSTERBRIDGE_VALID_REG \
	((UC_EGR_SHACC_BASE_ADDR) + 0x21C00)
#define PP_UC_EGR_SHACC_CLUSTERBRIDGE_VALID_RSVD0_OFF               (1)
#define PP_UC_EGR_SHACC_CLUSTERBRIDGE_VALID_RSVD0_LEN               (31)
#define PP_UC_EGR_SHACC_CLUSTERBRIDGE_VALID_RSVD0_MSK               (0xFFFFFFFE)
#define PP_UC_EGR_SHACC_CLUSTERBRIDGE_VALID_RSVD0_RST               (0x0)
#define PP_UC_EGR_SHACC_CLUSTERBRIDGE_VALID_CLUSTERBRIDGE_PERIPH_ID_VLD_OFF  (0)
#define PP_UC_EGR_SHACC_CLUSTERBRIDGE_VALID_CLUSTERBRIDGE_PERIPH_ID_VLD_LEN  (1)
#define PP_UC_EGR_SHACC_CLUSTERBRIDGE_VALID_CLUSTERBRIDGE_PERIPH_ID_VLD_MSK \
	(0x00000001)
#define PP_UC_EGR_SHACC_CLUSTERBRIDGE_VALID_CLUSTERBRIDGE_PERIPH_ID_VLD_RST \
	(0x0)
/**
 * REG_IDX_ACCESS   : PP_UC_EGR_SHACC_CLUSTERBRIDGE_VALID_REG_IDX
 * NUM OF REGISTERS : 56
 */
#define PP_UC_EGR_SHACC_CLUSTERBRIDGE_VALID_REG_IDX(idx) \
	(PP_UC_EGR_SHACC_CLUSTERBRIDGE_VALID_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_UC_EGR_SHACC_TDOX_STATUS_REG
 * HW_REG_NAME : reg_tdox_status
 * DESCRIPTION : Reflects the signals groups_status[15:0] outputted
 *               by the TDOX block, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31:16][RO] - Reserved
 *   [15: 0][RO] - Reflects the signals groups_status[15:0] outputted
 *                 by the TDOX block
 *
 */
#define PP_UC_EGR_SHACC_TDOX_STATUS_REG     ((UC_EGR_SHACC_BASE_ADDR) + 0x28000)
#define PP_UC_EGR_SHACC_TDOX_STATUS_RSVD0_OFF                       (16)
#define PP_UC_EGR_SHACC_TDOX_STATUS_RSVD0_LEN                       (16)
#define PP_UC_EGR_SHACC_TDOX_STATUS_RSVD0_MSK                       (0xFFFF0000)
#define PP_UC_EGR_SHACC_TDOX_STATUS_RSVD0_RST                       (0x0)
#define PP_UC_EGR_SHACC_TDOX_STATUS_TDOX_STATUS_OFF                 (0)
#define PP_UC_EGR_SHACC_TDOX_STATUS_TDOX_STATUS_LEN                 (16)
#define PP_UC_EGR_SHACC_TDOX_STATUS_TDOX_STATUS_MSK                 (0x0000FFFF)
#define PP_UC_EGR_SHACC_TDOX_STATUS_TDOX_STATUS_RST                 (0x0)

/**
 * SW_REG_NAME : PP_UC_EGR_SHACC_TDOX_MASK_REG
 * HW_REG_NAME : reg_tdox_mask
 * DESCRIPTION : Mask, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31:16][RO] - Reserved
 *   [15: 0][RW] - Mask
 *
 */
#define PP_UC_EGR_SHACC_TDOX_MASK_REG       ((UC_EGR_SHACC_BASE_ADDR) + 0x28400)
#define PP_UC_EGR_SHACC_TDOX_MASK_RSVD0_OFF                         (16)
#define PP_UC_EGR_SHACC_TDOX_MASK_RSVD0_LEN                         (16)
#define PP_UC_EGR_SHACC_TDOX_MASK_RSVD0_MSK                         (0xFFFF0000)
#define PP_UC_EGR_SHACC_TDOX_MASK_RSVD0_RST                         (0x0)
#define PP_UC_EGR_SHACC_TDOX_MASK_TDOX_MASK_OFF                     (0)
#define PP_UC_EGR_SHACC_TDOX_MASK_TDOX_MASK_LEN                     (16)
#define PP_UC_EGR_SHACC_TDOX_MASK_TDOX_MASK_MSK                     (0x0000FFFF)
#define PP_UC_EGR_SHACC_TDOX_MASK_TDOX_MASK_RST                     (0x0)

/**
 * SW_REG_NAME : PP_UC_EGR_SHACC_TDOX_COMMAND_REG
 * HW_REG_NAME : reg_tdox_command
 * DESCRIPTION : Enabling to set the 2 input bits stop_timers and
 *               clock_en, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 2][RO] - Reserved
 *   [ 1: 0][RW] - Enabling to set the 2 input bits stop_timers and
 *                 clock_en
 *
 */
#define PP_UC_EGR_SHACC_TDOX_COMMAND_REG    ((UC_EGR_SHACC_BASE_ADDR) + 0x28800)
#define PP_UC_EGR_SHACC_TDOX_COMMAND_RSVD0_OFF                      (2)
#define PP_UC_EGR_SHACC_TDOX_COMMAND_RSVD0_LEN                      (30)
#define PP_UC_EGR_SHACC_TDOX_COMMAND_RSVD0_MSK                      (0xFFFFFFFC)
#define PP_UC_EGR_SHACC_TDOX_COMMAND_RSVD0_RST                      (0x0)
#define PP_UC_EGR_SHACC_TDOX_COMMAND_TDOX_COMMAND_OFF               (0)
#define PP_UC_EGR_SHACC_TDOX_COMMAND_TDOX_COMMAND_LEN               (2)
#define PP_UC_EGR_SHACC_TDOX_COMMAND_TDOX_COMMAND_MSK               (0x00000003)
#define PP_UC_EGR_SHACC_TDOX_COMMAND_TDOX_COMMAND_RST               (0x0)

/**
 * SW_REG_NAME : PP_UC_EGR_SHACC_TDOX_ENDIANITY_REG
 * HW_REG_NAME : reg_tdox_endianity
 * DESCRIPTION : Endianity, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 1][RO] - Reserved
 *   [ 0: 0][RW] - Endianity
 *
 */
#define PP_UC_EGR_SHACC_TDOX_ENDIANITY_REG  ((UC_EGR_SHACC_BASE_ADDR) + 0x28C00)
#define PP_UC_EGR_SHACC_TDOX_ENDIANITY_RSVD0_OFF                    (1)
#define PP_UC_EGR_SHACC_TDOX_ENDIANITY_RSVD0_LEN                    (31)
#define PP_UC_EGR_SHACC_TDOX_ENDIANITY_RSVD0_MSK                    (0xFFFFFFFE)
#define PP_UC_EGR_SHACC_TDOX_ENDIANITY_RSVD0_RST                    (0x0)
#define PP_UC_EGR_SHACC_TDOX_ENDIANITY_TDOX_ENDIANITY_OFF           (0)
#define PP_UC_EGR_SHACC_TDOX_ENDIANITY_TDOX_ENDIANITY_LEN           (1)
#define PP_UC_EGR_SHACC_TDOX_ENDIANITY_TDOX_ENDIANITY_MSK           (0x00000001)
#define PP_UC_EGR_SHACC_TDOX_ENDIANITY_TDOX_ENDIANITY_RST           (0x0)

#endif
