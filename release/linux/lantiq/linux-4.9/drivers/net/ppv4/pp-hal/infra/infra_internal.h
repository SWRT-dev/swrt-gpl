/*
 * infra_internal.h
 * Description: PP Infrastructure Internal Definitions
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018-2020 Intel Corporation
 */

#ifndef __INFRA_INTERNAL_H__
#define __INFRA_INTERNAL_H__
#include <linux/types.h>
#include <linux/bitfield.h>
#include "pp_regs.h"
#include "pp_logger.h"
#include "pp_common.h"
#include "bootcfg_regs.h"
#include "clk_ctrl_regs.h"

extern u64 bootcfg_base_addr;
extern u64 clk_ctrl_base_addr;
#define BOOTCFG_BASE_ADDR  bootcfg_base_addr
#define CLK_CTRL_BASE_ADDR clk_ctrl_base_addr

#ifdef pr_fmt
#undef pr_fmt
#define pr_fmt(fmt) "[INFRA]:%s:%d: " fmt, __func__, __LINE__
#endif

/**
 * PP HW CRU clock modules enumeration
 * @note Aligned to modules cru registers order as described in
 *       ppv4_regs.doc, DO NOT change!!!
 */
enum cru_module {
	CRU_MOD_FIRST,
	CRU_MOD_INFRA = CRU_MOD_FIRST,
	CRU_MOD_INGR_SS,
	CRU_MOD_QOS_SS,
	CRU_MOD_INGR_UC_SS,
	CRU_MOD_EGRS_UC_SS,
	CRU_MOD_CHK_TBM,
	CRU_MOD_NUM
};

/**
 * @brief Shortcut to check if PP module index is valid
 * @param m module index to check
 */
#define INFRA_IS_CRU_MODULE_VALID(m) \
	((m) >= CRU_MOD_FIRST && (m) < CRU_MOD_NUM)

/**
 * @brief Shortcut to iterate through PP cru modules
 * @param m module index loop iterator
 */
#define for_each_infra_cru_module(m) \
	for (m = CRU_MOD_FIRST; m < CRU_MOD_NUM; m++)

/**
 * @brief PP HW Modules CRU state machine states
 * @disable Module disable
 * @sync_rst TBD ask silicon team
 * @clk_disable TBD ask silicon team
 * @enable Module enable
 */
enum cru_state {
	CRU_STATE_FIRST,
	CRU_STATE_DISABLE = CRU_STATE_FIRST,
	CRU_STATE_SYNC_RST,
	CRU_STATE_CLCK_DISABLE,
	CRU_STATE_ENABLE,
	CRU_STATE_NUM,
};

/**
 * @brief Shortcut to check if CRU state is valid
 * @param s state to check
 */
#define INFRA_IS_CRU_STATE_VALID(s) \
	((s) >= CRU_STATE_FIRST && (s) < CRU_STATE_NUM)

/**
 * @brief Dynamic clock modules definitions
 */
enum dclk_module {
	DCLK_MOD_FIRST,
	DCLK_MOD_PORT_DIST = DCLK_MOD_FIRST,
	DCLK_MOD_RPB_SYS,
	DCLK_MOD_RPB_PPV4,
	DCLK_MOD_PARSER,
	DCLK_MOD_CLS_CORE,
	DCLK_MOD_CLS_SESS_CACHE,
	DCLK_MOD_CLS_CHK,
	DCLK_MOD_CLS_CHK_CACHE,
	DCLK_MOD_MOD,
	DCLK_MOD_RX_DMA,
	DCLK_MOD_BUFF_MGR,
	DCLK_MOD_QOS_WRED,
	DCLK_MOD_QOS_TSCD,
	DCLK_MOD_QOS_QMGR,
	DCLK_MOD_QOS_TX_MGR,
	DCLK_MOD_QOS_UC,
	DCLK_MOD_NUM
};

/**
 * @brief Shortcut to check if PP module index is valid
 * @param m module index to check
 */
#define INFRA_IS_DCLK_MODULE_VALID(m) \
	((m) >= DCLK_MOD_FIRST && (m) < DCLK_MOD_NUM)

/**
 * @brief for loop over dynamic clock modules shortcut
 *
 */
#define for_each_infra_dclk_mod(m) \
	for (m = DCLK_MOD_FIRST; m < DCLK_MOD_NUM; m++)

/**
 * @brief RCU module enable register, refer to following registers:
 *        infra_enable, ingress_ss_enable, qos_ss_enable,
 *        uc_ingress_enable, uc_egress_enable, chk_tbm_enable
 *      [31-1] [RO]: Reserved <br>
 *      [0]    [RW]: State configuring
 * @param m module index
 */
