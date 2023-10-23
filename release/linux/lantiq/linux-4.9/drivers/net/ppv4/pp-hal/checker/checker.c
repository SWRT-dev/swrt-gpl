/*
 * Description: PP Checker Driver
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018-2020 Intel Corporation
 */

#define pr_fmt(fmt) "[CHK]:%s:%d: " fmt, __func__, __LINE__

#include <linux/kernel.h>
#include <linux/bitops.h>
#include <linux/spinlock.h>
#include <linux/types.h>
#include "pp_common.h"
#include "pp_regs.h"
#include "pp_buffer_mgr.h"
#include "checker_regs.h"
#include "checker_internal.h"
#include "checker.h"
#include "infra.h"
#include "pp_si.h"
#include "uc.h"

/**
 * @brief Defines the default crawler scan time in microseconds for
 *        the maximum iteration size 'CHK_CRWLR_MAX_ITR_SIZE'
 */
#define CRWLR_DFLT_SCAN_TIME_US (32000U)

struct chk_database {
	spinlock_t lock;      /*! protecting hw and DB accesses */
	u32        n_sessions;
	u32        n_syncqs;
	u32        hw_clk;   /*! hw clock in MHZ */
	void      *dsi_base_virt;
	dma_addr_t dsi_base_phys;
	size_t     dsi_sz;
	/* shadow DB for exception sessions to be used by FWs */
	void      *exp_base_virt;
	dma_addr_t exp_base_phys;
	size_t     exp_sz;
	bool       nf_reass_en;
	 /*! specify which exception session is used */
	ulong      excp_sess_bmap[BITS_TO_LONGS(CHK_NUM_EXCEPTION_SESSIONS)];
};

/**
 * @brief Defines all statistics required for implementing SW reset
 *        statistics
 */
static struct chk_stats rstats;

/**
 * @brief module database
 */
static struct chk_database *db;

/**
 * @brief checker HW base address
 */
u64 chk_base_addr;
u64 chk_ram_base_addr;
u64 chk_cache_base_addr; /* counters cache */

/**
 * @brief Acquire checker lock
 */
void __chk_lock(void)
{
	pr_debug("caller %ps\n", __builtin_return_address(0));
	/* we are not allowed to run in irq context */
	WARN(in_irq(), "checker shouldn't be used from irq context\n");
	spin_lock_bh(&db->lock);
}

/**
 * @brief Release checker lock
 */
void __chk_unlock(void)
{
	pr_debug("caller %ps\n", __builtin_return_address(0));
	spin_unlock_bh(&db->lock);
}

dma_addr_t chk_excp_si_phys_base_get(void)
{
	if (unlikely(!db))
		return 0;

	return pp_virt_to_phys((void *)(ulong)CHK_ALT_SI_RAM(0));
}

dma_addr_t chk_sh_excp_si_phys_base_get(void)
{
	if (unlikely(!db))
		return 0;

	return db->exp_base_phys;
}

dma_addr_t chk_dsi_phys_base_get(void)
{
	if (unlikely(!db))
		return 0;

	return db->dsi_base_phys;
}

/**
 * @brief Dispatch mailbox command to the checker
 * @note MUST be called under a lock
 * @param id session id
 * @param cmd the command
 */
static inline void __chk_cmd_dispatch(u32 id, u32 cmd)
{
	struct uc_chk_cmd uc_cmd;
	s32 ret;

	uc_cmd.id = id;
	uc_cmd.cmd = cmd;
	pr_debug("id %u, cmd %#x\n", id, cmd);

	/* send command */
	ret = uc_egr_mbox_cmd_send(UC_CMD_CHK_MBX, 0, (const void *)&uc_cmd,
				   sizeof(uc_cmd), NULL, 0);
	if (ret)
		pr_err("UC_CMD_CHK_MBX command failed\n");
}

/**
 * @brief Test whether session id is valid or not, session index
 *        must be verified due to we use it to access the HW
 * @param id session id to test
 * @return bool true if the id is valid, false otherwise
 */
static inline bool __chk_is_sess_id_valid(u32 id)
{
	if (likely(id < db->n_sessions))
		return true;

	pr_err("invalid session id %u, max sessions id %u\n", id,
	       db->n_sessions - 1);
	return false;
}

static inline bool __chk_is_exp_sess_id_valid(u8 id)
{
	if (likely(PP_IS_EXCEPTION_ID_VALID(id)))
		return true;

	pr_err("Invalid exception session id %u\n", id);
	return -EINVAL;
}

static s32 __chk_session_dsi_evict(u32 id)
{
	dma_addr_t phys;
	ulong mask = PP_CNTR_CACHE_SINGLE_OP_STATUS_STATUS_SINGLE_OP_MSK;

	phys = db->dsi_base_phys + CHK_DSI_OFFSET_GET(id);

	/* evict */
	__chk_lock();

	/* DSI size (16 bytes) is exactly 1 cache line, so we evict 1 line */
	PP_REG_WR32(PP_CNTR_CACHE_EVICT_ADDR_REG, phys >> 4);
	if (unlikely(pp_reg_poll(PP_CNTR_CACHE_SINGLE_OP_STATUS_REG,
				 mask, 0))) {
		__chk_unlock();
		pr_err("cache is busy, cannot run single op\n");
		return -EBUSY;
	}
	__chk_unlock();

	return 0;
}

s32 chk_session_dsi_get(u32 id, struct pp_hw_dsi *dsi)
{
	void *virt;

	if (unlikely(ptr_is_null(dsi)))
		return -EINVAL;
	if (unlikely(!__chk_is_sess_id_valid(id)))
		return -EINVAL;
	pr_debug("id %u, dsi %p\n", id, dsi);

	if (pp_silicon_step_get() == PP_SSTEP_A) {
		virt = db->dsi_base_virt + CHK_DSI_OFFSET_GET(id);
		__chk_session_dsi_evict(id);
		memcpy(dsi, virt, sizeof(*dsi));
	} else {
		__chk_lock();
		PP_REG_WR32(PP_CHK_MBX_CRWLR_R3_REG, 0);
		__chk_cmd_dispatch(id, CHK_MBOX_CMD_READ);
		/* poll till reg != 0 */
		pp_reg_poll_ne(PP_CHK_MBX_CRWLR_R3_REG, ~0U, 0);
		PP_REG_RD_REP(PP_CHK_MBX_CRWLR_R0_REG, dsi, sizeof(dsi->word));
		__chk_unlock();
	}

	return 0;
}

