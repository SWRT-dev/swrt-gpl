/**
 * rpb_regs.h
 * Description: ppv4_rpb_s HW registers definitions
 * Auto-generated file by RDL Parser, DO NOT CHANGE
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2019-2020 Intel Corporation
 */

#ifndef _PP_RPB_H_
#define _PP_RPB_H_

#define PP_RPB_GEN_DATE_STR                   "Sunday May 17, 2020 [5:26:11 pm]"
#define PP_RPB_BASE                                              (0xF0018000ULL)

/**
 * SW_REG_NAME : PP_RPB_GLGEN_DUAL40_REG
 * HW_REG_NAME : glgen_dual40_rpb_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 5][RO] - Reserved.
 *   [ 4: 4][RW] - Rx DMA RPB command source. 1b if the source is PE
 *                 packet
 *   [ 3: 3][RW] - Ingres UC RPB command source. 1b if the source is
 *                 PE packet
 *   [ 2: 2][RW] - Modifier RPB command source. 1b if the source is
 *                 PE packet
 *   [ 1: 1][RW] - Parser RPB command source. 1b if the source is PE
 *                 packet
 *   [ 0: 0][RW] - 0b = Single 40 GbE port mode.    1b = Dual 40 GbE
 *                 port mode.
 *
 */
#define PP_RPB_GLGEN_DUAL40_REG                        ((RPB_BASE_ADDR) + 0x7E0)
#define PP_RPB_GLGEN_DUAL40_RSVD_2_OFF                              (5)
#define PP_RPB_GLGEN_DUAL40_RSVD_2_LEN                              (27)
#define PP_RPB_GLGEN_DUAL40_RSVD_2_MSK                              (0xFFFFFFE0)
#define PP_RPB_GLGEN_DUAL40_RSVD_2_RST                              (0x0)
#define PP_RPB_GLGEN_DUAL40_RX_DMA_RPB_CMD_SRC_OFF                  (4)
#define PP_RPB_GLGEN_DUAL40_RX_DMA_RPB_CMD_SRC_LEN                  (1)
#define PP_RPB_GLGEN_DUAL40_RX_DMA_RPB_CMD_SRC_MSK                  (0x00000010)
#define PP_RPB_GLGEN_DUAL40_RX_DMA_RPB_CMD_SRC_RST                  (0x1)
#define PP_RPB_GLGEN_DUAL40_INGRESS_UC_RPB_CMD_SRC_OFF              (3)
#define PP_RPB_GLGEN_DUAL40_INGRESS_UC_RPB_CMD_SRC_LEN              (1)
#define PP_RPB_GLGEN_DUAL40_INGRESS_UC_RPB_CMD_SRC_MSK              (0x00000008)
#define PP_RPB_GLGEN_DUAL40_INGRESS_UC_RPB_CMD_SRC_RST              (0x0)
#define PP_RPB_GLGEN_DUAL40_MODIFIER_RPB_CMD_SRC_OFF                (2)
#define PP_RPB_GLGEN_DUAL40_MODIFIER_RPB_CMD_SRC_LEN                (1)
#define PP_RPB_GLGEN_DUAL40_MODIFIER_RPB_CMD_SRC_MSK                (0x00000004)
#define PP_RPB_GLGEN_DUAL40_MODIFIER_RPB_CMD_SRC_RST                (0x0)
#define PP_RPB_GLGEN_DUAL40_RCU_RPB_CMD_SRC_OFF                     (1)
#define PP_RPB_GLGEN_DUAL40_RCU_RPB_CMD_SRC_LEN                     (1)
#define PP_RPB_GLGEN_DUAL40_RCU_RPB_CMD_SRC_MSK                     (0x00000002)
#define PP_RPB_GLGEN_DUAL40_RCU_RPB_CMD_SRC_RST                     (0x0)
#define PP_RPB_GLGEN_DUAL40_DUAL_40G_MODE_OFF                       (0)
#define PP_RPB_GLGEN_DUAL40_DUAL_40G_MODE_LEN                       (1)
#define PP_RPB_GLGEN_DUAL40_DUAL_40G_MODE_MSK                       (0x00000001)
#define PP_RPB_GLGEN_DUAL40_DUAL_40G_MODE_RST                       (0x0)

/**
 * SW_REG_NAME : PP_RPB_GLPM_DMCTH_REG
 * HW_REG_NAME : glpm_dmcth_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:10][RO] - Reserved    Write 0, ignore on read.
 *   [ 9: 0][RW] - DMA Coalescing Receive Threshold    This value
 *                 defines the DMA coalescing Receive threshold in 1
 *                 Kilobyte units. When amount of data in  internal
 *                 receive buffer exceeds DMACRXT value, DMA
 *                 coalescing is stopped.    Notes:    1. If value is
 *                 0x0, condition to move out of DMA coalescing as a
 *                 result of passing DMA Coalescing Receive
 *                 Threshold is disabled.    2. Value written to the
 *                 field should take into account Latency tolerance
 *                 requirements (LTR) sent over  the PCIe and XOFF
 *                 receive threshold values, to avoid needless
 *                 generation of flow control packets when  in DMA
 *                 coalescing operating mode and flow control is
 *                 enabled.
 *
 */
#define PP_RPB_GLPM_DMCTH_REG                          ((RPB_BASE_ADDR) + 0x7E4)
#define PP_RPB_GLPM_DMCTH_RESERVED_2_OFF                            (10)
#define PP_RPB_GLPM_DMCTH_RESERVED_2_LEN                            (22)
#define PP_RPB_GLPM_DMCTH_RESERVED_2_MSK                            (0xFFFFFC00)
#define PP_RPB_GLPM_DMCTH_RESERVED_2_RST                            (0x0)
#define PP_RPB_GLPM_DMCTH_DMACRXT_OFF                               (0)
#define PP_RPB_GLPM_DMCTH_DMACRXT_LEN                               (10)
#define PP_RPB_GLPM_DMCTH_DMACRXT_MSK                               (0x000003FF)
#define PP_RPB_GLPM_DMCTH_DMACRXT_RST                               (0x0)

/**
 * SW_REG_NAME : PP_RPB_GLFCSTAT_REG
 * HW_REG_NAME : glrpb_fcstat_t
 * DESCRIPTION : Flow Control status, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31:16][RO] - Reserved.
 *   [15: 0][RW] - 0b = Flow Control was asserted. 1b = Flow Control
 *                 was not asserted. Write one to clear status
 *
 */
#define PP_RPB_GLFCSTAT_REG                            ((RPB_BASE_ADDR) + 0x7E8)
#define PP_RPB_GLFCSTAT_RSVD_OFF                                    (16)
#define PP_RPB_GLFCSTAT_RSVD_LEN                                    (16)
#define PP_RPB_GLFCSTAT_RSVD_MSK                                    (0xFFFF0000)
#define PP_RPB_GLFCSTAT_RSVD_RST                                    (0x0)
#define PP_RPB_GLFCSTAT_FC_STAT_OFF                                 (0)
#define PP_RPB_GLFCSTAT_FC_STAT_LEN                                 (16)
#define PP_RPB_GLFCSTAT_FC_STAT_MSK                                 (0x0000FFFF)
#define PP_RPB_GLFCSTAT_FC_STAT_RST                                 (0x0)

/**
 * SW_REG_NAME : PP_RPB_GLDHWS_REG
 * HW_REG_NAME : glrpb_dhws_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19: 0][RW] - Dedicated Pool High Watermark for the Single
 *                 shared buffer state, for all TCs of all
 *                 ports.andedsp;    It is expressed in bytes.
 *
 */
#define PP_RPB_GLDHWS_REG                              ((RPB_BASE_ADDR) + 0x820)
#define PP_RPB_GLDHWS_RESERVED_2_OFF                                (20)
#define PP_RPB_GLDHWS_RESERVED_2_LEN                                (12)
#define PP_RPB_GLDHWS_RESERVED_2_MSK                                (0xFFF00000)
#define PP_RPB_GLDHWS_RESERVED_2_RST                                (0x0)
#define PP_RPB_GLDHWS_DHW_TCN_OFF                                   (0)
#define PP_RPB_GLDHWS_DHW_TCN_LEN                                   (20)
#define PP_RPB_GLDHWS_DHW_TCN_MSK                                   (0x000FFFFF)
#define PP_RPB_GLDHWS_DHW_TCN_RST                                   (0x3ffff)

/**
 * SW_REG_NAME : PP_RPB_GLDLWS_REG
 * HW_REG_NAME : glrpb_dlws_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19: 0][RW] - Dedicated Pool Low Watermark for the Single shared
 *                 buffer state, for all TCs of all ports.    It is
 *                 expressed in bytes.
 *
 */
#define PP_RPB_GLDLWS_REG                              ((RPB_BASE_ADDR) + 0x824)
#define PP_RPB_GLDLWS_RESERVED_2_OFF                                (20)
#define PP_RPB_GLDLWS_RESERVED_2_LEN                                (12)
#define PP_RPB_GLDLWS_RESERVED_2_MSK                                (0xFFF00000)
#define PP_RPB_GLDLWS_RESERVED_2_RST                                (0x0)
#define PP_RPB_GLDLWS_DLW_TCN_OFF                                   (0)
#define PP_RPB_GLDLWS_DLW_TCN_LEN                                   (20)
#define PP_RPB_GLDLWS_DLW_TCN_MSK                                   (0x000FFFFF)
#define PP_RPB_GLDLWS_DLW_TCN_RST                                   (0x0)

/**
 * SW_REG_NAME : PP_RPB_GLDPSS_REG
 * HW_REG_NAME : glrpb_dpss_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19: 0][RW] - Dedicated Pool Size for the Single shared buffer
 *                 state, for all TCs of all ports.    It is
 *                 expressed in bytes.
 *
 */
#define PP_RPB_GLDPSS_REG                              ((RPB_BASE_ADDR) + 0x828)
#define PP_RPB_GLDPSS_RESERVED_2_OFF                                (20)
#define PP_RPB_GLDPSS_RESERVED_2_LEN                                (12)
#define PP_RPB_GLDPSS_RESERVED_2_MSK                                (0xFFF00000)
#define PP_RPB_GLDPSS_RESERVED_2_RST                                (0x0)
#define PP_RPB_GLDPSS_DPS_TCN_OFF                                   (0)
#define PP_RPB_GLDPSS_DPS_TCN_LEN                                   (20)
#define PP_RPB_GLDPSS_DPS_TCN_MSK                                   (0x000FFFFF)
#define PP_RPB_GLDPSS_DPS_TCN_RST                                   (0x0)

/**
 * SW_REG_NAME : PP_RPB_GLGFC_REG
 * HW_REG_NAME : glrpb_gfc_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19: 0][RO] - Global Filling Counter.    Total number of bytes
 *                 currently stored in the Rx packet buffer on the
 *                 account of all ports altogether.andedsp;
 *
 */
#define PP_RPB_GLGFC_REG                               ((RPB_BASE_ADDR) + 0x82C)
#define PP_RPB_GLGFC_RESERVED_2_OFF                                 (20)
#define PP_RPB_GLGFC_RESERVED_2_LEN                                 (12)
#define PP_RPB_GLGFC_RESERVED_2_MSK                                 (0xFFF00000)
#define PP_RPB_GLGFC_RESERVED_2_RST                                 (0x0)
#define PP_RPB_GLGFC_GFC_OFF                                        (0)
#define PP_RPB_GLGFC_GFC_LEN                                        (20)
#define PP_RPB_GLGFC_GFC_MSK                                        (0x000FFFFF)
#define PP_RPB_GLGFC_GFC_RST                                        (0x0)

/**
 * SW_REG_NAME : PP_RPB_GLGHW_REG
 * HW_REG_NAME : glrpb_ghw_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19: 0][RW] - Global High Watermark.    It is expressed in
 *                 bytes.
 *
 */
#define PP_RPB_GLGHW_REG                               ((RPB_BASE_ADDR) + 0x830)
#define PP_RPB_GLGHW_RESERVED_2_OFF                                 (20)
#define PP_RPB_GLGHW_RESERVED_2_LEN                                 (12)
#define PP_RPB_GLGHW_RESERVED_2_MSK                                 (0xFFF00000)
#define PP_RPB_GLGHW_RESERVED_2_RST                                 (0x0)
#define PP_RPB_GLGHW_GHW_OFF                                        (0)
#define PP_RPB_GLGHW_GHW_LEN                                        (20)
#define PP_RPB_GLGHW_GHW_MSK                                        (0x000FFFFF)
#define PP_RPB_GLGHW_GHW_RST                                        (0x3ffff)

/**
 * SW_REG_NAME : PP_RPB_GLGLW_REG
 * HW_REG_NAME : glrpb_glw_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19: 0][RW] - Global Low Watermark.    It is expressed in bytes.
 *
 */
#define PP_RPB_GLGLW_REG                               ((RPB_BASE_ADDR) + 0x834)
#define PP_RPB_GLGLW_RESERVED_2_OFF                                 (20)
#define PP_RPB_GLGLW_RESERVED_2_LEN                                 (12)
#define PP_RPB_GLGLW_RESERVED_2_MSK                                 (0xFFF00000)
#define PP_RPB_GLGLW_RESERVED_2_RST                                 (0x0)
#define PP_RPB_GLGLW_GLW_OFF                                        (0)
#define PP_RPB_GLGLW_GLW_LEN                                        (20)
#define PP_RPB_GLGLW_GLW_MSK                                        (0x000FFFFF)
#define PP_RPB_GLGLW_GLW_RST                                        (0x0)

/**
 * SW_REG_NAME : PP_RPB_GLGPC_REG
 * HW_REG_NAME : glrpb_gpc_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:14][RO] - Reserved
 *   [13: 0][RO] - Global Packet Counter.    Total number of packets
 *                 currently stored in the Rx packet buffer on the
 *                 account of all ports altogether.andedsp;
 *
 */
#define PP_RPB_GLGPC_REG                               ((RPB_BASE_ADDR) + 0x838)
#define PP_RPB_GLGPC_RESERVED_2_OFF                                 (14)
#define PP_RPB_GLGPC_RESERVED_2_LEN                                 (18)
#define PP_RPB_GLGPC_RESERVED_2_MSK                                 (0xFFFFC000)
#define PP_RPB_GLGPC_RESERVED_2_RST                                 (0x0)
#define PP_RPB_GLGPC_GPC_OFF                                        (0)
#define PP_RPB_GLGPC_GPC_LEN                                        (14)
#define PP_RPB_GLGPC_GPC_MSK                                        (0x00003FFF)
#define PP_RPB_GLGPC_GPC_RST                                        (0x0)

/**
 * SW_REG_NAME : PP_RPB_GLLTRTL_REG
 * HW_REG_NAME : glrpb_ltrtl_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:10][RO] - Reserved
 *   [ 9: 0][RW] - LTR Timer Load Value.    It is the time in us
 *                 units that the device shall wait before returning
 *                 to the normal setting of Rx  packet buffer since
 *                 it has exited the conditions for the DMA
 *                 Coalescing state.
 *
 */
#define PP_RPB_GLLTRTL_REG                             ((RPB_BASE_ADDR) + 0x83C)
#define PP_RPB_GLLTRTL_RESERVED_2_OFF                               (10)
#define PP_RPB_GLLTRTL_RESERVED_2_LEN                               (22)
#define PP_RPB_GLLTRTL_RESERVED_2_MSK                               (0xFFFFFC00)
#define PP_RPB_GLLTRTL_RESERVED_2_RST                               (0x0)
#define PP_RPB_GLLTRTL_LTRTL_OFF                                    (0)
#define PP_RPB_GLLTRTL_LTRTL_LEN                                    (10)
#define PP_RPB_GLLTRTL_LTRTL_MSK                                    (0x000003FF)
#define PP_RPB_GLLTRTL_LTRTL_RST                                    (0x0)

/**
 * SW_REG_NAME : PP_RPB_GLLTRTV_REG
 * HW_REG_NAME : glrpb_ltrtv_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:10][RO] - Reserved
 *   [ 9: 0][RO] - LTR Timer Value.    Current value of the LTR timer
 *                 expressed in us units.    This is a countdown
 *                 timer that saturates when it reaches zero.
 *
 */
#define PP_RPB_GLLTRTV_REG                             ((RPB_BASE_ADDR) + 0x840)
#define PP_RPB_GLLTRTV_RESERVED_2_OFF                               (10)
#define PP_RPB_GLLTRTV_RESERVED_2_LEN                               (22)
#define PP_RPB_GLLTRTV_RESERVED_2_MSK                               (0xFFFFFC00)
#define PP_RPB_GLLTRTV_RESERVED_2_RST                               (0x0)
#define PP_RPB_GLLTRTV_LTRTV_OFF                                    (0)
#define PP_RPB_GLLTRTV_LTRTV_LEN                                    (10)
#define PP_RPB_GLLTRTV_LTRTV_MSK                                    (0x000003FF)
#define PP_RPB_GLLTRTV_LTRTV_RST                                    (0x0)

/**
 * SW_REG_NAME : PP_RPB_GLPHW_REG
 * HW_REG_NAME : glrpb_phw_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19: 0][RW] - Packet High     Watermark.    It is relative to
 *                 the total number of packets stored in the RPB.
 *
 */
#define PP_RPB_GLPHW_REG                               ((RPB_BASE_ADDR) + 0x844)
#define PP_RPB_GLPHW_RESERVED_2_OFF                                 (20)
#define PP_RPB_GLPHW_RESERVED_2_LEN                                 (12)
#define PP_RPB_GLPHW_RESERVED_2_MSK                                 (0xFFF00000)
#define PP_RPB_GLPHW_RESERVED_2_RST                                 (0x0)
#define PP_RPB_GLPHW_PHW_OFF                                        (0)
#define PP_RPB_GLPHW_PHW_LEN                                        (20)
#define PP_RPB_GLPHW_PHW_MSK                                        (0x000FFFFF)
#define PP_RPB_GLPHW_PHW_RST                                        (0x4d6)

/**
 * SW_REG_NAME : PP_RPB_GLPLW_REG
 * HW_REG_NAME : glrpb_plw_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19: 0][RW] - Packet Low     Watermark.    It is relative to the
 *                 total number of packets stored in the RPB.
 *
 */
#define PP_RPB_GLPLW_REG                               ((RPB_BASE_ADDR) + 0x848)
#define PP_RPB_GLPLW_RESERVED_2_OFF                                 (20)
#define PP_RPB_GLPLW_RESERVED_2_LEN                                 (12)
#define PP_RPB_GLPLW_RESERVED_2_MSK                                 (0xFFF00000)
#define PP_RPB_GLPLW_RESERVED_2_RST                                 (0x0)
#define PP_RPB_GLPLW_PLW_OFF                                        (0)
#define PP_RPB_GLPLW_PLW_LEN                                        (20)
#define PP_RPB_GLPLW_PLW_MSK                                        (0x000FFFFF)
#define PP_RPB_GLPLW_PLW_RST                                        (0x230)

/**
 * SW_REG_NAME : PP_RPB_GLSHTS_REG
 * HW_REG_NAME : glrpb_shts_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19: 0][RW] - Shared Pool High Threshold for the Single shared
 *                 buffer state, for all TCs of all ports.andedsp;
 *                 It is expressed in bytes.
 *
 */
#define PP_RPB_GLSHTS_REG                              ((RPB_BASE_ADDR) + 0x84C)
#define PP_RPB_GLSHTS_RESERVED_2_OFF                                (20)
#define PP_RPB_GLSHTS_RESERVED_2_LEN                                (12)
#define PP_RPB_GLSHTS_RESERVED_2_MSK                                (0xFFF00000)
#define PP_RPB_GLSHTS_RESERVED_2_RST                                (0x0)
#define PP_RPB_GLSHTS_SHT_TCN_OFF                                   (0)
#define PP_RPB_GLSHTS_SHT_TCN_LEN                                   (20)
#define PP_RPB_GLSHTS_SHT_TCN_MSK                                   (0x000FFFFF)
#define PP_RPB_GLSHTS_SHT_TCN_RST                                   (0x3ffff)

/**
 * SW_REG_NAME : PP_RPB_GLSHWS_REG
 * HW_REG_NAME : glrpb_shws_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19: 0][RW] - Shared Pool High Watermark for the Single shared
 *                 buffer state.    It is expressed in bytes.
 *
 */
#define PP_RPB_GLSHWS_REG                              ((RPB_BASE_ADDR) + 0x850)
#define PP_RPB_GLSHWS_RESERVED_2_OFF                                (20)
#define PP_RPB_GLSHWS_RESERVED_2_LEN                                (12)
#define PP_RPB_GLSHWS_RESERVED_2_MSK                                (0xFFF00000)
#define PP_RPB_GLSHWS_RESERVED_2_RST                                (0x0)
#define PP_RPB_GLSHWS_SHW_OFF                                       (0)
#define PP_RPB_GLSHWS_SHW_LEN                                       (20)
#define PP_RPB_GLSHWS_SHW_MSK                                       (0x000FFFFF)
#define PP_RPB_GLSHWS_SHW_RST                                       (0x3ffff)

/**
 * SW_REG_NAME : PP_RPB_GLSLTS_REG
 * HW_REG_NAME : glrpb_slts_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19: 0][RW] - Shared Pool Low Threshold for the Single shared
 *                 buffer state, for all TCs of all Ports.andedsp;
 *                 It is expressed in bytes.
 *
 */
#define PP_RPB_GLSLTS_REG                              ((RPB_BASE_ADDR) + 0x854)
#define PP_RPB_GLSLTS_RESERVED_2_OFF                                (20)
#define PP_RPB_GLSLTS_RESERVED_2_LEN                                (12)
#define PP_RPB_GLSLTS_RESERVED_2_MSK                                (0xFFF00000)
#define PP_RPB_GLSLTS_RESERVED_2_RST                                (0x0)
#define PP_RPB_GLSLTS_SLT_TCN_OFF                                   (0)
#define PP_RPB_GLSLTS_SLT_TCN_LEN                                   (20)
#define PP_RPB_GLSLTS_SLT_TCN_MSK                                   (0x000FFFFF)
#define PP_RPB_GLSLTS_SLT_TCN_RST                                   (0x0)

/**
 * SW_REG_NAME : PP_RPB_GLSLWS_REG
 * HW_REG_NAME : glrpb_slws_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19: 0][RW] - Shared Pool Low Watermark for the Single shared
 *                 buffer state.    It is expressed in bytes.
 *
 */
#define PP_RPB_GLSLWS_REG                              ((RPB_BASE_ADDR) + 0x858)
#define PP_RPB_GLSLWS_RESERVED_2_OFF                                (20)
#define PP_RPB_GLSLWS_RESERVED_2_LEN                                (12)
#define PP_RPB_GLSLWS_RESERVED_2_MSK                                (0xFFF00000)
#define PP_RPB_GLSLWS_RESERVED_2_RST                                (0x0)
#define PP_RPB_GLSLWS_SLW_OFF                                       (0)
#define PP_RPB_GLSLWS_SLW_LEN                                       (20)
#define PP_RPB_GLSLWS_SLW_MSK                                       (0x000FFFFF)
#define PP_RPB_GLSLWS_SLW_RST                                       (0x0)

/**
 * SW_REG_NAME : PP_RPB_GLSPSS_REG
 * HW_REG_NAME : glrpb_spss_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19: 0][RW] - Shared Pool Size for Single shared buffer state.
 *                 Number of bytes allocated to the shared pool of
 *                 each port.
 *
 */
#define PP_RPB_GLSPSS_REG                              ((RPB_BASE_ADDR) + 0x85C)
#define PP_RPB_GLSPSS_RESERVED_2_OFF                                (20)
#define PP_RPB_GLSPSS_RESERVED_2_LEN                                (12)
#define PP_RPB_GLSPSS_RESERVED_2_MSK                                (0xFFF00000)
#define PP_RPB_GLSPSS_RESERVED_2_RST                                (0x0)
#define PP_RPB_GLSPSS_SPS_OFF                                       (0)
#define PP_RPB_GLSPSS_SPS_LEN                                       (20)
#define PP_RPB_GLSPSS_SPS_MSK                                       (0x000FFFFF)
#define PP_RPB_GLSPSS_SPS_RST                                       (0x3ffff)

/**
 * SW_REG_NAME : PP_RPB_CC_CNT_MEM_CFG_REG
 * HW_REG_NAME : rpb_cc_cnt_mem_cfg_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:20][RO] - MISSING_DESCRIPTION
 *   [19:16][RW] - Read-Write margin Input for port A. This input is
 *                 used for setting the Read-Write margin. It
 *                 programs  the sense amp differential setting. and
 *                 allows the trade off between speed and robustness.
 *                 RM[1:0] = 2'b00 is the slowest possible mode of
 *                 operation for the memory. This setting is required
 *                 for VDDMIN operation.    RM[1:0] values control
 *                 access time and cycle time of the memory. Refer to
 *                 the timing table for more  details.    RM[3:2] are
 *                 factory pins reserved for debug mode. User should
 *                 tie these pins to logic 0.
 *   [15:13][RO] - MISSING_DESCRIPTION
 *   [12:12][RW] - Read-Write margin Enable Input for port A. This
 *                 selects between the default RM setting, and the RM
 *                 settting.
 *   [11:10][RO] - MISSING_DESCRIPTION
 *   [ 9: 9][RW] - Count ECC un-correctable error events
 *   [ 8: 8][RW] - Count ECC correctable error events
 *   [ 7: 6][RO] - MISSING_DESCRIPTION
 *   [ 5: 5][RW] - Mask ECC un-correctable error interrupt
 *                 generation.
 *   [ 4: 4][RW] - Light Sleep mechanism bypass. '1' - prevent the
 *                 memory from entering light-sleep mode.
 *   [ 3: 3][RW] - Light Sleep Force    Force the entrence of the
 *                 memory to the Light Sleep mode.
 *   [ 2: 2][RW] - ECC Invert for testing
 *   [ 1: 1][RW] - ECC Invert for testing
 *   [ 0: 0][RW] - ECC mechanism enable. '1' - Enabled. '0' -
 *                 Disabled
 *
 */
