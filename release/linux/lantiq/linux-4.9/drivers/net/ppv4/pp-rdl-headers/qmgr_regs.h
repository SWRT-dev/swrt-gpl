/**
 * qmgr_regs.h
 * Description: qmgr_s HW registers definitions
 * Auto-generated file by RDL Parser, DO NOT CHANGE
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2019-2020 Intel Corporation
 */

#ifndef _PP_QMGR_H_
#define _PP_QMGR_H_

#define PP_QMGR_GEN_DATE_STR                  "Sunday May 17, 2020 [5:26:11 pm]"
#define PP_QMGR_BASE                                             (0xF11C0000ULL)

/**
 * SW_REG_NAME : PP_QMGR_Q_DIVERSION_REG
 * HW_REG_NAME : queue_diversion
 * DESCRIPTION : Divert content from source to destination queue
 *
 *  Register Fields :
 *   [31:31][RW] - Set this field to start divert
 *   [30:30][RO] - When set setting divert_cmd will start divert
 *   [29:24][RO] - Reserved
 *   [23:23][RW] - 1: Head divert, 0: Tail divert
 *   [22:21][RO] - Reserved
 *   [20:12][RW] - Destination queue number
 *   [11: 9][RO] - Reserved
 *   [ 8: 0][RW] - Source queue number
 *
 */
#define PP_QMGR_Q_DIVERSION_REG                         ((QMGR_BASE_ADDR) + 0x0)
#define PP_QMGR_Q_DIVERSION_DIVERT_CMD_OFF                          (31)
#define PP_QMGR_Q_DIVERSION_DIVERT_CMD_LEN                          (1)
#define PP_QMGR_Q_DIVERSION_DIVERT_CMD_MSK                          (0x80000000)
#define PP_QMGR_Q_DIVERSION_DIVERT_CMD_RST                          (0x0)
#define PP_QMGR_Q_DIVERSION_DIVERT_AVAILABLE_OFF                    (30)
#define PP_QMGR_Q_DIVERSION_DIVERT_AVAILABLE_LEN                    (1)
#define PP_QMGR_Q_DIVERSION_DIVERT_AVAILABLE_MSK                    (0x40000000)
#define PP_QMGR_Q_DIVERSION_DIVERT_AVAILABLE_RST                    (0x1)
#define PP_QMGR_Q_DIVERSION_RSVD0_OFF                               (24)
#define PP_QMGR_Q_DIVERSION_RSVD0_LEN                               (6)
#define PP_QMGR_Q_DIVERSION_RSVD0_MSK                               (0x3F000000)
#define PP_QMGR_Q_DIVERSION_RSVD0_RST                               (0x0)
#define PP_QMGR_Q_DIVERSION_DIVERT_HEAD_OFF                         (23)
#define PP_QMGR_Q_DIVERSION_DIVERT_HEAD_LEN                         (1)
#define PP_QMGR_Q_DIVERSION_DIVERT_HEAD_MSK                         (0x00800000)
#define PP_QMGR_Q_DIVERSION_DIVERT_HEAD_RST                         (0x0)
#define PP_QMGR_Q_DIVERSION_RSVD1_OFF                               (21)
#define PP_QMGR_Q_DIVERSION_RSVD1_LEN                               (2)
#define PP_QMGR_Q_DIVERSION_RSVD1_MSK                               (0x00600000)
#define PP_QMGR_Q_DIVERSION_RSVD1_RST                               (0x0)
#define PP_QMGR_Q_DIVERSION_DST_QNUM_OFF                            (12)
#define PP_QMGR_Q_DIVERSION_DST_QNUM_LEN                            (9)
#define PP_QMGR_Q_DIVERSION_DST_QNUM_MSK                            (0x001FF000)
#define PP_QMGR_Q_DIVERSION_DST_QNUM_RST                            (0x0)
#define PP_QMGR_Q_DIVERSION_RSVD2_OFF                               (9)
#define PP_QMGR_Q_DIVERSION_RSVD2_LEN                               (3)
#define PP_QMGR_Q_DIVERSION_RSVD2_MSK                               (0x00000E00)
#define PP_QMGR_Q_DIVERSION_RSVD2_RST                               (0x0)
#define PP_QMGR_Q_DIVERSION_SRC_QNUM_OFF                            (0)
#define PP_QMGR_Q_DIVERSION_SRC_QNUM_LEN                            (9)
#define PP_QMGR_Q_DIVERSION_SRC_QNUM_MSK                            (0x000001FF)
#define PP_QMGR_Q_DIVERSION_SRC_QNUM_RST                            (0x0)

/**
 * SW_REG_NAME : PP_QMGR_DEL_EN_REG
 * HW_REG_NAME : delete_enable
 * DESCRIPTION : Enable delete operation when NULL descriptor is
 *               pushed
 *
 *  Register Fields :
 *   [31: 1][RO] - Reserved
 *   [ 0: 0][RW] - Delete Enable
 *
 */
#define PP_QMGR_DEL_EN_REG                              ((QMGR_BASE_ADDR) + 0x4)
#define PP_QMGR_DEL_EN_RSVD0_OFF                                    (1)
#define PP_QMGR_DEL_EN_RSVD0_LEN                                    (31)
#define PP_QMGR_DEL_EN_RSVD0_MSK                                    (0xFFFFFFFE)
#define PP_QMGR_DEL_EN_RSVD0_RST                                    (0x0)
#define PP_QMGR_DEL_EN_OFF                                          (0)
#define PP_QMGR_DEL_EN_LEN                                          (1)
#define PP_QMGR_DEL_EN_MSK                                          (0x00000001)
#define PP_QMGR_DEL_EN_RST                                          (0x0)

/**
 * SW_REG_NAME : PP_QMGR_DDR_BASE_ADDR_REG
 * HW_REG_NAME : ddr_base_addr
 * DESCRIPTION : Base Address of QMGR area in DDR
 *
 *  Register Fields :
 *   [31: 0][RW] - 32 upper bits of 36b DDR Base Address (the lowest
 *                 4 bits are zero)
 *
 */
#define PP_QMGR_DDR_BASE_ADDR_REG                       ((QMGR_BASE_ADDR) + 0x8)
#define PP_QMGR_DDR_BASE_ADDR_OFF                                   (0)
#define PP_QMGR_DDR_BASE_ADDR_LEN                                   (32)
#define PP_QMGR_DDR_BASE_ADDR_MSK                                   (0xFFFFFFFF)
#define PP_QMGR_DDR_BASE_ADDR_RST                                   (0x0)

/**
 * SW_REG_NAME : PP_QMGR_NUM_OF_PREFETCH_OUTSTANDING_REG
 * HW_REG_NAME : num_of_prefetch_outstanding
 * DESCRIPTION : Number of allowed Prefetch outstanding pops
 *
 *  Register Fields :
 *   [31: 5][RO] - Reserved
 *   [ 4: 0][RW] - Number of Outstanding Requests the prefetch unit
 *                 is allowed to pop
 *
 */
