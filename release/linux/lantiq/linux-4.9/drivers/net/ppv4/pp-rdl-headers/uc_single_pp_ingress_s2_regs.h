/**
 * uc_single_pp_ingress_s2_regs.h
 * Description: uc_single_pp_ingress_s2 HW registers definitions
 *              NOTE: B Step ONLY module
 * Auto-generated file by RDL Parser, DO NOT CHANGE
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2019-2020 Intel Corporation
 */

#ifndef _PP_UC_ING_S2_H_
#define _PP_UC_ING_S2_H_

#define PP_UC_ING_S2_GEN_DATE_STR             "Sunday May 17, 2020 [5:26:11 pm]"
#define PP_UC_ING_S2_BASE                                        (0xF0A80000ULL)

/**
 * SW_REG_NAME : PP_UC_ING_S2_BOOT_RESET_VECTOR_REG
 * HW_REG_NAME : reg_boot_reset_vector
 * DESCRIPTION : uC vector table, set by host, NOTE, B Step ONLY
 *               register
 *
 *  Register Fields :
 *   [31: 0][RW] - uC vector table, set by host
 *
 */
#define PP_UC_ING_S2_BOOT_RESET_VECTOR_REG         ((UC_ING_S2_BASE_ADDR) + 0x0)
#define PP_UC_ING_S2_BOOT_RESET_VECTOR_BOOT_RESET_VECTOR_OFF        (0)
#define PP_UC_ING_S2_BOOT_RESET_VECTOR_BOOT_RESET_VECTOR_LEN        (32)
#define PP_UC_ING_S2_BOOT_RESET_VECTOR_BOOT_RESET_VECTOR_MSK        (0xFFFFFFFF)
#define PP_UC_ING_S2_BOOT_RESET_VECTOR_BOOT_RESET_VECTOR_RST        (0x0)
/**
 * REG_IDX_ACCESS   : PP_UC_ING_S2_BOOT_RESET_VECTOR_REG_IDX
 * NUM OF REGISTERS : 20
 */
