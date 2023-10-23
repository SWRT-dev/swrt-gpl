/**
 * bootcfg_regs.h
 * Description: ppv4_bootcfg_s HW registers definitions
 * Auto-generated file by RDL Parser, DO NOT CHANGE
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2019-2020 Intel Corporation
 */

#ifndef _PP_BOOTCFG_H_
#define _PP_BOOTCFG_H_

#define PP_BOOTCFG_GEN_DATE_STR               "Sunday May 17, 2020 [5:26:11 pm]"
#define PP_BOOTCFG_BASE                                          (0xF0000000ULL)

/**
 * SW_REG_NAME : PP_BOOTCFG_VERSION_REG
 * HW_REG_NAME : version
 * DESCRIPTION : ppv4 bootcfg version
 *
 *  Register Fields :
 *   [31:16][RO] - Reserved
 *   [15: 0][RO] - cls ver
 *
 */
#define PP_BOOTCFG_VERSION_REG                       ((BOOTCFG_BASE_ADDR) + 0x0)
#define PP_BOOTCFG_VERSION_RSVD0_OFF                                (16)
#define PP_BOOTCFG_VERSION_RSVD0_LEN                                (16)
#define PP_BOOTCFG_VERSION_RSVD0_MSK                                (0xFFFF0000)
#define PP_BOOTCFG_VERSION_RSVD0_RST                                (0x0)
#define PP_BOOTCFG_VERSION_PPV4_VERSION_OFF                         (0)
#define PP_BOOTCFG_VERSION_PPV4_VERSION_LEN                         (16)
#define PP_BOOTCFG_VERSION_PPV4_VERSION_MSK                         (0x0000FFFF)
#define PP_BOOTCFG_VERSION_PPV4_VERSION_RST                         (0x2)

/**
 * SW_REG_NAME : PP_BOOTCFG_CLS_CFG_REG
 * HW_REG_NAME : classifier_cache_cfg_reg
 * DESCRIPTION : classifier caches disable
 *
 *  Register Fields :
 *   [31: 5][RO] - Reserved
 *   [ 4: 4][RW] - counter_cache disable
 *   [ 3: 1][RO] - Reserved
 *   [ 0: 0][RW] - session_cache disable
 *
 */
#define PP_BOOTCFG_CLS_CFG_REG                       ((BOOTCFG_BASE_ADDR) + 0x4)
#define PP_BOOTCFG_CLS_CFG_RSVD0_OFF                                (5)
#define PP_BOOTCFG_CLS_CFG_RSVD0_LEN                                (27)
#define PP_BOOTCFG_CLS_CFG_RSVD0_MSK                                (0xFFFFFFE0)
#define PP_BOOTCFG_CLS_CFG_RSVD0_RST                                (0x0)
#define PP_BOOTCFG_CLS_CFG_CNT_CACHE_DIS_OFF                        (4)
#define PP_BOOTCFG_CLS_CFG_CNT_CACHE_DIS_LEN                        (1)
#define PP_BOOTCFG_CLS_CFG_CNT_CACHE_DIS_MSK                        (0x00000010)
#define PP_BOOTCFG_CLS_CFG_CNT_CACHE_DIS_RST                        (0x0)
#define PP_BOOTCFG_CLS_CFG_RSVD1_OFF                                (1)
#define PP_BOOTCFG_CLS_CFG_RSVD1_LEN                                (3)
#define PP_BOOTCFG_CLS_CFG_RSVD1_MSK                                (0x0000000E)
#define PP_BOOTCFG_CLS_CFG_RSVD1_RST                                (0x0)
#define PP_BOOTCFG_CLS_CFG_SESS_CACHE_DIS_OFF                       (0)
#define PP_BOOTCFG_CLS_CFG_SESS_CACHE_DIS_LEN                       (1)
#define PP_BOOTCFG_CLS_CFG_SESS_CACHE_DIS_MSK                       (0x00000001)
#define PP_BOOTCFG_CLS_CFG_SESS_CACHE_DIS_RST                       (0x0)

