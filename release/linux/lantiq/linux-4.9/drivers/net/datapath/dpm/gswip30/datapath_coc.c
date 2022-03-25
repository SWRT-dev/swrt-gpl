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
#include <linux/cpufreq.h>
#include <linux/workqueue.h>
#include <net/datapath_api.h>
#include <net/datapath_proc_api.h>
#include "../datapath.h"
#include "datapath_misc.h"

#define DP_MODULE  LTQ_CPUFREQ_MODULE_DP
#define DP_ID 0	 /* this ID should represent the Datapath interface No. */
#define LTQ_CPUFREQ_PS_D0 0
#define LTQ_CPUFREQ_PS_D1 1
#define LTQ_CPUFREQ_PS_D2 2
#define LTQ_CPUFREQ_PS_D3 3
static struct timer_list dp_coc_timer;
static u32 polling_period;	/*seconds */
static int rmon_timer_en;
static spinlock_t dp_coc_lock;

/* threshold data for D0:D3 */
unsigned int rmon_threshold[4] = { 0, 30, 20, 10 }; /*packets */
/* driver is busy and needs highest performance */
static int dp_coc_init_stat;	/*DP COC is Initialized or not */
static int dp_coc_ena;		/*DP COC is enabled or not */
int dp_coc_ps_curr = -1;/*current state*/
/*new statue wanted to switch to */
int dp_coc_ps_new = -1;
struct work_struct coc_work_q;
static GSW_RMON_Port_cnt_t rmon_last[PMAC_MAX_NUM];
static u64 last_rmon_rx;
int dp_coc_ps_max;/*system maximum*/
int dp_coc_ps_min;/*system minimum*/

int intr = -1;
int inst;
/*meter */
#define PCE_OVERHD 20
static u32 meter_id;
/*3 ~ 4 packet size */
static u32 meter_ncbs = 0x8000 + (1514 + PCE_OVERHD) * 3 + 200;
/*1 ~ 2 packet size */
static u32 meter_nebs = 0x8000 + (1514 + PCE_OVERHD) * 1 + 200;
/*k bits */
static u32 meter_nrate[4] = { 0/*D0 */, 700/*D1*/, 600/*D2*/, 500/*D3*/};
static int apply_meter_rate(u32 rate, unsigned int new_state);

int dp_set_meter_rate(int stat, unsigned int rate)
{/*set the rate for upscaling to D0 from specified stat */
	if (stat == LTQ_CPUFREQ_PS_D1)
		meter_nrate[1] = rate;
	else if (stat == LTQ_CPUFREQ_PS_D2)
		meter_nrate[2] = rate;
	else if (stat == LTQ_CPUFREQ_PS_D3)
		meter_nrate[3] = rate;
	else
		return -1;
	if (dp_coc_ps_curr == dp_coc_ps_min)
		apply_meter_rate(-1, stat);
	return 0;
}

int dp_set_rmon_threshold(uint32_t *threshold)
{
	int i;

	if (!threshold)
		return -1;
	for (i = 0 ; i < ARRAY_SIZE(rmon_threshold); i++)
		rmon_threshold[i] = threshold[i];
	return	0;
}
EXPORT_SYMBOL(dp_set_rmon_threshold);

static inline void coc_lock(void)
{
	if (unlikely(in_irq())) {
		pr_err("Not allowed to call COC API in_irq mode\n");
		return;
	}
	spin_lock_bh(&dp_coc_lock);
}

static inline void coc_unlock(void)
{
	spin_unlock_bh(&dp_coc_lock);
}

static char *get_coc_stat_string(int stat)
{
	if (stat == LTQ_CPUFREQ_PS_D0)
		return "D0";
	else if (stat == LTQ_CPUFREQ_PS_D1)
		return "D1";
	else if (stat == LTQ_CPUFREQ_PS_D2)
		return "D2";
	else if (stat == LTQ_CPUFREQ_PS_D3)
		return "D3";
	else
		return "Undef";
}

