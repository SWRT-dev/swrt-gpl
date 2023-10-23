/*
 * classifier.c
 * Description: PP Classifier Driver
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018-2020 Intel Corporation
 */
#include <linux/types.h>
#include <linux/init.h>
#include <linux/log2.h>
#include <linux/cuckoo_hash.h>
#include <linux/pp_api.h>
#include "pp_regs.h"
#include "pp_common.h"
#include "pp_si.h"
#include "classifier.h"
#include "classifier_internal.h"
#include "infra.h"
#include "rpb.h"
/* For A-step only*/
#include "uc.h"
#include "uc_common.h"

/**
 * @brief classifier HW base address
 */
u64 cls_base_addr;

/**
 * @brief Session cache HW base address
 */
u64 cls_cache_base_addr;

/**
 * @brief Defines all statistics required for implementing SW reset
 *        statistics
 */
struct cls_rstats {
	struct cls_stats stats;
	u32 scache_hit[CLS_SIG_CACHE_CNT];
	u32 scache_long_trans[CLS_SIG_CACHE_CNT];
};

/**
 * @brief module database
 */
static struct cls_database *db;
/**
 * @brief saved stats from last reset request
 */
static struct cls_rstats rstats;

dma_addr_t cls_si_phys_base_get(void)
{
	if (unlikely(!db))
		return 0;

	return db->si_base_phys;
}

s32 cls_session_si_get(u32 id, struct pp_hw_si *si)
{
	unsigned long src;
	u32 i;

	if (unlikely(ptr_is_null(si)))
		return -EINVAL;

	pr_debug("id %u, si %p\n", id, si);

	src = (unsigned long)(db->si_base_virt) + CLS_SESSION_SI_OFF(id);
	if (id >= db->num_sessions) {
		pr_err("invalid session id %u, support %u sessions\n",
		       id, db->num_sessions);
		return -EINVAL;
	}
	memcpy((void *)si, (void *)src, sizeof(*si));

	for (i = 0; i < ARRAY_SIZE(si->word); i++) {
		src += (i * sizeof(u32));
		pr_debug("addr 0x%08lx = 0x%08x\n", src, si->word[i]);
	}

	return 0;
}

s32 cls_dbg_last_fv_get(void *fv)
{
	u32 i, addr;

	if (unlikely(ptr_is_null(fv)))
		return -EINVAL;

	/* read the last fv registers */
	PP_REG_RD_REP((unsigned long)PP_CLS_DBG_FV_REG, fv,
		      sizeof(struct cls_dbg_fv));

	pr_debug("Field Vector:\n");
	for (i = 0; i < CLS_DBG_FV_NUM_REGS; i++) {
		addr = PP_CLS_DBG_FV_REG + (i * sizeof(u32));
		pr_debug("addr 0x%08x = 0x%08x\n", addr, ((u32 *)fv)[i]);
	}

	return 0;
}

s32 cls_lu_thrd_stats_get(u32 thrd, struct cls_lu_thrd_stats *stats)
{
	u32 stm_reg;

	if (unlikely(!stats)) {
		pr_err("Invalid lookup counters paramete\n");
		return -EINVAL;
	}

	if (unlikely(thrd >= db->num_lu_threads)) {
		pr_err("Invalid lookup thread parameter, %u\n", thrd);
		return -EINVAL;
	}

	/* read lookup thread counters */
	stats->request = PP_REG_RD32(PP_CLS_LU_COUNT_REG_IDX(thrd));
	stats->match   = PP_REG_RD32(PP_CLS_LU_MATCH_COUNT_REG_IDX(thrd));
	stats->long_lu = PP_REG_RD32(PP_CLS_LU_CNT_PC_RESULT_REG_IDX(thrd));

	stm_reg = PP_REG_RD32(CLS_LU_THREAD_STM_STATUS_REG(thrd));
	stats->state = PP_FIELD_GET(PP_CLS_LU_THREAD_STM_STATUS_FLD(thrd),
				    stm_reg);

	pr_debug("lu_thread=%u, lu_request=%u, lu_match=%u, lu_long=%u, stm=0x%x\n",
		 thrd, stats->request, stats->match,
		 stats->long_lu, stats->state);

	return 0;
}

s32 cls_dbg_hash_get(struct pp_hash *hash)
{
	if (unlikely(!hash)) {
		pr_err("Invalid PP hash parameter\n");
		return -EINVAL;
	}

	/* read debug registers of last hash results
	 * NOTE: this is a pure hash results, no mask involved
	 */
	hash->h1  = PP_REG_RD32(PP_CLS_DBG_HASH_1_REG);
	hash->h2  = PP_REG_RD32(PP_CLS_DBG_HASH_2_REG);
	hash->sig = PP_REG_RD32(PP_CLS_DBG_HASH_SIG_REG);

	pr_debug("hash1=%#x, hash2=%#x, sig=%#x\n",
		 hash->h1, hash->h2, hash->sig);

	return 0;
}

s32 cls_dbg_stw_get(struct cls_hw_stw *hw_stw)
{
	if (unlikely(ptr_is_null(hw_stw)))
		return -EINVAL;

	PP_REG_RD_REP(PP_CLS_DBG_STW_REG, hw_stw, sizeof(*hw_stw));

	return 0;
}

s32 cls_hw_ver_get(struct pp_version *ver)
{
	if (unlikely(ptr_is_null(ver)))
		return -EINVAL;

	memset(ver, 0, sizeof(*ver));
	if (PP_REG_RD32(PP_CLS_CLS_VER_REG)) {
		ver->major = FIELD_GET(PP_CLS_CLS_VER_MSK,
				       PP_REG_RD32(PP_CLS_CLS_VER_REG));
	}

	pr_debug("version=%u\n", ver->major);

	return 0;
}

void cls_fv_endian_swap_set(bool in_swap, bool eg_swap)
{
	u32 reg;

	pr_debug("swap_ingress=%s, swap_egress=%s\n",
		 in_swap ? "YES" : "NO", eg_swap ? "YES" : "NO");

	reg  = PP_FIELD_PREP(PP_CLS_SWAP_EN_ENDIANESS_EGRESS_MSK,  eg_swap);
	reg |= PP_FIELD_PREP(PP_CLS_SWAP_EN_ENDIANESS_INGRESS_MSK, in_swap);

	/* Classifier receives the FV from the parser in 2 blocks of 512 bits
	 * we disable the 2 blocks swapping
	 */
	reg |= PP_FIELD_PREP(PP_CLS_SWAP_EN_SWAP_INGRESS_MSK, false);
	PP_REG_WR32(PP_CLS_SWAP_EN_REG, reg);
}

