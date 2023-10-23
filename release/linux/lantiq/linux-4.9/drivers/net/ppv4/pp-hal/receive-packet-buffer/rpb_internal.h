/*
 * rpb_internal.h
 * Description: PP receive packet buffer internal h file
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018-2019 Intel Corporation
 */

#ifndef __RBP_INTERNAL_H__
#define __RBP_INTERNAL_H__

#include <linux/kernel.h>
#include <linux/bitops.h>
#include "pp_regs.h"
#include "rpb_regs.h"
#include "rpb_drp_cntr_regs.h"
#include "pp_common.h"
#include "pp_logger.h"

extern u64 rpb_base_addr;
extern u64 rpb_drop_base_addr;
/* combine both RTL RPB modules into 1 memory region */
#define RPB_DRP_BASE_ADDR (rpb_drop_base_addr)
#define RPB_BASE_ADDR     (rpb_base_addr)

/**
 * @brief Add function name and line number for all pr_* prints
 */
#ifdef pr_fmt
#undef pr_fmt
#define pr_fmt(fmt) "[RPB]:%s:%d: " fmt, __func__, __LINE__
#endif

#define STW_RPB_WORD_CNT (1 + STW_COMMON_WORD_CNT)
/**
 * @brief STW rpb fields enumaration
 */
enum pp_rpb_stw_fld {
	STW_RPB_FLD_FIRST = STW_COMMON_FLDS_NUM,
	STW_RPB_FLD_ERR_IND = STW_RPB_FLD_FIRST,
	STW_RPB_FLD_DISCARD_IND,
	STW_RPB_FLD_LAST,
	STW_RPB_FLDS_NUM = STW_RPB_FLD_LAST
};

/**
 * @brief STW rpb fields masks
 */
#define STW_RPB_FLD_ERR_IND_LSB      (0 + 4 * BITS_PER_U32)
#define STW_RPB_FLD_ERR_IND_MSB      (0 + 4 * BITS_PER_U32)
#define STW_RPB_FLD_DISCARD_IND_LSB  (1 + 4 * BITS_PER_U32)
#define STW_RPB_FLD_DISCARD_IND_MSB  (1 + 4 * BITS_PER_U32)

/**
 * @brief STW rpb fields description
 */
#define STW_RPB_FLD_ERR_IND_DESC     "Error"
#define STW_RPB_FLD_DISCARD_IND_DESC "Discard"

/**
 * @brief RPB default profile ports id
 * @RPB_PORT_STR_FLOW0 RPB port for streaming ports with flow
 *                         control support.
 * @RPB_PORT_STR_FLOW1 RPB port for streaming ports without flow
 *                         control support.
 * @RPB_PORT_MEM_FLOW0 RPB port for memory ports with flow
 *                         control support.
 * @RPB_PORT_MEM_FLOW1 RPB port for memory ports without flow
 *                         control support.
 */
enum rpb_dflt_prof_port_id {
	RPB_PORT_STR_FLOW0,
	RPB_PORT_STR_FLOW1,
	RPB_PORT_MEM_FLOW0,
	RPB_PORT_MEM_FLOW1,
};

/**
 * @brief Shortcut to check if rpb profile is valid
 * @param p profile id to check
 */
#define RPB_IS_PROFILE_ID_VALID(p) \
	(p >= RPB_PROFILE_FIRST && p < RPB_PROFILE_NUM)

#define RPB_HIGH_TC             (0)
#define RPB_HIGH_TC_POOL_SZ     (8 * 1024)

/**
 * @brief RBP TC packets drop counter, 32bits <br>
 *        rpb_drp_cntr_pkt
 * @param port RPB port number
 * @param tc port's tc index
 */
#define RPB_TC_PKTS_DROP_CNT_REG(port, tc) \
	(PP_RPB_DRP_CNTR_PKT_PRT0_TC0_REG + (port << 4) + (tc << 2))
/**
 * @brief RBP TC bytes drop counter, 32bits <br>
 *        rpb_drp_cntr_byte
 * @param port RPB port number
 * @param tc port's tc index
 */
#define RPB_TC_BYTES_DROP_CNT_REG(port, tc)  \
	(PP_RPB_DRP_CNTR_BYTE_PRT0_TC0_REG + \
	(port << 4) + (tc << 2))

/**
 * @brief Command source type and dual 40 mode register <br>
 *        glgen_dual40_rpb <br>
 *        [31-5][RO]: reserved
 *        [4]   [RW]: rx dma rpb cmd src, 1 for PE packet <br>
 *        [3]   [RW]: ingress uc rpb cmd src, 1 for PE packet <br>
 *        [2]   [RW]: modifier rpb cmd src, 1 for PE packet <br>
 *        [1]   [RW]: rcu rpb command source, 1 for PE packet <br>
 *        [0]   [RW]: dual 40 mode, 0 for normal, 1 for dual 40 <br>
 * @note DUAL 40 mode is not supported for PPv4
 */
