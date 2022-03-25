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
#include "../datapath_tx.h"
#include "datapath_misc.h"
#include "../datapath_instance.h"

struct dp_tx_32 {
	struct dp_tx_common cmn;
	struct pmac_port_info *port;
	struct dp_subif_info *sif;
	struct dma_rx_desc_1 *desc_1;
};

static int cqm_preprocess(struct sk_buff *skb, struct dp_tx_common *cmn,
			  void *p)
{
	struct dp_tx_32 *tx = container_of(cmn, struct dp_tx_32, cmn);

#if IS_ENABLED(CONFIG_INTEL_DATAPATH_EXTRA_DEBUG)
	if (unlikely(!cmn->gpid)) {
		pr_info("Why after get_dma_pmac_templ ep is zero\n");
		return DP_XMIT_ERR_EP_ZERO;
	}
#endif
	tx->desc_1->field.pmac = !!(cmn->flags & DP_TX_FLAG_INSERT_PMAC);
	if (is_stream_port(tx->port->alloc_flags))
		cmn->flags |= DP_TX_FLAG_STREAM_PORT;
	return DP_TX_FN_CONTINUE;
}

static int pp_tx(struct sk_buff *skb, struct dp_tx_common *cmn, void *p)
{
	pp_tx_pkt_hook(skb, cmn->gpid);
	return DP_TX_FN_CONTINUE;
}

static int cqm_tx(struct sk_buff *skb, struct dp_tx_common *cmn, void *p)
{
	struct cbm_tx_data data;
	int ret;

	/*No PMAC for WAVE500 and DSL by default except bonding case */
	if (likely(cmn->flags & DP_TX_FLAG_INSERT_PMAC)) {
		data.pmac = cmn->pmac;
		data.pmac_len = cmn->len;
	} else {
		data.pmac = NULL;
		data.pmac_len = 0;
	}
	data.dp_inst = 0;
	data.f_byqos = !(cmn->flags & DP_TX_FLAG_STREAM_PORT);
	ret = cbm_cpu_pkt_tx(skb, &data, 0);
	UP_STATS(cmn->mib->tx_cbm_pkt);
	return ret;
}

int dp_tx_init_32(int inst)
{
	dp_tx_register_preprocess(DP_TX_CQM, cqm_preprocess, NULL, false);
#if IS_ENABLED(CONFIG_PPV4)
	dp_tx_register_process(DP_TX_PP, pp_tx, NULL);
#endif /* CONFIG_PPV4 */
#if IS_ENABLED(CONFIG_INTEL_CBM)
	dp_tx_register_process(DP_TX_CQM, cqm_tx, NULL);
#endif /* CONFIG_INTEL_CBM */
	return dp_tx_update_list();
}

int32_t dp_xmit_32(struct net_device *rx_if, dp_subif_t *rx_subif,
		   struct sk_buff *skb, int32_t len, uint32_t flags)
{
	struct dma_rx_desc_0 *desc_0;
	struct dma_rx_desc_1 *desc_1;
	struct dma_rx_desc_2 *desc_2;
	struct dma_rx_desc_3 *desc_3;
	struct pmac_tx_hdr pmac = {0};
	u32 ip_offset, tcp_h_offset, tcp_type;
	char tx_chksum_flag = 0; /*check csum cal can be supported or not */
	enum dp_xmit_errors err_ret;
	int inst = 0;
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_PTP1588)
	struct mac_ops *ops;
	int rec_id = 0;
#endif
	struct dp_tx_32 tx = {
		.cmn = {
			.pmac = (u8 *)&pmac,
			.len = sizeof(struct pmac_tx_hdr),
			.dpid = rx_subif->port_id,
			.flags = DP_TX_FLAG_INSERT_PMAC,
		},
	};

#if IS_ENABLED(CONFIG_INTEL_DATAPATH_EXTRA_DEBUG)
	if (unlikely(!dp_init_ok))
		return dp_tx_err(skb, &tx.cmn, DP_XMIT_ERR_NOT_INIT);

	if (unlikely(!rx_subif))
		return dp_tx_err(skb, &tx.cmn, DP_XMIT_ERR_NULL_SUBIF);

	if (unlikely(!skb))
		return dp_tx_err(skb, &tx.cmn, DP_XMIT_ERR_NULL_SKB);
#endif
	if (unlikely(tx.cmn.dpid >=
		     dp_port_prop[inst].info.cap.max_num_dp_ports))
		return dp_tx_err(skb, &tx.cmn, DP_XMIT_ERR_PORT_TOO_BIG);

