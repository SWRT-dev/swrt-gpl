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
#include "../datapath_instance.h"

void dp_xmit_dbg(
	char *title,
	struct sk_buff *skb,
	s32 ep,
	s32 len,
	u32 flags,
	struct pmac_tx_hdr *pmac,
	dp_subif_t *rx_subif,
	int need_pmac,
	int gso,
	int checksum)
{
	int data_len;

#if defined(DP_SKB_HACK)
	DP_DEBUG(DP_DBG_FLAG_DUMP_TX,
		 "%s: dp_xmit:skb->data/len=0x%p/%d data_ptr=%x from port=%d and subitf=%d\n",
		 title,	skb->data, len,
		 ((struct dma_tx_desc_2 *)&skb->DW2)->field.data_ptr,
		 ep, rx_subif->subif);
#endif
	if (dp_dbg_flag & DP_DBG_FLAG_DUMP_TX_DATA) {
		data_len = skb->len > dp_print_len ? skb->len : dp_print_len;
		if (pmac)
			dp_dump_raw_data((char *)pmac, PMAC_SIZE, "Tx Data");
		dp_dump_raw_data(skb->data, data_len, "Tx Data");
	}
	DP_DEBUG(DP_DBG_FLAG_DUMP_TX_SUM,
		 "ip_summed=%s(%d) encapsulation=%s\n",
		 dp_skb_csum_str(skb), skb->ip_summed,
		 skb->encapsulation ? "Yes" : "No");
	if (skb->encapsulation)
		DP_DEBUG(DP_DBG_FLAG_DUMP_TX_SUM,
			 "inner ip start=0x%x(%d), transport=0x%x(%d)\n",
			 (unsigned int)skb_inner_network_header(skb),
			 (int)(skb_inner_network_header(skb) -
			 skb->data),
			 (unsigned int)
			 skb_inner_transport_header(skb),
			 (int)(skb_inner_transport_header(skb) -
			 skb_inner_network_header(skb)));
	else
		DP_DEBUG(DP_DBG_FLAG_DUMP_TX_SUM,
			 "ip start=0x%x(%d), transport=0x%x(%d)\n",
			 (unsigned int)(unsigned int)
			 skb_network_header(skb),
			 (int)(skb_network_header(skb) - skb->data),
			 (unsigned int)skb_transport_header(skb),
			 (int)(skb_transport_header(skb) -
			 skb_network_header(skb)));

	if (dp_dbg_flag & DP_DBG_FLAG_DUMP_TX_DESCRIPTOR)
#if defined(DP_SKB_HACK)
		dp_port_prop[0].info.dump_tx_dma_desc(
				(struct dma_tx_desc_0 *)&skb->DW0,
				(struct dma_tx_desc_1 *)&skb->DW1,
				(struct dma_tx_desc_2 *)&skb->DW2,
				(struct dma_tx_desc_3 *)&skb->DW3);
#endif

	DP_DEBUG(DP_DBG_FLAG_DUMP_TX, "flags=0x%x skb->len=%d\n",
		 flags, skb->len);
	DP_DEBUG(DP_DBG_FLAG_DUMP_TX,
		 "skb->data=0x%p with pmac hdr size=%u\n", skb->data,
		 sizeof(struct pmac_tx_hdr));
	if (need_pmac) { /*insert one pmac header */
		DP_DEBUG(DP_DBG_FLAG_DUMP_TX,
			 "need pmac\n");
		if (pmac && (dp_dbg_flag & DP_DBG_FLAG_DUMP_TX_DESCRIPTOR))
			dp_port_prop[0].info.dump_tx_pmac(pmac);
	} else {
		DP_DEBUG(DP_DBG_FLAG_DUMP_TX, "no pmac\n");
	}
	if (gso)
		DP_DEBUG(DP_DBG_FLAG_DUMP_TX, "GSO pkt\n");
	else
		DP_DEBUG(DP_DBG_FLAG_DUMP_TX, "Non-GSO pkt\n");
	if (checksum)
		DP_DEBUG(DP_DBG_FLAG_DUMP_TX, "Need checksum offload\n");
	else
		DP_DEBUG(DP_DBG_FLAG_DUMP_TX, "No need checksum offload pkt\n");

	DP_DEBUG(DP_DBG_FLAG_DUMP_TX, "\n\n");
}