/**
 * SW_REG_NAME : PP_BOOTCFG_INIT_DONE_REG
 * HW_REG_NAME : init_done_reg
 * DESCRIPTION : init memory done
 *
 *  Register Fields :
 *   [31: 5][RO] - Reserved
 *   [ 4: 4][RO] - RPB init done
 *   [ 3: 1][RO] - Reserved
 *   [ 0: 0][RO] - port distr init done
 *
 */
#define PP_BOOTCFG_INIT_DONE_REG                     ((BOOTCFG_BASE_ADDR) + 0x8)
#define PP_BOOTCFG_INIT_DONE_RSVD0_OFF                              (5)
#define PP_BOOTCFG_INIT_DONE_RSVD0_LEN                              (27)
#define PP_BOOTCFG_INIT_DONE_RSVD0_MSK                              (0xFFFFFFE0)
#define PP_BOOTCFG_INIT_DONE_RSVD0_RST                              (0x0)
#define PP_BOOTCFG_INIT_DONE_RPB_INIT_DONE_OFF                      (4)
#define PP_BOOTCFG_INIT_DONE_RPB_INIT_DONE_LEN                      (1)
#define PP_BOOTCFG_INIT_DONE_RPB_INIT_DONE_MSK                      (0x00000010)
#define PP_BOOTCFG_INIT_DONE_RPB_INIT_DONE_RST                      (0x0)
#define PP_BOOTCFG_INIT_DONE_RSVD1_OFF                              (1)
#define PP_BOOTCFG_INIT_DONE_RSVD1_LEN                              (3)
#define PP_BOOTCFG_INIT_DONE_RSVD1_MSK                              (0x0000000E)
#define PP_BOOTCFG_INIT_DONE_RSVD1_RST                              (0x0)
#define PP_BOOTCFG_INIT_DONE_PORT_DISTR_INIT_DONE_OFF               (0)
#define PP_BOOTCFG_INIT_DONE_PORT_DISTR_INIT_DONE_LEN               (1)
#define PP_BOOTCFG_INIT_DONE_PORT_DISTR_INIT_DONE_MSK               (0x00000001)
#define PP_BOOTCFG_INIT_DONE_PORT_DISTR_INIT_DONE_RST               (0x0)

/**
 * SW_REG_NAME : PP_BOOTCFG_TIMERS_CLK_EN_REG
 * HW_REG_NAME : timers_clk_en_reg
 * DESCRIPTION : timers_clk_en
 *
 *  Register Fields :
 *   [31: 9][RO] - Reserved
 *   [ 8: 8][RW] - ingress timers clk enable
 *   [ 7: 1][RO] - Reserved
 *   [ 0: 0][RW] - engress timers clk enable
 *
 */
#define PP_BOOTCFG_TIMERS_CLK_EN_REG                ((BOOTCFG_BASE_ADDR) + 0x0C)
#define PP_BOOTCFG_TIMERS_CLK_EN_RSVD0_OFF                          (9)
#define PP_BOOTCFG_TIMERS_CLK_EN_RSVD0_LEN                          (23)
#define PP_BOOTCFG_TIMERS_CLK_EN_RSVD0_MSK                          (0xFFFFFE00)
#define PP_BOOTCFG_TIMERS_CLK_EN_RSVD0_RST                          (0x0)
#define PP_BOOTCFG_TIMERS_CLK_EN_UC_ING_OFF                         (8)
#define PP_BOOTCFG_TIMERS_CLK_EN_UC_ING_LEN                         (1)
#define PP_BOOTCFG_TIMERS_CLK_EN_UC_ING_MSK                         (0x00000100)
#define PP_BOOTCFG_TIMERS_CLK_EN_UC_ING_RST                         (0x1)
#define PP_BOOTCFG_TIMERS_CLK_EN_RSVD1_OFF                          (1)
#define PP_BOOTCFG_TIMERS_CLK_EN_RSVD1_LEN                          (7)
#define PP_BOOTCFG_TIMERS_CLK_EN_RSVD1_MSK                          (0x000000FE)
#define PP_BOOTCFG_TIMERS_CLK_EN_RSVD1_RST                          (0x0)
#define PP_BOOTCFG_TIMERS_CLK_EN_UC_EGR_OFF                         (0)
#define PP_BOOTCFG_TIMERS_CLK_EN_UC_EGR_LEN                         (1)
#define PP_BOOTCFG_TIMERS_CLK_EN_UC_EGR_MSK                         (0x00000001)
#define PP_BOOTCFG_TIMERS_CLK_EN_UC_EGR_RST                         (0x1)

