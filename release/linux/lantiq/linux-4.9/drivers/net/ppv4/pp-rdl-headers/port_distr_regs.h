/**
 * port_distr_regs.h
 * Description: port_distr_s HW registers definitions
 * Auto-generated file by RDL Parser, DO NOT CHANGE
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2019-2020 Intel Corporation
 */

#ifndef _PP_PORT_DIST_H_
#define _PP_PORT_DIST_H_

#define PP_PORT_DIST_GEN_DATE_STR             "Sunday May 17, 2020 [5:26:11 pm]"
#define PP_PORT_DIST_BASE                                        (0xF0004000ULL)

/**
 * SW_REG_NAME : PP_PORT_DIST_SOURCE_PORT_TO_RPB_PORT_REG
 * HW_REG_NAME : port_distr_source_port_to_rpb_port
 * DESCRIPTION : match source port n to rpb port and tc + tc select
 *               - 0 to take tc from status word, 1 to take tc from
 *               table.
 *
 *  Register Fields :
 *   [31: 5][RO] - Reserved
 *   [ 4: 3][RW] - MISSING_DESCRIPTION
 *   [ 2: 2][RW] - MISSING_DESCRIPTION
 *   [ 1: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_PORT_DIST_SOURCE_PORT_TO_RPB_PORT_REG   ((PORT_DIST_BASE_ADDR) + 0x0)
#define PP_PORT_DIST_SOURCE_PORT_TO_RPB_PORT_RESERVED_1_OFF         (5)
#define PP_PORT_DIST_SOURCE_PORT_TO_RPB_PORT_RESERVED_1_LEN         (27)
#define PP_PORT_DIST_SOURCE_PORT_TO_RPB_PORT_RESERVED_1_MSK         (0xFFFFFFE0)
#define PP_PORT_DIST_SOURCE_PORT_TO_RPB_PORT_RESERVED_1_RST         (0x0)
#define PP_PORT_DIST_SOURCE_PORT_TO_RPB_PORT_TC_OFF                 (3)
#define PP_PORT_DIST_SOURCE_PORT_TO_RPB_PORT_TC_LEN                 (2)
#define PP_PORT_DIST_SOURCE_PORT_TO_RPB_PORT_TC_MSK                 (0x00000018)
#define PP_PORT_DIST_SOURCE_PORT_TO_RPB_PORT_TC_RST                 (0x0)
#define PP_PORT_DIST_SOURCE_PORT_TO_RPB_PORT_TC_SELECT_OFF          (2)
#define PP_PORT_DIST_SOURCE_PORT_TO_RPB_PORT_TC_SELECT_LEN          (1)
#define PP_PORT_DIST_SOURCE_PORT_TO_RPB_PORT_TC_SELECT_MSK          (0x00000004)
#define PP_PORT_DIST_SOURCE_PORT_TO_RPB_PORT_TC_SELECT_RST          (0x0)
#define PP_PORT_DIST_SOURCE_PORT_TO_RPB_PORT_OFF                    (0)
#define PP_PORT_DIST_SOURCE_PORT_TO_RPB_PORT_LEN                    (2)
#define PP_PORT_DIST_SOURCE_PORT_TO_RPB_PORT_MSK                    (0x00000003)
#define PP_PORT_DIST_SOURCE_PORT_TO_RPB_PORT_RST                    (0x0)
/**
 * REG_IDX_ACCESS   : PP_PORT_DIST_SOURCE_PORT_TO_RPB_PORT_REG_IDX
 * NUM OF REGISTERS : 256
 */
