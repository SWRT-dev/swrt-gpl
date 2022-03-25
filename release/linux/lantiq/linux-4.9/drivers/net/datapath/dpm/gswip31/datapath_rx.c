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

static void rx_dbg(u32 f, struct sk_buff *skb, struct dma_rx_desc_0 *desc0,
		   struct dma_rx_desc_1 *desc1, struct dma_rx_desc_2 *desc2,
		   struct dma_rx_desc_3 *desc3, unsigned char *parser,
		   struct pmac_rx_hdr *pmac, int paser_exist)

{
	int inst = 0;
	int data_len = skb->len > dp_print_len ? skb->len : dp_print_len;

	DP_DEBUG(DP_DBG_FLAG_DUMP_RX,
		 "\ndp_rx:skb->data=%p Loc=%x offset=%d skb->len=%d\n",
		 skb->data, desc2->field.data_ptr,
		 desc3->field.byte_offset, skb->len);
	if ((f) & DP_DBG_FLAG_DUMP_RX_DATA)
		dp_dump_raw_data(skb->data, data_len, "Original Data");
	DP_DEBUG(DP_DBG_FLAG_DUMP_RX, "parse hdr size = %d\n",
		 paser_exist);
	if ((f) & DP_DBG_FLAG_DUMP_RX_DESCRIPTOR)
		dp_port_prop[inst].info.dump_rx_dma_desc(desc0, desc1,
							 desc2, desc3);
	if (paser_exist && (dp_dbg_flag & DP_DBG_FLAG_DUMP_RX_PASER))
		dump_parser_flag(parser);
	if ((f) & DP_DBG_FLAG_DUMP_RX_PMAC)
		dp_port_prop[inst].info.dump_rx_pmac(pmac);
}

#define PRINT_INTERVAL  (5 * HZ) /* 5 seconds */
unsigned long dp_err_interval = PRINT_INTERVAL;
static void rx_dbg_zero_port(struct sk_buff *skb, struct dma_rx_desc_0 *desc0,
			     struct dma_rx_desc_1 *desc1,
			     struct dma_rx_desc_2 *desc2,
			     struct dma_rx_desc_3 *desc3,
			     unsigned char *parser,
			     struct pmac_rx_hdr *pmac, int paser_exist,
			     u32 ep, u32 port_id, int vap)
{
	int inst = 0;
	static unsigned long last;

	if (!dp_dbg_err) /*bypass dump */
		return;
	if (time_before(jiffies, last + dp_err_interval))
		/* not print in order to keep console not busy */
		return;
	last = jiffies;
	DP_DEBUG(-1, "%s=%d vap=%d\n",
		 (ep) ? "ep" : "port_id", port_id, vap);
	pr_err("\nDrop for ep and source port id both zero ??\n");
	dp_port_prop[inst].info.dump_rx_dma_desc(desc0, desc1, desc2, desc3);

	if (paser_exist)
		dump_parser_flag(parser);
	if (pmac)
		dp_port_prop[inst].info.dump_rx_pmac(pmac);
	dp_dump_raw_data(skb->data,
			 skb->len > dp_print_len ? skb->len : dp_print_len,
			 "Recv Data");
}

/* clone skb to send one copy to lct dev for multicast/broadcast
 * otherwise for unicast send only to lct device
 * return 0 - Caller will not proceed handling i.e. for unicast do rx only for
 *	      LCT port
 *	  1 - Caller continue to handle rx for other device
 */
static int dp_handle_lct(struct pmac_port_info *dp_port,
			 struct sk_buff *skb, dp_rx_fn_t rx_fn)
{
	struct sk_buff *lct_skb;
	struct dp_subif_info *sif;
	struct dev_mib *mib;
	int vap;