/**
 * SW_REG_NAME : PP_BOOTCFG_NOC_EVENT_CTRL_REG
 * HW_REG_NAME : noc_event_ctrl_reg
 * DESCRIPTION : noc_event_ctrl
 *
 *  Register Fields :
 *   [31:20][RO] - Reserved
 *   [19:19][RW] - UC egress TimeOut enable, NOTE, B Step ONLY field
 *   [18:18][RW] - uC ingress TimeOut enable, NOTE, B Step ONLY field
 *   [17:17][RW] - qos TimeOut enable
 *   [16:16][RW] - ingress TimeOut enable
 *   [15:10][RO] - Reserved
 *   [ 9: 9][RW] - pp_req_ddr_nsp_prob_mainExtEvent_1
 *   [ 8: 8][RW] - pp_req_ddr_nsp_prob_mainExtEvent_0
 *   [ 7: 5][RO] - Reserved
 *   [ 4: 4][RW] - pp_resp_ddr_nsp_probe_mainExtEvent_1
 *   [ 3: 1][RO] - Reserved
 *   [ 0: 0][RW] - pp_resp_ddr_nsp_probe_mainExtEvent_0
 *
 */
#define PP_BOOTCFG_NOC_EVENT_CTRL_REG               ((BOOTCFG_BASE_ADDR) + 0x10)
#define PP_BOOTCFG_NOC_EVENT_CTRL_RSVD0_OFF                         (20)
#define PP_BOOTCFG_NOC_EVENT_CTRL_RSVD0_LEN                         (12)
#define PP_BOOTCFG_NOC_EVENT_CTRL_RSVD0_MSK                         (0xFFF00000)
#define PP_BOOTCFG_NOC_EVENT_CTRL_RSVD0_RST                         (0x0)
#define PP_BOOTCFG_NOC_EVENT_CTRL_UC_EGR_NOC_TIMEOUT_EN_OFF         (19)
#define PP_BOOTCFG_NOC_EVENT_CTRL_UC_EGR_NOC_TIMEOUT_EN_LEN         (1)
#define PP_BOOTCFG_NOC_EVENT_CTRL_UC_EGR_NOC_TIMEOUT_EN_MSK         (0x00080000)
#define PP_BOOTCFG_NOC_EVENT_CTRL_UC_EGR_NOC_TIMEOUT_EN_RST         (0x0)
#define PP_BOOTCFG_NOC_EVENT_CTRL_UC_ING_NOC_TIMEOUT_EN_OFF         (18)
#define PP_BOOTCFG_NOC_EVENT_CTRL_UC_ING_NOC_TIMEOUT_EN_LEN         (1)
#define PP_BOOTCFG_NOC_EVENT_CTRL_UC_ING_NOC_TIMEOUT_EN_MSK         (0x00040000)
#define PP_BOOTCFG_NOC_EVENT_CTRL_UC_ING_NOC_TIMEOUT_EN_RST         (0x0)
#define PP_BOOTCFG_NOC_EVENT_CTRL_QOS_NOC_TIMEOUT_EN_OFF            (17)
#define PP_BOOTCFG_NOC_EVENT_CTRL_QOS_NOC_TIMEOUT_EN_LEN            (1)
#define PP_BOOTCFG_NOC_EVENT_CTRL_QOS_NOC_TIMEOUT_EN_MSK            (0x00020000)
#define PP_BOOTCFG_NOC_EVENT_CTRL_QOS_NOC_TIMEOUT_EN_RST            (0x0)
#define PP_BOOTCFG_NOC_EVENT_CTRL_ING_NOC_TIMEOUT_EN_OFF            (16)
#define PP_BOOTCFG_NOC_EVENT_CTRL_ING_NOC_TIMEOUT_EN_LEN            (1)
#define PP_BOOTCFG_NOC_EVENT_CTRL_ING_NOC_TIMEOUT_EN_MSK            (0x00010000)
#define PP_BOOTCFG_NOC_EVENT_CTRL_ING_NOC_TIMEOUT_EN_RST            (0x0)
#define PP_BOOTCFG_NOC_EVENT_CTRL_RSVD1_OFF                         (10)
#define PP_BOOTCFG_NOC_EVENT_CTRL_RSVD1_LEN                         (6)
#define PP_BOOTCFG_NOC_EVENT_CTRL_RSVD1_MSK                         (0x0000FC00)
#define PP_BOOTCFG_NOC_EVENT_CTRL_RSVD1_RST                         (0x0)
#define PP_BOOTCFG_NOC_EVENT_CTRL_PP_REQ_DDR_NSP_PROB_MAINEXTEVENT_1_OFF     (9)
#define PP_BOOTCFG_NOC_EVENT_CTRL_PP_REQ_DDR_NSP_PROB_MAINEXTEVENT_1_LEN     (1)
#define PP_BOOTCFG_NOC_EVENT_CTRL_PP_REQ_DDR_NSP_PROB_MAINEXTEVENT_1_MSK \
	(0x00000200)
