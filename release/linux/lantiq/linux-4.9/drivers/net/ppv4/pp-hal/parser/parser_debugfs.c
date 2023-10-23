/*
 * Description: PP Infrastructure Driver Debug FS Interface
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018-2020 Intel Corporation
 */

#define pr_fmt(fmt) "[PP_PARSER_DBG]: %s:%d: " fmt, __func__, __LINE__

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/bitops.h>
#include <linux/debugfs.h>
#include <linux/parser.h>
#include <linux/string.h>

#include "pp_debugfs_common.h"
#include "parser.h"
#include "pp_fv.h"
#include "classifier.h"
#include "parser_internal.h"

/* Infra Debug FS directories */
static struct dentry *dbgfs;
static struct dentry *protos_dir;
static struct dentry *tables_dir;
static struct dentry *protos_dbgfs[PRSR_PROTOS_NUM];
static struct buf_fld_info prsr_stw_flds[STW_PRSR_FLDS_NUM];

const char * const l2_cmp_op_str[L2_CMP_OP_NUM] = {
	"EQ",
	"LT",
};

const char * const np_logic_str[NP_LOGICS_NUM] = {
	"TUNNEL",
	"IPV4",
	"IPV6",
	"IP_SEL",
	"IPV4_2",
	"IPV6_2",
	"UDP",
	"END",
};

const char * const etype_str[ETYPE_NUM] = {
	"NONE",
	"IPV4",
	"IPV6",
	"OTHER_L3",
	"UDP",
	"TCP",
	"RSRV_L4",
	"OTHER_L4",
	"L2",
	"PAYLOAD",
	"IPV6_FR",
	"IPV6_EXT",
};

const char * const skip_logic_str[SKIP_LOGICS_NUM] = {
	"ALU",
	"LOOKUP",
	"IM1",
};

/**
 * @brief Print a pit info into a sequential file
 * @param f the file
 * @param pit the info
 * @param align alignment
 */
static inline void __pit_seq_printf(struct seq_file *f,
				    const struct pit_info *pit,
				    u8 align)
{
	switch (pit->cnt) {
	case 0:
		seq_printf(f, " | %-*s", align, "none");
		break;
	case 1:
		seq_printf(f, " | %-*u", align, pit->base);
		break;
	default:
		align -= 1;
		align /= 2;
		seq_printf(f, " | %0*u-%0*u", align, pit->base,
			   align, pit->base + pit->cnt - 1);
		break;
	}
}

const char * const err_flag_str[PRSR_NUM_ERR_FLAGS] = {
	"Abort on Payload Timeout",
	"Abort on L3 Processing Timeout",
	"Abort on IP Len Error",
	"Abort on IMEM protocol Overflow",
	"Abort on L3 hdr len longer than cfg",
	"Abort on Unknown Last Ethertype",
	NULL,
	NULL,
	NULL,
	"Error on Payload Timeout",
	"Error on L3 Processing Timeout",
	"Error on IP Len Error",
	"Error on IMEM protocol Overflow",
	"Error on L3 hdr len longer than cfg",
	"Error on L3 hdr len exceeds min-max",
	"Error on Unknown Last Ethertype",
	"Error on RPB Zero Len",
	"Error on RPB Error",
	"Error on RPB Discard",
	"Drop on Payload Timeout",
	"Drop on L3 Processing Timeout",
	"Drop on IP Len Error",
	"Drop on IMEM protocol Overflow",
	"Drop on L3 hdr len longer than cfg",
	"Drop on L3 hdr len exceeds min-max",
	"Drop on Unknown Last Ethertype",
	"Drop on RPB Zero Len",
	"Drop on RPB Error",
	"Drop on RPB Discard",
	NULL,
	NULL,
	"FW Fallback on Error",
};

/**
 * @brief Dump status word
 */
void __prsr_stw_show(struct seq_file *f)
{
	struct buf_fld_info *fld;
	struct prsr_hw_stw hw_stw;
	u32 fld_val[STW_PRSR_FLDS_NUM];
	u32 i;

	if (unlikely(prsr_last_stw_get(&hw_stw)))
		return;

	seq_puts(f, " Parser Status Word:\n");
	seq_puts(f, " =======================\n");
	for (i = 0 ; i < STW_PRSR_FLDS_NUM ; i++) {
		fld = &prsr_stw_flds[i];
		fld_val[i] = buf_fld_get(fld, hw_stw.word);

		if (i == STW_PRSR_FLD_DROP_PKT)
			seq_printf(f, " %-35s: %s (%u)\n", fld->desc,
				   STW_DROP_PKT_STR(fld_val[i]), fld_val[i]);

		else if (fld->msb == fld->lsb)
			seq_printf(f, " %-35s: %s\n", fld->desc,
				   BOOL2STR(fld_val[i]));
		else
			seq_printf(f, " %-35s: %u [%#x]\n", fld->desc,
				   fld_val[i], fld_val[i]);
	}
}

PP_DEFINE_DEBUGFS(stw_show, __prsr_stw_show, NULL);

/**
 * @brief Reset stw stickiness
 */
void __prsr_stw_sticky_reset(struct seq_file *f)
{
	prsr_stw_sticky_reset();
}

PP_DEFINE_DEBUGFS(stw_sticky_reset, __prsr_stw_sticky_reset, NULL);

/**
 * @brief Dump stats
 */
void __prsr_stats_show(struct seq_file *f)
{
	struct prsr_stats st;
	s32 ret;

	ret = prsr_stats_get(&st);
	if (unlikely(ret))
		return;

	seq_puts(f, "\n");
	seq_puts(f,   " +----------------------------------------+\n");
	seq_puts(f,   " |       Parser Stats                     |\n");
	seq_puts(f,   " +----------------------------------------+\n");
	seq_printf(f, " | %-25s | %10u |\n", "RX Packets from RPB", st.rx_pkts);
	seq_puts(f,   " +---------------------------+------------+\n");
	seq_printf(f, " | %-25s | %10u |\n", "Valid RX Packets from RPB",
		   st.rx_valid_pkts);
	seq_puts(f,   " +---------------------------+------------+\n");
	seq_printf(f, " | %-25s | %10u |\n", "TX Packets", st.tx_pkts);
	seq_puts(f,   " +---------------------------+------------+\n");
	seq_printf(f, " | %-25s | %10u |\n", "TX to Classifier",
		   st.tx_pkts_to_cls);
	seq_puts(f,   " +---------------------------+------------+\n");
	seq_printf(f, " | %-25s | %10u |\n", "TX to UC", st.tx_pkts_to_uc);
	seq_puts(f,   " +---------------------------+------------+\n");
	seq_printf(f, " | %-25s | %10u |\n", "Discarded Prior to Parser",
		   st.rpb_discard);
	seq_puts(f,   " +---------------------------+------------+\n");
	seq_printf(f, " | %-25s | %10u |\n", "Discarded by RPB",
		   st.pprs_discard);
	seq_puts(f,   " +---------------------------+------------+\n");
	seq_printf(f, " | %-25s | %10u |\n", "Discarded by UC",
		   st.uc_discard);
	seq_puts(f,   " +---------------------------+------------+\n");
	seq_printf(f, " | %-25s | %10u |\n", "RPB Error", st.rpb_err);
	seq_puts(f,   " +---------------------------+------------+\n");
	seq_printf(f, " | %-25s | %10u |\n", "Zero Length Packets",
		   st.zero_len);
	seq_puts(f,   " +---------------------------+------------+\n");
	seq_printf(f, " | %-25s | %10u |\n", "No Last ETH Type",
		   st.last_eth_err);
	seq_puts(f,   " +---------------------------+------------+\n");
	seq_printf(f, " | %-25s | %10u |\n", "L3 Hdr Exceeds Min-Max",
		   st.proto_len_err);
	seq_puts(f,   " +---------------------------+------------+\n");
	seq_printf(f, " | %-25s | %10u |\n", "L3 Hdr Longer Than Cfg",
		   st.hdr_len_err);
	seq_puts(f,   " +---------------------------+------------+\n");
	seq_printf(f, " | %-25s | %10u |\n", "Protocol Overflow",
		   st.proto_overflow);
	seq_puts(f,   " +---------------------------+------------+\n");
	seq_printf(f, " | %-25s | %10u |\n", "IP Length Error",
		   st.ip_len_err);
	seq_puts(f,   " +---------------------------+------------+\n");
	seq_printf(f, " | %-25s | %10u |\n", "Payload Error",
		   st.payload_err);
	seq_puts(f,   " +---------------------------+------------+\n");
	seq_printf(f, " | %-25s | %10u |\n", "Aborted Packets",
		   st.aborted_pkts);
	seq_puts(f,   " +----------------------------------------+\n");
	seq_puts(f, "\n");
	seq_puts(f, " Legend:\n");
	seq_puts(f, " -------\n");
	seq_puts(f, " rpb discard           - packets marked by the rpb with discard flag\n");
	seq_puts(f, " rpb error             - packets marked by the rpb with error flag\n");
	seq_puts(f, " no last eth           - packets that no last ethernet type was found\n");
	seq_puts(f, " protocol length error - packets with layer 3 too big or too small than max or min\n");
	seq_puts(f, " protocol header error - packets with layer 3 header too big or too small than imem configuration\n");
	seq_puts(f, " protocol overflow     - packets with too much imem protocols found\n");
	seq_puts(f, " ip len error          - packets with mismatch between the IP header packets length to rpb packet length\n");
	seq_puts(f, " payload error         - packets with no payload\n");
	seq_puts(f, "\n");
}

PP_DEFINE_DEBUGFS(stats_show, __prsr_stats_show, NULL);

enum global_opts {
	global_opt_help,
	global_opt_dflt_proto,
	global_opt_fv_swap,
	global_opt_fv_block_swap,
	global_opt_fv_mask_en,
	global_opt_fv_mask_swap,
	global_opt_max_hdr_len,
	global_opt_max_imem_hdr_len,
	global_opt_prefetch_hdr_len,
	global_opt_ipv6_nexthdr,
	global_opt_err_flag,
	global_opt_err_flags,
	global_opt_rst_err_flags,
};

static const match_table_t global_tokens = {
	{global_opt_help,             "help"},
	{global_opt_dflt_proto,       "dflt=%u"},
	{global_opt_fv_swap,          "fv_swap"},
	{global_opt_fv_block_swap,    "fv_bswap"},
	{global_opt_fv_mask_en,       "fv_mask"},
	{global_opt_fv_mask_swap,     "fv_mswap"},
	{global_opt_max_hdr_len,      "max_hdr_len=%u"},
	{global_opt_max_imem_hdr_len, "max_imem=%u"},
	{global_opt_prefetch_hdr_len, "pre_len=%u"},
	{global_opt_ipv6_nexthdr,     "nexthdr=%u"},
	{global_opt_err_flag,         "err_flag=%u"},
	{global_opt_err_flags,        "err_flags=%u"},
	{global_opt_rst_err_flags,    "rst_err_flags"},
};

/**
 * @brief Print parser global configuration help, how to change global
 *        configuration settings
 */
static void __prsr_global_cfg_help(void)
{
	pr_info("\n");
	pr_info(" Usage: echo <option>[=value] > global\n");
	pr_info(" Options:\n");
	pr_info("   help        - print this help function\n");
	pr_info("   dflt        - set default protocol when last ethertype is unknown\n");
	pr_info("   fv_swap     - toggle field vector output swapping\n");
	pr_info("   fv_bswap    - toggle field vector blocks output swapping\n");
	pr_info("   fv_mask     - toggle field vector masking\n");
	pr_info("   fv_mswap    - toggle field vector mask swapping\n");
	pr_info("   max_hdr_len - set max parsing header length\n");
	pr_info("   max_imem    - set max imem protocol header length\n");
	pr_info("   pre_len     - set prefetched header length\n");
	pr_info("   nexthdr     - toggle ipv6 next header support\n");
	pr_info("   err_flag    - toggle hw error flag\n");
	pr_info("   err_flags   - set hw error flags\n");
	pr_info(" Examples:\n");
	pr_info("   toggle 'drop on unknown last ethertype' error flag:\n"
		"     echo err_flag=25 > global\n");
	pr_info("   toggle support for ipv6 frag header:\n"
		"     echo nexthdr=44 > global\n");
	pr_info("   change more than 1 setting:\n"
		"     echo dflt=35 pre_len=200 fv_swap=0 > global\n");
}

