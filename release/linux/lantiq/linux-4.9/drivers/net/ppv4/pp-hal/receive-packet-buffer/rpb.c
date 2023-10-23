/*
 * rpb.c
 * Description: PP receive packet buffer driver
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018-2020 Intel Corporation
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/spinlock.h>
#include <linux/if_ether.h>
#include "pp_logger.h"
#include "pp_regs.h"
#include "pp_common.h"
#include "rpb.h"
#include "rpb_internal.h"
#include "infra.h"

#define RPB_IS_MODE_VALID(m) \
	(m == RPB_MODE_NORMAL || m == RPB_MODE_DUAL_40)

struct rpb_database {
	/*! saved stats from last reset request */
	struct rpb_hw_stats rstats;
	/*! profiles */
	struct rpb_profile_cfg rpb_profs[RPB_PROFILE_NUM];
	/*! lock to protect hw and db accesses */
	spinlock_t lock;
	/*! current active profile */
	enum rpb_profile_id curr_rpb_prof;
	/*! RPB hw memory size, this memory is used by the HW to store
	 *  the packets during the inline stage
	 */
	u32 mem_sz;
	/*! maximum number of packets the rpb can can store in
	 *  his internal memory
	 */
	u32 max_pending_pkts;
};

static struct rpb_database *db;
u64 rpb_base_addr;
u64 rpb_drop_base_addr;

static bool __rpb_profile_validate(const struct rpb_profile_cfg *prof_cfg);
static inline bool __rpb_is_profile_id_valid(enum rpb_profile_id p);
static void __rpb_default_profile_init(void);
static void __rpb_bmgr_addr_set(u64 buf_mgr_base_addr);
static s32 __rpb_mode_set(u32 mode);
static s32 __rpb_memory_init(void);

/**
 * @brief Acquire lock
 */
static inline void __rpb_lock(void)
{
	WARN_ON(in_irq() || irqs_disabled());
	spin_lock_bh(&db->lock);
}

/**
 * @brief Release lock
 */
static inline void __rpb_unlock(void)
{
	spin_unlock_bh(&db->lock);
}

s32 rpb_dma_coal_timeout_get(u32 *to)
{
	if (unlikely(ptr_is_null(to)))
		return -EINVAL;

	*to = RPB_TIMER_LOAD_VAL_GET(PP_REG_RD32(RPB_TIMER_LOAD_VAL_REG));
	pr_debug("RPB dma coalescing timeout is %u(0x%x)\n", *to, *to);
	return 0;
}

s32 rpb_dma_coal_timeout_set(u32 to)
{
	if (to > RPB_TIMER_MAX_VAL) {
		pr_err("Error: Out of range timeout, max allowed is %u\n",
		       RPB_TIMER_MAX_VAL);
		return -EINVAL;
	}

	pr_debug("Set RPB dma coalescing timeout to %u(0x%x)\n", to, to);
	PP_REG_WR32(RPB_TIMER_LOAD_VAL_REG, to);
	return 0;
}

s32 rpb_dma_coal_threshold_get(u32 *thr)
{
	if (unlikely(ptr_is_null(thr)))
		return -EINVAL;

	*thr = RPB_DMA_COAL_THR_GET(PP_REG_RD32(RPB_DMA_COAL_THR_REG));
	pr_debug("RPB dma coalescing threshold is %u(0x%x)\n", *thr, *thr);
	return 0;
}

s32 rpb_dma_coal_threshold_set(u32 thr)
{
	if (thr > RPB_DMA_COAL_MAX_TH) {
		pr_err("Error: Out of range threshold, max allowed is %u\n",
		       RPB_DMA_COAL_MAX_TH);
		return -EINVAL;
	}

	pr_debug("Set RPB dma coalescing threshold to %u(0x%x)\n", thr, thr);
	PP_REG_WR32(RPB_DMA_COAL_THR_REG, thr);
	return 0;
}

s32 rpb_glb_high_threshold_get(u32 *high_thr)
{
	if (unlikely(ptr_is_null(high_thr)))
		return -EINVAL;

	*high_thr = RPB_THRESHOLD_GET(PP_REG_RD32(RPB_GLB_BYTES_HWM_REG));
	pr_debug("RPB global bytes high threshold is %u(0x%x)\n",
		 *high_thr, *high_thr);
	return 0;
}

s32 rpb_glb_high_threshold_set(u32 high_thr)
{
	if (high_thr > RPB_MAX_THRESHOLD) {
		pr_err("Error: Invalid threshold %u, max allowed is %u\n",
		       high_thr, RPB_MAX_THRESHOLD);
		return -EINVAL;
	}

	pr_debug("Set RPB global bytes high threshold to %u(0x%x)\n",
		 high_thr, high_thr);
	PP_REG_WR32(RPB_GLB_BYTES_HWM_REG, high_thr);
	return 0;
}

s32 rpb_glb_low_threshold_get(u32 *low_thr)
{
	if (unlikely(ptr_is_null(low_thr)))
		return -EINVAL;

	*low_thr = RPB_THRESHOLD_GET(PP_REG_RD32(RPB_GLB_BYTES_LWM_REG));
	pr_debug("RPB global bytes low threshold is %u(0x%x)\n",
		 *low_thr, *low_thr);
	return 0;
}

s32 rpb_glb_low_threshold_set(u32 low_thr)
{
	if (low_thr > RPB_MAX_THRESHOLD) {
		pr_err("Error: Invalid threshold %u, max allowed is %u\n",
		       low_thr, RPB_MAX_THRESHOLD);
		return -EINVAL;
	}

	pr_debug("Set RPB global bytes low threshold to %u(0x%x)\n",
		 low_thr, low_thr);
	PP_REG_WR32(RPB_GLB_BYTES_LWM_REG, low_thr);
	return 0;
}

s32 rpb_glb_pkts_high_threshold_get(u32 *high_thr)
{
	if (unlikely(ptr_is_null(high_thr)))
		return -EINVAL;

	*high_thr = RPB_THRESHOLD_GET(PP_REG_RD32(RPB_GLB_PKTS_HWM_REG));
	pr_debug("RPB global packets high threshold is %u(0x%x)\n",
		 *high_thr, *high_thr);
	return 0;
}

s32 rpb_glb_pkts_high_threshold_set(u32 high_thr)
{
	if (high_thr > RPB_MAX_THRESHOLD) {
		pr_err("Error: Invalid threshold %u, max allowed is %u\n",
		       high_thr, RPB_MAX_THRESHOLD);
		return -EINVAL;
	}

	pr_debug("Set RPB global packets high threshold to %u(0x%x)\n",
		 high_thr, high_thr);
	PP_REG_WR32(RPB_GLB_PKTS_HWM_REG, high_thr);
	return 0;
}

s32 rpb_glb_pkts_low_threshold_get(u32 *low_thr)
{
	if (unlikely(ptr_is_null(low_thr)))
		return -EINVAL;

	*low_thr = RPB_THRESHOLD_GET(PP_REG_RD32(RPB_GLB_PKTS_LWM_REG));
	pr_debug("RPB global packets low threshold is %u(0x%x)\n",
		 *low_thr, *low_thr);
	return 0;
}

