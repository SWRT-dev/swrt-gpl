/*
 * classifier_debugfs.c
 * Description: Packet Processor Classifier Debug FS Interface
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018-2020 Intel Corporation
 */
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/debugfs.h>
#include <linux/pp_api.h>
#include "pp_common.h"
#include "pp_debugfs_common.h"
#include "pp_si.h"
#include "classifier.h"
#include "modifier.h"
#include "classifier_internal.h"

/**
 * @brief main classifier debugfs dir
 */
static struct dentry *dbgfs;
static struct buf_fld_info cls_stw_flds[STW_CLS_FLDS_NUM];

const char * const cls_stm_str[] = {
	"N/A", "IDLE", "FETCH_BUCKET", "SEARCH", "FETCH_SESSION",
	"FM", "SEND_RESULT", "REQ_FW_FLAG", "FW_FLAG_HANDLE",
	"FW_REQ_HT_CMD", "FW_REQ_HT_DATA ", "EXT_ADD", "FW_REQ_SI_CMD",
	"FW_REQ_SI_DATA", "DROP_BMGR_WAIT"
};

/**
 * @brief Debugfs FV last show API
 */
void cls_dbg_last_fv_show(struct seq_file *f)
{
	u32 i, fv[CLS_DBG_FV_NUM_REGS];

	if (unlikely(cls_dbg_last_fv_get(fv))) {
		pr_err("failed to get last fv\n");
		return;
	}

	seq_puts(f, "\n");
	seq_puts(f, " +---------------+-------------+\n");
	seq_puts(f, " |               |     VALUE   |\n");
	seq_puts(f, " +---------------+-------------+\n");
	for (i = 0; i < CLS_DBG_FV_NUM_REGS; i++)
		seq_printf(f, " |   FV WORD%02u   | %#-10x  |\n", i, fv[i]);
	seq_puts(f, " +---------------+-------------+\n\n");
}

PP_DEFINE_DEBUGFS(last_fv_show, cls_dbg_last_fv_show, NULL);

/**
 * @brief Debugfs FV mask show API
 */
void cls_dbg_fv_mask_show(struct seq_file *f)
{
	u32 i, mask[CLS_FV_MSK_NUM_REGS];

	if (unlikely(cls_fv_mask_get(mask))) {
		pr_err("failed to get fv mask\n");
		return;
	}

	seq_puts(f, "\n");
	seq_puts(f, " +---------------+-------------+\n");
	seq_puts(f, " |               |     MASK    |\n");
	seq_puts(f, " +---------------+-------------+\n");
	for (i = 0; i < CLS_FV_MSK_NUM_REGS; i++)
		seq_printf(f, " |  MASK WORD%02u  | %#-10X  |\n", i, mask[i]);
	seq_puts(f, " +---------------+-------------+\n\n");
}

/**
 * @brief Reset lookup counters using debugfs file
 */
void cls_dbg_fv_gmask_reset(char *cmd_buf, void *data)
{
	s32  err;
	u32  mask[CLS_FV_MSK_NUM_REGS];
	bool reset;

	if (strtobool(cmd_buf, &reset)) {
		pr_err("strtobool error\n");
		return;
	}

	if (reset) {
		pr_err("failed to reset fv mask, write 0 for reset\n");
		return;
	}

	memset(mask, 0, sizeof(mask));

	err = cls_fv_mask_set(mask);
	if (unlikely(err)) {
		pr_err("failed to reset fv mask\n");
		return;
	}
}

PP_DEFINE_DEBUGFS(fv_mask, cls_dbg_fv_mask_show, cls_dbg_fv_gmask_reset);

/**
 * @brief Debugfs cuckoo hash config show API
 */
void cls_dbg_cuckoo_cfg_show(struct seq_file *f)
{
	cls_cuckoo_hash_cfg_dump();
}

PP_DEFINE_DEBUGFS(cuckoo_cfg_show, cls_dbg_cuckoo_cfg_show, NULL);

/**
 * @brief Debugfs cuckoo hash stats show API
 */
void cls_dbg_cuckoo_stats_show(struct seq_file *f)
{
	cls_cuckoo_hash_stats_dump();
}

PP_DEFINE_DEBUGFS(cuckoo_stats_show, cls_dbg_cuckoo_stats_show, NULL);

/**
 * @brief Debugfs last hash show API
 */
void cls_dbg_last_hash_show(struct seq_file *f)
{
	struct pp_hash hash;

	if (unlikely(cls_dbg_hash_get(&hash))) {
		pr_err("failed to get last hash\n");
		return;
	}

	seq_puts(f, "\n");
	seq_puts(f, " +---------------+-------------+\n");
	seq_puts(f, " | Hash Function | Last Result |\n");
	seq_puts(f, " +---------------+-------------+\n");
	seq_printf(f, " | HASH1         | 0x%-8X  |\n", hash.h1);
	seq_puts(f, " +---------------+-------------+\n");
	seq_printf(f, " | HASH2         | 0x%-8X  |\n", hash.h2);
	seq_puts(f, " +---------------+-------------+\n");
	seq_printf(f, " | SIG           | 0x%-8X  |\n", hash.sig);
	seq_puts(f, " +---------------+-------------+\n\n");
}

PP_DEFINE_DEBUGFS(last_hash_show, cls_dbg_last_hash_show, NULL);

/**
 * @brief swap fv endian at classifier ingress
 * @param data pointer to the date to set
 * @param val value to set
 * @return 0 on success
 */
static int cls_dbg_in_swap_set(void *data, u64 val)
{
	int ret;
	bool in_swap, eg_swap;

	ret = cls_fv_endian_swap_get(&in_swap, &eg_swap);
	if (unlikely(ret)) {
		pr_err("failed to get the fv swap mode\n");
		return ret;
	}

	cls_fv_endian_swap_set(!!val, eg_swap);

	return 0;
}

static int cls_dbg_in_swap_get(void *data, u64 *val)
{
	int ret;
	bool in_swap, eg_swap;

	ret = cls_fv_endian_swap_get(&in_swap, &eg_swap);
	if (unlikely(ret)) {
		pr_err("failed to get the fv swap mode\n");
		return ret;
	}

	*val = (u64)in_swap;

	return 0;
}

PP_DEFINE_DBGFS_ATT(cls_dbg_fv_in_swap, cls_dbg_in_swap_get,
		    cls_dbg_in_swap_set);

/**
 * @brief swap fv endian at classifier ingress
 * @param data pointer to the date to set
 * @param val value to set
 * @return 0 on success
 */
static int cls_dbg_eg_swap_set(void *data, u64 val)
{
	int ret;
	bool in_swap, eg_swap;

	ret = cls_fv_endian_swap_get(&in_swap, &eg_swap);
	if (unlikely(ret)) {
		pr_err("failed to get the fv swap mode\n");
		return ret;
	}

	cls_fv_endian_swap_set(in_swap, !!val);

	return 0;
}

static int cls_dbg_eg_swap_get(void *data, u64 *val)
{
	int ret;
	bool in_swap, eg_swap;

	ret = cls_fv_endian_swap_get(&in_swap, &eg_swap);
	if (unlikely(ret)) {
		pr_err("failed to get the fv swap mode\n");
		return ret;
	}

	*val = (u64)eg_swap;

	return 0;
}

PP_DEFINE_DBGFS_ATT(cls_dbg_fv_eg_swap, cls_dbg_eg_swap_get,
		    cls_dbg_eg_swap_set);

/**
 * @brief Select the sig hash function
 * @param data pointer to the date to set
 * @param val value to set
 * @return 0 on success
 */
static int cls_dbg_sig_sel_set(void *data, u64 val)
{
	int ret;
	u32 sig, hash;

	ret = cls_hash_select_get(&sig, &hash);
	if (unlikely(ret)) {
		pr_err("failed to get the hash select\n");
		return ret;
	}

	ret = cls_hash_select_set((u32)val, hash);
	if (unlikely(ret)) {
		pr_err("failed to get the hash select\n");
		return ret;
	}

	return 0;
}

