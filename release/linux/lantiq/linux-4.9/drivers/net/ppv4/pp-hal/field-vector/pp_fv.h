/*
 * Description: PP field vector definitions
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018-2020 Intel Corporation
 */

#ifndef __PP_FV_H__
#define __PP_FV_H__

#include <linux/types.h>
#include <linux/in6.h>
#include <linux/if_ether.h>
#include <linux/netdevice.h>
#include <linux/l2tp.h>
#include <linux/ppp_defs.h>

#include "pp_regs.h"

/**
 * @brief external/internal_proto_info
 */
enum pp_fv_proto_info {
	PP_FV_UNKNOWN_L3,
	PP_FV_IPV4_UNKNOWN_L4,
	PP_FV_IPV4_UDP,
	PP_FV_IPV4_TCP,
	PP_FV_IPV4_RESERVED_L4,
	PP_FV_IPV4_OTHER_L4,
	PP_FV_IPV6_UNKNOWN_L4,
	PP_FV_IPV6_UDP,
	PP_FV_IPV6_TCP,
	PP_FV_IPV6_RESERVED_L4,
	PP_FV_IPV6_OTHER_L4,
	PP_FV_OTHER_L3_UNKNOWN_L4,
	PP_FV_OTHER_L3_UDP,
	PP_FV_OTHER_L3_TCP,
	PP_FV_OTHER_L3_RESERVED_L4,
	PP_FV_OTHER_L3_OTHER_L4,
	PP_FV_PROTO_INFO_MAX
};

/**
 * @brief PP field vector OOB classified info
 * @in_pid ingress PP port id
 * @proto_info external and internal protocols info <br>
 *            [7-4]: external protocol info
 *            [3-0]: internal protocol info
 * @stw_data data copied from status word by the parser HW according
 *           to a per port configuration
 */
struct pp_fv_oob {
	__u8   in_pid;
	__u8   proto_info;
	__be16 stw_data;
} __packed;

/**
 * @brief PP field vector external protocol info set
 */
#define PP_FV_OOB_EXT_PROTO_INFO_SET(pi, info)	\
		PP_FIELD_MOD((__u8)GENMASK(7, 4), (__u8)info, (__u8)pi)
/**
 * @brief PP field vector external protocol info get
 */
#define PP_FV_OOB_EXT_PROTO_INFO_GET(pi)		\
		PP_FIELD_GET((__u8)GENMASK(7, 4), (__u8)pi)
/**
 * @brief PP field vector internal protocol info set
 */
#define PP_FV_OOB_INT_PROTO_INFO_SET(pi, info)	\
		PP_FIELD_MOD((__u8)GENMASK(3, 0), (__u8)info, (__u8)pi)
/**
 * @brief PP field vector internal protocol info get
 */
#define PP_FV_OOB_INT_PROTO_INFO_GET(pi)		\
		PP_FIELD_GET((__u8)GENMASK(3, 0), (__u8)pi)

/**
 * @brief PP field vector layer 2 fields
 * @note some fields were copied from 'struct ethhdr'
 */
struct pp_fv_l2 {
	__u8   h_dst[ETH_ALEN]; /* destination eth addr */
	__u8   h_src[ETH_ALEN]; /* source ether addr    */
	__be16 ext_vlan;        /* external VLAN        */
	__be16 int_vlan;        /* internal VLAN        */
	__be16 h_prot;          /* packet type ID field */
	__be16 pppoe_id;        /* PPPoE session ID     */
} __packed;

/**
 * @brief PP field vector ipv6 fields
 * @note subset of 'struct ipv6hdr'
 */
struct pp_fv_ipv6 {
#if defined(__LITTLE_ENDIAN_BITFIELD)
	__u8   priority:4,
	       version:4;
#elif defined(__BIG_ENDIAN_BITFIELD)
	__u8   version:4,
	       priority:4;
#endif
	__u8   flow_lbl[3];
	struct in6_addr	saddr;
	struct in6_addr	daddr;
} __packed;

/**
 * @brief PP field vector ipv4 fields
 * @note subset of 'struct iphdr'
 */
