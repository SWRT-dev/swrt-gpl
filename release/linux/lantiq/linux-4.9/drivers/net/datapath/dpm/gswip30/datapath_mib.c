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
#include "datapath_misc.h"
#ifdef CONFIG_LTQ_TMU
#include <net/drv_tmu_ll.h>
#endif
#if IS_ENABLED(CONFIG_PPA_TMU_MIB_SUPPORT)
#include <net/ppa/ltq_tmu_hal_api.h>
#include <net/ppa/ltq_mpe_hal.h>
#endif

static struct gsw_itf itf_assign[] = {
		{0, 1, 0, 16, 17},
		{15, 1, 17, 33, 17},
		{1, 1, 34, 50, 16},
		{2, 1, 51, 67, 16},
		{3, 1, 68, 84, 16},
		{4, 1, 85, 101, 16},
		{5, 1, 102, 118, 16},
		{-1, 1, 119, 135, 16},
		{-1, 1, 136, 152, 16},
		{-1, 1, 153, 169, 16},
		{-1, 1, 170, 186, 16},
		{-1, 1, 187, 203, 16},
		{-1, 1, 204, 220, 16},
		{-1, 1, 221, 237, 16},
		{-1, 1, 238, 255, 17}
};

#define WRAPAROUND32   0xFFFFFFFF
/*timer interval for mib wraparound handling:
 * Most mib counter is 32 bits, ie, maximu ix 0xFFFFFFFF
 * one pmac port maximum (cpu port) can support less than 3G, ie,
 * 1488096 * 3 packets for 64 bytes case.
 * so the time to have 1 wrapround is:
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
	 *  to zero in order to share same algo
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

	/*tmu related */
	u64 tx_tmu_drop_pkts;
	u64 tx_mpe_drop_pkts;
	u64 tx_tmu_csum_offload_pkts;
	u64 tx_tmu_csum_offload_bytes;

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
	u64 tx_disc_pkts_tmu;
	u64 tx_disc_pkts_mpe;
	u64 tx_disc_pkts_drv;
};

struct port_mib {
	struct mibs_port curr; /*tmp variable used for mib counter cal*/
	struct mib_vap curr_vap[MAX_SUBIF_PER_PORT];	/*for future */
};

struct mibs_low_lvl_port {
	GSW_RMON_Port_cnt_t l;          /*only for ethernet LAN ports */
	GSW_RMON_Port_cnt_t r;
	GSW_RMON_Redirect_cnt_t redir; /*only for ethernet WAN port */
	dp_drv_mib_t drv;
#ifdef CONFIG_INTEL_DATAPATH_MIB_TMU_MPE_MIB
	struct tmu_hal_qos_stats tmu_qos[MAX_SUBIF_PER_PORT];
	struct tmu_hal_qos_stats tmu_chksum;  /*only for ethernet WAN port */
	struct mpe_hal_if_stats mpe;
#endif
};

struct mibs_low_lvl_vap {
	GSW_RMON_If_cnt_t gsw_if; /*for pae only(L not support interface mib)*/
#ifdef CONFIG_INTEL_DATAPATH_MIB_TMU_MPE_MIB
	struct tmu_hal_qos_stats tmu_qos;
	struct mpe_hal_if_stats mpe;
#endif
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
 *for wraparound checking
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

#if IS_ENABLED(CONFIG_INTEL_DATAPATH_MIB_TMU_MPE_MIB)
static int32_t (*tmu_hal_get_qos_m_local)(struct net_device *dev,
					  dp_subif_t *subif_id,
					  s32 queue_id,
					  struct tmu_hal_qos_stats *qos_mib,
					  u32 flag);
static int32_t (*tmu_hal_get_csum_ol_m_local)(
	struct tmu_hal_qos_stats *csum_mib, uint32_t flag);
static int32_t (*tmu_hal_clear_qos_m_local)(struct net_device *dev,
					    dp_subif_t *subif,
					    s32 queue_id,
					    uint32_t flag);
static int32_t (*tmu_hal_clear_csum_ol_m_local)(uint32_t flag);
static int32_t (*mpe_hal_get_netif_m_local)(struct net_device *dev,
					    dp_subif_t *subif_id,
					    struct mpe_hal_if_stats *mpe_mib,
					    uint32_t flag);
static int32_t (*mpe_hal_clear_if_m_local)(struct net_device *dev,
					   dp_subif_t *subif_id,
					   uint32_t flag);
#endif
int dp_get_port_vap_mib_30(dp_subif_t *subif, void *priv,
			   struct rtnl_link_stats64 *net_mib, u32 flag);

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
	DP_DEBUG(DP_DBG_FLAG_MIB, "Wraparound happen:%s 0x%llx %s 0x%llx\n",
		 "current mib: ", curr, "  last mib: ", last);
	return ((u64)WRAPAROUND_MAX_32) + (u64)curr - last;
}

static int port_mib_wraparound(u32 ep, struct mibs_low_lvl_port *curr,
			       struct mibs_low_lvl_port *last)
{
/* RMON_PORT_WRAP: c-current, l-last x-size in bytes */
#define RMON_PORT_WRAP(c, l, x) wraparound((c)->x, (l)->x, sizeof((l)->x))
	GSW_RMON_Port_cnt_t *curr_tmp;
	GSW_RMON_Port_cnt_t *last_tmp;
#ifdef CONFIG_INTEL_DATAPATH_MIB_TMU_MPE_MIB
	int i;
#endif

	if (unlikely(ep >= PMAC_MAX_NUM))
		return -1;
	if (ep < PAMC_LAN_MAX_NUM) {
		curr_tmp = &curr->l;
		last_tmp = &last->l;
	} else {
		curr_tmp = &curr->r;
		last_tmp = &last->r;
	}
	/*First handle common RMON mib */
	//spin_lock_bh(&dp_mib_lock);
	aggregate_mib[ep].curr.rx_good_bytes +=
	    RMON_PORT_WRAP(curr_tmp, last_tmp, nRxGoodBytes);
	aggregate_mib[ep].curr.rx_bad_bytes +=
	    RMON_PORT_WRAP(curr_tmp, last_tmp, nRxBadBytes);
	aggregate_mib[ep].curr.rx_good_pkts +=
	    RMON_PORT_WRAP(curr_tmp, last_tmp, nRxGoodPkts);
	aggregate_mib[ep].curr.rx_drop_pkts +=
	    RMON_PORT_WRAP(curr_tmp, last_tmp, nRxDroppedPkts);
	aggregate_mib[ep].curr.rx_fcs_err_pkts +=
	    RMON_PORT_WRAP(curr_tmp, last_tmp, nRxFCSErrorPkts);
	aggregate_mib[ep].curr.rx_undersize_good_pkts +=
	    RMON_PORT_WRAP(curr_tmp, last_tmp, nRxUnderSizeGoodPkts);
	aggregate_mib[ep].curr.rx_oversize_good_pkts +=
	    RMON_PORT_WRAP(curr_tmp, last_tmp, nRxOversizeGoodPkts);
	aggregate_mib[ep].curr.rx_undersize_err_pkts +=
	    RMON_PORT_WRAP(curr_tmp, last_tmp, nRxUnderSizeErrorPkts);
	aggregate_mib[ep].curr.rx_oversize_err_pkts +=
	    RMON_PORT_WRAP(curr_tmp, last_tmp, nRxOversizeErrorPkts);
	aggregate_mib[ep].curr.rx_align_err_pkts +=
	    RMON_PORT_WRAP(curr_tmp, last_tmp, nRxAlignErrorPkts);
	aggregate_mib[ep].curr.rx_filter_pkts +=
	    RMON_PORT_WRAP(curr_tmp, last_tmp, nRxFilteredPkts);
	aggregate_mib[ep].curr.tx_good_bytes +=
	    RMON_PORT_WRAP(curr_tmp, last_tmp, nTxGoodBytes);
	aggregate_mib[ep].curr.tx_good_pkts +=
	    RMON_PORT_WRAP(curr_tmp, last_tmp, nTxGoodPkts);
	aggregate_mib[ep].curr.tx_drop_pkts +=
	    RMON_PORT_WRAP(curr_tmp, last_tmp, nTxDroppedPkts);
	aggregate_mib[ep].curr.tx_acm_drop_pkts +=
	    RMON_PORT_WRAP(curr_tmp, last_tmp, nTxAcmDroppedPkts);
	aggregate_mib[ep].curr.tx_coll_pkts +=
	    RMON_PORT_WRAP(curr_tmp, last_tmp, nTxCollCount);

	/*pae port 0 is not required by concept, for debugging only*/
	if (ep == 0) {
		curr_tmp = &curr->r;
		last_tmp = &last->r;
		aggregate_mib_r[0].curr.rx_good_bytes +=
		    RMON_PORT_WRAP(curr_tmp, last_tmp, nRxGoodBytes);
		aggregate_mib_r[0].curr.rx_bad_bytes +=
		    RMON_PORT_WRAP(curr_tmp, last_tmp, nRxBadBytes);
		aggregate_mib_r[0].curr.rx_good_pkts +=
		    RMON_PORT_WRAP(curr_tmp, last_tmp, nRxGoodPkts);
		aggregate_mib_r[0].curr.rx_drop_pkts +=
		    RMON_PORT_WRAP(curr_tmp, last_tmp, nRxDroppedPkts);
		aggregate_mib_r[0].curr.rx_fcs_err_pkts +=
		    RMON_PORT_WRAP(curr_tmp, last_tmp, nRxFCSErrorPkts);
		aggregate_mib_r[0].curr.rx_undersize_good_pkts +=
		    RMON_PORT_WRAP(curr_tmp, last_tmp, nRxUnderSizeGoodPkts);
		aggregate_mib_r[0].curr.rx_oversize_good_pkts +=
		    RMON_PORT_WRAP(curr_tmp, last_tmp, nRxOversizeGoodPkts);
		aggregate_mib_r[0].curr.rx_undersize_err_pkts +=
		    RMON_PORT_WRAP(curr_tmp, last_tmp, nRxUnderSizeErrorPkts);
		aggregate_mib_r[0].curr.rx_oversize_err_pkts +=
		    RMON_PORT_WRAP(curr_tmp, last_tmp, nRxOversizeErrorPkts);
		aggregate_mib_r[0].curr.rx_align_err_pkts +=
		    RMON_PORT_WRAP(curr_tmp, last_tmp, nRxAlignErrorPkts);
		aggregate_mib_r[0].curr.rx_filter_pkts +=
		    RMON_PORT_WRAP(curr_tmp, last_tmp, nRxFilteredPkts);
		aggregate_mib_r[0].curr.tx_good_bytes +=
		    RMON_PORT_WRAP(curr_tmp, last_tmp, nTxGoodBytes);
		aggregate_mib_r[0].curr.tx_good_pkts +=
		    RMON_PORT_WRAP(curr_tmp, last_tmp, nTxGoodPkts);
		aggregate_mib_r[0].curr.tx_drop_pkts +=
		    RMON_PORT_WRAP(curr_tmp, last_tmp, nTxDroppedPkts);
		aggregate_mib_r[0].curr.tx_acm_drop_pkts +=
		    RMON_PORT_WRAP(curr_tmp, last_tmp, nTxAcmDroppedPkts);
		aggregate_mib_r[0].curr.tx_coll_pkts +=
		    RMON_PORT_WRAP(curr_tmp, last_tmp, nTxCollCount);

		/*save */
		*last = *curr;
		//spin_unlock_bh(&dp_mib_lock);
		return 0;
	}

#ifdef CONFIG_INTEL_DATAPATH_MIB_TMU_MPE_MIB
	/*TMU drop counters */
	for (i = 0; i < MAX_SUBIF_PER_PORT; i++)
		aggregate_mib[ep].curr.tx_tmu_drop_pkts +=
			wraparound(curr->tmu_qos[i].dropPkts,
				   last->tmu_qos[i].dropPkts,
				   sizeof(last->tmu_qos[i].dropPkts));
	aggregate_mib[ep].curr.tx_mpe_drop_pkts +=
		wraparound(curr->mpe.dropPkts, last->mpe.dropPkts,
			   sizeof(last->mpe.dropPkts));
	aggregate_mib[ep].curr.tx_tmu_csum_offload_pkts +=
		wraparound(curr->tmu_chksum.deqPkts,
			   last->tmu_chksum.deqPkts,
			   sizeof(last->tmu_chksum.deqPkts));
	aggregate_mib[ep].curr.tx_tmu_csum_offload_bytes +=
		wraparound(curr->tmu_chksum.deqBytes,
			   last->tmu_chksum.deqBytes,
			   sizeof(last->tmu_chksum.deqBytes));
#endif
	if (ep < PAMC_LAN_MAX_NUM) {
		/*speical handling for eth0_x: */
		/*   get drop cnt from its pae's mapped port,*/
		/*   Don't save whole last as *last = *curr */
		curr_tmp = &curr->r;
		last_tmp = &last->r;
		aggregate_mib[ep].curr.rx_drop_pkts_pae +=
		    RMON_PORT_WRAP(curr_tmp, last_tmp, nRxDroppedPkts);
		aggregate_mib[ep].curr.tx_drop_pkts_pae +=
		    RMON_PORT_WRAP(curr_tmp, last_tmp, nTxDroppedPkts);
		aggregate_mib[ep].curr.tx_coll_pkts_pae +=
		    RMON_PORT_WRAP(curr_tmp, last_tmp, nTxCollCount);

		aggregate_mib[ep].curr.rx_fcs_err_pkts_pae +=
		    RMON_PORT_WRAP(curr_tmp, last_tmp, nRxFCSErrorPkts);
		aggregate_mib[ep].curr.rx_undersize_err_pkts_pae +=
		    RMON_PORT_WRAP(curr_tmp, last_tmp, nRxUnderSizeErrorPkts);
		aggregate_mib[ep].curr.rx_oversize_err_pkts_pae +=
		    RMON_PORT_WRAP(curr_tmp, last_tmp, nRxOversizeErrorPkts);
		aggregate_mib[ep].curr.rx_align_err_pkts_pae +=
		    RMON_PORT_WRAP(curr_tmp, last_tmp, nRxAlignErrorPkts);

	} else if (ep == WAN_EP) {	/*redirect mib */
		aggregate_mib[ep].curr.rx_disc_pkts_redir +=
		    wraparound(curr->redir.nRxDiscPktsCount,
			       last->redir.nRxDiscPktsCount,
			       sizeof(last->redir.nRxDiscPktsCount));
		aggregate_mib[ep].curr.tx_disc_pkts_redir +=
		    wraparound(curr->redir.nTxDiscPktsCount,
			       last->redir.nTxDiscPktsCount,
			       sizeof(last->redir.nTxDiscPktsCount));
		aggregate_mib[ep].curr.tx_pkts_redir +=
		    wraparound(curr->redir.nTxPktsCount,
			       last->redir.nTxPktsCount,
			       sizeof(last->redir.nTxPktsCount));
		aggregate_mib[ep].curr.tx_bytes_redir +=
		    wraparound(curr->redir.nTxBytesCount,
			       last->redir.nTxBytesCount,
			       sizeof(last->redir.nTxBytesCount));
	}
	/*save */
	*last = *curr;
	//spin_unlock_bh(&dp_mib_lock);
	return 0;
}