int32_t dp_xmit_31(struct net_device *rx_if, dp_subif_t *rx_subif,
		   struct sk_buff *skb, int32_t len, uint32_t flags)
{
	struct dma_tx_desc_0 *desc_0;
	struct dma_tx_desc_1 *desc_1;
	struct dma_tx_desc_2 *desc_2;
	struct dma_tx_desc_3 *desc_3;
	struct pmac_port_info *dp_info;
	struct pmac_tx_hdr pmac = {0};
	u32 ip_offset, tcp_h_offset, tcp_type;
	char tx_chksum_flag = 0; /*check csum cal can be supported or not */
	char insert_pmac_f = 1; /*flag to insert one pmac */
	int res = DP_SUCCESS;
	int ep, vap, num_deq_port, deq_port_idx, class;
	enum dp_xmit_errors err_ret = 0;
	int inst = 0;
	struct cbm_tx_data data;
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_PTP1588)
	struct mac_ops *ops;
	int rec_id = 0;
#endif
	struct dev_mib *mib;

#if IS_ENABLED(CONFIG_INTEL_DATAPATH_EXTRA_DEBUG)
	if (unlikely(!dp_init_ok)) {
		err_ret = DP_XMIT_ERR_NOT_INIT;
		goto lbl_err_ret;
	}
	if (unlikely(!rx_subif)) {
		err_ret = DP_XMIT_ERR_NULL_SUBIF;
		goto lbl_err_ret;
	}
	if (unlikely(!skb)) {
		err_ret = DP_XMIT_ERR_NULL_SKB;
		goto lbl_err_ret;
	}
#endif
	ep = rx_subif->port_id;
	if (unlikely(ep >= dp_port_prop[inst].info.cap.max_num_dp_ports)) {
		err_ret = DP_XMIT_ERR_PORT_TOO_BIG;
		goto lbl_err_ret;
	}
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_EXTRA_DEBUG)
	if (unlikely(in_irq())) {
		err_ret = DP_XMIT_ERR_IN_IRQ;
		goto lbl_err_ret;
	}
#endif
	dp_info = get_dp_port_info(inst, ep);
	vap = GET_VAP(rx_subif->subif, dp_info->vap_offset, dp_info->vap_mask);
	mib = get_dp_port_subif_mib(get_dp_port_subif(dp_info, vap));
	if (unlikely(!rx_if && /*For atm pppoa case, rx_if is NULL now */
		     !(dp_info->alloc_flags & DP_F_FAST_DSL))) {
		err_ret = DP_XMIT_ERR_NULL_IF;
		goto lbl_err_ret;
	}
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_MPE_FASTHOOK_TEST)
	if (unlikely(ltq_mpe_fasthook_tx_fn))
		ltq_mpe_fasthook_tx_fn(skb, 0, NULL);
#endif
	if (unlikely(dp_dbg_flag))
		dp_xmit_dbg("\nOrig", skb, ep, len, flags,
			    NULL, rx_subif, 0, 0, flags & DP_TX_CAL_CHKSUM);

	/*No PMAC for WAVE500 and DSL by default except bonding case */
	if (unlikely(NO_NEED_PMAC(dp_info->alloc_flags)))
		insert_pmac_f = 0;

	/**********************************************
	 *Must put these 4 lines after INSERT_PMAC
	 *since INSERT_PMAC will change skb if needed
	 *********************************************/
#if defined(DP_SKB_HACK)
	desc_0 = (struct dma_tx_desc_0 *)&skb->DW0;
	desc_1 = (struct dma_tx_desc_1 *)&skb->DW1;
	desc_2 = (struct dma_tx_desc_2 *)&skb->DW2;
	desc_3 = (struct dma_tx_desc_3 *)&skb->DW3;
#endif
	if (flags & DP_TX_CAL_CHKSUM) {
		int ret_flg;

		if (!dp_port_prop[inst].info.check_csum_cap()) {
			err_ret = DP_XMIT_ERR_CSM_NO_SUPPORT;
			goto lbl_err_ret;
		}
		ret_flg = get_offset_clear_chksum(skb, &ip_offset,
						  &tcp_h_offset, &tcp_type);
		if (likely(ret_flg == 0))
			/*HW can support checksum offload*/
			tx_chksum_flag = 1;
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_EXTRA_DEBUG)
		else if (ret_flg == -1)
			pr_info_once("packet can't do hw checksum\n");