struct pp_fv_ipv4 {
#if defined(__LITTLE_ENDIAN_BITFIELD)
	__u8   ihl:4,
	       version:4;
#elif defined(__BIG_ENDIAN_BITFIELD)
	__u8   version:4,
	       ihl:4;
#endif
	__u8   tos;
	__be16 tot_len;
	__be16 id;
	__be16 frag_off;
	__u8   ttl;
	__u8   protocol;
	__be32 saddr;
	__be32 daddr;
} __packed;

/**
 * @brief PP field vector layer 3 fields
 */
union pp_fv_l3 {
	struct pp_fv_ipv4 v4;
	struct pp_fv_ipv6 v6;
} __packed;

/**
 * @brief source and destination layer 4 ports for TCP, UDP, and SCTP
 *        protocols
 */
struct pp_fv_l4_ports {
	__be16 src;
	__be16 dst;
} __packed;

/**
 * @brief PP field vector icmp fields
 * @note subset of 'struct icmphdr' and 'struct icmp6hdr'
 */
struct pp_fv_icmp {
	__u8 type;
	__u8 code;
} __packed;

/**
 * @brief PP field vector esp field
 * @note subset of 'struct ip_esp_hdr'
 */
struct pp_fv_esp {
	__be32 spi;
} __packed;

/**
 * @brief PP field vector l2tp over ip fields
 */
struct pp_fv_l2tpoip {
	__be32 sess_id; // can we unify pp_fv_l2tpoip and pp_fv_l2tpoudp?
} __packed;

/**
 * @brief PP field vector l4 fields
 */
union pp_fv_l4 {
	struct pp_fv_l4_ports tcp;
	struct pp_fv_l4_ports udp;
	struct pp_fv_l4_ports sctp;
	struct pp_fv_icmp     icmp;
	struct pp_fv_l2tpoip  l2tpoip;
	struct pp_fv_esp      esp;
} __packed;

/**
 * @brief PP field vector VxLAN fields
 * @note subset of 'struct vxlanhdr'
 */
struct pp_fv_vxlan {
	__be32 vni;
} __packed;

/**
 * @brief PP field vector geneve fields
 * @note subset of 'struct genevehdr'
 */
struct pp_fv_gnv {
	__u8 vni[3];
} __packed;

/**
 * @brief PP field vector SCTP fields
 * @note subset of 'struct sctphdr'
 */
struct pp_fv_sctp {
	__be32 vtag;
} __packed;

/**
 * @brief PP field vector l2tp over UDP session ID and tunnel ID
 */
struct pp_fv_l2tpoudp {
	__be16 tunn_id;
	__be16 sess_id;
} __packed;

/**
 * @brief PP field vector l2tp v3 over UDP session ID
 */
struct pp_fv_l2tpv3oudp {
	__be32 sess_id;
} __packed;

/**
 * @brief PP field vector tunnel fields
 */
union pp_fv_tunnel {
	struct pp_fv_vxlan      vxlan;
	struct pp_fv_gnv        geneve;
	struct pp_fv_sctp       sctp;
	struct pp_fv_l2tpoudp   l2tpoudp;
	struct pp_fv_l2tpv3oudp l2tpv3oudp;
} __packed;

/**
 * @brief PP field vector packet headers fields
 */
struct pp_fv_headers {
	struct pp_fv_l2 l2;
	union  pp_fv_l3 l3;
	union  pp_fv_l4 l4;
} __packed;

/**
 * @brief PPv4 HW field vector data structure definition. This
 *        structure defines the field vector as defined in the PPv4
 *        HW.
 * @oob  oob classified info
 * @out  outer headers info
 * @tunn tunnel info
 * @in   inner headers info
 */
struct __aligned(128) pp_fv {
	struct pp_fv_oob     oob;
	struct pp_fv_headers first;
	union  pp_fv_tunnel  tunn;
	struct pp_fv_headers second;
} __packed;

#define PP_FV_IS_L3_IPV4(proto_info) \
	(proto_info >= PP_FV_IPV4_UNKNOWN_L4 && \
	 proto_info <= PP_FV_IPV4_OTHER_L4)

#define PP_FV_IS_L3_IPV6(proto_info) \
	(proto_info >= PP_FV_IPV6_UNKNOWN_L4 && \
	 proto_info <= PP_FV_IPV6_OTHER_L4)