/**
 * @brief Print the parser global configuration
 */
void __prsr_global_cfg_show(struct seq_file *f)
{
	enum prsr_proto_id dflt_id;
	const struct proto_db_e *dflt_proto;
	bool fv_swap, fv_bswap, fv_mask_swap, fv_mask_en;
	unsigned long err_flags, ipv6_supp[BITS_TO_LONGS(NEXTHDR_MAX)];
	u32 n, prefetch_len, max_imem_hdr_len, max_hdr_len;
	char buf[128];
	s32 ret;

	/* get all info */
	ret = prsr_dflt_ethertype_get(&dflt_id);
	if (unlikely(ret))
		return;
	ret = prsr_ipv6_nexthdr_support_get_all(ipv6_supp, NEXTHDR_MAX);
	if (unlikely(ret))
		return;
	pr_buf(buf, sizeof(buf), n, "%*pbl", NEXTHDR_MAX, ipv6_supp);
	dflt_proto       = prsr_proto_info_get(dflt_id);
	fv_swap          = prsr_fv_swap_get();
	fv_bswap         = prsr_fv_blocks_swap_get();
	fv_mask_en       = prsr_fv_mask_en_get();
	fv_mask_swap     = prsr_fv_mask_swap_get();
	max_hdr_len      = prsr_max_hdr_len_get();
	max_imem_hdr_len = prsr_max_imem_proto_hdr_len_get();
	prefetch_len     = prsr_prefetch_hdr_len_get();
	err_flags        = (unsigned long)prsr_hw_err_flags_get();

	seq_puts(f, "\n");
	seq_puts(f, " +------------------------------------------+----------------------+\n");
	seq_puts(f, " | Config                                   | Value                |\n");
	seq_puts(f, " +------------------------------------------+----------------------+\n");
	seq_printf(f, " | %-40s | %-20s |\n", "Default Last Ethertype",
		   dflt_proto->name);
	seq_printf(f, " | %-40s | %-20s |\n", "Field Vector Swap",
		   BOOL2EN(fv_swap));
	seq_printf(f, " | %-40s | %-20s |\n", "Field Vector Blocks Swap",
		   BOOL2EN(fv_bswap));
	seq_printf(f, " | %-40s | %-20s |\n", "Field Vector Masking",
		   BOOL2EN(fv_mask_en));
	seq_printf(f, " | %-40s | %-20s |\n", "Field Vector Mask Swap",
		   BOOL2EN(fv_mask_swap));
	seq_printf(f, " | %-40s | %-20u |\n", "Max Header Length", max_hdr_len);
	seq_printf(f, " | %-40s | %-20u |\n", "Max IMEM Header Length",
		   max_imem_hdr_len);
	seq_printf(f, " | %-40s | %-20u |\n", "Prefetch Header Len",
		   prefetch_len);
	seq_printf(f, " | %-40s | %-20s |\n", "IPv6 Next Header Support", buf);
	for (n = 0; n < sizeof(u32) * BITS_PER_BYTE; n++) {
		if (!err_flag_str[n])
			continue;
		seq_printf(f, " | %-35s (%2u) | %-20s |\n", err_flag_str[n],
			   n, test_bit(n, &err_flags) ?
			   "enable" : "disable");
	}
	seq_puts(f, " +------------------------------------------+----------------------+\n");
	seq_puts(f, "\n");
}

/**
 * @brief Change parser global configuration setting using debugfs
 *        file
 */
void __prsr_global_cfg_set(char *cmd_buf, void *data)
{
	substring_t args[MAX_OPT_ARGS];
	enum global_opts opt;
	unsigned long err_flags;
	char *tok, *params;
	bool bval;
	int val;

	params = cmd_buf;
	/* iterate over user params */
	while ((tok = strsep(&params, " \t\n")) != NULL) {
		if (!*tok)
			continue;
		opt = match_token(tok, global_tokens, args);
		switch (opt) {
		case global_opt_help:
			__prsr_global_cfg_help();
			return;
		case global_opt_dflt_proto:
			if (match_int(&args[0], &val))
				return;
			prsr_dflt_ethertype_set((enum prsr_proto_id)val);
			break;
		case global_opt_fv_swap:
			/* toggle */
			prsr_fv_swap_set(!prsr_fv_swap_get());
			break;
		case global_opt_fv_block_swap:
			/* toggle */
			prsr_fv_blocks_swap_set(!prsr_fv_blocks_swap_get());
			break;
		case global_opt_fv_mask_en:
			/* toggle */
			prsr_fv_mask_en_set(!prsr_fv_mask_en_get());
			break;
		case global_opt_fv_mask_swap:
			/* toggle */
			prsr_fv_mask_swap_set(!prsr_fv_mask_swap_get());
			break;
		case global_opt_max_hdr_len:
			if (match_int(&args[0], &val))
				return;
			prsr_max_hdr_len_set((u32)val);
			break;
		case global_opt_max_imem_hdr_len:
			if (match_int(&args[0], &val))
				return;
			prsr_max_imem_proto_hdr_len_set((u32)val);
			break;
		case global_opt_prefetch_hdr_len:
			if (match_int(&args[0], &val))
				return;
			prsr_prefetch_hdr_len_set((u32)val);
			break;
		case global_opt_ipv6_nexthdr:
			if (match_int(&args[0], &val))
				return;
			/* toggle */
			if (!prsr_ipv6_nexthdr_support_get(val, &bval))
				prsr_ipv6_nexthdr_support_set(val, !bval);
			break;
		case global_opt_err_flag:
			if (match_int(&args[0], &val))
				return;
			/* toggle */
			err_flags = (unsigned long)prsr_hw_err_flags_get();
			if (!test_bit(val, &err_flags))
				err_flags = PP_FIELD_MOD(BIT(val),
							 1, err_flags);
			else
				err_flags = PP_FIELD_MOD(BIT(val),
							 0, err_flags);
			prsr_hw_err_flags_set((u32)err_flags, true);
			break;
		case global_opt_err_flags:
			if (match_hex(&args[0], &val))
				return;
			prsr_hw_err_flags_set((u32)val, true);
			break;
		case global_opt_rst_err_flags:
			/* reset hw error flags */
			prsr_hw_err_flags_init();
			break;
		default:
			break;
		}
	}
}

PP_DEFINE_DEBUGFS(prsr_global_cfg, __prsr_global_cfg_show,
		  __prsr_global_cfg_set);

/**
 * @brief Print all active protocols
 */
void __prsr_protocols_show(struct seq_file *f)
{
	const struct proto_db_e *proto;
	char buf[256];
	u32 n = 0;
	u16 i;

	seq_puts(f, "\n");
	seq_puts(f, " +-------------+---------------------------+----------+--------+----------+-------+-----------------+-----------------+-----------------+-----------------+\n");
	seq_puts(f, " | Internal Id | Name                      | Proto Id | L2 Idx | IMEM Idx | Pit   | Last Eth Entrs  | NP Sel Entries  | Skip Entries    | Merge Entries   |\n");
	seq_puts(f, " +-------------+---------------------------+----------+--------+----------+-------+-----------------+-----------------+-----------------+-----------------+\n");

	PRSR_FOR_EACH_PROTO(i) {
		proto = prsr_proto_info_get(i);
		if (unlikely(!proto))
			goto done;
		if (!proto->valid)
			continue;
		seq_printf(f, " | %-11u | %-25s | %#06x  ",
			   proto->id, proto->name, proto->proto_id);
		if (proto->l2_idx != TABLE_ENTRY_INVALID_IDX)
			seq_printf(f, " | %-6u", proto->l2_idx);
		else
			seq_printf(f, " | %-6s", "N/A");
		if (proto->imem_idx != TABLE_ENTRY_INVALID_IDX)
			seq_printf(f, " | %-8u", proto->imem_idx);
		else
			seq_printf(f, " | %-8s", "N/A");
		__pit_seq_printf(f, &proto->pit, 5);
		pr_buf(buf, sizeof(buf), n, "%*pbl",
		       PRSR_NUM_LAST_ETHTYPE_ENTRIES, proto->leth_map);
		seq_printf(f, " | %-15s", buf);
		pr_buf(buf, sizeof(buf), n, "%*pbl",
		       PRSR_NUM_NP_SELECT_ENTRIES, proto->np_map);
		seq_printf(f, " | %-15s", buf);
		pr_buf(buf, sizeof(buf), n, "%*pbl",
		       PRSR_NUM_SKIP_ENTRIES, proto->skip_map);
		seq_printf(f, " | %-15s", buf);
		pr_buf(buf, sizeof(buf), n, "%*pbl",
		       PRSR_NUM_MERGE_ENTRIES, proto->merge_map);
		seq_printf(f, " | %-15s |\n", buf);
	}

done:
	seq_puts(f, " +-------------+---------------------------+----------+--------+----------+-------+-----------------+-----------------+-----------------+-----------------+\n");
	seq_puts(f, "\n");
}

PP_DEFINE_DEBUGFS(protocols_show, __prsr_protocols_show, NULL);

enum proto_del_opts {
	proto_del_opt_idx,
};

static const match_table_t proto_del_tokens = {
	{proto_del_opt_idx, "idx=%u"},
};

void __prsr_dbg_proto_del_help(struct seq_file *f)
{
	seq_puts(f, "idx=<protocol_number>\n");
}

/**
 * @brief Delete a protocol from the parser driver and hw
 * @note the idx should be the internal id
 */
void __prsr_dbg_proto_del(char *cmd_buf, void *data)
{
	substring_t args[MAX_OPT_ARGS];
	enum proto_del_opts opt;
	char *tok, *params;
	int id;

	params = cmd_buf;
	/* iterate over user params */
	while ((tok = strsep(&params, " \t\n")) != NULL) {
		if (!*tok)
			continue;
		opt = match_token(tok, proto_del_tokens, args);
		switch (opt) {
		case proto_del_opt_idx:
			if (match_int(&args[0], &id))
				return;
			/* delete the protocol w/o deleting the debugfs files
			 * so we don't get into a deadlock
			 */
			id = __prsr_proto_del((enum prsr_proto_id)id, false);
			break;
		default:
			break;
		}
	}
}

PP_DEFINE_DEBUGFS(proto_del, __prsr_dbg_proto_del_help, __prsr_dbg_proto_del);

/**
 * @brief Print all tables status
 */
void __prsr_tables_show(struct seq_file *f)
{
	struct table *tbl;
	char buf[256];
	u32 n = 0;
	u16 i;

	seq_puts(f, "\n");
	seq_puts(f, " +----------------+--------------------+-------------+-------------+----------------------+\n");
	seq_puts(f, " | Name           | Num Entries (rsrv) | Used (rsrv) | Free (rsrv) | Used Entries         |\n");
	seq_puts(f, " +----------------+--------------------+-------------+-------------+----------------------+\n");

	PRSR_FOR_EACH_TABLE(i) {
		tbl = prsr_get_tbl(i);
		if (unlikely(!tbl))
			goto done;
		seq_printf(f, " | %-14s | %-11u (%4u)",
			   tbl->name, tbl->n_ent, tbl->n_rsrv);
		seq_printf(f, " | %-4u (%4u)",
			   tbl->reg.n_used + tbl->rsrv.n_used,
			   tbl->rsrv.n_used);
		seq_printf(f, " | %-4u (%4u)",
			   tbl->reg.n_free + tbl->rsrv.n_free,
			   tbl->rsrv.n_free);
		pr_buf(buf, sizeof(buf), n, "%*pbl",
		       tbl->n_ent, tbl->entrs_map);
		seq_printf(f, " | %-20s |\n", buf);
	}
done:
	seq_puts(f, " +----------------+--------------------+-------------+-------------+----------------------+\n");

	if (unlikely(!tbl))
		seq_printf(f, "failed to get table %i info\n", i);
}

PP_DEFINE_DEBUGFS(tables_show, __prsr_tables_show, NULL);

/**
 * @brief Print 'n' layer 2 entries from HW
 */