#define PP_QMGR_NUM_OF_PREFETCH_OUTSTANDING_REG        ((QMGR_BASE_ADDR) + 0x0C)
#define PP_QMGR_NUM_OF_PREFETCH_OUTSTANDING_RSVD0_OFF               (5)
#define PP_QMGR_NUM_OF_PREFETCH_OUTSTANDING_RSVD0_LEN               (27)
#define PP_QMGR_NUM_OF_PREFETCH_OUTSTANDING_RSVD0_MSK               (0xFFFFFFE0)
#define PP_QMGR_NUM_OF_PREFETCH_OUTSTANDING_RSVD0_RST               (0x0)
#define PP_QMGR_NUM_OF_PREFETCH_OUTSTANDING_OS_NUM_OFF              (0)
#define PP_QMGR_NUM_OF_PREFETCH_OUTSTANDING_OS_NUM_LEN              (5)
#define PP_QMGR_NUM_OF_PREFETCH_OUTSTANDING_OS_NUM_MSK              (0x0000001F)
#define PP_QMGR_NUM_OF_PREFETCH_OUTSTANDING_OS_NUM_RST              (0x10)

/**
 * SW_REG_NAME : PP_QMGR_CACHE_CLEAN_THR_REG
 * HW_REG_NAME : cache_clean_threshold
 * DESCRIPTION : The number of free pages in cache that Cache-
 *               Manager has to get equal or below before starting
 *               to transfer pages to DDR
 *
 *  Register Fields :
 *   [31:12][RO] - Reserved
 *   [11: 0][RW] - Threshold for CM
 *
 */
#define PP_QMGR_CACHE_CLEAN_THR_REG                    ((QMGR_BASE_ADDR) + 0x10)
#define PP_QMGR_CACHE_CLEAN_THR_RSVD0_OFF                           (12)
#define PP_QMGR_CACHE_CLEAN_THR_RSVD0_LEN                           (20)
#define PP_QMGR_CACHE_CLEAN_THR_RSVD0_MSK                           (0xFFFFF000)
#define PP_QMGR_CACHE_CLEAN_THR_RSVD0_RST                           (0x0)
#define PP_QMGR_CACHE_CLEAN_THR_CACHE_TH_OFF                        (0)
#define PP_QMGR_CACHE_CLEAN_THR_CACHE_TH_LEN                        (12)
#define PP_QMGR_CACHE_CLEAN_THR_CACHE_TH_MSK                        (0x00000FFF)
#define PP_QMGR_CACHE_CLEAN_THR_CACHE_TH_RST                        (0x0)

/**
 * SW_REG_NAME : PP_QMGR_FREE_Q_AEMPTY_THR_REG
 * HW_REG_NAME : free_q_aempty_threshold
 * DESCRIPTION : The number of free pages in DDR below it+1 the
 *               aempty indication is asserted
 *
 *  Register Fields :
 *   [31:17][RO] - Reserved
 *   [16: 0][RW] - Free Q aempty low threshold
 *
 */
#define PP_QMGR_FREE_Q_AEMPTY_THR_REG                  ((QMGR_BASE_ADDR) + 0x14)
#define PP_QMGR_FREE_Q_AEMPTY_THR_RSVD0_OFF                         (17)
#define PP_QMGR_FREE_Q_AEMPTY_THR_RSVD0_LEN                         (15)
#define PP_QMGR_FREE_Q_AEMPTY_THR_RSVD0_MSK                         (0xFFFE0000)
#define PP_QMGR_FREE_Q_AEMPTY_THR_RSVD0_RST                         (0x0)
#define PP_QMGR_FREE_Q_AEMPTY_THR_OFF                               (0)
#define PP_QMGR_FREE_Q_AEMPTY_THR_LEN                               (17)
#define PP_QMGR_FREE_Q_AEMPTY_THR_MSK                               (0x0001FFFF)
#define PP_QMGR_FREE_Q_AEMPTY_THR_RST                               (0x20)

/**
 * SW_REG_NAME : PP_QMGR_CACHE_FREE_CNT_REG
 * HW_REG_NAME : cache_free_cnt
 * DESCRIPTION : The number of free pages in Cache
 *
 *  Register Fields :
 *   [31:12][RO] - Reserved
 *   [11: 0][RO] - Cache Free Counter
 *
 */
#define PP_QMGR_CACHE_FREE_CNT_REG                     ((QMGR_BASE_ADDR) + 0x18)
#define PP_QMGR_CACHE_FREE_CNT_RSVD0_OFF                            (12)
#define PP_QMGR_CACHE_FREE_CNT_RSVD0_LEN                            (20)
#define PP_QMGR_CACHE_FREE_CNT_RSVD0_MSK                            (0xFFFFF000)
#define PP_QMGR_CACHE_FREE_CNT_RSVD0_RST                            (0x0)
#define PP_QMGR_CACHE_FREE_CNT_OFF                                  (0)
#define PP_QMGR_CACHE_FREE_CNT_LEN                                  (12)
#define PP_QMGR_CACHE_FREE_CNT_MSK                                  (0x00000FFF)
#define PP_QMGR_CACHE_FREE_CNT_RST                                  (0x480)

/**
 * SW_REG_NAME : PP_QMGR_FREE_EMPTY_INTR_REG
 * HW_REG_NAME : free_empty_interrupt
 * DESCRIPTION : Interupt if the cache free queue or the ddr free
 *               queue are empty
 *
 *  Register Fields :
 *   [31: 3][RO] - Reserved
 *   [ 2: 2][RO] - Push drop interrupt - Read clear
 *   [ 1: 1][RO] - DDR Free Empty Interrupt - Read clear
 *   [ 0: 0][RO] - Cache Free Empty Interrupt - Read clear
 *
 */
#define PP_QMGR_FREE_EMPTY_INTR_REG                    ((QMGR_BASE_ADDR) + 0x1C)
#define PP_QMGR_FREE_EMPTY_INTR_RSVD0_OFF                           (3)
#define PP_QMGR_FREE_EMPTY_INTR_RSVD0_LEN                           (29)
#define PP_QMGR_FREE_EMPTY_INTR_RSVD0_MSK                           (0xFFFFFFF8)
#define PP_QMGR_FREE_EMPTY_INTR_RSVD0_RST                           (0x0)
#define PP_QMGR_FREE_EMPTY_INTR_PUSH_DROPPED_INT_OFF                (2)
#define PP_QMGR_FREE_EMPTY_INTR_PUSH_DROPPED_INT_LEN                (1)
#define PP_QMGR_FREE_EMPTY_INTR_PUSH_DROPPED_INT_MSK                (0x00000004)
#define PP_QMGR_FREE_EMPTY_INTR_PUSH_DROPPED_INT_RST                (0x0)
#define PP_QMGR_FREE_EMPTY_INTR_DDR_FREE_EMPTY_INT_OFF              (1)
#define PP_QMGR_FREE_EMPTY_INTR_DDR_FREE_EMPTY_INT_LEN              (1)
#define PP_QMGR_FREE_EMPTY_INTR_DDR_FREE_EMPTY_INT_MSK              (0x00000002)
#define PP_QMGR_FREE_EMPTY_INTR_DDR_FREE_EMPTY_INT_RST              (0x0)
#define PP_QMGR_FREE_EMPTY_INTR_CACHE_FREE_EMPTY_INT_OFF            (0)
#define PP_QMGR_FREE_EMPTY_INTR_CACHE_FREE_EMPTY_INT_LEN            (1)
#define PP_QMGR_FREE_EMPTY_INTR_CACHE_FREE_EMPTY_INT_MSK            (0x00000001)
#define PP_QMGR_FREE_EMPTY_INTR_CACHE_FREE_EMPTY_INT_RST            (0x0)

