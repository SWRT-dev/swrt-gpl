/*
 * Copyright (C) Intel Corporation
 * Author: Shao Guohua <guohua.shao@intel.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#include <linux/module.h>
#include <net/datapath_proc_api.h>	/*for proc api */
#include <net/datapath_api.h>
#include <net/datapath_api_vlan.h>
#include <linux/version.h>

#include <linux/list.h>
#include <linux/of_device.h>
#include <linux/of_platform.h>
#include "datapath.h"
#include "datapath_instance.h"

/*meter alloc,add macros*/
#define DP_METER_ALLOC(inst, id, flag) \
	dp_port_prop[(inst)].info.dp_meter_alloc(inst, &(id), (flag))
#define DP_METER_CFGAPI(inst, func, dev, meter, flag, mtr_subif) \
	dp_port_prop[(inst)].info.func((dev), &(meter), (flag), (mtr_subif))

#define DP_PROC_NAME       "dp"
#define DP_PROC_BASE       "/proc/" DP_PROC_NAME "/"
#define DP_PROC_PARENT     ""

#define PROC_DBG   "dbg"
#define PROC_PORT   "port"
#define PROC_INST_DEV "inst_dev"
#define PROC_INST_MOD "inst_mod"
#define PROC_INST_HAL "inst_hal"
#define PROC_INST "inst"
#define PROC_QOS  "qos"

static int tmp_inst;
static ssize_t proc_port_write(struct file *file, const char *buf,
			       size_t count, loff_t *ppos);
#if defined(CONFIG_INTEL_DATAPATH_DBG) && CONFIG_INTEL_DATAPATH_DBG
static void proc_dbg_read(struct seq_file *s);
static ssize_t proc_dbg_write(struct file *, const char *, size_t, loff_t *);
#endif
static int proc_port_dump(struct seq_file *s, int pos);
static int proc_port_init(void);

int proc_port_init(void)
{
	tmp_inst = 0;
	return 0;
}