s32 rpb_glb_pkts_low_threshold_set(u32 low_thr)
{
	if (low_thr > RPB_MAX_THRESHOLD) {
		pr_err("Error: Invalid threshold %u, max allowed is %u\n",
		       low_thr, RPB_MAX_THRESHOLD);
		return -EINVAL;
	}

	pr_debug("Set RPB global packets low threshold to %u(0x%x)\n",
		 low_thr, low_thr);
	PP_REG_WR32(RPB_GLB_PKTS_LWM_REG, low_thr);
	return 0;
}

s32 rpb_tc_high_prio_get(u32 port, u32 tc, bool *high_prio)
{
	u32 reg;

	if (unlikely(ptr_is_null(high_prio)))
		return -EINVAL;

	if (!__rpb_is_port_valid(port) || !__rpb_is_tc_valid(tc))
		return -EINVAL;

	__rpb_lock();
	reg = PP_REG_RD32(RPB_TC_HI_PRI_REG(port));
	__rpb_unlock();

	*high_prio = RPB_TC_HI_PRI_GET(reg, tc);
	pr_debug("RPB port%u tc%u high priority is %u, reg 0x%x\n",
		 port, tc, *high_prio, reg);
	return 0;
}

s32 rpb_tc_high_prio_set(u32 port, u32 tc, bool hi_prio)
{
	u32 reg;

	if (!__rpb_is_port_valid(port) || !__rpb_is_tc_valid(tc))
		return -EINVAL;

	__rpb_lock();
	reg = PP_REG_RD32(RPB_TC_HI_PRI_REG(port));
	pr_debug("%s high priority for port %u tc %u, 0x%llx = 0x%x --> 0x%x\n",
		 BOOL2EN(hi_prio), port, tc, RPB_TC_HI_PRI_REG(port), reg,
		 RPB_TC_HI_PRI_SET(reg, tc, hi_prio));
	reg = RPB_TC_HI_PRI_SET(reg, tc, hi_prio);
	PP_REG_WR32(RPB_TC_HI_PRI_REG(port), reg);
	__rpb_unlock();

	return 0;
}

s32 rpb_tc_flow_ctrl_get(u32 port, u32 tc, bool *flow_ctrl)
{
	u32 reg;

	if (unlikely(ptr_is_null(flow_ctrl)))
		return -EINVAL;
	if (!__rpb_is_port_valid(port) || !__rpb_is_tc_valid(tc))
		return -EINVAL;

	__rpb_lock();
	reg = PP_REG_RD32(RPB_TC_FLOW_CTRL_REG(port));
	__rpb_unlock();

	*flow_ctrl = RPB_TC_FLOW_CTRL_GET(reg, tc);
	pr_debug("RPB port%u tc%u flow control is %u, reg 0x%x\n",
		 port, tc, *flow_ctrl, reg);
	return 0;
}

s32 rpb_tc_flow_ctrl_set(u32 port, u32 tc, bool en)
{
	u32 reg;

	if (!__rpb_is_port_valid(port) || !__rpb_is_tc_valid(tc))
		return -EINVAL;

	__rpb_lock();
	reg = PP_REG_RD32(RPB_TC_FLOW_CTRL_REG(port));
	pr_debug("%s flow control for port %u tc %u, 0x%llx = 0x%x --> 0x%x\n",
		 BOOL2EN(en), port, tc, RPB_TC_FLOW_CTRL_REG(port), reg,
		 RPB_TC_FLOW_CTRL_SET(reg, tc, en));
	reg = RPB_TC_FLOW_CTRL_SET(reg, tc, en);
	PP_REG_WR32(RPB_TC_FLOW_CTRL_REG(port), reg);
	__rpb_unlock();

	return 0;
}

s32 rpb_tc_dpool_sz_get(u32 port, u32 tc, u32 *dpool_sz)
{
	if (unlikely(ptr_is_null(dpool_sz)))
		return -EINVAL;
	if (!__rpb_is_port_valid(port) || !__rpb_is_tc_valid(tc))
		return -EINVAL;

	*dpool_sz = RPB_POOL_SIZE_GET(PP_REG_RD32(
		RPB_TC_DPOOL_SZ_REG(port, tc)));
	pr_debug("RPB port%u tc%u dedicated pool size is %u(0x%x)\n",
		 port, tc, *dpool_sz, *dpool_sz);
	return 0;
}

s32 rpb_tc_dpool_sz_set(u32 port, u32 tc, u32 dpool_sz)
{
	if (dpool_sz > RPB_MAX_POOL_SIZE) {
		pr_err("Error: Invalid pool size %u, max allowed is %u\n",
		       dpool_sz, RPB_MAX_POOL_SIZE);
		return -EINVAL;
	}
	if (!__rpb_is_port_valid(port) || !__rpb_is_tc_valid(tc))
		return -EINVAL;

	pr_debug("Set RPB port%u tc%u dedicated pool size to %u(0x%x)\n",
		 port, tc, dpool_sz, dpool_sz);
	PP_REG_WR32(RPB_TC_DPOOL_SZ_REG(port, tc), dpool_sz);
	return 0;
}

s32 rpb_tc_dpool_high_threshold_get(u32 port, u32 tc, u32 *high_thr)
{
	if (unlikely(ptr_is_null(high_thr)))
		return -EINVAL;
	if (!__rpb_is_port_valid(port) || !__rpb_is_tc_valid(tc))
		return 0;

	*high_thr = RPB_THRESHOLD_GET(PP_REG_RD32(
		RPB_TC_DPOOL_HWM_REG(port, tc)));
	pr_debug("RPB port%u tc%u dedicated pool high threshold is %u(0x%x)\n",
		 port, tc, *high_thr, *high_thr);
	return 0;
}

s32 rpb_tc_dpool_high_threshold_set(u32 port, u32 tc, u32 high_thr)
{
	if (high_thr > RPB_MAX_THRESHOLD) {
		pr_err("Error: Invalid threshold %u, max allowed is %u\n",
		       high_thr, RPB_MAX_THRESHOLD);
		return -EINVAL;
	}
	if (!__rpb_is_port_valid(port) || !__rpb_is_tc_valid(tc))
		return -EINVAL;

	pr_debug("Set RPB port%u tc%u dedicated pool high threshold to %u(0x%x)\n",
		 port, tc, high_thr, high_thr);
	PP_REG_WR32(RPB_TC_DPOOL_HWM_REG(port, tc), high_thr);
	return 0;
}

s32 rpb_tc_dpool_low_threshold_get(u32 port, u32 tc, u32 *low_thr)
{
	if (unlikely(ptr_is_null(low_thr)))
		return -EINVAL;
	if (!__rpb_is_port_valid(port) || !__rpb_is_tc_valid(tc))
		return -EINVAL;

	*low_thr = RPB_THRESHOLD_GET(PP_REG_RD32(
		RPB_TC_DPOOL_LWM_REG(port, tc)));
	pr_debug("RPB port%u tc%u dedicated pool low threshold is %u(0x%x)\n",
		 port, tc, *low_thr, *low_thr);
	return 0;
}

