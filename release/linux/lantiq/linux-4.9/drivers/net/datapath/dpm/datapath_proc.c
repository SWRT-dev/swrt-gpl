// SPDX-License-Identifier: GPL-2.0
/*****************************************************************************
 * Copyright (c) 2020 - 2021, MaxLinear, Inc.
 * Copyright 2016 - 2020 Intel Corporation
 * Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
 * Copyright 2012 - 2014 Lantiq Deutschland GmbH
 *
 *****************************************************************************/
#include <linux/list.h>
#include <net/datapath_api.h>
#include <net/datapath_api_vlan.h>
#include <net/datapath_proc_api.h>
#include "datapath.h"
#include "datapath_instance.h"
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_SWITCHDEV)
#include "datapath_swdev_api.h"
#endif

#define DP_DBGFS_NAME		"dp"
#define DBGFS_DBG		"dbg"
#define DBGFS_EVENT		"event"
#define DBGFS_INST		"inst"
#define DBGFS_INST_DEV		"inst_dev"
#define DBGFS_INST_HAL		"inst_hal"
#define DBGFS_INST_MOD		"inst_mod"
#define DBGFS_MIB_GLOBAL	"mib_global_stats"
#define DBGFS_PCE		"pce"
#define DBGFS_CTP_LIST		"pmap_ctp_list"
#define DBGFS_PORT		"port"
#define DBGFS_QOS		"qos"
#define DBGFS_BR_VLAN		"vlan"
#define DBGFS_DPID		"dpid"

#define DEBUGFS_DBG DP_DEBUGFS_PATH	"/" DBGFS_DBG
#define DBG_CTP DP_DEBUGFS_PATH		"/" DBGFS_CTP_LIST
#define DBG_PORT DP_DEBUGFS_PATH	"/" DBGFS_PORT

/* NOTE: keep in sync with enum DP_RXOUT_QOS_MODE */
const char *dp_rxout_qos_mode_str[] = {
	"bypass QoS only",	/* DP_RXOUT_BYPASS_QOS_ONLY */
	"with QoS",		/* DP_RXOUT_QOS */
	"bypass QoS and FSQM",	/* DP_RXOUT_BYPASS_QOS_FSQM */
	"invalid"		/* DP_RXOUT_QOS_MAX */
};

/* NOTE: keep in sync with enum umt_sw_msg */
const char *umt_sw_msg_str[] = {
	"No MSG",		/* UMT_NO_MSG */
	"MSG0 only",		/* UMT_MSG0_ONLY */
	"MSG1 only",		/* UMT_MSG1_ONLY */
	"MSG0 and MSG1"		/* UMT_MSG0_MSG1 */
};

/* CQM Lookup Mode for LGM */
const char *cqm_lookup_mode_lgm[] = {
	"subif_id[13:8] + class[1:0]",
	"subif_id[7:0]",
	"subif_id[11:8] + class[3:0]",
	"subif_id[4:0] + class[2:0]",
	"class[1:0] + subif_id[5:0]",
	"subif_id[15:8]",
	"subif_id[1:0] + class[3:0] + color[1:0]",
	"subif_id[14:8] + class[0]"
};

/* CQM Lookup Mode for FMx */
const char *cqm_lookup_mode_fmx[] = {
	"flowid[7:6] + dec + enc + class[3:0]",
	"subif_id[7:0]",
	"subif_id[11:8] + class[3:0]",
	"subif_id[4:0] + class[2:0]"
};

const char *dp_port_type[] = {
	"Linux",
	"DPDK"
};

const char *gsw_port_mode[] = {
	"8BIT_WLAN",
	"9BIT_WLAN",
	"PORT_GPON",
	"PORT_EPON",
	"PORT_GINT"
};

const char *dp_spl_conn_type[] = {
	"SPL_TOE",
	"SPL_VOICE",
	"SPL_VPNA",
	"SPL_APP_LITEPATH",
	"SPL_PP_NF",
	"NO VALID"
};

typedef int (*print_ctp_bp_t)(struct seq_file *s, int inst,
			      struct pmac_port_info *port,
			      int subif_index, u32 flag);

#define PROC_WRITE_PORT_DEF -1
static int tmp_inst;
static int proc_write_port = PROC_WRITE_PORT_DEF;
static int proc_write_vap = PROC_WRITE_PORT_DEF;
static bool dump_one_port;
static bool dump_one_vap;
static int proc_port_init(void);
static int proc_port_dump(struct seq_file *s, int pos);
static ssize_t proc_port_write(struct file *file, const char *buf,
			       size_t count, loff_t *ppos);
#if defined(CONFIG_INTEL_DATAPATH_DBG) && CONFIG_INTEL_DATAPATH_DBG
static void proc_dbg_read(struct seq_file *s);
static ssize_t proc_dbg_write(struct file *, const char *, size_t, loff_t *);
#endif

#if IS_ENABLED(CONFIG_INTEL_DATAPATH_SWITCHDEV)
static int proc_brvlan_dump(struct seq_file *s, int pos)
{
	struct pmac_port_info *port_info;
	struct bridge_member_port *bmp;
	struct br_info *brdev_info;
	struct vlist_entry *vlist;
	struct vlan_entry *vlan;
	struct hlist_head *head;
	struct pce_entry *pce;
	dp_subif_t subif;
	int i, vap;
	bool entry;

	if (pos == 0) {
		seq_printf(s, "|%-10s|%-10s|%-10s|%-10s|%-10s|%-5s",
			   "Br", "BrFid", "BrVlanID", "Dev", "Bp", "VID");
		seq_printf(s, "|%-5s|%-10s|%-10s|%-15s\n",
			   "FiD", "RefCnt", "PCE Idx", "PCE Status");
		for (i = 0; i < 100; i++)
			seq_puts(s, "=");
		seq_puts(s, "\n");
	}

	head = get_dp_g_bridge_id_entry_hash_table_info(pos);
	brdev_info = hlist_entry_safe(head->first, struct br_info, br_hlist);
	if (!brdev_info || !brdev_info->br_vlan_en)
		goto exit;

	entry = false;
	list_for_each_entry(vlist, &brdev_info->br_vlan_list, list) {
		if (!vlist->vlan_entry)
			continue;
		if (entry)
			seq_puts(s, "\n");

		seq_printf(s, "|%-10s", brdev_info->dev->name);
		seq_printf(s, "|%-10d", brdev_info->fid);
		seq_printf(s, "|%-10d", vlist->vlan_entry->vlan_id);
		entry = true;
	}

	if (!entry)
		seq_printf(s, "%-11s", "");

	list_for_each_entry(bmp, &brdev_info->bp_list, list) {
		seq_puts(s, "\n");
		seq_printf(s, "%-11s%-11s%-11s", "", "", "");
		if (dp_get_netif_subifid(bmp->dev, NULL, NULL, NULL,
					 &subif, 0)) {
			seq_printf(s, "%s: dp_get_netif_subifid failed\n",
				   __func__);
			return DP_FAILURE;
		}
		port_info = get_dp_port_info(subif.inst, subif.port_id);
		vap = GET_VAP(subif.subif, port_info->vap_offset,
			      port_info->vap_mask);
		entry = false;
		list_for_each_entry(vlist, &bmp->bport_vlan_list, list) {
			if (!vlist->vlan_entry)
				continue;
			vlan = vlist->vlan_entry;
			if (entry) {
				seq_puts(s, "\n");
				seq_printf(s, "%-11s%-11s%-11s",
					   "", "", "");
			}
			seq_printf(s, "|%-10s", bmp->dev->name);
			seq_printf(s, "|%-10d", bmp->bportid);
			seq_printf(s, "|%-5d|%-5d|%-10d",
				   vlan->vlan_id, vlan->fid, vlan->ref_cnt);
			entry = true;
			pce =
			   get_pce_entry_from_ventry(vlan, subif.port_id, vap);
			if (!pce)
				continue;
			seq_printf(s, "|%-10d|%-15s",
				   pce->blk_info->info.subblk_firstidx +
				   pce->idx,
				   pce->disable ? "Disabled" : "Enabled");
		}
	}
	seq_puts(s, "\n");

exit:
	if (!seq_has_overflowed(s)) {
		pos++;
		if (pos == BR_ID_ENTRY_HASH_TABLE_SIZE)
			pos = -1;
	}

	return pos;
}
#endif

