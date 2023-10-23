/*
 * classifier_internal.h
 * Description: Packet Processor Classifier Internal Definitions
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018-2020 Intel Corporation
 */
#ifndef __CLASSIFIER_INTERNAL_H__
#define __CLASSIFIER_INTERNAL_H__
#include <linux/types.h>
#include <linux/bitmap.h>
#include <linux/io.h>
#include "pp_regs.h"
#include "cls_regs.h"
#include "session_cache_regs.h"
#include "pp_fv.h"
#include "pp_si.h"

extern u64 cls_base_addr;
#define CLS_BASE_ADDR cls_base_addr

extern u64 cls_cache_base_addr;
#define SESS_CACHE_BASE_ADDR cls_cache_base_addr

#ifdef pr_fmt
#undef pr_fmt
#define pr_fmt(fmt) "[CLASSIFIER]:%s:%d: " fmt, __func__, __LINE__
#endif

#define HT_ENTRY_SIZE                 (8)

/* HW values */
#define CLS_LOOKUP_THRD_MAX           (16)
#define CLS_FV_MSK_NUM_REGS           (32)
#define CLS_DBG_FV_NUM_REGS           (32)
#define CLS_CMD_IF_ADD                (1)
#define CLS_CMD_IF_UPDATE             (2)
#define CLS_CMD_IF_DEL                (3)
#define CLS_DFLT_SI_W0                (0)
#define CLS_DFLT_SI_W1                (1)
#define CLS_HASH_SZ_MAX               (PP_CLS_HASH_MASK_MSK + 1)

/* Default values */
#define CLS_DFLT_SIG_SEL              (0)
#define CLS_DFLT_HASH_SEL             (0)
#define CLS_DFLT_LONG_LU_THR          (50)
#define CLS_DFLT_SI_IDX               (0xFFFFFFFF)
#define CLS_DFLT_SI_FV_SZ             (0x07000000)

#define CLS_DRP_FLW_DIS               (true)
#define CLS_SRCH_MOD_NORMAL           (false)
#define CLS_FM_FLW_FULL               (true)
#define CLS_FV_MASKED                 (true)
#define CLS_NO_ENDIAN_SWP             (false)
#define CLS_HASH_NUM_TBLS             BIT(1)
#define CLS_HASH_BKT_SZ               BIT(3)

#define CLS_SESS_2_HASH_SZ(n)         (n >> 4)

/* cuckoo hash default values */
#define CLS_CUCKOO_HASH_MAX_RPLC      (2)
#define CLS_CUCKOO_HASH_LOCK_EN       (true)
#define CLS_CUCKOO_HASH_COMP_EN       (false)
#define CLS_CUCKOO_HASH_LU_INSR       (false)
#define CLS_CUCKOO_HASH_HIST_SZ       (16)
#define CLS_SESSION_SI_OFF(sess)      ((sess) * sizeof(struct pp_hw_si))
#define CLS_FV_ADDR(b, s, fv_sz)                     \
	((b) + CLS_SESSION_SI_OFF(s + 1) - (fv_sz))

#define CLS_FV_START_ADDR(si, fv_sz)                       \
	(((u8 *)(si)) + sizeof(struct pp_hw_si) - (fv_sz))

#define CLS_LU_THREAD_STM_STATUS_REG(thread_id)             \
	PP_CLS_LU_THREAD_STM_STATUS_REG_IDX(thread_id >> 2)

#define PP_CLS_LU_THREAD_STM_STATUS_FLD(thread_id)                \
	GENMASK(((thread_id % 4) * 6) + 5, ((thread_id % 4) * 6))
/**
 * Session command interface
 */
#define CLS_ADD_CMD_SZ                (69)
#define CLS_UPDATE_CMD_SZ             (5)
#define CLS_DEL_CMD_SZ                (5)

#define CLS_CMD_UPDT_SIG_ENT          BIT(31)
#define CLS_CMD_UPDT_DSI_INFO         BIT(30)
#define CLS_CMD_SIG_INVALIDATE        BIT(29)
#define CLS_CMD_SESS_INVALIDATE       BIT(28)

#define CLS_CMD_HASH_TBL              BIT(27)
#define CLS_CMD_BKT_OFF               GENMASK(26, 24)
#define CLS_CMD_SESS_ID               GENMASK(23, 0)