#if IS_ENABLED(CONFIG_INTEL_DATAPATH_EXTRA_DEBUG)
	if (unlikely(in_irq()))
		return dp_tx_err(skb, &tx.cmn, DP_XMIT_ERR_IN_IRQ);
#endif
	tx.port = get_dp_port_info(inst, tx.cmn.dpid);
	tx.cmn.vap = GET_VAP(rx_subif->subif, tx.port->vap_offset,
			     tx.port->vap_mask);
	tx.sif = get_dp_port_subif(tx.port, tx.cmn.vap);
	tx.cmn.mib = get_dp_port_subif_mib(tx.sif);

	if (unlikely(NO_NEED_PMAC(tx.port->alloc_flags)))
		tx.cmn.flags &= ~DP_TX_FLAG_INSERT_PMAC;

	if (unlikely(!rx_if && /*For atm pppoa case, rx_if is NULL now */
		     !(tx.port->alloc_flags & DP_F_FAST_DSL)))
		return dp_tx_err(skb, &tx.cmn, DP_XMIT_ERR_NULL_IF);

	if (unlikely(dp_dbg_flag))
		dp_tx_dbg("\nOrig", skb, tx.cmn.dpid, len, flags, NULL,
			  rx_subif, 0, 0, flags & DP_TX_CAL_CHKSUM);

	/**********************************************
	 *Must put these 4 lines after INSERT_PMAC
	 *since INSERT_PMAC will change skb if needed
	 *********************************************/
#if defined(DP_SKB_HACK)
	desc_0 = (struct dma_tx_desc_0 *)&skb->DW0;
	desc_1 = tx.desc_1 = (struct dma_tx_desc_1 *)&skb->DW1;
	desc_2 = (struct dma_tx_desc_2 *)&skb->DW2;
	desc_3 = (struct dma_tx_desc_3 *)&skb->DW3;