#define RPB_CMD_SRC_40MODE_REG          (PP_RPB_GLGEN_DUAL40_REG)
#define RPB_RX_DMA_CMD_SRC_SET(r, s)    PP_FIELD_MOD((u32)BIT(4), s, r)
#define RPB_RX_DMA_CMD_SRC_GET(r)       PP_FIELD_GET((u32)BIT(4), r)
#define RPB_INGR_UC_CMD_SRC_SET(r, s)   PP_FIELD_MOD((u32)BIT(3), s, r)
#define RPB_INGR_UC_CMD_SRC_GET(r)      PP_FIELD_GET((u32)BIT(3), r)
#define RPB_MODIFIER_CMD_SRC_SET(r, s)  PP_FIELD_MOD((u32)BIT(2), s, r)
#define RPB_MODIFIER_CMD_SRC_GET(r)     PP_FIELD_GET((u32)BIT(2), r)
#define RPB_RCU_CMD_SRC_SET(r, s)       PP_FIELD_MOD((u32)BIT(1), s, r)
#define RPB_RCU_CMD_SRC_GET(r)          PP_FIELD_GET((u32)BIT(1), r)
#define RPB_CMD_SRC_XX_PKT              (0)
#define RPB_CMD_SRC_PE_PKT              (1)
#define RPB_MODE_SET(r, m)              PP_FIELD_MOD((u32)BIT(0), m, r)
#define RPB_MODE_GET(r)                 PP_FIELD_GET((u32)BIT(0), r)
#define RPB_MODE_NORMAL                 (0)
#define RPB_MODE_DUAL_40                (1)
/**
 * @brief DMA coalescing receive threshold register <br>
 *        glpm_dmacth
 *        [31-10][RO]: reserved <br>
 *        [9-0]  [RW]: threshold <br>
 */
#define RPB_DMA_COAL_THR_REG            (PP_RPB_GLPM_DMCTH_REG)
#define RPB_DMA_COAL_THR_GET(r)         PP_FIELD_GET((u32)GENMASK(9, 0), r)
#define RPB_DMA_COAL_MAX_TH             (0x3FFU)
/**
 * @brief Dedicated Pool High Watermark for the Single shared buffer
 *        state, for all TCs of all ports.&edsp; It is expressed in
 *        bytes <br>
 *        glrpb_dhws
 */
#define RPB_DPOOL_BYTES_HWM_REG         (PP_RPB_GLDHWS_REG)
/**
 * @brief Dedicated Pool Low Watermark for the Single shared buffer
 *        state, for all TCs of all ports. It is expressed in
 *        bytes <br>
 *        glrpb_dlws
 */
#define RPB_DPOOL_BYTES_LWM_REG         (PP_RPB_GLDLWS_REG)
/**
 * @brief Dedicated Pool Size for the Single shared buffer state, for
 *        all TCs of all ports. It is expressed in bytes <br>
 *        glrpb_dpss
 */
#define RPB_DPOOL_BYTES_SZ_REG          (PP_RPB_GLDPSS_REG)
#define RPB_DPOOL_BYTES_SZ_GET(r)       PP_FIELD_GET((u32)GENMASK(19, 0), r)
/**
 * @brief Global filling counter, total number of bytes currently
 *        stored in the RPB memory on the account of all ports <br>
 *        glrpb_gfc
 */
#define RPB_GLB_FILLING_CNT_REG         (PP_RPB_GLGFC_REG)
#define RPB_GLB_FILLING_CNT_GET(r)      PP_FIELD_GET((u32)GENMASK(19, 0), r)
/**
 * @brief Global high watermark, in bytes <br>
 *        glrpb_ghw
 */
#define RPB_GLB_BYTES_HWM_REG           (PP_RPB_GLGHW_REG)
/**
 * @brief Global low watermark, in bytes <br>
 *        glrpb_glw
 */
#define RPB_GLB_BYTES_LWM_REG           (PP_RPB_GLGLW_REG)
/**
 * @brief Global packets counter, total number of packets currently
 *        stored in the RPB memory on the account of all ports <br>
 *        glrpb_gpc
 */
#define RPB_GLB_PKTS_CNT_REG            (PP_RPB_GLGPC_REG)
#define RPB_GLB_PKTS_CNT_GET(r)         PP_FIELD_GET((u32)GENMASK(13, 0), r)
/**
 * @brief LTR timer load value, number of micro seconds that the RPB
 *        shall wait before returning to the normal setting of Rx
 *        packet buffer since it has exited the conditions for the DMA
 *        Coalescing state <br>
 *        glrpb_ltrtl
 */
#define RPB_TIMER_LOAD_VAL_REG          (PP_RPB_GLLTRTL_REG)
#define RPB_TIMER_LOAD_VAL_GET(r)       PP_FIELD_GET((u32)GENMASK(9, 0), r)
#define RPB_TIMER_MAX_VAL               (0x3FFU)
/**
 * @brief LTR Timer Value. Current value of the LTR timer expressed in
 *        micro seconds units. This is a countdown timer that
 *        saturates when it reaches zero.<br>
 *        glrpb_ltrtv
 */
#define RPB_TIMER_CURR_VAL_REG          (PP_RPB_GLLTRTV_REG)
#define RPB_TIMER_CURR_VAL_GET(r)       PP_FIELD_GET((u32)GENMASK(9, 0), r)
/**
 * @brief Global Number of packet high watermark <br>
 *        glrpb_phw
 */
#define RPB_GLB_PKTS_HWM_REG            (PP_RPB_GLPHW_REG)
/**
 * @brief Global Number of packet low watermark <br>
 *        glrpb_plw
 */
#define RPB_GLB_PKTS_LWM_REG            (PP_RPB_GLPLW_REG)
/**
 * @brief Global Shared Pool High Threshold for the Single shared
 *        buffer state, for all TCs of all ports, in bytes <br>
 *        glrpb_shts
 */
#define RPB_GLB_SPOOL_HTHR_REG          (PP_RPB_GLSHTS_REG)
/**
 * @brief Shared Pool High Watermark for the Single shared buffer
 *        state, in bytes <br>
 *        glrpb_shws
 */
#define RPB_GLB_SPOOL_HWM_REG           (PP_RPB_GLSHWS_REG)
/**
 * @brief Global Shared Pool Low Threshold for the Single shared
 *        buffer state, for all TCs of all Ports.&edsp; It is
 *        expressed in bytes <br>
 *        glrpb_slts
 */