#define CLS_CMD_DSI_VVAL              BIT(31) /* valid bit value             */
#define CLS_CMD_DSI_AVAL              BIT(30) /* active bit value            */
#define CLS_CMD_DSI_SVAL              BIT(29) /* stale bit value             */
#define CLS_CMD_DSI_DVAL              BIT(28) /* divert bit value            */
#define CLS_CMD_DSI_VWR               BIT(27) /* whether to write valid bit  */
#define CLS_CMD_DSI_AWR               BIT(26) /* whether to write active bit */
#define CLS_CMD_DSI_SWR               BIT(25) /* whether to write stale bit  */
#define CLS_CMD_DSI_DWR               BIT(24) /* whether to write divert bit */
#define CLS_CMD_DSI_DST_Q             GENMASK(23, 12) /* dest queue value    */
#define CLS_CMD_DSI_DQWR              BIT(11) /* whether to write dest queue */
#define CLS_CMD_DSI_CNT_CLR           BIT(10) /* whether to reset counters   */
#define CLS_CMD_DSI_RSRV              GENMASK(9, 0)

#define CLS_DBG_FV_START_WORD         (PP_SI_WORDS_CNT - CLS_DBG_FV_NUM_REGS)

struct cls_dbg_fv {
	u32 word[CLS_DBG_FV_NUM_REGS];
};

/**
 * @enum pp_stw_cls_fld
 * @brief STW classifier fields enumaration
 */
enum pp_stw_cls_fld {
	STW_CLS_FLD_FIRST = STW_COMMON_FLDS_NUM,
	STW_CLS_FLD_TTL_EXPIRED = STW_CLS_FLD_FIRST,
	STW_CLS_FLD_IP_UNSUPP,
	STW_CLS_FLD_EXT_DF,
	STW_CLS_FLD_INT_DF,
	STW_CLS_FLD_EXT_FRAG_TYPE,
	STW_CLS_FLD_INT_FRAG_TYPE,
	STW_CLS_FLD_TCP_FIN,
	STW_CLS_FLD_TCP_SYN,
	STW_CLS_FLD_TCP_RST,
	STW_CLS_FLD_TCP_ACK,
	STW_CLS_FLD_TCP_DATA_OFFSET,
	STW_CLS_FLD_EXT_L3_OFFSET,
	STW_CLS_FLD_INT_L3_OFFSET,
	STW_CLS_FLD_EXT_L4_OFFSET,
	STW_CLS_FLD_INT_L4_OFFSET,
	STW_CLS_FLD_EXT_FRAG_INFO_OFFSET,
	STW_CLS_FLD_INT_FRAG_INFO_OFFSET,
	STW_CLS_FLD_L3_HDR_OFFSET_5,
	STW_CLS_FLD_L3_HDR_OFFSET_4,
	STW_CLS_FLD_L3_HDR_OFFSET_3,
	STW_CLS_FLD_L3_HDR_OFFSET_2,
	STW_CLS_FLD_L3_HDR_OFFSET_1,
	STW_CLS_FLD_L3_HDR_OFFSET_0,
	STW_CLS_FLD_ANA_PKT_TYPE,
	STW_CLS_FLD_TUNN_INNER_OFFSET,
	STW_CLS_FLD_PAYLOAD_OFFSET,
	STW_CLS_FLD_ERROR_IND,
	STW_CLS_FLD_LRU_EXP,
	STW_CLS_FLD_RPB_CLID,
	STW_CLS_FLD_DROP_PKT,
	STW_CLS_FLD_FV_FLD_1,
	STW_CLS_FLD_LAST = STW_CLS_FLD_FV_FLD_1,
	STW_CLS_FLDS_NUM
};

#define STW_CLS_WORD_CNT                        (5 + STW_COMMON_WORD_CNT)

/**
 * @brief STW classifier fields masks
 */
