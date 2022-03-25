/*
 * Copyright (C) Intel Corporation
 * Author: Shao Guohua <guohua.shao@intel.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */
#include<linux/init.h>
#include<linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/version.h>
#include <linux/timer.h>
#include <linux/skbuff.h>
#include <linux/if_ether.h>
#include <linux/ethtool.h>
#include <linux/proc_fs.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/clk.h>
#include <linux/if_ether.h>
#include <linux/clk.h>

#include <lantiq_soc.h>
#include <net/datapath_api.h>
#include <net/datapath_proc_api.h>
#include "../datapath.h"

#define WRAPAROUND32   0xFFFFFFFF
/*timer interval for mib wraparound handling:
 * Most mib counter is 32 bits, ie, maximu ix 0xFFFFFFFF
 * one pmac port maximum (cpu port) can support less than 3G, ie,
 * 1488096 * 3 packets for 64 bytes case. so the time to have 1 wrapround is:
 * 0xFFFFFFFF / (1488096 * 3) = 962 seconds
 * If each timer check one port and its subif, then 962/16 = 60 sec.
 */
#define POLL_INTERVAL (60 * HZ)
#define WAN_EP          15	/*WAN Interface's EP value */
#define MAX_RMON_ITF    256	/*maximum 256 GSW RMON interface supported */

struct mibs_port {
	u64 rx_good_bytes;
	u64 rx_bad_bytes;
	u64 rx_good_pkts;
	u64 rx_drop_pkts;
	/*For eth0_x only, for all others, must keep it
	 * to zero in order to share same algo
	 */
	u64 rx_drop_pkts_pae;
	u64 rx_disc_pkts_redir;	/*for eth1 only */
	u64 rx_fcs_err_pkts;
	u64 rx_undersize_good_pkts;
	u64 rx_oversize_good_pkts;
	u64 rx_undersize_err_pkts;
	u64 rx_oversize_err_pkts;
	u64 rx_align_err_pkts;
	u64 rx_filter_pkts;

	u64 tx_good_bytes;
	u64 tx_good_pkts;
	u64 tx_drop_pkts;
	/*For eth0_x only, for all others, must keep it
	 *to zero in order to share same algo
	 */
	u64 tx_drop_pkts_pae;
	u64 tx_acm_drop_pkts;
	u64 tx_acm_drop_pkts_pae;	/*for eth0_x only */
	u64 tx_disc_pkts_redir;	/*for eth1 only */
	u64 tx_coll_pkts;
	u64 tx_coll_pkts_pae;	/*for eth0_x only */
	u64 tx_pkts_redir;	/*for eth1 only */
	u64 tx_bytes_redir;	/*for eth1 only */
	u64 rx_fcs_err_pkts_pae;
	u64 rx_undersize_err_pkts_pae;
	u64 rx_oversize_err_pkts_pae;
	u64 rx_align_err_pkts_pae;

	/*driver related */
	u64 rx_drv_drop_pkts;
	u64 rx_drv_error_pkts;
	u64 tx_drv_drop_pkts;
	u64 tx_drv_error_pkts;

	/*for DSL ATM only */
	u64 tx_drv_pkts;
	u64 rx_drv_pkts;
	u64 tx_drv_bytes;
	u64 rx_drv_bytes;
};

struct mib_vap {
	u64 rx_pkts_itf;
	u64 rx_disc_pkts_itf;
	u64 rx_disc_pkts_drv;
	u64 tx_pkts_itf;
	u64 tx_disc_pkts_itf;
	u64 tx_disc_pkts_drv;
};

struct port_mib {
	struct mibs_port curr;/*tmp variable used for mib counter calculation */
	struct mib_vap curr_vap[MAX_SUBIF_PER_PORT];	/*for future */
};

struct mibs_low_lvl_port {
	GSW_RMON_Port_cnt_t l;          /*only for ethernet LAN ports */
	GSW_RMON_Port_cnt_t r;
	GSW_RMON_Redirect_cnt_t redir; /*only for ethernet WAN port */
	dp_drv_mib_t drv;
};

