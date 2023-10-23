/**
 * qmgr_mems_regs.h
 * Description: qmgr_mems_s HW registers definitions
 * Auto-generated file by RDL Parser, DO NOT CHANGE
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2019-2020 Intel Corporation
 */

#ifndef _PP_QMGR_MEM_H_
#define _PP_QMGR_MEM_H_

#define PP_QMGR_MEM_GEN_DATE_STR              "Sunday May 17, 2020 [5:26:11 pm]"
#define PP_QMGR_MEM_BASE                                         (0xF11C0800ULL)

/**
 * SW_REG_NAME : PP_QMGR_MEM_IMMEDIATE_POP_VLD_IP_REG
 * HW_REG_NAME : immediate_pop_vld_ip
 * DESCRIPTION : Immediate pop Valid in process per Queue
 *
 *  Register Fields :
 *   [31: 0][RO] - Each bit is flag per queue
 *
 */
#define PP_QMGR_MEM_IMMEDIATE_POP_VLD_IP_REG      ((QMGR_MEM_BASE_ADDR) + 0x800)
#define PP_QMGR_MEM_IMMEDIATE_POP_VLD_IP_OFF                        (0)
#define PP_QMGR_MEM_IMMEDIATE_POP_VLD_IP_LEN                        (32)
#define PP_QMGR_MEM_IMMEDIATE_POP_VLD_IP_MSK                        (0xFFFFFFFF)
#define PP_QMGR_MEM_IMMEDIATE_POP_VLD_IP_RST                        (0x0)
/**
 * REG_IDX_ACCESS   : PP_QMGR_MEM_IMMEDIATE_POP_VLD_IP_REG_IDX
 * NUM OF REGISTERS : 16
 */