#endif
	}

	/*reset all descriptors as SWAS required since SWAS 3.7 */
	/*As new SWAS 3.7 required, MPE1/Color/FlowID is set by applications */
	desc_0->all &= dma_tx_desc_mask0.all;
	desc_1->all &= dma_tx_desc_mask1.all;

	if (desc_3->field.dic) {
		desc_3->all = 0; /*keep DIC bit to support test tool*/
		desc_3->field.dic = 1;
	} else {
		desc_3->all = 0;
	}

	if (flags & DP_TX_OAM) /* OAM */
		desc_3->field.pdu_type = 1;
	desc_1->field.classid = (skb->priority >= 15) ? 15 : skb->priority;
	desc_2->field.data_ptr = (uint32_t)skb->data;

	/*for ETH LAN/WAN */
	if (dp_info->alloc_flags & (DP_F_FAST_ETH_LAN | DP_F_FAST_ETH_WAN |
				    DP_F_GPON | DP_F_EPON)) {
		/*always with pmac*/
		if (likely(tx_chksum_flag)) {
			DP_CB(inst, get_dma_pmac_templ)(TEMPL_CHECKSUM, &pmac,
							desc_0, desc_1,
							dp_info);
			set_chksum(&pmac, tcp_type, ip_offset,
				   ip_offset_hw_adjust, tcp_h_offset);
		} else if (flags & DP_TX_INSERT) {
			DP_CB(inst, get_dma_pmac_templ)(TEMPL_INSERT, &pmac,
							desc_0, desc_1,
							dp_info);

			/* Set the GSWIP Logical Port ID to insert */
			SET_PMAC_IGP_EGP(&pmac, ep);

			/* if DP_TX_INSERT_POINT Ins Point 1, else Ins Point 0
			 * Insertion Point 0 - Full GSWIP processing
			 * Insertion Point 1 - Skipping Ingress CTP processing
			 */
			if (flags & DP_TX_INSERT_POINT)
				pmac.record_id_msb = 1;
			else
				pmac.record_id_msb = 0;
		} else {
			DP_CB(inst, get_dma_pmac_templ)(TEMPL_NORMAL, &pmac,
							desc_0, desc_1,
							dp_info);
		}

#if IS_ENABLED(CONFIG_INTEL_DATAPATH_PTP1588)
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_PTP1588_SW_WORKAROUND)
		if (dp_info->f_ptp)
#else
		if (dp_info->f_ptp &&
		    (skb_shinfo(skb)->tx_flags & SKBTX_HW_TSTAMP))
#endif
		{
			ops = dp_port_prop[inst].mac_ops[dp_info->port_id];
			if (!ops) {
				err_ret = DP_XMIT_PTP_ERR;
				goto lbl_err_ret;
			}
			rec_id = ops->do_tx_hwts(ops, skb);
			if (rec_id < 0) {
				err_ret = DP_XMIT_PTP_ERR;
				goto lbl_err_ret;
			}
			DP_CB(inst, get_dma_pmac_templ)(TEMPL_PTP, &pmac,
							desc_0, desc_1,
							dp_info);
			pmac.record_id_msb = rec_id;
		}
#endif

	} else if (dp_info->alloc_flags & DP_F_FAST_DSL) { /*some with pmac*/
		if (unlikely(flags & DP_TX_CAL_CHKSUM)) { /* w/ pmac*/
			DP_CB(inst, get_dma_pmac_templ)(TEMPL_CHECKSUM, &pmac,
							desc_0, desc_1,
							dp_info);
			set_chksum(&pmac, tcp_type, ip_offset,
				   ip_offset_hw_adjust, tcp_h_offset);
		} else if (flags & DP_TX_DSL_FCS) {/* after checksum check */
			/* w/ pmac for FCS purpose*/
			DP_CB(inst, get_dma_pmac_templ)(TEMPL_OTHERS, &pmac,
							desc_0, desc_1,
							dp_info);
			insert_pmac_f = 1;
		} else { /*no pmac */
			DP_CB(inst, get_dma_pmac_templ)(TEMPL_NORMAL, NULL,
							desc_0, desc_1,
							dp_info);
		}
	} else if (dp_info->alloc_flags & DP_F_FAST_WLAN) {/*some with pmac*/
		/*normally no pmac. But if need checksum, need pmac*/
		if (unlikely(tx_chksum_flag)) { /*with pmac*/
			DP_CB(inst, get_dma_pmac_templ)(TEMPL_CHECKSUM, &pmac,
							desc_0, desc_1,
							dp_info);
			set_chksum(&pmac, tcp_type, ip_offset,
				   ip_offset_hw_adjust, tcp_h_offset);
		} else { /*no pmac*/
			DP_CB(inst, get_dma_pmac_templ)(TEMPL_NORMAL, NULL,
							desc_0, desc_1,
							dp_info);
		}
	}  else if (dp_info->alloc_flags & DP_F_VUNI) {
		DP_CB(inst, get_dma_pmac_templ)(TEMPL_NORMAL, &pmac,
						desc_0, desc_1,
						dp_info);
	} else { /*normal directpath: always w/ pmac */
		if (unlikely(tx_chksum_flag)) {
			DP_CB(inst, get_dma_pmac_templ)(TEMPL_CHECKSUM,
							&pmac,
							desc_0,
							desc_1,
							dp_info);
			set_chksum(&pmac, tcp_type, ip_offset,
				   ip_offset_hw_adjust, tcp_h_offset);
		} else { /*w/ pmac */
			DP_CB(inst, get_dma_pmac_templ)(TEMPL_NORMAL, &pmac,
							desc_0, desc_1,
							dp_info);
		}
	}
	desc_3->field.data_len = skb->len;

	/* Subifid is taken from PMAC header for
	 * Egress Port 2/3/4
	 */
	if (insert_pmac_f)
		DP_CB(inst, set_pmac_subif)(&pmac, rx_subif->subif);

	/* For EPON subifid set as Deq Port Idx + Class */
	if (dp_info->alloc_flags & DP_F_EPON) {
		num_deq_port = get_dp_port_subif(dp_info, vap)->num_qid;
		deq_port_idx = get_dp_port_subif(dp_info, vap)->cqm_port_idx;
		if (skb->priority <= num_deq_port)
			class = skb->priority;
		else
			class = num_deq_port - 1;
		desc_0->field.dest_sub_if_id = deq_port_idx + class;
		DP_CB(inst, set_pmac_subif)(&pmac, deq_port_idx + class);
	}

	if (unlikely(dp_dbg_flag)) {
		if (insert_pmac_f)
			dp_xmit_dbg("After", skb, ep, len, flags, &pmac,
				    rx_subif, insert_pmac_f, skb_is_gso(skb),
				    tx_chksum_flag);
		else
			dp_xmit_dbg("After", skb, ep, len, flags, NULL,
				    rx_subif, insert_pmac_f, skb_is_gso(skb),
				    tx_chksum_flag);
	}

