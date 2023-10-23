/*
 * pp_desc.c
 * Description: PP descriptor module
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018-2019 Intel Corporation
 */

#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/types.h>
#include "pp_common.h"
#include "pp_fv.h"
#include "pp_desc.h"

#ifdef pr_fmt
#undef pr_fmt
#define pr_fmt(fmt) "[PP_DESC]: " fmt
#endif

/**
 * @brief descriptor fields encoding/decoding info
 */
static struct buf_fld_info desc_flds[DESC_FLDS_NUM];

/**
 * @brief Get descriptor field from hardware descriptor buffer
 * @note for internal use only, doesn't verify input parameters
 * @param desc hw descriptor to get the field from
 * @param id field id
 * @return s32 the field value
 */
static inline s32 __desc_fld_get(const struct pp_hw_desc *desc,
				 enum pp_desc_fld id)
{
	return buf_fld_get(&desc_flds[id], desc->word);
}

/**
 * @brief Decode UD hw buffer to host UD form
 * @param ud host UD descriptor to store the host form
 * @param desc hw descriptor to decode
 * @return s32 0 on success, error code otherwise
 */
static s32 __desc_ud_decode(struct pp_pkt_ud *ud,
			    const struct pp_hw_desc *desc)
{
	if (unlikely(!(!ptr_is_null(ud) &&
		       !ptr_is_null(desc))))
		return -EINVAL;

	ud->rx_port        = __desc_fld_get(desc, DESC_FLD_RX_PORT);
	ud->tdox_flow      = __desc_fld_get(desc, DESC_FLD_TDOX_FLOW);
	ud->lro            = __desc_fld_get(desc, DESC_FLD_LRO);
	ud->sess_id        = __desc_fld_get(desc, DESC_FLD_SESSION_ID);
	ud->is_exc_sess    = __desc_fld_get(desc, DESC_FLD_IS_ALT_SESSION);
	ud->hash_sig       = __desc_fld_get(desc, DESC_FLD_HASH_SIG);
	ud->hash_h1        = __desc_fld_get(desc, DESC_FLD_H1);
	ud->hash_h2        = __desc_fld_get(desc, DESC_FLD_H2_H) << 12 |
			     __desc_fld_get(desc, DESC_FLD_H2_L);
#ifdef PP_FULL_DESC_DBG
	ud->int_proto_info = __desc_fld_get(desc, DESC_FLD_INT_PROTO_INFO);
	ud->ext_proto_info = __desc_fld_get(desc, DESC_FLD_EXT_PROTO_INFO);
	ud->ttl_exp       = __desc_fld_get(desc, DESC_FLD_TTL_EXP);
	ud->ip_opt        = __desc_fld_get(desc, DESC_FLD_IP_OPT);
	ud->ext_df        = __desc_fld_get(desc, DESC_FLD_EXT_DF);
	ud->int_df        = __desc_fld_get(desc, DESC_FLD_INT_DF);
	ud->ext_frag_type = __desc_fld_get(desc, DESC_FLD_EXT_FRAG_TYPE);
	ud->int_frag_type = __desc_fld_get(desc, DESC_FLD_INT_FRAG_TYPE);
	ud->tcp_fin       = __desc_fld_get(desc, DESC_FLD_TCP_FIN);
	ud->tcp_syn       = __desc_fld_get(desc, DESC_FLD_TCP_SYN);
	ud->tcp_rst       = __desc_fld_get(desc, DESC_FLD_TCP_RST);
	ud->tcp_ack       = __desc_fld_get(desc, DESC_FLD_TCP_ACK);
	ud->tcp_data_off  = __desc_fld_get(desc, DESC_FLD_TCP_DATA_OFF) << 2;
	ud->ext_l3_off    = __desc_fld_get(desc, DESC_FLD_EXT_L3_OFF);
	ud->int_l3_off    = __desc_fld_get(desc, DESC_FLD_INT_L3_OFF);
	ud->ext_l4_off    = __desc_fld_get(desc, DESC_FLD_EXT_L4_OFF);
	ud->int_l4_off    = __desc_fld_get(desc, DESC_FLD_INT_L4_OFF);
	ud->ext_frag_off  = __desc_fld_get(desc, DESC_FLD_EXT_FRG_INFO_OFF);
	ud->int_frag_off  = __desc_fld_get(desc, DESC_FLD_INT_FRG_INFO_OFF);
	ud->l2_off        = __desc_fld_get(desc, DESC_FLD_L2_OFF) << 4;
	ud->error         = __desc_fld_get(desc, DESC_FLD_ERROR);
	ud->drop          = __desc_fld_get(desc, DESC_FLD_DROP_PKT) ? 1 : 0;
	ud->l3_off[5]     = __desc_fld_get(desc, DESC_FLD_L3_OFF_5);
	ud->l3_off[4]     = __desc_fld_get(desc, DESC_FLD_L3_OFF_4);
	ud->l3_off[3]     = __desc_fld_get(desc, DESC_FLD_L3_OFF_3);
	ud->l3_off[2]     = __desc_fld_get(desc, DESC_FLD_L3_OFF_2);
	ud->l3_off[1]     = __desc_fld_get(desc, DESC_FLD_L3_OFF_1);
	ud->l3_off[0]     = __desc_fld_get(desc, DESC_FLD_L3_OFF_0);
	ud->tunn_off_id   = __desc_fld_get(desc, DESC_FLD_TUNN_OFF_ID);
	ud->payld_off_id  = __desc_fld_get(desc, DESC_FLD_PAYLD_OFF_ID);
#endif

	return 0;
}