static int proc_mib_stats_dump(struct seq_file *s, int pos)
{
	struct mib_global_stats stats = {0};
	int cpu;

	if (!capable(CAP_SYS_PACCT))
		return -1;

	for_each_online_cpu(cpu) {
		stats.rx_rxif_pkts += MIB_G_STATS_GET(rx_rxif_pkts, cpu);
		stats.rx_txif_pkts += MIB_G_STATS_GET(rx_txif_pkts, cpu);
		stats.rx_drop += MIB_G_STATS_GET(rx_drop, cpu);
		stats.tx_pkts += MIB_G_STATS_GET(tx_pkts, cpu);
		stats.tx_drop += MIB_G_STATS_GET(tx_drop, cpu);
	}

	seq_puts(s, "MIB Global Rx/Tx counters\n");
	seq_printf(s, "rx_rxif_pkts  = %llu\n", stats.rx_rxif_pkts);
	seq_printf(s, "rx_txif_pkts  = %llu\n", stats.rx_txif_pkts);
	seq_printf(s, "rx_drop       = %llu\n", stats.rx_drop);
	seq_printf(s, "tx_pkts       = %llu\n", stats.tx_pkts);
	seq_printf(s, "tx_drop       = %llu\n", stats.tx_drop);

	return -1;
}

ssize_t proc_mib_stats_write(struct file *file, const char *buf, size_t count,
			     loff_t *ppos)
{
	int cpu;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	/* only clear action is supported i.e. '0' */
	if (count > 2 || buf[0] != '0') {
		pr_info("usage: to clear stats\n");
		pr_info("  echo 0 > %s\n", DBGFS_MIB_GLOBAL);
	} else {
		for_each_online_cpu(cpu) {
			MIB_G_STATS_RESET(rx_rxif_pkts, cpu);
			MIB_G_STATS_RESET(rx_txif_pkts, cpu);
			MIB_G_STATS_RESET(rx_drop, cpu);
			MIB_G_STATS_RESET(tx_pkts, cpu);
			MIB_G_STATS_RESET(tx_drop, cpu);
		}
	}

	return count;
}

int proc_port_init(void)
{
	tmp_inst = 0;
	dump_one_vap = false;
	dump_one_port = false;

	/* Check if we have been requested to dump only a specific port */
	if (proc_write_port != PROC_WRITE_PORT_DEF) {
		dump_one_port = true;
		if (proc_write_vap != PROC_WRITE_PORT_DEF)
			dump_one_vap = true;
		/* Change to requested port id */
		return proc_write_port;
	}

	return 0;
}

static int dump_dc_info(struct seq_file *s, struct pmac_port_info *port)
{
	u8 cid, pid;
	u16 nid;
	int i;

	for (i = 0; i < port->num_tx_ring; i++) {
		seq_printf(s, "    DC TxRing:         %d\n", i);
		seq_printf(s, "      TXIN  DeqRingSize/paddr:      %d/0x%px\n",
			   port->tx_ring[i].in_deq_ring_size,
			   port->tx_ring[i].in_deq_paddr);
		seq_printf(s, "      TXOUT FreeRingSize/paddr:     %d/0x%px\n",
			   port->tx_ring[i].out_free_ring_size,
			   port->tx_ring[i].out_free_paddr);
		seq_printf(s, "      TXOUT PolicyBase/Poolid:      %d/%d\n",
			   port->tx_ring[i].txout_policy_base,
			   port->tx_ring[i].tx_poolid);
		seq_printf(s, "      PolicyNum:                    %d\n",
			   port->tx_ring[i].policy_num);
		seq_printf(s, "      NumOfTxPkt/TxPktSize:         %d/%d\n",
			   port->tx_ring[i].num_tx_pkt,
			   port->tx_ring[i].tx_pkt_size);
	}

	for (i = 0; i < port->num_rx_ring; i++) {
		seq_printf(s, "    DC RxRing:         %d\n", i);
		seq_printf(s, "      RXOUT EnqRingSize/paddr/pid:  %d/0x%px/%d\n",
			   port->rx_ring[i].out_enq_ring_size,
			   port->rx_ring[i].out_enq_paddr,
			   port->rx_ring[i].out_enq_port_id);
		seq_printf(s, "      RXOUT NumOfDmaCh:             %d\n",
			   port->rx_ring[i].num_out_tx_dma_ch);
		dp_dma_parse_id(port->rx_ring[i].out_dma_ch_to_gswip, &cid,
				&pid, &nid);
		seq_printf(s, "      RXOUT dma-ctrl/port/chan:     %d/%d/%d\n",
			   cid, pid, nid);
		seq_printf(s, "      RXOUT NumOfCqmDqPort/pid:     %d/%d\n",
			   port->rx_ring[i].num_out_cqm_deq_port,
			   port->rx_ring[i].out_cqm_deq_port_id);
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP32)
		seq_printf(s, "      RXOUT Poolid:                 %d\n",
			   port->rx_ring[i].rx_poolid);
#endif
		seq_printf(s, "      RXIN  InAllocRingSize/Paddr:  %d/0x%px\n",
			   port->rx_ring[i].in_alloc_ring_size,
			   port->rx_ring[i].in_alloc_paddr);
		seq_printf(s, "      NumPkt/Pktsize/Policybase:    %d/%d/%d\n",
			   port->rx_ring[i].num_pkt,
			   port->rx_ring[i].rx_pkt_size,
			   port->rx_ring[i].rx_policy_base);
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP32)
		seq_printf(s, "      PolicyNum:                    %d\n",
			   port->rx_ring[i].policy_num);
		seq_printf(s, "      RXOUT MsgMode/QosMode:        %d/%d\n",
			   port->rx_ring[i].out_msg_mode,
			   port->rx_ring[i].out_qos_mode);
#endif
		seq_printf(s, "      PreFillPktNum/PktBase:        %d/0x%px\n",
			   port->rx_ring[i].prefill_pkt_num,
			   port->rx_ring[i].pkt_base_paddr);
	}

#if !IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP30)
	for (i = 0; i < port->num_umt_port; i++) {
		seq_printf(s, "    UMT[%d] id/CqmDeqPid/msg_mode:  %d/%d/%d\n",
			   i, port->umt[i].ctl.id, port->umt[i].res.cqm_dq_pid,
			   port->umt[i].ctl.msg_mode);
		seq_printf(s, "    UMT[%d] period/daddr:           %d/0x%llx\n",
			   i, port->umt[i].ctl.msg_interval,
			   (unsigned long long)port->umt[i].ctl.daddr);
	}
