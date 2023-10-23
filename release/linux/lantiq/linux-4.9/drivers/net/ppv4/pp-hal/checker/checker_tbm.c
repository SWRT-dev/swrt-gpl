/*
 * Description: PP Checker TBM sub-module
 * This is a helper module to control the checker's token bucket meter HW.
 * The HW contains 256 dual TBMs where each dual TBM is built from 2 single
 * TBMs where the cir TBM indexes are 0-255, and the pir TBM indexes
 * are 256-511.
 * The checker has another crawler which is resposible for updating all
 * the 256 dual TBMs entries every CRAWLER PERIOD.
 * The crawler has 24 bits timer which has same clock as the system clock.
 * The crawler period is configurable via 2 parameters in the HW:
 * 1. the timer prescaler - use for lowering the clock rate as needed.
 * 2. crawler msb - define the bit number out of the timer's 24 bits
 *                    which triggers the crawler scan.
 *                    Also, the crawler is updating 1 dual TBM every 32 cycles.
 * A single TBM entry rate is configured to HW using floating point like
 * method in which we have mantissa and exponent.
 * This implementation tries to support a wide range of byte rates per second
 * as possible and to be as accurate as it can be by using the maximum
 * possible exponent.
 * The maximum rate a single TBM can support is dictated by the HW clock.
 * The maximum exponent to use is dictated by the maximum range and the
 * maximum value we can store, which is MAX_U64.
 * Roughly, we can say that the supported ranges are:
 * 1. 50MHz-600MHz: 512bps-100Gbps
 * 2. 11MHz       : 512bps-22Gbps
 * 3. 5Mhz        : 512bps-10Gbps
 * 4. 1MHz        : 512bps-2Gbps
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2019 Intel Corporation
 */

#define pr_fmt(fmt) "[CHK_TBM]:%s:%d: " fmt, __func__, __LINE__

#include <linux/kernel.h>
#include <linux/spinlock.h>
#include <linux/types.h>
#include "pp_common.h"
#include "pp_regs.h"
#include "checker_regs.h"
#include "checker_internal.h"
#include "checker.h"
#include "infra.h"

/**
 * @enum chk_tbm_fld
 * @brief tbm fields enumeration
 */
enum chk_tbm_fld {
	TBM_FLDS_FIRST,
	TBM_FLD_CREDITS = TBM_FLDS_FIRST,
	TBM_FLD_TS,
	TBM_FLD_MAX_BURST,
	TBM_FLD_EXPONENT,
	TBM_FLD_MANTISSA,
	TBM_FLD_MODE,
	TBM_FLD_CP,
	TBM_FLD_LEN_TYPE,
	TBM_FLD_EN,
	TBM_FLDS_LAST = TBM_FLD_EN,
	TBM_FLDS_NUM,
};

/**
 * @brief TBM fields masks
 */
#define TBM_FLD_CREDITS_LSB                        (0)
#define TBM_FLD_CREDITS_MSB                        (23)
#define TBM_FLD_TS_LSB                             (24)
#define TBM_FLD_TS_MSB                             (47)
#define TBM_FLD_MAX_BURST_LSB                      (48)
#define TBM_FLD_MAX_BURST_MSB                      (71)
#define TBM_FLD_EXPONENT_LSB                       (72)
#define TBM_FLD_EXPONENT_MSB                       (77)
#define TBM_FLD_MANTISSA_LSB                       (78)
#define TBM_FLD_MANTISSA_MSB                       (89)
#define TBM_FLD_MODE_LSB                           (90)
#define TBM_FLD_MODE_MSB                           (91)
#define TBM_FLD_CP_LSB                             (92)
#define TBM_FLD_CP_MSB                             (92)
#define TBM_FLD_LEN_TYPE_LSB                       (93)
#define TBM_FLD_LEN_TYPE_MSB                       (93)
#define TBM_FLD_EN_LSB                             (95)
#define TBM_FLD_EN_MSB                             (95)

/**
 * @brief TBM HW entry size in number of 32bit words
 */
#define TBM_NUM_WORDS                              (3)

/**
 * @brief TBM hw entry size in bytes for reading/writing using
 *        the indirect mechanism, the actual size if 12 bytes
 */
#define TBM_HW_ENTRY_SIZE                          (4)

#define TBM_CREDITS_PER_BYTE                       (16)
/**
 * @brief Convert TBM HW credits to bytes
 */
#define TBM_CREDITS_TO_BYTES(c)                    ((c) / TBM_CREDITS_PER_BYTE)

/**
 * @brief Convert bytes to TBM HW credits
 */
#define TBM_BYTES_TO_CREDITS(b)                    ((b) * TBM_CREDITS_PER_BYTE)

/**
 * @brief Convert committed TBM index to a peak TBM index
 */
#define CTBM_TO_PTBM_IDX(idx)                      ((idx) + CHK_DUAL_TBM_NUM)

