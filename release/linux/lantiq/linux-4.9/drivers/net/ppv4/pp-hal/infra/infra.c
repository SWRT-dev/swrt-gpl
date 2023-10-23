/*
 * infra.c
 * Description: PP Infrastructure Driver
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018-2020 Intel Corporation
 */
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/bitops.h>
#include <linux/pp_api.h>
#include "pp_logger.h"
#include "pp_regs.h"
#include "pp_common.h"
#include "infra.h"
#include "infra_internal.h"

u64 bootcfg_base_addr;
u64 clk_ctrl_base_addr;

/* @brief Default number of cycles to wait before HW module
 *        will enter power save mode, suggested by silicon team
 */
#define DFLT_DCLK_GRACE                 (30)

static const char *cru_state_str[CRU_STATE_NUM + 1] = {
	[CRU_STATE_DISABLE]      = "DISABLE",
	[CRU_STATE_SYNC_RST]     = "SYNC_RESET",
	[CRU_STATE_CLCK_DISABLE] = "CLOCK_DISABLE",
	[CRU_STATE_ENABLE]       = "ENABLE",
	[CRU_STATE_NUM]          = "Invalid"
};

static const char *cru_module_str[CRU_MOD_NUM + 1] = {
	[CRU_MOD_INFRA]      = "INFRA",
	[CRU_MOD_INGR_SS]    = "INGRESS_SS",
	[CRU_MOD_QOS_SS]     = "QOS_SS",
	[CRU_MOD_INGR_UC_SS] = "INGRESS_UC_SS",
	[CRU_MOD_EGRS_UC_SS] = "EGRESS_UC_SS",
	[CRU_MOD_CHK_TBM]    = "CHK_TBM",
	[CRU_MOD_NUM]        = "Invalid"
};

static const char *dclk_module_str[DCLK_MOD_NUM + 1] = {
	[DCLK_MOD_PORT_DIST]      = "PORT_DIST",
	[DCLK_MOD_RPB_SYS]        = "RPB_SYS",
	[DCLK_MOD_RPB_PPV4]       = "RPB_PPV4",
	[DCLK_MOD_PARSER]         = "PARSER",
	[DCLK_MOD_CLS_CORE]       = "CLS_CORE",
	[DCLK_MOD_CLS_SESS_CACHE] = "CLS_CACHE",
	[DCLK_MOD_CLS_CHK]        = "CHK",
	[DCLK_MOD_CLS_CHK_CACHE]  = "CHK_CACHE",
	[DCLK_MOD_MOD]            = "MOD",
	[DCLK_MOD_RX_DMA]         = "RX_DMA",
	[DCLK_MOD_BUFF_MGR]       = "BUFF_MGR",
	[DCLK_MOD_QOS_WRED]       = "QOS_WRED",
	[DCLK_MOD_QOS_TSCD]       = "QOS_TSCD",
	[DCLK_MOD_QOS_QMGR]       = "QOS_QMGR",
	[DCLK_MOD_QOS_TX_MGR]     = "QOS_TX_MGR",
	[DCLK_MOD_QOS_UC]         = "QOS_UC",
	[DCLK_MOD_NUM]            = "Invalid"
};

/**
 * @brief Convert pp cru state index into a string
 * @param s state index
 * @return const char* state in string format
 */
const char *infra_cru_state_str(enum cru_state s)
{
	if (likely(INFRA_IS_CRU_STATE_VALID(s)))
		return cru_state_str[s];

	return cru_state_str[CRU_STATE_NUM];
}

/**
 * @brief Convert PP cru module index into a string
 * @param m module index
 * @return const char* module name in string format
 */
const char *infra_cru_module_str(enum cru_module m)
{
	if (likely(INFRA_IS_CRU_MODULE_VALID(m)))
		return cru_module_str[m];

	return cru_module_str[CRU_MOD_NUM];
}

/**
 * @brief Convert PP dynamic clock module index into a string
 * @param m module index
 * @return const char* module name in string format
 */