static int vap_mib_wraparound(dp_subif_t *subif,
			      struct mibs_low_lvl_vap *curr,
			      struct mibs_low_lvl_vap *last)
{
#define VAP_RMON_WRAP_ITF(c, l, x) wraparound((c)->gsw_if.x, (l)->gsw_if.x, sizeof((l)->gsw_if.x))
#define VAP_RMON_WRAP_TMU(c, l, x) wraparound((c)->tmu_qos.x, (l)->tmu_qos.x, sizeof((l)->tmu_qos.x))
#define VAP_RMON_WRAP_MPE(c, l, x) wraparound((c)->mpe.x, (l)->mpe.x, sizeof((l)->mpe.x))
#define VAP_RMON_WRAP_DRV(c, l, x) wraparound((c)->drv.x, (l)->drv.x, sizeof((l)->drv.x))

	int ep = subif->port_id;
	struct pmac_port_info *pi = get_dp_port_info(0, subif->port_id);
	int vap = GET_VAP(subif->subif, pi->vap_offset, pi->vap_mask);

	if ((ep <= 0) ||
	    (ep >= PMAC_MAX_NUM))
		return -1;
	//spin_lock_bh(&dp_mib_lock);
	aggregate_mib[ep].curr_vap[vap].rx_pkts_itf +=
	    VAP_RMON_WRAP_ITF(curr, last, nRxPktsCount);
	aggregate_mib[ep].curr_vap[vap].rx_disc_pkts_itf +=
	    VAP_RMON_WRAP_ITF(curr, last, nRxDiscPktsCount);
	aggregate_mib[ep].curr_vap[vap].rx_disc_pkts_drv +=
	    VAP_RMON_WRAP_ITF(curr, last, nRxDiscPktsCount);

	aggregate_mib[ep].curr_vap[vap].tx_pkts_itf +=
	    VAP_RMON_WRAP_ITF(curr, last, nTxPktsCount);
	aggregate_mib[ep].curr_vap[vap].tx_disc_pkts_itf +=
	    VAP_RMON_WRAP_ITF(curr, last, nTxDiscPktsCount);
#ifdef CONFIG_INTEL_DATAPATH_MIB_TMU_MPE_MIB
	aggregate_mib[ep].curr_vap[vap].tx_disc_pkts_tmu +=
	    VAP_RMON_WRAP_TMU(curr, last, dropPkts);
	aggregate_mib[ep].curr_vap[vap].tx_disc_pkts_mpe +=
	    VAP_RMON_WRAP_MPE(curr, last, dropPkts);
#endif
	aggregate_mib[ep].curr_vap[vap].tx_disc_pkts_drv +=
	    VAP_RMON_WRAP_DRV(curr, last, tx_drop_pkts);
	/*save */
	*last = *curr;
	//spin_unlock_bh(&dp_mib_lock);
	return 0;
}

static int get_gsw_port_rmon(u32 ep, int index,
			     GSW_RMON_Port_cnt_t *mib)
{
	GSW_return_t ret;
	struct core_ops *gsw_handle;

	if (!mib) {
		pr_err("why mib pointer is %p\n", mib);
		return -1;
	}
	if (ep >= PMAC_MAX_NUM)
		return -1;
	memset(mib, 0, sizeof(*mib));
	mib->nPortId = ep;
	gsw_handle = dp_port_prop[0].ops[index];
	ret = gsw_handle->gsw_rmon_ops.RMON_Port_Get(gsw_handle, mib);
	if (ret) {
		pr_err("GSW_RMON_PORT_GET failed(%d) for port %d\n",
		       ret, ep);
		return -1;
	}

	return 0;
}

static int get_gsw_redirect_rmon(u32 ep, int index,
				 GSW_RMON_Redirect_cnt_t *mib)
{
	GSW_return_t ret;
	struct core_ops *gsw_handle;

	if (!mib) {
		pr_err("why mib pointer is %p\n", mib);
		return -1;
	}

	memset(mib, 0, sizeof(*mib));
	gsw_handle = dp_port_prop[0].ops[index];
	ret = gsw_handle->gsw_rmon_ops.RMON_Redirect_Get(gsw_handle, mib);
	if (ret) {
		pr_err("GSW_RMON_REDIRECT_GET failed\n");
		return -1;
	}

	return 0;
}

static int get_gsw_itf_rmon(u32 itf_index, int index,
			    GSW_RMON_If_cnt_t *mib)
{
	GSW_return_t ret;
	struct core_ops *gsw_handle;

	if (!mib) {
		pr_err("why mib pointer is %p\n", mib);
		return -1;
	}
	memset(mib, 0, sizeof(*mib));
	mib->nIfId = itf_index;
	gsw_handle = dp_port_prop[0].ops[index];
	ret = gsw_handle->gsw_rmon_ops.RMON_If_Get(gsw_handle, mib);
	if (ret) {
		pr_err
		    ("GSW_RMON_PORT_GET GSW_RMON_IF_GET for index %d\n", index);
		return -1;
	}
	return 0;
}

int get_gsw_interface_base(int port_id)
{
	struct pmac_port_info *port_info;

	if ((port_id <= 0) || (port_id >= PMAC_MAX_NUM)) {
		pr_err("Wrong subif\n");
		return -1;
	}

	port_info = get_dp_port_info(0, port_id);
	if (!port_info)
		return -1;
	if (!port_info->itf_info)
		return -1;
	return (int)port_info->itf_info->start;
}

/* if ethernet WAN redirect is enabled, return 1,
 * else return 0
 */
int gsw_eth_wan_redirect_status(void)
{
	GSW_QoS_queuePort_t q_cfg;
	int i;
	GSW_return_t ret;
	struct core_ops *gsw_handle;
	struct qos_ops *gsw_qos;
	#define MAX_CLASS_NUM 16

	gsw_handle = dp_port_prop[0].ops[1];
	gsw_qos = &gsw_handle->gsw_qos_ops;
	memset(&q_cfg, 0, sizeof(q_cfg));
	q_cfg.nPortId = WAN_EP;
	for (i = 0; i <= MAX_CLASS_NUM; i++) {
		q_cfg.nTrafficClassId = i;
		ret = gsw_qos->QoS_QueuePortGet(gsw_handle, &q_cfg);
		if (ret) {
			pr_err("%s failed(%d) from %s for port %d\n",
			       "GSW_QOS_QUEUE_PORT_GET",
			       ret, "GSWIP_R", WAN_EP);
			return -1;
		}
		if (q_cfg.nRedirectPortId == 0)
			return 1;
	}

	return 0;
}

/*Note:
 *Update mib counter for physical port only
 *flag so far no much use only
 */
static int update_port_mib_lower_lvl(dp_subif_t *subif, u32 flag)
{
	GSW_return_t ret;
	struct mibs_low_lvl_port *curr;
	dp_subif_t tmp;
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_MIB_TMU_MPE_MIB)
	int update_flag, i;
	struct pmac_port_info *port;
#endif

	/*update struct pmac_port_info[subif->ep].net_mib */
	if (!subif ||
	    (subif->port_id < 0) ||
	    (subif->port_id >= PMAC_MAX_NUM)) {
		if (!subif)
			DP_DEBUG(DP_DBG_FLAG_MIB,
				 "%s error:NULL subif\n",
				 "update_port_mib_lower_lvl");
		else
			DP_DEBUG(DP_DBG_FLAG_MIB,
				 "%s wrong port_id %d\n",
				 "update_port_mib_lower_lvl",
				 subif->port_id);
		return -1;
	}
	curr = kmalloc(sizeof(*curr), GFP_KERNEL);
	if (!curr)
		return -1;
	memset(curr, 0, sizeof(*curr));
	tmp = *subif;
	if (tmp.port_id  == 0) {
		/*not required by concept, just for debugging purpose*/
		ret = get_gsw_port_rmon(tmp.port_id, GSWIP_L,
					&curr->l);
		if (ret)	/*workaroud otherwise wrongly wrapround */
			curr->l = last[tmp.port_id].l;
		ret = get_gsw_port_rmon(tmp.port_id, GSWIP_R,
					&curr->r);
		if (ret)	/*workaroud otherwise wrongly wrapround */
			curr->r = last[tmp.port_id].r;
		goto HANDLE_WRAPWROUND;
	} else if (tmp.port_id < PAMC_LAN_MAX_NUM) { /* For port_id: 1 ~ 6 */
		ret =
		    get_gsw_port_rmon(tmp.port_id, GSWIP_L,
				      &curr->l);
		if (ret)	/*workaroud otherwise wrongly wrapround */
			curr->l = last[tmp.port_id].l;
	}
	/*common to all: pae mib still needs for eth0_x */
	ret = get_gsw_port_rmon(tmp.port_id, GSWIP_R, &curr->r);
	if (ret)/*workaroud */
		curr->r = last[tmp.port_id].r;

#if IS_ENABLED(CONFIG_INTEL_DATAPATH_MIB_TMU_MPE_MIB)
	/* collect all mib per VAP for TMU and MPE MIB */
	tmu_hal_get_qos_m_local = tmu_hal_get_qos_mib_hook_fn;
	port = get_dp_port_info(0, tmp.port_id);
	if (tmu_hal_get_qos_m_local &&
	    port && port->status) { /*get all VAP's TMU MIB*/
		for (i = 0; i < MAX_SUBIF_PER_PORT; i++) {
			DP_DEBUG(DP_DBG_FLAG_MIB,
				 "tmu_hal_get_qos_m_local: %d/%d\n",
				 tmp.port_id, tmp.subif);
			if (!get_dp_port_subif(port, i)->flags) {
				ret = -1;
			} else {
				tmp.subif = SET_VAP(i, port->vap_offset,
						    port->vap_mask);
				ret = tmu_hal_get_qos_m_local(NULL,
							      &tmp, -1,
							      &curr->tmu_qos[i],
							      0);
			}
			if (ret) /*workaround */
				memcpy(&curr->tmu_qos[i],
				       &last[tmp.port_id].tmu_qos[i],
				       sizeof(curr->tmu_qos[i]));
		}
	} else
		memcpy(&curr->tmu_qos, &last[tmp.port_id].tmu_qos,
		       sizeof(curr->tmu_qos));
	tmp = *subif;
	mpe_hal_get_netif_m_local = mpe_hal_get_netif_mib_hook_fn;
	update_flag = 0;
	if (mpe_hal_get_netif_m_local) {
		ret = mpe_hal_get_netif_m_local(NULL, &tmp,
						&curr->mpe, 0);
		if (!ret)
			update_flag  = 1; /*succeed */
	}
	if (!update_flag)
		memcpy(&curr->mpe, &last[tmp.port_id].mpe,
		       sizeof(curr->mpe));
