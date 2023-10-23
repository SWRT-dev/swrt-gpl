/**
 * qmgr_pqm_regs.h
 * Description: qmgr_pqm_s HW registers definitions
 * Auto-generated file by RDL Parser, DO NOT CHANGE
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2019-2020 Intel Corporation
 */

#ifndef _PP_QMGR_PQM_H_
#define _PP_QMGR_PQM_H_

#define PP_QMGR_PQM_GEN_DATE_STR              "Sunday May 17, 2020 [5:26:11 pm]"
#define PP_QMGR_PQM_BASE                                           (0x100000ULL)

/**
 * SW_REG_NAME : PP_QMGR_PQM_LRAM_R0_BA_REG
 * HW_REG_NAME : lram_r0_ba
 * DESCRIPTION : The Linking RAM Region 0 Base Address Register is
 *               used to set the base address for the first portion
 *               of the Linking RAM. This address must be 32-bit
 *               aligned. It is used by the Queue Manager to
 *               calculate the 32-bit linking address for a given
 *               descriptor index. It does not support byte
 *               accesses
 *
 *  Register Fields :
 *   [31: 0][RW] - Linking RAM Region 0 Base Address Register
 *
 */
#define PP_QMGR_PQM_LRAM_R0_BA_REG                 ((QMGR_PQM_BASE_ADDR) + 0x80)
#define PP_QMGR_PQM_LRAM_R0_BA_REGION0_BASE_OFF                     (0)
#define PP_QMGR_PQM_LRAM_R0_BA_REGION0_BASE_LEN                     (32)
#define PP_QMGR_PQM_LRAM_R0_BA_REGION0_BASE_MSK                     (0xFFFFFFFF)
#define PP_QMGR_PQM_LRAM_R0_BA_REGION0_BASE_RST                     (0x0)

/**
 * SW_REG_NAME : PP_QMGR_PQM_LRAM_R0_SIZE_REG
 * HW_REG_NAME : lram_r0_size
 * DESCRIPTION : The Linking RAM Region 0 Size Register is used to
 *               set the size of the array of linking pointers that
 *               are located in Region 0 of Linking RAM. The size
 *               specified the number of descriptors for which
 *               linking information is stored in this region. It
 *               does not support byte accesses
 *
 *  Register Fields :
 *   [31:16][RO] - Reserved
 *   [15: 0][RW] - Linking RAM Region 0 Size Register
 *
 */
#define PP_QMGR_PQM_LRAM_R0_SIZE_REG               ((QMGR_PQM_BASE_ADDR) + 0x84)
#define PP_QMGR_PQM_LRAM_R0_SIZE_RSVD0_OFF                          (16)
#define PP_QMGR_PQM_LRAM_R0_SIZE_RSVD0_LEN                          (16)
#define PP_QMGR_PQM_LRAM_R0_SIZE_RSVD0_MSK                          (0xFFFF0000)
#define PP_QMGR_PQM_LRAM_R0_SIZE_RSVD0_RST                          (0x0)
#define PP_QMGR_PQM_LRAM_R0_SIZE_REGION0_SIZE_OFF                   (0)
#define PP_QMGR_PQM_LRAM_R0_SIZE_REGION0_SIZE_LEN                   (16)
#define PP_QMGR_PQM_LRAM_R0_SIZE_REGION0_SIZE_MSK                   (0x0000FFFF)
#define PP_QMGR_PQM_LRAM_R0_SIZE_REGION0_SIZE_RST                   (0x0)

/**
 * SW_REG_NAME : PP_QMGR_PQM_MEMION0_BA_REG
 * HW_REG_NAME : mem_region0_ba
 * DESCRIPTION : The Memory Region 0 Base Address Register is
 *               written by the Host to set the base address of
 *               memory region 0. This memory region will store a
 *               number of descriptors of a particular size as
 *               determined by the Memory Region 0 Control
 *               Register. It does not support byte accesses
 *
 *  Register Fields :
 *   [31: 0][RW] - This field contains the base address of the memory
 *                 region 0
 *
 */
#define PP_QMGR_PQM_MEMION0_BA_REG              ((QMGR_PQM_BASE_ADDR) + 0x10000)
#define PP_QMGR_PQM_MEMION0_BA_MEM_BASE_OFF                         (0)
#define PP_QMGR_PQM_MEMION0_BA_MEM_BASE_LEN                         (32)
#define PP_QMGR_PQM_MEMION0_BA_MEM_BASE_MSK                         (0xFFFFFFFF)
#define PP_QMGR_PQM_MEMION0_BA_MEM_BASE_RST                         (0x0)

