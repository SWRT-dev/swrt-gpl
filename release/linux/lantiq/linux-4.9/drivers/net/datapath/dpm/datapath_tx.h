// SPDX-License-Identifier: GPL-2.0
/* Copyright (C) Intel Corporation
 * Author: Shao Guohua <guohua.shao@intel.com>
 */

#ifndef DATAPATH_TX_H
#define DATAPATH_TX_H

#include <net/datapath_api_tx.h>

struct pmac_port_info;
struct dp_subif_info;
struct pmac_tx_hdr;
struct dev_mib;

/**
 * struct dp_checksum_info - datapath checksum info to pass to GSWIP HW
 */
/*! @brief datapath checksum parameters */
struct dp_checksum_info {
	u32 tcp_h_offset;
	u32 ip_offset;
	u32 tcp_type;
};

struct dp_tx_common_ex {
	struct dp_tx_common cmn;
	struct net_device *dev;	  	  /*!< net device */
	struct dev_mib *mib;          /*!< mib info */
	struct pmac_port_info *port;  /*!< port info */
	struct dp_subif_info *sif;    /*!< subif info */
	dp_subif_t *rx_subif;		  /*!< rx_subif */
	enum DP_TEMP_DMA_PMAC tmpl;   /*!< DMA Pmac Template */
	struct dp_checksum_info csum_info;      /*!< DP checksum parameters */
};

int dp_tx_ctx_init(int inst);

void dp_tx_register_process(enum DP_TX_PRIORITY priority, tx_fn fn,
			    void *priv);

void dp_tx_register_preprocess(enum DP_TX_PRIORITY priority, tx_fn fn,
			       void *priv, bool always_enabled);

int dp_tx_update_list(void);

int dp_tx_start(struct sk_buff *skb, struct dp_tx_common *cmn);

void dp_tx_dbg(char *title, struct sk_buff *skb, struct dp_tx_common_ex *ex);

static inline struct pmac_tx_hdr *
dp_tx_get_pmac(const struct dp_tx_common_ex *ex)
{
	return (struct pmac_tx_hdr *)ex->cmn.pmac;
}

static inline u8 dp_tx_get_pmac_len(const struct dp_tx_common_ex *ex)
{
	return ex->cmn.pmac_len;
}

#endif /* end of include guard: DATAPATH_TX_H */
