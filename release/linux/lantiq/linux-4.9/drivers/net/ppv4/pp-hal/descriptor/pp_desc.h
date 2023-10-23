/*
 * pp_desc.h
 * Description: PP descriptor definitions
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018-2019 Intel Corporation
 */

#ifndef __PP_DESC_H__
#define __PP_DESC_H__

#include <linux/types.h>
#include <linux/bitops.h>
#include <linux/pp_api.h>

/**
 * @define QOS_DESC_WORDS_CNT
 * @brief qos descriptor size in 32bit words granularity
 */
#define QOS_DESC_WORDS_CNT             (4)

#ifdef CONFIG_PPV4_LGM
/**
 * @enum pp_desc_fld
 * @brief descriptor fields enumaration
 */
enum pp_desc_fld {
	DESC_FLD_FIRST,
	DESC_FLD_PS_A = DESC_FLD_FIRST,
	DESC_FLD_PS_B,
	DESC_FLD_DATA_OFF,
	DESC_FLD_PMAC,
	DESC_FLD_COLOR,
	DESC_FLD_SRC_POOL,
	DESC_FLD_BUFF_PTR,
	DESC_FLD_PRE_L2,
	DESC_FLD_TS,
	DESC_FLD_TX_PORT,
	DESC_FLD_BM_POLICY,
	DESC_FLD_PKT_LEN,
	DESC_FLD_LSP_PKT,
	DESC_FLD_OWN,
	/* descriptor UD section */
	DESC_FLD_INT_PROTO_INFO,
	DESC_FLD_EXT_PROTO_INFO,
	DESC_FLD_RX_PORT,
	DESC_FLD_TTL_EXP,
	DESC_FLD_IP_OPT,
	DESC_FLD_EXT_DF,
	DESC_FLD_INT_DF,
	DESC_FLD_EXT_FRAG_TYPE,
	DESC_FLD_INT_FRAG_TYPE,
	DESC_FLD_TCP_FIN,
	DESC_FLD_TCP_SYN,
	DESC_FLD_TCP_RST,
	DESC_FLD_TCP_ACK,
	DESC_FLD_TCP_DATA_OFF,
	DESC_FLD_EXT_L3_OFF,
	DESC_FLD_INT_L3_OFF,
	DESC_FLD_EXT_L4_OFF,
	DESC_FLD_INT_L4_OFF,
	DESC_FLD_EXT_FRG_INFO_OFF,
	DESC_FLD_INT_FRG_INFO_OFF,
	DESC_FLD_TDOX_FLOW,
	DESC_FLD_LRO,
	DESC_FLD_L2_OFF,
	DESC_FLD_SESSION_ID,
	DESC_FLD_IS_ALT_SESSION,
	DESC_FLD_ERROR,
	DESC_FLD_DROP_PKT,
	DESC_FLD_HASH_SIG,
	DESC_FLD_H1,
	DESC_FLD_H2_L,
	DESC_FLD_H2_H,
	DESC_FLD_L3_OFF_5,
	DESC_FLD_L3_OFF_4,
	DESC_FLD_L3_OFF_3,
	DESC_FLD_L3_OFF_2,
	DESC_FLD_L3_OFF_1,
	DESC_FLD_L3_OFF_0,
	DESC_FLD_TUNN_OFF_ID,
	DESC_FLD_PAYLD_OFF_ID,
	DESC_FLD_PRV_PS_A,
	DESC_FLD_PRV_PS_B,
	DESC_FLD_LAST = DESC_FLD_PRV_PS_B,
	DESC_FLDS_NUM
};

/**
 * @brief Session information fields masks
 */