s32 rpb_tc_dpool_low_threshold_set(u32 port, u32 tc, u32 low_thr)
{
	if (low_thr > RPB_MAX_THRESHOLD) {
		pr_err("Error: Invalid threshold %u, max allowed is %u\n",
		       low_thr, RPB_MAX_THRESHOLD);
		return -EINVAL;
	}
	if (!__rpb_is_port_valid(port) || !__rpb_is_tc_valid(tc))
		return -EINVAL;

	pr_debug("Set RPB port%u tc%u dedicated pool low threshold to %u(0x%x)\n",
		 port, tc, low_thr, low_thr);
	PP_REG_WR32(RPB_TC_DPOOL_LWM_REG(port, tc), low_thr);
	return 0;
}

s32 rpb_tc_spool_high_threshold_get(u32 port, u32 tc, u32 *high_thr)
{
	if (unlikely(ptr_is_null(high_thr)))
		return -EINVAL;
	if (!__rpb_is_port_valid(port) || !__rpb_is_tc_valid(tc))
		return -EINVAL;

	*high_thr = RPB_THRESHOLD_GET(PP_REG_RD32(
		RPB_TC_SPOOL_HTHR_REG(port, tc)));
	pr_debug("RPB port%u tc%u shared pool high threshold is %u(0x%x)\n",
		 port, tc, *high_thr, *high_thr);
	return 0;
}

s32 rpb_tc_spool_high_threshold_set(u32 port, u32 tc, u32 high_thr)
{
	if (high_thr > RPB_MAX_THRESHOLD) {
		pr_err("Error: Invalid threshold %u, max allowed is %u\n\n",
		       high_thr, RPB_MAX_THRESHOLD);
		return -EINVAL;
	}
	if (!__rpb_is_port_valid(port) || !__rpb_is_tc_valid(tc))
		return -EINVAL;

	pr_debug("Set RPB port%u tc%u shared pool high threshold to %u(0x%x)\n",
		 port, tc, high_thr, high_thr);
	PP_REG_WR32(RPB_TC_SPOOL_HTHR_REG(port, tc), high_thr);
	return 0;
}

s32 rpb_tc_spool_low_threshold_get(u32 port, u32 tc, u32 *low_thr)
{
	if (unlikely(ptr_is_null(low_thr)))
		return -EINVAL;
	if (!__rpb_is_port_valid(port) || !__rpb_is_tc_valid(tc))
		return -EINVAL;

	*low_thr = RPB_THRESHOLD_GET(PP_REG_RD32(
		RPB_TC_SPOOL_LTHR_REG(port, tc)));
	pr_debug("RPB port%u tc%u shared pool low threshold is %u(0x%x)\n",
		 port, tc, *low_thr, *low_thr);
	return 0;
}

s32 rpb_tc_spool_low_threshold_set(u32 port, u32 tc, u32 low_thr)
{
	if (low_thr > RPB_MAX_THRESHOLD) {
		pr_err("Error: Invalid threshold %u, max allowed is %u\n",
		       low_thr, RPB_MAX_THRESHOLD);
		return -EINVAL;
	}
	if (!__rpb_is_port_valid(port) || !__rpb_is_tc_valid(tc))
		return -EINVAL;

	pr_debug("Set RPB port%u tc%u shared pool low threshold to %u(0x%x)\n",
		 port, tc, low_thr, low_thr);
	PP_REG_WR32(RPB_TC_SPOOL_LTHR_REG(port, tc), low_thr);
	return 0;
}

s32 rpb_port_spool_sz_get(u32 port, u32 *spool_sz)
{
	if (unlikely(ptr_is_null(spool_sz)))
		return -EINVAL;
	if (!__rpb_is_port_valid(port))
		return -EINVAL;

	*spool_sz = RPB_POOL_SIZE_GET(PP_REG_RD32(RPB_PORT_SPOOL_SZ_REG(port)));
	pr_debug("RPB port%u shared pool size is %u(0x%x)\n",
		 port, *spool_sz, *spool_sz);
	return 0;
}

s32 rpb_port_spool_sz_set(u32 port, u32 spool_sz)
{
	if (spool_sz > RPB_MAX_POOL_SIZE) {
		pr_err("Error: Invalid pool size %u, max allowed is %u\n",
		       spool_sz, RPB_MAX_POOL_SIZE);
		return -EINVAL;
	}
	if (!__rpb_is_port_valid(port))
		return -EINVAL;

	pr_debug("Set RPB port%u shared pool size to %u(0x%x)\n",
		 port, spool_sz, spool_sz);
	PP_REG_WR32(RPB_PORT_SPOOL_SZ_REG(port), spool_sz);
	return 0;
}

s32 rpb_port_high_threshold_get(u32 port, u32 *high_thr)
{
	if (unlikely(ptr_is_null(high_thr)))
		return -EINVAL;
	if (!__rpb_is_port_valid(port))
		return -EINVAL;

	*high_thr = RPB_THRESHOLD_GET(PP_REG_RD32(
		RPB_PORT_SPOOL_HWM_REG(port)));
	pr_debug("RPB port%u high threshold is %u(0x%x)\n",
		 port, *high_thr, *high_thr);
	return 0;
}

s32 rpb_port_high_threshold_set(u32 port, u32 high_thr)
{
	if (high_thr > RPB_MAX_THRESHOLD) {
		pr_err("Error: Invalid threshold %u, max allowed is %u\n",
		       high_thr, RPB_MAX_THRESHOLD);
		return -EINVAL;
	}
	if (!__rpb_is_port_valid(port))
		return -EINVAL;

	pr_debug("Set RPB port%u high threshold to %u(0x%x)\n",
		 port, high_thr, high_thr);
	PP_REG_WR32(RPB_PORT_SPOOL_HWM_REG(port), high_thr);
	return 0;
}

s32 rpb_port_low_threshold_get(u32 port, u32 *low_thr)
{
	if (unlikely(ptr_is_null(low_thr)))
		return -EINVAL;
	if (!__rpb_is_port_valid(port))
		return -EINVAL;

	*low_thr = RPB_THRESHOLD_GET(PP_REG_RD32(
		RPB_PORT_SPOOL_LWM_REG(port)));
	pr_debug("RPB port%u low threshold is %u(0x%x)\n",
		 port, *low_thr, *low_thr);
	return 0;
}

s32 rpb_port_low_threshold_set(u32 port, u32 low_thr)
{
	if (low_thr > RPB_MAX_THRESHOLD) {
		pr_err("Error: Invalid threshold %u, max allowed is %u\n",
		       low_thr, RPB_MAX_THRESHOLD);
		return -EINVAL;
	}
	if (!__rpb_is_port_valid(port))
		return -EINVAL;

	pr_debug("Set RPB port%u low threshold to %u(0x%x)\n",
		 port, low_thr, low_thr);
	PP_REG_WR32(RPB_PORT_SPOOL_LWM_REG(port), low_thr);
	return 0;
}