void proc_coc_read_30(struct seq_file *s)
{
	GSW_register_t reg;
	GSW_QoS_meterCfg_t meter_cfg;
	struct core_ops *gsw_handle;

	if (!capable(CAP_NET_ADMIN))
		return;
	gsw_handle = dp_port_prop[inst].ops[GSWIP_R];
	seq_puts(s, "  Basic DP COC Info:\n");
	seq_printf(s, "    dp_coc_ena=%d @ 0x%p (DP %s)\n", dp_coc_ena,
		   &dp_coc_ena, dp_coc_ena ? "COC Enabled" : "COC Disabled");
	seq_printf(s, "    Rmon timer interval: %u sec (Timer %s)\n",
		   (unsigned int)polling_period,
		   rmon_timer_en ? "enabled" : "disabled");
	seq_printf(s, "    RMON D1 Threshold: %d\n", rmon_threshold[1]);
	seq_printf(s, "    RMON D2 Threshold: %d\n", rmon_threshold[2]);
	seq_printf(s, "    RMON D3 Threshold: %d\n", rmon_threshold[3]);
	seq_printf(s, "    dp_coc_init_stat=%d @ %p (%s)\n", dp_coc_init_stat,
		   &dp_coc_init_stat,
		   dp_coc_init_stat ? "initialized ok" : "Not initialized");
	seq_printf(s, "    dp_coc_ps_curr=%d (%s) @ 0x%p\n", dp_coc_ps_curr,
		   get_coc_stat_string(dp_coc_ps_curr), &dp_coc_ps_curr);
	seq_printf(s, "    dp_coc_ps_new=%d\n", dp_coc_ps_new);
	seq_printf(s, "    last_rmon_rx=%llu pkts@ 0x%p (%s)\n", last_rmon_rx,
		   &last_rmon_rx, rmon_timer_en ? "Valid" : "Not valid");

	seq_puts(s, "    Metering Info:\n");
	/*PCE_OVERHD */
	memset(&reg, 0, sizeof(reg));
	reg.nRegAddr = 0x46C;	/*PCE_OVERHD */
	gsw_handle->gsw_common_ops.RegisterGet(gsw_handle, &reg);
	seq_printf(s, "    PCE_OVERHD=%d\n", reg.nData);

	meter_cfg.nMeterId = meter_id;
	gsw_handle->gsw_qos_ops.QoS_MeterCfgGet(gsw_handle, &meter_cfg);
	seq_printf(s, "    meter id=%u (%s)\n", meter_id,
		   meter_cfg.bEnable ? "Enabled" : "Disabled");
	seq_printf(s, "    meter nCbs=%u\n", meter_cfg.nCbs);
	seq_printf(s, "    meter nEbs=%u\n", meter_cfg.nEbs);
	seq_printf(s, "    meter nRate=%u\n", meter_cfg.nRate);
	seq_printf(s, "    meter nPiRate=%u\n", meter_cfg.nPiRate);
	seq_printf(s, "    meter eMtrType=%u\n", (int)meter_cfg.eMtrType);
	seq_printf(s, "    D1 nRate=%u\n", meter_nrate[1]);
	seq_printf(s, "    D2 nRate=%u\n", meter_nrate[2]);
	seq_printf(s, "    D3 nRate=%u\n", meter_nrate[3]);

	memset(&reg, 0, sizeof(reg));
	reg.nRegAddr = 0x489 + meter_id * 10;
	gsw_handle->gsw_common_ops.RegisterGet(gsw_handle, &reg);
	seq_printf(s, "    PCE_PISR(0x%x)=%u(0x%x)-interrupt %s\n",
		   reg.nRegAddr, reg.nData, reg.nData,
		   (reg.nData & 0x100) ? "on" : "off");
}