const char *infra_dclk_module_str(enum dclk_module m)
{
	if (likely(INFRA_IS_DCLK_MODULE_VALID(m)))
		return dclk_module_str[m];

	return dclk_module_str[DCLK_MOD_NUM];
}

/**
 * @brief Polling on PP module CRU state, print error message in
 *        case of failure
 * @param module module index ''
 * @param state state to poll on
 * @return s32 0 on successful polling, non-zero value
 *         otherwise.
 */
static inline s32 __infa_cru_module_state_poll(enum cru_module m,
					       enum cru_state s)
{
	if (pp_reg_poll(INFRA_CLK_CTRL_CRU_MOD_ST_REG(m),
			PP_CLK_CTRL_INFRA_EN_MOD_EN_MSK, s)) {
		pr_err("Failed polling on %s(%u) CRU state for %s(%u)\n",
		       infra_cru_module_str(m), m, infra_cru_state_str(s), s);
		return -1;
	}
	return 0;
}

/**
 * @brief Checks in PP cru module is valid or not, print an
 *        error message in case not
 * @note Use 'PP_INFRA_IS_CRU_MODULE_VALID' to avoid error print
 * @param m module
 * @return bool true in case 'm' is valid, false otherwise
 */
static inline bool __infra_is_cru_module_valid(enum cru_module m)
{
	if (INFRA_IS_CRU_MODULE_VALID(m))
		return true;

	pr_err("Invalid PP cru module index %u\n", m);
	return false;
}

/**
 * @brief Checks in PP cru state is valid or not, print
 *        an error message in case not
 * @note Use 'PP_INFRA_IS_CRU_STATE_VALID' to avoid error print
 * @param s state
 * @return bool true in case 's' is valid, false otherwise
 */
static inline bool __infra_is_cru_state_valid(enum cru_state s)
{
	if (INFRA_IS_CRU_STATE_VALID(s))
		return true;

	pr_err("Invalid PP cru state %u\n", s);
	return false;
}

/**
 * @brief Checks in PP 'm' is a valid dynamic clock module or not, print
 *        an error message in case not
 * @param m module to test
 * @return bool true in case 'm' is valid, false otherwise
 */
static inline bool __infra_is_dclk_module_valid(enum dclk_module m)
{
	if (likely(INFRA_IS_DCLK_MODULE_VALID(m)))
		return true;

	pr_err("Invalid PP dynamic clock module %u\n", m);
	return false;
}

s32 infra_cru_mod_state_get(enum cru_module m, enum cru_state *s)
{
	u32 reg;

	if (unlikely(ptr_is_null(s)))
		return -EINVAL;
	if (unlikely(!__infra_is_cru_module_valid(m)))
		return -EINVAL;

	reg = PP_REG_RD32(INFRA_CLK_CTRL_CRU_MOD_ST_REG(m));
	*s = (enum cru_state)PP_FIELD_GET(PP_CLK_CTRL_INFRA_EN_MOD_EN_MSK, reg);

	return 0;
}

/**
 * @brief Init PP infra boot and config registers
 * @return s32 0 on success, non-zero value in case polling on
 *         some registers fails.
 */