s32 cls_fv_endian_swap_get(bool *in_swap, bool *eg_swap)
{
	u32 reg;

	if (unlikely(!(!ptr_is_null(in_swap) &&
		       !ptr_is_null(eg_swap))))
		return -EINVAL;

	reg      = PP_REG_RD32(PP_CLS_SWAP_EN_REG);
	*in_swap = PP_FIELD_GET(PP_CLS_SWAP_EN_ENDIANESS_INGRESS_MSK, reg);
	*eg_swap = PP_FIELD_GET(PP_CLS_SWAP_EN_ENDIANESS_EGRESS_MSK,  reg);

	pr_debug("swap_ingress=%s, swap_egress=%s\n",
		 *in_swap ? "YES" : "NO", *eg_swap ? "YES" : "NO");

	return 0;
}

void cls_hash1_base_addr_set(u64 ht1_base_addr)
{
	pr_debug("set hash1 base address=%#llx\n", ht1_base_addr);

	PP_REG_WR32(PP_CLS_HASH_TABLE_1_BASE_REG, (u32)(ht1_base_addr >> 4));
}

void cls_hash2_base_addr_set(u64 ht2_base_addr)
{
	pr_debug("set hash2 base address=%#llx\n", ht2_base_addr);

	PP_REG_WR32(PP_CLS_HASH_TABLE_2_BASE_REG, (u32)(ht2_base_addr >> 4));
}

void cls_si_base_addr_set(u64 si_base_addr)
{
	pr_debug("set si base address=%#llx\n", si_base_addr);

	PP_REG_WR32(PP_CLS_SI_DDR_BASE_REG,  (u32)(si_base_addr >> 4));
}

void cls_sig_cache_long_trans_cntr_thr_set(u64 thr)
{
	pr_debug("sig_cache_transaction_counter_threshold=%llu\n", thr);

	PP_REG_WR32(PP_CLS_CACHE_STAT_MSB_INTERVAL_1_REG, thr >> 32);
	PP_REG_WR32(PP_CLS_CACHE_STAT_LSB_INTERVAL_1_REG, thr & U32_MAX);
	PP_REG_WR32(PP_CLS_CACHE_STAT_MSB_INTERVAL_2_REG, thr >> 32);
	PP_REG_WR32(PP_CLS_CACHE_STAT_LSB_INTERVAL_2_REG, thr & U32_MAX);
}

s32 cls_sig_cache_long_trans_cntr_thr_get(u64 *thr)
{
	u32 regh, regl;

	if (unlikely(ptr_is_null(thr)))
		return -EINVAL;

	regh = PP_REG_RD32(PP_CLS_CACHE_STAT_MSB_INTERVAL_1_REG);
	regl = PP_REG_RD32(PP_CLS_CACHE_STAT_LSB_INTERVAL_1_REG);
	*thr = PP_BUILD_64BIT_WORD(regl, regh);

	pr_debug("sig_cache_transaction_counter_threshold=%llu\n", *thr);

	return 0;
}

void __cls_sig_cache_stats_get(enum cls_sig_cache cache,
			       u32 *hit, u32 *long_trns)
{
	unsigned long hist_cntr_addr, lt_cntr_addr;

	switch (cache) {
	case CLS_SIG_CACHE1:
		hist_cntr_addr = PP_CLS_CACHE_HIT_STAT_1_REG;
		lt_cntr_addr   = PP_CLS_CACHE_TRANS_STAT_1_REG;
		break;
	case CLS_SIG_CACHE2:
		hist_cntr_addr = PP_CLS_CACHE_HIT_STAT_2_REG;
		lt_cntr_addr   = PP_CLS_CACHE_TRANS_STAT_2_REG;
		break;
	default:
		pr_err("invalid sig cache type, %d\n", cache);
		return;
	}

	*hit       = PP_REG_RD32(hist_cntr_addr);
	*long_trns = PP_REG_RD32(lt_cntr_addr);

	pr_debug("sig_cache%d: hit_cntr=%u, long_transaction_cntr=%u\n",
		 cache, *hit, *long_trns);
}

s32 cls_sig_cache_stats_get(enum cls_sig_cache cache, u32 *hit, u32 *long_trns)
{
	if (unlikely(!(!ptr_is_null(hit) &&
		       !ptr_is_null(long_trns))))
		return -EINVAL;

	if (unlikely(!(cache < CLS_SIG_CACHE_CNT))) {
		pr_err("invalid sig cache type, %d\n", cache);
		return -EINVAL;
	}

	__cls_sig_cache_stats_get(cache, hit, long_trns);
	*hit       -= rstats.scache_hit[cache];
	*long_trns -= rstats.scache_long_trans[cache];

	return 0;
}

void cls_sig_cache_cfg_set(bool enable, bool invalidate)
{
	u32 reg;

	pr_debug("cache_enable=%s, cache_invalidate=%s\n",
		 enable ? "YES" : "NO", invalidate ? "YES" : "NO");

	reg  = PP_FIELD_PREP(PP_CLS_CACHE_CFG_CACHE_DIS_MSK, !enable);
	reg |= PP_FIELD_PREP(PP_CLS_CACHE_CFG_CACHE_INVALIDATE_MSK, invalidate);
	PP_REG_WR32(PP_CLS_CACHE_CFG_REG, reg);
	reg  = PP_FIELD_MOD(PP_CLS_CACHE_CFG_CACHE_INVALIDATE_MSK, 0, reg);
	PP_REG_WR32(PP_CLS_CACHE_CFG_REG, reg);
}

s32 cls_sig_cache_cfg_get(bool *enable)
{
	u32 reg;

	if (unlikely(ptr_is_null(enable)))
		return -EINVAL;

	reg     = PP_REG_RD32(PP_CLS_CACHE_CFG_REG);
	*enable = !PP_FIELD_GET(PP_CLS_CACHE_CFG_CACHE_DIS_MSK, reg);

	pr_debug("cache_enable=%s\n", *enable ? "YES" : "NO");

	return 0;
}

s32 cls_sig_error_cntr_get(u32 *sig_err)
{
	if (unlikely(ptr_is_null(sig_err)))
		return -EINVAL;

	*sig_err = PP_REG_RD32(PP_CLS_LU_CNT_SIG_STATUS_REG);

	pr_debug("sig_err=%u\n", *sig_err);

	return 0;
}

bool cls_sess_cache_bypass_get(void)
{
	return !!PP_REG_RD32(PP_SESS_CACHE_BYPASS_REG);
}

void cls_sess_cache_bypass_set(bool bypass)
{
	PP_REG_WR32(PP_SESS_CACHE_BYPASS_REG, bypass);
}

