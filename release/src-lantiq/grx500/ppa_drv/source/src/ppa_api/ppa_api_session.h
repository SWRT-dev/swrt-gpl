#ifndef __PPA_API_SESSION_H__20081104_1139__
#define __PPA_API_SESSION_H__20081104_1139__

/*******************************************************************************
 **
 ** FILE NAME	: ppa_api_session.h
 ** PROJECT	: PPA
 ** MODULES	: PPA API (Routing/Bridging Acceleration APIs)
 **
 ** DATE	: 4 NOV 2008
 ** AUTHOR	: Xu Liang
 ** DESCRIPTION	: PPA Protocol Stack Hook API Session Operation Functions Header
 **				File
 ** COPYRIGHT	: Copyright (c) 2020, MaxLinear, Inc.
 **               Copyright (c) 2017 Intel Corporation
 **
 ** HISTORY
 ** $Date		$Author		$Comment
 ** 04 NOV 2008		Xu Liang	Initiate Version
 *******************************************************************************/
/*! \file ppa_api_session.h
	\brief This file contains all ppa routing session api.
 */

/** \defgroup PPA_SESSION_API PPA Session API
	\brief	PPA Session API provide API to get/set/delete/modify all ppa routing session
	- ppa_api_session.h: Header file for PPA PROC API
	- ppa_api_session.c: C Implementation file for PPA API
 */
/* @{ */

#include <linux/types.h>
#if IS_ENABLED(CONFIG_X86_INTEL_LGM)
#include <linux/pp_api.h>
#endif
/*
 * ####################################
 *				Definition
 * ####################################
 */
/*
 *	default settings
 */
#define DEFAULT_TIMEOUT_IN_SEC			3600	/*	1 hour*/
#define DEFAULT_BRIDGING_TIMEOUT_IN_SEC		300	/*	5 minute*/
#define DEFAULT_MTU				1500	/*	IP frame size (including IP header)*/
#define DEFAULT_CH_ID				0
#if IS_ENABLED(CONFIG_SOC_GRX500)
#define DEFAULT_HIT_POLLING_TIME		1	/*	1 second*/
#define DEFAULT_HIT_MULTIPLIER			20
#else
#define DEFAULT_HIT_POLLING_TIME		3	/*	change to 3 seconds from original 1 minute for MIB Poll requirement*/
#define DEFAULT_HIT_MULTIPLIER			2
#endif
#define DEFAULT_MIB_POLLING_TIME		1	/*	1 second*/
#define DEFAULT_BRIDGING_HIT_POLLING_TIME	20	/*	20 seconds*/

#ifndef ENABLE_SESSION_DEBUG_FLAGS
#define ENABLE_SESSION_DEBUG_FLAGS		1
#endif

#define PPA_IS_NAT_SESSION(flags) ((flags) & (SESSION_VALID_NAT_IP | SESSION_VALID_NAT_PORT))

#if defined(ENABLE_SESSION_DEBUG_FLAGS) && ENABLE_SESSION_DEBUG_FLAGS
#define SESSION_DBG_NOT_REACH_MIN_HITS		0x00000001
#define SESSION_DBG_ALG				0x00000002
#define SESSION_DBG_ZERO_DST_MAC		0x00000004
#define SESSION_DBG_TCP_NOT_ESTABLISHED		0x00000008
#define SESSION_DBG_RX_IF_NOT_IN_IF_LIST	0x00000010
#define SESSION_DBG_TX_IF_NOT_IN_IF_LIST	0x00000020
#define SESSION_DBG_RX_IF_UPDATE_FAIL		0x00000040
#define SESSION_DBG_TX_IF_UPDATE_FAIL		0x00000080
#define SESSION_DBG_SRC_BRG_IF_NOT_IN_BRG_TBL	0x00000100
#define SESSION_DBG_SRC_IF_NOT_IN_IF_LIST	0x00000200
#define SESSION_DBG_DST_BRG_IF_NOT_IN_BRG_TBL	0x00000400
#define SESSION_DBG_DST_IF_NOT_IN_IF_LIST	0x00000800
#define SESSION_DBG_ADD_PPPOE_ENTRY_FAIL	0x00001000
#define SESSION_DBG_ADD_MTU_ENTRY_FAIL		0x00002000
#define SESSION_DBG_ADD_MAC_ENTRY_FAIL		0x00004000
#define SESSION_DBG_ADD_OUT_VLAN_ENTRY_FAIL	0x00008000
#define SESSION_DBG_RX_PPPOE			0x00010000
#define SESSION_DBG_TX_PPPOE			0x00020000
#define SESSION_DBG_TX_BR2684_EOA		0x00040000
#define SESSION_DBG_TX_BR2684_IPOA		0x00080000
#define SESSION_DBG_TX_PPPOA			0x00100000
#define SESSION_DBG_GET_DST_MAC_FAIL		0x00200000
#define SESSION_DBG_RX_INNER_VLAN		0x00400000
#define SESSION_DBG_RX_OUTER_VLAN		0x00800000
#define SESSION_DBG_TX_INNER_VLAN		0x01000000
#define SESSION_DBG_TX_OUTER_VLAN		0x02000000
#define SESSION_DBG_RX_VLAN_CANT_SUPPORT	0x04000000
#define SESSION_DBG_TX_VLAN_CANT_SUPPORT	0x08000000
#define SESSION_DBG_UPDATE_HASH_FAIL		0x10000000
#define SESSION_DBG_PPE_EDIT_LIMIT		0x20000000
#define SESSION_DBG_SW_ACC_HEADER		0x40000000
#if defined(L2TP_CONFIG) && L2TP_CONFIG
#define SESSION_DBG_TX_PPPOL2TP			0x60000000
#define SESSION_DBG_RX_PPPOL2TP			0x80000000
#endif