#endif

	/*get redirect mib for eth1 only */
	if (tmp.port_id == WAN_EP) {
		ret =
		    get_gsw_redirect_rmon(tmp.port_id, GSWIP_R,
					  &curr->redir);
		if (ret)	/*workaroud */
			curr->redir = last[tmp.port_id].redir;
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_MIB_TMU_MPE_MIB)
		tmu_hal_get_csum_ol_m_local =
			tmu_hal_get_csum_ol_mib_hook_fn;
		update_flag = 0;
		if (tmu_hal_get_csum_ol_m_local) {
			ret = tmu_hal_get_csum_ol_m_local(&curr->tmu_chksum, 0);
			if (!ret)
				update_flag = 1;
		}
		if (!update_flag)
			memcpy(&curr->tmu_chksum, &last[tmp.port_id].tmu_chksum,
			       sizeof(curr->tmu_chksum));
#endif
	}
	/*get drv mib */
	ret = dp_get_drv_mib(subif, &curr->drv, 0);
	if (ret)	/*workaroud */
		curr->drv = last[tmp.port_id].drv;
HANDLE_WRAPWROUND:
	ret =
	    port_mib_wraparound(tmp.port_id, curr, &last[tmp.port_id]);
	kfree(curr);
	return ret;
}

static void mib_wraparound_timer_poll(unsigned long data)
{
	int i;
	dp_subif_t subif = {0};
#define START_PORT_ID 0
	/*start from port 1, not 0 since 0 is no device registered*/
	static int port = START_PORT_ID;

	subif.port_id = port;
	subif.subif = 0;
	/* update physical port mib only */
	update_port_mib_lower_lvl(&subif, 0);
	/* update vap if necessary */
	if (port) {
		for (i = 0; i < MAX_SUBIF_PER_PORT; i++) {
			struct pmac_port_info *pi = get_dp_port_info(0, port);

			subif.subif = SET_VAP(i, pi->vap_offset, pi->vap_mask);
			/* update sub-interface/vap mib only */
			if (update_vap_mib_lower_lvl(&subif, 0))
				break;
		}
	}
	port++;
	if (port == PMAC_MAX_NUM)
		port = START_PORT_ID;

#ifndef THREAD_MODE
	exp_timer.expires = jiffies + poll_interval;
	add_timer(&exp_timer);
#endif
}

static int update_vap_mib_lower_lvl(dp_subif_t *subif, u32 flag)
{
	int ret;
	u8 vap;
	int itf_index;
	int itf_base;
	struct mibs_low_lvl_vap *curr;
	int port_id;
	struct pmac_port_info *pi;

	/*update struct pmac_port_info[subif->ep].net_mib */
	if (!subif || (subif->port_id <= 0) || (subif->port_id >= PMAC_MAX_NUM))
		return -1;

	curr = kmalloc(sizeof(*curr), GFP_KERNEL);
	if (!curr)
		return -1;
	memset(curr, 0, sizeof(*curr));
	port_id = subif->port_id;
	pi = get_dp_port_info(0, subif->port_id);
	vap = GET_VAP(subif->subif, pi->vap_offset, pi->vap_mask);
	/* get gsw PAE interface mib counter */
	itf_base = get_gsw_interface_base(port_id);
	if (itf_base < 0) {
		ret = -1;
		DP_DEBUG(DP_DBG_FLAG_MIB, "wrong itf_base(%d) for port %d\n",
			 itf_base, port_id);
		goto EXIT;
	}
	itf_index = itf_base + vap;
	if (itf_index >= MAX_RMON_ITF) {
		ret = -1;
		pr_err("wrong itf_index(%d) for port %d\n", itf_index,
		       port_id);
		goto EXIT;
	}
	ret = get_gsw_itf_rmon(itf_index, GSWIP_R, &curr->gsw_if);
	if (ret) {
		curr->gsw_if = last_vap[port_id][vap].gsw_if;
		pr_err("get_gsw_itf_rmon failed for port/vap(%d/%d)", port_id,
		       vap);
	}
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_MIB_TMU_MPE_MIB)
	/*get TMU mib */
	tmu_hal_get_qos_m_local = tmu_hal_get_qos_mib_hook_fn;
	if (!tmu_hal_get_qos_m_local) {
		curr->tmu_qos = last_vap[port_id][vap].tmu_qos;
	} else {
		ret = tmu_hal_get_qos_m_local(NULL, subif, -1,
					      &curr->tmu_qos, 0);
		if (ret) {
			curr->tmu_qos = last_vap[port_id][vap].tmu_qos;
			DP_DEBUG(DP_DBG_FLAG_MIB, "%s failed for port.vap(%d.%d):%d\n",
			       "tmu_hal_get_qos_mib_hook_fn",
			       port_id, vap, ret);
		}
	}
	/*get MPE mib */
	mpe_hal_get_netif_m_local = mpe_hal_get_netif_mib_hook_fn;
	if (!mpe_hal_get_netif_m_local) {
		curr->mpe = last_vap[port_id][vap].mpe;
	} else {
		ret = mpe_hal_get_netif_m_local(NULL,
						subif,
						&curr->mpe,
						0);
		if (ret) {
			curr->mpe = last_vap[port_id][vap].mpe;
			pr_err("%s failed for port.vap(%d.%d):%d\n",
			       "mpe_hal_get_netif_mib_hook_fn",
			       port_id, vap, ret);
		}
	}
#endif
	/*get driver mib */
	ret = dp_get_drv_mib(subif, &curr->drv, DP_F_STATS_SUBIF);
	if (ret)	/*workaroud */
		curr->drv = last_vap[port_id][vap].drv;

	ret = vap_mib_wraparound(subif, curr, &last_vap[port_id][vap]);
EXIT:
	kfree(curr);

	return ret;
}

int dp_reset_sys_mib(u32 flag)
{
	dp_clear_netif_stats(NULL, NULL, 0);
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
	pr_info("new poll_interval=%u sec\n",
		(unsigned int)poll_interval / HZ);
	return count;
}

static unsigned int proc_mib_port_start_id = 1;
static unsigned int proc_mib_port_end_id = PMAC_MAX_NUM - 1;
int proc_mib_inside_dump(struct seq_file *s, int pos)
{
	dp_subif_t subif;
	struct rtnl_link_stats64 net_mib;

	seq_printf(s, "EP=%d\n", pos);
	/*l */
	seq_printf(s, "  %-45s=%40u\n", "last.l.nRxGoodPkts",
		   last[pos].l.nRxGoodPkts);
	seq_printf(s, "  %-45s=%40u\n", "last.l.nRxFCSErrorPkts",
		   last[pos].l.nRxFCSErrorPkts);
	seq_printf(s, "  %-45s=%40u\n", "last.l.nRxUnderSizeGoodPkts",
		   last[pos].l.nRxUnderSizeGoodPkts);
	seq_printf(s, "  %-45s=%40u\n", "last.l.nRxOversizeGoodPkts",
		   last[pos].l.nRxOversizeGoodPkts);
	seq_printf(s, "  %-45s=%40u\n", "last.l.nRxUnderSizeErrorPkts",
		   last[pos].l.nRxUnderSizeErrorPkts);
	seq_printf(s, "  %-45s=%40u\n", "last.l.nRxOversizeErrorPkts",
		   last[pos].l.nRxOversizeErrorPkts);
	seq_printf(s, "  %-45s=%40u\n", "last.l.nRxAlignErrorPkts",
		   last[pos].l.nRxAlignErrorPkts);
	seq_printf(s, "  %-45s=%40u\n", "last.l.nRxDroppedPkts",
		   last[pos].l.nRxDroppedPkts);
	seq_printf(s, "  %-45s=%40u\n", "last.l.nRxFilteredPkts",
		   last[pos].l.nRxFilteredPkts);
	seq_printf(s, "  %-45s=%40u\n", "last.l.nRxGoodPkts",
		   last[pos].l.nTxGoodPkts);
	seq_printf(s, "  %-45s=%40u\n", "last.l.nTxDroppedPkts",
		   last[pos].l.nTxDroppedPkts);
	seq_printf(s, "  %-45s=%40u\n", "last.l.nTxCollCount",
		   last[pos].l.nTxCollCount);
	/*r */
	seq_printf(s, "  %-45s=%40u\n", "last.r.nRxGoodPkts",
		   last[pos].r.nRxGoodPkts);
	seq_printf(s, "  %-45s=%40u\n", "last.r.nRxFCSErrorPkts",
		   last[pos].r.nRxFCSErrorPkts);
	seq_printf(s, "  %-45s=%40u\n", "last.r.nRxUnderSizeGoodPkts",
		   last[pos].r.nRxUnderSizeGoodPkts);
	seq_printf(s, "  %-45s=%40u\n", "last.r.nRxOversizeGoodPkts",
		   last[pos].r.nRxOversizeGoodPkts);
	seq_printf(s, "  %-45s=%40u\n", "last.r.nRxUnderSizeErrorPkts",
		   last[pos].r.nRxUnderSizeErrorPkts);
	seq_printf(s, "  %-45s=%40u\n", "last.r.nRxOversizeErrorPkts",
		   last[pos].r.nRxOversizeErrorPkts);
	seq_printf(s, "  %-45s=%40u\n", "last.r.nRxAlignErrorPkts",
		   last[pos].r.nRxAlignErrorPkts);
	seq_printf(s, "  %-45s=%40u\n", "last.r.nRxDroppedPkts",
		   last[pos].r.nRxDroppedPkts);
	seq_printf(s, "  %-45s=%40u\n", "last.r.nRxFilteredPkts",
		   last[pos].r.nRxFilteredPkts);
	/*redirct */
	seq_printf(s, "  %-45s=%40u\n", "last.redir.nRxPktsCount",
		   last[pos].redir.nRxPktsCount);
	seq_printf(s, "  %-45s=%40u\n", "last.redir.nRxDiscPktsCount",
		   last[pos].redir.nRxDiscPktsCount);
	seq_printf(s, "  %-45s=%40u\n", "last.redir.nTxPktsCount",
		   last[pos].redir.nTxPktsCount);
	seq_printf(s, "  %-45s=%40u\n", "last.redir.nTxDiscPktsCount",
		   last[pos].redir.nTxDiscPktsCount);
#ifdef CONFIG_INTEL_DATAPATH_MIB_TMU_MPE_MIB
	/*checksum */
	seq_printf(s, "  %-45s=%40llu\n", "last.tmu_chksum.deqPkts",
		   last[pos].tmu_chksum.deqPkts);
	seq_printf(s, "  %-45s=%40llu\n", "last.tmu_chksum.deqBytes",
		   last[pos].tmu_chksum.deqBytes);
#endif
	/*mib array */
	seq_printf(s, "  %-45s=%40llu\n", "a_mib.curr.rx_good_pkts",
		   aggregate_mib[pos].curr.rx_good_pkts);
	seq_printf(s, "  %-45s=%40llu\n", "a_mib.curr.rx_drop_pkts_pae",
		   aggregate_mib[pos].curr.rx_drop_pkts_pae);
	seq_printf(s, "  %-45s=%40llu\n", "a_mib.curr.rx_disc_pkts_redir",
		   aggregate_mib[pos].curr.rx_disc_pkts_redir);
	seq_printf(s, "  %-45s=%40llu\n", "a_mib.curr.rx_drop_pkts",
		   aggregate_mib[pos].curr.rx_drop_pkts);
	seq_printf(s, "  %-45s=%40llu\n", "a_mib.curr.rx_drop_pkts_pae",
		   aggregate_mib[pos].curr.rx_drop_pkts_pae);
	seq_printf(s, "  %-45s=%40llu\n", "a_mib.curr.rx_disc_pkts_redir",
		   aggregate_mib[pos].curr.rx_disc_pkts_redir);
	seq_printf(s, "  %-45s=%40llu\n", "a_mib.curr.rx_fcs_err_pkts",
		   aggregate_mib[pos].curr.rx_fcs_err_pkts);
	seq_printf(s, "  %-45s=%40llu\n", "a_mib.curr.rx_undersize_good_pkts",
		   aggregate_mib[pos].curr.rx_undersize_good_pkts);
	seq_printf(s, "  %-45s=%40llu\n", "a_mib.curr.rx_oversize_good_pkts",
		   aggregate_mib[pos].curr.rx_oversize_good_pkts);
	seq_printf(s, "  %-45s=%40llu\n", "a_mib.curr.rx_undersize_err_pkts",
		   aggregate_mib[pos].curr.rx_undersize_err_pkts);
	seq_printf(s, "  %-45s=%40llu\n", "a_mib.curr.rx_oversize_err_pkts",
		   aggregate_mib[pos].curr.rx_oversize_err_pkts);
	seq_printf(s, "  %-45s=%40llu\n", "a_mib.curr.rx_align_err_pkts",
		   aggregate_mib[pos].curr.rx_align_err_pkts);
	seq_printf(s, "  %-45s=%40llu\n", "a_mib.curr.rx_filter_pkts",
		   aggregate_mib[pos].curr.rx_filter_pkts);
	seq_printf(s, "  %-45s=%40llu\n", "a_mib.curr.tx_good_pkts",
		   aggregate_mib[pos].curr.tx_good_pkts);
	seq_printf(s, "  %-45s=%40llu\n", "a_mib.curr.tx_drop_pkts",
		   aggregate_mib[pos].curr.tx_drop_pkts);
	seq_printf(s, "  %-45s=%40llu\n", "a_mib.curr.tx_drop_pkts_pae",
		   aggregate_mib[pos].curr.tx_drop_pkts_pae);
	seq_printf(s, "  %-45s=%40llu\n", "a_mib.curr.tx_acm_drop_pkts",
		   aggregate_mib[pos].curr.tx_acm_drop_pkts);
	seq_printf(s, "  %-45s=%40llu\n", "a_mib.curr.tx_acm_drop_pkts_pae",
		   aggregate_mib[pos].curr.tx_acm_drop_pkts_pae);
	seq_printf(s, "  %-45s=%40llu\n", "a_mib.curr.tx_disc_pkts_redir",
		   aggregate_mib[pos].curr.tx_disc_pkts_redir);
	seq_printf(s, "  %-45s=%40llu\n", "a_mib.curr.tx_coll_pkts",
		   aggregate_mib[pos].curr.tx_coll_pkts);
	seq_printf(s, "  %-45s=%40llu\n", "a_mib.curr.tx_coll_pkts_pae",
		   aggregate_mib[pos].curr.tx_coll_pkts_pae);
	seq_printf(s, "  %-45s=%40llu\n", "a_mib.curr.tx_tmu_drop_pkts",
		   aggregate_mib[pos].curr.tx_tmu_drop_pkts);
	seq_printf(s, "  %-45s=%40llu\n",
		   "a_mib.curr.tx_tmu_csum_offload_bytes",
		   aggregate_mib[pos].curr.tx_tmu_csum_offload_bytes);
	seq_printf(s, "  %-45s=%40llu\n",
		   "a_mib.curr.tx_tmu_csum_offload_pkts",
		   aggregate_mib[pos].curr.tx_tmu_csum_offload_pkts);
	seq_printf(s, "  %-45s=%40llu\n", "a_mib.curr.tx_drv_drop_pkts",
		   aggregate_mib[pos].curr.tx_drv_drop_pkts);
	seq_printf(s, "  %-45s=%40llu\n", "a_mib.curr.tx_drv_error_pkts",
		   aggregate_mib[pos].curr.tx_drv_error_pkts);
	subif.port_id = pos;
	subif.subif = 0;

	if (dp_get_port_vap_mib_30(&subif, NULL, &net_mib, 0)) {
		seq_puts(s, "dp_get_port_vap_mib_30 failed\n");
		goto EXIT;
	}

	seq_printf(s, "  %-45s=%40llu\n", "aggregated rx_packets",
		   net_mib.rx_packets);
	seq_printf(s, "  %-45s=%40llu\n", "aggregated rx_dropped",
		   net_mib.rx_dropped);
	seq_printf(s, "  %-45s=%40llu\n", "aggregated tx_packets",
		   net_mib.tx_packets);
	seq_printf(s, "  %-45s=%40llu\n", "aggregated tx_dropped",
		   net_mib.tx_dropped);
	if (seq_has_overflowed(s)) /*buffer over flow and don't increase pos */
		return pos;

 EXIT:

	if ((pos >= PMAC_MAX_NUM - 1) || (pos >= proc_mib_port_end_id))
		return -1;	/*loop finished */

	pos++;
	return pos;
}