static int cls_dbg_sig_sel_get(void *data, u64 *val)
{
	int ret;
	u32 sig, hash;

	ret = cls_hash_select_get(&sig, &hash);
	if (unlikely(ret)) {
		pr_err("failed to get the hash select\n");
		return ret;
	}

	*val = (u64)sig;

	return 0;
}

PP_DEFINE_DBGFS_ATT(cls_dbg_sig_sel, cls_dbg_sig_sel_get,
		    cls_dbg_sig_sel_set);

/**
 * @brief Select the hash function
 * @param data pointer to the date to set
 * @param val value to set
 * @return 0 on success
 */
static int cls_dbg_hash_sel_set(void *data, u64 val)
{
	int ret;
	u32 sig, hash;

	ret = cls_hash_select_get(&sig, &hash);
	if (unlikely(ret)) {
		pr_err("failed to get the hash select\n");
		return ret;
	}

	ret = cls_hash_select_set(sig, (u32)val);
	if (unlikely(ret)) {
		pr_err("failed to get the hash select\n");
		return ret;
	}

	return 0;
}

static int cls_dbg_hash_sel_get(void *data, u64 *val)
{
	int ret;
	u32 sig, hash;

	ret = cls_hash_select_get(&sig, &hash);
	if (unlikely(ret)) {
		pr_err("failed to get the hash select\n");
		return ret;
	}

	*val = (u64)hash;

	return 0;
}

PP_DEFINE_DBGFS_ATT(cls_dbg_hash_sel, cls_dbg_hash_sel_get,
		    cls_dbg_hash_sel_set);

static int cls_dbg_hash_sz_get(void *data, u64 *val)
{
	int ret;
	u32 size;

	ret = cls_hash_size_get(&size);
	if (unlikely(ret)) {
		pr_err("failed to get the hash size\n");
		return ret;
	}

	*val = (u64)size;

	return 0;
}

PP_DEFINE_DBGFS_ATT(cls_dbg_hash_size, cls_dbg_hash_sz_get, NULL);

static int cls_dbg_sig_err_get(void *data, u64 *val)
{
	int ret;
	s32 sig_err;

	ret = cls_sig_error_cntr_get(&sig_err);
	if (unlikely(ret)) {
		pr_err("failed to get the sig error counter size\n");
		return ret;
	}

	*val = (u64)sig_err;

	return 0;
}

PP_DEFINE_DBGFS_ATT(cls_dbg_sig_err, cls_dbg_sig_err_get, NULL);

/**
 * @brief Debugfs hash calc
 */
static void cls_dbg_hash_calc_set(char *cmd_buf, void *data)
{
	unsigned long fv[CLS_DBG_FV_NUM_REGS] = {0};
	struct pp_hash hash;
	char *tok, *args;
	u32 cnt = 0;

	args = cmd_buf;
	args = strim(args);

	/* iterate over user arguments */
	while ((tok = strsep(&args, " \t\n")) != NULL) {
		if (!*tok)
			continue;

		if (kstrtoul(tok, 0, &fv[cnt++])) {
			pr_err("kstrtou32 err\n");
			return;
		}
	}

	if (cls_hash_calc((u32 *)&fv[0], cnt << 2, &hash)) {
		pr_err("cls_hash_calc failed\n");
		return;
	}

	pr_info("\n");
	pr_info(" +---------------+-------------+\n");
	pr_info(" | Hash Function | Last Result |\n");
	pr_info(" +---------------+-------------+\n");
	pr_info(" | HASH1         | 0x%-8X  |\n", hash.h1);
	pr_info(" +---------------+-------------+\n");
	pr_info(" | HASH2         | 0x%-8X  |\n", hash.h2);
	pr_info(" +---------------+-------------+\n");
	pr_info(" | SIG           | 0x%-8X  |\n", hash.sig);
	pr_info(" +---------------+-------------+\n\n");
}

static void cls_dbg_hash_calc_help(struct seq_file *f)
{
	seq_puts(f, " <FV WORD0> <FV WORD1> <FV WORD2> ... <FV WORD31> \n");
}

PP_DEFINE_DEBUGFS(cls_dbg_hash_calc, cls_dbg_hash_calc_help,
		  cls_dbg_hash_calc_set);

/**
 * @brief Debugfs lookup stats show API
 */
void cls_dbg_lu_stats_show(struct seq_file *f)
{
	struct cls_lu_thrd_stats stats;
	u32 thrd, num_threads = 0;

	if (unlikely(cls_num_lu_thread_get(&num_threads))) {
		pr_err("Failed to get number of threads\n");
		return;
	}

	seq_puts(f, "\n");
	seq_puts(f, " +--------+----------------+----------------+----------------+------------------------+\n");
	seq_puts(f, " | Thread | Lookup Request |  Lookup Match  |  Lookup long   |        State           |\n");
	seq_puts(f, " +--------+----------------+----------------+----------------+------------------------+\n");

	for (thrd = 0; thrd < num_threads; thrd++) {
		if (unlikely(cls_lu_thrd_stats_get(thrd, &stats))) {
			pr_err("failed to get lu stats\n");
			return;
		}
		seq_printf(f, " |%-8u|%-16u|%-16u|%-16u| %-15s (0x%2x) |\n",
			   thrd, stats.request, stats.match,
			   stats.long_lu,
			   stats.state > 14 ? "Err" : cls_stm_str[stats.state],
			   stats.state);
		seq_puts(f, " +--------+----------------+----------------+----------------+------------------------+\n");
	}
	seq_puts(f, "\n");
}

/**
 * @brief Reset lookup counters using debugfs file
 */
void cls_dbg_lu_stats_reset(char *cmd_buf, void *data)
{
	bool reset;

	if (strtobool(cmd_buf, &reset)) {
		pr_err("strtobool error\n");
		return;
	}

	if (reset) {
		pr_err("failed to reset lookup stats, write 0 for reset\n");
		return;
	}

	cls_lu_stats_reset();
}

PP_DEFINE_DEBUGFS(lu_stats, cls_dbg_lu_stats_show, cls_dbg_lu_stats_reset);

/**
 * @brief Set lookup long counter threshold
 * @param data pointer to the date to set
 * @param val value to set
 * @return 0 on success
 */
static int cls_dbg_long_thr_set(void *data, u64 val)
{
	cls_long_lu_thr_set((u32)val);

	return 0;
}

static int cls_dbg_long_thr_get(void *data, u64 *val)
{
	int ret;
	u32 thr;

	ret = cls_long_lu_thr_get(&thr);
	if (unlikely(ret)) {
		pr_err("failed to get the long lookup threshold\n");
		return ret;
	}

	*val = (u64)thr;

	return 0;
}

PP_DEFINE_DBGFS_ATT(cls_dbg_long_thr, cls_dbg_long_thr_get,
		    cls_dbg_long_thr_set);

/**
 * @brief Set the number of lookup threads
 * @param data pointer to the date to set
 * @param val value to set
 * @return 0 on success
 */
static int cls_dbg_threads_set(void *data, u64 val)
{
	int ret;

	ret = cls_num_lu_thread_set((u32)val);
	if (unlikely(ret)) {
		pr_err("failed to set the number of lookup threads\n");
		return ret;
	}

	return 0;
}

static int cls_dbg_threads_get(void *data, u64 *val)
{
	int ret;
	u32 threads;

	ret = cls_num_lu_thread_get(&threads);
	if (unlikely(ret)) {
		pr_err("failed to get the number of lookup threads\n");
		return ret;
	}

	*val = (u64)threads;

	return 0;
}

PP_DEFINE_DBGFS_ATT(cls_dbg_threads, cls_dbg_threads_get,
		    cls_dbg_threads_set);

/**
 * @brief Set the lookup thread drop flow mode
 * @param data pointer to the date to set
 * @param val value to set
 * @return 0 on success
 */
