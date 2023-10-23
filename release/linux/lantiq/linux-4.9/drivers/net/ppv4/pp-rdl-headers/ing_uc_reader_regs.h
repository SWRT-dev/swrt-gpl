/**
 * ing_uc_reader_regs.h
 * Description: ing_uc_reader_s HW registers definitions
 *              NOTE: B Step ONLY module
 * Auto-generated file by RDL Parser, DO NOT CHANGE
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2019-2020 Intel Corporation
 */

#ifndef _PP_UC_ING_READER_H_
#define _PP_UC_ING_READER_H_

#define PP_UC_ING_READER_GEN_DATE_STR         "Sunday May 17, 2020 [5:26:11 pm]"
#define PP_UC_ING_READER_BASE                                    (0xF0BE8000ULL)

/**
 * SW_REG_NAME : PP_UC_ING_READER_WRED_BASE_REG
 * HW_REG_NAME : wred_base
 * DESCRIPTION : WRED base address, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][RW] - address of port #0 in WRED. The address of queue q
 *                 is (q((4)|WRED_base. Programmed by Host at init
 *
 */
#define PP_UC_ING_READER_WRED_BASE_REG         ((UC_ING_READER_BASE_ADDR) + 0x0)
#define PP_UC_ING_READER_WRED_BASE_OFF                              (0)
#define PP_UC_ING_READER_WRED_BASE_LEN                              (32)
#define PP_UC_ING_READER_WRED_BASE_MSK                              (0xFFFFFFFF)
#define PP_UC_ING_READER_WRED_BASE_RST                              (0x0)

/**
 * SW_REG_NAME : PP_UC_ING_READER_QMGR_BASE_REG
 * HW_REG_NAME : qmgr_base
 * DESCRIPTION : QMGR base address, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][RW] - address of port #0 in QMngr. The address of queue
 *                 q is (q((4)|QMngr_base.         Programmed by Host
 *                 at init
 *
 */
#define PP_UC_ING_READER_QMGR_BASE_REG       ((UC_ING_READER_BASE_ADDR) + 0x400)
#define PP_UC_ING_READER_QMGR_BASE_OFF                              (0)
#define PP_UC_ING_READER_QMGR_BASE_LEN                              (32)
#define PP_UC_ING_READER_QMGR_BASE_MSK                              (0xFFFFFFFF)
#define PP_UC_ING_READER_QMGR_BASE_RST                              (0x0)

/**
 * SW_REG_NAME : PP_UC_ING_READER_WRED_CLIENT_BASE_REG
 * HW_REG_NAME : wred_client_base
 * DESCRIPTION : WRED client base address, NOTE, B Step ONLY
 *               register
 *
 *  Register Fields :
 *   [31:16][RO] - Reserved
 *   [15: 0][RW] - Used by port of type from_txmngr to build the
 *                 field Client address in the WRED Q push command.
 *                 Programmed by Host at init
 *
 */
#define PP_UC_ING_READER_WRED_CLIENT_BASE_REG \
	((UC_ING_READER_BASE_ADDR) + 0x800)
#define PP_UC_ING_READER_WRED_CLIENT_BASE_RSVD0_OFF                 (16)
#define PP_UC_ING_READER_WRED_CLIENT_BASE_RSVD0_LEN                 (16)
#define PP_UC_ING_READER_WRED_CLIENT_BASE_RSVD0_MSK                 (0xFFFF0000)
#define PP_UC_ING_READER_WRED_CLIENT_BASE_RSVD0_RST                 (0x0)
#define PP_UC_ING_READER_WRED_CLIENT_BASE_OFF                       (0)
#define PP_UC_ING_READER_WRED_CLIENT_BASE_LEN                       (16)
#define PP_UC_ING_READER_WRED_CLIENT_BASE_MSK                       (0x0000FFFF)
#define PP_UC_ING_READER_WRED_CLIENT_BASE_RST                       (0x0)

/**
 * SW_REG_NAME : PP_UC_ING_READER_WRED_CLIENT_RESP_REG
 * HW_REG_NAME : wred_client_resp
 * DESCRIPTION : WRED client response address, NOTE, B Step ONLY
 *               register
 *
 *  Register Fields :
 *   [31: 8][RO] - Reserved
 *   [ 7: 0][RW] - WRED_client_resp[p][0] holds the response command
 *                 of WRED upon push command by port            Note:
 *                 the used entries of the WRED_client array are at
 *                 offsets 0x0,0x8,0x10,0x18... the reason is the
 *                 usage by WRED via 64b bus
 *
 */