s32 cls_sess_cache_stats_get(u64 *read_trans, u64 *read_hits)
{
	u32 h, l;

	if (unlikely(ptr_is_null(read_trans) || ptr_is_null(read_hits)))
		return -EINVAL;

	h = PP_REG_RD32(PP_SESS_CACHE_READ_TRANS_CNT_H_REG);
	l = PP_REG_RD32(PP_SESS_CACHE_READ_TRANS_CNT_L_REG);
	*read_trans = PP_BUILD_64BIT_WORD(l, h);

	h = PP_REG_RD32(PP_SESS_CACHE_READ_HIT_TRANS_CNT_H_REG);
	l = PP_REG_RD32(PP_SESS_CACHE_READ_HIT_TRANS_CNT_L_REG);
	*read_hits = PP_BUILD_64BIT_WORD(l, h);

	return 0;
}

static void __cls_sess_cache_init(void)
{
	u32 reg;

	/* set the cache to write back, write only to the cache,
	 * cause the counters are being written every accelerated
	 * packet and we want to reduce the DDR bandwidth
	 */
	reg = PP_REG_RD32(PP_SESS_CACHE_POLICY_REG);
	reg = PP_FIELD_MOD(PP_SESS_CACHE_POLICY_WRITE_POLICY_MSK,
			   PP_CACHE_WR_THR, reg);
	PP_REG_WR32(PP_SESS_CACHE_POLICY_REG, reg);

	/* enable the session cache */
	cls_sess_cache_bypass_set(false);
}

void cls_thrd_cfg_set(bool drop_flow_dis, bool search_mode, bool fm_flow,
		      bool fv_masked)
{
	u32 reg;

	pr_debug("drop_flow_dis=%s, search_mode=%s, fm_flow=%s, fv_masked %s\n",
		 BOOL2EN(!drop_flow_dis), search_mode ? "ON_SIG" : "NORMAL",
		 fm_flow ? "FM" : "SIG", BOOL2EN(fv_masked));

	reg = PP_REG_RD32(PP_CLS_THREAD_CFG_REG);
	reg = PP_FIELD_MOD(PP_CLS_THREAD_CFG_DROP_FLOW_MSK, drop_flow_dis, reg);
	reg = PP_FIELD_MOD(PP_CLS_THREAD_CFG_SEARCH_MODE_MSK, search_mode, reg);
	reg = PP_FIELD_MOD(PP_CLS_THREAD_CFG_FM_FLOW_MSK,     fm_flow, reg);
	reg = PP_FIELD_MOD(PP_CLS_THREAD_CFG_DBG_FV_SEL_MSK,  fv_masked, reg);
	PP_REG_WR32(PP_CLS_THREAD_CFG_REG, reg);
}

s32 cls_thrd_cfg_get(bool *drop_flow_dis, bool *search_mode, bool *fm_flow,
		     bool *fv_masked)
{
	u32 reg;

	if (unlikely(!(!ptr_is_null(drop_flow_dis) &&
		       !ptr_is_null(search_mode)   &&
		       !ptr_is_null(fm_flow)       &&
		       !ptr_is_null(fv_masked))))
		return -EINVAL;

	reg            = PP_REG_RD32(PP_CLS_THREAD_CFG_REG);
	*drop_flow_dis = PP_FIELD_GET(PP_CLS_THREAD_CFG_DROP_FLOW_MSK,   reg);
	*search_mode   = PP_FIELD_GET(PP_CLS_THREAD_CFG_SEARCH_MODE_MSK, reg);
	*fm_flow       = PP_FIELD_GET(PP_CLS_THREAD_CFG_FM_FLOW_MSK,     reg);
	*fv_masked     = PP_FIELD_GET(PP_CLS_THREAD_CFG_DBG_FV_SEL_MSK,  reg);

	pr_debug("drop_flow_dis=%s, search_mode=%s, fm_flow=%s, fv_masked %s\n",
		 BOOL2EN(!(*drop_flow_dis)), *search_mode ? "ON_SIG" : "NORMAL",
		 *fm_flow ? "FM" : "SIG", BOOL2EN(*fv_masked));

	return 0;
}

s32 cls_num_lu_thread_set(u32 num_threads)
{
	if (unlikely(num_threads > db->num_lu_threads)) {
		pr_err("invalid number of lookup threads %u, max=%u",
		       num_threads, db->num_lu_threads);
		return -EINVAL;
	}

	pr_debug("num_threads=%u\n", num_threads);

	/* set zeros in the 'num_threads' first bits */
	PP_REG_WR32(PP_CLS_LU_MASK_REG, U32_MAX << num_threads);

	return 0;
}

s32 cls_num_lu_thread_get(u32 *num_threads)
{
	u32 reg;

	if (unlikely(ptr_is_null(num_threads)))
		return -EINVAL;

	reg = ~PP_REG_RD32(PP_CLS_LU_MASK_REG);
	*num_threads = hweight32(reg);

	pr_debug("num_threads=%u\n", *num_threads);
	return 0;
}

s32 cls_hash_size_set(u32 size)
{
	u32 reg, val;

	if (unlikely(size > CLS_HASH_SZ_MAX)) {
		pr_err("hash size out of range, size=%u, max=%u\n",
		       size, CLS_HASH_SZ_MAX);
		return -EINVAL;
	}

	if (unlikely(!is_power_of_2(size))) {
		pr_err("hash size must be power of 2, size=%u\n", size);
		return -EINVAL;
	}

	pr_debug("hash_size=%u\n", size);

	db->hash_mask = size - 1;
	val = (~db->hash_mask) & PP_CLS_HASH_MASK_MSK;
	reg = PP_FIELD_PREP(PP_CLS_HASH_MASK_MSK, val);
	PP_REG_WR32(PP_CLS_HASH_MASK_REG, reg);

	pr_debug("hash_mask=%#x\n", reg);

	return 0;
}

s32 cls_hash_size_get(u32 *size)
{
	u32 mask, reg;

	if (unlikely(ptr_is_null(size)))
		return -EINVAL;

	reg   = PP_REG_RD32(PP_CLS_HASH_MASK_REG);
	mask  = PP_FIELD_GET(PP_CLS_HASH_MASK_MSK, reg) | ~PP_CLS_HASH_MASK_MSK;
	*size = (~mask) + 1;

	pr_debug("hash_size=%u\n", *size);

	return 0;
}

void cls_long_lu_thr_set(u32 thr)
{
	pr_debug("long process counter threshold=%u\n", thr);

	PP_REG_WR32(PP_CLS_LU_CNT_PC_CFG_REG, thr);
}

s32 cls_long_lu_thr_get(u32 *thr)
{
	if (unlikely(ptr_is_null(thr)))
		return -EINVAL;

	*thr = PP_REG_RD32(PP_CLS_LU_CNT_PC_CFG_REG);

	pr_debug("long process counter threshold=%u\n", *thr);

	return 0;
}