int proc_mib_inside_start(void)
{
	return proc_mib_port_start_id;
}

ssize_t proc_mib_inside_write(struct file *file, const char *buf,
			      size_t count, loff_t *ppos)
{
	int len;
	char str[64];
	char *s[2] = { 0 };
	unsigned int num[2] = { -1 };

	len = (sizeof(str) > count) ? count : sizeof(str) - 1;
	len -= copy_from_user(str, buf, len);
	str[len] = 0;

	if (dp_split_buffer(str, s, 2) < 2)
		goto help;

	if (s[0] && strlen(s[0]))
		num[0] = dp_atoi(s[0]);

	if (s[1] && strlen(s[1]))
		num[1] = dp_atoi(s[1]);

	set_start_end_id(num[0], num[1], PMAC_MAX_NUM - 1, PMAC_MAX_NUM - 1,
			 0, PMAC_MAX_NUM - 1, &proc_mib_port_start_id,
			 &proc_mib_port_end_id);
	pr_info("proc_mib_port_start_id=%u, proc_mib_port_end_id=%u\n",
		proc_mib_port_start_id, proc_mib_port_end_id);
	return count;

 help:
	pr_info("ussage echo start_id end_id > /proc/dp/mib_inside\n");
	pr_info("       then display the selected port info via cat %s\n",
		"/proc/dp/mib_inside");
	return count;
}

/*Note:
 *if (flag & DP_F_STATS_SUBIF), get sub-interface/vap mib only
 *otherwise, get physical port's mib
 */
int dp_get_port_vap_mib_30(dp_subif_t *subif, void *priv,
			   struct rtnl_link_stats64 *net_mib, u32 flag)
{
	dp_subif_t tmp_subif = {0};
	unsigned int port_id, vap;
	struct pmac_port_info *port_info;

	if (!subif || (subif->port_id < 0) ||
	    (subif->port_id >= PMAC_MAX_NUM) ||
	    !net_mib)
		return -1;
	spin_lock_bh(&dp_mib_lock);
	port_id = subif->port_id;
	port_info = get_dp_port_info(0, port_id);
	vap = GET_VAP(subif->subif, port_info->vap_offset, port_info->vap_mask);
	memset(net_mib, 0, sizeof(*net_mib));

	if ((flag & DP_F_STATS_SUBIF)) {	/*only sub-interface/VAP's
						 *mib mib only
						 */
		DP_DEBUG(DP_DBG_FLAG_MIB,
			 "%s for port.vap(%u.%u) with flag=0x%x\n",
			 "dp_get_port_vap_mib_30",
			 subif->port_id, vap, flag);
		if (update_vap_mib_lower_lvl(subif, flag))
			pr_err("%s failed for port %d VAP=%d\n",
			       "update_vap_mib_lower_lvl",
			       port_id, subif->subif);
		net_mib->rx_dropped =
		    aggregate_mib[port_id].curr_vap[vap].rx_disc_pkts_itf +
		    aggregate_mib[port_id].curr_vap[vap].rx_disc_pkts_drv;
		net_mib->rx_packets =
		    aggregate_mib[port_id].curr_vap[vap].rx_pkts_itf -
		    net_mib->rx_dropped;
		net_mib->tx_dropped =
		    aggregate_mib[port_id].curr_vap[vap].tx_disc_pkts_itf +
		    aggregate_mib[port_id].curr_vap[vap].tx_disc_pkts_tmu +
		    aggregate_mib[port_id].curr_vap[vap].tx_disc_pkts_mpe +
		    aggregate_mib[port_id].curr_vap[vap].tx_disc_pkts_drv;
		net_mib->tx_packets =
		    aggregate_mib[port_id].curr_vap[vap].tx_pkts_itf;
		spin_unlock_bh(&dp_mib_lock);
		return 0;
	}

	/*Get physical port's MIB only */
	DP_DEBUG(DP_DBG_FLAG_MIB,
		 "dp_get_port_vap_mib_30 for physical port %u w/ flag=0x%x\n",
		 port_id, flag);
	tmp_subif.port_id = port_id;
	tmp_subif.subif = 0;/*for dp_get_drv_mib to get all mib of all VAPs */