s32 chk_session_dsi_q_update(u32 id, u16 q)
{
	u32 cmd;

	if (unlikely(!__chk_is_sess_id_valid(id)))
		return -EINVAL;

	pr_debug("id %u, queue %hu\n", id, q);

	cmd = CHK_MBOX_CMD_NORMAL;
	cmd = PP_FIELD_MOD(CHK_CMD_NORMAL_DQ_WR_MSK, 1, cmd);
	cmd = PP_FIELD_MOD(CHK_CMD_NORMAL_DST_Q_MSK, q, cmd);
	__chk_lock();
	__chk_cmd_dispatch(id, cmd);
	__chk_unlock();

	return 0;
}

s32 chk_session_stats_reset(u32 id)
{
	u32 cmd;

	if (unlikely(!__chk_is_sess_id_valid(id)))
		return -EINVAL;

	pr_debug("id %u\n", id);

	cmd = CHK_MBOX_CMD_NORMAL;
	cmd = PP_FIELD_MOD(CHK_CMD_NORMAL_CNT_CLR_MSK, 1, cmd);
	__chk_lock();
	__chk_cmd_dispatch(id, cmd);
	__chk_unlock();

	return 0;
}

s32 chk_session_stats_mod(u32 id, bool add, u8 pkts, u32 bytes, bool excp)
{
	u32 cmd;

	if (unlikely(!__chk_is_sess_id_valid(id)))
		return -EINVAL;

	if (unlikely(!pkts || !bytes)) {
		pr_err("pkts %u, bytes %u, both MUST be non-zero value\n",
		       pkts, bytes);
		return -EINVAL;
	}

	pr_debug("id %u, inc %s, pkts %u, bytes %u, excp %s\n",
		 id, BOOL2STR(add), pkts, bytes, BOOL2STR(excp));

	cmd = CHK_MBOX_CMD_COUNTERS;
	cmd = PP_FIELD_MOD(CHK_CMD_CNTS_BYTES_MSK,    bytes, cmd);
	cmd = PP_FIELD_MOD(CHK_CMD_CNTS_PKTS_MSK,     pkts,  cmd);
	cmd = PP_FIELD_MOD(CHK_CMD_CNTS_EXP_SESS_MSK, excp,  cmd);
	cmd = PP_FIELD_MOD(CHK_CMD_CNTS_INC_MSK,      add,   cmd);
	__chk_lock();
	__chk_cmd_dispatch(id, cmd);
	__chk_unlock();

	return 0;
}

/**
 * @brief Init crawler HW
 */
static inline void __chk_crawler_init(void)
{
	struct crawler_cfg cfg;
	int    ret = 0;

	ret = chk_crawler_get(&cfg);
	if (unlikely(ret)) {
		pr_err("failed to get crawler config!\n");
		return;
	}

	/* Configure the crawler to return stale sessions absolute indexes
	 * e.g. if crawler start scanning from session 1000 and found
	 *      that session 1005 and 1020 are staled, he will set bits
	 *      1005 and 1020 in the bitmap
	 */
	cfg.ddr_mode = 0;

	/* Configure pace to 1 */
	cfg.timer_pace = PP_CHK_MBX_CRWLR_CTRL_MBX_TIMER_PACE_RST;

	/* Configure the crawler to iterate over 8K sessions in ~80ms */
	cfg.timer_init_val =
	US_TO_HW_CYCLES(db->hw_clk, CRWLR_DFLT_SCAN_TIME_US) /
		CHK_CRWLR_MAX_ITR_SIZE;

	ret = chk_crawler_set(&cfg);
	if (unlikely(ret)) {
		pr_err("failed to set crawler config!\n");
		return;
	}

	pr_debug("timer %u(%#x)\n", cfg.timer_init_val, cfg.timer_init_val);
}

/**
 * @brief Set crawler configuration
 * @return 0 for success
 */
s32 chk_crawler_set(struct crawler_cfg *cfg)
{
	u32 reg = 0;

	if (unlikely(ptr_is_null(cfg)))
		return -EINVAL;

	reg = PP_FIELD_MOD(PP_CHK_MBX_CRWLR_TIMER_INIT_MBX_TIMER_INIT_VAL_MSK,
			   cfg->timer_init_val, reg);
	PP_REG_WR32(PP_CHK_MBX_CRWLR_TIMER_INIT_REG, reg);

	reg = 0;
	reg = PP_FIELD_MOD(PP_CHK_MBX_CRWLR_CTRL_MBX_TIMER_PACE_MSK,
			   cfg->timer_pace, reg);

	reg = PP_FIELD_MOD(PP_CHK_MBX_CRWLR_CTRL_MBX_CRWLR_DDR_MODE_MSK,
			   cfg->ddr_mode, reg);

	reg = PP_FIELD_MOD(PP_CHK_MBX_CRWLR_CTRL_MBX_CRWLR_RESET_MSK,
			   cfg->reset, reg);

	PP_REG_WR32(PP_CHK_MBX_CRWLR_CTRL_REG, reg);

	return 0;
}

/**
 * @brief Get crawler configuration
 * @return 0 for success
 */