#define INFRA_CLK_CTRL_CRU_MOD_EN_REG(m) \
	(clk_ctrl_base_addr + ((m) << 4) + 0x0)
/**
 * @brief RCU module status register, refer to following registers:
 *        infra_status, ingress_ss_status, qos_ss_status,
 *        uc_ingress_status, uc_egress_status, chk_tbm_status
 *      [31-2] [RO]: Reserved <br>
 *      [1-0]  [RO]: State status
 * @param m module index
 */
#define INFRA_CLK_CTRL_CRU_MOD_ST_REG(m) \
	(clk_ctrl_base_addr + ((m) << 4) + 0x4)
/**
 * @brief RCU module force register, used for forcing RCU state <br>
 *        refer to following registers:
 *        infra_force, ingress_ss_force, qos_ss_force,
 *        uc_ingress_force, uc_egress_force, chk_tbm_force
 *      [31-2] [RO]: Reserved <br>
 *      [1-0]  [RW]: Force module reset and opens the clock
 *                   gater
 * @param m module index
 */
#define INFRA_CLK_CTRL_CRU_MOD_FR_REG(m) \
	(clk_ctrl_base_addr + ((m) << 4) + 0x8)

#define INFRA_GET_CRU0_OVERRIDE_EN(r)      PP_FIELD_GET((u32)BIT(0), r)
#define INFRA_GET_CRU1_OVERRIDE_EN(r)      PP_FIELD_GET((u32)BIT(1), r)
#define INFRA_GET_CRU2_OVERRIDE_EN(r)      PP_FIELD_GET((u32)BIT(2), r)
#define INFRA_GET_CRU3_OVERRIDE_EN(r)      PP_FIELD_GET((u32)BIT(3), r)
#define INFRA_GET_CRU4_OVERRIDE_EN(r)      PP_FIELD_GET((u32)BIT(4), r)
#define INFRA_GET_CRU5_OVERRIDE_EN(r)      PP_FIELD_GET((u32)BIT(5), r)
#define INFRA_GET_CRU0_OVERRIDE_VAL(r)     PP_FIELD_GET((u32)GENMASK(21, 20), r)
#define INFRA_GET_CRU1_OVERRIDE_VAL(r)     PP_FIELD_GET((u32)GENMASK(23, 22), r)
#define INFRA_GET_CRU2_OVERRIDE_VAL(r)     PP_FIELD_GET((u32)GENMASK(25, 24), r)
#define INFRA_GET_CRU3_OVERRIDE_VAL(r)     PP_FIELD_GET((u32)GENMASK(27, 26), r)
#define INFRA_GET_CRU4_OVERRIDE_VAL(r)     PP_FIELD_GET((u32)GENMASK(29, 28), r)
#define INFRA_GET_CRU5_OVERRIDE_VAL(r)     PP_FIELD_GET((u32)GENMASK(31, 30), r)

/**
 * Set Packet Processor module clock state in the specified reg
 * @reg register to copy the state to
 */
#define INFRA_SET_MODULE_CRU_STATE(reg, state) \
	PP_FIELD_MOD(GENMASK(1, 0), state, reg)

/**
 * Enable/Disable Packet Processor module force bit
 * @reg register to enable/disable the force bit
 */
#define INFRA_SET_MODULE_CRU_FORCE(reg, en) \
	PP_FIELD_MOD(BIT(0), en, reg)

#define INFRA_SET_ICG_FORCE_CLCK(r, en)        PP_FIELD_MOD((u32)BIT(0), en, r)
#define INFRA_SET_CRU0_OVR_EN(r, en)           PP_FIELD_MOD((u32)BIT(0), en, r)
#define INFRA_SET_CRU1_OVR_EN(r, en)           PP_FIELD_MOD((u32)BIT(1), en, r)
#define INFRA_SET_CRU2_OVR_EN(r, en)           PP_FIELD_MOD((u32)BIT(2), en, r)
#define INFRA_SET_CRU3_OVR_EN(r, en)           PP_FIELD_MOD((u32)BIT(3), en, r)
#define INFRA_SET_CRU4_OVR_EN(r, en)           PP_FIELD_MOD((u32)BIT(4), en, r)
#define INFRA_SET_CRU5_OVR_EN(r, en)           PP_FIELD_MOD((u32)BIT(5), en, r)
#define INFRA_SET_CRU0_OVR_VAL(r, v)    PP_FIELD_MOD((u32)GENMASK(21, 20), v, r)
#define INFRA_SET_CRU1_OVR_VAL(r, v)    PP_FIELD_MOD((u32)GENMASK(23, 22), v, r)
#define INFRA_SET_CRU2_OVR_VAL(r, v)    PP_FIELD_MOD((u32)GENMASK(25, 24), v, r)
#define INFRA_SET_CRU3_OVR_VAL(r, v)    PP_FIELD_MOD((u32)GENMASK(27, 26), v, r)
#define INFRA_SET_CRU4_OVR_VAL(r, v)    PP_FIELD_MOD((u32)GENMASK(29, 28), v, r)
#define INFRA_SET_CRU5_OVR_VAL(r, v)    PP_FIELD_MOD((u32)GENMASK(31, 30), v, r)