#define PP_UC_ING_READER_WRED_CLIENT_RESP_REG \
	((UC_ING_READER_BASE_ADDR) + 0xc00)
#define PP_UC_ING_READER_WRED_CLIENT_RESP_RSVD0_OFF                 (8)
#define PP_UC_ING_READER_WRED_CLIENT_RESP_RSVD0_LEN                 (24)
#define PP_UC_ING_READER_WRED_CLIENT_RESP_RSVD0_MSK                 (0xFFFFFF00)
#define PP_UC_ING_READER_WRED_CLIENT_RESP_RSVD0_RST                 (0x0)
#define PP_UC_ING_READER_WRED_CLIENT_RESP_OFF                       (0)
#define PP_UC_ING_READER_WRED_CLIENT_RESP_LEN                       (8)
#define PP_UC_ING_READER_WRED_CLIENT_RESP_MSK                       (0x000000FF)
#define PP_UC_ING_READER_WRED_CLIENT_RESP_RST                       (0x0)
/**
 * REG_IDX_ACCESS   : PP_UC_ING_READER_WRED_CLIENT_RESP_REG_IDX
 * NUM OF REGISTERS : 2
 */
#define PP_UC_ING_READER_WRED_CLIENT_RESP_REG_IDX(idx) \
	(PP_UC_ING_READER_WRED_CLIENT_RESP_REG + ((idx) << 3))

/**
 * SW_REG_NAME : PP_UC_ING_READER_BMGR_BASE_REG
 * HW_REG_NAME : bmgr_base
 * DESCRIPTION : BMGR base address, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31:16][RO] - Reserved
 *   [15: 0][RW] - Holds the base address of the BMngr (buffer
 *                 manager). Programmed by Host at init.         Used
 *                 by a port of type from_txmngr to reclaim buffer in
 *                 case the push (to QMngr) failed.         Note that
 *                 when the Reader tries to access the BMngr, the
 *                 Cluster Bridge replaces the 4 MSBs of the addr,
 *                 namely BMngr_base[15:12], by the 8 bits
 *                 FAT[BMngr_base[15:12]][7:0]
 *
 */
#define PP_UC_ING_READER_BMGR_BASE_REG      ((UC_ING_READER_BASE_ADDR) + 0x1000)
#define PP_UC_ING_READER_BMGR_BASE_RSVD0_OFF                        (16)
#define PP_UC_ING_READER_BMGR_BASE_RSVD0_LEN                        (16)
#define PP_UC_ING_READER_BMGR_BASE_RSVD0_MSK                        (0xFFFF0000)
#define PP_UC_ING_READER_BMGR_BASE_RSVD0_RST                        (0x0)
#define PP_UC_ING_READER_BMGR_BASE_OFF                              (0)
#define PP_UC_ING_READER_BMGR_BASE_LEN                              (16)
#define PP_UC_ING_READER_BMGR_BASE_MSK                              (0x0000FFFF)
#define PP_UC_ING_READER_BMGR_BASE_RST                              (0x0)

/**
 * SW_REG_NAME : PP_UC_ING_READER_IRR_REG
 * HW_REG_NAME : irr
 * DESCRIPTION : Interrupt Request Register, NOTE, B Step ONLY
 *               register
 *
 *  Register Fields :
 *   [31: 2][RO] - Reserved
 *   [ 1: 0][RO] - interrupt request register (RO). bit per port of
 *                 type eg_mbox (=channel). Used by host to identify
 *                 the interrupt source
 *
 */
#define PP_UC_ING_READER_IRR_REG            ((UC_ING_READER_BASE_ADDR) + 0x1400)
#define PP_UC_ING_READER_IRR_RSVD0_OFF                              (2)
#define PP_UC_ING_READER_IRR_RSVD0_LEN                              (30)
#define PP_UC_ING_READER_IRR_RSVD0_MSK                              (0xFFFFFFFC)
#define PP_UC_ING_READER_IRR_RSVD0_RST                              (0x0)
#define PP_UC_ING_READER_IRR_OFF                                    (0)
#define PP_UC_ING_READER_IRR_LEN                                    (2)
#define PP_UC_ING_READER_IRR_MSK                                    (0x00000003)
#define PP_UC_ING_READER_IRR_RST                                    (0x0)