/**
 * SW_REG_NAME : PP_QMGR_SM_STATE_REG
 * HW_REG_NAME : sm_state
 * DESCRIPTION : Current state of various state machines inside
 *               QMGR
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19:15][RO] - PQMAC state machine current_state
 *   [14:13][RO] - prefetch_unit divert state machine current state
 *   [12:10][RO] - page_pop divert state machine current state
 *   [ 9: 9][RO] - page_alloc state machine current state
 *   [ 8: 7][RO] - Reserved
 *   [ 6: 5][RO] - divert_sm current state
 *   [ 4: 3][RO] - cmd_dist delete state machine current state
 *   [ 2: 0][RO] - cmd_dist divert state machine current state
 *
 */
#define PP_QMGR_SM_STATE_REG                           ((QMGR_BASE_ADDR) + 0x20)
#define PP_QMGR_SM_STATE_RSVD0_OFF                                  (20)
#define PP_QMGR_SM_STATE_RSVD0_LEN                                  (12)
#define PP_QMGR_SM_STATE_RSVD0_MSK                                  (0xFFF00000)
#define PP_QMGR_SM_STATE_RSVD0_RST                                  (0x0)
#define PP_QMGR_SM_STATE_PQM_STATE_OFF                              (15)
#define PP_QMGR_SM_STATE_PQM_STATE_LEN                              (5)
#define PP_QMGR_SM_STATE_PQM_STATE_MSK                              (0x000F8000)
#define PP_QMGR_SM_STATE_PQM_STATE_RST                              (0x0)
#define PP_QMGR_SM_STATE_PREFETCH_UNIT_DIVERT_STATE_OFF             (13)
#define PP_QMGR_SM_STATE_PREFETCH_UNIT_DIVERT_STATE_LEN             (2)
#define PP_QMGR_SM_STATE_PREFETCH_UNIT_DIVERT_STATE_MSK             (0x00006000)
#define PP_QMGR_SM_STATE_PREFETCH_UNIT_DIVERT_STATE_RST             (0x0)
#define PP_QMGR_SM_STATE_PAGE_POP_DIVERT_STATE_OFF                  (10)
#define PP_QMGR_SM_STATE_PAGE_POP_DIVERT_STATE_LEN                  (3)
#define PP_QMGR_SM_STATE_PAGE_POP_DIVERT_STATE_MSK                  (0x00001C00)
#define PP_QMGR_SM_STATE_PAGE_POP_DIVERT_STATE_RST                  (0x0)
#define PP_QMGR_SM_STATE_PAGE_ALLOC_STATE_OFF                       (9)
#define PP_QMGR_SM_STATE_PAGE_ALLOC_STATE_LEN                       (1)
#define PP_QMGR_SM_STATE_PAGE_ALLOC_STATE_MSK                       (0x00000200)
#define PP_QMGR_SM_STATE_PAGE_ALLOC_STATE_RST                       (0x0)
#define PP_QMGR_SM_STATE_RSVD1_OFF                                  (7)
#define PP_QMGR_SM_STATE_RSVD1_LEN                                  (2)
#define PP_QMGR_SM_STATE_RSVD1_MSK                                  (0x00000180)
#define PP_QMGR_SM_STATE_RSVD1_RST                                  (0x0)
#define PP_QMGR_SM_STATE_DIVERT_SM_STATE_OFF                        (5)
#define PP_QMGR_SM_STATE_DIVERT_SM_STATE_LEN                        (2)
#define PP_QMGR_SM_STATE_DIVERT_SM_STATE_MSK                        (0x00000060)
#define PP_QMGR_SM_STATE_DIVERT_SM_STATE_RST                        (0x0)
#define PP_QMGR_SM_STATE_CMD_DIST_DEL_STATE_OFF                     (3)
#define PP_QMGR_SM_STATE_CMD_DIST_DEL_STATE_LEN                     (2)
#define PP_QMGR_SM_STATE_CMD_DIST_DEL_STATE_MSK                     (0x00000018)
#define PP_QMGR_SM_STATE_CMD_DIST_DEL_STATE_RST                     (0x0)
#define PP_QMGR_SM_STATE_CMD_DIST_DIVERT_STATE_OFF                  (0)
#define PP_QMGR_SM_STATE_CMD_DIST_DIVERT_STATE_LEN                  (3)
#define PP_QMGR_SM_STATE_CMD_DIST_DIVERT_STATE_MSK                  (0x00000007)
#define PP_QMGR_SM_STATE_CMD_DIST_DIVERT_STATE_RST                  (0x0)

/**
 * SW_REG_NAME : PP_QMGR_CMD_MACHINE_BUSY_REG
 * HW_REG_NAME : cmd_machine_busy
 * DESCRIPTION : If machine is busy
 *
 *  Register Fields :
 *   [31:16][RO] - Reserved
 *   [15: 0][RO] - Machine that are currently busy
 *
 */
#define PP_QMGR_CMD_MACHINE_BUSY_REG                   ((QMGR_BASE_ADDR) + 0x24)
#define PP_QMGR_CMD_MACHINE_BUSY_RSVD0_OFF                          (16)
#define PP_QMGR_CMD_MACHINE_BUSY_RSVD0_LEN                          (16)
#define PP_QMGR_CMD_MACHINE_BUSY_RSVD0_MSK                          (0xFFFF0000)
#define PP_QMGR_CMD_MACHINE_BUSY_RSVD0_RST                          (0x0)
#define PP_QMGR_CMD_MACHINE_BUSY_OFF                                (0)
#define PP_QMGR_CMD_MACHINE_BUSY_LEN                                (16)
#define PP_QMGR_CMD_MACHINE_BUSY_MSK                                (0x0000FFFF)
#define PP_QMGR_CMD_MACHINE_BUSY_RST                                (0x0)

/**
 * SW_REG_NAME : PP_QMGR_CMD_MACHINE_POP_BUSY_REG
 * HW_REG_NAME : cmd_machine_pop_busy
 * DESCRIPTION : If machine is busy
 *
 *  Register Fields :
 *   [31:16][RO] - Reserved
 *   [15: 0][RO] - Machine that are currently busy with pop operation
 *
 */
