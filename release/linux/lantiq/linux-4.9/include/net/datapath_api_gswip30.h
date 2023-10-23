// SPDX-License-Identifier: GPL-2.0
/******************************************************************************
 * Copyright (c) 2020 - 2021, MaxLinear, Inc.
 * Copyright 2016 - 2020 Intel Corporation
 *
 *****************************************************************************/

#ifndef DATAPATH_API_GRX500_GSWIP_H
#define DATAPATH_API_GRX500_GSWIP_H

#if IS_ENABLED(CONFIG_LITTLE_ENDIAN)
struct dma_rx_desc_0 {
	/* DWORD 0 */
	union {
		struct {
			u32 dest_sub_if_id:15;
			u32 eth_type:2;
			u32 flow_id:8;
			u32 tunnel_id:4;
			u32 resv0:3;
		} __packed field;
		u32 all;
	};
} __packed;

struct dma_rx_desc_1 {
	/* DWORD 1 */
	union {
		struct {
			u32 classid:4;
			u32 resv1:4;
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

struct dma_rx_desc_2 {
	/*DWORD 2 */
	union {
		struct {
			u32 data_ptr;
		} __packed field;
		u32 all;
	};

} __packed;

struct dma_rx_desc_3 {
	/*DWORD 3 */
	union {
		struct {
			u32 data_len:16;
			u32 mpoa_mode:2;
			u32 mpoa_pt:1;
			u32 qid:4;
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

struct dma_tx_desc_0 {
	/* DWORD 0 */
	union {
		struct {
			u32 dest_sub_if_id:15;
			u32 eth_type:2;
			u32 flow_id:8;
			u32 tunnel_id:4;
			u32 resv0:3;
		} __packed field;
		u32 all;
	};
} __packed;

struct dma_tx_desc_1 {
	/* DWORD 1 */
	union {
		struct {
			u32 classid:4;
			u32 resv1:4;
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

struct dma_tx_desc_2 {
	/*DWORD 2 */
	union {
		struct {
			u32 data_ptr;
		} __packed field;
		u32 all;
	};
} __packed;

struct dma_tx_desc_3 {
	/*DWORD 3 */
	union {
		struct {
			u32 data_len:16;
			u32 mpoa_mode:2;
			u32 mpoa_pt:1;
			u32 qid:4;
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

#else				/*big endian */

struct dma_rx_desc_0 {
	/* DWORD 0 */
	union {
		struct {
			u32 resv0:3;
			u32 tunnel_id:4;
			u32 flow_id:8;
			u32 eth_type:2;
			u32 dest_sub_if_id:15;
		} __packed field;
		u32 all;
	};
} __packed;

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
			u32 resv1:4;
			u32 classid:4;
		} __packed field;
		u32 all;
	};
} __packed;

struct dma_rx_desc_2 {
	/*DWORD 2 */
	union {
		struct {
			u32 data_ptr;
		} __packed field;
		u32 all;
	};
} __packed;

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
			u32 qid:4;
			u32 mpoa_pt:1;
			u32 mpoa_mode:2;
			u32 data_len:16;
		} __packed field;
		u32 all;
	};
} __packed;

struct dma_tx_desc_0 {
	union {
		struct {
			/* DWORD 0 */
			u32 resv0:3;
			u32 tunnel_id:4;
			u32 flow_id:8;
			u32 eth_type:2;
			u32 dest_sub_if_id:15;
		} __packed field;
		u32 all;
	};
} __packed;

struct dma_tx_desc_1 {
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
			u32 resv1:4;
			u32 classid:4;
		} __packed field;
		u32 all;
	};
} __packed;

struct dma_tx_desc_2 {
	/*DWORD 2 */
	union {
		struct {
			u32 data_ptr;
		} __packed field;
		u32 all;
	};
} __packed;

struct dma_tx_desc_3 {
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
			u32 qid:4;
			u32 mpoa_pt:1;
			u32 mpoa_mode:2;
			u32 data_len:16;
		} __packed field;
		u32 all;
	};
} __packed;

#endif
#if IS_ENABLED(CONFIG_CPU_BIG_ENDIAN)
/*Note:pmac normally not DWORD aligned. Most time 2 bytes aligment */
struct pmac_rx_hdr {
	/*byte 0 */
	u8 res1:1;
	u8 ver_done:1;
	u8 ip_offset:6;

