/*
 * DirectConnect provides a common interface for the network devices to achieve the full or partial 
   acceleration services from the underlying packet acceleration engine
 * Copyright (c) 2017, Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 */

#ifndef _DATAPATH_API_COMMON_H_
#define _DATAPATH_API_COMMON_H_

#include <linux/skbuff.h>

/**
 * Channel configuration / info structure.
 */
typedef struct
{
    /** [in / out] Caller-side counter physical address. For send and free channels
        this is a head pointer; For recv and alloc this is a tail pointer. Caller
        can choose to submit its own counter address when calling to #cppp_create.
        Alternatively, setting it to zero, instructs common proxy to allocate
        a counter and return it back after the call is successfully completed. */
    uint32_t cnt_caller;

    /** [out] Callee-side counter physical address. For send and free channels
        this is a tail pointer; For recv and alloc this is a head pointer */
    uint32_t cnt_callee;

    /** [in ] Maximum number of entries in the underlying ring. Must be N^2.
        Must not exceed 8K. Must be valid any time #cppp_create_t is used! */
    uint32_t ents;

    /** [out] Physical address of the underlying ring */
    uint32_t addr;
} ltqmips_dcdp_channel_t;

/**
 * Structure encapsulating #cppp_create options and return info
 */
typedef struct
{
    ltqmips_dcdp_channel_t recv;  ///< Required with #CPPP_FLAG_DIR set
    ltqmips_dcdp_channel_t send;  ///< Required with #CPPP_FLAG_DIR set
    ltqmips_dcdp_channel_t alloc; ///< Required with #CPPP_FLAG_DIR set
    ltqmips_dcdp_channel_t free;  ///< Required with #CPPP_FLAG_DIR set
    uint32_t      id;    ///< Required with #CPPP_FLAG_EMU set
} ltqmips_dcdp_res_t;
#define DP_TX_DSL_FCS        2  /*! Only for DSL FCS Checksum calculation */
#define DP_TX_OAM            8  /*! OAM packet */
#define DP_F_ENUM_OR_STRING(name, value, short_name) name = value

/*Note:per bit one variable */
#define DP_F_FLAG_LIST  { \
    DP_F_ENUM_OR_STRING(DP_F_DEREGISTER, 0x00000001, "De-Register"), \
    DP_F_ENUM_OR_STRING(DP_F_FAST_ETH_LAN,   0x00000002, "ETH_LAN"), \
    DP_F_ENUM_OR_STRING(DP_F_FAST_ETH_WAN,   0x00000004, "ETH_WAN"),\
    DP_F_ENUM_OR_STRING(DP_F_FAST_WLAN,      0x00000008, "FAST_WLAN"),\
    DP_F_ENUM_OR_STRING(DP_F_FAST_DSL,       0x00000010, "DSL"),\
    DP_F_ENUM_OR_STRING(DP_F_DIRECT,         0x00000020, "DirectPath"), \
    DP_F_ENUM_OR_STRING(DP_F_LOOPBACK,       0x00000040, "Tunne_loop"),\
    DP_F_ENUM_OR_STRING(DP_F_DIRECTPATH_RX,  0x00000080, "Directpath_RX"),\
    DP_F_ENUM_OR_STRING(DP_F_MPE_ACCEL,      0x00000100, "MPE_FW"), \
    DP_F_ENUM_OR_STRING(DP_F_CHECKSUM,       0x00000200, "Checksum"),\
    DP_F_ENUM_OR_STRING(DP_F_DONTCARE,       0x00000400, "DontCare"),\
    DP_F_ENUM_OR_STRING(DP_F_DIRECTLINK,     0x00000800, "DirectLink"),\
    DP_F_ENUM_OR_STRING(DP_F_SUBIF_LOGICAL, 0x00001000, "LogicalIf"), \
    DP_F_ENUM_OR_STRING(DP_F_LRO,           0x00002000, "LRO"), \
    DP_F_ENUM_OR_STRING(DP_F_FAST_DSL_DOWNSTREAM, 0x00004000, "DSL_Down"),\
    DP_F_ENUM_OR_STRING(DP_F_DSL_BONDING, 0x00008000, "DSL_Bonding") \
}