s32 chk_crawler_get(struct crawler_cfg *cfg)
{
	u32 reg;

	if (unlikely(ptr_is_null(cfg)))
		return -EINVAL;

	reg = PP_REG_RD32(PP_CHK_MBX_CRWLR_TIMER_INIT_REG);
	cfg->timer_init_val  =
	PP_FIELD_GET(PP_CHK_MBX_CRWLR_TIMER_INIT_MBX_TIMER_INIT_VAL_MSK, reg);

	reg = PP_REG_RD32(PP_CHK_MBX_CRWLR_STATUS_REG);
	cfg->idle_status = !!PP_FIELD_GET(CHK_CRWLR_STATUS_IDLE, reg);
	cfg->wait_status = !!PP_FIELD_GET(CHK_CRWLR_STATUS_WAIT, reg);
	cfg->command_counter = PP_FIELD_GET(CHK_CRWLR_STATUS_CMD_CNTR, reg);
	cfg->wait_for_final_status = !!PP_FIELD_GET(CHK_CRWLR_STATUS_WAIT_FINAL,
						    reg);
	cfg->dsi_sync_loss_error =
	!!PP_FIELD_GET(CHK_CRWLR_STATUS_DSI_SYNC_LOSS, reg);

	reg = PP_REG_RD32(PP_CHK_MBX_CRWLR_CTRL_REG);
	cfg->timer_pace =
	PP_FIELD_GET(PP_CHK_MBX_CRWLR_CTRL_MBX_TIMER_PACE_MSK, reg);

	cfg->ddr_mode =
	!!PP_FIELD_GET(PP_CHK_MBX_CRWLR_CTRL_MBX_CRWLR_DDR_MODE_MSK, reg);

	cfg->reset =
	!!PP_FIELD_GET(PP_CHK_MBX_CRWLR_CTRL_MBX_CRWLR_RESET_MSK, reg);

	return 0;
}

/**
 * @brief Test whether the crawler in idle state and can receive new command
 * @return bool true if crawler is idle, false otherwise
 */
static inline bool __chk_is_crawler_idle(void)
{
	u32 reg = PP_REG_RD32(PP_CHK_MBX_CRWLR_STATUS_REG);

	return !!PP_FIELD_GET(CHK_CRWLR_STATUS_IDLE, reg);
}

/**
 * @brief Test whether the crawler in busy state
 * @return bool true if crawler is busy, false otherwise
 */
static inline bool __chk_is_crawler_busy(void)
{
	return !__chk_is_crawler_idle();
}

/**
 * @brief Test whether crawler iteration size is valid
 * @param it_sz iteration size to test
 * @return bool true is it_sz is valid, false otherwise
 */
static inline bool __chk_is_iteration_size_valid(u32 it_sz)
{
	if (likely(it_sz <= CHK_CRWLR_MAX_ITR_SIZE))
		return true;

	pr_err("invalid crawler iteration size %u, max value: %u\n",
	       it_sz, CHK_CRWLR_MAX_ITR_SIZE);
	return false;
}

/**
 * @brief Round up iteration size according to HW limitations
 * @param it_sz iteration size to roundup
 * @return u32 new iterations size
 */
static inline u32 __chk_roundup_iteration_sz(u32 it_sz)
{
	u32 _it_sz;

	/* at least CHK_CRWLR_MIN_ITR_SIZE */
	_it_sz = max(it_sz, CHK_CRWLR_MIN_ITR_SIZE);
	/* no more than CHK_CRWLR_MAX_ITR_SIZE */
	_it_sz = min(_it_sz, CHK_CRWLR_MAX_ITR_SIZE);
	/* roundup to the next power of 2 */
	_it_sz = roundup_pow_of_two(_it_sz);

	pr_debug("it_sz: before %u, after it_sz %u\n", it_sz, _it_sz);
	return _it_sz;
}

/**
 * @brief Convert crawler iteration size to HW value for configuring
 *        crawler registers
 * @param it_sz iteration size to convert
 */
static inline u32 __chk_iteration_sz_to_hw(u32 it_sz)
{
	return ilog2(it_sz) - ilog2(CHK_CRWLR_MIN_ITR_SIZE);
}

s32 chk_scan_time_get(u32 *n_sessions, u32 *time)
{
	u32 reg, timer, pace, cycles;

	if (unlikely(ptr_is_null(time)))
		return -EINVAL;
	if (unlikely(!__chk_is_iteration_size_valid(*n_sessions)))
		return -EINVAL;

	/* use 8 times timer values to make sure it will also work
	 * with high load
	 */
	timer = 8 * PP_REG_RD32(PP_CHK_MBX_CRWLR_TIMER_INIT_REG);
	reg   = PP_REG_RD32(PP_CHK_MBX_CRWLR_CTRL_REG);
	pace  = PP_FIELD_GET(PP_CHK_MBX_CRWLR_CTRL_MBX_TIMER_PACE_MSK, reg);
	if (unlikely(pace == 0)) {
		pr_err("crawler pace is ZERO!!!!\n");
		return -EPERM;
	}

	*n_sessions = __chk_roundup_iteration_sz(*n_sessions);
	cycles      = (timer / pace) * (*n_sessions);
	*time       = HW_CYCLES_TO_US(db->hw_clk, cycles);

	pr_debug("reg %#x, pace %u, timer %u, n_sessions %u, cycles %u, time %u\n",
		 reg, pace, timer, *n_sessions, cycles, *time);

	return 0;
}

/**
 * @brief Set cache bypass window where the all checker access
 *        will bypass the cache
 * @param base physical base address
 * @param sz window size
 */
static inline void __chk_cache_bypass_win_set(dma_addr_t base, size_t sz)
{
	pr_debug("base %pad, sz = %zu, (base + sz) = %#llx\n", &base, sz,
			 (u64)base + (u64)sz);
	PP_REG_WR32(PP_CNTR_CACHE_BYPASS_WIN_LOW_REG, base >> 4);
	PP_REG_WR32(PP_CNTR_CACHE_BYPASS_WIN_HIGH_REG, (base + sz) >> 4);
	PP_REG_WR32(PP_CNTR_CACHE_BYPASS_WIN_VALID_REG, 1);
}