/**
 * @brief Defines the default crawler MSB, this crawler MSB
 *        is the bit number in the 24 bit timer which will
 *        trigger the crawler scan when it will toggle from 0 to 1.
 *        we se it to the maximum supported in HW to the crawler
 *        will interfere the least to the checker packets processing
 * @note based on PP_CHK_TBM_TIMER_CRLWR_MSB_MSK
 */
#define TBM_DFLT_CRWLR_MSB                         (7U)

/**
 * @struct tbm_buf
 * @brief Defines the tbm hw structure which is basically 3
 *        32bits words
 */
struct tbm_buf {
	u32 buf[TBM_NUM_WORDS];
};

/**
 * @brief TBM module database
 */
struct tbm_database {
	/* TBM shadow entries */
	struct tbm_buf *shadow;
	/* dual TBMs array */
	struct dual_tbm_db_info dtbms[CHK_DUAL_TBM_NUM];
	/* dual TBMs free list */
	struct list_head free;
	/* dual TBMs busy list */
	struct list_head busy;
	/*! lock for protecting the db */
	spinlock_t lock;
	/*! HW clock rate in MHZ */
	u32 hw_clk;
	/*! initial value of the exponent used for mantissa calculations */
	u32 exp_init;
	/*! maximum bytes per second rate supported with current hw clock */
	u64 max_rate;
	/*! pp hw silicon step */
	enum pp_silicon_step silicon_step;
};

/**
 * @brief Checker tbm fields
 */
struct buf_fld_info tbm_flds[TBM_FLDS_NUM];

/**
 * @brief module database
 */
static struct tbm_database *db;

/**
 * @brief TBM color modes description string
 */
static const char * const mode_str[PP_TBM_MODES_NUM + 1] = {
	[PP_TBM_MODE_4115_CB] = "4115 Color Blind",
	[PP_TBM_MODE_4115_CA] = "4115 Color Aware",
	[PP_TBM_MODE_2698_CB] = "2698 Color Blind",
	[PP_TBM_MODE_2698_CA] = "2698 Color Aware",
	[PP_TBM_MODES_NUM]    = "Invalid",
};

/**
 * @brief TBM length type description
 */
static const char * const len_type_str[PP_TBM_LEN_TYPES_NUM + 1] = {
	[PP_TBM_NEW_LEN]       = "Post Modification",
	[PP_TBM_ORG_LEN]       = "Pre Modification",
	[PP_TBM_LEN_TYPES_NUM] = "Invalid",
};

/**
 * @brief Acquire session manager lock
 */
static inline void __tbm_lock(void)
{
	pr_debug("caller %ps\n", __builtin_return_address(0));

	/* we are not allowed to run in irq context */
	WARN(in_irq(), "cannot run from irq context\n");
	spin_lock_bh(&db->lock);
}

/**
 * @brief Release session manager lock
 */
static inline void __tbm_unlock(void)
{
	pr_debug("caller %ps\n", __builtin_return_address(0));
	spin_unlock_bh(&db->lock);
}

/**
 * @brief Convert bytes to TBM HW credits
 */
static s64 __tbm_credits_to_hw(s64 bytes)
{
	return TBM_BYTES_TO_CREDITS(bytes);
}

/**
 * @brief Convert TBM HW credits to bytes
 */
static s64 __tbm_credits_from_hw(s64 credits)
{
	return TBM_CREDITS_TO_BYTES(credits);
}

/**
 * @brief Test whether TBM index is valid
 * @param idx index to check
 * @return bool true in case the index is valid, false otherwise
 */
static inline bool __tbm_is_idx_valid(u16 idx)
{
	if (likely(idx < CHK_TBM_NUM))
		return true;

	pr_err("Invalid TBM index %u\n", idx);
	return false;
}

/**
 * @brief Test whether Dual TBM index is valid
 * @param idx index to check
 * @return bool true in case the index is valid, false otherwise
 */
static inline bool __dtbm_is_idx_valid(u16 idx)
{
	if (likely(idx < CHK_DUAL_TBM_NUM))
		return true;

	pr_err("Invalid dual TBM index %u\n", idx);
	return false;
}

/**
 * @brief Test whether the specified tbm is allocated.
 * @param idx tbm index to check
 * @param err_print indicate whether to print error message or not
 * @return bool true if the tbm is allocated, false otherwise
 */
static inline bool __dtbm_is_allocated(u16 idx, bool err_print)
{
	if (likely(db->dtbms[idx].allocated))
		return true;

	if (err_print)
		pr_err("TBM [%u] isn't allocated\n", idx);
	return false;
}

/**
 * @brief Test whether the specified TBM is in use. A used TBM
 *        is a TBM which is attached to sessions
 * @param idx tbm index to check
 * @return bool true if the TBM is used, false otherwise
 */