#define PP_RPB_CC_CNT_MEM_CFG_REG                      ((RPB_BASE_ADDR) + 0x860)
#define PP_RPB_CC_CNT_MEM_CFG_RESERVED_14_OFF                       (20)
#define PP_RPB_CC_CNT_MEM_CFG_RESERVED_14_LEN                       (12)
#define PP_RPB_CC_CNT_MEM_CFG_RESERVED_14_MSK                       (0xFFF00000)
#define PP_RPB_CC_CNT_MEM_CFG_RESERVED_14_RST                       (0x0)
#define PP_RPB_CC_CNT_MEM_CFG_RM_OFF                                (16)
#define PP_RPB_CC_CNT_MEM_CFG_RM_LEN                                (4)
#define PP_RPB_CC_CNT_MEM_CFG_RM_MSK                                (0x000F0000)
#define PP_RPB_CC_CNT_MEM_CFG_RM_RST                                (0x2)
#define PP_RPB_CC_CNT_MEM_CFG_RESERVED_12_OFF                       (13)
#define PP_RPB_CC_CNT_MEM_CFG_RESERVED_12_LEN                       (3)
#define PP_RPB_CC_CNT_MEM_CFG_RESERVED_12_MSK                       (0x0000E000)
#define PP_RPB_CC_CNT_MEM_CFG_RESERVED_12_RST                       (0x0)
#define PP_RPB_CC_CNT_MEM_CFG_RME_OFF                               (12)
#define PP_RPB_CC_CNT_MEM_CFG_RME_LEN                               (1)
#define PP_RPB_CC_CNT_MEM_CFG_RME_MSK                               (0x00001000)
#define PP_RPB_CC_CNT_MEM_CFG_RME_RST                               (0x0)
#define PP_RPB_CC_CNT_MEM_CFG_RESERVED_10_OFF                       (10)
#define PP_RPB_CC_CNT_MEM_CFG_RESERVED_10_LEN                       (2)
#define PP_RPB_CC_CNT_MEM_CFG_RESERVED_10_MSK                       (0x00000C00)
#define PP_RPB_CC_CNT_MEM_CFG_RESERVED_10_RST                       (0x0)
#define PP_RPB_CC_CNT_MEM_CFG_ERR_CNT_OFF                           (9)
#define PP_RPB_CC_CNT_MEM_CFG_ERR_CNT_LEN                           (1)
#define PP_RPB_CC_CNT_MEM_CFG_ERR_CNT_MSK                           (0x00000200)
#define PP_RPB_CC_CNT_MEM_CFG_ERR_CNT_RST                           (0x1)
#define PP_RPB_CC_CNT_MEM_CFG_FIX_CNT_OFF                           (8)
#define PP_RPB_CC_CNT_MEM_CFG_FIX_CNT_LEN                           (1)
#define PP_RPB_CC_CNT_MEM_CFG_FIX_CNT_MSK                           (0x00000100)
#define PP_RPB_CC_CNT_MEM_CFG_FIX_CNT_RST                           (0x1)
#define PP_RPB_CC_CNT_MEM_CFG_RESERVED_7_OFF                        (6)
#define PP_RPB_CC_CNT_MEM_CFG_RESERVED_7_LEN                        (2)
#define PP_RPB_CC_CNT_MEM_CFG_RESERVED_7_MSK                        (0x000000C0)
#define PP_RPB_CC_CNT_MEM_CFG_RESERVED_7_RST                        (0x0)
#define PP_RPB_CC_CNT_MEM_CFG_MASK_INT_OFF                          (5)
#define PP_RPB_CC_CNT_MEM_CFG_MASK_INT_LEN                          (1)
#define PP_RPB_CC_CNT_MEM_CFG_MASK_INT_MSK                          (0x00000020)
#define PP_RPB_CC_CNT_MEM_CFG_MASK_INT_RST                          (0x0)
#define PP_RPB_CC_CNT_MEM_CFG_LS_BYPASS_OFF                         (4)
#define PP_RPB_CC_CNT_MEM_CFG_LS_BYPASS_LEN                         (1)
#define PP_RPB_CC_CNT_MEM_CFG_LS_BYPASS_MSK                         (0x00000010)
#define PP_RPB_CC_CNT_MEM_CFG_LS_BYPASS_RST                         (0x1)
#define PP_RPB_CC_CNT_MEM_CFG_LS_FORCE_OFF                          (3)
#define PP_RPB_CC_CNT_MEM_CFG_LS_FORCE_LEN                          (1)
#define PP_RPB_CC_CNT_MEM_CFG_LS_FORCE_MSK                          (0x00000008)
#define PP_RPB_CC_CNT_MEM_CFG_LS_FORCE_RST                          (0x0)
#define PP_RPB_CC_CNT_MEM_CFG_ECC_INVERT_2_OFF                      (2)
#define PP_RPB_CC_CNT_MEM_CFG_ECC_INVERT_2_LEN                      (1)
#define PP_RPB_CC_CNT_MEM_CFG_ECC_INVERT_2_MSK                      (0x00000004)
#define PP_RPB_CC_CNT_MEM_CFG_ECC_INVERT_2_RST                      (0x0)
#define PP_RPB_CC_CNT_MEM_CFG_ECC_INVERT_1_OFF                      (1)
#define PP_RPB_CC_CNT_MEM_CFG_ECC_INVERT_1_LEN                      (1)
#define PP_RPB_CC_CNT_MEM_CFG_ECC_INVERT_1_MSK                      (0x00000002)
#define PP_RPB_CC_CNT_MEM_CFG_ECC_INVERT_1_RST                      (0x0)
#define PP_RPB_CC_CNT_MEM_CFG_ECC_EN_OFF                            (0)
#define PP_RPB_CC_CNT_MEM_CFG_ECC_EN_LEN                            (1)
#define PP_RPB_CC_CNT_MEM_CFG_ECC_EN_MSK                            (0x00000001)
#define PP_RPB_CC_CNT_MEM_CFG_ECC_EN_RST                            (0x1)

/**
 * SW_REG_NAME : PP_RPB_CC_CNT_MEM_STATUS_REG
 * HW_REG_NAME : rpb_cc_cnt_mem_status_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 4][RO] - MISSING_DESCRIPTION
 *   [ 3: 3][RO] - All block's memories initialization completed
 *                 indication.
 *   [ 2: 2][RO] - Memory initialization completed indication.
 *   [ 1: 1][RO] - ECC correctable error indication (cleared on read)
 *   [ 0: 0][RO] - ECC un-correctable error indication (cleared on
 *                 read)
 *
 */
#define PP_RPB_CC_CNT_MEM_STATUS_REG                   ((RPB_BASE_ADDR) + 0x864)
#define PP_RPB_CC_CNT_MEM_STATUS_RESERVED_5_OFF                     (4)
#define PP_RPB_CC_CNT_MEM_STATUS_RESERVED_5_LEN                     (28)
#define PP_RPB_CC_CNT_MEM_STATUS_RESERVED_5_MSK                     (0xFFFFFFF0)
#define PP_RPB_CC_CNT_MEM_STATUS_RESERVED_5_RST                     (0x0)
#define PP_RPB_CC_CNT_MEM_STATUS_GLOBAL_INIT_DONE_OFF               (3)
#define PP_RPB_CC_CNT_MEM_STATUS_GLOBAL_INIT_DONE_LEN               (1)
#define PP_RPB_CC_CNT_MEM_STATUS_GLOBAL_INIT_DONE_MSK               (0x00000008)
#define PP_RPB_CC_CNT_MEM_STATUS_GLOBAL_INIT_DONE_RST               (0x0)
#define PP_RPB_CC_CNT_MEM_STATUS_INIT_DONE_OFF                      (2)
#define PP_RPB_CC_CNT_MEM_STATUS_INIT_DONE_LEN                      (1)
#define PP_RPB_CC_CNT_MEM_STATUS_INIT_DONE_MSK                      (0x00000004)
#define PP_RPB_CC_CNT_MEM_STATUS_INIT_DONE_RST                      (0x0)
#define PP_RPB_CC_CNT_MEM_STATUS_ECC_FIX_OFF                        (1)
#define PP_RPB_CC_CNT_MEM_STATUS_ECC_FIX_LEN                        (1)
#define PP_RPB_CC_CNT_MEM_STATUS_ECC_FIX_MSK                        (0x00000002)
#define PP_RPB_CC_CNT_MEM_STATUS_ECC_FIX_RST                        (0x0)
#define PP_RPB_CC_CNT_MEM_STATUS_ECC_ERR_OFF                        (0)
#define PP_RPB_CC_CNT_MEM_STATUS_ECC_ERR_LEN                        (1)
#define PP_RPB_CC_CNT_MEM_STATUS_ECC_ERR_MSK                        (0x00000001)
#define PP_RPB_CC_CNT_MEM_STATUS_ECC_ERR_RST                        (0x0)

/**
 * SW_REG_NAME : PP_RPB_PKT_MEM_CFG_REG
 * HW_REG_NAME : rpb_pkt_mem_cfg_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:20][RO] - MISSING_DESCRIPTION
 *   [19:16][RW] - Read-Write margin Input for port A. This input is
 *                 used for setting the Read-Write margin. It
 *                 programs  the sense amp differential setting. and
 *                 allows the trade off between speed and robustness.
 *                 RM[1:0] = 2'b00 is the slowest possible mode of
 *                 operation for the memory. This setting is required
 *                 for VDDMIN operation.    RM[1:0] values control
 *                 access time and cycle time of the memory. Refer to
 *                 the timing table for more  details.    RM[3:2] are
 *                 factory pins reserved for debug mode. User should
 *                 tie these pins to logic 0.
 *   [15:13][RO] - MISSING_DESCRIPTION
 *   [12:12][RW] - Read-Write margin Enable Input for port A. This
 *                 selects between the default RM setting, and the RM
 *                 settting.
 *   [11:10][RO] - MISSING_DESCRIPTION
 *   [ 9: 9][RW] - Count ECC un-correctable error events
 *   [ 8: 8][RW] - Count ECC correctable error events
 *   [ 7: 6][RO] - MISSING_DESCRIPTION
 *   [ 5: 5][RW] - Mask ECC un-correctable error interrupt
 *                 generation.
 *   [ 4: 4][RW] - Light Sleep mechanism bypass. '1' - prevent the
 *                 memory from entering light-sleep mode.
 *   [ 3: 3][RW] - Light Sleep Force    Force the entrence of the
 *                 memory to the Light Sleep mode.
 *   [ 2: 2][RW] - ECC Invert for testing
 *   [ 1: 1][RW] - ECC Invert for testing
 *   [ 0: 0][RW] - ECC mechanism enable. '1' - Enabled. '0' -
 *                 Disabled
 *
 */
#define PP_RPB_PKT_MEM_CFG_REG                         ((RPB_BASE_ADDR) + 0x868)
#define PP_RPB_PKT_MEM_CFG_RESERVED_14_OFF                          (20)
#define PP_RPB_PKT_MEM_CFG_RESERVED_14_LEN                          (12)
#define PP_RPB_PKT_MEM_CFG_RESERVED_14_MSK                          (0xFFF00000)
#define PP_RPB_PKT_MEM_CFG_RESERVED_14_RST                          (0x0)
#define PP_RPB_PKT_MEM_CFG_RM_OFF                                   (16)
#define PP_RPB_PKT_MEM_CFG_RM_LEN                                   (4)
#define PP_RPB_PKT_MEM_CFG_RM_MSK                                   (0x000F0000)
#define PP_RPB_PKT_MEM_CFG_RM_RST                                   (0x2)
#define PP_RPB_PKT_MEM_CFG_RESERVED_12_OFF                          (13)
#define PP_RPB_PKT_MEM_CFG_RESERVED_12_LEN                          (3)
#define PP_RPB_PKT_MEM_CFG_RESERVED_12_MSK                          (0x0000E000)
#define PP_RPB_PKT_MEM_CFG_RESERVED_12_RST                          (0x0)
#define PP_RPB_PKT_MEM_CFG_RME_OFF                                  (12)
#define PP_RPB_PKT_MEM_CFG_RME_LEN                                  (1)
#define PP_RPB_PKT_MEM_CFG_RME_MSK                                  (0x00001000)
#define PP_RPB_PKT_MEM_CFG_RME_RST                                  (0x0)
#define PP_RPB_PKT_MEM_CFG_RESERVED_10_OFF                          (10)
#define PP_RPB_PKT_MEM_CFG_RESERVED_10_LEN                          (2)
#define PP_RPB_PKT_MEM_CFG_RESERVED_10_MSK                          (0x00000C00)
#define PP_RPB_PKT_MEM_CFG_RESERVED_10_RST                          (0x0)
#define PP_RPB_PKT_MEM_CFG_ERR_CNT_OFF                              (9)
#define PP_RPB_PKT_MEM_CFG_ERR_CNT_LEN                              (1)
#define PP_RPB_PKT_MEM_CFG_ERR_CNT_MSK                              (0x00000200)
#define PP_RPB_PKT_MEM_CFG_ERR_CNT_RST                              (0x1)
#define PP_RPB_PKT_MEM_CFG_FIX_CNT_OFF                              (8)
#define PP_RPB_PKT_MEM_CFG_FIX_CNT_LEN                              (1)
#define PP_RPB_PKT_MEM_CFG_FIX_CNT_MSK                              (0x00000100)
#define PP_RPB_PKT_MEM_CFG_FIX_CNT_RST                              (0x1)
#define PP_RPB_PKT_MEM_CFG_RESERVED_7_OFF                           (6)
#define PP_RPB_PKT_MEM_CFG_RESERVED_7_LEN                           (2)
#define PP_RPB_PKT_MEM_CFG_RESERVED_7_MSK                           (0x000000C0)
#define PP_RPB_PKT_MEM_CFG_RESERVED_7_RST                           (0x0)
#define PP_RPB_PKT_MEM_CFG_MASK_INT_OFF                             (5)
#define PP_RPB_PKT_MEM_CFG_MASK_INT_LEN                             (1)
#define PP_RPB_PKT_MEM_CFG_MASK_INT_MSK                             (0x00000020)
#define PP_RPB_PKT_MEM_CFG_MASK_INT_RST                             (0x0)
#define PP_RPB_PKT_MEM_CFG_LS_BYPASS_OFF                            (4)
#define PP_RPB_PKT_MEM_CFG_LS_BYPASS_LEN                            (1)
#define PP_RPB_PKT_MEM_CFG_LS_BYPASS_MSK                            (0x00000010)
#define PP_RPB_PKT_MEM_CFG_LS_BYPASS_RST                            (0x1)
#define PP_RPB_PKT_MEM_CFG_LS_FORCE_OFF                             (3)
#define PP_RPB_PKT_MEM_CFG_LS_FORCE_LEN                             (1)
#define PP_RPB_PKT_MEM_CFG_LS_FORCE_MSK                             (0x00000008)
#define PP_RPB_PKT_MEM_CFG_LS_FORCE_RST                             (0x0)
#define PP_RPB_PKT_MEM_CFG_ECC_INVERT_2_OFF                         (2)
#define PP_RPB_PKT_MEM_CFG_ECC_INVERT_2_LEN                         (1)
#define PP_RPB_PKT_MEM_CFG_ECC_INVERT_2_MSK                         (0x00000004)
#define PP_RPB_PKT_MEM_CFG_ECC_INVERT_2_RST                         (0x0)
#define PP_RPB_PKT_MEM_CFG_ECC_INVERT_1_OFF                         (1)
#define PP_RPB_PKT_MEM_CFG_ECC_INVERT_1_LEN                         (1)
#define PP_RPB_PKT_MEM_CFG_ECC_INVERT_1_MSK                         (0x00000002)
#define PP_RPB_PKT_MEM_CFG_ECC_INVERT_1_RST                         (0x0)
#define PP_RPB_PKT_MEM_CFG_ECC_EN_OFF                               (0)
#define PP_RPB_PKT_MEM_CFG_ECC_EN_LEN                               (1)
#define PP_RPB_PKT_MEM_CFG_ECC_EN_MSK                               (0x00000001)
#define PP_RPB_PKT_MEM_CFG_ECC_EN_RST                               (0x1)

/**
 * SW_REG_NAME : PP_RPB_PKT_MEM_STATUS_REG
 * HW_REG_NAME : rpb_pkt_mem_status_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 4][RO] - MISSING_DESCRIPTION
 *   [ 3: 3][RO] - All block's memories initialization completed
 *                 indication.
 *   [ 2: 2][RO] - Memory initialization completed indication.
 *   [ 1: 1][RO] - ECC correctable error indication (cleared on read)
 *   [ 0: 0][RO] - ECC un-correctable error indication (cleared on
 *                 read)
 *
 */
#define PP_RPB_PKT_MEM_STATUS_REG                      ((RPB_BASE_ADDR) + 0x86C)
#define PP_RPB_PKT_MEM_STATUS_RESERVED_5_OFF                        (4)
#define PP_RPB_PKT_MEM_STATUS_RESERVED_5_LEN                        (28)
#define PP_RPB_PKT_MEM_STATUS_RESERVED_5_MSK                        (0xFFFFFFF0)
#define PP_RPB_PKT_MEM_STATUS_RESERVED_5_RST                        (0x0)
#define PP_RPB_PKT_MEM_STATUS_GLOBAL_INIT_DONE_OFF                  (3)
#define PP_RPB_PKT_MEM_STATUS_GLOBAL_INIT_DONE_LEN                  (1)
#define PP_RPB_PKT_MEM_STATUS_GLOBAL_INIT_DONE_MSK                  (0x00000008)
#define PP_RPB_PKT_MEM_STATUS_GLOBAL_INIT_DONE_RST                  (0x0)
#define PP_RPB_PKT_MEM_STATUS_INIT_DONE_OFF                         (2)
#define PP_RPB_PKT_MEM_STATUS_INIT_DONE_LEN                         (1)
#define PP_RPB_PKT_MEM_STATUS_INIT_DONE_MSK                         (0x00000004)
#define PP_RPB_PKT_MEM_STATUS_INIT_DONE_RST                         (0x0)
#define PP_RPB_PKT_MEM_STATUS_ECC_FIX_OFF                           (1)
#define PP_RPB_PKT_MEM_STATUS_ECC_FIX_LEN                           (1)
#define PP_RPB_PKT_MEM_STATUS_ECC_FIX_MSK                           (0x00000002)
#define PP_RPB_PKT_MEM_STATUS_ECC_FIX_RST                           (0x0)
#define PP_RPB_PKT_MEM_STATUS_ECC_ERR_OFF                           (0)
#define PP_RPB_PKT_MEM_STATUS_ECC_ERR_LEN                           (1)
#define PP_RPB_PKT_MEM_STATUS_ECC_ERR_MSK                           (0x00000001)
#define PP_RPB_PKT_MEM_STATUS_ECC_ERR_RST                           (0x0)

/**
 * SW_REG_NAME : PP_RPB_CLID_MEM_CFG_REG
 * HW_REG_NAME : rpb_clid_mem_cfg_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:20][RO] - MISSING_DESCRIPTION
 *   [19:16][RW] - Read-Write margin Input for port A. This input is
 *                 used for setting the Read-Write margin. It
 *                 programs  the sense amp differential setting. and
 *                 allows the trade off between speed and robustness.
 *                 RM[1:0] = 2'b00 is the slowest possible mode of
 *                 operation for the memory. This setting is required
 *                 for VDDMIN operation.    RM[1:0] values control
 *                 access time and cycle time of the memory. Refer to
 *                 the timing table for more  details.    RM[3:2] are
 *                 factory pins reserved for debug mode. User should
 *                 tie these pins to logic 0.
 *   [15:13][RO] - MISSING_DESCRIPTION
 *   [12:12][RW] - Read-Write margin Enable Input for port A. This
 *                 selects between the default RM setting, and the RM
 *                 settting.
 *   [11:10][RO] - MISSING_DESCRIPTION
 *   [ 9: 9][RW] - Count ECC un-correctable error events
 *   [ 8: 8][RW] - Count ECC correctable error events
 *   [ 7: 6][RO] - MISSING_DESCRIPTION
 *   [ 5: 5][RW] - Mask ECC un-correctable error interrupt
 *                 generation.
 *   [ 4: 4][RW] - Light Sleep mechanism bypass. '1' - prevent the
 *                 memory from entering light-sleep mode.
 *   [ 3: 3][RW] - Light Sleep Force    Force the entrence of the
 *                 memory to the Light Sleep mode.
 *   [ 2: 2][RW] - ECC Invert for testing
 *   [ 1: 1][RW] - ECC Invert for testing
 *   [ 0: 0][RW] - ECC mechanism enable. '1' - Enabled. '0' -
 *                 Disabled
 *
 */
#define PP_RPB_CLID_MEM_CFG_REG                        ((RPB_BASE_ADDR) + 0x870)
#define PP_RPB_CLID_MEM_CFG_RESERVED_14_OFF                         (20)
#define PP_RPB_CLID_MEM_CFG_RESERVED_14_LEN                         (12)
#define PP_RPB_CLID_MEM_CFG_RESERVED_14_MSK                         (0xFFF00000)
#define PP_RPB_CLID_MEM_CFG_RESERVED_14_RST                         (0x0)
#define PP_RPB_CLID_MEM_CFG_RM_OFF                                  (16)
#define PP_RPB_CLID_MEM_CFG_RM_LEN                                  (4)
#define PP_RPB_CLID_MEM_CFG_RM_MSK                                  (0x000F0000)
#define PP_RPB_CLID_MEM_CFG_RM_RST                                  (0x2)
#define PP_RPB_CLID_MEM_CFG_RESERVED_12_OFF                         (13)
#define PP_RPB_CLID_MEM_CFG_RESERVED_12_LEN                         (3)
#define PP_RPB_CLID_MEM_CFG_RESERVED_12_MSK                         (0x0000E000)
#define PP_RPB_CLID_MEM_CFG_RESERVED_12_RST                         (0x0)
#define PP_RPB_CLID_MEM_CFG_RME_OFF                                 (12)
#define PP_RPB_CLID_MEM_CFG_RME_LEN                                 (1)
#define PP_RPB_CLID_MEM_CFG_RME_MSK                                 (0x00001000)
#define PP_RPB_CLID_MEM_CFG_RME_RST                                 (0x0)
#define PP_RPB_CLID_MEM_CFG_RESERVED_10_OFF                         (10)
#define PP_RPB_CLID_MEM_CFG_RESERVED_10_LEN                         (2)
#define PP_RPB_CLID_MEM_CFG_RESERVED_10_MSK                         (0x00000C00)
#define PP_RPB_CLID_MEM_CFG_RESERVED_10_RST                         (0x0)
#define PP_RPB_CLID_MEM_CFG_ERR_CNT_OFF                             (9)
#define PP_RPB_CLID_MEM_CFG_ERR_CNT_LEN                             (1)
#define PP_RPB_CLID_MEM_CFG_ERR_CNT_MSK                             (0x00000200)
#define PP_RPB_CLID_MEM_CFG_ERR_CNT_RST                             (0x1)
#define PP_RPB_CLID_MEM_CFG_FIX_CNT_OFF                             (8)
#define PP_RPB_CLID_MEM_CFG_FIX_CNT_LEN                             (1)
#define PP_RPB_CLID_MEM_CFG_FIX_CNT_MSK                             (0x00000100)
#define PP_RPB_CLID_MEM_CFG_FIX_CNT_RST                             (0x1)
#define PP_RPB_CLID_MEM_CFG_RESERVED_7_OFF                          (6)
#define PP_RPB_CLID_MEM_CFG_RESERVED_7_LEN                          (2)
#define PP_RPB_CLID_MEM_CFG_RESERVED_7_MSK                          (0x000000C0)
#define PP_RPB_CLID_MEM_CFG_RESERVED_7_RST                          (0x0)
#define PP_RPB_CLID_MEM_CFG_MASK_INT_OFF                            (5)
#define PP_RPB_CLID_MEM_CFG_MASK_INT_LEN                            (1)
#define PP_RPB_CLID_MEM_CFG_MASK_INT_MSK                            (0x00000020)
#define PP_RPB_CLID_MEM_CFG_MASK_INT_RST                            (0x0)
#define PP_RPB_CLID_MEM_CFG_LS_BYPASS_OFF                           (4)
#define PP_RPB_CLID_MEM_CFG_LS_BYPASS_LEN                           (1)
#define PP_RPB_CLID_MEM_CFG_LS_BYPASS_MSK                           (0x00000010)
#define PP_RPB_CLID_MEM_CFG_LS_BYPASS_RST                           (0x1)
#define PP_RPB_CLID_MEM_CFG_LS_FORCE_OFF                            (3)
#define PP_RPB_CLID_MEM_CFG_LS_FORCE_LEN                            (1)
#define PP_RPB_CLID_MEM_CFG_LS_FORCE_MSK                            (0x00000008)
#define PP_RPB_CLID_MEM_CFG_LS_FORCE_RST                            (0x0)
#define PP_RPB_CLID_MEM_CFG_ECC_INVERT_2_OFF                        (2)
#define PP_RPB_CLID_MEM_CFG_ECC_INVERT_2_LEN                        (1)
#define PP_RPB_CLID_MEM_CFG_ECC_INVERT_2_MSK                        (0x00000004)
#define PP_RPB_CLID_MEM_CFG_ECC_INVERT_2_RST                        (0x0)
#define PP_RPB_CLID_MEM_CFG_ECC_INVERT_1_OFF                        (1)
#define PP_RPB_CLID_MEM_CFG_ECC_INVERT_1_LEN                        (1)
#define PP_RPB_CLID_MEM_CFG_ECC_INVERT_1_MSK                        (0x00000002)
#define PP_RPB_CLID_MEM_CFG_ECC_INVERT_1_RST                        (0x0)
#define PP_RPB_CLID_MEM_CFG_ECC_EN_OFF                              (0)
#define PP_RPB_CLID_MEM_CFG_ECC_EN_LEN                              (1)
#define PP_RPB_CLID_MEM_CFG_ECC_EN_MSK                              (0x00000001)
#define PP_RPB_CLID_MEM_CFG_ECC_EN_RST                              (0x1)

/**
 * SW_REG_NAME : PP_RPB_CLID_MEM_STATUS_REG
 * HW_REG_NAME : rpb_clid_mem_status_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 4][RO] - MISSING_DESCRIPTION
 *   [ 3: 3][RO] - All block's memories initialization completed
 *                 indication.
 *   [ 2: 2][RO] - Memory initialization completed indication.
 *   [ 1: 1][RO] - ECC correctable error indication (cleared on read)
 *   [ 0: 0][RO] - ECC un-correctable error indication (cleared on
 *                 read)
 *
 */