#define RPB_GLB_SPOOL_LTHR_REG          (PP_RPB_GLSLTS_REG)
/**
 * @brief Global shared Pool Low Watermark for the Single shared
 *        buffer state, in bytes <br>
 *        glrpb_slws
 */
#define RPB_GLB_SPOOL_LWM_REG           (PP_RPB_GLSLWS_REG)
/**
 * @brief Global shared Pool Size for Single shared buffer state.
 *        Number of bytes allocated to the shared pool <br>
 *        glrpb_spss
 */
#define RPB_GLB_SPOOL_SZ_REG            (PP_RPB_GLSPSS_REG)
#define RPB_GLB_SPOOL_SZ_GET(r)         PP_FIELD_GET((u32)GENMASK(19, 0), r)
/**
 * @brief CC CNT memory configuration, rpb_cc_cnt_mem_cfg <br>
 *        [31-20][RO]: reserved <br>
 *        [19-16][RW]: RM <br>
 *        [15-13][RO]: reserved <br>
 *        [12]   [RW]: RME <br>
 *        [11-10][RO]:reserved <br>
 *        [9]    [RW]: ERR count enable <br>
 *        [8]    [RW]: ERR count enable <br>
 *        [7-6]  [RO]: reserved <br>
 *        [5]    [RW]: Mask interrupt <br>
 *        [4]    [RW]: Light sleep bypass, prevent from the memory
 *                     from entring light sleep mode <br>
 *        [3]    [RW]: Light sleep force <br>
 *        [2]    [RW]: ECC invert 2 <br>
 *        [1]    [RW]: ECC invert 1 <br>
 *        [0]    [RW]: ECC enable
 */
#define RPB_CC_CNT_MEM_CFG_REG          (PP_RPB_CC_CNT_MEM_CFG_REG)
/**
 * @brief CC CNT memory status, rpb_cc_cnt_mem_status <br>
 *        [31-4][RO]: reserved <br> [3]   [RO]: global init done, all
 *        block's memories init
 *                    complete indication <br>
 *        [2]   [RO]: memory init complete indication <br>
 *        [1]   [RO]: ECC-FIX correctable error indication <br>
 *        [0]   [RO]: ECC-ERR un-correctable error indication <br>
 */
#define RPB_CC_CNT_MEM_STATUS_REG       (PP_RPB_CC_CNT_MEM_STATUS_REG)
/**
 * @brief Packets memory configuration, rpb_pkt_mem_cfg <br>
 *        [31-20][RO]: reserved <br>
 *        [19-16][RW]: RM <br>
 *        [15-13][RO]: reserved <br>
 *        [12]   [RW]: RME <br>
 *        [11-10][RO]:reserved <br>
 *        [9]    [RW]: ERR count enable <br>
 *        [8]    [RW]: ERR count enable <br>
 *        [7-6]  [RO]: reserved <br>
 *        [5]    [RW]: Mask interrupt <br>
 *        [4]    [RW]: Light sleep bypass, prevent from the memory
 *                     from entring light sleep mode <br>
 *        [3]    [RW]: Light sleep force <br>
 *        [2]    [RW]: ECC invert 2 <br>
 *        [1]    [RW]: ECC invert 1 <br>
 *        [0]    [RW]: ECC enable
 */
#define RPB_PKTS_MEM_CFG_REG            (PP_RPB_PKT_MEM_CFG_REG)
/**
 * @brief Packets memory status, rpb_pkt_mem_status <br>
 *        [31-4][RO]: reserved <br>
 *        [3]   [RO]: global init done, all block's memories init
 *                    complete indication <br>
 *        [2]   [RO]: memory init complete indication <br>
 *        [1]   [RO]: ECC-FIX correctable error indication <br>
 *        [0]   [RO]: ECC-ERR un-correctable error indication <br>
 */
#define RPB_PKTS_MEM_STATUS_REG         (PP_RPB_PKT_MEM_STATUS_REG)
/**
 * @brief CLID memory configuration, rpb_clid_mem_cfg <br>
 *        [31-20][RO]: reserved <br>
 *        [19-16][RW]: RM <br>
 *        [15-13][RO]: reserved <br>
 *        [12]   [RW]: RME <br>
 *        [11-10][RO]:reserved <br>
 *        [9]    [RW]: ERR count enable <br>
 *        [8]    [RW]: ERR count enable <br>
 *        [7-6]  [RO]: reserved <br>
 *        [5]    [RW]: Mask interrupt <br>
 *        [4]    [RW]: Light sleep bypass, prevent from the memory
 *                     from entring light sleep mode <br>
 *        [3]    [RW]: Light sleep force <br>
 *        [2]    [RW]: ECC invert 2 <br>
 *        [1]    [RW]: ECC invert 1 <br>
 *        [0]    [RW]: ECC enable
 */
#define RPB_CLID_MEM_CFG_REG            (PP_RPB_CLID_MEM_CFG_REG)
/**
 * @brief CLID memory status, rpb_clid_mem_status <br>
 *        [31-4][RO]: reserved <br>
 *        [3]   [RO]: global init done, all block's memories init
 *                    complete indication <br>
 *        [2]   [RO]: memory init complete indication <br>
 *        [1]   [RO]: ECC-FIX correctable error indication <br>
 *        [0]   [RO]: ECC-ERR un-correctable error indication <br>
 */