#define STW_CLS_FLD_TTL_EXPIRED_LSB             (0  + 4 * BITS_PER_U32)
#define STW_CLS_FLD_TTL_EXPIRED_MSB             (0  + 4 * BITS_PER_U32)
#define STW_CLS_FLD_IP_UNSUPP_LSB               (1  + 4 * BITS_PER_U32)
#define STW_CLS_FLD_IP_UNSUPP_MSB               (1  + 4 * BITS_PER_U32)
#define STW_CLS_FLD_EXT_DF_LSB                  (2  + 4 * BITS_PER_U32)
#define STW_CLS_FLD_EXT_DF_MSB                  (2  + 4 * BITS_PER_U32)
#define STW_CLS_FLD_INT_DF_LSB                  (3  + 4 * BITS_PER_U32)
#define STW_CLS_FLD_INT_DF_MSB                  (3  + 4 * BITS_PER_U32)
#define STW_CLS_FLD_EXT_FRAG_TYPE_LSB           (4  + 4 * BITS_PER_U32)
#define STW_CLS_FLD_EXT_FRAG_TYPE_MSB           (5  + 4 * BITS_PER_U32)
#define STW_CLS_FLD_INT_FRAG_TYPE_LSB           (6  + 4 * BITS_PER_U32)
#define STW_CLS_FLD_INT_FRAG_TYPE_MSB           (7  + 4 * BITS_PER_U32)
#define STW_CLS_FLD_TCP_FIN_LSB                 (8  + 4 * BITS_PER_U32)
#define STW_CLS_FLD_TCP_FIN_MSB                 (8  + 4 * BITS_PER_U32)
#define STW_CLS_FLD_TCP_SYN_LSB                 (9  + 4 * BITS_PER_U32)
#define STW_CLS_FLD_TCP_SYN_MSB                 (9  + 4 * BITS_PER_U32)
#define STW_CLS_FLD_TCP_RST_LSB                 (10 + 4 * BITS_PER_U32)
#define STW_CLS_FLD_TCP_RST_MSB                 (10 + 4 * BITS_PER_U32)
#define STW_CLS_FLD_TCP_ACK_LSB                 (11 + 4 * BITS_PER_U32)
#define STW_CLS_FLD_TCP_ACK_MSB                 (11 + 4 * BITS_PER_U32)
#define STW_CLS_FLD_TCP_DATA_OFFSET_LSB         (12 + 4 * BITS_PER_U32)
#define STW_CLS_FLD_TCP_DATA_OFFSET_MSB         (15 + 4 * BITS_PER_U32)
#define STW_CLS_FLD_EXT_L3_OFFSET_LSB           (16 + 4 * BITS_PER_U32)
#define STW_CLS_FLD_EXT_L3_OFFSET_MSB           (23 + 4 * BITS_PER_U32)
#define STW_CLS_FLD_INT_L3_OFFSET_LSB           (24 + 4 * BITS_PER_U32)
#define STW_CLS_FLD_INT_L3_OFFSET_MSB           (31 + 4 * BITS_PER_U32)
#define STW_CLS_FLD_EXT_L4_OFFSET_LSB           (0  + 5 * BITS_PER_U32)
#define STW_CLS_FLD_EXT_L4_OFFSET_MSB           (7  + 5 * BITS_PER_U32)
#define STW_CLS_FLD_INT_L4_OFFSET_LSB           (8  + 5 * BITS_PER_U32)
#define STW_CLS_FLD_INT_L4_OFFSET_MSB           (15 + 5 * BITS_PER_U32)
#define STW_CLS_FLD_EXT_FRAG_INFO_OFFSET_LSB    (16 + 5 * BITS_PER_U32)
#define STW_CLS_FLD_EXT_FRAG_INFO_OFFSET_MSB    (23 + 5 * BITS_PER_U32)
#define STW_CLS_FLD_INT_FRAG_INFO_OFFSET_LSB    (24 + 5 * BITS_PER_U32)
#define STW_CLS_FLD_INT_FRAG_INFO_OFFSET_MSB    (31 + 5 * BITS_PER_U32)
#define STW_CLS_FLD_L3_HDR_OFFSET_5_LSB         (0  + 6 * BITS_PER_U32)
#define STW_CLS_FLD_L3_HDR_OFFSET_5_MSB         (7  + 6 * BITS_PER_U32)
#define STW_CLS_FLD_L3_HDR_OFFSET_4_LSB         (8  + 6 * BITS_PER_U32)
#define STW_CLS_FLD_L3_HDR_OFFSET_4_MSB         (15 + 6 * BITS_PER_U32)
#define STW_CLS_FLD_L3_HDR_OFFSET_3_LSB         (16 + 6 * BITS_PER_U32)
#define STW_CLS_FLD_L3_HDR_OFFSET_3_MSB         (23 + 6 * BITS_PER_U32)
#define STW_CLS_FLD_L3_HDR_OFFSET_2_LSB         (24 + 6 * BITS_PER_U32)
#define STW_CLS_FLD_L3_HDR_OFFSET_2_MSB         (31 + 6 * BITS_PER_U32)
#define STW_CLS_FLD_L3_HDR_OFFSET_1_LSB         (0  + 7 * BITS_PER_U32)
#define STW_CLS_FLD_L3_HDR_OFFSET_1_MSB         (7  + 7 * BITS_PER_U32)
#define STW_CLS_FLD_L3_HDR_OFFSET_0_LSB         (8  + 7 * BITS_PER_U32)
#define STW_CLS_FLD_L3_HDR_OFFSET_0_MSB         (15 + 7 * BITS_PER_U32)
#define STW_CLS_FLD_ANA_PKT_TYPE_LSB            (16 + 7 * BITS_PER_U32)
#define STW_CLS_FLD_ANA_PKT_TYPE_MSB            (23 + 7 * BITS_PER_U32)
#define STW_CLS_FLD_TUNN_INNER_OFFSET_LSB       (24 + 7 * BITS_PER_U32)
#define STW_CLS_FLD_TUNN_INNER_OFFSET_MSB       (26 + 7 * BITS_PER_U32)
#define STW_CLS_FLD_PAYLOAD_OFFSET_LSB          (27 + 7 * BITS_PER_U32)
#define STW_CLS_FLD_PAYLOAD_OFFSET_MSB          (29 + 7 * BITS_PER_U32)
#define STW_CLS_FLD_ERROR_IND_LSB               (30 + 7 * BITS_PER_U32)
#define STW_CLS_FLD_ERROR_IND_MSB               (30 + 7 * BITS_PER_U32)
#define STW_CLS_FLD_LRU_EXP_LSB                 (31 + 7 * BITS_PER_U32)
#define STW_CLS_FLD_LRU_EXP_MSB                 (31 + 7 * BITS_PER_U32)
#define STW_CLS_FLD_RPB_CLID_LSB                (0  + 8 * BITS_PER_U32)
#define STW_CLS_FLD_RPB_CLID_MSB                (9  + 8 * BITS_PER_U32)
#define STW_CLS_FLD_DROP_PKT_LSB                (10 + 8 * BITS_PER_U32)
#define STW_CLS_FLD_DROP_PKT_MSB                (12 + 8 * BITS_PER_U32)
#define STW_CLS_FLD_FV_FLD_1_LSB                (13 + 8 * BITS_PER_U32)
#define STW_CLS_FLD_FV_FLD_1_MSB                (28 + 8 * BITS_PER_U32)