	if (update_port_mib_lower_lvl(&tmp_subif, 0))
		pr_err("update_port_mib_lower_lvl failed for port %d VAP=%d\n",
		       tmp_subif.port_id, tmp_subif.subif);
	if (port_id == 0) { /*not required by concept, for debugging only */
		struct mibs_port *aggregate_cpu;

		if (flag & DP_F_STATS_PAE_CPU)/*pae cpu*/
			aggregate_cpu = &aggregate_mib_r[0].curr;
		else /*gsw-l cpu */
			aggregate_cpu = &aggregate_mib[0].curr;
		net_mib->tx_errors =
		    aggregate_cpu->tx_coll_pkts;
		DP_DEBUG(DP_DBG_FLAG_MIB_ALGO,
			 "%02d: tx_errors(%llu)=tx_coll_pkts(%llu)\n",
			 port_id, net_mib->tx_errors,
			 aggregate_cpu->tx_coll_pkts);
		net_mib->tx_dropped =
		    aggregate_cpu->tx_drop_pkts +
		    aggregate_cpu->tx_acm_drop_pkts;
		DP_DEBUG(DP_DBG_FLAG_MIB_ALGO,
			 "%02d: tx_dropped(%llu)=tx_drop_pkts(%llu) + tx_acm_drop_pkts(%llu)\n",
			 port_id, net_mib->tx_dropped,
			 aggregate_cpu->tx_drop_pkts,
			 aggregate_cpu->tx_drv_drop_pkts);

		net_mib->tx_packets = aggregate_cpu->tx_good_pkts +
			net_mib->tx_dropped + net_mib->tx_errors;
		DP_DEBUG(DP_DBG_FLAG_MIB_ALGO,
			 "%02d: tx_packets(%llu)=tx_good_pkts(%llu) + tx_dropped(%llu) + tx_errors(%llu)\n",
			 port_id, net_mib->tx_packets,
			 aggregate_cpu->tx_good_pkts,
			 net_mib->tx_dropped, net_mib->tx_errors);

		net_mib->tx_bytes = aggregate_cpu->tx_good_bytes;
		DP_DEBUG(DP_DBG_FLAG_MIB_ALGO,
			 "%02d: tx_bytes(%llu)=tx_good_pkts(%llu)\n", port_id,
			 net_mib->tx_bytes,
			 aggregate_cpu->tx_good_bytes);

		/*rx mib */
		net_mib->rx_dropped =
		    aggregate_cpu->rx_drop_pkts +
		    aggregate_cpu->rx_filter_pkts +
		    aggregate_cpu->rx_drv_drop_pkts;
		DP_DEBUG(DP_DBG_FLAG_MIB_ALGO,
			 "%02d: rx_dropped(%llu)=rx_drop_pkts(%llu) + rx_filter_pkts(%llu) + aggregate_cpu->rx_drv_drop_pkts(%llu)\n",
			 port_id, net_mib->rx_dropped,
			 aggregate_cpu->rx_drop_pkts,
			 aggregate_cpu->rx_filter_pkts,
			 aggregate_cpu->rx_drv_drop_pkts);

		net_mib->rx_errors =
		    aggregate_cpu->rx_fcs_err_pkts +
		    aggregate_cpu->rx_undersize_good_pkts +
		    aggregate_cpu->rx_undersize_err_pkts +
		    aggregate_cpu->rx_oversize_good_pkts +
		    aggregate_cpu->rx_oversize_err_pkts;
		DP_DEBUG(DP_DBG_FLAG_MIB_ALGO,
			 "%02d: rx_errors(%llu)=rx_fcs_err_pkts(%llu) + rx_undersize_good_pkts(%llu) + rx_undersize_err_pkts(%llu) + rx_oversize_good_pkts(%llu) + rx_oversize_err_pkts(%llu)\n",
			 port_id, net_mib->rx_errors,
			 aggregate_cpu->rx_fcs_err_pkts,
			 aggregate_cpu->rx_undersize_good_pkts,
			 aggregate_cpu->rx_undersize_err_pkts,
			 aggregate_cpu->rx_oversize_good_pkts,
			 aggregate_cpu->rx_oversize_err_pkts);

		net_mib->rx_packets =
		    aggregate_cpu->rx_good_pkts +
		    net_mib->rx_dropped + net_mib->rx_errors;
		DP_DEBUG(DP_DBG_FLAG_MIB_ALGO,
			 "%02d: rx_packets(%llu)=rx_good_pkts(%llu) + rx_dropped(%llu) + rx_errors(%llu)\n",
			 port_id, net_mib->rx_packets,
			 aggregate_cpu->rx_good_pkts,
			 net_mib->rx_dropped, net_mib->rx_errors);

		net_mib->rx_bytes =
		    aggregate_cpu->rx_good_bytes +
		    aggregate_cpu->rx_bad_bytes;
		DP_DEBUG(DP_DBG_FLAG_MIB_ALGO,
			 "%02d: rx_bytes(%llu)=rx_good_bytes(%llu) + rx_bad_bytes(%llu)\n",
			 port_id, net_mib->rx_bytes,
			 aggregate_cpu->rx_good_bytes,
			 aggregate_cpu->rx_bad_bytes);
	} else if (port_id == WAN_EP) {
		/*tx mib */
		net_mib->tx_errors =
		    aggregate_mib[port_id].curr.tx_drv_error_pkts;
		DP_DEBUG(DP_DBG_FLAG_MIB_ALGO,
			 "%02d: tx_errors(%llu)=tx_drv_error_pkts(%llu)\n",
			 port_id, net_mib->tx_errors,
			 aggregate_mib[port_id].curr.tx_drv_error_pkts);
		net_mib->tx_dropped =
		    aggregate_mib[port_id].curr.tx_disc_pkts_redir +
		    aggregate_mib[port_id].curr.tx_tmu_drop_pkts +
		    aggregate_mib[port_id].curr.tx_mpe_drop_pkts +
		    aggregate_mib[port_id].curr.tx_drv_drop_pkts;
		DP_DEBUG(DP_DBG_FLAG_MIB_ALGO,
			 "%02d: tx_dropped(%llu)=tx_disc_pkts_redir(%llu) + tx_tmu_drop_pkts(%llu) + tx_mpe_drop_pkts(%llu) + tx_drv_drop_pkts(%llu)\n",
			 port_id, net_mib->tx_dropped,
			 aggregate_mib[port_id].curr.tx_disc_pkts_redir,
			 aggregate_mib[port_id].curr.tx_tmu_drop_pkts,
			 aggregate_mib[port_id].curr.tx_mpe_drop_pkts,
			 aggregate_mib[port_id].curr.tx_drv_drop_pkts);

		net_mib->tx_packets =
			aggregate_mib[port_id].curr.tx_pkts_redir +
			net_mib->tx_dropped -
			aggregate_mib[port_id].curr.tx_tmu_csum_offload_pkts;
		DP_DEBUG(DP_DBG_FLAG_MIB_ALGO,
			 "%02d: tx_packets(%llu)=tx_pkts_redir(%llu) + tx_dropped(%llu) - tx_tmu_csum_offload_pkts(%llu)\n",
			 port_id, net_mib->tx_packets,
			 aggregate_mib[port_id].curr.tx_pkts_redir,
			 net_mib->tx_dropped,
			 aggregate_mib[port_id].curr.tx_tmu_csum_offload_pkts);

		net_mib->tx_bytes = aggregate_mib[port_id].curr.tx_bytes_redir -
			aggregate_mib[port_id].curr.tx_tmu_csum_offload_bytes;
		DP_DEBUG(DP_DBG_FLAG_MIB_ALGO,
			 "%02d: tx_bytes(%llu)=tx_bytes_redir(%llu) - tx_tmu_csum_offload_bytes(%llu)\n",
			 port_id,
			 net_mib->tx_bytes,
			 aggregate_mib[port_id].curr.tx_bytes_redir,
			 aggregate_mib[port_id].curr.tx_tmu_csum_offload_bytes);

		/*rx mib */
		net_mib->rx_dropped =
		    aggregate_mib[port_id].curr.rx_drop_pkts +
		    aggregate_mib[port_id].curr.rx_filter_pkts +
		    aggregate_mib[port_id].curr.rx_drv_drop_pkts;
		DP_DEBUG(DP_DBG_FLAG_MIB_ALGO,
			 "%02d: rx_dropped(%llu)=rx_drop_pkts(%llu) + rx_filter_pkts(%llu) + rx_drv_drop_pkts(%llu)\n",
			 port_id, net_mib->rx_dropped,
			 aggregate_mib[port_id].curr.rx_drop_pkts,
			 aggregate_mib[port_id].curr.rx_filter_pkts,
			 aggregate_mib[port_id].curr.rx_drv_drop_pkts);

		/*ErrPktsCount = nRxFCSErrorPkts + nRxUnderSizeErrorPkts +
		 * nRxOverSizeErrorPkts + nRxAlignErrorPkts.
		 */
		net_mib->rx_errors =
		    aggregate_mib[port_id].curr.rx_fcs_err_pkts +
		    aggregate_mib[port_id].curr.rx_undersize_err_pkts +
		    aggregate_mib[port_id].curr.rx_oversize_err_pkts +
		    aggregate_mib[port_id].curr.rx_align_err_pkts +
		    aggregate_mib[port_id].curr.rx_drv_error_pkts;
		DP_DEBUG(DP_DBG_FLAG_MIB_ALGO,
			 "%02d: rx_errors(%llu)=rx_fcs_err_pkts(%llu) + rx_undersize_err_pkts(%llu) + rx_oversize_err_pkts(%llu) + rx_align_err_pkts(%llu) + rx_drv_error_pkts(%llu)\n",
			 port_id, net_mib->rx_errors,
			 aggregate_mib[port_id].curr.rx_fcs_err_pkts,
			 aggregate_mib[port_id].curr.rx_undersize_err_pkts,
			 aggregate_mib[port_id].curr.rx_oversize_err_pkts,
			 aggregate_mib[port_id].curr.rx_align_err_pkts,
			 aggregate_mib[port_id].curr.rx_drv_error_pkts);

		net_mib->rx_packets =
		    aggregate_mib[port_id].curr.rx_good_pkts +
		    net_mib->rx_dropped + net_mib->rx_errors +
		    aggregate_mib[port_id].curr.rx_drv_drop_pkts;
		DP_DEBUG(DP_DBG_FLAG_MIB_ALGO,
			 "%02d: rx_packets(%llu)=rx_good_pkts(%llu) + rx_dropped(%llu) + rx_errors(%llu) + rx_drv_drop_pkts(%llu)\n",
			 port_id, net_mib->rx_packets,
			 aggregate_mib[port_id].curr.rx_good_pkts,
			 net_mib->rx_dropped, net_mib->rx_errors,
			 aggregate_mib[port_id].curr.rx_drv_drop_pkts);

		net_mib->rx_bytes =
		    aggregate_mib[port_id].curr.rx_good_bytes +
		    aggregate_mib[port_id].curr.rx_bad_bytes;
		DP_DEBUG(DP_DBG_FLAG_MIB_ALGO,
			 "%02d: rx_bytes(%llu)=rx_good_bytes(%llu) + rx_bad_bytes(%llu)\n",
			 port_id, net_mib->rx_bytes,
			 aggregate_mib[port_id].curr.rx_good_bytes,
			 aggregate_mib[port_id].curr.rx_bad_bytes);
	} else if (port_id < PAMC_LAN_MAX_NUM) {
		/*tx mib */
		net_mib->tx_errors =
		    aggregate_mib[port_id].curr.tx_coll_pkts +
		    aggregate_mib[port_id].curr.tx_coll_pkts_pae +
		    aggregate_mib[port_id].curr.tx_drv_error_pkts;
		DP_DEBUG(DP_DBG_FLAG_MIB_ALGO,
			 "%02d: tx_errors(%llu)=tx_coll_pkts(%llu) + tx_coll_pkts_pae(%llu) + tx_drv_error_pkts(%llu)\n",
			 port_id, net_mib->tx_errors,
			 aggregate_mib[port_id].curr.tx_coll_pkts,
			 aggregate_mib[port_id].curr.tx_coll_pkts_pae,
			 aggregate_mib[port_id].curr.tx_drv_error_pkts);

		net_mib->tx_dropped =
		    aggregate_mib[port_id].curr.tx_drop_pkts +
		    aggregate_mib[port_id].curr.tx_acm_drop_pkts +
		    aggregate_mib[port_id].curr.tx_drop_pkts_pae +
		    aggregate_mib[port_id].curr.tx_tmu_drop_pkts +
		    aggregate_mib[port_id].curr.tx_mpe_drop_pkts +
		    aggregate_mib[port_id].curr.tx_drv_drop_pkts;
		DP_DEBUG(DP_DBG_FLAG_MIB_ALGO,
			 "%02d: tx_dropped(%llu)=tx_drop_pkts(%llu) + tx_acm_drop_pkts(%llu) + tx_drop_pkts_pae(%llu) + tx_tmu_drop_pkts(%llu) + tx_tmu_drop_pkts(%llu)+ tx_drv_drop_pkts(%llu)\n",
			 port_id, net_mib->tx_dropped,
			 aggregate_mib[port_id].curr.tx_drop_pkts,
			 aggregate_mib[port_id].curr.tx_acm_drop_pkts,
			 aggregate_mib[port_id].curr.tx_drop_pkts_pae,
			 aggregate_mib[port_id].curr.tx_tmu_drop_pkts,
			 aggregate_mib[port_id].curr.tx_mpe_drop_pkts,
			 aggregate_mib[port_id].curr.tx_drv_drop_pkts);

		net_mib->tx_packets =
		    aggregate_mib[port_id].curr.tx_good_pkts +
		    net_mib->tx_dropped + net_mib->tx_errors;
		DP_DEBUG(DP_DBG_FLAG_MIB_ALGO,
			 "%02d: tx_packets(%llu)=tx_good_pkts(%llu) + tx_dropped(%llu) + tx_errors(%llu)\n",
			 port_id, net_mib->tx_packets,
			 aggregate_mib[port_id].curr.tx_good_pkts,
			 net_mib->tx_dropped, net_mib->tx_errors);

		net_mib->tx_bytes = aggregate_mib[port_id].curr.tx_good_bytes;
		DP_DEBUG(DP_DBG_FLAG_MIB_ALGO,
			 "%02d: tx_bytes(%llu)=tx_good_bytes(%llu)\n",
			 port_id, net_mib->tx_bytes,
			 aggregate_mib[port_id].curr.tx_good_bytes);

		/*rx mib */
		net_mib->rx_dropped =
		    aggregate_mib[port_id].curr.rx_drop_pkts +
		    aggregate_mib[port_id].curr.rx_filter_pkts +
		    aggregate_mib[port_id].curr.rx_drop_pkts_pae +
		    aggregate_mib[port_id].curr.rx_drv_drop_pkts;
		DP_DEBUG(DP_DBG_FLAG_MIB_ALGO,
			 "%02d: rx_dropped(%llu)=rx_drop_pkts(%llu) + rx_filter_pkts(%llu) + rx_drop_pkts_pae(%llu) + rx_drv_drop_pkts(%llu)\n",
			 port_id, net_mib->rx_dropped,
			 aggregate_mib[port_id].curr.rx_drop_pkts,
			 aggregate_mib[port_id].curr.rx_filter_pkts,
			 aggregate_mib[port_id].curr.rx_drop_pkts_pae,
			 aggregate_mib[port_id].curr.rx_drv_drop_pkts);

		/*ErrPktsCount = nRxFCSErrorPkts + nRxUnderSizeErrorPkts +
		 * nRxOverSizeErrorPkts + nRxAlignErrorPkts
		 */
		net_mib->rx_errors =
		    aggregate_mib[port_id].curr.rx_fcs_err_pkts +
		    aggregate_mib[port_id].curr.rx_undersize_err_pkts +
		    aggregate_mib[port_id].curr.rx_oversize_err_pkts +
		    aggregate_mib[port_id].curr.rx_align_err_pkts +
		    aggregate_mib[port_id].curr.rx_fcs_err_pkts_pae +
		    aggregate_mib[port_id].curr.rx_undersize_err_pkts_pae +
		    aggregate_mib[port_id].curr.rx_oversize_err_pkts_pae +
		    aggregate_mib[port_id].curr.rx_align_err_pkts_pae +
		    aggregate_mib[port_id].curr.rx_drv_error_pkts;
		DP_DEBUG(DP_DBG_FLAG_MIB_ALGO,
			 "%02d: rx_errors(%llu)=rx_fcs_err_pkts(%llu) + rx_undersize_err_pkts(%llu) + rx_oversize_err_pkts(%llu) + rx_align_err_pkts(%llu) + rx_fcs_err_pkts_pae(%llu) + rx_undersize_err_pkts_pae(%llu) + rx_oversize_err_pkts_pae(%llu) + rx_align_err_pkts_pae(%llu)+  rx_drv_error_pkts(%llu)\n",
			 port_id, net_mib->rx_errors,
			 aggregate_mib[port_id].curr.rx_fcs_err_pkts,
			 aggregate_mib[port_id].curr.rx_undersize_err_pkts,
			 aggregate_mib[port_id].curr.rx_oversize_err_pkts,
			 aggregate_mib[port_id].curr.rx_align_err_pkts,
			 aggregate_mib[port_id].curr.rx_fcs_err_pkts_pae,
			 aggregate_mib[port_id].curr.rx_undersize_err_pkts_pae,
			 aggregate_mib[port_id].curr.rx_oversize_err_pkts_pae,
			 aggregate_mib[port_id].curr.rx_align_err_pkts_pae,
			 aggregate_mib[port_id].curr.rx_drv_error_pkts);

		net_mib->rx_packets =
		    aggregate_mib[port_id].curr.rx_good_pkts +
		    net_mib->rx_dropped + net_mib->rx_errors;
		DP_DEBUG(DP_DBG_FLAG_MIB_ALGO,
			 "%02d: rx_packets(%llu)=rx_good_pkts(%llu) + rx_dropped(%llu) + rx_errors(%llu)\n",
			 port_id, net_mib->rx_packets,
			 aggregate_mib[port_id].curr.rx_good_pkts,
			 net_mib->rx_dropped, net_mib->rx_errors);

		net_mib->rx_bytes =
		    aggregate_mib[port_id].curr.rx_good_bytes +
		    aggregate_mib[port_id].curr.rx_bad_bytes;
		DP_DEBUG(DP_DBG_FLAG_MIB_ALGO,
			 "%02d: rx_bytes(%llu)=rx_good_bytes(%llu) + rx_bad_bytes(%llu)\n",
			 port_id, net_mib->rx_bytes,
			 aggregate_mib[port_id].curr.rx_good_bytes,
			 aggregate_mib[port_id].curr.rx_bad_bytes);
	} else if (!(port_info->alloc_flags &
		   DP_F_FAST_DSL)) {	/*WIFI/LTE Directpath */
		/*tx mib */
		net_mib->tx_errors =
		    aggregate_mib[port_id].curr.tx_drv_error_pkts +
		    aggregate_mib[port_id].curr.tx_coll_pkts;
		DP_DEBUG(DP_DBG_FLAG_MIB_ALGO,
			 "%02d: tx_errors(%llu)=tx_drv_error_pkts(%llu) + tx_coll_pkts(%llu)\n",
			 port_id, net_mib->tx_errors,
			 aggregate_mib[port_id].curr.tx_drv_error_pkts,
			 aggregate_mib[port_id].curr.tx_coll_pkts);

		net_mib->tx_dropped =
		    aggregate_mib[port_id].curr.tx_drop_pkts +
		    aggregate_mib[port_id].curr.tx_acm_drop_pkts +
		    aggregate_mib[port_id].curr.tx_tmu_drop_pkts +
		    aggregate_mib[port_id].curr.tx_mpe_drop_pkts +
		    aggregate_mib[port_id].curr.tx_drv_drop_pkts;
		DP_DEBUG(DP_DBG_FLAG_MIB_ALGO,
			 "%02d: tx_dropped(%llu)=tx_drop_pkts(%llu) + tx_acm_drop_pkts(%llu) + tx_tmu_drop_pkts(%llu) + tx_mpe_drop_pkts(%llu) + tx_drv_drop_pkts(%llu)\n",
			 port_id, net_mib->tx_dropped,
			 aggregate_mib[port_id].curr.tx_drop_pkts,
			 aggregate_mib[port_id].curr.tx_acm_drop_pkts,
			 aggregate_mib[port_id].curr.tx_tmu_drop_pkts,
			 aggregate_mib[port_id].curr.tx_mpe_drop_pkts,
			 aggregate_mib[port_id].curr.tx_drv_drop_pkts);

		net_mib->tx_packets =
		    aggregate_mib[port_id].curr.tx_good_pkts +
		    net_mib->tx_dropped + net_mib->tx_errors;
		DP_DEBUG(DP_DBG_FLAG_MIB_ALGO,
			 "%02d: tx_packets(%llu)=tx_good_pkts(%llu) + tx_dropped(%llu) + tx_errors(%llu)\n",
			 port_id, net_mib->tx_packets,
			 aggregate_mib[port_id].curr.tx_good_pkts,
			 net_mib->tx_dropped, net_mib->tx_errors);

		net_mib->tx_bytes = aggregate_mib[port_id].curr.tx_good_bytes;
		DP_DEBUG(DP_DBG_FLAG_MIB_ALGO,
			 "%02d: tx_bytes(%llu)=tx_good_bytes(%llu)\n",
			 port_id, net_mib->tx_bytes,
			 aggregate_mib[port_id].curr.tx_good_bytes);

		/*rx mib */
		net_mib->rx_dropped =
		    aggregate_mib[port_id].curr.rx_drop_pkts +
		    aggregate_mib[port_id].curr.rx_filter_pkts +
		    aggregate_mib[port_id].curr.rx_drv_drop_pkts;
		DP_DEBUG(DP_DBG_FLAG_MIB_ALGO,
			 "%02d: rx_dropped(%llu)=rx_drop_pkts(%llu) + rx_filter_pkts(%llu) + rx_drv_drop_pkts(%llu)\n",
			 port_id, net_mib->rx_dropped,
			 aggregate_mib[port_id].curr.rx_drop_pkts,
			 aggregate_mib[port_id].curr.rx_filter_pkts,
			 aggregate_mib[port_id].curr.rx_drv_drop_pkts);

		/*ErrPktsCount = nRxFCSErrorPkts + nRxUnderSizeErrorPkts +
		 * nRxOverSizeErrorPkts + nRxAlignErrorPkts
		 */
		net_mib->rx_errors =
			aggregate_mib[port_id].curr.rx_fcs_err_pkts +
			aggregate_mib[port_id].curr.rx_undersize_err_pkts +
			aggregate_mib[port_id].curr.rx_oversize_err_pkts +
			aggregate_mib[port_id].curr.rx_align_err_pkts +
			aggregate_mib[port_id].curr.rx_drv_error_pkts;
		DP_DEBUG(DP_DBG_FLAG_MIB_ALGO,
			 "%02d: rx_errors(%llu)=rx_fcs_err_pkts(%llu) + rx_undersize_err_pkts(%llu) + rx_oversize_err_pkts(%llu) + rx_align_err_pkts(%llu)  + rx_drv_error_pkts(%llu)\n",
			 port_id, net_mib->rx_errors,
			 aggregate_mib[port_id].curr.rx_fcs_err_pkts,
			 aggregate_mib[port_id].curr.rx_undersize_err_pkts,
			 aggregate_mib[port_id].curr.rx_oversize_err_pkts,
			 aggregate_mib[port_id].curr.rx_align_err_pkts,
			 aggregate_mib[port_id].curr.rx_drv_error_pkts);

		net_mib->rx_packets =
			aggregate_mib[port_id].curr.rx_good_pkts +
			net_mib->rx_dropped + net_mib->rx_errors;
		DP_DEBUG(DP_DBG_FLAG_MIB_ALGO,
			 "%02d: rx_packets(%llu)=rx_good_pkts(%llu) + rx_dropped(%llu) + rx_errors(%llu)\n",
			 port_id, net_mib->rx_packets,
			 aggregate_mib[port_id].curr.rx_good_pkts,
			 net_mib->rx_dropped, net_mib->rx_errors);

		net_mib->rx_bytes =
		    aggregate_mib[port_id].curr.rx_good_bytes +
		    aggregate_mib[port_id].curr.rx_bad_bytes;
		DP_DEBUG(DP_DBG_FLAG_MIB_ALGO,
			 "%02d: rx_bytes(%llu)=rx_good_bytes(%llu) + rx_bad_bytes(%llu)\n",
			 port_id, net_mib->rx_bytes,
			 aggregate_mib[port_id].curr.rx_good_bytes,
			 aggregate_mib[port_id].curr.rx_bad_bytes);
	} else {		/*DSL */
		/*Here datapath follow PTM algo. For ATM it
		 * should be handled by VRX518 directly
		 */
		net_mib->tx_errors =
		    aggregate_mib[port_id].curr.tx_drv_error_pkts +
		    aggregate_mib[port_id].curr.tx_coll_pkts;
		DP_DEBUG(DP_DBG_FLAG_MIB_ALGO,
			 "%02d: tx_errors(%llu)=tx_drv_error_pkts(%llu) + tx_coll_pkts(%llu)\n",
			 port_id, net_mib->tx_errors,
			 aggregate_mib[port_id].curr.tx_drv_error_pkts,
			 aggregate_mib[port_id].curr.tx_coll_pkts);

		net_mib->tx_dropped =
		    aggregate_mib[port_id].curr.tx_drop_pkts +
		    aggregate_mib[port_id].curr.tx_acm_drop_pkts +
		    aggregate_mib[port_id].curr.tx_tmu_drop_pkts +
		    aggregate_mib[port_id].curr.tx_mpe_drop_pkts +
		    aggregate_mib[port_id].curr.tx_drv_drop_pkts;
		DP_DEBUG(DP_DBG_FLAG_MIB_ALGO,
			 "%02d: tx_dropped(%llu)=tx_drop_pkts(%llu) + tx_acm_drop_pkts(%llu) + tx_tmu_drop_pkts(%llu) + tx_mpe_drop_pkts(%llu) +tx_drv_drop_pkts(%llu)\n",
			 port_id, net_mib->tx_dropped,
			 aggregate_mib[port_id].curr.tx_drop_pkts,
			 aggregate_mib[port_id].curr.tx_acm_drop_pkts,
			 aggregate_mib[port_id].curr.tx_tmu_drop_pkts,
			 aggregate_mib[port_id].curr.tx_mpe_drop_pkts,
			 aggregate_mib[port_id].curr.tx_drv_drop_pkts);

		net_mib->tx_packets =
		    aggregate_mib[port_id].curr.tx_good_pkts +
		    net_mib->tx_dropped + net_mib->tx_errors;
		DP_DEBUG(DP_DBG_FLAG_MIB_ALGO,
			 "%02d: tx_packets(%llu)=tx_good_pkts(%llu) + tx_dropped(%llu) tx_errors(%llu)\n",
			 port_id, net_mib->tx_packets,
			 aggregate_mib[port_id].curr.tx_good_pkts,
			 net_mib->tx_dropped, net_mib->tx_errors);

		net_mib->tx_bytes = aggregate_mib[port_id].curr.tx_good_bytes;
		DP_DEBUG(DP_DBG_FLAG_MIB_ALGO,
			 "%02d: tx_bytes(%llu)=tx_good_bytes(%llu)\n",
			 port_id, net_mib->tx_bytes,
			 aggregate_mib[port_id].curr.tx_good_bytes);

		/*rx mib */
		net_mib->rx_dropped =
		    aggregate_mib[port_id].curr.rx_drop_pkts +
		    aggregate_mib[port_id].curr.rx_filter_pkts +
		    aggregate_mib[port_id].curr.rx_drv_drop_pkts;
		DP_DEBUG(DP_DBG_FLAG_MIB_ALGO,
			 "%02d: rx_dropped(%llu)=rx_drop_pkts(%llu) + rx_filter_pkts(%llu) + rx_drv_drop_pkts(%llu)\n",
			 port_id, net_mib->rx_dropped,
			 aggregate_mib[port_id].curr.rx_drop_pkts,
			 aggregate_mib[port_id].curr.rx_filter_pkts,
			 aggregate_mib[port_id].curr.rx_drv_drop_pkts);

		/*ErrPktsCount = nRxFCSErrorPkts + nRxUnderSizeErrorPkts +
		 * nRxOverSizeErrorPkts + nRxAlignErrorPkts
		 */
		net_mib->rx_errors =
		    aggregate_mib[port_id].curr.rx_fcs_err_pkts +
		    aggregate_mib[port_id].curr.rx_undersize_err_pkts +
		    aggregate_mib[port_id].curr.rx_oversize_err_pkts +
		    aggregate_mib[port_id].curr.rx_align_err_pkts_pae +
		    aggregate_mib[port_id].curr.rx_drv_error_pkts;
		DP_DEBUG(DP_DBG_FLAG_MIB_ALGO,
			 "%02d: rx_errors(%llu)=rx_fcs_err_pkts(%llu) + rx_undersize_err_pkts(%llu) + rx_oversize_err_pkts(%llu) + rx_align_err_pkts_pae(%llu) + rx_drv_error_pkts(%llu)\n",
			 port_id, net_mib->rx_errors,
			 aggregate_mib[port_id].curr.rx_fcs_err_pkts,
			 aggregate_mib[port_id].curr.rx_undersize_err_pkts,
			 aggregate_mib[port_id].curr.rx_oversize_err_pkts,
			 aggregate_mib[port_id].curr.rx_align_err_pkts_pae,
			 aggregate_mib[port_id].curr.rx_drv_error_pkts);

		net_mib->rx_packets =
		    aggregate_mib[port_id].curr.rx_good_pkts +
		    net_mib->rx_dropped + net_mib->rx_errors;
		DP_DEBUG(DP_DBG_FLAG_MIB_ALGO,
			 "%02d: rx_packets(%llu)=rx_good_pkts(%llu) + rx_dropped(%llu) + rx_errors(%llu)\n",
			 port_id, net_mib->rx_packets,
			 aggregate_mib[port_id].curr.rx_good_pkts,
			 net_mib->rx_dropped, net_mib->rx_errors);

		net_mib->rx_bytes =
		    aggregate_mib[port_id].curr.rx_good_bytes +
		    aggregate_mib[port_id].curr.rx_bad_bytes;
		DP_DEBUG(DP_DBG_FLAG_MIB_ALGO,
			 "%02d: rx_bytes(%llu)=rx_good_bytes(%llu) + rx_bad_bytes(%llu)\n",
			 port_id, net_mib->rx_bytes,
			 aggregate_mib[port_id].curr.rx_good_bytes,
			 aggregate_mib[port_id].curr.rx_bad_bytes);
	}
	spin_unlock_bh(&dp_mib_lock);
	return 0;
}