static inline bool __dtbm_is_used(u16 idx)
{
	/* if the tbm have sessions attached, he is used */
	if (likely(db->dtbms[idx].n_sessions > 0))
		return true;

	return false;
}

/**
 * @brief Encode tbm hw configuration into an TBM buffer that can be
 *        written to hw registers
 * @param hw buffer to encode to
 * @param cfg configuration to encode
 */
static void __tbm_encode(struct tbm_buf *hw, struct tbm_cfg *cfg)
{
	memset(hw, 0, sizeof(*hw));
	buf_fld_set(&tbm_flds[TBM_FLD_MAX_BURST], hw->buf, cfg->max_burst);
	buf_fld_set(&tbm_flds[TBM_FLD_CREDITS],   hw->buf, cfg->credits);
	buf_fld_set(&tbm_flds[TBM_FLD_TS],        hw->buf, cfg->timestamp);
	buf_fld_set(&tbm_flds[TBM_FLD_EXPONENT],  hw->buf, cfg->exponent);
	buf_fld_set(&tbm_flds[TBM_FLD_MANTISSA],  hw->buf, cfg->mantissa);
	buf_fld_set(&tbm_flds[TBM_FLD_MODE],      hw->buf, cfg->mode);
	buf_fld_set(&tbm_flds[TBM_FLD_CP],        hw->buf, cfg->coupling);
	buf_fld_set(&tbm_flds[TBM_FLD_LEN_TYPE],  hw->buf, cfg->len_type);
	buf_fld_set(&tbm_flds[TBM_FLD_EN],        hw->buf, cfg->enable);
}

/**
 * @brief Decode an TBM hw buffer to tbm hw configuration
 * @param hw buffer to decode from
 * @param cfg configuration
 */
static void __tbm_decode(struct tbm_buf *hw, struct tbm_cfg *cfg)
{
	memset(cfg, 0, sizeof(*cfg));
	cfg->max_burst = buf_fld_get(&tbm_flds[TBM_FLD_MAX_BURST], hw->buf);
	cfg->credits   = buf_fld_get(&tbm_flds[TBM_FLD_CREDITS],   hw->buf);
	cfg->timestamp = buf_fld_get(&tbm_flds[TBM_FLD_TS],        hw->buf);
	cfg->exponent  = buf_fld_get(&tbm_flds[TBM_FLD_EXPONENT],  hw->buf);
	cfg->mantissa  = buf_fld_get(&tbm_flds[TBM_FLD_MANTISSA],  hw->buf);
	cfg->mode      = buf_fld_get(&tbm_flds[TBM_FLD_MODE],      hw->buf);
	cfg->coupling  = buf_fld_get(&tbm_flds[TBM_FLD_CP],        hw->buf);
	cfg->len_type  = buf_fld_get(&tbm_flds[TBM_FLD_LEN_TYPE],  hw->buf);
	cfg->enable    = buf_fld_get(&tbm_flds[TBM_FLD_EN],        hw->buf);
}

/**
 * @brief Calculate tbm entry address for reading/writhing HW
 *        using the indirect mechanism
 * @param idx tbm index
 * @return u64 the address
 */
static inline u64 __tbm_hw_entry_addr_calc(u16 idx)
{
	return (idx * TBM_HW_ENTRY_SIZE) + CHK_TBM_RAM_BASE;
}

/**
 * @brief Read the timer prescaler currently configured in HW
 * @return u8
 */
static inline u8 __tbm_timer_prescaler_get(void)
{
	u32 reg;
	u8  prescaler;

	reg       = PP_REG_RD32(PP_CHK_TBM_TIMER_REG);
	prescaler = (u8)PP_FIELD_GET(PP_CHK_TBM_TIMER_TIMER_MSB_MSK, reg);

	return prescaler;
}

/**
 * @brief Write TBM configuration to HW via the indirect mechanism
 * @param idx tbm index in HW
 * @param tbm the configuration
 */
static inline void __tbm_set(u16 idx, struct tbm_cfg *tbm)
{
	struct tbm_buf hw = { 0 };

	/* encode the tbm to HW form, in case tbm is NULL, invalidate the TBM */
	if (tbm)
		__tbm_encode(&hw, tbm);
	/* write the hw form into the indirect registers */
	PP_REG_WR32(PP_CHK_TBM_RAM_W0_REG, hw.buf[0]);
	PP_REG_WR32(PP_CHK_TBM_RAM_W1_REG, hw.buf[1]);
	PP_REG_WR32(PP_CHK_TBM_RAM_W2_REG, hw.buf[2]);
	/* trigger the indirect write */
	PP_REG_WR32(__tbm_hw_entry_addr_calc(idx), 0);

	/* save in shadow db */
	if (db->silicon_step == PP_SSTEP_A)
		memcpy(&db->shadow[idx], &hw, sizeof(hw));
}