#define SET_DBG_FLAG(pitem, flag) ((pitem)->debug_flags |= (flag))
#define CLR_DBG_FLAG(pitem, flag) ((pitem)->debug_flags &= ~(flag))
#else
#define SET_DBG_FLAG(pitem, flag) do { } while (0)
#define CLR_DBG_FLAG(pitem, flag) do { } while (0)
#endif

#define SKB_PRIORITY_DEBUG 	1

#define PSEUDO_MC_ANY_SRC	~0

#define	IP_HEADER_LEN		0x5

#if defined(CAP_WAP_CONFIG) && CAP_WAP_CONFIG

#define	ETHER_TYPE			0x0800
#define	IP_VERSION			0x4
#define	IP_TOTAL_LEN			0x0
#define	IP_IDENTIFIER			0x0
#define	IP_FLAG				0x0
#define	IP_FRAG_OFF			0x0
#define	IP_PROTO_UDP			0x11
#define	UDP_TOTAL_LENGTH		0x0
#define	IP_PSEUDO_UDP_LENGTH		0x0 /*???*/

#endif

#define PPA_QOS_QUEUE_EXISTS 		1
#define PPA_QOS_QUEUE_NOT_FOUND 	0
#define PPA_QOS_SHAPER_EXISTS 		1
#define PPA_QOS_SHAPER_NOT_FOUND 	0

#define MAX_RT_SESS_CAPS 	4
#define MAX_MC_SESS_CAPS 	3

#if IS_ENABLED(CONFIG_X86_INTEL_LGM)
#define MC_GRP_MASK 0x1FF	/*bit 0-8*/
#define MC_DST_MASK 0x3C0000	/*bit 18-21*/
#endif /*IS_ENABLED(CONFIG_X86_INTEL_LGM)*/
/*#define MAX_QOS_CAPS 		2*/

#define  PPA_IPPROTO_ESP	0x32	/*50 */
/*
 * ####################################
 *		Data Type
 * ####################################
 */
#define nat_ip      natip.natsrcip.ip
#define nat_src_ip  natip.natsrcip.srcip
#define nat_dst_ip  natip.natdstip
#define ppa_session_get_index(key) (((u64)(key) * (SESSION_LIST_HASH_TABLE_SIZE)) >> 32)

typedef struct {
	uint8_t			dst_mac[PPA_ETH_ALEN]; 		/*destination mac address */
	uint8_t			src_mac[PPA_ETH_ALEN]; 		/* source mac address */
	uint8_t			nat_src_mac[PPA_ETH_ALEN]; 	/* l2nat new source mac */
	uint16_t		protocol; 			/* l2 protocol */
	uint16_t		ip_proto;			/*IP protocol */
	uint16_t		ip_tos;				/*IP TOS*/
/* 24 bytes */
	PPA_IPADDR		src_ip;				/* Source Ip */
	PPA_IPADDR		dst_ip;				/* destination IP*/
	uint16_t		src_port;			/* Source port */
	uint16_t		dst_port;			/* Destination port */
/* 60 bytes */
	PPA_NATIP		natip;	 			/* IP address to be replaced by NAT if NAT applies */
	uint16_t		nat_port;		 	/*	Port to be replaced by NAT if NAT applies */
	uint16_t		pppoe_session_id; 		/*PPPoE session id for a PPPoE egress traffic */
	uint16_t		new_vci;			/*outgoing vlan*/
	uint32_t		out_vlan_tag;			/*Outer vlan tag for stacked vlan*/
	uint32_t		priority;	 		/*skb priority	*/
	uint32_t		mark;		 		/*skb mark value */
	uint32_t		extmark;			/*skb ext mark value*/
	uint32_t		sixrd_daddr;			/* 6rd destination address */
	uint32_t		new_dscp; 			/* new dscp value for remarking */
/* 104 bytes*/
#if IS_ENABLED(L2TP_CONFIG)
	uint32_t		pppol2tp_session_id;		/*L2TP session Id*/
	uint32_t		pppol2tp_tunnel_id;		/*L2TP tunnelid*/
#endif
	uint32_t		vni;				/* VxLAN VNI */
	uint16_t		udp_vxlan_port;			/* UDP port for VxLAN */
//} __attribute__((packed)) PKT_INFO;
} PKT_INFO;

struct uc_session_node {
	PPA_HLIST_NODE		hlist;
	PPA_ATOMIC		used;

	PPA_SESSION		*session;		/* connection track pointer */
	uint32_t		hash;			/* Hash based on ct or 5tuple */
	uint32_t		routing_entry; 		/* hardware index */
	uint32_t		timeout;		/* session timeout */

	PKT_INFO		pkt;			/* packet related information */
	PPA_NETIF		*rx_if;			/* Receive logical interface */
	PPA_NETIF		*tx_if;			/* Transmit logical interface */