ssize_t proc_coc_write_30(struct file *file, const char *buf, size_t count,
			  loff_t *ppos)
{
	int len, num;
	char str[64];
	char *param_list[3];
#define MIN_POLL_TIME 1

	if (!capable(CAP_NET_ADMIN))
		return -EPERM;
	len = (sizeof(str) > count) ? count : sizeof(str) - 1;
	len -= copy_from_user(str, buf, len);
	str[len] = 0;

	num = dp_split_buffer(str, param_list, ARRAY_SIZE(param_list));
	if (dp_strncmpi(param_list[0], "timer", strlen("timer") + 1) == 0) {
		polling_period = dp_atoi(param_list[1]);

		if (polling_period < MIN_POLL_TIME)
			polling_period = MIN_POLL_TIME;

		coc_lock();

		if (rmon_timer_en) {
			mod_timer(&dp_coc_timer,
				  jiffies +
				  msecs_to_jiffies(polling_period * 1000));
		}

		coc_unlock();
	} else if (dp_strncmpi(param_list[0], "threshold0",
					strlen("threshold0") + 1) == 0) {
		coc_lock();
		rmon_threshold[0] = dp_atoi(param_list[1]);

		if (!rmon_threshold[0])
			rmon_threshold[0] = 1;

		coc_unlock();
	} else if (dp_strncmpi(param_list[0], "threshold1",
					strlen("threshold1") + 1) == 0) {
		coc_lock();
		rmon_threshold[1] = dp_atoi(param_list[1]);

		if (!rmon_threshold[1])
			rmon_threshold[1] = 1;

		coc_unlock();
	} else if (dp_strncmpi(param_list[0], "threshold2",
					strlen("threshold2") + 1) == 0) {
		coc_lock();
		rmon_threshold[2] = dp_atoi(param_list[1]);

		if (!rmon_threshold[2])
			rmon_threshold[2] = 1;

		coc_unlock();
	} else if (dp_strncmpi(param_list[0], "threshold3",
					strlen("threshold3") + 1) == 0) {
		coc_lock();
		rmon_threshold[3] = dp_atoi(param_list[1]);

		if (!rmon_threshold[3])
			rmon_threshold[3] = 1;

		coc_unlock();
	} else if (dp_strncmpi(param_list[0], "D0", strlen("D0") + 1) == 0) {
		dp_coc_ps_new = dp_coc_ps_max;
		schedule_work(&coc_work_q);
	} else if (dp_strncmpi(param_list[0], "D3", strlen("D3") + 1) == 0) {
		dp_coc_ps_new = dp_coc_ps_min;
		schedule_work(&coc_work_q);
	} else if (dp_strncmpi(param_list[0], "rate", 4) == 0) {
		/*meter rate */
		u32 rate = dp_atoi(param_list[1]);

		if (!rate) {
			pr_info("rate should not be zero\n");
			return count;
		}
		if (dp_strncmpi(param_list[0], "rate1",
				strlen("rate1") + 1) == 0) {
			dp_set_meter_rate(LTQ_CPUFREQ_PS_D1, rate);

		} else if (dp_strncmpi(param_list[0], "rate2",
					strlen("rate2") + 1) == 0) {
			dp_set_meter_rate(LTQ_CPUFREQ_PS_D2, rate);
		} else if (dp_strncmpi(param_list[0], "rate3",
				strlen("rate3") + 1) == 0) {
			dp_set_meter_rate(LTQ_CPUFREQ_PS_D3, rate);
		} else {
			pr_info("Wrong COC state, it should be D1/D2/D3 only\n");
		}
	} else {
		goto help;
	}
	return count;
 help:
	pr_info("Datapath COC Proc Usage:\n");
	pr_info("echo timer polling_interval_in_seconds > /sys/kernel/debug/dp/coc\n");
	pr_info("echo <thresholdx> its_threshold_value > /sys/kernel/debug/dp/coc\n");
	pr_info("       Note:Valid x of ranage: 1 2 3\n");
	pr_info("            For downscale to D<x> if rmon<threshold<x>'s cfg\n");
	pr_info("            threshold1's >= threshold'2 > threshold'3\n");
	pr_info("echo <ratex> <meter_rate_in_knps> /sys/kernel/debug/dp/coc\n");
	pr_info("       Note:Valid x of range: 1 2 3\n");
	pr_info("            For upscale to D0 from D<x> if rmon>=rate<x>'s cfg\n");
	pr_info("            Rate1's >= Rate2's > D3's threshold\n");
	return count;
}

int clear_meter_interrupt(void)
{
	GSW_register_t reg;
	struct core_ops *gsw_handle;

	gsw_handle = dp_port_prop[inst].ops[GSWIP_R];
	memset(&reg, 0, sizeof(reg));
	reg.nRegAddr = 0x489 + meter_id * 10;
	reg.nData = -1;
	gsw_handle->gsw_common_ops.RegisterSet(gsw_handle, &reg);
	return 0;
}