#define PP_PORT_DIST_SOURCE_PORT_TO_RPB_PORT_REG_IDX(idx) \
	(PP_PORT_DIST_SOURCE_PORT_TO_RPB_PORT_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_PORT_DIST_PKT_CNT_REG
 * HW_REG_NAME : port_distr_pkt_cnt
 * DESCRIPTION : counts packets transmitted. counter n for port i
 *               tc j: n=4*i+j
 *
 *  Register Fields :
 *   [31: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_PORT_DIST_PKT_CNT_REG                 ((PORT_DIST_BASE_ADDR) + 0x400)
#define PP_PORT_DIST_PKT_CNT_OFF                                    (0)
#define PP_PORT_DIST_PKT_CNT_LEN                                    (32)
#define PP_PORT_DIST_PKT_CNT_MSK                                    (0xFFFFFFFF)
#define PP_PORT_DIST_PKT_CNT_RST                                    (0x0)
/**
 * REG_IDX_ACCESS   : PP_PORT_DIST_PKT_CNT_REG_IDX
 * NUM OF REGISTERS : 16
 */
#define PP_PORT_DIST_PKT_CNT_REG_IDX(idx) \
	(PP_PORT_DIST_PKT_CNT_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_PORT_DIST_BYTE_CNT_REG
 * HW_REG_NAME : port_distr_byte_cnt
 * DESCRIPTION : counts bytes transmitted. counter n for port i tc
 *               j: n=4*i+j
 *
 *  Register Fields :
 *   [31: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_PORT_DIST_BYTE_CNT_REG                ((PORT_DIST_BASE_ADDR) + 0x440)
#define PP_PORT_DIST_BYTE_CNT_OFF                                   (0)
#define PP_PORT_DIST_BYTE_CNT_LEN                                   (32)
#define PP_PORT_DIST_BYTE_CNT_MSK                                   (0xFFFFFFFF)
#define PP_PORT_DIST_BYTE_CNT_RST                                   (0x0)
/**
 * REG_IDX_ACCESS   : PP_PORT_DIST_BYTE_CNT_REG_IDX
 * NUM OF REGISTERS : 16
 */
#define PP_PORT_DIST_BYTE_CNT_REG_IDX(idx) \
	(PP_PORT_DIST_BYTE_CNT_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_PORT_DIST_INIT_DONE_REG
 * HW_REG_NAME : port_distr_init_done
 * DESCRIPTION : counters memories zero init done.
 *
 *  Register Fields :
 *   [31: 1][RO] - Reserved
 *   [ 0: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_PORT_DIST_INIT_DONE_REG               ((PORT_DIST_BASE_ADDR) + 0x480)
#define PP_PORT_DIST_INIT_DONE_RSVD0_OFF                            (1)
#define PP_PORT_DIST_INIT_DONE_RSVD0_LEN                            (31)
#define PP_PORT_DIST_INIT_DONE_RSVD0_MSK                            (0xFFFFFFFE)
#define PP_PORT_DIST_INIT_DONE_RSVD0_RST                            (0x0)
#define PP_PORT_DIST_INIT_DONE_OFF                                  (0)
#define PP_PORT_DIST_INIT_DONE_LEN                                  (1)
#define PP_PORT_DIST_INIT_DONE_MSK                                  (0x00000001)
#define PP_PORT_DIST_INIT_DONE_RST                                  (0x0)

/**
 * SW_REG_NAME : PP_PORT_DIST_PORT_BYTE_CNT_LO_REG
 * HW_REG_NAME : port_distr_port_byte_cnt_lo
 * DESCRIPTION : counts bytes transmitted. 48 bit counter
 *
 *  Register Fields :
 *   [31: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_PORT_DIST_PORT_BYTE_CNT_LO_REG        ((PORT_DIST_BASE_ADDR) + 0x500)
#define PP_PORT_DIST_PORT_BYTE_CNT_LO_OFF                           (0)
#define PP_PORT_DIST_PORT_BYTE_CNT_LO_LEN                           (32)
#define PP_PORT_DIST_PORT_BYTE_CNT_LO_MSK                           (0xFFFFFFFF)
#define PP_PORT_DIST_PORT_BYTE_CNT_LO_RST                           (0x0)
/**
 * REG_IDX_ACCESS   : PP_PORT_DIST_PORT_BYTE_CNT_LO_REG_IDX
 * NUM OF REGISTERS : 256
 */
#define PP_PORT_DIST_PORT_BYTE_CNT_LO_REG_IDX(idx) \
	(PP_PORT_DIST_PORT_BYTE_CNT_LO_REG + ((idx) << 3))

/**
 * SW_REG_NAME : PP_PORT_DIST_PORT_BYTE_CNT_HI_REG
 * HW_REG_NAME : port_distr_port_byte_cnt_hi
 * DESCRIPTION : counts bytes transmitted. 48 bit counter
 *
 *  Register Fields :
 *   [31:16][RO] - Reserved
 *   [15: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_PORT_DIST_PORT_BYTE_CNT_HI_REG        ((PORT_DIST_BASE_ADDR) + 0x504)
#define PP_PORT_DIST_PORT_BYTE_CNT_HI_RSVD0_OFF                     (16)
#define PP_PORT_DIST_PORT_BYTE_CNT_HI_RSVD0_LEN                     (16)
#define PP_PORT_DIST_PORT_BYTE_CNT_HI_RSVD0_MSK                     (0xFFFF0000)
#define PP_PORT_DIST_PORT_BYTE_CNT_HI_RSVD0_RST                     (0x0)
#define PP_PORT_DIST_PORT_BYTE_CNT_HI_OFF                           (0)
#define PP_PORT_DIST_PORT_BYTE_CNT_HI_LEN                           (16)
#define PP_PORT_DIST_PORT_BYTE_CNT_HI_MSK                           (0x0000FFFF)
#define PP_PORT_DIST_PORT_BYTE_CNT_HI_RST                           (0x0)
/**
 * REG_IDX_ACCESS   : PP_PORT_DIST_PORT_BYTE_CNT_HI_REG_IDX
 * NUM OF REGISTERS : 256
 */
#define PP_PORT_DIST_PORT_BYTE_CNT_HI_REG_IDX(idx) \
	(PP_PORT_DIST_PORT_BYTE_CNT_HI_REG + ((idx) << 3))

/**
 * SW_REG_NAME : PP_PORT_DIST_PORT_PKT_CNT_LO_REG
 * HW_REG_NAME : port_distr_port_pkt_cnt_lo
 * DESCRIPTION : counts pkts transmitted. 40 bit counter
 *
 *  Register Fields :
 *   [31: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_PORT_DIST_PORT_PKT_CNT_LO_REG         ((PORT_DIST_BASE_ADDR) + 0xd00)
#define PP_PORT_DIST_PORT_PKT_CNT_LO_OFF                            (0)
#define PP_PORT_DIST_PORT_PKT_CNT_LO_LEN                            (32)
#define PP_PORT_DIST_PORT_PKT_CNT_LO_MSK                            (0xFFFFFFFF)
#define PP_PORT_DIST_PORT_PKT_CNT_LO_RST                            (0x0)
/**
 * REG_IDX_ACCESS   : PP_PORT_DIST_PORT_PKT_CNT_LO_REG_IDX
 * NUM OF REGISTERS : 256
 */
#define PP_PORT_DIST_PORT_PKT_CNT_LO_REG_IDX(idx) \
	(PP_PORT_DIST_PORT_PKT_CNT_LO_REG + ((idx) << 3))

/**
 * SW_REG_NAME : PP_PORT_DIST_PORT_PKT_CNT_HI_REG
 * HW_REG_NAME : port_distr_port_pkt_cnt_hi
 * DESCRIPTION : counts pkts transmitted. 40 bit counter
 *
 *  Register Fields :
 *   [31: 8][RO] - Reserved
 *   [ 7: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_PORT_DIST_PORT_PKT_CNT_HI_REG         ((PORT_DIST_BASE_ADDR) + 0xd04)
#define PP_PORT_DIST_PORT_PKT_CNT_HI_RSVD0_OFF                      (8)
#define PP_PORT_DIST_PORT_PKT_CNT_HI_RSVD0_LEN                      (24)
#define PP_PORT_DIST_PORT_PKT_CNT_HI_RSVD0_MSK                      (0xFFFFFF00)
#define PP_PORT_DIST_PORT_PKT_CNT_HI_RSVD0_RST                      (0x0)
#define PP_PORT_DIST_PORT_PKT_CNT_HI_OFF                            (0)
#define PP_PORT_DIST_PORT_PKT_CNT_HI_LEN                            (8)
#define PP_PORT_DIST_PORT_PKT_CNT_HI_MSK                            (0x000000FF)
#define PP_PORT_DIST_PORT_PKT_CNT_HI_RST                            (0x0)
/**
 * REG_IDX_ACCESS   : PP_PORT_DIST_PORT_PKT_CNT_HI_REG_IDX
 * NUM OF REGISTERS : 256
 */
#define PP_PORT_DIST_PORT_PKT_CNT_HI_REG_IDX(idx) \
	(PP_PORT_DIST_PORT_PKT_CNT_HI_REG + ((idx) << 3))

/**
 * SW_REG_NAME : PP_PORT_DIST_PKT_COUNT_REG
 * HW_REG_NAME : packet_count
 * DESCRIPTION : counts packet SOP at the period on cycles [define
 *               at the cycles_reg]
 *
 *  Register Fields :
 *   [31: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_PORT_DIST_PKT_COUNT_REG              ((PORT_DIST_BASE_ADDR) + 0x1500)
#define PP_PORT_DIST_PKT_COUNT_PKT_COUNT_I_OFF                      (0)
#define PP_PORT_DIST_PKT_COUNT_PKT_COUNT_I_LEN                      (32)
#define PP_PORT_DIST_PKT_COUNT_PKT_COUNT_I_MSK                      (0xFFFFFFFF)
#define PP_PORT_DIST_PKT_COUNT_PKT_COUNT_I_RST                      (0x0)

/**
 * SW_REG_NAME : PP_PORT_DIST_CYCLES_PERIOD_REG
 * HW_REG_NAME : cycles_period
 * DESCRIPTION : define the period cycles which the packet count
 *               register work
 *
 *  Register Fields :
 *   [31: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_PORT_DIST_CYCLES_PERIOD_REG          ((PORT_DIST_BASE_ADDR) + 0x1504)
#define PP_PORT_DIST_CYCLES_PERIOD_CYCLES_PERIOD_I_OFF              (0)
#define PP_PORT_DIST_CYCLES_PERIOD_CYCLES_PERIOD_I_LEN              (32)
#define PP_PORT_DIST_CYCLES_PERIOD_CYCLES_PERIOD_I_MSK              (0xFFFFFFFF)
#define PP_PORT_DIST_CYCLES_PERIOD_CYCLES_PERIOD_I_RST              (0x0)

/**
 * SW_REG_NAME : PP_PORT_DIST_MPPS_GO_REG
 * HW_REG_NAME : mpps_go
 * DESCRIPTION : mpps_go ; start to count the MPPS [ 1- start
 *               counting ; read 0 mean that the period cycle done
 *
 *  Register Fields :
 *   [31: 1][RO] - Reserved
 *   [ 0: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_PORT_DIST_MPPS_GO_REG                ((PORT_DIST_BASE_ADDR) + 0x1508)
#define PP_PORT_DIST_MPPS_GO_RSVD0_OFF                              (1)
#define PP_PORT_DIST_MPPS_GO_RSVD0_LEN                              (31)
#define PP_PORT_DIST_MPPS_GO_RSVD0_MSK                              (0xFFFFFFFE)
#define PP_PORT_DIST_MPPS_GO_RSVD0_RST                              (0x0)
#define PP_PORT_DIST_MPPS_GO_GO_I_OFF                               (0)
#define PP_PORT_DIST_MPPS_GO_GO_I_LEN                               (1)
#define PP_PORT_DIST_MPPS_GO_GO_I_MSK                               (0x00000001)
#define PP_PORT_DIST_MPPS_GO_GO_I_RST                               (0x0)

/**
 * SW_REG_NAME : PP_PORT_DIST_STATUS_DBG_REG
 * HW_REG_NAME : status_dbg_reg
 * DESCRIPTION : status_dbg -capture the input status for debug ;
 *               will be sticky when Error_bit is aserted in the
 *               STW ; write status_dbg[0] will release the sticky
 *
 *  Register Fields :
 *   [31: 0][RW] - status_dbg
 *
 */
#define PP_PORT_DIST_STATUS_DBG_REG             ((PORT_DIST_BASE_ADDR) + 0x150C)
#define PP_PORT_DIST_STATUS_DBG_STATUS_DBG_I_OFF                    (0)
#define PP_PORT_DIST_STATUS_DBG_STATUS_DBG_I_LEN                    (32)
#define PP_PORT_DIST_STATUS_DBG_STATUS_DBG_I_MSK                    (0xFFFFFFFF)
#define PP_PORT_DIST_STATUS_DBG_STATUS_DBG_I_RST                    (0x0)
/**
 * REG_IDX_ACCESS   : PP_PORT_DIST_STATUS_DBG_REG_IDX
 * NUM OF REGISTERS : 5
 */
#define PP_PORT_DIST_STATUS_DBG_REG_IDX(idx) \
	(PP_PORT_DIST_STATUS_DBG_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_PORT_DIST_STATUS_FORCE_DBG0_REG
 * HW_REG_NAME : status_force_dbg0_reg
 * DESCRIPTION : [STEP B only] status_force_dbg - force the STW at
 *               the ppv4 input , NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31:26][RO] - Reserved
 *   [25:18][RW] - force port dbg
 *   [17:17][RW] - force timestamp_indication dbg
 *   [16:15][RW] - force prel2_size dbg
 *   [14:11][RW] - force source pool dbg
 *   [10:10][RW] - force status tc dbg
 *   [ 9: 9][RW] - force status pmac_indication dbg
 *   [ 8: 0][RW] - force status data_offset
 *
 */
#define PP_PORT_DIST_STATUS_FORCE_DBG0_REG      ((PORT_DIST_BASE_ADDR) + 0x1520)
#define PP_PORT_DIST_STATUS_FORCE_DBG0_RSVD0_OFF                    (26)
#define PP_PORT_DIST_STATUS_FORCE_DBG0_RSVD0_LEN                    (6)
#define PP_PORT_DIST_STATUS_FORCE_DBG0_RSVD0_MSK                    (0xFC000000)
#define PP_PORT_DIST_STATUS_FORCE_DBG0_RSVD0_RST                    (0x0)
#define PP_PORT_DIST_STATUS_FORCE_DBG0_FORCE_PORT_DBG_I_OFF         (18)
#define PP_PORT_DIST_STATUS_FORCE_DBG0_FORCE_PORT_DBG_I_LEN         (8)
#define PP_PORT_DIST_STATUS_FORCE_DBG0_FORCE_PORT_DBG_I_MSK         (0x03FC0000)
#define PP_PORT_DIST_STATUS_FORCE_DBG0_FORCE_PORT_DBG_I_RST         (0x0)
#define PP_PORT_DIST_STATUS_FORCE_DBG0_FORCE_TS_DBG_I_OFF           (17)
#define PP_PORT_DIST_STATUS_FORCE_DBG0_FORCE_TS_DBG_I_LEN           (1)
#define PP_PORT_DIST_STATUS_FORCE_DBG0_FORCE_TS_DBG_I_MSK           (0x00020000)
#define PP_PORT_DIST_STATUS_FORCE_DBG0_FORCE_TS_DBG_I_RST           (0x0)
#define PP_PORT_DIST_STATUS_FORCE_DBG0_FORCE_PREL2_SIZE_DBG_I_OFF   (15)
#define PP_PORT_DIST_STATUS_FORCE_DBG0_FORCE_PREL2_SIZE_DBG_I_LEN   (2)
#define PP_PORT_DIST_STATUS_FORCE_DBG0_FORCE_PREL2_SIZE_DBG_I_MSK   (0x00018000)
#define PP_PORT_DIST_STATUS_FORCE_DBG0_FORCE_PREL2_SIZE_DBG_I_RST   (0x0)
#define PP_PORT_DIST_STATUS_FORCE_DBG0_FORCE_SOURCE_POOL_DBG_I_OFF  (11)
#define PP_PORT_DIST_STATUS_FORCE_DBG0_FORCE_SOURCE_POOL_DBG_I_LEN  (4)
#define PP_PORT_DIST_STATUS_FORCE_DBG0_FORCE_SOURCE_POOL_DBG_I_MSK  (0x00007800)
#define PP_PORT_DIST_STATUS_FORCE_DBG0_FORCE_SOURCE_POOL_DBG_I_RST  (0x0)
#define PP_PORT_DIST_STATUS_FORCE_DBG0_FORCE_TC_DBG_I_OFF           (10)
#define PP_PORT_DIST_STATUS_FORCE_DBG0_FORCE_TC_DBG_I_LEN           (1)
#define PP_PORT_DIST_STATUS_FORCE_DBG0_FORCE_TC_DBG_I_MSK           (0x00000400)
#define PP_PORT_DIST_STATUS_FORCE_DBG0_FORCE_TC_DBG_I_RST           (0x0)
#define PP_PORT_DIST_STATUS_FORCE_DBG0_FORCE_PMAC_INDICATION_DBG_I_OFF       (9)
#define PP_PORT_DIST_STATUS_FORCE_DBG0_FORCE_PMAC_INDICATION_DBG_I_LEN       (1)
#define PP_PORT_DIST_STATUS_FORCE_DBG0_FORCE_PMAC_INDICATION_DBG_I_MSK \
	(0x00000200)
#define PP_PORT_DIST_STATUS_FORCE_DBG0_FORCE_PMAC_INDICATION_DBG_I_RST     (0x0)
#define PP_PORT_DIST_STATUS_FORCE_DBG0_FORCE_DATA_OFFSET_DBG_I_OFF  (0)
#define PP_PORT_DIST_STATUS_FORCE_DBG0_FORCE_DATA_OFFSET_DBG_I_LEN  (9)
#define PP_PORT_DIST_STATUS_FORCE_DBG0_FORCE_DATA_OFFSET_DBG_I_MSK  (0x000001FF)
#define PP_PORT_DIST_STATUS_FORCE_DBG0_FORCE_DATA_OFFSET_DBG_I_RST  (0x0)

/**
 * SW_REG_NAME : PP_PORT_DIST_STATUS_FORCE_DBG1_REG
 * HW_REG_NAME : status_force_dbg1_reg
 * DESCRIPTION : [STEP B only] status_force_dbg - force the STW at
 *               the ppv4 input , NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31:29][RO] - Reserved
 *   [28: 0][RW] - status_force_buffer_pointer_dbg
 *
 */
#define PP_PORT_DIST_STATUS_FORCE_DBG1_REG      ((PORT_DIST_BASE_ADDR) + 0x1524)
#define PP_PORT_DIST_STATUS_FORCE_DBG1_RSVD0_OFF                    (29)
#define PP_PORT_DIST_STATUS_FORCE_DBG1_RSVD0_LEN                    (3)
#define PP_PORT_DIST_STATUS_FORCE_DBG1_RSVD0_MSK                    (0xE0000000)
#define PP_PORT_DIST_STATUS_FORCE_DBG1_RSVD0_RST                    (0x0)
#define PP_PORT_DIST_STATUS_FORCE_DBG1_FORCE_BUFFER_POINTER_DBG_I_OFF        (0)
#define PP_PORT_DIST_STATUS_FORCE_DBG1_FORCE_BUFFER_POINTER_DBG_I_LEN       (29)
#define PP_PORT_DIST_STATUS_FORCE_DBG1_FORCE_BUFFER_POINTER_DBG_I_MSK \
	(0x1FFFFFFF)
#define PP_PORT_DIST_STATUS_FORCE_DBG1_FORCE_BUFFER_POINTER_DBG_I_RST      (0x0)

/**
 * SW_REG_NAME : PP_PORT_DIST_STATUS_FORCE_DBG2_REG
 * HW_REG_NAME : status_force_dbg2_reg
 * DESCRIPTION : [STEP B only] status_force_dbg - force the STW at
 *               the ppv4 input , NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31:26][RO] - Reserved
 *   [25:25][RW] - force discard indication dbg
 *   [24:24][RW] - force error_indication dbg
 *   [23:23][RW] - force header mode dbg
 *   [22:22][RW] - force 2nd_round dbg
 *   [21: 8][RW] - force packet length dbg
 *   [ 7: 0][RW] - force pool policy dbg
 *
 */
#define PP_PORT_DIST_STATUS_FORCE_DBG2_REG      ((PORT_DIST_BASE_ADDR) + 0x1528)
#define PP_PORT_DIST_STATUS_FORCE_DBG2_RSVD0_OFF                    (26)
#define PP_PORT_DIST_STATUS_FORCE_DBG2_RSVD0_LEN                    (6)
#define PP_PORT_DIST_STATUS_FORCE_DBG2_RSVD0_MSK                    (0xFC000000)
#define PP_PORT_DIST_STATUS_FORCE_DBG2_RSVD0_RST                    (0x0)
#define PP_PORT_DIST_STATUS_FORCE_DBG2_FORCE_DISCARD_INDICATION_DBG_I_OFF   (25)
#define PP_PORT_DIST_STATUS_FORCE_DBG2_FORCE_DISCARD_INDICATION_DBG_I_LEN    (1)
#define PP_PORT_DIST_STATUS_FORCE_DBG2_FORCE_DISCARD_INDICATION_DBG_I_MSK \
	(0x02000000)
#define PP_PORT_DIST_STATUS_FORCE_DBG2_FORCE_DISCARD_INDICATION_DBG_I_RST  (0x0)
#define PP_PORT_DIST_STATUS_FORCE_DBG2_FORCE_ERROR_INDICATION_DBG_I_OFF     (24)
#define PP_PORT_DIST_STATUS_FORCE_DBG2_FORCE_ERROR_INDICATION_DBG_I_LEN      (1)
#define PP_PORT_DIST_STATUS_FORCE_DBG2_FORCE_ERROR_INDICATION_DBG_I_MSK \
	(0x01000000)
#define PP_PORT_DIST_STATUS_FORCE_DBG2_FORCE_ERROR_INDICATION_DBG_I_RST    (0x0)
#define PP_PORT_DIST_STATUS_FORCE_DBG2_FORCE_HEADER_MODE_DBG_I_OFF  (23)
#define PP_PORT_DIST_STATUS_FORCE_DBG2_FORCE_HEADER_MODE_DBG_I_LEN  (1)
#define PP_PORT_DIST_STATUS_FORCE_DBG2_FORCE_HEADER_MODE_DBG_I_MSK  (0x00800000)
#define PP_PORT_DIST_STATUS_FORCE_DBG2_FORCE_HEADER_MODE_DBG_I_RST  (0x0)
#define PP_PORT_DIST_STATUS_FORCE_DBG2_FORCE_2ND_ROUND_DBG_I_OFF    (22)
#define PP_PORT_DIST_STATUS_FORCE_DBG2_FORCE_2ND_ROUND_DBG_I_LEN    (1)
#define PP_PORT_DIST_STATUS_FORCE_DBG2_FORCE_2ND_ROUND_DBG_I_MSK    (0x00400000)
#define PP_PORT_DIST_STATUS_FORCE_DBG2_FORCE_2ND_ROUND_DBG_I_RST    (0x0)
#define PP_PORT_DIST_STATUS_FORCE_DBG2_FORCE_PKT_LEN_DBG_I_OFF      (8)
#define PP_PORT_DIST_STATUS_FORCE_DBG2_FORCE_PKT_LEN_DBG_I_LEN      (14)
#define PP_PORT_DIST_STATUS_FORCE_DBG2_FORCE_PKT_LEN_DBG_I_MSK      (0x003FFF00)
#define PP_PORT_DIST_STATUS_FORCE_DBG2_FORCE_PKT_LEN_DBG_I_RST      (0x0)
#define PP_PORT_DIST_STATUS_FORCE_DBG2_FORCE_POOL_POLICY_DBG_I_OFF  (0)
#define PP_PORT_DIST_STATUS_FORCE_DBG2_FORCE_POOL_POLICY_DBG_I_LEN  (8)
#define PP_PORT_DIST_STATUS_FORCE_DBG2_FORCE_POOL_POLICY_DBG_I_MSK  (0x000000FF)
#define PP_PORT_DIST_STATUS_FORCE_DBG2_FORCE_POOL_POLICY_DBG_I_RST  (0x0)

/**
 * SW_REG_NAME : PP_PORT_DIST_STATUS_FORCE_DBG3_REG
 * HW_REG_NAME : status_force_dbg3_reg
 * DESCRIPTION : [STEP B only] status_force_dbg - force the STW at
 *               the ppv4 input , NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][RW] - status_force_pc_dbg lsb [31:0]
 *
 */
#define PP_PORT_DIST_STATUS_FORCE_DBG3_REG      ((PORT_DIST_BASE_ADDR) + 0x152C)
#define PP_PORT_DIST_STATUS_FORCE_DBG3_FORCE_PC_DBG_I_OFF           (0)
#define PP_PORT_DIST_STATUS_FORCE_DBG3_FORCE_PC_DBG_I_LEN           (32)
#define PP_PORT_DIST_STATUS_FORCE_DBG3_FORCE_PC_DBG_I_MSK           (0xFFFFFFFF)
#define PP_PORT_DIST_STATUS_FORCE_DBG3_FORCE_PC_DBG_I_RST           (0x0)

/**
 * SW_REG_NAME : PP_PORT_DIST_STATUS_FORCE_DBG4_REG
 * HW_REG_NAME : status_force_dbg4_reg
 * DESCRIPTION : [STEP B only] status_force_dbg - force the STW at
 *               the ppv4 input , NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31:16][RO] - Reserved
 *   [15: 0][RW] - status_force_pc_dbg [47:32]
 *
 */
#define PP_PORT_DIST_STATUS_FORCE_DBG4_REG      ((PORT_DIST_BASE_ADDR) + 0x1530)
#define PP_PORT_DIST_STATUS_FORCE_DBG4_RSVD0_OFF                    (16)
#define PP_PORT_DIST_STATUS_FORCE_DBG4_RSVD0_LEN                    (16)
#define PP_PORT_DIST_STATUS_FORCE_DBG4_RSVD0_MSK                    (0xFFFF0000)
#define PP_PORT_DIST_STATUS_FORCE_DBG4_RSVD0_RST                    (0x0)
#define PP_PORT_DIST_STATUS_FORCE_DBG4_FORCE_PC_DBG_I_OFF           (0)
#define PP_PORT_DIST_STATUS_FORCE_DBG4_FORCE_PC_DBG_I_LEN           (16)
#define PP_PORT_DIST_STATUS_FORCE_DBG4_FORCE_PC_DBG_I_MSK           (0x0000FFFF)
#define PP_PORT_DIST_STATUS_FORCE_DBG4_FORCE_PC_DBG_I_RST           (0x0)

/**
 * SW_REG_NAME : PP_PORT_DIST_STATUS_GLOBAL_FORCE_EN_DBG_REG
 * HW_REG_NAME : status_global_force_enable_dbg_reg
 * DESCRIPTION : [STEP B only] status force enable dbg - force the
 *               STW at the ppv4 input , NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 1][RO] - Reserved
 *   [ 0: 0][RW] - status global force enable dbg
 *
 */
#define PP_PORT_DIST_STATUS_GLOBAL_FORCE_EN_DBG_REG \
	((PORT_DIST_BASE_ADDR) + 0x1534)
#define PP_PORT_DIST_STATUS_GLOBAL_FORCE_EN_DBG_RSVD0_OFF           (1)
#define PP_PORT_DIST_STATUS_GLOBAL_FORCE_EN_DBG_RSVD0_LEN           (31)
#define PP_PORT_DIST_STATUS_GLOBAL_FORCE_EN_DBG_RSVD0_MSK           (0xFFFFFFFE)
#define PP_PORT_DIST_STATUS_GLOBAL_FORCE_EN_DBG_RSVD0_RST           (0x0)
#define PP_PORT_DIST_STATUS_GLOBAL_FORCE_EN_DBG_GLOBAL_FORCE_EN_DBG_I_OFF    (0)
#define PP_PORT_DIST_STATUS_GLOBAL_FORCE_EN_DBG_GLOBAL_FORCE_EN_DBG_I_LEN    (1)
#define PP_PORT_DIST_STATUS_GLOBAL_FORCE_EN_DBG_GLOBAL_FORCE_EN_DBG_I_MSK \
	(0x00000001)
#define PP_PORT_DIST_STATUS_GLOBAL_FORCE_EN_DBG_GLOBAL_FORCE_EN_DBG_I_RST  (0x0)

/**
 * SW_REG_NAME : PP_PORT_DIST_STATUS_FORCE_EN_DBG_REG
 * HW_REG_NAME : status_force_enable_dbg_reg
 * DESCRIPTION : [STEP B only] status force enable dbg - force the
 *               STW at the ppv4 input  , NOTE, B Step ONLY
 *               register
 *
 *  Register Fields :
 *   [31:15][RO] - Reserved
 *   [14:14][RW] - force enable discard indication dbg
 *   [13:13][RW] - force enable error_indication dbg
 *   [12:12][RW] - force enable header mode dbg
 *   [11:11][RW] - force enable 2nd_round dbg
 *   [10:10][RW] - force enable packet length dbg
 *   [ 9: 9][RW] - force enable pool policy dbg
 *   [ 8: 8][RW] - force enable port dbg
 *   [ 7: 7][RW] - force enable timestamp_indication dbg
 *   [ 6: 6][RW] - force enable prel2_size dbg
 *   [ 5: 5][RW] - status_force_buffer_pointer_dbg
 *   [ 4: 4][RW] - force enable source pool dbg
 *   [ 3: 3][RW] - force enable status tc dbg
 *   [ 2: 2][RW] - force enable status pmac_indication dbg
 *   [ 1: 1][RW] - force enable status data_offset
 *   [ 0: 0][RW] - status force enable pc_dbg lsb]
 *
 */
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_REG    ((PORT_DIST_BASE_ADDR) + 0x1538)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_RSVD0_OFF                  (15)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_RSVD0_LEN                  (17)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_RSVD0_MSK                  (0xFFFF8000)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_RSVD0_RST                  (0x0)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_FORCE_EN_DISCARD_INDICATION_DBG_I_OFF \
	(14)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_FORCE_EN_DISCARD_INDICATION_DBG_I_LEN \
	(1)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_FORCE_EN_DISCARD_INDICATION_DBG_I_MSK \
	(0x00004000)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_FORCE_EN_DISCARD_INDICATION_DBG_I_RST \
	(0x0)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_FORCE_EN_ERROR_INDICATION_DBG_I_OFF \
	(13)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_FORCE_EN_ERROR_INDICATION_DBG_I_LEN (1)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_FORCE_EN_ERROR_INDICATION_DBG_I_MSK \
	(0x00002000)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_FORCE_EN_ERROR_INDICATION_DBG_I_RST \
	(0x0)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_FORCE_EN_HEADER_MODE_DBG_I_OFF     (12)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_FORCE_EN_HEADER_MODE_DBG_I_LEN      (1)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_FORCE_EN_HEADER_MODE_DBG_I_MSK \
	(0x00001000)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_FORCE_EN_HEADER_MODE_DBG_I_RST    (0x0)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_FORCE_EN_2ND_ROUND_DBG_I_OFF       (11)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_FORCE_EN_2ND_ROUND_DBG_I_LEN        (1)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_FORCE_EN_2ND_ROUND_DBG_I_MSK \
	(0x00000800)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_FORCE_EN_2ND_ROUND_DBG_I_RST      (0x0)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_FORCE_EN_PKT_LEN_DBG_I_OFF (10)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_FORCE_EN_PKT_LEN_DBG_I_LEN (1)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_FORCE_EN_PKT_LEN_DBG_I_MSK (0x00000400)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_FORCE_EN_PKT_LEN_DBG_I_RST (0x0)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_FORCE_EN_POOL_POLICY_DBG_I_OFF      (9)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_FORCE_EN_POOL_POLICY_DBG_I_LEN      (1)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_FORCE_EN_POOL_POLICY_DBG_I_MSK \
	(0x00000200)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_FORCE_EN_POOL_POLICY_DBG_I_RST    (0x0)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_FORCE_EN_PORT_DBG_I_OFF    (8)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_FORCE_EN_PORT_DBG_I_LEN    (1)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_FORCE_EN_PORT_DBG_I_MSK    (0x00000100)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_FORCE_EN_PORT_DBG_I_RST    (0x0)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_FORCE_EN_TS_DBG_I_OFF      (7)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_FORCE_EN_TS_DBG_I_LEN      (1)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_FORCE_EN_TS_DBG_I_MSK      (0x00000080)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_FORCE_EN_TS_DBG_I_RST      (0x0)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_FORCE_EN_PREL2_SIZE_DBG_I_OFF       (6)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_FORCE_EN_PREL2_SIZE_DBG_I_LEN       (1)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_FORCE_EN_PREL2_SIZE_DBG_I_MSK \
	(0x00000040)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_FORCE_EN_PREL2_SIZE_DBG_I_RST     (0x0)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_FORCE_EN_BUFFER_POINTER_DBG_I_OFF   (5)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_FORCE_EN_BUFFER_POINTER_DBG_I_LEN   (1)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_FORCE_EN_BUFFER_POINTER_DBG_I_MSK \
	(0x00000020)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_FORCE_EN_BUFFER_POINTER_DBG_I_RST (0x0)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_FORCE_EN_SOURCE_POOL_DBG_I_OFF      (4)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_FORCE_EN_SOURCE_POOL_DBG_I_LEN      (1)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_FORCE_EN_SOURCE_POOL_DBG_I_MSK \
	(0x00000010)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_FORCE_EN_SOURCE_POOL_DBG_I_RST    (0x0)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_FORCE_EN_TC_DBG_I_OFF      (3)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_FORCE_EN_TC_DBG_I_LEN      (1)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_FORCE_EN_TC_DBG_I_MSK      (0x00000008)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_FORCE_EN_TC_DBG_I_RST      (0x0)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_FORCE_EN_PMAC_INDICATION_DBG_I_OFF  (2)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_FORCE_EN_PMAC_INDICATION_DBG_I_LEN  (1)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_FORCE_EN_PMAC_INDICATION_DBG_I_MSK \
	(0x00000004)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_FORCE_EN_PMAC_INDICATION_DBG_I_RST \
	(0x0)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_FORCE_EN_DATA_OFFSET_DBG_I_OFF      (1)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_FORCE_EN_DATA_OFFSET_DBG_I_LEN      (1)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_FORCE_EN_DATA_OFFSET_DBG_I_MSK \
	(0x00000002)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_FORCE_EN_DATA_OFFSET_DBG_I_RST    (0x0)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_FORCE_EN_PC_DBG_I_OFF      (0)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_FORCE_EN_PC_DBG_I_LEN      (1)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_FORCE_EN_PC_DBG_I_MSK      (0x00000001)
#define PP_PORT_DIST_STATUS_FORCE_EN_DBG_FORCE_EN_PC_DBG_I_RST      (0x0)

#endif