static bool __rpb_profile_validate(const struct rpb_profile_cfg *prof_cfg)
{
	const struct rpb_port_cfg *port_cfg;
	const struct rpb_tc_cfg   *tc_cfg;
	u32 p, tc, total_pools_sum = 0;
	bool result = true;

	if (!prof_cfg) {
		pr_err("Error: Null profile\n");
		return false;
	}

	if (prof_cfg->high_thr > db->mem_sz) {
		pr_debug("global high threshold %u is greater than total memory size %u\n",
			 prof_cfg->high_thr, db->mem_sz);
		result = false;
	}
	if (prof_cfg->low_thr > prof_cfg->high_thr) {
		pr_debug("global low threshold %u is greater or equal than global high threshold %u\n",
			 prof_cfg->low_thr, prof_cfg->high_thr);
		result = false;
	}
	if (prof_cfg->pkt_high_thr > db->max_pending_pkts) {
		pr_debug("global high packets threshold %u is greater than the maximum %u\n",
			 prof_cfg->pkt_high_thr, db->max_pending_pkts);
		result = false;
	}
	if (prof_cfg->pkt_low_thr > prof_cfg->pkt_high_thr) {
		pr_debug("global low packets threshold %u is greater or equal than the global low packets threshold %u\n",
			 prof_cfg->pkt_low_thr, prof_cfg->pkt_high_thr);
		result = false;
	}

	RPB_FOR_EACH_PORT(p) {
		port_cfg   = &prof_cfg->port[p];
		total_pools_sum += port_cfg->pool_sz;
		if (port_cfg->high_thr > port_cfg->pool_sz) {
			pr_debug("port%u high threshold %u is greater than his pool size %u\n",
				 p, port_cfg->high_thr, port_cfg->pool_sz);
			result = false;
		}
		if (port_cfg->low_thr > port_cfg->high_thr) {
			pr_debug("port%u low threshold %u is greater or equal than the high threshold %u\n",
				 p, port_cfg->low_thr, port_cfg->high_thr);
			result = false;
		}
		RPB_FOR_EACH_TC(tc) {
			tc_cfg = &port_cfg->tc[tc];
			total_pools_sum += tc_cfg->dpool_sz;
			if (tc_cfg->dpool_high_thr > tc_cfg->dpool_sz) {
				pr_debug("port%u tc%u high threshold %u is greater than his pool size %u\n",
					 p, tc, tc_cfg->dpool_high_thr,
					 tc_cfg->dpool_sz);
				result = false;
			}
			if (tc_cfg->dpool_low_thr > tc_cfg->dpool_high_thr) {
				pr_debug("port%u tc%u low threshold %u is greater or equal than the high threshold %u\n",
					 p, tc, tc_cfg->dpool_low_thr,
					 tc_cfg->dpool_high_thr);
				result = false;
			}
			if (tc_cfg->spool_high_thr > port_cfg->pool_sz) {
				pr_debug("port%u tc%u high threshold %u is greater than the port shared pool %u\n",
					 p, tc, tc_cfg->spool_high_thr,
					 port_cfg->pool_sz);
				result = false;
			}
			if (tc_cfg->spool_low_thr > tc_cfg->spool_high_thr) {
				pr_debug("port%u tc%u low threshold %u is greater or equal than the high threshold %u\n",
					 p, tc, tc_cfg->spool_low_thr,
					 tc_cfg->spool_high_thr);
				result = false;
			}
		}
	}
	/* check total memory allocated by all ports and TCs */
	if (total_pools_sum > db->mem_sz) {
		pr_debug("Total ports and TCs pools size %u is greater than total memory size %u\n",
			 total_pools_sum, db->mem_sz);
		result = false;
	}

	return result;
}

/**
 * @brief Read TC configuration from RPB registers.
 * @param port port index
 * @param tc index
 * @param tc_cfg tc configuration buffer to store the configuration
 * @return s32 0 on success, non-zero value otherwise
 */
s32 rpb_tc_cfg_get(u32 port, u32 tc, struct rpb_tc_cfg *cfg)
{
	u32 ret;

	if (unlikely(ptr_is_null(cfg)))
		return -EINVAL;

	if (!__rpb_is_port_valid(port) || !__rpb_is_tc_valid(tc))
		return -EINVAL;

	memset(cfg, 0, sizeof(*cfg));

	ret  = rpb_tc_dpool_sz_get(port, tc, &cfg->dpool_sz);
	ret |= rpb_tc_high_prio_get(port, tc, &cfg->hi_prio);
	ret |= rpb_tc_flow_ctrl_get(port, tc, &cfg->flow_ctrl);
	ret |= rpb_tc_dpool_high_threshold_get(port, tc, &cfg->dpool_high_thr);
	ret |= rpb_tc_dpool_low_threshold_get(port, tc, &cfg->dpool_low_thr);
	ret |= rpb_tc_spool_high_threshold_get(port, tc, &cfg->spool_high_thr);
	ret |= rpb_tc_spool_low_threshold_get(port, tc, &cfg->spool_low_thr);
	if (unlikely(ret))
		pr_err("Failed to get port%u tc%u configuration\n", port, tc);

	return ret;
}

/**
 * @brief Write TC configuration to RPB registers.
 * @param port port index
 * @param tc index
 * @param tc_cfg tc configuration
 * @return s32 0 on success, non-zero value otherwise
 */
s32 rpb_tc_cfg_set(u32 port, u32 tc, const struct rpb_tc_cfg *cfg)
{
	u32 ret;

	if (!cfg)
		return -EINVAL;

	if (!__rpb_is_port_valid(port) || !__rpb_is_tc_valid(tc))
		return -EINVAL;

	/* First we disable the dedicated pool */
	ret  = rpb_tc_dpool_sz_set(port, tc, 0);
	/* Set flow control and high priority */
	ret |= rpb_tc_high_prio_set(port, tc, cfg->hi_prio);
	ret |= rpb_tc_flow_ctrl_set(port, tc, cfg->flow_ctrl);
	/* Set thresholds */
	ret |= rpb_tc_dpool_high_threshold_set(port, tc, cfg->dpool_high_thr);
	ret |= rpb_tc_dpool_low_threshold_set(port, tc, cfg->dpool_low_thr);
	/* Now we can enable the dedicated pool */
	ret |= rpb_tc_dpool_sz_set(port, tc, cfg->dpool_sz);
	/* Set thresholds */
	ret |= rpb_tc_spool_high_threshold_set(port, tc, cfg->spool_high_thr);
	ret |= rpb_tc_spool_low_threshold_set(port, tc, cfg->spool_low_thr);

	if (unlikely(ret))
		pr_err("Failed to set port%u tc%u configuration\n", port, tc);

	return ret;
}

