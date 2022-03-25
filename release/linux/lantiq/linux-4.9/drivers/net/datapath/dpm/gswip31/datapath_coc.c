/*
 * Copyright (C) Intel Corporation
 * Author: Shao Guohua <guohua.shao@intel.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */
#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/module.h>
#include <linux/types.h>	/* size_t */
#include <linux/timer.h>
#include <net/datapath_api.h>
#include <net/datapath_proc_api.h>
#include "../datapath.h"

#define DP_MODULE  LTQ_CPUFREQ_MODULE_DP
#define DP_ID 0	 /* this ID should represent the Datapath interface No. */
static struct timer_list dp_coc_timer;
static u32 polling_period;	/*seconds */
static int rmon_timer_en;
static spinlock_t dp_coc_lock;

/* threshold data for D0:D3 */
struct ltq_cpufreq_threshold rmon_threshold = { 0, 30, 20, 10 }; /*packets */

/* driver is busy and needs highest performance */
static int dp_coc_init_stat;	/*DP COC is Initialized or not */
static int dp_coc_ena;		/*DP COC is enabled or not */
enum ltq_cpufreq_state dp_coc_ps_curr = LTQ_CPUFREQ_PS_UNDEF;/*current state*/
/*new statue wanted to switch to */
enum ltq_cpufreq_state dp_coc_ps_new = LTQ_CPUFREQ_PS_UNDEF;

static GSW_RMON_Port_cnt_t rmon_last[PMAC_MAX_NUM];
static u64 last_rmon_rx;

/*meter */
#define PCE_OVERHD 20
static u32 meter_id;
/*3 ~ 4 packet size */
static u32 meter_ncbs = 0x8000 + (1514 + PCE_OVERHD) * 3 + 200;
/*1 ~ 2 packet size */
static u32 meter_nebs = 0x8000 + (1514 + PCE_OVERHD) * 1 + 200;
/*k bits */
static u32 meter_nrate[4] = { 0/*D0 */, 700/*D1*/, 600/*D2*/, 500/*D3*/};

static int dp_coc_cpufreq_notifier(struct notifier_block *nb,
				   unsigned long val, void *data);
static int dp_coc_stateget(enum ltq_cpufreq_state *state);
static int dp_coc_fss_ena(int ena);
static int apply_meter_rate(u32 rate, enum ltq_cpufreq_state new_state);
static int enable_meter_interrupt(void);
static int clear_meter_interrupt(void);

int dp_set_meter_rate(enum ltq_cpufreq_state stat, unsigned int rate)
{/*set the rate for upscaling to D0 from specified stat */
	if (stat == LTQ_CPUFREQ_PS_D1)
		meter_nrate[1] = rate;
	else if (stat == LTQ_CPUFREQ_PS_D2)
		meter_nrate[2] = rate;
	else if (stat == LTQ_CPUFREQ_PS_D3)
		meter_nrate[3] = rate;
	else
		return -1;
	if (dp_coc_ps_curr == stat)
		apply_meter_rate(-1, stat);
	return 0;
}
EXPORT_SYMBOL(dp_set_meter_rate);

static struct notifier_block dp_coc_cpufreq_notifier_block = {
	.notifier_call = dp_coc_cpufreq_notifier
};

static inline void coc_lock(void)
{
}

static inline void coc_unlock(void)
{
	spin_unlock_bh(&dp_coc_lock);
}

struct ltq_cpufreq_module_info dp_coc_feature_fss = {
	.name = "Datapath FSS",
	.pmcuModule = DP_MODULE,
	.pmcuModuleNr = DP_ID,
	.powerFeatureStat = 1,
	.ltq_cpufreq_state_get = dp_coc_stateget,
	.ltq_cpufreq_pwr_feature_switch = dp_coc_fss_ena,
};

#if IS_ENABLED(CONFIG_INTEL_DATAPATH_DBG) && CONFIG_INTEL_DATAPATH_DBG
static char *get_sub_module_str(uint32_t flag)
{
	if (flag == DP_COC_REQ_DP)
		return "DP";
	else if (flag == DP_COC_REQ_ETHERNET)
		return "Ethernet";
	else if (flag == DP_COC_REQ_VRX318)
		return "VRX318";
	else
		return "Unknown";
}
#endif

static char *get_coc_stat_string(enum ltq_cpufreq_state stat)
{
	if (stat == LTQ_CPUFREQ_PS_D0)
		return "D0";
	else if (stat == LTQ_CPUFREQ_PS_D1)
		return "D1";
	else if (stat == LTQ_CPUFREQ_PS_D2)
		return "D2";
	else if (stat == LTQ_CPUFREQ_PS_D3)
		return "D3";
	else if (stat == LTQ_CPUFREQ_PS_D0D3)
		return "D0D3-NotCare";
	else if (stat == LTQ_CPUFREQ_PS_BOOST)
		return "Boost";
	else
		return "Undef";
}

static void dp_rmon_polling(unsigned long data)
{
}

static int dp_coc_stateget(enum ltq_cpufreq_state *state)
{
}

static int dp_coc_fss_ena(int ena)
{
}

void update_rmon_last(void)
{
}

int update_coc_rmon_timer(enum ltq_cpufreq_state new_state, uint32_t flag)
{
	return 0;
}

static int update_coc_cfg(enum ltq_cpufreq_state new_state,
			  enum ltq_cpufreq_state old_state, uint32_t flag)
{
	return 0;
}

static int dp_coc_prechange(enum ltq_cpufreq_module module,
			    enum ltq_cpufreq_state new_state,
			    enum ltq_cpufreq_state old_state, u8 flags)
{
	return 0;
}

static int dp_coc_postchange(enum ltq_cpufreq_module module,
			     enum ltq_cpufreq_state new_state,
			     enum ltq_cpufreq_state old_state, u8 flags)
{
	return 0;
}

/* keep track of frequency transitions */
static int dp_coc_cpufreq_notifier(struct notifier_block *nb,
				   unsigned long val, void *data)
{
	return 0;
}

void proc_coc_read(struct seq_file *s)
{
	if (!capable(CAP_NET_ADMIN))
		return;
}

int dp_set_rmon_threshold(struct ltq_cpufreq_threshold *threshold,
			  uint32_t flags)
{
	return 0;
}
EXPORT_SYMBOL(dp_set_rmon_threshold);

ssize_t proc_coc_write(struct file *file, const char *buf, size_t count,
		       loff_t *ppos)
{
	if (!capable(CAP_NET_ADMIN))
		return -EPERM;
	return count;
}

int clear_meter_interrupt(void)
{
	return 0;
}

int enable_meter_interrupt(void)
{
	return 0;
}

/* rate      0: disable meter
 * -1: enable meter
 * others: really change rate.
 */
int apply_meter_rate(u32 rate, enum ltq_cpufreq_state new_state)
{
	return 0;
}

int meter_set_default(void)
{
	return 0;
}

/* For Datapth's sub-module to request power state change, esp for
 *  Ethernet/VRX318 driver to call it if there is state change needed.
 *   The flag can be:
 *     DP_COC_REQ_DP
 *     DP_COC_REQ_ETHERNET
 *     DP_COC_REQ_VRX318
 */
int dp_coc_new_stat_req(enum ltq_cpufreq_state new_state, uint32_t flag)
{
	return 0;
}
EXPORT_SYMBOL(dp_coc_new_stat_req);

int dp_coc_cpufreq_init(void)
{
	return 0;
}

int dp_coc_cpufreq_exit(void)
{
	return 0;
}