#endif
	return 0;
}

#if IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP32)
static void dump_subif_spl_cfg_info(struct seq_file *s,
				    struct dp_subif_info *sif)
{
	struct dp_spl_cfg *cfg = sif->spl_cfg;
	struct dp_spl_igp *igp;
	struct dp_spl_egp *egp;
	u8 cid, pid;
	u16 nid;
	int i;

	seq_puts(s, "           CPU Special config info:\n");
	seq_printf(s, "           spl_conn_type:  %s\n",
		   dp_spl_conn_type[cfg->type]);
	seq_printf(s, "           spl_conn_idx:   %d\n", cfg->spl_id);

	if (cfg->spl_gpid)
		seq_printf(s, "           spl_gpid:       %d\n",
			   cfg->spl_gpid);
	seq_printf(s, "           dp_cb:          %px\n",
		   cfg->dp_cb);

	/* IGP info */
	for (i = 0; i < cfg->num_igp; i++) {
		igp = &cfg->igp[i];
		seq_printf(s, "           igp[%d]:\n", i);
		seq_printf(s, "             igp_id:               %d\n",
			   igp->igp_id);
		seq_printf(s, "             igp_ring_size:        %d\n",
			   igp->igp_ring_size);
		seq_printf(s, "             igp_paddr:            %px\n",
			   igp->igp_paddr);
		seq_printf(s, "             alloc_paddr:          %px\n",
			   igp->alloc_paddr);
		seq_printf(s, "             alloc_vaddr:          %px\n",
			   igp->alloc_vaddr);
		dp_dma_parse_id(igp->igp_dma_ch_to_gswip, &cid, &pid, &nid);
		if (igp->num_out_tx_dma_ch)
			seq_printf(s, "             dma-ctrl/port/chan:   %d/%d/%d\n",
				   cid, pid, nid);
		seq_printf(s, "             num_out_tx_dma_ch:    %d\n",
			   igp->num_out_tx_dma_ch);
		seq_printf(s, "             out_qos_mode:         %d:%s\n",
			   igp->out_qos_mode,
			   dp_rxout_qos_mode_str[igp->out_qos_mode]);
		seq_printf(s, "             num_out_cqm_deq_port: %d\n",
			   igp->num_out_cqm_deq_port);

		if (igp->egp) {
			seq_printf(s, "             igp->egp->egp_id:     %d\n",
				   igp->egp->egp_id);
			seq_printf(s, "             igp->egp->qid:        %d\n",
				   igp->egp->qid);
		}
	}

	/* EGP info */
	for (i = 0; i < cfg->num_egp; i++) {
		egp = &cfg->egp[i];
		seq_printf(s, "           egp[%d]:\n", i);
		seq_printf(s, "             egp_id:               %d\n",
			   egp->egp_id);
		seq_printf(s, "             port type:            to %s\n",
			   egp->type ? "GSWIP" : "DEV");
		seq_printf(s, "             pp_ring_size:         %d\n",
			   egp->pp_ring_size);
		seq_printf(s, "             egp_paddr:            %px\n",
			   egp->egp_paddr);
		seq_printf(s, "             tx_pkt_credit:        %d\n",
			   egp->tx_pkt_credit);
		seq_printf(s, "             tx_push_paddr_qos:    %px\n",
			   egp->tx_push_paddr_qos);
		seq_printf(s, "             free_paddr:           %px\n",
			   egp->free_paddr);
		seq_printf(s, "             free_vaddr:           %px\n",
			   egp->free_vaddr);
		if (egp->qid)
			seq_printf(s, "             qid:                  %d\n",
				   egp->qid);
	}

	/* SPL policy info */
	if (cfg->f_policy) {
		seq_puts(s, "           Policy:\n");
		seq_printf(s, "             num_pkts:             %d\n",
			   cfg->policy[0].num_pkt);
		seq_printf(s, "             rx_pkt_size:          %d\n",
			   cfg->policy[0].rx_pkt_size);
		seq_printf(s, "             pool_id:              %d\n",
			   cfg->policy[0].pool_id[0]);
		seq_printf(s, "             policy_map:           0x%x\n",
			   cfg->policy[0].policy_map);
	}

	/* UMT info - print only if explicitly enabled */
	if (cfg->umt[0].ctl.enable == 1) {
		struct umt_port_ctl *ctl = &cfg->umt[0].ctl;
		struct umt_port_res *res = &cfg->umt[0].res;

		seq_puts(s, "           UMT port:\n");
		seq_printf(s, "             port id:              %d\n",
			   ctl->id);
		seq_printf(s, "             daddr:                0x%llx\n",
			   ctl->daddr);
		seq_printf(s, "             msg_interval:         %d\n",
			   ctl->msg_interval);
		seq_printf(s, "             msg_mode:             %d:%s\n",
			   ctl->msg_mode, ctl->msg_mode ? "User" : "Self");
		seq_printf(s, "             cnt_mode:             %d:%s count\n",
			   ctl->cnt_mode,
			   ctl->cnt_mode ? "Accumulate" : "Increamental");
		seq_printf(s, "             sw_msg:               %d:%s\n",
			   ctl->sw_msg, umt_sw_msg_str[ctl->sw_msg]);
		seq_printf(s, "             rx_msg_mode:          %d:Rx %s\n",
			   ctl->rx_msg_mode,
			   ctl->rx_msg_mode ? "IN Add" : "OUT Sub");
		seq_printf(s, "             enable:               %d\n",
			   ctl->enable);
		seq_printf(s, "             fflag:                0x%lx\n",
			   ctl->fflag);
		seq_printf(s, "             dma_id:               %d\n",
			   res->dma_id);
		seq_printf(s, "             dma_ch_num:           %d\n",
			   res->dma_ch_num);
		seq_printf(s, "             cqm_enq_pid:          %d\n",
			   res->cqm_enq_pid);
		seq_printf(s, "             cqm_dq_pid:           %d\n",
			   res->cqm_dq_pid);
		seq_printf(s, "             rx_src:               %d:from %s\n",
			   res->rx_src, res->rx_src ? "DMA" : "CQM");
	}
}
#endif

void dump_subif_info(struct seq_file *s, struct pmac_port_info *port, int n)
{
	struct dp_subif_info *sif = get_dp_port_subif(port, n);
	struct dev_mib *mib = get_dp_port_subif_mib(sif);
	print_ctp_bp_t print_ctp_bp = DP_CB(tmp_inst, proc_print_ctp_bp_info);
	struct dma_chan_info *dma;
	struct cqm_port_info *cqm;
	int i, cqm_p;
	u8 cid, pid;
	u16 nid;
	char *flag_other;

	if (!sif->flags)
		return;

	flag_other = kzalloc(1024, GFP_ATOMIC);
	if (!flag_other)
		return;

	seq_printf(s, "      [%02d]:%s=0x%04x %s=0x%0lx(%s=%s),%s=%s\n",
		   n, "subif", sif->subif, "netif", (uintptr_t)sif->netif,
		   "netif", sif->netif ? sif->netif->name : "NULL/DSL",
		   "device_name", sif->device_name);
	seq_printf(s, "           subif_flag:     0x%08x", sif->subif_flag);
	for (i = 0; i < get_dp_port_type_str_size(); i++) {
		if (sif->subif_flag & dp_port_flag[i])
			seq_printf(s, " %s ", dp_port_type_str[i]);
	}
	seq_puts(s, "\n");
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP32)
	seq_printf(s, "           type:           %d (%s)\n", sif->type,
		   dp_port_type[sif->type]);