#define DESC_FLD_PS_A_LSB              (0  + 0  * BITS_PER_U32)
#define DESC_FLD_PS_A_MSB              (31 + 0  * BITS_PER_U32)
#define DESC_FLD_PS_B_LSB              (0  + 1  * BITS_PER_U32)
#define DESC_FLD_PS_B_MSB              (15 + 1  * BITS_PER_U32)
#define DESC_FLD_DATA_OFF_LSB          (16 + 1  * BITS_PER_U32)
#define DESC_FLD_DATA_OFF_MSB          (24 + 1  * BITS_PER_U32)
#define DESC_FLD_PMAC_LSB              (25 + 1  * BITS_PER_U32)
#define DESC_FLD_PMAC_MSB              (25 + 1  * BITS_PER_U32)
#define DESC_FLD_COLOR_LSB             (26 + 1  * BITS_PER_U32)
#define DESC_FLD_COLOR_MSB             (27 + 1  * BITS_PER_U32)
#define DESC_FLD_SRC_POOL_LSB          (28 + 1  * BITS_PER_U32)
#define DESC_FLD_SRC_POOL_MSB          (31 + 1  * BITS_PER_U32)
#define DESC_FLD_BUFF_PTR_LSB          (0  + 2  * BITS_PER_U32)
#define DESC_FLD_BUFF_PTR_MSB          (28 + 2  * BITS_PER_U32)
#define DESC_FLD_PRE_L2_LSB            (29 + 2  * BITS_PER_U32)
#define DESC_FLD_PRE_L2_MSB            (30 + 2  * BITS_PER_U32)
#define DESC_FLD_TS_LSB                (31 + 2  * BITS_PER_U32)
#define DESC_FLD_TS_MSB                (31 + 2  * BITS_PER_U32)
#define DESC_FLD_TX_PORT_LSB           (0  + 3  * BITS_PER_U32)
#define DESC_FLD_TX_PORT_MSB           (7  + 3  * BITS_PER_U32)
#define DESC_FLD_BM_POLICY_LSB         (8  + 3  * BITS_PER_U32)
#define DESC_FLD_BM_POLICY_MSB         (15 + 3  * BITS_PER_U32)
#define DESC_FLD_PKT_LEN_LSB           (16 + 3  * BITS_PER_U32)
#define DESC_FLD_PKT_LEN_MSB           (29 + 3  * BITS_PER_U32)
#define DESC_FLD_LSP_PKT_LSB           (30 + 3  * BITS_PER_U32)
#define DESC_FLD_LSP_PKT_MSB           (30 + 3  * BITS_PER_U32)
#define DESC_FLD_OWN_LSB               (31 + 3  * BITS_PER_U32)
#define DESC_FLD_OWN_MSB               (31 + 3  * BITS_PER_U32)
#define DESC_FLD_RX_PORT_LSB           (8  + 4  * BITS_PER_U32)
#define DESC_FLD_RX_PORT_MSB           (15 + 4  * BITS_PER_U32)
#define DESC_FLD_INT_PROTO_INFO_LSB    (0  + 4  * BITS_PER_U32)
#define DESC_FLD_INT_PROTO_INFO_MSB    (3  + 4  * BITS_PER_U32)
#define DESC_FLD_EXT_PROTO_INFO_LSB    (4  + 4  * BITS_PER_U32)
#define DESC_FLD_EXT_PROTO_INFO_MSB    (7  + 4  * BITS_PER_U32)
#define DESC_FLD_TTL_EXP_LSB           (16 + 4  * BITS_PER_U32)
#define DESC_FLD_TTL_EXP_MSB           (16 + 4  * BITS_PER_U32)
#define DESC_FLD_IP_OPT_LSB            (17 + 4  * BITS_PER_U32)
#define DESC_FLD_IP_OPT_MSB            (17 + 4  * BITS_PER_U32)
#define DESC_FLD_EXT_DF_LSB            (18 + 4  * BITS_PER_U32)
#define DESC_FLD_EXT_DF_MSB            (18 + 4  * BITS_PER_U32)
#define DESC_FLD_INT_DF_LSB            (19 + 4  * BITS_PER_U32)
#define DESC_FLD_INT_DF_MSB            (19 + 4  * BITS_PER_U32)
#define DESC_FLD_EXT_FRAG_TYPE_LSB     (20 + 4  * BITS_PER_U32)
#define DESC_FLD_EXT_FRAG_TYPE_MSB     (21 + 4  * BITS_PER_U32)
#define DESC_FLD_INT_FRAG_TYPE_LSB     (22 + 4  * BITS_PER_U32)
#define DESC_FLD_INT_FRAG_TYPE_MSB     (23 + 4  * BITS_PER_U32)
#define DESC_FLD_TCP_FIN_LSB           (24 + 4  * BITS_PER_U32)
#define DESC_FLD_TCP_FIN_MSB           (24 + 4  * BITS_PER_U32)
#define DESC_FLD_TCP_SYN_LSB           (25 + 4  * BITS_PER_U32)
#define DESC_FLD_TCP_SYN_MSB           (25 + 4  * BITS_PER_U32)
#define DESC_FLD_TCP_RST_LSB           (26 + 4  * BITS_PER_U32)
#define DESC_FLD_TCP_RST_MSB           (26 + 4  * BITS_PER_U32)
#define DESC_FLD_TCP_ACK_LSB           (27 + 4  * BITS_PER_U32)
#define DESC_FLD_TCP_ACK_MSB           (27 + 4  * BITS_PER_U32)
#define DESC_FLD_TCP_DATA_OFF_LSB      (28 + 4  * BITS_PER_U32)
#define DESC_FLD_TCP_DATA_OFF_MSB      (31 + 4  * BITS_PER_U32)
#define DESC_FLD_EXT_L3_OFF_LSB        (0  + 5  * BITS_PER_U32)
#define DESC_FLD_EXT_L3_OFF_MSB        (7  + 5  * BITS_PER_U32)
#define DESC_FLD_INT_L3_OFF_LSB        (8  + 5  * BITS_PER_U32)
#define DESC_FLD_INT_L3_OFF_MSB        (15 + 5  * BITS_PER_U32)
#define DESC_FLD_EXT_L4_OFF_LSB        (16 + 5  * BITS_PER_U32)
#define DESC_FLD_EXT_L4_OFF_MSB        (23 + 5  * BITS_PER_U32)
#define DESC_FLD_INT_L4_OFF_LSB        (24 + 5  * BITS_PER_U32)
#define DESC_FLD_INT_L4_OFF_MSB        (31 + 5  * BITS_PER_U32)
#define DESC_FLD_EXT_FRG_INFO_OFF_LSB  (0  + 6  * BITS_PER_U32)
#define DESC_FLD_EXT_FRG_INFO_OFF_MSB  (7  + 6  * BITS_PER_U32)
#define DESC_FLD_INT_FRG_INFO_OFF_LSB  (8  + 6  * BITS_PER_U32)
#define DESC_FLD_INT_FRG_INFO_OFF_MSB  (15 + 6  * BITS_PER_U32)
#define DESC_FLD_TDOX_FLOW_LSB         (16 + 6  * BITS_PER_U32)
#define DESC_FLD_TDOX_FLOW_MSB         (25 + 6  * BITS_PER_U32)
#define DESC_FLD_LRO_LSB               (28 + 6  * BITS_PER_U32)
#define DESC_FLD_LRO_MSB               (28 + 6  * BITS_PER_U32)
#define DESC_FLD_L2_OFF_LSB            (30 + 6  * BITS_PER_U32)
#define DESC_FLD_L2_OFF_MSB            (31 + 6  * BITS_PER_U32)
#define DESC_FLD_SESSION_ID_LSB        (0  + 7  * BITS_PER_U32)
#define DESC_FLD_SESSION_ID_MSB        (23 + 7  * BITS_PER_U32)
#define DESC_FLD_IS_ALT_SESSION_LSB    (24 + 7  * BITS_PER_U32)
#define DESC_FLD_IS_ALT_SESSION_MSB    (24 + 7  * BITS_PER_U32)
#define DESC_FLD_ERROR_LSB             (28 + 7  * BITS_PER_U32)
#define DESC_FLD_ERROR_MSB             (28 + 7  * BITS_PER_U32)
#define DESC_FLD_DROP_PKT_LSB          (29 + 7  * BITS_PER_U32)
#define DESC_FLD_DROP_PKT_MSB          (31 + 7  * BITS_PER_U32)
#define DESC_FLD_HASH_SIG_LSB          (0  + 8  * BITS_PER_U32)
#define DESC_FLD_HASH_SIG_MSB          (31 + 8  * BITS_PER_U32)
#define DESC_FLD_H1_LSB                (0  + 9  * BITS_PER_U32)
#define DESC_FLD_H1_MSB                (19 + 9  * BITS_PER_U32)
#define DESC_FLD_H2_L_LSB              (20 + 9  * BITS_PER_U32)
#define DESC_FLD_H2_L_MSB              (31 + 9  * BITS_PER_U32)
#define DESC_FLD_H2_H_LSB              (0  + 10 * BITS_PER_U32)
#define DESC_FLD_H2_H_MSB              (7  + 10 * BITS_PER_U32)
#define DESC_FLD_L3_OFF_5_LSB          (8  + 10 * BITS_PER_U32)
#define DESC_FLD_L3_OFF_5_MSB          (15 + 10 * BITS_PER_U32)
#define DESC_FLD_L3_OFF_4_LSB          (16 + 10 * BITS_PER_U32)
#define DESC_FLD_L3_OFF_4_MSB          (23 + 10 * BITS_PER_U32)
#define DESC_FLD_L3_OFF_3_LSB          (24 + 10 * BITS_PER_U32)
#define DESC_FLD_L3_OFF_3_MSB          (31 + 10 * BITS_PER_U32)
#define DESC_FLD_L3_OFF_2_LSB          (0  + 11 * BITS_PER_U32)
#define DESC_FLD_L3_OFF_2_MSB          (7  + 11 * BITS_PER_U32)
#define DESC_FLD_L3_OFF_1_LSB          (8  + 11 * BITS_PER_U32)
#define DESC_FLD_L3_OFF_1_MSB          (15 + 11 * BITS_PER_U32)
#define DESC_FLD_L3_OFF_0_LSB          (16 + 11 * BITS_PER_U32)
#define DESC_FLD_L3_OFF_0_MSB          (23 + 11 * BITS_PER_U32)
#define DESC_FLD_TUNN_OFF_ID_LSB       (24 + 11 * BITS_PER_U32)
#define DESC_FLD_TUNN_OFF_ID_MSB       (26 + 11 * BITS_PER_U32)
#define DESC_FLD_PAYLD_OFF_ID_LSB      (27 + 11 * BITS_PER_U32)
#define DESC_FLD_PAYLD_OFF_ID_MSB      (29 + 11 * BITS_PER_U32)
#define DESC_FLD_PRV_PS_A_LSB          (0  + 12 * BITS_PER_U32)
#define DESC_FLD_PRV_PS_A_MSB          (31 + 12 * BITS_PER_U32)
#define DESC_FLD_PRV_PS_B_LSB          (0  + 13 * BITS_PER_U32)
#define DESC_FLD_PRV_PS_B_MSB          (15 + 13 * BITS_PER_U32)