static void __cls_dflt_si_set(void)
{
	pr_debug("default si set\n");

	/* set invalid session id and recipe */
	PP_REG_WR32(PP_CLS_SESSION_INFO_DEFAULT_REG_IDX(CLS_DFLT_SI_W0),
		    CLS_DFLT_SI_IDX);

	/* WA: prevent counting lu requests as matches by set the fv size */
	PP_REG_WR32(PP_CLS_SESSION_INFO_DEFAULT_REG_IDX(CLS_DFLT_SI_W1),
		    CLS_DFLT_SI_FV_SZ);
}

static inline void __cls_cmd_lock(void)
{
	if (pp_silicon_step_get() == PP_SSTEP_A)
		uc_ccu_gpreg_set(UC_IS_EGR, CLS_LOCK_REG_IDX, 1);
}

static inline void __cls_cmd_unlock(void)
{
	if (pp_silicon_step_get() == PP_SSTEP_A)
		uc_ccu_gpreg_set(UC_IS_EGR, CLS_LOCK_REG_IDX, 0);
}

void cls_lu_stats_reset(void)
{
	u32 reg;

	pr_debug("reset lu counters\n");

	/* set the enable bit and clear bit */
	reg  = PP_FIELD_PREP(PP_CLS_LU_CNT_CONFG_CNT_EN_MSK,  1);
	reg |= PP_FIELD_PREP(PP_CLS_LU_CNT_CONFG_CNT_CLR_MSK, 1);
	PP_REG_WR32(PP_CLS_LU_CNT_CONFG_REG, reg);

	/* clear the clear bit */
	reg  = PP_FIELD_MOD(PP_CLS_LU_CNT_CONFG_CNT_CLR_MSK, 0, reg);
	PP_REG_WR32(PP_CLS_LU_CNT_CONFG_REG, reg);
}

s32 cls_hash_select_set(u32 sig, u32 hash)
{
	u32 reg;

	if (unlikely(!__pp_mask_value_valid(PP_CLS_HASH_SEL_SBOX_CTRL_MSK,
					    sig))) {
		pr_err("invalid sig select value %hhu\n", sig);
		return -EINVAL;
	}

	if (unlikely(!__pp_mask_value_valid(PP_CLS_HASH_SEL_HASH_CTRL_MSK,
					    hash))) {
		pr_err("invalid hash select value %hhu\n", hash);
		return -EINVAL;
	}

	pr_debug("sig_select=%hhu, hash_select=%hhu\n", sig, hash);

	reg  = PP_FIELD_PREP(PP_CLS_HASH_SEL_SBOX_CTRL_MSK, sig);
	reg |= PP_FIELD_PREP(PP_CLS_HASH_SEL_HASH_CTRL_MSK, hash);
	PP_REG_WR32(PP_CLS_HASH_SEL_REG, reg);

	return 0;
}

s32 cls_hash_select_get(u32 *sig, u32 *hash)
{
	u32 reg;

	if (unlikely(!(!ptr_is_null(sig) &&
		       !ptr_is_null(hash))))
		return -EINVAL;

	reg   = PP_REG_RD32(PP_CLS_HASH_SEL_REG);
	*sig  = PP_FIELD_GET(PP_CLS_HASH_SEL_SBOX_CTRL_MSK, reg);
	*hash = PP_FIELD_GET(PP_CLS_HASH_SEL_HASH_CTRL_MSK, reg);

	pr_debug("sig_select=%hhu, hash_select=%hhu\n", *sig, *hash);

	return 0;
}

s32 cls_fv_mask_set(void *mask)
{
	u32 i;

	if (unlikely(ptr_is_null(mask)))
		return -EINVAL;

	pr_debug("Update field vector mask:\n");

	for (i = 0; i < CLS_FV_MSK_NUM_REGS; i++) {
		PP_REG_WR32(PP_CLS_FV_MASK_REG_IDX(i), ((u32 *)mask)[i]);
		pr_debug("FV WORD%02u = 0x%08x\n", i, ((u32 *)mask)[i]);
	}

	return 0;
}

s32 cls_fv_mask_get(void *mask)
{
	u32 i;

	if (unlikely(ptr_is_null(mask)))
		return -EINVAL;

	pr_debug("Read field vector mask:\n");

	for (i = 0; i < CLS_FV_MSK_NUM_REGS; i++) {
		((u32 *)mask)[i] = PP_REG_RD32(PP_CLS_FV_MASK_REG_IDX(i));
		pr_debug("FV WORD%02u = 0x%08x\n", i, ((u32 *)mask)[i]);
	}

	return 0;
}

s32 cls_session_lookup(void *fv, u32 fv_sz, struct pp_hash *hash, u32 *id)
{
	return cls_session_lookup_verbose(fv, fv_sz, hash, id, NULL, NULL);
}

s32 cls_session_lookup_verbose(void *fv,
			       u32 fv_sz,
			       struct pp_hash *hash,
			       u32 *id,
			       u8 *tbl_idx,
			       u8 *slot_idx)
{
	struct cuckoo_hash_item session;

	if (unlikely(!(!ptr_is_null(fv)   &&
		       !ptr_is_null(hash) &&
		       !ptr_is_null(id))))
		return -EINVAL;

	/* set invalid value */
	*id             = CLS_SESSION_INVALID;
	/* prepare the cuckoo item */
	session.key     = fv;
	session.key_len = fv_sz;
	session.h1      = hash->h1;
	session.h2      = hash->h2;
	session.sig     = hash->sig;

	if (cuckoo_hash_lookup_verbose(db->cuckoo, &session,
				       tbl_idx, slot_idx) != EEXIST) {
		pr_debug("session not found in cuckoo hash (verbose)\n");
		goto done;
	}

	*id = (unsigned long)session.value;
	pr_debug("session %u found in cuckoo hash\n", *id);

done:
	return 0;
}

s32 cls_session_add(void *si, struct pp_hash *hash)
{
	struct cuckoo_hash_item session;
	u32 s_id, fv_sz;
	s32 ret;

	if (unlikely(!(!ptr_is_null(si) &&
		       !ptr_is_null(hash))))
		return -EINVAL;

	/* store the si template for the event handling */
	db->cls_si_req     = si;
	db->cls_cmd_status = 0;

	ret = pp_si_fld_get(si, SI_FLD_SESS_ID, &s_id);
	if (unlikely(ret))
		return -EINVAL;

	ret = pp_si_fld_get(si, SI_FLD_FV_SZ, &fv_sz);
	if (unlikely(ret))
		return -EINVAL;

	/* prepare the cuckoo item */
	if (s_id >= db->num_sessions) {
		pr_err("invalid session id %u, support %u sessions\n",
		       s_id, db->num_sessions);
		return -EINVAL;
	}

	__cls_cmd_lock();
	session.key = (void *)CLS_FV_ADDR((unsigned long)(db->si_base_virt),
					  s_id, fv_sz);
	session.key_len    = fv_sz;
	session.value      = (void *)(unsigned long)s_id;
	session.h1         = hash->h1;
	session.h2         = hash->h2;
	session.sig        = hash->sig;

	ret = cuckoo_hash_insert(db->cuckoo, &session);
	if (ret) {
		pr_err("failed to add the session to cuckoo hash, %u\n", s_id);
		__cls_cmd_unlock();
		return ret;
	}
	__cls_cmd_unlock();

	return db->cls_cmd_status;
}