static int cls_dbg_drop_flow_set(void *data, u64 val)
{
	int ret;
	bool drop_flow_dis, search_mode, fm_flow, fv_masked;

	ret = cls_thrd_cfg_get(&drop_flow_dis, &search_mode,
			       &fm_flow, &fv_masked);
	if (unlikely(ret)) {
		pr_err("failed to get the lookup thread config\n");
		return ret;
	}

	cls_thrd_cfg_set(!val, search_mode, fm_flow, fv_masked);

	return 0;
}

static int cls_dbg_drop_flow_get(void *data, u64 *val)
{
	int ret;
	bool drop_flow_dis, search_mode, fm_flow, fv_masked;

	ret = cls_thrd_cfg_get(&drop_flow_dis, &search_mode,
			       &fm_flow, &fv_masked);
	if (unlikely(ret)) {
		pr_err("failed to get the lookup thread config\n");
		return ret;
	}

	*val = (u64)!drop_flow_dis;

	return 0;
}

PP_DEFINE_DBGFS_ATT(cls_dbg_drop_flow, cls_dbg_drop_flow_get,
		    cls_dbg_drop_flow_set);

/**
 * @brief Set the lookup thread drop flow mode
 * @param data pointer to the date to set
 * @param val value to set
 * @return 0 on success
 */
static int cls_dbg_search_mode_set(void *data, u64 val)
{
	int ret;
	bool drop_flow_dis, search_mode, fm_flow, fv_masked;

	ret = cls_thrd_cfg_get(&drop_flow_dis, &search_mode,
			       &fm_flow, &fv_masked);
	if (unlikely(ret)) {
		pr_err("failed to get the lookup thread config\n");
		return ret;
	}

	cls_thrd_cfg_set(drop_flow_dis, !!val, fm_flow, fv_masked);

	return 0;
}

static int cls_dbg_search_mode_get(void *data, u64 *val)
{
	int ret;
	bool drop_flow_dis, search_mode, fm_flow, fv_masked;

	ret = cls_thrd_cfg_get(&drop_flow_dis, &search_mode,
			       &fm_flow, &fv_masked);
	if (unlikely(ret)) {
		pr_err("failed to get the lookup thread config\n");
		return ret;
	}

	*val = (u64)search_mode;

	return 0;
}

PP_DEFINE_DBGFS_ATT(cls_dbg_search_mode, cls_dbg_search_mode_get,
		    cls_dbg_search_mode_set);

/**
 * @brief Set the lookup thread fm flow mode
 * @param data pointer to the date to set
 * @param val value to set
 * @return 0 on success
 */
static int cls_dbg_fm_flow_set(void *data, u64 val)
{
	int ret;
	bool drop_flow_dis, search_mode, fm_flow, fv_masked;

	ret = cls_thrd_cfg_get(&drop_flow_dis, &search_mode,
			       &fm_flow, &fv_masked);
	if (unlikely(ret)) {
		pr_err("failed to get the lookup thread config\n");
		return ret;
	}

	cls_thrd_cfg_set(drop_flow_dis, search_mode, !!val, fv_masked);

	return 0;
}

static int cls_dbg_fm_flow_get(void *data, u64 *val)
{
	int ret;
	bool drop_flow_dis, search_mode, fm_flow, fv_masked;

	ret = cls_thrd_cfg_get(&drop_flow_dis, &search_mode,
			       &fm_flow, &fv_masked);
	if (unlikely(ret)) {
		pr_err("failed to get the lookup thread config\n");
		return ret;
	}

	*val = (u64)fm_flow;

	return 0;
}

PP_DEFINE_DBGFS_ATT(cls_dbg_fm_flow, cls_dbg_fm_flow_get,
		    cls_dbg_fm_flow_set);

/**
 * @brief Set whether to get the field vector masked or unmasked
 * @param data pointer to the date to set
 * @param val value to set
 * @return 0 on success
 */
static int cls_dbg_fv_masked_set(void *data, u64 val)
{
	int ret;
	bool drop_flow_dis, search_mode, fm_flow, fv_masked;

	ret = cls_thrd_cfg_get(&drop_flow_dis, &search_mode,
			       &fm_flow, &fv_masked);
	if (unlikely(ret)) {
		pr_err("failed to get the lookup thread config\n");
		return ret;
	}

	cls_thrd_cfg_set(drop_flow_dis, search_mode, fm_flow, !!val);

	return 0;
}

static int cls_dbg_fv_masked_get(void *data, u64 *val)
{
	int ret;
	bool drop_flow_dis, search_mode, fm_flow, fv_masked;

	ret = cls_thrd_cfg_get(&drop_flow_dis, &search_mode,
			       &fm_flow, &fv_masked);
	if (unlikely(ret)) {
		pr_err("failed to get the lookup thread config\n");
		return ret;
	}

	*val = (u64)fv_masked;

	return 0;
}

PP_DEFINE_DBGFS_ATT(cls_dbg_fv_masked, cls_dbg_fv_masked_get,
		    cls_dbg_fv_masked_set);

/**
 * @brief Set the signature cache mode
 * @param data pointer to the date to set
 * @param val value to set
 * @return 0 on success
 */
static int cls_dbg_cache_enable_set(void *data, u64 val)
{
	cls_sig_cache_cfg_set(!!val, true);

	return 0;
}

static int cls_dbg_cache_enable_get(void *data, u64 *val)
{
	int ret;
	bool enable;

	ret = cls_sig_cache_cfg_get(&enable);
	if (unlikely(ret)) {
		pr_err("failed to get the lookup thread config\n");
		return ret;
	}

	*val = (u64)enable;

	return 0;
}

PP_DEFINE_DBGFS_ATT(cls_dbg_cache_enable, cls_dbg_cache_enable_get,
		    cls_dbg_cache_enable_set);

/**
 * @brief Invalidate the signature cache
 * @param data pointer to the date to set
 * @param val value to set
 * @return 0 on success
 */
static int cls_dbg_cache_invalidate_set(void *data, u64 val)
{
	cls_sig_cache_cfg_set(true, !!val);

	return 0;
}

PP_DEFINE_DBGFS_ATT(cls_dbg_cache_invalidate, NULL,
		    cls_dbg_cache_invalidate_set);

static int cls_dbg_ver_get(void *data, u64 *val)
{
	int ret;
	struct pp_version ver;

	ret = cls_hw_ver_get(&ver);
	if (unlikely(ret)) {
		pr_err("failed to get the lookup thread config\n");
		return ret;
	}

	*val = (u64)ver.major;

	return 0;
}

PP_DEFINE_DBGFS_ATT(cls_dbg_ver_show, cls_dbg_ver_get, NULL);

/**
 * @brief Set the classifier polling num retries
 * @param data unused
 * @param val value to set
 * @return 0 on success
 */
static int cls_dbg_n_retries_set(void *data, u64 val)
{
	cls_n_retries_set(val);
	return 0;
}

/**
 * @brief Get the classifier polling num retries
 * @param data unused
 * @param val value to set
 * @return 0 on success
 */
static int cls_dbg_n_retries_get(void *data, u64 *val)
{
	u32 n = 0;

	cls_n_retries_get(&n);
	*val = (u64)n;
	return 0;
}

PP_DEFINE_DBGFS_ATT(cls_dbg_n_retries, cls_dbg_n_retries_get,
		    cls_dbg_n_retries_set);

/**
 * @brief Set the signature cache mode
 * @param data pointer to the date to set
 * @param val value to set
 * @return 0 on success
 */
static int cls_dbg_cache_long_trans_thr_set(void *data, u64 val)
{
	cls_sig_cache_long_trans_cntr_thr_set(val);

	return 0;
}

static int cls_dbg_cache_long_trans_thr_get(void *data, u64 *val)
{
	int ret;
	u64 thr;

	ret = cls_sig_cache_long_trans_cntr_thr_get(&thr);
	if (unlikely(ret)) {
		pr_err("failed to get the lookup thread config\n");
		return ret;
	}

	*val = thr;

	return 0;
}