/**
 * @brief STW classifier fields word index
 */
#define STW_CLS_FLD_TTL_EXPIRED_WORD            (0 * BITS_PER_U32)
#define STW_CLS_FLD_TTL_EXPIRED_WORD            (0 * BITS_PER_U32)
#define STW_CLS_FLD_IP_UNSUPP_WORD              (0 * BITS_PER_U32)
#define STW_CLS_FLD_IP_UNSUPP_WORD              (0 * BITS_PER_U32)
#define STW_CLS_FLD_EXT_DF_WORD                 (0 * BITS_PER_U32)
#define STW_CLS_FLD_EXT_DF_WORD                 (0 * BITS_PER_U32)
#define STW_CLS_FLD_INT_DF_WORD                 (0 * BITS_PER_U32)
#define STW_CLS_FLD_INT_DF_WORD                 (0 * BITS_PER_U32)
#define STW_CLS_FLD_EXT_FRAG_TYPE_WORD          (0 * BITS_PER_U32)
#define STW_CLS_FLD_EXT_FRAG_TYPE_WORD          (0 * BITS_PER_U32)
#define STW_CLS_FLD_INT_FRAG_TYPE_WORD          (0 * BITS_PER_U32)
#define STW_CLS_FLD_INT_FRAG_TYPE_WORD          (0 * BITS_PER_U32)
#define STW_CLS_FLD_TCP_FIN_WORD                (0 * BITS_PER_U32)
#define STW_CLS_FLD_TCP_FIN_WORD                (0 * BITS_PER_U32)
#define STW_CLS_FLD_TCP_SYN_WORD                (0 * BITS_PER_U32)
#define STW_CLS_FLD_TCP_SYN_WORD                (0 * BITS_PER_U32)
#define STW_CLS_FLD_TCP_RST_WORD                (0 * BITS_PER_U32)
#define STW_CLS_FLD_TCP_RST_WORD                (0 * BITS_PER_U32)
#define STW_CLS_FLD_TCP_ACK_WORD                (0 * BITS_PER_U32)
#define STW_CLS_FLD_TCP_ACK_WORD                (0 * BITS_PER_U32)
#define STW_CLS_FLD_TCP_DATA_OFFSET_WORD        (0 * BITS_PER_U32)
#define STW_CLS_FLD_TCP_DATA_OFFSET_WORD        (0 * BITS_PER_U32)
#define STW_CLS_FLD_EXT_L3_OFFSET_WORD          (0 * BITS_PER_U32)
#define STW_CLS_FLD_EXT_L3_OFFSET_WORD          (0 * BITS_PER_U32)
#define STW_CLS_FLD_INT_L3_OFFSET_WORD          (0 * BITS_PER_U32)
#define STW_CLS_FLD_INT_L3_OFFSET_WORD          (0 * BITS_PER_U32)
#define STW_CLS_FLD_EXT_L4_OFFSET_WORD          (1 * BITS_PER_U32)
#define STW_CLS_FLD_EXT_L4_OFFSET_WORD          (1 * BITS_PER_U32)
#define STW_CLS_FLD_INT_L4_OFFSET_WORD          (1 * BITS_PER_U32)
#define STW_CLS_FLD_INT_L4_OFFSET_WORD          (1 * BITS_PER_U32)
#define STW_CLS_FLD_EXT_FRAG_INFO_OFFSET_WORD   (1 * BITS_PER_U32)
#define STW_CLS_FLD_EXT_FRAG_INFO_OFFSET_WORD   (1 * BITS_PER_U32)
#define STW_CLS_FLD_INT_FRAG_INFO_OFFSET_WORD   (1 * BITS_PER_U32)
#define STW_CLS_FLD_INT_FRAG_INFO_OFFSET_WORD   (1 * BITS_PER_U32)
#define STW_CLS_FLD_L3_HDR_OFFSET_5_WORD        (2 * BITS_PER_U32)
#define STW_CLS_FLD_L3_HDR_OFFSET_5_WORD        (2 * BITS_PER_U32)
#define STW_CLS_FLD_L3_HDR_OFFSET_4_WORD        (2 * BITS_PER_U32)
#define STW_CLS_FLD_L3_HDR_OFFSET_4_WORD        (2 * BITS_PER_U32)
#define STW_CLS_FLD_L3_HDR_OFFSET_3_WORD        (2 * BITS_PER_U32)
#define STW_CLS_FLD_L3_HDR_OFFSET_3_WORD        (2 * BITS_PER_U32)
#define STW_CLS_FLD_L3_HDR_OFFSET_2_WORD        (2 * BITS_PER_U32)
#define STW_CLS_FLD_L3_HDR_OFFSET_2_WORD        (2 * BITS_PER_U32)
#define STW_CLS_FLD_L3_HDR_OFFSET_1_WORD        (3 * BITS_PER_U32)
#define STW_CLS_FLD_L3_HDR_OFFSET_1_WORD        (3 * BITS_PER_U32)
#define STW_CLS_FLD_L3_HDR_OFFSET_0_WORD        (3 * BITS_PER_U32)
#define STW_CLS_FLD_L3_HDR_OFFSET_0_WORD        (3 * BITS_PER_U32)
#define STW_CLS_FLD_ANA_PKT_TYPE_WORD           (3 * BITS_PER_U32)
#define STW_CLS_FLD_ANA_PKT_TYPE_WORD           (3 * BITS_PER_U32)
#define STW_CLS_FLD_TUNN_INNER_OFFSET_WORD      (3 * BITS_PER_U32)
#define STW_CLS_FLD_TUNN_INNER_OFFSET_WORD      (3 * BITS_PER_U32)
#define STW_CLS_FLD_PAYLOAD_OFFSET_WORD         (3 * BITS_PER_U32)
#define STW_CLS_FLD_PAYLOAD_OFFSET_WORD         (3 * BITS_PER_U32)
#define STW_CLS_FLD_ERROR_IND_WORD              (3 * BITS_PER_U32)
#define STW_CLS_FLD_ERROR_IND_WORD              (3 * BITS_PER_U32)
#define STW_CLS_FLD_LRU_EXP_WORD                (3 * BITS_PER_U32)
#define STW_CLS_FLD_LRU_EXP_WORD                (3 * BITS_PER_U32)
#define STW_CLS_FLD_RPB_CLID_WORD               (4 * BITS_PER_U32)
#define STW_CLS_FLD_RPB_CLID_WORD               (4 * BITS_PER_U32)
#define STW_CLS_FLD_DROP_PKT_WORD               (4 * BITS_PER_U32)
#define STW_CLS_FLD_DROP_PKT_WORD               (4 * BITS_PER_U32)
#define STW_CLS_FLD_FV_FLD_1_WORD               (4 * BITS_PER_U32)
#define STW_CLS_FLD_FV_FLD_1_WORD               (4 * BITS_PER_U32)