int dump_dc_info(struct seq_file *s, struct pmac_port_info *port)
{
	int i = 0;
	u32 cid, pid, nid;

	for (i = 0; i < port->num_tx_ring; i++) {
		seq_printf(s, "    DC TxRing:         %d\n", i);
		seq_printf(s, "      TXIN  DeqRingSize/paddr:      %d/0x%p\n",
			   port->tx_ring[i].in_deq_ring_size,
			   port->tx_ring[i].in_deq_paddr);
		seq_printf(s, "      TXOUT FreeRingSize/paddr:     %d/0x%p\n",
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
		seq_printf(s, "      RXOUT EnqRingSize/paddr/pid:  %d/0x%p/%d\n",
			   port->rx_ring[i].out_enq_ring_size,
			   port->rx_ring[i].out_enq_paddr,
			   port->rx_ring[i].out_enq_port_id);
		seq_printf(s, "      RXOUT NumOfDmaCh:             %d\n",
			   port->rx_ring[i].num_out_tx_dma_ch);
		cid = _DMA_CONTROLLER(port->rx_ring[i].out_dma_ch_to_gswip);
		pid = _DMA_PORT(port->rx_ring[i].out_dma_ch_to_gswip);
		nid = _DMA_CHANNEL(port->rx_ring[i].out_dma_ch_to_gswip);
		seq_printf(s, "      RXOUT dma-ctrl/port/chan:     %d/%d/%d\n",
			   cid, pid, nid);
		seq_printf(s, "      RXOUT NumOfCqmDqPort/pid:     %d/%d\n",
			   port->rx_ring[i].num_out_cqm_deq_port,
			   port->rx_ring[i].out_cqm_deq_port_id);
		seq_printf(s, "      RXIN  InAllocRingSize/Paddr:  %d/0x%p\n",
			   port->rx_ring[i].in_alloc_ring_size,
			   port->rx_ring[i].in_alloc_paddr);
		seq_printf(s, "      NumPkt/Pktsize/Policybase:    %d/%d/%d\n",
			   port->rx_ring[i].num_pkt,
			   port->rx_ring[i].rx_pkt_size,
			   port->rx_ring[i].rx_policy_base);
		seq_printf(s, "      PreFillPktNum/PktBase:        %d/0x%p\n",
			   port->rx_ring[i].prefill_pkt_num,
			   port->rx_ring[i].pkt_base_paddr);
	}
#if !IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP30)
	seq_printf(s, "    UMT id/CqmDeqPid/msg_mode:      %d/%d/%d\n",
		   port->umt.ctl.id,
		   port->umt.res.cqm_dq_pid,
		   port->umt.ctl.msg_mode);
	seq_printf(s, "    UMT period/daddr:               %d/0x%08x\n",
		   port->umt.ctl.msg_interval,
		   port->umt.ctl.daddr);
#endif

	return 0;
}

int proc_port_dump(struct seq_file *s, int pos)
{
	int i, j;
	int cqm_p;
	int (*print_ctp_bp)(struct seq_file *s, int inst,
			    struct pmac_port_info *port,
			    int subif_index, u32 flag);
	struct pmac_port_info *port = get_dp_port_info(tmp_inst, pos);
	u16 start = 0;
	u32 cid, pid, nid;
	int loop;
	struct inst_info *info = NULL;
	struct dma_chan_info *dma;
	struct cqm_port_info *cqm;

	if (!capable(CAP_SYS_PACCT))
		return -1;
	if (!port) {
		pr_err("Why port is NULL\n");
		return -1;
	}
	info = &dp_port_prop[tmp_inst].info;
	print_ctp_bp = DP_CB(tmp_inst, proc_print_ctp_bp_info);
	DP_CB(tmp_inst, get_itf_start_end)(port->itf_info, &start, NULL);

	if (port->status == PORT_FREE) {
		if (pos == 0) {
			struct dp_subif_info *sif = get_dp_port_subif(port, 0);

			seq_printf(s, "Reserved Port: rx_err_drop=0x%08x  tx_err_drop=0x%08x\n",
				   STATS_GET(port->rx_err_drop),
				   STATS_GET(port->tx_err_drop));
			if (print_ctp_bp && pos) /* just to print bridge
						  * port zero's member.
						  * CPU port no ctp/bridge port
						  */
				print_ctp_bp(s, tmp_inst, port, 0, 0);
			seq_printf(s, "           qid/node:       %d/%d\n",
				   sif->qid, sif->q_node[0]);
			seq_printf(s, "           port/node:      %d/%d\n",
				   sif->cqm_deq_port[0], sif->qos_deq_port[0]);
		} else
			seq_printf(s, "%02d: rx_err_drop=0x%08x  tx_err_drop=0x%08x\n",
				   pos, STATS_GET(port->rx_err_drop),
				   STATS_GET(port->tx_err_drop));

		goto EXIT;
	}

	seq_printf(s, "%02d:%s=0x%0lx(%s:%8s) %s=%02d %s=%02d %s=%d(%s) %s=%d\n",
		   pos, "module", (uintptr_t)port->owner,
		   "name", port->owner->name,
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
	seq_printf(s, "    mode:              %d\n", port->cqe_lu_mode);
	seq_printf(s, "    LCT:               %d\n", port->lct_idx);
	seq_printf(s, "    subif_max:         %d\n", port->subif_max);
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_SWITCHDEV)
	seq_printf(s, "    Swdev:             %d\n", port->swdev_en);
#endif
	seq_printf(s, "    cb->rx_fn:         0x%px\n", port->cb.rx_fn);
	seq_printf(s, "    cb->restart_fn:    0x%px\n", port->cb.restart_fn);
	seq_printf(s, "    cb->stop_fn:       0x%px\n", port->cb.stop_fn);
	seq_printf(s, "    cb->get_subifid_fn:0x%px\n",
		   port->cb.get_subifid_fn);
	seq_printf(s, "    num_subif:         %d\n", port->num_subif);
	seq_printf(s, "    vap_offset/mask:   %d/0x%x\n", port->vap_offset,
		   port->vap_mask);
	seq_printf(s, "    flag_other:        0x%x\n", port->flag_other);
	seq_printf(s, "    resv_queue:        %d\n", port->num_resv_q);
	seq_printf(s, "    resv_queue_base:   %d\n", port->res_qid_base);
	seq_printf(s, "    deq_port_base:     %d\n", port->deq_port_base);
	seq_printf(s, "    deq_port_num:      %d\n", port->deq_port_num);
	seq_printf(s, "    num_dma_chan:      %d\n", port->num_dma_chan);
	seq_printf(s, "    dma_chan:          0x%x\n", port->dma_chan);
	seq_printf(s, "    gpid_base:         %d\n", port->gpid_base);
	seq_printf(s, "    gpid_num:          %d\n", port->gpid_num);
	seq_printf(s, "    policy_base:       %d\n", port->policy_base);
	seq_printf(s, "    policy_num:        %d\n", port->policy_num);
	seq_printf(s, "    tx_pkt_credit:     %d\n", port->tx_pkt_credit);
	seq_printf(s, "    tx_b_credit:       %02d\n", port->tx_b_credit);
	seq_printf(s, "    txpush_addr:       0x%px\n", port->txpush_addr);
	seq_printf(s, "    txpush_addr_qos:   0x%px\n", port->txpush_addr_qos);
	seq_printf(s, "    tx_ring_size:      %d\n", port->tx_ring_size);
	seq_printf(s, "    tx_ring_offset:    %d(to next dequeue port)\n",
		   port->tx_ring_offset);

	if (port->num_rx_ring || port->num_tx_ring)
		dump_dc_info(s, port);

	if (pos == 0)
		loop = info->cap.max_num_subif_per_port;
	else
		loop = port->subif_max;
	for (i = 0; i < loop; i++) {
		struct dp_subif_info *sif = get_dp_port_subif(port, i);
		struct dev_mib *mib = get_dp_port_subif_mib(sif);

		if (!sif->flags)
			continue;
		seq_printf(s, "      [%02d]:%s=0x%04x %s=0x%0lx(%s=%s),%s=%s\n",
			   i, "subif", sif->subif,
			   "netif", (uintptr_t)sif->netif,
			   "netif", sif->netif ? sif->netif->name : "NULL/DSL",
			   "device_name", sif->device_name);

		seq_puts(s, "           subif_flag:     ");
		for (j = 0; j < get_dp_port_type_str_size(); j++) {
			if (sif->subif_flag & dp_port_flag[j]) {
				seq_puts(s, dp_port_type_str[j]);
				seq_puts(s, " ");
			}
		}
		seq_puts(s, "\n");
		seq_printf(s, "           rx_fn_rxif_pkt =0x%08x\n",
			   STATS_GET(mib->rx_fn_rxif_pkt));
		seq_printf(s, "           rx_fn_txif_pkt =0x%08x\n",
			   STATS_GET(mib->rx_fn_txif_pkt));
		seq_printf(s, "           rx_fn_dropped  =0x%08x\n",
			   STATS_GET(mib->rx_fn_dropped));
		seq_printf(s, "           tx_cbm_pkt     =0x%08x\n",
			   STATS_GET(mib->tx_cbm_pkt));
		seq_printf(s, "           tx_tso_pkt     =0x%08x\n",
			   STATS_GET(mib->tx_tso_pkt));
		seq_printf(s, "           tx_pkt_dropped =0x%08x\n",
			   STATS_GET(mib->tx_pkt_dropped));
		seq_printf(s, "           tx_clone_pkt   =0x%08x\n",
			   STATS_GET(mib->tx_clone_pkt));
		seq_printf(s, "           tx_hdr_room_pkt=0x%08x\n",
			   STATS_GET(mib->tx_hdr_room_pkt));
		if (print_ctp_bp)
			print_ctp_bp(s, tmp_inst, port, i, 0);
		seq_printf(s, "           subif_qid:      %d\n", sif->num_qid);
		seq_printf(s, "           dqport_idx:     %d\n",
			   sif->cqm_port_idx);
		for (j = 0; j < sif->num_qid; j++) {
			seq_printf(s, "%13s[%02d]:qid/node:    %d/%d\n", "",
				   j, sif->qid_list[j], sif->q_node[j]);
			cqm_p = sif->cqm_deq_port[j];
			cqm = &dp_deq_port_tbl[tmp_inst][cqm_p];
			seq_printf(s, "%18sport/node:   %d/%d(ref=%d)\n", "",
				   cqm_p, sif->qos_deq_port[j], cqm->ref_cnt);
			cid = _DMA_CONTROLLER(cqm->dma_chan);
			pid = _DMA_PORT(cqm->dma_chan);
			nid = _DMA_CHANNEL(cqm->dma_chan);
			dma = dp_dma_chan_tbl[tmp_inst];
			if (port->num_dma_chan && dma) {
				dma += cqm->dma_ch_offset;
				seq_printf(s, "%18stx_dma_ch:   0x%x(ref=%d,dma-ctrl=%d,port=%d,channel=%d)\n",
					   "", cqm->dma_chan,
					   atomic_read(&dma->ref_cnt),
					   cid, pid, nid);
			}
		}
		seq_printf(s, "           mac_learn_dis:  %d\n",
			   sif->mac_learn_dis);
		seq_printf(s, "           gpid:           %d\n", sif->gpid);
		seq_puts(s, "           ctp_dev:        ");
		if (sif->ctp_dev && sif->ctp_dev->name)
			seq_puts(s, sif->ctp_dev->name);
		else
			seq_puts(s, "NULL");
		seq_puts(s, "\n");
		seq_printf(s, "           rx_en_flag:     %d\n",
			   STATS_GET(sif->rx_flag));
	}
	seq_printf(s, "    rx_err_drop=0x%08x  tx_err_drop=0x%08x\n",
		   STATS_GET(port->rx_err_drop),
		   STATS_GET(port->tx_err_drop));
EXIT:
	if (!seq_has_overflowed(s))
		pos++;
	if (pos >= MAX_DP_PORTS) {
		tmp_inst++;
		pos = 0;
	}
	if (tmp_inst >= dp_inst_num)
		pos = -1;	/*end of the loop */
	return pos;
}

int display_port_info(int inst, u8 pos, int start_vap, int end_vap, u32 flag)
{
	int i;
	int ret;
	struct pmac_port_info *port = get_dp_port_info(inst, pos);
	u16 start = 0;

	if (!port) {
		pr_err("Why port is NULL\n");
		return -1;
	}

	if (port->status == PORT_FREE) {
		if (pos == 0) {
			pr_info("%s:rx_err_drop=0x%08x  tx_err_drop=0x%08x\n",
				"Reserved Port",
				STATS_GET(port->rx_err_drop),
				STATS_GET(port->tx_err_drop));

		} else
			pr_info("%02d:rx_err_drop=0x%08x  tx_err_drop=0x%08x\n",
				pos,
				STATS_GET(port->rx_err_drop),
				STATS_GET(port->tx_err_drop));

		goto EXIT;
	}

	DP_CB(tmp_inst, get_itf_start_end)(port->itf_info, &start, NULL);

	pr_info("%02d: %s=0x0x%0lx(name:%8s) %s=%02d %s=%02d itf_base=%d(%s)\n",
		pos,
		"module",
		(uintptr_t)port->owner, port->owner->name,
		"dev_port",
		port->dev_port,
		"dp_port",
		port->port_id,
		start,
		port->itf_info ? "Enabled" : "Not Enabled");
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

	for (i = start_vap; i < end_vap; i++) {
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

	ret = pr_info("    rx_err_drop=0x%08x  tx_err_drop=0x%08x\n",
		      STATS_GET(port->rx_err_drop),
		      STATS_GET(port->tx_err_drop));
EXIT:
	return 0;
}

ssize_t proc_port_write(struct file *file, const char *buf, size_t count,
			loff_t *ppos)
{
	int len;
	char str[64];
	int num, i;
	/*later need to put real inst value */
	struct inst_info *info = &dp_port_prop[0].info;
	u8 index_start = 0;
	u8 index_end = MAX_DP_PORTS;
	int vap_start = 0;
	int vap_end = info->cap.max_num_subif_per_port;
	char *param_list[10];
	int inst;

	if (!capable(CAP_SYS_PACCT))
		return count;
	len = (sizeof(str) > count) ? count : sizeof(str) - 1;
	len -= copy_from_user(str, buf, len);
	str[len] = 0;
	num = dp_split_buffer(str, param_list, ARRAY_SIZE(param_list));

	if (num <= 1)
		goto help;
	if (param_list[1]) {
		index_start = dp_atoi(param_list[1]);
		index_end = index_start + 1;
	}

	if (param_list[2]) {
		vap_start = dp_atoi(param_list[2]);
		vap_end = vap_start + 1;
	}

	if (index_start >= MAX_DP_PORTS) {
		pr_err("wrong index: 0 ~ 15\n");
		return count;
	}

	if (vap_start >= info->cap.max_num_subif_per_port) {
		pr_err("wrong VAP: 0 ~ 15\n");
		return count;
	}

	if (dp_strncmpi(param_list[0], "mib", strlen("mib")) == 0) {
		for (inst = 0; inst < dp_inst_num; inst++)
		for (i = index_start; i < index_end; i++)
			display_port_info(inst, i, vap_start, vap_end, 1);

	} else if (dp_strncmpi(param_list[0], "port", strlen("port")) == 0) {
		for (inst = 0; inst < dp_inst_num; inst++)
		for (i = index_start; i < index_end; i++)
			display_port_info(inst, i, vap_start, vap_end, 0);

	} else {
		goto help;
	}

	return count;
 help:
	pr_info("usage:\n");
	pr_info("  echo mib  [ep][vap] > /prooc/dp/port\n");
	pr_info("  echo port [ep][vap] > /prooc/dp/port\n");
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
		if ((dp_dbg_flag & dp_dbg_flag_list[i]) ==
		    dp_dbg_flag_list[i])
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
	int len, i, j;
	char str[64];
	int num;
	char *param_list[20];
	int f_enable;
	char *tmp_buf;
	#define BUF_SIZE_TMP 2000
	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	len = (sizeof(str) > count) ? count : sizeof(str) - 1;
	len -= copy_from_user(str, buf, len);
	str[len] = 0;
	num = dp_split_buffer(str, param_list, ARRAY_SIZE(param_list));

	if (dp_strncmpi(param_list[0], "enable", strlen("enable")) == 0)
		f_enable = 1;
	else if (dp_strncmpi(param_list[0], "disable", strlen("disable")) == 0)
		f_enable = -1;
	else
		goto help;

	if (!param_list[1]) {	/*no parameter after enable or disable */
		set_ltq_dbg_flag(dp_dbg_flag, f_enable, -1);
		goto EXIT;
	}

	for (i = 1; i < num; i++) {
		for (j = 0; j < get_dp_dbg_flag_str_size() - 1; j++)
			if (dp_strncmpi(param_list[i],
					dp_dbg_flag_str[j],
					strlen(dp_dbg_flag_str[j]) + 1) == 0) {
				set_ltq_dbg_flag(dp_dbg_flag, f_enable,
						 dp_dbg_flag_list[j]);
				break;
			}
	}

EXIT:
	return count;
help:
	tmp_buf = kmalloc(BUF_SIZE_TMP, GFP_KERNEL);
	if (!tmp_buf)
		return count;
	num = 0;
	num = snprintf(tmp_buf + num, BUF_SIZE_TMP - num - 1,
		       "echo <enable/disable> ");
	for (i = 0; i < get_dp_dbg_flag_str_size(); i++)
		num += snprintf(tmp_buf + num, BUF_SIZE_TMP - num - 1,
				"%s ", dp_dbg_flag_str[i]);

	num += snprintf(tmp_buf + num, BUF_SIZE_TMP - num - 1,
			" > /proc/dp/dbg\n");
	num += snprintf(tmp_buf + num, BUF_SIZE_TMP - num - 1,
			" display command: cat /proc/dp/cmd\n");
	pr_info("%s", tmp_buf);
	kfree(tmp_buf);
	return count;
}
#endif

static struct dp_proc_entry dp_proc_entries[] = {
	/*name single_callback_t multi_callback_t/_start write_callback_t */
#if defined(CONFIG_INTEL_DATAPATH_DBG) && CONFIG_INTEL_DATAPATH_DBG
	{PROC_DBG, proc_dbg_read, NULL, NULL, proc_dbg_write},
#endif
	{PROC_PORT, NULL, proc_port_dump, proc_port_init, proc_port_write},
	{PROC_INST_DEV, NULL, proc_inst_dev_dump, proc_inst_dev_start, NULL},
	{PROC_INST_MOD, NULL, proc_inst_mod_dump, proc_inst_mod_start, NULL},
	{PROC_INST_HAL, NULL, proc_inst_hal_dump, NULL, NULL},
	{PROC_INST, NULL, proc_inst_dump, NULL, NULL},
	{PROC_QOS, NULL, qos_dump, qos_dump_start, proc_qos_write},

	/*the last place holder */
	{NULL, NULL, NULL, NULL, NULL}
};

struct dentry *dp_proc_node;
EXPORT_SYMBOL(dp_proc_node);

struct dentry *dp_proc_install(void)
{
	dp_proc_node = debugfs_create_dir(DP_PROC_PARENT DP_PROC_NAME, NULL);

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