/**
 * SW_REG_NAME : PP_QMGR_PQM_MEMION0_CTL_REG
 * HW_REG_NAME : mem_region0_ctl
 * DESCRIPTION : The Memory Region 0 Control Register is written by
 *               the Host to configure various parameters of this
 *               memory region. It does not support byte accesses
 *
 *  Register Fields :
 *   [31:15][RW] - This field indicates where in linking RAM does the
 *                 descriptor linking information corresponding to
 *                 memory region 0 starts
 *   [14:12][RO] - Reserved
 *   [11: 8][RW] - This field indicates the size of each descriptor
 *                 in this memory region. It is an encoded value that
 *                 specifies descriptor size as 2^(5+desc_size)
 *                 number of bytes. The settings of desc_size from
 *                 9-15 are reserved
 *   [ 7: 3][RO] - Reserved
 *   [ 2: 0][RW] - This field indicates the size of the memory region
 *                 (in terms of number of descriptors). It is an
 *                 encoded value that specifies region size as
 *                 2^(5+reg_size) number of descriptors. Note: this
 *                 field is doesnt affect the Queue Manager behavior
 *                 (up to 64K descriptors can be used regardless of
 *                 written value)
 *
 */
#define PP_QMGR_PQM_MEMION0_CTL_REG             ((QMGR_PQM_BASE_ADDR) + 0x10004)
#define PP_QMGR_PQM_MEMION0_CTL_START_INDEX_OFF                     (15)
#define PP_QMGR_PQM_MEMION0_CTL_START_INDEX_LEN                     (17)
#define PP_QMGR_PQM_MEMION0_CTL_START_INDEX_MSK                     (0xFFFF8000)
#define PP_QMGR_PQM_MEMION0_CTL_START_INDEX_RST                     (0x0)
#define PP_QMGR_PQM_MEMION0_CTL_RSVD0_OFF                           (12)
#define PP_QMGR_PQM_MEMION0_CTL_RSVD0_LEN                           (3)
#define PP_QMGR_PQM_MEMION0_CTL_RSVD0_MSK                           (0x00007000)
#define PP_QMGR_PQM_MEMION0_CTL_RSVD0_RST                           (0x0)
#define PP_QMGR_PQM_MEMION0_CTL_DESC_SIZE_OFF                       (8)
#define PP_QMGR_PQM_MEMION0_CTL_DESC_SIZE_LEN                       (4)
#define PP_QMGR_PQM_MEMION0_CTL_DESC_SIZE_MSK                       (0x00000F00)
#define PP_QMGR_PQM_MEMION0_CTL_DESC_SIZE_RST                       (0x0)
#define PP_QMGR_PQM_MEMION0_CTL_RSVD1_OFF                           (3)
#define PP_QMGR_PQM_MEMION0_CTL_RSVD1_LEN                           (5)
#define PP_QMGR_PQM_MEMION0_CTL_RSVD1_MSK                           (0x000000F8)
#define PP_QMGR_PQM_MEMION0_CTL_RSVD1_RST                           (0x0)
#define PP_QMGR_PQM_MEMION0_CTL_SIZE_OFF                            (0)
#define PP_QMGR_PQM_MEMION0_CTL_SIZE_LEN                            (3)
#define PP_QMGR_PQM_MEMION0_CTL_SIZE_MSK                            (0x00000007)
#define PP_QMGR_PQM_MEMION0_CTL_SIZE_RST                            (0x0)

/**
 * SW_REG_NAME : PP_QMGR_PQM_FREE_Q_INIT_REG
 * HW_REG_NAME : free_q_init
 * DESCRIPTION : The Free Queue Init Register is written to add a
 *               page to the queue. It does not support byte
 *               accesses
 *
 *  Register Fields :
 *   [31: 5][RW] - Page pointer. It will indicate a 32-bit aligned
 *                 address that points to a page when the queue is
 *                 not empty.
 *   [ 4: 0][RO] - Reserved
 *
 */
#define PP_QMGR_PQM_FREE_Q_INIT_REG             ((QMGR_PQM_BASE_ADDR) + 0x2100C)
#define PP_QMGR_PQM_FREE_Q_INIT_PAGE_PTR_OFF                        (5)
#define PP_QMGR_PQM_FREE_Q_INIT_PAGE_PTR_LEN                        (27)
#define PP_QMGR_PQM_FREE_Q_INIT_PAGE_PTR_MSK                        (0xFFFFFFE0)
#define PP_QMGR_PQM_FREE_Q_INIT_PAGE_PTR_RST                        (0x0)
#define PP_QMGR_PQM_FREE_Q_INIT_RSVD0_OFF                           (0)
#define PP_QMGR_PQM_FREE_Q_INIT_RSVD0_LEN                           (5)
#define PP_QMGR_PQM_FREE_Q_INIT_RSVD0_MSK                           (0x0000001F)
#define PP_QMGR_PQM_FREE_Q_INIT_RSVD0_RST                           (0x0)

#endif