/**
 * @define QOS_DESC_PKT_PTR_SHIFT
 * @brief Once packet pointer is written to descriptor is must be shifted
 */
#define QOS_DESC_PKT_PTR_SHIFT         (7)

/**
 * @define UD_DESC_WORDS_CNT
 * @brief ud size in descriptor in 32bit words granularity
 */
#define UD_DESC_WORDS_CNT              (10)

/**
 * @define DESC_WORDS_CNT
 * @brief descriptor size in 32bit words granularity
 */
#define DESC_WORDS_CNT                 (QOS_DESC_WORDS_CNT + UD_DESC_WORDS_CNT)

/**
 * @struct pp_qos_hw_desc
 * @brief  PP QoS HW descriptor definition it is impossible to map
 *         the descriptor hw structure exactly so we just define
 *         an array of 12 32bit registers
 */
struct pp_qos_hw_desc {
	u32 word[QOS_DESC_WORDS_CNT];
};

/**
 * @struct pp_hw_desc
 * @brief  PP HW descriptor definition it is impossible to map
 *         the descriptor hw structure exactly so we just define
 *         an array of 12 32bit registers
 */
struct pp_hw_desc {
	u32 word[DESC_WORDS_CNT];
};

/**
 * @struct pp_qos_desc
 * @brief qos pp decriptor host structure
 */