#define RPB_CLID_MEM_STATUS_REG         (PP_RPB_CLID_MEM_STATUS_REG)
/**
 * @brief PPDB memory configuration, rpb_ppdb_mem_cfg <br>
 *        [31-20][RO]: reserved <br>
 *        [19-16][RW]: RM <br>
 *        [15-13][RO]: reserved <br>
 *        [12]   [RW]: RME <br>
 *        [11-10][RO]:reserved <br>
 *        [9]    [RW]: ERR count enable <br>
 *        [8]    [RW]: ERR count enable <br>
 *        [7-6]  [RO]: reserved <br>
 *        [5]    [RW]: Mask interrupt <br>
 *        [4]    [RW]: Light sleep bypass, prevent from the memory
 *                     from entring light sleep mode <br>
 *        [3]    [RW]: Light sleep force <br>
 *        [2]    [RW]: ECC invert 2 <br>
 *        [1]    [RW]: ECC invert 1 <br>
 *        [0]    [RW]: ECC enable
 */
#define RPB_PPDB_MEM_CFG_REG            (PP_RPB_PPDB_MEM_CFG_REG)
/**
 * @brief PPDB memory status, rpb_ppdb_mem_status <br>
 *        [31-4][RO]: reserved <br>
 *        [3]   [RO]: global init done, all block's memories init
 *                    complete indication <br>
 *        [2]   [RO]: memory init complete indication <br>
 *        [1]   [RO]: ECC-FIX correctable error indication <br>
 *        [0]   [RO]: ECC-ERR un-correctable error indication <br>
 */
#define RPB_PPDB_MEM_STATUS_REG         (PP_RPB_PPDB_MEM_STATUS_REG)
/**
 * @brief Report 2 memory configuration, rpb_report_ii_mem_cfg <br>
 *        [31-20][RO]: reserved <br>
 *        [19-16][RW]: RM <br>
 *        [15-13][RO]: reserved <br>
 *        [12]   [RW]: RME <br>
 *        [11-10][RO]:reserved <br>
 *        [9]    [RW]: ERR count enable <br>
 *        [8]    [RW]: ERR count enable <br>
 *        [7-6]  [RO]: reserved <br>
 *        [5]    [RW]: Mask interrupt <br>
 *        [4]    [RW]: Light sleep bypass, prevent from the memory
 *                     from entring light sleep mode <br>
 *        [3]    [RW]: Light sleep force <br>
 *        [2]    [RW]: ECC invert 2 <br>
 *        [1]    [RW]: ECC invert 1 <br>
 *        [0]    [RW]: ECC enable
 */
#define RPB_REPORT_II_MEM_CFG_REG       (PP_RPB_REPORT_LL_MEM_CFG_REG)
/**
 * @brief Report 2 memory status, rpb_report_ii_mem_status <br>
 *        [31-4][RO]: reserved <br>
 *        [3]   [RO]: global init done, all block's memories init
 *                    complete indication <br>
 *        [2]   [RO]: memory init complete indication <br>
 *        [1]   [RO]: ECC-FIX correctable error indication <br>
 *        [0]   [RO]: ECC-ERR un-correctable error indication <br>
 */
#define RPB_REPORT_II_MEM_STATUS_REG    (PP_RPB_REPORT_LL_MEM_STATUS_REG)
/**
 * @brief Report memory configuration, rpb_report_mem_cfg <br>
 *        [31-20][RO]: reserved <br>
 *        [19-16][RW]: RM <br>
 *        [15-13][RO]: reserved <br>
 *        [12]   [RW]: RME <br>
 *        [11-10][RO]:reserved <br>
 *        [9]    [RW]: ERR count enable <br>
 *        [8]    [RW]: ERR count enable <br>
 *        [7-6]  [RO]: reserved <br>
 *        [5]    [RW]: Mask interrupt <br>
 *        [4]    [RW]: Light sleep bypass, prevent from the memory
 *                     from entring light sleep mode <br>
 *        [3]    [RW]: Light sleep force <br>
 *        [2]    [RW]: ECC invert 2 <br>
 *        [1]    [RW]: ECC invert 1 <br>
 *        [0]    [RW]: ECC enable
 */
#define RPB_REPORT_MEM_CFG_REG          (PP_RPB_REPORT_MEM_CFG_REG)
/**
 * @brief Report memory status, rpb_report_mem_status <br>
 *        [31-4][RO]: reserved <br>
 *        [3]   [RO]: global init done, all block's memories init
 *                    complete indication <br>
 *        [2]   [RO]: memory init complete indication <br>
 *        [1]   [RO]: ECC-FIX correctable error indication <br>
 *        [0]   [RO]: ECC-ERR un-correctable error indication <br>
 */
#define RPB_REPORT_MEM_STATUS_REG       (PP_RPB_REPORT_MEM_STATUS_REG)
/**
 * @brief CC memory configuration, rpb_cc_mem_cfg <br>
 *        [31-20][RO]: reserved <br>
 *        [19-16][RW]: RM <br>
 *        [15-13][RO]: reserved <br>
 *        [12]   [RW]: RME <br>
 *        [11-10][RO]:reserved <br>
 *        [9]    [RW]: ERR count enable <br>
 *        [8]    [RW]: ERR count enable <br>
 *        [7-6]  [RO]: reserved <br>
 *        [5]    [RW]: Mask interrupt <br>
 *        [4]    [RW]: Light sleep bypass, prevent from the memory
 *                     from entring light sleep mode <br>
 *        [3]    [RW]: Light sleep force <br>
 *        [2]    [RW]: ECC invert 2 <br>
 *        [1]    [RW]: ECC invert 1 <br>
 *        [0]    [RW]: ECC enable
 */