#endif

	if (sif->data_flag_ops & DP_SUBIF_AUTO_NEW_Q)
		sprintf(flag_other + strlen(flag_other), "%s | ",
			"DP_SUBIF_AUTO_NEW_Q");
	if (sif->data_flag_ops & DP_SUBIF_SPECIFIC_Q)
		sprintf(flag_other + strlen(flag_other), "%s | ",
			"DP_SUBIF_SPECIFIC_Q");
	if (sif->data_flag_ops & DP_SUBIF_LCT)
		sprintf(flag_other + strlen(flag_other), "%s | ",
			"DP_SUBIF_LCT");
	if (sif->data_flag_ops & DP_SUBIF_VANI)
		sprintf(flag_other + strlen(flag_other), "%s | ",
			"DP_SUBIF_VANI");
	if (sif->data_flag_ops & DP_SUBIF_DEQPORT_NUM)
		sprintf(flag_other + strlen(flag_other), "%s | ",
			"DP_SUBIF_DEQPORT_NUM");
	if (sif->data_flag_ops & DP_SUBIF_RX_FLAG)
		sprintf(flag_other + strlen(flag_other), "%s | ",
			"DP_SUBIF_RX_FLAG");
	if (sif->data_flag_ops & DP_SUBIF_SWDEV)
		sprintf(flag_other + strlen(flag_other), "%s | ",
			"DP_SUBIF_SWDEV");
	if (sif->data_flag_ops & DP_SUBIF_PREL2)
		sprintf(flag_other + strlen(flag_other), "%s | ",
			"DP_SUBIF_PREL2");
	if (sif->data_flag_ops & DP_SUBIF_CPU_QMAP)
		sprintf(flag_other + strlen(flag_other), "%s | ",
			"DP_SUBIF_CPU_QMAP");
	if (sif->data_flag_ops & DP_SUBIF_NO_HOSTIF)
		sprintf(flag_other + strlen(flag_other), "%s | ",
			"DP_SUBIF_NO_HOSTIF");

	seq_printf(s, "           data_flag_ops   0x%08x (%s)\n",
		   sif->data_flag_ops, flag_other);
	seq_printf(s, "           rx_fn_rxif_pkt  0x%08x\n",
		   STATS_GET(mib->rx_fn_rxif_pkt));
	seq_printf(s, "           rx_fn_txif_pkt  0x%08x\n",
		   STATS_GET(mib->rx_fn_txif_pkt));
	seq_printf(s, "           rx_fn_dropped   0x%08x\n",
		   STATS_GET(mib->rx_fn_dropped));
	seq_printf(s, "           tx_cbm_pkt      0x%08x\n",
		   STATS_GET(mib->tx_cbm_pkt));
	seq_printf(s, "           tx_tso_pkt      0x%08x\n",
		   STATS_GET(mib->tx_tso_pkt));
	seq_printf(s, "           tx_pkt_dropped  0x%08x\n",
		   STATS_GET(mib->tx_pkt_dropped));
	seq_printf(s, "           tx_clone_pkt    0x%08x\n",
		   STATS_GET(mib->tx_clone_pkt));
	seq_printf(s, "           tx_hdr_room_pkt 0x%08x\n",
		   STATS_GET(mib->tx_hdr_room_pkt));
	seq_printf(s, "           rx_fn           0x%px\n", sif->rx_fn);
	seq_printf(s, "           get_subifid_fn  0x%px\n",
		   sif->get_subifid_fn);
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_SWITCHDEV)
	seq_printf(s, "           swdev:	   %d (%s)\n", sif->swdev_en,
		   sif->swdev_en ? "Enabled" : "Disabled");
#endif
	seq_printf(s, "           subif_groupid:  %d\n",
		   sif->subif_groupid);
	if (print_ctp_bp)
		print_ctp_bp(s, tmp_inst, port, n, 0);
	seq_printf(s, "           num_qid:        %d\n", sif->num_qid);
	seq_printf(s, "           dqport_idx:     %d\n", sif->cqm_port_idx);
	for (i = 0; i < sif->num_qid; i++) {
		seq_printf(s, "%13s[%02d]:qid/node:    %d/%d\n", "",
			   i, sif->qid_list[i], sif->q_node[i]);
		cqm_p = sif->cqm_deq_port[i];
		cqm = get_dp_deqport_info(tmp_inst, cqm_p);
		seq_printf(s, "%18sport/node:   %d/%d(ref=%d)\n", "",
			   cqm_p, sif->qos_deq_port[i], cqm->ref_cnt);
		dp_dma_parse_id(cqm->dma_chan, &cid, &pid, &nid);
		dma = dp_dma_chan_tbl[tmp_inst];
		if (port->num_dma_chan && dma) {
			dma += cqm->dma_ch_offset;
			seq_printf(s, "%18stx_dma_ch:   0x%x(ref=%d,dma-ctrl=%d,port=%d,channel=%d)\n",
				   "", cqm->dma_chan,
				   atomic_read(&dma->ref_cnt),
				   cid, pid, nid);
		}
	}
	seq_printf(s, "           mac_learn_dis:  %d (%s)\n",
		   sif->mac_learn_dis,
		   sif->mac_learn_dis ? "Disabled" : "Enabled");
	seq_printf(s, "           gpid:           %d\n", sif->gpid);
	seq_printf(s, "           cpu_port_en:    %d (%s)\n", sif->cpu_port_en,
		   sif->cpu_port_en ? "Enabled" : "Disabled");
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP32)
	seq_printf(s, "           prel2_len:      %d (%s)\n", sif->prel2_len,
		   sif->prel2_len ? "Enabled" : "Disabled");
	seq_printf(s, "           spl_conn_type:  %s\n",
		   dp_spl_conn_type[sif->spl_conn_type]);
#endif
	seq_puts(s, "           ctp_dev:        ");
	if (sif->ctp_dev && sif->ctp_dev->name)
		seq_puts(s, sif->ctp_dev->name);
	else
		seq_puts(s, "NULL");
	seq_puts(s, "\n");
	seq_printf(s, "           rx_en_flag:     %d\n",
		   STATS_GET(sif->rx_flag));
	seq_printf(s, "           tx_policy base/num/map:      %d/%d/%x\n",
		   sif->tx_policy_base, sif->tx_policy_num, sif->tx_policy_map);
	seq_printf(s, "           rx_policy base/num/map:      %d/%d/%x\n",
		   sif->rx_policy_base, sif->rx_policy_num, sif->rx_policy_map);
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP32)
	/* CPU Special path config */
	if (sif->spl_cfg)
		dump_subif_spl_cfg_info(s, sif);
#endif

	kfree(flag_other);
}