#define PP_RPB_CLID_MEM_STATUS_REG                     ((RPB_BASE_ADDR) + 0x874)
#define PP_RPB_CLID_MEM_STATUS_RESERVED_5_OFF                       (4)
#define PP_RPB_CLID_MEM_STATUS_RESERVED_5_LEN                       (28)
#define PP_RPB_CLID_MEM_STATUS_RESERVED_5_MSK                       (0xFFFFFFF0)
#define PP_RPB_CLID_MEM_STATUS_RESERVED_5_RST                       (0x0)
#define PP_RPB_CLID_MEM_STATUS_GLOBAL_INIT_DONE_OFF                 (3)
#define PP_RPB_CLID_MEM_STATUS_GLOBAL_INIT_DONE_LEN                 (1)
#define PP_RPB_CLID_MEM_STATUS_GLOBAL_INIT_DONE_MSK                 (0x00000008)
#define PP_RPB_CLID_MEM_STATUS_GLOBAL_INIT_DONE_RST                 (0x0)
#define PP_RPB_CLID_MEM_STATUS_INIT_DONE_OFF                        (2)
#define PP_RPB_CLID_MEM_STATUS_INIT_DONE_LEN                        (1)
#define PP_RPB_CLID_MEM_STATUS_INIT_DONE_MSK                        (0x00000004)
#define PP_RPB_CLID_MEM_STATUS_INIT_DONE_RST                        (0x0)
#define PP_RPB_CLID_MEM_STATUS_ECC_FIX_OFF                          (1)
#define PP_RPB_CLID_MEM_STATUS_ECC_FIX_LEN                          (1)
#define PP_RPB_CLID_MEM_STATUS_ECC_FIX_MSK                          (0x00000002)
#define PP_RPB_CLID_MEM_STATUS_ECC_FIX_RST                          (0x0)
#define PP_RPB_CLID_MEM_STATUS_ECC_ERR_OFF                          (0)
#define PP_RPB_CLID_MEM_STATUS_ECC_ERR_LEN                          (1)
#define PP_RPB_CLID_MEM_STATUS_ECC_ERR_MSK                          (0x00000001)
#define PP_RPB_CLID_MEM_STATUS_ECC_ERR_RST                          (0x0)

/**
 * SW_REG_NAME : PP_RPB_PPDB_MEM_CFG_REG
 * HW_REG_NAME : rpb_ppdb_mem_cfg_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:24][RO] - MISSING_DESCRIPTION
 *   [23:20][RW] - MISSING_DESCRIPTION
 *   [19:16][RW] - Read-Write margin Input for port A. This input is
 *                 used for setting the Read-Write margin. It
 *                 programs  the sense amp differential setting. and
 *                 allows the trade off between speed and robustness.
 *                 RM[1:0] = 2'b00 is the slowest possible mode of
 *                 operation for the memory. This setting is required
 *                 for VDDMIN operation.    RM[1:0] values control
 *                 access time and cycle time of the memory. Refer to
 *                 the timing table for more  details.    RM[3:2] are
 *                 factory pins reserved for debug mode. User should
 *                 tie these pins to logic 0.
 *   [15:14][RO] - MISSING_DESCRIPTION
 *   [13:13][RW] - MISSING_DESCRIPTION
 *   [12:12][RW] - Read-Write margin Enable Input for port A. This
 *                 selects between the default RM setting, and the RM
 *                 settting.
 *   [11:10][RO] - MISSING_DESCRIPTION
 *   [ 9: 9][RW] - Count ECC un-correctable error events
 *   [ 8: 8][RW] - Count ECC correctable error events
 *   [ 7: 6][RO] - MISSING_DESCRIPTION
 *   [ 5: 5][RW] - Mask ECC un-correctable error interrupt
 *                 generation.
 *   [ 4: 4][RW] - Light Sleep mechanism bypass. '1' - prevent the
 *                 memory from entering light-sleep mode.
 *   [ 3: 3][RW] - Light Sleep Force    Force the entrence of the
 *                 memory to the Light Sleep mode.
 *   [ 2: 2][RW] - ECC Invert for testing
 *   [ 1: 1][RW] - ECC Invert for testing
 *   [ 0: 0][RW] - ECC mechanism enable. '1' - Enabled. '0' -
 *                 Disabled
 *
 */
#define PP_RPB_PPDB_MEM_CFG_REG                        ((RPB_BASE_ADDR) + 0x878)
#define PP_RPB_PPDB_MEM_CFG_RESERVED_16_OFF                         (24)
#define PP_RPB_PPDB_MEM_CFG_RESERVED_16_LEN                         (8)
#define PP_RPB_PPDB_MEM_CFG_RESERVED_16_MSK                         (0xFF000000)
#define PP_RPB_PPDB_MEM_CFG_RESERVED_16_RST                         (0x0)
#define PP_RPB_PPDB_MEM_CFG_RM_B_OFF                                (20)
#define PP_RPB_PPDB_MEM_CFG_RM_B_LEN                                (4)
#define PP_RPB_PPDB_MEM_CFG_RM_B_MSK                                (0x00F00000)
#define PP_RPB_PPDB_MEM_CFG_RM_B_RST                                (0x2)
#define PP_RPB_PPDB_MEM_CFG_RM_A_OFF                                (16)
#define PP_RPB_PPDB_MEM_CFG_RM_A_LEN                                (4)
#define PP_RPB_PPDB_MEM_CFG_RM_A_MSK                                (0x000F0000)
#define PP_RPB_PPDB_MEM_CFG_RM_A_RST                                (0x2)
#define PP_RPB_PPDB_MEM_CFG_RESERVED_13_OFF                         (14)
#define PP_RPB_PPDB_MEM_CFG_RESERVED_13_LEN                         (2)
#define PP_RPB_PPDB_MEM_CFG_RESERVED_13_MSK                         (0x0000C000)
#define PP_RPB_PPDB_MEM_CFG_RESERVED_13_RST                         (0x0)
#define PP_RPB_PPDB_MEM_CFG_RME_B_OFF                               (13)
#define PP_RPB_PPDB_MEM_CFG_RME_B_LEN                               (1)
#define PP_RPB_PPDB_MEM_CFG_RME_B_MSK                               (0x00002000)
#define PP_RPB_PPDB_MEM_CFG_RME_B_RST                               (0x0)
#define PP_RPB_PPDB_MEM_CFG_RME_A_OFF                               (12)
#define PP_RPB_PPDB_MEM_CFG_RME_A_LEN                               (1)
#define PP_RPB_PPDB_MEM_CFG_RME_A_MSK                               (0x00001000)
#define PP_RPB_PPDB_MEM_CFG_RME_A_RST                               (0x0)
#define PP_RPB_PPDB_MEM_CFG_RESERVED_10_OFF                         (10)
#define PP_RPB_PPDB_MEM_CFG_RESERVED_10_LEN                         (2)
#define PP_RPB_PPDB_MEM_CFG_RESERVED_10_MSK                         (0x00000C00)
#define PP_RPB_PPDB_MEM_CFG_RESERVED_10_RST                         (0x0)
#define PP_RPB_PPDB_MEM_CFG_ERR_CNT_OFF                             (9)
#define PP_RPB_PPDB_MEM_CFG_ERR_CNT_LEN                             (1)
#define PP_RPB_PPDB_MEM_CFG_ERR_CNT_MSK                             (0x00000200)
#define PP_RPB_PPDB_MEM_CFG_ERR_CNT_RST                             (0x1)
#define PP_RPB_PPDB_MEM_CFG_FIX_CNT_OFF                             (8)
#define PP_RPB_PPDB_MEM_CFG_FIX_CNT_LEN                             (1)
#define PP_RPB_PPDB_MEM_CFG_FIX_CNT_MSK                             (0x00000100)
#define PP_RPB_PPDB_MEM_CFG_FIX_CNT_RST                             (0x1)
#define PP_RPB_PPDB_MEM_CFG_RESERVED_7_OFF                          (6)
#define PP_RPB_PPDB_MEM_CFG_RESERVED_7_LEN                          (2)
#define PP_RPB_PPDB_MEM_CFG_RESERVED_7_MSK                          (0x000000C0)
#define PP_RPB_PPDB_MEM_CFG_RESERVED_7_RST                          (0x0)
#define PP_RPB_PPDB_MEM_CFG_MASK_INT_OFF                            (5)
#define PP_RPB_PPDB_MEM_CFG_MASK_INT_LEN                            (1)
#define PP_RPB_PPDB_MEM_CFG_MASK_INT_MSK                            (0x00000020)
#define PP_RPB_PPDB_MEM_CFG_MASK_INT_RST                            (0x0)
#define PP_RPB_PPDB_MEM_CFG_LS_BYPASS_OFF                           (4)
#define PP_RPB_PPDB_MEM_CFG_LS_BYPASS_LEN                           (1)
#define PP_RPB_PPDB_MEM_CFG_LS_BYPASS_MSK                           (0x00000010)
#define PP_RPB_PPDB_MEM_CFG_LS_BYPASS_RST                           (0x1)
#define PP_RPB_PPDB_MEM_CFG_LS_FORCE_OFF                            (3)
#define PP_RPB_PPDB_MEM_CFG_LS_FORCE_LEN                            (1)
#define PP_RPB_PPDB_MEM_CFG_LS_FORCE_MSK                            (0x00000008)
#define PP_RPB_PPDB_MEM_CFG_LS_FORCE_RST                            (0x0)
#define PP_RPB_PPDB_MEM_CFG_ECC_INVERT_2_OFF                        (2)
#define PP_RPB_PPDB_MEM_CFG_ECC_INVERT_2_LEN                        (1)
#define PP_RPB_PPDB_MEM_CFG_ECC_INVERT_2_MSK                        (0x00000004)
#define PP_RPB_PPDB_MEM_CFG_ECC_INVERT_2_RST                        (0x0)
#define PP_RPB_PPDB_MEM_CFG_ECC_INVERT_1_OFF                        (1)
#define PP_RPB_PPDB_MEM_CFG_ECC_INVERT_1_LEN                        (1)
#define PP_RPB_PPDB_MEM_CFG_ECC_INVERT_1_MSK                        (0x00000002)
#define PP_RPB_PPDB_MEM_CFG_ECC_INVERT_1_RST                        (0x0)
#define PP_RPB_PPDB_MEM_CFG_ECC_EN_OFF                              (0)
#define PP_RPB_PPDB_MEM_CFG_ECC_EN_LEN                              (1)
#define PP_RPB_PPDB_MEM_CFG_ECC_EN_MSK                              (0x00000001)
#define PP_RPB_PPDB_MEM_CFG_ECC_EN_RST                              (0x1)

/**
 * SW_REG_NAME : PP_RPB_PPDB_MEM_STATUS_REG
 * HW_REG_NAME : rpb_ppdb_mem_status_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 4][RO] - MISSING_DESCRIPTION
 *   [ 3: 3][RO] - All block's memories initialization completed
 *                 indication.
 *   [ 2: 2][RO] - Memory initialization completed indication.
 *   [ 1: 1][RO] - ECC correctable error indication (cleared on read)
 *   [ 0: 0][RO] - ECC un-correctable error indication (cleared on
 *                 read)
 *
 */
#define PP_RPB_PPDB_MEM_STATUS_REG                     ((RPB_BASE_ADDR) + 0x87C)
#define PP_RPB_PPDB_MEM_STATUS_RESERVED_5_OFF                       (4)
#define PP_RPB_PPDB_MEM_STATUS_RESERVED_5_LEN                       (28)
#define PP_RPB_PPDB_MEM_STATUS_RESERVED_5_MSK                       (0xFFFFFFF0)
#define PP_RPB_PPDB_MEM_STATUS_RESERVED_5_RST                       (0x0)
#define PP_RPB_PPDB_MEM_STATUS_GLOBAL_INIT_DONE_OFF                 (3)
#define PP_RPB_PPDB_MEM_STATUS_GLOBAL_INIT_DONE_LEN                 (1)
#define PP_RPB_PPDB_MEM_STATUS_GLOBAL_INIT_DONE_MSK                 (0x00000008)
#define PP_RPB_PPDB_MEM_STATUS_GLOBAL_INIT_DONE_RST                 (0x0)
#define PP_RPB_PPDB_MEM_STATUS_INIT_DONE_OFF                        (2)
#define PP_RPB_PPDB_MEM_STATUS_INIT_DONE_LEN                        (1)
#define PP_RPB_PPDB_MEM_STATUS_INIT_DONE_MSK                        (0x00000004)
#define PP_RPB_PPDB_MEM_STATUS_INIT_DONE_RST                        (0x0)
#define PP_RPB_PPDB_MEM_STATUS_ECC_FIX_OFF                          (1)
#define PP_RPB_PPDB_MEM_STATUS_ECC_FIX_LEN                          (1)
#define PP_RPB_PPDB_MEM_STATUS_ECC_FIX_MSK                          (0x00000002)
#define PP_RPB_PPDB_MEM_STATUS_ECC_FIX_RST                          (0x0)
#define PP_RPB_PPDB_MEM_STATUS_ECC_ERR_OFF                          (0)
#define PP_RPB_PPDB_MEM_STATUS_ECC_ERR_LEN                          (1)
#define PP_RPB_PPDB_MEM_STATUS_ECC_ERR_MSK                          (0x00000001)
#define PP_RPB_PPDB_MEM_STATUS_ECC_ERR_RST                          (0x0)

/**
 * SW_REG_NAME : PP_RPB_REPORT_LL_MEM_CFG_REG
 * HW_REG_NAME : rpb_report_ll_mem_cfg_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:20][RO] - MISSING_DESCRIPTION
 *   [19:16][RW] - Read-Write margin Input for port A. This input is
 *                 used for setting the Read-Write margin. It
 *                 programs  the sense amp differential setting. and
 *                 allows the trade off between speed and robustness.
 *                 RM[1:0] = 2'b00 is the slowest possible mode of
 *                 operation for the memory. This setting is required
 *                 for VDDMIN operation.    RM[1:0] values control
 *                 access time and cycle time of the memory. Refer to
 *                 the timing table for more  details.    RM[3:2] are
 *                 factory pins reserved for debug mode. User should
 *                 tie these pins to logic 0.
 *   [15:13][RO] - MISSING_DESCRIPTION
 *   [12:12][RW] - Read-Write margin Enable Input for port A. This
 *                 selects between the default RM setting, and the RM
 *                 settting.
 *   [11:10][RO] - MISSING_DESCRIPTION
 *   [ 9: 9][RW] - Count ECC un-correctable error events
 *   [ 8: 8][RW] - Count ECC correctable error events
 *   [ 7: 6][RO] - MISSING_DESCRIPTION
 *   [ 5: 5][RW] - Mask ECC un-correctable error interrupt
 *                 generation.
 *   [ 4: 4][RW] - Light Sleep mechanism bypass. '1' - prevent the
 *                 memory from entering light-sleep mode.
 *   [ 3: 3][RW] - Light Sleep Force    Force the entrence of the
 *                 memory to the Light Sleep mode.
 *   [ 2: 2][RW] - ECC Invert for testing
 *   [ 1: 1][RW] - ECC Invert for testing
 *   [ 0: 0][RW] - ECC mechanism enable. '1' - Enabled. '0' -
 *                 Disabled
 *
 */
#define PP_RPB_REPORT_LL_MEM_CFG_REG                   ((RPB_BASE_ADDR) + 0x880)
#define PP_RPB_REPORT_LL_MEM_CFG_RESERVED_14_OFF                    (20)
#define PP_RPB_REPORT_LL_MEM_CFG_RESERVED_14_LEN                    (12)
#define PP_RPB_REPORT_LL_MEM_CFG_RESERVED_14_MSK                    (0xFFF00000)
#define PP_RPB_REPORT_LL_MEM_CFG_RESERVED_14_RST                    (0x0)
#define PP_RPB_REPORT_LL_MEM_CFG_RM_OFF                             (16)
#define PP_RPB_REPORT_LL_MEM_CFG_RM_LEN                             (4)
#define PP_RPB_REPORT_LL_MEM_CFG_RM_MSK                             (0x000F0000)
#define PP_RPB_REPORT_LL_MEM_CFG_RM_RST                             (0x2)
#define PP_RPB_REPORT_LL_MEM_CFG_RESERVED_12_OFF                    (13)
#define PP_RPB_REPORT_LL_MEM_CFG_RESERVED_12_LEN                    (3)
#define PP_RPB_REPORT_LL_MEM_CFG_RESERVED_12_MSK                    (0x0000E000)
#define PP_RPB_REPORT_LL_MEM_CFG_RESERVED_12_RST                    (0x0)
#define PP_RPB_REPORT_LL_MEM_CFG_RME_OFF                            (12)
#define PP_RPB_REPORT_LL_MEM_CFG_RME_LEN                            (1)
#define PP_RPB_REPORT_LL_MEM_CFG_RME_MSK                            (0x00001000)
#define PP_RPB_REPORT_LL_MEM_CFG_RME_RST                            (0x0)
#define PP_RPB_REPORT_LL_MEM_CFG_RESERVED_10_OFF                    (10)
#define PP_RPB_REPORT_LL_MEM_CFG_RESERVED_10_LEN                    (2)
#define PP_RPB_REPORT_LL_MEM_CFG_RESERVED_10_MSK                    (0x00000C00)
#define PP_RPB_REPORT_LL_MEM_CFG_RESERVED_10_RST                    (0x0)
#define PP_RPB_REPORT_LL_MEM_CFG_ERR_CNT_OFF                        (9)
#define PP_RPB_REPORT_LL_MEM_CFG_ERR_CNT_LEN                        (1)
#define PP_RPB_REPORT_LL_MEM_CFG_ERR_CNT_MSK                        (0x00000200)
#define PP_RPB_REPORT_LL_MEM_CFG_ERR_CNT_RST                        (0x1)
#define PP_RPB_REPORT_LL_MEM_CFG_FIX_CNT_OFF                        (8)
#define PP_RPB_REPORT_LL_MEM_CFG_FIX_CNT_LEN                        (1)
#define PP_RPB_REPORT_LL_MEM_CFG_FIX_CNT_MSK                        (0x00000100)
#define PP_RPB_REPORT_LL_MEM_CFG_FIX_CNT_RST                        (0x1)
#define PP_RPB_REPORT_LL_MEM_CFG_RESERVED_7_OFF                     (6)
#define PP_RPB_REPORT_LL_MEM_CFG_RESERVED_7_LEN                     (2)
#define PP_RPB_REPORT_LL_MEM_CFG_RESERVED_7_MSK                     (0x000000C0)
#define PP_RPB_REPORT_LL_MEM_CFG_RESERVED_7_RST                     (0x0)
#define PP_RPB_REPORT_LL_MEM_CFG_MASK_INT_OFF                       (5)
#define PP_RPB_REPORT_LL_MEM_CFG_MASK_INT_LEN                       (1)
#define PP_RPB_REPORT_LL_MEM_CFG_MASK_INT_MSK                       (0x00000020)
#define PP_RPB_REPORT_LL_MEM_CFG_MASK_INT_RST                       (0x0)
#define PP_RPB_REPORT_LL_MEM_CFG_LS_BYPASS_OFF                      (4)
#define PP_RPB_REPORT_LL_MEM_CFG_LS_BYPASS_LEN                      (1)
#define PP_RPB_REPORT_LL_MEM_CFG_LS_BYPASS_MSK                      (0x00000010)
#define PP_RPB_REPORT_LL_MEM_CFG_LS_BYPASS_RST                      (0x1)
#define PP_RPB_REPORT_LL_MEM_CFG_LS_FORCE_OFF                       (3)
#define PP_RPB_REPORT_LL_MEM_CFG_LS_FORCE_LEN                       (1)
#define PP_RPB_REPORT_LL_MEM_CFG_LS_FORCE_MSK                       (0x00000008)
#define PP_RPB_REPORT_LL_MEM_CFG_LS_FORCE_RST                       (0x0)
#define PP_RPB_REPORT_LL_MEM_CFG_ECC_INVERT_2_OFF                   (2)
#define PP_RPB_REPORT_LL_MEM_CFG_ECC_INVERT_2_LEN                   (1)
#define PP_RPB_REPORT_LL_MEM_CFG_ECC_INVERT_2_MSK                   (0x00000004)
#define PP_RPB_REPORT_LL_MEM_CFG_ECC_INVERT_2_RST                   (0x0)
#define PP_RPB_REPORT_LL_MEM_CFG_ECC_INVERT_1_OFF                   (1)
#define PP_RPB_REPORT_LL_MEM_CFG_ECC_INVERT_1_LEN                   (1)
#define PP_RPB_REPORT_LL_MEM_CFG_ECC_INVERT_1_MSK                   (0x00000002)
#define PP_RPB_REPORT_LL_MEM_CFG_ECC_INVERT_1_RST                   (0x0)
#define PP_RPB_REPORT_LL_MEM_CFG_ECC_EN_OFF                         (0)
#define PP_RPB_REPORT_LL_MEM_CFG_ECC_EN_LEN                         (1)
#define PP_RPB_REPORT_LL_MEM_CFG_ECC_EN_MSK                         (0x00000001)
#define PP_RPB_REPORT_LL_MEM_CFG_ECC_EN_RST                         (0x1)

/**
 * SW_REG_NAME : PP_RPB_REPORT_LL_MEM_STATUS_REG
 * HW_REG_NAME : rpb_report_ll_mem_status_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 4][RO] - MISSING_DESCRIPTION
 *   [ 3: 3][RO] - All block's memories initialization completed
 *                 indication.
 *   [ 2: 2][RO] - Memory initialization completed indication.
 *   [ 1: 1][RO] - ECC correctable error indication (cleared on read)
 *   [ 0: 0][RO] - ECC un-correctable error indication (cleared on
 *                 read)
 *
 */
#define PP_RPB_REPORT_LL_MEM_STATUS_REG                ((RPB_BASE_ADDR) + 0x884)
#define PP_RPB_REPORT_LL_MEM_STATUS_RESERVED_5_OFF                  (4)
#define PP_RPB_REPORT_LL_MEM_STATUS_RESERVED_5_LEN                  (28)
#define PP_RPB_REPORT_LL_MEM_STATUS_RESERVED_5_MSK                  (0xFFFFFFF0)
#define PP_RPB_REPORT_LL_MEM_STATUS_RESERVED_5_RST                  (0x0)
#define PP_RPB_REPORT_LL_MEM_STATUS_GLOBAL_INIT_DONE_OFF            (3)
#define PP_RPB_REPORT_LL_MEM_STATUS_GLOBAL_INIT_DONE_LEN            (1)
#define PP_RPB_REPORT_LL_MEM_STATUS_GLOBAL_INIT_DONE_MSK            (0x00000008)
#define PP_RPB_REPORT_LL_MEM_STATUS_GLOBAL_INIT_DONE_RST            (0x0)
#define PP_RPB_REPORT_LL_MEM_STATUS_INIT_DONE_OFF                   (2)
#define PP_RPB_REPORT_LL_MEM_STATUS_INIT_DONE_LEN                   (1)
#define PP_RPB_REPORT_LL_MEM_STATUS_INIT_DONE_MSK                   (0x00000004)
#define PP_RPB_REPORT_LL_MEM_STATUS_INIT_DONE_RST                   (0x0)
#define PP_RPB_REPORT_LL_MEM_STATUS_ECC_FIX_OFF                     (1)
#define PP_RPB_REPORT_LL_MEM_STATUS_ECC_FIX_LEN                     (1)
#define PP_RPB_REPORT_LL_MEM_STATUS_ECC_FIX_MSK                     (0x00000002)
#define PP_RPB_REPORT_LL_MEM_STATUS_ECC_FIX_RST                     (0x0)
#define PP_RPB_REPORT_LL_MEM_STATUS_ECC_ERR_OFF                     (0)
#define PP_RPB_REPORT_LL_MEM_STATUS_ECC_ERR_LEN                     (1)
#define PP_RPB_REPORT_LL_MEM_STATUS_ECC_ERR_MSK                     (0x00000001)
#define PP_RPB_REPORT_LL_MEM_STATUS_ECC_ERR_RST                     (0x0)

/**
 * SW_REG_NAME : PP_RPB_REPORT_MEM_CFG_REG
 * HW_REG_NAME : rpb_report_mem_cfg_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:20][RO] - MISSING_DESCRIPTION
 *   [19:16][RW] - Read-Write margin Input for port A. This input is
 *                 used for setting the Read-Write margin. It
 *                 programs  the sense amp differential setting. and
 *                 allows the trade off between speed and robustness.
 *                 RM[1:0] = 2'b00 is the slowest possible mode of
 *                 operation for the memory. This setting is required
 *                 for VDDMIN operation.    RM[1:0] values control
 *                 access time and cycle time of the memory. Refer to
 *                 the timing table for more  details.    RM[3:2] are
 *                 factory pins reserved for debug mode. User should
 *                 tie these pins to logic 0.
 *   [15:13][RO] - MISSING_DESCRIPTION
 *   [12:12][RW] - Read-Write margin Enable Input for port A. This
 *                 selects between the default RM setting, and the RM
 *                 settting.
 *   [11:10][RO] - MISSING_DESCRIPTION
 *   [ 9: 9][RW] - Count ECC un-correctable error events
 *   [ 8: 8][RW] - Count ECC correctable error events
 *   [ 7: 6][RO] - MISSING_DESCRIPTION
 *   [ 5: 5][RW] - Mask ECC un-correctable error interrupt
 *                 generation.
 *   [ 4: 4][RW] - Light Sleep mechanism bypass. '1' - prevent the
 *                 memory from entering light-sleep mode.
 *   [ 3: 3][RW] - Light Sleep Force    Force the entrence of the
 *                 memory to the Light Sleep mode.
 *   [ 2: 2][RW] - ECC Invert for testing
 *   [ 1: 1][RW] - ECC Invert for testing
 *   [ 0: 0][RW] - ECC mechanism enable. '1' - Enabled. '0' -
 *                 Disabled
 *
 */
