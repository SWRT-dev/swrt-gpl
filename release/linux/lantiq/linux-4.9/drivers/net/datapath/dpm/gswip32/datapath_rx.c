/*
 * Copyright (C) Intel Corporation
 * Author: Shao Guohua <guohua.shao@intel.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/etherdevice.h>
#include <net/datapath_api.h>
#include "../datapath.h"
#include "datapath_misc.h"
#include "datapath_ppv4_session.h"

void rx_dbg_32(u32 f, struct sk_buff *skb, struct dma_rx_desc_0 *desc0,
	       struct dma_rx_desc_1 *desc1, struct dma_rx_desc_2 *desc2,
	       struct dma_rx_desc_3 *desc3, struct pmac_rx_hdr *pmac,
	       u32 *prel2, int prel2_len,
	       int gpid, int dpid)
{
	int inst = 0;

	DP_DEBUG(DP_DBG_FLAG_DUMP_RX, "\nDPID=%d GPID=%d\n", dpid, gpid);
	DP_DEBUG(DP_DBG_FLAG_DUMP_RX,
		 "\ndp_rx:skb->data=%px Loc=%x offset=%d skb->len=%d\n",
		 skb->data, desc2->field.data_ptr,
		 desc2->field.byte_offset, skb->len);

	if ((f) & DP_DBG_FLAG_DUMP_RX_DESCRIPTOR)
		dp_port_prop[inst].info.dump_rx_dma_desc(desc0, desc1,
				desc2, desc3);

	if (((f) & DP_DBG_FLAG_DUMP_RX_PMAC) && pmac)
		dp_port_prop[inst].info.dump_rx_pmac(pmac);

	if (((f) & DP_DBG_FLAG_DUMP_RX) && prel2)
		dp_dump_raw_data((char *)prel2, prel2_len, "Pre L2 Header");

	if ((f) & DP_DBG_FLAG_DUMP_RX_DATA)
		dp_dump_raw_data((char *)skb->data,
				 skb->len,
				 "Original Data");
}

int32_t dp_rx_32(struct sk_buff *skb, u32 flags)
{
	int res = DP_SUCCESS;
#if IS_ENABLED(CONFIG_INTEL_CBM_SKB)
	struct dma_rx_desc_0 *desc_0 = (struct dma_rx_desc_0 *)&skb->DW0;
	struct dma_rx_desc_1 *desc_1 = (struct dma_rx_desc_1 *)&skb->DW1;
	struct dma_rx_desc_2 *desc_2 = (struct dma_rx_desc_2 *)&skb->DW2;
	struct dma_rx_desc_3 *desc_3 = (struct dma_rx_desc_3 *)&skb->DW3;
#else
	#error "need add proper logic here"
	struct dma_rx_desc_0 *desc_0 = NULL;
	struct dma_rx_desc_1 *desc_1 = NULL;
	struct dma_rx_desc_2 *desc_2 = NULL;
	struct dma_rx_desc_3 *desc_3 = NULL;
#endif
	int vap; /*vap: 0-15 */
	struct net_device *dev;
	dp_rx_fn_t rx_fn;
	char decryp = 0;
	u8 inst = 0;
	struct pmac_port_info *dp_port;
	struct dp_subif_info *p_subif;
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_PTP1588)
	struct mac_ops *ops;