#define PP_FV_IS_L3_IPV6_OTHER_L4(proto_info) \
	(proto_info == PP_FV_IPV6_OTHER_L4)

#define PP_FV_IS_L4_TCP(proto_info) \
	(proto_info == PP_FV_IPV4_TCP || \
	 proto_info == PP_FV_IPV6_TCP || \
	 proto_info == PP_FV_OTHER_L3_TCP)

#define PP_FV_IS_L4_UDP(proto_info) \
	(proto_info == PP_FV_IPV4_UDP || \
	 proto_info == PP_FV_IPV6_UDP || \
	 proto_info == PP_FV_OTHER_L3_UDP)

#define PP_FV_ETHTYPE_STR(h_proto) \
	(ntohs(h_proto) == ETH_P_IP   ? "IPv4" :       \
	(ntohs(h_proto) == ETH_P_IPV6 ? "IPv6" :       \
	(ntohs(h_proto) == PPP_IP     ? "PPPoE IPv4" : \
	(ntohs(h_proto) == PPP_IPV6   ? "PPPoE IPv6" : \
	("Other")))))

#define PP_FV_L3_PROTOCOL_STR(proto) \
	(proto == IPPROTO_ICMP   ? "ICMP"   : \
	(proto == IPPROTO_ICMPV6 ? "ICMPv6" : \
	(proto == IPPROTO_TCP    ? "TCP"    : \
	(proto == IPPROTO_UDP    ? "UDP"    : \
	(proto == IPPROTO_ESP    ? "ESP"    : \
	(proto == IPPROTO_SCTP   ? "SCTP"   : \
	(proto == IPPROTO_L2TP   ? "L2TP"   : \
	(proto == IPPROTO_GRE    ? "GRE"    : \
	(proto == IPPROTO_IGMP   ? "IGMP"   : \
	(proto == IPPROTO_SCTP   ? "SCTP"   : \
	("Other")))))))))))

#define PP_FV_PROTOCOL_STR(proto) \
	(proto == PP_FV_UNKNOWN_L3           ? "UNKNOWN_L3"           : \
	(proto == PP_FV_IPV4_UNKNOWN_L4      ? "IPV4_UNKNOWN_L4"      : \
	(proto == PP_FV_IPV4_UDP             ? "IPV4_UDP"             : \
	(proto == PP_FV_IPV4_TCP             ? "IPV4_TCP"             : \
	(proto == PP_FV_IPV4_RESERVED_L4     ? "IPV4_RESERVED_L4"     : \
	(proto == PP_FV_IPV6_UDP             ? "IPV6_UDP"             : \
	(proto == PP_FV_IPV6_TCP             ? "IPV6_TCP"             : \
	(proto == PP_FV_IPV6_RESERVED_L4     ? "IPV6_RESERVED_L4"     : \
	(proto == PP_FV_IPV6_OTHER_L4        ? "IPV6_OTHER_L4"        : \
	(proto == PP_FV_OTHER_L3_UNKNOWN_L4  ? "OTHER_L3_UNKNOWN_L4"  : \
	(proto == PP_FV_OTHER_L3_UDP         ? "OTHER_L3_UDP"         : \
	(proto == PP_FV_OTHER_L3_TCP         ? "OTHER_L3_TCP"         : \
	(proto == PP_FV_OTHER_L3_RESERVED_L4 ? "OTHER_L3_RESERVED_L4" : \
	(proto == PP_FV_OTHER_L3_OTHER_L4    ? "OTHER_L3_OTHER_L4"    : \
	("Other")))))))))))))))

/**
 * @brief Convert packet parser structure to PP field vector structure
 * @param pkt packet headers
 * @param pp pp field vector
 * @return s32 0 on success, error code otherwise
 */
s32 pp_fv_build(struct pktprs_hdr *pkt, struct pp_fv *pp);

/**
 * @brief Convert packet parser to pp field vector oob protocol info
 * @param pkt packet headers
 * @param lvl headers level (outer/inner)
 * @return u8
 */
u8 pp_fv_proto_set(struct pktprs_hdr *pkt, u8 lvl);
#endif /* __PP_FV_H__ */