int proc_port_dump(struct seq_file *s, int pos)
{
	struct inst_info *info = get_dp_prop_info(tmp_inst);
	struct cqm_port_info *cqm_info;
	struct pmac_port_info *port;
	struct dp_subif_info *sif;
	int i, start_vap, end_vap;
	u16 start = 0, nid;
	u8 cid, pid;
	char *flag_other;

	if (!capable(CAP_SYS_PACCT))
		return -1;

	port = get_dp_port_info(tmp_inst, pos);

	DP_CB(tmp_inst, get_itf_start_end)(port->itf_info, &start, NULL);

	if (port->status == PORT_FREE) {
		if (pos == 0) {
			sif = get_dp_port_subif(port, 0);

			seq_printf(s, "Reserved Port: %s=0x%08x  %s=0x%08x\n",
				   "rx_err_drop", STATS_GET(port->rx_err_drop),
				   "tx_err_drop", STATS_GET(port->tx_err_drop));
			seq_printf(s, "           qid/node:     %d/%d\n",
				   sif->qid, sif->q_node[0]);
			seq_printf(s, "           port/node:    %d/%d\n",
				   sif->cqm_deq_port[0], sif->qos_deq_port[0]);
		} else {
			seq_printf(s, "%02d: %s=0x%08x  %s=0x%08x\n",
				   pos,
				   "rx_err_drop", STATS_GET(port->rx_err_drop),
				   "tx_err_drop", STATS_GET(port->tx_err_drop));
		}

		goto exit;
	}

	seq_printf(s, "%02d:%s=0x%0lx(%s:%8s) %s=%02d %s=%02d %s=%d(%s) %s=%d\n",
		   pos, "module", (uintptr_t)port->owner,
		   "name", module_name(port->owner),
		   "dev_port", port->dev_port,
		   "dp_port", port->port_id,
		   "itf_base", start,
		   port->itf_info ? "Enabled" : "Not Enabled",
		   "ctp_max", port->ctp_max);
	seq_printf(s, "    status:            %s\n",
		   dp_port_status_str[port->status]);
	seq_puts(s, "    allocate_flags:    ");
	for (i = 0; i < get_dp_port_type_str_size(); i++) {
		if (port->alloc_flags & dp_port_flag[i])
			seq_printf(s, "%s ", dp_port_type_str[i]);
	}
	seq_puts(s, "\n");
	if (get_dp_port_prop(tmp_inst)->info.type < GSWIP32_TYPE)
		seq_printf(s, "    Lookup mode:       %d [%s]\n",
			   port->cqe_lu_mode,
			   cqm_lookup_mode_fmx[port->cqe_lu_mode]);
	else
		seq_printf(s, "    Lookup mode:       %d [%s]\n",
			   port->cqe_lu_mode,
			   cqm_lookup_mode_lgm[port->cqe_lu_mode]);

	seq_printf(s, "    gswip mode:        %d [%s]\n", port->gsw_mode,
		   gsw_port_mode[port->gsw_mode]);
	seq_printf(s, "    Port type:         %d (%s)\n", port->type,
		   dp_port_type[port->type]);
	seq_printf(s, "    LCT:               %d\n", port->lct_idx);
	seq_printf(s, "    subif_max:         %d\n", port->subif_max);
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_SWITCHDEV)
	seq_printf(s, "    p_swdev:	       %d (%s)\n", port->swdev_en,
		   port->swdev_en ? "Enabled" : "Disabled");
#endif
	seq_printf(s, "    cb->rx_fn:         0x%px\n", port->cb.rx_fn);
	seq_printf(s, "    cb->restart_fn:    0x%px\n", port->cb.restart_fn);
	seq_printf(s, "    cb->stop_fn:       0x%px\n", port->cb.stop_fn);
	seq_printf(s, "    cb->get_subifid_fn:0x%px\n",
		   port->cb.get_subifid_fn);
	seq_printf(s, "    num_subif:         %d\n", port->num_subif);
	seq_printf(s, "    vap_offset/mask:   %d/0x%x\n", port->vap_offset,
		   port->vap_mask);

	flag_other = kzalloc(1024, GFP_ATOMIC);
	if (!flag_other)
		return -ENOBUFS;

	if (port->flag_other & CBM_PORT_DP_SET)
		sprintf(flag_other + strlen(flag_other), "%s | ",
			"CBM_PORT_DP_SET");
	if (port->flag_other & CBM_PORT_DQ_SET)
		sprintf(flag_other + strlen(flag_other), "%s | ",
			"CBM_PORT_DQ_SET");
	if (port->flag_other & CBM_PORT_DMA_CHAN_SET)
		sprintf(flag_other + strlen(flag_other), "%s | ",
			"CBM_PORT_DMA_CHAN_SET");
	if (port->flag_other & CBM_PORT_PKT_CRDT_SET)
		sprintf(flag_other + strlen(flag_other), "%s | ",
			"CBM_PORT_PKT_CRDT_SET");
	if (port->flag_other & CBM_PORT_BYTE_CRDT_SET)
		sprintf(flag_other + strlen(flag_other), "%s | ",
			"CBM_PORT_BYTE_CRDT_SET");
	if (port->flag_other & CBM_PORT_RING_ADDR_SET)
		sprintf(flag_other + strlen(flag_other), "%s | ",
			"CBM_PORT_RING_ADDR_SET");
	if (port->flag_other & CBM_PORT_RING_SIZE_SET)
		sprintf(flag_other + strlen(flag_other), "%s | ",
			"CBM_PORT_RING_SIZE_SET");
	if (port->flag_other & CBM_PORT_RING_OFFSET_SET)
		sprintf(flag_other + strlen(flag_other), "%s | ",
			"CBM_PORT_RING_OFFSET_SET");

	seq_printf(s, "    cbm_port_flags:    0x%x [%s]\n", port->flag_other,
		   flag_other);

	kfree(flag_other);

	seq_printf(s, "    num_resv_queue:    %d\n", port->num_resv_q);
	seq_printf(s, "    resv_queue_base:   %d\n", port->res_qid_base);
	seq_printf(s, "    deq_port_base:     %d\n", port->deq_port_base);
	seq_printf(s, "    deq_port_num:      %d\n", port->deq_port_num);
	seq_printf(s, "    num_dma_chan:      %d\n", port->num_dma_chan);
	dp_dma_parse_id(port->dma_chan, &cid, &pid, &nid);
	seq_printf(s, "    dma_chan:          0x%x(dma-ctrl=%d,port=%d,channel=%d)\n",
		   port->dma_chan, cid, pid, nid);
	seq_printf(s, "    dma_chan_tbl_idx:  %d\n", port->dma_chan_tbl_idx);
	seq_printf(s, "    gpid_base/spl:     %d/%d\n", port->gpid_base,
		   port->gpid_spl);
	seq_printf(s, "    gpid_num:          %d\n", port->gpid_num);
	seq_printf(s, "    PTP:               %d (%s)\n", port->f_ptp,
		   port->f_ptp ? "Enabled" : "Disabled");
	seq_printf(s, "    loop_dis:          %d\n", port->loop_dis);
	seq_printf(s, "    tx_policy_base/num:%d/%d\n",
		   port->tx_policy_base, port->tx_policy_num);
	seq_printf(s, "    rx_policy_base/num:%d/%d\n",
		   port->rx_policy_base, port->rx_policy_num);
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP32)
	seq_printf(s, "    spl_tx_policy_base/num/map:%d/%d/%x\n",
		   port->spl_tx_policy_base, port->spl_tx_policy_num,
		   port->spl_tx_policy_map);
	seq_printf(s, "    spl_rx_policy_base/num/map:%d/%d/%x\n",
		   port->spl_rx_policy_base, port->spl_rx_policy_num,
		   port->spl_rx_policy_map);