/**
 * @brief Read RPB port configuration to RPB registers.
 * @param port port index
 * @param port_cfg port configuration buffer to store the
 *                 configuration
 * @return s32 0 on success, non-zero value otherwise
 */
static s32 rpb_port_cfg_get(u32 port, struct rpb_port_cfg *port_cfg)
{
	struct rpb_tc_cfg *tc_cfg;
	u32 tc, ret;

	if (unlikely(ptr_is_null(port_cfg)))
		return -EINVAL;

	memset(port_cfg, 0, sizeof(*port_cfg));

	/* Read port configuration */
	ret  = rpb_port_spool_sz_get(port, &port_cfg->pool_sz);
	ret |= rpb_port_high_threshold_get(port, &port_cfg->high_thr);
	ret |= rpb_port_low_threshold_get(port, &port_cfg->low_thr);
	if (unlikely(ret)) {
		pr_err("Failed to get port%u configuration\n", port);
		return ret;
	}

	port_cfg->is_mem =
		db->rpb_profs[db->curr_rpb_prof].port[port].is_mem;
	port_cfg->flow_ctrl =
		db->rpb_profs[db->curr_rpb_prof].port[port].flow_ctrl;
	port_cfg->num_of_pp_ports =
		db->rpb_profs[db->curr_rpb_prof].port[port].num_of_pp_ports;

	/* Read TCs configuration */
	RPB_FOR_EACH_TC(tc) {
		tc_cfg = &port_cfg->tc[tc];
		ret = rpb_tc_cfg_get(port, tc, tc_cfg);
		if (unlikely(ret)) {
			pr_err("Error: Failed to read port%u tc%u configuration\n",
			       port, tc);
			return ret;
		}
	}

	return 0;
}

/**
 * @brief Write RPB port configuration to RPB registers.
 * @param port port index
 * @param port_cfg port configuration
 * @return s32 0 on success, non-zero value otherwise
 */
static s32 rpb_port_cfg_set(u32 port, const struct rpb_port_cfg *port_cfg)
{
	const struct rpb_tc_cfg *tc_cfg;
	u32 tc, ret;

	if (unlikely(ptr_is_null(port_cfg)))
		return -EINVAL;

	if (!__rpb_is_port_valid(port))
		return -EINVAL;

	/* First we disable the port shared pool */
	ret  = rpb_port_spool_sz_set(port, 0);
	ret |= rpb_port_high_threshold_set(port, port_cfg->high_thr);
	ret |= rpb_port_low_threshold_set(port, port_cfg->low_thr);
	if (unlikely(ret)) {
		pr_err("Error: Failed to set port%u config\n", port);
		return ret;
	}

	/* TCs configuration */
	RPB_FOR_EACH_TC(tc) {
		tc_cfg = &port_cfg->tc[tc];
		ret = rpb_tc_cfg_set(port, tc, tc_cfg);
		if (unlikely(ret)) {
			pr_err("Error: Failed to set port%u tc%u config\n",
			       port, tc);
			return ret;
		}
	}
	/* now enable the shared pool */
	return rpb_port_spool_sz_set(port, port_cfg->pool_sz);
}

s32 rpb_profile_cfg_set(const struct rpb_profile_cfg *prof_cfg)
{
	u32 port;
	u32 ret = 0;

	if (!prof_cfg)
		return -EINVAL;

	if (__rpb_profile_validate(prof_cfg) == false) {
		pr_err("Error: Illegal RPB configuration profile\n");
		return -EINVAL;
	}

	/* global thresholds */
	ret |= rpb_glb_high_threshold_set(prof_cfg->high_thr);
	ret |= rpb_glb_low_threshold_set(prof_cfg->low_thr);
	/* global packets thresholds */
	ret |= rpb_glb_pkts_high_threshold_set(prof_cfg->pkt_high_thr);
	ret |= rpb_glb_pkts_low_threshold_set(prof_cfg->pkt_low_thr);
	if (unlikely(ret))
		return ret;

	/* ports configurations */
	RPB_FOR_EACH_PORT(port) {
		ret = rpb_port_cfg_set(port, &prof_cfg->port[port]);
		if (unlikely(ret)) {
			pr_err("Error: Failed to write port%u to hw\n", port);
			return ret;
		}
	}

	return 0;
}

s32 rpb_profile_cfg_get(struct rpb_profile_cfg *prof_cfg)
{
	u32 port, ret;

	if (!prof_cfg)
		return -EINVAL;

	memset(prof_cfg, 0, sizeof(*prof_cfg));

	/* Read global thresholds */
	ret  = rpb_glb_high_threshold_get(&prof_cfg->high_thr);
	ret |= rpb_glb_low_threshold_get(&prof_cfg->low_thr);
	/* Read global packets threshold */
	ret |= rpb_glb_pkts_high_threshold_get(&prof_cfg->pkt_high_thr);
	ret |= rpb_glb_pkts_low_threshold_get(&prof_cfg->pkt_low_thr);
	if (unlikely(ret)) {
		pr_err("Failed to get global configurations\n");
		return -EPERM;
	}

	/* Read ports configurations */
	RPB_FOR_EACH_PORT(port) {
		ret = rpb_port_cfg_get(port, &prof_cfg->port[port]);
		if (unlikely(ret)) {
			pr_err("Error: Failed to read port%u configuration\n",
			       port);
			return ret;
		}
	}

	return 0;
}

s32 rpb_bmgr_addr_get(u64 *addr)
{
	if (unlikely(ptr_is_null(addr)))
		return -EINVAL;

	*addr = PP_BUILD_64BIT_WORD(PP_REG_RD32(PP_RPB_DRP_BM_BASE_ADDRL_REG),
				    PP_REG_RD32(PP_RPB_DRP_BM_BASE_ADDRH_REG));
	return 0;
}

s32 rpb_tc_stats_get(u32 port, u32 tc, struct rpb_tc_stats *stats)
{
	if (unlikely(ptr_is_null(stats)))
		return -EINVAL;
	if (!__rpb_is_port_valid(port) || !__rpb_is_tc_valid(tc))
		return -EINVAL;

	memset(stats, 0, sizeof(*stats));

	stats->drop_bytes = PP_REG_RD32(RPB_TC_BYTES_DROP_CNT_REG(port, tc));
	stats->drop_packets = PP_REG_RD32(RPB_TC_PKTS_DROP_CNT_REG(port, tc));
	stats->dpool_bytes_used = RPB_TC_DPOOL_FILLING_CNT_GET(
		PP_REG_RD32(RPB_TC_DPOOL_FILLING_CNT_REG(port, tc)));
	stats->spool_bytes_used = RPB_TC_SPOOL_OCC_CNT_GET(
		PP_REG_RD32(RPB_TC_SPOOL_OCC_CNT_REG(port, tc)));

	return 0;
}