s32 chk_crawler_start(u32 base_id, u32 n_sessions, dma_addr_t phy_map,
		      size_t sz)
{
	u32 cmd, it_sz;

	if (unlikely(!__chk_is_sess_id_valid(base_id)))
		return -EINVAL;

	it_sz = __chk_roundup_iteration_sz(n_sessions);
	it_sz = __chk_iteration_sz_to_hw(it_sz);
	pr_debug("base_id %u, n_sessions %u, db->n_sessions %u, it_sz %u, phy_map %pad, sz %zu\n",
		 base_id, n_sessions, db->n_sessions, it_sz, &phy_map, sz);

	/* prepare the command */
	cmd = CHK_MBOX_CMD_CRAWLER;
	cmd = PP_FIELD_MOD(CHK_CMD_CRWLR_ITER_SZ_MSK, it_sz, cmd);
	/* dispatch */
	__chk_lock();
	if (__chk_is_crawler_busy()) {
		__chk_unlock();
		return -EBUSY;
	}
	/* set the cache to bypass the bitmap */
	__chk_cache_bypass_win_set(phy_map, sz);
	/* set address where the crawler will copy the stalled sessions */
	PP_REG_WR32(PP_CHK_MBX_CRWLR_DDR_BASE_REG, phy_map >> 4);
	__chk_cmd_dispatch(base_id, cmd);
	__chk_unlock();

	return 0;
}

s32 chk_crawler_idle_wait(u32 *attempts)
{
	s32 ret;

	ret = pp_raw_reg_poll(PP_CHK_MBX_CRWLR_STATUS_REG,
			      CHK_CRWLR_STATUS_IDLE, 1,
			      PP_REG_MAX_RETRIES, attempts,
			      NULL, PP_POLL_EQ);
	if (unlikely(ret)) {
		pr_err("crawler stuck in busy state\n");
		return -EBUSY;
	}

	ret = pp_raw_reg_poll(PP_CHK_MBX_CRWLR_STATUS_REG,
			      CHK_CRWLR_STATUS_CMD_CNTR, 0,
			      PP_REG_MAX_RETRIES, attempts,
			      NULL, PP_POLL_EQ);
	if (unlikely(ret)) {
		pr_err("crawler stuck in busy state\n");
		return -EBUSY;
	}

	return ret;
}

static s32 __chk_exception_session_si_get(u32 id, struct pp_si *alt_si)
{
	struct pp_hw_si hw_si;
	s32 ret;

	/* Get alt si address */
	PP_REG_RD_REP(CHK_ALT_SI_RAM(id), &hw_si,
		      PP_SI_TOP_WORDS_CNT * sizeof(u32));

	ret = pp_si_decode_top(alt_si, &hw_si);
	if (unlikely(ret))
		pr_err("pp_si_decode_top failed for session %u\n", id);

	return ret;
}

static s32 __chk_exception_session_si_set(u32 id, struct pp_si *alt_si)
{
	struct pp_hw_si hw_si;
	u32 i;
	s32 ret;
	u32 *ddr_addr, *sram_addr;

	memset(&hw_si, 0, sizeof(hw_si));
	ret = pp_si_encode_top(&hw_si, alt_si);
	if (unlikely(ret)) {
		pr_err("pp_si_encode_top failed for session %u\n", id);
		return ret;
	}

	ddr_addr = db->exp_base_virt + CHK_EXCEPTION_SI_OFF(id);
	sram_addr = (u32 *)CHK_ALT_SI_RAM(id);

	/* Write alt si to checker ram */
	for (i = 0 ; i < PP_SI_TOP_WORDS_CNT ; i++, sram_addr++, ddr_addr++) {
		PP_REG_WR32(sram_addr, hw_si.word[i]);

		/* for a-step we keep the shadow alt si updated */
		if (pp_silicon_step_get() == PP_SSTEP_A)
			*ddr_addr = hw_si.word[i];
	}

	return 0;
}

static void __chk_reassembly_nf_set(void)
{
	struct pp_si alt_si;
	u32 id;

	__chk_lock();
	if (db->nf_reass_en)
		goto unlock;

	for_each_set_bit(id, db->excp_sess_bmap, CHK_NUM_EXCEPTION_SESSIONS) {

		/* read exception SI */
		__chk_exception_session_si_get(id, &alt_si);

		/* set frags divert flag */
		alt_si.chck_flags |= BIT(SI_CHCK_FLAG_DVRT_FRAG);

		/* write exception SI */
		__chk_exception_session_si_set(id, &alt_si);
	}

	db->nf_reass_en = true;
unlock:
	__chk_unlock();
}

s32 chk_nf_set(u16 queue, enum pp_nf_type nf)
{
	u32 reg_val;

	if (nf >= PP_NF_NUM) {
		pr_err("Invalid nf_queue id %u\n", nf);
		return -EINVAL;
	}

	switch (nf) {
	case PP_NF_REASSEMBLY:
		reg_val = PP_REG_RD32(PP_CHK_NF_Q0_REG);
		reg_val = PP_FIELD_MOD(PP_CHK_NF_Q0_MSK,
				       queue, reg_val);
		PP_REG_WR32(PP_CHK_NF_Q0_REG, reg_val);
		__chk_reassembly_nf_set();
		break;

	case PP_NF_TURBODOX:
		reg_val = PP_REG_RD32(PP_CHK_NF_Q0_REG);
		reg_val = PP_FIELD_MOD(PP_CHK_NF_Q0_NF_Q1_MSK,
				       queue, reg_val);
		PP_REG_WR32(PP_CHK_NF_Q0_REG, reg_val);
		break;

	case PP_NF_FRAGMENTER:
		reg_val = PP_REG_RD32(PP_CHK_NF_Q1_REG);
		reg_val = PP_FIELD_MOD(PP_CHK_NF_Q1_NF_Q2_MSK,
				       queue, reg_val);
		PP_REG_WR32(PP_CHK_NF_Q1_REG, reg_val);
		break;

	case PP_NF_MULTICAST:
		reg_val = PP_REG_RD32(PP_CHK_NF_Q1_REG);
		reg_val = PP_FIELD_MOD(PP_CHK_NF_Q1_NF_Q3_MSK,
				       queue, reg_val);
		PP_REG_WR32(PP_CHK_NF_Q1_REG, reg_val);
		break;

	case PP_NF_IPSEC:
		/* Nothing to do for IPSEC NF */
		break;

	default:
		pr_err("Unknown nf_queue id\n");
	}

	return 0;
}