PP_DEFINE_DBGFS_ATT(cls_dbg_cache_long_trans_thr,
		    cls_dbg_cache_long_trans_thr_get,
		    cls_dbg_cache_long_trans_thr_set);

/**
 * @brief Debugfs signature cache stats show API
 */
void cls_dbg_sig_cache_stats_show(struct seq_file *f)
{
	u32 c1_hit, c2_hit, c1_long_trns, c2_long_trns;

	if (unlikely(cls_sig_cache_stats_get(CLS_SIG_CACHE1, &c1_hit,
					     &c1_long_trns))) {
		pr_err("failed to get signature cache1 stats\n");
		return;
	}

	if (unlikely(cls_sig_cache_stats_get(CLS_SIG_CACHE2, &c2_hit,
					     &c2_long_trns))) {
		pr_err("failed to get signature cache2 stats\n");
		return;
	}

	seq_puts(f, "\n");
	seq_puts(f, " +--------+------------+------------+\n");
	seq_puts(f, " |        | CACHE HIT  | LONG TRANS |\n");
	seq_puts(f, " +--------+------------+------------+\n");
	seq_printf(f, " | CACHE1 | %-10u | %-10u |\n", c1_hit, c1_long_trns);
	seq_puts(f, " +--------+------------+------------+\n");
	seq_printf(f, " | CACHE2 | %-10u | %-10u |\n", c2_hit, c2_long_trns);
	seq_puts(f, " +--------+------------+------------+\n");
}

PP_DEFINE_DEBUGFS(sig_cache_stats_show, cls_dbg_sig_cache_stats_show, NULL);

/**
 * @brief Debugfs session cache stats show API
 */
static void cls_dbg_sess_cache_stats_show(struct seq_file *f)
{
	u64 trans, hits;

	if (unlikely(cls_sess_cache_stats_get(&trans, &hits))) {
		pr_err("failed to get session cache stats\n");
		return;
	}

	seq_puts(f, "\n");
	seq_puts(f, "   Session Cache Stats\n");
	seq_puts(f, " +---------------------+\n");
	seq_printf(f, "   Transactions: %llu\n", trans);
	seq_printf(f, "   Hits        : %llu\n", hits);
	seq_puts(f, "\n");
}

PP_DEFINE_DEBUGFS(sess_cache_stats_show, cls_dbg_sess_cache_stats_show, NULL);

static s32 __cls_sess_cache_bypass_get(void *data, u64 *val)
{
	bool en;

	en = cls_sess_cache_bypass_get();
	*val = (u64)en;
	return 0;
}

static s32 __cls_sess_cache_bypass_set(void *data, u64 val)
{
	cls_sess_cache_bypass_set(!!val);
	return 0;
}

PP_DEFINE_DBGFS_ATT(sess_cache_bypass, __cls_sess_cache_bypass_get,
		    __cls_sess_cache_bypass_set);

/**
 * @brief Debugfs static session delete API
 */
void cls_dbg_static_sess_del(struct seq_file *f)
{
	struct pp_hash hash;

	if (unlikely(cls_dbg_hash_get(&hash))) {
		pr_err("failed to get last hash\n");
		return;
	}

	if (cls_session_del(0, CLS_DBG_FV_NUM_REGS * sizeof(u32), &hash))
		pr_err("failed to delete static session\n");
	else
		seq_puts(f, "static session was deleted\n");
}

PP_DEFINE_DEBUGFS(static_sess_del, cls_dbg_static_sess_del, NULL);

/**
 * @brief Debugfs static session lookup API
 */
void cls_dbg_static_sess_lookup(struct seq_file *f)
{
	u32 sess_id, fv[CLS_DBG_FV_NUM_REGS] = { 0 };
	struct pp_hash hash;

	if (unlikely(cls_dbg_hash_get(&hash))) {
		pr_err("failed to get last hash\n");
		return;
	}

	if (unlikely(cls_dbg_last_fv_get(&fv[0]))) {
		pr_err("failed to get last FV\n");
		return;
	}

	if (unlikely(cls_session_lookup(&fv[0],
					CLS_DBG_FV_NUM_REGS * sizeof(u32),
					&hash, &sess_id))) {
		pr_err("failed to lookup static session\n");
		return;
	}

	if (sess_id == CLS_SESSION_INVALID)
		seq_puts(f, "static session not found\n");
	else
		seq_printf(f, "static session id is %u\n", sess_id);
}

PP_DEFINE_DEBUGFS(static_sess_lookup, cls_dbg_static_sess_lookup, NULL);

/**
 * @brief Debugfs static bridge session add API
 */
static void cls_dbg_bridge_sess_create(char *cmd_buf, void *data)
{
	u16 pid, dst_q;
	struct pp_hw_si si;
	struct pp_hash hash;
	s32 ret = 0;

	if (unlikely(sscanf(cmd_buf, "%hu %hu", &pid, &dst_q) != 2)) {
		pr_err("sscanf error\n");
		return;
	}

	memset(&si, 0, sizeof(si));
	/* Build static SI */
	ret |= pp_si_fld_set(&si, SI_FLD_RECIPE_ID, 0xFF);
	ret |= pp_si_fld_set(&si, SI_FLD_SESS_ID,   0);
	ret |= pp_si_fld_set(&si, SI_FLD_PLCY_BASE, 0);
	ret |= pp_si_fld_set(&si, SI_FLD_FV_SZ,     sizeof(struct cls_dbg_fv));
	ret |= pp_si_fld_set(&si, SI_FLD_EGRS_PORT, pid);
	ret |= pp_si_fld_set(&si, SI_FLD_DST_QUEUE, dst_q);
	ret |= pp_si_fld_set(&si, SI_FLD_COLOR,     PP_COLOR_GREEN);
	ret |= pp_si_fld_set(&si, SI_FLD_CHKR_FLAGS,
			     BIT(SI_CHCK_FLAG_DYN_SESS) |
			     BIT(SI_CHCK_FLAG_UPDT_SESS_CNT));
	ret |= pp_si_fld_set(&si, SI_FLD_TRIM_L3_OFF_ID, SI_NO_TRIM_VALUE);
	ret |= pp_si_fld_set(&si, SI_FLD_PLICIES_BITMAP, 1);
	if (unlikely(ret)) {
		pr_err("failed to set si\n");
		return;
	}

	if (unlikely(cls_dbg_hash_get(&hash))) {
		pr_err("failed to get last hash\n");
		return;
	}

	if (unlikely(cls_dbg_last_fv_get(&si.word[CLS_DBG_FV_START_WORD]))) {
		pr_err("failed to get last FV\n");
		return;
	}

	if (cls_session_add(&si.word[0], &hash))
		pr_err("failed to add static session\n");
	else
		pr_info("bridge static session added\n");
}

static void cls_dbg_bridge_sess_help(struct seq_file *f)
{
	seq_puts(f, " <pid> <dst-phys-queue>\n");
}

PP_DEFINE_DEBUGFS(bridge_sess_create, cls_dbg_bridge_sess_help,
		  cls_dbg_bridge_sess_create);

/**
 * @brief Debugfs static session add API
 */