#define PP_BOOTCFG_NOC_EVENT_CTRL_PP_REQ_DDR_NSP_PROB_MAINEXTEVENT_1_RST   (0x0)
#define PP_BOOTCFG_NOC_EVENT_CTRL_PP_REQ_DDR_NSP_PROB_MAINEXTEVENT_0_OFF     (8)
#define PP_BOOTCFG_NOC_EVENT_CTRL_PP_REQ_DDR_NSP_PROB_MAINEXTEVENT_0_LEN     (1)
#define PP_BOOTCFG_NOC_EVENT_CTRL_PP_REQ_DDR_NSP_PROB_MAINEXTEVENT_0_MSK \
	(0x00000100)
#define PP_BOOTCFG_NOC_EVENT_CTRL_PP_REQ_DDR_NSP_PROB_MAINEXTEVENT_0_RST   (0x0)
#define PP_BOOTCFG_NOC_EVENT_CTRL_RSVD2_OFF                         (5)
#define PP_BOOTCFG_NOC_EVENT_CTRL_RSVD2_LEN                         (3)
#define PP_BOOTCFG_NOC_EVENT_CTRL_RSVD2_MSK                         (0x000000E0)
#define PP_BOOTCFG_NOC_EVENT_CTRL_RSVD2_RST                         (0x0)
#define PP_BOOTCFG_NOC_EVENT_CTRL_PP_RESP_DDR_NSP_PROBE_MAINEXTEVENT_1_OFF   (4)
#define PP_BOOTCFG_NOC_EVENT_CTRL_PP_RESP_DDR_NSP_PROBE_MAINEXTEVENT_1_LEN   (1)
#define PP_BOOTCFG_NOC_EVENT_CTRL_PP_RESP_DDR_NSP_PROBE_MAINEXTEVENT_1_MSK \
	(0x00000010)
#define PP_BOOTCFG_NOC_EVENT_CTRL_PP_RESP_DDR_NSP_PROBE_MAINEXTEVENT_1_RST (0x0)
#define PP_BOOTCFG_NOC_EVENT_CTRL_RSVD3_OFF                         (1)
#define PP_BOOTCFG_NOC_EVENT_CTRL_RSVD3_LEN                         (3)
#define PP_BOOTCFG_NOC_EVENT_CTRL_RSVD3_MSK                         (0x0000000E)
#define PP_BOOTCFG_NOC_EVENT_CTRL_RSVD3_RST                         (0x0)
#define PP_BOOTCFG_NOC_EVENT_CTRL_PP_RESP_DDR_NSP_PROBE_MAINEXTEVENT_0_OFF   (0)
#define PP_BOOTCFG_NOC_EVENT_CTRL_PP_RESP_DDR_NSP_PROBE_MAINEXTEVENT_0_LEN   (1)
#define PP_BOOTCFG_NOC_EVENT_CTRL_PP_RESP_DDR_NSP_PROBE_MAINEXTEVENT_0_MSK \
	(0x00000001)
#define PP_BOOTCFG_NOC_EVENT_CTRL_PP_RESP_DDR_NSP_PROBE_MAINEXTEVENT_0_RST (0x0)

#endif