	uint32_t		last_hit_time;		/* Last session hit time; Updated by bookkeeping thread */
	uint32_t		mtu;			/* MTU of egress */
	uint64_t		host_bytes; 		/* Number of bytes transmitted through CPU */
	uint64_t		acc_bytes; 		/* Accelerated bytes */
	uint64_t		last_bytes; 		/* number of accelerated(HW or SW) bytes during the last bookkeeping polling*/
	uint64_t		prev_sess_bytes; 	/* number of accelerated bytes between last two PPA session management polls*/
	uint64_t		prev_clear_acc_bytes;	/* number of accelerated byes during last session management poll */
	uint64_t		prev_clear_host_bytes;	/* number of non acceleerated bytes during last session management poll */

	uint16_t		dslwan_qid;		/* dest queue id */
	uint16_t		dest_ifid;		/* dest	port id */
	uint16_t		dest_subifid;		/* destination subinterface id */

	uint32_t		tunnel_idx; 		/* PPA tunnel table idx */
	uint16_t		num_adds; 		/* Number of attempts to add session */

	void			*session_meta;		/* One selected HAL can store metadata information */

#if IS_ENABLED(CONFIG_X86_INTEL_LGM)
	struct pp_hash 		hwhash;			/* to store the hw hash coming in the ud */
	uint8_t 		ig_gpid;		/* ingress gpid of the session packets */
	uint8_t 		is_loopback;		/* to specify whether the session is intermediate stage processing*/
	uint32_t		spi;			/* SPI for this session */
	struct list_head	tun_node;		/* for listing all sessions of specific tunnel */
#endif
#if IS_ENABLED(CONFIG_SOC_GRX500)
	int32_t			sess_hash; 		/* index of the session in HW */
	uint16_t		src_ifid;		/* source port id */
	uint8_t			s_mac[PPA_ETH_ALEN];	/* actual source mac of packet for PAE wanport learning */
	uint32_t		cp_rt_index; 		/* Complimentary processing route index */
#endif
#if IS_ENABLED(CONFIG_LTQ_TOE_DRIVER) || IS_ENABLED(CONFIG_LGM_TOE)
	uint16_t		lro_sessid;		/* lro session identifier */
#endif
#if IS_ENABLED(PPA_IF_MIB)
	bool			is_rx_logical;		/* rx is logical interface */
	bool			is_tx_logical;		/* tx is logical interface */
#endif
	PPA_NETIF		*br_tx_if;		/*record its subinterface name under bridge interface */
	PPA_NETIF		*br_rx_if;		/*record its subinterface name under bridge interface */

#if IS_ENABLED(SESSION_STATISTIC_DEBUG)
	/*below variable is used for session management debugging purpose */
	uint16_t		hash_index;
	uint16_t	 	hash_table_id; 		/* 0-first hash table, 1 WAN */
	uint16_t		src_ip6_index;		/* Note, 0 means not valid data. so for its correct value, it should be "real index + 1 "	*/
	uint16_t		src_ip6_psuedo_ip;
	uint16_t		dst_ip6_index;		/* Note, 0 means not valid data. so for its correct value, it should be "real index + 1 " */
	uint16_t		dst_ip6_psuedo_ip;
#endif
#if IS_ENABLED(CONFIG_PPA_HANDLE_CONNTRACK_SESSIONS)
	PPA_LIST_NODE		priority_list;
	uint16_t		ewma_num_adds;		/* Number of attempts over which session criteria is decided */
	uint64_t		ewma_bytes;		/* EWMA in bytes */
	uint64_t		ewma_time;		/* EWMA in time	*/
	PPA_TIMESPEC		timespent;		/* Time spent in EWMA window */
	uint64_t		ewma_session_bytes;	/* bytes accmulated by mips over a EWMA window*/
	uint32_t		session_priority;	/* priority of session */
	uint32_t		session_class;
	uint32_t		mult_factor;		/*introduce to calculate average data rate of session over multiple min time intervals session tried to add*/
#endif
	uint32_t		flags;			/* Internal flag : SESSION_IS_REPLY, SESSION_IS_TCP,
	//						SESSION_ADDED_IN_HW, SESSION_NOT_ACCEL_FOR_MGM
	//						SESSION_VALID_NAT_IP, SESSION_VALID_NAT_PORT,
	//						SESSION_VALID_VLAN_INS, SESSION_VALID_VLAN_RM,
	//						SESSION_VALID_OUT_VLAN_INS, SESSION_VALID_OUT_VLAN_RM,
	//						SESSION_VALID_PPPOE, SESSION_VALID_NEW_SRC_MAC,
	//						SESSION_VALID_MTU, SESSION_VALID_NEW_DSCP,
	//						SESSION_VALID_DSLWAN_QID,
	//						SESSION_TX_ITF_IPOA, SESSION_TX_ITF_PPPOA
	//						SESSION_LAN_ENTRY, SESSION_WAN_ENTRY, SESSION_IS_IPV6*/
	uint32_t		flag2; 			/*SESSION_FLAG2_HASH_INDEX_DONE/SESSION_FLAG2_ADD_HW_FAIL*/
#if IS_ENABLED(ENABLE_SESSION_DEBUG_FLAGS)
	uint32_t		debug_flags;
#endif
	uint16_t		num_caps;		/* number of capabilities needed for this session */
	PPA_HSEL_CAP_NODE	caps_list[MAX_RT_SESS_CAPS]; 	/* session capabilities list */

	/* These two variables does not affect the any other modules */
	PPA_TIMER	 	timer;
	PPA_RCU_HEAD		rcu;
};

/* PPA session supporting routines/Macros */
static inline uint32_t ppa_is_GreSession(struct uc_session_node *const p_item)
{
	return (p_item->flag2 & SESSION_FLAG2_GRE);
}