s32 pp_qos_desc_decode(struct pp_qos_desc *desc,
		       const struct pp_qos_hw_desc *qos_hw_desc)
{
	const struct pp_hw_desc *hw_desc =
		(const struct pp_hw_desc *)qos_hw_desc;

	if (unlikely(!(!ptr_is_null(desc) &&
		       !ptr_is_null(hw_desc))))
		return -EINVAL;

	desc->ps         = (u64)__desc_fld_get(hw_desc, DESC_FLD_PS_B) << 32 |
			   (u32)__desc_fld_get(hw_desc, DESC_FLD_PS_A);
	desc->data_off   = __desc_fld_get(hw_desc, DESC_FLD_DATA_OFF);
	desc->src_pool   = __desc_fld_get(hw_desc, DESC_FLD_SRC_POOL);
	desc->buff_ptr   = (u64)__desc_fld_get(hw_desc, DESC_FLD_BUFF_PTR) << 7;
	desc->tx_port    = __desc_fld_get(hw_desc, DESC_FLD_TX_PORT);
	desc->bm_policy  = __desc_fld_get(hw_desc, DESC_FLD_BM_POLICY);
	desc->pkt_len    = __desc_fld_get(hw_desc, DESC_FLD_PKT_LEN);
	desc->lsp_pkt    = __desc_fld_get(hw_desc, DESC_FLD_LSP_PKT);
	desc->pmac       = __desc_fld_get(hw_desc, DESC_FLD_PMAC);
	desc->color      = __desc_fld_get(hw_desc, DESC_FLD_COLOR);
	desc->pre_l2_sz  = __desc_fld_get(hw_desc, DESC_FLD_PRE_L2) << 4;
	desc->ts         = __desc_fld_get(hw_desc, DESC_FLD_TS);
	desc->own        = __desc_fld_get(hw_desc, DESC_FLD_OWN);

	return 0;
}

s32 pp_desc_top_decode(struct pp_desc *desc, const struct pp_hw_desc *hw_desc)
{
	if (unlikely(!(!ptr_is_null(desc) &&
		       !ptr_is_null(hw_desc))))
		return -EINVAL;

	desc->ps         = (u64)__desc_fld_get(hw_desc, DESC_FLD_PS_B) << 32 |
			   (u32)__desc_fld_get(hw_desc, DESC_FLD_PS_A);
	desc->data_off   = __desc_fld_get(hw_desc, DESC_FLD_DATA_OFF);
	desc->src_pool   = __desc_fld_get(hw_desc, DESC_FLD_SRC_POOL);
	desc->buff_ptr   = (u64)__desc_fld_get(hw_desc, DESC_FLD_BUFF_PTR) << 7;
	desc->tx_port    = __desc_fld_get(hw_desc, DESC_FLD_TX_PORT);
	desc->bm_policy  = __desc_fld_get(hw_desc, DESC_FLD_BM_POLICY);
	desc->pkt_len    = __desc_fld_get(hw_desc, DESC_FLD_PKT_LEN);
	desc->lsp_pkt    = __desc_fld_get(hw_desc, DESC_FLD_LSP_PKT);
	desc->prv_ps   = (u64)__desc_fld_get(hw_desc, DESC_FLD_PRV_PS_B) << 32 |
			 (u32)__desc_fld_get(hw_desc, DESC_FLD_PRV_PS_A);
#ifdef PP_FULL_DESC_DBG
	desc->pmac       = __desc_fld_get(hw_desc, DESC_FLD_PMAC);
	desc->color      = __desc_fld_get(hw_desc, DESC_FLD_COLOR);
	desc->pre_l2_sz  = __desc_fld_get(hw_desc, DESC_FLD_PRE_L2) << 4;
	desc->ts         = __desc_fld_get(hw_desc, DESC_FLD_TS);
	desc->own        = __desc_fld_get(hw_desc, DESC_FLD_OWN);
#endif

	return 0;
}

