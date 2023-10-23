// SPDX-License-Identifier: GPL-2.0
/******************************************************************************
 * Copyright (c) 2020 - 2021, MaxLinear, Inc.
 * Copyright 2018 - 2020 Intel Corporation
 *
 ******************************************************************************/ 

#ifndef DATAPATH_API_GSWIP32_H
#define DATAPATH_API_GSWIP32_H

#include <linux/bitfield.h>
#include <linux/bits.h>
#include <linux/dma-mapping.h>
#include <linux/kernel.h>
#include <linux/pfn.h>
#include <linux/smp.h>
#include <linux/types.h>
#include <net/intel_cbm_api.h>

struct pmac_rx_hdr { /*Egress PMAC header*/
	/*byte 0 */
	u8 ip_offset:7;
	u8 ver_done:1;

	/*byte 1 */
	u8 tcp_type:3;
	u8 tcp_h_offset:5;

	/*byte 2 */
	u8 class:4;
	u8 src_dst_subif_id_14_12:3;
	u8 res2:1;

	/*byte 3 */
	u8 pkt_type:2;
	u8 ext:1;
	u8 ins:1;
	u8 res32:2;
	u8 oam:1;
	u8 pre_l2:1;

	/*byte 4 */
	u8 src_dst_subif_id_msb:5;
	u8 ptp:1;
	u8 one_step:1;
	u8 fcs:1;

	/*byte 5 */
	u8 src_dst_subif_id_lsb:8;

	/*byte 6 */
	u8 record_id_msb:8;

	/*byte 7 */
	u8 igp_egp:4;
	u8 record_id_lsb:4;

	/* reserving 8 bytes as LGM is 16 byte PMAC header */
	u8 res[8];
} __packed;

struct pmac_tx_hdr { /*Ingress PMAC header*/
	/*byte 0 */
	u8 ip_offset:7;
	u8 tcp_chksum:1;

	/*byte 1 */
	u8 tcp_type:3;
	u8 tcp_h_offset:5;

	/*byte 2 */
	u8 res2:4;
	u8 src_dst_subif_id_14_12:3;
	u8 res1:1;

	/*byte 3 */
	u8 pkt_type:2;
	u8 ext:1;
	u8 ins:1;
	u8 class_en:1;/*TC Enable*/
	u8 lrnmd:1;
	u8 oam:1;
	u8 res3:1;

	/*byte 4 */
	u8 src_dst_subif_id_msb:5;
	u8 ptp:1;
	u8 one_step:1;
	u8 fcs_ins_dis:1;

	/*byte 5 */
	u8 src_dst_subif_id_lsb:8;

	/*byte 6 */
	u8 record_id_msb:8;

	/*byte 7 */
	u8 igp_egp:4;
	u8 record_id_lsb:4;

	/* reserving 8 bytes as LGM is 16 byte PMAC header */
	u8 res[8];
} __packed;

struct dp_dma_desc {
	union {
		u32 dw[4];
		u64 qw[2];
	};
};

#ifdef CONFIG_ARCH_DMA_ADDR_T_64BIT
#define DP_ADDRH_MASK          GENMASK_ULL(35, 32)
#define DP_MAKE_PHYS(high, low) (FIELD_PREP(DP_ADDRH_MASK, high) | (low))
#else
#define DP_MAKE_PHYS(high, low) (low)
#endif

#endif /*DATAPATH_API_GSWIP32_H*/