static inline uint32_t ppa_is_IngressGreSession(struct uc_session_node *const p_item)
{
	return (p_item->flag2 & SESSION_FLAG2_IG_GRE);
}

static inline uint32_t ppa_is_EgressGreSession(struct uc_session_node *const p_item)
{
	return (p_item->flag2 & SESSION_FLAG2_EG_GRE);
}

static inline void ppa_set_GreSession(struct uc_session_node *p_item)
{
	p_item->flag2 |= SESSION_FLAG2_GRE;
}

static inline uint32_t ppa_is_vxlan_session(struct uc_session_node *const p_item)
{
	return (p_item->flag2 & SESSION_FLAG2_VXLAN);
}

static inline void ppa_set_vxlan_session(struct uc_session_node *p_item)
{
	p_item->flag2 |= SESSION_FLAG2_VXLAN;
}

static inline void ppa_set_BrSession(struct uc_session_node *p_item)
{
	p_item->flag2 |= SESSION_FLAG2_BRIDGED_SESSION;
}

static inline void ppa_reset_BrSession(struct uc_session_node *p_item)
{
	p_item->flag2 &= ~SESSION_FLAG2_BRIDGED_SESSION;
}

static inline uint32_t ppa_is_BrSession(struct uc_session_node *const p_item)
{
	return (p_item->flag2 & SESSION_FLAG2_BRIDGED_SESSION);
}

static inline uint32_t ppa_is_LanSession(struct uc_session_node *const p_item)
{
	return (p_item->flags & SESSION_LAN_ENTRY);
}

static inline uint32_t ppa_is_EgressPppoeSession(struct uc_session_node *const p_item)
{
	return (ppa_is_LanSession(p_item) && (p_item->flags & SESSION_VALID_PPPOE));
}

static inline uint32_t ppa_is_IngressL2tpSession(struct uc_session_node *const p_item)
{
	return (!ppa_is_LanSession(p_item) && (p_item->flags & SESSION_VALID_PPPOL2TP));
}

static inline uint32_t ppa_is_6rdSession(struct uc_session_node *const p_item)
{
	return (p_item->flags & SESSION_TUNNEL_6RD);
}

static inline uint32_t ppa_is_DsLiteSession(struct uc_session_node *const p_item)
{
	return (p_item->flags & SESSION_TUNNEL_DSLITE);
}

static inline uint32_t ppa_is_MapESession(struct uc_session_node *const p_item)
{
	return ((p_item->flags & SESSION_TUNNEL_DSLITE) && (p_item->flags & SESSION_VALID_NAT_IP));
}

static inline uint32_t ppa_is_IpsecSession(struct uc_session_node *const p_item)
{
	return (p_item->flags & SESSION_TUNNEL_ESP);
}

static inline uint32_t ppa_is_TunneledSession(struct uc_session_node *const p_item)
{
	return ((p_item->flags & SESSION_TUNNEL_ESP) | (p_item->flags & SESSION_TUNNEL_DSLITE) |
			(p_item->flags & SESSION_TUNNEL_6RD) | (p_item->flag2 & SESSION_FLAG2_GRE) |
			(p_item->flag2 & SESSION_FLAG2_VALID_IPSEC_OUTBOUND) | (p_item->flag2 & SESSION_FLAG2_VALID_IPSEC_OUTBOUND_SA) |
			(p_item->flag2 & SESSION_FLAG2_VALID_IPSEC_INBOUND) | (p_item->flag2 & SESSION_FLAG2_VXLAN));
}

static inline uint32_t ppa_is_IngressTunneledSession(struct uc_session_node *const p_item)
{
	if (ppa_is_LanSession(p_item)) {
		return (p_item->flag2 & SESSION_FLAG2_IG_GRE);
	} else {
		return ((p_item->flags & SESSION_TUNNEL_ESP) | (p_item->flags & SESSION_TUNNEL_DSLITE) |
			(p_item->flags & SESSION_TUNNEL_6RD) | (p_item->flags & SESSION_VALID_PPPOL2TP) | (p_item->flag2 & SESSION_FLAG2_IG_GRE) |
			(p_item->flag2 & SESSION_FLAG2_VALID_IPSEC_OUTBOUND) | (p_item->flag2 & SESSION_FLAG2_VALID_IPSEC_OUTBOUND_SA) |
			(p_item->flag2 & SESSION_FLAG2_VALID_IPSEC_INBOUND) | (p_item->flag2 & SESSION_FLAG2_VXLAN));
	}
}

static inline uint32_t ppa_is_EgressTunneledSession(struct uc_session_node *const p_item)
{
	if (ppa_is_LanSession(p_item)) {
		return ((p_item->flags & SESSION_TUNNEL_ESP) | (p_item->flags & SESSION_TUNNEL_DSLITE) |
			(p_item->flags & SESSION_TUNNEL_6RD) | (p_item->flags & SESSION_VALID_PPPOL2TP) | (p_item->flag2 & SESSION_FLAG2_EG_GRE) |
			(p_item->flag2 & SESSION_FLAG2_VALID_IPSEC_OUTBOUND) | (p_item->flag2 & SESSION_FLAG2_VALID_IPSEC_OUTBOUND_SA) |
			(p_item->flag2 & SESSION_FLAG2_VALID_IPSEC_INBOUND) | (p_item->flag2 & SESSION_FLAG2_VXLAN));
	} else {
		return (p_item->flag2 & SESSION_FLAG2_EG_GRE);
	}
}