/**
 * @brief Read TBM configuration from HW via the indirect mechanism
 * @param idx tbm index in HW
 * @param tbm configuration buffer
 */
static inline void __tbm_get(u16 idx, struct tbm_cfg *tbm)
{
	struct tbm_buf hw;

	if (db->silicon_step == PP_SSTEP_A) {
		/* read from shadow db */
		memcpy(&hw, &db->shadow[idx], sizeof(hw));
	} else {
		/* read directly from hw */
		/* trigger the indirect read */
		PP_REG_RD32(__tbm_hw_entry_addr_calc(idx));
		/* read the hw form from the indirect registers */
		hw.buf[0] = PP_REG_RD32(PP_CHK_TBM_RAM_R0_REG);
		hw.buf[1] = PP_REG_RD32(PP_CHK_TBM_RAM_R1_REG);
		hw.buf[2] = PP_REG_RD32(PP_CHK_TBM_RAM_R2_REG);
	}
	/* encode the tbm to HW form */
	__tbm_decode(&hw, tbm);
}

/**
 * @brief Calculate the maximum exponent we can start with
 *        to support the specified rate for mantissa and
 *        exponent calculations
 *        the bigger the exponent we start with the bigger
 *        accuracy we get
 * @note the exponent value limitation is derived from the u64
 *       number of bits
 * @param rate bytes per seconds
 * @return u32 the maximum exponent
 */
static u32 __tbm_exp_init_calc(u64 rate)
{
	u32 msb;

	/* convert to credits */
	rate = TBM_BYTES_TO_CREDITS(rate);

	if (rate > U32_MAX)
		msb = BITS_PER_U32 + fls(rate >> BITS_PER_U32);
	else
		msb = fls((u32)rate);

	return BITS_PER_U64 - msb;
}

/**
 * @brief Calculates the maximum supported rate under
 *        the specified clock
 * @param clk_MHz clock frequency in MHz
 * @return u32 maximum supported rate
 */
static u64 __tbm_max_rate_calc(u32 clk_MHz)
{
	u16 max_mantissa;
	u64 clk_hz;
	u8  prescaler;
	u64 max_rate;

	prescaler    = __tbm_timer_prescaler_get();
	clk_hz       = MHZ_TO_HZ(clk_MHz) >> prescaler;
	max_mantissa = buf_fld_max_val_get(&tbm_flds[TBM_FLD_MANTISSA]);
	max_rate     = TBM_CREDITS_TO_BYTES(clk_hz * max_mantissa);

	return max_rate;
}

/**
 * @brief Calculates the mantissa and the exponent for a tbm based
 *        on the specified rate.
 *        For a tbm, the mantissa and the exponent should reflect
 *        the number of bytes the tbm should accumulate every clock cycle
 * @param tbm the tbm
 * @param rate the rate
 */
static inline s32 __tbm_mantissa_calc(struct tbm_cfg *tbm, u64 rate)
{
	u64 mantissa;
	u32 max_mantissa, clk_hz;
	u8  prescaler;
	s8  exp;

	prescaler    = __tbm_timer_prescaler_get();
	clk_hz       = MHZ_TO_HZ(db->hw_clk) >> prescaler; /* timer rate */
	max_mantissa = buf_fld_max_val_get(&tbm_flds[TBM_FLD_MANTISSA]);
	exp          = db->exp_init;

	pr_debug("rate %llu, clk_hz %u, prescaler %u, max_mantissa %u, exp %d\n",
		 rate, clk_hz, prescaler, max_mantissa, exp);

	mantissa  = TBM_BYTES_TO_CREDITS(rate); /* convert to credits */
	/* scale up to increase accuracy */
	mantissa *= (1ULL << exp);
	do_div(mantissa, clk_hz);

	/* scale down till it fits the register field */
	while (mantissa > max_mantissa) {
		mantissa >>= 1;
		exp       -= 1;
	}
	pr_debug("mantissa %llu, exponent %d\n", mantissa, exp);

	/* make sure the rate is not too big */
	if (exp < 0) {
		pr_err("rate %llu too big\n", rate);
		return -ERANGE;
	}

	tbm->mantissa  = (u16)mantissa;
	tbm->exponent  = (u8)exp;

	return 0;
}

/**
 * @brief Prepare tbm hw configuration, all other fields than the ones
 * @param tbm the configuration to prepare
 * @param enable
 * @param max_burst
 * @param mode
 * @param len_type
 * @return s32 0 on success, error code otherwise
 */
static inline s32 __tbm_hw_cfg_prepare(struct tbm_cfg *tbm, bool enable,
				       u64 rate, u32 max_burst,
				       enum pp_tbm_mode mode,
				       enum pp_tbm_len_type len_type)
{
	memset(tbm, 0, sizeof(*tbm));
	tbm->coupling  = true; /* always do coupling as HW support ONLY that */
	tbm->enable    = enable;
	tbm->max_burst = max_burst;
	tbm->mode      = mode;
	tbm->len_type  = len_type;
	tbm->credits   = max_burst;
	tbm->timestamp = 0;
	return __tbm_mantissa_calc(tbm, rate);
}