static int __pr_l2_entries(struct seq_file *f, u16 start,
			   u32 n, bool header, bool footer)
{
	struct pit_info pit;
	struct l2_entry ent;
	struct table *tbl;
	const char *name;
	bool valid;
	ulong hit_map, last_hit_map;
	s32 ret = 0;
	u16 i;

	if (unlikely(!f))
		return -EINVAL;
	tbl = prsr_get_tbl(PRSR_L2_TBL);
	hit_map      = (ulong)prsr_l2_table_hit_map_get();
	last_hit_map = (ulong)prsr_l2_table_last_hit_map_get();
	/* First 2 bits represent the pre L2 and the eth header
	 * and not the table entries, so we ignore them
	 */
	hit_map      = hit_map      >> 2;
	last_hit_map = last_hit_map >> 2;

	if (header) {
		seq_puts(f, " +-------+-----------------+-----+----------+----------+-------+------------+-----------+-----+-------+\n");
		seq_puts(f, " | ==================================  Layer 2 Protocols Table  ===================================== |\n");
		seq_puts(f, " +-------+-----------------+-----+----------+----------+-------+------------+-----------+-----+-------+\n");
		seq_puts(f, " | Index | Portocol        | Hit | Last Hit | Fallback | Valid | Compare OP | Ethertype | Len | PIT   |\n");
		seq_puts(f, " +-------+-----------------+-----+----------+----------+-------+------------+-----------+-----+-------+\n");
	}
	n = min(n, tbl->n_ent);
	for (i = start; i < start + n; i++) {
		ret = prsr_l2_entry_get(i, &valid, &ent, &pit);
		if (unlikely(ret))
			goto done;

		name = table_entry_name_get(tbl, i);

		seq_printf(f, " | %-5u | %-15s", i, name);
		seq_printf(f, " | %-3u", test_bit(i, &hit_map));
		seq_printf(f, " | %-8u", test_bit(i, &last_hit_map));
		seq_printf(f, " | %-8u | %-5u | %1u-%-8s | %#09x | %-3u",
			   ent.fallback, valid, ent.cmp_op,
			   l2_cmp_op_str[ent.cmp_op], ent.ethtype, ent.hdr_len);
		__pit_seq_printf(f, &pit, 5);
		seq_puts(f, " |\n");
	}

done:
	if (footer)
		seq_puts(f, " +-------+-----------------+-----+----------+----------+-------+------------+-----------+-----+-------+\n");
	if (unlikely(ret))
		seq_printf(f, "failed to get entry %i info\n", i);

	return 0;
}

enum l2_opt {
	l2_opt_idx,
	l2_opt_valid,
	l2_opt_fallback,
	l2_opt_cmp_op,
	l2_opt_ethtype,
	l2_opt_len,
	l2_opt_pit_idx,
	l2_opt_pit_cnt,
};

static const match_table_t l2_tokens = {
	{l2_opt_idx,      "idx=%u"     },
	{l2_opt_valid,    "valid=%u"   },
	{l2_opt_fallback, "fallback=%u"},
	{l2_opt_cmp_op,   "cmp_op=%u"  },
	{l2_opt_ethtype,  "ethtype=%u" },
	{l2_opt_len,      "len=%u"     },
	{l2_opt_pit_idx,  "pidx=%u"    },
	{l2_opt_pit_cnt,  "pcnt=%u"    },
};

/**
 * @brief Change layer 2 entry configuration
 */
void __prsr_l2_set(char *cmd_buf, void *data)
{
	substring_t args[MAX_OPT_ARGS];
	struct l2_entry hw_ent;
	struct pit_info pit;
	enum l2_opt     opt;
	char *tok, *params;
	s32   val, ret, idx;
	bool  valid = true;

	params = cmd_buf;
	idx = TABLE_ENTRY_INVALID_IDX;
	/* iterate over user params */
	while ((tok = strsep(&params, " \t\n")) != NULL) {
		if (!*tok)
			continue;
		opt = match_token(tok, l2_tokens, args);
		switch (opt) {
		case l2_opt_idx:
			if (match_int(&args[0], &idx))
				return;
			ret = prsr_l2_entry_get((u16)idx, &valid, &hw_ent,
						&pit);
			if (ret)
				return;
			break;
		case l2_opt_valid:
			if (match_int(&args[0], &val))
				return;
			valid = val ? true : false;
			break;
		case l2_opt_fallback:
			if (match_int(&args[0], &val))
				return;
			hw_ent.fallback = val ? true : false;
			break;
		case l2_opt_cmp_op:
			if (match_int(&args[0], &val))
				return;
			hw_ent.cmp_op = (enum l2_cmp_op)val;
			break;
		case l2_opt_ethtype:
			if (match_hex(&args[0], &val))
				return;
			hw_ent.ethtype = val;
			break;
		case l2_opt_len:
			if (match_int(&args[0], &val))
				return;
			hw_ent.hdr_len = val;
			break;
		case l2_opt_pit_idx:
			if (match_int(&args[0], &val))
				return;
			pit.base = val;
			break;
		case l2_opt_pit_cnt:
			if (match_int(&args[0], &val))
				return;
			pit.cnt = val;
			break;
		default:
			break;
		}
	}

	if (valid)
		prsr_l2_entry_set((u16)idx, &hw_ent, &pit);
	else
		prsr_l2_entry_set((u16)idx, NULL, NULL);
}

/**
 * @brief Print all layer 2 entries from hw
 */
void __prsr_l2_show(struct seq_file *f)
{
	struct pit_info pit;
	s32 ret = 0;
	u16 len, dflt;

	ret = prsr_mac_cfg_get(&len, &pit);
	if (unlikely(ret)) {
		seq_puts(f, "failed to get last ethertype map info\n");
		return;
	}
	seq_puts(f, "\n");
	seq_puts(f, " +-------------------------+\n");
	seq_puts(f, " | MAC Configuration       |\n");
	seq_puts(f, " +-------+-----------------+\n");
	seq_puts(f, " | PIT   | Length          |\n");
	seq_puts(f, " +-------+-----------------+\n");
	__pit_seq_printf(f, &pit, 5);
	seq_printf(f, " | %-15u |\n", len);

	ret = prsr_ethertype_cfg_get(&dflt, &pit);
	if (unlikely(ret)) {
		seq_puts(f, "failed to get last ethertype map info\n");
		return;
	}
	seq_puts(f, " +-------------------------+\n");
	seq_puts(f, " | Last Ethertype Map      |\n");
	seq_puts(f, " +-------+-----------------+\n");
	seq_puts(f, " | PIT   | Default PC      |\n");
	seq_puts(f, " +-------+-----------------+\n");
	__pit_seq_printf(f, &pit, 5);
	seq_printf(f, " | %-15u |\n", dflt);

	__pr_l2_entries(f, 0, ~0, true, true);
}

PP_DEFINE_DEBUGFS(l2, __prsr_l2_show, __prsr_l2_set);

enum last_ethtype_opt {
	leth_opt_idx,
	leth_opt_valid,
	leth_opt_imem_idx,
	leth_opt_ethtype,
};

static const match_table_t last_ethtype_tokens = {
	{leth_opt_idx,      "idx=%u"     },
	{leth_opt_valid,    "valid=%u"   },
	{leth_opt_imem_idx, "imem_idx=%u"},
	{leth_opt_ethtype,  "ethtype=%u" },
};

/**
 * @brief Change last ethertype entry configuration
 */
void __prsr_last_ethertype_set(char *cmd_buf, void *data)
{
	struct last_ethertype_entry hw_ent;
	enum last_ethtype_opt       opt;
	substring_t args[MAX_OPT_ARGS];
	char *tok, *params;
	s32   val, ret, idx;
	bool  valid = true;

	params = cmd_buf;
	idx = TABLE_ENTRY_INVALID_IDX;
	/* iterate over user params */
	while ((tok = strsep(&params, " \t\n")) != NULL) {
		if (!*tok)
			continue;
		opt = match_token(tok, last_ethtype_tokens, args);
		switch (opt) {
		case leth_opt_idx:
			if (match_int(&args[0], &idx))
				return;
			ret = prsr_last_ethertype_entry_get((u16)idx, &valid,
							    &hw_ent);
			if (ret)
				return;
			break;
		case leth_opt_valid:
			if (match_int(&args[0], &val))
				return;
			valid = val ? true : false;
			break;
		case leth_opt_imem_idx:
			if (match_int(&args[0], &val))
				return;
			hw_ent.imem_idx = val;
			break;
		case leth_opt_ethtype:
			if (match_hex(&args[0], &val))
				return;
			hw_ent.proto_id = val;
			break;
		default:
			break;
		}
	}

	if (valid)
		prsr_last_ethertype_entry_set(idx, &hw_ent);
	else
		prsr_last_ethertype_entry_set(idx, NULL);
}

/**
 * @brief Print 'n' last ethertype entries starting at 'start'
 */
static int __pr_last_ethertype_entries(struct seq_file *f, u16 start,
				       u32 n, bool header, bool footer)
{
	struct last_ethertype_entry ent;
	struct table *tbl;
	const char *name;
	bool valid;
	s32 ret = 0;
	u16 i;

	if (unlikely(!f))
		return -EINVAL;
	tbl = prsr_get_tbl(PRSR_LAST_ETHERTYPE_TBL);

	if (header) {
		seq_puts(f, " +-------+-----------------+-------+----------+----------+\n");
		seq_puts(f, " | ========  Last Ethertype Protocols table  =========== |\n");
		seq_puts(f, " +-------+-----------------+-------+----------+----------+\n");
		seq_puts(f, " | Index | Protocol        | Valid | Proto id | IMEM IDX |\n");
		seq_puts(f, " +-------+-----------------+-------+----------+----------+\n");
	}

	n = min(n, tbl->n_ent);
	for (i = start; i < start + n; i++) {
		ret = prsr_last_ethertype_entry_get(i, &valid, &ent);
		if (unlikely(ret))
			goto done;

		name = table_entry_name_get(tbl, i);
		seq_printf(f, " | %-5u | %-15s", i, name);
		seq_printf(f, " | %-5u | %#06x   | %-8u |\n",
			   valid, ent.proto_id, ent.imem_idx);
	}

done:
	if (footer)
		seq_puts(f, " +-------+-----------------+-------+----------+----------+\n");
	if (unlikely(ret))
		seq_printf(f, "failed to get entry %i info\n", i);
	return 0;
}

/**
 * @brief Print all last ethertype entries
 */
void __prsr_last_ethertype_show(struct seq_file *f)
{
	seq_puts(f, "\n");
	__pr_last_ethertype_entries(f, 0, ~0, true, true);
	seq_puts(f, "\n");
}

PP_DEFINE_DEBUGFS(last_ethertype, __prsr_last_ethertype_show,
		  __prsr_last_ethertype_set);

enum imem_opt {
	imem_opt_idx,
	imem_opt_valid,
	imem_opt_hdr_len,
	imem_opt_np_off,
	imem_opt_np_sz,
	imem_opt_np_dflt_im0,
	imem_opt_np_logic,
	imem_opt_hdr_len_off,
	imem_opt_hdr_len_sz,
	imem_opt_hdr_len_im1,
	imem_opt_skip_logic,
	imem_opt_skip_op,
	imem_opt_hdr_len_chck,
	imem_opt_hdr_len_max,
	imem_opt_hdr_len_min,
	imem_opt_rep_valid,
	imem_opt_rep_idx,
	imem_opt_dont_care,
	imem_opt_fallback,
	imem_opt_etype,
	imem_opt_pidx,
	imem_opt_pcnt,
};