#endif
	seq_printf(s, "    tx_pkt_credit:     %d\n", port->tx_pkt_credit);
	seq_printf(s, "    tx_b_credit:       %02d\n", port->tx_b_credit);
	seq_printf(s, "    txpush_addr:       0x%px\n", port->txpush_addr);
	seq_printf(s, "    txpush_addr_qos:   0x%px\n", port->txpush_addr_qos);
	seq_printf(s, "    tx_ring_size:      %d\n", port->tx_ring_size);
	seq_printf(s, "    tx_ring_offset:    %d(to next dequeue port)\n",
		   port->tx_ring_offset);

	if (port->num_rx_ring || port->num_tx_ring)
		dump_dc_info(s, port);

	/* check if there is a request to dump only specific subif info */
	if (dump_one_vap) {
		start_vap = proc_write_vap;
		end_vap = proc_write_vap + 1;
	} else {
		start_vap = 0;
		if (pos == 0)
			end_vap = info->cap.max_num_subif_per_port;
		else
			end_vap = port->subif_max;
	}
	for (i = start_vap; i < end_vap; i++)
		dump_subif_info(s, port, i);

	seq_printf(s, "    rx_err_drop=0x%08x  tx_err_drop=0x%08x\n",
		   STATS_GET(port->rx_err_drop),
		   STATS_GET(port->tx_err_drop));
exit:
	if (pos == 0 && reinsert_deq_port) {
		cqm_info = get_dp_deqport_info(tmp_inst, reinsert_deq_port);
		seq_printf(s, "    insertion_qid:       %d\n",
			   cqm_info->qid[0]);
		seq_printf(s, "    insertion_deqport:   %d\n",
			   reinsert_deq_port);
		dp_dma_parse_id(cqm_info->dma_chan, &cid, &pid, &nid);
		seq_printf(s, "%s0x%x(%s=%d,%s=%d,%s=%d)\n",
			   "    insertion_tx_dma_ch: ", cqm_info->dma_chan,
			   "dma-ctrl", cid, "port", pid, "channel", nid);
	}

	if (!seq_has_overflowed(s))
		pos++;
	if (pos >= info->cap.max_num_dp_ports || dump_one_port) {
		tmp_inst++;
		pos = 0;
	}
	if (tmp_inst >= dp_inst_num)
		pos = -1;	/*end of the loop */

	return pos;
}

static
void show_port_info(int inst, u8 pos, int vap_start, int vap_end, u32 flag)
{
	struct pmac_port_info *port = get_dp_port_info(inst, pos);
	u16 start;
	int i;

	if (port->status == PORT_FREE) {
		if (!pos)
			pr_info("%s:rx_err_drop=0x%08x  tx_err_drop=0x%08x\n",
				"Reserved Port",
				STATS_GET(port->rx_err_drop),
				STATS_GET(port->tx_err_drop));

		else
			pr_info("%02d:rx_err_drop=0x%08x  tx_err_drop=0x%08x\n",
				pos,
				STATS_GET(port->rx_err_drop),
				STATS_GET(port->tx_err_drop));

		return;
	}

	DP_CB(tmp_inst, get_itf_start_end)(port->itf_info, &start, NULL);

	pr_info("%02d: %s=0x0x%0lx(name:%8s) %s=%02d %s=%02d itf_base=%d(%s)\n",
		pos,
		"module", (uintptr_t)port->owner, port->owner->name,
		"dev_port", port->dev_port, "dp_port", port->port_id,
		start, port->itf_info ? "Enabled" : "Not Enabled");
	pr_info("    status:            %s\n",
		dp_port_status_str[port->status]);
	pr_info("    allocate_flags:    ");

	for (i = 0; i < get_dp_port_type_str_size(); i++) {
		if (port->alloc_flags & dp_port_flag[i])
			pr_info("%s ", dp_port_type_str[i]);
	}

	pr_info("\n");

	if (!flag) {
		pr_info("    cb->rx_fn:         0x%0lx\n",
			(uintptr_t)port->cb.rx_fn);
		pr_info("    cb->restart_fn:    0x%0lx\n",
			(uintptr_t)port->cb.restart_fn);
		pr_info("    cb->stop_fn:       0x%0lx\n",
			(uintptr_t)port->cb.stop_fn);
		pr_info("    cb->get_subifid_fn:0x%0lx\n",
			(uintptr_t)port->cb.get_subifid_fn);
		pr_info("    num_subif:         %02d\n", port->num_subif);
	}

	for (i = vap_start; i < vap_end; i++) {
		struct dp_subif_info *sif = get_dp_port_subif(port, i);
		struct dev_mib *mib = get_dp_port_subif_mib(sif);

		if (sif->flags) {
			pr_info("      [%02d]:%s=0x%04x %s=0x%0lx(%s=%s),%s=%s\n",
				i, "subif", sif->subif,
				"netif", (uintptr_t)sif->netif,
				"device_name",
				sif->netif ? sif->netif->name : "NULL/DSL",
				"name", sif->device_name);
			pr_info("          : rx_fn_rxif_pkt =0x%08x\n",
				STATS_GET(mib->rx_fn_rxif_pkt));
			pr_info("          : rx_fn_txif_pkt =0x%08x\n",
				STATS_GET(mib->rx_fn_txif_pkt));
			pr_info("          : rx_fn_dropped  =0x%08x\n",
				STATS_GET(mib->rx_fn_dropped));
			pr_info("          : tx_cbm_pkt     =0x%08x\n",
				STATS_GET(mib->tx_cbm_pkt));
			pr_info("          : tx_tso_pkt     =0x%08x\n",
				STATS_GET(mib->tx_tso_pkt));
			pr_info("          : tx_pkt_dropped =0x%08x\n",
				STATS_GET(mib->tx_pkt_dropped));
			pr_info("          : tx_clone_pkt   =0x%08x\n",
				STATS_GET(mib->tx_clone_pkt));
			pr_info("          : tx_hdr_room_pkt=0x%08x\n",
				STATS_GET(mib->tx_hdr_room_pkt));
		}
	}

	pr_info("    rx_err_drop=0x%08x  tx_err_drop=0x%08x\n",
		STATS_GET(port->rx_err_drop), STATS_GET(port->tx_err_drop));
}

static int set_switchdev(int inst, int ep, int vap, bool en)
{
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_SWITCHDEV)
	struct pmac_port_info *port = get_dp_port_info(inst, ep);
	struct dp_subif_info *sif = get_dp_port_subif(port, vap);
	struct net_device *dev = sif->netif;
	struct net_device *br_dev;
	struct dp_dev *dp_dev;

	if (port->status != PORT_SUBIF_REGISTERED || !sif->flags) {
		pr_err("subif not registered\n");
		return DP_FAILURE;
	}

	if (!sif->swdev_en == !en)
		return DP_SUCCESS;

	dp_dev = dp_dev_lookup(dev);
	if (!dp_dev) {
		pr_err("dp_dev lookup failed\n");
		return DP_FAILURE;
	}

	rtnl_lock();
	br_dev = netdev_master_upper_dev_get(dev);
	rtnl_unlock();

	if (en) {
		sif->swdev_en = 1;
		if (dp_register_switchdev_ops(dp_dev->dev, 0)) {
			pr_err("fail to register swdev ops\n");
			return DP_FAILURE;
		}
		pr_info("swdev is enabled\n");
		if (br_dev) {
			pr_info("master upper device (bridge?) detected\n");
			pr_info("please reinit network for switchdev to be effective\n");
		}
	} else {
		if (br_dev) {
			pr_err("master upper device (bridge?) detected\n");
			pr_err("please remove link to upper dev before disabling switchdev\n");
			return DP_FAILURE;
		}
		if (dp_register_switchdev_ops(dp_dev->dev, 1)) {
			pr_err("fail to deregister swdev ops\n");
			return DP_FAILURE;
		}
		sif->swdev_en = 0;
		pr_info("swdev is disabled\n");
	}