enum DP_F_FLAG DP_F_FLAG_LIST;
#define MAX_ETH_ALEN 6
#define PMAC_LEN     8

typedef struct ingress_pmac {
    uint32_t tx_dma_chan:8; /*!< Tx DMA channel Number for which PMAC
                   configuration is to be done
                 */
    uint32_t err_disc:1;    /*!< Is Discard Error Enable */
    uint32_t pmac:1;    /*!< Is Ingress PMAC Hdr Present */
    uint32_t def_pmac:1;    /*!< Is Ingress PMAC Hdr Present */
    uint32_t def_pmac_pmap:1;   /*!< Is Default PMAC Header configured
                       for Tx DMA Channel
                     */
    uint32_t def_pmac_en_pmap:1;    /*!< Is PortMap Enable to be used from
                   Default PMAC hrd (else use Ingress PMAC hdr)
                 */
    uint32_t def_pmac_tc:1; /*!< Is TC (class) to be used from Default PMAC
                   hdr (else use Ingress PMAC hdr)
                 */
    uint32_t def_pmac_en_tc:1;  /*!< Are TC bits to be used for TC from
                   Default PMAC hdr (else use Ingress PMAC hdr)
                   Alternately, EN/DE/MPE1/MPE2 bits can
                   be used for TC
                 */
    uint32_t def_pmac_subifid:1; /*!< Is Sub-interfaceId to be taken from
                   Default PMAC hdr (else use Ingress PMAC hdr)
                 */
    uint32_t def_pmac_src_port:1;   /*!< Packet Source Port determined from
                   Default PMAC hdr (else use Ingress PMAC hdr)
                 */
    uint32_t res_ing:15;    /*!< Reserved bits */
    uint8_t def_pmac_hdr[PMAC_LEN]; /*!< Default PMAC header configuration
                   for the Tx DMA channel. Useful if Src Port
                   does not send PMAC header with packet
                     */
} ingress_pmac_t;

typedef struct egress_pmac {
    uint32_t rx_dma_chan:8; /*!< Rx DMA channel Number for which PMAC
                   configuration is to be done
                 */
    uint32_t rm_l2hdr:1;    /*!< If Layer 2 Header is to be removed
               before Egress (for eg. for IP interfaces like LTE)
             */

    uint32_t num_l2hdr_bytes_rm:8;  /*!< If rm_l2hdr=1,
                   then number of L2 hdr bytes to be removed
                     */
    uint32_t fcs:1;     /*!< If FCS is enabled on the port */
    uint32_t pmac:1;    /*!< If PMAC header is enabled on the port */
    uint32_t dst_port:8;    /*!< Destination Port Identifier */
    uint32_t res_dw1:4; /*!< reserved field in DMA descriptor - DW1*/
    uint32_t res1_dw0:3;    /*!< reserved field in DMA descriptor - DW0*/
    uint32_t res2_dw0:2;    /*!< reserved field in DMA descriptor - DW0*/
    uint32_t tc_enable:1;   /*!<Selector for traffic class bits */
    uint32_t traffic_class:8;/*!< If tc_enable=true,sets egress queue
                    traffic class
                    */
    uint32_t flow_id:8; /*!< flow id msb*/
    uint32_t dec_flag:1; /*!< If tc_enable=false,sets decryption flag*/
    uint32_t enc_flag:1; /*!< If tc_enable=false,sets encryption flag*/
    uint32_t mpe1_flag:1; /*!< If tc_enable=false,mpe1 marked flag valid*/
    uint32_t mpe2_flag:1; /*!< If tc_enable=false,mpe1 marked flag valid*/
    uint32_t res_eg:5; /*!< Reserved bits */
} egress_pmac_t;

typedef struct dp_pmac_cfg {
    uint32_t ig_pmac_flags; /*!< one bit for one ingress_pmac_t fields */
    uint32_t eg_pmac_flags; /*!< one bit for one egress_pmac_t fields */
    ingress_pmac_t ig_pmac; /*!< Ingress PMAC configuration */
    egress_pmac_t eg_pmac;  /*!< Egress PMAC configuration */
} dp_pmac_cfg_t;