#endif
	char *data_offset;
	int data_len;
	int gpid = desc_1->field.ep;
	int dpid = 0;
	struct cbm_tx_data cbm_data = {0};
	struct pmac_rx_hdr *pmac = NULL;
	int pmac_len = desc_1->field.pmac ? sizeof(struct pmac_rx_hdr) : 0;
	int prel2_len = 0;
	u32 *pre_l2 = NULL;
	struct pp_desc *pp_desc;
	struct dev_mib *mib;

	dp_port = get_dp_port_info(inst, 0);

	if (!skb || !skb->data) {
		pr_err("skb NULL or skb->data is NULL\n");
		return DP_FAILURE;
	}

	if (pmac_len) {
		pmac = (struct pmac_rx_hdr *)(skb->data);
		if (desc_1->field.pre_l2 == 3) {
			DP_DEBUG(DP_DBG_FLAG_DUMP_RX,
				 "With Pmac Hdr 48 byte PreL2 not supported\n");
			return DP_FAILURE;
		}
		prel2_len = pmac_len;
	}

	prel2_len += (desc_1->field.pre_l2 * 16);
	if (prel2_len)
		pre_l2 = (u32 *)(skb->data + pmac_len);

	/* only for those packet from PPv4 should  call pp hook ? */
	pp_rx_pkt_hook(skb);
	pp_desc = pp_pkt_desc_get(skb);
	if (!pp_desc) {
		pr_err("pp_pkt_desc_get fail\n");
		goto RX_DROP2;
	}

	if (desc_1->field.ep < (get_dp_port_info(inst, 0)->gpid_base +
				get_dp_port_info(inst, 0)->gpid_num))
		gpid = pp_desc->ud.rx_port;

	dpid = get_dpid_from_gpid(0, gpid);

	if (unlikely(dp_dbg_flag))
		rx_dbg_32(dp_dbg_flag, skb, desc_0, desc_1, desc_2,
			  desc_3, pmac, pre_l2, prel2_len,
			  gpid, dpid);

	if (unlikely(!dpid)) { /*Normally shouldnot go to here */
		pr_err("%s %s D0: %08x D1: %08x D2: %08x D3: %08x\n",
		       "Impossible: DPID Invalid (0),", "Desc rx'd:",
		       *(u32 *)desc_0, *(u32 *)desc_1,
		       *(u32 *)desc_2, *(u32 *)desc_3);
		pr_err("%s %px %s D0: %08x D1: %08x D2: %08x D3: %08x\n",
		       "QoS Descriptor at buf_base", skb->buf_base,
		       "Desc rx'd:", *skb->buf_base,
		       *(skb->buf_base + sizeof(u32)),
		       *(skb->buf_base + (2 * sizeof(u32))),
		       *(skb->buf_base + (3 * sizeof(u32))));
		goto RX_DROP;
	}

	vap = GET_VAP(desc_0->field.dest_sub_if_id,
		      get_dp_port_info(inst, dpid)->vap_offset,
		      get_dp_port_info(inst, dpid)->vap_mask);
	dp_port = get_dp_port_info(inst, dpid);
	p_subif = get_dp_port_subif(dp_port, vap);
	mib = get_dp_port_subif_mib(p_subif);
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_PTP1588)

	if (dp_port->f_ptp) {
		ops = dp_port_prop[inst].mac_ops[dpid];

		if (ops)
			ops->do_rx_hwts(ops, skb);
	}

#endif

	if (IS_SPECIAL_GPID(gpid)) {
		u8 classid = desc_1->field.classid;

		if (atomic_inc_and_test(&p_subif->f_dfl_sess[classid]) == 1) {
			struct dp_session sess;

			sess.inst = inst;
			sess.in_port = gpid;
			sess.eg_port = p_subif->gpid;
			sess.qid = p_subif->qid;
			sess.vap = vap;
			sess.h1 = pp_desc->ud.hash_h1;
			sess.h2 = pp_desc->ud.hash_h2;
			sess.sig = pp_desc->ud.hash_sig;
			if (dp_add_default_egress_sess(&sess, 0)) {
				atomic_dec(&p_subif->f_dfl_sess[classid]);
				pr_err("Fail to create default egress\n");
				goto RX_DROP;
			}
		} else {
			atomic_dec(&p_subif->f_dfl_sess[classid]);
		}

		cbm_data.dp_inst = inst;
		cbm_data.f_byqos = 1;
		/* no need to insert pmac since it should be already there */
		res = cbm_cpu_pkt_tx(skb, &cbm_data, 0);
		UP_STATS(mib->tx_cbm_pkt);
		return res;
	}

	/*PON traffic always have timestamp attached,removing Timestamp */
	if (dp_port->alloc_flags & (DP_F_GPON | DP_F_EPON)) {
		/* Stripping of last 10 bytes timestamp */
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_PTP1588)
		if (!dp_port->f_ptp)
			__pskb_trim(skb, skb->len - DP_TS_HDRLEN);