int enable_meter_interrupt(void)
{
	GSW_register_t reg;
	struct core_ops *gsw_handle;

	gsw_handle = dp_port_prop[inst].ops[GSWIP_R];
	/*#Enable PCE Interrupt
	 *  switch_cli GSW_REGISTER_SET nRegAddr=0x14 nData=0x2 dev=1
	 */
	memset(&reg, 0, sizeof(reg));
	reg.nRegAddr = 0x14;	/*ETHSW_IER */
	gsw_handle->gsw_common_ops.RegisterGet(gsw_handle, &reg);
	reg.nRegAddr |= 1 << 1; /*Enable PCEIE */
	gsw_handle->gsw_common_ops.RegisterSet(gsw_handle, &reg);

	/*#Enable PCE Port Interrupt
	 *  switch_cli GSW_REGISTER_SET nRegAddr=0x465 nData=0x1 dev=1
	 */
	memset(&reg, 0, sizeof(reg));
	reg.nRegAddr = 0x465;	/*PCE_IER_0 */
	gsw_handle->gsw_common_ops.RegisterGet(gsw_handle, &reg);
	reg.nRegAddr |= 1 << 0; /*Enable PCE Port 0 */
	gsw_handle->gsw_common_ops.RegisterSet(gsw_handle, &reg);

	memset(&reg, 0, sizeof(reg));
	reg.nRegAddr = 0x488;	/*PCE_PIER */
	gsw_handle->gsw_common_ops.RegisterGet(gsw_handle, &reg);
	/*Enable Metering Based Backpressure Status Change Interrupt Enable */
	reg.nRegAddr |= 1 << 8;
	gsw_handle->gsw_common_ops.RegisterSet(gsw_handle, &reg);

	return 0;
}

/* rate      0: disable meter
 * -1: enable meter
 * others: really change rate.
 */
int apply_meter_rate(u32 rate, unsigned int new_state)
{
	GSW_QoS_meterCfg_t meter_cfg;
	struct core_ops *gsw_handle;

	DP_DEBUG(DP_DBG_FLAG_COC, "rate=%d new state=%d\n", rate, new_state);
	gsw_handle = dp_port_prop[inst].ops[GSWIP_R];
	memset(&meter_cfg, 0, sizeof(meter_cfg));
	meter_cfg.nMeterId = meter_id;
	gsw_handle->gsw_qos_ops.QoS_MeterCfgGet(gsw_handle, &meter_cfg);
	if (rate == 0) {		/*only need to disable the meter */
		meter_cfg.bEnable = 0;
	} else if (rate == -1) {
		meter_cfg.bEnable = 1;
		/*set PAE metering */
		if (new_state == LTQ_CPUFREQ_PS_D1)
			meter_cfg.nRate = meter_nrate[1];
		else if (new_state == LTQ_CPUFREQ_PS_D2)
			meter_cfg.nRate = meter_nrate[2];
		else if (new_state == LTQ_CPUFREQ_PS_D3)
			meter_cfg.nRate = meter_nrate[3];
		else
			meter_cfg.nRate = new_state;
	} else {
		return -1;
	}

	gsw_handle->gsw_qos_ops.QoS_MeterCfgSet(gsw_handle, &meter_cfg);

	return 0;
}

