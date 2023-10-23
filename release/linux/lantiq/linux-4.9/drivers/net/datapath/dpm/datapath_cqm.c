// SPDX-License-Identifier: GPL-2.0
/*****************************************************************************
 * Copyright (c) 2020 - 2021, MaxLinear, Inc.
 * Copyright 2016 - 2020 Intel Corporation
 *
 *****************************************************************************/

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/etherdevice.h>
#include <net/datapath_api.h>
#include "datapath.h"

void print_cbm_alloc_data(struct cbm_dp_alloc_data *data)
{
	pr_info("===========================\n");
	pr_info("dp_inst                = %d\n", data->dp_inst);
	pr_info("cbm_inst               = %d\n", data->cbm_inst);
	pr_info("flags                  = %d\n", data->flags);
	pr_info("dp_port                = %d\n", data->dp_port);
	pr_info("deq_port_num           = %d\n", data->deq_port_num);
	pr_info("deq_port               = %d\n", data->deq_port);
	pr_info("dma_chan               = %08x\n", data->dma_chan);
	pr_info("tx_pkt_credit          = %d\n", data->tx_pkt_credit);
	pr_info("tx_b_credit            = %d\n", data->tx_b_credit);
	pr_info("txpush_addr_qos        = %px\n", (void *)data->txpush_addr_qos);
	pr_info("txpush_addr            = %px\n", (void *)data->txpush_addr);
	pr_info("tx_ring_size           = %d\n", data->tx_ring_size);
	pr_info("tx_ring_offset         = %d\n", data->tx_ring_offset);
	pr_info("num_dma_chan           = %d\n", data->num_dma_chan);
	pr_info("\n\n");
}

void print_cbm_en_data(struct cbm_dp_en_data *data)
{
	pr_info("===========================\n");
	pr_info("dp_inst                = %d\n", data->dp_inst);
	pr_info("cbm_inst               = %d\n", data->cbm_inst);
	pr_info("deq_port               = %d\n", data->deq_port);
	pr_info("dma_chnl_init          = %d\n", data->dma_chnl_init);
	pr_info("f_policy               = %d\n", data->f_policy);
	pr_info("tx_max_pkt_size        = %d\n", data->tx_max_pkt_size[0]);
	pr_info("tx_policy_base         = %d\n", data->tx_policy_base);
	pr_info("tx_policy_num          = %d\n", data->tx_policy_num);
	pr_info("tx_ring_size           = %d\n", data->tx_ring_size);
	pr_info("rx_policy_base         = %d\n", data->rx_policy_base);
	pr_info("rx_policy_num          = %d\n", data->rx_policy_num);
	pr_info("num_dma_chan           = %d\n", data->num_dma_chan);
	pr_info("\n\n");
}