#define PP_RPB_REPORT_MEM_CFG_REG                      ((RPB_BASE_ADDR) + 0x888)
#define PP_RPB_REPORT_MEM_CFG_RESERVED_14_OFF                       (20)
#define PP_RPB_REPORT_MEM_CFG_RESERVED_14_LEN                       (12)
#define PP_RPB_REPORT_MEM_CFG_RESERVED_14_MSK                       (0xFFF00000)
#define PP_RPB_REPORT_MEM_CFG_RESERVED_14_RST                       (0x0)
#define PP_RPB_REPORT_MEM_CFG_RM_OFF                                (16)
#define PP_RPB_REPORT_MEM_CFG_RM_LEN                                (4)
#define PP_RPB_REPORT_MEM_CFG_RM_MSK                                (0x000F0000)
#define PP_RPB_REPORT_MEM_CFG_RM_RST                                (0x2)
#define PP_RPB_REPORT_MEM_CFG_RESERVED_12_OFF                       (13)
#define PP_RPB_REPORT_MEM_CFG_RESERVED_12_LEN                       (3)
#define PP_RPB_REPORT_MEM_CFG_RESERVED_12_MSK                       (0x0000E000)
#define PP_RPB_REPORT_MEM_CFG_RESERVED_12_RST                       (0x0)
#define PP_RPB_REPORT_MEM_CFG_RME_OFF                               (12)
#define PP_RPB_REPORT_MEM_CFG_RME_LEN                               (1)
#define PP_RPB_REPORT_MEM_CFG_RME_MSK                               (0x00001000)
#define PP_RPB_REPORT_MEM_CFG_RME_RST                               (0x0)
#define PP_RPB_REPORT_MEM_CFG_RESERVED_10_OFF                       (10)
#define PP_RPB_REPORT_MEM_CFG_RESERVED_10_LEN                       (2)
#define PP_RPB_REPORT_MEM_CFG_RESERVED_10_MSK                       (0x00000C00)
#define PP_RPB_REPORT_MEM_CFG_RESERVED_10_RST                       (0x0)
#define PP_RPB_REPORT_MEM_CFG_ERR_CNT_OFF                           (9)
#define PP_RPB_REPORT_MEM_CFG_ERR_CNT_LEN                           (1)
#define PP_RPB_REPORT_MEM_CFG_ERR_CNT_MSK                           (0x00000200)
#define PP_RPB_REPORT_MEM_CFG_ERR_CNT_RST                           (0x1)
#define PP_RPB_REPORT_MEM_CFG_FIX_CNT_OFF                           (8)
#define PP_RPB_REPORT_MEM_CFG_FIX_CNT_LEN                           (1)
#define PP_RPB_REPORT_MEM_CFG_FIX_CNT_MSK                           (0x00000100)
#define PP_RPB_REPORT_MEM_CFG_FIX_CNT_RST                           (0x1)
#define PP_RPB_REPORT_MEM_CFG_RESERVED_7_OFF                        (6)
#define PP_RPB_REPORT_MEM_CFG_RESERVED_7_LEN                        (2)
#define PP_RPB_REPORT_MEM_CFG_RESERVED_7_MSK                        (0x000000C0)
#define PP_RPB_REPORT_MEM_CFG_RESERVED_7_RST                        (0x0)
#define PP_RPB_REPORT_MEM_CFG_MASK_INT_OFF                          (5)
#define PP_RPB_REPORT_MEM_CFG_MASK_INT_LEN                          (1)
#define PP_RPB_REPORT_MEM_CFG_MASK_INT_MSK                          (0x00000020)
#define PP_RPB_REPORT_MEM_CFG_MASK_INT_RST                          (0x0)
#define PP_RPB_REPORT_MEM_CFG_LS_BYPASS_OFF                         (4)
#define PP_RPB_REPORT_MEM_CFG_LS_BYPASS_LEN                         (1)
#define PP_RPB_REPORT_MEM_CFG_LS_BYPASS_MSK                         (0x00000010)
#define PP_RPB_REPORT_MEM_CFG_LS_BYPASS_RST                         (0x1)
#define PP_RPB_REPORT_MEM_CFG_LS_FORCE_OFF                          (3)
#define PP_RPB_REPORT_MEM_CFG_LS_FORCE_LEN                          (1)
#define PP_RPB_REPORT_MEM_CFG_LS_FORCE_MSK                          (0x00000008)
#define PP_RPB_REPORT_MEM_CFG_LS_FORCE_RST                          (0x0)
#define PP_RPB_REPORT_MEM_CFG_ECC_INVERT_2_OFF                      (2)
#define PP_RPB_REPORT_MEM_CFG_ECC_INVERT_2_LEN                      (1)
#define PP_RPB_REPORT_MEM_CFG_ECC_INVERT_2_MSK                      (0x00000004)
#define PP_RPB_REPORT_MEM_CFG_ECC_INVERT_2_RST                      (0x0)
#define PP_RPB_REPORT_MEM_CFG_ECC_INVERT_1_OFF                      (1)
#define PP_RPB_REPORT_MEM_CFG_ECC_INVERT_1_LEN                      (1)
#define PP_RPB_REPORT_MEM_CFG_ECC_INVERT_1_MSK                      (0x00000002)
#define PP_RPB_REPORT_MEM_CFG_ECC_INVERT_1_RST                      (0x0)
#define PP_RPB_REPORT_MEM_CFG_ECC_EN_OFF                            (0)
#define PP_RPB_REPORT_MEM_CFG_ECC_EN_LEN                            (1)
#define PP_RPB_REPORT_MEM_CFG_ECC_EN_MSK                            (0x00000001)
#define PP_RPB_REPORT_MEM_CFG_ECC_EN_RST                            (0x1)

/**
 * SW_REG_NAME : PP_RPB_REPORT_MEM_STATUS_REG
 * HW_REG_NAME : rpb_report_mem_status_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 4][RO] - MISSING_DESCRIPTION
 *   [ 3: 3][RO] - All block's memories initialization completed
 *                 indication.
 *   [ 2: 2][RO] - Memory initialization completed indication.
 *   [ 1: 1][RO] - ECC correctable error indication (cleared on read)
 *   [ 0: 0][RO] - ECC un-correctable error indication (cleared on
 *                 read)
 *
 */
#define PP_RPB_REPORT_MEM_STATUS_REG                   ((RPB_BASE_ADDR) + 0x88C)
#define PP_RPB_REPORT_MEM_STATUS_RESERVED_5_OFF                     (4)
#define PP_RPB_REPORT_MEM_STATUS_RESERVED_5_LEN                     (28)
#define PP_RPB_REPORT_MEM_STATUS_RESERVED_5_MSK                     (0xFFFFFFF0)
#define PP_RPB_REPORT_MEM_STATUS_RESERVED_5_RST                     (0x0)
#define PP_RPB_REPORT_MEM_STATUS_GLOBAL_INIT_DONE_OFF               (3)
#define PP_RPB_REPORT_MEM_STATUS_GLOBAL_INIT_DONE_LEN               (1)
#define PP_RPB_REPORT_MEM_STATUS_GLOBAL_INIT_DONE_MSK               (0x00000008)
#define PP_RPB_REPORT_MEM_STATUS_GLOBAL_INIT_DONE_RST               (0x0)
#define PP_RPB_REPORT_MEM_STATUS_INIT_DONE_OFF                      (2)
#define PP_RPB_REPORT_MEM_STATUS_INIT_DONE_LEN                      (1)
#define PP_RPB_REPORT_MEM_STATUS_INIT_DONE_MSK                      (0x00000004)
#define PP_RPB_REPORT_MEM_STATUS_INIT_DONE_RST                      (0x0)
#define PP_RPB_REPORT_MEM_STATUS_ECC_FIX_OFF                        (1)
#define PP_RPB_REPORT_MEM_STATUS_ECC_FIX_LEN                        (1)
#define PP_RPB_REPORT_MEM_STATUS_ECC_FIX_MSK                        (0x00000002)
#define PP_RPB_REPORT_MEM_STATUS_ECC_FIX_RST                        (0x0)
#define PP_RPB_REPORT_MEM_STATUS_ECC_ERR_OFF                        (0)
#define PP_RPB_REPORT_MEM_STATUS_ECC_ERR_LEN                        (1)
#define PP_RPB_REPORT_MEM_STATUS_ECC_ERR_MSK                        (0x00000001)
#define PP_RPB_REPORT_MEM_STATUS_ECC_ERR_RST                        (0x0)

/**
 * SW_REG_NAME : PP_RPB_CC_MEM_CFG_REG
 * HW_REG_NAME : rpb_cc_mem_cfg_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:20][RO] - MISSING_DESCRIPTION
 *   [19:16][RW] - Read-Write margin Input for port A. This input is
 *                 used for setting the Read-Write margin. It
 *                 programs  the sense amp differential setting. and
 *                 allows the trade off between speed and robustness.
 *                 RM[1:0] = 2'b00 is the slowest possible mode of
 *                 operation for the memory. This setting is required
 *                 for VDDMIN operation.    RM[1:0] values control
 *                 access time and cycle time of the memory. Refer to
 *                 the timing table for more  details.    RM[3:2] are
 *                 factory pins reserved for debug mode. User should
 *                 tie these pins to logic 0.
 *   [15:13][RO] - MISSING_DESCRIPTION
 *   [12:12][RW] - Read-Write margin Enable Input for port A. This
 *                 selects between the default RM setting, and the RM
 *                 settting.
 *   [11:10][RO] - MISSING_DESCRIPTION
 *   [ 9: 9][RW] - Count ECC un-correctable error events
 *   [ 8: 8][RW] - Count ECC correctable error events
 *   [ 7: 6][RO] - MISSING_DESCRIPTION
 *   [ 5: 5][RW] - Mask ECC un-correctable error interrupt
 *                 generation.
 *   [ 4: 4][RW] - Light Sleep mechanism bypass. '1' - prevent the
 *                 memory from entering light-sleep mode.
 *   [ 3: 3][RW] - Light Sleep Force    Force the entrence of the
 *                 memory to the Light Sleep mode.
 *   [ 2: 2][RW] - ECC Invert for testing
 *   [ 1: 1][RW] - ECC Invert for testing
 *   [ 0: 0][RW] - ECC mechanism enable. '1' - Enabled. '0' -
 *                 Disabled
 *
 */
#define PP_RPB_CC_MEM_CFG_REG                          ((RPB_BASE_ADDR) + 0x890)
#define PP_RPB_CC_MEM_CFG_RESERVED_14_OFF                           (20)
#define PP_RPB_CC_MEM_CFG_RESERVED_14_LEN                           (12)
#define PP_RPB_CC_MEM_CFG_RESERVED_14_MSK                           (0xFFF00000)
#define PP_RPB_CC_MEM_CFG_RESERVED_14_RST                           (0x0)
#define PP_RPB_CC_MEM_CFG_RM_OFF                                    (16)
#define PP_RPB_CC_MEM_CFG_RM_LEN                                    (4)
#define PP_RPB_CC_MEM_CFG_RM_MSK                                    (0x000F0000)
#define PP_RPB_CC_MEM_CFG_RM_RST                                    (0x2)
#define PP_RPB_CC_MEM_CFG_RESERVED_12_OFF                           (13)
#define PP_RPB_CC_MEM_CFG_RESERVED_12_LEN                           (3)
#define PP_RPB_CC_MEM_CFG_RESERVED_12_MSK                           (0x0000E000)
#define PP_RPB_CC_MEM_CFG_RESERVED_12_RST                           (0x0)
#define PP_RPB_CC_MEM_CFG_RME_OFF                                   (12)
#define PP_RPB_CC_MEM_CFG_RME_LEN                                   (1)
#define PP_RPB_CC_MEM_CFG_RME_MSK                                   (0x00001000)
#define PP_RPB_CC_MEM_CFG_RME_RST                                   (0x0)
#define PP_RPB_CC_MEM_CFG_RESERVED_10_OFF                           (10)
#define PP_RPB_CC_MEM_CFG_RESERVED_10_LEN                           (2)
#define PP_RPB_CC_MEM_CFG_RESERVED_10_MSK                           (0x00000C00)
#define PP_RPB_CC_MEM_CFG_RESERVED_10_RST                           (0x0)
#define PP_RPB_CC_MEM_CFG_ERR_CNT_OFF                               (9)
#define PP_RPB_CC_MEM_CFG_ERR_CNT_LEN                               (1)
#define PP_RPB_CC_MEM_CFG_ERR_CNT_MSK                               (0x00000200)
#define PP_RPB_CC_MEM_CFG_ERR_CNT_RST                               (0x1)
#define PP_RPB_CC_MEM_CFG_FIX_CNT_OFF                               (8)
#define PP_RPB_CC_MEM_CFG_FIX_CNT_LEN                               (1)
#define PP_RPB_CC_MEM_CFG_FIX_CNT_MSK                               (0x00000100)
#define PP_RPB_CC_MEM_CFG_FIX_CNT_RST                               (0x1)
#define PP_RPB_CC_MEM_CFG_RESERVED_7_OFF                            (6)
#define PP_RPB_CC_MEM_CFG_RESERVED_7_LEN                            (2)
#define PP_RPB_CC_MEM_CFG_RESERVED_7_MSK                            (0x000000C0)
#define PP_RPB_CC_MEM_CFG_RESERVED_7_RST                            (0x0)
#define PP_RPB_CC_MEM_CFG_MASK_INT_OFF                              (5)
#define PP_RPB_CC_MEM_CFG_MASK_INT_LEN                              (1)
#define PP_RPB_CC_MEM_CFG_MASK_INT_MSK                              (0x00000020)
#define PP_RPB_CC_MEM_CFG_MASK_INT_RST                              (0x0)
#define PP_RPB_CC_MEM_CFG_LS_BYPASS_OFF                             (4)
#define PP_RPB_CC_MEM_CFG_LS_BYPASS_LEN                             (1)
#define PP_RPB_CC_MEM_CFG_LS_BYPASS_MSK                             (0x00000010)
#define PP_RPB_CC_MEM_CFG_LS_BYPASS_RST                             (0x1)
#define PP_RPB_CC_MEM_CFG_LS_FORCE_OFF                              (3)
#define PP_RPB_CC_MEM_CFG_LS_FORCE_LEN                              (1)
#define PP_RPB_CC_MEM_CFG_LS_FORCE_MSK                              (0x00000008)
#define PP_RPB_CC_MEM_CFG_LS_FORCE_RST                              (0x0)
#define PP_RPB_CC_MEM_CFG_ECC_INVERT_2_OFF                          (2)
#define PP_RPB_CC_MEM_CFG_ECC_INVERT_2_LEN                          (1)
#define PP_RPB_CC_MEM_CFG_ECC_INVERT_2_MSK                          (0x00000004)
#define PP_RPB_CC_MEM_CFG_ECC_INVERT_2_RST                          (0x0)
#define PP_RPB_CC_MEM_CFG_ECC_INVERT_1_OFF                          (1)
#define PP_RPB_CC_MEM_CFG_ECC_INVERT_1_LEN                          (1)
#define PP_RPB_CC_MEM_CFG_ECC_INVERT_1_MSK                          (0x00000002)
#define PP_RPB_CC_MEM_CFG_ECC_INVERT_1_RST                          (0x0)
#define PP_RPB_CC_MEM_CFG_ECC_EN_OFF                                (0)
#define PP_RPB_CC_MEM_CFG_ECC_EN_LEN                                (1)
#define PP_RPB_CC_MEM_CFG_ECC_EN_MSK                                (0x00000001)
#define PP_RPB_CC_MEM_CFG_ECC_EN_RST                                (0x1)

/**
 * SW_REG_NAME : PP_RPB_CC_MEM_STATUS_REG
 * HW_REG_NAME : rpb_cc_mem_status_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 4][RO] - MISSING_DESCRIPTION
 *   [ 3: 3][RO] - All block's memories initialization completed
 *                 indication.
 *   [ 2: 2][RO] - Memory initialization completed indication.
 *   [ 1: 1][RO] - ECC correctable error indication (cleared on read)
 *   [ 0: 0][RO] - ECC un-correctable error indication (cleared on
 *                 read)
 *
 */
#define PP_RPB_CC_MEM_STATUS_REG                       ((RPB_BASE_ADDR) + 0x894)
#define PP_RPB_CC_MEM_STATUS_RESERVED_5_OFF                         (4)
#define PP_RPB_CC_MEM_STATUS_RESERVED_5_LEN                         (28)
#define PP_RPB_CC_MEM_STATUS_RESERVED_5_MSK                         (0xFFFFFFF0)
#define PP_RPB_CC_MEM_STATUS_RESERVED_5_RST                         (0x0)
#define PP_RPB_CC_MEM_STATUS_GLOBAL_INIT_DONE_OFF                   (3)
#define PP_RPB_CC_MEM_STATUS_GLOBAL_INIT_DONE_LEN                   (1)
#define PP_RPB_CC_MEM_STATUS_GLOBAL_INIT_DONE_MSK                   (0x00000008)
#define PP_RPB_CC_MEM_STATUS_GLOBAL_INIT_DONE_RST                   (0x0)
#define PP_RPB_CC_MEM_STATUS_INIT_DONE_OFF                          (2)
#define PP_RPB_CC_MEM_STATUS_INIT_DONE_LEN                          (1)
#define PP_RPB_CC_MEM_STATUS_INIT_DONE_MSK                          (0x00000004)
#define PP_RPB_CC_MEM_STATUS_INIT_DONE_RST                          (0x0)
#define PP_RPB_CC_MEM_STATUS_ECC_FIX_OFF                            (1)
#define PP_RPB_CC_MEM_STATUS_ECC_FIX_LEN                            (1)
#define PP_RPB_CC_MEM_STATUS_ECC_FIX_MSK                            (0x00000002)
#define PP_RPB_CC_MEM_STATUS_ECC_FIX_RST                            (0x0)
#define PP_RPB_CC_MEM_STATUS_ECC_ERR_OFF                            (0)
#define PP_RPB_CC_MEM_STATUS_ECC_ERR_LEN                            (1)
#define PP_RPB_CC_MEM_STATUS_ECC_ERR_MSK                            (0x00000001)
#define PP_RPB_CC_MEM_STATUS_ECC_ERR_RST                            (0x0)

/**
 * SW_REG_NAME : PP_RPB_ECC_UNCOR_ERR_REG
 * HW_REG_NAME : rpb_ecc_uncor_err_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:12][RO] - RESERVED
 *   [11: 0][RO] - Counter
 *
 */
#define PP_RPB_ECC_UNCOR_ERR_REG                       ((RPB_BASE_ADDR) + 0x8D8)
#define PP_RPB_ECC_UNCOR_ERR_RESERVED_2_OFF                         (12)
#define PP_RPB_ECC_UNCOR_ERR_RESERVED_2_LEN                         (20)
#define PP_RPB_ECC_UNCOR_ERR_RESERVED_2_MSK                         (0xFFFFF000)
#define PP_RPB_ECC_UNCOR_ERR_RESERVED_2_RST                         (0x0)
#define PP_RPB_ECC_UNCOR_ERR_CNT_OFF                                (0)
#define PP_RPB_ECC_UNCOR_ERR_CNT_LEN                                (12)
#define PP_RPB_ECC_UNCOR_ERR_CNT_MSK                                (0x00000FFF)
#define PP_RPB_ECC_UNCOR_ERR_CNT_RST                                (0x0)

/**
 * SW_REG_NAME : PP_RPB_ECC_COR_ERR_REG
 * HW_REG_NAME : rpb_ecc_cor_err_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:12][RO] - RESERVED
 *   [11: 0][RO] - Counter
 *
 */
#define PP_RPB_ECC_COR_ERR_REG                         ((RPB_BASE_ADDR) + 0x8DC)
#define PP_RPB_ECC_COR_ERR_RESERVED_2_OFF                           (12)
#define PP_RPB_ECC_COR_ERR_RESERVED_2_LEN                           (20)
#define PP_RPB_ECC_COR_ERR_RESERVED_2_MSK                           (0xFFFFF000)
#define PP_RPB_ECC_COR_ERR_RESERVED_2_RST                           (0x0)
#define PP_RPB_ECC_COR_ERR_CNT_OFF                                  (0)
#define PP_RPB_ECC_COR_ERR_CNT_LEN                                  (12)
#define PP_RPB_ECC_COR_ERR_CNT_MSK                                  (0x00000FFF)
#define PP_RPB_ECC_COR_ERR_CNT_RST                                  (0x0)

/**
 * SW_REG_NAME : PP_RPB_DBG_ACC_CNT_REG
 * HW_REG_NAME : rpb_dbg_acc_cnt_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:16][RO] - MISSING_DESCRIPTION
 *   [15: 0][RO] - Amount of indirect access read accesses made. This
 *                 counter shall reset on read.
 *
 */
#define PP_RPB_DBG_ACC_CNT_REG                         ((RPB_BASE_ADDR) + 0x8E0)
#define PP_RPB_DBG_ACC_CNT_RSVD_2_OFF                               (16)
#define PP_RPB_DBG_ACC_CNT_RSVD_2_LEN                               (16)
#define PP_RPB_DBG_ACC_CNT_RSVD_2_MSK                               (0xFFFF0000)
#define PP_RPB_DBG_ACC_CNT_RSVD_2_RST                               (0x0)
#define PP_RPB_DBG_ACC_CNT_OFF                                      (0)
#define PP_RPB_DBG_ACC_CNT_LEN                                      (16)
#define PP_RPB_DBG_ACC_CNT_MSK                                      (0x0000FFFF)
#define PP_RPB_DBG_ACC_CNT_RST                                      (0x0)

/**
 * SW_REG_NAME : PP_RPB_DBG_ACC_CTL_REG
 * HW_REG_NAME : rpb_dbg_acc_ctl_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:21][RO] - MISSING_DESCRIPTION
 *   [20:20][RO] - Execute read command.. This bit will be cleared on
 *                 write
 *   [19:16][RW] - Memory select:    4'd0 : Report occupancy counters
 *                 (address width is 5 bits, data width is 13 bits).
 *                 4'd1andedsp;andedsp;andedsp;: UP report FIFO head
 *                 pointers (address width is 5 bits, data width is
 *                 13 bits).    4'd2andedsp;andedsp;andedsp;: Reports
 *                 linked-list (address width is 13 bits, data width
 *                 is 13 bits).    4'd3andedsp;andedsp;andedsp;:
 *                 Reports FIFO memory (address width is 13 bits,
 *                 data width is 156 bits).
 *                 4'd4andedsp;andedsp;andedsp;: Used copy-counters
 *                 counter (address width is 1 bit, data width is 9
 *                 bits).    4'd5andedsp;andedsp;andedsp;: Free copy-
 *                 counters head and tail pointers (address width is
 *                 1 bit, data width  is 9 bits).
 *                 4'd6andedsp;andedsp;andedsp;: Copy counters
 *                 linked-list (address width is 9 bits, data width
 *                 is 9 bits).    4'd7andedsp;andedsp;andedsp;:
 *                 Packet duplication memory (address width is 9
 *                 bits, data width is 57 bits).
 *                 4'd8andedsp;andedsp;andedsp;: Packet ID table
 *                 (address width is 9 bits, data width is 16 bits).
 *                 4'd9andedsp;andedsp;andedsp;: CLID memory (address
 *                 width is 14 bits, data width is 14 bits).    4'd10
 *                 : Packet data memory (address width is 15 bits,
 *                 data width is 256 bits).    4'd11 : Reserved.
 *                 4'd12 : Reserved.    4'd13 : Reserved.    4'd14 :
 *                 Reserved.    4'd15 : Reserved.
 *   [15: 0][RW] - Memory address. Bit relevancy is diffrent
 *                 according to selected memory.
 *
 */
#define PP_RPB_DBG_ACC_CTL_REG                         ((RPB_BASE_ADDR) + 0x8E4)
#define PP_RPB_DBG_ACC_CTL_RESERVED_4_OFF                           (21)
#define PP_RPB_DBG_ACC_CTL_RESERVED_4_LEN                           (11)
#define PP_RPB_DBG_ACC_CTL_RESERVED_4_MSK                           (0xFFE00000)
#define PP_RPB_DBG_ACC_CTL_RESERVED_4_RST                           (0x0)
#define PP_RPB_DBG_ACC_CTL_EXECUTE_OFF                              (20)
#define PP_RPB_DBG_ACC_CTL_EXECUTE_LEN                              (1)
#define PP_RPB_DBG_ACC_CTL_EXECUTE_MSK                              (0x00100000)
#define PP_RPB_DBG_ACC_CTL_EXECUTE_RST                              (0x0)
#define PP_RPB_DBG_ACC_CTL_MEM_SEL_OFF                              (16)
#define PP_RPB_DBG_ACC_CTL_MEM_SEL_LEN                              (4)
#define PP_RPB_DBG_ACC_CTL_MEM_SEL_MSK                              (0x000F0000)
#define PP_RPB_DBG_ACC_CTL_MEM_SEL_RST                              (0x0)
#define PP_RPB_DBG_ACC_CTL_ADDR_OFF                                 (0)
#define PP_RPB_DBG_ACC_CTL_ADDR_LEN                                 (16)
#define PP_RPB_DBG_ACC_CTL_ADDR_MSK                                 (0x0000FFFF)
#define PP_RPB_DBG_ACC_CTL_ADDR_RST                                 (0x0)

/**
 * SW_REG_NAME : PP_RPB_DBG_ACC_STAT_REG
 * HW_REG_NAME : rpb_dbg_acc_stat_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 7][RO] - MISSING_DESCRIPTION
 *   [ 6: 6][RO] - Watchdog error. This bit will clear on EXECUTE
 *                 assertion.
 *   [ 5: 5][RO] - Memory selection error indication. This bit will
 *                 be cleared on EXECUTE assertion and set if the
 *                 memory  selection is out of range.
 *   [ 4: 4][RO] - Address error indication. This bit will be cleared
 *                 on EXECUTE assertion and set if the requested
 *                 address  is out of range for the selected memory.
 *   [ 3: 2][RO] - MISSING_DESCRIPTION
 *   [ 1: 1][RO] - Operation in progress indication. This bit will be
 *                 set on EXECUTE assertion and cleared on READY
 *                 assertion  (if no error signal was asserted).
 *   [ 0: 0][RO] - Data ready indication. This bit will clear on
 *                 EXECUTE bit assertion, and will be set by the HW
 *                 when  data is ready to be read.
 *
 */