static s32 __infra_bootcfg_init(void)
{
	u32 clfr_cache_cfg   = 0;
	u32 uc_timers_clk_en = 0;

	/* poll on port distributer memory init done bit */
	if (unlikely(pp_reg_poll(PP_BOOTCFG_INIT_DONE_REG,
				 PP_BOOTCFG_INIT_DONE_PORT_DISTR_INIT_DONE_MSK,
				 1))) {
		pr_err("Port distributer memory init failed to finish\n");
		return -EPERM;
	}

	/* poll on RPB memory init done bit */
	if (unlikely(pp_reg_poll(PP_BOOTCFG_INIT_DONE_REG,
				 PP_BOOTCFG_INIT_DONE_RPB_INIT_DONE_MSK, 1))) {
		pr_err("RPB memory init failed to finish\n");
		return -EPERM;
	}

	/* enable sessions and counters caches clocks */
	clfr_cache_cfg = PP_FIELD_MOD(PP_BOOTCFG_CLS_CFG_CNT_CACHE_DIS_MSK,
				      0, clfr_cache_cfg);
	clfr_cache_cfg = PP_FIELD_MOD(PP_BOOTCFG_CLS_CFG_SESS_CACHE_DIS_MSK,
				      0, clfr_cache_cfg);
	/* enable ingress and egress timers clocks */
	uc_timers_clk_en = PP_FIELD_MOD(PP_BOOTCFG_TIMERS_CLK_EN_UC_ING_MSK,
					1, uc_timers_clk_en);
	uc_timers_clk_en = PP_FIELD_MOD(PP_BOOTCFG_TIMERS_CLK_EN_UC_EGR_MSK,
					1, uc_timers_clk_en);

	/* write registers */
	PP_REG_WR32(PP_BOOTCFG_CLS_CFG_REG, clfr_cache_cfg);
	PP_REG_WR32(PP_BOOTCFG_TIMERS_CLK_EN_REG, uc_timers_clk_en);

	pr_debug("clfr_cache_cfg = 0x%x, uc_timers_clk_en = 0x%x\n",
		 clfr_cache_cfg, uc_timers_clk_en);

	return 0;
}

#if IS_ENABLED(PP_POWER_ON_REGS_TEST)
s32 infra_clck_ctrl_set(struct infra_init_param *init_param, bool en)
{
	enum cru_state state = en ? CRU_STATE_ENABLE : CRU_STATE_DISABLE;
	u32 cru = 0;
	u32 m;

	if (!init_param->valid)
		return -EINVAL;

	bootcfg_base_addr  = init_param->bootcfg_base;
	clk_ctrl_base_addr = init_param->clk_ctrl_base;

	for_each_infra_cru_module(m) {
		pr_debug("%s CRU module %s(%u), enable address %#llx\n",
			 BOOL2EN(en), infra_cru_module_str(m), m,
			 INFRA_CLK_CTRL_CRU_MOD_EN_REG(m));
		cru = INFRA_SET_MODULE_CRU_STATE(cru, state);
		PP_REG_WR32(INFRA_CLK_CTRL_CRU_MOD_EN_REG(m), cru);
		/* don't poll on infra when it was disabled */
		if (m == CRU_MOD_INFRA || state == CRU_STATE_DISABLE)
			continue;
		if (__infa_cru_module_state_poll(m, state))
			return -EIO;
	}

	return 0;
}
#endif

/**
 * @brief Init PP infra clock control registers
 * @return s32 0 on success, non-zero value if the function
 *         fails to enable on of the modules CRU clock
 */
static s32 __infra_clck_ctrl_init(void)
{
	u32 cru = 0;
	u32 m;

	for_each_infra_cru_module(m) {
		pr_debug("Enable CRU module %s(%u), enable address %#llx\n",
			 infra_cru_module_str(m), m,
			 INFRA_CLK_CTRL_CRU_MOD_EN_REG(m));
		cru = INFRA_SET_MODULE_CRU_STATE(cru, CRU_STATE_ENABLE);
		PP_REG_WR32(INFRA_CLK_CTRL_CRU_MOD_EN_REG(m), cru);
		if (__infa_cru_module_state_poll(m, CRU_STATE_ENABLE))
			return -EIO;
	}

	return 0;
}

u32 infra_version_get(void)
{
	return PP_REG_RD32(PP_BOOTCFG_VERSION_REG);
}

enum pp_silicon_step pp_silicon_step_get(void)
{
	return (enum pp_silicon_step)infra_version_get();
}

bool infra_cls_sess_cache_en_get(void)
{
	u32 reg;

	reg = PP_REG_RD32(PP_BOOTCFG_CLS_CFG_REG);
	return !(!!PP_FIELD_GET(PP_BOOTCFG_CLS_CFG_SESS_CACHE_DIS_MSK, reg));
}