void cls_dbg_l2_sess_create(struct seq_file *f)
{
	struct pp_hw_si si;
	struct pp_hash hash;

	memset(&si, 0, sizeof(si));

	/* Session 0, header replace recipe */
	si.word[0]  = MOD_RCP_HDR_RPLC;
	/* 0x04 dpu start 0x30, fv size 128, bce start 0x40, bce size 16 */
	si.word[1]  = 0x87410000;
	/* 0x08 color green, Q 0, port 0 */
	si.word[2]  = 0x40000000;
	/* L3 trim off 0x7 (INVALID), pkt diff 0 (no changes) */
	si.word[3]  = 0xE8040000;
	/* 0x1 policy bmap 1 */
	si.word[7]  = 0x01000000;
	/* 0x30 dpu: hdr size 14 */
	si.word[12] = 0x000E0000;
	/* 0x40 bce: new header - dst mac */
	si.word[16] = 0x44332211;
	/* 0x44 bce: new header - dst mac + src mac */
	si.word[17] = 0x88776655;
	/* 0x48 bce: new header - src mac */
	si.word[18] = 0xCCBBAA99;
	/* 0x4c bce: new header - next proto */
	si.word[19] = 0x00000008;

	if (unlikely(cls_dbg_hash_get(&hash))) {
		pr_err("failed to get last hash\n");
		return;
	}

	if (unlikely(cls_dbg_last_fv_get(&si.word[CLS_DBG_FV_START_WORD]))) {
		pr_err("failed to get last FV\n");
		return;
	}

	if (cls_session_add(&si.word[0], &hash))
		pr_err("failed to add static session\n");
	else
		seq_puts(f, "add static session to classifier\n");
}

PP_DEFINE_DEBUGFS(l2_sess_create, cls_dbg_l2_sess_create, NULL);

/**
 * @brief Debugfs static session add API
 */
void cls_dbg_vlan_add_sess_create(struct seq_file *f)
{
	struct pp_hw_si si;
	struct pp_hash hash;

	memset(&si, 0, sizeof(si));

	/* Session 0, header replace recipe */
	si.word[0]  = MOD_RCP_HDR_RPLC;
	/* 0x04 dpu start 0x30, fv size 128, bce start 0x40, bce size 32 */
	si.word[1]  = 0x87420000;
	/* 0x08 color green, Q 0, port 0 */
	si.word[2]  = 0x40000000;
	/* L3 trim off 0x7 (INVALID), pkt diff +4 (+vlan) */
	//si.word[3]  = 0x08040012;
	si.word[3]  = 0xE8040004;
	/* 0x1 policy bmap 1 */
	si.word[7]  = 0x01000000;
	/* 0x30 dpu: hdr size 18 */
	si.word[12] = 0x00120000;
	/* 0x40 bce: new header - dst mac */
	si.word[16] = 0x44332211;
	/* 0x44 bce: new header - dst mac + src mac */
	si.word[17] = 0x88776655;
	/* 0x48 bce: new header - src mac */
	si.word[18] = 0xCCBBAA99;
	/* 0x4c bce: new header - vlan type + vlan id */
	si.word[19] = 0x02000081;
	/* 0x50 bce: new header - next proto */
	si.word[20] = 0x00000008;

	if (unlikely(cls_dbg_hash_get(&hash))) {
		pr_err("failed to get last hash\n");
		return;
	}

	if (unlikely(cls_dbg_last_fv_get(&si.word[CLS_DBG_FV_START_WORD]))) {
		pr_err("failed to get last FV\n");
		return;
	}

	if (cls_session_add(&si.word[0], &hash))
		pr_err("failed to add static session\n");
	else
		seq_puts(f, "add static session to classifier\n");
}

PP_DEFINE_DEBUGFS(vlan_add_sess_create, cls_dbg_vlan_add_sess_create, NULL);

/**
 * @brief Debugfs static session add API
 */
void cls_dbg_vlan_rmv_sess_create(struct seq_file *f)
{
	struct pp_hw_si si;
	struct pp_hash hash;

	memset(&si, 0, sizeof(si));

	/* Session 0, header replace recipe */
	si.word[0]  = MOD_RCP_HDR_RPLC;
	/* 0x04 dpu start 0x30, fv size 128, bce start 0x40, bce size 32 */
	si.word[1]  = 0x87420000;
	/* 0x08 color green, Q 0, port 0 */
	si.word[2]  = 0x40000000;
	/* L3 trim off 0x7 (INVALID), pkt diff -4 (-vlan) */
	//si.word[3]  = 0x08040012;
	si.word[3]  = 0xE80401FC;
	/* 0x1 policy bmap 1 */
	si.word[7]  = 0x01000000;
	/* 0x30 dpu: hdr size 18 */
	si.word[12] = 0x000E0000;
	/* 0x40 bce: new header - dst mac */
	si.word[16] = 0x44332211;
	/* 0x44 bce: new header - dst mac + src mac */
	si.word[17] = 0x88776655;
	/* 0x48 bce: new header - src mac */
	si.word[18] = 0xCCBBAA99;
	/* 0x4c bce: new header - next proto */
	si.word[19] = 0x00000008;

	if (unlikely(cls_dbg_hash_get(&hash))) {
		pr_err("failed to get last hash\n");
		return;
	}

	if (unlikely(cls_dbg_last_fv_get(&si.word[CLS_DBG_FV_START_WORD]))) {
		pr_err("failed to get last FV\n");
		return;
	}

	if (cls_session_add(&si.word[0], &hash))
		pr_err("failed to add static session\n");
	else
		seq_puts(f, "add static session to classifier\n");
}

PP_DEFINE_DEBUGFS(vlan_rmv_sess_create, cls_dbg_vlan_rmv_sess_create, NULL);

/**
 * @brief Debugfs static session add API - this session is
 *        replacing the L2 and doing nat on the L3 ipv4, replace
 *        ip address, tos and update checksum
 */
void cls_dbg_ipv6_routed_sess_create(struct seq_file *f)
{
	struct pp_hw_si si;
	struct pp_hash hash;

	memset(&si, 0, sizeof(si));

	/* Session 0, IPv6 recipe */
	si.word[0]  = MOD_RCP_IPV6_ROUTED;
	/* dpu start 0x30, sce start 0x40, fv size 128, bce start 0x50,
	 * bce size 32
	 */
	si.word[1]  = 0x9F520000;
	/* 0x08 color green, Q 0, port 0 */
	si.word[2]  = 0x40000000;
	/* L3 trim off 0x7 (INVALID), pkt diff 0 (no changes) */
	si.word[3]  = 0xE8040000;
	/* 0x1 policy bmap 1 */
	si.word[7]  = 0x01000000;

	/* 0x30 dpu: nat size 8, hdr size 14 */
	si.word[12] = 0x000E0000;

	/* 0x50 bce: new header - dst mac */
	si.word[20] = 0x44332211;
	/* 0x54 bce: new header - dst mac + src mac */
	si.word[21] = 0x88776655;
	/* 0x58 bce: new header - src mac */
	si.word[22] = 0xCCBBAA99;
	/* 0x5C bce: new header - next proto */
	si.word[23] = 0x0000DD86;

	if (unlikely(cls_dbg_hash_get(&hash))) {
		pr_err("failed to get last hash\n");
		return;
	}

	if (unlikely(cls_dbg_last_fv_get(&si.word[CLS_DBG_FV_START_WORD]))) {
		pr_err("failed to get last FV\n");
		return;
	}

	if (cls_session_add(&si.word[0], &hash))
		pr_err("failed to add static session\n");
	else
		seq_puts(f, "add static session to classifier\n");
}

PP_DEFINE_DEBUGFS(ipv6_routed_sess_create, cls_dbg_ipv6_routed_sess_create,
		  NULL);

/**
 * @brief Debugfs static session add API - this session is
 *        replacing the L2 and doing nat on the L3 ipv4, replace
 *        ip address, tos and update checksum
 */