#define RPB_CC_MEM_CFG_REG              (PP_RPB_CC_MEM_CFG_REG)
/**
 * @brief CC memory status, rpb_cc_mem_status <br>
 *        [31-4][RO]: reserved <br>
 *        [3]   [RO]: global init done, all block's memories init
 *                    complete indication <br>
 *        [2]   [RO]: memory init complete indication <br>
 *        [1]   [RO]: ECC-FIX correctable error indication <br>
 *        [0]   [RO]: ECC-ERR un-correctable error indication <br>
 */
#define RPB_CC_MEM_STATUS_REG           (PP_RPB_CC_MEM_STATUS_REG)
/**
 * @brief ECC un-correctable errors counter (bytes?) <br>
 *        rpb_ecc_uncor_err <br>
 */
#define RPB_ECC_UNCOR_ERR_CNT_REG       (PP_RPB_ECC_UNCOR_ERR_REG)
/**
 * @brief ECC correctable errors counter (bytes?) <br>
 *        rpb_ecc_cor_err <br>
 */
#define RPB_ECC_COR_ERR_CNT_REG         (PP_RPB_ECC_COR_ERR_REG)
/**
 * @brief Debug registers, rpb_dbg_acc_cnt, rpb_dbg_acc_ctl,
 *        rpb_dbg_acc_stat, rpb_shr_mod_cnt, rpb_dbg_feat,
 *        rpb_gen_dbg_cnt,
 *
 */
#define RPB_DBG_ACC_CNT_REG             (PP_RPB_DBG_ACC_CNT_REG)
#define RPB_DBG_ACC_CTRL_REG            (PP_RPB_DBG_ACC_CTL_REG)
#define RPB_DBG_ACC_STAT_REG            (PP_RPB_DBG_ACC_STAT_REG)
#define RPB_DBG_SH_MODE_CNT_REG         (PP_RPB_SHR_MOD_CNT_REG)
#define RPB_DBG_FEATURES_REG            (PP_RPB_DBG_FEAT_REG)
#define RPB_DBG_GEN_CNT_REG             (PP_RPB_GEN_DBG_CNT_REG)
/**
 * @brief Back pressure indication, rpb_back_prs_stat <br>
 *        [31-16][RO]: reserved
 *        [15]   [RO]: port 3 flow control (Xoff) status, 1 for
 *                     transmit off signal
 *        [14]   [RO]: port 2 flow control (Xoff) status
 *        [13]   [RO]: port 1 flow control (Xoff) status
 *        [12]   [RO]: port 0 flow control (Xoff) status
 */
#define RPB_BACK_PRS_STATUS_REG         (PP_RPB_BACK_PRS_STAT_REG)
#define RPB_PORT_BACK_PRS_STATUS_GET(r, p) PP_FIELD_GET((u32)BIT(p + 12), r)
/**
 * @brief Data requests counters, cleared on read, rpb_egr_cnt
 */
#define RPB_EGRS_DATA_REQ_CNT_REG       (PP_RPB_EGR_CNT_REG)
/**
 * @brief Reports to RCU counter, rpb_rpt_cnt
 */
#define RPB_RCU_REPORTS_CNT_REG         (PP_RPB_RPT_CNT_REG)
/**
 * @brief Current pending reports to RCU, rpb_rpt_stat
 */
#define RPB_RCU_REPORTS_PEND_CNT_REG    (PP_RPB_RPT_STAT_REG)
/**
 * @brief High priority TC configuration per port, defined which TC is
 *        considered high priority.
 *        Packet received on high priority TC will cause exit from DMA
 *        coalescing mode <br>
 *        prtpm_hptc <br>
 *        [31-8][RO]: reserved <br>
 *        [7-0] [RW]: bit 0 for TC 0, bit 1 for TC 1 and so on <br>
 */
#define RPB_TC_HI_PRI_REG(p)            (PP_RPB_PRTPM_HPTC_REG + (p << 2))
#define RPB_TC_HI_PRI_SET(r, tc, hi)    PP_FIELD_MOD((u32)BIT(tc), hi, r)
#define RPB_TC_HI_PRI_GET(r, tc)        PP_FIELD_GET((u32)BIT(tc), r)
/**
 * @brief Port TCs flow control (PFC) enable, when bit n set to 1b, TC
 *        n uses PFC in bote rx and tx of RPB and the TC is referred
 *        to as non-drop TC. <br>
 *        When bit n is cleared, the RPB will not issue PFC pause
 *        frames for that TC. <br>
 *        prtrbp_tc2pfc <br>
 *        [31-8][RO]: reserved <br>
 *        [7-0] [RW]: bit 0 for TC 0, bit 1 for TC 1 and so on. <br>
 */
#define RPB_TC_FLOW_CTRL_REG(p)         (PP_RPB_PRTTC2PFC_REG + (p << 2))
#define RPB_TC_FLOW_CTRL_SET(r, tc, en) PP_FIELD_MOD((u32)BIT(tc), en, r)
#define RPB_TC_FLOW_CTRL_GET(r, tc)     PP_FIELD_GET((u32)BIT(tc), r)
/**
 * @brief Port Filling Counter, Total number of bytes currently stored
 *        in the Rx packet buffer on the account of the port. It
 *        includes bytes stored on the account of shared pool and
 *        dedicated pools altogether <br>
 *        prtrpb_pfc
 */
#define RPB_PORT_FILLING_CNT_REG(p)     (PP_RPB_PRTPFC_REG + (p << 2))
#define RPB_PORT_FILLING_CNT_GET(r)     PP_FIELD_GET((u32)GENMASK(19, 0), r)
/**
 * @brief User priority to TC mapping, prtrpb_rup2tc
 * @note User priority to TC mapping isn't supported in PPv4 as it is
 *       already covered by the port distributor.
 */