int meter_set_default(void)
{
	int i;
	GSW_QoS_WRED_PortCfg_t wred_p;
	GSW_QoS_WRED_QueueCfg_t wred_q;
	GSW_QoS_WRED_Cfg_t wred_cfg;
	GSW_QoS_meterCfg_t meter_cfg;
	GSW_QoS_meterPort_t meter_port;
	GSW_register_t reg;
	struct core_ops *gsw_handle;
	struct qos_ops *gsw_qos;

	gsw_handle = dp_port_prop[inst].ops[GSWIP_R];
	gsw_qos = &gsw_handle->gsw_qos_ops;
	/*#currently directly change global setting, later should use
	 * GSW_QOS_WRED_PORT_CFG_SET instead
	 * switch_cli dev=1 GSW_REGISTER_SET nRegAddr=0x4a nData=0x518
	 */
	memset(&wred_p, 0, sizeof(wred_p));

	for (i = 0; i < PMAC_MAX_NUM; i++) {/*cp green setting to yellow/red*/
		wred_p.nPortId = i;
		gsw_qos->QoS_WredPortCfgGet(gsw_handle, &wred_p);
		wred_p.nYellow_Min = wred_p.nGreen_Min;
		wred_p.nYellow_Max = wred_p.nGreen_Max;
		wred_p.nRed_Min = wred_p.nGreen_Min;
		wred_p.nRed_Max = wred_p.nGreen_Max;
		gsw_qos->QoS_WredPortCfgSet(gsw_handle, &wred_p);
	}

	/*#Enable Meter 0, configure the rate
	 * switch_cli GSW_QOS_METER_CFG_SET bEnable=1
	 *	nMeterId=0 nCbs=0xA000 nEbs=0x9000 nRate=500 dev=1
	 */
	memset(&meter_cfg, 0, sizeof(meter_cfg));
	/*tmp.meter_cfg.bEnable = 1; */
	meter_cfg.nMeterId = meter_id;
	meter_cfg.nCbs = meter_ncbs;
	meter_cfg.nEbs = meter_nebs;
	meter_cfg.nRate = meter_nrate[3];
	meter_cfg.nPiRate = 0xFFFFFF; /* try to set maximum */
	meter_cfg.eMtrType = GSW_QOS_Meter_trTCM;
	gsw_qos->QoS_MeterCfgSet(gsw_handle, &meter_cfg);

	/*#Assign Port0 ingress to Meter 0
	 * switch_cli GSW_QOS_METER_PORT_ASSIGN nMeterId=0 eDir=1
	 *	nPortIngressId=0 dev=1
	 */
	memset(&meter_port, 0, sizeof(meter_port));

	for (i = 0; i < PMAC_MAX_NUM; i++) {
		meter_port.nMeterId = meter_id;
		meter_port.eDir = 1;
		meter_port.nPortIngressId = i;
		gsw_qos->QoS_MeterPortAssign(gsw_handle, &meter_port);
	}

	/*#Enable Port 0 Meter Based Flow control (Bit 2 MFCEN)
	 *  switch_cli GSW_REGISTER_SET nRegAddr=0xBC0 nData=0x5107 dev=1
	 */
	memset(&reg, 0, sizeof(reg));
	reg.nRegAddr = 0xBC0;
	gsw_handle->gsw_common_ops.RegisterGet(gsw_handle, &reg);
	reg.nData |= 1 << 2;
	gsw_handle->gsw_common_ops.RegisterSet(gsw_handle, &reg);

	/*#Configure Red and Yellow watermark for each queue
	 *  (Yellow and Red shall not be 0 in CoC case in order to avoid
	 *  packet drop)
	 *  i=0
	 *  while [$i -lt 32]
	 *  do
	 *  switch_cli GSW_QOS_WRED_QUEUE_CFG_SET nQueueId=$i nRed_Min=0x40
	 *  nRed_Max=0x40 nYellow_Min=0x40 nYellow_Max=0x40 nGreen_Min=0x40
	 *  nGreen_Max=0x40 dev=1
	 *  i=$(($i+1))
	 *  done
	 */
	memset(&wred_q, 0, sizeof(wred_q));
	for (i = 0; i < 32; i++) {	/*copy green setting to yellow/red */
		wred_q.nQueueId = i;
		gsw_qos->QoS_WredQueueCfgGet(gsw_handle, &wred_q);

		wred_q.nYellow_Min = wred_q.nGreen_Min;
		wred_q.nYellow_Max = wred_q.nGreen_Max;
		wred_q.nRed_Min = wred_q.nGreen_Min;
		wred_q.nRed_Max = wred_q.nGreen_Max;
		gsw_qos->QoS_WredQueueCfgSet(gsw_handle, &wred_q);
	}

	/*Configure Red and Yellow watermark for each queue (Yellow and Red
	 * shall not be 0 in CoC case in order to avoid packet drop)
	 * switch_cli GSW_QOS_WRED_CFG_SET nRed_Min=255 nRed_Max=255
	 * nYellow_Min=255 nYellow_Max=255  nGreen_Min=255 nGreen_Max=255 dev=1
	 */
	memset(&wred_cfg, 0, sizeof(wred_cfg));
	gsw_qos->QoS_WredCfgGet(gsw_handle, &wred_cfg);
	wred_cfg.nYellow_Min = wred_cfg.nGreen_Min;
	wred_cfg.nYellow_Max = wred_cfg.nGreen_Max;
	wred_cfg.nRed_Min = wred_cfg.nGreen_Min;
	wred_cfg.nRed_Max = wred_cfg.nGreen_Max;
	gsw_qos->QoS_WredCfgSet(gsw_handle, &wred_cfg);

	/*#Configure OVERHEAD */
	memset(&reg, 0, sizeof(reg));
	reg.nRegAddr = 0x46C;	/*PCE_OVERHD */
	reg.nData = PCE_OVERHD;
	gsw_handle->gsw_common_ops.RegisterSet(gsw_handle, &reg);

	return 0;
}