void cls_n_retries_set(u32 n)
{
	db->num_retries = n;
}

void cls_n_retries_get(u32 *n)
{
	if (unlikely(ptr_is_null(n)))
		return;

	*n = db->num_retries;
}

static inline s32 __cls_cmd_status_poll(void)
{
	return pp_reg_poll_x(PP_CLS_CMD_STATUS_REG, PP_CLS_CMD_STATUS_MSK, 0,
			     db->num_retries);
}

static inline bool __cls_cmd_is_done(void)
{
	if (!__cls_cmd_status_poll())
		return true;

	/* polling failed, stop the RPB egress and try again */
	rpb_rcu_egress_set(false);
	if (!__cls_cmd_status_poll()) {
		rpb_rcu_egress_set(true);
		return true;
	}
	rpb_rcu_egress_set(true);
	pr_err("Failed polling on classifier command status\n");
	return false;
}

/**
 * @brief Dispatch mailbox command to the classifier
 * @note MUST be called under a lock
 * @param id session id
 * @param cmd the command
 */
static inline s32 __cls_cmd_dispatch(struct uc_cls_cmd *uc_cmd)
{
	s32 ret = 0;

	pr_debug("cmd_id %u\n", uc_cmd->cmd_id);

	/* send command */
	ret = uc_egr_mbox_cmd_send(UC_CMD_CLS_MBX, 0, (const void *)uc_cmd,
				   sizeof(*uc_cmd), NULL, 0);
	if (ret)
		pr_err("UC_CMD_CLS_MBX command failed\n");

	return ret;
}

static inline s32 __cls_cmd_execute(struct uc_cls_cmd *cmd)
{
	s32 ret = 0;
	u32 i;

	pr_debug("Executing CLS CMD %u\n", cmd->cmd_id);

	/* A Step */
	if (pp_silicon_step_get() == PP_SSTEP_A) {
		ret = __cls_cmd_dispatch(cmd);
		if (ret)
			pr_err("CLS CMD %u failed\n", cmd->cmd_id);
		else
			pr_debug("CLS CMD done successfully\n");
		return ret;
	}

	/* B Step */
	PP_REG_WR32(PP_CLS_CMD_DATA_REG, cmd->h1);
	PP_REG_WR32(PP_CLS_CMD_DATA_REG, cmd->h2);
	PP_REG_WR32(PP_CLS_CMD_DATA_REG, cmd->sig);
	PP_REG_WR32(PP_CLS_CMD_DATA_REG, cmd->cmd_info);
	PP_REG_WR32(PP_CLS_CMD_DATA_REG, cmd->dsi_info);

	/* Write SI */
	if (cmd->cmd_id == CLS_CMD_IF_ADD) {
		for (i = 0; i < PP_SI_WORDS_CNT; i++) {
			pr_debug("CLS_CMD_WORD[%-2u]:  0x%X\n",
					 i, cmd->si[i]);
			PP_REG_WR32_NO_SWAP(PP_CLS_CMD_DATA_REG, cmd->si[i]);
		}
	}

	/* Kick command */
	PP_REG_WR32(PP_CLS_CMD_KICK_REG, cmd->cmd_id);

	if (unlikely(!__cls_cmd_is_done())) {
		db->cls_cmd_status = -ETIME;
		return -EINVAL;
	}

	pr_debug("CLS CMD done successfully\n");

	return 0;
}

s32 cls_session_dsi_update(u32 id, u16 dst_q, bool clr_cntr)
{
	struct uc_cls_cmd uc_cmd;
	u32 reg;
	s32 ret = 0;

	pr_debug("CLS CMD update session %u: new_queue=%hu clear_counters=%s\n",
		 id, dst_q, clr_cntr ? "YES" : "NO");

	/* do we have something to update ?*/
	if (unlikely(dst_q == PP_QOS_INVALID_ID && !clr_cntr)) {
		pr_err("nothing to update for session %u\n", id);
		return -EINVAL;
	}

	/* Write the hash results - NA for update command */
	uc_cmd.h1 = 0;
	uc_cmd.h2 = 0;
	uc_cmd.sig = 0;

	/* Write the command info */
	reg  = PP_FIELD_PREP(CLS_CMD_UPDT_DSI_INFO, 1);
	reg |= PP_FIELD_PREP(CLS_CMD_SESS_ID, id);
	uc_cmd.cmd_info = reg;

	/* Write DSI info */
	if (dst_q != PP_QOS_INVALID_ID) {
		reg |= PP_FIELD_PREP(CLS_CMD_DSI_DQWR, 1);
		reg |= PP_FIELD_PREP(CLS_CMD_DSI_DST_Q, dst_q);
	}
	reg |= PP_FIELD_PREP(CLS_CMD_DSI_CNT_CLR, clr_cntr);
	uc_cmd.dsi_info = reg;

	/* Kick command */
	uc_cmd.cmd_id = CLS_CMD_IF_UPDATE;

	__cls_cmd_lock();

	/* Execute command */
	ret = __cls_cmd_execute(&uc_cmd);
	if (unlikely(ret))
		db->cls_cmd_status = ret;

	__cls_cmd_unlock();

	return ret;
}

s32 cls_session_del(u32 id, u32 fv_sz, struct pp_hash *hash)
{
	struct cuckoo_hash_item session;
	s32 ret;

	if (unlikely(ptr_is_null(hash)))
		return -EINVAL;

	if (unlikely(!fv_sz)) {
		pr_err("field vector size is zero\n");
		return -EINVAL;
	}

	db->cls_cmd_status = 0;
	__cls_cmd_lock();
	/* prepare the cuckoo item */
	session.key = (void *)CLS_FV_ADDR((unsigned long)(db->si_base_virt),
					  id, fv_sz);
	session.key_len    = fv_sz;
	session.h1         = hash->h1;
	session.h2         = hash->h2;
	session.sig        = hash->sig;

	ret = cuckoo_hash_remove(db->cuckoo, &session);
	if (ret) {
		pr_err("failed to del the session from cuckoo hash, %u\n", id);
		__cls_cmd_unlock();
		return ret;
	}
	__cls_cmd_unlock();

	return db->cls_cmd_status;
}

/* TODO: s32 cls_hash_flush(void); */

s32 cls_hash_stats_get(struct cuckoo_hash_stats *stats)
{
	return cuckoo_hash_stats_get(db->cuckoo, stats);
}

void cls_cuckoo_hash_stats_dump(void)
{
	cuckoo_hash_stats_dump(db->cuckoo);
}