/**
 * @brief STW classifier fields description
 */
#define STW_CLS_FLD_TTL_EXPIRED_DESC            "TTL Expired"
#define STW_CLS_FLD_IP_UNSUPP_DESC              "IP Unsupported"
#define STW_CLS_FLD_EXT_DF_DESC                 "External Don't Frag"
#define STW_CLS_FLD_INT_DF_DESC                 "Internal Don't Frag"
#define STW_CLS_FLD_EXT_FRAG_TYPE_DESC          "External Frag Type"
#define STW_CLS_FLD_INT_FRAG_TYPE_DESC          "Internal Frag Type"
#define STW_CLS_FLD_TCP_FIN_DESC                "TCP Fin"
#define STW_CLS_FLD_TCP_SYN_DESC                "TCP Syn"
#define STW_CLS_FLD_TCP_RST_DESC                "TCP Reset"
#define STW_CLS_FLD_TCP_ACK_DESC                "TCP Ack"
#define STW_CLS_FLD_TCP_DATA_OFFSET_DESC        "TCP Data Offset"
#define STW_CLS_FLD_EXT_L3_OFFSET_DESC          "External L3 Offset"
#define STW_CLS_FLD_INT_L3_OFFSET_DESC          "Internal L3 Offset"
#define STW_CLS_FLD_EXT_L4_OFFSET_DESC          "External L4 Offset"
#define STW_CLS_FLD_INT_L4_OFFSET_DESC          "Internal L4 Offset"
#define STW_CLS_FLD_EXT_FRAG_INFO_OFFSET_DESC   "External Frag Info Offset"
#define STW_CLS_FLD_INT_FRAG_INFO_OFFSET_DESC   "Internal Frag Info Offset"
#define STW_CLS_FLD_L3_HDR_OFFSET_5_DESC        "L3 Header Offset 5"
#define STW_CLS_FLD_L3_HDR_OFFSET_4_DESC        "L3 Header Offset 4"
#define STW_CLS_FLD_L3_HDR_OFFSET_3_DESC        "L3 Header Offset 3"
#define STW_CLS_FLD_L3_HDR_OFFSET_2_DESC        "L3 Header Offset 2"
#define STW_CLS_FLD_L3_HDR_OFFSET_1_DESC        "L3 Header Offset 1"
#define STW_CLS_FLD_L3_HDR_OFFSET_0_DESC        "L3 Header Offset 0"
#define STW_CLS_FLD_ANA_PKT_TYPE_DESC           "Analyzer Packet Type"
#define STW_CLS_FLD_TUNN_INNER_OFFSET_DESC      "Tunnel Inner Offset"
#define STW_CLS_FLD_PAYLOAD_OFFSET_DESC         "Payload Offset"
#define STW_CLS_FLD_ERROR_IND_DESC              "Error"
#define STW_CLS_FLD_LRU_EXP_DESC                "LRU Exception"
#define STW_CLS_FLD_RPB_CLID_DESC               "RPB CLID"
#define STW_CLS_FLD_DROP_PKT_DESC               "Drop"
#define STW_CLS_FLD_FV_FLD_1_DESC               "FV Field"