int dump_ring_info(struct cbm_dp_alloc_complete_data *data)
{
	u8 cid, pid;
	int i = 0;
	u16 nid;

	for (i = 0; i < data->num_tx_ring; i++) {
		pr_info("    DC TxRing:         %d\n", i);

		if (data->tx_ring[i]) {
			pr_info("      TXIN  DeqRingSize/paddr:     %d/0x%px\n",
				data->tx_ring[i]->in_deq_ring_size,
				data->tx_ring[i]->in_deq_paddr);
			pr_info("      TXOUT FreeRingSize/paddr:    %d/0x%px\n",
				data->tx_ring[i]->out_free_ring_size,
				data->tx_ring[i]->out_free_paddr);
			pr_info("      TXOUT PolicyBase/Poolid:     %d/%d\n",
				data->tx_ring[i]->txout_policy_base,
				data->tx_ring[i]->tx_poolid);
			pr_info("      PolicyNum:                   %d\n",
				data->tx_ring[i]->policy_num);
			pr_info("      NumOfTxPkt/TxPktSize:        %d/%d\n",
				data->tx_ring[i]->num_tx_pkt,
				data->tx_ring[i]->tx_pkt_size);
		}
	}

	for (i = 0; i < data->num_rx_ring; i++) {
		pr_info("    DC RxRing:         %d\n", i);

		if (data->rx_ring[i]) {
			pr_info("      %s:  %d/0x%px/%d\n",
				"RXOUT EnqRingSize/paddr/pid",
				data->rx_ring[i]->out_enq_ring_size,
				data->rx_ring[i]->out_enq_paddr,
				data->rx_ring[i]->out_enq_port_id);
			pr_info("      %s:             %d\n",
				"RXOUT NumOfDmaCh",
				data->rx_ring[i]->num_out_tx_dma_ch);
			dp_dma_parse_id(data->rx_ring[i]->out_dma_ch_to_gswip,
					&cid, &pid, &nid);
			pr_info("      %s:     %d/%d/%d\n",
				"RXOUT dma-ctrl/port/chan",
				cid, pid, nid);
			pr_info("      RXOUT NumOfCqmDqPort/pid:     %d/%d\n",
				data->rx_ring[i]->num_out_cqm_deq_port,
				data->rx_ring[i]->out_cqm_deq_port_id);
			pr_info("      RXOUT Poolid:		    %d\n",
				data->rx_ring[i]->rx_poolid);
			pr_info("      %s:  %d/0x%px\n",
				"RXIN  InAllocRingSize/Paddr",
				data->rx_ring[i]->in_alloc_ring_size,
				data->rx_ring[i]->in_alloc_paddr);
			pr_info("      %s:    %d/%d/%d\n",
				"NumPkt/Pktsize/Policybase",
				data->rx_ring[i]->num_pkt,
				data->rx_ring[i]->rx_pkt_size,
				data->rx_ring[i]->rx_policy_base);
			pr_info("      %s:        %d/0x%px\n",
				"PreFillPktNum/PktBase",
				data->rx_ring[i]->prefill_pkt_num,
				data->rx_ring[i]->pkt_base_paddr);
		}
	}

	return 0;
}

void print_cqm_alloc_comp_data(struct cbm_dp_alloc_complete_data *data)
{
	pr_info("===========================\n");
	pr_info("num_rx_ring                = %d\n", data->num_rx_ring);
	pr_info("num_tx_ring                = %d\n", data->num_tx_ring);
	pr_info("num_umt_port               = %d\n", data->num_umt_port);
	pr_info("enable_cqm_meta            = %d\n", data->enable_cqm_meta);
	pr_info("alloc_flags                = %d\n", data->alloc_flags);
	pr_info("deq_port                   = %d\n", data->deq_port);
	pr_info("qid_base                   = %d\n", data->qid_base);
	pr_info("num_qid                    = %d\n", data->num_qid);
	pr_info("f_min_pkt_len              = %d\n",
		data->gpid_info.f_min_pkt_len);
	pr_info("seg_en                     = %d\n", data->gpid_info.seg_en);
	pr_info("min_pkt_len                = %d\n",
		data->gpid_info.min_pkt_len);
	pr_info("\n\n");
	dump_ring_info(data);
	pr_info("\n\n");
}

void print_cqm_deq_res(cbm_dq_port_res_t *res)
{
	int i = 0;
	cbm_dq_info_t	*deq_info;

	pr_info("===========================\n");
	pr_info("cbm_buf_free_base      = %px\n", res->cbm_buf_free_base);
	pr_info("num_free_entries       = %d\n", res->num_free_entries);
	pr_info("num_deq_ports          = %d\n", res->num_deq_ports);

	for (i = 0; i < res->num_deq_ports; i++) {
		deq_info = &res->deq_info[i];
		pr_info("deq port               = %d\n", i);
		pr_info("port_no                = %d\n", deq_info->port_no);
		pr_info("cbm_dq_port_base       = %px\n",
			deq_info->cbm_dq_port_base);
		pr_info("dma_tx_chan            = %d\n", deq_info->dma_tx_chan);
		pr_info("num_desc               = %d\n", deq_info->num_desc);
		pr_info("num_free_burst         = %d\n",
			deq_info->num_free_burst);
	}
	pr_info("\n\n");
}