void cls_cuckoo_hash_cfg_dump(void)
{
	cuckoo_hash_cfg_dump(db->cuckoo);
}

static s32 __cls_stats_get(struct cls_stats *stats)
{
	struct cls_lu_thrd_stats thread_stats;
	u32 thrd_idx;
	s32 ret = 0;

	if (unlikely(!stats)) {
		pr_err("Invalid stats parameter\n");
		return -EINVAL;
	}

	memset(stats, 0, sizeof(struct cls_stats));

	stats->pkts_rcvd_from_parser = PP_REG_RD32(PP_CLS_PKT_COUNT_PARSER_REG);
	stats->pkts_rcvd_from_uc = PP_REG_RD32(PP_CLS_PKT_COUNT_UC_REG);

	for (thrd_idx = 0 ; thrd_idx < db->num_lu_threads ; thrd_idx++) {
		ret = cls_lu_thrd_stats_get(thrd_idx, &thread_stats);
		if (ret)
			return ret;

		stats->total_lu_matches += thread_stats.match;
		stats->total_lu_requests += thread_stats.request;
	}

	return ret;
}

void cls_stats_reset(void)
{
	/* reset what we can from hw */
	cls_lu_stats_reset();

	/* save snapshot of all other counters */
	__cls_stats_get(&rstats.stats);
	__cls_sig_cache_stats_get(CLS_SIG_CACHE1,
				  &rstats.scache_hit[CLS_SIG_CACHE1],
				  &rstats.scache_long_trans[CLS_SIG_CACHE1]);
	__cls_sig_cache_stats_get(CLS_SIG_CACHE2,
				  &rstats.scache_hit[CLS_SIG_CACHE2],
				  &rstats.scache_long_trans[CLS_SIG_CACHE2]);
}

s32 cls_stats_get(struct cls_stats *stats)
{
	if (unlikely(!stats)) {
		pr_err("Invalid stats parameter\n");
		return -EINVAL;
	}

	__cls_stats_get(stats);
	U32_STRUCT_DIFF(&rstats, stats, stats);

	return 0;
}

static void __cls_add_cmd_send(const struct cuckoo_hash_action *event)
{
	struct uc_cls_cmd uc_cmd;
	u32 i, reg, dst_q;
	s32 ret;

	pr_debug("CLS CMD add session\n");

	ret = pp_si_fld_get((void *)db->cls_si_req, SI_FLD_DST_QUEUE, &dst_q);
	if (unlikely(ret)) {
		db->cls_cmd_status = ret;
		return;
	}

	/* Write the hash results */
	uc_cmd.h1 = event->item.h1;
	uc_cmd.h2 = event->item.h2;
	uc_cmd.sig = event->item.sig;

	/* Write the command info */
	reg  = PP_FIELD_PREP(CLS_CMD_UPDT_SIG_ENT, 1);
	reg |= PP_FIELD_PREP(CLS_CMD_UPDT_DSI_INFO, 1);
	reg |= PP_FIELD_PREP(CLS_CMD_HASH_TBL, event->new_tbl);
	reg |= PP_FIELD_PREP(CLS_CMD_BKT_OFF, event->new_slot);
	reg |= PP_FIELD_PREP(CLS_CMD_SESS_ID, (unsigned long)event->item.value);
	uc_cmd.cmd_info = reg;

	/* Write DSI info */
	reg  = PP_FIELD_PREP(CLS_CMD_DSI_VVAL,    1);
	reg |= PP_FIELD_PREP(CLS_CMD_DSI_VWR,     1);
	reg |= PP_FIELD_PREP(CLS_CMD_DSI_AWR,     1);
	reg |= PP_FIELD_PREP(CLS_CMD_DSI_SWR,     1);
	reg |= PP_FIELD_PREP(CLS_CMD_DSI_DWR,     1);
	reg |= PP_FIELD_PREP(CLS_CMD_DSI_DQWR,    1);
	reg |= PP_FIELD_PREP(CLS_CMD_DSI_CNT_CLR, 1);
	reg |= PP_FIELD_PREP(CLS_CMD_DSI_DST_Q,   dst_q);
	uc_cmd.dsi_info = reg;

	/* Write SI */
	for (i = 0; i < PP_SI_WORDS_CNT; i++) {
		pr_debug("CLS_CMD_WORD[%-2u]:  0x%X\n", i, db->cls_si_req[i]);
		uc_cmd.si[i] = db->cls_si_req[i];
	}

	uc_cmd.cmd_id = CLS_CMD_IF_ADD;

	/* Execute command */
	ret = __cls_cmd_execute(&uc_cmd);
	if (unlikely(ret))
		db->cls_cmd_status = ret;
}

s32 cls_session_update(u32 id, u32 fv_sz, struct pp_hash *hash,
		       struct pp_hw_si *new_si)
{
	struct cuckoo_hash_action event;
	s32 ret;
	u32 s_id;
	u8 tbl_idx;
	u8 slot_idx;

	if (unlikely(ptr_is_null(hash)))
		return -EINVAL;

	/* store the si template for the event handling */
	db->cls_si_req = (u32 *)new_si;
	db->cls_cmd_status = 0;

	/* prepare the cuckoo item */
	if (id >= db->num_sessions) {
		pr_err("invalid session id %u, support %u sessions\n",
		       id, db->num_sessions);
		return -EINVAL;
	}

	event.item.key = (void *)CLS_FV_ADDR((unsigned long)(db->si_base_virt),
					     id, fv_sz);
	ret = cls_session_lookup_verbose(event.item.key, fv_sz, hash, &s_id,
					 &tbl_idx, &slot_idx);
	if (ret) {
		pr_err("cls_session_lookup_verbose failed session id %u\n", id);
		return -EINVAL;
	}

	if (id != s_id) {
		pr_err("found session id %u instead of %u\n", s_id, id);
		return -EINVAL;
	}

	__cls_cmd_lock();
	event.type = ITEM_ADDED;
	event.item.key_len = fv_sz;
	event.item.value = (void *)(unsigned long)id;
	event.item.h1 = hash->h1;
	event.item.h2 = hash->h2;
	event.item.sig = hash->sig;

	event.new_tbl = tbl_idx;
	event.new_slot = slot_idx;
	event.old_tbl = tbl_idx;
	event.old_slot = slot_idx;

	__cls_add_cmd_send(&event);
	__cls_cmd_unlock();

	return db->cls_cmd_status;
}