/**
 * @brief Calculates TBM bytes rate per second base on the specified clock
 * @param tbm the tbm
 * @param clk the clock frequency in MHz
 * @return u32 tbm bytes rate
 */
static inline u64 __tbm_rate_calc(struct tbm_cfg *tbm, u32 clk_MHz)
{
	u64 rate;

	rate   = tbm->mantissa;
	rate  *= MHZ_TO_HZ(clk_MHz);
	rate >>= tbm->exponent;

	return TBM_CREDITS_TO_BYTES(rate);
}

s32 chk_dual_tbm_get(u16 idx, struct pp_dual_tbm *dtbm)
{
	struct dual_tbm_db_info *db_info;
	struct tbm_cfg ctbm, ptbm;

	if (unlikely(ptr_is_null(dtbm)))
		return -EINVAL;
	if (unlikely(!__dtbm_is_idx_valid(idx)))
		return -EINVAL;

	db_info = &db->dtbms[idx];

	__tbm_lock();
	__tbm_get(db_info->ctbm_idx, &ctbm); /* read committed tbm */
	__tbm_get(db_info->ptbm_idx, &ptbm); /* read peak tbm */
	__tbm_unlock();

	dtbm->cir      = __tbm_rate_calc(&ctbm, db->hw_clk);
	dtbm->pir      = __tbm_rate_calc(&ptbm, db->hw_clk);
	dtbm->cbs      = ctbm.max_burst;
	dtbm->pbs      = ptbm.max_burst;
	dtbm->mode     = ctbm.mode;
	dtbm->len_type = ctbm.len_type;
	dtbm->enable   = ctbm.enable;
	pr_debug("en %s, cir %llu, cbs %u, pir %llu, bs %u, mode %u, type %u\n",
		 BOOL2STR(dtbm->enable), dtbm->cir, dtbm->cbs, dtbm->pir,
		 dtbm->pbs, dtbm->mode, dtbm->len_type);

	return 0;
}

/**
 * @brief Test if dual TBM arguments are valid
 * @param idx tbm index
 * @param dtbm the dual tbm
 * @return bool true if it is valid, false otherwise
 */
static inline bool __tbm_dual_tbm_is_valid(u16 idx, struct pp_dual_tbm *dtbm)
{
	if (unlikely(ptr_is_null(dtbm)))
		return false;
	if (unlikely(!__dtbm_is_idx_valid(idx)))
		return false;
	if (unlikely(!__dtbm_is_allocated(idx, true)))
		return false;
	if (unlikely(db->max_rate < dtbm->cir)) {
		pr_err("committed rate %llu too high, max value %llu\n",
		       dtbm->cir, db->max_rate);
		return false;
	}
	if (unlikely(db->max_rate < dtbm->pir)) {
		pr_err("peak rate %llu too high, max value %llu\n", dtbm->pir,
		       db->max_rate);
		return false;
	}

	return true;
}

s32 chk_dual_tbm_set(u16 idx, struct pp_dual_tbm *dtbm)
{
	struct dual_tbm_db_info *db_info;
	struct tbm_cfg ctbm, ptbm;
	s32 ret;

	if (unlikely(!__tbm_dual_tbm_is_valid(idx, dtbm)))
		return -EINVAL;

	/* get reference to the database entry */
	db_info = &db->dtbms[idx];
	/* prepare the hw tbm configuration */
	ret = __tbm_hw_cfg_prepare(&ctbm, dtbm->enable, dtbm->cir, dtbm->cbs,
				   dtbm->mode, dtbm->len_type);
	if (unlikely(ret))
		return ret;
	ret = __tbm_hw_cfg_prepare(&ptbm, dtbm->enable, dtbm->pir, dtbm->pbs,
				   dtbm->mode, dtbm->len_type);
	if (unlikely(ret))
		return ret;

	/* configure the TBMs in HW */
	__tbm_lock();
	__tbm_set(db_info->ctbm_idx, &ctbm); /* write committed TBM to hw */
	__tbm_set(db_info->ptbm_idx, &ptbm); /* write peak TBM to hw     */
	db_info->org_cir = dtbm->cir;
	db_info->org_pir = dtbm->pir;
	__tbm_unlock();

	return 0;
}

/**
 * @brief Allocate dual tbm entry from the database
 * @return void* __dtbm_alloc
 */