s32 chk_mtu_set(u16 port, u32 mtu)
{
	u32 old_mtu;

	if (!PP_IS_PORT_VALID(port)) {
		pr_err("Invalid port %u\n", port);
		return -EINVAL;
	}

	old_mtu = PP_REG_RD32(CHK_MTU_RAM(port));
	PP_REG_WR32(CHK_MTU_RAM(port), mtu);

	pr_debug("Setting port %u mtu to %u (previous %u)\n",
		 port, mtu, old_mtu);

	return 0;
}

s32 chk_exception_session_set(struct exception_session_cfg *cfg)
{
	struct pp_si alt_si;
	s32 ret;

	if (!__chk_is_exp_sess_id_valid(cfg->id))
		return -EINVAL;

	if (!PP_IS_PORT_VALID(cfg->port)) {
		pr_err("Invalid port %u\n", cfg->port);
		return -EINVAL;
	}

	if (!PP_IS_COLOR_VALID(cfg->color)) {
		pr_err("Invalid color %u\n", cfg->color);
		return -EINVAL;
	}

	pr_debug("id %u. eg_port %u. color %u\n",
		 cfg->id, cfg->port, cfg->color);

	memset(&alt_si, 0, sizeof(struct pp_si));
	alt_si.recipe_idx  = CHK_EXCEPTION_RECIPE;
	alt_si.sess_id     = cfg->id;
	alt_si.ps_off      = PP_INVALID_PS_OFF;
	alt_si.color       = cfg->color;
	alt_si.dst_q       = cfg->queue;
	alt_si.tmpl_ud_sz  = CHK_EXCEPTION_TMP_UD_SZ;
	alt_si.trim_l3_id  = SI_NO_TRIM_VALUE;
	alt_si.chck_flags  = cfg->flags;

	ret = pp_si_sgc_info_set(cfg->sgc, ARRAY_SIZE(alt_si.sgc), &alt_si);
	if (ret) {
		pr_err("pp_si_sgc_info_set failed\n");
		return ret;
	}

	ret = pp_si_tbm_info_set(cfg->tbm, ARRAY_SIZE(alt_si.tbm), &alt_si);
	if (ret) {
		pr_err("pp_si_tbm_info_set failed\n");
		return ret;
	}

	ret = pp_si_port_info_set(PP_PORT_INVALID, cfg->port, &alt_si);
	if (ret) {
		pr_err("pp_si_port_info_set failed\n");
		return ret;
	}

	__chk_lock();
	if (db->nf_reass_en)
		alt_si.chck_flags |= BIT(SI_CHCK_FLAG_DVRT_FRAG);

	ret = __chk_exception_session_si_set(cfg->id, &alt_si);
	if (unlikely(ret)) {
		__chk_unlock();
		return ret;
	}

	set_bit(cfg->id, db->excp_sess_bmap);
	__chk_unlock();


	return 0;
}

s32 chk_exception_session_get(struct exception_session_cfg *cfg)
{
	struct pp_si alt_si;
	s32 ret;

	if (unlikely(!__chk_is_exp_sess_id_valid(cfg->id)))
		return -EINVAL;

	ret = __chk_exception_session_si_get(cfg->id, &alt_si);
	if (unlikely(ret))
		return ret;

	cfg->port = alt_si.eg_port;
	cfg->queue = alt_si.dst_q;
	cfg->color = alt_si.color;
	memcpy(cfg->sgc, &alt_si.sgc, sizeof(alt_si.sgc));
	memcpy(cfg->tbm, &alt_si.tbm, sizeof(alt_si.tbm));
	cfg->flags = alt_si.chck_flags;

	return 0;
}

s32 chk_exception_session_si_get(u32 id, struct pp_si *alt_si)
{
	if (unlikely(ptr_is_null(alt_si)))
		return -EINVAL;

	if (unlikely(!__chk_is_exp_sess_id_valid(id)))
		return -EINVAL;

	return __chk_exception_session_si_get(id, alt_si);
}

s32 chk_exception_session_si_set(u32 id, struct pp_si *alt_si)
{
	s32 ret;

	if (unlikely(ptr_is_null(alt_si)))
		return -EINVAL;

	if (unlikely(!__chk_is_exp_sess_id_valid(id)))
		return -EINVAL;

	__chk_lock();
	ret = __chk_exception_session_si_set(id, alt_si);
	__chk_unlock();

	return ret;
}

static void __chk_map_set(u16 port, u8 tc, u8 hoh, u8 id)
{
	unsigned long mapping_base_addr;
	u32 mapping_val;

	pr_debug("port %u. tc %u. hoh %u. id %u\n", port, tc, hoh, id);

	mapping_base_addr = CHK_ALT_SI_MAP_RAM(port, tc);
	mapping_val = PP_REG_RD32(mapping_base_addr);
	mapping_val &= ~(0xFF << (BITS_PER_BYTE * hoh));
	mapping_val |= id << (BITS_PER_BYTE * hoh);
	PP_REG_WR32(mapping_base_addr, mapping_val);
}

s32 chk_exception_session_map_set(u16 port, u8 tc, u8 hoh, u8 id)
{
	if (id >= (CHK_NUM_EXCEPTION_SESSIONS - 1)) {
		pr_err("Exception id %d exceeds maximum of %d\n",
		       id, CHK_NUM_EXCEPTION_SESSIONS - 1);
		return -EINVAL;
	}

	if (!PP_IS_PORT_VALID(port)) {
		pr_err("Invalid port %u\n", port);
		return -EINVAL;
	}

	if (!PP_IS_TC_VALID(tc)) {
		pr_err("Invalid tc %u\n", tc);
		return -EINVAL;
	}

	if (!PP_IS_HOH_VALID(hoh)) {
		pr_err("Invalid hoh %u\n", hoh);
		return -EINVAL;
	}

	__chk_map_set(port, tc, hoh, id);

	return 0;
}