static void __cls_move_cmd_send(const struct cuckoo_hash_action *event)
{
	struct uc_cls_cmd uc_cmd;
	u32 reg;
	s32 ret;

	pr_debug("CLS CMD move session\n");

	/* Write the hash results */
	uc_cmd.h1 = event->item.h1;
	uc_cmd.h2 = event->item.h2;
	uc_cmd.sig = event->item.sig;

	/* Write the command info */
	reg  = PP_FIELD_PREP(CLS_CMD_UPDT_SIG_ENT,  1);
	reg |= PP_FIELD_PREP(CLS_CMD_HASH_TBL, event->new_tbl);
	reg |= PP_FIELD_PREP(CLS_CMD_BKT_OFF, event->new_slot);
	reg |= PP_FIELD_PREP(CLS_CMD_SESS_ID, (unsigned long)event->item.value);
	uc_cmd.cmd_info = reg;

	/* Write DSI info */
	uc_cmd.dsi_info = 0;

	uc_cmd.cmd_id = CLS_CMD_IF_UPDATE;

	/* Execute command */
	ret = __cls_cmd_execute(&uc_cmd);
	if (unlikely(ret))
		db->cls_cmd_status = ret;
}

static void __cls_rmv_cmd_send(const struct cuckoo_hash_action *event)
{
	struct uc_cls_cmd uc_cmd;
	u32 reg;
	s32 ret;

	pr_debug("CLS CMD remove session\n");

	/* Write the hash results */
	uc_cmd.h1 = event->item.h1;
	uc_cmd.h2 = event->item.h2;
	uc_cmd.sig = event->item.sig;

	pr_debug("CMD del session %lu, from tbl %hhu slot %hhu\n",
		 (unsigned long)event->item.value, event->old_tbl,
		 event->old_slot);

	/* Write the command info */
	reg  = PP_FIELD_PREP(CLS_CMD_UPDT_DSI_INFO,   1);
	reg |= PP_FIELD_PREP(CLS_CMD_SIG_INVALIDATE,  1);
	reg |= PP_FIELD_PREP(CLS_CMD_SESS_INVALIDATE, 1);
	reg |= PP_FIELD_PREP(CLS_CMD_HASH_TBL, event->old_tbl);
	reg |= PP_FIELD_PREP(CLS_CMD_BKT_OFF, event->old_slot);
	reg |= PP_FIELD_PREP(CLS_CMD_SESS_ID, (unsigned long)event->item.value);
	uc_cmd.cmd_info = reg;

	/* Write DSI info */
	reg  = PP_FIELD_PREP(CLS_CMD_DSI_VWR,     1);
	reg |= PP_FIELD_PREP(CLS_CMD_DSI_AWR,     1);
	reg |= PP_FIELD_PREP(CLS_CMD_DSI_SWR,     1);
	reg |= PP_FIELD_PREP(CLS_CMD_DSI_DWR,     1);
	reg |= PP_FIELD_PREP(CLS_CMD_DSI_DQWR,    1);
	reg |= PP_FIELD_PREP(CLS_CMD_DSI_CNT_CLR, 1);
	uc_cmd.dsi_info = reg;

	/* Kick command */
	uc_cmd.cmd_id = CLS_CMD_IF_DEL;

	/* Execute command */
	ret = __cls_cmd_execute(&uc_cmd);
	if (unlikely(ret))
		db->cls_cmd_status = ret;
}

static void __cls_cuckoo_event_handler(const struct cuckoo_hash_action *event)
{
	if (unlikely(db->cls_cmd_status)) {
		pr_err("CLS_HASH_EVENT_ERROR: abort event due to a previous error\n");
		return;
	}

	switch (event->type) {
	case ITEM_ADDED:
		__cls_add_cmd_send(event);
		pr_debug("CLS_HASH_ITEM_ADDED EVENT:   new_table %hhu new_slot %hhu\n",
			 event->new_tbl, event->new_slot);
		break;
	case ITEM_MOVED:
		__cls_move_cmd_send(event);
		pr_debug("CLS_HASH_ITEM_MOVED EVENT:   new_table %hhu new_slot %hhu old_table %hhu old_slot %hhu\n",
			 event->new_tbl, event->new_slot,
			 event->old_tbl, event->old_slot);
		break;
	case ITEM_REMOVED:
		__cls_rmv_cmd_send(event);
		pr_debug("CLS_HASH_ITEM_REMOVED EVENT: old_table %hhu old_slot %hhu\n",
			 event->old_tbl, event->old_slot);
		break;
	default:
		db->cls_cmd_status = -EINVAL;
		pr_debug("CLS_HASH_INVALID EVENT TYPE %d\n",
			 event->type);
		break;
	}
}

static s32 __cls_cuckoo_hash_init(u32 num_sessions)
{
	struct cuckoo_hash_param param;

	param.hash_bits    = ilog2(CLS_SESS_2_HASH_SZ(num_sessions));
	param.bucket_size  = CLS_HASH_BKT_SZ;
	param.max_replace  = CLS_CUCKOO_HASH_MAX_RPLC;
	param.lock_enable  = CLS_CUCKOO_HASH_LOCK_EN;
	param.hash_compute = CLS_CUCKOO_HASH_COMP_EN;
	param.lu_on_insert = CLS_CUCKOO_HASH_LU_INSR;
	param.cb           = __cls_cuckoo_event_handler;
#ifdef CONFIG_CUCKOO_HASH_DEBUG
	param.dbg_hist_sz  = CLS_CUCKOO_HASH_HIST_SZ;
#endif /* CONFIG_CUCKOO_HASH_DEBUG */
	db->cuckoo = cuckoo_hash_create(&param);
	if (unlikely(!db->cuckoo)) {
		pr_err("failed to create cuckoo hash\n");
		return -ENOSPC;
	}

	return 0;
}

s32 __cls_ddr_allocate(void)
{
	db->si_sz =
		PAGE_ALIGN((db->num_sessions) * sizeof(struct pp_hw_si));

	db->si_base_virt =
		pp_dma_alloc(db->si_sz, GFP_KERNEL, &db->si_base_phys, false);
	if (!db->si_base_virt) {
		pr_err("Could not allocate %u bytes for SI\n", (u32)db->si_sz);
		goto mem_alloc_failed_si;
	}

	db->ht1_sz = PAGE_ALIGN((db->num_sessions >> 1) * HT_ENTRY_SIZE);
	db->ht1_base_virt =
		pp_dma_alloc(db->ht1_sz, GFP_KERNEL, &db->ht1_base_phys, false);
	if (!db->ht1_base_virt) {
		pr_err("Could not allocate %u bytes for HT1\n",
		       (u32)db->ht1_sz);
		goto mem_alloc_failed_ht1;
	}

	db->ht2_sz = PAGE_ALIGN((db->num_sessions >> 1) * HT_ENTRY_SIZE);
	db->ht2_base_virt =
		pp_dma_alloc(db->ht2_sz, GFP_KERNEL, &db->ht2_base_phys, false);
	if (!db->ht2_base_virt) {
		pr_err("Could not allocate %u bytes for HT2\n",
		       (u32)db->ht2_sz);
		goto mem_alloc_failed_ht2;
	}

	memset(db->ht1_base_virt, 0, db->ht1_sz);
	memset(db->ht2_base_virt, 0, db->ht2_sz);

	return 0;

mem_alloc_failed_ht2:
	/* Free HT1 */
	pp_dma_free(db->ht1_sz, db->ht1_base_virt, &db->ht1_base_phys, false);
	db->ht1_base_virt = NULL;

mem_alloc_failed_ht1:
	/* Free SI */
	pp_dma_free(db->si_sz, db->si_base_virt, &db->si_base_phys, false);
	db->si_base_virt = NULL;

mem_alloc_failed_si:
	return -ENOMEM;
}