#define RPB_PORT_USR_PRI_TO_TC_REG(p)   (PP_RPB_PRTRUP2TC_REG + (p << 2))
/**
 * @brief Port Shared Pool Filling Counter. Number of bytes currently
 *        stored in the Rx packet buffer on the account of the shared
 *        pool of the port <br>
 *        prtrpb_sfc
 */
#define RPB_PORT_SPOOL_FILLING_CNT_REG(p) (PP_RPB_PRTSFC_REG + (p << 2))
#define RPB_PORT_SPOOL_FILLING_CNT_GET(r) PP_FIELD_GET((u32)GENMASK(19, 0), r)
/**
 * @brief Port Shared Pool High Watermark. in bytes <br>
 *        prtrpb_shw
 */
#define RPB_PORT_SPOOL_HWM_REG(p)       (PP_RPB_PRTSHW_REG + (p << 2))
/**
 * @brief Port Shared Pool low Watermark. in bytes <br>
 *        prtrpb_slw
 */
#define RPB_PORT_SPOOL_LWM_REG(p)       (PP_RPB_PRTSLW_REG + (p << 2))
/**
 * @brief Port Shared Pool Size. Number of bytes allocated to the
 *        shared pool of the port <br>
 *        prtrpb_slw
 */
#define RPB_PORT_SPOOL_SZ_REG(p)       (PP_RPB_PRTSPS_REG + (p << 2))
#define RPB_PORT_SPOOL_SZ_GET(r)       PP_FIELD_GET((u32)GENMASK(19, 0), r)
/**
 * @brief Port TC Dedicated Pool Filling Counter in bytes <br>
 *        Number of bytes currently used by a TC <br>
 *        prtrpb_dfc_prt(n)
 */
#define RPB_TC_DPOOL_FILLING_CNT_REG(p, tc) \
	(PP_RPB_PRTDFC_PRT0_REG + (p << 2) + (tc << 5))
#define RPB_TC_DPOOL_FILLING_CNT_GET(r) PP_FIELD_GET((u32)GENMASK(19, 0), r)
/**
 * @brief Port TC Dedicated Pool High Watermark, in bytes <br>
 *        prtrpb_dhw_prt(n) <br>
 */
#define RPB_TC_DPOOL_HWM_REG(p, tc) \
	(PP_RPB_PRTDHW_PRT0_REG + (p << 2) + (tc << 5))
/**
 * @brief Port TC Dedicated Pool Low Watermark, in bytes <br>
 *        prtrpb_dlw_prt(n) <br>
 */
#define RPB_TC_DPOOL_LWM_REG(p, tc) \
	(PP_RPB_PRTDLW_PRT0_REG + (p << 2) + (tc << 5))
/**
 * @brief Port TC Dedicated Pool Size, in bytes <br>
 *        prtrpb_dps_prt(n) <br>
 */
#define RPB_TC_DPOOL_SZ_REG(p, tc) \
	(PP_RPB_PRTDPS_PRT0_REG + (p << 2) + (tc << 5))
#define RPB_TC_DPOOL_SZ_GET(r)          PP_FIELD_GET((u32)GENMASK(19, 0), r)
/**
 * @brief Port TC Shared Pool High Threshold, in bytes <br>
 *        prtrpb_sht_prt(n) <br>
 */
#define RPB_TC_SPOOL_HTHR_REG(p, tc) \
	(PP_RPB_PRTSHT_PRT0_REG + (p << 2) + (tc << 5))
/**
 * @brief Port TC Shared Pool low Threshold, in bytes <br>
 *        prtrpb_slt_prt(n) <br>
 */
#define RPB_TC_SPOOL_LTHR_REG(p, tc) \
	(PP_RPB_PRTSLT_PRT0_REG + (p << 2) + (tc << 5))
/**
 * @brief Port TC Shared Pool Occupancy Counter, in bytes <br>
 *        prtrpb_soc_prt(n)
 */
#define RPB_TC_SPOOL_OCC_CNT_REG(p, tc) \
	(PP_RPB_PRTSOC_PRT0_REG + (p << 2) + (tc << 5))
#define RPB_TC_SPOOL_OCC_CNT_GET(r)     PP_FIELD_GET((u32)GENMASK(19, 0), r)
/**
 * @brief Port datapipe memory configuration,
 *        rpb_data_pipe_mem_cfg <br>
 *        [31-20][RO]: reserved <br>
 *        [19-16][RW]: RM <br>
 *        [15-13][RO]: reserved <br>
 *        [12]   [RW]: RME <br>
 *        [11-10][RO]:reserved <br>
 *        [9]    [RW]: ERR count enable <br>
 *        [8]    [RW]: ERR count enable <br>
 *        [7-6]  [RO]: reserved <br>
 *        [5]    [RW]: Mask interrupt <br>
 *        [4]    [RW]: Light sleep bypass, prevent from the memory
 *                     from entring light sleep mode <br>
 *        [3]    [RW]: Light sleep force <br>
 *        [2]    [RW]: ECC invert 2 <br>
 *        [1]    [RW]: ECC invert 1 <br>
 *        [0]    [RW]: ECC enable
 */
#define RPB_PORT_DATAPIPE_MEM_CFG_REG(p) (PP_RPB_DATA_PIPE_MEM_CFG_REG + \
					  (p << 2))
/**
 * @brief Port datapipe status, rpb_data_pipe_mem_status <br>
 *        [31-4][RO]: reserved <br>
 *        [3]   [RO]: global init done, all block's memories init
 *                    complete indication <br>
 *        [2]   [RO]: memory init complete indication <br>
 *        [1]   [RO]: ECC-FIX correctable error indication <br>
 *        [0]   [RO]: ECC-ERR un-correctable error indication <br>
 */