static inline void *__must_check __dtbm_alloc(void)
{
	struct dual_tbm_db_info *dtbm = NULL;

	__tbm_lock();
	if (unlikely(list_empty(&db->free))) {
		pr_err("no free TBM is available\n");
		goto unlock;
	}

	/* take the first dual tbm in the list */
	dtbm = list_first_entry(&db->free, struct dual_tbm_db_info, node);
	/* remove from the free list */
	list_del_init(&dtbm->node);
	/* add to the busy list */
	list_add_tail(&dtbm->node, &db->busy);
	/* mark as allocated */
	dtbm->allocated = true;
unlock:
	__tbm_unlock();
	return dtbm;
}

s32 chk_dual_tbm_alloc(u16 *idx)
{
	struct dual_tbm_db_info *db_info;

	if (unlikely(ptr_is_null(idx)))
		return -EINVAL;

	db_info = __dtbm_alloc();
	if (unlikely(!db_info))
		return -ENOSPC;

	/* return the committed tbm index so the user can use it
	 * in the SI w/o any conversions
	 */
	*idx = db_info->ctbm_idx;
	pr_debug("dual TBM %u allocated\n", *idx);

	return 0;
}

s32 chk_dual_tbm_free(u16 idx)
{
	struct dual_tbm_db_info *db_info;

	if (unlikely(!__dtbm_is_idx_valid(idx)))
		return -EINVAL;

	db_info = &db->dtbms[idx];

	__tbm_lock();
	if (unlikely(!__dtbm_is_allocated(idx, true))) {
		__tbm_unlock();
		return -EINVAL;
	}

	if (unlikely(__dtbm_is_used(idx))) {
		pr_err("cannot free, TBM %u is used by %u sessions\n",
		       idx, db_info->n_sessions);
		__tbm_unlock();
		return -EBUSY;
	}

	/* invalidate committed and peak TBMs */
	__tbm_set(db_info->ctbm_idx, NULL);
	__tbm_set(db_info->ptbm_idx, NULL);
	/* remove from the busy list */
	list_del_init(&db_info->node);
	/* return back the free list */
	list_add_tail(&db_info->node, &db->free);
	/* mark as no allocated */
	db_info->allocated = false;
	__tbm_unlock();

	pr_debug("dual tbm %u freed\n", idx);
	return 0;
}

s32 chk_dual_tbm_session_attach(u16 idx)
{
	struct dual_tbm_db_info *db_info;

	if (unlikely(!__dtbm_is_idx_valid(idx)))
		return -EINVAL;

	db_info = &db->dtbms[idx];
	__tbm_lock();
	if (unlikely(!__dtbm_is_allocated(idx, true))) {
		__tbm_unlock();
		return -EINVAL;
	}

	db_info->n_sessions++;
	__tbm_unlock();

	pr_debug("session attached to dual tbm %u, n_sessions %u\n", idx,
		 db_info->n_sessions);

	return 0;
}

s32 chk_dual_tbm_session_detach(u16 idx)
{
	struct dual_tbm_db_info *db_info;

	if (unlikely(!__dtbm_is_idx_valid(idx)))
		return -EINVAL;

	db_info = &db->dtbms[idx];
	__tbm_lock();
	if (unlikely(!__dtbm_is_allocated(idx, true))) {
		__tbm_unlock();
		return -EINVAL;
	}

	if (unlikely(!__dtbm_is_used(idx))) {
		pr_err("cannot detach, dual tbm %u is not used by any sessions\n",
		       idx);
		__tbm_unlock();
		return -EINVAL;
	}

	db_info->n_sessions--;
	__tbm_unlock();

	pr_debug("session detached from dual tbm %u, n_sessions %u\n",
		 idx, db_info->n_sessions);
	return 0;
}

/**
 * @brief Update tbm due to clock change, clock change can also be
 *        be due to prescaler change.
 * @param idx tbm index
 * @param org_rate the original rate that the tbm was first configured to
 * @return s32 0 on success, error code otherwise
 */
static inline s32 __tbm_entry_clock_update(u16 idx, u64 org_rate)
{
	struct tbm_cfg tbm;
	s32 ret = 0;

	__tbm_lock();
	__tbm_get(idx, &tbm);

	/* if original rate is still valid, use it, otherwise, set the max */
	ret = __tbm_mantissa_calc(&tbm, min(org_rate, db->max_rate));
	if (unlikely(ret))
		goto unlock;

	tbm.credits   = tbm.max_burst;
	tbm.timestamp = 0;
	__tbm_set(idx, &tbm);

unlock:
	__tbm_unlock();
	return ret;
}

/**
 * @brief Update all dual TBMs following clock frequency change
 * @return s32 0 on success, error code otherwise
 */
static s32 __dual_tbm_update_all(void)
{
	struct dual_tbm_db_info *dtbm;
	s32 ret = 0;

	/* update all allocated tbms configuration due to the hw clock change */
	list_for_each_entry(dtbm, &db->busy, node) {
		ret = __tbm_entry_clock_update(dtbm->ctbm_idx, dtbm->org_cir);
		if (unlikely(ret))
			return ret;
		ret = __tbm_entry_clock_update(dtbm->ptbm_idx, dtbm->org_pir);
		if (unlikely(ret))
			return ret;
	}

	return ret;
}