	/*byte 1 */
	u8 tcp_h_offset:5;
	u8 tcp_type:3;

	/*byte 2 */
	u8 sppid:4;
	u8 class:4;

	/*byte 3 */
	u8 res2:6;
	u8 pkt_type:2;

	/*byte 4 */
	u8 res3:1;
	u8 redirect:1;
	u8 res4:1;
	u8 src_sub_inf_id:5;	/*high:mc:1 + vap:4 */

	/*byte 5 */
	u8 src_sub_inf_id2:8;	/*low:mc:1 + vap:4 */

	/*byte 6 */
	u8 port_map:8;	/*high:port map */

	/*byte 7 */
	u8 port_map2:8;	/*low:port map */
} __packed;

struct pmac_tx_hdr {
	/*byte 0 */
	u8 tcp_chksum:1;
	u8 res1:1;
	u8 ip_offset:6;

	/*byte 1 */
	u8 tcp_h_offset:5;
	u8 tcp_type:3;

	/*byte 2 */
	u8 sppid:4;
	u8 res:4;

	/*byte 3 */
	u8 port_map_en:1;
	u8 res2:1;
	u8 time_dis:1;
	u8 class_en:1;
	u8 res3:2;
	u8 pkt_type:2;

	/*byte 4 */
	u8 fcs_ins_dis:1;
	u8 redirect:1;
	u8 time_stmp:1;
	u8 src_sub_inf_id:5;	/*high:mc:1 + vap:4 */

	/*byte 5 */
	u8 src_sub_inf_id2:8;	/*low:mc:1 + vap:4 */

	/*byte 6 */
	u8 port_map:8;	/*high:port map */

	/*byte 7 */
	u8 port_map2:8;	/*low:port map */
} __packed;

#else

/*Note:pmac normally not DWORD aligned. Most time 2 bytes aligment */
struct pmac_rx_hdr {
	/*byte 0 */
	u8 ip_offset:6;
	u8 ver_done:1;
	u8 res1:1;

	/*byte 1 */
	u8 tcp_type:3;
	u8 tcp_h_offset:5;

	/*byte 2 */
	u8 class:4;
	u8 sppid:4;

	/*byte 3 */
	u8 pkt_type:2; /* refer to PMAC_TCP_TYPE */
	u8 res2:6;

	/*byte 4 */
	u8 src_sub_inf_id:5;	/*high:mc:1 + vap:4 */
	u8 res4:1;
	u8 redirect:1;
	u8 res3:1;

	/*byte 5 */
	u8 src_sub_inf_id2:8;	/*low:mc:1 + vap:4 */

	/*byte 6 */
	u8 port_map:8;	/*high:port map */

	/*byte 7 */
	u8 port_map2:8;	/*low:port map */
} __packed;

struct pmac_tx_hdr {
	/*byte 0 */
	u8 ip_offset:6;
	u8 res1:1;
	u8 tcp_chksum:1;

	/*byte 1 */
	u8 tcp_type:3;
	u8 tcp_h_offset:5;

	/*byte 2 */
	u8 res:4;
	u8 sppid:4;

	/*byte 3 */
	u8 pkt_type:2; /* refer to PMAC_TCP_TYPE */
	u8 res3:2;
	u8 class_en:1;
	u8 time_dis:1;
	u8 res2:1;
	u8 port_map_en:1;

	/*byte 4 */
	u8 src_sub_inf_id:5;	/*high:mc:1 + vap:4 */
	u8 time_stmp:1;
	u8 redirect:1;
	u8 fcs_ins_dis:1;

	/*byte 5 */
	u8 src_sub_inf_id2:8;	/*low:mc:1 + vap:4 */

	/*byte 6 */
	u8 port_map:8;	/*high:port map */

	/*byte 7 */
	u8 port_map2:8;	/*low:port map */
} __packed;

#endif

#endif