static void dp_rmon_polling(unsigned long data)
{
	GSW_RMON_Port_cnt_t curr;
	int i;
	u64 rx = 0;
	struct core_ops *gsw_handle;

	/*read gswip-r rmon counter */
	gsw_handle = dp_port_prop[inst].ops[GSWIP_R];
	for (i = 0; i < PMAC_MAX_NUM; i++) {
		memset(&curr, 0, sizeof(curr));
		curr.nPortId = i;
		gsw_handle->gsw_rmon_ops.RMON_Port_Get(gsw_handle, &curr);

		coc_lock();
		/*wrapround handling */
		if (curr.nRxGoodPkts >= rmon_last[i].nRxGoodPkts)
			rx += curr.nRxGoodPkts - rmon_last[i].nRxGoodPkts;
		else
			rx += (u64)0xFFFFFFFF + (u64)curr.nRxGoodPkts -
				rmon_last[i].nRxGoodPkts;

		if (curr.nRxDroppedPkts >= rmon_last[i].nRxDroppedPkts)
			rx += curr.nRxDroppedPkts - rmon_last[i].nRxDroppedPkts;
		else
			rx += (u64)0xFFFFFFFF + (u64)curr.nRxDroppedPkts -
				rmon_last[i].nRxDroppedPkts;

		memcpy(&rmon_last[i], &curr, sizeof(curr));
		coc_unlock();
	}
	last_rmon_rx = rx;
	if (dp_coc_ps_curr != -1) {
		if (rx < rmon_threshold[3]) {
			coc_lock();
			dp_coc_ps_new = dp_coc_ps_min;
			rmon_timer_en = 0;
			coc_unlock();
			schedule_work(&coc_work_q);
			DP_DEBUG(DP_DBG_FLAG_COC,
				 "Request to D3:rx (%u) < th_d3 %d dp_new=%d\n",
				 (unsigned int)rx, rmon_threshold[3],
				 dp_coc_ps_new);
		} else if (rx < rmon_threshold[2]) {
			coc_lock();
			dp_coc_ps_new = dp_coc_ps_min;
			coc_unlock();
			schedule_work(&coc_work_q);
			DP_DEBUG(DP_DBG_FLAG_COC,
				 "req to D2: rx (%u) < th_d2 %d dp_new=%d\n",
				 (unsigned int)rx, rmon_threshold[2],
				 dp_coc_ps_new);
		} else if (rx < rmon_threshold[1]) {
			coc_lock();
			dp_coc_ps_new = dp_coc_ps_min;
			coc_unlock();
			schedule_work(&coc_work_q);
			DP_DEBUG(DP_DBG_FLAG_COC,
				 "req to D1 since rx (%u) < th_d1 %d dp_new=%d\n",
				 (unsigned int)rx, rmon_threshold[1],
				 dp_coc_ps_new);
		} else {
			DP_DEBUG(DP_DBG_FLAG_COC,
				 "Stat no change:rx(%u)>=thresholds %d_%d_%d\n",
				 (unsigned int)rx, rmon_threshold[3],
				 rmon_threshold[2], rmon_threshold[1]);
		}
	} else {
		DP_DEBUG(DP_DBG_FLAG_COC,
			 "DP COC not get its initial power state yet\n");
	}
	coc_lock();
	if (rmon_timer_en)
		mod_timer(&dp_coc_timer,
			  jiffies + msecs_to_jiffies(polling_period * 1000));
	else
		last_rmon_rx = 0;

	coc_unlock();
}