/**
 * @brief Convert pp cru state index into a string
 * @param s state index
 * @return const char* state in string format
 */
const char *infra_cru_state_str(enum cru_state s);

/**
 * @brief Convert PP cru module index into a string
 * @param m module index
 * @return const char* module name in string format
 */
const char *infra_cru_module_str(enum cru_module m);

/**
 * @brief Convert dynamic clock module index to a string
 * @param m the module
 * @return const char* module name in string format
 */
const char *infra_dclk_module_str(enum dclk_module m);

/**
 * @brief Reset Packet Processor HW module
 * @param m module index to reset
 * @return s32 0 on success, non-zero in case m is not valid
 */
s32 infra_reset_hw(enum cru_module m);

/**
 * @brief Enable/Disable dynamic clock for specific module
 * @param m the module
 * @param en enable/disable
 * @return s32 0 on success, error code otherwise
 */
s32 infra_mod_dclk_set(enum dclk_module m, bool en);

/**
 * @brief Get dynamic clock enable status for specific module
 * @param m the module
 * @param en enable/disable
 * @return s32 0 on success, error code otherwise
 */
s32 infra_mod_dclk_get(enum dclk_module m, bool *en);

/**
 * @brief Get whether the dynamic clock off/on for specific module
 * @param m the module
 * @param on off/on
 * @return s32 0 on success, error code otherwise
 */
s32 infra_mod_dclk_status_get(enum dclk_module m, bool *on);

/**
 * @brief Set the number of cycles before the HW modules
 *        will enter to power save mode
 * @param grace number of cycles, 0 to disable
 * @return s32 0 on success, error code otherwise
 */
s32 infra_dclk_grace_set(u32 grace);

/**
 * @brief Get the number of cycles before the HW modules
 *        will enter to power save mode
 * @param grace number of cycles
 * @return s32 0 on success, error code otherwise
 */
s32 infra_dclk_grace_get(u32 *grace);

/**
 * @brief Get CRU module clock state
 * @param m the module
 * @param s the returned state
 * @return s32 0 on success, error code otherwise
 */
s32 infra_cru_mod_state_get(enum cru_module m, enum cru_state *s);

/**
 * @brief Get whether the classifier session cache is enabled or not
 * @return bool true if cache is enabled, false otherwise
 */
bool infra_cls_sess_cache_en_get(void);

/**
 * @brief Get whether the classifier counters cache is enabled or not
 * @return bool true if cache is enabled, false otherwise
 */
bool infra_cls_cntrs_cache_en_get(void);

/**
 * @brief Get whether the rpb hw init was done or not
 * @return bool true if the init was done, false otherwise
 */
bool infra_rpb_init_done_get(void);

/**
 * @brief Get whether the port distributor init was done or not
 * @return bool true if the init was done, false otherwise
 */
bool infra_port_dist_init_done_get(void);

/**
 * @brief Get whether the egress uc timers clock is enabled or not
 * @return bool true if the clock is enable, false otherwise
 */
bool infra_eg_uc_timers_clk_en_get(void);

/**
 * @brief Get whether the ingress uc timers clock is enabled or not
 * @return bool true if the clock is enable, false otherwise
 */
bool infra_ing_uc_timers_clk_en_get(void);

#ifdef CONFIG_DEBUG_FS
/**
 * @brief Infra debug init
 * @return s32
 */
s32 infra_dbg_init(struct dentry *parent);

/**
 * @brief Infra debug cleanup
 * @return s32
 */
s32 infra_dbg_clean(void);
#else
static inline s32 infra_dbg_init(struct dentry *parent)
{
	return 0;
}

static inline s32 infra_dbg_clean(void)
{
	return 0;
}
#endif
#endif /* __INFRA_INTERNAL_H__ */