#if IS_ENABLED(CONFIG_LTQ_TOE_DRIVER)
	if (skb_is_gso(skb)) {
		res = ltq_tso_xmit(skb, &pmac, sizeof(pmac), 0);
		UP_STATS(mib->tx_tso_pkt);
		return res;
	}
#endif /* CONFIG_LTQ_TOE_DRIVER */

#if IS_ENABLED(CONFIG_INTEL_DATAPATH_EXTRA_DEBUG)
	if (unlikely(!desc_1->field.ep)) {
		err_ret = DP_XMIT_ERR_EP_ZERO;
		goto lbl_err_ret;
	}
#endif
	if (insert_pmac_f) {
		data.pmac = (u8 *)&pmac;
		data.pmac_len = sizeof(pmac);
		data.dp_inst = inst;
		data.dp_inst = 0;
	} else {
		data.pmac = NULL;
		data.pmac_len = 0;
		data.dp_inst = inst;
		data.dp_inst = 0;
	}
	res = cbm_cpu_pkt_tx(skb, &data, 0);
	UP_STATS(mib->tx_cbm_pkt);
	return res;

lbl_err_ret:
	switch (err_ret) {
	case DP_XMIT_ERR_NOT_INIT:
		printk_ratelimited("dp_xmit failed for dp no init yet\n");
		break;
	case DP_XMIT_ERR_IN_IRQ:
		printk_ratelimited("dp_xmit not allowed in interrupt context\n");
		break;
	case DP_XMIT_ERR_NULL_SUBIF:
		printk_ratelimited("dp_xmit failed for rx_subif null\n");
		UP_STATS(get_dp_port_info(inst, 0)->tx_err_drop);
		break;
	case DP_XMIT_ERR_PORT_TOO_BIG:
		UP_STATS(get_dp_port_info(inst, 0)->tx_err_drop);
		printk_ratelimited("rx_subif->port_id >= max_ports");
		break;
	case DP_XMIT_ERR_NULL_SKB:
		printk_ratelimited("skb NULL");
		UP_STATS(get_dp_port_info(inst,
					  rx_subif->port_id)->tx_err_drop);
		break;
	case DP_XMIT_ERR_NULL_IF:
		UP_STATS(mib->tx_pkt_dropped);
		printk_ratelimited("rx_if NULL");
		break;
	case DP_XMIT_ERR_REALLOC_SKB:
		pr_info_once("dp_create_new_skb failed\n");
		break;
	case DP_XMIT_ERR_EP_ZERO:
		pr_err("Why ep zero in dp_xmit for %s\n",
		       skb->dev ? skb->dev->name : "NULL");
		break;
	case DP_XMIT_ERR_GSO_NOHEADROOM:
		pr_err("No enough skb headerroom(GSO). Need tune SKB buffer\n");
		break;
	case DP_XMIT_ERR_CSM_NO_SUPPORT:
		printk_ratelimited("dp_xmit not support checksum\n");
		break;
	case DP_XMIT_PTP_ERR:
		break;
	default:
		UP_STATS(mib->tx_pkt_dropped);
		pr_info_once("Why come to here:%x\n", dp_info->status);
	}
	if (skb)
		dev_kfree_skb_any(skb);
	return DP_FAILURE;
}
