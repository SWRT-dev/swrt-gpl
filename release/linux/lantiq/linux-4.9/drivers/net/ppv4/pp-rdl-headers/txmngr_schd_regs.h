/**
 * txmngr_schd_regs.h
 * Description: txmngr_schd_s HW registers definitions
 * Auto-generated file by RDL Parser, DO NOT CHANGE
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2019-2020 Intel Corporation
 */

#ifndef _PP_TX_MGR_SCHED_H_
#define _PP_TX_MGR_SCHED_H_

#define PP_TX_MGR_SCHED_GEN_DATE_STR          "Sunday May 17, 2020 [5:26:11 pm]"
#define PP_TX_MGR_SCHED_BASE                                     (0xF1200800ULL)

/**
 * SW_REG_NAME : PP_TX_MGR_SCHED_TXMNGR_SCHD_CMD_REG
 * HW_REG_NAME : txmngr_schd_cmd_reg
 * DESCRIPTION : Scheduler command
 *
 *  Register Fields :
 *   [63: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_TX_MGR_SCHED_TXMNGR_SCHD_CMD_REG     ((TX_MGR_SCHED_BASE_ADDR) + 0x0)
#define PP_TX_MGR_SCHED_TXMNGR_SCHD_CMD_OFF                         (0)
#define PP_TX_MGR_SCHED_TXMNGR_SCHD_CMD_LEN                         (64)
#define PP_TX_MGR_SCHED_TXMNGR_SCHD_CMD_MSK                 (0xFFFFFFFFFFFFFFFF)
#define PP_TX_MGR_SCHED_TXMNGR_SCHD_CMD_RST                         (0x0)

#endif