/**
 * @struct cls_hw_stw
 * @brief classifier debug status word registers definition
 */
struct cls_hw_stw {
	u32 word[STW_CLS_WORD_CNT];
};

struct cls_database {
	void         *cuckoo;
	u32          *cls_si_req;
	s32          cls_cmd_status;
	u32          hash_mask;
	u32          num_sessions;
	u32          num_lu_threads;
	void         *si_base_virt;
	dma_addr_t   si_base_phys;
	size_t       si_sz;
	void         *ht1_base_virt;
	dma_addr_t   ht1_base_phys;
	size_t       ht1_sz;
	void         *ht2_base_virt;
	dma_addr_t   ht2_base_phys;
	size_t       ht2_sz;
	u32          num_retries;
};

enum cls_sig_cache {
	CLS_SIG_CACHE1,
	CLS_SIG_CACHE2,
	CLS_SIG_CACHE_CNT
};

/**
 * @brief Set the polling num retries
 * @param n number of polling retries
 */
void cls_n_retries_set(u32 n);

/**
 * @brief Get the polling num retries
 * @param n number of polling retries
 */
void cls_n_retries_get(u32 *n);

/**
 * @brief Set the hash tables size
 * @param size number of element in each hash table
 * @return s32 0 on success, error code otherwise
 */