extern int32_t dp_alloc_port(struct module *owner, struct net_device *dev,
              uint32_t dev_port, int32_t port_id,
              dp_pmac_cfg_t *pmac_cfg, uint32_t flags);

/* FIXME: taken from original datapath_api.h - compilation fix for Legacy platform like, GRX330/300 */
#include <linux/skbuff.h>
#include <linux/etherdevice.h>

struct dp_subif {
	int32_t port_id; /*!< Datapath Port Id corresponds to PMAC Port Id */
	int32_t subif:15; /*!< Sub-interface Id info. In GRX500,
			   15 bits, only 13 bits in PAE handled [14, 11:0].\n
			   subif format is mc_flag[14:14]  Res[13:12] VAP[11:8]
			   GRP[7:7] Index/StationID[6:0] \n
			 */
};

struct dp_drv_mib {
	u64 rx_drop_pkts;
	u64 rx_error_pkts;
	u64 tx_drop_pkts;
	u64 tx_error_pkts;
	u64 tx_pkts;
	u64 tx_bytes;
};

#ifdef CONFIG_LITTLE_ENDIAN
struct dma_rx_desc_0 {
	/* DWORD 0 */
	union {
		struct {
			uint32_t dest_sub_if_id:15;
			uint32_t eth_type:2;
			uint32_t flow_id:8;
			uint32_t tunnel_id:4;
			uint32_t resv0:3;
		} __packed field;
		uint32_t all;
	};
} __packed;

struct dma_rx_desc_1 {
	/* DWORD 1 */
	union {
		struct {
			uint32_t classid:4;
			uint32_t resv1:4;
			uint32_t ep:4;
			uint32_t color:2;
			uint32_t mpe1:1;
			uint32_t mpe2:1;
			uint32_t enc:1;
			uint32_t dec:1;
			uint32_t nat:1;
			uint32_t tcp_err:1;
			uint32_t session_id:12;
		} __packed field;
		uint32_t all;
	};
} __packed;

struct dma_rx_desc_2 {
	/*DWORD 2 */
	union {
		struct {
			uint32_t data_ptr;
		} __packed field;
		uint32_t all;
	};

} __packed;

struct dma_rx_desc_3 {
	/*DWORD 3 */
	union {
		struct {
			uint32_t data_len:16;
			uint32_t mpoa_mode:2;
			uint32_t mpoa_pt:1;
			uint32_t qid:4;
			uint32_t byte_offset:3;
			uint32_t pdu_type:1;
			uint32_t dic:1;
			uint32_t eop:1;
			uint32_t sop:1;
			uint32_t c:1;
			uint32_t own:1;
		} __packed field;
		uint32_t all;
	};
} __packed;

struct dma_tx_desc_0 {
	/* DWORD 0 */
	union {
		struct {
			uint32_t dest_sub_if_id:15;
			uint32_t eth_type:2;
			uint32_t flow_id:8;
			uint32_t tunnel_id:4;
			uint32_t resv0:3;
		} __packed field;
		uint32_t all;
	};
} __packed;

struct dma_tx_desc_1 {
	/* DWORD 1 */
	union {
		struct {
			uint32_t classid:4;
			uint32_t resv1:4;
			uint32_t ep:4;
			uint32_t color:2;
			uint32_t mpe1:1;
			uint32_t mpe2:1;
			uint32_t enc:1;
			uint32_t dec:1;
			uint32_t nat:1;
			uint32_t tcp_err:1;
			uint32_t session_id:12;
		} __packed field;
		uint32_t all;
	};
} __packed;

struct dma_tx_desc_2 {
	/*DWORD 2 */
	union {
		struct {
			uint32_t data_ptr;
		} __packed field;
		uint32_t all;
	};
} __packed;

struct dma_tx_desc_3 {
	/*DWORD 3 */
	union {
		struct {
			uint32_t data_len:16;
			uint32_t mpoa_mode:2;
			uint32_t mpoa_pt:1;
			uint32_t qid:4;
			uint32_t byte_offset:3;
			uint32_t pdu_type:1;
			uint32_t dic:1;
			uint32_t eop:1;
			uint32_t sop:1;
			uint32_t c:1;
			uint32_t own:1;
		} __packed field;
		uint32_t all;
	};
} __packed;

