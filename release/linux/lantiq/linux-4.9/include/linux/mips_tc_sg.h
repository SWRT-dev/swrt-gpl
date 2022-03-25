/*
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 *
 * Copyright (C) 2017 Wu ZhiXian<zhixian.wu@intel.com>
 * Copyright (C) 2017 Intel Corporation.
 * MIPS TC FW Scatter-Gathering
 */
#ifndef __MIPS_TC_SG__
#define __MIPS_TC_SG__

#define MIPS_TC_DMA0_CS_REG 0xb6e00018
#define MIPS_TC_DMA_CDPTNRD_REG 0xb6e00034

#ifdef CONFIG_LTQ_UMT_518_FW_SG
#define MIPS_TC_OPTIMIZE 1

#ifdef MIPS_TC_OPTIMIZE
struct mips_tc_q_cfg_ctxt {
		u32 des_in_own_val;
		u32 _res0;
		u32 des_num;

		u32 des_base_addr;

		u32 rd_idx;
		u32 wr_idx;

		u32 _dw_res0;

		u32 rd_frag_cnt;

		u32 _dw_res1;

		u32 wr_frag_cnt;

		u32 _dw_res2;

		u32 rd_pkt_cnt;

		u32 rd_byte_cnt;

		u32 wr_pkt_cnt;

		u32 wr_byte_cnt;

};

struct mips_tc_fw_memcpy_ctxt_t {
		u32 _res0;
		u32 sop;
		u32 eop;
		u32 byte_off;
		u32 src_data_ptr;
		u32 dst_data_ptr;
		u32 data_len;

};

struct mips_tc_fw_pp_ctxt_t {
		u32 split_error;
		u32 status;
		u32 rx_in_hd_accum;
		u32 _res0;
		u32 sop;
		u32 eop;
		u32 byte_off;
		u32 _res1;
		u32 src_data_ptr;
		u32 dst_data_ptr;
		u32 data_len;
		u32 pkt_data_len;
		u32 prev_dma_tx_ptr;
		u32 prev_dma_rx_ptr;
		u32 _res2;
		u32 _res3;

};

struct mips_tc_metadata {
		u32 sop_frag;
		u32 eop_frag;
		u32 _res0;
		u32 _res1;
		u32 byte_off;
		u32 data_len;
		u32 src_data_ptr;
		u32 dst_data_ptr;

};

struct mips_tc_rxout_dst_cache_ctxt {
		u32 data_ptr;
};

#else /* MIPS_TC_OPTIMIZE */
/* Define DMA descriptors */
struct mips_tc_q_cfg_ctxt {
		u32 des_in_own_val:1;
		u32 _res0:15;
		u32 des_num:16;

		u32 des_base_addr;

		u32 rd_idx:16;
		u32 wr_idx:16;

		u32 _dw_res0;

		u32 rd_frag_cnt;

		u32 _dw_res1;

		u32 wr_frag_cnt;

		u32 _dw_res2;

		u32 rd_pkt_cnt;

		u32 rd_byte_cnt;

		u32 wr_pkt_cnt;

		u32 wr_byte_cnt;

};

struct mips_tc_fw_memcpy_ctxt_t {
		u32 _res0:30;
		u32 sop:1;
		u32 eop:1;
		u32 byte_off;
		u32 src_data_ptr;
		u32 dst_data_ptr;
		u32 data_len;

};

struct mips_tc_fw_pp_ctxt_t {
		u32 split_error:12;
		u32 status:4;
		u32 rx_in_hd_accum:12;
		u32 _res0:2;
		u32 sop:1;
		u32 eop:1;
		u32 byte_off;
		u32 src_data_ptr;
		u32 dst_data_ptr;
		u32 data_len;
		u32 pkt_data_len;

};

struct mips_tc_metadata {
		u32 sop_frag:1;
		u32 eop_frag:1;
		u32 _res0:14;
		u32 data_len:16;

};

struct mips_tc_rxout_dst_cache_ctxt {
		u32 data_ptr;
};

#endif /* MIPS_TC_OPTIMIZE */

struct mips_tc_tx_descriptor {
		u32 own:1;
		u32 c:1;
		u32 sop:1;
		u32 eop:1;
		u32 dic:1;
		u32 pdu_type:1;
		u32 byte_off:3;
		u32 qos:4;
		u32 mpoa_pt:1;
		//u32 mpoa_type:2;
		//u32 sop_frag:1;
		//u32 eop_frag:1;

		u32 sop_frag:1;
		u32 eop_frag:1;
		u32 data_len:16;
		u32 data_ptr:32;
};