bool infra_cls_cntrs_cache_en_get(void)
{
	u32 reg;

	reg = PP_REG_RD32(PP_BOOTCFG_CLS_CFG_REG);
	return !(!!PP_FIELD_GET(PP_BOOTCFG_CLS_CFG_CNT_CACHE_DIS_MSK, reg));
}

bool infra_rpb_init_done_get(void)
{
	u32 reg;

	reg = PP_REG_RD32(PP_BOOTCFG_INIT_DONE_REG);
	return !!PP_FIELD_GET(PP_BOOTCFG_INIT_DONE_RPB_INIT_DONE_MSK, reg);
}

bool infra_port_dist_init_done_get(void)
{
	u32 reg;

	reg = PP_REG_RD32(PP_BOOTCFG_INIT_DONE_REG);
	return !!PP_FIELD_GET(PP_BOOTCFG_INIT_DONE_PORT_DISTR_INIT_DONE_MSK,
			      reg);
}

bool infra_eg_uc_timers_clk_en_get(void)
{
	u32 reg;

	reg = PP_REG_RD32(PP_BOOTCFG_TIMERS_CLK_EN_REG);
	return !!PP_FIELD_GET(PP_BOOTCFG_TIMERS_CLK_EN_UC_EGR_MSK, reg);
}

bool infra_ing_uc_timers_clk_en_get(void)
{
	u32 reg;

	reg = PP_REG_RD32(PP_BOOTCFG_TIMERS_CLK_EN_REG);
	return !!PP_FIELD_GET(PP_BOOTCFG_TIMERS_CLK_EN_UC_ING_MSK, reg);
}

s32 infra_mod_dclk_set(enum dclk_module m, bool en)
{
	ulong reg;

	if (unlikely(!__infra_is_dclk_module_valid(m)))
		return -EINVAL;

	pr_debug("%s dynamic clock for %s(%u) module\n", BOOL2EN(en),
		 infra_dclk_module_str(m), m);
	/* @todo add lock in case this API will be used from contexts
	 *       other than debugfs
	 */
	reg = (ulong)PP_REG_RD32(PP_CLK_CTRL_DYNAMIC_CLK_GATED_EN_REG);
	if (en)
		set_bit(m, &reg);
	else
		clear_bit(m, &reg);
	PP_REG_WR32(PP_CLK_CTRL_DYNAMIC_CLK_GATED_EN_REG, (u32)reg);

	return 0;
}

s32 infra_mod_dclk_get(enum dclk_module m, bool *en)
{
	ulong reg;

	if (unlikely(!__infra_is_dclk_module_valid(m)))
		return -EINVAL;

	if (unlikely(ptr_is_null(en)))
		return -EINVAL;

	reg = (ulong)PP_REG_RD32(PP_CLK_CTRL_DYNAMIC_CLK_GATED_EN_REG);
	*en = !!test_bit(m, &reg);

	pr_debug("Module %s(%u) dynamic clock is %s\n",
		 infra_dclk_module_str(m), m, BOOL2EN(*en));

	return 0;
}

s32 infra_mod_dclk_status_get(enum dclk_module m, bool *on)
{
	ulong reg;

	if (unlikely(!__infra_is_dclk_module_valid(m)))
		return -EINVAL;

	if (unlikely(ptr_is_null(on)))
		return -EINVAL;

	reg = (ulong)PP_REG_RD32(PP_CLK_CTRL_INGRESS_CG_STATUS_REG);
	*on = !!test_bit(m, &reg);

	pr_debug("Module %s(%u) dynamic clock is %s\n",
		 infra_dclk_module_str(m), m, on ? "on" : "off");

	return 0;
}

s32 infra_dclk_grace_set(u32 grace)
{
	u32 mask = PP_CLK_CTRL_CG_GRACE_CNT_ICG_GRACE_CYCLE_MSK;

	if (unlikely(!pp_mask_value_valid(mask, grace))) {
		pr_err("invalid grace time %u\n", grace);
		return -EINVAL;
	}

	PP_REG_WR32(PP_CLK_CTRL_CG_GRACE_CNT_REG, grace);
	return 0;
}