#define PP_RPB_DBG_ACC_STAT_REG                        ((RPB_BASE_ADDR) + 0x8E8)
#define PP_RPB_DBG_ACC_STAT_RESERVED_7_OFF                          (7)
#define PP_RPB_DBG_ACC_STAT_RESERVED_7_LEN                          (25)
#define PP_RPB_DBG_ACC_STAT_RESERVED_7_MSK                          (0xFFFFFF80)
#define PP_RPB_DBG_ACC_STAT_RESERVED_7_RST                          (0x0)
#define PP_RPB_DBG_ACC_STAT_WD_ERR_OFF                              (6)
#define PP_RPB_DBG_ACC_STAT_WD_ERR_LEN                              (1)
#define PP_RPB_DBG_ACC_STAT_WD_ERR_MSK                              (0x00000040)
#define PP_RPB_DBG_ACC_STAT_WD_ERR_RST                              (0x0)
#define PP_RPB_DBG_ACC_STAT_SEL_ERR_OFF                             (5)
#define PP_RPB_DBG_ACC_STAT_SEL_ERR_LEN                             (1)
#define PP_RPB_DBG_ACC_STAT_SEL_ERR_MSK                             (0x00000020)
#define PP_RPB_DBG_ACC_STAT_SEL_ERR_RST                             (0x0)
#define PP_RPB_DBG_ACC_STAT_ADDR_ERR_OFF                            (4)
#define PP_RPB_DBG_ACC_STAT_ADDR_ERR_LEN                            (1)
#define PP_RPB_DBG_ACC_STAT_ADDR_ERR_MSK                            (0x00000010)
#define PP_RPB_DBG_ACC_STAT_ADDR_ERR_RST                            (0x0)
#define PP_RPB_DBG_ACC_STAT_RESERVED_3_OFF                          (2)
#define PP_RPB_DBG_ACC_STAT_RESERVED_3_LEN                          (2)
#define PP_RPB_DBG_ACC_STAT_RESERVED_3_MSK                          (0x0000000C)
#define PP_RPB_DBG_ACC_STAT_RESERVED_3_RST                          (0x0)
#define PP_RPB_DBG_ACC_STAT_BUSY_OFF                                (1)
#define PP_RPB_DBG_ACC_STAT_BUSY_LEN                                (1)
#define PP_RPB_DBG_ACC_STAT_BUSY_MSK                                (0x00000002)
#define PP_RPB_DBG_ACC_STAT_BUSY_RST                                (0x0)
#define PP_RPB_DBG_ACC_STAT_READY_OFF                               (0)
#define PP_RPB_DBG_ACC_STAT_READY_LEN                               (1)
#define PP_RPB_DBG_ACC_STAT_READY_MSK                               (0x00000001)
#define PP_RPB_DBG_ACC_STAT_READY_RST                               (0x0)

/**
 * SW_REG_NAME : PP_RPB_SHR_MOD_CNT_REG
 * HW_REG_NAME : rpb_shr_mod_cnt_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 0][RO] - Shared mode entry counter. This counter will be
 *                 cleared on read.
 *
 */
#define PP_RPB_SHR_MOD_CNT_REG                         ((RPB_BASE_ADDR) + 0x90C)
#define PP_RPB_SHR_MOD_CNT_OFF                                      (0)
#define PP_RPB_SHR_MOD_CNT_LEN                                      (32)
#define PP_RPB_SHR_MOD_CNT_MSK                                      (0xFFFFFFFF)
#define PP_RPB_SHR_MOD_CNT_RST                                      (0x0)

/**
 * SW_REG_NAME : PP_RPB_DBG_FEAT_REG
 * HW_REG_NAME : rpb_dbg_feat_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:20][RW] - LTR clock generation value. The core clock will be
 *                 divided by (value+1) to generate the LTR clock
 *                 (used  for LTR timer).    Default value is 399
 *                 meaning that the core clock will be divided by 400
 *                 (to create a 1 MHz clock =andgt;  1uS resolution
 *                 timer).
 *   [19:18][RO] - MISSING_DESCRIPTION
 *   [17:17][RW] - Disable last ECB fix
 *   [16:16][RW] - Force shared mode. When this bit is set, shared
 *                 mode is forced.
 *   [15:12][RW] - Force loop-back FC (Xoff) indication for all
 *                 priorities in port n.    Bit 12 - port 0    Bit 13
 *                 - port 1    Bit 14 - port 2    Bit 15 - port 3
 *   [11: 8][RW] - Force general FC (Xoff) indication for all
 *                 priorities in port n.    Bit 8- port 0    Bit 9-
 *                 port 1    Bit 10 - port 2    Bit 11 - port 3
 *   [ 7: 7][RW] - Disable RDPU egress interface. When setting this
 *                 bit, the RDPU egress interface will lower its
 *                 ready  indication and stop accepting RDPU
 *                 requests.
 *   [ 6: 6][RW] - Disable PE1 egress interface. When setting this
 *                 bit, the PE1 egress interface will lower its ready
 *                 indication and stop accepting PE1 requests.
 *   [ 5: 5][RW] - Disable PE0 egress interface. When setting this
 *                 bit, the PE0 egress interface will lower its ready
 *                 indication and stop accepting PE0 requests.
 *   [ 4: 4][RW] - Disable RCU egress interface. When setting this
 *                 bit, the RCU egress interface will lower its ready
 *                 indication and stop accepting RCU requests.
 *   [ 3: 3][RW] - Disable share mode. When setting this bit, the RPB
 *                 shall ignore shared buffer mode entry requests
 *                 from  host.
 *   [ 2: 2][RW] - Disable copy-counters. When setting this bit,
 *                 every release request will release the packet
 *                 without  checking the copy-counters. If this bit
 *                 is set together with DISABLE_RELEASE, no packet
 *                 release shall  occur.
 *   [ 1: 1][RW] - Disable packet release from RPB.  When setting
 *                 this bit, packets will not be released from the
 *                 RPB.
 *   [ 0: 0][RW] - Disable reports delivery to RCU.  When setting
 *                 this bit, no reports will be outputted to the RCU.
 *
 */
#define PP_RPB_DBG_FEAT_REG                            ((RPB_BASE_ADDR) + 0x940)
#define PP_RPB_DBG_FEAT_LTR_CLK_GEN_VAL_OFF                         (20)
#define PP_RPB_DBG_FEAT_LTR_CLK_GEN_VAL_LEN                         (12)
#define PP_RPB_DBG_FEAT_LTR_CLK_GEN_VAL_MSK                         (0xFFF00000)
#define PP_RPB_DBG_FEAT_LTR_CLK_GEN_VAL_RST                         (0x18f)
#define PP_RPB_DBG_FEAT_RESERVED_13_OFF                             (18)
#define PP_RPB_DBG_FEAT_RESERVED_13_LEN                             (2)
#define PP_RPB_DBG_FEAT_RESERVED_13_MSK                             (0x000C0000)
#define PP_RPB_DBG_FEAT_RESERVED_13_RST                             (0x0)
#define PP_RPB_DBG_FEAT_DIS_ECB_SYNC_OFF                            (17)
#define PP_RPB_DBG_FEAT_DIS_ECB_SYNC_LEN                            (1)
#define PP_RPB_DBG_FEAT_DIS_ECB_SYNC_MSK                            (0x00020000)
#define PP_RPB_DBG_FEAT_DIS_ECB_SYNC_RST                            (0x0)
#define PP_RPB_DBG_FEAT_FORCE_SHR_MODE_OFF                          (16)
#define PP_RPB_DBG_FEAT_FORCE_SHR_MODE_LEN                          (1)
#define PP_RPB_DBG_FEAT_FORCE_SHR_MODE_MSK                          (0x00010000)
#define PP_RPB_DBG_FEAT_FORCE_SHR_MODE_RST                          (0x0)
#define PP_RPB_DBG_FEAT_FORCE_TPB_FC_PORT_OFF                       (12)
#define PP_RPB_DBG_FEAT_FORCE_TPB_FC_PORT_LEN                       (4)
#define PP_RPB_DBG_FEAT_FORCE_TPB_FC_PORT_MSK                       (0x0000F000)
#define PP_RPB_DBG_FEAT_FORCE_TPB_FC_PORT_RST                       (0x0)
#define PP_RPB_DBG_FEAT_FORCE_FC_PORT_OFF                           (8)
#define PP_RPB_DBG_FEAT_FORCE_FC_PORT_LEN                           (4)
#define PP_RPB_DBG_FEAT_FORCE_FC_PORT_MSK                           (0x00000F00)
#define PP_RPB_DBG_FEAT_FORCE_FC_PORT_RST                           (0x0)
#define PP_RPB_DBG_FEAT_DIS_RDPU_EGR_OFF                            (7)
#define PP_RPB_DBG_FEAT_DIS_RDPU_EGR_LEN                            (1)
#define PP_RPB_DBG_FEAT_DIS_RDPU_EGR_MSK                            (0x00000080)
#define PP_RPB_DBG_FEAT_DIS_RDPU_EGR_RST                            (0x0)
#define PP_RPB_DBG_FEAT_DIS_PE1_EGR_OFF                             (6)
#define PP_RPB_DBG_FEAT_DIS_PE1_EGR_LEN                             (1)
#define PP_RPB_DBG_FEAT_DIS_PE1_EGR_MSK                             (0x00000040)
#define PP_RPB_DBG_FEAT_DIS_PE1_EGR_RST                             (0x0)
#define PP_RPB_DBG_FEAT_DIS_PE0_EGR_OFF                             (5)
#define PP_RPB_DBG_FEAT_DIS_PE0_EGR_LEN                             (1)
#define PP_RPB_DBG_FEAT_DIS_PE0_EGR_MSK                             (0x00000020)
#define PP_RPB_DBG_FEAT_DIS_PE0_EGR_RST                             (0x0)
#define PP_RPB_DBG_FEAT_DIS_RCU_EGR_OFF                             (4)
#define PP_RPB_DBG_FEAT_DIS_RCU_EGR_LEN                             (1)
#define PP_RPB_DBG_FEAT_DIS_RCU_EGR_MSK                             (0x00000010)
#define PP_RPB_DBG_FEAT_DIS_RCU_EGR_RST                             (0x0)
#define PP_RPB_DBG_FEAT_DIS_SHR_MODE_OFF                            (3)
#define PP_RPB_DBG_FEAT_DIS_SHR_MODE_LEN                            (1)
#define PP_RPB_DBG_FEAT_DIS_SHR_MODE_MSK                            (0x00000008)
#define PP_RPB_DBG_FEAT_DIS_SHR_MODE_RST                            (0x0)
#define PP_RPB_DBG_FEAT_DIS_CC_OFF                                  (2)
#define PP_RPB_DBG_FEAT_DIS_CC_LEN                                  (1)
#define PP_RPB_DBG_FEAT_DIS_CC_MSK                                  (0x00000004)
#define PP_RPB_DBG_FEAT_DIS_CC_RST                                  (0x0)
#define PP_RPB_DBG_FEAT_DIS_RELEASE_OFF                             (1)
#define PP_RPB_DBG_FEAT_DIS_RELEASE_LEN                             (1)
#define PP_RPB_DBG_FEAT_DIS_RELEASE_MSK                             (0x00000002)
#define PP_RPB_DBG_FEAT_DIS_RELEASE_RST                             (0x0)
#define PP_RPB_DBG_FEAT_DIS_REPORTS_OFF                             (0)
#define PP_RPB_DBG_FEAT_DIS_REPORTS_LEN                             (1)
#define PP_RPB_DBG_FEAT_DIS_REPORTS_MSK                             (0x00000001)
#define PP_RPB_DBG_FEAT_DIS_REPORTS_RST                             (0x0)

/**
 * SW_REG_NAME : PP_RPB_GEN_DBG_CNT_REG
 * HW_REG_NAME : rpb_gen_dbg_cnt_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:30][RO] - MISSING_DESCRIPTION
 *   [29:16][RO] - Amount of non-allocated clids
 *   [15: 9][RO] - MISSING_DESCRIPTION
 *   [ 8: 0][RO] - Amount of non-allocated copy-counters
 *
 */
#define PP_RPB_GEN_DBG_CNT_REG                         ((RPB_BASE_ADDR) + 0x944)
#define PP_RPB_GEN_DBG_CNT_RESERVED_4_OFF                           (30)
#define PP_RPB_GEN_DBG_CNT_RESERVED_4_LEN                           (2)
#define PP_RPB_GEN_DBG_CNT_RESERVED_4_MSK                           (0xC0000000)
#define PP_RPB_GEN_DBG_CNT_RESERVED_4_RST                           (0x0)
#define PP_RPB_GEN_DBG_CNT_FREE_CLIDS_OFF                           (16)
#define PP_RPB_GEN_DBG_CNT_FREE_CLIDS_LEN                           (14)
#define PP_RPB_GEN_DBG_CNT_FREE_CLIDS_MSK                           (0x3FFF0000)
#define PP_RPB_GEN_DBG_CNT_FREE_CLIDS_RST                           (0x0)
#define PP_RPB_GEN_DBG_CNT_RESERVED_2_OFF                           (9)
#define PP_RPB_GEN_DBG_CNT_RESERVED_2_LEN                           (7)
#define PP_RPB_GEN_DBG_CNT_RESERVED_2_MSK                           (0x0000FE00)
#define PP_RPB_GEN_DBG_CNT_RESERVED_2_RST                           (0x0)
#define PP_RPB_GEN_DBG_CNT_FREE_CC_OFF                              (0)
#define PP_RPB_GEN_DBG_CNT_FREE_CC_LEN                              (9)
#define PP_RPB_GEN_DBG_CNT_FREE_CC_MSK                              (0x000001FF)
#define PP_RPB_GEN_DBG_CNT_FREE_CC_RST                              (0x0)

/**
 * SW_REG_NAME : PP_RPB_BACK_PRS_STAT_REG
 * HW_REG_NAME : rpb_back_prs_stat_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:16][RO] - MISSING_DESCRIPTION
 *   [15:15][RO] - Flow-control (Xoff) status to port 3
 *   [14:14][RO] - Flow-control (Xoff) status to port 2
 *   [13:13][RO] - Flow-control (Xoff) status to port 1
 *   [12:12][RO] - Flow-control (Xoff) status to port 0
 *   [11:11][RO] - RDY to RDPU data request.
 *   [10:10][RO] - RDY to PE1
 *   [ 9: 9][RO] - RDY to PE0 data request.
 *   [ 8: 8][RO] - RDY to RCU data request.
 *   [ 7: 5][RO] - MISSING_DESCRIPTION
 *   [ 4: 4][RO] - valid signal of RCB-)RCU status indication
 *   [ 3: 3][RO] - reserved
 *   [ 2: 2][RO] - reserved
 *   [ 1: 1][RO] - reserved
 *   [ 0: 0][RO] - reserved
 *
 */
#define PP_RPB_BACK_PRS_STAT_REG                       ((RPB_BASE_ADDR) + 0x948)
#define PP_RPB_BACK_PRS_STAT_RESERVED_15_OFF                        (16)
#define PP_RPB_BACK_PRS_STAT_RESERVED_15_LEN                        (16)
#define PP_RPB_BACK_PRS_STAT_RESERVED_15_MSK                        (0xFFFF0000)
#define PP_RPB_BACK_PRS_STAT_RESERVED_15_RST                        (0x0)
#define PP_RPB_BACK_PRS_STAT_PORT_3_FC_OFF                          (15)
#define PP_RPB_BACK_PRS_STAT_PORT_3_FC_LEN                          (1)
#define PP_RPB_BACK_PRS_STAT_PORT_3_FC_MSK                          (0x00008000)
#define PP_RPB_BACK_PRS_STAT_PORT_3_FC_RST                          (0x0)
#define PP_RPB_BACK_PRS_STAT_PORT_2_FC_OFF                          (14)
#define PP_RPB_BACK_PRS_STAT_PORT_2_FC_LEN                          (1)
#define PP_RPB_BACK_PRS_STAT_PORT_2_FC_MSK                          (0x00004000)
#define PP_RPB_BACK_PRS_STAT_PORT_2_FC_RST                          (0x0)
#define PP_RPB_BACK_PRS_STAT_PORT_1_FC_OFF                          (13)
#define PP_RPB_BACK_PRS_STAT_PORT_1_FC_LEN                          (1)
#define PP_RPB_BACK_PRS_STAT_PORT_1_FC_MSK                          (0x00002000)
#define PP_RPB_BACK_PRS_STAT_PORT_1_FC_RST                          (0x0)
#define PP_RPB_BACK_PRS_STAT_PORT_0_FC_OFF                          (12)
#define PP_RPB_BACK_PRS_STAT_PORT_0_FC_LEN                          (1)
#define PP_RPB_BACK_PRS_STAT_PORT_0_FC_MSK                          (0x00001000)
#define PP_RPB_BACK_PRS_STAT_PORT_0_FC_RST                          (0x0)
#define PP_RPB_BACK_PRS_STAT_RDPU_BP_OFF                            (11)
#define PP_RPB_BACK_PRS_STAT_RDPU_BP_LEN                            (1)
#define PP_RPB_BACK_PRS_STAT_RDPU_BP_MSK                            (0x00000800)
#define PP_RPB_BACK_PRS_STAT_RDPU_BP_RST                            (0x1)
#define PP_RPB_BACK_PRS_STAT_PE1_BP_OFF                             (10)
#define PP_RPB_BACK_PRS_STAT_PE1_BP_LEN                             (1)
#define PP_RPB_BACK_PRS_STAT_PE1_BP_MSK                             (0x00000400)
#define PP_RPB_BACK_PRS_STAT_PE1_BP_RST                             (0x1)
#define PP_RPB_BACK_PRS_STAT_PE0_BP_OFF                             (9)
#define PP_RPB_BACK_PRS_STAT_PE0_BP_LEN                             (1)
#define PP_RPB_BACK_PRS_STAT_PE0_BP_MSK                             (0x00000200)
#define PP_RPB_BACK_PRS_STAT_PE0_BP_RST                             (0x1)
#define PP_RPB_BACK_PRS_STAT_RCU_BP_OFF                             (8)
#define PP_RPB_BACK_PRS_STAT_RCU_BP_LEN                             (1)
#define PP_RPB_BACK_PRS_STAT_RCU_BP_MSK                             (0x00000100)
#define PP_RPB_BACK_PRS_STAT_RCU_BP_RST                             (0x1)
#define PP_RPB_BACK_PRS_STAT_RESERVED_6_OFF                         (5)
#define PP_RPB_BACK_PRS_STAT_RESERVED_6_LEN                         (3)
#define PP_RPB_BACK_PRS_STAT_RESERVED_6_MSK                         (0x000000E0)
#define PP_RPB_BACK_PRS_STAT_RESERVED_6_RST                         (0x0)
#define PP_RPB_BACK_PRS_STAT_STATUS_BP_OFF                          (4)
#define PP_RPB_BACK_PRS_STAT_STATUS_BP_LEN                          (1)
#define PP_RPB_BACK_PRS_STAT_STATUS_BP_MSK                          (0x00000010)
#define PP_RPB_BACK_PRS_STAT_STATUS_BP_RST                          (0x0)
#define PP_RPB_BACK_PRS_STAT_PPRS_3_BP_OFF                          (3)
#define PP_RPB_BACK_PRS_STAT_PPRS_3_BP_LEN                          (1)
#define PP_RPB_BACK_PRS_STAT_PPRS_3_BP_MSK                          (0x00000008)
#define PP_RPB_BACK_PRS_STAT_PPRS_3_BP_RST                          (0x0)
#define PP_RPB_BACK_PRS_STAT_PPRS_2_BP_OFF                          (2)
#define PP_RPB_BACK_PRS_STAT_PPRS_2_BP_LEN                          (1)
#define PP_RPB_BACK_PRS_STAT_PPRS_2_BP_MSK                          (0x00000004)
#define PP_RPB_BACK_PRS_STAT_PPRS_2_BP_RST                          (0x0)
#define PP_RPB_BACK_PRS_STAT_PPRS_1_BP_OFF                          (1)
#define PP_RPB_BACK_PRS_STAT_PPRS_1_BP_LEN                          (1)
#define PP_RPB_BACK_PRS_STAT_PPRS_1_BP_MSK                          (0x00000002)
#define PP_RPB_BACK_PRS_STAT_PPRS_1_BP_RST                          (0x0)
#define PP_RPB_BACK_PRS_STAT_PPRS_0_BP_OFF                          (0)
#define PP_RPB_BACK_PRS_STAT_PPRS_0_BP_LEN                          (1)
#define PP_RPB_BACK_PRS_STAT_PPRS_0_BP_MSK                          (0x00000001)
#define PP_RPB_BACK_PRS_STAT_PPRS_0_BP_RST                          (0x0)

/**
 * SW_REG_NAME : PP_RPB_EGR_CNT_REG
 * HW_REG_NAME : rpb_egr_cnt_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:24][RO] - Amount of RDPU data requests. This counter shall
 *                 be cleared on read
 *   [23:16][RO] - Amount of PE1 data requests. This counter shall be
 *                 cleared on read
 *   [15: 8][RO] - Amount of PE0 data requests. This counter shall be
 *                 cleared on read
 *   [ 7: 0][RO] - Amount of RCU data requests. This counter shall be
 *                 cleared on read
 *
 */
#define PP_RPB_EGR_CNT_REG                             ((RPB_BASE_ADDR) + 0x94C)
#define PP_RPB_EGR_CNT_RDPU_REQ_OFF                                 (24)
#define PP_RPB_EGR_CNT_RDPU_REQ_LEN                                 (8)
#define PP_RPB_EGR_CNT_RDPU_REQ_MSK                                 (0xFF000000)
#define PP_RPB_EGR_CNT_RDPU_REQ_RST                                 (0x0)
#define PP_RPB_EGR_CNT_PE_1_REQ_OFF                                 (16)
#define PP_RPB_EGR_CNT_PE_1_REQ_LEN                                 (8)
#define PP_RPB_EGR_CNT_PE_1_REQ_MSK                                 (0x00FF0000)
#define PP_RPB_EGR_CNT_PE_1_REQ_RST                                 (0x0)
#define PP_RPB_EGR_CNT_PE_0_REQ_OFF                                 (8)
#define PP_RPB_EGR_CNT_PE_0_REQ_LEN                                 (8)
#define PP_RPB_EGR_CNT_PE_0_REQ_MSK                                 (0x0000FF00)
#define PP_RPB_EGR_CNT_PE_0_REQ_RST                                 (0x0)
#define PP_RPB_EGR_CNT_RCU_REQ_OFF                                  (0)
#define PP_RPB_EGR_CNT_RCU_REQ_LEN                                  (8)
#define PP_RPB_EGR_CNT_RCU_REQ_MSK                                  (0x000000FF)
#define PP_RPB_EGR_CNT_RCU_REQ_RST                                  (0x0)

/**
 * SW_REG_NAME : PP_RPB_RPT_CNT_REG
 * HW_REG_NAME : rpb_rpt_cnt_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:16][RO] - MISSING_DESCRIPTION
 *   [15: 0][RO] - Amount of reports to the RCU. This counter shall
 *                 be cleared on read.
 *
 */
#define PP_RPB_RPT_CNT_REG                             ((RPB_BASE_ADDR) + 0x950)
#define PP_RPB_RPT_CNT_RSVD_2_OFF                                   (16)
#define PP_RPB_RPT_CNT_RSVD_2_LEN                                   (16)
#define PP_RPB_RPT_CNT_RSVD_2_MSK                                   (0xFFFF0000)
#define PP_RPB_RPT_CNT_RSVD_2_RST                                   (0x0)
#define PP_RPB_RPT_CNT_OFF                                          (0)
#define PP_RPB_RPT_CNT_LEN                                          (16)
#define PP_RPB_RPT_CNT_MSK                                          (0x0000FFFF)
#define PP_RPB_RPT_CNT_RST                                          (0x0)

/**
 * SW_REG_NAME : PP_RPB_RPT_STAT_REG
 * HW_REG_NAME : rpb_rpt_stat_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 0][RO] - Current pending reports status. Each bit
 *                 correspondes to a reported UP.
 *
 */
#define PP_RPB_RPT_STAT_REG                            ((RPB_BASE_ADDR) + 0x954)
#define PP_RPB_RPT_STAT_OFF                                         (0)
#define PP_RPB_RPT_STAT_LEN                                         (32)
#define PP_RPB_RPT_STAT_MSK                                         (0xFFFFFFFF)
#define PP_RPB_RPT_STAT_RST                                         (0x0)

/**
 * SW_REG_NAME : PP_RPB_STW_W0_REG
 * HW_REG_NAME : rpb_stw_w0_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:12][RO] - MISSING_DESCRIPTION
 *   [11: 0][RO] - Captured Status Word MSBits. Freeze on Error.
 *
 */
#define PP_RPB_STW_W0_REG                              ((RPB_BASE_ADDR) + 0x980)
#define PP_RPB_STW_W0_RSVD_2_OFF                                    (12)
#define PP_RPB_STW_W0_RSVD_2_LEN                                    (20)
#define PP_RPB_STW_W0_RSVD_2_MSK                                    (0xFFFFF000)
#define PP_RPB_STW_W0_RSVD_2_RST                                    (0x0)
#define PP_RPB_STW_W0_RPB_STW_MSB_W_OFF                             (0)
#define PP_RPB_STW_W0_RPB_STW_MSB_W_LEN                             (12)
#define PP_RPB_STW_W0_RPB_STW_MSB_W_MSK                             (0x00000FFF)
#define PP_RPB_STW_W0_RPB_STW_MSB_W_RST                             (0x0)

/**
 * SW_REG_NAME : PP_RPB_STW_W1_REG
 * HW_REG_NAME : rpb_stw_w1_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 0][RO] - Captured Status Word. Freeze on Error. Release on
 *                 Read
 *
 */
#define PP_RPB_STW_W1_REG                              ((RPB_BASE_ADDR) + 0x984)
#define PP_RPB_STW_W1_RPB_STW_WORD_OFF                              (0)
#define PP_RPB_STW_W1_RPB_STW_WORD_LEN                              (32)
#define PP_RPB_STW_W1_RPB_STW_WORD_MSK                              (0xFFFFFFFF)
#define PP_RPB_STW_W1_RPB_STW_WORD_RST                              (0x0)

/**
 * SW_REG_NAME : PP_RPB_STW_W2_REG
 * HW_REG_NAME : rpb_stw_w2_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 0][RO] - Captured Status Word. Freeze on Error. Release on
 *                 Read
 *
 */
#define PP_RPB_STW_W2_REG                              ((RPB_BASE_ADDR) + 0x988)
#define PP_RPB_STW_W2_RPB_STW_WORD_OFF                              (0)
#define PP_RPB_STW_W2_RPB_STW_WORD_LEN                              (32)
#define PP_RPB_STW_W2_RPB_STW_WORD_MSK                              (0xFFFFFFFF)
#define PP_RPB_STW_W2_RPB_STW_WORD_RST                              (0x0)

/**
 * SW_REG_NAME : PP_RPB_STW_W3_REG
 * HW_REG_NAME : rpb_stw_w3_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 0][RO] - Captured Status Word. Freeze on Error. Release on
 *                 Read
 *
 */