int dp_dealloc_cqm_port(struct module *owner, u32 dev_port,
			struct pmac_port_info *port,
			struct cbm_dp_alloc_data *data, u32 flags)
{
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_DBG)
	if (dp_dbg_flag & DP_DBG_FLAG_REG) {
		pr_info("%s : %s :%px %s :%d %s :%d %s :%d\n",
			__func__,
			"owner", owner,
			"dev_port", dev_port,
			"cbm_port_id", port->port_id,
			"flags", flags | DP_F_DEREGISTER);
		print_cbm_alloc_data(data);
	}
#endif

	if (cbm_dp_port_dealloc(owner, dev_port, port->port_id, data,
				flags | DP_F_DEREGISTER)) {
		pr_err("%s : %s :%px %s :%d %s :%d %s :%d\n",
		       "cbm_dp_port_dealloc Failed",
		       "owner", owner,
		       "dev_port", dev_port,
		       "cbm_port_id", port->port_id,
		       "flags", flags | DP_F_DEREGISTER);
		print_cbm_alloc_data(data);
		return DP_FAILURE;
	}

#if IS_ENABLED(CONFIG_INTEL_DATAPATH_DBG)
	if (dp_dbg_flag & DP_DBG_FLAG_REG)
		pr_info("%s : Successful\n", __func__);
#endif

	return DP_SUCCESS;
}

int dp_alloc_cqm_port(struct module *owner, struct net_device *dev,
		      u32 dev_port, s32 port_id, struct cbm_dp_alloc_data *cbm_data,
			  u32 flags)
{
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_DBG)
	if (dp_dbg_flag & DP_DBG_FLAG_REG) {
		pr_info("%s : %s :%px %s :%s %s :%d %s :%d %s :%d\n",
			__func__,
			"owner", owner,
			"dev_name", dev ? dev->name : "NULL",
			"dev_port", dev_port,
			"cbm_port_id", port_id,
			"flags", flags);
		print_cbm_alloc_data(cbm_data);
	}
#endif

	if (cbm_dp_port_alloc(owner, dev, dev_port, port_id, cbm_data,
			      flags)) {
		pr_info("%s : %s :%px %s :%s %s :%d %s :%d %s :%d\n",
			__func__,
			"owner", owner,
			"dev_name", dev ? dev->name : "NULL",
			"dev_port", dev_port,
			"cbm_port_id", port_id,
			"flags", flags);
		print_cbm_alloc_data(cbm_data);
		return DP_FAILURE;
	}

	if (!(cbm_data->flags & CBM_PORT_DP_SET) &&
	    !(cbm_data->flags & CBM_PORT_DQ_SET)) {
		pr_err("%s NO DP_SET/DQ_SET(%x):%s/dev_port %d\n",
		       "cbm_dp_port_alloc",
		       cbm_data->flags,
		       owner->name, dev_port);
		return DP_FAILURE;
	}

#if IS_ENABLED(CONFIG_INTEL_DATAPATH_DBG)
	if (dp_dbg_flag & DP_DBG_FLAG_REG) {
		print_cbm_alloc_data(cbm_data);
		pr_info("%s : Successful\n", __func__);
	}
#endif

	return DP_SUCCESS;
}

int dp_enable_cqm_port(struct module *owner, struct pmac_port_info *port,
		       struct cbm_dp_en_data *data, u32 flags)
{
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_DBG)
	if (dp_dbg_flag & DP_DBG_FLAG_REG) {
		pr_info("%s : %s :%px %s :%d %s :%d %s :%d\n",
			__func__,
			"owner", owner,
			"dp_port", port->port_id,
			"flags", flags,
			"alloc_flags", port->alloc_flags);
		print_cbm_en_data(data);
	}
#endif

	if (cbm_dp_enable(owner, port->port_id, data, flags,
			  port->alloc_flags)) {
		pr_err("%s : %s :%px %s :%d %s :%d %s :%d\n",
		       "cbm_dp_enable Failed",
		       "owner", owner,
		       "dp_port", port->port_id,
		       "flags", flags,
		       "alloc_flags", port->alloc_flags);
		return DP_FAILURE;
	}