#define PP_QMGR_CMD_MACHINE_POP_BUSY_REG               ((QMGR_BASE_ADDR) + 0x28)
#define PP_QMGR_CMD_MACHINE_POP_BUSY_RSVD0_OFF                      (16)
#define PP_QMGR_CMD_MACHINE_POP_BUSY_RSVD0_LEN                      (16)
#define PP_QMGR_CMD_MACHINE_POP_BUSY_RSVD0_MSK                      (0xFFFF0000)
#define PP_QMGR_CMD_MACHINE_POP_BUSY_RSVD0_RST                      (0x0)
#define PP_QMGR_CMD_MACHINE_POP_BUSY_OFF                            (0)
#define PP_QMGR_CMD_MACHINE_POP_BUSY_LEN                            (16)
#define PP_QMGR_CMD_MACHINE_POP_BUSY_MSK                            (0x0000FFFF)
#define PP_QMGR_CMD_MACHINE_POP_BUSY_RST                            (0x0)

/**
 * SW_REG_NAME : PP_QMGR_INIT_STATUS_REG
 * HW_REG_NAME : init_status
 * DESCRIPTION : Init and init status register, Writing to this reg
 *               start init, init_done field is indication that
 *               init was finished
 *
 *  Register Fields :
 *   [31:31][RO] - Init done
 *   [30:30][RO] - Q context init done
 *   [29:29][RO] - Statistics init done
 *   [28:28][RO] - PQM init done
 *   [27:27][RW] - Writing to this bit starts QM init
 *   [26:17][RO] - Reserved
 *   [16: 0][RW] - Number of pages (in DDR) qm should init
 *
 */
#define PP_QMGR_INIT_STATUS_REG                        ((QMGR_BASE_ADDR) + 0x2C)
#define PP_QMGR_INIT_STATUS_INIT_DONE_OFF                           (31)
#define PP_QMGR_INIT_STATUS_INIT_DONE_LEN                           (1)
#define PP_QMGR_INIT_STATUS_INIT_DONE_MSK                           (0x80000000)
#define PP_QMGR_INIT_STATUS_INIT_DONE_RST                           (0x0)
#define PP_QMGR_INIT_STATUS_Q_CONTEXT_INIT_DONE_OFF                 (30)
#define PP_QMGR_INIT_STATUS_Q_CONTEXT_INIT_DONE_LEN                 (1)
#define PP_QMGR_INIT_STATUS_Q_CONTEXT_INIT_DONE_MSK                 (0x40000000)
#define PP_QMGR_INIT_STATUS_Q_CONTEXT_INIT_DONE_RST                 (0x0)
#define PP_QMGR_INIT_STATUS_STATS_INIT_DONE_OFF                     (29)
#define PP_QMGR_INIT_STATUS_STATS_INIT_DONE_LEN                     (1)
#define PP_QMGR_INIT_STATUS_STATS_INIT_DONE_MSK                     (0x20000000)
#define PP_QMGR_INIT_STATUS_STATS_INIT_DONE_RST                     (0x0)
#define PP_QMGR_INIT_STATUS_PQM_INIT_DONE_OFF                       (28)
#define PP_QMGR_INIT_STATUS_PQM_INIT_DONE_LEN                       (1)
#define PP_QMGR_INIT_STATUS_PQM_INIT_DONE_MSK                       (0x10000000)
#define PP_QMGR_INIT_STATUS_PQM_INIT_DONE_RST                       (0x0)
#define PP_QMGR_INIT_STATUS_START_INIT_OFF                          (27)
#define PP_QMGR_INIT_STATUS_START_INIT_LEN                          (1)
#define PP_QMGR_INIT_STATUS_START_INIT_MSK                          (0x08000000)
#define PP_QMGR_INIT_STATUS_START_INIT_RST                          (0x0)
#define PP_QMGR_INIT_STATUS_RSVD0_OFF                               (17)
#define PP_QMGR_INIT_STATUS_RSVD0_LEN                               (10)
#define PP_QMGR_INIT_STATUS_RSVD0_MSK                               (0x07FE0000)
#define PP_QMGR_INIT_STATUS_RSVD0_RST                               (0x0)
#define PP_QMGR_INIT_STATUS_NUM_DDR_PAGES_OFF                       (0)
#define PP_QMGR_INIT_STATUS_NUM_DDR_PAGES_LEN                       (17)
#define PP_QMGR_INIT_STATUS_NUM_DDR_PAGES_MSK                       (0x0001FFFF)
#define PP_QMGR_INIT_STATUS_NUM_DDR_PAGES_RST                       (0x10000)

/**
 * SW_REG_NAME : PP_QMGR_NULL_POP_CNT_REG
 * HW_REG_NAME : null_pop_cnt
 * DESCRIPTION : A generic register to count an operation
 *
 *  Register Fields :
 *   [31:31][RW] - Operation counter
 *   [30: 0][RO] - Operation counter
 *
 */
#define PP_QMGR_NULL_POP_CNT_REG                       ((QMGR_BASE_ADDR) + 0x30)
#define PP_QMGR_NULL_POP_CNT_CLR_OFF                                (31)
#define PP_QMGR_NULL_POP_CNT_CLR_LEN                                (1)
#define PP_QMGR_NULL_POP_CNT_CLR_MSK                                (0x80000000)
#define PP_QMGR_NULL_POP_CNT_CLR_RST                                (0x0)
#define PP_QMGR_NULL_POP_CNT_OFF                                    (0)
#define PP_QMGR_NULL_POP_CNT_LEN                                    (31)
#define PP_QMGR_NULL_POP_CNT_MSK                                    (0x7FFFFFFF)
#define PP_QMGR_NULL_POP_CNT_RST                                    (0x0)

/**
 * SW_REG_NAME : PP_QMGR_NULL_POP_Q_REG
 * HW_REG_NAME : null_pop_q_cnt
 * DESCRIPTION : A generic register to count an operation
 *
 *  Register Fields :
 *   [31:31][RW] - Operation counter
 *   [30:22][RW] - Queue to count for
 *   [21: 0][RO] - Operation counter
 *
 */
#define PP_QMGR_NULL_POP_Q_REG                         ((QMGR_BASE_ADDR) + 0x34)
#define PP_QMGR_NULL_POP_Q_CLR_OFF                                  (31)
#define PP_QMGR_NULL_POP_Q_CLR_LEN                                  (1)
#define PP_QMGR_NULL_POP_Q_CLR_MSK                                  (0x80000000)
#define PP_QMGR_NULL_POP_Q_CLR_RST                                  (0x0)
#define PP_QMGR_NULL_POP_Q_OFF                                      (22)
#define PP_QMGR_NULL_POP_Q_LEN                                      (9)
#define PP_QMGR_NULL_POP_Q_MSK                                      (0x7FC00000)
#define PP_QMGR_NULL_POP_Q_RST                                      (0x0)
#define PP_QMGR_NULL_POP_Q_CNT_OFF                                  (0)
#define PP_QMGR_NULL_POP_Q_CNT_LEN                                  (22)
#define PP_QMGR_NULL_POP_Q_CNT_MSK                                  (0x003FFFFF)
#define PP_QMGR_NULL_POP_Q_CNT_RST                                  (0x0)

