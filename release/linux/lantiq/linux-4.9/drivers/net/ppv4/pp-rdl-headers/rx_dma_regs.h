/**
 * rx_dma_regs.h
 * Description: rx_dma_s HW registers definitions
 * Auto-generated file by RDL Parser, DO NOT CHANGE
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2019-2020 Intel Corporation
 */

#ifndef _PP_RX_DMA_H_
#define _PP_RX_DMA_H_

#define PP_RX_DMA_GEN_DATE_STR                "Sunday May 17, 2020 [5:26:11 pm]"
#define PP_RX_DMA_BASE                                           (0xF0006000ULL)

/**
 * SW_REG_NAME : PP_RX_DMA_RX_VER_REG
 * HW_REG_NAME : rx_dma_ver_reg
 * DESCRIPTION : dma_ver
 *
 *  Register Fields :
 *   [31: 0][RO] - dma_ver
 *
 */
#define PP_RX_DMA_RX_VER_REG                          ((RX_DMA_BASE_ADDR) + 0x0)
#define PP_RX_DMA_RX_VER_DMA_VER_I_OFF                              (0)
#define PP_RX_DMA_RX_VER_DMA_VER_I_LEN                              (32)
#define PP_RX_DMA_RX_VER_DMA_VER_I_MSK                              (0xFFFFFFFF)
#define PP_RX_DMA_RX_VER_DMA_VER_I_RST                              (0x1)

/**
 * SW_REG_NAME : PP_RX_DMA_POOL_SIZE_REG
 * HW_REG_NAME : pool_size_reg
 * DESCRIPTION : pool_size
 *
 *  Register Fields :
 *   [31: 0][RW] - pool_size ,5 register which 0 is the smallest pool
 *                 size and the 4 is the biggest size ( please see
 *                 section 5.7.1        BMGR logic in rx_dma MAS)
 *
 */
#define PP_RX_DMA_POOL_SIZE_REG                       ((RX_DMA_BASE_ADDR) + 0x4)
#define PP_RX_DMA_POOL_SIZE_POOL_SIZE_I_OFF                         (0)
#define PP_RX_DMA_POOL_SIZE_POOL_SIZE_I_LEN                         (32)
#define PP_RX_DMA_POOL_SIZE_POOL_SIZE_I_MSK                         (0xFFFFFFFF)
#define PP_RX_DMA_POOL_SIZE_POOL_SIZE_I_RST                         (0x0)
/**
 * REG_IDX_ACCESS   : PP_RX_DMA_POOL_SIZE_REG_IDX
 * NUM OF REGISTERS : 5
 */
