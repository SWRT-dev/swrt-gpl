/*
 * Description: PP field vector APIs
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018-2020 Intel Corporation
 */

#define pr_fmt(fmt) "[PP_FV]: %s:%d: " fmt, __func__, __LINE__

#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/if_ether.h>
#include <linux/ppp_defs.h>
#include <linux/pp_api.h>
#include <linux/pktprs.h>

#include "pp_fv.h"

/**
 * @brief Helper function to convert packet parser header to
 *        PP field vector l4 structure
 */
static void __fv_l4_set(struct pktprs_hdr *pkt, struct pp_fv *fv, u8 lvl)
{
	union pp_fv_l4 *l4;
	union proto_ptr p;

	if (lvl)
		l4 = &fv->second.l4;
	else
		l4 = &fv->first.l4;

	p.udp = pktprs_udp_hdr(pkt, lvl);
	if (p.udp) {
		l4->udp.src = p.udp->source;
		l4->udp.dst = p.udp->dest;
		if (PKTPRS_IS_ESP(pkt, lvl)) {
			l4 = &fv->second.l4;
			goto esp;
		}
		return;
	}

	p.tcp = pktprs_tcp_hdr(pkt, lvl);
	if (p.tcp) {
		l4->tcp.src = p.tcp->source;
		l4->tcp.dst = p.tcp->dest;
		return;
	}

	p.sctp = pktprs_sctp_hdr(pkt, lvl);
	if (p.sctp) {
		l4->sctp.src = p.sctp->source;
		l4->sctp.dst = p.sctp->dest;
		return;
	}

	p.icmp = pktprs_icmp_hdr(pkt, lvl);
	if (p.icmp && p.icmp->code == 0) {
		l4->icmp.code = p.icmp->un.echo.id;
		l4->icmp.type = p.icmp->type;
		return;
	}

	p.icmp6 = pktprs_icmp6_hdr(pkt, lvl);
	if (p.icmp6 && p.icmp6->icmp6_code == 0) {
		l4->icmp.code = p.icmp6->icmp6_dataun.u_echo.identifier;
		l4->icmp.type = p.icmp6->icmp6_type;
		return;
	}

esp:
	p.esp = pktprs_esp_hdr(pkt, lvl);
	if (p.esp) {
		l4->esp.spi = p.esp->spi;
		return;
	}

	p.l2tp = pktprs_l2tp_oip_hdr(pkt, lvl);
	if (p.l2tp) {
		l4->l2tpoip.sess_id = p.l2tp->v3_oip.sess_id;
		return;
	}
}

/**
 * @brief Helper function to convert packet parser header to
 *        PP field vector l3 structure
 */
static void __fv_l3_set(struct pktprs_hdr *pkt, struct pp_fv *fv, u8 lvl)
{
	union pp_fv_l3 *l3;
	union proto_ptr p;

	if (lvl)
		l3 = &fv->second.l3;
	else
		l3 = &fv->first.l3;

	p.ipv4 = pktprs_ipv4_hdr(pkt, lvl);
	if (p.ipv4) {
		l3->v4.tos      = p.ipv4->tos;
		l3->v4.protocol = p.ipv4->protocol;
		l3->v4.saddr    = p.ipv4->saddr;
		l3->v4.daddr    = p.ipv4->daddr;
		return;
	}

	p.ipv6 = pktprs_ipv6_hdr(pkt, lvl);
	if (p.ipv6) {
		l3->v6.priority = p.ipv6->priority;
		l3->v6.version  = p.ipv6->version;
		memcpy(l3->v6.flow_lbl, p.ipv6->flow_lbl,
		       sizeof(l3->v6.flow_lbl));
		memcpy(&l3->v6.saddr, &p.ipv6->saddr, sizeof(l3->v6.saddr));
		memcpy(&l3->v6.daddr, &p.ipv6->daddr, sizeof(l3->v6.daddr));
		return;
	}
}

/**
 * @brief Helper function to convert packet parser header to
 *        PP field vector l2 structure
 */
static void __fv_l2_set(struct pktprs_hdr *pkt, struct pp_fv *fv, u8 lvl)
{
	struct pp_fv_l2 *l2;
	union proto_ptr p;

	if (lvl)
		l2 = &fv->second.l2;
	else
		l2 = &fv->first.l2;

	p.eth = pktprs_eth_hdr(pkt, lvl);
	if (p.eth) {
		memcpy(l2->h_dst, p.eth->h_dest,   ETH_ALEN);
		memcpy(l2->h_src, p.eth->h_source, ETH_ALEN);
		l2->h_prot = p.eth->h_proto;
	}

	p.vlan = pktprs_vlan_hdr(pkt, lvl, 0);
	if (p.vlan) {
		l2->ext_vlan = p.vlan->h_vlan_TCI;
		l2->h_prot = p.vlan->h_vlan_encapsulated_proto;
	}

	p.vlan = pktprs_vlan_hdr(pkt, lvl, 1);
	if (p.vlan) {
		l2->int_vlan = p.vlan->h_vlan_TCI;
		l2->h_prot = p.vlan->h_vlan_encapsulated_proto;
	}

	p.pppoe = pktprs_pppoe_hdr(pkt, lvl);
	if (p.pppoe) {
		l2->pppoe_id = p.pppoe->sid;
		l2->h_prot = *((__be16 *)((u8 *)p.pppoe + sizeof(*p.pppoe)));
	}
}