struct pp_qos_desc {
	u64 ps:48,           /*! protocol specific, up to 48bits         */
	    data_off:9,      /*! data_pointer = buff_ptr + data_off      */
	    src_pool:4;      /*! from which pool the buffer was taken    */
	u64 buff_ptr:36;     /*! buffer pointer                          */
	u8  tx_port;         /*! Egress Port @ RXDMA output              */
	u8  bm_policy;       /*! from which policy the buffer was taken  */
	u16 pkt_len:14,      /*! packet length. Defaults incl preL2 & ts */
	    lsp_pkt:1;       /*! last slow path packet, used for syncq   */
	u8  pmac:1,           /*! 16B of PMAC header Part of Pre L2       */
	    color:2;          /*! color at egress                         */
	u8  pre_l2_sz:6,      /*! preL2 size incl PMAC hdr                */
	    ts:1,             /*! indicate timestemp existence            */
	    own:1;            /*! descriptor ownership                    */
};

/**
 * @brief Decode qos descriptor hw buffer to host descriptor
 *        form
 * @param desc host descriptor to store the host form
 * @param hw_desc hw descriptor to decode
 * @return s32 0 on success, error code otherwise
 */
s32 pp_qos_desc_decode(struct pp_qos_desc *desc,
		       const struct pp_qos_hw_desc *qos_hw_desc);