static const match_table_t imem_tokens = {
	{imem_opt_idx,          "idx=%u"        },
	{imem_opt_valid,        "valid=%u"      },
	{imem_opt_hdr_len,      "hdr_len=%u"    },
	{imem_opt_np_off,       "np_off=%u"     },
	{imem_opt_np_sz,        "np_sz=%u"      },
	{imem_opt_np_dflt_im0,  "im0=%u"        },
	{imem_opt_np_logic,     "np_logic=%u"   },
	{imem_opt_hdr_len_off,  "hdrlen_off=%u" },
	{imem_opt_hdr_len_sz,   "hdrlen_sz=%u"  },
	{imem_opt_hdr_len_im1,  "im1=%u"        },
	{imem_opt_skip_logic,   "skip_logic=%u" },
	{imem_opt_skip_op,      "skip_op=%u"    },
	{imem_opt_hdr_len_chck, "hdrlen_chck=%u"},
	{imem_opt_hdr_len_max,  "hdrlen_max=%u" },
	{imem_opt_hdr_len_min,  "hdrlen_min=%u" },
	{imem_opt_rep_valid,    "rep_valid=%u"  },
	{imem_opt_rep_idx,      "rep_idx=%u"    },
	{imem_opt_dont_care,    "dont_care=%u"  },
	{imem_opt_fallback,     "fallback=%u"   },
	{imem_opt_etype,        "etype=%u"      },
	{imem_opt_pidx,         "pidx=%u"       },
	{imem_opt_pcnt,         "pcnt=%u"       },
};

/**
 * @brief Change imem entry configuration
 */
void __prsr_imem_set(char *cmd_buf, void *data)
{
	struct imem_entry hw_ent;
	enum imem_opt     opt;
	substring_t args[MAX_OPT_ARGS];
	char *tok, *params;
	s32   val, ret, idx;
	bool  valid = true;

	params = cmd_buf;
	idx = TABLE_ENTRY_INVALID_IDX;
	/* iterate over user params */
	while ((tok = strsep(&params, " \t\n")) != NULL) {
		if (!*tok)
			continue;
		opt = match_token(tok, imem_tokens, args);
		switch (opt) {
		case imem_opt_idx:
			if (match_int(&args[0], &idx))
				return;
			ret = prsr_imem_entry_get((u16)idx, &hw_ent);
			if (ret)
				return;
			break;
		case imem_opt_valid:
			if (match_int(&args[0], &val))
				return;
			valid = val ? true : false;
			break;
		case imem_opt_hdr_len:
			if (match_int(&args[0], &val))
				return;
			hw_ent.hdr_len = val;
			break;
		case imem_opt_np_off:
			if (match_int(&args[0], &val))
				return;
			hw_ent.np_off = val;
			break;
		case imem_opt_np_sz:
			if (match_int(&args[0], &val))
				return;
			hw_ent.np_sz = val;
			break;
		case imem_opt_np_dflt_im0:
			if (match_int(&args[0], &val))
				return;
			hw_ent.np_dflt_im0 = val;
			break;
		case imem_opt_np_logic:
			if (match_int(&args[0], &val))
				return;
			hw_ent.np_logic = val;
			break;
		case imem_opt_hdr_len_off:
			if (match_int(&args[0], &val))
				return;
			hw_ent.hdr_len_off = val;
			break;
		case imem_opt_hdr_len_sz:
			if (match_int(&args[0], &val))
				return;
			hw_ent.hdr_len_sz = val;
			break;
		case imem_opt_hdr_len_im1:
			if (match_int(&args[0], &val))
				return;
			hw_ent.hdr_len_im1 = val;
			break;
		case imem_opt_skip_logic:
			if (match_int(&args[0], &val))
				return;
			hw_ent.skip_logic = val;
			break;
		case imem_opt_skip_op:
			if (match_int(&args[0], &val))
				return;
			hw_ent.skip_op = val;
			break;
		case imem_opt_hdr_len_chck:
			if (match_int(&args[0], &val))
				return;
			hw_ent.hdr_len_chck = val ? true : false;
			break;
		case imem_opt_hdr_len_max:
			if (match_int(&args[0], &val))
				return;
			hw_ent.hdr_len_max = val;
			break;
		case imem_opt_hdr_len_min:
			if (match_int(&args[0], &val))
				return;
			hw_ent.hdr_len_min = val;
			break;
		case imem_opt_rep_valid:
			if (match_int(&args[0], &val))
				return;
			hw_ent.rep_valid = val ? true : false;
			break;
		case imem_opt_rep_idx:
			if (match_int(&args[0], &val))
				return;
			hw_ent.rep_idx = val;
			break;
		case imem_opt_dont_care:
			if (match_int(&args[0], &val))
				return;
			hw_ent.dont_care = val ? true : false;
			break;
		case imem_opt_fallback:
			if (match_int(&args[0], &val))
				return;
			hw_ent.fallback = val ? true : false;
			break;
		case imem_opt_etype:
			if (match_int(&args[0], &val))
				return;
			hw_ent.entry_type = val;
			break;
		case imem_opt_pidx:
			if (match_int(&args[0], &val))
				return;
			hw_ent.pit.base = val;
			break;
		case imem_opt_pcnt:
			if (match_int(&args[0], &val))
				return;
			hw_ent.pit.cnt = val;
			break;
		default:
			break;
		}
	}

	if (valid)
		prsr_imem_entry_set((u16)idx, &hw_ent);
	else
		prsr_imem_entry_set((u16)idx, NULL);
}

/**
 * @brief Print 'n' imem entries starting at 'start'
 */
static int __pr_imem_entries(struct seq_file *f, u16 start,
			     u32 n, bool header, bool footer)
{
	const struct table_entry *ent;
	struct imem_entry hw_ent;
	struct table *tbl;
	u64 map;
	ulong hit_map[BITS_TO_LONGS(PRSR_NUM_IMEM_ENTRIES)];
	s32 ret = 0;
	u16 i;

	if (unlikely(!f))
		return -EINVAL;

	tbl = prsr_get_tbl(PRSR_IMEM_TBL);
	map = prsr_imem_hit_map_get();
	memcpy(hit_map, &map, sizeof(map));

	pr_info("map %#llx\n", map);
	if (header) {
		seq_puts(f, " +-----+----------------------+-----+---------+-----------------------------+------------------+-------------------+-------------------+-------+-------------+-------------+----------+-------+\n");
		seq_puts(f, " | ===================================================================  IMEM Protocols Table  =============================================================================================== |\n");
		seq_puts(f, " +-----+----------------------+-----+---------+-----------------------------+------------------+-------------------+-------------------+-------+-------------+-------------+----------+-------+\n");
		seq_puts(f, " |     |                      |     |         |   Next Protocol fld0        |  Header Len fld1 |        Skip       |   Length Checker  |       | Replacement |             |          |       |\n");
		seq_puts(f, " |     |                      |     |         |-----------------------------|------------------|-------------------|-------------------| Don't |-------------|             |          |       |\n");
		seq_puts(f, " | Idx | Protocol             | Hit | HDR Len | Off | Size | IM0 | Logic    | Off | Size | IM1 | Logic    | Opcode | MAX | MIN | Valid | Care  | Valid | Idx | Entry Type  | Fallback | Pit   |\n");
		seq_puts(f, " +-----+----------------------+-----+---------+-----------------------------+------------------+-------------------+-------------------+-------+-------------+-------------+----------+-------+\n");
	}

	n = min(n, tbl->n_ent);
	for (i = start; i < start + n; i++) {
		ret = prsr_imem_entry_get(i, &hw_ent);
		if (unlikely(ret))
			goto done;

		ent = table_entry_get(tbl, i);
		if (unlikely(!ent))
			goto done;

		/* don't print unused entries */
		if (!ent->used)
			continue;

		seq_printf(f, " | %-3u | %-20s", i, ent->name);
		seq_printf(f, " | %3u", test_bit(i, hit_map));
		seq_printf(f, " | %-7u | %-3u | %-4u | %-3u | %u-%-6s",
			   hw_ent.hdr_len, hw_ent.np_off, hw_ent.np_sz,
			   hw_ent.np_dflt_im0, hw_ent.np_logic,
			   np_logic_str[hw_ent.np_logic]);
		seq_printf(f, " | %-3u | %-4u | %-3u",
			   hw_ent.hdr_len_off, hw_ent.hdr_len_sz,
			   hw_ent.hdr_len_im1);
		seq_printf(f, " | %1u-%-6s | %-6u", hw_ent.skip_logic,
			   skip_logic_str[hw_ent.skip_logic], hw_ent.skip_op);
		seq_printf(f, " | %-3u | %-3u | %-5u", hw_ent.hdr_len_max,
			   hw_ent.hdr_len_min, hw_ent.hdr_len_chck);
		seq_printf(f, " | %-5u | %-5u | %-3u | %2u-%-8s | %-8u",
			   hw_ent.dont_care, hw_ent.rep_valid, hw_ent.rep_idx,
			   hw_ent.entry_type, etype_str[hw_ent.entry_type],
			   hw_ent.fallback);
		__pit_seq_printf(f, &hw_ent.pit, 5);
		seq_puts(f, " |\n");
	}

done:
	if (footer)
		seq_puts(f, " +-----+----------------------+-----+---------+-----------------------------+------------------+-------------------+-------------------+-------+-------------+-------------+----------+-------+\n");
	if (unlikely(ret))
		seq_printf(f, "failed to get entry %i info\n", i);

	return 0;
}

/**
 * @brief Print all imem entries
 */
void __prsr_imem_show(struct seq_file *f)
{
	seq_puts(f, "\n");
	__pr_imem_entries(f, 0, ~0, true, true);
	seq_puts(f, "\n");
}

PP_DEFINE_DEBUGFS(imem, __prsr_imem_show, __prsr_imem_set);

enum np_sel_opt {
	np_sel_opt_idx,
	np_sel_opt_valid,
	np_sel_opt_key,
	np_sel_opt_imem_idx,
};

static const match_table_t np_sel_tokens = {
	{np_sel_opt_idx,      "idx=%u"     },
	{np_sel_opt_valid,    "valid=%u"   },
	{np_sel_opt_key,      "key=%u"     },
	{np_sel_opt_imem_idx, "imem_idx=%u"},
};

/**
 * @brief Change np select table entry configuration
 */
void __prsr_np_sel_set(char *cmd_buf, void *data)
{
	struct np_sel_entry hw_ent;
	enum np_sel_opt     opt;
	substring_t args[MAX_OPT_ARGS];
	char *tok, *params;
	s32   val, ret, idx;
	bool  valid = true;

	params = cmd_buf;
	idx = TABLE_ENTRY_INVALID_IDX;
	/* iterate over user params */
	while ((tok = strsep(&params, " \t\n")) != NULL) {
		if (!*tok)
			continue;
		opt = match_token(tok, np_sel_tokens, args);
		switch (opt) {
		case np_sel_opt_idx:
			if (match_int(&args[0], &idx))
				return;
			ret = prsr_np_sel_entry_get(idx, &valid, &hw_ent);
			if (ret)
				return;
			break;
		case np_sel_opt_valid:
			if (match_int(&args[0], &val))
				return;
			valid = val ? true : false;
			break;
		case np_sel_opt_key:
			if (match_int(&args[0], &val))
				return;
			hw_ent.key = val;
			break;
		case np_sel_opt_imem_idx:
			if (match_int(&args[0], &val))
				return;
			hw_ent.imem_idx = val;
			break;
		default:
			break;
		}
	}

	if (valid)
		prsr_np_sel_entry_set((u16)idx, &hw_ent);
	else
		prsr_np_sel_entry_set((u16)idx, NULL);
}

/**
 * @brief Print 'n' np select table entries starting at 'start'
 */