/**
 * @brief Helper function to convert packet parser header
 *        to PP field vector tunnels structure
 */
static void __fv_tunn_lvl_set(struct pktprs_hdr *pkt, struct pp_fv *fv, u8 lvl)
{
	union pp_fv_tunnel *tun = &fv->tunn;
	union proto_ptr p;

	p.vxlan = pktprs_vxlan_hdr(pkt, lvl);
	if (p.vxlan) {
		tun->vxlan.vni = p.vxlan->vx_vni;
		return;
	}

	p.geneve = pktprs_geneve_hdr(pkt, lvl);
	if (p.geneve) {
		memcpy(&tun->geneve.vni, &p.geneve->vni,
		       sizeof(tun->geneve.vni));
		return;
	}

	p.l2tp = pktprs_l2tp_oudp_hdr(pkt, lvl);
	if (p.l2tp) {
		if ((p.l2tp->v2.flags & L2TP_HDR_VER_MASK) == L2TP_HDR_VER_2) {
			if (p.l2tp->v2.flags & L2TP_HDRFLAG_L) {
				tun->l2tpoudp.sess_id = p.l2tp->v2_len.sess_id;
				tun->l2tpoudp.tunn_id = p.l2tp->v2_len.tunnel;
			} else {
				tun->l2tpoudp.sess_id = p.l2tp->v2.sess_id;
				tun->l2tpoudp.tunn_id = p.l2tp->v2.tunnel;
			}
		} else {
			tun->l2tpv3oudp.sess_id = p.l2tp->v3_oudp.sess_id;
		}
	}
}

/**
 * @brief Helper function to convert packet parser header
 *        to PP field vector headers structure
 */
static void __fv_hdrs_lvl_set(struct pktprs_hdr *pkt, struct pp_fv *fv, u8 lvl)
{
	__fv_l2_set(pkt, fv, lvl);
	__fv_l3_set(pkt, fv, lvl);
	__fv_l4_set(pkt, fv, lvl);
}

s32 pp_fv_build(struct pktprs_hdr *pkt, struct pp_fv *fv)
{
	if (unlikely(ptr_is_null(fv)))
		return -EINVAL;

	if (!pkt)
		return 0; /* no packet FV */

	__fv_hdrs_lvl_set(pkt, fv, PKTPRS_HDR_LEVEL0);
	__fv_tunn_lvl_set(pkt, fv, PKTPRS_HDR_LEVEL0);
	__fv_hdrs_lvl_set(pkt, fv, PKTPRS_HDR_LEVEL1);

	return 0;

}

u8 pp_fv_proto_set(struct pktprs_hdr *pkt, u8 lvl)
{
	u8 proto = PP_FV_UNKNOWN_L3;
	bool other_l4 = false;
	bool tunn_other_l4 = false;

	/* set other l4 protocols and tunnels for outer proto */
	if (!lvl) {
		other_l4 = PKTPRS_IS_SCTP(pkt, lvl) ||
			   PKTPRS_IS_ESP(pkt, lvl)  ||
			   PKTPRS_IS_ICMP(pkt, lvl) ||
			   PKTPRS_IS_ICMP6(pkt, lvl);
		tunn_other_l4 = PKTPRS_IS_L2TP_OIP(pkt, lvl) ||
				PKTPRS_IS_GRE(pkt, lvl);
	}

	if (PKTPRS_IS_IPV4(pkt, lvl)) {
		if (PKTPRS_IS_TCP(pkt, lvl))
			proto = PP_FV_IPV4_TCP;
		else if (PKTPRS_IS_UDP(pkt, lvl))
			proto = PP_FV_IPV4_UDP;
		else if (other_l4 || tunn_other_l4)
			proto = PP_FV_IPV4_OTHER_L4;
		else
			proto = PP_FV_IPV4_UNKNOWN_L4;
	} else if (PKTPRS_IS_IPV6(pkt, lvl)) {
		if (PKTPRS_IS_TCP(pkt, lvl))
			proto = PP_FV_IPV6_TCP;
		else if (PKTPRS_IS_UDP(pkt, lvl))
			proto = PP_FV_IPV6_UDP;
		else if (other_l4 || tunn_other_l4)
			proto = PP_FV_IPV6_OTHER_L4;
		else
			proto = PP_FV_IPV6_UNKNOWN_L4;
	} else {
		if (PKTPRS_IS_TCP(pkt, lvl))
			proto = PP_FV_OTHER_L3_TCP;
		else if (PKTPRS_IS_UDP(pkt, lvl))
			proto = PP_FV_OTHER_L3_UDP;
		else if (other_l4 || tunn_other_l4)
			proto = PP_FV_OTHER_L3_OTHER_L4;
		else
			proto = PP_FV_UNKNOWN_L3;
	}

	return proto;
}