void __cls_ddr_free(void)
{
	/* Free SI */
	if (db->si_base_virt) {
		pp_dma_free(db->si_sz, db->si_base_virt, &db->si_base_phys,
			    false);
	}

	/* Free HT1 */
	if (db->ht1_base_virt) {
		pp_dma_free(db->ht1_sz, db->ht1_base_virt, &db->ht1_base_phys,
			    false);
	}

	/* Free HT2 */
	if (db->ht2_base_virt) {
		pp_dma_free(db->ht2_sz, db->ht2_base_virt, &db->ht2_base_phys,
			    false);
	}
}

void cls_exit(void)
{
	if (!db)
		return;

	__cls_ddr_free();
	cls_dbg_clean();
	if (unlikely(cuckoo_hash_destroy(db->cuckoo)))
		pr_err("failed to destroy cuckoo hash\n");
	kfree(db);
	db = NULL;
}

s32 cls_init(struct pp_cls_init_param *init_param)
{
	u32 mask[CLS_FV_MSK_NUM_REGS];
	s32 ret = 0;

	if (unlikely(ptr_is_null(init_param)))
		return -EINVAL;

	if (!init_param->valid)
		return -EINVAL;

	pr_debug("base_addr %#llx, num_sessions %u\n",
		 init_param->cls_base, init_param->num_sessions);

	if (unlikely(!is_power_of_2(init_param->num_sessions))) {
		pr_err("number of sessions must be power of 2, %u\n",
		       init_param->num_sessions);
		return -EINVAL;
	}

	/* Allocate cuckoo hash context */
	db = kzalloc(sizeof(*db), GFP_KERNEL);
	if (unlikely(!db)) {
		pr_err("cannot allocate database\n");
		return -ENOMEM;
	}

	/* set db */
	db->num_sessions = init_param->num_sessions;
	db->num_lu_threads = init_param->num_lu_threads;

	/* set default polling retries */
	db->num_retries = PP_REG_DFLT_RETRIES;

	/* Allocate ddr */
	ret = __cls_ddr_allocate();
	if (unlikely(ret)) {
		pr_err("__cls_ddr_allocate failed\n");
		return ret;
	}

	/* set HW base address */
	cls_base_addr       = init_param->cls_base;
	cls_cache_base_addr = init_param->cls_cache_base;

	/* set the thread config */
	cls_thrd_cfg_set(CLS_DRP_FLW_DIS, CLS_SRCH_MOD_NORMAL,
			 CLS_FM_FLW_FULL, CLS_FV_MASKED);

	/* select the hash functions */
	ret = cls_hash_select_set(CLS_DFLT_SIG_SEL, CLS_DFLT_HASH_SEL);
	if (unlikely(ret))
		return ret;

	/* set fv mask */
	memset(mask, 0, sizeof(mask));
	ret = cls_fv_mask_set(mask);
	if (unlikely(ret))
		return ret;

	/* set number of lookup threads */
	ret = cls_num_lu_thread_set(db->num_lu_threads);
	if (unlikely(ret))
		return ret;

	/* init session cache */
	__cls_sess_cache_init();

	/* set lookup long process threshold */
	cls_long_lu_thr_set(CLS_DFLT_LONG_LU_THR);

	/* reset lookup counters */
	cls_lu_stats_reset();

	/* set default si */
	__cls_dflt_si_set();

	/* set hash table 1 base address */
	cls_hash1_base_addr_set(db->ht1_base_phys);

	/* set hash table 2 base address */
	cls_hash2_base_addr_set(db->ht2_base_phys);

	/* set si base address */
	cls_si_base_addr_set(db->si_base_phys);

	/* set hash tables size */
	ret = cls_hash_size_set(CLS_SESS_2_HASH_SZ(init_param->num_sessions));
	if (unlikely(ret))
		return ret;

	/* set field vector endianness */
	cls_fv_endian_swap_set(CLS_NO_ENDIAN_SWP, CLS_NO_ENDIAN_SWP);

	/* init the cuckoo hash database */
	ret = __cls_cuckoo_hash_init(init_param->num_sessions);
	if (unlikely(ret))
		return ret;

	/* init debugfs */
	ret = cls_dbg_init(init_param->dbgfs);
	if (unlikely(ret))
		return ret;

	pr_debug("classifier initialized\n");
	return 0;
}

s32 cls_hash_calc(void *fv, u32 fv_sz, struct pp_hash *hash)
{
	u8 idx;
	u32 fv_full[CLS_DBG_FV_NUM_REGS] = {0};
	u32 off, sz, reg = 0;

	if (pp_silicon_step_get() == PP_SSTEP_A) {
		pr_err("hash calc is not supported in A step\n");
		return -EINVAL;
	}

	if (unlikely(ptr_is_null(fv)) || unlikely(ptr_is_null(hash)))
		return -EINVAL;

	if (fv_sz > (4 * CLS_DBG_FV_NUM_REGS)) {
		pr_err("fv_sz %u too big\n", fv_sz);
		return -EINVAL;
	}

	off = CLS_DBG_FV_NUM_REGS - (fv_sz / 4);
	sz = sizeof(fv_full) - (off * 4);
	__buf_swap(&fv_full[off], sizeof(fv_full), fv, fv_sz);

	/* Write FV */
	for (idx = 0; idx < CLS_DBG_FV_NUM_REGS; idx++)
		PP_REG_WR32(PP_CLS_FV_TO_HASH_CALC_REG + (4 * idx),
			    fv_full[idx]);

	reg = PP_FIELD_PREP(PP_CLS_GO_CALC_MSK, 0x1);
	PP_REG_WR32(PP_CLS_GO_CALC_REG, reg);

	/* Wait for done bit */
	if (pp_reg_poll(PP_CLS_GO_CALC_REG, PP_CLS_GO_CALC_GO_READY_MSK, 1)) {
		pr_err("Failed polling on classifier calc reg\n");
		return -EIO;
	}

	hash->h1 = PP_REG_RD32(PP_CLS_CALC_HASH_1_REG);
	hash->h2 = PP_REG_RD32(PP_CLS_CALC_HASH_2_REG);
	hash->sig = PP_REG_RD32(PP_CLS_CALC_HASH_SIG_REG);

	return 0;
}