#endif
	if (flags & DP_TX_CAL_CHKSUM) {
		int ret_flg;

		if (!dp_port_prop[inst].info.check_csum_cap()) {
			return dp_tx_err(skb, &tx.cmn,
					 DP_XMIT_ERR_CSM_NO_SUPPORT);
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

	desc_3->all = (desc_3->all &
		       tx.port->dma3_mask_template[TEMPL_NORMAL].all);

	if (desc_3->field.dic)
		desc_3->field.dic = 1;

	desc_1->field.classid = (skb->priority >= 15) ? 15 : skb->priority;
	desc_2->all = ((uintptr_t)skb->data) & 0xFFFFFFFF;
	desc_3->field.haddr = (((uintptr_t)skb->data) >> 8) & 0xF;

	/*for ETH LAN/WAN */
	if (tx.port->alloc_flags & (DP_F_FAST_ETH_LAN | DP_F_FAST_ETH_WAN |
				    DP_F_GPON | DP_F_EPON)) {
		/*always with pmac*/
		if (likely(tx_chksum_flag)) {
			DP_CB(inst, get_dma_pmac_templ)(TEMPL_CHECKSUM, &pmac,
							desc_0, desc_1,
							tx.port);
			set_chksum(&pmac, tcp_type, ip_offset,
				   ip_offset_hw_adjust, tcp_h_offset);
			DP_CB(inst, set_pmac_subif)(&pmac, rx_subif->subif);
		} else {
			DP_CB(inst, get_dma_pmac_templ)(TEMPL_NORMAL, &pmac,
							desc_0, desc_1,
							tx.port);
			DP_CB(inst, set_pmac_subif)(&pmac, rx_subif->subif);
		}

#if IS_ENABLED(CONFIG_INTEL_DATAPATH_PTP1588)
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_PTP1588_SW_WORKAROUND)

		if (tx.port->f_ptp)
#else
		if (tx.port->f_ptp &&
		    (skb_shinfo(skb)->tx_flags & SKBTX_HW_TSTAMP))
#endif
		{
			ops = dp_port_prop[inst].mac_ops[tx.port->port_id];

			if (!ops)
				return dp_tx_err(skb, &tx.cmn, DP_XMIT_PTP_ERR);

			rec_id = ops->do_tx_hwts(ops, skb);

			if (rec_id < 0)
				return dp_tx_err(skb, &tx.cmn, DP_XMIT_PTP_ERR);

			DP_CB(inst, get_dma_pmac_templ)(TEMPL_PTP, &pmac,
							desc_0, desc_1,
							tx.port);
			pmac.record_id_msb = rec_id;
		}

#endif
	} else if (tx.port->alloc_flags & DP_F_FAST_DSL) { /*some with pmac*/
		if (unlikely(flags & DP_TX_CAL_CHKSUM)) { /* w/ pmac*/
			DP_CB(inst, get_dma_pmac_templ)(TEMPL_CHECKSUM, &pmac,
							desc_0, desc_1,
							tx.port);
			set_chksum(&pmac, tcp_type, ip_offset,
				   ip_offset_hw_adjust, tcp_h_offset);
			DP_CB(inst, set_pmac_subif)(&pmac, rx_subif->subif);
		} else if (flags & DP_TX_DSL_FCS) {/* after checksum check */
			/* w/ pmac for FCS purpose*/
			DP_CB(inst, get_dma_pmac_templ)(TEMPL_OTHERS, &pmac,
							desc_0, desc_1,
							tx.port);
			DP_CB(inst, set_pmac_subif)(&pmac, rx_subif->subif);
		} else { /*no pmac */
			DP_CB(inst, get_dma_pmac_templ)(TEMPL_NORMAL, NULL,
							desc_0, desc_1,
							tx.port);
		}
	} else if (tx.port->alloc_flags & DP_F_FAST_WLAN) {/*some with pmac*/
		/*normally no pmac. But if need checksum, need pmac*/
		if (unlikely(tx_chksum_flag)) { /*with pmac*/
			DP_CB(inst, get_dma_pmac_templ)(TEMPL_CHECKSUM, &pmac,
							desc_0, desc_1,
							tx.port);
			set_chksum(&pmac, tcp_type, ip_offset,
				   ip_offset_hw_adjust, tcp_h_offset);
			DP_CB(inst, set_pmac_subif)(&pmac, rx_subif->subif);
		} else { /*no pmac*/
			DP_CB(inst, get_dma_pmac_templ)(TEMPL_NORMAL, NULL,
							desc_0, desc_1,
							tx.port);
		}
	} else if (tx.port->alloc_flags & DP_F_DIRECTLINK) { /*always w/ pmac*/
		if (unlikely(flags & DP_TX_CAL_CHKSUM)) { /* w/ pmac*/
			DP_CB(inst, get_dma_pmac_templ)(TEMPL_CHECKSUM, &pmac,
							desc_0, desc_1,
							tx.port);
			set_chksum(&pmac, tcp_type, ip_offset,
				   ip_offset_hw_adjust, tcp_h_offset);
			DP_CB(inst, set_pmac_subif)(&pmac, rx_subif->subif);
		} else if (flags & DP_TX_TO_DL_MPEFW) { /*w/ pmac*/
			/*copy from checksum's pmac template setting,
			 *but need to reset tcp_chksum in TCP header
			 */
			DP_CB(inst, get_dma_pmac_templ)(TEMPL_OTHERS, &pmac,
							desc_0, desc_1,
							tx.port);
			DP_CB(inst, set_pmac_subif)(&pmac, rx_subif->subif);
		} else { /*do like normal directpath with pmac */
			DP_CB(inst, get_dma_pmac_templ)(TEMPL_NORMAL, &pmac,
							desc_0, desc_1,
							tx.port);
			DP_CB(inst, set_pmac_subif)(&pmac, rx_subif->subif);
		}
	} else { /*normal directpath: always w/ pmac */
		if (unlikely(tx_chksum_flag)) {
			DP_CB(inst, get_dma_pmac_templ)(TEMPL_CHECKSUM,
							&pmac,
							desc_0,
							desc_1,
							tx.port);
			set_chksum(&pmac, tcp_type, ip_offset,
				   ip_offset_hw_adjust, tcp_h_offset);
			DP_CB(inst, set_pmac_subif)(&pmac, rx_subif->subif);
		} else { /*w/ pmac */
			DP_CB(inst, get_dma_pmac_templ)(TEMPL_NORMAL, &pmac,
							desc_0, desc_1,
							tx.port);
			DP_CB(inst, set_pmac_subif)(&pmac, rx_subif->subif);
		}
	}

	desc_3->field.data_len = skb->len;
	tx.cmn.gpid = desc_1->field.ep = tx.sif->gpid; /* use gpid */

	if (unlikely(dp_dbg_flag)) {
		bool insert = tx.cmn.flags & DP_TX_FLAG_INSERT_PMAC;

		dp_tx_dbg("After", skb, tx.cmn.dpid, len, flags,
			  insert ? &pmac : NULL, rx_subif, insert,
			  skb_is_gso(skb), tx_chksum_flag);
	}

	err_ret = dp_tx_start(skb, &tx.cmn);
	if (unlikely(err_ret))
		return dp_tx_err(skb, &tx.cmn, err_ret);
	else
		return 0;
}