static int __pr_np_sel_entries(struct seq_file *f, u16 start,
			       u32 n, bool header, bool footer)
{
	const struct table_entry *imem_ent;
	const struct table_entry *np_ent;
	struct proto_db_e *proto;
	struct np_sel_entry np_hw_ent;
	struct table *np_tbl;
	struct table *imem_tbl;
	bool valid;
	s32 ret = 0;
	u16 i;

	if (unlikely(!f))
		return -EINVAL;
	np_tbl   = prsr_get_tbl(PRSR_NP_SEL_TBL);
	imem_tbl = prsr_get_tbl(PRSR_IMEM_TBL);

	if (header) {
		seq_puts(f, " +-------+-------------------------------------+---------+---------------------------+-------+\n");
		seq_puts(f, " | =========================  Next Protocol Select Table  ================================== |\n");
		seq_puts(f, " +-------+-------------------------------------+---------+---------------------------+-------+\n");
		seq_puts(f, " | Index | Name                                | Key     | IMEM Index                | Valid |\n");
		seq_puts(f, " +-------+-------------------------------------+---------+---------------------------+-------+\n");
	}

	n = min(n, np_tbl->n_ent);
	for (i = start; i < start + n; i++) {
		ret = prsr_np_sel_entry_get(i, &valid, &np_hw_ent);
		if (unlikely(ret))
			goto done;
		np_ent = table_entry_get(np_tbl, i);
		if (unlikely(!np_ent))
			goto done;
		/* don't print unused entries */
		if (!np_ent->used)
			continue;

		/* get the protocol who own the imem index */
		imem_ent = table_entry_get(imem_tbl, np_hw_ent.imem_idx);
		if (unlikely(!imem_ent))
			goto done;
		proto = (struct proto_db_e  *)imem_ent->data;
		seq_printf(f, " | %-5u | %-35s | %#07x | %2u-%-22s | %-5u |\n",
			   i, np_ent->name, np_hw_ent.key, np_hw_ent.imem_idx,
			   (proto ? proto->name : (char *)proto), valid);
	}

done:
	if (footer)
		seq_puts(f, " +-------+-------------------------------------+---------+---------------------------+-------+\n");
	if (unlikely(ret))
		seq_printf(f, "failed to get np select entry %i info\n", i);

	return 0;
}

/**
 * @brief Print all np select tables entries
 */
void __prsr_np_sel_show(struct seq_file *f)
{
	seq_puts(f, "\n");
	__pr_np_sel_entries(f, 0, ~0, true, true);
	seq_puts(f, "\n");
}

PP_DEFINE_DEBUGFS(np_sel, __prsr_np_sel_show, __prsr_np_sel_set);

enum skip_opt {
	skip_opt_idx,
	skip_opt_valid,
	skip_opt_key,
	skip_opt_hdrlen,
};

static const match_table_t skip_tokens = {
	{skip_opt_idx,    "idx=%u"     },
	{skip_opt_valid,  "valid=%u"   },
	{skip_opt_key,    "key=%u"     },
	{skip_opt_hdrlen, "hdrlen=%u"},
};

/**
 * @brief Change skip table entry configuration
 */
void __prsr_skip_set(char *cmd_buf, void *data)
{
	struct skip_entry hw_ent;
	enum skip_opt     opt;
	substring_t args[MAX_OPT_ARGS];
	char *tok, *params;
	s32   val, ret, idx;
	bool  valid = true;

	params = cmd_buf;
	idx = TABLE_ENTRY_INVALID_IDX;
	/* iterate over user params */
	while ((tok = strsep(&params, " \t\n")) != NULL) {
		if (!*tok)
			continue;
		opt = match_token(tok, skip_tokens, args);
		switch (opt) {
		case skip_opt_idx:
			if (match_int(&args[0], &idx))
				return;
			ret = prsr_skip_entry_get((u16)idx, &valid, &hw_ent);
			if (ret)
				return;
			break;
		case skip_opt_valid:
			if (match_int(&args[0], &val))
				return;
			valid = val ? true : false;
			break;
		case skip_opt_key:
			if (match_int(&args[0], &val))
				return;
			hw_ent.key = val;
			break;
		case skip_opt_hdrlen:
			if (match_int(&args[0], &val))
				return;
			hw_ent.hdr_len = val;
			break;
		default:
			continue;
		}
	}

	if (valid)
		prsr_skip_entry_set((u16)idx, &hw_ent);
	else
		prsr_skip_entry_set((u16)idx, NULL);
}

/**
 * @brief Print 'n' skip table entries starting at 'start'
 */
static int __pr_skip_entries(struct seq_file *f, u16 start,
			     u32 n, bool header, bool footer)
{
	struct skip_entry ent;
	struct table *tbl;
	bool valid;
	const char *name;
	s32 ret = 0;
	u16 i;

	if (unlikely(!f))
		return -EINVAL;
	tbl = prsr_get_tbl(PRSR_SKIP_TBL);

	if (header) {
		seq_puts(f, " +-------+-----------------+---------+-------+-------+\n");
		seq_puts(f, " | ==================  Skip Table  ================= |\n");
		seq_puts(f, " +-------+-----------------+---------+-------+-------+\n");
		seq_puts(f, " | Index | Name            | Key     | Value | Valid |\n");
		seq_puts(f, " +-------+-----------------+---------+-------+-------+\n");
	}

	n = min(n, tbl->n_ent);
	for (i = start; i < start + n; i++) {
		ret = prsr_skip_entry_get(i, &valid, &ent);
		if (unlikely(ret))
			goto done;
		name = table_entry_name_get(tbl, i);

		seq_printf(f, " | %-5u | %-15s | %#-7x | %-5u | %-5u |\n",
			   i, name, ent.key, ent.hdr_len, valid);
	}

done:
	if (footer)
		seq_puts(f, " +-------+-----------------+---------+-------+-------+\n");
	if (unlikely(ret))
		seq_printf(f, "failed to get skip cam entry %i info\n", i);

	return 0;
}

/**
 * @brief Print all skip table entries
 */
void __prsr_skip_show(struct seq_file *f)
{
	seq_puts(f, "\n");
	__pr_skip_entries(f, 0, ~0, true, true);
	seq_puts(f, "\n");
}

PP_DEFINE_DEBUGFS(skip, __prsr_skip_show, __prsr_skip_set);

enum merge_opt {
	merge_opt_idx,
	merge_opt_valid,
	merge_opt_prev,
	merge_opt_curr,
	merge_opt_merged,
};

static const match_table_t merge_tokens = {
	{merge_opt_idx,    "idx=%u"   },
	{merge_opt_valid,  "valid=%u" },
	{merge_opt_prev,   "prev=%u" },
	{merge_opt_curr,   "curr=%u"},
	{merge_opt_merged, "merged=%u"},
};

/**
 * @brief Change merge table entry configuration
 */
void __prsr_merge_set(char *cmd_buf, void *data)
{
	struct merge_entry hw_ent;
	enum merge_opt     opt;
	substring_t args[MAX_OPT_ARGS];
	char *tok, *params;
	s32   val, ret, idx;
	bool  valid = true;

	params = cmd_buf;
	idx = TABLE_ENTRY_INVALID_IDX;
	/* iterate over user params */
	while ((tok = strsep(&params, " \t\n")) != NULL) {
		if (!*tok)
			continue;
		opt = match_token(tok, merge_tokens, args);
		switch (opt) {
		case merge_opt_idx:
			if (match_int(&args[0], &idx))
				return;
			ret = prsr_merge_entry_get((u16)idx, &valid, &hw_ent);
			if (ret)
				return;
			break;
		case merge_opt_valid:
			if (match_int(&args[0], &val))
				return;
			valid = val ? true : false;
			break;
		case merge_opt_prev:
			if (match_int(&args[0], &val))
				return;
			hw_ent.prev = val;
			break;
		case merge_opt_curr:
			if (match_int(&args[0], &val))
				return;
			hw_ent.curr = val;
			break;
		case merge_opt_merged:
			if (match_int(&args[0], &val))
				return;
			hw_ent.merged = val;
			break;
		default:
			break;
		}
	}

	if (valid)
		prsr_merge_entry_set((u16)idx, &hw_ent);
	else
		prsr_merge_entry_set((u16)idx, NULL);
}

/**
 * @brief Print 'n' merge table entries starting at 'start'
 */
static int __pr_merge_entries(struct seq_file *f, u16 start,
			      u32 n, bool header, bool footer)
{
	struct merge_entry ent;
	struct table *tbl;
	bool valid;
	const char *name;
	s32 ret = 0;
	u16 i;

	if (unlikely(!f))
		return -EINVAL;
	tbl = prsr_get_tbl(PRSR_MERGE_TBL);

	if (header) {
		seq_puts(f, " +-------+------------------------------------------+----------+---------+--------+-------+\n");
		seq_puts(f, " | ===========================  Merge Protocols Table  ================================== |\n");
		seq_puts(f, " +-------+------------------------------------------+----------+---------+--------+-------+\n");
		seq_puts(f, " | Index | Name                                     | Previous | Current | Merged | Valid |\n");
		seq_puts(f, " +-------+------------------------------------------+----------+---------+--------+-------+\n");
	}

	n = min(n, tbl->n_ent);
	for (i = start; i < start + n; i++) {
		ret = prsr_merge_entry_get(i, &valid, &ent);
		if (unlikely(ret))
			goto done;
		name = table_entry_name_get(tbl, i);

		seq_printf(f, " | %-5u | %-40s | %-8u | %-7u | %-6u | %-5u |\n",
			   i, name, ent.prev, ent.curr, ent.merged, valid);
	}

done:
	if (footer)
		seq_puts(f, " +-------+------------------------------------------+----------+---------+--------+-------+\n");
	if (unlikely(ret))
		seq_printf(f, "failed to get merge table entry %i info\n", i);

	return 0;
}

/**
 * @brief Print all merge tables entries
 */
void __prsr_merge_show(struct seq_file *f)
{
	seq_puts(f, "\n");
	__pr_merge_entries(f, 0, ~0, true, true);
	seq_puts(f, "\n");
}

PP_DEFINE_DEBUGFS(merge, __prsr_merge_show, __prsr_merge_set);

enum pit_opt {
	pit_opt_idx,
	pit_opt_valid,
	pit_opt_fv_idx,
	pit_opt_len,
	pit_opt_off,
	pit_opt_mask,
};

static const match_table_t pit_tokens = {
	{pit_opt_idx,    "idx=%u"   },
	{pit_opt_valid,  "valid=%u" },
	{pit_opt_fv_idx, "fv_idx=%u"},
	{pit_opt_len,    "len=%u"   },
	{pit_opt_off,    "off=%u"   },
	{pit_opt_mask,   "mask=%u"  },
};

/**
 * @brief Change pit table entry configuration
 */
void __prsr_pit_set(char *cmd_buf, void *data)
{
	struct pit_entry hw_ent;
	enum pit_opt     opt;
	substring_t args[MAX_OPT_ARGS];
	char *tok, *params;
	s32   val, ret, idx;
	bool  valid = true;

	params = cmd_buf;
	idx = TABLE_ENTRY_INVALID_IDX;
	/* iterate over user params */
	while ((tok = strsep(&params, " \t\n")) != NULL) {
		if (!*tok)
			continue;
		opt = match_token(tok, pit_tokens, args);
		switch (opt) {
		case pit_opt_idx:
			if (match_int(&args[0], &idx))
				return;
			ret = prsr_pit_entry_get((u16)idx, &hw_ent);
			if (ret)
				return;
			break;
		case pit_opt_valid:
			if (match_int(&args[0], &val))
				return;
			valid = val ? true : false;
			break;
		case pit_opt_fv_idx:
			if (match_int(&args[0], &val))
				return;
			hw_ent.fv_idx = val;
			break;
		case pit_opt_len:
			if (match_int(&args[0], &val))
				return;
			hw_ent.len = val;
			break;
		case pit_opt_off:
			if (match_int(&args[0], &val))
				return;
			hw_ent.off = val;
			break;
		case pit_opt_mask:
			if (match_hex(&args[0], &val))
				return;
			hw_ent.mask = val;
			break;
		default:
			break;
		}
	}

	if (valid)
		prsr_pit_entry_set((u16)idx, &hw_ent);
	else
		prsr_pit_entry_set((u16)idx, NULL);
}

/**
 * @brief Print 'n' pit table entries starting at 'start'
 */
