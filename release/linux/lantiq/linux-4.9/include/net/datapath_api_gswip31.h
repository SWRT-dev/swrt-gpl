/*
 * Copyright (C) Intel Corporation
 * Author: Shao Guohua <guohua.shao@intel.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */
#ifndef DATAPATH_API_PRX300_H
#define DATAPATH_API_PRX300_H

#if IS_ENABLED(CONFIG_LITTLE_ENDIAN)
struct dma_rx_desc_0 {
	/* DWORD 0 */
	union {
		struct {
			u32 dest_sub_if_id:15;
			u32 eth_type:2;
			u32 flow_id:8;
			u32 tunnel_id:4;
			u32 resv:2;
			u32 redir:1;
		} __packed field;
		u32 all;
	};
} __packed;
#else /*big endian */
struct dma_rx_desc_0 {
	/* DWORD 0 */
	union {
		struct {
			u32 redir:1;
			u32 resv:2;
			u32 tunnel_id:4;
			u32 flow_id:8;
			u32 eth_type:2;
			u32 dest_sub_if_id:15;
		} __packed field;
		u32 all;
	};
} __packed;
#endif

#if IS_ENABLED(CONFIG_LITTLE_ENDIAN)
struct dma_rx_desc_1 {
	/* DWORD 1 */
	union {
		struct {
			u32 classid:4;
			u32 ip:4;
			u32 ep:4;
			u32 color:2;
			u32 mpe1:1;
			u32 mpe2:1;
			u32 enc:1;
			u32 dec:1;
			u32 nat:1;
			u32 tcp_err:1;
			u32 session_id:12;
		} __packed field;
		u32 all;
	};
} __packed;
#else /*big endian */
struct dma_rx_desc_1 {
	/* DWORD 1 */
	union {
		struct {
			u32 session_id:12;
			u32 tcp_err:1;
			u32 nat:1;
			u32 dec:1;
			u32 enc:1;
			u32 mpe2:1;
			u32 mpe1:1;
			u32 color:2;
			u32 ep:4;
			u32 ip:4;
			u32 classid:4;
		} __packed field;
		u32 all;
	};
} __packed;
#endif

#if IS_ENABLED(CONFIG_LITTLE_ENDIAN)
struct dma_rx_desc_2 {
	/*DWORD 2 */
	union {
		struct {
			u32 data_ptr;
		} __packed field;
		u32 all;
	};

} __packed;
#else /*big endian */
struct dma_rx_desc_2 {
	/*DWORD 2 */
	union {
		struct {
			u32 data_ptr;
		} __packed field;
		u32 all;
	};
} __packed;
#endif

#if IS_ENABLED(CONFIG_LITTLE_ENDIAN)
struct dma_rx_desc_3 {
	/*DWORD 3 */
	union {
		struct {
			u32 data_len:16;
			u32 pool:3;
			u32 res:1;
			u32 policy:3;
			u32 byte_offset:3;
			u32 pdu_type:1;
			u32 dic:1;
			u32 eop:1;
			u32 sop:1;
			u32 c:1;
			u32 own:1;
		} __packed field;
		u32 all;
	};
} __packed;
#else /*big endian */
struct dma_rx_desc_3 {
	/*DWORD 3 */
	union {
		struct {
			u32 own:1;
			u32 c:1;
			u32 sop:1;
			u32 eop:1;
			u32 dic:1;
			u32 pdu_type:1;
			u32 byte_offset:3;
			u32 policy:3;
			u32 res:1;
			u32 pool:3;
			u32 data_len:16;
		} __packed field;
		u32 all;
	};
} __packed;
#endif

#define dma_tx_desc_0 dma_rx_desc_0
#define dma_tx_desc_1 dma_rx_desc_1
#define dma_tx_desc_2 dma_rx_desc_2
#define dma_tx_desc_3 dma_rx_desc_3