typedef struct {
	PPA_NETIF	*netif;		/*netdevice pointer */
	uint32_t	ttl;		/*TTL per interface */
	uint32_t	if_flags;	/*netif flags*/
} PPA_MC_NETIF;

typedef struct {
	IP_ADDR_C		ip_mc_group;			/*multicast group address*/
	IP_ADDR_C		source_ip;			/*!<	source ip address */
	uint32_t		num_ifs;			/*number of interfaces*/
	PPA_MC_NETIF		txif[PPA_MAX_MC_IFS_NUM]; 	/*Interface list where mc traffic will be delivered to*/
	PPA_NETIF		*src_netif;			/*the interface which received the mc streaming traffic*/
	uint16_t		if_mask;			/*Interface bitmask */
	uint16_t		new_dscp;			/*new dscp value*/
	uint16_t		new_vci;			/*new vci value*/
	uint16_t		group_id;			/*muliticast group_id allocated by the mc daemon*/
	uint32_t		out_vlan_tag;			/*out VLAN tag if any*/
#if IS_ENABLED(CONFIG_SOC_GRX500)
	uint16_t		src_port;
	uint16_t		dst_port;
#endif
} MC_GROUP_INFO;

struct mc_session_node {
	PPA_HLIST_NODE		mc_hlist;

	MC_GROUP_INFO		grp;
	uint8_t			src_mac[PPA_ETH_ALEN]; 		/* source mac address */

	uint16_t		dslwan_qid;
	uint16_t		dest_ifid;
	uint32_t		mc_entry;
	uint32_t		bridging_flag;		/* sgh add: 0 - routing mode/igmp proxy, 1 - bring mode/igmp snooping.*/
	uint8_t			SSM_flag;	 	/*!< Set the flag if source specific forwarding is required default 0*/
#if IS_ENABLED(RTP_SAMPLING_ENABLE)
	uint8_t			RTP_flag;		/*!< rtp flag */
	uint32_t		rtp_pkt_cnt;		/*!< RTP packet mib */
	uint32_t		rtp_seq_num;		/*!< RTP sequence number */
#endif
	uint32_t		last_hit_time;		/* Updated by timer*/
	uint64_t		acc_bytes; 		/*bytes handled by HW acceleration*/
	uint64_t		acc_pkts; 		/*packets handled by HW acceleration*/
	uint64_t		prev_clear_acc_bytes; 	/**/
	uint32_t		last_bytes; 		/*last updated bytes handled by PPE acceleration*/
	void 			*session_action;
#if IS_ENABLED(CONFIG_SOC_GRX500)
	uint8_t			num_vap;
	uint16_t		dest_subifid;
	int32_t			sess_hash;
	uint8_t			s_mac[PPA_ETH_ALEN];
#endif
	uint32_t		flags;			/*	Internal flag : SESSION_IS_REPLY,*/
	/*						SESSION_ADDED_IN_HW, SESSION_NOT_ACCEL_FOR_MGM*/
	/*						SESSION_VALID_NAT_IP, SESSION_VALID_NAT_PORT,*/
	/*						SESSION_VALID_VLAN_INS, SESSION_VALID_VLAN_RM,*/
	/*						SESSION_VALID_OUT_VLAN_INS, SESSION_VALID_OUT_VLAN_RM,*/
	/*						SESSION_VALID_PPPOE, SESSION_VALID_SRC_MAC,*/
	/*						SESSION_VALID_MTU, SESSION_VALID_NEW_DSCP,*/
	/*						SESSION_VALID_DSLWAN_QID,*/
	/*						SESSION_TX_ITF_IPOA, SESSION_TX_ITF_PPPOA*/
	/*						SESSION_LAN_ENTRY, SESSION_WAN_ENTRY*/
	uint32_t		flag2;
#if IS_ENABLED(ENABLE_SESSION_DEBUG_FLAGS)
	uint32_t		debug_flags;
#endif
	uint16_t		num_adds;		/* number of attempts to add for this session */
	uint16_t		num_caps;		/* number of capabilities needed for this sessions */
	PPA_HSEL_CAP_NODE	caps_list[MAX_MC_SESS_CAPS]; /* capabilities, weights and hal_ids*/
};

#if IS_ENABLED(CONFIG_X86_INTEL_LGM)
struct lgm_mc_args {
	struct pktprs_desc	*desc;			/* pktprs descriptor from the parsing driver */
	struct pp_hash 		hwhash;			/* To store the hw hash coming in the ud */
	uint8_t 		ig_gpid;		/* Ingress gpid of the session packets */
	uint8_t 		groupid;		/* Ingress packet groupid from desc */
	uint16_t		dst_idx; 		/* Index of the client to which packet is to be forwarded*/
};
#endif /*IS_ENABLED(CONFIG_X86_INTEL_LGM)*/

typedef struct{
	uint16_t	enabled;	/*!< Entry is valid*/
	uint16_t	session_id;	/*!< PPA aggrate mtu*/
	uint16_t	aggr_mtu;       /*!< PPA aggrate mtu*/
	uint32_t	timeout;	/*!< PPA timeout*/
#if IS_ENABLED(CONFIG_SOC_GRX500)
        uint16_t        f_ipv6;		/*!< PPA flag ipv6*/
        IP_ADDR         srcip;		/*!< PPA source ip*/
        IP_ADDR         dstip;		/*!< PPA destination ip*/
        uint32_t        srcport;        /*!< PPA source port*/
        uint32_t        dstport;        /*!< PPA destination port*/
        uint16_t        session_uid[2]; /*!< PPA session id*/
        uint8_t         session_uid_enabled; /*!< PPA session id enablled */
#endif/* IS_ENABLED(CONFIG_SOC_GRX500)*/
} ppa_lro_entry;