#define PP_RPB_STW_W3_REG                              ((RPB_BASE_ADDR) + 0x98C)
#define PP_RPB_STW_W3_RPB_STW_WORD_OFF                              (0)
#define PP_RPB_STW_W3_RPB_STW_WORD_LEN                              (32)
#define PP_RPB_STW_W3_RPB_STW_WORD_MSK                              (0xFFFFFFFF)
#define PP_RPB_STW_W3_RPB_STW_WORD_RST                              (0x0)

/**
 * SW_REG_NAME : PP_RPB_STW_W4_REG
 * HW_REG_NAME : rpb_stw_w4_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 0][RO] - Captured Status Word. Freeze on Error. Release on
 *                 Read
 *
 */
#define PP_RPB_STW_W4_REG                              ((RPB_BASE_ADDR) + 0x990)
#define PP_RPB_STW_W4_RPB_STW_WORD_OFF                              (0)
#define PP_RPB_STW_W4_RPB_STW_WORD_LEN                              (32)
#define PP_RPB_STW_W4_RPB_STW_WORD_MSK                              (0xFFFFFFFF)
#define PP_RPB_STW_W4_RPB_STW_WORD_RST                              (0x0)

/**
 * SW_REG_NAME : PP_RPB_STW_RD_CNT_REG
 * HW_REG_NAME : rpb_stw_rd_cnt_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 0][RO] - Status words read by Parser counter
 *
 */
#define PP_RPB_STW_RD_CNT_REG                          ((RPB_BASE_ADDR) + 0x998)
#define PP_RPB_STW_RD_CNT_OFF                                       (0)
#define PP_RPB_STW_RD_CNT_LEN                                       (32)
#define PP_RPB_STW_RD_CNT_MSK                                       (0xFFFFFFFF)
#define PP_RPB_STW_RD_CNT_RST                                       (0x0)

/**
 * SW_REG_NAME : PP_RPB_RXDMA_RLS_CNT_REG
 * HW_REG_NAME : rpb_rxdma_rls_cnt_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 0][RO] - No. of  Packets released by RX_DMA counter
 *
 */
#define PP_RPB_RXDMA_RLS_CNT_REG                       ((RPB_BASE_ADDR) + 0x99C)
#define PP_RPB_RXDMA_RLS_CNT_OFF                                    (0)
#define PP_RPB_RXDMA_RLS_CNT_LEN                                    (32)
#define PP_RPB_RXDMA_RLS_CNT_MSK                                    (0xFFFFFFFF)
#define PP_RPB_RXDMA_RLS_CNT_RST                                    (0x0)

/**
 * SW_REG_NAME : PP_RPB_PORT0_IN_PKT_CNT_REG
 * HW_REG_NAME : rpb_port0_in_pkt_cnt_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 0][RO] - No. of Packets into PORT X [0-3] counter
 *
 */
#define PP_RPB_PORT0_IN_PKT_CNT_REG                    ((RPB_BASE_ADDR) + 0x9A0)
#define PP_RPB_PORT0_IN_PKT_CNT_RPB_PORT_IN_PKT_CNT_OFF             (0)
#define PP_RPB_PORT0_IN_PKT_CNT_RPB_PORT_IN_PKT_CNT_LEN             (32)
#define PP_RPB_PORT0_IN_PKT_CNT_RPB_PORT_IN_PKT_CNT_MSK             (0xFFFFFFFF)
#define PP_RPB_PORT0_IN_PKT_CNT_RPB_PORT_IN_PKT_CNT_RST             (0x0)

/**
 * SW_REG_NAME : PP_RPB_PORT1_IN_PKT_CNT_REG
 * HW_REG_NAME : rpb_port1_in_pkt_cnt_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 0][RO] - No. of Packets into PORT X [0-3] counter
 *
 */
#define PP_RPB_PORT1_IN_PKT_CNT_REG                    ((RPB_BASE_ADDR) + 0x9A4)
#define PP_RPB_PORT1_IN_PKT_CNT_RPB_PORT_IN_PKT_CNT_OFF             (0)
#define PP_RPB_PORT1_IN_PKT_CNT_RPB_PORT_IN_PKT_CNT_LEN             (32)
#define PP_RPB_PORT1_IN_PKT_CNT_RPB_PORT_IN_PKT_CNT_MSK             (0xFFFFFFFF)
#define PP_RPB_PORT1_IN_PKT_CNT_RPB_PORT_IN_PKT_CNT_RST             (0x0)

/**
 * SW_REG_NAME : PP_RPB_PORT2_IN_PKT_CNT_REG
 * HW_REG_NAME : rpb_port2_in_pkt_cnt_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 0][RO] - No. of Packets into PORT X [0-3] counter
 *
 */
#define PP_RPB_PORT2_IN_PKT_CNT_REG                    ((RPB_BASE_ADDR) + 0x9C0)
#define PP_RPB_PORT2_IN_PKT_CNT_RPB_PORT_IN_PKT_CNT_OFF             (0)
#define PP_RPB_PORT2_IN_PKT_CNT_RPB_PORT_IN_PKT_CNT_LEN             (32)
#define PP_RPB_PORT2_IN_PKT_CNT_RPB_PORT_IN_PKT_CNT_MSK             (0xFFFFFFFF)
#define PP_RPB_PORT2_IN_PKT_CNT_RPB_PORT_IN_PKT_CNT_RST             (0x0)

/**
 * SW_REG_NAME : PP_RPB_PORT3_IN_PKT_CNT_REG
 * HW_REG_NAME : rpb_port3_in_pkt_cnt_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 0][RO] - No. of Packets into PORT X [0-3] counter
 *
 */
#define PP_RPB_PORT3_IN_PKT_CNT_REG                    ((RPB_BASE_ADDR) + 0x9C4)
#define PP_RPB_PORT3_IN_PKT_CNT_RPB_PORT_IN_PKT_CNT_OFF             (0)
#define PP_RPB_PORT3_IN_PKT_CNT_RPB_PORT_IN_PKT_CNT_LEN             (32)
#define PP_RPB_PORT3_IN_PKT_CNT_RPB_PORT_IN_PKT_CNT_MSK             (0xFFFFFFFF)
#define PP_RPB_PORT3_IN_PKT_CNT_RPB_PORT_IN_PKT_CNT_RST             (0x0)

/**
 * SW_REG_NAME : PP_RPB_PRTPM_HPTC_REG
 * HW_REG_NAME : prtpm_hptc_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 8][RO] - Reserved
 *   [ 7: 0][RW] - High Priority Traffic Classandedsp;    This field
 *                 defines which TC is considered high priority and a
 *                 packet received for this TC will cause  exit from
 *                 DMA Coalescing. When a TC bit is set it indicates
 *                 this TC is high priority.    Bit 0 TC0    Bit 1
 *                 TC1    ...    Bit 7 TC7
 *
 */
#define PP_RPB_PRTPM_HPTC_REG                          ((RPB_BASE_ADDR) + 0x800)
#define PP_RPB_PRTPM_HPTC_RESERVED_2_OFF                            (8)
#define PP_RPB_PRTPM_HPTC_RESERVED_2_LEN                            (24)
#define PP_RPB_PRTPM_HPTC_RESERVED_2_MSK                            (0xFFFFFF00)
#define PP_RPB_PRTPM_HPTC_RESERVED_2_RST                            (0x0)
#define PP_RPB_PRTPM_HPTC_HIGH_PRI_TC_OFF                           (0)
#define PP_RPB_PRTPM_HPTC_HIGH_PRI_TC_LEN                           (8)
#define PP_RPB_PRTPM_HPTC_HIGH_PRI_TC_MSK                           (0x000000FF)
#define PP_RPB_PRTPM_HPTC_HIGH_PRI_TC_RST                           (0x0)
/**
 * REG_IDX_ACCESS   : PP_RPB_PRTPM_HPTC_REG_IDX
 * NUM OF REGISTERS : 4
 */
#define PP_RPB_PRTPM_HPTC_REG_IDX(idx) \
	(PP_RPB_PRTPM_HPTC_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_RPB_PRTTC2PFC_REG
 * HW_REG_NAME : prtrpb_tc2pfc_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 8][RO] - MISSING_DESCRIPTION
 *   [ 7: 0][RW] - Bitmap that controls the use of Priority Flow
 *                 Control (PFC) per each TC.  Bit n set to 1b ;TC n
 *                 uses PFC in Rx and Tx. The TC is referred as a no-
 *                 drop TC.    Bit n clear to 0b - The device will
 *                 not issue PFC pause frames with bits set to 1b in
 *                 the priority_enable_vector  for the UPs attached
 *                 to that TC. It will not react to bits set to
 *                 1bandedsp;andedsp;for the UPs attached  to that TC
 *                 in the priority_enable_vector of a received PFC
 *                 pause frame . The TC is referred as a drop  UP.
 *
 */
#define PP_RPB_PRTTC2PFC_REG                           ((RPB_BASE_ADDR) + 0x200)
#define PP_RPB_PRTTC2PFC_RESERVED_2_OFF                             (8)
#define PP_RPB_PRTTC2PFC_RESERVED_2_LEN                             (24)
#define PP_RPB_PRTTC2PFC_RESERVED_2_MSK                             (0xFFFFFF00)
#define PP_RPB_PRTTC2PFC_RESERVED_2_RST                             (0x0)
#define PP_RPB_PRTTC2PFC_TC2PFC_OFF                                 (0)
#define PP_RPB_PRTTC2PFC_TC2PFC_LEN                                 (8)
#define PP_RPB_PRTTC2PFC_TC2PFC_MSK                                 (0x000000FF)
#define PP_RPB_PRTTC2PFC_TC2PFC_RST                                 (0x0)
/**
 * REG_IDX_ACCESS   : PP_RPB_PRTTC2PFC_REG_IDX
 * NUM OF REGISTERS : 4
 */
#define PP_RPB_PRTTC2PFC_REG_IDX(idx) \
	(PP_RPB_PRTTC2PFC_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_RPB_PRTPFC_REG
 * HW_REG_NAME : prtrpb_pfc_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19: 0][RO] - Per Port Filling Counter.    Total number of bytes
 *                 currently stored in the Rx packet buffer on the
 *                 account of the port. It includes  bytes stored on
 *                 the account of shared pool and dedicated pools
 *                 altogether.
 *
 */
#define PP_RPB_PRTPFC_REG                              ((RPB_BASE_ADDR) + 0x420)
#define PP_RPB_PRTPFC_RESERVED_2_OFF                                (20)
#define PP_RPB_PRTPFC_RESERVED_2_LEN                                (12)
#define PP_RPB_PRTPFC_RESERVED_2_MSK                                (0xFFF00000)
#define PP_RPB_PRTPFC_RESERVED_2_RST                                (0x0)
#define PP_RPB_PRTPFC_PFC_OFF                                       (0)
#define PP_RPB_PRTPFC_PFC_LEN                                       (20)
#define PP_RPB_PRTPFC_PFC_MSK                                       (0x000FFFFF)
#define PP_RPB_PRTPFC_PFC_RST                                       (0x0)
/**
 * REG_IDX_ACCESS   : PP_RPB_PRTPFC_REG_IDX
 * NUM OF REGISTERS : 4
 */
#define PP_RPB_PRTPFC_REG_IDX(idx) \
	(PP_RPB_PRTPFC_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_RPB_PRTRUP2TC_REG
 * HW_REG_NAME : prtrpb_rup2tc_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:24][RO] - Reserved
 *   [23:21][RW] - TC index to which UP 7 is mapped.
 *   [20:18][RW] - TC index to which UP 6 is mapped.
 *   [17:15][RW] - TC index to which UP 5 is mapped.
 *   [14:12][RW] - TC index to which UP 4 is mapped.
 *   [11: 9][RW] - TC index to which UP 3 is mapped.
 *   [ 8: 6][RW] - TC index to which UP 2 is mapped.
 *   [ 5: 3][RW] - TC index to which UP 1 is mapped.
 *   [ 2: 0][RW] - TC index to which UP 0 is mapped.
 *
 */
#define PP_RPB_PRTRUP2TC_REG                           ((RPB_BASE_ADDR) + 0x440)
#define PP_RPB_PRTRUP2TC_RESERVED_9_OFF                             (24)
#define PP_RPB_PRTRUP2TC_RESERVED_9_LEN                             (8)
#define PP_RPB_PRTRUP2TC_RESERVED_9_MSK                             (0xFF000000)
#define PP_RPB_PRTRUP2TC_RESERVED_9_RST                             (0x0)
#define PP_RPB_PRTRUP2TC_UP7TC_OFF                                  (21)
#define PP_RPB_PRTRUP2TC_UP7TC_LEN                                  (3)
#define PP_RPB_PRTRUP2TC_UP7TC_MSK                                  (0x00E00000)
#define PP_RPB_PRTRUP2TC_UP7TC_RST                                  (0x0)
#define PP_RPB_PRTRUP2TC_UP6TC_OFF                                  (18)
#define PP_RPB_PRTRUP2TC_UP6TC_LEN                                  (3)
#define PP_RPB_PRTRUP2TC_UP6TC_MSK                                  (0x001C0000)
#define PP_RPB_PRTRUP2TC_UP6TC_RST                                  (0x0)
#define PP_RPB_PRTRUP2TC_UP5TC_OFF                                  (15)
#define PP_RPB_PRTRUP2TC_UP5TC_LEN                                  (3)
#define PP_RPB_PRTRUP2TC_UP5TC_MSK                                  (0x00038000)
#define PP_RPB_PRTRUP2TC_UP5TC_RST                                  (0x0)
#define PP_RPB_PRTRUP2TC_UP4TC_OFF                                  (12)
#define PP_RPB_PRTRUP2TC_UP4TC_LEN                                  (3)
#define PP_RPB_PRTRUP2TC_UP4TC_MSK                                  (0x00007000)
#define PP_RPB_PRTRUP2TC_UP4TC_RST                                  (0x0)
#define PP_RPB_PRTRUP2TC_UP3TC_OFF                                  (9)
#define PP_RPB_PRTRUP2TC_UP3TC_LEN                                  (3)
#define PP_RPB_PRTRUP2TC_UP3TC_MSK                                  (0x00000E00)
#define PP_RPB_PRTRUP2TC_UP3TC_RST                                  (0x0)
#define PP_RPB_PRTRUP2TC_UP2TC_OFF                                  (6)
#define PP_RPB_PRTRUP2TC_UP2TC_LEN                                  (3)
#define PP_RPB_PRTRUP2TC_UP2TC_MSK                                  (0x000001C0)
#define PP_RPB_PRTRUP2TC_UP2TC_RST                                  (0x0)
#define PP_RPB_PRTRUP2TC_UP1TC_OFF                                  (3)
#define PP_RPB_PRTRUP2TC_UP1TC_LEN                                  (3)
#define PP_RPB_PRTRUP2TC_UP1TC_MSK                                  (0x00000038)
#define PP_RPB_PRTRUP2TC_UP1TC_RST                                  (0x0)
#define PP_RPB_PRTRUP2TC_UP0TC_OFF                                  (0)
#define PP_RPB_PRTRUP2TC_UP0TC_LEN                                  (3)
#define PP_RPB_PRTRUP2TC_UP0TC_MSK                                  (0x00000007)
#define PP_RPB_PRTRUP2TC_UP0TC_RST                                  (0x0)
/**
 * REG_IDX_ACCESS   : PP_RPB_PRTRUP2TC_REG_IDX
 * NUM OF REGISTERS : 4
 */
#define PP_RPB_PRTRUP2TC_REG_IDX(idx) \
	(PP_RPB_PRTRUP2TC_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_RPB_PRTSFC_REG
 * HW_REG_NAME : prtrpb_sfc_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19: 0][RO] - Shared Pool Filling Counter.    Number of bytes
 *                 currently stored in the Rx packet buffer on the
 *                 account of the shared pool of the port.andedsp;
 *
 */
#define PP_RPB_PRTSFC_REG                              ((RPB_BASE_ADDR) + 0x460)
#define PP_RPB_PRTSFC_RESERVED_2_OFF                                (20)
#define PP_RPB_PRTSFC_RESERVED_2_LEN                                (12)
#define PP_RPB_PRTSFC_RESERVED_2_MSK                                (0xFFF00000)
#define PP_RPB_PRTSFC_RESERVED_2_RST                                (0x0)
#define PP_RPB_PRTSFC_SFC_OFF                                       (0)
#define PP_RPB_PRTSFC_SFC_LEN                                       (20)
#define PP_RPB_PRTSFC_SFC_MSK                                       (0x000FFFFF)
#define PP_RPB_PRTSFC_SFC_RST                                       (0x0)
/**
 * REG_IDX_ACCESS   : PP_RPB_PRTSFC_REG_IDX
 * NUM OF REGISTERS : 4
 */
#define PP_RPB_PRTSFC_REG_IDX(idx) \
	(PP_RPB_PRTSFC_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_RPB_PRTSHW_REG
 * HW_REG_NAME : prtrpb_shw_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19: 0][RW] - Shared Pool High Watermark.    It is expressed in
 *                 bytes.
 *
 */
#define PP_RPB_PRTSHW_REG                              ((RPB_BASE_ADDR) + 0x580)
#define PP_RPB_PRTSHW_RESERVED_2_OFF                                (20)
#define PP_RPB_PRTSHW_RESERVED_2_LEN                                (12)
#define PP_RPB_PRTSHW_RESERVED_2_MSK                                (0xFFF00000)
#define PP_RPB_PRTSHW_RESERVED_2_RST                                (0x0)
#define PP_RPB_PRTSHW_SHW_OFF                                       (0)
#define PP_RPB_PRTSHW_SHW_LEN                                       (20)
#define PP_RPB_PRTSHW_SHW_MSK                                       (0x000FFFFF)
#define PP_RPB_PRTSHW_SHW_RST                                       (0x10000)
/**
 * REG_IDX_ACCESS   : PP_RPB_PRTSHW_REG_IDX
 * NUM OF REGISTERS : 4
 */
#define PP_RPB_PRTSHW_REG_IDX(idx) \
	(PP_RPB_PRTSHW_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_RPB_PRTSLW_REG
 * HW_REG_NAME : prtrpb_slw_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19: 0][RW] - Shared Pool Low Watermark.    It is expressed in
 *                 bytes.
 *
 */
#define PP_RPB_PRTSLW_REG                              ((RPB_BASE_ADDR) + 0x6a0)
#define PP_RPB_PRTSLW_RESERVED_2_OFF                                (20)
#define PP_RPB_PRTSLW_RESERVED_2_LEN                                (12)
#define PP_RPB_PRTSLW_RESERVED_2_MSK                                (0xFFF00000)
#define PP_RPB_PRTSLW_RESERVED_2_RST                                (0x0)
#define PP_RPB_PRTSLW_SLW_OFF                                       (0)
#define PP_RPB_PRTSLW_SLW_LEN                                       (20)
#define PP_RPB_PRTSLW_SLW_MSK                                       (0x000FFFFF)
#define PP_RPB_PRTSLW_SLW_RST                                       (0x0)
/**
 * REG_IDX_ACCESS   : PP_RPB_PRTSLW_REG_IDX
 * NUM OF REGISTERS : 4
 */
#define PP_RPB_PRTSLW_REG_IDX(idx) \
	(PP_RPB_PRTSLW_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_RPB_PRTSPS_REG
 * HW_REG_NAME : prtrpb_sps_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19: 0][RW] - Shared Pool Size.    Number of bytes allocated to
 *                 the shared pool of the port.
 *
 */
#define PP_RPB_PRTSPS_REG                              ((RPB_BASE_ADDR) + 0x7c0)
#define PP_RPB_PRTSPS_RESERVED_2_OFF                                (20)
#define PP_RPB_PRTSPS_RESERVED_2_LEN                                (12)
#define PP_RPB_PRTSPS_RESERVED_2_MSK                                (0xFFF00000)
#define PP_RPB_PRTSPS_RESERVED_2_RST                                (0x0)
#define PP_RPB_PRTSPS_SPS_OFF                                       (0)
#define PP_RPB_PRTSPS_SPS_LEN                                       (20)
#define PP_RPB_PRTSPS_SPS_MSK                                       (0x000FFFFF)
#define PP_RPB_PRTSPS_SPS_RST                                       (0x10000)
/**
 * REG_IDX_ACCESS   : PP_RPB_PRTSPS_REG_IDX
 * NUM OF REGISTERS : 4
 */
#define PP_RPB_PRTSPS_REG_IDX(idx) \
	(PP_RPB_PRTSPS_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_RPB_PRTDFC_PRT0_REG
 * HW_REG_NAME : prtrpb_dfc_prt0_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19: 0][RO] - Dedicated Pool Filling Counter for TC .It is
 *                 expressed in bytes.
 *
 */
#define PP_RPB_PRTDFC_PRT0_REG                           ((RPB_BASE_ADDR) + 0x0)
#define PP_RPB_PRTDFC_PRT0_RESERVED_2_OFF                           (20)
#define PP_RPB_PRTDFC_PRT0_RESERVED_2_LEN                           (12)
#define PP_RPB_PRTDFC_PRT0_RESERVED_2_MSK                           (0xFFF00000)
#define PP_RPB_PRTDFC_PRT0_RESERVED_2_RST                           (0x0)
#define PP_RPB_PRTDFC_PRT0_DFC_TCN_OFF                              (0)
#define PP_RPB_PRTDFC_PRT0_DFC_TCN_LEN                              (20)
#define PP_RPB_PRTDFC_PRT0_DFC_TCN_MSK                              (0x000FFFFF)
#define PP_RPB_PRTDFC_PRT0_DFC_TCN_RST                              (0x0)
/**
 * REG_IDX_ACCESS   : PP_RPB_PRTDFC_PRT0_REG_IDX
 * NUM OF REGISTERS : 4
 */
#define PP_RPB_PRTDFC_PRT0_REG_IDX(idx) \
	(PP_RPB_PRTDFC_PRT0_REG + ((idx) << 5))

/**
 * SW_REG_NAME : PP_RPB_PRTDFC_PRT1_REG
 * HW_REG_NAME : prtrpb_dfc_prt1_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19: 0][RO] - Dedicated Pool Filling Counter for TC .It is
 *                 expressed in bytes.
 *
 */
#define PP_RPB_PRTDFC_PRT1_REG                           ((RPB_BASE_ADDR) + 0x4)
#define PP_RPB_PRTDFC_PRT1_RESERVED_2_OFF                           (20)
#define PP_RPB_PRTDFC_PRT1_RESERVED_2_LEN                           (12)
#define PP_RPB_PRTDFC_PRT1_RESERVED_2_MSK                           (0xFFF00000)
#define PP_RPB_PRTDFC_PRT1_RESERVED_2_RST                           (0x0)
#define PP_RPB_PRTDFC_PRT1_DFC_TCN_OFF                              (0)
#define PP_RPB_PRTDFC_PRT1_DFC_TCN_LEN                              (20)
#define PP_RPB_PRTDFC_PRT1_DFC_TCN_MSK                              (0x000FFFFF)
#define PP_RPB_PRTDFC_PRT1_DFC_TCN_RST                              (0x0)
/**
 * REG_IDX_ACCESS   : PP_RPB_PRTDFC_PRT1_REG_IDX
 * NUM OF REGISTERS : 4
 */
#define PP_RPB_PRTDFC_PRT1_REG_IDX(idx) \
	(PP_RPB_PRTDFC_PRT1_REG + ((idx) << 5))

/**
 * SW_REG_NAME : PP_RPB_PRTDFC_PRT2_REG
 * HW_REG_NAME : prtrpb_dfc_prt2_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19: 0][RO] - Dedicated Pool Filling Counter for TC .It is
 *                 expressed in bytes.
 *
 */
#define PP_RPB_PRTDFC_PRT2_REG                           ((RPB_BASE_ADDR) + 0x8)
#define PP_RPB_PRTDFC_PRT2_RESERVED_2_OFF                           (20)
#define PP_RPB_PRTDFC_PRT2_RESERVED_2_LEN                           (12)
#define PP_RPB_PRTDFC_PRT2_RESERVED_2_MSK                           (0xFFF00000)
#define PP_RPB_PRTDFC_PRT2_RESERVED_2_RST                           (0x0)
#define PP_RPB_PRTDFC_PRT2_DFC_TCN_OFF                              (0)
#define PP_RPB_PRTDFC_PRT2_DFC_TCN_LEN                              (20)
#define PP_RPB_PRTDFC_PRT2_DFC_TCN_MSK                              (0x000FFFFF)
#define PP_RPB_PRTDFC_PRT2_DFC_TCN_RST                              (0x0)
/**
 * REG_IDX_ACCESS   : PP_RPB_PRTDFC_PRT2_REG_IDX
 * NUM OF REGISTERS : 4
 */
#define PP_RPB_PRTDFC_PRT2_REG_IDX(idx) \
	(PP_RPB_PRTDFC_PRT2_REG + ((idx) << 5))

/**
 * SW_REG_NAME : PP_RPB_PRTDFC_PRT3_REG
 * HW_REG_NAME : prtrpb_dfc_prt3_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19: 0][RO] - Dedicated Pool Filling Counter for TC .It is
 *                 expressed in bytes.
 *
 */
#define PP_RPB_PRTDFC_PRT3_REG                           ((RPB_BASE_ADDR) + 0xc)
#define PP_RPB_PRTDFC_PRT3_RESERVED_2_OFF                           (20)
#define PP_RPB_PRTDFC_PRT3_RESERVED_2_LEN                           (12)
#define PP_RPB_PRTDFC_PRT3_RESERVED_2_MSK                           (0xFFF00000)
#define PP_RPB_PRTDFC_PRT3_RESERVED_2_RST                           (0x0)
#define PP_RPB_PRTDFC_PRT3_DFC_TCN_OFF                              (0)
#define PP_RPB_PRTDFC_PRT3_DFC_TCN_LEN                              (20)
#define PP_RPB_PRTDFC_PRT3_DFC_TCN_MSK                              (0x000FFFFF)
#define PP_RPB_PRTDFC_PRT3_DFC_TCN_RST                              (0x0)
/**
 * REG_IDX_ACCESS   : PP_RPB_PRTDFC_PRT3_REG_IDX
 * NUM OF REGISTERS : 4
 */
#define PP_RPB_PRTDFC_PRT3_REG_IDX(idx) \
	(PP_RPB_PRTDFC_PRT3_REG + ((idx) << 5))

/**
 * SW_REG_NAME : PP_RPB_PRTDHW_PRT0_REG
 * HW_REG_NAME : prtrpb_dhw_prt0_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19: 0][RW] - Dedicated Pool High Watermark for TC .It is
 *                 expressed in bytes.
 *
 */