#if IS_ENABLED(CONFIG_CPU_BIG_ENDIAN)
/*Note:pmac normally not DWORD aligned. Most time 2 bytes aligment */
struct pmac_rx_hdr { /*Egress PMAC header*/
	/*byte 0 */
	u8 res0:1;
	u8 ver_done:1;
	u8 ip_offset:6;

	/*byte 1 */
	u8 tcp_h_offset:5;
	u8 tcp_type:3;

	/*byte 2 */
	u8 res2:4;
	u8 class:4;

	/*byte 3 */
	u8 res31:1;
	u8 oam:1;
	u8 res32:2;
	u8 ins:1;
	u8 ext:1;
	u8 pkt_type:2;

	/*byte 4 */
	u8 res4:1;
	u8 one_step:1;
	u8 ptp:1;
	u8 src_dst_subif_id_msb:5;

	/*byte 5 */
	u8 src_dst_subif_id_lsb:8;

	/*byte 6 */
	u8 record_id_msb:8;

	/*byte 7 */
	u8 record_id_lsb:4;
	u8 igp_egp:4;
} __packed;
#else /*little endian */
struct pmac_rx_hdr { /*Egress PMAC header*/
	/*byte 0 */
	u8 ip_offset:6;
	u8 ver_done:1;
	u8 res0:1;

	/*byte 1 */
	u8 tcp_type:3;
	u8 tcp_h_offset:5;

	/*byte 2 */
	u8 class:4;
	u8 res2:4;

	/*byte 3 */
	u8 pkt_type:2;
	u8 ext:1;
	u8 ins:1;
	u8 res32:2;
	u8 oam:1;
	u8 res31:1;

	/*byte 4 */
	u8 src_dst_subif_id_msb:5;
	u8 ptp:1;
	u8 one_step:1;
	u8 res4:1;

	/*byte 5 */
	u8 src_dst_subif_id_lsb:8;

	/*byte 6 */
	u8 record_id_msb:8;

	/*byte 7 */
	u8 igp_egp:4;
	u8 record_id_lsb:4;
} __packed;
#endif

#if IS_ENABLED(CONFIG_CPU_BIG_ENDIAN)
struct pmac_tx_hdr { /*Ingress PMAC header*/
	/*byte 0 */
	u8 tcp_chksum:1;
	u8 res1:1;
	u8 ip_offset:6;

	/*byte 1 */
	u8 tcp_h_offset:5;
	u8 tcp_type:3;

	/*byte 2 */
	u8 igp_msb:4; /*gswip30: sppid */
	u8 res2:4;

	/*byte 3 */
	u8 res3:1;
	u8 oam:1;
	u8 lrnmd:1;
	u8 class_en:1;/*TC Enable*/
	u8 ins:1;
	u8 ext:1;
	u8 pkt_type:2;

	/*byte 4 */
	u8 fcs_ins_dis:1;
	u8 one_step:1;
	u8 ptp:1;
	u8 src_dst_subif_id_msb:5;

	/*byte 5 */
	u8 src_dst_subif_id_lsb:8;

	/*byte 6 */
	u8 record_id_msb:8;

	/*byte 7 */
	u8 record_id_lsb:4;
	u8 igp_egp:4;
} __packed;
#else /*little endian */
struct pmac_tx_hdr { /*Ingress PMAC header*/
	/*byte 0 */
	u8 ip_offset:6;
	u8 res1:1;
	u8 tcp_chksum:1;

	/*byte 1 */
	u8 tcp_type:3;
	u8 tcp_h_offset:5;

	/*byte 2 */
	u8 res2:4;
	u8 igp_msb:4;

	/*byte 3 */
	u8 pkt_type:2; /* refer to PMAC_TCP_TYPE */
	u8 ext:1;
	u8 ins:1;
	u8 class_en:1; /*TC Enable*/
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
} __packed;
#endif

#endif /*DATAPATH_API_PRX300_H*/