#if defined(L2TP_CONFIG) && L2TP_CONFIG
struct		 l2tp_udp_hdr {
	uint32_t saddr;
	uint32_t daddr;
	uint16_t src_port;
	uint16_t dst_port;
	uint16_t protocol;
};

struct l2tp_ip_hdr {
#if defined(__LITTLE_ENDIAN_BITFIELD)
	__u8	ihl:4,
		version:4;
#elif defined (__BIG_ENDIAN_BITFIELD)
	__u8	version:4,
		ihl:4;
#else
#error	"Please fix <asm/byteorder.h>"
#endif
	__u8	tos;
	__be16	tot_len;
	__be16	id;
	__be16	frag_off;
	__u8	ttl;
	__u8	protocol;
	__sum16 check;
	__be32	saddr;
	__be32	daddr;
	/*The options start here. */
};
#endif

struct br_mac_node {
	PPA_HLIST_NODE		br_hlist;

	uint8_t			mac[PPA_ETH_ALEN];
	PPA_NETIF		*netif;
	uint16_t		vci;
	uint16_t		new_vci;
	uint32_t		timeout;
	uint32_t		last_hit_time;	/* Updated by bookkeeping thread*/
	uint16_t		dslwan_qid;
	uint16_t		dest_ifid;

	uint32_t		flags;		/* Internal flag :*/
	/*					SESSION_ADDED_IN_HW, SESSION_NOT_ACCEL_FOR_MGM*/
	/*					SESSION_STATIC, SESSION_DROP*/
	/*					SESSION_VALID_VLAN_INS, SESSION_VALID_VLAN_RM*/
	/*					SESSION_SRC_MAC_DROP_EN*/
#if defined(ENABLE_SESSION_DEBUG_FLAGS) && ENABLE_SESSION_DEBUG_FLAGS
	uint32_t		debug_flags;
#endif
#if IS_ENABLED(CONFIG_SOC_GRX500)
	uint16_t		fid;
	uint16_t		sub_ifid;
	uint32_t		ref_count;
#endif
	uint32_t		bridging_entry;
	uint32_t		last_mac_violation_time;
};

/*
 *	ppa acceleration type
 */
enum ppa_accl_type { SOFT_ACCL, HARD_ACCL};

/*
 *	ppa operation type indicator
 */
enum ppa_oper_type { ADD, DELETE};

/*
 *	 ppa session class type
 */
enum sess_class_type{HIGH = 1, DEFAULT = 2, LOW = 3};

/*
 * ####################################
 *		Declaration
 * ####################################
 */

/*
 *	Operation Functions
 */

/*	routing session operation*/
int32_t ppa_session_find_by_tuple(PPA_SESSION *,
		uint32_t,
		struct uc_session_node **);
int32_t ppa_sess_find_by_tuple(PPA_SESSION *,
		uint32_t,
		uint32_t *, uint32_t, uint32_t,
		struct uc_session_node **);
int ppa_find_session_from_skb(PPA_BUF *, uint8_t, struct uc_session_node **);
int ppa_find_sess_frm_skb(PPA_BUF *, uint8_t,
			uint32_t *, uint32_t, uint32_t, struct uc_session_node **);

/* This function must be called within read lock */
int32_t __ppa_session_find_by_ct(PPA_SESSION *,
		uint32_t,
		struct uc_session_node **);

int32_t ppa_session_find_by_ct(PPA_SESSION *,
		uint32_t,
		struct uc_session_node **);
/* This function must be called within read lock */
int __ppa_find_session_from_skb(PPA_BUF *,
		uint8_t pf,
		struct uc_session_node **);

/* This function must be called within read lock */
int __ppa_session_find_ct_hash(const PPA_SESSION *p_session,
		uint32_t hash,
		struct uc_session_node **pp_item);
/*
 * Find session entry by HW routing entry
 * Must call __ppa_session_put on return entry
 */
struct uc_session_node*
__ppa_session_find_by_routing_entry(uint32_t routingEntry);
/*
 * Find session entry by p_item->session
 * Must call __ppa_session_put on return entry
 */
struct uc_session_node*
__ppa_session_find_by_session(PPA_SESSION *session);

/* This function must be called within write lock */
void __ppa_session_put(struct uc_session_node *p_item);

void ppa_session_put(struct uc_session_node *p_item);

/* delete all the sessions linked to a connectiontrack hash*/
int32_t ppa_session_del(uint32_t hash, PPA_SESSION *p_session);

#if IS_ENABLED(CONFIG_PPA_RT_SESS_LEARNING)
int32_t ppa_alloc_session(PPA_BUF *, PPA_SESSION *, uint32_t, struct uc_session_node **);
int32_t ppa_session_delete(PPA_SESSION *, uint32_t);
int32_t ppa_update_session(PPA_BUF *, struct uc_session_node *, uint32_t);
int32_t ppa_update_session_info(PPA_BUF *, struct uc_session_node *, uint32_t);
int32_t ppa_prerouting_speed_handle_frame(PPA_BUF *, struct uc_session_node *, uint32_t);
int32_t ppa_postrouting_speed_handle_frame(PPA_BUF *, struct uc_session_node *, uint32_t);
#endif /*CONFIG_PPA_RT_SESS_LEARNING*/