void cls_dbg_v4_nat_sess_create(struct seq_file *f)
{
	struct pp_hw_si si;
	struct pp_hash hash;

	memset(&si, 0, sizeof(si));

	/* Session 0, IPv4 routed recipe */
	si.word[0]  = MOD_RCP_IPV4_NAT;
	/* dpu start 0x30, sce start 0x40, fv size 128, bce start 0x50,
	 * bce size 48
	 */
	si.word[1]  = 0x9F530000;
	/* 0x08 color green, Q 0, port 0 */
	si.word[2]  = 0x40000000;
	/* L3 trim off 0x7 (INVALID), pkt diff 0 (no changes) */
	si.word[3]  = 0xE8040000;
	/* 0x1 policy bmap 1 */
	si.word[7]  = 0x01000000;

	/* 0x30 dpu: nat size 8, hdr size 14 */
	si.word[12] = 0x080E0000;

	/* 0x40 sce: csum delta */
	si.word[16] = 0x12340000;
	/* 0x44 sce: new tos */
	si.word[17] = 0x00002A00;

	/* 0x50 bce: nat - dst ip */
	si.word[20] = 0xAAAAAAAA;
	/* 0x54 bce: nat - dst ip */
	si.word[21] = 0xBBBBBBBB;
	/* 0x58 bce: nat - reserve */
	si.word[22] = 0x00000000;
	/* 0x5C bce: nat - reserve */
	si.word[23] = 0x00000000;

	/* 0x60 bce: new header - dst mac */
	si.word[24] = 0x44332211;
	/* 0x64 bce: new header - dst mac + src mac */
	si.word[25] = 0x88776655;
	/* 0x68 bce: new header - src mac */
	si.word[26] = 0xCCBBAA99;
	/* 0x6C bce: new header - next proto */
	si.word[27] = 0x00000008;

	if (unlikely(cls_dbg_hash_get(&hash))) {
		pr_err("failed to get last hash\n");
		return;
	}

	if (unlikely(cls_dbg_last_fv_get(&si.word[CLS_DBG_FV_START_WORD]))) {
		pr_err("failed to get last FV\n");
		return;
	}

	if (cls_session_add(&si.word[0], &hash))
		pr_err("failed to add static session\n");
	else
		seq_puts(f, "add static session to classifier\n");
}

PP_DEFINE_DEBUGFS(v4_nat_sess_create, cls_dbg_v4_nat_sess_create, NULL);

/**
 * @brief Debugfs static session add API - this session is
 *        replacing the L2 with new gre header
 */
void cls_dbg_gre_v4_v4_us_sess_create(struct seq_file *f)
{
	struct pp_hw_si si;
	struct pp_hash hash;

	memset(&si, 0, sizeof(si));

	/* Session 0, eogre encap ipv4 ipv4 recipe */
	si.word[0]  = MOD_RCP_IPV4_IPV4_ENCAP;
	/* 0x04 dpu start 0x30, fv size 128, bce start 0x40, bce size 64 */
	si.word[1]  = 0x57440000;
	/* 0x08 color green, Q 0, port 0 */
	si.word[2]  = 0x40000000;
	/* L3 trim off 0x7 (INVALID), pkt diff +38 */
	//si.word[3]  = 0x08040012;
	si.word[3]  = 0xE8040026;
	/* 0x1 policy bmap 1 */
	si.word[7]  = 0x01000000;

	/* 0x20 dpu: hdr size 18 */
	si.word[8] = 0x00340000;
	si.word[9] = 0x0000000E;

	/* 0x30 sce */
	si.word[12] = 0x00000000;
	/* 0x34 sce: hdr csum 0x1234, total_len_diff 0x18 (24) */
	si.word[13] = 0x12340018;
	/* 0x38 sce */
	si.word[14] = 0x00000000;

	/* 0x40 bce: new header */
	si.word[16] = htonl(0x11223344);
	si.word[17] = htonl(0x55667788);
	si.word[18] = htonl(0x99AABBCC);
	si.word[19] = htonl(0x08004500);
	si.word[20] = htonl(0x003c0000);
	si.word[21] = htonl(0x4000402f);
	si.word[22] = htonl(0xb126c0a8);
	si.word[23] = htonl(0x0420c0a8);
	si.word[24] = htonl(0x041a0000);
	si.word[25] = htonl(0x65581855);
	si.word[26] = htonl(0x73c90491);
	si.word[27] = htonl(0xd6b29f2f);
	si.word[28] = htonl(0x555d0800);

	if (unlikely(cls_dbg_hash_get(&hash))) {
		pr_err("failed to get last hash\n");
		return;
	}

	if (unlikely(cls_dbg_last_fv_get(&si.word[CLS_DBG_FV_START_WORD]))) {
		pr_err("failed to get last FV\n");
		return;
	}

	if (cls_session_add(&si.word[0], &hash))
		pr_err("failed to add static session\n");
	else
		seq_puts(f, "add static session to classifier\n");
}

PP_DEFINE_DEBUGFS(gre_v4_v4_us_sess_create, cls_dbg_gre_v4_v4_us_sess_create,
		  NULL);

/**
 * @brief Debugfs static session add API - this session is
 *        replacing the gre header with L2 header
 */
void cls_dbg_gre_v4_v4_ds_sess_create(struct seq_file *f)
{
	struct pp_hw_si si;
	struct pp_hash hash;

	memset(&si, 0, sizeof(si));

	/* Session 0, eogre decap */
	si.word[0]  = MOD_RCP_EOGRE_DECP_IPV4_NAPT;
	/* dpu start 0x30, sce start 0x40, fv size 128, bce start 0x50,
	 * bce size 48
	 */
	si.word[1]  = 0x9F530000;
	/* 0x08 color green, Q 0, port 0 */
	si.word[2]  = 0x40000000;
	/* L3 trim off 0x7 (INVALID), pkt diff -38 */
	si.word[3]  = 0xE80401DA;
	/* 0x1 policy bmap 1 */
	si.word[7]  = 0x01000000;
	/* 0x30 dpu: nat size 8, hdr size 14 */
	si.word[12] = 0x080E0000;
	/* 0x34 dpu: lyr_fld_off 6 for udp */
	si.word[13] = 0x00000600;

	/* 0x40 sce: csum delta */
	si.word[16] = 0x12340000;
	/* 0x44 sce: new tos */
	si.word[17] = 0x00002A00;

	/* 0x50 bce: nat - dst ip */
	si.word[20] = 0xAABBCCDD;
	/* 0x54 bce: nat - dst ip */
	si.word[21] = 0x11223344;
	/* 0x58 bce: nat - reserve */
	si.word[22] = 0x00000000;
	/* 0x5C bce: nat - reserve */
	si.word[23] = 0x00000000;

	/* 0x60 bce: new header - dst mac */
	si.word[24] = htonl(0x11223344);
	si.word[25] = htonl(0x55667788);
	si.word[26] = htonl(0x99AABBCC);
	si.word[27] = htonl(0x08000000);

	if (unlikely(cls_dbg_hash_get(&hash))) {
		pr_err("failed to get last hash\n");
		return;
	}

	if (unlikely(cls_dbg_last_fv_get(&si.word[CLS_DBG_FV_START_WORD]))) {
		pr_err("failed to get last FV\n");
		return;
	}

	if (cls_session_add(&si.word[0], &hash))
		pr_err("failed to add static session\n");
	else
		seq_puts(f, "add static session to classifier\n");
}

PP_DEFINE_DEBUGFS(gre_v4_v4_ds_sess_create, cls_dbg_gre_v4_v4_ds_sess_create,
		  NULL);

/**
 * @brief Debugfs static session add API - this session is
 *        replacing the L2 and doing nat on the L3 ipv4,
 *        updating the udp csum, replace ip address, tos and
 *        update checksum
 */