s32 chk_exception_session_map_get(u16 port, u8 tc, u8 hoh, u8 *id)
{
	unsigned long mapping_base_addr;
	u32 mapping_val;

	if (port >= PP_MAX_PORT) {
		pr_err("Invalid port %u\n", port);
		return -EINVAL;
	}

	if (tc >= PP_MAX_TC) {
		pr_err("Invalid tc %u\n", tc);
		return -EINVAL;
	}

	if (hoh >= CHK_HOH_MAX) {
		pr_err("Invalid hoh %u\n", hoh);
		return -EINVAL;
	}

	mapping_base_addr = CHK_ALT_SI_MAP_RAM(port, tc);
	mapping_val = PP_REG_RD32(mapping_base_addr);

	*id = (mapping_val >> (BITS_PER_BYTE * hoh)) & 0xFF;

	pr_debug("port %u. tc %u. hoh %u. id %u\n",
		 port, tc, hoh, *id);

	return 0;
}

s32 chk_exception_session_port_map_set(u16 port,
				       u8 id[PP_MAX_TC][CHK_HOH_MAX])
{
	u8 tc_ind;
	u8 hoh_ind;
	u8 max_excep_sess = CHK_NUM_EXCEPTION_SESSIONS - 1;

	pr_debug("set port %u mapping\n", port);

	if (!PP_IS_PORT_VALID(port)) {
		pr_err("Invalid port %u\n", port);
		return -EINVAL;
	}

	/* Verify ID's are valid */
	for (tc_ind = 0; tc_ind < PP_MAX_TC; tc_ind++) {
		for (hoh_ind = 0; hoh_ind < CHK_HOH_MAX; hoh_ind++) {
			if (id[tc_ind][hoh_ind] >= max_excep_sess) {
				pr_err("id %u exceeds maximum of %u\n",
				       id[tc_ind][hoh_ind],
				       max_excep_sess);
				return -EINVAL;
			}
		}
	}

	for (tc_ind = 0; tc_ind < PP_MAX_TC; tc_ind++) {
		for (hoh_ind = 0; hoh_ind < CHK_HOH_MAX; hoh_ind++) {
			__chk_map_set(port, tc_ind, hoh_ind,
				      id[tc_ind][hoh_ind]);
		}
	}

	return 0;
}

s32 chk_exception_session_port_map_get(u16 port,
				       u8 id[PP_MAX_TC][CHK_HOH_MAX])
{
	unsigned long mapping_base_addr;
	u32 mapping_val;
	u8 tc_ind;
	u8 hoh_ind;

	if (port >= PP_MAX_PORT) {
		pr_err("Invalid port %u\n", port);
		return -EINVAL;
	}

	for (tc_ind = 0; tc_ind < PP_MAX_TC; tc_ind++) {
		mapping_base_addr = CHK_ALT_SI_MAP_RAM(port, tc_ind);
		mapping_val = PP_REG_RD32(mapping_base_addr);

		for (hoh_ind = 0; hoh_ind < CHK_HOH_MAX; hoh_ind++) {
			id[tc_ind][hoh_ind] = (mapping_val >>
					      (BITS_PER_BYTE * hoh_ind)) &
					      0xFF;

			pr_debug("port %u. tc %u. hoh %u. id %u\n",
				port, tc_ind, hoh_ind, id[tc_ind][hoh_ind]);
		}
	}

	return 0;
}

s32 chk_exception_stats_get(u8 id, struct pp_stats *stats)
{
	struct hw_sgc hw_stats = { 0 };

	if (!__chk_is_exp_sess_id_valid(id))
		return -EINVAL;

	if (unlikely(ptr_is_null(stats)))
		return -EINVAL;

	/* a dummy read to trigger the indirect mechanism */
	PP_REG_RD32(CHK_ALT_SI_CNTS_RAM(id));
	hw_stats.buf[0] = PP_REG_RD32(PP_CHK_ALT_SI_CNTS_RAM_R0_REG);
	hw_stats.buf[1] = PP_REG_RD32(PP_CHK_ALT_SI_CNTS_RAM_R1_REG);
	hw_stats.buf[2] = PP_REG_RD32(PP_CHK_ALT_SI_CNTS_RAM_R2_REG);

	sgc_decode(&hw_stats, &stats->packets, &stats->bytes);
	pr_debug("buf[2] %#x, buf[1] %#x, buf[0] %#x, pkts = %llu, bytes = %llu\n",
		 hw_stats.buf[2], hw_stats.buf[1], hw_stats.buf[0],
		 stats->packets, stats->bytes);

	return 0;
}

s32 chk_sq_entry_enable(u8 id, u32 session, u16 queue)
{
	u32 _sess_reg  = 0;
	u32 _queue_reg = 0;

	pr_debug("sync-Q %hhu, session %u, queue %hu\n", id, session, queue);

	if (unlikely(id >= db->n_syncqs)) {
		pr_err("invalid sync-Q id %hhu\n", id);
		return -EINVAL;
	}

	_queue_reg = PP_FIELD_MOD(CHK_SYNCQ_QUEUE_ID_MSK, queue, _queue_reg);
	_queue_reg = PP_FIELD_MOD(CHK_SYNCQ_QUEUE_STATE_MSK, 1,  _queue_reg);
	PP_REG_WR32(CHK_SYNCQ_QUEUE(id), _queue_reg);
	_sess_reg = PP_FIELD_MOD(CHK_SYNCQ_SESSION_ID_MSK, session, _sess_reg);
	_sess_reg = PP_FIELD_MOD(CHK_SYNCQ_SESSION_VALID_MSK, 1,    _sess_reg);
	PP_REG_WR32(CHK_SYNCQ_SESSION(id), _sess_reg);

	return 0;
}