int32_t ppa_update_session_extra(PPA_SESSION_EXTRA *, struct uc_session_node *, uint32_t);
void dump_list_item(struct uc_session_node *, char *);


int32_t ppa_session_start_iteration(uint32_t *, struct uc_session_node **);
int32_t ppa_session_iterate_next(uint32_t *, struct uc_session_node **);
void ppa_session_stop_iteration(void);
int	ppa_get_hw_session_cnt(void);

/*	routing session hardware/firmware operation*/
int32_t ppa_hw_add_session(struct uc_session_node *, uint32_t);
int32_t ppa_hw_update_session_extra(struct uc_session_node *, uint32_t);
void ppa_hw_del_session(struct uc_session_node *);
#if defined(CONFIG_PPA_HANDLE_CONNTRACK_SESSIONS)
uint32_t ppa_decide_collision(struct uc_session_node *);
#endif

/*	multicast routing operation*/
int32_t __ppa_lookup_mc_group(IP_ADDR_C *, IP_ADDR_C *, struct mc_session_node **);
int32_t ppa_mc_group_setup(PPA_MC_GROUP *p_mc_group, struct mc_session_node *p_item, uint32_t flags);
int32_t ppa_mc_group_checking(PPA_MC_GROUP *, struct mc_session_node *, uint32_t);
int32_t ppa_add_mc_group(PPA_MC_GROUP *, struct mc_session_node **, uint32_t);
int32_t ppa_update_mc_group_extra(PPA_SESSION_EXTRA *, struct mc_session_node *, uint32_t);
void ppa_remove_mc_group(struct mc_session_node *);
void ppa_delete_mc_group(PPA_MC_GROUP *ppa_mc_entry);

int32_t ppa_mc_group_start_iteration(uint32_t *, struct mc_session_node **);
int32_t ppa_mc_group_iterate_next(uint32_t *, struct mc_session_node **);
void ppa_mc_group_stop_iteration(void);
void ppa_mc_get_htable_lock(void);
void ppa_mc_release_htable_lock(void);
uint32_t ppa_get_mc_group_count(uint32_t);

/*	multicast routing hardware/firmware operation*/
int32_t ppa_hw_add_mc_group(struct mc_session_node *);
int32_t ppa_hw_update_mc_group_extra(struct mc_session_node *, uint32_t);
#if defined(RTP_SAMPLING_ENABLE) && RTP_SAMPLING_ENABLE
int32_t ppa_hw_set_mc_rtp(struct mc_session_node *);
int32_t ppa_hw_get_mc_rtp_sampling_cnt(struct mc_session_node *);
#endif
void ppa_hw_del_mc_group(struct mc_session_node *);
int32_t ppa_update_mc_hw_group(struct mc_session_node *);
int32_t __ppa_mc_group_update(PPA_MC_GROUP *, struct mc_session_node *, uint32_t);

#if IS_ENABLED(CONFIG_PPA_MPE_IP97)
int ppa_find_session_for_ipsec(PPA_BUF *, uint8_t pf, struct uc_session_node **);
void ppa_ipsec_get_session_lock(void);
void ppa_ipsec_release_session_lock(void);
void ppa_remove_ipsec_group(struct uc_session_node *p_item);
int32_t ppa_ipsec_add_entry(uint32_t tunnel_index);
int32_t ppa_ipsec_add_entry_outbound(uint32_t tunnel_index);
int32_t __ppa_lookup_ipsec_group(PPA_XFRM_STATE *ppa_x, struct uc_session_node **pp_item);
int32_t ppa_ipsec_del_entry(struct uc_session_node *p_item);
int32_t ppa_ipsec_del_entry_outbound(uint32_t tunnel_index);
void    ppa_ipsec_session_list_free(void);

/*extern int32_t __ppa_update_ipsec_tunnelindex(struct uc_session_node *pp_item);*/
extern int32_t __ppa_update_ipsec_tunnelindex(PPA_BUF *ppa_buf, uint32_t *tunnel_index);

extern uint32_t ppa_ipsec_tunnel_tbl_routeindex(uint32_t tunnel_index, uint32_t routing_entry);

extern uint32_t ppa_get_ipsec_tunnel_tbl_entry(PPA_XFRM_STATE *entry, sa_direction *dir, uint32_t *tunnel_index);

extern struct ipsec_tunnel_intf ipsec_tnl_info;
#endif

/*	routing polling timer*/
void ppa_set_polling_timer(uint32_t, uint32_t);

/* ip test functions*/
uint32_t ip_equal(IP_ADDR_C *, IP_ADDR_C *);
unsigned int is_ip_allbit1(IP_ADDR_C *ip);
static inline uint32_t is_ip_zero(IP_ADDR_C *ip)
{
	if(ip->f_ipv6){
		return ((ip->ip.ip6[0] | ip->ip.ip6[1] | ip->ip.ip6[2] | ip->ip.ip6[3]) == 0);
	}else{
		return (ip->ip.ip == 0);
	}
}

/*	bridging mac operation*/
int32_t __ppa_bridging_lookup_mac(uint8_t *, uint16_t, PPA_NETIF *, struct br_mac_node **);
int32_t ppa_bridging_add_mac(uint8_t *, uint16_t, PPA_NETIF *, struct br_mac_node **, uint32_t);
int32_t ppa_bridging_update_mac(struct br_mac_node *,
	struct netif_info *, PPA_NETIF *, uint8_t *, uint32_t);