struct mibs_low_lvl_vap {
	GSW_RMON_If_cnt_t gsw_if; /*for pae only since GSW-L not
				   *support interface mib
				   */
	dp_drv_mib_t drv;
};

static unsigned int proc_mib_vap_start_id = 1;
static unsigned int proc_mib_vap_end_id = PMAC_MAX_NUM - 1;
static spinlock_t dp_mib_lock;
static unsigned long poll_interval = POLL_INTERVAL;

/*save port based lower level last mib counter
 * for wraparound checking
 */
struct mibs_low_lvl_port last[PMAC_MAX_NUM];
/*save vap/sub interface based lower level last mib counter
 * for wraparound checking
 */
struct mibs_low_lvl_vap last_vap[PMAC_MAX_NUM][MAX_SUBIF_PER_PORT];
/*Save all necessary aggregated basic MIB */
static struct port_mib aggregate_mib[PMAC_MAX_NUM];
/*For PAE CPU port only */
static struct port_mib aggregate_mib_r[1];

#define THREAD_MODE

#ifdef THREAD_MODE
#include <linux/kthread.h>
struct task_struct *thread;
#else
static struct timer_list exp_timer;	/*timer setting */
#endif

/*internal API: update local net mib counters periodically */
static int update_port_mib_lower_lvl(dp_subif_t *subif, u32 flag);
static int update_vap_mib_lower_lvl(dp_subif_t *subif, u32 flag);

/* ----- API implementation ------- */
static u64 wraparound(u64 curr, u64 last, u32 size)
{
#define WRAPAROUND_MAX_32 0xFFFFFFFF

	if ((size > 4) || /*for 8 bytes(64bit mib),no need to do wraparound*/
	    (curr >= last))
		return curr - last;

	return ((u64)WRAPAROUND_MAX_32) + (u64)curr - last;
}

static int port_mib_wraparound(u32 ep, struct mibs_low_lvl_port *curr,
			       struct mibs_low_lvl_port *last)
{
	return 0;
}

static int vap_mib_wraparound(dp_subif_t *subif,
			      struct mibs_low_lvl_vap *curr,
			      struct mibs_low_lvl_vap *last)
{
	return 0;
}

static int get_gsw_port_rmon(u32 ep, char *gsw_drv_name,
			     GSW_RMON_Port_cnt_t *mib)
{
	return 0;
}

static int get_gsw_redirect_rmon(u32 ep, int index,
				 GSW_RMON_Redirect_cnt_t *mib)
{
	return 0;
}

static int get_gsw_itf_rmon(u32 index, int index,
			    GSW_RMON_If_cnt_t *mib)
{
	return 0;
}

int get_gsw_interface_base(int port_id)
{
	return 0;
}

/* if ethernet WAN redirect is enabled, return 1,
 * else return 0
 */
int gsw_eth_wan_redirect_status(void)
{
	return 0;
}

/*Note:
 * Update mib counter for physical port only
 * flag so far no much use only
 */
static int update_port_mib_lower_lvl(dp_subif_t *subif, u32 flag)
{
	return 0;
}

static void mib_wraparound_timer_poll(unsigned long data)
{
}

static int update_vap_mib_lower_lvl(dp_subif_t *subif, u32 flag)
{
	return 0;
}

int dp_reset_sys_mib(u32 flag)
{
	return 0;
}

void proc_mib_timer_read(struct seq_file *s)
{
	seq_printf(s, "\nMib timer interval is %u sec\n",
		   (unsigned int)poll_interval / HZ);
}