s32 chk_sq_entry_disable(u8 id)
{
	pr_debug("sync-Q %hhu\n", id);

	if (unlikely(id >= db->n_syncqs)) {
		pr_err("invalid sync-Q id %hhu\n", id);
		return -EINVAL;
	}

	PP_REG_WR32(CHK_SYNCQ_QUEUE(id),   0);
	PP_REG_WR32(CHK_SYNCQ_SESSION(id), 0);

	return 0;
}

static void __chk_sq_init(void)
{
	u8 i;

	for (i = 0; i < db->n_syncqs; i++)
		chk_sq_entry_disable(i);
}

static void __chk_stats_get(struct chk_stats *stats)
{
	struct pp_stats s_stats;
	u8 s_idx;

	memset(stats, 0, sizeof(struct chk_stats));

	if (pp_silicon_step_get() != PP_SSTEP_A) {
		for (s_idx = 0 ; s_idx < CHK_NUM_EXCEPTION_SESSIONS ; s_idx++) {
			chk_exception_stats_get(s_idx, &s_stats);
			if (s_idx == CHK_DEFAULT_SESSION_ID) {
				stats->dflt_session_byte_cnt += s_stats.bytes;
				stats->dflt_session_pkt_cnt  += s_stats.packets;
			} else {
				stats->exception_byte_cnt += s_stats.bytes;
				stats->exception_pkt_cnt  += s_stats.packets;
			}
		}
	}

	stats->pkts_rcvd_from_cls = PP_REG_RD32(PP_CHK_CLS_PKT_CNT_REG);
	stats->accelerated_pkts = stats->pkts_rcvd_from_cls -
				  stats->exception_pkt_cnt -
				  stats->dflt_session_pkt_cnt;
}

s32 chk_stats_get(struct chk_stats *stats)
{
	if (unlikely(!stats)) {
		pr_err("Invalid stats parameter\n");
		return -EINVAL;
	}

	__chk_stats_get(stats);
	U32_STRUCT_DIFF(&rstats, stats, stats);

	return 0;
}

void chk_stats_reset(void)
{
	u32 reset_val = 0xFFFFFFFF;

	/* reset the only counter we can by HW */
	PP_REG_WR32(PP_CHK_CLS_PKT_CNT_REG, reset_val);
	/* save a snapshot for all others */
	__chk_stats_get(&rstats);
	rstats.pkts_rcvd_from_cls = 0;
}

s32 chk_exceptions_stats_get(void *stats, u32 num_stats, void *data)
{
	struct pp_stats *it;
	u32 num = min_t(u32, CHK_NUM_EXCEPTION_SESSIONS, num_stats);
	u16 idx = 0;
	s32 ret;

	if (unlikely(ptr_is_null(stats)))
		return -EINVAL;

	for_each_arr_entry(it, stats, num, idx++) {
		ret = chk_exception_stats_get(idx, it);
		if (unlikely(ret))
			return ret;
	}

	return 0;
}

s32 chk_exceptions_stats_diff(void *pre, u32 num_pre, void *post, u32 num_post,
			      void *delta, u32 num_delta, void *data)
{
	struct pp_stats *pre_it, *post_it, *delta_it;
	u32 num = min_t(u32, CHK_NUM_EXCEPTION_SESSIONS, num_pre);

	if (unlikely(ptr_is_null(pre) || ptr_is_null(post) ||
		     ptr_is_null(delta)))
		return -EINVAL;

	if (unlikely(num_pre != num_post && num_pre != num_delta))
		return -EINVAL;

	post_it  = post;
	delta_it = delta;
	for_each_arr_entry(pre_it, pre, num, post_it++, delta_it++)
		U64_STRUCT_DIFF(pre_it, post_it, delta_it);

	return 0;
}

/**
 * @brief Print exception sessions stats into a buffer
 */
s32 chk_exceptions_stats_show(char *buf, size_t sz, size_t *n, void *stats,
			      u32 num_stats, void *data)
{
	struct pp_stats *it;
	u32 num = min_t(u32, CHK_NUM_EXCEPTION_SESSIONS, num_stats);

	if (unlikely(ptr_is_null(buf) || ptr_is_null(n) || ptr_is_null(stats)))
		return -EINVAL;

	pr_buf(buf, sz, *n, "\n");
	pr_buf_cat(buf, sz, *n,
		   "+--------------+----------------------+---------------------------+\n");
	pr_buf_cat(buf, sz, *n, "| %-12s | %-20s | %-25s |\n", "Exception ID",
		   "Packets", "Bytes");
	pr_buf_cat(buf, sz, *n,
		   "+--------------+----------------------+---------------------------+\n");

	for_each_arr_entry_cond(it, stats, num, pp_stats_is_non_zero)
		pr_buf_cat(buf, sz, *n, "| %-12u | %-20llu | %-25llu |\n",
			   arr_entry_idx(stats, it), it->packets, it->bytes);

	pr_buf_cat(buf, sz, *n,
		   "+--------------+----------------------+---------------------------+\n");

	return 0;
}

s32 chk_clock_get(u32 *clk_MHz)
{
	if (unlikely(ptr_is_null(clk_MHz)))
		return -EINVAL;

	*clk_MHz = db->hw_clk;
	return 0;
}

s32 chk_clock_set(u32 clk_MHz)
{
	__chk_lock();
	db->hw_clk = clk_MHz;
	__chk_unlock();

	return chk_tbm_hw_clock_update(clk_MHz);
}

bool chk_cntr_cache_bypass_get(void)
{
	return !!PP_REG_RD32(PP_CNTR_CACHE_BYPASS_REG);
}

void chk_cntr_cache_bypass_set(bool bypass)
{
	PP_REG_WR32(PP_CNTR_CACHE_BYPASS_REG, bypass);
}

s32 chk_cntr_cache_stats_get(u64 *read_trans, u64 *read_hits)
{
	u32 h, l;

	if (unlikely(ptr_is_null(read_trans) || ptr_is_null(read_hits)))
		return -EINVAL;

	l = PP_REG_RD32(PP_CNTR_CACHE_READ_TRANS_CNT_L_REG);
	h = PP_REG_RD32(PP_CNTR_CACHE_READ_TRANS_CNT_H_REG);
	*read_trans = PP_BUILD_64BIT_WORD(l, h);

	l = PP_REG_RD32(PP_CNTR_CACHE_READ_HIT_TRANS_CNT_L_REG);
	h = PP_REG_RD32(PP_CNTR_CACHE_READ_HIT_TRANS_CNT_H_REG);
	*read_hits = PP_BUILD_64BIT_WORD(l, h);

	return 0;
}

