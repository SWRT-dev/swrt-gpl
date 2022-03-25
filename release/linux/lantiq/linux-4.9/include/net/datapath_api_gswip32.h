/*
 * Copyright (C) Intel Corporation
 * Author: Shao Guohua <guohua.shao@intel.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */
#ifndef DATAPATH_API_LGM_H
#define DATAPATH_API_LGM_H

struct dma_rx_desc_0 {
	/* DWORD 0 */
	union {
		struct {
			u32 dest_sub_if_id:16;
			u32 dw0bit16:1;
			u32 flow_id:8;
			u32 dw0bit31:7;
		} __packed field;
		u32 all;
	};
} __packed;

struct dma_rx_desc_1 {
	/* DWORD 1 */
	union {
		struct {
			u32 classid:4;
			u32 ep:8;	/* This is the Gpid/Port */
			u32 color:2;
			u32 lro_type:2;
			u32 enc:1;
			u32 dec:1;
			u32 src_pool:4;
			u32 pkt_type:2;
			u32 fcs:1;
			u32 classen:1;
			u32 pre_l2:2;
			u32 ts:1;
			u32 pmac:1;
			u32 header_mode:1;
			u32 redir:1;  /* egflag */
		} __packed field;
		u32 all;
	};
} __packed;

struct dma_rx_desc_2 {
	/*DWORD 2 */
	union {
		struct {
			u32 byte_offset:3;
			u32 data_ptr:29;
		} __packed field;
		u32 all;
	};

} __packed;

struct dma_rx_desc_3 {
	/*DWORD 3 */
	union {
		struct {
			u32 data_len:14;
			u32 pool_policy:8;
			u32 sp:1;
			u32 haddr:4;
			u32 dic:1;
			u32 eop:1;
			u32 sop:1;
			u32 c:1;
			u32 own:1;
		} __packed field;
		u32 all;
	};
} __packed;


#define dma_tx_desc_0 dma_rx_desc_0
#define dma_tx_desc_1 dma_rx_desc_1
#define dma_tx_desc_2 dma_rx_desc_2
#define dma_tx_desc_3 dma_rx_desc_3

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
	u8 record_id_lsb:4;
	u8 igp_egp:4;

	/* reserving 8 bytes as LGM is 16 byte PMAC header */
	u8 res[8];
} __packed;

#endif /*DATAPATH_API_LGM_H*/