/*Clear GSW Interface MIB: only for sub interface/vap only  */
int clear_gsw_itf_mib(dp_subif_t *subif, u32 flag)
{
	int start, end;
	GSW_RMON_clear_t rmon;
	int i;
	struct pmac_port_info *port_info;
	struct core_ops *gsw_handle;

	gsw_handle = dp_port_prop[0].ops[GSWIP_R];
	if (!(flag & DP_F_STATS_SUBIF))
		return 0;
	if (!subif) { /* clear all */
		start = 0;
		end = MAX_RMON_ITF;
	} else if ((subif->port_id < 0) || (subif->port_id >= PMAC_MAX_NUM)) {
		DP_DEBUG(DP_DBG_FLAG_MIB, "wrong port_id %d\n",
			 subif->port_id);
		return -1;
	}

	if (subif) {
		port_info = get_dp_port_info(0, subif->port_id);
		if (!port_info || !port_info->itf_info)
			return 0;
		start = port_info->itf_info->start +
			GET_VAP(subif->subif, port_info->vap_offset,
				port_info->vap_mask);
		end = start + 1;
	}
	rmon.eRmonType = GSW_RMON_IF_TYPE;
	for (i = start; i < end; i++) {
		rmon.nRmonId = i;
		if (rmon.nRmonId >= MAX_RMON_ITF) {
			pr_err("Why Port[%d]'s interface ID %d so big\n",
			       subif ? subif->port_id : -1,
			       rmon.nRmonId);
			return -1;
		}
		gsw_handle->gsw_rmon_ops.RMON_Clear(gsw_handle, &rmon);
	}
	return 0;
}