void update_rmon_last(void)
{
	int i;
	struct core_ops *gsw_handle;
	struct rmon_ops *gsw_rmon;

	gsw_handle = dp_port_prop[inst].ops[GSWIP_R];
	gsw_rmon = &gsw_handle->gsw_rmon_ops;
	memset(rmon_last, 0, sizeof(rmon_last));
	for (i = 0; i < PMAC_MAX_NUM; i++) {
		rmon_last[i].nPortId = i;
		gsw_rmon->RMON_Port_Get(gsw_handle, &rmon_last[i]);
	}
}

int update_coc_rmon_timer(uint32_t new_state)
{
	DP_DEBUG(DP_DBG_FLAG_COC, "%s new_state:%u\n", __func__, new_state);

	if (new_state == dp_coc_ps_max) {
		/*enable rmon timer */
		if (!rmon_timer_en)
			update_rmon_last();
		mod_timer(&dp_coc_timer,
			  jiffies + msecs_to_jiffies(polling_period * 1000));
		rmon_timer_en = 1;

		/*disable meter */
		apply_meter_rate(0, 0);
	} else if ((new_state > dp_coc_ps_min) &&
				(new_state < dp_coc_ps_max)) {
		/*enable meter, but first disable to fix red color issue
		 * if last already triggered
		 */
		apply_meter_rate(0, 0);
		apply_meter_rate(-1, LTQ_CPUFREQ_PS_D3);	/*enable again*/

	} else if (new_state == dp_coc_ps_min) {
		/*disable rmon timer */
		del_timer(&dp_coc_timer);
		rmon_timer_en = 0;
		last_rmon_rx = 0;

		/*enable meter */
		/*enable meter, but first disable to fix red color issue
		 * if last already triggered
		 */
		apply_meter_rate(0, 0);
		apply_meter_rate(-1, LTQ_CPUFREQ_PS_D3);	/*enable again*/
	}

	return 0;
}

static int dp_coc_policy_notify(struct cpufreq_policy *policy)
{
	/* Datapath COC supports only in conservative governor */
	if (dp_strncmpi(policy->governor->name, "conservative",
			strlen("conservative") + 1) == 0) {
		coc_lock();
		if (dp_coc_ps_curr == -1) {
			dp_coc_ps_curr = policy->cur;
			dp_coc_ps_max = policy->user_policy.max;
			dp_coc_ps_min = policy->user_policy.min;
		}
		DP_DEBUG(DP_DBG_FLAG_COC, "DP gov:%s(min,max,cur):%u, %u, %u\n",
			 policy->governor->name,
			 dp_coc_ps_min, dp_coc_ps_max, dp_coc_ps_curr);
		if (dp_coc_ps_new == -1) {
			if (dp_coc_ps_curr == dp_coc_ps_max) {
				coc_unlock();
				goto limit_high;
			} else {
				dp_coc_ps_curr = dp_coc_ps_min;
				dp_coc_ps_new = -1;
				update_coc_rmon_timer(dp_coc_ps_curr);
			}
		} else if (dp_coc_ps_new == dp_coc_ps_max) {
			coc_unlock();
			goto limit_high;
		} else {
			dp_coc_ps_curr = dp_coc_ps_min;
			dp_coc_ps_new = -1;
			update_coc_rmon_timer(dp_coc_ps_curr);
		}
		coc_unlock();
		return NOTIFY_OK;
limit_high:
		coc_lock();
		dp_coc_ps_curr = dp_coc_ps_max;
		dp_coc_ps_new = -1;
		update_coc_rmon_timer(dp_coc_ps_curr);
		DP_DEBUG(DP_DBG_FLAG_COC,
			 "Up scale-limit to freq=%d dp_curr=%d dp_new=%d\n",
			 dp_coc_ps_max, dp_coc_ps_curr, dp_coc_ps_new);
		pr_debug("Datapath requesting max cpu frequency\n");
		/*No down scaling allowed, limit the frequency to max */
		cpufreq_verify_within_limits(policy, dp_coc_ps_curr,
					     policy->max);
		coc_unlock();
		return NOTIFY_OK;
	}
	/* to handle other CPU governor transition after conservative governor*/
	coc_lock();
	/* disable timer */
	rmon_timer_en = 0;
	/*disable meter */
	apply_meter_rate(0, 0);
	last_rmon_rx = 0;
	dp_coc_ps_curr = -1;
	dp_coc_ps_new = -1;
	coc_unlock();
	return NOTIFY_OK;
}