/**
 * SW_REG_NAME : PP_UC_ING_READER_IER_REG
 * HW_REG_NAME : ier
 * DESCRIPTION : Interrupt Enable Register, NOTE, B Step ONLY
 *               register
 *
 *  Register Fields :
 *   [31: 2][RO] - Reserved
 *   [ 1: 0][RW] - interrupt enable register (RW); bit per channel.
 *                 Programmed at init.         By default IER is all
 *                 0, namely interrupt disable
 *
 */
#define PP_UC_ING_READER_IER_REG            ((UC_ING_READER_BASE_ADDR) + 0x1800)
#define PP_UC_ING_READER_IER_RSVD0_OFF                              (2)
#define PP_UC_ING_READER_IER_RSVD0_LEN                              (30)
#define PP_UC_ING_READER_IER_RSVD0_MSK                              (0xFFFFFFFC)
#define PP_UC_ING_READER_IER_RSVD0_RST                              (0x0)
#define PP_UC_ING_READER_IER_OFF                                    (0)
#define PP_UC_ING_READER_IER_LEN                                    (2)
#define PP_UC_ING_READER_IER_MSK                                    (0x00000003)
#define PP_UC_ING_READER_IER_RST                                    (0x0)

/**
 * SW_REG_NAME : PP_UC_ING_READER_ICR_REG
 * HW_REG_NAME : icr
 * DESCRIPTION : Interrupt Clear Register, NOTE, B Step ONLY
 *               register
 *
 *  Register Fields :
 *   [31: 2][RO] - Reserved
 *   [ 1: 0][RW] - interrupt clear register (WO); bit per channel.
 *                 Used by host to clear bits in IRR
 *
 */
#define PP_UC_ING_READER_ICR_REG            ((UC_ING_READER_BASE_ADDR) + 0x1C00)
#define PP_UC_ING_READER_ICR_RSVD0_OFF                              (2)
#define PP_UC_ING_READER_ICR_RSVD0_LEN                              (30)
#define PP_UC_ING_READER_ICR_RSVD0_MSK                              (0xFFFFFFFC)
#define PP_UC_ING_READER_ICR_RSVD0_RST                              (0x0)
#define PP_UC_ING_READER_ICR_OFF                                    (0)
#define PP_UC_ING_READER_ICR_LEN                                    (2)
#define PP_UC_ING_READER_ICR_MSK                                    (0x00000003)
#define PP_UC_ING_READER_ICR_RST                                    (0x0)

/**
 * SW_REG_NAME : PP_UC_ING_READER_PORT_SPECIFIC_REG
 * HW_REG_NAME : port_specific
 * DESCRIPTION : Port specific, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][RW] - place holder for the specific ports
 *
 */
#define PP_UC_ING_READER_PORT_SPECIFIC_REG  ((UC_ING_READER_BASE_ADDR) + 0x2000)
#define PP_UC_ING_READER_PORT_SPECIFIC_OFF                          (0)
#define PP_UC_ING_READER_PORT_SPECIFIC_LEN                          (32)
#define PP_UC_ING_READER_PORT_SPECIFIC_MSK                          (0xFFFFFFFF)
#define PP_UC_ING_READER_PORT_SPECIFIC_RST                          (0x0)
/**
 * REG_IDX_ACCESS   : PP_UC_ING_READER_PORT_SPECIFIC_REG_IDX
 * NUM OF REGISTERS : 6
 */
#define PP_UC_ING_READER_PORT_SPECIFIC_REG_IDX(idx) \
	(PP_UC_ING_READER_PORT_SPECIFIC_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_UC_ING_READER_PORT_CNT_REG
 * HW_REG_NAME : port_counter
 * DESCRIPTION : Port counter, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][RO] - place holder for the specific ports
 *
 */
#define PP_UC_ING_READER_PORT_CNT_REG       ((UC_ING_READER_BASE_ADDR) + 0x2400)
#define PP_UC_ING_READER_PORT_CNT_OFF                               (0)
#define PP_UC_ING_READER_PORT_CNT_LEN                               (32)
#define PP_UC_ING_READER_PORT_CNT_MSK                               (0xFFFFFFFF)
#define PP_UC_ING_READER_PORT_CNT_RST                               (0x0)
/**
 * REG_IDX_ACCESS   : PP_UC_ING_READER_PORT_CNT_REG_IDX
 * NUM OF REGISTERS : 2
 */
#define PP_UC_ING_READER_PORT_CNT_REG_IDX(idx) \
	(PP_UC_ING_READER_PORT_CNT_REG + ((idx) << 2))

#endif