static int __pr_pit_entries(struct seq_file *f, u16 start,
			    u32 n, bool header, bool footer)
{
	const struct table_entry *ent;
	struct pit_entry hw_ent;
	struct table *tbl;
	s32 ret = 0;
	u16 i;

	if (unlikely(!f))
		return -EINVAL;
	tbl = prsr_get_tbl(PRSR_PIT_TBL);

	if (header) {
		seq_puts(f, " +-------+-----+-------------------------------------+--------------+----------+-------------+------------+\n");
		seq_puts(f, " | ============================================  PIT Table  ============================================= |\n");
		seq_puts(f, " +-------+-----+-------------------------------------+--------------+----------+-------------+------------+\n");
		seq_puts(f, " | Index | Ref | Protocol                            | Target Index | Data Len | Data Offset | FV Mask    |\n");
		seq_puts(f, " +-------+-----+-------------------------------------+--------------+----------+-------------+------------+\n");
	}

	n = min(n, tbl->n_ent);
	for (i = start; i < start + n; i++) {
		ret = prsr_pit_entry_get(i, &hw_ent);
		if (unlikely(ret))
			goto done;

		ent = table_entry_get(tbl, i);
		if (unlikely(!ent))
			goto done;

		/* don't print unused entries */
		if (!ent->used)
			continue;

		seq_printf(f, " | %-5u | %-3u | %-35s",
			   i, ent->ref_cnt, ent->name);
		seq_printf(f, " | %-12u | %-8u | %-11u | %#010x |\n",
			   hw_ent.fv_idx, hw_ent.len, hw_ent.off, hw_ent.mask);
	}

done:
	if (footer)
		seq_puts(f, " +-------+-----+-------------------------------------+--------------+----------+-------------+------------+\n");
	if (unlikely(ret))
		seq_printf(f, "failed to get entry %i info\n", i);

	return 0;
}

/**
 * @brief Print all pit table entries
 */
void __prsr_pit_show(struct seq_file *f)
{
	seq_puts(f, "\n");
	__pr_pit_entries(f, 0, ~0, true, true);
	seq_puts(f, "\n");
}

PP_DEFINE_DEBUGFS(pit, __prsr_pit_show, __prsr_pit_set);

enum ports_opt {
	ports_opt_idx,
	ports_opt_fallback,
	ports_opt_imem_idx,
	ports_opt_fld0_stw,
	ports_opt_fld0_len,
	ports_opt_fld0_fv,
	ports_opt_fld1_stw,
	ports_opt_fld1_len,
	ports_opt_fld1_fv,
	ports_opt_fld2_stw,
	ports_opt_fld2_len,
	ports_opt_fld2_fv,
	ports_opt_fld3_stw,
	ports_opt_fld3_len,
	ports_opt_fld3_fv,
};

static const match_table_t ports_tokens = {
	{ports_opt_idx,      "idx=%u"     },
	{ports_opt_fallback, "fallback=%u"},
	{ports_opt_imem_idx, "imem_idx=%u"},
	{ports_opt_fld0_stw, "fld0_stw=%u"},
	{ports_opt_fld0_len, "fld0_len=%u"},
	{ports_opt_fld0_fv,  "fld0_fv=%u" },
	{ports_opt_fld1_stw, "fld1_stw=%u"},
	{ports_opt_fld1_len, "fld1_len=%u"},
	{ports_opt_fld1_fv,  "fld1_fv=%u" },
	{ports_opt_fld2_stw, "fld2_stw=%u"},
	{ports_opt_fld2_len, "fld2_len=%u"},
	{ports_opt_fld2_fv,  "fld2_fv=%u" },
	{ports_opt_fld3_stw, "fld3_stw=%u"},
	{ports_opt_fld3_len, "fld3_len=%u"},
	{ports_opt_fld3_fv,  "fld3_fv=%u" },
};

/**
 * @brief Change port configuration
 */
void __prsr_ports_set(char *cmd_buf, void *data)
{
	struct port_cfg_entry hw_ent;
	enum ports_opt     opt;
	substring_t args[MAX_OPT_ARGS];
	char *tok, *params;
	s32   val, ret, idx;

	params = cmd_buf;
	idx = TABLE_ENTRY_INVALID_IDX;
	/* iterate over user params */
	while ((tok = strsep(&params, " \t\n")) != NULL) {
		if (!*tok)
			continue;
		opt = match_token(tok, ports_tokens, args);
		switch (opt) {
		case ports_opt_idx:
			if (match_int(&args[0], &idx))
				return;
			ret = prsr_port_cfg_entry_get(idx, &hw_ent);
			if (ret)
				return;
			break;
		case ports_opt_fallback:
			if (match_int(&args[0], &val))
				return;
			hw_ent.fallback = val;
			break;
		case ports_opt_imem_idx:
			if (match_int(&args[0], &val))
				return;
			hw_ent.skip_l2_pc = val;
			break;
		case ports_opt_fld0_stw:
			if (match_int(&args[0], &val))
				return;
			hw_ent.fld[0].stw_off = (u16)val;
			break;
		case ports_opt_fld0_len:
			if (match_int(&args[0], &val))
				return;
			hw_ent.fld[0].len = (u16)val;
			break;
		case ports_opt_fld0_fv:
			if (match_int(&args[0], &val))
				return;
			hw_ent.fld[0].fv_off = (u16)val;
			break;
		case ports_opt_fld1_stw:
			if (match_int(&args[0], &val))
				return;
			hw_ent.fld[1].stw_off = (u16)val;
			break;
		case ports_opt_fld1_len:
			if (match_int(&args[0], &val))
				return;
			hw_ent.fld[1].len = (u16)val;
			break;
		case ports_opt_fld1_fv:
			if (match_int(&args[0], &val))
				return;
			hw_ent.fld[1].fv_off = (u16)val;
			break;
		case ports_opt_fld2_stw:
			if (match_int(&args[0], &val))
				return;
			hw_ent.fld[2].stw_off = (u16)val;
			break;
		case ports_opt_fld2_len:
			if (match_int(&args[0], &val))
				return;
			hw_ent.fld[2].len = (u16)val;
			break;
		case ports_opt_fld2_fv:
			if (match_int(&args[0], &val))
				return;
			hw_ent.fld[2].fv_off = (u16)val;
			break;
		case ports_opt_fld3_stw:
			if (match_int(&args[0], &val))
				return;
			hw_ent.fld[3].stw_off = (u16)val;
			break;
		case ports_opt_fld3_len:
			if (match_int(&args[0], &val))
				return;
			hw_ent.fld[3].len = (u16)val;
			break;
		case ports_opt_fld3_fv:
			if (match_int(&args[0], &val))
				return;
			hw_ent.fld[3].fv_off = (u16)val;
			break;
		default:
			break;
		}
	}

	prsr_port_cfg_entry_set((u16)idx, &hw_ent);
}

/**
 * @brief Print all ports configuration
 */
void __prsr_ports_show(struct seq_file *f)
{
	struct port_cfg_entry cfg;
	s32 ret = 0;
	u16 i, j;

	seq_puts(f, "\n");
	seq_puts(f, " +------+------------+----------+\n");
	seq_puts(f, " | =========  Ports  ========== |\n");
	seq_puts(f, " +------+------------+----------+------------------------+------------------------+------------------------+------------------------+\n");
	seq_puts(f, " |      |            |          |         Field 0        |         Field 1        |         Field 2        |         Field 3        |\n");
	seq_puts(f, " +      |            |          +---------+-----+--------+---------+-----+--------+---------+-----+--------+---------+-----+--------+\n");
	seq_puts(f, " | Port | Skip L2 PC | Fallback | STW Off | Len | FV Off | STW Off | LEN | FV Off | STW Off | LEN | FV Off | STW Off | LEN | FV Off |\n");
	seq_puts(f, " +------+------------+----------+---------+-----+--------+---------+-----+--------+---------+-----+--------+---------+-----+--------+\n");

	for (i = 0; i < PP_MAX_PORT; i++) {
		ret = prsr_port_cfg_entry_get(i, &cfg);
		if (unlikely(ret))
			goto done;

		seq_printf(f, " | %-4u | %-10u | %-8u ",
			   i, cfg.skip_l2_pc, cfg.fallback);
		for (j = 0; j < ARRAY_SIZE(cfg.fld); j++) {
			seq_printf(f, "| %-7u | %-3u | %-6u ",
				   cfg.fld[j].stw_off, cfg.fld[j].len,
				   cfg.fld[j].fv_off);
		}
		seq_puts(f, "|\n");
	}

done:
	seq_puts(f, " +------+------------+----------+---------+-----+--------+---------+-----+--------+---------+-----+--------+---------+-----+--------+\n");
	seq_puts(f, "\n");

	if (unlikely(ret))
		seq_printf(f, "failed to get port %i info\n", i);
}

PP_DEFINE_DEBUGFS(ports, __prsr_ports_show, __prsr_ports_set);

/**
 * @brief Print protocol's information including all tables entries
 */
static int __prsr_proto_cfg_show(struct seq_file *f, void *offset)
{
	enum prsr_proto_id id = (enum prsr_proto_id)f->private;
	const struct proto_db_e *proto;
	u32 bit;

	proto = prsr_proto_info_get(id);
	if (unlikely(!proto)) {
		seq_printf(f, "failed to get proto %u info\n", id);
		return 0;
	}

	seq_puts(f, "\n");
	/* layer 2 table entry */
	if (proto->l2_idx != TABLE_ENTRY_INVALID_IDX)
		__pr_l2_entries(f, proto->l2_idx, 1, true, true);
	/* imem entry */
	if (proto->imem_idx != TABLE_ENTRY_INVALID_IDX)
		__pr_imem_entries(f, proto->imem_idx, 1, true, true);
	/* last ethertype entry */
	if (!bitmap_empty(proto->leth_map, PRSR_NUM_LAST_ETHTYPE_ENTRIES)) {
		bit = find_first_bit(proto->leth_map,
				     PRSR_NUM_LAST_ETHTYPE_ENTRIES);
		__pr_last_ethertype_entries(f, bit, 1, true, false);
		bit++;
		for_each_set_bit_from(bit, proto->leth_map,
				      PRSR_NUM_LAST_ETHTYPE_ENTRIES)
			__pr_last_ethertype_entries(f, bit, 1, false, false);
		/* print only the footer */
		__pr_last_ethertype_entries(f, 0, 0, false, true);
	}
	/* np select entries */
	if (!bitmap_empty(proto->np_map, PRSR_NUM_NP_SELECT_ENTRIES)) {
		bit = find_first_bit(proto->np_map, PRSR_NUM_NP_SELECT_ENTRIES);
		__pr_np_sel_entries(f, bit, 1, true, false);
		bit++;
		for_each_set_bit_from(bit, proto->np_map,
				      PRSR_NUM_NP_SELECT_ENTRIES)
			__pr_np_sel_entries(f, bit, 1, false, false);
		/* print only the footer */
		__pr_np_sel_entries(f, 0, 0, false, true);
	}
	/* skip entries */
	if (!bitmap_empty(proto->skip_map, PRSR_NUM_SKIP_ENTRIES)) {
		bit = find_first_bit(proto->skip_map, PRSR_NUM_SKIP_ENTRIES);
		__pr_skip_entries(f, bit, 1, true, false);
		bit++;
		for_each_set_bit_from(bit, proto->skip_map,
				      PRSR_NUM_SKIP_ENTRIES)
			__pr_skip_entries(f, bit, 1, false, false);
		/* print only the footer */
		__pr_skip_entries(f, 0, 0, false, true);
	}
	/* merge entries */
	if (!bitmap_empty(proto->merge_map, PRSR_NUM_MERGE_ENTRIES)) {
		bit = find_first_bit(proto->merge_map, PRSR_NUM_MERGE_ENTRIES);
		__pr_merge_entries(f, bit, 1, true, false);
		bit++;
		for_each_set_bit_from(bit, proto->merge_map,
				      PRSR_NUM_MERGE_ENTRIES)
			__pr_merge_entries(f, bit, 1, false, false);
		/* print only the footer */
		__pr_merge_entries(f, 0, 0, false, true);
	}
	/* pit entries */
	if (proto->pit.cnt)
		__pr_pit_entries(f, proto->pit.base, proto->pit.cnt,
				 true, true);
	seq_puts(f, "\n");

	return 0;
}

static int __prsr_proto_cfg_open(struct inode *inode, struct file *file)
{
	return single_open(file, __prsr_proto_cfg_show, inode->i_private);
}

static const struct file_operations proto_cfg_fops = {
	.open    = __prsr_proto_cfg_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = single_release,
};

/**
 * @brief Create protocol's debugfs files
 */