/**
 * SW_REG_NAME : PP_QMGR_EMPTY_POP_CNT_REG
 * HW_REG_NAME : empty_pop_cnt
 * DESCRIPTION : A generic register to count an operation
 *
 *  Register Fields :
 *   [31:31][RW] - Operation counter
 *   [30: 0][RO] - Operation counter
 *
 */
#define PP_QMGR_EMPTY_POP_CNT_REG                      ((QMGR_BASE_ADDR) + 0x38)
#define PP_QMGR_EMPTY_POP_CNT_CLR_OFF                               (31)
#define PP_QMGR_EMPTY_POP_CNT_CLR_LEN                               (1)
#define PP_QMGR_EMPTY_POP_CNT_CLR_MSK                               (0x80000000)
#define PP_QMGR_EMPTY_POP_CNT_CLR_RST                               (0x0)
#define PP_QMGR_EMPTY_POP_CNT_OFF                                   (0)
#define PP_QMGR_EMPTY_POP_CNT_LEN                                   (31)
#define PP_QMGR_EMPTY_POP_CNT_MSK                                   (0x7FFFFFFF)
#define PP_QMGR_EMPTY_POP_CNT_RST                                   (0x0)

/**
 * SW_REG_NAME : PP_QMGR_EMPTY_POP_Q_REG
 * HW_REG_NAME : empty_pop_q_cnt
 * DESCRIPTION : A generic register to count an operation
 *
 *  Register Fields :
 *   [31:31][RW] - Operation counter
 *   [30:22][RW] - Queue to count for
 *   [21: 0][RO] - Operation counter
 *
 */
#define PP_QMGR_EMPTY_POP_Q_REG                        ((QMGR_BASE_ADDR) + 0x3C)
#define PP_QMGR_EMPTY_POP_Q_CLR_OFF                                 (31)
#define PP_QMGR_EMPTY_POP_Q_CLR_LEN                                 (1)
#define PP_QMGR_EMPTY_POP_Q_CLR_MSK                                 (0x80000000)
#define PP_QMGR_EMPTY_POP_Q_CLR_RST                                 (0x0)
#define PP_QMGR_EMPTY_POP_Q_OFF                                     (22)
#define PP_QMGR_EMPTY_POP_Q_LEN                                     (9)
#define PP_QMGR_EMPTY_POP_Q_MSK                                     (0x7FC00000)
#define PP_QMGR_EMPTY_POP_Q_RST                                     (0x0)
#define PP_QMGR_EMPTY_POP_Q_CNT_OFF                                 (0)
#define PP_QMGR_EMPTY_POP_Q_CNT_LEN                                 (22)
#define PP_QMGR_EMPTY_POP_Q_CNT_MSK                                 (0x003FFFFF)
#define PP_QMGR_EMPTY_POP_Q_CNT_RST                                 (0x0)

/**
 * SW_REG_NAME : PP_QMGR_NULL_PUSH_CNT_REG
 * HW_REG_NAME : null_push_cnt
 * DESCRIPTION : A generic register to count an operation
 *
 *  Register Fields :
 *   [31:31][RW] - Operation counter
 *   [30: 0][RO] - Operation counter
 *
 */
#define PP_QMGR_NULL_PUSH_CNT_REG                      ((QMGR_BASE_ADDR) + 0x40)
#define PP_QMGR_NULL_PUSH_CNT_CLR_OFF                               (31)
#define PP_QMGR_NULL_PUSH_CNT_CLR_LEN                               (1)
#define PP_QMGR_NULL_PUSH_CNT_CLR_MSK                               (0x80000000)
#define PP_QMGR_NULL_PUSH_CNT_CLR_RST                               (0x0)
#define PP_QMGR_NULL_PUSH_CNT_OFF                                   (0)
#define PP_QMGR_NULL_PUSH_CNT_LEN                                   (31)
#define PP_QMGR_NULL_PUSH_CNT_MSK                                   (0x7FFFFFFF)
#define PP_QMGR_NULL_PUSH_CNT_RST                                   (0x0)

/**
 * SW_REG_NAME : PP_QMGR_NULL_PUSH_Q_REG
 * HW_REG_NAME : null_push_q_cnt
 * DESCRIPTION : A generic register to count an operation
 *
 *  Register Fields :
 *   [31:31][RW] - Operation counter
 *   [30:22][RW] - Queue to count for
 *   [21: 0][RO] - Operation counter
 *
 */
#define PP_QMGR_NULL_PUSH_Q_REG                        ((QMGR_BASE_ADDR) + 0x44)
#define PP_QMGR_NULL_PUSH_Q_CLR_OFF                                 (31)
#define PP_QMGR_NULL_PUSH_Q_CLR_LEN                                 (1)
#define PP_QMGR_NULL_PUSH_Q_CLR_MSK                                 (0x80000000)
#define PP_QMGR_NULL_PUSH_Q_CLR_RST                                 (0x0)
#define PP_QMGR_NULL_PUSH_Q_OFF                                     (22)
#define PP_QMGR_NULL_PUSH_Q_LEN                                     (9)
#define PP_QMGR_NULL_PUSH_Q_MSK                                     (0x7FC00000)
#define PP_QMGR_NULL_PUSH_Q_RST                                     (0x0)
#define PP_QMGR_NULL_PUSH_Q_CNT_OFF                                 (0)
#define PP_QMGR_NULL_PUSH_Q_CNT_LEN                                 (22)
#define PP_QMGR_NULL_PUSH_Q_CNT_MSK                                 (0x003FFFFF)
#define PP_QMGR_NULL_PUSH_Q_CNT_RST                                 (0x0)

/**
 * SW_REG_NAME : PP_QMGR_FW_POP_BYTE_WIDTH_REG
 * HW_REG_NAME : fw_pop_byte_width
 * DESCRIPTION : The byte width of every FW pop cmd, if it's less
 *               than descriptor size the QMGR wait until the
 *               commands combine size reach descriptor width
 *
 *  Register Fields :
 *   [31: 2][RO] - Reserved
 *   [ 1: 0][RW] - 0-DESCRIPTOR_SIZE, 1-DESCRIPTOR_SIZE/2,
 *                 2-DESCRIPTOR_SIZE/4, 3-DESCRIPTOR_SIZE/8
 *
 */
#define PP_QMGR_FW_POP_BYTE_WIDTH_REG                  ((QMGR_BASE_ADDR) + 0x48)
#define PP_QMGR_FW_POP_BYTE_WIDTH_RSVD0_OFF                         (2)
#define PP_QMGR_FW_POP_BYTE_WIDTH_RSVD0_LEN                         (30)
#define PP_QMGR_FW_POP_BYTE_WIDTH_RSVD0_MSK                         (0xFFFFFFFC)
#define PP_QMGR_FW_POP_BYTE_WIDTH_RSVD0_RST                         (0x0)
#define PP_QMGR_FW_POP_BYTE_WIDTH_OFF                               (0)
#define PP_QMGR_FW_POP_BYTE_WIDTH_LEN                               (2)
#define PP_QMGR_FW_POP_BYTE_WIDTH_MSK                               (0x00000003)
#define PP_QMGR_FW_POP_BYTE_WIDTH_RST                               (0x0)