static int dp_coc_prechange(struct cpufreq_freqs *freq)
{
	/*check whether can be switched or not
	 * and accept the request
	 */
	if (!dp_coc_init_stat || !dp_coc_ena)
		return NOTIFY_OK;

	DP_DEBUG(DP_DBG_FLAG_COC,
		 "dp_coc_prechange:to switch from %d to %d\n",
		 freq->old, freq->new);

	return NOTIFY_OK;
}

static int dp_coc_postchange(struct cpufreq_freqs *freq)
{
	if (!dp_coc_init_stat || !dp_coc_ena)
		return NOTIFY_OK;

	DP_DEBUG(DP_DBG_FLAG_COC,
		 "dp_coc_postchange:to switch from %d to %d\n",
		 freq->old, freq->new);
	return NOTIFY_OK;
}

int dp_handle_cpufreq_event_30(int event_id, void *cfg)
{
	int res = DP_FAILURE;

	if (!cfg)
		return res;

	switch (event_id) {
	case PRE_CHANGE:
		res = dp_coc_prechange((struct cpufreq_freqs *)cfg);
		break;
	case POST_CHANGE:
		res = dp_coc_postchange((struct cpufreq_freqs *)cfg);
		break;
	case POLICY_NOTIFY:
		res = dp_coc_policy_notify((struct cpufreq_policy *)cfg);
		break;
	default:
		pr_err("no support for %d\n", event_id);
		break;
	}
	return res;
}

/* Create Work Q to call CPUFREQ policy notifier
 *in order to run out of interrupt context
 */
static int coc_work_task(struct work_struct *work)
{
	cpufreq_update_policy(0);
	return 1;
}

void dp_meter_interrupt_cb(void *param)
{
	DP_DEBUG(DP_DBG_FLAG_COC,
		 "triggered meter intr with dp curr freq=%d\n",
		 dp_coc_ps_curr);
	/* schedule work Q only once when DP's current status is low */
	if (dp_coc_ps_curr != dp_coc_ps_max) {
		dp_coc_ps_new = dp_coc_ps_max;
		schedule_work(&coc_work_q); /* schedule work queue */
	}
}

int dp_coc_cpufreq_init(void)
{
	GSW_Irq_Op_t irq;
	struct core_ops *gsw_handle;

	dp_coc_init_stat = 0;
	dp_coc_ena = 0;

	pr_info("enter dp_coc_cpufreq_init\n");
	spin_lock_init(&dp_coc_lock);
	gsw_handle = dp_port_prop[inst].ops[GSWIP_R];

	if (!polling_period)
		polling_period = 2;
	meter_set_default();
	irq.blk = PCE;
	irq.event = PCE_METER_EVENT;
	irq.portid = 0; // logical port id
	irq.call_back = dp_meter_interrupt_cb;// Callback API
	irq.param = NULL; // Callback  API parameter
	gsw_handle->gsw_irq_ops.IRQ_Register(gsw_handle, &irq);
	gsw_handle->gsw_irq_ops.IRQ_Enable(gsw_handle, &irq);

	INIT_WORK(&coc_work_q, (work_func_t)coc_work_task);/*initialize work Q*/
	init_timer_on_stack(&dp_coc_timer);
	dp_coc_timer.data = 0;
	dp_coc_timer.function = dp_rmon_polling;
	dp_coc_init_stat = 1;
	dp_coc_ena = 1;
	schedule_work(&coc_work_q);
	pr_info("Register DP to CPUFREQ successfully.\n");
	return 0;
}

int dp_coc_cpufreq_exit(void)
{
	int ret;

	if (dp_coc_init_stat) {
		coc_lock();
		ret = del_timer(&dp_coc_timer);
		if (ret)
			pr_err("The timer is still in use...\n");
		dp_coc_init_stat = 0;
		dp_coc_ena = 0;
		dp_coc_ps_curr = -1; /* reset current state*/
		dp_coc_ps_new = -1;
		coc_unlock();
	}

	return 0;
}