s32 prsr_dbg_proto_add(enum prsr_proto_id id)
{
	const struct proto_db_e *proto;
	char *str;
	s32 ret = 0;

	proto = prsr_proto_info_get(id);
	if (unlikely(!proto))
		return -EINVAL;

	str = kstrdup(proto->name, GFP_KERNEL);
	if (unlikely(!str))
		return -ENOMEM;
	/* remove all spaces */
	(void)strreplace(str, ' ', '_');
	/* convert to lower case */
	(void)str_tolower(str);
	protos_dbgfs[id] = debugfs_create_file(str, 0600, protos_dir,
					       (void *)id, &proto_cfg_fops);
	if (unlikely(IS_ERR_OR_NULL(protos_dbgfs[id]))) {
		pr_err("Failed to create %s debugfs file\n", proto->name);
		ret = (s32)PTR_ERR(protos_dbgfs[id]);
	}

	kfree(str);
	return ret;
}

/**
 * @brief Delete protocol's debugfs files
 */
s32 prsr_dbg_proto_del(enum prsr_proto_id id)
{
	if (unlikely(!prsr_proto_info_get(id)))
		return -EINVAL;

	debugfs_remove_recursive(protos_dbgfs[id]);
	protos_dbgfs[id] = NULL;

	return 0;
}

/**
 * @brief Print tables configuration changes help
 */
void __prsr_tables_help_show(struct seq_file *f)
{
	seq_puts(f, "Usage:");
	seq_puts(f, "  echo idx=<index> param=<value> [param1]=<value>... > <table>\n\n");
	seq_puts(f, "layer2         params: valid, fallback, cmo_op, ethtype, len, pidx, pcnt\n");
	seq_puts(f, "last_ethertype params: valid, imem_idx, ethtype\n");
	seq_puts(f, "imem           params: valid, hdr_len, np_off, np_sz,\n"
		 "                       im0, np_logic, hdrlen_off, hdrlen_sz,\n"
		 "                       im1, skip_logic, skip_op, hdrlen_chck,\n"
		 "                       hdrlen_max, hdrlen_min, rep_valid,\n"
		 "                       rep_idx, dont_care, fallback, etype, pidx, pcnt\n");
	seq_puts(f, "np_select      params: valid, key, imem_idx\n");
	seq_puts(f, "skip           params: valid, key, hdrlen\n");
	seq_puts(f, "merge          params: valid, first, second, merged\n");
	seq_puts(f, "pit            params: valid, fv_idx, off, len ,mask\n");
	seq_puts(f, "ports          params: fallback, imem_idx, fld[0-3]_stw, fld[0-3]_len, fld[0-3]_fv\n");
	seq_puts(f, "\n");
}

PP_DEFINE_DEBUGFS(tables_help_show, __prsr_tables_help_show, NULL);

/**
 * @brief Print field vector layer 2 fields
 */
static void __prsr_fv_l2_show(struct seq_file *f, struct pp_fv_l2 *l2)
{
	/* fix endianity */
	l2->ext_vlan  = ntohs(l2->ext_vlan);
	l2->int_vlan  = ntohs(l2->int_vlan);
	l2->pppoe_id  = ntohs(l2->pppoe_id);

	seq_printf(f, "|         | SRC MAC          | %-39pM |\n", l2->h_src);
	seq_printf(f, "| Layer 2 | DST MAC          | %-39pM |\n", l2->h_dst);
	seq_printf(f, "|         | EXT VLAN         | %-39u |\n",
		   l2->ext_vlan);
	seq_printf(f, "|         | INT VLAN         | %-39u |\n",
		   l2->int_vlan);
	seq_printf(f, "|         | Ether Type       | %-31s(0x%04x) |\n",
		   PP_FV_ETHTYPE_STR(l2->h_prot), ntohs(l2->h_prot));
	seq_printf(f, "|         | PPPoE id         | %-39u |\n", l2->pppoe_id);
}

/**
 * @brief Print field vector layer 3 fields
 */
static void __prsr_fv_l3_show(struct seq_file *f, union pp_fv_l3 *l3,
			      enum pp_fv_proto_info pinfo)
{
	struct pp_fv_ipv4 *v4;
	struct pp_fv_ipv6 *v6;
	u32 flow = 0;
	u8  tc   = 0;

	if (PP_FV_IS_L3_IPV4(pinfo)) {
		v4 = &l3->v4;
		seq_printf(f, "|         | Protocol         | %-31s(0x%04x) |\n",
			   PP_FV_L3_PROTOCOL_STR(v4->protocol), v4->protocol);
		seq_printf(f, "| Layer 3 | TOS              | %-39x |\n",
			   v4->tos);
		seq_printf(f, "|         | SRC IP           | %-39pI4b |\n",
			   &v4->saddr);
		seq_printf(f, "|         | DST IP           | %-39pI4b |\n",
			   &v4->daddr);
	} else if (PP_FV_IS_L3_IPV6(pinfo)) {
		v6 = &l3->v6;
		tc   = PP_FIELD_GET(GENMASK(7, 4),   v6->flow_lbl[0]);
		tc   = PP_FIELD_MOD(GENMASK(7, 4),   v6->priority, tc);
		flow = PP_FIELD_MOD(GENMASK(19, 16), (v6->flow_lbl[0] & 0x0f),
				    flow);
		flow = PP_FIELD_MOD(GENMASK(15, 8),  v6->flow_lbl[1], flow);
		flow = PP_FIELD_MOD(GENMASK(7, 0),   v6->flow_lbl[2], flow);
		seq_printf(f, "|         | TC               | %-39x |\n", tc);
		seq_printf(f, "| Layer 3 | Flow             | %-39x |\n", flow);
		seq_printf(f, "|         | SRC IP           | %-39pI6 |\n",
			   &v6->saddr);
		seq_printf(f, "|         | DST IP           | %-39pI6 |\n",
			   &v6->daddr);
	}
}

/**
 * @brief Print field vector layer 4 fields
 */
static void __prsr_fv_l4_show(struct seq_file *f, union pp_fv_l4 *l4,
			      union pp_fv_tunnel *tunn,
			      enum pp_fv_proto_info pinfo, u8 l3_proto)
{
	if (PP_FV_IS_L3_IPV4(pinfo)) {
		switch (l3_proto) {
		case IPPROTO_ESP:
			seq_printf(f, "| Layer 4 | ESP SPI          | %-39u |\n",
				   ntohs(l4->esp.spi));
			break;
		case IPPROTO_ICMP:
		case IPPROTO_ICMPV6:
			seq_printf(f, "| Layer 4 | Type             | %-39u |\n",
				   l4->icmp.type);
			seq_printf(f, "|         | Code             | %-39u |\n",
				   l4->icmp.code);
			break;
		case IPPROTO_UDP:
			seq_printf(f, "| Layer 4 | SRC Port         | %-39u |\n",
				   ntohs(l4->udp.src));
			seq_printf(f, "|         | DST Port         | %-39u |\n",
				   ntohs(l4->udp.dst));
				switch (ntohs(l4->udp.dst)) {
				case VXLAN_UDP_DPORT:
					seq_printf(f, "|         | Tunnel           | %-39s |\n",
						   "VXLAN");
					break;
				case GENEVE_UDP_DPORT:
					seq_printf(f, "|         | Tunnel           | %-39s |\n",
						   "GENEVE");
					break;
				case L2TP_UDP_DPORT:
					seq_printf(f, "|         | Tunnel           | %-39s |\n",
						   "L2TP");
					break;
				default:
					break;
				}
				break;
		case IPPROTO_TCP:
		case IPPROTO_SCTP:
			seq_printf(f, "| Layer 4 | SRC Port         | %-39u |\n",
				   ntohs(l4->tcp.src));
			seq_printf(f, "|         | DST Port         | %-39u |\n",
				   ntohs(l4->tcp.dst));
			if (l3_proto == IPPROTO_SCTP)
				seq_printf(f, "|         | Verification Tag | %#-39x |\n",
					   ntohl(tunn->sctp.vtag));
			break;
		case IPPROTO_L2TP:
			seq_printf(f, "| Layer 4 | Session ID       | %-39u |\n",
				   ntohl(l4->l2tpoip.sess_id));
			break;
		case IPPROTO_GRE:
			break;
		default:
			seq_printf(f, "| Layer 4 | Unknown          | %-39u |\n",
				   ntohl(l4->l2tpoip.sess_id));
			break;
		}
	} else if (PP_FV_IS_L3_IPV6(pinfo)) {
		if (PP_FV_IS_L4_TCP(pinfo) || PP_FV_IS_L4_UDP(pinfo)) {
			seq_printf(f, "| Layer 4 | SRC Port         | %-39u |\n",
				   ntohs(l4->tcp.src));
			seq_printf(f, "|         | DST Port         | %-39u |\n",
				   ntohs(l4->tcp.dst));
			if (PP_FV_IS_L4_UDP(pinfo)) {
				switch (ntohs(l4->udp.dst)) {
				case VXLAN_UDP_DPORT:
					seq_printf(f, "|         | Tunnel           | %-39s |\n",
						   "VXLAN");
					break;
				case GENEVE_UDP_DPORT:
					seq_printf(f, "|         | Tunnel           | %-39s |\n",
						   "GENEVE");
					break;
				case L2TP_UDP_DPORT:
					seq_printf(f, "|         | Tunnel           | %-39s |\n",
						   "L2TP");
					break;
				default:
					break;
				}
			}
		} else {
			seq_printf(f, "| Layer 4 | Unknown/Other    | %-39x |\n",
				   ntohl(l4->l2tpoip.sess_id));
		}
	} else if (PP_FV_OTHER_L3_OTHER_L4) {
		seq_printf(f, "| Layer 4 | Unknown/Other    | %-39x |\n",
			   ntohl(l4->l2tpoip.sess_id));
	}
}

/**
 * @brief Print field vector tunnels fields
 */
static void __prsr_fv_tunn_show(struct seq_file *f, union pp_fv_tunnel *tunn,
				enum pp_fv_proto_info pinfo, u8 l3_proto,
				u16 udp_dst_port)
{
	if (PP_FV_IS_L4_UDP(pinfo)) {
		switch (ntohs(udp_dst_port)) {
		case VXLAN_UDP_DPORT:
			seq_printf(f, "| Tunnel  | VNI              | %-39x |\n",
				   ntohl(tunn->vxlan.vni));
			break;
		case GENEVE_UDP_DPORT:
			seq_printf(f, "| Tunnel  | VNI              | %02x,%02x,%02x%32s |\n",
				   tunn->geneve.vni[2],
				   tunn->geneve.vni[1],
				   tunn->geneve.vni[0], "");
			break;
		case L2TP_UDP_DPORT:
			seq_printf(f, "| Tunnel  | Tunnel ID        | %-39x |\n",
				   tunn->l2tpoudp.tunn_id);
			seq_printf(f, "|         | Session ID       | %-39x |\n",
				   tunn->l2tpoudp.sess_id);
		default:
			break;
		}
	}
}

const char * const proto_info_str[PP_FV_PROTO_INFO_MAX] = {
	"Unknown L3          ",
	"IPv4/Unknown L4     ",
	"IPv4/UDP            ",
	"IPv4/TCP            ",
	"IPv4/Reserved L4    ",
	"IPv4/Other L4       ",
	"IPv6/Unknown L4     ",
	"IPv6/UDP            ",
	"IPv6/TCP            ",
	"IPv6/Reserved L4    ",
	"IPv6/Other L4       ",
	"Other L3/Unknown L4 ",
	"Other L3/UDP        ",
	"Other L3/TCP        ",
	"Other L3/Reserved L4",
	"Other L3/Other L4   ",
};

/**
 * @brief Print last field vector created by parser
 */
void __prsr_fv_raw_show(struct seq_file *f)
{
	struct pp_fv fv, masked_fv;
	u32 i;

	memset(&fv, 0, sizeof(fv));
	memset(&masked_fv, 0, sizeof(masked_fv));

	prsr_last_fv_get(&fv, &masked_fv);

	seq_puts(f, "\n");
	seq_puts(f, " +---------------+-------------+-------------+\n");
	seq_puts(f, " |               |  Unmasked   |  Masked     |\n");
	seq_puts(f, " +---------------+-------------+-------------+\n");
	for (i = 0; i < (sizeof(fv) / sizeof(u32)); i++) {
		seq_printf(f, " |   FV WORD%02u   | %#010x  | %#010x  |\n", i,
			   (((u32 *)&fv)[i]), (((u32 *)&masked_fv)[i]));
	}
	seq_puts(f, " +---------------+-------------+-------------+\n");
}