/**
 * SW_REG_NAME : PP_QMGR_DDR_STOP_PUSH_THR_REG
 * HW_REG_NAME : ddr_stop_push_threshold
 * DESCRIPTION : The number of free pages below it the QM won't
 *               accept push commands, if 'drop_blocked_descriptor'
 *               is asserted incoming descriptors will be dropped
 *               only if there're  no resources for them
 *
 *  Register Fields :
 *   [31:31][RO] - Reserved
 *   [30:30][RW] - Is to drop incoming descriptors when 'stop push
 *                 threshold' is reached
 *   [29:17][RO] - Reserved
 *   [16: 0][RO] - DDR Stop Push Free-Pages Low Threshold, below this
 *                 threshold QM will stop push to prevent deadlock
 *
 */
#define PP_QMGR_DDR_STOP_PUSH_THR_REG                  ((QMGR_BASE_ADDR) + 0x4C)
#define PP_QMGR_DDR_STOP_PUSH_THR_RSVD0_OFF                         (31)
#define PP_QMGR_DDR_STOP_PUSH_THR_RSVD0_LEN                         (1)
#define PP_QMGR_DDR_STOP_PUSH_THR_RSVD0_MSK                         (0x80000000)
#define PP_QMGR_DDR_STOP_PUSH_THR_RSVD0_RST                         (0x0)
#define PP_QMGR_DDR_STOP_PUSH_THR_DROP_BLOCKED_DESC_OFF             (30)
#define PP_QMGR_DDR_STOP_PUSH_THR_DROP_BLOCKED_DESC_LEN             (1)
#define PP_QMGR_DDR_STOP_PUSH_THR_DROP_BLOCKED_DESC_MSK             (0x40000000)
#define PP_QMGR_DDR_STOP_PUSH_THR_DROP_BLOCKED_DESC_RST             (0x0)
#define PP_QMGR_DDR_STOP_PUSH_THR_RSVD1_OFF                         (17)
#define PP_QMGR_DDR_STOP_PUSH_THR_RSVD1_LEN                         (13)
#define PP_QMGR_DDR_STOP_PUSH_THR_RSVD1_MSK                         (0x3FFE0000)
#define PP_QMGR_DDR_STOP_PUSH_THR_RSVD1_RST                         (0x0)
#define PP_QMGR_DDR_STOP_PUSH_THR_OFF                               (0)
#define PP_QMGR_DDR_STOP_PUSH_THR_LEN                               (17)
#define PP_QMGR_DDR_STOP_PUSH_THR_MSK                               (0x0001FFFF)
#define PP_QMGR_DDR_STOP_PUSH_THR_RST                               (0x20)

/**
 * SW_REG_NAME : PP_QMGR_FIFO_ERROR_REG
 * HW_REG_NAME : fifo_error
 * DESCRIPTION : Sticky bits of fifo error
 *
 *  Register Fields :
 *   [31: 0][RO] - QMGR FIFO error
 *
 */
#define PP_QMGR_FIFO_ERROR_REG                         ((QMGR_BASE_ADDR) + 0x50)
#define PP_QMGR_FIFO_ERROR_OFF                                      (0)
#define PP_QMGR_FIFO_ERROR_LEN                                      (32)
#define PP_QMGR_FIFO_ERROR_MSK                                      (0xFFFFFFFF)
#define PP_QMGR_FIFO_ERROR_RST                                      (0x0)

/**
 * SW_REG_NAME : PP_QMGR_OCP_ERROR_REG
 * HW_REG_NAME : ocp_error
 * DESCRIPTION : Sticky bits of ocp error
 *
 *  Register Fields :
 *   [31: 5][RO] - Reserved
 *   [ 4: 0][RO] - QMGR OCP error
 *
 */
#define PP_QMGR_OCP_ERROR_REG                          ((QMGR_BASE_ADDR) + 0x54)
#define PP_QMGR_OCP_ERROR_RSVD0_OFF                                 (5)
#define PP_QMGR_OCP_ERROR_RSVD0_LEN                                 (27)
#define PP_QMGR_OCP_ERROR_RSVD0_MSK                                 (0xFFFFFFE0)
#define PP_QMGR_OCP_ERROR_RSVD0_RST                                 (0x0)
#define PP_QMGR_OCP_ERROR_OFF                                       (0)
#define PP_QMGR_OCP_ERROR_LEN                                       (5)
#define PP_QMGR_OCP_ERROR_MSK                                       (0x0000001F)
#define PP_QMGR_OCP_ERROR_RST                                       (0x0)

/**
 * SW_REG_NAME : PP_QMGR_DROP_PUSH_CNT_REG
 * HW_REG_NAME : drop_push_cnt
 * DESCRIPTION : A generic register to count an operation
 *
 *  Register Fields :
 *   [31:31][RW] - Operation counter
 *   [30: 0][RO] - Operation counter
 *
 */
#define PP_QMGR_DROP_PUSH_CNT_REG                      ((QMGR_BASE_ADDR) + 0x58)
#define PP_QMGR_DROP_PUSH_CNT_CLR_OFF                               (31)
#define PP_QMGR_DROP_PUSH_CNT_CLR_LEN                               (1)
#define PP_QMGR_DROP_PUSH_CNT_CLR_MSK                               (0x80000000)
#define PP_QMGR_DROP_PUSH_CNT_CLR_RST                               (0x0)
#define PP_QMGR_DROP_PUSH_CNT_OFF                                   (0)
#define PP_QMGR_DROP_PUSH_CNT_LEN                                   (31)
#define PP_QMGR_DROP_PUSH_CNT_MSK                                   (0x7FFFFFFF)
#define PP_QMGR_DROP_PUSH_CNT_RST                                   (0x0)

/**
 * SW_REG_NAME : PP_QMGR_DROP_POP_Q_REG
 * HW_REG_NAME : drop_push_q_cnt
 * DESCRIPTION : A generic register to count an operation
 *
 *  Register Fields :
 *   [31:31][RW] - Operation counter
 *   [30:22][RW] - Queue to count for
 *   [21: 0][RO] - Operation counter
 *
 */
#define PP_QMGR_DROP_POP_Q_REG                         ((QMGR_BASE_ADDR) + 0x5C)
#define PP_QMGR_DROP_POP_Q_CLR_OFF                                  (31)
#define PP_QMGR_DROP_POP_Q_CLR_LEN                                  (1)
#define PP_QMGR_DROP_POP_Q_CLR_MSK                                  (0x80000000)
#define PP_QMGR_DROP_POP_Q_CLR_RST                                  (0x0)
#define PP_QMGR_DROP_POP_Q_OFF                                      (22)
#define PP_QMGR_DROP_POP_Q_LEN                                      (9)
#define PP_QMGR_DROP_POP_Q_MSK                                      (0x7FC00000)
#define PP_QMGR_DROP_POP_Q_RST                                      (0x0)
#define PP_QMGR_DROP_POP_Q_CNT_OFF                                  (0)
#define PP_QMGR_DROP_POP_Q_CNT_LEN                                  (22)
#define PP_QMGR_DROP_POP_Q_CNT_MSK                                  (0x003FFFFF)
#define PP_QMGR_DROP_POP_Q_CNT_RST                                  (0x0)