	vap = dp_port->lct_idx;
	sif = get_dp_port_subif(dp_port, vap);
	mib = get_dp_port_subif_mib(sif);
	skb->dev = sif->netif;
	if (skb->data[PMAC_SIZE] & 0x1) {
		/* multicast/broadcast */
		DP_DEBUG(DP_DBG_FLAG_PAE, "LCT mcast or broadcast\n");
		if ((STATS_GET(sif->rx_flag) <= 0)) {
			UP_STATS(mib->rx_fn_dropped);
			return 1;
		}
		lct_skb = skb_clone(skb, GFP_ATOMIC);
		if (!lct_skb) {
			pr_err("LCT mcast/bcast skb clone fail\n");
			return -1;
		}
		lct_skb->dev = sif->netif;
		UP_STATS(mib->rx_fn_rxif_pkt);
		DP_DEBUG(DP_DBG_FLAG_PAE, "pkt sent lct(%s)\n",
			 lct_skb->dev->name ? lct_skb->dev->name : "NULL");
		rx_fn(lct_skb->dev, NULL, lct_skb, lct_skb->len);
		return 1;
	} else if (memcmp(skb->data + PMAC_SIZE, skb->dev->dev_addr, 6) == 0) {
		/* unicast */
		DP_DEBUG(DP_DBG_FLAG_PAE, "LCT unicast\n");
		DP_DEBUG(DP_DBG_FLAG_PAE, "unicast pkt sent lct(%s)\n",
			 skb->dev->name ? skb->dev->name : "NULL");
		if ((STATS_GET(sif->rx_flag) <= 0)) {
			UP_STATS(mib->rx_fn_dropped);
			dev_kfree_skb_any(skb);
			return 0;
		}
		rx_fn(skb->dev, NULL, skb, skb->len);
		UP_STATS(mib->rx_fn_rxif_pkt);
		return 0;
	}
	return 1;
}

int32_t dp_rx_31(struct sk_buff *skb, u32 flags)
{
	int res = DP_SUCCESS;
#ifdef DP_SKB_HACK
	struct dma_rx_desc_0 *desc_0 = (struct dma_rx_desc_0 *)&skb->DW0;
	struct dma_rx_desc_1 *desc_1 = (struct dma_rx_desc_1 *)&skb->DW1;
	struct dma_rx_desc_2 *desc_2 = (struct dma_rx_desc_2 *)&skb->DW2;
	struct dma_rx_desc_3 *desc_3 = (struct dma_rx_desc_3 *)&skb->DW3;
#endif
	struct pmac_rx_hdr *pmac;
	unsigned char *parser = NULL;
	int rx_tx_flag = 0;	/*0-rx, 1-tx */
	u32 ep = desc_1->field.ep;	/* ep: 0 -15 */
	int vap; /*vap: 0-15 */
	int paser_exist;
	u32 port_id = ep; /*same with ep now, later set to sspid if ep is 0 */
	struct net_device *dev = NULL;
	dp_rx_fn_t rx_fn;
	char decryp = 0;
	u8 inst = 0;
	struct pmac_port_info *dp_port;
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_PTP1588)
	struct mac_ops *ops;
#endif
	int ret_lct = 1;
	struct dp_subif_info *sif;
	struct dev_mib *mib;

	dp_port = get_dp_port_info(inst, 0);
	if (!skb) {
		pr_err("skb NULL\n");
		return DP_FAILURE;
	}
	if (!skb->data) {
		pr_err("skb->data NULL\n");
		return DP_FAILURE;
	}

	paser_exist = parser_enabled(port_id, desc_1);
	if (paser_exist)
		parser = skb->data;
	pmac = (struct pmac_rx_hdr *)(skb->data + paser_exist);

	if (unlikely(dp_dbg_flag))
		rx_dbg(dp_dbg_flag, skb, desc_0, desc_1, desc_2,
		       desc_3, parser, pmac, paser_exist);
	if (paser_exist) {
		skb_pull(skb, paser_exist);	/*remove parser */
#if IS_ENABLED(CONFIG_PPA_API_SW_FASTPATH)
		skb->mark |= FLG_PPA_PROCESSED;
#endif
	}
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_EXTRA_DEBUG)
	/*Sanity check */
	if (unlikely(dp_port_prop[inst].info.not_valid_rx_ep(ep))) {
		DP_DEBUG(DP_DBG_FLAG_DUMP_RX, "Wrong: why ep=%d??\n", ep);
		rx_dbg(-1, skb, desc_0, desc_1, desc_2, desc_3,
		       parser, pmac, paser_exist);
		goto RX_DROP;
	}
	if (unlikely(dp_drop_all_tcp_err && desc_1->field.tcp_err)) {
		DP_DEBUG(DP_DBG_FLAG_DUMP_RX, "\n----dp_rx why tcp_err ???\n");
		rx_dbg(-1, skb, desc_0, desc_1, desc_2, desc_3, parser,
		       pmac, paser_exist);
		goto RX_DROP;
	}