s32 cls_hash_size_set(u32 size);

/**
 * @brief Get the hash tables size
 * @param size number of element in each hash table
 * @return s32 0 on success, error code otherwise
 */
s32 cls_hash_size_get(u32 *size);

/**
 * @brief Reset the lookup counters
 * @param void
 */
void cls_lu_stats_reset(void);

/**
 * @brief Set the threshold for the long lookup process counter
 * @param thr threshold in clocks units
 */
void cls_long_lu_thr_set(u32 thr);

/**
 * @brief Get the threshold for the long lookup process counter
 * @param thr threshold in clocks units
 * @return s32 0 on success, error code otherwise
 */
s32 cls_long_lu_thr_get(u32 *thr);

/**
 * @brief Set the number of lookup threads
 * @param num_threads number of lookup threads
 * @return s32 0 on success, error code otherwise
 */
s32 cls_num_lu_thread_set(u32 num_threads);

/**
 * @brief Get the number of lookup threads
 * @param num_threads number of lookup threads
 * @return s32 0 on success, error code otherwise
 */
s32 cls_num_lu_thread_get(u32 *num_threads);

/**
 * @brief Get the sig error counter, incremented when 2 sig are
 *        inserted to the same hash bucket
 * @param sig_err sig error counter
 * @return s32 0 on success, error code otherwise
 */
s32 cls_sig_error_cntr_get(u32 *sig_err);

/**
 * @brief Set the lookup thread configuration
 * @param drop_flow_dis true for disabling the drop flow
 * @param search_mode fasle for normal mode, true for 'on sig'
 *        mode
 * @param fm_flow true for full match, false for sig match
 * @param fv_masked true to get masked field vector for the debug
 *                  field vector, false to get unmasked field vector
 */
void cls_thrd_cfg_set(bool drop_flow_dis, bool search_mode, bool fm_flow,
		      bool fv_masked);

/**
 * @brief Get the lookup thread configuration
 * @param drop_flow_dis true for disabling the drop flow
 * @param search_mode fasle for normal mode, true for 'on sig'
 *        mode
 * @param fm_flow true for full match, false for sig match
 * @param fv_masked true to get masked field vector for the debug
 *                  field vector, false to get unmasked field vector
 * @return s32 0 on success, error code otherwise
 */
s32 cls_thrd_cfg_get(bool *drop_flow_dis, bool *search_mode, bool *fm_flow,
		     bool *fv_masked);

/**
 * @brief Set the hash table 1 base address
 * @param ht1_base_addr hash table 1 base address
 */