void cls_dbg_tcp_v4_nat_sess_create(struct seq_file *f)
{
	struct pp_hw_si si;
	struct pp_hash hash;

	memset(&si, 0, sizeof(si));

	/* Session 0, IPv4 routed tcp nat recipe */
	si.word[0]  = MOD_RCP_IPV4_NAPT;
	/* dpu start 0x30, sce start 0x40, fv size 128, bce start 0x50,
	 * bce size 48
	 */
	si.word[1]  = 0x9F530000;
	/* 0x08 color green, Q 0, port 0 */
	si.word[2]  = 0x40000000;
	/* L3 trim off 0x7 (INVALID), pkt diff 0 (no changes) */
	si.word[3]  = 0xE8040000;
	/* 0x1 policy bmap 1 */
	si.word[7]  = 0x01000000;

	/* 0x30 dpu: nat size 8, hdr size 14 */
	si.word[12] = 0x080E0000;
	/* 0x34 dpu: lyr_fld_off 16 */
	si.word[13] = 0x00001000;

	/* 0x40 sce: csum delta */
	si.word[16] = 0x12345678;
	/* 0x44 sce: new tos */
	si.word[17] = 0x00002A00;
	/* 0x48 sce: new udp ports */
	si.word[18] = 0x12345678;

	/* 0x50 bce: nat - dst ip */
	si.word[20] = 0xAAAAAAAA;
	/* 0x54 bce: nat - dst ip */
	si.word[21] = 0xBBBBBBBB;
	/* 0x58 bce: nat - reserve */
	si.word[22] = 0x00000000;
	/* 0x5C bce: nat - reserve */
	si.word[23] = 0x00000000;

	/* 0x60 bce: new header - dst mac */
	si.word[24] = 0x44332211;
	/* 0x64 bce: new header - dst mac + src mac */
	si.word[25] = 0x88776655;
	/* 0x68 bce: new header - src mac */
	si.word[26] = 0xCCBBAA99;
	/* 0x6C bce: new header - next proto */
	si.word[27] = 0x00000008;

	if (unlikely(cls_dbg_hash_get(&hash))) {
		pr_err("failed to get last hash\n");
		return;
	}

	if (unlikely(cls_dbg_last_fv_get(&si.word[CLS_DBG_FV_START_WORD]))) {
		pr_err("failed to get last FV\n");
		return;
	}

	if (cls_session_add(&si.word[0], &hash))
		pr_err("failed to add static session\n");
	else
		seq_puts(f, "add static session to classifier\n");
}

PP_DEFINE_DEBUGFS(tcp_v4_nat_sess_create, cls_dbg_tcp_v4_nat_sess_create, NULL);

/**
 * @brief Debugfs static session add API - this session is
 *        replacing the L2 and doing nat on the L3 ipv4,
 *        updating the udp csum, replace ip address, tos and
 *        update checksum
 */
void cls_dbg_udp_v4_nat_sess_create(struct seq_file *f)
{
	struct pp_hw_si si;
	struct pp_hash hash;

	memset(&si, 0, sizeof(si));

	/* Session 0, IPv4 routed udp nat recipe */
	si.word[0]  = MOD_RCP_IPV4_NAPT;
	/* dpu start 0x30, sce start 0x40, fv size 128, bce start 0x50,
	 * bce size 48
	 */
	si.word[1]  = 0x9F530000;
	/* 0x08 color green, Q 0, port 0 */
	si.word[2]  = 0x40000000;
	/* L3 trim off 0x7 (INVALID), pkt diff 0 (no changes) */
	si.word[3]  = 0xE8040000;
	/* 0x1 policy bmap 1 */
	si.word[7]  = 0x01000000;

	/* 0x30 dpu: nat size 8, hdr size 14 */
	si.word[12] = 0x080E0000;
	/* 0x34 dpu: lyr_fld_off 6 */
	si.word[13] = 0x00000600;

	/* 0x40 sce: csum delta */
	si.word[16] = 0x12345678;
	/* 0x44 sce: new tos */
	si.word[17] = 0x00002A00;
	/* 0x48 sce: new udp ports */
	si.word[18] = 0x12345678;

	/* 0x50 bce: nat - dst ip */
	si.word[20] = 0xAAAAAAAA;
	/* 0x54 bce: nat - dst ip */
	si.word[21] = 0xBBBBBBBB;
	/* 0x58 bce: nat - reserve */
	si.word[22] = 0x00000000;
	/* 0x5C bce: nat - reserve */
	si.word[23] = 0x00000000;

	/* 0x60 bce: new header - dst mac */
	si.word[24] = 0x44332211;
	/* 0x64 bce: new header - dst mac + src mac */
	si.word[25] = 0x88776655;
	/* 0x68 bce: new header - src mac */
	si.word[26] = 0xCCBBAA99;
	/* 0x6C bce: new header - next proto */
	si.word[27] = 0x00000008;

	if (unlikely(cls_dbg_hash_get(&hash))) {
		pr_err("failed to get last hash\n");
		return;
	}

	if (unlikely(cls_dbg_last_fv_get(&si.word[CLS_DBG_FV_START_WORD]))) {
		pr_err("failed to get last FV\n");
		return;
	}

	if (cls_session_add(&si.word[0], &hash))
		pr_err("failed to add static session\n");
	else
		seq_puts(f, "add static session to classifier\n");
}

PP_DEFINE_DEBUGFS(udp_v4_nat_sess_create, cls_dbg_udp_v4_nat_sess_create, NULL);

/**
 * @brief Dump status word
 */