#endif
	return DP_SUCCESS;
}

ssize_t proc_port_write(struct file *file, const char *buf, size_t count,
			loff_t *ppos)
{
	int inst, i, num, idx_s,  idx_e, vap_s, vap_e, swdev_en;
	struct inst_info *info = &dp_port_prop[0].info;
	struct pmac_port_info *port;
	char *param_list[10], *str;

	if (!capable(CAP_SYS_PACCT))
		return count;

	str = kzalloc(count, GFP_ATOMIC);
	if (!str)
		return -ENOMEM;

	if (copy_from_user(str, buf, count))
		return -EFAULT;

	for (num = 0; num < ARRAY_SIZE(param_list); num++) {
		param_list[num] = strsep(&str, " \n");
		if (!param_list[num])
			break;
	}

	if (param_list[1]) {
		idx_s = dp_atoi(param_list[1]);
		if (idx_s >= info->cap.max_num_dp_ports) {
			pr_err("invalid port index, expect 0 ~ %d\n",
			       info->cap.max_num_dp_ports - 1);
			return count;
		}
		idx_e = idx_s + 1;
	} else {
		idx_s = 0;
		idx_e = info->cap.max_num_dp_ports;
	}

	if (param_list[2]) {
		vap_s = dp_atoi(param_list[2]);
		if (vap_s >= info->cap.max_num_subif_per_port) {
			pr_err("invalid VAP index, expect 0 ~ %d\n",
			       info->cap.max_num_subif_per_port - 1);
			return count;
		}
		vap_e = vap_s + 1;
	} else {
		vap_s = 0;
		vap_e = info->cap.max_num_subif_per_port;
	}

	if (!strcasecmp(param_list[0], "mib")) {
		for (inst = 0; inst < dp_inst_num; inst++)
			for (i = idx_s; i < idx_e; i++)
				show_port_info(inst, i, vap_s, vap_e, 1);
	} else if (!strcasecmp(param_list[0], "port")) {
		if (proc_write_port != idx_s) {
			pr_info("%s: changed port id to be dumped to %d\n",
				__func__, idx_s);
			proc_write_port = idx_s;
		}

		if (param_list[2] && proc_write_vap != vap_s) {
			pr_info("%s: changed sif id to be dumped to %d\n",
				__func__, vap_s);
			proc_write_vap = vap_s;
		}

		pr_info("%s: run 'cat %s' to dump requested port info\n",
			__func__, DEBUGFS_DBG);
	} else if (!strcasecmp(param_list[0], "swdev") && num == 4) {
		DP_LIB_LOCK(&dp_lock);
		port = get_dp_port_info(0, idx_s);
		if (!port->num_subif) {
			pr_err("subif unavailable for port index %d\n", idx_s);
		} else if (vap_s >= port->num_subif) {
			pr_err("invalid VAP index, expect 0 ~ %d\n",
			       port->num_subif - 1);
		} else {
			swdev_en = dp_atoi(param_list[3]);
			set_switchdev(0, idx_s, vap_s, !!swdev_en);
		}
		DP_LIB_UNLOCK(&dp_lock);
	} else {
		pr_info("usage:\n");
		pr_info("  echo mib   [ep][vap] > %s\n", DBG_PORT);
		pr_info("  echo port  [ep][vap] > %s\n", DBG_PORT);
		pr_info("    to print all port info- echo port -1 > %s\n",
			DBG_PORT);
		pr_info("  echo swdev [ep][vap][swdev_en] > %s\n", DBG_PORT);
	}

	return count;
}

ssize_t proc_ctp_list_write(struct file *file, const char *buf, size_t count,
			    loff_t *ppos)
{
	char *param_list[5], *str;
	int num;
	u16 bp;

	if (!capable(CAP_SYS_PACCT))
		return count;

	str = kzalloc(count, GFP_ATOMIC);
	if (!str)
		return -ENOMEM;

	if (copy_from_user(str, buf, count))
		return -EFAULT;

	for (num = 0; num < ARRAY_SIZE(param_list); num++) {
		param_list[num] = strsep(&str, " \n");
		if (!param_list[num])
			break;
	}

	if (param_list[1])
		bp = dp_atoi(param_list[1]);
	else
		bp = 0;

	if (!strcasecmp(param_list[0], "bp_ctp")) {
		struct ctp_dev *ctp_dev;
		struct bp_pmapper *bp_info;

		bp_info = get_dp_bp_info(0, bp);
		pr_info("%-10s%-10d\n", "Bp", bp);

		if (!bp_info->ref_cnt) {
			pr_info("dev(%s) is not a pmapper\n",
				bp_info->dev ? bp_info->dev->name : "NULL");
			return count;
		}

		list_for_each_entry(ctp_dev, &bp_info->ctp_dev, list) {
			if (!ctp_dev->dev)
				continue;
			pr_info("%-s:%-s(%-s:%-d)\n", "ctp_dev",
				ctp_dev->dev->name, "ctp", ctp_dev->ctp);
		}
	} else {
		pr_info("usage:\n");
		pr_info("  echo bp_ctp [bp] > %s\n", DBG_CTP);
	}

	return count;
}

#if defined(CONFIG_INTEL_DATAPATH_DBG) && CONFIG_INTEL_DATAPATH_DBG
void proc_dbg_read(struct seq_file *s)
{
	int i;

	if (!capable(CAP_SYS_ADMIN))
		return;

	seq_printf(s, "dp_dbg_flag=0x%08x\n", dp_dbg_flag);
	seq_printf(s, "Supported Flags =%d\n", get_dp_dbg_flag_str_size());
	seq_printf(s, "Enabled Flags(0x%0x):", dp_dbg_flag);

	for (i = 0; i < get_dp_dbg_flag_str_size(); i++)
		if ((dp_dbg_flag & dp_dbg_flag_list[i]) == dp_dbg_flag_list[i])
			seq_printf(s, "%s ", dp_dbg_flag_str[i]);

	seq_puts(s, "\n\n");
	seq_printf(s, "dp_drop_all_tcp_err=%d @ 0x%px\n", dp_drop_all_tcp_err,
		   &dp_drop_all_tcp_err);
	seq_printf(s, "dp_pkt_size_check=%d @ 0x%px\n", dp_pkt_size_check,
		   &dp_pkt_size_check);
	seq_printf(s, "dp_rx_test_mode=%d @ 0x%px\n", dp_rx_test_mode,
		   &dp_rx_test_mode);
	seq_printf(s, "dp_dbg_err(flat to print error or not)=%d @ 0x%px\n",
		   dp_dbg_err,
		   &dp_dbg_err);

	print_parser_status(s);
}