s32 rpb_port_stats_get(u32 port, struct rpb_port_stats *stats)
{
	u32 tc;

	if (unlikely(ptr_is_null(stats)))
		return -EINVAL;
	if (!__rpb_is_port_valid(port))
		return -EINVAL;

	memset(stats, 0, sizeof(*stats));

	stats->total_bytes_used = RPB_PORT_FILLING_CNT_GET(
		PP_REG_RD32(RPB_PORT_FILLING_CNT_REG(port)));
	stats->spool_bytes_used = RPB_PORT_SPOOL_FILLING_CNT_GET(
		PP_REG_RD32(RPB_PORT_SPOOL_FILLING_CNT_REG(port)));
	stats->rx_packets = PP_REG_RD32(PP_RPB_PORT_IN_PKT_CNT_REG(port));

	RPB_FOR_EACH_TC(tc) {
		if (rpb_tc_stats_get(port, tc, &stats->tc[tc])) {
			pr_err("Error: Failed to get port %u tc %u stats\n",
			       port, tc);
			return -EPERM;
		}
	}

	return 0;
}

s32 rpb_port_back_pressure_status_get(u32 port, bool *back_pres)
{
	if (unlikely(ptr_is_null(back_pres)))
		return -EINVAL;
	if (!__rpb_is_port_valid(port))
		return -EINVAL;

	*back_pres = RPB_PORT_BACK_PRS_STATUS_GET(
		PP_REG_RD32(RPB_BACK_PRS_STATUS_REG), port);
	return 0;
}

static void __rpb_hw_stats_get(struct rpb_hw_stats *stats)
{
	u32 port;

	memset(stats, 0, sizeof(*stats));

	stats->total_bytes_used =
		RPB_GLB_FILLING_CNT_GET(PP_REG_RD32(RPB_GLB_FILLING_CNT_REG));
	stats->total_pkts_used =
		RPB_GLB_PKTS_CNT_GET(PP_REG_RD32(RPB_GLB_PKTS_CNT_REG));
	stats->total_packets_rx_dma_recycled =
		PP_REG_RD32(PP_RPB_RXDMA_RLS_CNT_REG);

	RPB_FOR_EACH_PORT(port)
		rpb_port_stats_get(port, &stats->port[port]);
}

s32 rpb_hw_stats_get(struct rpb_hw_stats *stats)
{
	if (unlikely(ptr_is_null(stats)))
		return -EINVAL;

	__rpb_hw_stats_get(stats);
	U32_STRUCT_DIFF(&db->rstats, stats, stats);

	return 0;
}

void rpb_stats_reset(void)
{
	__rpb_hw_stats_get(&db->rstats);
}

s32 rpb_memory_sz_get(u32 *sz)
{
	if (unlikely(ptr_is_null(sz)))
		return -EINVAL;

	*sz = db->mem_sz;
	return 0;
}

s32 rpb_max_pending_pkts_get(u32 *max)
{
	if (unlikely(ptr_is_null(max)))
		return -EINVAL;

	*max = db->max_pending_pkts;
	return 0;
}

s32 rpb_stats_get(struct rpb_stats *stats)
{
	u32 port, tc;
	struct rpb_hw_stats hw_stats;

	if (unlikely(ptr_is_null(stats)))
		return -EINVAL;

	rpb_hw_stats_get(&hw_stats);

	memset(stats, 0, sizeof(struct rpb_stats));

	RPB_FOR_EACH_PORT(port) {
		RPB_FOR_EACH_TC(tc) {
			stats->total_packets_dropped +=
				hw_stats.port[port].tc[tc].drop_packets;
		}
		stats->pkts_rcvd_from_port_dist +=
			hw_stats.port[port].rx_packets;
	}

	stats->total_packets_rx_dma_recycled =
		hw_stats.total_packets_rx_dma_recycled;

	return 0;
}

/**
 * @brief Check if RPB profile is valid, print error message in
 *        case not
 * @note Use 'RPB_IS_PROFILE_ID_VALID' macro to avoid error
 *       print
 * @param p RPB profile id
 * @return bool true in case profile is valid, false otherwise
 */
static inline bool __rpb_is_profile_id_valid(enum rpb_profile_id p)
{
	if (likely(RPB_IS_PROFILE_ID_VALID(p)))
		return true;

	pr_err("Invalid rpb profile id %u\n", p);
	return false;
}

s32 rpb_custom_profile_set(struct rpb_profile_cfg *prof)
{
	if (unlikely(ptr_is_null(prof)))
		return -EINVAL;
	if (!__rpb_profile_validate(prof)) {
		pr_err("Invalid RPB profile custom profile\n");
		return -EINVAL;
	}

	memcpy(&db->rpb_profs[RPB_PROFILE_COSTUM], prof, sizeof(*prof));

	return rpb_profile_set(RPB_PROFILE_COSTUM);
}

s32 rpb_profile_set(enum rpb_profile_id profile_id)
{
	if (!__rpb_is_profile_id_valid(profile_id))
		return -EINVAL;

	db->curr_rpb_prof = profile_id;
	return rpb_profile_cfg_set(&db->rpb_profs[profile_id]);
}

s32 rpb_profile_get(enum rpb_profile_id prof_id, struct rpb_profile_cfg *prof)
{
	if (!__rpb_is_profile_id_valid(prof_id))
		return -EINVAL;

	memcpy(prof, &db->rpb_profs[prof_id], sizeof(*prof));

	return 0;
}

s32 rpb_profile_id_get(enum rpb_profile_id *curr_id)
{
	if (unlikely(ptr_is_null(curr_id)))
		return -EINVAL;

	*curr_id = db->curr_rpb_prof;
	return 0;
}

s32 rpb_pp_port_map(bool is_mem, bool flow_ctrl, u16 *rpb)
{
	struct rpb_profile_cfg *prof_cfg;
	u32 mapped = U32_MAX;
	u16 p, selected_rpb_port = PP_MAX_RPB_PORT;
	s32 ret = 0;

	if (unlikely(ptr_is_null(rpb)))
		return -EINVAL;

	__rpb_lock();
	prof_cfg = &db->rpb_profs[db->curr_rpb_prof];

	RPB_FOR_EACH_PORT(p) {
		if (is_mem != prof_cfg->port[p].is_mem)
			continue;
		if ((prof_cfg->port[p].num_of_pp_ports < mapped)) {
			mapped = prof_cfg->port[p].num_of_pp_ports;
			selected_rpb_port = p;
		}
	}

	if (unlikely(!PP_IS_RPB_PORT_VALID(selected_rpb_port))) {
		ret = -EINVAL;
		goto unlock;
	}

	prof_cfg->port[selected_rpb_port].num_of_pp_ports++;
	*rpb = selected_rpb_port;

unlock:
	__rpb_unlock();
	return ret;
}

s32 rpb_pp_port_unmap(u16 rpb)
{
	struct rpb_profile_cfg *prof_cfg;

	if (likely(PP_IS_RPB_PORT_VALID(rpb))) {
		__rpb_lock();
		prof_cfg = &db->rpb_profs[db->curr_rpb_prof];
		prof_cfg->port[rpb].num_of_pp_ports--;
		__rpb_unlock();
		return 0;
	}

	return -EINVAL;
}