#else				/*big endian */

struct dma_rx_desc_0 {
	/* DWORD 0 */
	union {
		struct {
			uint32_t resv0:3;
			uint32_t tunnel_id:4;
			uint32_t flow_id:8;
			uint32_t eth_type:2;
			uint32_t dest_sub_if_id:15;
		} __packed field;
		uint32_t all;
	};
} __packed;

struct dma_rx_desc_1 {
	/* DWORD 1 */
	union {
		struct {
			uint32_t session_id:12;
			uint32_t tcp_err:1;
			uint32_t nat:1;
			uint32_t dec:1;
			uint32_t enc:1;
			uint32_t mpe2:1;
			uint32_t mpe1:1;
			uint32_t color:2;
			uint32_t ep:4;
			uint32_t resv1:4;
			uint32_t classid:4;
		} __packed field;
		uint32_t all;
	};
} __packed;

struct dma_rx_desc_2 {
	/*DWORD 2 */
	union {
		struct {
			uint32_t data_ptr;
		} __packed field;
		uint32_t all;
	};
} __packed;

struct dma_rx_desc_3 {
	/*DWORD 3 */
	union {
		struct {
			uint32_t own:1;
			uint32_t c:1;
			uint32_t sop:1;
			uint32_t eop:1;
			uint32_t dic:1;
			uint32_t pdu_type:1;
			uint32_t byte_offset:3;
			uint32_t qid:4;
			uint32_t mpoa_pt:1;
			uint32_t mpoa_mode:2;
			uint32_t data_len:16;
		} __packed field;
		uint32_t all;
	};
} __packed;

struct dma_tx_desc_0 {
	union {
		struct {
			/* DWORD 0 */
			uint32_t resv0:3;
			uint32_t tunnel_id:4;
			uint32_t flow_id:8;
			uint32_t eth_type:2;
			uint32_t dest_sub_if_id:15;
		} __packed field;
		uint32_t all;
	};
} __packed;

struct dma_tx_desc_1 {
	/* DWORD 1 */
	union {
		struct {
			uint32_t session_id:12;
			uint32_t tcp_err:1;
			uint32_t nat:1;
			uint32_t dec:1;
			uint32_t enc:1;
			uint32_t mpe2:1;
			uint32_t mpe1:1;
			uint32_t color:2;
			uint32_t ep:4;
			uint32_t resv1:4;
			uint32_t classid:4;
		} __packed field;
		uint32_t all;
	};
} __packed;

struct dma_tx_desc_2 {
	/*DWORD 2 */
	union {
		struct {
			uint32_t data_ptr;
		} __packed field;
		uint32_t all;
	};
} __packed;

struct dma_tx_desc_3 {
	/*DWORD 3 */
	union {
		struct {
			uint32_t own:1;
			uint32_t c:1;
			uint32_t sop:1;
			uint32_t eop:1;
			uint32_t dic:1;
			uint32_t pdu_type:1;
			uint32_t byte_offset:3;
			uint32_t qid:4;
			uint32_t mpoa_pt:1;
			uint32_t mpoa_mode:2;
			uint32_t data_len:16;
		} __packed field;
		uint32_t all;
	};
} __packed;

#define MAX_ETH_ALEN 6
#endif /* #ifndef CONFIG_LITTLE_ENDIAN */

typedef int32_t(*dp_stop_tx_fn_t) (
	struct net_device *dev
); /*! The Driver Stop Tx function callback */

typedef int32_t(*dp_restart_tx_fn_t) (
	struct net_device *dev
); /*! Driver Restart Tx function callback */

typedef int32_t(*dp_reset_mib_fn_t) (
	struct dp_subif *subif,
	int32_t flag
); /*! Driver reset its mib counter callback */

typedef int32_t(*dp_get_mib_fn_t) (
	struct dp_subif *subif,
	struct dp_drv_mib *mib,
	int32_t flag
); /*! Driver get mib counter of the specified subif interface.  */

#endif /* _DATAPATH_API_COMMON_H_ */