/**
 * @brief Update the tbm database with the new hw clock frequency
 * @param clk the new clock in MHz
 */
static inline void __tbm_hw_clock_set(u32 clk_MHz)
{
	db->hw_clk   = clk_MHz;
	db->max_rate = __tbm_max_rate_calc(clk_MHz);
	db->exp_init = __tbm_exp_init_calc(db->max_rate);
}

s32 chk_tbm_hw_clock_update(u32 clk_MHz)
{
	__tbm_hw_clock_set(clk_MHz);
	return __dual_tbm_update_all();
}

bool chk_dual_tbm_is_allocated(u16 idx)
{
	bool allocated;

	if (unlikely(!__dtbm_is_idx_valid(idx)))
		return false;

	__tbm_lock();
	allocated = __dtbm_is_allocated(idx, false);
	__tbm_unlock();

	return allocated;
}

s32 chk_dual_tbm_num_sessions_get(u16 idx, u32 *num_sessions)
{
	s32 ret = 0;

	if (unlikely(ptr_is_null(num_sessions)))
		return -EINVAL;

	if (unlikely(!__dtbm_is_idx_valid(idx)))
		return -EINVAL;

	__tbm_lock();
	if (unlikely(!__dtbm_is_allocated(idx, true))) {
		ret = -EINVAL;
		goto unlock;
	}
	*num_sessions = db->dtbms[idx].n_sessions;
unlock:
	__tbm_unlock();
	return ret;
}

const char * const chk_tbm_mode_str(enum pp_tbm_mode m)
{
	if (likely(m < PP_TBM_MODES_NUM))
		return mode_str[m];
	return mode_str[PP_TBM_MODES_NUM];
}

const char * const chk_tbm_len_type_str(enum pp_tbm_len_type t)
{
	if (likely(t < PP_TBM_LEN_TYPES_NUM))
		return len_type_str[t];
	return len_type_str[PP_TBM_LEN_TYPES_NUM];
}

#ifdef CONFIG_DEBUG_FS
s32 chk_tbm_get(u16 idx, struct tbm_cfg *tbm)
{
	if (unlikely(ptr_is_null(tbm)))
		return -EINVAL;
	if (unlikely(!__tbm_is_idx_valid(idx)))
		return -EINVAL;

	__tbm_lock();
	__tbm_get(idx, tbm);
	__tbm_unlock();

	return 0;
}

s32 chk_tbm_set(u16 idx, struct tbm_cfg *tbm)
{
	if (unlikely(ptr_is_null(tbm)))
		return -EINVAL;
	if (unlikely(!__tbm_is_idx_valid(idx)))
		return -EINVAL;

	__tbm_lock();
	__tbm_set(idx, tbm);
	__tbm_unlock();

	return 0;
}

s32 chk_dual_tbm_db_info_get(u16 idx, struct dual_tbm_db_info *dtbm)
{
	if (unlikely(ptr_is_null(dtbm)))
		return -EINVAL;
	if (unlikely(!__dtbm_is_idx_valid(idx)))
		return -EINVAL;

	memcpy(dtbm, &db->dtbms[idx], sizeof(*dtbm));
	return 0;
}
#endif

/**
 * @brief Init all the dual TBMs info in the database and invalidate
 *        all the TBMs in hw
 */
static inline void __tbm_dual_tbms_init(void)
{
	struct dual_tbm_db_info *dtbm;
	u32 i = 0;

	for_each_arr_entry(dtbm, db->dtbms, ARRAY_SIZE(db->dtbms)) {
		/* using 2 tbms for implementing dual tbm */
		dtbm->ctbm_idx = i++;
		dtbm->ptbm_idx = CTBM_TO_PTBM_IDX(dtbm->ctbm_idx);

		/* invalidate the TBMs in hw */
		__tbm_set(dtbm->ctbm_idx, NULL);
		__tbm_set(dtbm->ptbm_idx, NULL);
		/* init list node */
		INIT_LIST_HEAD(&dtbm->node);
		/* add to free list */
		list_add_tail(&dtbm->node, &db->free);
	}
}

/**
 * @brief Init tbm database
 */