int dp_clear_netif_mib_30(dp_subif_t *subif, void *priv, u32 flag)
{
	unsigned int port_id, vap;
	GSW_RMON_clear_t rmon;
	int i;
	dp_subif_t tmp_subif;
	struct core_ops *gsw_l, *gsw_r;
	struct pmac_port_info *pi;

	gsw_l = dp_port_prop[0].ops[GSWIP_L];
	gsw_r = dp_port_prop[0].ops[GSWIP_R];

	if (!subif) { /*clear all */
		spin_lock_bh(&dp_mib_lock);
		gsw_mib_reset_30(0, 0);
		gsw_mib_reset_30(1, 0);
		tmu_reset_mib_all(flag);
		dp_clear_all_mib_inside(flag);
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_MIB_TMU_MPE_MIB)
		tmu_hal_clear_qos_m_local =
			tmu_hal_clear_qos_mib_hook_fn;
		if (tmu_hal_clear_qos_m_local)
			tmu_hal_clear_qos_m_local(NULL, NULL, -1, 0);
		mpe_hal_clear_if_m_local =
			mpe_hal_clear_if_mib_hook_fn;
		if (mpe_hal_clear_if_m_local)
			mpe_hal_clear_if_m_local(NULL, NULL, 0);
		tmu_hal_clear_csum_ol_m_local =
			tmu_hal_clear_csum_ol_mib_hook_fn;
		if (tmu_hal_clear_csum_ol_m_local)
			tmu_hal_clear_csum_ol_m_local(0);
#endif
		memset(aggregate_mib, 0, sizeof(aggregate_mib));
		memset(aggregate_mib_r, 0, sizeof(aggregate_mib_r));
		memset(&last, 0, sizeof(last));
		memset(&last_vap, 0, sizeof(last_vap));
		spin_unlock_bh(&dp_mib_lock);
		return 0;
	}

	if ((subif->port_id <= 0) ||
	    subif->port_id >= PMAC_MAX_NUM)
		return -1;

	port_id = subif->port_id;
	pi = get_dp_port_info(0, port_id);
	vap = GET_VAP(subif->subif, pi->vap_offset, pi->vap_mask);
	if ((flag & DP_F_STATS_SUBIF)) {
		spin_lock_bh(&dp_mib_lock);
		/*clear the specific subif mib counter */
		clear_gsw_itf_mib(subif, flag);
		dp_clear_mib(subif, 0);

		/*workaround since this VAP's mib is cleared*/
		DP_DEBUG(DP_DBG_FLAG_MIB, "Clear aggregate_mib: %d/%d\n",
			 port_id, vap);
		if (aggregate_mib[port_id].curr_vap[vap].tx_disc_pkts_tmu <=
		    aggregate_mib[port_id].curr.tx_tmu_drop_pkts)
			aggregate_mib[port_id].curr.tx_tmu_drop_pkts -=
			 aggregate_mib[port_id].curr_vap[vap].tx_disc_pkts_tmu;
		if (aggregate_mib[port_id].curr_vap[vap].tx_disc_pkts_mpe <=
		    aggregate_mib[port_id].curr.tx_mpe_drop_pkts)
			aggregate_mib[port_id].curr.tx_mpe_drop_pkts -=
			aggregate_mib[port_id].curr_vap[vap].tx_disc_pkts_mpe;
		memset(&aggregate_mib[port_id].curr_vap[vap], 0,
		       sizeof(aggregate_mib[port_id].curr_vap[vap]));
		memset(&last_vap[port_id][vap], 0,
		       sizeof(last_vap[port_id][vap]));
		/*how about last[] & last_vap[]*/
		spin_unlock_bh(&dp_mib_lock);
		return 0;
	}
	spin_lock_bh(&dp_mib_lock);
	/*Clear port based RMON mib */
	DP_DEBUG(DP_DBG_FLAG_MIB,
		 "dp_clear_netif_mib port %u mib flag=0x%x\n", port_id, flag);

	/*First we delete all its VAP mib counter first since TMU/MPE port mib
	 * counter is based on its VAP's
	 */
	tmp_subif = *subif;
	for (i = 0; i < MAX_SUBIF_PER_PORT; i++) {
		pi = get_dp_port_info(0, tmp_subif.port_id);
		DP_DEBUG(DP_DBG_FLAG_MIB, "dp_clear_netif_mib: %d/%d\n",
			 tmp_subif.port_id, i);
		tmp_subif.subif =
			SET_VAP(i, pi->vap_offset, pi->vap_mask);
		dp_clear_netif_mib_30(&tmp_subif, NULL, DP_F_STATS_SUBIF);
	}

	memset(&aggregate_mib[port_id], 0, sizeof(aggregate_mib[port_id]));
	memset(&last[port_id], 0, sizeof(last[port_id]));

	if (port_id == WAN_EP) {
		/*reset GSWIP-R rmon counters */
		rmon.eRmonType = GSW_RMON_PORT_TYPE;
		rmon.nRmonId = port_id;
		gsw_r->gsw_rmon_ops.RMON_Clear(gsw_r, &rmon);

		/*reset GSWIP-R redirect counters */
		rmon.eRmonType = GSW_RMON_REDIRECT_TYPE;
		rmon.nRmonId = 0;
		gsw_r->gsw_rmon_ops.RMON_Clear(gsw_r, &rmon);

#if IS_ENABLED(CONFIG_INTEL_DATAPATH_MIB_TMU_MPE_MIB)
		tmu_hal_clear_csum_ol_m_local =
			tmu_hal_clear_csum_ol_mib_hook_fn;
		if (tmu_hal_clear_csum_ol_m_local)
			tmu_hal_clear_csum_ol_m_local(0);
#endif
		/*how about last[] & last_vap[] */
	} else if (port_id < PAMC_LAN_MAX_NUM) {
		/*reset GSWIP-L/R rmon counters */
		rmon.eRmonType = GSW_RMON_PORT_TYPE;
		rmon.nRmonId = port_id;
		gsw_l->gsw_rmon_ops.RMON_Clear(gsw_l, &rmon);
		gsw_r->gsw_rmon_ops.RMON_Clear(gsw_r, &rmon);
	} else {		/*port 7 ~ 14 */
		rmon.eRmonType = GSW_RMON_PORT_TYPE;
		rmon.nRmonId = port_id;
		gsw_r->gsw_rmon_ops.RMON_Clear(gsw_r, &rmon);
	}
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_MIB_TMU_MPE_MIB)
	tmu_hal_clear_qos_m_local = tmu_hal_clear_qos_mib_hook_fn;
	if (tmu_hal_clear_qos_m_local)
		tmu_hal_clear_qos_m_local(NULL, subif, -1, flag);
	mpe_hal_clear_if_m_local =
		mpe_hal_clear_if_m_local;
	if (mpe_hal_clear_if_m_local)
		mpe_hal_clear_if_m_local(NULL, subif, flag);
#endif

	/*reset datapath's all vap mib */
	subif->subif = -1;
	dp_clear_mib(subif, 0);

	/*reset GSW interface mib */
	subif->subif = -1;
	clear_gsw_itf_mib(subif, 0);
	memset(&last[port_id], 0, sizeof(last[port_id]));
	spin_unlock_bh(&dp_mib_lock);
	return 0;
}

int proc_mib_port_start(void)
{
	return 0;
}

int proc_mib_port_dump(struct seq_file *s, int pos)
{
	int i = 0;
	dp_subif_t subif;
	struct rtnl_link_stats64 stats_mib = {0};

	if (pos == 0) {
		memset(&subif, 0, sizeof(subif));
		seq_printf(s, "%-12s %20s %20s %20s %20s\n", "PORT",
			   "Tx_Pkts/", "Tx_Bytes/", "Tx_Drop_Pkts/",
			   "Tx_Err_Pkts/");
		seq_printf(s, "%-12s %20s %20s %20s %20s\n", "    ",
			   "Rx_Pkts", "Rx_Bytes", "Rx_Drop_Pkts",
			   "Rx_Err_Pkts");
		seq_puts(s, "-------------------------------------------------------------------------------------------------------\n");
	}

	i = pos;
	subif.port_id = i;
	if (dp_get_port_vap_mib_30(&subif, NULL, &stats_mib, 0) == 0) {
		if (pos == 0)
			seq_printf(s, "%-7d(L  ) %20llu %20llu %20llu %20llu\n",
				   subif.port_id,
				   stats_mib.tx_packets,
				   stats_mib.tx_bytes,
				   stats_mib.tx_dropped,
				   stats_mib.tx_errors);
		else
			seq_printf(s, "%-12d %20llu %20llu %20llu %20llu\n",
				   subif.port_id,
				   stats_mib.tx_packets,
				   stats_mib.tx_bytes,
				   stats_mib.tx_dropped,
				   stats_mib.tx_errors);
		seq_printf(s, "             %20llu %20llu %20llu %20llu\n",
			   stats_mib.rx_packets,
			   stats_mib.rx_bytes,
			   stats_mib.rx_dropped,
			   stats_mib.rx_errors);
		seq_puts(s, "\n");
	}

	if ((pos == 0) &&
	    (dp_get_port_vap_mib_30(&subif, NULL, &stats_mib,
	    DP_F_STATS_PAE_CPU) == 0)) {
		seq_printf(s, "%-7d(pae) %20llu %20llu %20llu %20llu\n",
			   subif.port_id,
			   stats_mib.tx_packets,
			   stats_mib.tx_bytes,
			   stats_mib.tx_dropped,
			   stats_mib.tx_errors);
		seq_printf(s, "             %20llu %20llu %20llu %20llu\n",
			   stats_mib.rx_packets,
			   stats_mib.rx_bytes,
			   stats_mib.rx_dropped,
			   stats_mib.rx_errors);
		seq_puts(s, "\n");
	}

	pos++;

	if (pos > (PMAC_MAX_NUM - 1)) {
		pos = -1;	/*end of the loop */
		/* check GSWIP whether redirect enabled or not */
		if (!gsw_eth_wan_redirect_status())
			seq_puts(s, "Note: Ethernet WAN redirect is not enabled. Its MIB algo not work\n");
	}

	return pos;
}