#define PP_RX_DMA_POOL_SIZE_REG_IDX(idx) \
	(PP_RX_DMA_POOL_SIZE_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_RX_DMA_SEGMENTATION_SIZE_REG
 * HW_REG_NAME : segmentation_size_reg
 * DESCRIPTION : segmentation_size
 *
 *  Register Fields :
 *   [31:11][RO] - Reserved
 *   [10: 0][RW] - on-chip memory segmented size through FSQM (in
 *                 128B granularity)
 *
 */
#define PP_RX_DMA_SEGMENTATION_SIZE_REG              ((RX_DMA_BASE_ADDR) + 0x20)
#define PP_RX_DMA_SEGMENTATION_SIZE_RSVD0_OFF                       (11)
#define PP_RX_DMA_SEGMENTATION_SIZE_RSVD0_LEN                       (21)
#define PP_RX_DMA_SEGMENTATION_SIZE_RSVD0_MSK                       (0xFFFFF800)
#define PP_RX_DMA_SEGMENTATION_SIZE_RSVD0_RST                       (0x0)
#define PP_RX_DMA_SEGMENTATION_SIZE_SEGMENTATION_SIZE_I_OFF         (0)
#define PP_RX_DMA_SEGMENTATION_SIZE_SEGMENTATION_SIZE_I_LEN         (11)
#define PP_RX_DMA_SEGMENTATION_SIZE_SEGMENTATION_SIZE_I_MSK         (0x000007FF)
#define PP_RX_DMA_SEGMENTATION_SIZE_SEGMENTATION_SIZE_I_RST         (0x0)

/**
 * SW_REG_NAME : PP_RX_DMA_FSQM_CFG_REG
 * HW_REG_NAME : fsqm_cfg_reg
 * DESCRIPTION : fsqm config ( see 5.8.1 Segmentation logic in
 *               rx_dma MAS) need to be align with CQM
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19:16][RW] - burst size for fsqm descriptor writes
 *   [15:12][RW] - number of buffers that rx_dma keep in his internal
 *                 FIFOs; min th is 20 buffers
 *   [11: 8][RO] - Reserved
 *   [ 7: 4][RW] - force fsqm buffer requests burst size
 *   [ 3: 1][RO] - Reserved
 *   [ 0: 0][RW] - fsqm ready to recive buffer requests ; if set the
 *                 rx_dma start to request buffer from fsqm
 *
 */
#define PP_RX_DMA_FSQM_CFG_REG                       ((RX_DMA_BASE_ADDR) + 0x24)
#define PP_RX_DMA_FSQM_CFG_RSVD0_OFF                                (20)
#define PP_RX_DMA_FSQM_CFG_RSVD0_LEN                                (12)
#define PP_RX_DMA_FSQM_CFG_RSVD0_MSK                                (0xFFF00000)
#define PP_RX_DMA_FSQM_CFG_RSVD0_RST                                (0x0)
#define PP_RX_DMA_FSQM_CFG_FSQM_DESC_BURST_I_OFF                    (16)
#define PP_RX_DMA_FSQM_CFG_FSQM_DESC_BURST_I_LEN                    (4)
#define PP_RX_DMA_FSQM_CFG_FSQM_DESC_BURST_I_MSK                    (0x000F0000)
#define PP_RX_DMA_FSQM_CFG_FSQM_DESC_BURST_I_RST                    (0x8)
#define PP_RX_DMA_FSQM_CFG_INTERNAL_BUFFER_TH_REQ_I_OFF             (12)
#define PP_RX_DMA_FSQM_CFG_INTERNAL_BUFFER_TH_REQ_I_LEN             (4)
#define PP_RX_DMA_FSQM_CFG_INTERNAL_BUFFER_TH_REQ_I_MSK             (0x0000F000)
#define PP_RX_DMA_FSQM_CFG_INTERNAL_BUFFER_TH_REQ_I_RST             (0x8)
#define PP_RX_DMA_FSQM_CFG_RSVD1_OFF                                (8)
#define PP_RX_DMA_FSQM_CFG_RSVD1_LEN                                (4)
#define PP_RX_DMA_FSQM_CFG_RSVD1_MSK                                (0x00000F00)
#define PP_RX_DMA_FSQM_CFG_RSVD1_RST                                (0x0)
#define PP_RX_DMA_FSQM_CFG_FSQM_BURST_REQ_I_OFF                     (4)
#define PP_RX_DMA_FSQM_CFG_FSQM_BURST_REQ_I_LEN                     (4)
#define PP_RX_DMA_FSQM_CFG_FSQM_BURST_REQ_I_MSK                     (0x000000F0)
#define PP_RX_DMA_FSQM_CFG_FSQM_BURST_REQ_I_RST                     (0x8)
#define PP_RX_DMA_FSQM_CFG_RSVD2_OFF                                (1)
#define PP_RX_DMA_FSQM_CFG_RSVD2_LEN                                (3)
#define PP_RX_DMA_FSQM_CFG_RSVD2_MSK                                (0x0000000E)
#define PP_RX_DMA_FSQM_CFG_RSVD2_RST                                (0x0)
#define PP_RX_DMA_FSQM_CFG_FSQM_READY_I_OFF                         (0)
#define PP_RX_DMA_FSQM_CFG_FSQM_READY_I_LEN                         (1)
#define PP_RX_DMA_FSQM_CFG_FSQM_READY_I_MSK                         (0x00000001)
#define PP_RX_DMA_FSQM_CFG_FSQM_READY_I_RST                         (0x0)

/**
 * SW_REG_NAME : PP_RX_DMA_OWNERSHIP_BIT_REG
 * HW_REG_NAME : ownership_bit_reg
 * DESCRIPTION : ownership_bit
 *
 *  Register Fields :
 *   [31: 1][RO] - Reserved
 *   [ 0: 0][RW] - RXDMA update this bit in the descriptor for every
 *                 packet going through it.The polarity is according
 *                 to this register value
 *
 */
#define PP_RX_DMA_OWNERSHIP_BIT_REG                  ((RX_DMA_BASE_ADDR) + 0x28)
#define PP_RX_DMA_OWNERSHIP_BIT_RSVD0_OFF                           (1)
#define PP_RX_DMA_OWNERSHIP_BIT_RSVD0_LEN                           (31)
#define PP_RX_DMA_OWNERSHIP_BIT_RSVD0_MSK                           (0xFFFFFFFE)
#define PP_RX_DMA_OWNERSHIP_BIT_RSVD0_RST                           (0x0)
#define PP_RX_DMA_OWNERSHIP_BIT_OWNERSHIP_BIT_I_OFF                 (0)
#define PP_RX_DMA_OWNERSHIP_BIT_OWNERSHIP_BIT_I_LEN                 (1)
#define PP_RX_DMA_OWNERSHIP_BIT_OWNERSHIP_BIT_I_MSK                 (0x00000001)
#define PP_RX_DMA_OWNERSHIP_BIT_OWNERSHIP_BIT_I_RST                 (0x0)

/**
 * SW_REG_NAME : PP_RX_DMA_PORT_PKT_LENGTH_MIN_CFG_SIZE_REG
 * HW_REG_NAME : port_pkt_length_min_cfg_size_reg
 * DESCRIPTION : port_pkt_length_min_cfg_size
 *
 *  Register Fields :
 *   [31:16][RO] - Reserved
 *   [15: 0][RW] - In case of packet that is smaller than destination
 *                 port minimum size , the RX DMA should pad the
 *                 Packet to minimum size by updating Packet length
 *
 */
#define PP_RX_DMA_PORT_PKT_LENGTH_MIN_CFG_SIZE_REG   ((RX_DMA_BASE_ADDR) + 0x2C)
#define PP_RX_DMA_PORT_PKT_LENGTH_MIN_CFG_SIZE_RSVD0_OFF            (16)
#define PP_RX_DMA_PORT_PKT_LENGTH_MIN_CFG_SIZE_RSVD0_LEN            (16)
#define PP_RX_DMA_PORT_PKT_LENGTH_MIN_CFG_SIZE_RSVD0_MSK            (0xFFFF0000)
#define PP_RX_DMA_PORT_PKT_LENGTH_MIN_CFG_SIZE_RSVD0_RST            (0x0)
#define PP_RX_DMA_PORT_PKT_LENGTH_MIN_CFG_SIZE_POOL_SIZE_I_OFF      (0)
#define PP_RX_DMA_PORT_PKT_LENGTH_MIN_CFG_SIZE_POOL_SIZE_I_LEN      (16)
#define PP_RX_DMA_PORT_PKT_LENGTH_MIN_CFG_SIZE_POOL_SIZE_I_MSK      (0x0000FFFF)
#define PP_RX_DMA_PORT_PKT_LENGTH_MIN_CFG_SIZE_POOL_SIZE_I_RST      (0x0)
/**
 * REG_IDX_ACCESS   : PP_RX_DMA_PORT_PKT_LENGTH_MIN_CFG_SIZE_REG_IDX
 * NUM OF REGISTERS : 4
 */
#define PP_RX_DMA_PORT_PKT_LENGTH_MIN_CFG_SIZE_REG_IDX(idx) \
	(PP_RX_DMA_PORT_PKT_LENGTH_MIN_CFG_SIZE_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_RX_DMA_EGRESS_PORT_CFG_REG
 * HW_REG_NAME : egress_port_cfg_reg
 * DESCRIPTION : egress_port_cfg
 *
 *  Register Fields :
 *   [31:31][RW] - if set the RXDMA will report data offset and
 *                 packet len to include the packet only, even if
 *                 PreL2 and/or timestamp exists
 *   [30:30][RW] - if set, the DMA will put error indication instead
 *                 of PMAC bit in the descriptor
 *   [29:28][RW] - if the packet is shorter (packet only without
 *                 PreL2andtimestamp) then this size, then the packet
 *                 is to be extended to the minimum size (just
 *                 length, no padding)
 *   [27:27][RW] - if set, the RX DMA will write the descriptor to
 *                 the buffer (not just the QoS)
 *   [26:26][RO] - Reserved
 *   [25:16][RW] - when selecting a buffer size RXDMA will make sure
 *                 there are at least Tailroom bytes from the end of
 *                 the packet (incl. timestamp) to the end of the
 *                 buffer
 *   [15:10][RO] - Reserved
 *   [ 9: 0][RW] - this determines the starting offset for the packet
 *                 write in a new buffer
 *
 */
#define PP_RX_DMA_EGRESS_PORT_CFG_REG                ((RX_DMA_BASE_ADDR) + 0x40)
#define PP_RX_DMA_EGRESS_PORT_CFG_PKT_ONLY_OFF                      (31)
#define PP_RX_DMA_EGRESS_PORT_CFG_PKT_ONLY_LEN                      (1)
#define PP_RX_DMA_EGRESS_PORT_CFG_PKT_ONLY_MSK                      (0x80000000)
#define PP_RX_DMA_EGRESS_PORT_CFG_PKT_ONLY_RST                      (0x0)
#define PP_RX_DMA_EGRESS_PORT_CFG_ERROR_BIT_OFF                     (30)
#define PP_RX_DMA_EGRESS_PORT_CFG_ERROR_BIT_LEN                     (1)
#define PP_RX_DMA_EGRESS_PORT_CFG_ERROR_BIT_MSK                     (0x40000000)
#define PP_RX_DMA_EGRESS_PORT_CFG_ERROR_BIT_RST                     (0x0)
#define PP_RX_DMA_EGRESS_PORT_CFG_MIN_PKT_SIZE_OFF                  (28)
#define PP_RX_DMA_EGRESS_PORT_CFG_MIN_PKT_SIZE_LEN                  (2)
#define PP_RX_DMA_EGRESS_PORT_CFG_MIN_PKT_SIZE_MSK                  (0x30000000)
#define PP_RX_DMA_EGRESS_PORT_CFG_MIN_PKT_SIZE_RST                  (0x0)
#define PP_RX_DMA_EGRESS_PORT_CFG_DESC_TO_BUFFER_OFF                (27)
#define PP_RX_DMA_EGRESS_PORT_CFG_DESC_TO_BUFFER_LEN                (1)
#define PP_RX_DMA_EGRESS_PORT_CFG_DESC_TO_BUFFER_MSK                (0x08000000)
#define PP_RX_DMA_EGRESS_PORT_CFG_DESC_TO_BUFFER_RST                (0x0)
#define PP_RX_DMA_EGRESS_PORT_CFG_RSVD0_OFF                         (26)
#define PP_RX_DMA_EGRESS_PORT_CFG_RSVD0_LEN                         (1)
#define PP_RX_DMA_EGRESS_PORT_CFG_RSVD0_MSK                         (0x04000000)
#define PP_RX_DMA_EGRESS_PORT_CFG_RSVD0_RST                         (0x0)
#define PP_RX_DMA_EGRESS_PORT_CFG_TAILROOM_OFF                      (16)
#define PP_RX_DMA_EGRESS_PORT_CFG_TAILROOM_LEN                      (10)
#define PP_RX_DMA_EGRESS_PORT_CFG_TAILROOM_MSK                      (0x03FF0000)
#define PP_RX_DMA_EGRESS_PORT_CFG_TAILROOM_RST                      (0x0)
#define PP_RX_DMA_EGRESS_PORT_CFG_RSVD1_OFF                         (10)
#define PP_RX_DMA_EGRESS_PORT_CFG_RSVD1_LEN                         (6)
#define PP_RX_DMA_EGRESS_PORT_CFG_RSVD1_MSK                         (0x0000FC00)
#define PP_RX_DMA_EGRESS_PORT_CFG_RSVD1_RST                         (0x0)
#define PP_RX_DMA_EGRESS_PORT_CFG_HEADROOM_OFF                      (0)
#define PP_RX_DMA_EGRESS_PORT_CFG_HEADROOM_LEN                      (10)
#define PP_RX_DMA_EGRESS_PORT_CFG_HEADROOM_MSK                      (0x000003FF)
#define PP_RX_DMA_EGRESS_PORT_CFG_HEADROOM_RST                      (0x0)
/**
 * REG_IDX_ACCESS   : PP_RX_DMA_EGRESS_PORT_CFG_REG_IDX
 * NUM OF REGISTERS : 256
 */
#define PP_RX_DMA_EGRESS_PORT_CFG_REG_IDX(idx) \
	(PP_RX_DMA_EGRESS_PORT_CFG_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_RX_DMA_HW_PARSER_DROP_CNT_REG
 * HW_REG_NAME : hw_parser_drop_counter_reg
 * DESCRIPTION : hw_parser_drop_counter
 *
 *  Register Fields :
 *   [31: 0][RW] - Counts number of packets drop by STW indication
 *                 dropped in HW parser to uC (32 bit- wrap around)
 *
 */
#define PP_RX_DMA_HW_PARSER_DROP_CNT_REG            ((RX_DMA_BASE_ADDR) + 0x500)
#define PP_RX_DMA_HW_PARSER_DROP_CNT_HW_PARSER_DROP_CNT_I_OFF       (0)
#define PP_RX_DMA_HW_PARSER_DROP_CNT_HW_PARSER_DROP_CNT_I_LEN       (32)
#define PP_RX_DMA_HW_PARSER_DROP_CNT_HW_PARSER_DROP_CNT_I_MSK       (0xFFFFFFFF)
#define PP_RX_DMA_HW_PARSER_DROP_CNT_HW_PARSER_DROP_CNT_I_RST       (0x0)

/**
 * SW_REG_NAME : PP_RX_DMA_PARSER_UC_DROP_CNT_REG
 * HW_REG_NAME : parser_uc_drop_counter_reg
 * DESCRIPTION : parser_uc_drop_counter
 *
 *  Register Fields :
 *   [31: 0][RW] - Counts number of packets drop by STW indication
 *                 dropped in parser uC (32 bit- wrap around)
 *
 */
#define PP_RX_DMA_PARSER_UC_DROP_CNT_REG            ((RX_DMA_BASE_ADDR) + 0x504)
#define PP_RX_DMA_PARSER_UC_DROP_CNT_PARSER_UC_DROP_CNT_I_OFF       (0)
#define PP_RX_DMA_PARSER_UC_DROP_CNT_PARSER_UC_DROP_CNT_I_LEN       (32)
#define PP_RX_DMA_PARSER_UC_DROP_CNT_PARSER_UC_DROP_CNT_I_MSK       (0xFFFFFFFF)
#define PP_RX_DMA_PARSER_UC_DROP_CNT_PARSER_UC_DROP_CNT_I_RST       (0x0)

/**
 * SW_REG_NAME : PP_RX_DMA_HW_CLS_DROP_CNT_REG
 * HW_REG_NAME : hw_classifier_drop_counter_reg
 * DESCRIPTION : hw_classifier_drop_counter
 *
 *  Register Fields :
 *   [31: 0][RW] - Counts number of packets drop by STW indication
 *                 dropped in classifier to uC (32 bit- wrap around)
 *
 */
#define PP_RX_DMA_HW_CLS_DROP_CNT_REG               ((RX_DMA_BASE_ADDR) + 0x508)
#define PP_RX_DMA_HW_CLS_DROP_CNT_HW_CLS_DROP_CNT_I_OFF             (0)
#define PP_RX_DMA_HW_CLS_DROP_CNT_HW_CLS_DROP_CNT_I_LEN             (32)
#define PP_RX_DMA_HW_CLS_DROP_CNT_HW_CLS_DROP_CNT_I_MSK             (0xFFFFFFFF)
#define PP_RX_DMA_HW_CLS_DROP_CNT_HW_CLS_DROP_CNT_I_RST             (0x0)

/**
 * SW_REG_NAME : PP_RX_DMA_CLS_UC_DROP_CNT_REG
 * HW_REG_NAME : classifier_uC_drop_counter_reg
 * DESCRIPTION : classifier_uC_drop_counter
 *
 *  Register Fields :
 *   [31: 0][RW] - Counts number of packets drop by STW indication
 *                 dropped in classifier uC (32 bit- wrap around)
 *
 */
#define PP_RX_DMA_CLS_UC_DROP_CNT_REG               ((RX_DMA_BASE_ADDR) + 0x50C)
#define PP_RX_DMA_CLS_UC_DROP_CNT_CLS_UC_DROP_CNT_I_OFF             (0)
#define PP_RX_DMA_CLS_UC_DROP_CNT_CLS_UC_DROP_CNT_I_LEN             (32)
#define PP_RX_DMA_CLS_UC_DROP_CNT_CLS_UC_DROP_CNT_I_MSK             (0xFFFFFFFF)
#define PP_RX_DMA_CLS_UC_DROP_CNT_CLS_UC_DROP_CNT_I_RST             (0x0)

/**
 * SW_REG_NAME : PP_RX_DMA_PPRS_DROP_CNT_REG
 * HW_REG_NAME : pprs_drop_counter_reg
 * DESCRIPTION : pprs_drop_counter
 *
 *  Register Fields :
 *   [31: 0][RW] - Counts number of packets drop by STW indication
 *                 dropped in in PPRS (32 bit- wrap around)
 *
 */
#define PP_RX_DMA_PPRS_DROP_CNT_REG                 ((RX_DMA_BASE_ADDR) + 0x510)
#define PP_RX_DMA_PPRS_DROP_CNT_PPRS_DROP_CNT_I_OFF                 (0)
#define PP_RX_DMA_PPRS_DROP_CNT_PPRS_DROP_CNT_I_LEN                 (32)
#define PP_RX_DMA_PPRS_DROP_CNT_PPRS_DROP_CNT_I_MSK                 (0xFFFFFFFF)
#define PP_RX_DMA_PPRS_DROP_CNT_PPRS_DROP_CNT_I_RST                 (0x0)

/**
 * SW_REG_NAME : PP_RX_DMA_CHECKER_DROP_CNT_REG
 * HW_REG_NAME : checker_drop_counter_reg
 * DESCRIPTION : checker_drop_counter
 *
 *  Register Fields :
 *   [31: 0][RW] - Counts number of packets drop by STW indication
 *                 dropped in in checker  (32 bit- wrap around)
 *
 */
#define PP_RX_DMA_CHECKER_DROP_CNT_REG              ((RX_DMA_BASE_ADDR) + 0x514)
#define PP_RX_DMA_CHECKER_DROP_CNT_CHECKER_DROP_CNT_I_OFF           (0)
#define PP_RX_DMA_CHECKER_DROP_CNT_CHECKER_DROP_CNT_I_LEN           (32)
#define PP_RX_DMA_CHECKER_DROP_CNT_CHECKER_DROP_CNT_I_MSK           (0xFFFFFFFF)
#define PP_RX_DMA_CHECKER_DROP_CNT_CHECKER_DROP_CNT_I_RST           (0x0)

/**
 * SW_REG_NAME : PP_RX_DMA_PARSER_DROP_CNT_REG
 * HW_REG_NAME : parser_drop_counter_reg
 * DESCRIPTION : parser_drop_counter
 *
 *  Register Fields :
 *   [31: 0][RW] - Counts number of packets drop by STW indication
 *                 dropped in parser(32 bit- wrap around)
 *
 */
#define PP_RX_DMA_PARSER_DROP_CNT_REG               ((RX_DMA_BASE_ADDR) + 0x518)
#define PP_RX_DMA_PARSER_DROP_CNT_PARSER_DROP_CNT_I_OFF             (0)
#define PP_RX_DMA_PARSER_DROP_CNT_PARSER_DROP_CNT_I_LEN             (32)
#define PP_RX_DMA_PARSER_DROP_CNT_PARSER_DROP_CNT_I_MSK             (0xFFFFFFFF)
#define PP_RX_DMA_PARSER_DROP_CNT_PARSER_DROP_CNT_I_RST             (0x0)

/**
 * SW_REG_NAME : PP_RX_DMA_WRED_PKT_DROP_CNT_REG
 * HW_REG_NAME : wred_packet_drop_counter_reg
 * DESCRIPTION : wred_packet_drop_counter
 *
 *  Register Fields :
 *   [31: 0][RW] - Counts number of packets drop by WRED (32 bit-
 *                 wrap around)
 *
 */
#define PP_RX_DMA_WRED_PKT_DROP_CNT_REG             ((RX_DMA_BASE_ADDR) + 0x51C)
#define PP_RX_DMA_WRED_PKT_DROP_CNT_WRED_PKT_DROP_CNT_I_OFF         (0)
#define PP_RX_DMA_WRED_PKT_DROP_CNT_WRED_PKT_DROP_CNT_I_LEN         (32)
#define PP_RX_DMA_WRED_PKT_DROP_CNT_WRED_PKT_DROP_CNT_I_MSK         (0xFFFFFFFF)
#define PP_RX_DMA_WRED_PKT_DROP_CNT_WRED_PKT_DROP_CNT_I_RST         (0x0)

/**
 * SW_REG_NAME : PP_RX_DMA_BMGR_PKT_DROP_CNT_REG
 * HW_REG_NAME : bmgr_packet_drop_counter_reg
 * DESCRIPTION : bmgr_packet_drop_counter
 *
 *  Register Fields :
 *   [31: 0][RW] - Counts number of packets drop by BMGR (32 bit-
 *                 wrap around)
 *
 */
#define PP_RX_DMA_BMGR_PKT_DROP_CNT_REG             ((RX_DMA_BASE_ADDR) + 0x520)
#define PP_RX_DMA_BMGR_PKT_DROP_CNT_FIELD_OFF                       (0)
#define PP_RX_DMA_BMGR_PKT_DROP_CNT_FIELD_LEN                       (32)
#define PP_RX_DMA_BMGR_PKT_DROP_CNT_FIELD_MSK                       (0xFFFFFFFF)
#define PP_RX_DMA_BMGR_PKT_DROP_CNT_FIELD_RST                       (0x0)

/**
 * SW_REG_NAME : PP_RX_DMA_WRED_PKT_CANDIDATE_CNT_REG
 * HW_REG_NAME : wred_packet_candidate_counter_reg
 * DESCRIPTION : wred_packet_candidate_counter
 *
 *  Register Fields :
 *   [31: 0][RW] - Counts number of packets candidate by WRED (32
 *                 bit- wrap around)
 *
 */
#define PP_RX_DMA_WRED_PKT_CANDIDATE_CNT_REG        ((RX_DMA_BASE_ADDR) + 0x524)
#define PP_RX_DMA_WRED_PKT_CANDIDATE_CNT_WRED_PKT_DROP_CNT_I_OFF    (0)
#define PP_RX_DMA_WRED_PKT_CANDIDATE_CNT_WRED_PKT_DROP_CNT_I_LEN    (32)
#define PP_RX_DMA_WRED_PKT_CANDIDATE_CNT_WRED_PKT_DROP_CNT_I_MSK    (0xFFFFFFFF)
#define PP_RX_DMA_WRED_PKT_CANDIDATE_CNT_WRED_PKT_DROP_CNT_I_RST    (0x0)

/**
 * SW_REG_NAME : PP_RX_DMA_DROP_CNT_NO_POLICY_CNT_REG
 * HW_REG_NAME : drop_counter_no_policy_counter_reg
 * DESCRIPTION : BM_No_Policy_Match_Discard
 *
 *  Register Fields :
 *   [31: 0][RW] - BM_No_Policy_Match_Discard
 *
 */
#define PP_RX_DMA_DROP_CNT_NO_POLICY_CNT_REG        ((RX_DMA_BASE_ADDR) + 0x528)
#define PP_RX_DMA_DROP_CNT_NO_POLICY_CNT_FIELD_OFF                  (0)
#define PP_RX_DMA_DROP_CNT_NO_POLICY_CNT_FIELD_LEN                  (32)
#define PP_RX_DMA_DROP_CNT_NO_POLICY_CNT_FIELD_MSK                  (0xFFFFFFFF)
#define PP_RX_DMA_DROP_CNT_NO_POLICY_CNT_FIELD_RST                  (0x0)

/**
 * SW_REG_NAME : PP_RX_DMA_BYTE_CNT_H_REG
 * HW_REG_NAME : byte_counter_reg_h
 * DESCRIPTION : byte_counter
 *
 *  Register Fields :
 *   [31: 0][RW] - Number of bytes passed(64bit- wrap around)
 *
 */
#define PP_RX_DMA_BYTE_CNT_H_REG                    ((RX_DMA_BASE_ADDR) + 0x52C)
#define PP_RX_DMA_BYTE_CNT_H_BYTE_CNT_I_OFF                         (0)
#define PP_RX_DMA_BYTE_CNT_H_BYTE_CNT_I_LEN                         (32)
#define PP_RX_DMA_BYTE_CNT_H_BYTE_CNT_I_MSK                         (0xFFFFFFFF)
#define PP_RX_DMA_BYTE_CNT_H_BYTE_CNT_I_RST                         (0x0)

/**
 * SW_REG_NAME : PP_RX_DMA_BYTE_CNT_L_REG
 * HW_REG_NAME : byte_counter_reg_l
 * DESCRIPTION : byte_counter
 *
 *  Register Fields :
 *   [31: 0][RW] - Number of bytes passed(64bit- wrap around)
 *
 */
#define PP_RX_DMA_BYTE_CNT_L_REG                    ((RX_DMA_BASE_ADDR) + 0x530)
#define PP_RX_DMA_BYTE_CNT_L_BYTE_CNT_I_OFF                         (0)
#define PP_RX_DMA_BYTE_CNT_L_BYTE_CNT_I_LEN                         (32)
#define PP_RX_DMA_BYTE_CNT_L_BYTE_CNT_I_MSK                         (0xFFFFFFFF)
#define PP_RX_DMA_BYTE_CNT_L_BYTE_CNT_I_RST                         (0x0)

/**
 * SW_REG_NAME : PP_RX_DMA_WRED_ADDR_RESP_REG
 * HW_REG_NAME : wred_addr_resp_reg
 * DESCRIPTION : wred_addr_resp
 *
 *  Register Fields :
 *   [31: 0][RW] - wred_addr_resp
 *
 */
#define PP_RX_DMA_WRED_ADDR_RESP_REG                ((RX_DMA_BASE_ADDR) + 0x534)
#define PP_RX_DMA_WRED_ADDR_RESP_WRED_ADDR_RESP_I_OFF               (0)
#define PP_RX_DMA_WRED_ADDR_RESP_WRED_ADDR_RESP_I_LEN               (32)
#define PP_RX_DMA_WRED_ADDR_RESP_WRED_ADDR_RESP_I_MSK               (0xFFFFFFFF)
#define PP_RX_DMA_WRED_ADDR_RESP_WRED_ADDR_RESP_I_RST               (0x0)

/**
 * SW_REG_NAME : PP_RX_DMA_PKT_CNT_DBG_REG
 * HW_REG_NAME : packet_cnt_dbg_reg
 * DESCRIPTION : in_packet_count - count the packet as DMA recive
 *               SOP of packet
 *
 *  Register Fields :
 *   [31: 0][RO] - in_packet_count_dbg
 *
 */
#define PP_RX_DMA_PKT_CNT_DBG_REG                   ((RX_DMA_BASE_ADDR) + 0x938)
#define PP_RX_DMA_PKT_CNT_DBG_PKT_CNT_DBG_I_OFF                     (0)
#define PP_RX_DMA_PKT_CNT_DBG_PKT_CNT_DBG_I_LEN                     (32)
#define PP_RX_DMA_PKT_CNT_DBG_PKT_CNT_DBG_I_MSK                     (0xFFFFFFFF)
#define PP_RX_DMA_PKT_CNT_DBG_PKT_CNT_DBG_I_RST                     (0x0)

/**
 * SW_REG_NAME : PP_RX_DMA_DESC_DBG_REG
 * HW_REG_NAME : desc_dbg_reg
 * DESCRIPTION : descritor_dbg -capture the out descriptor for
 *               debug
 *
 *  Register Fields :
 *   [31: 0][RO] - status_dbg
 *
 */
#define PP_RX_DMA_DESC_DBG_REG                      ((RX_DMA_BASE_ADDR) + 0x93C)
#define PP_RX_DMA_DESC_DBG_DESC_DBG_I_OFF                           (0)
#define PP_RX_DMA_DESC_DBG_DESC_DBG_I_LEN                           (32)
#define PP_RX_DMA_DESC_DBG_DESC_DBG_I_MSK                           (0xFFFFFFFF)
#define PP_RX_DMA_DESC_DBG_DESC_DBG_I_RST                           (0x0)
/**
 * REG_IDX_ACCESS   : PP_RX_DMA_DESC_DBG_REG_IDX
 * NUM OF REGISTERS : 4
 */
#define PP_RX_DMA_DESC_DBG_REG_IDX(idx) \
	(PP_RX_DMA_DESC_DBG_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_RX_DMA_CFG_REG
 * HW_REG_NAME : dma_config_reg
 * DESCRIPTION : dma_config
 *
 *  Register Fields :
 *   [31:31][RW] - do buffer-exchange if the policies are identical
 *   [30:29][RO] - Reserved
 *   [28:24][RW] - set the max nPendingTransaction (outstanding)
 *                 support by the RX_DMA , can be setbetween 1-16 ,
 *                 NOTE, B Step ONLY field
 *   [23:23][RW] - WNP disable at end of the packet ; DMA will not
 *                 wait for response before send descriptor to Q-mng
 *   [22:17][RO] - Reserved
 *   [16:16][RW] - ddr_align_enable- the DMA will deivide the
 *                 transaction to burst which align to DDR line
 *   [15: 0][RW] - max burst - the DMA will deivide the transaction
 *                 to several burst with mac burst size
 *
 */
#define PP_RX_DMA_CFG_REG                           ((RX_DMA_BASE_ADDR) + 0x94C)
#define PP_RX_DMA_CFG_BUFFER_EXCHANGE_POLICIES_IDENTICAL_I_OFF      (31)
#define PP_RX_DMA_CFG_BUFFER_EXCHANGE_POLICIES_IDENTICAL_I_LEN      (1)
#define PP_RX_DMA_CFG_BUFFER_EXCHANGE_POLICIES_IDENTICAL_I_MSK      (0x80000000)
#define PP_RX_DMA_CFG_BUFFER_EXCHANGE_POLICIES_IDENTICAL_I_RST      (0x0)
#define PP_RX_DMA_CFG_RSVD0_OFF                                     (29)
#define PP_RX_DMA_CFG_RSVD0_LEN                                     (2)
#define PP_RX_DMA_CFG_RSVD0_MSK                                     (0x60000000)
#define PP_RX_DMA_CFG_RSVD0_RST                                     (0x0)
#define PP_RX_DMA_CFG_MAX_NPENDINGTRANS_OFF                         (24)
#define PP_RX_DMA_CFG_MAX_NPENDINGTRANS_LEN                         (5)
#define PP_RX_DMA_CFG_MAX_NPENDINGTRANS_MSK                         (0x1F000000)
#define PP_RX_DMA_CFG_MAX_NPENDINGTRANS_RST                         (0x10)
#define PP_RX_DMA_CFG_WNP_DIS_OFF                                   (23)
#define PP_RX_DMA_CFG_WNP_DIS_LEN                                   (1)
#define PP_RX_DMA_CFG_WNP_DIS_MSK                                   (0x00800000)
#define PP_RX_DMA_CFG_WNP_DIS_RST                                   (0x0)
#define PP_RX_DMA_CFG_RSVD1_OFF                                     (17)
#define PP_RX_DMA_CFG_RSVD1_LEN                                     (6)
#define PP_RX_DMA_CFG_RSVD1_MSK                                     (0x007E0000)
#define PP_RX_DMA_CFG_RSVD1_RST                                     (0x0)
#define PP_RX_DMA_CFG_DDR_ALIGN_EN_OFF                              (16)
#define PP_RX_DMA_CFG_DDR_ALIGN_EN_LEN                              (1)
#define PP_RX_DMA_CFG_DDR_ALIGN_EN_MSK                              (0x00010000)
#define PP_RX_DMA_CFG_DDR_ALIGN_EN_RST                              (0x0)
#define PP_RX_DMA_CFG_MAX_BURST_OFF                                 (0)
#define PP_RX_DMA_CFG_MAX_BURST_LEN                                 (16)
#define PP_RX_DMA_CFG_MAX_BURST_MSK                                 (0x0000FFFF)
#define PP_RX_DMA_CFG_MAX_BURST_RST                                 (0x8)

/**
 * SW_REG_NAME : PP_RX_DMA_BMGR_BASE_REG
 * HW_REG_NAME : bmgr_base_reg
 * DESCRIPTION : bmgr_base
 *
 *  Register Fields :
 *   [31: 0][RW] - bmgr_base ; MSB bits
 *
 */
#define PP_RX_DMA_BMGR_BASE_REG                     ((RX_DMA_BASE_ADDR) + 0x95C)
#define PP_RX_DMA_BMGR_BASE_BMGR_BASE_I_OFF                         (0)
#define PP_RX_DMA_BMGR_BASE_BMGR_BASE_I_LEN                         (32)
#define PP_RX_DMA_BMGR_BASE_BMGR_BASE_I_MSK                         (0xFFFFFFFF)
#define PP_RX_DMA_BMGR_BASE_BMGR_BASE_I_RST                         (0x0)

/**
 * SW_REG_NAME : PP_RX_DMA_FSQM_BUFFER_BASE_REG
 * HW_REG_NAME : fsqm_buffer_base_reg
 * DESCRIPTION : fsqm_base
 *
 *  Register Fields :
 *   [31: 0][RW] - fsqm_base ; 32 LSB bits
 *
 */
#define PP_RX_DMA_FSQM_BUFFER_BASE_REG              ((RX_DMA_BASE_ADDR) + 0x960)
#define PP_RX_DMA_FSQM_BUFFER_BASE_FSQM_BASE_I_OFF                  (0)
#define PP_RX_DMA_FSQM_BUFFER_BASE_FSQM_BASE_I_LEN                  (32)
#define PP_RX_DMA_FSQM_BUFFER_BASE_FSQM_BASE_I_MSK                  (0xFFFFFFFF)
#define PP_RX_DMA_FSQM_BUFFER_BASE_FSQM_BASE_I_RST                  (0x0)

/**
 * SW_REG_NAME : PP_RX_DMA_WRED_BASE_REG
 * HW_REG_NAME : wred_base_reg
 * DESCRIPTION : wred_base
 *
 *  Register Fields :
 *   [31: 0][RW] - wred_base ; MSB bits
 *
 */
#define PP_RX_DMA_WRED_BASE_REG                     ((RX_DMA_BASE_ADDR) + 0x964)
#define PP_RX_DMA_WRED_BASE_WRED_BASE_I_OFF                         (0)
#define PP_RX_DMA_WRED_BASE_WRED_BASE_I_LEN                         (32)
#define PP_RX_DMA_WRED_BASE_WRED_BASE_I_MSK                         (0xFFFFFFFF)
#define PP_RX_DMA_WRED_BASE_WRED_BASE_I_RST                         (0x0)

/**
 * SW_REG_NAME : PP_RX_DMA_Q_MANAGER_BASE_REG
 * HW_REG_NAME : queue_manager_base_reg
 * DESCRIPTION : queue_manager_base
 *
 *  Register Fields :
 *   [31: 0][RW] - queue_manager_base ; MSB bits
 *
 */
#define PP_RX_DMA_Q_MANAGER_BASE_REG                ((RX_DMA_BASE_ADDR) + 0x968)
#define PP_RX_DMA_Q_MANAGER_BASE_Q_MANAGER_BASE_I_OFF               (0)
#define PP_RX_DMA_Q_MANAGER_BASE_Q_MANAGER_BASE_I_LEN               (32)
#define PP_RX_DMA_Q_MANAGER_BASE_Q_MANAGER_BASE_I_MSK               (0xFFFFFFFF)
#define PP_RX_DMA_Q_MANAGER_BASE_Q_MANAGER_BASE_I_RST               (0x0)

/**
 * SW_REG_NAME : PP_RX_DMA_FSQM_BUFFER_MSB_BASE_REG
 * HW_REG_NAME : fsqm_buffer_msb_base_reg
 * DESCRIPTION : fsqm MSB 4b bit for the desctiptor address
 *
 *  Register Fields :
 *   [31: 4][RO] - Reserved
 *   [ 3: 0][RW] - fsqm buffer base.4 MSB bits to send the buffers
 *                 from the fsqm which is 32 bit
 *
 */
#define PP_RX_DMA_FSQM_BUFFER_MSB_BASE_REG          ((RX_DMA_BASE_ADDR) + 0x96C)
#define PP_RX_DMA_FSQM_BUFFER_MSB_BASE_RSVD0_OFF                    (4)
#define PP_RX_DMA_FSQM_BUFFER_MSB_BASE_RSVD0_LEN                    (28)
#define PP_RX_DMA_FSQM_BUFFER_MSB_BASE_RSVD0_MSK                    (0xFFFFFFF0)
#define PP_RX_DMA_FSQM_BUFFER_MSB_BASE_RSVD0_RST                    (0x0)
#define PP_RX_DMA_FSQM_BUFFER_MSB_BASE_FSQM_BUFF_BASE_I_OFF         (0)
#define PP_RX_DMA_FSQM_BUFFER_MSB_BASE_FSQM_BUFF_BASE_I_LEN         (4)
#define PP_RX_DMA_FSQM_BUFFER_MSB_BASE_FSQM_BUFF_BASE_I_MSK         (0x0000000F)
#define PP_RX_DMA_FSQM_BUFFER_MSB_BASE_FSQM_BUFF_BASE_I_RST         (0x0)

/**
 * SW_REG_NAME : PP_RX_DMA_FSQM_DESC_ADDR_REG
 * HW_REG_NAME : fsqm_desc_addr_reg
 * DESCRIPTION : fsqm_descriptor_address
 *
 *  Register Fields :
 *   [31: 0][RW] - fsqm descriptor address. this IF use as FIFO same
 *                 address for all descriptors
 *
 */
#define PP_RX_DMA_FSQM_DESC_ADDR_REG                ((RX_DMA_BASE_ADDR) + 0x970)
#define PP_RX_DMA_FSQM_DESC_ADDR_FSQM_DESC_ADDR_I_OFF               (0)
#define PP_RX_DMA_FSQM_DESC_ADDR_FSQM_DESC_ADDR_I_LEN               (32)
#define PP_RX_DMA_FSQM_DESC_ADDR_FSQM_DESC_ADDR_I_MSK               (0xFFFFFFFF)
#define PP_RX_DMA_FSQM_DESC_ADDR_FSQM_DESC_ADDR_I_RST               (0x0)

/**
 * SW_REG_NAME : PP_RX_DMA_SRAM_START_ADDR_REG
 * HW_REG_NAME : sram_start_addr_reg
 * DESCRIPTION : sram_start_addr
 *
 *  Register Fields :
 *   [31: 0][RW] - sram_start_addr ; internal SRAM will use diffrent
 *                 TAG's in the transaction then the packet wrriten
 *                 to DDR; this register define the addr[35:4]
 *
 */
#define PP_RX_DMA_SRAM_START_ADDR_REG               ((RX_DMA_BASE_ADDR) + 0x974)
#define PP_RX_DMA_SRAM_START_ADDR_SRAM_START_ADDR_I_OFF             (0)
#define PP_RX_DMA_SRAM_START_ADDR_SRAM_START_ADDR_I_LEN             (32)
#define PP_RX_DMA_SRAM_START_ADDR_SRAM_START_ADDR_I_MSK             (0xFFFFFFFF)
#define PP_RX_DMA_SRAM_START_ADDR_SRAM_START_ADDR_I_RST             (0x0)

/**
 * SW_REG_NAME : PP_RX_DMA_SRAM_SIZE_REG
 * HW_REG_NAME : sram_size_reg
 * DESCRIPTION : sram_size
 *
 *  Register Fields :
 *   [31: 0][RW] - sram_size  ; define the internal SRAM size in 16B
 *                 granularit ; this register define the size[35:4]
 *
 */
#define PP_RX_DMA_SRAM_SIZE_REG                     ((RX_DMA_BASE_ADDR) + 0x978)
#define PP_RX_DMA_SRAM_SIZE_SRAM_SIZE_I_OFF                         (0)
#define PP_RX_DMA_SRAM_SIZE_SRAM_SIZE_I_LEN                         (32)
#define PP_RX_DMA_SRAM_SIZE_SRAM_SIZE_I_MSK                         (0xFFFFFFFF)
#define PP_RX_DMA_SRAM_SIZE_SRAM_SIZE_I_RST                         (0x0)

/**
 * SW_REG_NAME : PP_RX_DMA_DROP_CAUSES_CFG_REG
 * HW_REG_NAME : drop_causes_cfg_reg
 * DESCRIPTION : drop_causes_cfg
 *
 *  Register Fields :
 *   [31: 2][RO] - Reserved
 *   [ 1: 0][RW] - counter to drop count while drop by both bmgr and
 *                 wred; 0 none ; 1- bmgr ; 2 wred ; 3 both
 *
 */
#define PP_RX_DMA_DROP_CAUSES_CFG_REG               ((RX_DMA_BASE_ADDR) + 0x97C)
#define PP_RX_DMA_DROP_CAUSES_CFG_RSVD0_OFF                         (2)
#define PP_RX_DMA_DROP_CAUSES_CFG_RSVD0_LEN                         (30)
#define PP_RX_DMA_DROP_CAUSES_CFG_RSVD0_MSK                         (0xFFFFFFFC)
#define PP_RX_DMA_DROP_CAUSES_CFG_RSVD0_RST                         (0x0)
#define PP_RX_DMA_DROP_CAUSES_CFG_DROP_COUNT_CFG_I_OFF              (0)
#define PP_RX_DMA_DROP_CAUSES_CFG_DROP_COUNT_CFG_I_LEN              (2)
#define PP_RX_DMA_DROP_CAUSES_CFG_DROP_COUNT_CFG_I_MSK              (0x00000003)
#define PP_RX_DMA_DROP_CAUSES_CFG_DROP_COUNT_CFG_I_RST              (0x0)

/**
 * SW_REG_NAME : PP_RX_DMA_FSQM_BUFFER_NULL_CNT_REG
 * HW_REG_NAME : fsqm_buffer_null_counter_reg
 * DESCRIPTION : fsqm_buffer_null_counter
 *
 *  Register Fields :
 *   [31: 0][RW] - Counts number of null buffers from FSQM (32 bit-
 *                 wrap around)
 *
 */
#define PP_RX_DMA_FSQM_BUFFER_NULL_CNT_REG          ((RX_DMA_BASE_ADDR) + 0x980)
#define PP_RX_DMA_FSQM_BUFFER_NULL_CNT_FSQM_BUFFER_NULL_CNT_I_OFF   (0)
#define PP_RX_DMA_FSQM_BUFFER_NULL_CNT_FSQM_BUFFER_NULL_CNT_I_LEN   (32)
#define PP_RX_DMA_FSQM_BUFFER_NULL_CNT_FSQM_BUFFER_NULL_CNT_I_MSK   (0xFFFFFFFF)
#define PP_RX_DMA_FSQM_BUFFER_NULL_CNT_FSQM_BUFFER_NULL_CNT_I_RST   (0x0)

/**
 * SW_REG_NAME : PP_RX_DMA_ERROR_CNT_REG
 * HW_REG_NAME : error_cnt_reg
 * DESCRIPTION : error counter - robustness violation (see
 *               robustness section on RX_DMA MAS
 *
 *  Register Fields :
 *   [31: 0][RW] - error_cnt
 *
 */
#define PP_RX_DMA_ERROR_CNT_REG                     ((RX_DMA_BASE_ADDR) + 0x984)
#define PP_RX_DMA_ERROR_CNT_ERROR_CNT_I_OFF                         (0)
#define PP_RX_DMA_ERROR_CNT_ERROR_CNT_I_LEN                         (32)
#define PP_RX_DMA_ERROR_CNT_ERROR_CNT_I_MSK                         (0xFFFFFFFF)
#define PP_RX_DMA_ERROR_CNT_ERROR_CNT_I_RST                         (0x0)

/**
 * SW_REG_NAME : PP_RX_DMA_ING_PORT_DROP_CAUSE_SEL_REG
 * HW_REG_NAME : ing_port_drop_cause_sel_reg
 * DESCRIPTION : port_drop_cause_sel
 *
 *  Register Fields :
 *   [31:12][RO] - Reserved
 *   [11:11][RW] - port drop counter cause select; enable for
 *                 counting drops according to ERROR packet
 *   [10:10][RW] - port drop counter cause select; enable for
 *                 counting drops according to NO_POLICY
 *   [ 9: 9][RW] - port drop counter cause select; enable for
 *                 counting drops according to BMGR
 *   [ 8: 8][RW] - port drop counter cause select; enable for
 *                 counting drops according to WRED
 *   [ 7: 0][RW] - port drop counter cause select; enable for
 *                 counting drops according to STW drop bits ; do not
 *                 set bit zero to 1
 *
 */
#define PP_RX_DMA_ING_PORT_DROP_CAUSE_SEL_REG       ((RX_DMA_BASE_ADDR) + 0x988)
#define PP_RX_DMA_ING_PORT_DROP_CAUSE_SEL_RSVD0_OFF                 (12)
#define PP_RX_DMA_ING_PORT_DROP_CAUSE_SEL_RSVD0_LEN                 (20)
#define PP_RX_DMA_ING_PORT_DROP_CAUSE_SEL_RSVD0_MSK                 (0xFFFFF000)
#define PP_RX_DMA_ING_PORT_DROP_CAUSE_SEL_RSVD0_RST                 (0x0)
#define PP_RX_DMA_ING_PORT_DROP_CAUSE_SEL_PORT_DROP_CAUSE_ERROR_SEL_I_OFF   (11)
#define PP_RX_DMA_ING_PORT_DROP_CAUSE_SEL_PORT_DROP_CAUSE_ERROR_SEL_I_LEN    (1)
#define PP_RX_DMA_ING_PORT_DROP_CAUSE_SEL_PORT_DROP_CAUSE_ERROR_SEL_I_MSK \
	(0x00000800)
#define PP_RX_DMA_ING_PORT_DROP_CAUSE_SEL_PORT_DROP_CAUSE_ERROR_SEL_I_RST  (0x0)
#define PP_RX_DMA_ING_PORT_DROP_CAUSE_SEL_PORT_DROP_CAUSE_NO_POLICY_SEL_I_OFF \
	(10)
#define PP_RX_DMA_ING_PORT_DROP_CAUSE_SEL_PORT_DROP_CAUSE_NO_POLICY_SEL_I_LEN \
	(1)
#define PP_RX_DMA_ING_PORT_DROP_CAUSE_SEL_PORT_DROP_CAUSE_NO_POLICY_SEL_I_MSK \
	(0x00000400)
#define PP_RX_DMA_ING_PORT_DROP_CAUSE_SEL_PORT_DROP_CAUSE_NO_POLICY_SEL_I_RST \
	(0x0)
#define PP_RX_DMA_ING_PORT_DROP_CAUSE_SEL_PORT_DROP_CAUSE_BMGR_SEL_I_OFF     (9)
#define PP_RX_DMA_ING_PORT_DROP_CAUSE_SEL_PORT_DROP_CAUSE_BMGR_SEL_I_LEN     (1)
#define PP_RX_DMA_ING_PORT_DROP_CAUSE_SEL_PORT_DROP_CAUSE_BMGR_SEL_I_MSK \
	(0x00000200)
#define PP_RX_DMA_ING_PORT_DROP_CAUSE_SEL_PORT_DROP_CAUSE_BMGR_SEL_I_RST   (0x0)
#define PP_RX_DMA_ING_PORT_DROP_CAUSE_SEL_PORT_DROP_CAUSE_WRED_SEL_I_OFF     (8)
#define PP_RX_DMA_ING_PORT_DROP_CAUSE_SEL_PORT_DROP_CAUSE_WRED_SEL_I_LEN     (1)
#define PP_RX_DMA_ING_PORT_DROP_CAUSE_SEL_PORT_DROP_CAUSE_WRED_SEL_I_MSK \
	(0x00000100)
#define PP_RX_DMA_ING_PORT_DROP_CAUSE_SEL_PORT_DROP_CAUSE_WRED_SEL_I_RST   (0x0)
#define PP_RX_DMA_ING_PORT_DROP_CAUSE_SEL_PORT_DROP_CAUSE_STW_SEL_I_OFF      (0)
#define PP_RX_DMA_ING_PORT_DROP_CAUSE_SEL_PORT_DROP_CAUSE_STW_SEL_I_LEN      (8)
#define PP_RX_DMA_ING_PORT_DROP_CAUSE_SEL_PORT_DROP_CAUSE_STW_SEL_I_MSK \
	(0x000000FF)
#define PP_RX_DMA_ING_PORT_DROP_CAUSE_SEL_PORT_DROP_CAUSE_STW_SEL_I_RST    (0x0)

/**
 * SW_REG_NAME : PP_RX_DMA_EGR_PORT_DROP_CAUSE_SEL_REG
 * HW_REG_NAME : egr_port_drop_cause_sel_reg
 * DESCRIPTION : port_drop_cause_sel
 *
 *  Register Fields :
 *   [31:12][RO] - Reserved
 *   [11:11][RW] - port drop counter cause select; enable for
 *                 counting drops according to ERROR packet
 *   [10:10][RW] - port drop counter cause select; enable for
 *                 counting drops according to NO_POLICY
 *   [ 9: 9][RW] - port drop counter cause select; enable for
 *                 counting drops according to BMGR
 *   [ 8: 8][RW] - port drop counter cause select; enable for
 *                 counting drops according to WRED
 *   [ 7: 0][RW] - port drop counter cause select; enable for
 *                 counting drops according to STW drop bits ; do not
 *                 set bit zero to 1
 *
 */
#define PP_RX_DMA_EGR_PORT_DROP_CAUSE_SEL_REG       ((RX_DMA_BASE_ADDR) + 0x98C)
#define PP_RX_DMA_EGR_PORT_DROP_CAUSE_SEL_RSVD0_OFF                 (12)
#define PP_RX_DMA_EGR_PORT_DROP_CAUSE_SEL_RSVD0_LEN                 (20)
#define PP_RX_DMA_EGR_PORT_DROP_CAUSE_SEL_RSVD0_MSK                 (0xFFFFF000)
#define PP_RX_DMA_EGR_PORT_DROP_CAUSE_SEL_RSVD0_RST                 (0x0)
#define PP_RX_DMA_EGR_PORT_DROP_CAUSE_SEL_PORT_DROP_CAUSE_ERROR_SEL_I_OFF   (11)
#define PP_RX_DMA_EGR_PORT_DROP_CAUSE_SEL_PORT_DROP_CAUSE_ERROR_SEL_I_LEN    (1)
#define PP_RX_DMA_EGR_PORT_DROP_CAUSE_SEL_PORT_DROP_CAUSE_ERROR_SEL_I_MSK \
	(0x00000800)
#define PP_RX_DMA_EGR_PORT_DROP_CAUSE_SEL_PORT_DROP_CAUSE_ERROR_SEL_I_RST  (0x0)
#define PP_RX_DMA_EGR_PORT_DROP_CAUSE_SEL_PORT_DROP_CAUSE_NO_POLICY_SEL_I_OFF \
	(10)
#define PP_RX_DMA_EGR_PORT_DROP_CAUSE_SEL_PORT_DROP_CAUSE_NO_POLICY_SEL_I_LEN \
	(1)
#define PP_RX_DMA_EGR_PORT_DROP_CAUSE_SEL_PORT_DROP_CAUSE_NO_POLICY_SEL_I_MSK \
	(0x00000400)
#define PP_RX_DMA_EGR_PORT_DROP_CAUSE_SEL_PORT_DROP_CAUSE_NO_POLICY_SEL_I_RST \
	(0x0)
#define PP_RX_DMA_EGR_PORT_DROP_CAUSE_SEL_PORT_DROP_CAUSE_BMGR_SEL_I_OFF     (9)
#define PP_RX_DMA_EGR_PORT_DROP_CAUSE_SEL_PORT_DROP_CAUSE_BMGR_SEL_I_LEN     (1)
#define PP_RX_DMA_EGR_PORT_DROP_CAUSE_SEL_PORT_DROP_CAUSE_BMGR_SEL_I_MSK \
	(0x00000200)
#define PP_RX_DMA_EGR_PORT_DROP_CAUSE_SEL_PORT_DROP_CAUSE_BMGR_SEL_I_RST   (0x0)
#define PP_RX_DMA_EGR_PORT_DROP_CAUSE_SEL_PORT_DROP_CAUSE_WRED_SEL_I_OFF     (8)
#define PP_RX_DMA_EGR_PORT_DROP_CAUSE_SEL_PORT_DROP_CAUSE_WRED_SEL_I_LEN     (1)
#define PP_RX_DMA_EGR_PORT_DROP_CAUSE_SEL_PORT_DROP_CAUSE_WRED_SEL_I_MSK \
	(0x00000100)
#define PP_RX_DMA_EGR_PORT_DROP_CAUSE_SEL_PORT_DROP_CAUSE_WRED_SEL_I_RST   (0x0)
#define PP_RX_DMA_EGR_PORT_DROP_CAUSE_SEL_PORT_DROP_CAUSE_STW_SEL_I_OFF      (0)
#define PP_RX_DMA_EGR_PORT_DROP_CAUSE_SEL_PORT_DROP_CAUSE_STW_SEL_I_LEN      (8)
#define PP_RX_DMA_EGR_PORT_DROP_CAUSE_SEL_PORT_DROP_CAUSE_STW_SEL_I_MSK \
	(0x000000FF)
#define PP_RX_DMA_EGR_PORT_DROP_CAUSE_SEL_PORT_DROP_CAUSE_STW_SEL_I_RST    (0x0)

/**
 * SW_REG_NAME : PP_RX_DMA_FSQM_PKT_LEN_MAX_REG
 * HW_REG_NAME : fsqm_packet_len_max_reg
 * DESCRIPTION : fsqm_packet_len_max - define the max packet len
 *               for FSQM default as Max_line_cnt*16-Max
 *               Header_length
 *
 *  Register Fields :
 *   [31:15][RO] - Reserved
 *   [14: 0][RW] - fsqm_packet_len_max
 *
 */
#define PP_RX_DMA_FSQM_PKT_LEN_MAX_REG              ((RX_DMA_BASE_ADDR) + 0x990)
#define PP_RX_DMA_FSQM_PKT_LEN_MAX_RSVD0_OFF                        (15)
#define PP_RX_DMA_FSQM_PKT_LEN_MAX_RSVD0_LEN                        (17)
#define PP_RX_DMA_FSQM_PKT_LEN_MAX_RSVD0_MSK                        (0xFFFF8000)
#define PP_RX_DMA_FSQM_PKT_LEN_MAX_RSVD0_RST                        (0x0)
#define PP_RX_DMA_FSQM_PKT_LEN_MAX_FSQM_PKT_LEN_MAX_I_OFF           (0)
#define PP_RX_DMA_FSQM_PKT_LEN_MAX_FSQM_PKT_LEN_MAX_I_LEN           (15)
#define PP_RX_DMA_FSQM_PKT_LEN_MAX_FSQM_PKT_LEN_MAX_I_MSK           (0x00007FFF)
#define PP_RX_DMA_FSQM_PKT_LEN_MAX_FSQM_PKT_LEN_MAX_I_RST           (0x3f80)

/**
 * SW_REG_NAME : PP_RX_DMA_FSQM_PKT_LEN_MAX_DROP_CNT_REG
 * HW_REG_NAME : fsqm_packet_len_max_drop_counter_reg
 * DESCRIPTION : fsqm_packet_len_max_drop_counter - count the drop
 *               cause by max packet len for FSQM flow
 *
 *  Register Fields :
 *   [31: 0][RW] - fsqm_packet_len_max_drop_counter
 *
 */
#define PP_RX_DMA_FSQM_PKT_LEN_MAX_DROP_CNT_REG     ((RX_DMA_BASE_ADDR) + 0x994)
#define PP_RX_DMA_FSQM_PKT_LEN_MAX_DROP_CNT_FSQM_PKT_LEN_MAX_DROP_CNT_I_OFF  (0)
#define PP_RX_DMA_FSQM_PKT_LEN_MAX_DROP_CNT_FSQM_PKT_LEN_MAX_DROP_CNT_I_LEN (32)
#define PP_RX_DMA_FSQM_PKT_LEN_MAX_DROP_CNT_FSQM_PKT_LEN_MAX_DROP_CNT_I_MSK \
	(0xFFFFFFFF)
#define PP_RX_DMA_FSQM_PKT_LEN_MAX_DROP_CNT_FSQM_PKT_LEN_MAX_DROP_CNT_I_RST \
	(0x0)

/**
 * SW_REG_NAME : PP_RX_DMA_HEADER_LEN_ZERO_DROP_CNT_REG
 * HW_REG_NAME : header_len_zero_drop_counter_reg
 * DESCRIPTION : Header_len_zero_drop_counter
 *
 *  Register Fields :
 *   [31: 0][RW] - Header_len_zero_drop_counter
 *
 */
#define PP_RX_DMA_HEADER_LEN_ZERO_DROP_CNT_REG      ((RX_DMA_BASE_ADDR) + 0x998)
#define PP_RX_DMA_HEADER_LEN_ZERO_DROP_CNT_HEADER_LEN_ZERO_DROP_CNT_I_OFF    (0)
#define PP_RX_DMA_HEADER_LEN_ZERO_DROP_CNT_HEADER_LEN_ZERO_DROP_CNT_I_LEN   (32)
#define PP_RX_DMA_HEADER_LEN_ZERO_DROP_CNT_HEADER_LEN_ZERO_DROP_CNT_I_MSK \
	(0xFFFFFFFF)
#define PP_RX_DMA_HEADER_LEN_ZERO_DROP_CNT_HEADER_LEN_ZERO_DROP_CNT_I_RST  (0x0)

/**
 * SW_REG_NAME : PP_RX_DMA_STATUS_DBG_REG
 * HW_REG_NAME : status_dbg_reg
 * DESCRIPTION : status_dbg -capture the input status for debug
 *
 *  Register Fields :
 *   [31: 0][RO] - status_dbg
 *
 */
#define PP_RX_DMA_STATUS_DBG_REG                    ((RX_DMA_BASE_ADDR) + 0x99C)
#define PP_RX_DMA_STATUS_DBG_STATUS_DBG_I_OFF                       (0)
#define PP_RX_DMA_STATUS_DBG_STATUS_DBG_I_LEN                       (32)
#define PP_RX_DMA_STATUS_DBG_STATUS_DBG_I_MSK                       (0xFFFFFFFF)
#define PP_RX_DMA_STATUS_DBG_STATUS_DBG_I_RST                       (0x0)
/**
 * REG_IDX_ACCESS   : PP_RX_DMA_STATUS_DBG_REG_IDX
 * NUM OF REGISTERS : 7
 */
#define PP_RX_DMA_STATUS_DBG_REG_IDX(idx) \
	(PP_RX_DMA_STATUS_DBG_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_RX_DMA_FSQM_PKT_CNT_REG
 * HW_REG_NAME : fsqm_packet_counter_reg
 * DESCRIPTION : fsqm_packet_counter, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][RW] - Counts number of FSQM packets  (32 bit- wrap
 *                 around)
 *
 */
#define PP_RX_DMA_FSQM_PKT_CNT_REG                  ((RX_DMA_BASE_ADDR) + 0x9B8)
#define PP_RX_DMA_FSQM_PKT_CNT_FSQM_PKT_CNT_I_OFF                   (0)
#define PP_RX_DMA_FSQM_PKT_CNT_FSQM_PKT_CNT_I_LEN                   (32)
#define PP_RX_DMA_FSQM_PKT_CNT_FSQM_PKT_CNT_I_MSK                   (0xFFFFFFFF)
#define PP_RX_DMA_FSQM_PKT_CNT_FSQM_PKT_CNT_I_RST                   (0x0)

/**
 * SW_REG_NAME : PP_RX_DMA_INGRESS_PORT_BYTE_DROP_CNT_LOW_REG
 * HW_REG_NAME : ingress_port_byte_drop_cnt_low_reg
 * DESCRIPTION : ingress_drop_cnt
 *
 *  Register Fields :
 *   [31: 0][RW] - ingress_drop_cnt ; 64 bit counter
 *
 */
#define PP_RX_DMA_INGRESS_PORT_BYTE_DROP_CNT_LOW_REG \
	((RX_DMA_BASE_ADDR) + 0x01000)
#define PP_RX_DMA_INGRESS_PORT_BYTE_DROP_CNT_LOW_FIELD_OFF          (0)
#define PP_RX_DMA_INGRESS_PORT_BYTE_DROP_CNT_LOW_FIELD_LEN          (32)
#define PP_RX_DMA_INGRESS_PORT_BYTE_DROP_CNT_LOW_FIELD_MSK          (0xFFFFFFFF)
#define PP_RX_DMA_INGRESS_PORT_BYTE_DROP_CNT_LOW_FIELD_RST          (0x0)
/**
 * REG_IDX_ACCESS   : PP_RX_DMA_INGRESS_PORT_BYTE_DROP_CNT_LOW_REG_IDX
 * NUM OF REGISTERS : 256
 */
#define PP_RX_DMA_INGRESS_PORT_BYTE_DROP_CNT_LOW_REG_IDX(idx) \
	(PP_RX_DMA_INGRESS_PORT_BYTE_DROP_CNT_LOW_REG + ((idx) << 3))

/**
 * SW_REG_NAME : PP_RX_DMA_INGRESS_PORT_BYTE_DROP_CNT_HIGH_REG
 * HW_REG_NAME : ingress_port_byte_drop_cnt_high_reg
 * DESCRIPTION : ingress_drop_cnt
 *
 *  Register Fields :
 *   [31: 0][RW] - ingress_drop_cnt ; 64 bit counter
 *
 */
#define PP_RX_DMA_INGRESS_PORT_BYTE_DROP_CNT_HIGH_REG \
	((RX_DMA_BASE_ADDR) + 0x1004)
#define PP_RX_DMA_INGRESS_PORT_BYTE_DROP_CNT_HIGH_FIELD_OFF         (0)
#define PP_RX_DMA_INGRESS_PORT_BYTE_DROP_CNT_HIGH_FIELD_LEN         (32)
#define PP_RX_DMA_INGRESS_PORT_BYTE_DROP_CNT_HIGH_FIELD_MSK         (0xFFFFFFFF)
#define PP_RX_DMA_INGRESS_PORT_BYTE_DROP_CNT_HIGH_FIELD_RST         (0x0)
/**
 * REG_IDX_ACCESS   : PP_RX_DMA_INGRESS_PORT_BYTE_DROP_CNT_HIGH_REG_IDX
 * NUM OF REGISTERS : 256
 */
#define PP_RX_DMA_INGRESS_PORT_BYTE_DROP_CNT_HIGH_REG_IDX(idx) \
	(PP_RX_DMA_INGRESS_PORT_BYTE_DROP_CNT_HIGH_REG + ((idx) << 3))

/**
 * SW_REG_NAME : PP_RX_DMA_EGRESS_PORT_BYTE_DROP_CNT_LOW_REG
 * HW_REG_NAME : egress_port_byte_drop_cnt_low_reg
 * DESCRIPTION : ingress_drop_cnt
 *
 *  Register Fields :
 *   [31: 0][RW] - ingress_drop_cnt ; 64 bit counter
 *
 */
#define PP_RX_DMA_EGRESS_PORT_BYTE_DROP_CNT_LOW_REG \
	((RX_DMA_BASE_ADDR) + 0x01800)
#define PP_RX_DMA_EGRESS_PORT_BYTE_DROP_CNT_LOW_FIELD_OFF           (0)
#define PP_RX_DMA_EGRESS_PORT_BYTE_DROP_CNT_LOW_FIELD_LEN           (32)
#define PP_RX_DMA_EGRESS_PORT_BYTE_DROP_CNT_LOW_FIELD_MSK           (0xFFFFFFFF)
#define PP_RX_DMA_EGRESS_PORT_BYTE_DROP_CNT_LOW_FIELD_RST           (0x0)
/**
 * REG_IDX_ACCESS   : PP_RX_DMA_EGRESS_PORT_BYTE_DROP_CNT_LOW_REG_IDX
 * NUM OF REGISTERS : 256
 */
#define PP_RX_DMA_EGRESS_PORT_BYTE_DROP_CNT_LOW_REG_IDX(idx) \
	(PP_RX_DMA_EGRESS_PORT_BYTE_DROP_CNT_LOW_REG + ((idx) << 3))

/**
 * SW_REG_NAME : PP_RX_DMA_EGRESS_PORT_BYTE_DROP_CNT_HIGH_REG
 * HW_REG_NAME : egress_port_byte_drop_cnt_high_reg
 * DESCRIPTION : ingress_drop_cnt
 *
 *  Register Fields :
 *   [31: 0][RW] - ingress_drop_cnt ; 64 bit counter
 *
 */
#define PP_RX_DMA_EGRESS_PORT_BYTE_DROP_CNT_HIGH_REG \
	((RX_DMA_BASE_ADDR) + 0x01804)
#define PP_RX_DMA_EGRESS_PORT_BYTE_DROP_CNT_HIGH_FIELD_OFF          (0)
#define PP_RX_DMA_EGRESS_PORT_BYTE_DROP_CNT_HIGH_FIELD_LEN          (32)
#define PP_RX_DMA_EGRESS_PORT_BYTE_DROP_CNT_HIGH_FIELD_MSK          (0xFFFFFFFF)
#define PP_RX_DMA_EGRESS_PORT_BYTE_DROP_CNT_HIGH_FIELD_RST          (0x0)
/**
 * REG_IDX_ACCESS   : PP_RX_DMA_EGRESS_PORT_BYTE_DROP_CNT_HIGH_REG_IDX
 * NUM OF REGISTERS : 256
 */
#define PP_RX_DMA_EGRESS_PORT_BYTE_DROP_CNT_HIGH_REG_IDX(idx) \
	(PP_RX_DMA_EGRESS_PORT_BYTE_DROP_CNT_HIGH_REG + ((idx) << 3))

/**
 * SW_REG_NAME : PP_RX_DMA_INGRESS_PORT_PKT_DROP_CNT_REG
 * HW_REG_NAME : ingress_port_pkt_drop_cnt_reg
 * DESCRIPTION : ingress_packet_drop_cnt
 *
 *  Register Fields :
 *   [31: 0][RW] - ingress_packet_drop_cnt
 *
 */
#define PP_RX_DMA_INGRESS_PORT_PKT_DROP_CNT_REG   ((RX_DMA_BASE_ADDR) + 0x02000)
#define PP_RX_DMA_INGRESS_PORT_PKT_DROP_CNT_FIELD_OFF               (0)
#define PP_RX_DMA_INGRESS_PORT_PKT_DROP_CNT_FIELD_LEN               (32)
#define PP_RX_DMA_INGRESS_PORT_PKT_DROP_CNT_FIELD_MSK               (0xFFFFFFFF)
#define PP_RX_DMA_INGRESS_PORT_PKT_DROP_CNT_FIELD_RST               (0x0)
/**
 * REG_IDX_ACCESS   : PP_RX_DMA_INGRESS_PORT_PKT_DROP_CNT_REG_IDX
 * NUM OF REGISTERS : 256
 */
#define PP_RX_DMA_INGRESS_PORT_PKT_DROP_CNT_REG_IDX(idx) \
	(PP_RX_DMA_INGRESS_PORT_PKT_DROP_CNT_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_RX_DMA_EGRESS_PORT_PKT_DROP_CNT_REG
 * HW_REG_NAME : egress_port_pkt_drop_cnt_reg
 * DESCRIPTION : eggress_packet_drop_cnt
 *
 *  Register Fields :
 *   [31: 0][RW] - egress_packet_drop_cnt
 *
 */
#define PP_RX_DMA_EGRESS_PORT_PKT_DROP_CNT_REG    ((RX_DMA_BASE_ADDR) + 0x02400)
#define PP_RX_DMA_EGRESS_PORT_PKT_DROP_CNT_FIELD_OFF                (0)
#define PP_RX_DMA_EGRESS_PORT_PKT_DROP_CNT_FIELD_LEN                (32)
#define PP_RX_DMA_EGRESS_PORT_PKT_DROP_CNT_FIELD_MSK                (0xFFFFFFFF)
#define PP_RX_DMA_EGRESS_PORT_PKT_DROP_CNT_FIELD_RST                (0x0)
/**
 * REG_IDX_ACCESS   : PP_RX_DMA_EGRESS_PORT_PKT_DROP_CNT_REG_IDX
 * NUM OF REGISTERS : 256
 */
#define PP_RX_DMA_EGRESS_PORT_PKT_DROP_CNT_REG_IDX(idx) \
	(PP_RX_DMA_EGRESS_PORT_PKT_DROP_CNT_REG + ((idx) << 2))

#endif