#define RPB_PORT_DATAPIPE_MEM_STATUS_REG(p)  (PP_RPB_DATA_PIPE_MEM_STATUS_REG \
					      + (p << 2))
/**
 * @brief Debug register per port, rpb_dbg_acc_data, rpb_pprs_err_cnt,
 *        pkt_indications,
 */
#define RPB_DBG_ACC_CNT_DATA_REG(p)     (PP_RPB_DBG_ACC_DATA_REG + (p << 2))
#define RPB_DBG_PPRS_ERR_CNT_REG(p)     (PP_RPB_PPRS_ERR_CNT_REG + (p << 2))
#define RPB_DBG_PKT_INDICATION_REG(p)   (PP_RPB_PKT_INDICATIONS_REG + (p << 2))

/**
 * @brief RPB common definitions
 */
#define RPB_MEMORY_INIT_DONE_MASK       ((u32)GENMASK(3, 2))
#define RPB_MEMORY_INIT_DONE_VAL        (0x3)
#define RPB_MEMORY_ECC_EN_SET(r, en)    PP_FIELD_MOD((u32)BIT(0), en, r)
#define RPB_THRESHOLD_GET(r)            PP_FIELD_GET((u32)GENMASK(19, 0), r)
#define RPB_POOL_SIZE_GET(r)            PP_FIELD_GET((u32)GENMASK(19, 0), r)
#define RPB_MAX_THRESHOLD               0xFFFFFU
#define RPB_MAX_POOL_SIZE               0xFFFFFU

#define PP_RPB_PORT_IN_PKT_CNT_REG(port)                  \
	((port) == 0 ? PP_RPB_PORT0_IN_PKT_CNT_REG :      \
	((port) == 1 ? PP_RPB_PORT1_IN_PKT_CNT_REG :      \
	((port) == 2 ? PP_RPB_PORT2_IN_PKT_CNT_REG :      \
		       PP_RPB_PORT3_IN_PKT_CNT_REG)))

/**
 * @brief Check if port is valid and print error message in case not
 * @param port port number to check
 * @return bool true in case the port number is valid, false otherwise
 */
static inline bool __rpb_is_port_valid(u32 port)
{
	if (likely(PP_IS_RPB_PORT_VALID(port)))
		return true;

	pr_err("Invalid port %u, valid values: 0 - %u\n", port,
	       PP_MAX_RPB_PORT - 1);
	return false;
}

/**
 * @brief Check if tc is valid and print error message in case not
 * @param tc tc number to check
 * @return bool true in case the tc number is valid, false otherwise
 */
static inline bool __rpb_is_tc_valid(u32 tc)
{
	if (likely(PP_IS_TC_VALID(tc)))
		return true;

	pr_err("Invalid TC %u, valid values: 0 - %u\n", tc, PP_MAX_TC - 1);
	return false;
}

/**
 * @brief Read buffer manager base address from RPB registers
 * @param addr buffer to store the addr
 * @return s32 0 on success, non-zero value otherwise
 */
s32 rpb_bmgr_addr_get(u64 *addr);

/**
 * @brief Get RPB global high bytes threshold
 * @param high_thr buffer to store high threshold
 * @return s32 0 on success, non-zero value otherwise
 */
s32 rpb_glb_high_threshold_get(u32 *high_thr);

/**
 * @brief Set RPB global high bytes threshold
 * @param high_thr high threshold
 * @return s32 0 on success, non-zero value otherwise
 */
s32 rpb_glb_high_threshold_set(u32 high_thr);

/**
 * @brief Get RPB global low bytes threshold
 * @param buffer to store low threshold
 * @return s32 0 on success, non-zero value otherwise
 */
s32 rpb_glb_low_threshold_get(u32 *low_thr);

/**
 * @brief Set RPB global low bytes threshold
 * @param low_thr low threshold
 * @return s32 0 on success, non-zero value otherwise
 */
s32 rpb_glb_low_threshold_set(u32 low_thr);

/**
 * @brief Get RPB global high packets threshold
 * @param high_thr buffer to store the threshold
 * @return s32 0 on success, non-zero value otherwise
 */
s32 rpb_glb_pkts_high_threshold_get(u32 *high_thr);

/**
 * @brief Set RPB global high packets threshold
 * @param high_thr high threshold
 * @return s32 0 on success, non-zero value otherwise
 */
s32 rpb_glb_pkts_high_threshold_set(u32 high_thr);

/**
 * @brief Get RPB global low packets threshold
 * @param low_thr buffer to store the threshold
 * @return s32 0 on success, non-zero value otherwise
 */
s32 rpb_glb_pkts_low_threshold_get(u32 *low_thr);

/**
 * @brief Set RPB global low packets threshold
 * @param low_thr low threshold
 * @return s32 0 on success, non-zero value otherwise
 */
s32 rpb_glb_pkts_low_threshold_set(u32 low_thr);

/**
 * @brief Get TC high priority state
 * @param port RPB port index
 * @param tc TC index
 * @param hi_prio buffer to store the result
 * @return s32 0 on success, non-zero value otherwise
 */
s32 rpb_tc_high_prio_get(u32 port, u32 tc, bool *high_prio);

/**
 * @brief Mark/Unmark TC as high priority
 * @param port RPB port index
 * @param tc TC index
 * @param hi_prio true/false
 * @return s32 0 on success, non-zero value otherwise
 */
s32 rpb_tc_high_prio_set(u32 port, u32 tc, bool hi_prio);

/**
 * @brief Get TC's flow control status
 * @param port RPB port index
 * @param tc TC index
 * @param flow_ctrl buffer to store the result
 * @return s32 TC's flow control status
 */
s32 rpb_tc_flow_ctrl_get(u32 port, u32 tc, bool *flow_ctrl);