#endif

	if (port_id == PMAC_CPU_ID) { /*To CPU and need check src pmac port */
		dp_port_prop[inst].info.update_port_vap(inst, &port_id, &vap,
			skb,
			pmac, &decryp);
	} else {		/*GSWIP-R already know the destination */
		rx_tx_flag = 1;
		vap = GET_VAP(desc_0->field.dest_sub_if_id,
			      get_dp_port_info(inst, port_id)->vap_offset,
			      get_dp_port_info(inst, port_id)->vap_mask);
	}
	if (unlikely(!port_id)) { /*Normally shouldnot go to here */
		rx_dbg_zero_port(skb, desc_0, desc_1, desc_2, desc_3, parser,
				 pmac, paser_exist, ep, port_id, vap);
		goto RX_DROP;
	}
	dp_port = get_dp_port_info(inst, port_id);
	sif = get_dp_port_subif(dp_port, vap);
	mib = get_dp_port_subif_mib(sif);
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_PTP1588)
	if (dp_port->f_ptp) {
		ops = dp_port_prop[inst].mac_ops[port_id];
		if (ops)
			ops->do_rx_hwts(ops, skb);
	}
#endif
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
		//desc_1->all &= dma_rx_desc_mask1.all;
		desc_3->all &= dma_rx_desc_mask3.all;
		skb->priority = desc_1->field.classid;
		skb->dev = sif->netif; /* note: for DSL ATM case, skb->dev can
					* be NULL since DSL subif is not real
					* subif as defined by GSWIP
					* and we cannot get the proper vap.
					* At the same time, for pppoa case,
					* DSL DP driver register with NULL dev
					* So its driver must correct it
					*/
		dev = sif->netif;
		if (decryp) { /*workaround mark for bypass xfrm policy*/
			desc_1->field.dec = 1;
			desc_1->field.enc = 1;
		}
		if (!dev &&
		    (!(dp_port->alloc_flags & DP_F_FAST_DSL))) {
			UP_STATS(mib->rx_fn_dropped);
			goto RX_DROP;
		}

		if (unlikely(dp_dbg_flag)) {
			DP_DEBUG(DP_DBG_FLAG_DUMP_RX, "%s=%d vap=%d\n",
				 (ep) ? "ep" : "port_id", port_id, vap);

			if (dp_dbg_flag & DP_DBG_FLAG_DUMP_RX_DATA) {
				dp_dump_raw_data(skb->data, PMAC_SIZE,
						 "pmac to top drv");
				dp_dump_raw_data(skb->data + PMAC_SIZE,
						 ((skb->len - PMAC_SIZE) >
							dp_print_len) ?
							skb->len - PMAC_SIZE :
							dp_print_len,
						 "Data to top drv");
			}
			if (dp_dbg_flag & DP_DBG_FLAG_DUMP_RX_DESCRIPTOR)
				dp_port_prop[inst].info.dump_rx_dma_desc(
					desc_0, desc_1,
					desc_2, desc_3);
		}
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_MPE_FASTHOOK_TEST)
		if (unlikely(ltq_mpe_fasthook_rx_fn))
			ltq_mpe_fasthook_rx_fn(skb, 1, NULL);	/*with pmac */
#endif
		if (unlikely((enum TEST_MODE)dp_rx_test_mode ==
			DP_RX_MODE_LAN_WAN_BRIDGE)) {
			/*for datapath performance test only */
			dp_lan_wan_bridging(port_id, skb);
			/*return DP_SUCCESS;*/
		}
		/*If switch h/w acceleration is enabled,setting of this bit
		 *avoid forwarding duplicate packets from linux
		 */
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_SWITCHDEV)
			if (sif->fid > 0)
				skb->offload_fwd_mark = 1;
		#endif
		if (rx_tx_flag == 0) {
			if (dp_port->lct_idx > 0)
				ret_lct = dp_handle_lct(dp_port, skb, rx_fn);
			if (ret_lct) {
				if (STATS_GET(sif->rx_flag) <= 0) {
					UP_STATS(mib->rx_fn_dropped);
					goto RX_DROP2;
				}
				rx_fn(dev, NULL, skb, skb->len);
				UP_STATS(mib->rx_fn_rxif_pkt);
			}
		} else {
			if (STATS_GET(sif->rx_flag) <= 0) {
				UP_STATS(mib->rx_fn_dropped);
				goto RX_DROP2;
			}
			rx_fn(NULL, dev, skb, skb->len);
			UP_STATS(mib->rx_fn_txif_pkt);
		}

		return DP_SUCCESS;
	}

	if (unlikely(port_id >=
	    dp_port_prop[inst].info.cap.max_num_dp_ports - 1)) {
		pr_err("Drop for wrong ep or src port id=%u ??\n",
		       port_id);
		goto RX_DROP;
	} else if (unlikely(dp_port->status == PORT_FREE)) {
		DP_DEBUG(DP_DBG_FLAG_DUMP_RX, "Drop for port %u free\n",
			 port_id);
		goto RX_DROP;
	} else if (unlikely(!rx_fn)) {
		DP_DEBUG(DP_DBG_FLAG_DUMP_RX,
			 "Drop for subif of port %u not registered yet\n",
			 port_id);
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