static s32 __tbm_db_init(struct pp_chk_init_param *init_args)
{
	/* Allocate db */
	db = devm_kzalloc(pp_dev_get(), sizeof(*db), GFP_KERNEL);
	if (unlikely(!db)) {
		pr_err("failed to allocate database\n");
		return -ENOMEM;
	};
	db->silicon_step = pp_silicon_step_get();

	/* allocate shadow db only for step A */
	if (db->silicon_step == PP_SSTEP_A) {
		db->shadow = devm_kcalloc(pp_dev_get(), CHK_TBM_NUM,
					  sizeof(*db->shadow), GFP_KERNEL);
		if (unlikely(!db->shadow)) {
			pr_err("failed to allocate shadow database\n");
			return -ENOMEM;
		};
	}

	spin_lock_init(&db->lock);
	INIT_LIST_HEAD(&db->free);
	INIT_LIST_HEAD(&db->busy);
	__tbm_dual_tbms_init();
	__tbm_hw_clock_set(init_args->hw_clk);

	pr_debug("clock %uMHz, max_rate %llu Bps, exp_init %u\n", db->hw_clk,
		 db->max_rate, db->exp_init);
	return 0;
}

static void __tbm_crawler_init(void)
{
	u32 reg = 0;

	/* set timer prescaler to 0 cause reduced clock will affect
	 * the maximum rate which can be supported
	 */
	reg = PP_FIELD_MOD(PP_CHK_TBM_TIMER_TIMER_MSB_MSK, 0, reg);
	/* set the crawler intervals to the maximum, to interfere the least
	 * as possible to traffic
	 * the crawler will update a dual tbm entry every 128 cycles
	 * Total scan time for 600MHz: (128 * 256) / 600MHz ~= 54us
	 * Total scan time for 100MHz: (128 * 256) / 100MHz ~= 327us
	 * Total scan time for 11MHz : (128 * 256) / 11MHz  ~= 3ms
	 * Total scan time for 5MHz  : (128 * 256) / 5MHz   ~= 6.5ms
	 */
	reg = PP_FIELD_MOD(PP_CHK_TBM_TIMER_CRLWR_MSB_MSK, TBM_DFLT_CRWLR_MSB,
			   reg);
	PP_REG_WR32(PP_CHK_TBM_TIMER_REG, reg);
	pr_debug("timer reg = %#08x\n", reg);
}

static void __tbm_crawler_enable(void)
{
	u32 reg;

	reg = PP_REG_RD32(PP_CHK_TBM_TIMER_REG);
	/* enable tbm timer */
	reg = PP_FIELD_MOD(PP_CHK_TBM_TIMER_TIMER_EN_MSK, 1, reg);
	/* enable tbm crawler */
	reg = PP_FIELD_MOD(PP_CHK_TBM_TIMER_CRLWR_EN_MSK, 1, reg);
	PP_REG_WR32(PP_CHK_TBM_TIMER_REG, reg);
	pr_debug("timer reg = %#08x\n", reg);
}

/**
 * @brief Destroy tbm database
 */
static void __tbm_db_destroy(void)
{
	if (unlikely(!db))
		return;

	if (db->shadow)
		devm_kfree(pp_dev_get(), db->shadow);

	devm_kfree(pp_dev_get(), db);
	db = NULL;
}

static void __tbm_init_fields(void)
{
	/* init tbm fields */
	BUF_FLD_INIT_SIMPLE(tbm_flds, TBM_FLD_CREDITS);
	BUF_FLD_INIT_SIMPLE(tbm_flds, TBM_FLD_TS);
	BUF_FLD_INIT_SIMPLE(tbm_flds, TBM_FLD_MAX_BURST);
	BUF_FLD_INIT_SIMPLE(tbm_flds, TBM_FLD_EXPONENT);
	BUF_FLD_INIT_SIMPLE(tbm_flds, TBM_FLD_MANTISSA);
	BUF_FLD_INIT_SIMPLE(tbm_flds, TBM_FLD_MODE);
	BUF_FLD_INIT_SIMPLE(tbm_flds, TBM_FLD_CP);
	BUF_FLD_INIT_SIMPLE(tbm_flds, TBM_FLD_LEN_TYPE);
	BUF_FLD_INIT_SIMPLE(tbm_flds, TBM_FLD_EN);

	/* set granularity conversion functions to credits and max burst */
	tbm_flds[TBM_FLD_CREDITS].from_hw   = __tbm_credits_from_hw;
	tbm_flds[TBM_FLD_CREDITS].to_hw     = __tbm_credits_to_hw;
	tbm_flds[TBM_FLD_MAX_BURST].from_hw = __tbm_credits_from_hw;
	tbm_flds[TBM_FLD_MAX_BURST].to_hw   = __tbm_credits_to_hw;
}

s32 chk_tbm_init(struct pp_chk_init_param *init_args)
{
	s32 ret;

	if (unlikely(ptr_is_null(init_args)))
		return -EINVAL;

	__tbm_init_fields();
	__tbm_crawler_init();

	ret = __tbm_db_init(init_args);
	if (unlikely(ret))
		return ret;

	/* everything is ready, start the crawler */
	__tbm_crawler_enable();

	return 0;
}

void chk_tbm_exit(void)
{
	__tbm_db_destroy();
}