void __cls_stw_show(struct seq_file *f)
{
	struct buf_fld_info *fld;
	struct cls_hw_stw hw_stw;
	u32 fld_val[STW_CLS_FLDS_NUM];
	u32 i;

	if (unlikely(cls_dbg_stw_get(&hw_stw)))
		return;

	seq_puts(f, " Classifier Status Word:\n");
	seq_puts(f, " =======================\n");
	for (i = 0 ; i < STW_CLS_FLDS_NUM ; i++) {
		fld = &cls_stw_flds[i];
		fld_val[i] = buf_fld_get(fld, hw_stw.word);

		if (i == STW_CLS_FLD_DROP_PKT)
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

PP_DEFINE_DEBUGFS(stw_show, __cls_stw_show, NULL);

static struct debugfs_file cls_root_files[] = {
	{ "version", &PP_DEBUGFS_FOPS(cls_dbg_ver_show) },
	{ "stw_show", &PP_DEBUGFS_FOPS(stw_show) },
	{ "poll_n_retries", &PP_DEBUGFS_FOPS(cls_dbg_n_retries) },
};

static struct debugfs_file session_dbg_files[] = {
	{ "create_gre_ipv4_us",
	  &PP_DEBUGFS_FOPS(gre_v4_v4_us_sess_create) },
	{ "create_gre_ipv4_ds",
	  &PP_DEBUGFS_FOPS(gre_v4_v4_ds_sess_create) },
	{ "create_ipv6_routed",
	  &PP_DEBUGFS_FOPS(ipv6_routed_sess_create) },
	{ "create_bridge", &PP_DEBUGFS_FOPS(bridge_sess_create) },
	{ "create_l2", &PP_DEBUGFS_FOPS(l2_sess_create) },
	{ "create_vlan_add", &PP_DEBUGFS_FOPS(vlan_add_sess_create) },
	{ "create_vlan_rmv", &PP_DEBUGFS_FOPS(vlan_rmv_sess_create) },
	{ "create_ipv4_nat", &PP_DEBUGFS_FOPS(v4_nat_sess_create) },
	{ "create_udp_v4_nat", &PP_DEBUGFS_FOPS(udp_v4_nat_sess_create) },
	{ "create_tcp_v4_nat", &PP_DEBUGFS_FOPS(tcp_v4_nat_sess_create) },
	{ "lookup", &PP_DEBUGFS_FOPS(static_sess_lookup) },
	{ "delete", &PP_DEBUGFS_FOPS(static_sess_del) },
};

static struct debugfs_file fv_dbg_files[] = {
	{ "last", &PP_DEBUGFS_FOPS(last_fv_show) },
	{ "gmask_show", &PP_DEBUGFS_FOPS(fv_mask) },
	{ "mask", &PP_DEBUGFS_FOPS(cls_dbg_fv_masked) },
	{ "in_swap", &PP_DEBUGFS_FOPS(cls_dbg_fv_in_swap) },
	{ "eg_swap", &PP_DEBUGFS_FOPS(cls_dbg_fv_eg_swap) },
};

static struct debugfs_file hash_dbg_files[] = {
	{ "last", &PP_DEBUGFS_FOPS(last_hash_show) },
	{ "cuckoo_cfg", &PP_DEBUGFS_FOPS(cuckoo_cfg_show) },
	{ "cuckoo_stats", &PP_DEBUGFS_FOPS(cuckoo_stats_show) },
	{ "sig_select", &PP_DEBUGFS_FOPS(cls_dbg_sig_sel) },
	{ "hash_select", &PP_DEBUGFS_FOPS(cls_dbg_hash_sel) },
	{ "size", &PP_DEBUGFS_FOPS(cls_dbg_hash_size) },
	{ "sig_err", &PP_DEBUGFS_FOPS(cls_dbg_sig_err) },
	{ "hash_calc", &PP_DEBUGFS_FOPS(cls_dbg_hash_calc) },
};

static struct debugfs_file lookup_dbg_files[] = {
	{ "stats", &PP_DEBUGFS_FOPS(lu_stats) },
	{ "long_threshold", &PP_DEBUGFS_FOPS(cls_dbg_long_thr) },
	{ "threads", &PP_DEBUGFS_FOPS(cls_dbg_threads) },
	{ "drop_flow", &PP_DEBUGFS_FOPS(cls_dbg_drop_flow) },
	{ "search_mode", &PP_DEBUGFS_FOPS(cls_dbg_search_mode) },
	{ "fm_mode", &PP_DEBUGFS_FOPS(cls_dbg_fm_flow) },
};

static struct debugfs_file sig_cache_dbg_files[] = {
	{ "stats", &PP_DEBUGFS_FOPS(sig_cache_stats_show) },
	{ "enable", &PP_DEBUGFS_FOPS(cls_dbg_cache_enable) },
	{ "invalidate", &PP_DEBUGFS_FOPS(cls_dbg_cache_invalidate) },
	{ "long_trans_thr",
	  &PP_DEBUGFS_FOPS(cls_dbg_cache_long_trans_thr) },
};

static struct debugfs_file sess_cache_files[] = {
	{ "stats", &PP_DEBUGFS_FOPS(sess_cache_stats_show) },
	{ "bypass", &PP_DEBUGFS_FOPS(sess_cache_bypass) }
};

s32 cls_dbg_init(struct dentry *parent)
{
	s32 ret;

	if (unlikely(!debugfs_initialized())) {
		pr_err("Debugfs not initialized yet\n");
		return -EPERM;
	}

	/* classifier debugfs dir */
	ret = pp_debugfs_create(parent, "classifier", &dbgfs, cls_root_files,
				ARRAY_SIZE(cls_root_files), NULL);
	if (unlikely(ret))
		return ret;

	/* debug sessions */
	ret = pp_debugfs_create(dbgfs, "dbg_session", NULL, session_dbg_files,
				ARRAY_SIZE(session_dbg_files), NULL);
	if (unlikely(ret))
		return ret;

	/* FV debugfs */
	ret = pp_debugfs_create(dbgfs, "fv", NULL, fv_dbg_files,
				ARRAY_SIZE(fv_dbg_files), NULL);
	if (unlikely(ret))
		return ret;

	/**
	 * hash debugfs
	 */
	ret = pp_debugfs_create(dbgfs, "hash", NULL, hash_dbg_files,
				ARRAY_SIZE(hash_dbg_files), NULL);
	if (unlikely(ret))
		return ret;

	/**
	 * lookup debugfs
	 */
	ret = pp_debugfs_create(dbgfs, "lu", NULL, lookup_dbg_files,
				ARRAY_SIZE(lookup_dbg_files), NULL);
	if (unlikely(ret))
		return ret;

	/**
	 * signature cache debugfs
	 */
	ret = pp_debugfs_create(dbgfs, "sig_cache", NULL, sig_cache_dbg_files,
				ARRAY_SIZE(sig_cache_dbg_files), NULL);
	if (unlikely(ret))
		return ret;

	/**
	 * session cache debugfs
	 */
	ret = pp_debugfs_create(dbgfs, "sess_cache", NULL, sess_cache_files,
				ARRAY_SIZE(sess_cache_files), NULL);
	if (unlikely(ret))
		return ret;

	/* Init status word common fields */
	STW_INIT_COMMON_FLDS(cls_stw_flds);
	/* Init classifier specific status word fields */
	BUF_FLD_INIT_DESC(cls_stw_flds, STW_CLS_FLD_TTL_EXPIRED);
	BUF_FLD_INIT_DESC(cls_stw_flds, STW_CLS_FLD_IP_UNSUPP);
	BUF_FLD_INIT_DESC(cls_stw_flds, STW_CLS_FLD_EXT_DF);
	BUF_FLD_INIT_DESC(cls_stw_flds, STW_CLS_FLD_INT_DF);
	BUF_FLD_INIT_DESC(cls_stw_flds, STW_CLS_FLD_EXT_FRAG_TYPE);
	BUF_FLD_INIT_DESC(cls_stw_flds, STW_CLS_FLD_INT_FRAG_TYPE);
	BUF_FLD_INIT_DESC(cls_stw_flds, STW_CLS_FLD_TCP_FIN);
	BUF_FLD_INIT_DESC(cls_stw_flds, STW_CLS_FLD_TCP_SYN);
	BUF_FLD_INIT_DESC(cls_stw_flds, STW_CLS_FLD_TCP_RST);
	BUF_FLD_INIT_DESC(cls_stw_flds, STW_CLS_FLD_TCP_ACK);
	BUF_FLD_INIT_DESC(cls_stw_flds, STW_CLS_FLD_TCP_DATA_OFFSET);
	BUF_FLD_INIT_DESC(cls_stw_flds, STW_CLS_FLD_EXT_L3_OFFSET);
	BUF_FLD_INIT_DESC(cls_stw_flds, STW_CLS_FLD_INT_L3_OFFSET);
	BUF_FLD_INIT_DESC(cls_stw_flds, STW_CLS_FLD_EXT_L4_OFFSET);
	BUF_FLD_INIT_DESC(cls_stw_flds, STW_CLS_FLD_INT_L4_OFFSET);
	BUF_FLD_INIT_DESC(cls_stw_flds, STW_CLS_FLD_EXT_FRAG_INFO_OFFSET);
	BUF_FLD_INIT_DESC(cls_stw_flds, STW_CLS_FLD_INT_FRAG_INFO_OFFSET);
	BUF_FLD_INIT_DESC(cls_stw_flds, STW_CLS_FLD_L3_HDR_OFFSET_5);
	BUF_FLD_INIT_DESC(cls_stw_flds, STW_CLS_FLD_L3_HDR_OFFSET_4);
	BUF_FLD_INIT_DESC(cls_stw_flds, STW_CLS_FLD_L3_HDR_OFFSET_3);
	BUF_FLD_INIT_DESC(cls_stw_flds, STW_CLS_FLD_L3_HDR_OFFSET_2);
	BUF_FLD_INIT_DESC(cls_stw_flds, STW_CLS_FLD_L3_HDR_OFFSET_1);
	BUF_FLD_INIT_DESC(cls_stw_flds, STW_CLS_FLD_L3_HDR_OFFSET_0);
	BUF_FLD_INIT_DESC(cls_stw_flds, STW_CLS_FLD_ANA_PKT_TYPE);
	BUF_FLD_INIT_DESC(cls_stw_flds, STW_CLS_FLD_TUNN_INNER_OFFSET);
	BUF_FLD_INIT_DESC(cls_stw_flds, STW_CLS_FLD_PAYLOAD_OFFSET);
	BUF_FLD_INIT_DESC(cls_stw_flds, STW_CLS_FLD_ERROR_IND);
	BUF_FLD_INIT_DESC(cls_stw_flds, STW_CLS_FLD_LRU_EXP);
	BUF_FLD_INIT_DESC(cls_stw_flds, STW_CLS_FLD_RPB_CLID);
	BUF_FLD_INIT_DESC(cls_stw_flds, STW_CLS_FLD_DROP_PKT);
	BUF_FLD_INIT_DESC(cls_stw_flds, STW_CLS_FLD_FV_FLD_1);

	return 0;
}

void cls_dbg_clean(void)
{
	debugfs_remove_recursive(dbgfs);
	dbgfs = NULL;
}