s32 infra_dclk_grace_get(u32 *grace)
{
	if (unlikely(ptr_is_null(grace)))
		return -EINVAL;

	*grace = PP_REG_RD32(PP_CLK_CTRL_CG_GRACE_CNT_REG);
	return 0;
}

void infra_dynamic_clk_init(void)
{
	ulong dclk_reg = 0;
	u32 m;

	/* disable dynamic clock for all modules */
	PP_REG_WR32(PP_CLK_CTRL_DYNAMIC_CLK_GATED_EN_REG, 0);
	/* set default grace time */
	infra_dclk_grace_set(DFLT_DCLK_GRACE);

	/* enable dynamic clock for all modules */
	for_each_infra_dclk_mod(m) {
		switch (m) {
		/* skip due to feature not working properly */
		case DCLK_MOD_CLS_SESS_CACHE:
		case DCLK_MOD_CLS_CHK_CACHE:
		case DCLK_MOD_PORT_DIST:
		/* skip due to feature no implemented in HW */
		case DCLK_MOD_RX_DMA:
		case DCLK_MOD_QOS_WRED:
		case DCLK_MOD_QOS_TSCD:
		case DCLK_MOD_QOS_UC:
		case DCLK_MOD_QOS_TX_MGR:
			continue;
		default:
			break;
		}
		pr_debug("Enable dynamic clock for %s(%u)\n",
			 infra_dclk_module_str(m), m);
		set_bit(m, &dclk_reg);
	}
	pr_debug("dclk reg ==> %#lx\n", dclk_reg);
	PP_REG_WR32(PP_CLK_CTRL_DYNAMIC_CLK_GATED_EN_REG, (u32)dclk_reg);
	/* disable force clock on */
	PP_REG_WR32(PP_CLK_CTRL_ICG_FORCE_CLK_REG, 0);
}

s32 infra_reset_hw(enum cru_module m)
{
	u32 cru = 0;

	if (unlikely(!__infra_is_cru_module_valid(m)))
		return -EINVAL;

	/* set cru state to disable */
	cru = INFRA_SET_MODULE_CRU_STATE(cru, CRU_STATE_DISABLE);
	PP_REG_WR32(INFRA_CLK_CTRL_CRU_MOD_EN_REG(m), cru);
	if (__infa_cru_module_state_poll(m, CRU_STATE_DISABLE))
		return -EPERM;
	pr_debug("CRU module %s(%u) disabled\n", infra_cru_module_str(m), m);

	/* set cru state to enable */
	cru = INFRA_SET_MODULE_CRU_STATE(cru, CRU_STATE_ENABLE);
	PP_REG_WR32(INFRA_CLK_CTRL_CRU_MOD_EN_REG(m), cru);
	if (__infa_cru_module_state_poll(m, CRU_STATE_ENABLE))
		return -EPERM;
	pr_debug("CRU module %s(%u) enabled\n", infra_cru_module_str(m), m);

	return 0;
}

void infra_exit(void)
{
	/* clean debug stuff */
	infra_dbg_clean();
	pr_debug("done\n");
}

s32 infra_init(struct infra_init_param *init_param)
{
	s32 ret;

	if (!init_param->valid)
		return -EINVAL;

	bootcfg_base_addr  = init_param->bootcfg_base;
	clk_ctrl_base_addr = init_param->clk_ctrl_base;

	/* debug */
	ret = infra_dbg_init(init_param->dbgfs);
	if (unlikely(ret)) {
		pr_err("Failed to init debugfs, ret %d\n", ret);
		return ret;
	}
	/* clock control */
	ret = __infra_clck_ctrl_init();
	if (unlikely(ret)) {
		pr_err("Failed to init clock control, ret %d\n", ret);
		return ret;
	}
	/* boot and config */
	ret = __infra_bootcfg_init();
	if (unlikely(ret)) {
		pr_err("Failed to init boot and config, ret %d\n", ret);
		return ret;
	}

	pr_debug("done\n");
	return 0;
}
