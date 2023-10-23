/*
 * Description: PP parser HAL protocols definitions, this file includes
 *              all the definitions or header files includes which
 *              the parser driver will ever need
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018-2019 Intel Corporation
 */

#ifndef __PARSER_PROTOCOLS_H__
#define __PARSER_PROTOCOLS_H__

#include <linux/in.h>
#include <linux/if_ether.h>
#include <linux/if_vlan.h>
#include <linux/if_pppox.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/sctp.h>
#include <linux/icmp.h>
#include <linux/icmpv6.h>
#include <linux/l2tp.h>
#include <linux/ppp_defs.h>
#include <net/ip.h>
#include <net/ipv6.h>
#include <net/gre.h>
#include <net/geneve.h>
#include <net/vxlan.h>
#ifdef CONFIG_L2TP
#include <../net/l2tp/l2tp_core.h>
#endif

/**
 * @brief Enum defining the internal IDs for all protocols
 *        which are supported by the parser driver. <br>
 *        The Convention is to add 'INT' postfix for every internal
 *        protocol excluding the layer 2 VLANs protocols
 */
enum prsr_proto_id {
	PRSR_PROTO_FIRST, /* First Entry !!! */

	PRSR_PROTO_MAC = PRSR_PROTO_FIRST,
	PRSR_PROTO_L2_FIRST,
	PRSR_PROTO_EXT_VLAN = PRSR_PROTO_L2_FIRST, /* ETH_P_8021Q */
	PRSR_PROTO_STAG, /* ETH_P_8021AD */
	PRSR_PROTO_VLAN, /* ETH_P_8021Q */
	PRSR_PROTO_PPPOE, /* ETH_P_PPP_SES */
	PRSR_PROTO_SNAP, /* ETH_P_802_3_MIN */
	PRSR_PROTO_L2_LAST = PRSR_PROTO_SNAP,
	PRSR_PROTO_UPPER_FIRST,
	/* first upper level protocol */
	PRSR_PROTO_PTP = PRSR_PROTO_UPPER_FIRST, /* ETH_P_1588 */
	PRSR_PROTO_IPV4, /* ETH_P_IP */
	PRSR_PROTO_IPV6, /* ETH_P_IPV6 */
	PRSR_PROTO_PPPOE_IPV4, /* PPP_IP */
	PRSR_PROTO_PPPOE_IPV6, /* PPP_IPV6 */
	PRSR_PROTO_IP_SEL, /* IP Select */
	PRSR_PROTO_ICMP, /* IPPROTO_ICMP */
	PRSR_PROTO_IPV4_INT, /* ETH_P_IP */
	PRSR_PROTO_TCP, /* IPPROTO_TCP */
	PRSR_PROTO_UDP, /* IPPROTO_UDP */
	PRSR_PROTO_IPV6_INT, /* ETH_P_IPV6 */
	PRSR_PROTO_GRE, /* IPPROTO_GRE */
	PRSR_PROTO_ESP, /* IPPROTO_ESP */
	PRSR_PROTO_L2TPV3_IP, /* IPPROTO_L2TP */
	PRSR_PROTO_SCTP, /* IPPROTO_SCTP */
	PRSR_PROTO_UDPLITE, /* IPPROTO_UDPLITE */
	PRSR_PROTO_IPV6EXT, /* IPPROTO_HOPOPTS/ROUTING/DSTOPTS */
	PRSR_PROTO_IPV6FRAG, /* IPPROTO_FRAGMENT */
	PRSR_PROTO_ICMPV6, /* IPPROTO_ICMPV6 */
	PRSR_PROTO_PPP, /* PPP always following L2TP */
	PRSR_PROTO_L2TP_SEL, /* L2TP_UDP_DPORT */
	PRSR_PROTO_L2TPV2_START, /* L2TPV2_START */
	PRSR_PROTO_L2TPV2_LEN, /* L2TPV2_LEN_PRESENT */
	PRSR_PROTO_L2TPV2_NO_LEN, /* L2TPV2_NO_LEN_PRESENT */
	PRSR_PROTO_L2TPV2_NO_PAD, /* L2TPV2_NO_SEQ_NO_PAD/L2TPV2_SEQ_NO_PAD */
	PRSR_PROTO_L2TPV3_UDP, /* L2TPV3_UDP_VER */
	PRSR_PROTO_VXLAN, /* VXLAN_UDP_DPORT */
	PRSR_PROTO_GENEVE, /* GENEVE_UDP_DPORT */
	PRSR_PROTO_TCP_INT, /* IPPROTO_TCP */
	PRSR_PROTO_UDP_INT, /* IPPROTO_UDP */
	PRSR_PROTO_UDPLITE_INT, /* IPPROTO_UDPLITE */
	PRSR_PROTO_IPV6EXT_INT, /* IPPROTO_HOPOPTS/ROUTING/DSTOPTS */
	PRSR_PROTO_IPV6FRAG_INT, /* IPPROTO_FRAGMENT */
	PRSR_PROTO_MAC_INT, /* MAC addresses */
	PRSR_PROTO_VLAN_INT, /* ETH_P_8021Q */
	PRSR_PROTO_QINQ, /* ETH_P_8021AD */
	PRSR_PROTO_PPPOE_INT, /* ETH_P_PPP_SES */
	PRSR_PROTO_ESP_INT, /* IPPROTO_ESP */
	PRSR_PROTO_PAYLOAD, /* Payload */
	PRSR_PROTO_IDLE, /* Idle */
	PRSR_PROTO_NO_PARSE, /* Dummy protocol to support no parsing */
	/* reserved region for dynamic allocation */
	PRSR_PROTO_RSRV_FIRST,
	PRSR_PROTO_RSRV_0 = PRSR_PROTO_RSRV_FIRST,
	PRSR_PROTO_RSRV_1,
	PRSR_PROTO_RSRV_2,
	PRSR_PROTO_RSRV_3,
	PRSR_PROTO_RSRV_4,
	PRSR_PROTO_RSRV_5,
	PRSR_PROTO_RSRV_6,
	PRSR_PROTO_RSRV_7,
	PRSR_PROTO_RSRV_8,
	PRSR_PROTO_RSRV_LAST = PRSR_PROTO_RSRV_8,
	PRSR_PROTO_UPPER_LAST = PRSR_PROTO_RSRV_LAST,