ssize_t proc_dbg_write(struct file *file, const char *buf, size_t count,
		       loff_t *ppos)
{
	int i, j, num, dbg_str_list;
	char *param_list[20], *str;
	int en;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	str = kzalloc(count, GFP_ATOMIC);
	if (!str)
		return -ENOMEM;

	if (copy_from_user(str, buf, count))
		return -EFAULT;

	for (num = 0; num < ARRAY_SIZE(param_list); num++) {
		param_list[num] = strsep(&str, " \n");
		if (!param_list[num])
			break;
	}

	dbg_str_list = get_dp_dbg_flag_str_size() - 1;

	if (!strcasecmp(param_list[0], "enable")) {
		en = 1;
	} else if (!strcasecmp(param_list[0], "disable")) {
		en = -1;
	} else {
		pr_info("echo <enable/disable> ");
		for (i = 0; i < dbg_str_list; i++)
			pr_cont("%s ", dp_dbg_flag_str[i]);
		pr_cont(" > %s\n", DEBUGFS_DBG);
		goto exit;
	}

	if (!param_list[1]) {
		set_ltq_dbg_flag(dp_dbg_flag, en, -1);
		return count;
	}

	for (i = 1; i < num; i++) {
		for (j = 0; j < dbg_str_list; j++)
			if (!strcasecmp(param_list[i], dp_dbg_flag_str[j])) {
				set_ltq_dbg_flag(dp_dbg_flag, en,
						 dp_dbg_flag_list[j]);
				break;
			}
	}

exit:
	kfree(str);

	return count;
}
#endif

int proc_dpid_dump(struct seq_file *s, int pos)
{
	int tmp_inst = 0;
	struct inst_info *info = get_dp_prop_info(tmp_inst);
	struct pmac_port_info *port;
	struct dp_subif_info *sif;
	u16 ctp_s, ctp_e;
	int i;

	if (!capable(CAP_SYS_PACCT))
		return -1;
	port = get_dp_port_info(tmp_inst, pos);
	DP_CB(tmp_inst, get_itf_start_end)(port->itf_info, &ctp_s, &ctp_e);

	if (pos == 0) {
		seq_printf(s, "%4s %6s %3s %4s %4s %8s %3s %3s %16s\n",
			   "dpid", "subif", "grp", "gpid", "qid", "deq_port",
			   "ctp", "bp", "comment");
		seq_printf(s, "%4s %6s %3s %4s %4s %8s %3s %3s %16s\n",
			   "", "Hex", "Dec", "Dec", "Dec", "Dec",
			   "Dec", "Dec", "");
	}
	for (i = 0; i < port->subif_max; i++) {
		sif = get_dp_port_subif(port, i);
		if (!sif->flags)
			continue;
		seq_printf(s, "%4d %6x %3d ",
			   pos,
			   sif->subif,
			   GET_VAP(sif->subif,port->vap_offset,port->vap_mask));
			   
		if (sif->gpid != (u16)(-1))
			seq_printf(s, "%4d ", sif->gpid);
		else 
			seq_printf(s, "%4s ", "-");

		if (sif->qid) {
			seq_printf(s, "%4u ", sif->qid);

			if (sif->spl_cfg && sif->spl_cfg->num_egp)
				seq_printf(s, "%8u ",
					   sif->spl_cfg->egp[0].egp_id);
			else 
				seq_printf(s, "%8u ", sif->cqm_deq_port[0]);
		} else {
			seq_printf(s, "%4s ", "-");
			seq_printf(s, "%8s ", "-");
		}

		seq_printf(s, "%3u %3u ",
			   (ctp_s + i < ctp_e) ? (ctp_s + i) : ctp_e,
			   sif->bp);
		if (sif->netif)
			seq_printf(s, "%16s ", sif->netif->name);
		else if (sif->spl_cfg)
			seq_printf(s, "%16s ",
				   dp_spl_conn_type[sif->spl_cfg->type]);
		else if (port->alloc_flags & DP_F_CPU)
			seq_printf(s, "%16s ",
				   dp_port_type[sif->type]);
		else if (port->alloc_flags & DP_F_FAST_DSL)
			seq_printf(s, "%16s ", "DSL");
		else
			seq_printf(s, "%16s ", "-");

		seq_puts(s, "\n");
	}
	if (port->gpid_spl > 0) {
		seq_printf(s, "%4d %6s %3s %4u %4s %8s %3s %3s %16s\n",
			   pos,
			   "-",
			   "-",
			   port->gpid_spl,
			   "-",
			   "-",
			   "-",
			   "-",
			   "SPL_GPID");
	}
	if (seq_has_overflowed(s))
		return pos;
	pos++;
	if (pos >= info->cap.max_num_dp_ports) {
		pos = -1; /* end of the loop */
		/* print title again in order to avoid scroll the console */
		seq_printf(s, "%4s %6s %3s %4s %4s %8s %3s %3s %16s\n",
			   "dpid", "subif", "grp", "gpid", "qid", "deq_port",
			   "ctp", "bp", "comment");
	}

	return pos;
}

static struct dp_proc_entry dp_proc_entries[] = {
	/* name|single_callback|multi_callback|init_callback|write_callback */
#if defined(CONFIG_INTEL_DATAPATH_DBG) && CONFIG_INTEL_DATAPATH_DBG
	{DBGFS_DBG, proc_dbg_read, NULL, NULL, proc_dbg_write},
#endif
	{DBGFS_PORT, NULL, proc_port_dump, proc_port_init, proc_port_write},
	{DBGFS_INST_DEV, NULL, proc_inst_dev_dump, proc_inst_dev_start, NULL},
	{DBGFS_INST_MOD, NULL, proc_inst_mod_dump, proc_inst_mod_start, NULL},
	{DBGFS_INST_HAL, NULL, proc_inst_hal_dump, NULL, NULL},
	{DBGFS_INST, NULL, proc_inst_dump, NULL, NULL},
	{DBGFS_EVENT, NULL, proc_dp_event_list_dump, NULL, NULL},
	{DBGFS_MIB_GLOBAL, NULL, proc_mib_stats_dump, NULL, proc_mib_stats_write},
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_SWITCHDEV)
	{DBGFS_BR_VLAN, NULL, proc_brvlan_dump, NULL, NULL},
#endif
	{DBGFS_CTP_LIST, NULL, NULL, NULL, proc_ctp_list_write},
	{DBGFS_PCE, NULL, proc_pce_dump, NULL, proc_pce_write},
	{DBGFS_DPID, NULL, proc_dpid_dump, NULL, NULL},
	/* last place holder */
	{NULL, NULL, NULL, NULL, NULL}
};

struct dentry *dp_proc_node;
EXPORT_SYMBOL(dp_proc_node);

struct dentry *dp_proc_install(void)
{
	dp_proc_node = debugfs_create_dir(DP_DBGFS_NAME, NULL);

	if (dp_proc_node) {
		int i;

		for (i = 0; i < ARRAY_SIZE(dp_proc_entries); i++)
			dp_proc_entry_create(dp_proc_node,
					     &dp_proc_entries[i]);
	} else {
		pr_err("datapath cannot create proc entry");
		return NULL;
	}

	return dp_proc_node;
}