s32 pp_desc_decode(struct pp_desc *desc, const struct pp_hw_desc *hw_desc)
{
	if (unlikely(!(!ptr_is_null(desc) &&
		       !ptr_is_null(hw_desc))))
		return -EINVAL;

	pp_desc_top_decode(desc, hw_desc);
	return __desc_ud_decode(&desc->ud, hw_desc);
}

s32 pp_qos_desc_dump(struct pp_qos_desc *desc)
{
	if (unlikely(ptr_is_null(desc)))
		return -EINVAL;

	pr_info("\n");
	pr_info("DESCRIPTOR:\n");
	pr_info(" tx port           : %hhu\n", (u8)desc->tx_port);
	pr_info(" packet len        : %hu\n", (u16)desc->pkt_len);
	pr_info(" buff addr         : %#llx\n", (u64)desc->buff_ptr);
	pr_info(" data offset       : %hu\n", (u16)desc->data_off);
	pr_info(" PS                : %#llx\n", (u64)desc->ps);
	pr_info(" src pool          : %llu\n", (u64)desc->src_pool);
	pr_info(" bm policy         : %hhu\n", (u8)desc->bm_policy);
	pr_info(" color             : %s\n", PP_COLOR_TO_STR(desc->color));
	pr_info(" pmac              : %s\n", BOOL2STR(desc->pmac));
	pr_info(" pre L2 size       : %llu\n", (u64)desc->pre_l2_sz);
	pr_info(" ts                : %s\n", BOOL2STR(desc->ts));
	pr_info(" ownership         : %s\n", BOOL2STR(desc->own));

	return 0;
}

s32 pp_desc_top_dump(struct pp_desc *desc)
{
	if (unlikely(ptr_is_null(desc)))
		return -EINVAL;

	pr_info("\n");
	pr_info("DESCRIPTOR:\n");
	pr_info(" tx port           : %hhu\n", (u8)desc->tx_port);
	pr_info(" packet len        : %hu\n", (u16)desc->pkt_len);
	pr_info(" buff addr         : %#llx\n", (u64)desc->buff_ptr);
	pr_info(" data offset       : %hu\n", (u16)desc->data_off);
	pr_info(" PS                : %#llx\n", (u64)desc->ps);
	pr_info(" src pool          : %llu\n", (u64)desc->src_pool);
	pr_info(" bm policy         : %hhu\n", (u8)desc->bm_policy);
	pr_info(" last slow path pkt: %s\n", BOOL2STR(desc->lsp_pkt));
	pr_info(" prev PS           : %#llx\n", (u64)desc->prv_ps);
#ifdef PP_FULL_DESC_DBG
	pr_info(" color             : %s\n", PP_COLOR_TO_STR(desc->color));
	pr_info(" pmac              : %s\n", BOOL2STR(desc->pmac));
	pr_info(" pre L2 size       : %llu\n", (u64)desc->pre_l2_sz);
	pr_info(" ts                : %s\n", BOOL2STR(desc->ts));
	pr_info(" ownership         : %s\n", BOOL2STR(desc->own));
#endif
	pr_info("\n");

	return 0;
}