ssize_t proc_mib_timer_write(struct file *file, const char *buf, size_t count,
			     loff_t *ppos)
{
	int len, num;
	char str[64];
	char *param_list[2];
#define MIN_POLL_TIME 2
	len = (sizeof(str) > count) ? count : sizeof(str) - 1;
	len -= copy_from_user(str, buf, len);
	str[len] = 0;
	num = dp_split_buffer(str, param_list, ARRAY_SIZE(param_list));
	poll_interval = dp_atoi(param_list[0]);

	if (poll_interval < MIN_POLL_TIME)
		poll_interval = MIN_POLL_TIME;

	poll_interval *= HZ;
#ifndef THREAD_MODE
	mod_timer(&exp_timer, jiffies + poll_interval);
#endif
	DP_DEBUG(DP_DBG_FLAG_MIB, "new poll_interval=%u sec\n",
		 (unsigned int)poll_interval / HZ);
	return count;
}

static unsigned int proc_mib_port_start_id = 1;
static unsigned int proc_mib_port_end_id = PMAC_MAX_NUM - 1;
int proc_mib_inside_dump(struct seq_file *s, int pos)
{
	return -1;
}

int proc_mib_inside_start(void)
{
	return proc_mib_port_start_id;
}

ssize_t proc_mib_inside_write(struct file *file, const char *buf,
			      size_t count, loff_t *ppos)
{
	return count;
}

/*Note:
 *if (flag & DP_F_STATS_SUBIF), get sub-interface/vap mib only
 *otherwise, get physical port's mib
 */
int dp_get_port_vap_mib_31(dp_subif_t *subif, void *priv,
			   struct rtnl_link_stats64 *net_mib, u32 flag)
{
	return -1;
}

/*Clear GSW Interface MIB: only for sub interface/vap only  */
int clear_gsw_itf_mib(dp_subif_t *subif, u32 flag)
{
	return 0;
}

int dp_clear_netif_mib_31(dp_subif_t *subif, void *priv, u32 flag)
{
	return 0;
}

int proc_mib_port_start(void)
{
	return 0;
}

int proc_mib_port_dump(struct seq_file *s, int pos)
{
	return -1;
}

ssize_t proc_mib_port_write(struct file *file, const char *buf, size_t count,
			    loff_t *ppos)
{
	return count;
}

int proc_mib_vap_dump(struct seq_file *s, int pos)
{
	return -1;
}

int proc_mib_vap_start(void)
{
	return proc_mib_vap_start_id;
}

ssize_t proc_mib_vap_write(struct file *file, const char *buf, size_t count,
			   loff_t *ppos)
{
	return count;
}

#ifdef THREAD_MODE
int mib_wraparound_thread(void *data)
{
	while (1) {
		mib_wraparound_timer_poll(0);
		msleep(poll_interval / HZ * 1000 / PMAC_MAX_NUM / 2);
		DP_DEBUG(DP_DBG_FLAG_MIB, "mib_wraparound_thread\n");
	}
}
#endif

int adjust_itf(void)
{
	return 0;
}

int set_gsw_itf(u8 ep, u8 ena, int start)
{
	return 0;
}

int reset_gsw_itf(u8 ep)
{
}

int dp_mib_init(u32 flag)
{
	spin_lock_init(&dp_mib_lock);
	memset(&aggregate_mib, 0, sizeof(aggregate_mib));
	memset(&last, 0, sizeof(last));
	memset(&last_vap, 0, sizeof(last_vap));
	adjust_itf();

#ifdef THREAD_MODE
	thread = kthread_run(mib_wraparound_thread, 0, "dp_mib");
#else
	init_timer_on_stack(&exp_timer);
	exp_timer.expires = jiffies + poll_interval;
	exp_timer.data = 0;
	exp_timer.function = mib_wraparound_timer_poll;
	add_timer(&exp_timer);
	DP_DEBUG(DP_DBG_FLAG_MIB, "dp_mib_init done\n");
#endif
	return 0;
}

void dp_mib_exit(void)
{
#ifdef THREAD_MODE
	if (thread)
		kthread_stop(thread);
#else
	del_timer(&exp_timer);
#endif
}