ssize_t proc_mib_port_write(struct file *file, const char *buf, size_t count,
			    loff_t *ppos)
{
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_MIB_TMU_MPE_MIB)
	int len;
	char str[64];
	int i, num, res;
	char *param_list[10];
	struct pmac_port_info *port;
	dp_subif_t tmp;
	static struct tmu_hal_qos_stats qos_stats;

	len = (sizeof(str) > count) ? count : sizeof(str) - 1;
	len -= copy_from_user(str, buf, len);
	str[len] = 0;
	num = dp_split_buffer(str, param_list, ARRAY_SIZE(param_list));

	if (num < 1)
		goto help;
	memset(&qos_stats, 0, sizeof(qos_stats));
	if (dp_strncmpi(param_list[0], "qos_mib", strlen("qos_mib")) == 0) {
		s32 port_list[] = {15, 2, 3, 4, 5, 7, 8};
		int k;

		tmu_hal_get_qos_m_local =
			tmu_hal_get_qos_mib_hook_fn;
		if (!tmu_hal_get_qos_m_local) {
			pr_info("tmu_hal_get_qos_mib_hook_fn NULL\n");
			return count;
		}
		for (k = 0; k < ARRAY_SIZE(port_list); k++) {
			tmp.port_id = port_list[k];
			port = get_dp_port_info(0, tmp.port_id);
			if (!port)
				continue;
			for (i = 0; i < MAX_SUBIF_PER_PORT; i++) {
				if (!get_dp_port_subif(port, i)->flags)
					continue;
				tmp.subif = SET_VAP(i, port->vap_offset,
						    port->vap_mask);
				res = tmu_hal_get_qos_m_local(NULL,
							      &tmp,
							      -1,
							      &qos_stats,
							      0);
				if (res) {
					pr_info("%s failed for p[%d]\n",
						"tmu_hal_get_qos_mib_hook_fn",
						tmp.port_id);
					continue;
				}
				pr_info("%s[%d/%d]:drop_pkts=%llu bytes=%llu\n",
					"qos_mib", tmp.port_id,
					GET_VAP(tmp.subif, port->vap_offset,
						port->vap_mask),
					qos_stats.dropPkts,
					qos_stats.dropBytes);
				pr_info("%s[%d/%d]:enqPkts=%llu Bytes=%llu\n",
					"qos_mib", tmp.port_id,
					GET_VAP(tmp.subif, port->vap_offset,
						port->vap_mask),
				       qos_stats.enqPkts,
				       qos_stats.enqBytes);
				pr_info("%s[%d/%d]:deqPkts=%llu Bytes=%llu\n",
					"qos_mib", tmp.port_id,
					GET_VAP(tmp.subif, port->vap_offset,
						port->vap_mask),
					qos_stats.deqPkts,
					qos_stats.deqBytes);
			}
			pr_info("\n");
		}
	} else if (dp_strncmpi(param_list[0],
			"csum_mib",
			strlen("csum_mib")) == 0) {
		tmu_hal_get_csum_ol_m_local =
			tmu_hal_get_csum_ol_mib_hook_fn;
		if (!tmu_hal_get_csum_ol_m_local) {
			pr_info("tmu_hal_get_csum_ol_mib_hook_fn NULL\n");
			return count;
		}
		res = tmu_hal_get_csum_ol_m_local(&qos_stats, 0);
		if (res)
			pr_info("tmu_hal_get_csum_ol_mib_hook_fn failed\n");
		else
			pr_info("csum_mib: deqPkts=%llu  deqBytes=%llu\n",
				qos_stats.deqPkts,
				qos_stats.deqBytes);

	} else if (dp_strncmpi(param_list[0],
			"qos_mib_clear",
			strlen("qos_mib_clear")) == 0) {
		tmu_hal_clear_qos_m_local =
			tmu_hal_clear_qos_mib_hook_fn;
		if (!tmu_hal_clear_qos_m_local) {
			pr_info("tmu_hal_clear_qos_m_local NULL ?\n");
			return count;
		}
		res = tmu_hal_clear_qos_m_local(NULL, NULL, -1, 0);
		if (res)
			pr_info("tmu_hal_clear_qos_m_local failed\n");
		else
			pr_info("%s done\n",
				"tmu_hal_clear_qos_m_local(NULL, NULL, -1, 0)");

	} else if (dp_strncmpi(param_list[0],
			"csum_mib_clear",
			strlen("csum_mib_clear")) == 0) {
		tmu_hal_clear_csum_ol_m_local =
			tmu_hal_clear_csum_ol_mib_hook_fn;
		if (!tmu_hal_clear_csum_ol_m_local) {
			pr_info("tmu_hal_clear_csum_ol_mib_hook_fn NULL\n");
			return count;
		}
		res = tmu_hal_clear_csum_ol_m_local(0);
		if (res)
			pr_info("tmu_hal_clear_csum_ol_mib_hook_fn failed\n");
		else
			pr_info("tmu_hal_clear_csum_ol_mib_hook_fn(0) done\n");

	} else {
		goto help;
	}
#else
	return count;
#endif
	return count;
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_MIB_TMU_MPE_MIB)
help:
	pr_info("usage:\n");
	pr_info("  test qos_mib  api:      echo qos_mib        > %s\n",
		"/proc/dp/mib_port");
	pr_info("  test csum_mib api       echo csum_mib       > %s\n",
		"/proc/dp/mib_port\n");
	pr_info("  test cear_qos_mib  api  echo qos_mib_clear  > %s\n",
		"/proc/dp/mib_port");
	pr_info("  test csum_mib_clear api echo csum_mib_clear > %s\n",
		"/proc/dp/mib_port");

	return count;
#endif
}

int proc_mib_vap_dump(struct seq_file *s, int pos)
{
	int j = 0;
	int f_newline = 0;
	dp_subif_t subif;
	struct rtnl_link_stats64 stats_mib;
	int itf_base;
	struct pmac_port_info *port;

	if ((pos > (PMAC_MAX_NUM - 1)) || (pos > proc_mib_vap_end_id)) {
		pos = -1;	/*end of the loop */
		return pos;
	}

	if (pos == proc_mib_vap_start_id) {
		memset(&subif, 0, sizeof(dp_subif_t));
		memset(&stats_mib, 0, sizeof(stats_mib));
		seq_printf(s, "%5s/%3s %5s %22s %22s %22s %22s\n", "Port",
			   "VAP", "IfID", "Rx_PKTS", "Tx_PKTS",
			   "Rx_DROP_PKTS", "Tx_DROP_PKTS\n");
	}
	port = get_dp_port_info(0, pos);

	if (!port || !port->status)		/*not allocated yet*/
		goto EXIT;

	for (j = 0; j <= (MAX_SUBIF_PER_PORT - 1); j++) {
		subif.port_id = pos;
		subif.subif = SET_VAP(j, port->vap_offset, port->vap_mask);
		itf_base = get_gsw_interface_base(pos);
		if (itf_base < 0)	/*no GSW itf assigned*/
			continue;
		if (!get_dp_port_subif(port, j)->flags)	/*not registered yet*/
			continue;
		if (dp_get_port_vap_mib_30
		    (&subif, NULL, &stats_mib,
		     DP_F_STATS_SUBIF) == 0) {
			seq_printf(s,
				   "%5d/%03d %5d %22llu %22llu %22llu %22llu\n",
				   subif.port_id, j, itf_base + j,
				   stats_mib.rx_packets, stats_mib.tx_packets,
				   stats_mib.rx_dropped,
				   stats_mib.tx_dropped);
			seq_puts(s, "\n");
			f_newline = 1;
		} else
			seq_printf(s,
				   "port_vap_mib failed for port/vap %d/%d\n",
				   pos, j);
	}
	if (f_newline)
		seq_puts(s, "\n");

	/*buffer over flow and don't increase pos */
	if (seq_has_overflowed(s))
		return pos;

 EXIT:
	pos++;
	return pos;
}

int proc_mib_vap_start(void)
{
	return proc_mib_vap_start_id;
}

ssize_t proc_mib_vap_write(struct file *file, const char *buf, size_t count,
			   loff_t *ppos)
{
	int len;
	char str[64];
	char *param_list[2] = { 0 };
	unsigned int num[2] = { -1 };

	len = (sizeof(str) > count) ? count : sizeof(str) - 1;
	len -= copy_from_user(str, buf, len);
	str[len] = 0;
	if (dp_split_buffer(str, param_list, ARRAY_SIZE(param_list)) < 2)
		goto help;

	if (param_list[0] && strlen(param_list[0]))
		num[0] = dp_atoi(param_list[0]);

	if (param_list[1] && strlen(param_list[1]))
		num[1] = dp_atoi(param_list[1]);

	set_start_end_id(num[0], num[1], MAX_SUBIF_PER_PORT - 1,
			 MAX_SUBIF_PER_PORT - 1, 0, -1,
			 &proc_mib_vap_start_id, &proc_mib_vap_end_id);
	pr_info("proc_mib_vap_start_id=%d, proc_mib_vap_end_id=%d\n",
		proc_mib_vap_start_id, proc_mib_vap_end_id);
	return count;

 help:
	pr_info("usage: echo start_id end_id > /proc/dp/mib_vap\n");
	return count;
}

#ifdef THREAD_MODE
int mib_wraparound_thread(void *data)
{
	while (1) {
		if (poll_interval)
			mib_wraparound_timer_poll(0);
		msleep(poll_interval / HZ * 1000 / PMAC_MAX_NUM / 2);
		DP_DEBUG(DP_DBG_FLAG_MIB, "mib_wraparound_thread\n");
	}
}
#endif

int set_gsw_itf(u8 ep, u8 ena, int start)
{
	GSW_portCfg_t port_cfg;
	struct core_ops *gsw_r;

	gsw_r = dp_port_prop[0].ops[GSWIP_R];

	if (ep >= PMAC_MAX_NUM)
		return -1;

	/*get this ports itf base */
	port_cfg.nPortId = ep;
	if (gsw_r->gsw_common_ops.PortCfgGet(gsw_r, &port_cfg)) {
		DP_DEBUG(DP_DBG_FLAG_MIB,
			 "Why gsw_core_api return failure: GSW_PORT_CFG_GET for port_id=%d\n",
			 port_cfg.nPortId);
		return -1;
	}
	port_cfg.nIfCountStartIdx = start;
	port_cfg.bIfCounters = ena ? 1 : 0;
	if (gsw_r->gsw_common_ops.PortCfgSet(gsw_r, &port_cfg)) {
		DP_DEBUG(DP_DBG_FLAG_MIB,
			 "Why gsw_core_api return failure: GSW_PORT_CFG_SET for port_id=%d\n",
			 port_cfg.nPortId);
		return -1;
	}

	return 0;
}

struct gsw_itf *get_free_itf(u8 ep, u32 flag)
{
	int i;
	int free_id = -1;

	if (ep >= PMAC_MAX_NUM)
		return NULL;

	for (i = 0; i < ARRAY_SIZE(itf_assign); i++) {
		if (ep == itf_assign[i].ep) {
			DP_DEBUG(DP_DBG_FLAG_MIB,
				 "Found the free gsw itf for itf_assign[i]=%d\n",
				 i);
			set_gsw_itf(ep, 1, itf_assign[i].start);
			return &itf_assign[i];
		}
		if ((itf_assign[i].ep == (u8)-1) && (free_id == -1))
			free_id = i;
	}
	if (free_id != -1) {
		i = free_id;
		DP_DEBUG(DP_DBG_FLAG_MIB,
			 "Found the free gsw itf for itf_assign[i]=%d\n",
			 i);
		itf_assign[i].ep = ep;
		set_gsw_itf(ep, 1, itf_assign[i].start);
		return &itf_assign[i];
	}

	return NULL;
}

int reset_gsw_itf(u8 ep)
{
	int i;

	if (ep >= PMAC_MAX_NUM)
		return -1;

	/* only matched the fixed assignment one */
	for (i = 0; i < ARRAY_SIZE(itf_assign); i++)
		if (itf_assign[i].ep == ep)
			break;
	if (i >= ARRAY_SIZE(itf_assign)) {
		set_gsw_itf(ep, 0, 0);
	} else if (itf_assign[i].fixed) {
		set_gsw_itf(ep, 1, itf_assign[i].start);
	} else {
		set_gsw_itf(ep, 0, 0);
		itf_assign[i].ep = -1;
	}

	return 0;
}

int dp_mib_init(u32 flag)
{
	spin_lock_init(&dp_mib_lock);
	memset(&aggregate_mib, 0, sizeof(aggregate_mib));
	memset(&last, 0, sizeof(last));
	memset(&last_vap, 0, sizeof(last_vap));

#ifdef THREAD_MODE
	thread = kthread_run(mib_wraparound_thread, 0, "dp_mib");
#else
	init_timer_on_stack(&exp_timer);
	exp_timer.expires = jiffies + poll_interval;
	exp_timer.data = 0;
	exp_timer.function = mib_wraparound_timer_poll;
	add_timer(&exp_timer);
	pr_info("dp_mib_init done\n");
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