/**
 * @brief Decode descriptor top hw buffer to host descriptor
 *        form
 * @param desc host descriptor to store the host form
 * @param hw_desc hw descriptor to decode
 * @return s32 0 on success, error code otherwise
 */
s32 pp_desc_top_decode(struct pp_desc *desc, const struct pp_hw_desc *hw_desc);

/**
 * @brief Decode descriptor hw buffer to host descriptor form
 * @param desc host descriptor to store the host form
 * @param hw_desc hw descriptor to decode
 * @return s32 0 on success, error code otherwise
 */
s32 pp_desc_decode(struct pp_desc *desc, const struct pp_hw_desc *hw_desc);

/**
 * @brief Dump dbg QoS descriptor in host format
 * @param desc host descriptor
 * @return s32 0 on success, error code otherwise
 */
s32 pp_qos_desc_dump(struct pp_qos_desc *desc);

/**
 * @brief Dump dbg descriptor in host format
 * @param desc host descriptor
 * @return s32 0 on success, error code otherwise
 */
s32 pp_desc_top_dump(struct pp_desc *desc);

/**
 * @brief Dump descriptor in host format
 * @param desc host descriptor
 * @return s32 0 on success, error code otherwise
 */
s32 pp_desc_dump(struct pp_desc *desc);

/**
 * @brief Initialize some stuff to make decoding easier
 */
void pp_desc_init(void);
#else
/**
 * @define QOS_DESC_PKT_PTR_SHIFT
 * @brief Once packet pointer is written to descriptor is must be shifted
 */
#define QOS_DESC_PKT_PTR_SHIFT         (0)

static inline void pp_desc_init(void)
{
}
#endif /* CONFIG_PPV4_LGM */
#endif /* __PP_DESC_H__ */