/**
 * @brief Enable/Disable TC's flow control
 * @param port RPB port index
 * @param tc TC index
 * @param en true/false
 * @return s32 0 on success, non-zero value otherwise
 */
s32 rpb_tc_flow_ctrl_set(u32 port, u32 tc, bool en);

/**
 * @brief Get RPB TC's dedicated pool size
 * @param port RPB port id
 * @param tc tc id
 * @param dpool_sz buffer to store pool size
 * @return u32 TC's dedicated pool size
 */
s32 rpb_tc_dpool_sz_get(u32 port, u32 tc, u32 *dpool_sz);

/**
 * @brief Set RPB TC's dedicated pool size
 * @param port RPB port id
 * @param tc tc id
 * @param dpool_sz dedicated pool size
 * @return s32 0 on success, non-zero value otherwise
 */
s32 rpb_tc_dpool_sz_set(u32 port, u32 tc, u32 dpool_sz);

/**
 * @brief Get RPB TC's dedicated pool high threshold
 * @param port RPB port id
 * @param tc tc id
 * @param high_thr buffer to store threshold value
 * @return s32 0 on success, non-zero value otherwise
 */
s32 rpb_tc_dpool_high_threshold_get(u32 port, u32 tc, u32 *high_thr);

/**
 * @brief Set RPB TC's dedicated pool high threshold
 * @param port
 * @param tc
 * @param high_thr
 * @return s32 0 on success, non-zero value otherwise
 */
s32 rpb_tc_dpool_high_threshold_set(u32 port, u32 tc, u32 high_thr);

/**
 * @brief Get RPB TC's dedicated pool low threshold
 * @param port RPB port id
 * @param tc tc id
 * @param low_thr buffer to store threshold value
 * @return s32 0 on success, non-zero value otherwise
 */
s32 rpb_tc_dpool_low_threshold_get(u32 port, u32 tc, u32 *low_thr);

/**
 * @brief Set RPB TC's dedicated pool low threshold
 * @param port
 * @param tc
 * @param low_thr
 * @return s32 0 on success, non-zero value otherwise
 */
s32 rpb_tc_dpool_low_threshold_set(u32 port, u32 tc, u32 low_thr);

/**
 * @brief Get RPB TC's shared pool high threshold
 * @param port RPB port id
 * @param tc tc id
 * @param high_thr buffer to store threshold value
 * @return s32 0 on success, non-zero value otherwise
 */
s32 rpb_tc_spool_high_threshold_get(u32 port, u32 tc, u32 *high_thr);

/**
 * @brief Set RPB TC's shared pool high threshold
 * @param port
 * @param tc
 * @param high_thr
 * @return s32 0 on success, non-zero value otherwise
 */
s32 rpb_tc_spool_high_threshold_set(u32 port, u32 tc, u32 high_thr);

/**
 * @brief Get RPB TC's shared pool low threshold
 * @param port RPB port id
 * @param tc tc id
 * @param low_thr buffer to store threshold value
 * @return s32 0 on success, non-zero value otherwise
 */
s32 rpb_tc_spool_low_threshold_get(u32 port, u32 tc, u32 *low_thr);

/**
 * @brief Set RPB TC's shared pool low threshold
 * @param port
 * @param tc
 * @param low_th
 * @return s32 0 on success, non-zero value otherwise
 */
s32 rpb_tc_spool_low_threshold_set(u32 port, u32 tc, u32 low_thr);

/**
 * @brief Get RPB port's dedicated pool size
 * @param port RPB port id
 * @param spool_sz buffer to store pool size
 * @return s32 0 on success, non-zero value otherwise
 */
s32 rpb_port_spool_sz_get(u32 port, u32 *spool_sz);

/**
 * @brief Set RPB port's shared pool size
 * @param port
 * @param tc
 * @param spool_sz pool size
 * @return s32 0 on success, non-zero value otherwise
 */
s32 rpb_port_spool_sz_set(u32 port, u32 spool_sz);

/**
 * @brief Get RPB port's pool high threshold
 * @param port RPB port id
 * @param high_thr buffer to store the threshold
 * @return s32 0 on success, non-zero value otherwise
 */
s32 rpb_port_high_threshold_get(u32 port, u32 *high_thr);

/**
 * @brief Set RPB port's poll high threshold
 * @param port
 * @param tc
 * @param high_thr threshold size
 * @return s32 0 on success, non-zero value otherwise
 */
s32 rpb_port_high_threshold_set(u32 port, u32 high_thr);

/**
 * @brief Get RPB port's pool low threshold
 * @param port RPB port id
 * @param tc tc id
 * @param low_thr buffer to store the threshold
 * @return s32 0 on success, non-zero value otherwise
 */
s32 rpb_port_low_threshold_get(u32 port, u32 *low_thr);

/**
 * @brief Set RPB port's pool low threshold
 * @param port
 * @param tc
 * @param low_thr threshold size
 * @return s32 0 on success, non-zero value otherwise
 */
s32 rpb_port_low_threshold_set(u32 port, u32 low_thr);

#ifdef CONFIG_DEBUG_FS
/**
 * @brief RPB debug init
 * @return int 0 on success, non-zero value otherwise
 */
s32 rpb_dbg_init(struct dentry *parent);
/**
 * @brief RPB debug clean
 * @return int 0 on success, non-zero value otherwise
 */
s32 rpb_dbg_clean(void);
#else
static inline s32 rpb_dbg_init(struct dentry *parent)
{
	return 0;
}

static inline s32 rpb_dbg_clean(void)
{
	return 0;
}
#endif /* CONFIG_DEBUG_FS */

#endif /* __RBP_INTERNAL_H__ */