void cls_hash1_base_addr_set(u64 ht1_base_addr);

/**
 * @brief Set the hash table 2 base address
 * @param ht2_base_addr hash table 2 base address
 */
void cls_hash2_base_addr_set(u64 ht2_base_addr);

/**
 * @brief Set the session information base address
 * @param si_base_addr session information base address
 */
void cls_si_base_addr_set(u64 si_base_addr);

/**
 * @brief Set the HW FV endian swap mode
 * @param in_swap swap FV at classifier ingress
 * @param eg_swap swap FV at classifier egress
 */
void cls_fv_endian_swap_set(bool in_swap, bool eg_swap);

/**
 * @brief Get the HW FV endian swap mode
 * @param in_swap swap FV at classifier ingress
 * @param eg_swap swap FV at classifier egress
 */
s32 cls_fv_endian_swap_get(bool *in_swap, bool *eg_swap);

/**
 * @brief Set the signature cache long transaction counter
 *        threshold
 * @param thr threshold in cycles, above this number of cycles
 *            the counter will be incremented
 */
void cls_sig_cache_long_trans_cntr_thr_set(u64 thr);

/**
 * @brief Set the signature cache long transaction counter
 *        threshold
 * @param thr threshold in cycles, above this number of cycles
 *            the counter will be incremented
 * @return s32 0 on success, error code otherwise
 */
s32 cls_sig_cache_long_trans_cntr_thr_get(u64 *thr);

/**
 * @brief Set the signature cache configuration
 * @param enable enable/disable the signature cache
 * @param invalidate invalidate the cache in 1 cycle
 */
void cls_sig_cache_cfg_set(bool enable, bool invalidate);

/**
 * @brief Get the signature cache configuration
 * @param enable enable/disable the signature cache
 * @return s32 0 on success, error code otherwise
 */
s32 cls_sig_cache_cfg_get(bool *enable);

/**
 * @brief Get the signature cache hit counter and long
 *        transaction counter
 * @param cache cache type CLS_SIG_CACHE1/CLS_SIG_CACHE2
 * @param hit signature cache hit counter
 * @param long_trns signature cache long transaction counter
 * @return s32 0 on success, error code otherwise
 */
s32 cls_sig_cache_stats_get(enum cls_sig_cache cache, u32 *hit, u32 *long_trns);

/**
 * @brief Dump cuckoo hash statistics
 * @param void
 */
void cls_cuckoo_hash_stats_dump(void);

/**
 * @brief Dump cuckoo hash configuration
 * @param void
 */
void cls_cuckoo_hash_cfg_dump(void);

/**
 * @brief Get sessions cache bypass status
 * @note this cache is used for caching the session information (SI)
 * @return bool true is the cache is in bypass mode, false otherwise
 */
bool cls_sess_cache_bypass_get(void);

/**
 * @brief Set sessions cache byepass
 * @note this cache is used for caching the session information (SI)
 * @param bypass
 */
void cls_sess_cache_bypass_set(bool bypass);

/**
 * @brief Get sessions cache counters
 * @note this cache is used for caching the session information (SI)
 * @param read_trans number of read transactions
 * @param read_hits number of cache hits
 * @return s32 0 on success, error code otherwise
 */
s32 cls_sess_cache_stats_get(u64 *read_trans, u64 *read_hits);

/**
 * @brief Get debug status word, the status word returned here is from
 *        the egress of the classifier HW
 * @param stw status word buffer
 * @return s32 0 on success, error code otherwise
 */
s32 cls_dbg_stw_get(struct cls_hw_stw *stw);

s32 __cls_ddr_allocate(void);
void __cls_ddr_free(void);

#ifdef CONFIG_DEBUG_FS
/**
 * @brief Classifier debug init
 * @return s32 0 on success, error code otherwise
 */
s32 cls_dbg_init(struct dentry *parent);

/**
 * @brief Classifier debug cleanup
 */
void cls_dbg_clean(void);
#else /* !CONFIG_DEBUG_FS */
static inline s32 cls_dbg_init(struct dentry *parent)
{
	return 0;
}

static inline void cls_dbg_clean(void)
{
}
#endif /* CONFIG_DEBUG_FS */
#endif /* __CLASSIFIER_INTERNAL_H__ */
