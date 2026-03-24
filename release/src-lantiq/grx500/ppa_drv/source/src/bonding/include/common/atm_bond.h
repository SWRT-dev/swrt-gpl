
/**
 * Copyright(c) 2020 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * The full GNU General Public License is included in this distribution in
 * the file called "COPYING".
 */

#ifndef __ATM_BOND_H__
#define __ATM_BOND_H__

#include <linux/atm.h>
#include <net/dc_ep.h>

#ifndef EP_MAX_NUM
#define EP_MAX_NUM      (DC_EP_MAX_PEER + 1)
#endif

struct atm_aal5_t {
	/**
	 * @skb
	 * skb->data points to reassembled aal5 frame with trailer
	 * skb->len is total aal5 ressembled frame with trailer
	 */
	struct sk_buff *skb;
	int conn;		/* QID */
};

struct aal5_trailer {
	uint8_t uu;
	uint8_t cpi;	/*XXX TODO using it as "conn" storing*/
	union {
		uint16_t dlen;
		struct error_status {
			union {
				struct {
					uint8_t cpi:1;
					uint8_t cpcs_uu:1;
					uint8_t ec:1;
					uint8_t clp:1;
					uint8_t res1:4;
				};
				uint8_t stw1;
			};
			union {
				struct {
					uint8_t res0:2;
					uint8_t ra:1;
					uint8_t il:1;
					uint8_t crc:1;
					uint8_t usz:1;
					uint8_t mfl:1;
					uint8_t ovz:1;
				};
				uint8_t stw0;
			};
		} err_status;
	};
	union {
		uint32_t crc;
		uint32_t lcell_hdr;
	};
} __packed;

typedef int (*cb_tc_recv_atm_frame)(struct atm_aal5_t *atm_rx_pkt);

struct ring_param {
	u32 soc_desc_base;		/* SoC descriptor base addr */
	u32 soc_desc_base_phys;
	u32 soc_desc_num;		/* SoC descriptor Number */
	u32 pd_desc_base;		/* PD descriptor base addr */
	u32 pd_desc_num;		/* PD descriptor Number */
	u32 pd_desc_threshold;	/* PD descriptor threshold */
} __packed;

struct connection_params {
	u32 task_mask;
	struct ring_param txin;		/* TXIN ring params */
	struct ring_param txout;	/* TXOUT ring params */
	struct ring_param rxin;		/* RXIN ring params */
	struct ring_param rxout;	/* RXOUT ring params */
	u32 dbg_base;
	u32 dbg_size;
	u32 magic;
} __packed;

struct bond_drv_cfg_params {
	u32 *soc_umt_dst;		/* SoC UMT dst vaddr */
	u32 umt_period;			/* SoC UMT period */
	u32 aca_umt_dst[2];		/* Per EP device ACA UMT dst paddr */
	/*
	 * i: 0,1 represents epid
	 * j: 0, 1, 2, 3 are ACA_TXIN, ACA_TXOUT, ACA_RXIN and ACA_RXOUT
	 * respectively
	 */
	u32 aca_umt_hdparams[EP_MAX_NUM][ACA_MAX];
	struct connection_params ep_conn[2]; /* Per EP device conn-params*/
	struct connection_params soc_conn;	/* SoC conn-params*/
};

struct atm_bonding_stats {
	__u64 atm_plus_tx_pkts;
	__u64 atm_plus_tx_bytes;
	__u64 atm_plus_tx_errors;
	__u64 atm_plus_tx_dropped;
	__u64 atm_plus_rx_pkts;
	__u64 atm_plus_rx_bytes;
	__u64 atm_plus_rx_errors;
	__u64 atm_plus_rx_dropped;
	__u64 asm_tx_pkts;
	__u64 asm_tx_bytes;
	__u64 asm_tx_errors;
	__u64 asm_tx_dropped;
	__u64 asm_rx_pkts;
	__u64 asm_rx_bytes;
	__u64 asm_rx_errors;
	__u64 asm_rx_dropped;
};

struct bonding_cb_ops {
	int (*cb_bonding_device_init)(struct bond_drv_cfg_params *cfg);
	int (*cb_bonding_device_deinit)(void);
	int (*cb_showtime_enter)(int ep_id);
	int (*cb_showtime_exit)(int ep_id);
	/*TODO epid for stats depends on bonding driver stat impl*/
	void (*cb_bonding_get_stats)(int epid, struct atm_bonding_stats *stats_ptr);
};

struct tc_cb_ops {
	cb_tc_recv_atm_frame rx_atm_frame_cb;
	void (*tc_drv_cbm_buf_free)(void *phys);
	/*writes current cbm dqptr value in @p, returns -1 on failure*/
	int (*tc_drv_cbm_dqptr_read)(u32 *p);
	void (*cb_tc_receive_asm_ind)(const unsigned char ep_id);
	//!TODO: Need to add more params;
};

struct atm_tc_bonding {
	struct bonding_cb_ops cb_ops;
	struct bond_drv_cfg_params bond_cfg;
	struct atm_bonding_stats bond_stats;
};

extern int vrx_register_cb_init(struct bonding_cb_ops *v1,
							struct tc_cb_ops *v2);
extern void vrx_deregister_cb_uninit(void);
#endif /* ___ATM_BOND_H__*/