void ppa_bridging_remove_mac(struct br_mac_node *);
int32_t ppa_bridging_flush_macs(void);
void dump_bridging_list_item(struct br_mac_node *, char *);

int32_t ppa_bridging_session_start_iteration(uint32_t *, struct br_mac_node **);
int32_t ppa_bridging_session_iterate_next(uint32_t *, struct br_mac_node **);
void ppa_bridging_session_stop_iteration(void);
uint32_t ppa_get_br_count (uint32_t);

void ppa_br_get_htable_lock(void);
void ppa_br_release_htable_lock(void);

uint32_t ppa_del_hw_bridging_session_by_mac(uint8_t *src_mac, short *is_drop);

/*	bridging session hardware/firmware operation*/
int32_t ppa_bridging_hw_add_mac(struct br_mac_node *);
void ppa_bridging_hw_del_mac(struct br_mac_node *);

/*	bridging polling timer*/
void ppa_bridging_set_polling_timer(uint32_t);

/*	special function;*/
void ppa_remove_sessions_on_netif(PPA_NETIF *);
#if IS_ENABLED(CONFIG_PPA_API_DIRECTCONNECT)
void ppa_remove_sessions_on_subif(PPA_DP_SUBIF *subif);
void ppa_remove_sessions_on_macaddr(uint8_t *mac);
#endif
/*
 *	Init/Uninit Functions
 */
int32_t ppa_api_session_manager_init(void);
void ppa_api_session_manager_exit(void);
int32_t ppa_api_session_manager_create(void);
void ppa_api_session_manager_destroy(void);

int ppa_update_base_inf_mib(PPA_ITF_MIB_INFO *mib_tmp, uint32_t hal_id);
int32_t ppa_get_underlying_vlan_interface_if_eogre(PPA_NETIF *netif, PPA_NETIF **base_netif, uint8_t *isEoGre);
uint16_t	checksum(uint8_t *ip, int len);

uint32_t ppa_api_get_session_poll_timer(void);
uint32_t ppa_api_set_test_qos_priority_via_mark(PPA_BUF *ppa_buf);
uint32_t ppa_api_set_test_qos_priority_via_tos(PPA_BUF *ppa_buf);
void ppa_check_hit_stat_clear_mib(int32_t flag);
#if defined(PPA_IF_MIB) && PPA_IF_MIB
int32_t update_netif_hw_mib(PPA_IFNAME *ifname, uint64_t new_mib, uint8_t is_rx);
int32_t update_netif_sw_mib(PPA_IFNAME *ifname, uint64_t new_mib, uint8_t is_rx);
void clear_all_netif_mib(void);
void sw_del_session_mgmt_stats(struct uc_session_node *p_item);
#if IS_ENABLED(CONFIG_IPV4_IPV6_COUNTER_SUPPORT)
int32_t update_netif_hw_mib_ip_based(PPA_IFNAME *ifname, uint64_t new_mib, uint8_t is_rx, uint8_t is_ipv4);
int32_t update_netif_sw_mib_ip_based(PPA_IFNAME *ifname, uint64_t new_mib, uint8_t is_rx, uint8_t is_ipv4);
#endif /* CONFIG_IPV4_IPV6_COUNTER_SUPPORT */
#endif /* PPA_IF_MIB */
/* @} */

void ppa_init_mc_session_node(struct mc_session_node *);

int32_t get_ppa_hash_count(PPA_CMD_COUNT_INFO *count_info);

/*
 * Following functions related to session list are intentionally re-dfined here
 * These routines are used by other PPA files
 */
uint32_t ppa_sesssion_get_count_in_hash(uint32_t hashIndex);
int32_t ppa_session_get_items_in_hash(uint32_t index, /* Hash index */
		struct uc_session_node **pp_item,
		uint32_t maxToCopy,
		uint32_t *copiedItems,
		uint32_t flag);

void ppa_session_list_lock(void);
void ppa_session_list_unlock(void);

void ppa_session_bucket_lock(uint32_t);
void ppa_session_bucket_unlock(uint32_t);

void ppa_session_not_bridged(struct uc_session_node *p_item,
		PPA_SESSION *p_session);

uint32_t ppa_session_get_routing_count(uint16_t, uint32_t, uint32_t);


uint32_t ppa_get_hit_polling_time(void);
void ppa_session_bridged_flow_set_status(uint8_t fEnable);
uint8_t ppa_session_bridged_flow_status(void);

#if defined(L2TP_CONFIG) && L2TP_CONFIG
void ppa_pppol2tp_get_l2tpinfo(struct net_device *dev, PPA_L2TP_INFO *l2tpinfo);
uint16_t	checksum_l2tp(uint8_t *ip, int len);
#endif
#if IS_ENABLED(CONFIG_PPA_EXT_PKT_LEARNING)
/**
 * parsing hook type, RX or TX
 */
enum {
	PKT_RX, /* packet RX */
	PKT_TX, /* packet TX */
};
int32_t ppa_get_hw_hash_from_skb(PPA_BUF *skb, uint32_t *hash1, uint32_t *hash2, uint32_t* hash3, uint8_t *);
#endif /*IS_ENABLED(CONFIG_PPA_EXT_PKT_LEARNING)*/

#endif	/*	__PPA_API_SESSION_H__20081104_1139__*/