static void __chk_cntr_cache_init(void)
{
	u32 reg;

	/* set the cache to write back, write only to the cache,
	 * cause the counters are being written every accelerated
	 * packet and we want to reduce the DDR bandwidth
	 */
	reg = PP_REG_RD32(PP_CNTR_CACHE_POLICY_REG);
	reg = PP_FIELD_MOD(PP_CNTR_CACHE_POLICY_WRITE_POLICY_MSK,
			   PP_CACHE_WR_BACK, reg);
	PP_REG_WR32(PP_CNTR_CACHE_POLICY_REG, reg);

	/* enable the counters cache */
	chk_cntr_cache_bypass_set(false);
}

s32 __chk_ddr_allocate(void)
{
	db->dsi_sz = PAGE_ALIGN(db->n_sessions * sizeof(struct pp_hw_dsi));
	db->dsi_base_virt =
		pp_dma_alloc(db->dsi_sz, GFP_KERNEL, &db->dsi_base_phys, false);
	if (!db->dsi_base_virt) {
		pr_err("Could not allocate %u bytes for DSI\n",
		       (u32)db->dsi_sz);
		return -ENOMEM;
	}
	memset(db->dsi_base_virt, 0, db->dsi_sz);

	if (pp_silicon_step_get() == PP_SSTEP_A) {
		db->exp_sz =
			PAGE_ALIGN(CHK_NUM_EXCEPTION_SESSIONS * PP_SI_TOP_SZ);
		db->exp_base_virt = pp_dma_alloc(db->exp_sz, GFP_KERNEL,
						 &db->exp_base_phys, false);
		if (!db->exp_base_virt) {
			pr_err("Could not allocate %zu bytes exception session shadow DB\n",
			       db->exp_sz);
			return -ENOMEM;
		}
		memset(db->exp_base_virt, 0, db->exp_sz);
	}

	return 0;
}

void __chk_ddr_free(void)
{
	if (db->dsi_base_virt)
		pp_dma_free(db->dsi_sz, db->dsi_base_virt, &db->dsi_base_phys,
			    false);

	if (db->exp_base_virt)
		pp_dma_free(db->exp_sz, db->exp_base_virt, &db->exp_base_phys,
			    false);
}

s32 chk_init(struct pp_chk_init_param *init_param)
{
	s32 ret;
	u32 reg, port_idx, tc_idx;

	if (!init_param->valid)
		return -EINVAL;

	chk_base_addr       = init_param->chk_base;
	chk_ram_base_addr   = init_param->chk_ram_base;
	chk_cache_base_addr = init_param->chk_cache_base;

	/* Allocate db */
	db = kzalloc(sizeof(*db), GFP_KERNEL);
	if (unlikely(!db)) {
		pr_err("cannot allocate database\n");
		return -ENOMEM;
	}

	spin_lock_init(&db->lock);
	db->n_sessions  = init_param->num_sessions;
	db->n_syncqs    = init_param->num_syncqs;
	db->hw_clk      = init_param->hw_clk;
	db->nf_reass_en = false;

	/* Allocate ddr */
	ret = __chk_ddr_allocate();
	if (unlikely(ret)) {
		pr_err("chk_ddr_allocate failed\n");
		return ret;
	}

	/* wait for HW init to finish */
	if (pp_reg_poll(PP_CHK_MAIN_STATUS_REG,
			PP_CHK_MAIN_STATUS_INIT_MSK, 0)) {
		pr_err("HW init fails\n");
		return -EPERM;
	}

	/* Disable STW Error becoming exception ALT SI #63 */
	reg = PP_REG_RD32(PP_CHK_STAGE2_MATCH_FALLBACK_REG);
	reg = PP_FIELD_MOD(PP_CHK_STAGE2_MATCH_FALLBACK_DIS_SW_ERR_ALT_SI_MSK,
			   1, reg);
	/* Divert all frags to ingress uc for 2 purposes:
	 * 1. a-step only - fix the egress nf queue
	 * 2. Other steps - if no ud, divert to host.
	 *    This is done to handle frag packets received on non frag session
	 */
	reg = PP_FIELD_MOD(BIT(SI_CHCK_FLAG_DVRT_FRAG), 1, reg);
	PP_REG_WR32(PP_CHK_STAGE2_MATCH_FALLBACK_REG, reg);

	/* DSI BASE ADDR */
	PP_REG_WR32(PP_CHK_SI_DDR_BASE_ADDR_REG, db->dsi_base_phys >> 4);

	/* Reset Exception Mapping */
	for (port_idx = 0 ; port_idx < PP_MAX_PORT ; port_idx++) {
		for (tc_idx = 0 ; tc_idx < PP_MAX_TC ; tc_idx++)
			PP_REG_WR32(CHK_ALT_SI_MAP_RAM(port_idx, tc_idx), 0);
	}

	/* init syncq */
	__chk_sq_init();

	/* init crawler */
	__chk_crawler_init();

	/* init counters cache */
	__chk_cntr_cache_init();

	ret = chk_sgc_init();
	if (unlikely(ret))
		return ret;

	ret = chk_tbm_init(init_param);
	if (unlikely(ret))
		return ret;

	/* init debugfs */
	ret = chk_dbg_init(init_param);
	if (unlikely(ret))
		return ret;

	ret = chk_sysfs_init(init_param);
	if (unlikely(ret))
		return ret;

	return 0;
}

void chk_exit(void)
{
	if (!db)
		return;

	chk_sgc_exit();
	chk_tbm_exit();
	__chk_ddr_free();
	chk_dbg_clean();
	kfree(db);
	db = NULL;
}
