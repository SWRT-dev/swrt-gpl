/* SPDX-License-Identifier: GPL-2.0 */
/*
 *  Copyright (C) 2018 Intel Corporation.
 *  Wu ZhiXian <zhixian.wu@intel.com>
 */

#ifndef _DATAPATH_UMT_H_
#define _DATAPATH_UMT_H_

/* UMT feature flag */
#define UMT_SND_DIS			BIT(0)
#define UMT_CLEAR_CNT			BIT(1)
#define UMT_NOT_SND_ZERO_CNT		BIT(2)
#define UMT_SND_RX_CNT_ONLY		BIT(3)
#define UMT_F_MAX			UMT_SND_RX_CNT_ONLY

enum umt_rx_src {
	UMT_RX_SRC_CQEM, /* RX count from CQM */
	UMT_RX_SRC_DMA,  /* RX count from DMA */
};

enum umt_msg_mode {
	UMT_MSG_SELFCNT,   /* HW count mode */
	UMT_MSG_USER_MODE, /* User count mode */
};

enum umt_sw_msg {
	UMT_NO_MSG,
	UMT_MSG0_ONLY,
	UMT_MSG1_ONLY,
	UMT_MSG0_MSG1,
};

enum umt_cnt_mode {
	UMT_CNT_INC, /* Incremental count */
	UMT_CNT_ACC, /* Accumulate count */
};

enum umt_rx_msg_mode {
	UMT_RXOUT_MSG_SUB, /* RX OUT SUB mode */
	UMT_RXIN_MSG_ADD   /* RX IN Add mode */
};

struct umt_port_ctl {
	int                  id;	    /* UMT Port ID */
	dma_addr_t           daddr;	    /* MSG ADDR */
	u32                  msg_interval;  /* MSG interval */
	enum umt_msg_mode    msg_mode;
	enum umt_cnt_mode    cnt_mode;
	enum umt_sw_msg      sw_msg;	    /* SW mode cfg */
	enum umt_rx_msg_mode rx_msg_mode;
	unsigned int         enable;
	unsigned long        fflag;	    /* UMT feature flag */
};

struct umt_port_res {
	u32               dma_id;
	unsigned int      dma_ch_num;
	unsigned int      cqm_enq_pid;
	unsigned int      cqm_dq_pid;
	enum umt_rx_src   rx_src;
};

struct dp_umt_port {
	struct umt_port_ctl ctl; /* User input info, e.g. DCDP */
	struct umt_port_res res; /* DP manager input info */
};

struct umt_ops {
	struct device *umt_dev;
	int (*umt_request)(struct device *umt_dev, struct dp_umt_port *port);
	int (*umt_enable)(struct device *umt_dev, unsigned int id, bool en);
	int (*umt_set_ctrl)(struct device *umt_dev, unsigned int id,
			    unsigned long flag_mask, unsigned long vflag);
	int (*umt_release)(struct device *umt_dev, unsigned int id);
};

#endif /* _DATAPATH_UMT_H_ */