void rpb_rcu_egress_set(bool enable)
{
	u32 reg, val = enable ? 0 : 1;

	__rpb_lock();
	reg = PP_REG_RD32(PP_RPB_DBG_FEAT_REG);
	reg = PP_FIELD_MOD(PP_RPB_DBG_FEAT_DIS_RCU_EGR_MSK, val, reg);
	PP_REG_WR32(PP_RPB_DBG_FEAT_REG, reg);
	__rpb_unlock();
}

/**
 * @brief Initial default RPB profile <br>
 *        Port 0: for streaming ports with flow control enabled, get
 *                32KB total memory including shared and TC
 *                dedicated <br>
 *                Shared pool of 88KB <br>
 *                TC0: high priority and a dedicated pool of 8KB <br>
 *                TC1-3: low priority with no dedicated pool <br>
 *        Port 1: same as port 0 just with flow control disabled <br>
 *        Port 2: for streaming ports with flow control enabled, get
 *        96KB total memory including shared and TC dedicated <br>
 *               Shared pool of 24KB <br>
 *               TC0: high priority and a dedicated pool of 8KB <br>
 *               TC1-3: low priority with no dedicated pool <br>
 *        Port 3: same as port 2 just with flow control disabled <br>
 * @return void
 */
static void __rpb_default_profile_init(void)
{
	struct rpb_profile_cfg *prof_cfg;
	struct rpb_port_cfg *port_cfg;
	struct rpb_tc_cfg   *tc_cfg;
	u32 p, tc, high_delta, low_delta;

	prof_cfg = &db->rpb_profs[RPB_PROFILE_DEFAULT];

	/* global bytes watermarks */
	prof_cfg->high_thr = db->mem_sz - (1  * ETH_FRAME_LEN);
	prof_cfg->low_thr  = db->mem_sz - (10 * ETH_FRAME_LEN);

	/* global packets watermarks */
	if (db->max_pending_pkts > 500) {
		prof_cfg->pkt_high_thr = db->max_pending_pkts - 148;
		prof_cfg->pkt_low_thr  = db->max_pending_pkts - 348;
	} else {
		prof_cfg->pkt_high_thr = db->max_pending_pkts - 5;
		prof_cfg->pkt_low_thr  = db->max_pending_pkts - 10;
	}

	/* set port pool size base on his type */
	prof_cfg->port[RPB_PORT_STR_FLOW0].pool_sz = (db->mem_sz * 3) / 8 -
		RPB_HIGH_TC_POOL_SZ;
	prof_cfg->port[RPB_PORT_STR_FLOW1].pool_sz = (db->mem_sz * 3) / 8 -
		RPB_HIGH_TC_POOL_SZ;
	prof_cfg->port[RPB_PORT_MEM_FLOW0].pool_sz = (db->mem_sz * 1) / 8 -
		RPB_HIGH_TC_POOL_SZ;
	prof_cfg->port[RPB_PORT_MEM_FLOW1].pool_sz = (db->mem_sz * 1) / 8 -
		RPB_HIGH_TC_POOL_SZ;

	RPB_FOR_EACH_PORT(p) {
		port_cfg = &prof_cfg->port[p];
		if (p == RPB_PORT_STR_FLOW0 || p == RPB_PORT_STR_FLOW1) {
			high_delta = (1 * ETH_FRAME_LEN);
			low_delta  = (3 * ETH_FRAME_LEN);
			port_cfg->is_mem = false;
		} else {
			high_delta = (1 * PP_DEFAULT_PREFETCH_SZ);
			low_delta  = (3 * PP_DEFAULT_PREFETCH_SZ);
			port_cfg->is_mem = true;
		}

		port_cfg->high_thr = port_cfg->pool_sz - high_delta;
		port_cfg->low_thr = port_cfg->pool_sz - low_delta;
		port_cfg->num_of_pp_ports = 0;

		/* Enable the flow control by default */
		port_cfg->flow_ctrl = true;

		RPB_FOR_EACH_TC(tc) {
			tc_cfg = &port_cfg->tc[tc];
			/* set dedicated pool only for high priority TC */
			if (tc == RPB_HIGH_TC) {
				tc_cfg->hi_prio  = true;
				tc_cfg->dpool_sz = RPB_HIGH_TC_POOL_SZ;
				tc_cfg->dpool_high_thr = tc_cfg->dpool_sz -
					high_delta;
				tc_cfg->dpool_low_thr  = tc_cfg->dpool_sz -
					low_delta;
				tc_cfg->spool_high_thr = port_cfg->pool_sz -
					(3 * high_delta);
				tc_cfg->spool_low_thr  = port_cfg->pool_sz -
					(3 * low_delta);
			} else {
				tc_cfg->spool_high_thr = port_cfg->pool_sz -
					high_delta;
				tc_cfg->spool_low_thr  = port_cfg->pool_sz -
					low_delta;
			}
			/* set tc flow control according to the port */
			if (port_cfg->flow_ctrl)
				tc_cfg->flow_ctrl = true;
			else
				tc_cfg->flow_ctrl = false;
		}
	}
}

/**
 * @brief Set buffer manager base address
 */
static void __rpb_bmgr_addr_set(u64 buf_mgr_base_addr)
{
	u32 addr_h, addr_l;

	addr_h = (u32)(buf_mgr_base_addr >> 32);
	addr_l = (u32)buf_mgr_base_addr;
	PP_REG_WR32(PP_RPB_DRP_BM_BASE_ADDRH_REG, addr_h);
	PP_REG_WR32(PP_RPB_DRP_BM_BASE_ADDRL_REG, addr_l);
	pr_debug("Set buffer manager address to 0x%x%x\n", addr_h, addr_l);
}

/**
 * @brief Set RPB copy counters feature
 * When setting this bit, every release
 * request will release the packet without
 * checking the copy-counters.
 * @param enable true for enable, false for disable
 */
static void __rpb_copy_counters_set(bool enable)
{
	u32 reg, val = enable ? 0 : 1;

	__rpb_lock();
	reg = PP_REG_RD32(PP_RPB_DBG_FEAT_REG);
	reg = PP_FIELD_MOD(PP_RPB_DBG_FEAT_DIS_CC_MSK, val, reg);
	PP_REG_WR32(PP_RPB_DBG_FEAT_REG, reg);
	__rpb_unlock();
}

/**
 * @brief Set RPB working mode, available modes are: normal and dual40
 * @note dual40 mode is disabled in PPv4
 * @param mode 0 for normal, 1 for dual40
 * @return s32 0 on success, non-zero value otherwise
 */