#define PP_RPB_PRTDHW_PRT0_REG                         ((RPB_BASE_ADDR) + 0x100)
#define PP_RPB_PRTDHW_PRT0_RESERVED_2_OFF                           (20)
#define PP_RPB_PRTDHW_PRT0_RESERVED_2_LEN                           (12)
#define PP_RPB_PRTDHW_PRT0_RESERVED_2_MSK                           (0xFFF00000)
#define PP_RPB_PRTDHW_PRT0_RESERVED_2_RST                           (0x0)
#define PP_RPB_PRTDHW_PRT0_DHW_TCN_OFF                              (0)
#define PP_RPB_PRTDHW_PRT0_DHW_TCN_LEN                              (20)
#define PP_RPB_PRTDHW_PRT0_DHW_TCN_MSK                              (0x000FFFFF)
#define PP_RPB_PRTDHW_PRT0_DHW_TCN_RST                              (0x0)
/**
 * REG_IDX_ACCESS   : PP_RPB_PRTDHW_PRT0_REG_IDX
 * NUM OF REGISTERS : 4
 */
#define PP_RPB_PRTDHW_PRT0_REG_IDX(idx) \
	(PP_RPB_PRTDHW_PRT0_REG + ((idx) << 5))

/**
 * SW_REG_NAME : PP_RPB_PRTDHW_PRT1_REG
 * HW_REG_NAME : prtrpb_dhw_prt1_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19: 0][RW] - Dedicated Pool High Watermark for TC .It is
 *                 expressed in bytes.
 *
 */
#define PP_RPB_PRTDHW_PRT1_REG                         ((RPB_BASE_ADDR) + 0x104)
#define PP_RPB_PRTDHW_PRT1_RESERVED_2_OFF                           (20)
#define PP_RPB_PRTDHW_PRT1_RESERVED_2_LEN                           (12)
#define PP_RPB_PRTDHW_PRT1_RESERVED_2_MSK                           (0xFFF00000)
#define PP_RPB_PRTDHW_PRT1_RESERVED_2_RST                           (0x0)
#define PP_RPB_PRTDHW_PRT1_DHW_TCN_OFF                              (0)
#define PP_RPB_PRTDHW_PRT1_DHW_TCN_LEN                              (20)
#define PP_RPB_PRTDHW_PRT1_DHW_TCN_MSK                              (0x000FFFFF)
#define PP_RPB_PRTDHW_PRT1_DHW_TCN_RST                              (0x0)
/**
 * REG_IDX_ACCESS   : PP_RPB_PRTDHW_PRT1_REG_IDX
 * NUM OF REGISTERS : 4
 */
#define PP_RPB_PRTDHW_PRT1_REG_IDX(idx) \
	(PP_RPB_PRTDHW_PRT1_REG + ((idx) << 5))

/**
 * SW_REG_NAME : PP_RPB_PRTDHW_PRT2_REG
 * HW_REG_NAME : prtrpb_dhw_prt2_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19: 0][RW] - Dedicated Pool High Watermark for TC .It is
 *                 expressed in bytes.
 *
 */
#define PP_RPB_PRTDHW_PRT2_REG                         ((RPB_BASE_ADDR) + 0x108)
#define PP_RPB_PRTDHW_PRT2_RESERVED_2_OFF                           (20)
#define PP_RPB_PRTDHW_PRT2_RESERVED_2_LEN                           (12)
#define PP_RPB_PRTDHW_PRT2_RESERVED_2_MSK                           (0xFFF00000)
#define PP_RPB_PRTDHW_PRT2_RESERVED_2_RST                           (0x0)
#define PP_RPB_PRTDHW_PRT2_DHW_TCN_OFF                              (0)
#define PP_RPB_PRTDHW_PRT2_DHW_TCN_LEN                              (20)
#define PP_RPB_PRTDHW_PRT2_DHW_TCN_MSK                              (0x000FFFFF)
#define PP_RPB_PRTDHW_PRT2_DHW_TCN_RST                              (0x0)
/**
 * REG_IDX_ACCESS   : PP_RPB_PRTDHW_PRT2_REG_IDX
 * NUM OF REGISTERS : 4
 */
#define PP_RPB_PRTDHW_PRT2_REG_IDX(idx) \
	(PP_RPB_PRTDHW_PRT2_REG + ((idx) << 5))

/**
 * SW_REG_NAME : PP_RPB_PRTDHW_PRT3_REG
 * HW_REG_NAME : prtrpb_dhw_prt3_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19: 0][RW] - Dedicated Pool High Watermark for TC .It is
 *                 expressed in bytes.
 *
 */
#define PP_RPB_PRTDHW_PRT3_REG                         ((RPB_BASE_ADDR) + 0x10c)
#define PP_RPB_PRTDHW_PRT3_RESERVED_2_OFF                           (20)
#define PP_RPB_PRTDHW_PRT3_RESERVED_2_LEN                           (12)
#define PP_RPB_PRTDHW_PRT3_RESERVED_2_MSK                           (0xFFF00000)
#define PP_RPB_PRTDHW_PRT3_RESERVED_2_RST                           (0x0)
#define PP_RPB_PRTDHW_PRT3_DHW_TCN_OFF                              (0)
#define PP_RPB_PRTDHW_PRT3_DHW_TCN_LEN                              (20)
#define PP_RPB_PRTDHW_PRT3_DHW_TCN_MSK                              (0x000FFFFF)
#define PP_RPB_PRTDHW_PRT3_DHW_TCN_RST                              (0x0)
/**
 * REG_IDX_ACCESS   : PP_RPB_PRTDHW_PRT3_REG_IDX
 * NUM OF REGISTERS : 4
 */
#define PP_RPB_PRTDHW_PRT3_REG_IDX(idx) \
	(PP_RPB_PRTDHW_PRT3_REG + ((idx) << 5))

/**
 * SW_REG_NAME : PP_RPB_PRTDLW_PRT0_REG
 * HW_REG_NAME : prtrpb_dlw_prt0_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19: 0][RW] - Dedicated Pool Low Watermark for TC n.    It is
 *                 expressed in bytes.
 *
 */
#define PP_RPB_PRTDLW_PRT0_REG                         ((RPB_BASE_ADDR) + 0x220)
#define PP_RPB_PRTDLW_PRT0_RESERVED_2_OFF                           (20)
#define PP_RPB_PRTDLW_PRT0_RESERVED_2_LEN                           (12)
#define PP_RPB_PRTDLW_PRT0_RESERVED_2_MSK                           (0xFFF00000)
#define PP_RPB_PRTDLW_PRT0_RESERVED_2_RST                           (0x0)
#define PP_RPB_PRTDLW_PRT0_DLW_TCN_OFF                              (0)
#define PP_RPB_PRTDLW_PRT0_DLW_TCN_LEN                              (20)
#define PP_RPB_PRTDLW_PRT0_DLW_TCN_MSK                              (0x000FFFFF)
#define PP_RPB_PRTDLW_PRT0_DLW_TCN_RST                              (0x0)
/**
 * REG_IDX_ACCESS   : PP_RPB_PRTDLW_PRT0_REG_IDX
 * NUM OF REGISTERS : 4
 */
#define PP_RPB_PRTDLW_PRT0_REG_IDX(idx) \
	(PP_RPB_PRTDLW_PRT0_REG + ((idx) << 5))

/**
 * SW_REG_NAME : PP_RPB_PRTDLW_PRT1_REG
 * HW_REG_NAME : prtrpb_dlw_prt1_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19: 0][RW] - Dedicated Pool Low Watermark for TC n.    It is
 *                 expressed in bytes.
 *
 */
#define PP_RPB_PRTDLW_PRT1_REG                         ((RPB_BASE_ADDR) + 0x224)
#define PP_RPB_PRTDLW_PRT1_RESERVED_2_OFF                           (20)
#define PP_RPB_PRTDLW_PRT1_RESERVED_2_LEN                           (12)
#define PP_RPB_PRTDLW_PRT1_RESERVED_2_MSK                           (0xFFF00000)
#define PP_RPB_PRTDLW_PRT1_RESERVED_2_RST                           (0x0)
#define PP_RPB_PRTDLW_PRT1_DLW_TCN_OFF                              (0)
#define PP_RPB_PRTDLW_PRT1_DLW_TCN_LEN                              (20)
#define PP_RPB_PRTDLW_PRT1_DLW_TCN_MSK                              (0x000FFFFF)
#define PP_RPB_PRTDLW_PRT1_DLW_TCN_RST                              (0x0)
/**
 * REG_IDX_ACCESS   : PP_RPB_PRTDLW_PRT1_REG_IDX
 * NUM OF REGISTERS : 4
 */
#define PP_RPB_PRTDLW_PRT1_REG_IDX(idx) \
	(PP_RPB_PRTDLW_PRT1_REG + ((idx) << 5))

/**
 * SW_REG_NAME : PP_RPB_PRTDLW_PRT2_REG
 * HW_REG_NAME : prtrpb_dlw_prt2_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19: 0][RW] - Dedicated Pool Low Watermark for TC n.    It is
 *                 expressed in bytes.
 *
 */
#define PP_RPB_PRTDLW_PRT2_REG                         ((RPB_BASE_ADDR) + 0x228)
#define PP_RPB_PRTDLW_PRT2_RESERVED_2_OFF                           (20)
#define PP_RPB_PRTDLW_PRT2_RESERVED_2_LEN                           (12)
#define PP_RPB_PRTDLW_PRT2_RESERVED_2_MSK                           (0xFFF00000)
#define PP_RPB_PRTDLW_PRT2_RESERVED_2_RST                           (0x0)
#define PP_RPB_PRTDLW_PRT2_DLW_TCN_OFF                              (0)
#define PP_RPB_PRTDLW_PRT2_DLW_TCN_LEN                              (20)
#define PP_RPB_PRTDLW_PRT2_DLW_TCN_MSK                              (0x000FFFFF)
#define PP_RPB_PRTDLW_PRT2_DLW_TCN_RST                              (0x0)
/**
 * REG_IDX_ACCESS   : PP_RPB_PRTDLW_PRT2_REG_IDX
 * NUM OF REGISTERS : 4
 */
#define PP_RPB_PRTDLW_PRT2_REG_IDX(idx) \
	(PP_RPB_PRTDLW_PRT2_REG + ((idx) << 5))

/**
 * SW_REG_NAME : PP_RPB_PRTDLW_PRT3_REG
 * HW_REG_NAME : prtrpb_dlw_prt3_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19: 0][RW] - Dedicated Pool Low Watermark for TC n.    It is
 *                 expressed in bytes.
 *
 */
#define PP_RPB_PRTDLW_PRT3_REG                         ((RPB_BASE_ADDR) + 0x22c)
#define PP_RPB_PRTDLW_PRT3_RESERVED_2_OFF                           (20)
#define PP_RPB_PRTDLW_PRT3_RESERVED_2_LEN                           (12)
#define PP_RPB_PRTDLW_PRT3_RESERVED_2_MSK                           (0xFFF00000)
#define PP_RPB_PRTDLW_PRT3_RESERVED_2_RST                           (0x0)
#define PP_RPB_PRTDLW_PRT3_DLW_TCN_OFF                              (0)
#define PP_RPB_PRTDLW_PRT3_DLW_TCN_LEN                              (20)
#define PP_RPB_PRTDLW_PRT3_DLW_TCN_MSK                              (0x000FFFFF)
#define PP_RPB_PRTDLW_PRT3_DLW_TCN_RST                              (0x0)
/**
 * REG_IDX_ACCESS   : PP_RPB_PRTDLW_PRT3_REG_IDX
 * NUM OF REGISTERS : 4
 */
#define PP_RPB_PRTDLW_PRT3_REG_IDX(idx) \
	(PP_RPB_PRTDLW_PRT3_REG + ((idx) << 5))

/**
 * SW_REG_NAME : PP_RPB_PRTDPS_PRT0_REG
 * HW_REG_NAME : prtrpb_dps_prt0_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19: 0][RW] - Dedicated Pool Size for TC n.    It is expressed
 *                 in bytes.
 *
 */
#define PP_RPB_PRTDPS_PRT0_REG                         ((RPB_BASE_ADDR) + 0x320)
#define PP_RPB_PRTDPS_PRT0_RESERVED_2_OFF                           (20)
#define PP_RPB_PRTDPS_PRT0_RESERVED_2_LEN                           (12)
#define PP_RPB_PRTDPS_PRT0_RESERVED_2_MSK                           (0xFFF00000)
#define PP_RPB_PRTDPS_PRT0_RESERVED_2_RST                           (0x0)
#define PP_RPB_PRTDPS_PRT0_DPS_TCN_OFF                              (0)
#define PP_RPB_PRTDPS_PRT0_DPS_TCN_LEN                              (20)
#define PP_RPB_PRTDPS_PRT0_DPS_TCN_MSK                              (0x000FFFFF)
#define PP_RPB_PRTDPS_PRT0_DPS_TCN_RST                              (0x0)
/**
 * REG_IDX_ACCESS   : PP_RPB_PRTDPS_PRT0_REG_IDX
 * NUM OF REGISTERS : 4
 */
#define PP_RPB_PRTDPS_PRT0_REG_IDX(idx) \
	(PP_RPB_PRTDPS_PRT0_REG + ((idx) << 5))

/**
 * SW_REG_NAME : PP_RPB_PRTDPS_PRT1_REG
 * HW_REG_NAME : prtrpb_dps_prt1_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19: 0][RW] - Dedicated Pool Size for TC n.    It is expressed
 *                 in bytes.
 *
 */
#define PP_RPB_PRTDPS_PRT1_REG                         ((RPB_BASE_ADDR) + 0x324)
#define PP_RPB_PRTDPS_PRT1_RESERVED_2_OFF                           (20)
#define PP_RPB_PRTDPS_PRT1_RESERVED_2_LEN                           (12)
#define PP_RPB_PRTDPS_PRT1_RESERVED_2_MSK                           (0xFFF00000)
#define PP_RPB_PRTDPS_PRT1_RESERVED_2_RST                           (0x0)
#define PP_RPB_PRTDPS_PRT1_DPS_TCN_OFF                              (0)
#define PP_RPB_PRTDPS_PRT1_DPS_TCN_LEN                              (20)
#define PP_RPB_PRTDPS_PRT1_DPS_TCN_MSK                              (0x000FFFFF)
#define PP_RPB_PRTDPS_PRT1_DPS_TCN_RST                              (0x0)
/**
 * REG_IDX_ACCESS   : PP_RPB_PRTDPS_PRT1_REG_IDX
 * NUM OF REGISTERS : 4
 */
#define PP_RPB_PRTDPS_PRT1_REG_IDX(idx) \
	(PP_RPB_PRTDPS_PRT1_REG + ((idx) << 5))

/**
 * SW_REG_NAME : PP_RPB_PRTDPS_PRT2_REG
 * HW_REG_NAME : prtrpb_dps_prt2_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19: 0][RW] - Dedicated Pool Size for TC n.    It is expressed
 *                 in bytes.
 *
 */
#define PP_RPB_PRTDPS_PRT2_REG                         ((RPB_BASE_ADDR) + 0x328)
#define PP_RPB_PRTDPS_PRT2_RESERVED_2_OFF                           (20)
#define PP_RPB_PRTDPS_PRT2_RESERVED_2_LEN                           (12)
#define PP_RPB_PRTDPS_PRT2_RESERVED_2_MSK                           (0xFFF00000)
#define PP_RPB_PRTDPS_PRT2_RESERVED_2_RST                           (0x0)
#define PP_RPB_PRTDPS_PRT2_DPS_TCN_OFF                              (0)
#define PP_RPB_PRTDPS_PRT2_DPS_TCN_LEN                              (20)
#define PP_RPB_PRTDPS_PRT2_DPS_TCN_MSK                              (0x000FFFFF)
#define PP_RPB_PRTDPS_PRT2_DPS_TCN_RST                              (0x0)
/**
 * REG_IDX_ACCESS   : PP_RPB_PRTDPS_PRT2_REG_IDX
 * NUM OF REGISTERS : 4
 */
#define PP_RPB_PRTDPS_PRT2_REG_IDX(idx) \
	(PP_RPB_PRTDPS_PRT2_REG + ((idx) << 5))

/**
 * SW_REG_NAME : PP_RPB_PRTDPS_PRT3_REG
 * HW_REG_NAME : prtrpb_dps_prt3_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19: 0][RW] - Dedicated Pool Size for TC n.    It is expressed
 *                 in bytes.
 *
 */
#define PP_RPB_PRTDPS_PRT3_REG                         ((RPB_BASE_ADDR) + 0x32c)
#define PP_RPB_PRTDPS_PRT3_RESERVED_2_OFF                           (20)
#define PP_RPB_PRTDPS_PRT3_RESERVED_2_LEN                           (12)
#define PP_RPB_PRTDPS_PRT3_RESERVED_2_MSK                           (0xFFF00000)
#define PP_RPB_PRTDPS_PRT3_RESERVED_2_RST                           (0x0)
#define PP_RPB_PRTDPS_PRT3_DPS_TCN_OFF                              (0)
#define PP_RPB_PRTDPS_PRT3_DPS_TCN_LEN                              (20)
#define PP_RPB_PRTDPS_PRT3_DPS_TCN_MSK                              (0x000FFFFF)
#define PP_RPB_PRTDPS_PRT3_DPS_TCN_RST                              (0x0)
/**
 * REG_IDX_ACCESS   : PP_RPB_PRTDPS_PRT3_REG_IDX
 * NUM OF REGISTERS : 4
 */
#define PP_RPB_PRTDPS_PRT3_REG_IDX(idx) \
	(PP_RPB_PRTDPS_PRT3_REG + ((idx) << 5))

/**
 * SW_REG_NAME : PP_RPB_PRTSHT_PRT0_REG
 * HW_REG_NAME : prtrpb_sht_prt0_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19: 0][RW] - Shared Pool High Threshold for TC .It is expressed
 *                 in bytes.
 *
 */
#define PP_RPB_PRTSHT_PRT0_REG                         ((RPB_BASE_ADDR) + 0x480)
#define PP_RPB_PRTSHT_PRT0_RESERVED_2_OFF                           (20)
#define PP_RPB_PRTSHT_PRT0_RESERVED_2_LEN                           (12)
#define PP_RPB_PRTSHT_PRT0_RESERVED_2_MSK                           (0xFFF00000)
#define PP_RPB_PRTSHT_PRT0_RESERVED_2_RST                           (0x0)
#define PP_RPB_PRTSHT_PRT0_SHT_TCN_OFF                              (0)
#define PP_RPB_PRTSHT_PRT0_SHT_TCN_LEN                              (20)
#define PP_RPB_PRTSHT_PRT0_SHT_TCN_MSK                              (0x000FFFFF)
#define PP_RPB_PRTSHT_PRT0_SHT_TCN_RST                              (0x10000)
/**
 * REG_IDX_ACCESS   : PP_RPB_PRTSHT_PRT0_REG_IDX
 * NUM OF REGISTERS : 4
 */
#define PP_RPB_PRTSHT_PRT0_REG_IDX(idx) \
	(PP_RPB_PRTSHT_PRT0_REG + ((idx) << 5))

/**
 * SW_REG_NAME : PP_RPB_PRTSHT_PRT1_REG
 * HW_REG_NAME : prtrpb_sht_prt1_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19: 0][RW] - Shared Pool High Threshold for TC .It is expressed
 *                 in bytes.
 *
 */
#define PP_RPB_PRTSHT_PRT1_REG                         ((RPB_BASE_ADDR) + 0x484)
#define PP_RPB_PRTSHT_PRT1_RESERVED_2_OFF                           (20)
#define PP_RPB_PRTSHT_PRT1_RESERVED_2_LEN                           (12)
#define PP_RPB_PRTSHT_PRT1_RESERVED_2_MSK                           (0xFFF00000)
#define PP_RPB_PRTSHT_PRT1_RESERVED_2_RST                           (0x0)
#define PP_RPB_PRTSHT_PRT1_SHT_TCN_OFF                              (0)
#define PP_RPB_PRTSHT_PRT1_SHT_TCN_LEN                              (20)
#define PP_RPB_PRTSHT_PRT1_SHT_TCN_MSK                              (0x000FFFFF)
#define PP_RPB_PRTSHT_PRT1_SHT_TCN_RST                              (0x10000)
/**
 * REG_IDX_ACCESS   : PP_RPB_PRTSHT_PRT1_REG_IDX
 * NUM OF REGISTERS : 4
 */
#define PP_RPB_PRTSHT_PRT1_REG_IDX(idx) \
	(PP_RPB_PRTSHT_PRT1_REG + ((idx) << 5))

/**
 * SW_REG_NAME : PP_RPB_PRTSHT_PRT2_REG
 * HW_REG_NAME : prtrpb_sht_prt2_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19: 0][RW] - Shared Pool High Threshold for TC .It is expressed
 *                 in bytes.
 *
 */
#define PP_RPB_PRTSHT_PRT2_REG                         ((RPB_BASE_ADDR) + 0x488)
#define PP_RPB_PRTSHT_PRT2_RESERVED_2_OFF                           (20)
#define PP_RPB_PRTSHT_PRT2_RESERVED_2_LEN                           (12)
#define PP_RPB_PRTSHT_PRT2_RESERVED_2_MSK                           (0xFFF00000)
#define PP_RPB_PRTSHT_PRT2_RESERVED_2_RST                           (0x0)
#define PP_RPB_PRTSHT_PRT2_SHT_TCN_OFF                              (0)
#define PP_RPB_PRTSHT_PRT2_SHT_TCN_LEN                              (20)
#define PP_RPB_PRTSHT_PRT2_SHT_TCN_MSK                              (0x000FFFFF)
#define PP_RPB_PRTSHT_PRT2_SHT_TCN_RST                              (0x10000)
/**
 * REG_IDX_ACCESS   : PP_RPB_PRTSHT_PRT2_REG_IDX
 * NUM OF REGISTERS : 4
 */
#define PP_RPB_PRTSHT_PRT2_REG_IDX(idx) \
	(PP_RPB_PRTSHT_PRT2_REG + ((idx) << 5))

/**
 * SW_REG_NAME : PP_RPB_PRTSHT_PRT3_REG
 * HW_REG_NAME : prtrpb_sht_prt3_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19: 0][RW] - Shared Pool High Threshold for TC .It is expressed
 *                 in bytes.
 *
 */
#define PP_RPB_PRTSHT_PRT3_REG                         ((RPB_BASE_ADDR) + 0x48c)
#define PP_RPB_PRTSHT_PRT3_RESERVED_2_OFF                           (20)
#define PP_RPB_PRTSHT_PRT3_RESERVED_2_LEN                           (12)
#define PP_RPB_PRTSHT_PRT3_RESERVED_2_MSK                           (0xFFF00000)
#define PP_RPB_PRTSHT_PRT3_RESERVED_2_RST                           (0x0)
#define PP_RPB_PRTSHT_PRT3_SHT_TCN_OFF                              (0)
#define PP_RPB_PRTSHT_PRT3_SHT_TCN_LEN                              (20)
#define PP_RPB_PRTSHT_PRT3_SHT_TCN_MSK                              (0x000FFFFF)
#define PP_RPB_PRTSHT_PRT3_SHT_TCN_RST                              (0x10000)
/**
 * REG_IDX_ACCESS   : PP_RPB_PRTSHT_PRT3_REG_IDX
 * NUM OF REGISTERS : 4
 */
#define PP_RPB_PRTSHT_PRT3_REG_IDX(idx) \
	(PP_RPB_PRTSHT_PRT3_REG + ((idx) << 5))

/**
 * SW_REG_NAME : PP_RPB_PRTSLT_PRT0_REG
 * HW_REG_NAME : prtrpb_slt_prt0_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19: 0][RW] - Shared Pool Low Threshold for TC .It is expressed
 *                 in bytes.
 *
 */
#define PP_RPB_PRTSLT_PRT0_REG                         ((RPB_BASE_ADDR) + 0x5a0)
#define PP_RPB_PRTSLT_PRT0_RESERVED_2_OFF                           (20)
#define PP_RPB_PRTSLT_PRT0_RESERVED_2_LEN                           (12)
#define PP_RPB_PRTSLT_PRT0_RESERVED_2_MSK                           (0xFFF00000)
#define PP_RPB_PRTSLT_PRT0_RESERVED_2_RST                           (0x0)
#define PP_RPB_PRTSLT_PRT0_SLT_TCN_OFF                              (0)
#define PP_RPB_PRTSLT_PRT0_SLT_TCN_LEN                              (20)
#define PP_RPB_PRTSLT_PRT0_SLT_TCN_MSK                              (0x000FFFFF)
#define PP_RPB_PRTSLT_PRT0_SLT_TCN_RST                              (0x0)
/**
 * REG_IDX_ACCESS   : PP_RPB_PRTSLT_PRT0_REG_IDX
 * NUM OF REGISTERS : 4
 */
#define PP_RPB_PRTSLT_PRT0_REG_IDX(idx) \
	(PP_RPB_PRTSLT_PRT0_REG + ((idx) << 5))

/**
 * SW_REG_NAME : PP_RPB_PRTSLT_PRT1_REG
 * HW_REG_NAME : prtrpb_slt_prt1_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19: 0][RW] - Shared Pool Low Threshold for TC .It is expressed
 *                 in bytes.
 *
 */
#define PP_RPB_PRTSLT_PRT1_REG                         ((RPB_BASE_ADDR) + 0x5a4)
#define PP_RPB_PRTSLT_PRT1_RESERVED_2_OFF                           (20)
#define PP_RPB_PRTSLT_PRT1_RESERVED_2_LEN                           (12)
#define PP_RPB_PRTSLT_PRT1_RESERVED_2_MSK                           (0xFFF00000)
#define PP_RPB_PRTSLT_PRT1_RESERVED_2_RST                           (0x0)
#define PP_RPB_PRTSLT_PRT1_SLT_TCN_OFF                              (0)
#define PP_RPB_PRTSLT_PRT1_SLT_TCN_LEN                              (20)
#define PP_RPB_PRTSLT_PRT1_SLT_TCN_MSK                              (0x000FFFFF)
#define PP_RPB_PRTSLT_PRT1_SLT_TCN_RST                              (0x0)
/**
 * REG_IDX_ACCESS   : PP_RPB_PRTSLT_PRT1_REG_IDX
 * NUM OF REGISTERS : 4
 */
#define PP_RPB_PRTSLT_PRT1_REG_IDX(idx) \
	(PP_RPB_PRTSLT_PRT1_REG + ((idx) << 5))