#define PP_UC_ING_S2_BOOT_RESET_VECTOR_REG_IDX(idx) \
	(PP_UC_ING_S2_BOOT_RESET_VECTOR_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_UC_ING_S2_BOOT_CORE_RUN_HALT_REG
 * HW_REG_NAME : reg_boot_core_run_halt
 * DESCRIPTION : 0 upon reset, set by host, NOTE, B Step ONLY
 *               register
 *
 *  Register Fields :
 *   [31: 2][RO] - Reserved
 *   [ 1: 0][RW] - 1 upon reset, cleared by host
 *
 */
#define PP_UC_ING_S2_BOOT_CORE_RUN_HALT_REG      ((UC_ING_S2_BASE_ADDR) + 0x100)
#define PP_UC_ING_S2_BOOT_CORE_RUN_HALT_RSVD0_OFF                   (2)
#define PP_UC_ING_S2_BOOT_CORE_RUN_HALT_RSVD0_LEN                   (30)
#define PP_UC_ING_S2_BOOT_CORE_RUN_HALT_RSVD0_MSK                   (0xFFFFFFFC)
#define PP_UC_ING_S2_BOOT_CORE_RUN_HALT_RSVD0_RST                   (0x0)
#define PP_UC_ING_S2_BOOT_CORE_RUN_HALT_BOOT_CORE_RUN_HALT_OFF      (0)
#define PP_UC_ING_S2_BOOT_CORE_RUN_HALT_BOOT_CORE_RUN_HALT_LEN      (2)
#define PP_UC_ING_S2_BOOT_CORE_RUN_HALT_BOOT_CORE_RUN_HALT_MSK      (0x00000003)
#define PP_UC_ING_S2_BOOT_CORE_RUN_HALT_BOOT_CORE_RUN_HALT_RST      (0x0)

/**
 * SW_REG_NAME : PP_UC_ING_S2_BOOT_MY_CID_REG
 * HW_REG_NAME : reg_boot_my_cid
 * DESCRIPTION : My CPU ID, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 6][RO] - Reserved
 *   [ 5: 0][RO] - holds MyCID, hard-wired
 *
 */
#define PP_UC_ING_S2_BOOT_MY_CID_REG             ((UC_ING_S2_BASE_ADDR) + 0x200)
#define PP_UC_ING_S2_BOOT_MY_CID_RSVD0_OFF                          (6)
#define PP_UC_ING_S2_BOOT_MY_CID_RSVD0_LEN                          (26)
#define PP_UC_ING_S2_BOOT_MY_CID_RSVD0_MSK                          (0xFFFFFFC0)
#define PP_UC_ING_S2_BOOT_MY_CID_RSVD0_RST                          (0x0)
#define PP_UC_ING_S2_BOOT_MY_CID_BOOT_MY_CID_OFF                    (0)
#define PP_UC_ING_S2_BOOT_MY_CID_BOOT_MY_CID_LEN                    (6)
#define PP_UC_ING_S2_BOOT_MY_CID_BOOT_MY_CID_MSK                    (0x0000003F)
#define PP_UC_ING_S2_BOOT_MY_CID_BOOT_MY_CID_RST                    (0x0)

/**
 * SW_REG_NAME : PP_UC_ING_S2_BOOT_GP_REG
 * HW_REG_NAME : reg_boot_gp_reg
 * DESCRIPTION : General Purpose Register, NOTE, B Step ONLY
 *               register
 *
 *  Register Fields :
 *   [31: 0][RW] - General Purpose Register
 *
 */
#define PP_UC_ING_S2_BOOT_GP_REG                 ((UC_ING_S2_BASE_ADDR) + 0x300)
#define PP_UC_ING_S2_BOOT_GP_BOOT_GP_REG_OFF                        (0)
#define PP_UC_ING_S2_BOOT_GP_BOOT_GP_REG_LEN                        (32)
#define PP_UC_ING_S2_BOOT_GP_BOOT_GP_REG_MSK                        (0xFFFFFFFF)
#define PP_UC_ING_S2_BOOT_GP_BOOT_GP_REG_RST                        (0x0)

/**
 * SW_REG_NAME : PP_UC_ING_S2_BOOT_SW_REG
 * HW_REG_NAME : reg_boot_sw_reg
 * DESCRIPTION : General Purpose Register, NOTE, B Step ONLY
 *               register
 *
 *  Register Fields :
 *   [31: 0][RW] - General Purpose Register
 *
 */
#define PP_UC_ING_S2_BOOT_SW_REG                 ((UC_ING_S2_BASE_ADDR) + 0x400)
#define PP_UC_ING_S2_BOOT_SW_BOOT_SW_REG_OFF                        (0)
#define PP_UC_ING_S2_BOOT_SW_BOOT_SW_REG_LEN                        (32)
#define PP_UC_ING_S2_BOOT_SW_BOOT_SW_REG_MSK                        (0xFFFFFFFF)
#define PP_UC_ING_S2_BOOT_SW_BOOT_SW_REG_RST                        (0x1)
/**
 * REG_IDX_ACCESS   : PP_UC_ING_S2_BOOT_SW_REG_IDX
 * NUM OF REGISTERS : 16
 */
#define PP_UC_ING_S2_BOOT_SW_REG_IDX(idx) \
	(PP_UC_ING_S2_BOOT_SW_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_UC_ING_S2_BOOT_RUN_HALT_ACK_REG
 * HW_REG_NAME : reg_boot_run_halt_ack
 * DESCRIPTION : Run Ack, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 2][RO] - Reserved
 *   [ 1: 0][RO] - Run Ack
 *
 */
#define PP_UC_ING_S2_BOOT_RUN_HALT_ACK_REG       ((UC_ING_S2_BASE_ADDR) + 0x500)
#define PP_UC_ING_S2_BOOT_RUN_HALT_ACK_RSVD0_OFF                    (2)
#define PP_UC_ING_S2_BOOT_RUN_HALT_ACK_RSVD0_LEN                    (30)
#define PP_UC_ING_S2_BOOT_RUN_HALT_ACK_RSVD0_MSK                    (0xFFFFFFFC)
#define PP_UC_ING_S2_BOOT_RUN_HALT_ACK_RSVD0_RST                    (0x0)
#define PP_UC_ING_S2_BOOT_RUN_HALT_ACK_BOOT_RUN_HALT_ACK_OFF        (0)
#define PP_UC_ING_S2_BOOT_RUN_HALT_ACK_BOOT_RUN_HALT_ACK_LEN        (2)
#define PP_UC_ING_S2_BOOT_RUN_HALT_ACK_BOOT_RUN_HALT_ACK_MSK        (0x00000003)
#define PP_UC_ING_S2_BOOT_RUN_HALT_ACK_BOOT_RUN_HALT_ACK_RST        (0x0)

/**
 * SW_REG_NAME : PP_UC_ING_S2_CPU_AP_PARAM_REG
 * HW_REG_NAME : reg_cpu_ap_param
 * DESCRIPTION : CPU AP PARAM, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][RW] - CPU AP PARAM
 *
 */
#define PP_UC_ING_S2_CPU_AP_PARAM_REG            ((UC_ING_S2_BASE_ADDR) + 0x800)
#define PP_UC_ING_S2_CPU_AP_PARAM_OFF                               (0)
#define PP_UC_ING_S2_CPU_AP_PARAM_LEN                               (32)
#define PP_UC_ING_S2_CPU_AP_PARAM_MSK                               (0xFFFFFFFF)
#define PP_UC_ING_S2_CPU_AP_PARAM_RST                               (0x0)
/**
 * REG_IDX_ACCESS   : PP_UC_ING_S2_CPU_AP_PARAM_REG_IDX
 * NUM OF REGISTERS : 2
 */
#define PP_UC_ING_S2_CPU_AP_PARAM_REG_IDX(idx) \
	(PP_UC_ING_S2_CPU_AP_PARAM_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_UC_ING_S2_CPU_AP_PARAM_CTRL_REG
 * HW_REG_NAME : reg_cpu_ap_param_ctrl
 * DESCRIPTION : CPU AP PARAM CTRL, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 2][RO] - Reserved
 *   [ 1: 1][RW] - CPU AP PARAM Read
 *   [ 0: 0][RW] - CPU AP PARAM Write
 *
 */
#define PP_UC_ING_S2_CPU_AP_PARAM_CTRL_REG       ((UC_ING_S2_BASE_ADDR) + 0x900)
#define PP_UC_ING_S2_CPU_AP_PARAM_CTRL_RSVD0_OFF                    (2)
#define PP_UC_ING_S2_CPU_AP_PARAM_CTRL_RSVD0_LEN                    (30)
#define PP_UC_ING_S2_CPU_AP_PARAM_CTRL_RSVD0_MSK                    (0xFFFFFFFC)
#define PP_UC_ING_S2_CPU_AP_PARAM_CTRL_RSVD0_RST                    (0x0)
#define PP_UC_ING_S2_CPU_AP_PARAM_CTRL_CPU_AP_PARAM_READ_OFF        (1)
#define PP_UC_ING_S2_CPU_AP_PARAM_CTRL_CPU_AP_PARAM_READ_LEN        (1)
#define PP_UC_ING_S2_CPU_AP_PARAM_CTRL_CPU_AP_PARAM_READ_MSK        (0x00000002)
#define PP_UC_ING_S2_CPU_AP_PARAM_CTRL_CPU_AP_PARAM_READ_RST        (0x0)
#define PP_UC_ING_S2_CPU_AP_PARAM_CTRL_CPU_AP_PARAM_WRITE_OFF       (0)
#define PP_UC_ING_S2_CPU_AP_PARAM_CTRL_CPU_AP_PARAM_WRITE_LEN       (1)
#define PP_UC_ING_S2_CPU_AP_PARAM_CTRL_CPU_AP_PARAM_WRITE_MSK       (0x00000001)
#define PP_UC_ING_S2_CPU_AP_PARAM_CTRL_CPU_AP_PARAM_WRITE_RST       (0x0)
/**
 * REG_IDX_ACCESS   : PP_UC_ING_S2_CPU_AP_PARAM_CTRL_REG_IDX
 * NUM OF REGISTERS : 2
 */
#define PP_UC_ING_S2_CPU_AP_PARAM_CTRL_REG_IDX(idx) \
	(PP_UC_ING_S2_CPU_AP_PARAM_CTRL_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_UC_ING_S2_TSSM_NEXT_REG
 * HW_REG_NAME : reg_tssm_next
 * DESCRIPTION : Holds the ID of the next task, NOTE, B Step ONLY
 *               register
 *
 *  Register Fields :
 *   [31: 6][RO] - Reserved
 *   [ 5: 0][RO] - Holds the ID of the next task
 *
 */
#define PP_UC_ING_S2_TSSM_NEXT_REG              ((UC_ING_S2_BASE_ADDR) + 0x2000)
#define PP_UC_ING_S2_TSSM_NEXT_RSVD0_OFF                            (6)
#define PP_UC_ING_S2_TSSM_NEXT_RSVD0_LEN                            (26)
#define PP_UC_ING_S2_TSSM_NEXT_RSVD0_MSK                            (0xFFFFFFC0)
#define PP_UC_ING_S2_TSSM_NEXT_RSVD0_RST                            (0x0)
#define PP_UC_ING_S2_TSSM_NEXT_TSSM_NEXT_OFF                        (0)
#define PP_UC_ING_S2_TSSM_NEXT_TSSM_NEXT_LEN                        (6)
#define PP_UC_ING_S2_TSSM_NEXT_TSSM_NEXT_MSK                        (0x0000003F)
#define PP_UC_ING_S2_TSSM_NEXT_TSSM_NEXT_RST                        (0x0)

/**
 * SW_REG_NAME : PP_UC_ING_S2_TSSM_PREV_REG
 * HW_REG_NAME : reg_tssm_prev
 * DESCRIPTION : Holds the ID of the previous task, NOTE, B Step
 *               ONLY register
 *
 *  Register Fields :
 *   [31: 6][RO] - Reserved
 *   [ 5: 0][RO] - Holds the ID of the previous task
 *
 */
#define PP_UC_ING_S2_TSSM_PREV_REG              ((UC_ING_S2_BASE_ADDR) + 0x2100)
#define PP_UC_ING_S2_TSSM_PREV_RSVD0_OFF                            (6)
#define PP_UC_ING_S2_TSSM_PREV_RSVD0_LEN                            (26)
#define PP_UC_ING_S2_TSSM_PREV_RSVD0_MSK                            (0xFFFFFFC0)
#define PP_UC_ING_S2_TSSM_PREV_RSVD0_RST                            (0x0)
#define PP_UC_ING_S2_TSSM_PREV_TSSM_PREV_OFF                        (0)
#define PP_UC_ING_S2_TSSM_PREV_TSSM_PREV_LEN                        (6)
#define PP_UC_ING_S2_TSSM_PREV_TSSM_PREV_MSK                        (0x0000003F)
#define PP_UC_ING_S2_TSSM_PREV_TSSM_PREV_RST                        (0x0)

/**
 * SW_REG_NAME : PP_UC_ING_S2_TSSM_SLEEP_EN_REG
 * HW_REG_NAME : reg_tssm_sleep_en
 * DESCRIPTION : Sleep Enable, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 1][RO] - Reserved
 *   [ 0: 0][RW] - Sleep Enable
 *
 */
#define PP_UC_ING_S2_TSSM_SLEEP_EN_REG          ((UC_ING_S2_BASE_ADDR) + 0x2200)
#define PP_UC_ING_S2_TSSM_SLEEP_EN_RSVD0_OFF                        (1)
#define PP_UC_ING_S2_TSSM_SLEEP_EN_RSVD0_LEN                        (31)
#define PP_UC_ING_S2_TSSM_SLEEP_EN_RSVD0_MSK                        (0xFFFFFFFE)
#define PP_UC_ING_S2_TSSM_SLEEP_EN_RSVD0_RST                        (0x0)
#define PP_UC_ING_S2_TSSM_SLEEP_EN_TSSM_SLEEP_EN_OFF                (0)
#define PP_UC_ING_S2_TSSM_SLEEP_EN_TSSM_SLEEP_EN_LEN                (1)
#define PP_UC_ING_S2_TSSM_SLEEP_EN_TSSM_SLEEP_EN_MSK                (0x00000001)
#define PP_UC_ING_S2_TSSM_SLEEP_EN_TSSM_SLEEP_EN_RST                (0x0)

/**
 * SW_REG_NAME : PP_UC_ING_S2_TSSM_START_SM_REG
 * HW_REG_NAME : reg_tssm_start_sm
 * DESCRIPTION : Start Task Switch State Machine, NOTE, B Step ONLY
 *               register
 *
 *  Register Fields :
 *   [31: 1][RO] - Reserved
 *   [ 0: 0][RW] - Start Task Switch State Machine
 *
 */
#define PP_UC_ING_S2_TSSM_START_SM_REG          ((UC_ING_S2_BASE_ADDR) + 0x2300)
#define PP_UC_ING_S2_TSSM_START_SM_RSVD0_OFF                        (1)
#define PP_UC_ING_S2_TSSM_START_SM_RSVD0_LEN                        (31)
#define PP_UC_ING_S2_TSSM_START_SM_RSVD0_MSK                        (0xFFFFFFFE)
#define PP_UC_ING_S2_TSSM_START_SM_RSVD0_RST                        (0x0)
#define PP_UC_ING_S2_TSSM_START_SM_TSSM_START_SM_OFF                (0)
#define PP_UC_ING_S2_TSSM_START_SM_TSSM_START_SM_LEN                (1)
#define PP_UC_ING_S2_TSSM_START_SM_TSSM_START_SM_MSK                (0x00000001)
#define PP_UC_ING_S2_TSSM_START_SM_TSSM_START_SM_RST                (0x0)

/**
 * SW_REG_NAME : PP_UC_ING_S2_TSSM_WDT_VAL_REG
 * HW_REG_NAME : reg_tssm_wdt_val
 * DESCRIPTION : Watch dog initial value, NOTE, B Step ONLY
 *               register
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19: 0][RW] - Watch dog initial value
 *
 */
#define PP_UC_ING_S2_TSSM_WDT_VAL_REG           ((UC_ING_S2_BASE_ADDR) + 0x2400)
#define PP_UC_ING_S2_TSSM_WDT_VAL_RSVD0_OFF                         (20)
#define PP_UC_ING_S2_TSSM_WDT_VAL_RSVD0_LEN                         (12)
#define PP_UC_ING_S2_TSSM_WDT_VAL_RSVD0_MSK                         (0xFFF00000)
#define PP_UC_ING_S2_TSSM_WDT_VAL_RSVD0_RST                         (0x0)
#define PP_UC_ING_S2_TSSM_WDT_VAL_TSSM_WDT_VAL_OFF                  (0)
#define PP_UC_ING_S2_TSSM_WDT_VAL_TSSM_WDT_VAL_LEN                  (20)
#define PP_UC_ING_S2_TSSM_WDT_VAL_TSSM_WDT_VAL_MSK                  (0x000FFFFF)
#define PP_UC_ING_S2_TSSM_WDT_VAL_TSSM_WDT_VAL_RST                  (0x0)

/**
 * SW_REG_NAME : PP_UC_ING_S2_TSSM_WDT_RESET_REG
 * HW_REG_NAME : reg_tssm_wdt_reset
 * DESCRIPTION : Enables the uC to reset the WDT, NOTE, B Step ONLY
 *               register
 *
 *  Register Fields :
 *   [31: 1][RO] - Reserved
 *   [ 0: 0][RW] - Enables the uC to reset the WDT
 *
 */
#define PP_UC_ING_S2_TSSM_WDT_RESET_REG         ((UC_ING_S2_BASE_ADDR) + 0x2500)
#define PP_UC_ING_S2_TSSM_WDT_RESET_RSVD0_OFF                       (1)
#define PP_UC_ING_S2_TSSM_WDT_RESET_RSVD0_LEN                       (31)
#define PP_UC_ING_S2_TSSM_WDT_RESET_RSVD0_MSK                       (0xFFFFFFFE)
#define PP_UC_ING_S2_TSSM_WDT_RESET_RSVD0_RST                       (0x0)
#define PP_UC_ING_S2_TSSM_WDT_RESET_TSSM_WDT_RESET_OFF              (0)
#define PP_UC_ING_S2_TSSM_WDT_RESET_TSSM_WDT_RESET_LEN              (1)
#define PP_UC_ING_S2_TSSM_WDT_RESET_TSSM_WDT_RESET_MSK              (0x00000001)
#define PP_UC_ING_S2_TSSM_WDT_RESET_TSSM_WDT_RESET_RST              (0x0)

/**
 * SW_REG_NAME : PP_UC_ING_S2_TASKSCHED_NTID_REG
 * HW_REG_NAME : reg_tasksched_ntid
 * DESCRIPTION : Holds tha last result of the schedular
 *               calculation, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 6][RO] - Reserved
 *   [ 5: 0][RW] - Holds tha last result of the schedular calculation
 *
 */
#define PP_UC_ING_S2_TASKSCHED_NTID_REG         ((UC_ING_S2_BASE_ADDR) + 0x4000)
#define PP_UC_ING_S2_TASKSCHED_NTID_RSVD0_OFF                       (6)
#define PP_UC_ING_S2_TASKSCHED_NTID_RSVD0_LEN                       (26)
#define PP_UC_ING_S2_TASKSCHED_NTID_RSVD0_MSK                       (0xFFFFFFC0)
#define PP_UC_ING_S2_TASKSCHED_NTID_RSVD0_RST                       (0x0)
#define PP_UC_ING_S2_TASKSCHED_NTID_TASKSCHED_NTID_OFF              (0)
#define PP_UC_ING_S2_TASKSCHED_NTID_TASKSCHED_NTID_LEN              (6)
#define PP_UC_ING_S2_TASKSCHED_NTID_TASKSCHED_NTID_MSK              (0x0000003F)
#define PP_UC_ING_S2_TASKSCHED_NTID_TASKSCHED_NTID_RST              (0x0)

/**
 * SW_REG_NAME : PP_UC_ING_S2_TASKSCHED_EN_REG
 * HW_REG_NAME : reg_tasksched_enable
 * DESCRIPTION : 1 indicates that the TaskSched is running; 0
 *               enables to emulate the task scheduler, and update
 *               the NTID, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 1][RO] - Reserved
 *   [ 0: 0][RW] - 1 indicates that the TaskSched is running; 0
 *                 enables to emulate the task scheduler, and update
 *                 the NTID
 *
 */
#define PP_UC_ING_S2_TASKSCHED_EN_REG           ((UC_ING_S2_BASE_ADDR) + 0x4100)
#define PP_UC_ING_S2_TASKSCHED_EN_RSVD0_OFF                         (1)
#define PP_UC_ING_S2_TASKSCHED_EN_RSVD0_LEN                         (31)
#define PP_UC_ING_S2_TASKSCHED_EN_RSVD0_MSK                         (0xFFFFFFFE)
#define PP_UC_ING_S2_TASKSCHED_EN_RSVD0_RST                         (0x0)
#define PP_UC_ING_S2_TASKSCHED_EN_TASKSCHED_EN_OFF                  (0)
#define PP_UC_ING_S2_TASKSCHED_EN_TASKSCHED_EN_LEN                  (1)
#define PP_UC_ING_S2_TASKSCHED_EN_TASKSCHED_EN_MSK                  (0x00000001)
#define PP_UC_ING_S2_TASKSCHED_EN_TASKSCHED_EN_RST                  (0x1)

/**
 * SW_REG_NAME : PP_UC_ING_S2_TASKSCHED_ALGO_REG
 * HW_REG_NAME : reg_tasksched_algo
 * DESCRIPTION : Algo selection, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 3][RO] - Reserved
 *   [ 2: 0][RW] - Algo selection
 *
 */
#define PP_UC_ING_S2_TASKSCHED_ALGO_REG         ((UC_ING_S2_BASE_ADDR) + 0x4200)
#define PP_UC_ING_S2_TASKSCHED_ALGO_RSVD0_OFF                       (3)
#define PP_UC_ING_S2_TASKSCHED_ALGO_RSVD0_LEN                       (29)
#define PP_UC_ING_S2_TASKSCHED_ALGO_RSVD0_MSK                       (0xFFFFFFF8)
#define PP_UC_ING_S2_TASKSCHED_ALGO_RSVD0_RST                       (0x0)
#define PP_UC_ING_S2_TASKSCHED_ALGO_TASKSCHED_ALGO_OFF              (0)
#define PP_UC_ING_S2_TASKSCHED_ALGO_TASKSCHED_ALGO_LEN              (3)
#define PP_UC_ING_S2_TASKSCHED_ALGO_TASKSCHED_ALGO_MSK              (0x00000007)
#define PP_UC_ING_S2_TASKSCHED_ALGO_TASKSCHED_ALGO_RST              (0x0)

/**
 * SW_REG_NAME : PP_UC_ING_S2_MTSR_READY_REG
 * HW_REG_NAME : reg_mtsr_ready
 * DESCRIPTION : Ready bit per task, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 1][RO] - Reserved
 *   [ 0: 0][RW] - Ready bit per task
 *
 */
#define PP_UC_ING_S2_MTSR_READY_REG             ((UC_ING_S2_BASE_ADDR) + 0x6000)
#define PP_UC_ING_S2_MTSR_READY_RSVD0_OFF                           (1)
#define PP_UC_ING_S2_MTSR_READY_RSVD0_LEN                           (31)
#define PP_UC_ING_S2_MTSR_READY_RSVD0_MSK                           (0xFFFFFFFE)
#define PP_UC_ING_S2_MTSR_READY_RSVD0_RST                           (0x0)
#define PP_UC_ING_S2_MTSR_READY_MTSR_READY_OFF                      (0)
#define PP_UC_ING_S2_MTSR_READY_MTSR_READY_LEN                      (1)
#define PP_UC_ING_S2_MTSR_READY_MTSR_READY_MSK                      (0x00000001)
#define PP_UC_ING_S2_MTSR_READY_MTSR_READY_RST                      (0x0)
/**
 * REG_IDX_ACCESS   : PP_UC_ING_S2_MTSR_READY_REG_IDX
 * NUM OF REGISTERS : 16
 */
#define PP_UC_ING_S2_MTSR_READY_REG_IDX(idx) \
	(PP_UC_ING_S2_MTSR_READY_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_UC_ING_S2_MTSR_HIPRIOR_REG
 * HW_REG_NAME : reg_mtsr_hiprior
 * DESCRIPTION : Schedule this task in strict high priority, NOTE,
 *               B Step ONLY register
 *
 *  Register Fields :
 *   [31: 1][RO] - Reserved
 *   [ 0: 0][RW] - Schedule this task in strict high priority
 *
 */
#define PP_UC_ING_S2_MTSR_HIPRIOR_REG           ((UC_ING_S2_BASE_ADDR) + 0x6100)
#define PP_UC_ING_S2_MTSR_HIPRIOR_RSVD0_OFF                         (1)
#define PP_UC_ING_S2_MTSR_HIPRIOR_RSVD0_LEN                         (31)
#define PP_UC_ING_S2_MTSR_HIPRIOR_RSVD0_MSK                         (0xFFFFFFFE)
#define PP_UC_ING_S2_MTSR_HIPRIOR_RSVD0_RST                         (0x0)
#define PP_UC_ING_S2_MTSR_HIPRIOR_MTSR_HIPRIOR_OFF                  (0)
#define PP_UC_ING_S2_MTSR_HIPRIOR_MTSR_HIPRIOR_LEN                  (1)
#define PP_UC_ING_S2_MTSR_HIPRIOR_MTSR_HIPRIOR_MSK                  (0x00000001)
#define PP_UC_ING_S2_MTSR_HIPRIOR_MTSR_HIPRIOR_RST                  (0x0)
/**
 * REG_IDX_ACCESS   : PP_UC_ING_S2_MTSR_HIPRIOR_REG_IDX
 * NUM OF REGISTERS : 16
 */
#define PP_UC_ING_S2_MTSR_HIPRIOR_REG_IDX(idx) \
	(PP_UC_ING_S2_MTSR_HIPRIOR_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_UC_ING_S2_MTSR_CTID_REG
 * HW_REG_NAME : reg_mtsr_ctid
 * DESCRIPTION : Current Task ID, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 6][RO] - Reserved
 *   [ 5: 0][RO] - Current Task ID
 *
 */
#define PP_UC_ING_S2_MTSR_CTID_REG              ((UC_ING_S2_BASE_ADDR) + 0x6200)
#define PP_UC_ING_S2_MTSR_CTID_RSVD0_OFF                            (6)
#define PP_UC_ING_S2_MTSR_CTID_RSVD0_LEN                            (26)
#define PP_UC_ING_S2_MTSR_CTID_RSVD0_MSK                            (0xFFFFFFC0)
#define PP_UC_ING_S2_MTSR_CTID_RSVD0_RST                            (0x0)
#define PP_UC_ING_S2_MTSR_CTID_MTSR_CTID_OFF                        (0)
#define PP_UC_ING_S2_MTSR_CTID_MTSR_CTID_LEN                        (6)
#define PP_UC_ING_S2_MTSR_CTID_MTSR_CTID_MSK                        (0x0000003F)
#define PP_UC_ING_S2_MTSR_CTID_MTSR_CTID_RST                        (0x0)

/**
 * SW_REG_NAME : PP_UC_ING_S2_MTSR_QUSTAMP_REG
 * HW_REG_NAME : reg_mtsr_qustamp
 * DESCRIPTION : Queue Stamp, Get the counts from POPA, NOTE, B
 *               Step ONLY register
 *
 *  Register Fields :
 *   [31:16][RO] - Reserved
 *   [15: 0][RW] - Queue Stamp, Get the counts from POPA
 *
 */
#define PP_UC_ING_S2_MTSR_QUSTAMP_REG           ((UC_ING_S2_BASE_ADDR) + 0x6300)
#define PP_UC_ING_S2_MTSR_QUSTAMP_RSVD0_OFF                         (16)
#define PP_UC_ING_S2_MTSR_QUSTAMP_RSVD0_LEN                         (16)
#define PP_UC_ING_S2_MTSR_QUSTAMP_RSVD0_MSK                         (0xFFFF0000)
#define PP_UC_ING_S2_MTSR_QUSTAMP_RSVD0_RST                         (0x0)
#define PP_UC_ING_S2_MTSR_QUSTAMP_MTSR_QUSTAMP_OFF                  (0)
#define PP_UC_ING_S2_MTSR_QUSTAMP_MTSR_QUSTAMP_LEN                  (16)
#define PP_UC_ING_S2_MTSR_QUSTAMP_MTSR_QUSTAMP_MSK                  (0x0000FFFF)
#define PP_UC_ING_S2_MTSR_QUSTAMP_MTSR_QUSTAMP_RST                  (0x0)

/**
 * SW_REG_NAME : PP_UC_ING_S2_MTSR_CPU_SLEEP_CNT_LOW_REG
 * HW_REG_NAME : reg_mtsr_cpu_sleep_cnt_low
 * DESCRIPTION : CPU Sleep Counter Low, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][RW] - CPU Sleep Counter Low
 *
 */
#define PP_UC_ING_S2_MTSR_CPU_SLEEP_CNT_LOW_REG ((UC_ING_S2_BASE_ADDR) + 0x6400)
#define PP_UC_ING_S2_MTSR_CPU_SLEEP_CNT_LOW_CPU_SLEEP_L_OFF         (0)
#define PP_UC_ING_S2_MTSR_CPU_SLEEP_CNT_LOW_CPU_SLEEP_L_LEN         (32)
#define PP_UC_ING_S2_MTSR_CPU_SLEEP_CNT_LOW_CPU_SLEEP_L_MSK         (0xFFFFFFFF)
#define PP_UC_ING_S2_MTSR_CPU_SLEEP_CNT_LOW_CPU_SLEEP_L_RST         (0x0)

/**
 * SW_REG_NAME : PP_UC_ING_S2_MTSR_CPU_SLEEP_CNT_HIGH_REG
 * HW_REG_NAME : reg_mtsr_cpu_sleep_cnt_high
 * DESCRIPTION : CPU Sleep Counter High, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][RW] - CPU Sleep Counter High
 *
 */
#define PP_UC_ING_S2_MTSR_CPU_SLEEP_CNT_HIGH_REG \
	((UC_ING_S2_BASE_ADDR) + 0x6404)
#define PP_UC_ING_S2_MTSR_CPU_SLEEP_CNT_HIGH_CPU_SLEEP_H_OFF        (0)
#define PP_UC_ING_S2_MTSR_CPU_SLEEP_CNT_HIGH_CPU_SLEEP_H_LEN        (32)
#define PP_UC_ING_S2_MTSR_CPU_SLEEP_CNT_HIGH_CPU_SLEEP_H_MSK        (0xFFFFFFFF)
#define PP_UC_ING_S2_MTSR_CPU_SLEEP_CNT_HIGH_CPU_SLEEP_H_RST        (0x0)

/**
 * SW_REG_NAME : PP_UC_ING_S2_POPA_START_REG
 * HW_REG_NAME : reg_popa_start
 * DESCRIPTION : Start Operation, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 8][RO] - Reserved
 *   [ 7: 0][RW] - Start Operation
 *
 */
#define PP_UC_ING_S2_POPA_START_REG             ((UC_ING_S2_BASE_ADDR) + 0x8000)
#define PP_UC_ING_S2_POPA_START_RSVD0_OFF                           (8)
#define PP_UC_ING_S2_POPA_START_RSVD0_LEN                           (24)
#define PP_UC_ING_S2_POPA_START_RSVD0_MSK                           (0xFFFFFF00)
#define PP_UC_ING_S2_POPA_START_RSVD0_RST                           (0x0)
#define PP_UC_ING_S2_POPA_START_POPA_START_OFF                      (0)
#define PP_UC_ING_S2_POPA_START_POPA_START_LEN                      (8)
#define PP_UC_ING_S2_POPA_START_POPA_START_MSK                      (0x000000FF)
#define PP_UC_ING_S2_POPA_START_POPA_START_RST                      (0x0)

/**
 * SW_REG_NAME : PP_UC_ING_S2_POPA_MAP_REG
 * HW_REG_NAME : reg_popa_map
 * DESCRIPTION : reg_extinsjob_job_offset, NOTE, B Step ONLY
 *               register
 *
 *  Register Fields :
 *   [31: 6][RO] - Reserved
 *   [ 5: 0][RW] - reg_extinsjob_job_offset
 *
 */
#define PP_UC_ING_S2_POPA_MAP_REG               ((UC_ING_S2_BASE_ADDR) + 0x8100)
#define PP_UC_ING_S2_POPA_MAP_RSVD0_OFF                             (6)
#define PP_UC_ING_S2_POPA_MAP_RSVD0_LEN                             (26)
#define PP_UC_ING_S2_POPA_MAP_RSVD0_MSK                             (0xFFFFFFC0)
#define PP_UC_ING_S2_POPA_MAP_RSVD0_RST                             (0x0)
#define PP_UC_ING_S2_POPA_MAP_POPA_MAP_OFF                          (0)
#define PP_UC_ING_S2_POPA_MAP_POPA_MAP_LEN                          (6)
#define PP_UC_ING_S2_POPA_MAP_POPA_MAP_MSK                          (0x0000003F)
#define PP_UC_ING_S2_POPA_MAP_POPA_MAP_RST                          (0x0)
/**
 * REG_IDX_ACCESS   : PP_UC_ING_S2_POPA_MAP_REG_IDX
 * NUM OF REGISTERS : 16
 */
#define PP_UC_ING_S2_POPA_MAP_REG_IDX(idx) \
	(PP_UC_ING_S2_POPA_MAP_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_UC_ING_S2_POPA_TID_REG
 * HW_REG_NAME : reg_popa_tid
 * DESCRIPTION : Task ID, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 6][RO] - Reserved
 *   [ 5: 0][RO] - Task ID
 *
 */
#define PP_UC_ING_S2_POPA_TID_REG               ((UC_ING_S2_BASE_ADDR) + 0x8200)
#define PP_UC_ING_S2_POPA_TID_RSVD0_OFF                             (6)
#define PP_UC_ING_S2_POPA_TID_RSVD0_LEN                             (26)
#define PP_UC_ING_S2_POPA_TID_RSVD0_MSK                             (0xFFFFFFC0)
#define PP_UC_ING_S2_POPA_TID_RSVD0_RST                             (0x0)
#define PP_UC_ING_S2_POPA_TID_POPA_TID_OFF                          (0)
#define PP_UC_ING_S2_POPA_TID_POPA_TID_LEN                          (6)
#define PP_UC_ING_S2_POPA_TID_POPA_TID_MSK                          (0x0000003F)
#define PP_UC_ING_S2_POPA_TID_POPA_TID_RST                          (0x0)

/**
 * SW_REG_NAME : PP_UC_ING_S2_POPA_POP_TYPE_REG
 * HW_REG_NAME : reg_popa_pop_type
 * DESCRIPTION : POP Type, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 1][RO] - Reserved
 *   [ 0: 0][RO] - POP Type
 *
 */
#define PP_UC_ING_S2_POPA_POP_TYPE_REG          ((UC_ING_S2_BASE_ADDR) + 0x8300)
#define PP_UC_ING_S2_POPA_POP_TYPE_RSVD0_OFF                        (1)
#define PP_UC_ING_S2_POPA_POP_TYPE_RSVD0_LEN                        (31)
#define PP_UC_ING_S2_POPA_POP_TYPE_RSVD0_MSK                        (0xFFFFFFFE)
#define PP_UC_ING_S2_POPA_POP_TYPE_RSVD0_RST                        (0x0)
#define PP_UC_ING_S2_POPA_POP_TYPE_POPA_POP_TYPE_OFF                (0)
#define PP_UC_ING_S2_POPA_POP_TYPE_POPA_POP_TYPE_LEN                (1)
#define PP_UC_ING_S2_POPA_POP_TYPE_POPA_POP_TYPE_MSK                (0x00000001)
#define PP_UC_ING_S2_POPA_POP_TYPE_POPA_POP_TYPE_RST                (0x0)
/**
 * REG_IDX_ACCESS   : PP_UC_ING_S2_POPA_POP_TYPE_REG_IDX
 * NUM OF REGISTERS : 16
 */
#define PP_UC_ING_S2_POPA_POP_TYPE_REG_IDX(idx) \
	(PP_UC_ING_S2_POPA_POP_TYPE_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_UC_ING_S2_POPA_JOB_IDX_REG
 * HW_REG_NAME : reg_popa_job_idx
 * DESCRIPTION : Read Pointer Job Index, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 8][RO] - Reserved
 *   [ 7: 0][RO] - Read Pointer Job Index
 *
 */
#define PP_UC_ING_S2_POPA_JOB_IDX_REG           ((UC_ING_S2_BASE_ADDR) + 0x8400)
#define PP_UC_ING_S2_POPA_JOB_IDX_RSVD0_OFF                         (8)
#define PP_UC_ING_S2_POPA_JOB_IDX_RSVD0_LEN                         (24)
#define PP_UC_ING_S2_POPA_JOB_IDX_RSVD0_MSK                         (0xFFFFFF00)
#define PP_UC_ING_S2_POPA_JOB_IDX_RSVD0_RST                         (0x0)
#define PP_UC_ING_S2_POPA_JOB_IDX_POPA_JOB_IDX_OFF                  (0)
#define PP_UC_ING_S2_POPA_JOB_IDX_POPA_JOB_IDX_LEN                  (8)
#define PP_UC_ING_S2_POPA_JOB_IDX_POPA_JOB_IDX_MSK                  (0x000000FF)
#define PP_UC_ING_S2_POPA_JOB_IDX_POPA_JOB_IDX_RST                  (0x0)
/**
 * REG_IDX_ACCESS   : PP_UC_ING_S2_POPA_JOB_IDX_REG_IDX
 * NUM OF REGISTERS : 2
 */
#define PP_UC_ING_S2_POPA_JOB_IDX_REG_IDX(idx) \
	(PP_UC_ING_S2_POPA_JOB_IDX_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_UC_ING_S2_POPA_ACTIVE_LSB_REG
 * HW_REG_NAME : reg_popa_active_lsb
 * DESCRIPTION : POPA Active, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][RO] - POPA Active
 *
 */
#define PP_UC_ING_S2_POPA_ACTIVE_LSB_REG        ((UC_ING_S2_BASE_ADDR) + 0x8600)
#define PP_UC_ING_S2_POPA_ACTIVE_LSB_POPA_ACTIVE_LSB_OFF            (0)
#define PP_UC_ING_S2_POPA_ACTIVE_LSB_POPA_ACTIVE_LSB_LEN            (32)
#define PP_UC_ING_S2_POPA_ACTIVE_LSB_POPA_ACTIVE_LSB_MSK            (0xFFFFFFFF)
#define PP_UC_ING_S2_POPA_ACTIVE_LSB_POPA_ACTIVE_LSB_RST            (0x0)
/**
 * REG_IDX_ACCESS   : PP_UC_ING_S2_POPA_ACTIVE_LSB_REG_IDX
 * NUM OF REGISTERS : 2
 */
#define PP_UC_ING_S2_POPA_ACTIVE_LSB_REG_IDX(idx) \
	(PP_UC_ING_S2_POPA_ACTIVE_LSB_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_UC_ING_S2_POPA_ACTIVE_MSB_REG
 * HW_REG_NAME : reg_popa_active_msb
 * DESCRIPTION : POPA Active, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][RO] - POPA Active
 *
 */
#define PP_UC_ING_S2_POPA_ACTIVE_MSB_REG        ((UC_ING_S2_BASE_ADDR) + 0x8700)
#define PP_UC_ING_S2_POPA_ACTIVE_MSB_POPA_ACTIVE_MSB_OFF            (0)
#define PP_UC_ING_S2_POPA_ACTIVE_MSB_POPA_ACTIVE_MSB_LEN            (32)
#define PP_UC_ING_S2_POPA_ACTIVE_MSB_POPA_ACTIVE_MSB_MSK            (0xFFFFFFFF)
#define PP_UC_ING_S2_POPA_ACTIVE_MSB_POPA_ACTIVE_MSB_RST            (0x0)
/**
 * REG_IDX_ACCESS   : PP_UC_ING_S2_POPA_ACTIVE_MSB_REG_IDX
 * NUM OF REGISTERS : 2
 */
#define PP_UC_ING_S2_POPA_ACTIVE_MSB_REG_IDX(idx) \
	(PP_UC_ING_S2_POPA_ACTIVE_MSB_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_UC_ING_S2_POPA_NEW_IN_Q_REG
 * HW_REG_NAME : reg_popa_new_in_queue
 * DESCRIPTION : New In Queue, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 8][RO] - Reserved
 *   [ 7: 0][RO] - New In Queue
 *
 */
#define PP_UC_ING_S2_POPA_NEW_IN_Q_REG          ((UC_ING_S2_BASE_ADDR) + 0x8800)
#define PP_UC_ING_S2_POPA_NEW_IN_Q_RSVD0_OFF                        (8)
#define PP_UC_ING_S2_POPA_NEW_IN_Q_RSVD0_LEN                        (24)
#define PP_UC_ING_S2_POPA_NEW_IN_Q_RSVD0_MSK                        (0xFFFFFF00)
#define PP_UC_ING_S2_POPA_NEW_IN_Q_RSVD0_RST                        (0x0)
#define PP_UC_ING_S2_POPA_NEW_IN_Q_POPA_NEW_IN_Q_OFF                (0)
#define PP_UC_ING_S2_POPA_NEW_IN_Q_POPA_NEW_IN_Q_LEN                (8)
#define PP_UC_ING_S2_POPA_NEW_IN_Q_POPA_NEW_IN_Q_MSK                (0x000000FF)
#define PP_UC_ING_S2_POPA_NEW_IN_Q_POPA_NEW_IN_Q_RST                (0x0)
/**
 * REG_IDX_ACCESS   : PP_UC_ING_S2_POPA_NEW_IN_Q_REG_IDX
 * NUM OF REGISTERS : 2
 */
#define PP_UC_ING_S2_POPA_NEW_IN_Q_REG_IDX(idx) \
	(PP_UC_ING_S2_POPA_NEW_IN_Q_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_UC_ING_S2_POPA_LAST_JOB_REG
 * HW_REG_NAME : reg_popa_last_job
 * DESCRIPTION : Last Job (RJOB/RJOB2);holds the last updated
 *               JOB_BASE, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31:16][RO] - Reserved
 *   [15: 0][RO] - Last Job (RJOB/RJOB2);holds the last updated
 *                 JOB_BASE
 *
 */
#define PP_UC_ING_S2_POPA_LAST_JOB_REG          ((UC_ING_S2_BASE_ADDR) + 0x8900)
#define PP_UC_ING_S2_POPA_LAST_JOB_RSVD0_OFF                        (16)
#define PP_UC_ING_S2_POPA_LAST_JOB_RSVD0_LEN                        (16)
#define PP_UC_ING_S2_POPA_LAST_JOB_RSVD0_MSK                        (0xFFFF0000)
#define PP_UC_ING_S2_POPA_LAST_JOB_RSVD0_RST                        (0x0)
#define PP_UC_ING_S2_POPA_LAST_JOB_POPA_LAST_JOB_OFF                (0)
#define PP_UC_ING_S2_POPA_LAST_JOB_POPA_LAST_JOB_LEN                (16)
#define PP_UC_ING_S2_POPA_LAST_JOB_POPA_LAST_JOB_MSK                (0x0000FFFF)
#define PP_UC_ING_S2_POPA_LAST_JOB_POPA_LAST_JOB_RST                (0x0)

/**
 * SW_REG_NAME : PP_UC_ING_S2_PUSHE_PUTJOB_REG
 * HW_REG_NAME : reg_pushe_putjob
 * DESCRIPTION : Last Job (RJOB/RJOB2);holds the last updated
 *               JOB_BASE, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31:16][RO] - Reserved
 *   [15: 0][RW] - Used by task to push a job
 *
 */
#define PP_UC_ING_S2_PUSHE_PUTJOB_REG          ((UC_ING_S2_BASE_ADDR) + 0x0A000)
#define PP_UC_ING_S2_PUSHE_PUTJOB_RSVD0_OFF                         (16)
#define PP_UC_ING_S2_PUSHE_PUTJOB_RSVD0_LEN                         (16)
#define PP_UC_ING_S2_PUSHE_PUTJOB_RSVD0_MSK                         (0xFFFF0000)
#define PP_UC_ING_S2_PUSHE_PUTJOB_RSVD0_RST                         (0x0)
#define PP_UC_ING_S2_PUSHE_PUTJOB_PUSHE_PUTJOB_OFF                  (0)
#define PP_UC_ING_S2_PUSHE_PUTJOB_PUSHE_PUTJOB_LEN                  (16)
#define PP_UC_ING_S2_PUSHE_PUTJOB_PUSHE_PUTJOB_MSK                  (0x0000FFFF)
#define PP_UC_ING_S2_PUSHE_PUTJOB_PUSHE_PUTJOB_RST                  (0x0)

/**
 * SW_REG_NAME : PP_UC_ING_S2_PUSHE_JOB_DONE_REG
 * HW_REG_NAME : reg_pushe_job_done
 * DESCRIPTION : MISSING Description, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 1][RO] - Reserved
 *   [ 0: 0][RO] - Job_done[k][j]=1 indicates that job j in queue k
 *                 is done and can be forwarded to the Reader
 *
 */
#define PP_UC_ING_S2_PUSHE_JOB_DONE_REG        ((UC_ING_S2_BASE_ADDR) + 0x0A100)
#define PP_UC_ING_S2_PUSHE_JOB_DONE_RSVD0_OFF                       (1)
#define PP_UC_ING_S2_PUSHE_JOB_DONE_RSVD0_LEN                       (31)
#define PP_UC_ING_S2_PUSHE_JOB_DONE_RSVD0_MSK                       (0xFFFFFFFE)
#define PP_UC_ING_S2_PUSHE_JOB_DONE_RSVD0_RST                       (0x0)
#define PP_UC_ING_S2_PUSHE_JOB_DONE_PUSHE_JOB_DONE_OFF              (0)
#define PP_UC_ING_S2_PUSHE_JOB_DONE_PUSHE_JOB_DONE_LEN              (1)
#define PP_UC_ING_S2_PUSHE_JOB_DONE_PUSHE_JOB_DONE_MSK              (0x00000001)
#define PP_UC_ING_S2_PUSHE_JOB_DONE_PUSHE_JOB_DONE_RST              (0x0)
/**
 * REG_IDX_ACCESS   : PP_UC_ING_S2_PUSHE_JOB_DONE_REG_IDX
 * NUM OF REGISTERS : 12
 */
#define PP_UC_ING_S2_PUSHE_JOB_DONE_REG_IDX(idx) \
	(PP_UC_ING_S2_PUSHE_JOB_DONE_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_UC_ING_S2_BGCOPY_DMA_REQ_REG
 * HW_REG_NAME : reg_bgcopy_dma_req
 * DESCRIPTION : BGCOPY DMAE Request, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31:12][RO] - Reserved
 *   [11: 0][RW] - Holds tid, rd, p, see HAS
 *
 */
#define PP_UC_ING_S2_BGCOPY_DMA_REQ_REG        ((UC_ING_S2_BASE_ADDR) + 0x0C000)
#define PP_UC_ING_S2_BGCOPY_DMA_REQ_RSVD0_OFF                       (12)
#define PP_UC_ING_S2_BGCOPY_DMA_REQ_RSVD0_LEN                       (20)
#define PP_UC_ING_S2_BGCOPY_DMA_REQ_RSVD0_MSK                       (0xFFFFF000)
#define PP_UC_ING_S2_BGCOPY_DMA_REQ_RSVD0_RST                       (0x0)
#define PP_UC_ING_S2_BGCOPY_DMA_REQ_BGCOPY_DMAE_REQ_OFF             (0)
#define PP_UC_ING_S2_BGCOPY_DMA_REQ_BGCOPY_DMAE_REQ_LEN             (12)
#define PP_UC_ING_S2_BGCOPY_DMA_REQ_BGCOPY_DMAE_REQ_MSK             (0x00000FFF)
#define PP_UC_ING_S2_BGCOPY_DMA_REQ_BGCOPY_DMAE_REQ_RST             (0x0)

/**
 * SW_REG_NAME : PP_UC_ING_S2_DELAYA_START_REG
 * HW_REG_NAME : reg_delaya_start
 * DESCRIPTION : Trigger a delay operation, NOTE, B Step ONLY
 *               register
 *
 *  Register Fields :
 *   [31: 0][RW] - Trigger a delay operation
 *
 */
#define PP_UC_ING_S2_DELAYA_START_REG          ((UC_ING_S2_BASE_ADDR) + 0x0E000)
#define PP_UC_ING_S2_DELAYA_START_DELAYA_START_OFF                  (0)
#define PP_UC_ING_S2_DELAYA_START_DELAYA_START_LEN                  (32)
#define PP_UC_ING_S2_DELAYA_START_DELAYA_START_MSK                  (0xFFFFFFFF)
#define PP_UC_ING_S2_DELAYA_START_DELAYA_START_RST                  (0x0)

/**
 * SW_REG_NAME : PP_UC_ING_S2_DELAYA_SCALE_REG
 * HW_REG_NAME : reg_delaya_scale
 * DESCRIPTION : Scale Factor, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 5][RO] - Reserved
 *   [ 4: 0][RW] - Scale Factor
 *
 */
#define PP_UC_ING_S2_DELAYA_SCALE_REG          ((UC_ING_S2_BASE_ADDR) + 0x0E100)
#define PP_UC_ING_S2_DELAYA_SCALE_RSVD0_OFF                         (5)
#define PP_UC_ING_S2_DELAYA_SCALE_RSVD0_LEN                         (27)
#define PP_UC_ING_S2_DELAYA_SCALE_RSVD0_MSK                         (0xFFFFFFE0)
#define PP_UC_ING_S2_DELAYA_SCALE_RSVD0_RST                         (0x0)
#define PP_UC_ING_S2_DELAYA_SCALE_DELAYA_SCALE_OFF                  (0)
#define PP_UC_ING_S2_DELAYA_SCALE_DELAYA_SCALE_LEN                  (5)
#define PP_UC_ING_S2_DELAYA_SCALE_DELAYA_SCALE_MSK                  (0x0000001F)
#define PP_UC_ING_S2_DELAYA_SCALE_DELAYA_SCALE_RST                  (0x5)

/**
 * SW_REG_NAME : PP_UC_ING_S2_DELAYA_ABORT_REG
 * HW_REG_NAME : reg_delaya_abort
 * DESCRIPTION : Unblock a task, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 6][RO] - Reserved
 *   [ 5: 0][RW] - Unblock a task
 *
 */
#define PP_UC_ING_S2_DELAYA_ABORT_REG          ((UC_ING_S2_BASE_ADDR) + 0x0E200)
#define PP_UC_ING_S2_DELAYA_ABORT_RSVD0_OFF                         (6)
#define PP_UC_ING_S2_DELAYA_ABORT_RSVD0_LEN                         (26)
#define PP_UC_ING_S2_DELAYA_ABORT_RSVD0_MSK                         (0xFFFFFFC0)
#define PP_UC_ING_S2_DELAYA_ABORT_RSVD0_RST                         (0x0)
#define PP_UC_ING_S2_DELAYA_ABORT_DELAYA_ABORT_OFF                  (0)
#define PP_UC_ING_S2_DELAYA_ABORT_DELAYA_ABORT_LEN                  (6)
#define PP_UC_ING_S2_DELAYA_ABORT_DELAYA_ABORT_MSK                  (0x0000003F)
#define PP_UC_ING_S2_DELAYA_ABORT_DELAYA_ABORT_RST                  (0x0)

#endif