/**
 * SW_REG_NAME : PP_QMGR_Q_STATUS_SYNC_REG
 * HW_REG_NAME : q_status_sync
 * DESCRIPTION : Write Q num in order to initiate single status
 *               update
 *
 *  Register Fields :
 *   [31:31][RW] - TSCD WA autoupdate Enable, NOTE, B Step ONLY field
 *   [30:30][RW] - sync_q_status, NOTE, B Step ONLY field
 *   [29: 9][RO] - Reserved
 *   [ 8: 0][RW] - Queue Num
 *
 */
#define PP_QMGR_Q_STATUS_SYNC_REG                      ((QMGR_BASE_ADDR) + 0x60)
#define PP_QMGR_Q_STATUS_SYNC_SUP_EN_OFF                            (31)
#define PP_QMGR_Q_STATUS_SYNC_SUP_EN_LEN                            (1)
#define PP_QMGR_Q_STATUS_SYNC_SUP_EN_MSK                            (0x80000000)
#define PP_QMGR_Q_STATUS_SYNC_SUP_EN_RST                            (0x1)
#define PP_QMGR_Q_STATUS_SYNC_SYNC_Q_STATUS_OFF                     (30)
#define PP_QMGR_Q_STATUS_SYNC_SYNC_Q_STATUS_LEN                     (1)
#define PP_QMGR_Q_STATUS_SYNC_SYNC_Q_STATUS_MSK                     (0x40000000)
#define PP_QMGR_Q_STATUS_SYNC_SYNC_Q_STATUS_RST                     (0x0)
#define PP_QMGR_Q_STATUS_SYNC_RSVD0_OFF                             (9)
#define PP_QMGR_Q_STATUS_SYNC_RSVD0_LEN                             (21)
#define PP_QMGR_Q_STATUS_SYNC_RSVD0_MSK                             (0x3FFFFE00)
#define PP_QMGR_Q_STATUS_SYNC_RSVD0_RST                             (0x0)
#define PP_QMGR_Q_STATUS_SYNC_Q_NUM_OFF                             (0)
#define PP_QMGR_Q_STATUS_SYNC_Q_NUM_LEN                             (9)
#define PP_QMGR_Q_STATUS_SYNC_Q_NUM_MSK                             (0x000001FF)
#define PP_QMGR_Q_STATUS_SYNC_Q_NUM_RST                             (0x0)

/**
 * SW_REG_NAME : PP_QMGR_QM_EN_REG
 * HW_REG_NAME : qm_en
 * DESCRIPTION : QMGR Enable Register, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 1][RO] - Reserved
 *   [ 0: 0][RW] - QMGR is enabled, if not asserted push is dropped,
 *                 pop is responsed with NULL value
 *
 */
#define PP_QMGR_QM_EN_REG                              ((QMGR_BASE_ADDR) + 0x64)
#define PP_QMGR_QM_EN_RSVD0_OFF                                     (1)
#define PP_QMGR_QM_EN_RSVD0_LEN                                     (31)
#define PP_QMGR_QM_EN_RSVD0_MSK                                     (0xFFFFFFFE)
#define PP_QMGR_QM_EN_RSVD0_RST                                     (0x0)
#define PP_QMGR_QM_EN_OFF                                           (0)
#define PP_QMGR_QM_EN_LEN                                           (1)
#define PP_QMGR_QM_EN_MSK                                           (0x00000001)
#define PP_QMGR_QM_EN_RST                                           (0x0)

/**
 * SW_REG_NAME : PP_QMGR_DIS_PUSH_DROP_CNT_REG
 * HW_REG_NAME : disable_push_drop_cnt
 * DESCRIPTION : A generic register to count an operation, NOTE, B
 *               Step ONLY register
 *
 *  Register Fields :
 *   [31:31][RW] - Operation counter
 *   [30: 0][RO] - Operation counter
 *
 */
#define PP_QMGR_DIS_PUSH_DROP_CNT_REG                  ((QMGR_BASE_ADDR) + 0x68)
#define PP_QMGR_DIS_PUSH_DROP_CNT_CLR_OFF                           (31)
#define PP_QMGR_DIS_PUSH_DROP_CNT_CLR_LEN                           (1)
#define PP_QMGR_DIS_PUSH_DROP_CNT_CLR_MSK                           (0x80000000)
#define PP_QMGR_DIS_PUSH_DROP_CNT_CLR_RST                           (0x0)
#define PP_QMGR_DIS_PUSH_DROP_CNT_OFF                               (0)
#define PP_QMGR_DIS_PUSH_DROP_CNT_LEN                               (31)
#define PP_QMGR_DIS_PUSH_DROP_CNT_MSK                               (0x7FFFFFFF)
#define PP_QMGR_DIS_PUSH_DROP_CNT_RST                               (0x0)

/**
 * SW_REG_NAME : PP_QMGR_WA_INTERVAL_REG
 * HW_REG_NAME : wa_interval
 * DESCRIPTION : QMGR WA Interval, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][RW] - Interval between two 'enabled' cycles if DWAQ is
 *                 setfor a Q
 *
 */
#define PP_QMGR_WA_INTERVAL_REG                        ((QMGR_BASE_ADDR) + 0x6C)
#define PP_QMGR_WA_INTERVAL_OFF                                     (0)
#define PP_QMGR_WA_INTERVAL_LEN                                     (32)
#define PP_QMGR_WA_INTERVAL_MSK                                     (0xFFFFFFFF)
#define PP_QMGR_WA_INTERVAL_RST                                     (0x0)

/**
 * SW_REG_NAME : PP_QMGR_WA_TIMER_REG
 * HW_REG_NAME : wa_timer
 * DESCRIPTION : QMGR WA Timer, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][RO] - RO, HW timer count from Interval to zero, for
 *                 debug purpuse
 *
 */
#define PP_QMGR_WA_TIMER_REG                           ((QMGR_BASE_ADDR) + 0x70)
#define PP_QMGR_WA_TIMER_OFF                                        (0)
#define PP_QMGR_WA_TIMER_LEN                                        (32)
#define PP_QMGR_WA_TIMER_MSK                                        (0xFFFFFFFF)
#define PP_QMGR_WA_TIMER_RST                                        (0x0)

/**
 * SW_REG_NAME : PP_QMGR_CMD_MACHINE_SM_STATE_REG
 * HW_REG_NAME : cmd_machine_sm_state
 * DESCRIPTION : Current state of cmd_machine(s) state machines
 *               inside QMGR
 *
 *  Register Fields :
 *   [31:13][RO] - Reserved
 *   [12: 7][RO] - Machine FIFO error
 *   [ 6: 3][RO] - delete state machine current state
 *   [ 2: 0][RO] - divert state machine current state
 *
 */