/**
 * SW_REG_NAME : PP_RPB_PRTSLT_PRT2_REG
 * HW_REG_NAME : prtrpb_slt_prt2_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19: 0][RW] - Shared Pool Low Threshold for TC .It is expressed
 *                 in bytes.
 *
 */
#define PP_RPB_PRTSLT_PRT2_REG                         ((RPB_BASE_ADDR) + 0x5a8)
#define PP_RPB_PRTSLT_PRT2_RESERVED_2_OFF                           (20)
#define PP_RPB_PRTSLT_PRT2_RESERVED_2_LEN                           (12)
#define PP_RPB_PRTSLT_PRT2_RESERVED_2_MSK                           (0xFFF00000)
#define PP_RPB_PRTSLT_PRT2_RESERVED_2_RST                           (0x0)
#define PP_RPB_PRTSLT_PRT2_SLT_TCN_OFF                              (0)
#define PP_RPB_PRTSLT_PRT2_SLT_TCN_LEN                              (20)
#define PP_RPB_PRTSLT_PRT2_SLT_TCN_MSK                              (0x000FFFFF)
#define PP_RPB_PRTSLT_PRT2_SLT_TCN_RST                              (0x0)
/**
 * REG_IDX_ACCESS   : PP_RPB_PRTSLT_PRT2_REG_IDX
 * NUM OF REGISTERS : 4
 */
#define PP_RPB_PRTSLT_PRT2_REG_IDX(idx) \
	(PP_RPB_PRTSLT_PRT2_REG + ((idx) << 5))

/**
 * SW_REG_NAME : PP_RPB_PRTSLT_PRT3_REG
 * HW_REG_NAME : prtrpb_slt_prt3_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19: 0][RW] - Shared Pool Low Threshold for TC .It is expressed
 *                 in bytes.
 *
 */
#define PP_RPB_PRTSLT_PRT3_REG                         ((RPB_BASE_ADDR) + 0x5ac)
#define PP_RPB_PRTSLT_PRT3_RESERVED_2_OFF                           (20)
#define PP_RPB_PRTSLT_PRT3_RESERVED_2_LEN                           (12)
#define PP_RPB_PRTSLT_PRT3_RESERVED_2_MSK                           (0xFFF00000)
#define PP_RPB_PRTSLT_PRT3_RESERVED_2_RST                           (0x0)
#define PP_RPB_PRTSLT_PRT3_SLT_TCN_OFF                              (0)
#define PP_RPB_PRTSLT_PRT3_SLT_TCN_LEN                              (20)
#define PP_RPB_PRTSLT_PRT3_SLT_TCN_MSK                              (0x000FFFFF)
#define PP_RPB_PRTSLT_PRT3_SLT_TCN_RST                              (0x0)
/**
 * REG_IDX_ACCESS   : PP_RPB_PRTSLT_PRT3_REG_IDX
 * NUM OF REGISTERS : 4
 */
#define PP_RPB_PRTSLT_PRT3_REG_IDX(idx) \
	(PP_RPB_PRTSLT_PRT3_REG + ((idx) << 5))

/**
 * SW_REG_NAME : PP_RPB_PRTSOC_PRT0_REG
 * HW_REG_NAME : prtrpb_soc_prt0_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19: 0][RO] - Shared Pool Occupancy Counter for TC .It is
 *                 expressed in bytes.
 *
 */
#define PP_RPB_PRTSOC_PRT0_REG                         ((RPB_BASE_ADDR) + 0x6c0)
#define PP_RPB_PRTSOC_PRT0_RESERVED_2_OFF                           (20)
#define PP_RPB_PRTSOC_PRT0_RESERVED_2_LEN                           (12)
#define PP_RPB_PRTSOC_PRT0_RESERVED_2_MSK                           (0xFFF00000)
#define PP_RPB_PRTSOC_PRT0_RESERVED_2_RST                           (0x0)
#define PP_RPB_PRTSOC_PRT0_SOC_TCN_OFF                              (0)
#define PP_RPB_PRTSOC_PRT0_SOC_TCN_LEN                              (20)
#define PP_RPB_PRTSOC_PRT0_SOC_TCN_MSK                              (0x000FFFFF)
#define PP_RPB_PRTSOC_PRT0_SOC_TCN_RST                              (0x0)
/**
 * REG_IDX_ACCESS   : PP_RPB_PRTSOC_PRT0_REG_IDX
 * NUM OF REGISTERS : 4
 */
#define PP_RPB_PRTSOC_PRT0_REG_IDX(idx) \
	(PP_RPB_PRTSOC_PRT0_REG + ((idx) << 5))

/**
 * SW_REG_NAME : PP_RPB_PRTSOC_PRT1_REG
 * HW_REG_NAME : prtrpb_soc_prt1_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19: 0][RO] - Shared Pool Occupancy Counter for TC .It is
 *                 expressed in bytes.
 *
 */
#define PP_RPB_PRTSOC_PRT1_REG                         ((RPB_BASE_ADDR) + 0x6c4)
#define PP_RPB_PRTSOC_PRT1_RESERVED_2_OFF                           (20)
#define PP_RPB_PRTSOC_PRT1_RESERVED_2_LEN                           (12)
#define PP_RPB_PRTSOC_PRT1_RESERVED_2_MSK                           (0xFFF00000)
#define PP_RPB_PRTSOC_PRT1_RESERVED_2_RST                           (0x0)
#define PP_RPB_PRTSOC_PRT1_SOC_TCN_OFF                              (0)
#define PP_RPB_PRTSOC_PRT1_SOC_TCN_LEN                              (20)
#define PP_RPB_PRTSOC_PRT1_SOC_TCN_MSK                              (0x000FFFFF)
#define PP_RPB_PRTSOC_PRT1_SOC_TCN_RST                              (0x0)
/**
 * REG_IDX_ACCESS   : PP_RPB_PRTSOC_PRT1_REG_IDX
 * NUM OF REGISTERS : 4
 */
#define PP_RPB_PRTSOC_PRT1_REG_IDX(idx) \
	(PP_RPB_PRTSOC_PRT1_REG + ((idx) << 5))

/**
 * SW_REG_NAME : PP_RPB_PRTSOC_PRT2_REG
 * HW_REG_NAME : prtrpb_soc_prt2_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19: 0][RO] - Shared Pool Occupancy Counter for TC .It is
 *                 expressed in bytes.
 *
 */
#define PP_RPB_PRTSOC_PRT2_REG                         ((RPB_BASE_ADDR) + 0x6c8)
#define PP_RPB_PRTSOC_PRT2_RESERVED_2_OFF                           (20)
#define PP_RPB_PRTSOC_PRT2_RESERVED_2_LEN                           (12)
#define PP_RPB_PRTSOC_PRT2_RESERVED_2_MSK                           (0xFFF00000)
#define PP_RPB_PRTSOC_PRT2_RESERVED_2_RST                           (0x0)
#define PP_RPB_PRTSOC_PRT2_SOC_TCN_OFF                              (0)
#define PP_RPB_PRTSOC_PRT2_SOC_TCN_LEN                              (20)
#define PP_RPB_PRTSOC_PRT2_SOC_TCN_MSK                              (0x000FFFFF)
#define PP_RPB_PRTSOC_PRT2_SOC_TCN_RST                              (0x0)
/**
 * REG_IDX_ACCESS   : PP_RPB_PRTSOC_PRT2_REG_IDX
 * NUM OF REGISTERS : 4
 */
#define PP_RPB_PRTSOC_PRT2_REG_IDX(idx) \
	(PP_RPB_PRTSOC_PRT2_REG + ((idx) << 5))

/**
 * SW_REG_NAME : PP_RPB_PRTSOC_PRT3_REG
 * HW_REG_NAME : prtrpb_soc_prt3_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19: 0][RO] - Shared Pool Occupancy Counter for TC .It is
 *                 expressed in bytes.
 *
 */
#define PP_RPB_PRTSOC_PRT3_REG                         ((RPB_BASE_ADDR) + 0x6cc)
#define PP_RPB_PRTSOC_PRT3_RESERVED_2_OFF                           (20)
#define PP_RPB_PRTSOC_PRT3_RESERVED_2_LEN                           (12)
#define PP_RPB_PRTSOC_PRT3_RESERVED_2_MSK                           (0xFFF00000)
#define PP_RPB_PRTSOC_PRT3_RESERVED_2_RST                           (0x0)
#define PP_RPB_PRTSOC_PRT3_SOC_TCN_OFF                              (0)
#define PP_RPB_PRTSOC_PRT3_SOC_TCN_LEN                              (20)
#define PP_RPB_PRTSOC_PRT3_SOC_TCN_MSK                              (0x000FFFFF)
#define PP_RPB_PRTSOC_PRT3_SOC_TCN_RST                              (0x0)
/**
 * REG_IDX_ACCESS   : PP_RPB_PRTSOC_PRT3_REG_IDX
 * NUM OF REGISTERS : 4
 */
#define PP_RPB_PRTSOC_PRT3_REG_IDX(idx) \
	(PP_RPB_PRTSOC_PRT3_REG + ((idx) << 5))

/**
 * SW_REG_NAME : PP_RPB_DATA_PIPE_MEM_CFG_REG
 * HW_REG_NAME : rpb_data_pipe_mem_cfg_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:20][RO] - MISSING_DESCRIPTION
 *   [19:16][RW] - Read-Write margin Input for port A. This input is
 *                 used for setting the Read-Write margin. It
 *                 programs  the sense amp differential setting. and
 *                 allows the trade off between speed and robustness.
 *                 RM[1:0] = 2'b00 is the slowest possible mode of
 *                 operation for the memory. This setting is required
 *                 for VDDMIN operation.    RM[1:0] values control
 *                 access time and cycle time of the memory. Refer to
 *                 the timing table for more  details.    RM[3:2] are
 *                 factory pins reserved for debug mode. User should
 *                 tie these pins to logic 0.
 *   [15:13][RO] - MISSING_DESCRIPTION
 *   [12:12][RW] - Read-Write margin Enable Input for port A. This
 *                 selects between the default RM setting, and the RM
 *                 settting.
 *   [11:10][RO] - MISSING_DESCRIPTION
 *   [ 9: 9][RW] - Count ECC un-correctable error events
 *   [ 8: 8][RW] - Count ECC correctable error events
 *   [ 7: 6][RO] - MISSING_DESCRIPTION
 *   [ 5: 5][RW] - Mask ECC un-correctable error interrupt
 *                 generation.
 *   [ 4: 4][RW] - Light Sleep mechanism bypass. '1' - prevent the
 *                 memory from entering light-sleep mode.
 *   [ 3: 3][RW] - Light Sleep Force    Force the entrence of the
 *                 memory to the Light Sleep mode.
 *   [ 2: 2][RW] - ECC Invert for testing
 *   [ 1: 1][RW] - ECC Invert for testing
 *   [ 0: 0][RW] - ECC mechanism enable. '1' - Enabled. '0' -
 *                 Disabled
 *
 */
#define PP_RPB_DATA_PIPE_MEM_CFG_REG                   ((RPB_BASE_ADDR) + 0x898)
#define PP_RPB_DATA_PIPE_MEM_CFG_RESERVED_14_OFF                    (20)
#define PP_RPB_DATA_PIPE_MEM_CFG_RESERVED_14_LEN                    (12)
#define PP_RPB_DATA_PIPE_MEM_CFG_RESERVED_14_MSK                    (0xFFF00000)
#define PP_RPB_DATA_PIPE_MEM_CFG_RESERVED_14_RST                    (0x0)
#define PP_RPB_DATA_PIPE_MEM_CFG_RM_OFF                             (16)
#define PP_RPB_DATA_PIPE_MEM_CFG_RM_LEN                             (4)
#define PP_RPB_DATA_PIPE_MEM_CFG_RM_MSK                             (0x000F0000)
#define PP_RPB_DATA_PIPE_MEM_CFG_RM_RST                             (0x2)
#define PP_RPB_DATA_PIPE_MEM_CFG_RESERVED_12_OFF                    (13)
#define PP_RPB_DATA_PIPE_MEM_CFG_RESERVED_12_LEN                    (3)
#define PP_RPB_DATA_PIPE_MEM_CFG_RESERVED_12_MSK                    (0x0000E000)
#define PP_RPB_DATA_PIPE_MEM_CFG_RESERVED_12_RST                    (0x0)
#define PP_RPB_DATA_PIPE_MEM_CFG_RME_OFF                            (12)
#define PP_RPB_DATA_PIPE_MEM_CFG_RME_LEN                            (1)
#define PP_RPB_DATA_PIPE_MEM_CFG_RME_MSK                            (0x00001000)
#define PP_RPB_DATA_PIPE_MEM_CFG_RME_RST                            (0x0)
#define PP_RPB_DATA_PIPE_MEM_CFG_RESERVED_10_OFF                    (10)
#define PP_RPB_DATA_PIPE_MEM_CFG_RESERVED_10_LEN                    (2)
#define PP_RPB_DATA_PIPE_MEM_CFG_RESERVED_10_MSK                    (0x00000C00)
#define PP_RPB_DATA_PIPE_MEM_CFG_RESERVED_10_RST                    (0x0)
#define PP_RPB_DATA_PIPE_MEM_CFG_ERR_CNT_OFF                        (9)
#define PP_RPB_DATA_PIPE_MEM_CFG_ERR_CNT_LEN                        (1)
#define PP_RPB_DATA_PIPE_MEM_CFG_ERR_CNT_MSK                        (0x00000200)
#define PP_RPB_DATA_PIPE_MEM_CFG_ERR_CNT_RST                        (0x1)
#define PP_RPB_DATA_PIPE_MEM_CFG_FIX_CNT_OFF                        (8)
#define PP_RPB_DATA_PIPE_MEM_CFG_FIX_CNT_LEN                        (1)
#define PP_RPB_DATA_PIPE_MEM_CFG_FIX_CNT_MSK                        (0x00000100)
#define PP_RPB_DATA_PIPE_MEM_CFG_FIX_CNT_RST                        (0x1)
#define PP_RPB_DATA_PIPE_MEM_CFG_RESERVED_7_OFF                     (6)
#define PP_RPB_DATA_PIPE_MEM_CFG_RESERVED_7_LEN                     (2)
#define PP_RPB_DATA_PIPE_MEM_CFG_RESERVED_7_MSK                     (0x000000C0)
#define PP_RPB_DATA_PIPE_MEM_CFG_RESERVED_7_RST                     (0x0)
#define PP_RPB_DATA_PIPE_MEM_CFG_MASK_INT_OFF                       (5)
#define PP_RPB_DATA_PIPE_MEM_CFG_MASK_INT_LEN                       (1)
#define PP_RPB_DATA_PIPE_MEM_CFG_MASK_INT_MSK                       (0x00000020)
#define PP_RPB_DATA_PIPE_MEM_CFG_MASK_INT_RST                       (0x0)
#define PP_RPB_DATA_PIPE_MEM_CFG_LS_BYPASS_OFF                      (4)
#define PP_RPB_DATA_PIPE_MEM_CFG_LS_BYPASS_LEN                      (1)
#define PP_RPB_DATA_PIPE_MEM_CFG_LS_BYPASS_MSK                      (0x00000010)
#define PP_RPB_DATA_PIPE_MEM_CFG_LS_BYPASS_RST                      (0x1)
#define PP_RPB_DATA_PIPE_MEM_CFG_LS_FORCE_OFF                       (3)
#define PP_RPB_DATA_PIPE_MEM_CFG_LS_FORCE_LEN                       (1)
#define PP_RPB_DATA_PIPE_MEM_CFG_LS_FORCE_MSK                       (0x00000008)
#define PP_RPB_DATA_PIPE_MEM_CFG_LS_FORCE_RST                       (0x0)
#define PP_RPB_DATA_PIPE_MEM_CFG_ECC_INVERT_2_OFF                   (2)
#define PP_RPB_DATA_PIPE_MEM_CFG_ECC_INVERT_2_LEN                   (1)
#define PP_RPB_DATA_PIPE_MEM_CFG_ECC_INVERT_2_MSK                   (0x00000004)
#define PP_RPB_DATA_PIPE_MEM_CFG_ECC_INVERT_2_RST                   (0x0)
#define PP_RPB_DATA_PIPE_MEM_CFG_ECC_INVERT_1_OFF                   (1)
#define PP_RPB_DATA_PIPE_MEM_CFG_ECC_INVERT_1_LEN                   (1)
#define PP_RPB_DATA_PIPE_MEM_CFG_ECC_INVERT_1_MSK                   (0x00000002)
#define PP_RPB_DATA_PIPE_MEM_CFG_ECC_INVERT_1_RST                   (0x0)
#define PP_RPB_DATA_PIPE_MEM_CFG_ECC_EN_OFF                         (0)
#define PP_RPB_DATA_PIPE_MEM_CFG_ECC_EN_LEN                         (1)
#define PP_RPB_DATA_PIPE_MEM_CFG_ECC_EN_MSK                         (0x00000001)
#define PP_RPB_DATA_PIPE_MEM_CFG_ECC_EN_RST                         (0x1)
/**
 * REG_IDX_ACCESS   : PP_RPB_DATA_PIPE_MEM_CFG_REG_IDX
 * NUM OF REGISTERS : 4
 */
#define PP_RPB_DATA_PIPE_MEM_CFG_REG_IDX(idx) \
	(PP_RPB_DATA_PIPE_MEM_CFG_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_RPB_DATA_PIPE_MEM_STATUS_REG
 * HW_REG_NAME : rpb_data_pipe_mem_status_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 4][RO] - MISSING_DESCRIPTION
 *   [ 3: 3][RO] - All block's memories initialization completed
 *                 indication.
 *   [ 2: 2][RO] - Memory initialization completed indication.
 *   [ 1: 1][RO] - ECC correctable error indication (cleared on read)
 *   [ 0: 0][RO] - ECC un-correctable error indication (cleared on
 *                 read)
 *
 */
#define PP_RPB_DATA_PIPE_MEM_STATUS_REG                ((RPB_BASE_ADDR) + 0x8b8)
#define PP_RPB_DATA_PIPE_MEM_STATUS_RESERVED_5_OFF                  (4)
#define PP_RPB_DATA_PIPE_MEM_STATUS_RESERVED_5_LEN                  (28)
#define PP_RPB_DATA_PIPE_MEM_STATUS_RESERVED_5_MSK                  (0xFFFFFFF0)
#define PP_RPB_DATA_PIPE_MEM_STATUS_RESERVED_5_RST                  (0x0)
#define PP_RPB_DATA_PIPE_MEM_STATUS_GLOBAL_INIT_DONE_OFF            (3)
#define PP_RPB_DATA_PIPE_MEM_STATUS_GLOBAL_INIT_DONE_LEN            (1)
#define PP_RPB_DATA_PIPE_MEM_STATUS_GLOBAL_INIT_DONE_MSK            (0x00000008)
#define PP_RPB_DATA_PIPE_MEM_STATUS_GLOBAL_INIT_DONE_RST            (0x0)
#define PP_RPB_DATA_PIPE_MEM_STATUS_INIT_DONE_OFF                   (2)
#define PP_RPB_DATA_PIPE_MEM_STATUS_INIT_DONE_LEN                   (1)
#define PP_RPB_DATA_PIPE_MEM_STATUS_INIT_DONE_MSK                   (0x00000004)
#define PP_RPB_DATA_PIPE_MEM_STATUS_INIT_DONE_RST                   (0x0)
#define PP_RPB_DATA_PIPE_MEM_STATUS_ECC_FIX_OFF                     (1)
#define PP_RPB_DATA_PIPE_MEM_STATUS_ECC_FIX_LEN                     (1)
#define PP_RPB_DATA_PIPE_MEM_STATUS_ECC_FIX_MSK                     (0x00000002)
#define PP_RPB_DATA_PIPE_MEM_STATUS_ECC_FIX_RST                     (0x0)
#define PP_RPB_DATA_PIPE_MEM_STATUS_ECC_ERR_OFF                     (0)
#define PP_RPB_DATA_PIPE_MEM_STATUS_ECC_ERR_LEN                     (1)
#define PP_RPB_DATA_PIPE_MEM_STATUS_ECC_ERR_MSK                     (0x00000001)
#define PP_RPB_DATA_PIPE_MEM_STATUS_ECC_ERR_RST                     (0x0)
/**
 * REG_IDX_ACCESS   : PP_RPB_DATA_PIPE_MEM_STATUS_REG_IDX
 * NUM OF REGISTERS : 4
 */
#define PP_RPB_DATA_PIPE_MEM_STATUS_REG_IDX(idx) \
	(PP_RPB_DATA_PIPE_MEM_STATUS_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_RPB_DBG_ACC_DATA_REG
 * HW_REG_NAME : rpb_dbg_acc_data_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 0][RO] - Indirect access read data. Relevant bits deffer
 *                 according to selected memory.
 *
 */
#define PP_RPB_DBG_ACC_DATA_REG                        ((RPB_BASE_ADDR) + 0x8ec)
#define PP_RPB_DBG_ACC_DATA_RPB_DBG_READ_DATA_OFF                   (0)
#define PP_RPB_DBG_ACC_DATA_RPB_DBG_READ_DATA_LEN                   (32)
#define PP_RPB_DBG_ACC_DATA_RPB_DBG_READ_DATA_MSK                   (0xFFFFFFFF)
#define PP_RPB_DBG_ACC_DATA_RPB_DBG_READ_DATA_RST                   (0x0)
/**
 * REG_IDX_ACCESS   : PP_RPB_DBG_ACC_DATA_REG_IDX
 * NUM OF REGISTERS : 4
 */
#define PP_RPB_DBG_ACC_DATA_REG_IDX(idx) \
	(PP_RPB_DBG_ACC_DATA_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_RPB_PPRS_ERR_CNT_REG
 * HW_REG_NAME : rpb_pprs_err_cnt_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:16][RO] - MISSING_DESCRIPTION
 *   [15: 8][RO] - Amount of valid cycles between packets which are
 *                 not status cycles.
 *   [ 7: 0][RO] - Amount of mismatches between counted packet size
 *                 to reported packet size (in status word) from PPRS
 *                 interface.    This counter will not wrap around
 *                 and will be cleared on read.
 *
 */
#define PP_RPB_PPRS_ERR_CNT_REG                        ((RPB_BASE_ADDR) + 0x910)
#define PP_RPB_PPRS_ERR_CNT_RESERVED_3_OFF                          (16)
#define PP_RPB_PPRS_ERR_CNT_RESERVED_3_LEN                          (16)
#define PP_RPB_PPRS_ERR_CNT_RESERVED_3_MSK                          (0xFFFF0000)
#define PP_RPB_PPRS_ERR_CNT_RESERVED_3_RST                          (0x0)
#define PP_RPB_PPRS_ERR_CNT_VALID_BTW_PKT_OFF                       (8)
#define PP_RPB_PPRS_ERR_CNT_VALID_BTW_PKT_LEN                       (8)
#define PP_RPB_PPRS_ERR_CNT_VALID_BTW_PKT_MSK                       (0x0000FF00)
#define PP_RPB_PPRS_ERR_CNT_VALID_BTW_PKT_RST                       (0x0)
#define PP_RPB_PPRS_ERR_CNT_PKT_SIZE_ERR_OFF                        (0)
#define PP_RPB_PPRS_ERR_CNT_PKT_SIZE_ERR_LEN                        (8)
#define PP_RPB_PPRS_ERR_CNT_PKT_SIZE_ERR_MSK                        (0x000000FF)
#define PP_RPB_PPRS_ERR_CNT_PKT_SIZE_ERR_RST                        (0x0)
/**
 * REG_IDX_ACCESS   : PP_RPB_PPRS_ERR_CNT_REG_IDX
 * NUM OF REGISTERS : 4
 */
#define PP_RPB_PPRS_ERR_CNT_REG_IDX(idx) \
	(PP_RPB_PPRS_ERR_CNT_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_RPB_PKT_INDICATIONS_REG
 * HW_REG_NAME : pkt_indications_t
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:24][RO] - Packet drop indication counter.    The counter
 *                 will wrap around and will be cleared on read.
 *   [23:16][RO] - Packet status detected events counter.    The
 *                 counter will wrap around and will be cleared on
 *                 read.
 *   [15: 8][RO] - Counts the amount of end of packet indications.
 *                 The counter will wrap around and will be cleared
 *                 on read.
 *   [ 7: 0][RO] - Counts the amount of start of packet indications.
 *                 The counter will wrap around and will be cleared
 *                 on read.
 *
 */
#define PP_RPB_PKT_INDICATIONS_REG                     ((RPB_BASE_ADDR) + 0x920)
#define PP_RPB_PKT_INDICATIONS_DROP_CNT_OFF                         (24)
#define PP_RPB_PKT_INDICATIONS_DROP_CNT_LEN                         (8)
#define PP_RPB_PKT_INDICATIONS_DROP_CNT_MSK                         (0xFF000000)
#define PP_RPB_PKT_INDICATIONS_DROP_CNT_RST                         (0x0)
#define PP_RPB_PKT_INDICATIONS_STATUS_CNT_OFF                       (16)
#define PP_RPB_PKT_INDICATIONS_STATUS_CNT_LEN                       (8)
#define PP_RPB_PKT_INDICATIONS_STATUS_CNT_MSK                       (0x00FF0000)
#define PP_RPB_PKT_INDICATIONS_STATUS_CNT_RST                       (0x0)
#define PP_RPB_PKT_INDICATIONS_END_CNT_OFF                          (8)
#define PP_RPB_PKT_INDICATIONS_END_CNT_LEN                          (8)
#define PP_RPB_PKT_INDICATIONS_END_CNT_MSK                          (0x0000FF00)
#define PP_RPB_PKT_INDICATIONS_END_CNT_RST                          (0x0)
#define PP_RPB_PKT_INDICATIONS_START_CNT_OFF                        (0)
#define PP_RPB_PKT_INDICATIONS_START_CNT_LEN                        (8)
#define PP_RPB_PKT_INDICATIONS_START_CNT_MSK                        (0x000000FF)
#define PP_RPB_PKT_INDICATIONS_START_CNT_RST                        (0x0)
/**
 * REG_IDX_ACCESS   : PP_RPB_PKT_INDICATIONS_REG_IDX
 * NUM OF REGISTERS : 4
 */
#define PP_RPB_PKT_INDICATIONS_REG_IDX(idx) \
	(PP_RPB_PKT_INDICATIONS_REG + ((idx) << 2))

#endif