PP_DEFINE_DEBUGFS(fv_raw_show, __prsr_fv_raw_show, NULL);

/**
 * @brief Print last field vector created by parser
 */
void __prsr_fv_show(struct seq_file *f)
{
	struct pp_fv tmp_fv, fv;
	enum pp_fv_proto_info ext_pinfo, int_pinfo;

	memset(&tmp_fv, 0, sizeof(tmp_fv));
	memset(&fv,     0, sizeof(fv));

	prsr_last_fv_get(NULL, &tmp_fv);
	__buf_swap(&fv, sizeof(fv), &tmp_fv, sizeof(tmp_fv));

	ext_pinfo = PP_FV_OOB_EXT_PROTO_INFO_GET(fv.oob.proto_info);
	int_pinfo = PP_FV_OOB_INT_PROTO_INFO_GET(fv.oob.proto_info);
	if (ext_pinfo == PP_FV_UNKNOWN_L3 &&
	    int_pinfo != PP_FV_UNKNOWN_L3) {
		/* in case we don't have internal headers, only the internal
		 * protocol info will be set so we switch between the 2
		 */
		int_pinfo = PP_FV_OOB_EXT_PROTO_INFO_GET(fv.oob.proto_info);
		ext_pinfo = PP_FV_OOB_INT_PROTO_INFO_GET(fv.oob.proto_info);
	}

	/* fix endian */
	fv.oob.stw_data = be16_to_cpu(fv.oob.stw_data);

	seq_puts(f,   "\n");
	seq_puts(f,   "+----------------------------------------------------------------------+\n");
	seq_puts(f,   "|                Field Vector Masked                                   |\n");
	seq_puts(f,   "+----------------------------------------------------------------------+\n");
	seq_printf(f, "|         | PID              | %-39u |\n",
		   fv.oob.in_pid);
	seq_printf(f, "| OOB     | STW Data         | 0x%02x%35s |\n",
		   fv.oob.stw_data, "");
	seq_printf(f, "|         | First Prot       | %-39s |\n",
		   proto_info_str[ext_pinfo]);
	seq_printf(f, "|         | Second Proto     | %-39s |\n",
		   proto_info_str[int_pinfo]);
	seq_puts(f,   "+---------+------------------+-----------------------------------------+\n");
	seq_printf(f, "|----------------------------| %-39s |\n", "First");
	seq_puts(f,   "+---------+------------------+-----------------------------------------+\n");
	__prsr_fv_l2_show(f, &fv.first.l2);
	seq_puts(f,   "+---------+------------------+-----------------------------------------+\n");
	__prsr_fv_l3_show(f, &fv.first.l3, ext_pinfo);
	seq_puts(f,   "+---------+------------------+-----------------------------------------+\n");
	__prsr_fv_l4_show(f, &fv.first.l4, &fv.tunn, ext_pinfo,
			  fv.first.l3.v4.protocol);
	__prsr_fv_tunn_show(f, &fv.tunn, ext_pinfo, fv.first.l3.v4.protocol,
			    fv.first.l4.udp.dst);
	seq_puts(f,   "+---------+------------------+-----------------------------------------+\n");
	if (int_pinfo != PP_FV_UNKNOWN_L3) {
		seq_puts(f,   "+---------+------------------+-----------------------------------------+\n");
		__prsr_fv_l2_show(f, &fv.second.l2);
	}
	seq_puts(f,   "+---------+------------------+-----------------------------------------+\n");
	__prsr_fv_l3_show(f, &fv.second.l3, int_pinfo);
	seq_puts(f,   "+---------+------------------+-----------------------------------------+\n");
	__prsr_fv_l4_show(f, &fv.second.l4, &fv.tunn, int_pinfo,
			  fv.second.l3.v4.protocol);
	seq_puts(f,   "+---------+------------------+-----------------------------------------+\n");

	seq_puts(f, "\n");
}

PP_DEFINE_DEBUGFS(fv_show, __prsr_fv_show, NULL);

/**
 * @brief Print last packet flow in the tables
 */
void __prsr_last_pkt_show(struct seq_file *f)
{
	struct pkt_imem_flow flow;
	struct table *tbl;
	const char *name;
	u32 i;
	s32 ret;

	memset(&flow, 0, sizeof(flow));
	tbl = prsr_get_tbl(PRSR_IMEM_TBL);
	ret = prsr_imem_last_pkt_flow_get(&flow);
	if (ret) {
		seq_printf(f, "Failed to get last packet imem flow, ret %d\n",
			   ret);
		return;
	}

	seq_puts(f, "\n");
	seq_puts(f, " Parser Last Packet IMEM Flow\n");
	seq_puts(f, "==============================\n");
	for (i = 0; i < ARRAY_SIZE(flow.ent_idx); i++) {
		name = table_entry_name_get(tbl, flow.ent_idx[i]);
		seq_printf(f, " %u) %-20s [%u]\n", i, name, flow.ent_idx[i]);
	}
	seq_puts(f, "\n");
}

PP_DEFINE_DEBUGFS(last_pkt_show, __prsr_last_pkt_show, NULL);

static struct debugfs_file tables_dbg_files[] = {
	{ "all", &PP_DEBUGFS_FOPS(tables_show) },
	{ "help", &PP_DEBUGFS_FOPS(tables_help_show) },
	{ "ports", &PP_DEBUGFS_FOPS(ports) },
	{ "layer2", &PP_DEBUGFS_FOPS(l2) },
	{ "last_ethertype", &PP_DEBUGFS_FOPS(last_ethertype) },
	{ "imem", &PP_DEBUGFS_FOPS(imem) },
	{ "np_select", &PP_DEBUGFS_FOPS(np_sel) },
	{ "skip", &PP_DEBUGFS_FOPS(skip) },
	{ "merge", &PP_DEBUGFS_FOPS(merge) },
	{ "pit", &PP_DEBUGFS_FOPS(pit) },
};

static struct debugfs_file dbg_files[] = {
	{ "global", &PP_DEBUGFS_FOPS(prsr_global_cfg) },
	{ "last_fv", &PP_DEBUGFS_FOPS(fv_show) },
	{ "last_fv_raw", &PP_DEBUGFS_FOPS(fv_raw_show) },
	{ "last_pkt", &PP_DEBUGFS_FOPS(last_pkt_show) },
	{ "stw_show", &PP_DEBUGFS_FOPS(stw_show) },
	{ "stw_sticky_reset", &PP_DEBUGFS_FOPS(stw_sticky_reset) },
	{ "stats", &PP_DEBUGFS_FOPS(stats_show) },
};

static struct debugfs_file protos_dbg_files[] = {
	{ "all", &PP_DEBUGFS_FOPS(protocols_show) },
	{ "proto_delete", &PP_DEBUGFS_FOPS(proto_del) },
};

s32 prsr_dbg_init(struct dentry *parent)
{
	s32 ret;

	if (unlikely(!debugfs_initialized())) {
		prsr_err("Debugfs not initialized yet\n");
		return -EPERM;
	}

	/* parser debugfs dir */
	ret = pp_debugfs_create(parent, "parser", &dbgfs, dbg_files,
				ARRAY_SIZE(dbg_files), NULL);
	if (unlikely(ret)) {
		prsr_err("Failed to create the parser debugfs dir\n");
		return ret;
	}

	/* protocols */
	ret = pp_debugfs_create(dbgfs, "protocols", &protos_dir,
				protos_dbg_files, ARRAY_SIZE(protos_dbg_files),
				NULL);
	if (unlikely(ret)) {
		prsr_err("Failed to create the parser protocols dir\n");
		return ret;
	}

	/* tables */
	ret = pp_debugfs_create(dbgfs, "tables", &tables_dir, tables_dbg_files,
				ARRAY_SIZE(tables_dbg_files), NULL);
	if (unlikely(ret)) {
		prsr_err("Failed to create the parser tables dir\n");
		return ret;
	}

	/* Init status word common fields */
	STW_INIT_COMMON_FLDS(prsr_stw_flds);
	/* Init classifier specific status word fields */
	BUF_FLD_INIT_DESC(prsr_stw_flds, STW_PRSR_FLD_TTL_EXPIRED);
	BUF_FLD_INIT_DESC(prsr_stw_flds, STW_PRSR_FLD_IP_UNSUPP);
	BUF_FLD_INIT_DESC(prsr_stw_flds, STW_PRSR_FLD_EXT_DF);
	BUF_FLD_INIT_DESC(prsr_stw_flds, STW_PRSR_FLD_INT_DF);
	BUF_FLD_INIT_DESC(prsr_stw_flds, STW_PRSR_FLD_EXT_FRAG_TYPE);
	BUF_FLD_INIT_DESC(prsr_stw_flds, STW_PRSR_FLD_INT_FRAG_TYPE);
	BUF_FLD_INIT_DESC(prsr_stw_flds, STW_PRSR_FLD_TCP_FIN);
	BUF_FLD_INIT_DESC(prsr_stw_flds, STW_PRSR_FLD_TCP_SYN);
	BUF_FLD_INIT_DESC(prsr_stw_flds, STW_PRSR_FLD_TCP_RST);
	BUF_FLD_INIT_DESC(prsr_stw_flds, STW_PRSR_FLD_TCP_ACK);
	BUF_FLD_INIT_DESC(prsr_stw_flds, STW_PRSR_FLD_TCP_DATA_OFFSET);
	BUF_FLD_INIT_DESC(prsr_stw_flds, STW_PRSR_FLD_EXT_L3_OFFSET);
	BUF_FLD_INIT_DESC(prsr_stw_flds, STW_PRSR_FLD_INT_L3_OFFSET);
	BUF_FLD_INIT_DESC(prsr_stw_flds, STW_PRSR_FLD_EXT_L4_OFFSET);
	BUF_FLD_INIT_DESC(prsr_stw_flds, STW_PRSR_FLD_INT_L4_OFFSET);
	BUF_FLD_INIT_DESC(prsr_stw_flds, STW_PRSR_FLD_EXT_FRAG_INFO_OFFSET);
	BUF_FLD_INIT_DESC(prsr_stw_flds, STW_PRSR_FLD_INT_FRAG_INFO_OFFSET);
	BUF_FLD_INIT_DESC(prsr_stw_flds, STW_PRSR_FLD_L3_HDR_OFFSET_5);
	BUF_FLD_INIT_DESC(prsr_stw_flds, STW_PRSR_FLD_L3_HDR_OFFSET_4);
	BUF_FLD_INIT_DESC(prsr_stw_flds, STW_PRSR_FLD_L3_HDR_OFFSET_3);
	BUF_FLD_INIT_DESC(prsr_stw_flds, STW_PRSR_FLD_L3_HDR_OFFSET_2);
	BUF_FLD_INIT_DESC(prsr_stw_flds, STW_PRSR_FLD_L3_HDR_OFFSET_1);
	BUF_FLD_INIT_DESC(prsr_stw_flds, STW_PRSR_FLD_L3_HDR_OFFSET_0);
	BUF_FLD_INIT_DESC(prsr_stw_flds, STW_PRSR_FLD_ANA_PKT_TYPE);
	BUF_FLD_INIT_DESC(prsr_stw_flds, STW_PRSR_FLD_TUNN_INNER_OFFSET);
	BUF_FLD_INIT_DESC(prsr_stw_flds, STW_PRSR_FLD_PAYLOAD_OFFSET);
	BUF_FLD_INIT_DESC(prsr_stw_flds, STW_PRSR_FLD_ERROR_IND);
	BUF_FLD_INIT_DESC(prsr_stw_flds, STW_PRSR_FLD_LRU_EXP);
	BUF_FLD_INIT_DESC(prsr_stw_flds, STW_PRSR_FLD_RPB_CLID);
	BUF_FLD_INIT_DESC(prsr_stw_flds, STW_PRSR_FLD_DROP_PKT);

	pr_debug("done\n");
	return 0;
}

s32 prsr_dbg_clean(void)
{
	debugfs_remove_recursive(dbgfs);
	dbgfs = NULL;

	return 0;
}