s32 pp_desc_dump(struct pp_desc *desc)
{
	struct pp_pkt_ud *ud;
#ifdef PP_FULL_DESC_DBG
	u32 i;
#endif

	if (unlikely(ptr_is_null(desc)))
		return -EINVAL;

	ud = &desc->ud;
	pp_desc_top_dump(desc);

	pr_info("PACKET UD:\n");
	pr_info(" rx port           : %hhu\n", (u8)ud->rx_port);
	pr_info(" session id        : %u\n", (u32)ud->sess_id);
	pr_info(" exception session : %s\n", BOOL2STR(ud->is_exc_sess));
	pr_info(" SIG hash          : %#x\n", (u32)ud->hash_sig);
	pr_info(" H1 hash           : %#x\n", (u32)ud->hash_h1);
	pr_info(" H2 hash           : %#x\n", (u32)ud->hash_h2);
	pr_info(" data offset       : %llu\n", (u64)desc->data_off);
	pr_info(" tdox flow         : %hu\n", (u16)ud->tdox_flow);
	pr_info(" lro               : %s\n", BOOL2STR(ud->lro));
#ifdef PP_FULL_DESC_DBG
	pr_info(" error             : %s\n", BOOL2STR(ud->error));
	pr_info(" drop              : %s\n", BOOL2STR(ud->drop));
	pr_info(" L2 offset         : %hhu\n", (u8)ud->l2_off);
	pr_info(" ext L3 offset     : %hhu\n", (u8)ud->ext_l3_off);
	pr_info(" int L3 offset     : %hhu\n", (u8)ud->int_l3_off);
	pr_info(" ext L4 offset     : %hhu\n", (u8)ud->ext_l4_off);
	pr_info(" int L4 offset     : %hhu\n", (u8)ud->int_l4_off);
	pr_info(" ext frag offset   : %hhu\n", (u8)ud->ext_frag_off);
	pr_info(" int frag offset   : %hhu\n", (u8)ud->int_frag_off);
	for (i = 0; i < PP_DESC_NUM_OF_L3_OFF; i++)
		pr_info(" L3 offset %u       : %hhu\n", i, ud->l3_off[i]);
	pr_info(" tunnel offset id  : %hhu\n", (u8)ud->tunn_off_id);
	pr_info(" payload offset id : %hhu\n", (u8)ud->payld_off_id);
	pr_info(" ttl expired       : %s\n", BOOL2STR(ud->ttl_exp));
	pr_info(" ip option         : %s\n", BOOL2STR(ud->ip_opt));
	pr_info(" ext DF            : %s\n", BOOL2STR(ud->ext_df));
	pr_info(" int DF            : %s\n", BOOL2STR(ud->int_df));
	pr_info(" ext frag type     : %hhu\n", (u8)ud->ext_frag_type);
	pr_info(" int frag type     : %hhu\n", (u8)ud->int_frag_type);
	pr_info(" int proto info    : %s\n",
		PP_FV_PROTOCOL_STR(ud->int_proto_info));
	pr_info(" ext proto info    : %s\n",
		PP_FV_PROTOCOL_STR(ud->ext_proto_info));
	pr_info(" tcp fin           : %s\n", BOOL2STR(ud->tcp_fin));
	pr_info(" tcp syn           : %s\n", BOOL2STR(ud->tcp_syn));
	pr_info(" tcp rst           : %s\n", BOOL2STR(ud->tcp_rst));
	pr_info(" tcp ack           : %s\n", BOOL2STR(ud->tcp_ack));
	pr_info(" tcp data offset   : %hhu\n", (u8)ud->tcp_data_off);
#endif
	pr_info("\n");

	return 0;
}