static s32 __rpb_mode_set(u32 mode)
{
	u32 reg;

	if (!RPB_IS_MODE_VALID(mode)) {
		pr_err("Error: Invalid RPB mode %u\n", mode);
		return -EINVAL;
	}

	__rpb_lock();
	reg = PP_REG_RD32(RPB_CMD_SRC_40MODE_REG);
	reg = RPB_MODE_SET(reg, mode);
	PP_REG_WR32(RPB_CMD_SRC_40MODE_REG, reg);
	__rpb_unlock();

	return 0;
}

/**
 * @brief Initialize RPB memories
 * @return s32 0 on success, non-zero value otherwise
 */
static s32 __rpb_memory_init(void)
{
	#define NUM_OF_MEMS 11
	u32 i, reg;
	char name[NUM_OF_MEMS][32] = {
		"CC_CNT_MEM_CFG",
		"PKTS_MEM_CFG",
		"CLID_MEM_CFG",
		"PPDB_MEM_CFG",
		"REPORT_II_MEM_CFG",
		"REPORT_MEM_CFG",
		"CC_MEM_CFG",
		"PORT0_DATAPIPE_MEM_CFG",
		"PORT1_DATAPIPE_MEM_CFG",
		"PORT2_DATAPIPE_MEM_CFG",
		"PORT3_DATAPIPE_MEM_CFG",
	};
	u64 cfg_addr[NUM_OF_MEMS] = {
		RPB_CC_CNT_MEM_CFG_REG,
		RPB_PKTS_MEM_CFG_REG,
		RPB_CLID_MEM_CFG_REG,
		RPB_PPDB_MEM_CFG_REG,
		RPB_REPORT_II_MEM_CFG_REG,
		RPB_REPORT_MEM_CFG_REG,
		RPB_CC_MEM_CFG_REG,
		RPB_PORT_DATAPIPE_MEM_CFG_REG(0),
		RPB_PORT_DATAPIPE_MEM_CFG_REG(1),
		RPB_PORT_DATAPIPE_MEM_CFG_REG(2),
		RPB_PORT_DATAPIPE_MEM_CFG_REG(3),
	};
	u64 status_addr[NUM_OF_MEMS] = {
		RPB_CC_CNT_MEM_STATUS_REG,
		RPB_PKTS_MEM_STATUS_REG,
		RPB_CLID_MEM_STATUS_REG,
		RPB_PPDB_MEM_STATUS_REG,
		RPB_REPORT_II_MEM_STATUS_REG,
		RPB_REPORT_MEM_STATUS_REG,
		RPB_CC_MEM_STATUS_REG,
		RPB_PORT_DATAPIPE_MEM_STATUS_REG(0),
		RPB_PORT_DATAPIPE_MEM_STATUS_REG(1),
		RPB_PORT_DATAPIPE_MEM_STATUS_REG(2),
		RPB_PORT_DATAPIPE_MEM_STATUS_REG(3),
	};

	/* Verify all memories init is done */
	for (i = 0; i < NUM_OF_MEMS; i++) {
		pr_debug("polling on %s 0x%llx\n",
			 name[i], status_addr[i]);
		if (pp_reg_poll(status_addr[i], RPB_MEMORY_INIT_DONE_MASK,
				RPB_MEMORY_INIT_DONE_VAL)) {
			pr_debug("RPB memory %s HW init failed\n", name[i]);
			return -EPERM;
		}
	}

	/* Disable ECC */
	for (i = 0; i < NUM_OF_MEMS; i++) {
		reg = PP_REG_RD32(cfg_addr[i]);
		pr_debug("Disabling ECC for %s 0x%llx = 0x%x --> 0x%x\n",
			 name[i], cfg_addr[i], reg,
			RPB_MEMORY_ECC_EN_SET(reg, 0));
		reg = RPB_MEMORY_ECC_EN_SET(reg, 0);
		PP_REG_WR32(cfg_addr[i], reg);
	}

	return 0;
}

s32 rpb_start(struct rpb_init_param *init_param)
{
	s32 ret;

	/* this API has a sole purpose of opening the datapath
	 * once the PP driver is ready so we allow it to run
	 * only once
	 */
	if (unlikely(db->curr_rpb_prof != RPB_PROFILE_DROP)) {
		pr_err("RPB already started\n");
		return -EPERM;
	}

	ret = rpb_profile_set(init_param->rpb_profile);
	if (likely(!ret))
		return ret;

	pr_err("Error: Failed to set RPB profile%u, ret = %d\n",
	       RPB_PROFILE_DROP, ret);
	return ret;
}

static s32 __rpb_db_init(struct rpb_init_param *init_param)
{
	db = devm_kzalloc(pp_dev_get(), sizeof(*db), GFP_KERNEL);
	if (unlikely(!db)) {
		pr_err("failed to allocate db memory\n");
		return -ENOMEM;
	}

	db->mem_sz = init_param->mem_sz;
	db->max_pending_pkts = init_param->num_pending_pkts;
	spin_lock_init(&db->lock);

	return 0;
}

s32 rpb_init(struct rpb_init_param *init_param)
{
	u32 ret = 0;

	if (!init_param->valid)
		return -EINVAL;

	rpb_base_addr      = init_param->rpb_base;
	rpb_drop_base_addr = init_param->rpb_drop_base;

	ret = __rpb_db_init(init_param);
	if (unlikely(ret))
		return ret;

	/* debug */
	ret = rpb_dbg_init(init_param->dbgfs);
	if (unlikely(ret)) {
		pr_err("Error: Failed to initialize RPB debug, ret = %d\n",
		       ret);
		return ret;
	}
	/* memory init */
	ret = __rpb_memory_init();
	if (unlikely(ret)) {
		pr_err("Error: Failed to initialize RPB memory, ret = %d\n",
		       ret);
		return ret;
	}
	ret = __rpb_mode_set(RPB_MODE_NORMAL);
	if (unlikely(ret)) {
		pr_err("Error: Failed to set RPB mode to Normal, ret = %d",
		       ret);
		return ret;
	}
	ret = rpb_dma_coal_timeout_set(0);
	if (unlikely(ret)) {
		pr_err("Error: Failed to set dma coalescing timeout, ret = %d",
		       ret);
		return ret;
	}
	ret = rpb_dma_coal_threshold_set(0);
	if (unlikely(ret)) {
		pr_err("Error: Failed to set dma coalescing threshold, ret= %d",
		       ret);
		return ret;
	}
	__rpb_bmgr_addr_set(init_param->bm_phys_base);
	if (pp_silicon_step_get() == PP_SSTEP_A)
		__rpb_copy_counters_set(false);
	__rpb_default_profile_init();
	/* set rpb drop profile, with this profile all traffic should be
	 * dropped till the pp will finish the init sequence and will
	 * start the rpb
	 */
	ret = rpb_profile_set(RPB_PROFILE_DROP);
	if (unlikely(ret))
		pr_err("Error: Failed to set RPB profile%u, ret = %d\n",
		       RPB_PROFILE_DROP, ret);

	return ret;
}

void rpb_exit(void)
{
	if (unlikely(!db))
		return;

	rpb_dbg_clean();
	devm_kfree(pp_dev_get(), db);
	db = NULL;
}