	/* Last Entry !!! */
	PRSR_PROTOS_NUM,
	/* Invalid id */
	PRSR_PROTO_INVALID = PRSR_PROTOS_NUM
};

/**
 * @brief Shortcut for iterating array of protocols
 * @param i iterator
 */
#define PRSR_FOR_EACH_PROTO(i) \
	for (i = PRSR_PROTO_FIRST; i < PRSR_PROTOS_NUM; i++)

/**
 * @brief Shortcut for testing whether a protocol id is layer 2
 *        protocol id or not
 * @param id id to test
 */
#define PRSR_IS_L2_PROTO(id) \
	(id >= PRSR_PROTO_L2_FIRST && id <= PRSR_PROTO_L2_LAST)

/**
 * @brief Shortcut for testing whether a protocol id is upper layer
 *        protocol id or not
 * @param id id to test
 */
#define PRSR_IS_UPPER_LAYER_PROTO(id) \
	(id >= PRSR_PROTO_UPPER_FIRST && id <= PRSR_PROTO_UPPER_LAST)

/* IP select stuff */
#define IP_SELECT_NEXT_HDR_OFF 4
#define IP_SELECT_NEXT_HDR_LEN 4

/* L2TP select */
#define L2TP_SELECT_VER_OFF    8
#define L2TP_SELECT_VER_LEN    8
#define L2TP_UDP_DPORT         1701

/* L2TPv2 stuff */
#define L2TPV2_VER             2
#define L2TPV2_TUNN_SESS_LEN   4
#define L2TPV2_TUNN_SESS_OFF   2  // Offset when there are no optional fields
#define L2TPV2_BASE_HDR_SIZE   6  // Header size with no optional fields

/* L2TPv2 keys to np_select table */
#define L2TPV2_START           (L2TPV2_VER) // Key to start parsing L2TP v2
#define L2TPV2_LEN_PRESENT     4  // |T|L|x|x| => 0100
#define L2TPV2_NO_LEN_PRESENT  0  // |T|L|x|x| => 0000
#define L2TPV2_NO_SEQ_NO_PAD   0  // |T|L|x|x|S|x|O| => 0000000
#define L2TPV2_SEQ_NO_PAD      4  // |T|L|x|x|S|x|O| => 0000100

/* Optional L2TPv2 length field */
#define L2TPV2_OPT_LEN_OFF     2
#define L2TPV2_OPT_LEN_LEN     2

/* PPP */
#define PPP_HDR_LEN            4
#define PPP_PROTO_OFF          2
#define PPP_PROTO_LEN          2

/* L2TPv3 stuff */
#define L2TPV3_VER             3
#define L2TPV3_BASE_HDR_SIZE   8
#define L2TPV3_TUNN_SESS_OFF   4
#define L2TPV3_TUNN_SESS_LEN   4

/* L2TPv3 IP stuff */
#define L2TPV3IP_HDR_LEN       4
#define L2TPV3IP_SESS_ID_SZ    4
#define L2TPV3IP_TUNN_SESS_OFF 0
#define L2TPV3IP_TUNN_SESS_LEN 4

/* VxLAN stuff*/
#define VXLAN_UDP_DPORT        4789

/* ESP stuff*/
#define ESP_UDP_DPORT          4500

/* Geneve stuff*/
#define GENEVE_UDP_DPORT       6081
#define GENEVE_OPTS_OFF        2
#define GENEVE_OPTS_LEN        6

#endif /* __PARSER_PROTOCOLS_H__ */