#define PP_QMGR_MEM_IMMEDIATE_POP_VLD_IP_REG_IDX(idx) \
	(PP_QMGR_MEM_IMMEDIATE_POP_VLD_IP_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_QMGR_MEM_IMMEDIATE_POP_VLD_REG
 * HW_REG_NAME : immediate_pop_vld
 * DESCRIPTION : Immediate pop Valid per Queue
 *
 *  Register Fields :
 *   [31: 0][RO] - Each bit is flag per queue
 *
 */
#define PP_QMGR_MEM_IMMEDIATE_POP_VLD_REG        ((QMGR_MEM_BASE_ADDR) + 0x1000)
#define PP_QMGR_MEM_IMMEDIATE_POP_VLD_OFF                           (0)
#define PP_QMGR_MEM_IMMEDIATE_POP_VLD_LEN                           (32)
#define PP_QMGR_MEM_IMMEDIATE_POP_VLD_MSK                           (0xFFFFFFFF)
#define PP_QMGR_MEM_IMMEDIATE_POP_VLD_RST                           (0x0)
/**
 * REG_IDX_ACCESS   : PP_QMGR_MEM_IMMEDIATE_POP_VLD_REG_IDX
 * NUM OF REGISTERS : 16
 */
#define PP_QMGR_MEM_IMMEDIATE_POP_VLD_REG_IDX(idx) \
	(PP_QMGR_MEM_IMMEDIATE_POP_VLD_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_QMGR_MEM_CONTEXT_PEND_REG
 * HW_REG_NAME : context_pend
 * DESCRIPTION : Context pending per Queue
 *
 *  Register Fields :
 *   [31: 0][RO] - Each bit is flag per queue
 *
 */
#define PP_QMGR_MEM_CONTEXT_PEND_REG             ((QMGR_MEM_BASE_ADDR) + 0x1800)
#define PP_QMGR_MEM_CONTEXT_PEND_IMMEDIATE_POP_VLD_OFF              (0)
#define PP_QMGR_MEM_CONTEXT_PEND_IMMEDIATE_POP_VLD_LEN              (32)
#define PP_QMGR_MEM_CONTEXT_PEND_IMMEDIATE_POP_VLD_MSK              (0xFFFFFFFF)
#define PP_QMGR_MEM_CONTEXT_PEND_IMMEDIATE_POP_VLD_RST              (0x0)
/**
 * REG_IDX_ACCESS   : PP_QMGR_MEM_CONTEXT_PEND_REG_IDX
 * NUM OF REGISTERS : 16
 */
#define PP_QMGR_MEM_CONTEXT_PEND_REG_IDX(idx) \
	(PP_QMGR_MEM_CONTEXT_PEND_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_QMGR_MEM_PF_MEM_VLD_REG
 * HW_REG_NAME : pf_mem_vld
 * DESCRIPTION : Prefetch Memory Valid per Queue
 *
 *  Register Fields :
 *   [31: 0][RO] - Each bit is flag per queue
 *
 */
#define PP_QMGR_MEM_PF_MEM_VLD_REG               ((QMGR_MEM_BASE_ADDR) + 0x2000)
#define PP_QMGR_MEM_PF_MEM_VLD_PREFETCH_VLD_OFF                     (0)
#define PP_QMGR_MEM_PF_MEM_VLD_PREFETCH_VLD_LEN                     (32)
#define PP_QMGR_MEM_PF_MEM_VLD_PREFETCH_VLD_MSK                     (0xFFFFFFFF)
#define PP_QMGR_MEM_PF_MEM_VLD_PREFETCH_VLD_RST                     (0x0)
/**
 * REG_IDX_ACCESS   : PP_QMGR_MEM_PF_MEM_VLD_REG_IDX
 * NUM OF REGISTERS : 16
 */
#define PP_QMGR_MEM_PF_MEM_VLD_REG_IDX(idx) \
	(PP_QMGR_MEM_PF_MEM_VLD_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_QMGR_MEM_Q_PENDING_REG
 * HW_REG_NAME : queue_pending
 * DESCRIPTION : Queue pending per Queue
 *
 *  Register Fields :
 *   [31: 0][RO] - Each bit is flag per queue
 *
 */
#define PP_QMGR_MEM_Q_PENDING_REG                ((QMGR_MEM_BASE_ADDR) + 0x3800)
#define PP_QMGR_MEM_Q_PENDING_OFF                                   (0)
#define PP_QMGR_MEM_Q_PENDING_LEN                                   (32)
#define PP_QMGR_MEM_Q_PENDING_MSK                                   (0xFFFFFFFF)
#define PP_QMGR_MEM_Q_PENDING_RST                                   (0x0)
/**
 * REG_IDX_ACCESS   : PP_QMGR_MEM_Q_PENDING_REG_IDX
 * NUM OF REGISTERS : 16
 */
#define PP_QMGR_MEM_Q_PENDING_REG_IDX(idx) \
	(PP_QMGR_MEM_Q_PENDING_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_QMGR_MEM_CACHE_USED_PAGES_REG
 * HW_REG_NAME : cache_used_pages
 * DESCRIPTION : Cache Used Pages
 *
 *  Register Fields :
 *   [31: 0][RO] - Each bit is flag per page
 *
 */
#define PP_QMGR_MEM_CACHE_USED_PAGES_REG         ((QMGR_MEM_BASE_ADDR) + 0x4000)
#define PP_QMGR_MEM_CACHE_USED_PAGES_OFF                            (0)
#define PP_QMGR_MEM_CACHE_USED_PAGES_LEN                            (32)
#define PP_QMGR_MEM_CACHE_USED_PAGES_MSK                            (0xFFFFFFFF)
#define PP_QMGR_MEM_CACHE_USED_PAGES_RST                            (0x0)
/**
 * REG_IDX_ACCESS   : PP_QMGR_MEM_CACHE_USED_PAGES_REG_IDX
 * NUM OF REGISTERS : 36
 */
#define PP_QMGR_MEM_CACHE_USED_PAGES_REG_IDX(idx) \
	(PP_QMGR_MEM_CACHE_USED_PAGES_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_QMGR_MEM_CACHE_HEAD_PROT_PAGES_REG
 * HW_REG_NAME : cache_head_prot_pages
 * DESCRIPTION : Cache Head Protecteded Pages
 *
 *  Register Fields :
 *   [31: 0][RO] - Each bit is flag per page
 *
 */
#define PP_QMGR_MEM_CACHE_HEAD_PROT_PAGES_REG    ((QMGR_MEM_BASE_ADDR) + 0x6000)
#define PP_QMGR_MEM_CACHE_HEAD_PROT_PAGES_OFF                       (0)
#define PP_QMGR_MEM_CACHE_HEAD_PROT_PAGES_LEN                       (32)
#define PP_QMGR_MEM_CACHE_HEAD_PROT_PAGES_MSK                       (0xFFFFFFFF)
#define PP_QMGR_MEM_CACHE_HEAD_PROT_PAGES_RST                       (0x0)
/**
 * REG_IDX_ACCESS   : PP_QMGR_MEM_CACHE_HEAD_PROT_PAGES_REG_IDX
 * NUM OF REGISTERS : 36
 */
#define PP_QMGR_MEM_CACHE_HEAD_PROT_PAGES_REG_IDX(idx) \
	(PP_QMGR_MEM_CACHE_HEAD_PROT_PAGES_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_QMGR_MEM_CACHE_TAIL_PROT_PAGES_REG
 * HW_REG_NAME : cache_tail_prot_pages
 * DESCRIPTION : Cache Tail Protecteded Pages
 *
 *  Register Fields :
 *   [31: 0][RO] - Each bit is flag per page
 *
 */
#define PP_QMGR_MEM_CACHE_TAIL_PROT_PAGES_REG    ((QMGR_MEM_BASE_ADDR) + 0x8000)
#define PP_QMGR_MEM_CACHE_TAIL_PROT_PAGES_OFF                       (0)
#define PP_QMGR_MEM_CACHE_TAIL_PROT_PAGES_LEN                       (32)
#define PP_QMGR_MEM_CACHE_TAIL_PROT_PAGES_MSK                       (0xFFFFFFFF)
#define PP_QMGR_MEM_CACHE_TAIL_PROT_PAGES_RST                       (0x0)
/**
 * REG_IDX_ACCESS   : PP_QMGR_MEM_CACHE_TAIL_PROT_PAGES_REG_IDX
 * NUM OF REGISTERS : 36
 */
#define PP_QMGR_MEM_CACHE_TAIL_PROT_PAGES_REG_IDX(idx) \
	(PP_QMGR_MEM_CACHE_TAIL_PROT_PAGES_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_QMGR_MEM_Q_CONTEXT_REG
 * HW_REG_NAME : queue_context
 * DESCRIPTION : Queue Context
 *
 *  Register Fields :
 *   [31:27][RO] - Reserved
 *   [26:23][RO] - Tail Pointer
 *   [22:14][RO] - Tail Page Number
 *   [13:10][RO] - Head Pointer
 *   [ 9: 1][RO] - Head Page Number
 *   [ 0: 0][RO] - Next Head Page Valid
 *
 */
#define PP_QMGR_MEM_Q_CONTEXT_REG               ((QMGR_MEM_BASE_ADDR) + 0x10000)
#define PP_QMGR_MEM_Q_CONTEXT_RSVD0_OFF                             (27)
#define PP_QMGR_MEM_Q_CONTEXT_RSVD0_LEN                             (5)
#define PP_QMGR_MEM_Q_CONTEXT_RSVD0_MSK                             (0xF8000000)
#define PP_QMGR_MEM_Q_CONTEXT_RSVD0_RST                             (0x0)
#define PP_QMGR_MEM_Q_CONTEXT_TAIL_PTR_OFF                          (23)
#define PP_QMGR_MEM_Q_CONTEXT_TAIL_PTR_LEN                          (4)
#define PP_QMGR_MEM_Q_CONTEXT_TAIL_PTR_MSK                          (0x07800000)
#define PP_QMGR_MEM_Q_CONTEXT_TAIL_PTR_RST                          (0x0)
#define PP_QMGR_MEM_Q_CONTEXT_TAIL_PAGE_OFF                         (14)
#define PP_QMGR_MEM_Q_CONTEXT_TAIL_PAGE_LEN                         (9)
#define PP_QMGR_MEM_Q_CONTEXT_TAIL_PAGE_MSK                         (0x007FC000)
#define PP_QMGR_MEM_Q_CONTEXT_TAIL_PAGE_RST                         (0x0)
#define PP_QMGR_MEM_Q_CONTEXT_HEAD_PTR_OFF                          (10)
#define PP_QMGR_MEM_Q_CONTEXT_HEAD_PTR_LEN                          (4)
#define PP_QMGR_MEM_Q_CONTEXT_HEAD_PTR_MSK                          (0x00003C00)
#define PP_QMGR_MEM_Q_CONTEXT_HEAD_PTR_RST                          (0x0)
#define PP_QMGR_MEM_Q_CONTEXT_HEAD_PAGE_OFF                         (1)
#define PP_QMGR_MEM_Q_CONTEXT_HEAD_PAGE_LEN                         (9)
#define PP_QMGR_MEM_Q_CONTEXT_HEAD_PAGE_MSK                         (0x000003FE)
#define PP_QMGR_MEM_Q_CONTEXT_HEAD_PAGE_RST                         (0x0)
#define PP_QMGR_MEM_Q_CONTEXT_NEXT_VLD_OFF                          (0)
#define PP_QMGR_MEM_Q_CONTEXT_NEXT_VLD_LEN                          (1)
#define PP_QMGR_MEM_Q_CONTEXT_NEXT_VLD_MSK                          (0x00000001)
#define PP_QMGR_MEM_Q_CONTEXT_NEXT_VLD_RST                          (0x0)
/**
 * REG_IDX_ACCESS   : PP_QMGR_MEM_Q_CONTEXT_REG_IDX
 * NUM OF REGISTERS : 512
 */
#define PP_QMGR_MEM_Q_CONTEXT_REG_IDX(idx) \
	(PP_QMGR_MEM_Q_CONTEXT_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_QMGR_MEM_STATS_REG
 * HW_REG_NAME : statistics
 * DESCRIPTION : Number of descriptors in QMGR per queue
 *
 *  Register Fields :
 *   [31:21][RO] - Reserved
 *   [20: 0][RO] - Number of descriptors in QMGR per queue
 *
 */
#define PP_QMGR_MEM_STATS_REG                   ((QMGR_MEM_BASE_ADDR) + 0x12000)
#define PP_QMGR_MEM_STATS_RSVD0_OFF                                 (21)
#define PP_QMGR_MEM_STATS_RSVD0_LEN                                 (11)
#define PP_QMGR_MEM_STATS_RSVD0_MSK                                 (0xFFE00000)
#define PP_QMGR_MEM_STATS_RSVD0_RST                                 (0x0)
#define PP_QMGR_MEM_STATS_NUM_OF_DESC_OFF                           (0)
#define PP_QMGR_MEM_STATS_NUM_OF_DESC_LEN                           (21)
#define PP_QMGR_MEM_STATS_NUM_OF_DESC_MSK                           (0x001FFFFF)
#define PP_QMGR_MEM_STATS_NUM_OF_DESC_RST                           (0x0)
/**
 * REG_IDX_ACCESS   : PP_QMGR_MEM_STATS_REG_IDX
 * NUM OF REGISTERS : 512
 */
#define PP_QMGR_MEM_STATS_REG_IDX(idx) \
	(PP_QMGR_MEM_STATS_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_QMGR_MEM_IMMEDIATE_POP_REG
 * HW_REG_NAME : immediate_pop
 * DESCRIPTION : Immediate Pop page
 *
 *  Register Fields :
 *   [31: 9][RO] - Reserved
 *   [ 8: 0][RO] - Immediate Pop page
 *
 */
#define PP_QMGR_MEM_IMMEDIATE_POP_REG           ((QMGR_MEM_BASE_ADDR) + 0x14000)
#define PP_QMGR_MEM_IMMEDIATE_POP_RSVD0_OFF                         (9)
#define PP_QMGR_MEM_IMMEDIATE_POP_RSVD0_LEN                         (23)
#define PP_QMGR_MEM_IMMEDIATE_POP_RSVD0_MSK                         (0xFFFFFE00)
#define PP_QMGR_MEM_IMMEDIATE_POP_RSVD0_RST                         (0x0)
#define PP_QMGR_MEM_IMMEDIATE_POP_OFF                               (0)
#define PP_QMGR_MEM_IMMEDIATE_POP_LEN                               (9)
#define PP_QMGR_MEM_IMMEDIATE_POP_MSK                               (0x000001FF)
#define PP_QMGR_MEM_IMMEDIATE_POP_RST                               (0x0)
/**
 * REG_IDX_ACCESS   : PP_QMGR_MEM_IMMEDIATE_POP_REG_IDX
 * NUM OF REGISTERS : 512
 */
#define PP_QMGR_MEM_IMMEDIATE_POP_REG_IDX(idx) \
	(PP_QMGR_MEM_IMMEDIATE_POP_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_QMGR_MEM_CACHE2DDR_REG
 * HW_REG_NAME : cache2ddr
 * DESCRIPTION : Cache to DDR
 *
 *  Register Fields :
 *   [31:16][RO] - Reserved
 *   [15: 0][RO] - Cache to DDR
 *
 */
#define PP_QMGR_MEM_CACHE2DDR_REG               ((QMGR_MEM_BASE_ADDR) + 0x16000)
#define PP_QMGR_MEM_CACHE2DDR_RSVD0_OFF                             (16)
#define PP_QMGR_MEM_CACHE2DDR_RSVD0_LEN                             (16)
#define PP_QMGR_MEM_CACHE2DDR_RSVD0_MSK                             (0xFFFF0000)
#define PP_QMGR_MEM_CACHE2DDR_RSVD0_RST                             (0x0)
#define PP_QMGR_MEM_CACHE2DDR_IMMEDIATE_POP_OFF                     (0)
#define PP_QMGR_MEM_CACHE2DDR_IMMEDIATE_POP_LEN                     (16)
#define PP_QMGR_MEM_CACHE2DDR_IMMEDIATE_POP_MSK                     (0x0000FFFF)
#define PP_QMGR_MEM_CACHE2DDR_IMMEDIATE_POP_RST                     (0x0)
/**
 * REG_IDX_ACCESS   : PP_QMGR_MEM_CACHE2DDR_REG_IDX
 * NUM OF REGISTERS : 1152
 */
#define PP_QMGR_MEM_CACHE2DDR_REG_IDX(idx) \
	(PP_QMGR_MEM_CACHE2DDR_REG + ((idx) << 2))

#endif