#define PP_QMGR_CMD_MACHINE_SM_STATE_REG              ((QMGR_BASE_ADDR) + 0x100)
#define PP_QMGR_CMD_MACHINE_SM_STATE_RSVD0_OFF                      (13)
#define PP_QMGR_CMD_MACHINE_SM_STATE_RSVD0_LEN                      (19)
#define PP_QMGR_CMD_MACHINE_SM_STATE_RSVD0_MSK                      (0xFFFFE000)
#define PP_QMGR_CMD_MACHINE_SM_STATE_RSVD0_RST                      (0x0)
#define PP_QMGR_CMD_MACHINE_SM_STATE_FIFO_ERROR_OFF                 (7)
#define PP_QMGR_CMD_MACHINE_SM_STATE_FIFO_ERROR_LEN                 (6)
#define PP_QMGR_CMD_MACHINE_SM_STATE_FIFO_ERROR_MSK                 (0x00001F80)
#define PP_QMGR_CMD_MACHINE_SM_STATE_FIFO_ERROR_RST                 (0x0)
#define PP_QMGR_CMD_MACHINE_SM_STATE_DEL_STATE_OFF                  (3)
#define PP_QMGR_CMD_MACHINE_SM_STATE_DEL_STATE_LEN                  (4)
#define PP_QMGR_CMD_MACHINE_SM_STATE_DEL_STATE_MSK                  (0x00000078)
#define PP_QMGR_CMD_MACHINE_SM_STATE_DEL_STATE_RST                  (0x0)
#define PP_QMGR_CMD_MACHINE_SM_STATE_DIVERT_STATE_OFF               (0)
#define PP_QMGR_CMD_MACHINE_SM_STATE_DIVERT_STATE_LEN               (3)
#define PP_QMGR_CMD_MACHINE_SM_STATE_DIVERT_STATE_MSK               (0x00000007)
#define PP_QMGR_CMD_MACHINE_SM_STATE_DIVERT_STATE_RST               (0x0)
/**
 * REG_IDX_ACCESS   : PP_QMGR_CMD_MACHINE_SM_STATE_REG_IDX
 * NUM OF REGISTERS : 16
 */
#define PP_QMGR_CMD_MACHINE_SM_STATE_REG_IDX(idx) \
	(PP_QMGR_CMD_MACHINE_SM_STATE_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_QMGR_CMD_MACHINE_Q_REG
 * HW_REG_NAME : cmd_machine_queue
 * DESCRIPTION : Current queue cmd_machine(s) serve
 *
 *  Register Fields :
 *   [31: 9][RO] - Reserved
 *   [ 8: 0][RO] - The Queue a cmd_machine serves
 *
 */
#define PP_QMGR_CMD_MACHINE_Q_REG                     ((QMGR_BASE_ADDR) + 0x200)
#define PP_QMGR_CMD_MACHINE_Q_RSVD0_OFF                             (9)
#define PP_QMGR_CMD_MACHINE_Q_RSVD0_LEN                             (23)
#define PP_QMGR_CMD_MACHINE_Q_RSVD0_MSK                             (0xFFFFFE00)
#define PP_QMGR_CMD_MACHINE_Q_RSVD0_RST                             (0x0)
#define PP_QMGR_CMD_MACHINE_Q_OFF                                   (0)
#define PP_QMGR_CMD_MACHINE_Q_LEN                                   (9)
#define PP_QMGR_CMD_MACHINE_Q_MSK                                   (0x000001FF)
#define PP_QMGR_CMD_MACHINE_Q_RST                                   (0x1ff)
/**
 * REG_IDX_ACCESS   : PP_QMGR_CMD_MACHINE_Q_REG_IDX
 * NUM OF REGISTERS : 16
 */
#define PP_QMGR_CMD_MACHINE_Q_REG_IDX(idx) \
	(PP_QMGR_CMD_MACHINE_Q_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_QMGR_FAST_Q_REG
 * HW_REG_NAME : fast_queue
 * DESCRIPTION : Is queue is fast queue and fetches two head pages
 *
 *  Register Fields :
 *   [31: 0][RW] - Each bit represent a queue, if a bit that
 *                 represnts a queue is asserted QMGR will fetch for
 *                 this queue two pages to cache, BE CAREFULL not to
 *                 configure more than 128 queues as fast queues,
 *                 violating this may cause unrecoverable scenario in
 *                 qmgr!!!
 *
 */
#define PP_QMGR_FAST_Q_REG                            ((QMGR_BASE_ADDR) + 0x300)
#define PP_QMGR_FAST_Q_OFF                                          (0)
#define PP_QMGR_FAST_Q_LEN                                          (32)
#define PP_QMGR_FAST_Q_MSK                                          (0xFFFFFFFF)
#define PP_QMGR_FAST_Q_RST                                          (0x0)
/**
 * REG_IDX_ACCESS   : PP_QMGR_FAST_Q_REG_IDX
 * NUM OF REGISTERS : 16
 */
#define PP_QMGR_FAST_Q_REG_IDX(idx) \
	(PP_QMGR_FAST_Q_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_QMGR_RTC_EN_REG
 * HW_REG_NAME : rtc_en
 * DESCRIPTION : If to replace certein bits in descriptors with
 *               time-stamp value
 *
 *  Register Fields :
 *   [31: 0][RW] - Each bit represent a queue, if a bit that
 *                 represnts a queue is asserted QMGR will replace
 *                 certein bits in descriptors with time-stamp value
 *
 */
#define PP_QMGR_RTC_EN_REG                            ((QMGR_BASE_ADDR) + 0x400)
#define PP_QMGR_RTC_EN_OFF                                          (0)
#define PP_QMGR_RTC_EN_LEN                                          (32)
#define PP_QMGR_RTC_EN_MSK                                          (0xFFFFFFFF)
#define PP_QMGR_RTC_EN_RST                                          (0xffffffff)
/**
 * REG_IDX_ACCESS   : PP_QMGR_RTC_EN_REG_IDX
 * NUM OF REGISTERS : 16
 */
#define PP_QMGR_RTC_EN_REG_IDX(idx) \
	(PP_QMGR_RTC_EN_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_QMGR_DWAQ_REG
 * HW_REG_NAME : dwaq
 * DESCRIPTION : Delay WA queue, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][RW] - is delay WA is enabled for 32 queues (need list of
 *                 this register)
 *
 */
#define PP_QMGR_DWAQ_REG                              ((QMGR_BASE_ADDR) + 0x500)
#define PP_QMGR_DWAQ_OFF                                            (0)
#define PP_QMGR_DWAQ_LEN                                            (32)
#define PP_QMGR_DWAQ_MSK                                            (0xFFFFFFFF)
#define PP_QMGR_DWAQ_RST                                            (0x0)
/**
 * REG_IDX_ACCESS   : PP_QMGR_DWAQ_REG_IDX
 * NUM OF REGISTERS : 16
 */
#define PP_QMGR_DWAQ_REG_IDX(idx) \
	(PP_QMGR_DWAQ_REG + ((idx) << 2))

#endif