#if IS_ENABLED(CONFIG_INTEL_DATAPATH_DBG)
	if (dp_dbg_flag & DP_DBG_FLAG_REG)
		pr_info("%s : Successful\n", __func__);
#endif

	return DP_SUCCESS;
}

int dp_cqm_port_alloc_complete(struct module *owner,
			       struct pmac_port_info *port, s32 dp_port,
			       struct cbm_dp_alloc_complete_data *data,
			       u32 flags)
{
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_DBG)
	if (dp_dbg_flag & DP_DBG_FLAG_REG) {
		pr_info("%s : %s :%px %s :%s %s :%d %s :%d %s :%d\n",
			__func__,
			"owner", owner,
			"dev_name", port->dev ? port->dev->name : "NULL",
			"dev_port", port->dev_port,
			"dp_port", dp_port,
			"alloc_flags", port->alloc_flags);
		print_cqm_alloc_comp_data(data);
	}
#endif

	if (cbm_dp_port_alloc_complete(owner,
				       port->dev,
				       port->dev_port,
				       dp_port,
				       data,
				       port->alloc_flags | flags)) {
		pr_err("%s : %s :%px %s :%s %s :%d %s :%d %s :%d\n",
		       "cbm_dp_port_alloc_complete Failed",
		       "owner", owner,
		       "dev_name", port->dev ? port->dev->name : "NULL",
		       "dev_port", port->dev_port,
		       "dp_port", dp_port,
		       "alloc_flags", port->alloc_flags);
		print_cqm_alloc_comp_data(data);

		return DP_FAILURE;
	}

#if IS_ENABLED(CONFIG_INTEL_DATAPATH_DBG)
	if (dp_dbg_flag & DP_DBG_FLAG_REG) {
		print_cqm_alloc_comp_data(data);
		pr_info("%s : Successful\n", __func__);
	}
#endif

	return DP_SUCCESS;
}

int dp_cqm_deq_port_res_get(u32 dp_port, cbm_dq_port_res_t *res,
			    u32 flags)
{
	if (cbm_dequeue_port_resources_get(dp_port, res, flags)) {
		pr_err("%s :%s :%d %s :%d\n",
		       "cbm_dequeue_port_resources_get Failed",
		       "dp_port", dp_port,
		       "flags", flags);
		print_cqm_deq_res(res);
		return DP_FAILURE;
	}

#if IS_ENABLED(CONFIG_INTEL_DATAPATH_DBG)
	if (dp_dbg_flag & DP_DBG_FLAG_REG)
		print_cqm_deq_res(res);
#endif

#if IS_ENABLED(CONFIG_INTEL_DATAPATH_DBG)
	if (dp_dbg_flag & DP_DBG_FLAG_REG)
		pr_info("%s : Successful\n", __func__);
#endif

	return DP_SUCCESS;
}

#if IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP32)
int dp_cqm_gpid_lpid_map(struct cbm_gpid_lpid *map)
{
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_DBG)
	if (dp_dbg_flag & DP_DBG_FLAG_REG) {
		pr_info("%s : %s :%d %s (%d) <-> %s (%d) %s :%d\n",
			__func__,
			"cbm_inst", map->cbm_inst,
			"gpid", map->gpid,
			"lpid", map->lpid,
			"flag", map->flag);
	}
#endif

	if (cbm_gpid_lpid_map(map)) {
		pr_err("%s : %s :%d %s (%d) <-> %s (%d) %s :%d\n",
		       "cbm_gpid_lpid_map Failed",
		       "cbm_inst", map->cbm_inst,
		       "gpid", map->gpid,
		       "lpid", map->lpid,
		       "flag", map->flag);

		return DP_FAILURE;
	}

#if IS_ENABLED(CONFIG_INTEL_DATAPATH_DBG)
	if (dp_dbg_flag & DP_DBG_FLAG_REG)
		pr_info("%s : Successful\n", __func__);
#endif

	return DP_SUCCESS;
}
#endif