void pp_desc_init(void)
{
	/* Init all descriptor static fields that doesn't require any
	 * validity check or any special conversions
	 */
	BUF_FLD_INIT_SIMPLE(desc_flds, DESC_FLD_PS_A);
	BUF_FLD_INIT_SIMPLE(desc_flds, DESC_FLD_PS_B);
	BUF_FLD_INIT_SIMPLE(desc_flds, DESC_FLD_DATA_OFF);
	BUF_FLD_INIT_SIMPLE(desc_flds, DESC_FLD_PMAC);
	BUF_FLD_INIT_SIMPLE(desc_flds, DESC_FLD_COLOR);
	BUF_FLD_INIT_SIMPLE(desc_flds, DESC_FLD_SRC_POOL);
	BUF_FLD_INIT_SIMPLE(desc_flds, DESC_FLD_BUFF_PTR);
	BUF_FLD_INIT_SIMPLE(desc_flds, DESC_FLD_PRE_L2);
	BUF_FLD_INIT_SIMPLE(desc_flds, DESC_FLD_TS);
	BUF_FLD_INIT_SIMPLE(desc_flds, DESC_FLD_TX_PORT);
	BUF_FLD_INIT_SIMPLE(desc_flds, DESC_FLD_BM_POLICY);
	BUF_FLD_INIT_SIMPLE(desc_flds, DESC_FLD_PKT_LEN);
	BUF_FLD_INIT_SIMPLE(desc_flds, DESC_FLD_LSP_PKT);
	BUF_FLD_INIT_SIMPLE(desc_flds, DESC_FLD_OWN);
	BUF_FLD_INIT_SIMPLE(desc_flds, DESC_FLD_INT_PROTO_INFO);
	BUF_FLD_INIT_SIMPLE(desc_flds, DESC_FLD_EXT_PROTO_INFO);
	BUF_FLD_INIT_SIMPLE(desc_flds, DESC_FLD_RX_PORT);
	BUF_FLD_INIT_SIMPLE(desc_flds, DESC_FLD_TTL_EXP);
	BUF_FLD_INIT_SIMPLE(desc_flds, DESC_FLD_IP_OPT);
	BUF_FLD_INIT_SIMPLE(desc_flds, DESC_FLD_EXT_DF);
	BUF_FLD_INIT_SIMPLE(desc_flds, DESC_FLD_INT_DF);
	BUF_FLD_INIT_SIMPLE(desc_flds, DESC_FLD_EXT_FRAG_TYPE);
	BUF_FLD_INIT_SIMPLE(desc_flds, DESC_FLD_INT_FRAG_TYPE);
	BUF_FLD_INIT_SIMPLE(desc_flds, DESC_FLD_TCP_FIN);
	BUF_FLD_INIT_SIMPLE(desc_flds, DESC_FLD_TCP_SYN);
	BUF_FLD_INIT_SIMPLE(desc_flds, DESC_FLD_TCP_RST);
	BUF_FLD_INIT_SIMPLE(desc_flds, DESC_FLD_TCP_ACK);
	BUF_FLD_INIT_SIMPLE(desc_flds, DESC_FLD_TCP_DATA_OFF);
	BUF_FLD_INIT_SIMPLE(desc_flds, DESC_FLD_EXT_L3_OFF);
	BUF_FLD_INIT_SIMPLE(desc_flds, DESC_FLD_INT_L3_OFF);
	BUF_FLD_INIT_SIMPLE(desc_flds, DESC_FLD_EXT_L4_OFF);
	BUF_FLD_INIT_SIMPLE(desc_flds, DESC_FLD_INT_L4_OFF);
	BUF_FLD_INIT_SIMPLE(desc_flds, DESC_FLD_EXT_FRG_INFO_OFF);
	BUF_FLD_INIT_SIMPLE(desc_flds, DESC_FLD_INT_FRG_INFO_OFF);
	BUF_FLD_INIT_SIMPLE(desc_flds, DESC_FLD_TDOX_FLOW);
	BUF_FLD_INIT_SIMPLE(desc_flds, DESC_FLD_LRO);
	BUF_FLD_INIT_SIMPLE(desc_flds, DESC_FLD_L2_OFF);
	BUF_FLD_INIT_SIMPLE(desc_flds, DESC_FLD_SESSION_ID);
	BUF_FLD_INIT_SIMPLE(desc_flds, DESC_FLD_IS_ALT_SESSION);
	BUF_FLD_INIT_SIMPLE(desc_flds, DESC_FLD_ERROR);
	BUF_FLD_INIT_SIMPLE(desc_flds, DESC_FLD_DROP_PKT);
	BUF_FLD_INIT_SIMPLE(desc_flds, DESC_FLD_HASH_SIG);
	BUF_FLD_INIT_SIMPLE(desc_flds, DESC_FLD_H1);
	BUF_FLD_INIT_SIMPLE(desc_flds, DESC_FLD_H2_L);
	BUF_FLD_INIT_SIMPLE(desc_flds, DESC_FLD_H2_H);
	BUF_FLD_INIT_SIMPLE(desc_flds, DESC_FLD_L3_OFF_5);
	BUF_FLD_INIT_SIMPLE(desc_flds, DESC_FLD_L3_OFF_4);
	BUF_FLD_INIT_SIMPLE(desc_flds, DESC_FLD_L3_OFF_3);
	BUF_FLD_INIT_SIMPLE(desc_flds, DESC_FLD_L3_OFF_2);
	BUF_FLD_INIT_SIMPLE(desc_flds, DESC_FLD_L3_OFF_1);
	BUF_FLD_INIT_SIMPLE(desc_flds, DESC_FLD_L3_OFF_0);
	BUF_FLD_INIT_SIMPLE(desc_flds, DESC_FLD_TUNN_OFF_ID);
	BUF_FLD_INIT_SIMPLE(desc_flds, DESC_FLD_PAYLD_OFF_ID);
	BUF_FLD_INIT_SIMPLE(desc_flds, DESC_FLD_PRV_PS_A);
	BUF_FLD_INIT_SIMPLE(desc_flds, DESC_FLD_PRV_PS_B);
}
