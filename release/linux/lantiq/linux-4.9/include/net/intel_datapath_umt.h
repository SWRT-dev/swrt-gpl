// SPDX-License-Identifier: GPL-2.0
/*
 *  Copyright (C) 2018 Intel Corporation.
 *  Wu ZhiXian <zhixian.wu@intel.com>
 */
#ifndef _INTEL_DATAPATH_UMT_H
#define _INTEL_DATAPATH_UMT_H

#define DP_UMT_NOT_SENDING_ZERO_COUNT	BIT(0)
#define DP_UMT_SENDING_RX_COUNT_ONLY	BIT(1)
#define DP_UMT_SUSPEND_SENDING_COUNT	BIT(2)
#define DP_UMT_ENABLE_FLAG 				BIT(3)

enum dp_umt_rx_src {
	DP_UMT_RX_FROM_CQEM = 0,
	DP_UMT_RX_FROM_DMA  = 1,
	DP_UMT_RX_SRC_MAX,
};

enum dp_umt_msg_mode {
	DP_UMT_SELFCNT_MODE  = 0,
	DP_UMT_USER_MSG_MODE = 1,
	DP_UMT_MODE_MAX,
};

enum dp_umt_sw_msg {
	DP_UMT_NO_MSG	 = 0,
	DP_UMT_MSG0_ONLY = 1,
	DP_UMT_MSG1_ONLY = 2,
	DP_UMT_MSG0_MSG1 = 3,
	DP_UMT_MSG_MAX,
};

enum dp_umt_status {
	DP_UMT_STATUS_DISABLE = 0,
	DP_UMT_STATUS_ENABLE  = 1,
	DP_UMT_STATUS_MAX,
};

enum dp_umt_mux_mode {
    DP_UMT_MUX_CQEM = 0,
    DP_UMT_MUX_RX0  = 1,
    DP_UMT_MUX_DMA  = 2,
    DP_UMT_MUX_MODE_MAX,
};

/**
 * struct dp_umt_param
 * id: umt HW ID. (0 - 7)
 * rx_src: indicate rx msg source.
 * dma_id: it contains DMA controller ID, DMA port ID and base DMA channel ID.
 * dma_ch_num: number of dma channels used by this UMT port.
 * cqm_enq_pid: cqm enqueue port ID.
 * cqm_dq_pid: cqm dequeue port ID.
 * daddr: UMT message destination address.
 * msg_mode: UMT message mode.
 * period: UMT message interval period.
 * sw_msg: software message mode.
 * flag:  UMT message control flag.
 */
struct dp_umt_param {
	u8			id;
	enum dp_umt_rx_src	rx_src;
	u32			dma_id;
	u8			dma_ch_num;
	u8			cqm_enq_pid;
	u8			cqm_dq_pid;
	u32			daddr;
	enum dp_umt_msg_mode	msg_mode;
	u32			period;
	enum dp_umt_sw_msg	sw_msg;
	unsigned long		flag;
};

/**
 * struct dp_umt_priv
 * dev: platform device.
 * membase: UMT register base address.
 * umt_num: number of UMT entries.
 * umts: umt entry list.
 * umt_lock: protect the umt global resource.
 * clk: clock source.
 * debugfs: debugfs proc.
 * dw_swap: Control OCP SWAPPER WORD for UMT.
 * byte_swap: Control OCP SWAPPER byte for UMT.
 */
struct dp_umt_priv {
	struct platform_device *dev;
	void __iomem		*membase;
	u8			umt_num;
	struct dp_umt_entry	*umts;
	spinlock_t umt_lock;
	struct clk *clk;
	struct dentry	*debugfs;
	u8	dw_swap;
	u8	byte_swap;
	u32 reg_type;
};

/**
 * struct dp_umt_entry
 * param: umt configure parameters.
 * alloced: umt entry status.
 * enabled: umt entry status.
 * halted:  umt control status.
 * not_snd_zero_cnt: umt control status.
 * snd_rx_only: umt control status.
 * max_dam_ch_num: support max DMA channel numbers.
 */
union Flag_Bit {
	u8 flag;
	struct {
	u8			alloced:1;
	u8			enabled:1;
	u8			halted:1;
	u8			not_snd_zero_cnt:1;
	u8			snd_rx_only:1;
	u8			reserve:3;
	}flag_bit;
};

struct dp_umt_entry {
	struct dp_umt_param	param;
	union Flag_Bit flag_bit;
	int			max_dma_ch_num;
};

int dp_umt_request(struct dp_umt_param *umt, unsigned long flag);
int dp_umt_set(struct dp_umt_param *umt, unsigned long flag);
int dp_umt_enable(struct dp_umt_param *umt, unsigned long flag, int enable);
int dp_umt_suspend_sending(struct dp_umt_param *umt,
			   unsigned long flag, int halt);
int dp_umt_release(struct dp_umt_param *umt, unsigned long flag);

#endif