struct mips_tc_rx_descriptor {
		u32 own:1;
		u32 c:1;
		u32 sop:1;
		u32 eop:1;
		u32 dic:1;
		u32 pdu_type:1;
		u32 byte_off:3;
		u32 qos:4;
		u32 mpoa_pt:1;
		//u32 mpoa_type:2;
		u32 sop_frag:1;
		u32 eop_frag:1;
		u32 data_len:16;

		u32 data_ptr:32;

};

/* Section 1: Define MIPS TC control & debug structure */
#define MIPS_TC_DEBUG_MSG_LEN 256
#define MIPS_TC_DEBUG_MSG_ARRAY_LEN 4
struct mips_tc_msg_param {
	u32 flags;
	char message[MIPS_TC_DEBUG_MSG_LEN];
};

enum {
	MIPS_TC_C_STOP	= 0x0,
	MIPS_TC_C_RUN	= 0x1,

};

struct mips_tc_ctrl_dbg {
	u32 mips_tc_control;
	u32 dbg_flags;
	u32 ver_major:8;
	u32 ver_mid:8;
	u32 ver_minor:8;
	u32 ver_reserved:8;
	u32 priv_data_vir;
	u32 priv_data_phy;
	u32 priv_data_len;
	u32 ctrl_max_process_pkt;
	u32 reserved[16];
	u32 msg_cur_idx;
	struct mips_tc_msg_param msgs[MIPS_TC_DEBUG_MSG_ARRAY_LEN];
};

struct mips_tc_tx_des_soc {
		u32 dword0;

		u32 dword1;

		struct mips_tc_tx_descriptor dword2_3;

};

struct mips_tc_rx_des_soc {
		u32 dword0;

		u32 dword1;

		struct mips_tc_rx_descriptor dword2_3;

};

/* Section 2: Memory configuration */
struct mips_tc_mem_conf {
	/* Sec1: memory configure context */
	struct mips_tc_q_cfg_ctxt soc_rx_out_src_ctxt;
	struct mips_tc_q_cfg_ctxt soc_sg_dma_tx_ctxt;
	struct mips_tc_q_cfg_ctxt soc_sg_dma_rx_ctxt;
	struct mips_tc_q_cfg_ctxt soc_rx_out_dst_ctxt;
	/* Sec2: Current context */
	struct mips_tc_fw_memcpy_ctxt_t fw_memcpy_ctxt;
	struct mips_tc_fw_pp_ctxt_t fw_pp_ctxt;
	/* Sec3: control variable */
	u32 aca_hw_rxin_hd_acc_addr;
	u32 tc_mode_bonding;
	u32 reserved[16];
	struct mips_tc_rxout_dst_cache_ctxt *cache_rxout_ptr;
};

/* Section 3: MIPS TC Info */
enum {
	MIPS_TC_S_ERROR	= -1,
	MIPS_TC_S_IDLE	= 0x0,
	MIPS_TC_S_RUNNING = 0x1,
	MIPS_TC_S_BUSY = 0x2,

};

struct mips_tc_mib {
	u32 jobs;
	u32 reserved[32];
};

struct mips_tc_info {
	u32 state;
	struct mips_tc_mib mib;
};

/* Define shared memory: note that new field
 * should be added to the tail of the structure.
 * The layout:
 *   - Control and debug structure ( 518 <-> SoC): mips_tc_ctrl_dbg
 *   - Memory configuration(518 -> SoC): mips_tc_mem_conf
 *		+ Memory configure context.
 *	- MIPS TC information (SoC -> 518): mips_tc_info
 *		+ State machine, mib
 *	- Others
 */
struct mips_tc_sg_mem {
	struct mips_tc_ctrl_dbg ctrl;
	struct mips_tc_mem_conf conf;
	struct mips_tc_info mib;
};

extern int mips_tc_get_shared_mem(struct mips_tc_sg_mem **shared_mem);
#endif /* CONFIG_LTQ_UMT_518_FW_SG */

#define SUPPORT_DSL_BONDING_FEATURE 1
#ifdef SUPPORT_DSL_BONDING_FEATURE
#define RUN_MIPS_TC 1
/* #define USE_CACHED_MECHANISM 1 */
#define rx_out_des_size		16
extern void mips_tc_init(u32 *);
extern int mips_tc_sg(struct mips_tc_sg_mem *shared_vir_mem);

#if USE_CACHED_MECHANISM
int mips_tc_cache_flush(u32 start_addr, u32 size);
int mips_tc_cache_invalidate(u32 start_addr, u32 size);
#endif /* USE_CACHED_MECHANISM */

#endif
#endif /* __MIPS_TC_SG__ */