#else
		__pskb_trim(skb, skb->len - DP_TS_HDRLEN);
#endif
	}

	rx_fn = dp_port->cb.rx_fn;

	if (likely(rx_fn && dp_port->status)) {
		/*Clear some fields as SWAS V3.7 required */
		desc_3->all &= dma_rx_desc_mask3.all;
		skb->priority = desc_1->field.classid;
		skb->dev = p_subif->netif;
		dev = p_subif->netif;

		if (decryp) { /*workaround mark for bypass xfrm policy*/
			desc_1->field.dec = 1;
			desc_1->field.enc = 1;
		}

		if (!dev &&
		    ((dp_port->alloc_flags & DP_F_FAST_DSL) == 0)) {
			UP_STATS(mib->rx_fn_dropped);
			goto RX_DROP;
		}

		if (unlikely(dp_dbg_flag)) {
			DP_DEBUG(DP_DBG_FLAG_DUMP_RX,
				 "DPID=%d GPID=%d vap=%d\n", dpid, gpid, vap);

			if (dp_dbg_flag & DP_DBG_FLAG_DUMP_RX_DATA) {
				if (pmac_len) {
					data_len = skb->len - pmac_len;
					dp_dump_raw_data(skb->data,
							 pmac_len,
							 "pmac to top drv");
				} else {
					data_offset = skb->data;
					data_len = skb->len;
					dp_dump_raw_data(data_offset, data_len,
							 "Data to top drv");
				}
			}

			if (dp_dbg_flag & DP_DBG_FLAG_DUMP_RX_DESCRIPTOR)
				dp_port_prop[inst].info.dump_rx_dma_desc(
					desc_0, desc_1,
					desc_2, desc_3);
		}

		/*
		 * If switch h/w acceleration is enabled,setting of this bit
		 * avoid forwarding duplicate packets from linux
		 */
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_SWITCHDEV)

		if (p_subif->fid > 0)
			skb->offload_fwd_mark = 1;

#endif

		/* Remove PMAC from SKB */
		if (pmac_len)
			skb_pull(skb, pmac_len);

		if ((STATS_GET(p_subif->rx_flag) <= 0)) {
			UP_STATS(mib->rx_fn_dropped);
			goto RX_DROP2;
		}
		/* If Redirect Bit Set, Destination is Known */
		if (desc_1->field.redir == 0) {
			rx_fn(dev, NULL, skb, skb->len);
			UP_STATS(mib->rx_fn_rxif_pkt);
		} else {
			rx_fn(NULL, dev, skb, skb->len);
			UP_STATS(mib->rx_fn_txif_pkt);
		}

		return DP_SUCCESS;
	}

	if (unlikely(dpid >=
		     dp_port_prop[inst].info.cap.max_num_dp_ports - 1)) {
		pr_err("Drop for wrong ep or src port id=%u ??\n",
		       dpid);
		goto RX_DROP;
	} else if (unlikely(dp_port->status == PORT_FREE)) {
		DP_DEBUG(DP_DBG_FLAG_DUMP_RX, "Drop for port %u free\n",
			 dpid);
		goto RX_DROP;
	} else if (unlikely(!rx_fn)) {
		DP_DEBUG(DP_DBG_FLAG_DUMP_RX,
			 "Drop for subif of port %u not registered yet\n",
			 dpid);
		UP_STATS(mib->rx_fn_dropped);
		goto RX_DROP2;
	} else {
		pr_info("Unknown issue\n");
	}

RX_DROP:
	UP_STATS(dp_port->rx_err_drop);
RX_DROP2:

	if (skb)
		dev_kfree_skb_any(skb);

	return res;
}

