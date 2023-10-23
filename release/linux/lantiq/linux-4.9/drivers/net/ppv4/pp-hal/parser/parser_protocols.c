/*
 * Description: Parser protocols specific functions
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018-2020 Intel Corporation
 */

#define pr_fmt(fmt) "[PP_PARSER_PROTO]: %s:%d: " fmt, __func__, __LINE__

#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/bitops.h>

#include "infra.h"
#include "parser_protocols.h"
#include "pp_fv.h"
#include "parser.h"
#include "parser_internal.h"

/**
 * @brief L2 table protocols entries
 */
enum prsr_l2_entries {
	L2_ENT_EXT_VLAN,
	L2_ENT_STAG,
	L2_ENT_VLAN,
	L2_ENT_SNAP,
	L2_ENT_PPPOE,
};

/**
 * @brief Protocol info debug print using 'pr_debug'
 * @param info the protocol
 */
static inline void __proto_pr_debug(struct protocol_info *info)
{
	struct prsr_up_layer_proto_params *up;
	struct prsr_mac_proto_params      *mac;
	struct prsr_l2_proto_params       *l2;
	struct field_ext_info             *fld;
	u32 i;

	if (unlikely(!info))
		return;

	if (info->id == PRSR_PROTO_MAC) {
		mac = &info->mac;
		fld = &mac->mac_ext.fld[0];
		pr_debug("%s[%u]: prof_id %u, hdr_len %u",
			 mac->mac_name, info->id, info->prof_id, mac->hdr_len);
		pr_debug("%s ext: len %u, off %x, fv_idx %u, mask %u\n",
			 mac->mac_name, fld->len, fld->off,
			 fld->mask, fld->fv_idx);
		fld = &mac->ethtype_ext.fld[0];
		pr_debug("%s ext: len %u, off %x, fv_idx %u, mask %u\n",
			 mac->ethtype_name, fld->len, fld->off,
			 fld->mask, fld->fv_idx);
	} else if (PRSR_IS_L2_PROTO(info->id)) {
		l2 =  &info->l2;
		pr_debug("%s[%u]: prof_id %u, ethtype %x, hdr_len %u, fallback %u, cmp_op %u, hw_idx %u\n",
			 info->name, info->id, info->prof_id, l2->hw.ethtype,
			 l2->hw.hdr_len, l2->hw.fallback,
			 l2->hw.cmp_op, l2->hw_idx);
		for (i = 0; i < l2->ext.n_fld; i++) {
			fld = &l2->ext.fld[i];
			pr_debug("%s[%u]: extract[%u]: len %u, off %x, fv_idx %u, mask %u\n",
				 info->name, info->id, i, fld->len, fld->off,
				 fld->mask, fld->fv_idx);
		}
	} else if (PRSR_IS_UPPER_LAYER_PROTO(info->id)) {
		up = &info->up;
		pr_debug("%s[%u]: prof_id %u, proto_id %x: TBD\n",
			 info->name, info->id, info->prof_id, up->proto_id);
	}
}

/**
 * @brief init dummy protocol to represent the packet's payload
 */
static s32 __proto_payload_init(struct protocol_info *info)
{
	struct prsr_up_layer_proto_params *params;
	u32 n;

	if (unlikely(ptr_is_null(info)))
		return -EINVAL;

	params = &info->up;
	info->id       = PRSR_PROTO_PAYLOAD;
	info->prof_id  = PRSR_PROFILE_COMMON;
	info->name     = params->name; /* a reference to the name */
	n = strscpy(params->name, "Payload", sizeof(params->name));
	params->proto_id   = U16_MAX;
	params->hw_idx     = 48;
	params->np.logic   = NP_LOGIC_END;
	params->np.dflt    = PRSR_PROTO_IDLE;
	params->entry_type = ETYPE_PAYLOAD;

	__proto_pr_debug(info);
	pr_debug("done\n");

	return 0;
}

/**
 * @brief init dummy protocol to represent an idle IMEM entry
 */
static s32 __proto_idle_init(struct protocol_info *info)
{
	struct prsr_up_layer_proto_params *params;
	u32 n;

	if (unlikely(ptr_is_null(info)))
		return -EINVAL;

	params = &info->up;
	info->id       = PRSR_PROTO_IDLE;
	info->prof_id  = PRSR_PROFILE_COMMON;
	info->name     = params->name; /* a reference to the name */
	n = strscpy(params->name, "Idle", sizeof(params->name));
	params->proto_id   = U16_MAX;
	params->hw_idx     = 49;
	params->np.logic   = NP_LOGIC_END;
	params->np.dflt    = PRSR_PROTO_IDLE;
	params->ignore     = true;
	params->entry_type = ETYPE_NONE;

	__proto_pr_debug(info);
	pr_debug("done\n");

	return 0;
}

/**
 * @brief init dummy protocol to support no parsing, the protocol will
 *        just pretend that he has some l3 header of 14 bytes long so
 *        the packets will not be dropped in the checker due to a
 *        minimum header length of 14 bytes required by the modifier
 */
static s32 __proto_no_parse_init(struct protocol_info *info)
{
	struct prsr_up_layer_proto_params *params;
	u32 n;

	if (unlikely(ptr_is_null(info)))
		return -EINVAL;

	params = &info->up;
	info->id       = PRSR_PROTO_NO_PARSE;
	info->prof_id  = PRSR_PROFILE_COMMON;
	info->name     = params->name; /* a reference to the name */
	n = strscpy(params->name, "No Parse", sizeof(params->name));
	params->proto_id   = U16_MAX;
	params->hw_idx     = 62;
	params->entry_type = ETYPE_OTHER_L3;
	/* no next protocol, goto payload */
	params->np.logic   = NP_LOGIC_END;
	params->np.dflt    = PRSR_PROTO_PAYLOAD;
	/* set to the minimum header size required so the packets will
	 * no be dropped later on in the pipe
	 */
	params->skip.hdr_len_im = 14;
	params->skip.logic      = SKIP_LOGIC_IM;

	__proto_pr_debug(info);
	pr_debug("done\n");

	return 0;
}

/**
 * @brief Init mac protocol profile info
 */
static s32 __proto_mac_init(struct protocol_info *info)
{
	struct prsr_mac_proto_params *params;
	struct ext_info *ext;
	s32 n;

	if (unlikely(ptr_is_null(info)))
		return -EINVAL;

	params = &info->mac;
	info->id      = PRSR_PROTO_MAC;
	info->prof_id = PRSR_PROFILE_COMMON;
	info->name    = params->mac_name; /* a reference to the name */

	params->hdr_len = offsetof(struct ethhdr, h_proto);
	/* mac addresses */
	n = strscpy(params->mac_name, "MAC", sizeof(params->mac_name));
	ext = &params->mac_ext;
	ext->fld[0].len    = offsetof(struct ethhdr, h_proto);
	ext->fld[0].off    = offsetof(struct ethhdr, h_dest);
	ext->fld[0].fv_idx = offsetof(struct pp_fv, first.l2.h_dst);
	ext->fld[0].mask   = 0;
	ext->n_fld         = 1;
	/* ethertype */
	n = strscpy(params->ethtype_name, "Ethertype",
		    sizeof(params->ethtype_name));
	ext = &params->ethtype_ext;
	ext->fld[0].len    = FIELD_SIZEOF(struct ethhdr, h_proto);
	ext->fld[0].fv_idx = offsetof(struct pp_fv, first.l2.h_prot);
	ext->fld[0].off    = 0;
	ext->fld[0].mask   = 0;
	ext->n_fld         = 1;
	__proto_pr_debug(info);
	pr_debug("done\n");

	return 0;
}

/**
 * @brief Init internal mac protocol profile info, mac header which
 *        come after a tunnel
 */
static s32 __proto_mac_int_init(struct protocol_info *info)
{
	struct prsr_up_layer_proto_params *params;
	struct field_ext_info *fld;
	struct skip_info      *skip;
	struct np_info        *np;
	s32 n;

	if (unlikely(ptr_is_null(info)))
		return -EINVAL;

	params = &info->up;
	info->id      = PRSR_PROTO_MAC_INT;
	info->prof_id = PRSR_PROFILE_COMMON;
	info->name    = params->name; /* a reference to the name */
	n = strscpy(params->name, "MAC INT", sizeof(params->name));

	/* parsing info */
	params->proto_id   = U16_MAX; /* no protocol id for mac header */
	params->hw_idx     = 42;
	params->hdr_len    = 14;
	params->entry_type = ETYPE_L2;
	/* next protocol info */
	np = &params->np;
	np->off   = offsetof(struct ethhdr,  h_proto) * BITS_PER_BYTE;
	np->len   = FIELD_SIZEOF(struct ethhdr, h_proto) * BITS_PER_BYTE;
	np->logic = NP_LOGIC_TUNN;
	np->dflt  = PRSR_PROTO_PAYLOAD;
	/* header skip info */
	skip = &params->skip;
	skip->hdr_len_im  = sizeof(struct ethhdr);
	skip->logic       = SKIP_LOGIC_IM;
	/* preceding protocols */
	params->n_pre = 1;
	/* Tunnel -> MAC */
	params->pre[0].np_logic = NP_LOGIC_TUNN;
	params->pre[0].val      = ETH_P_TEB;
	/* for other protocols preceding internal mac this will be used as
	 * their default
	 */
	/* extract info */
	/* mac addresses */
	fld = &params->ext.fld[params->ext.n_fld++];
	fld->len    = offsetof(struct ethhdr, h_proto);
	fld->off    = offsetof(struct ethhdr, h_dest);
	fld->fv_idx = offsetof(struct pp_fv, second.l2.h_dst);
	fld->mask   = 0;
	/* ethertype */
	fld = &params->ext.fld[params->ext.n_fld++];
	fld->len    = FIELD_SIZEOF(struct ethhdr, h_proto);
	fld->off    = offsetof(struct ethhdr, h_proto);
	fld->fv_idx = offsetof(struct pp_fv, second.l2.h_prot);
	fld->mask   = 0;
	__proto_pr_debug(info);
	pr_debug("done\n");

	return 0;
}

static void __proto_vlan_common_init(struct protocol_info *info, char *name)
{
	struct prsr_l2_proto_params *params;
	s32 n;

	if (unlikely(!(!ptr_is_null(info) && !ptr_is_null(name))))
		return;

	params      = &info->l2;
	info->prof_id  = PRSR_PROFILE_COMMON;
	info->name     = params->name; /* a reference to the name */
	n = strscpy(params->name, name, sizeof(params->name));
	/* parsing info */
	params->hw.hdr_len    = offsetof(struct vlan_hdr,
					 h_vlan_encapsulated_proto);
	params->hw.fallback   = false;
	params->hw.cmp_op     = L2_CMP_OP_EQ;
	/* extracting the vlan id */
	params->ext.fld[0].len  = FIELD_SIZEOF(struct vlan_hdr, h_vlan_TCI);
	/* HW include the ethertype as part of the vlan header so we to add
	 * the h_proto size to reach to the VLAN id
	 */
	params->ext.fld[0].off  = offsetof(struct vlan_hdr, h_vlan_TCI) +
				  FIELD_SIZEOF(struct ethhdr, h_proto);
	params->ext.fld[0].mask = 0;
	params->ext.n_fld = 1;
}

/**
 * @brief Init external vlan protocol info
 */
static s32 __proto_ext_vlan_init(struct protocol_info *info)
{
	struct prsr_l2_proto_params *params;

	if (unlikely(ptr_is_null(info)))
		return -EINVAL;

	__proto_vlan_common_init(info, "EXT VLAN");

	params = &info->l2;
	info->id = PRSR_PROTO_EXT_VLAN;
	params->hw.ethtype = ETH_P_8021Q;
	/* L2 parsing requires the protocols to be in a specific order */
	params->hw_idx     = L2_ENT_EXT_VLAN;
	params->ext.fld[0].fv_idx = offsetof(struct pp_fv, first.l2.ext_vlan);
	__proto_pr_debug(info);
	pr_debug("done\n");

	return 0;
}

/**
 * @brief Init stag protocol info
 */
static s32 __proto_stag_init(struct protocol_info *info)
{
	struct prsr_l2_proto_params *params;

	if (unlikely(ptr_is_null(info)))
		return -EINVAL;

	__proto_vlan_common_init(info, "STAG");
	params = &info->l2;
	info->id = PRSR_PROTO_STAG;
	params->hw.ethtype = ETH_P_8021AD;
	/* L2 parsing requires the protocols to be in a specific order */
	params->hw_idx     = L2_ENT_STAG;
	params->ext.fld[0].fv_idx = offsetof(struct pp_fv, first.l2.ext_vlan);
	__proto_pr_debug(info);
	pr_debug("done\n");

	return 0;
}

/**
 * @brief Init l2 vlan protocol info
 */
static s32 __proto_vlan_init(struct protocol_info *info)
{
	struct prsr_l2_proto_params *params;

	if (unlikely(ptr_is_null(info)))
		return -EINVAL;

	__proto_vlan_common_init(info, "VLAN");

	params = &info->l2;
	info->id = PRSR_PROTO_VLAN;
	params->hw.ethtype = ETH_P_8021Q;
	/* L2 parsing requires the protocols to be in a specific order */
	params->hw_idx     = L2_ENT_VLAN;
	/* extract info */
	params->ext.fld[0].fv_idx = offsetof(struct pp_fv, first.l2.int_vlan);
	__proto_pr_debug(info);
	pr_debug("done\n");

	return 0;
}

/**
 * @brief Init internal vlan protocol profile info, vlan header which
 *        come after a tunnel
 */
static s32 __proto_vlan_int_init(struct protocol_info *info)
{
	struct prsr_up_layer_proto_params *params;
	struct field_ext_info *fld;
	struct skip_info      *skip;
	struct np_info        *np;
	s32 n;

	if (unlikely(ptr_is_null(info)))
		return -EINVAL;

	params = &info->up;
	info->id      = PRSR_PROTO_VLAN_INT;
	info->prof_id = PRSR_PROFILE_COMMON;
	info->name    = params->name; /* a reference to the name */
	n = strscpy(params->name, "VLAN INT", sizeof(params->name));

	/* parsing info */
	params->proto_id   = ETH_P_8021Q;
	params->hw_idx     = 43;
	params->hdr_len    = 6;
	params->entry_type = ETYPE_NONE;
	/* next protocol info */
	np = &params->np;
	np->off   = offsetof(struct vlan_hdr, h_vlan_encapsulated_proto) *
		    BITS_PER_BYTE;
	np->len   = FIELD_SIZEOF(struct vlan_hdr, h_vlan_encapsulated_proto) *
		    BITS_PER_BYTE;
	np->logic = NP_LOGIC_TUNN;
	np->dflt  = PRSR_PROTO_PAYLOAD;
	/* header skip info */
	skip = &params->skip;
	skip->hdr_len_im  = sizeof(struct vlan_hdr);
	skip->logic       = SKIP_LOGIC_IM;
	/* preceding protocols */
	params->n_pre = 1;
	/* Tunnel -> VLAN */
	params->pre[0].np_logic = NP_LOGIC_TUNN;
	params->pre[0].val      = ETH_P_8021Q;
	/* extract info */
	/* mac addresses */
	fld = &params->ext.fld[params->ext.n_fld++];
	fld->len    = offsetof(struct ethhdr, h_proto);
	fld->off    = offsetof(struct ethhdr, h_dest);
	fld->fv_idx = offsetof(struct pp_fv, second.l2.h_dst);
	fld->mask   = 0;
	/* VLAN ID */
	fld = &params->ext.fld[params->ext.n_fld++];
	fld->len    = FIELD_SIZEOF(struct vlan_hdr, h_vlan_TCI);
	fld->off    = sizeof(struct ethhdr) +
		      offsetof(struct vlan_hdr, h_vlan_TCI);
	fld->fv_idx = offsetof(struct pp_fv, second.l2.ext_vlan);
	fld->mask   = 0;
	/* ethertype */
	fld = &params->ext.fld[params->ext.n_fld++];
	fld->len    = FIELD_SIZEOF(struct vlan_hdr,
				   h_vlan_encapsulated_proto);
	fld->off    = sizeof(struct ethhdr) +
		      offsetof(struct vlan_hdr, h_vlan_encapsulated_proto);
	fld->fv_idx = offsetof(struct pp_fv, second.l2.h_prot);
	fld->mask   = 0;
	__proto_pr_debug(info);
	pr_debug("done\n");

	return 0;
}

/**
 * @brief Init Q in Q protocol profile info, this is actually the
 *        internal STAG protocol.<br>
 *        Assuming always second vlan header exist, the next
 *        protocol of the second vlan will be extracted
 */
static s32 __proto_q_in_q_init(struct protocol_info *info)
{
	struct prsr_up_layer_proto_params *params;
	struct field_ext_info *fld;
	struct skip_info      *skip;
	struct np_info        *np;
	s32 n;

	if (unlikely(ptr_is_null(info)))
		return -EINVAL;

	params = &info->up;
	info->id      = PRSR_PROTO_QINQ;
	info->prof_id = PRSR_PROFILE_COMMON;
	info->name    = params->name; /* a reference to the name */
	n = strscpy(params->name, "QinQ", sizeof(params->name));

	/* parsing info */
	params->proto_id   = ETH_P_8021AD;
	params->hw_idx     = 44;
	params->hdr_len    = 10;
	params->entry_type = ETYPE_NONE;
	/* next protocol info */
	np = &params->np;
	np->off   = (sizeof(struct vlan_hdr) +
		     offsetof(struct vlan_hdr, h_vlan_encapsulated_proto)) *
		    BITS_PER_BYTE;
	np->len   = FIELD_SIZEOF(struct vlan_hdr, h_vlan_encapsulated_proto) *
		    BITS_PER_BYTE;
	np->logic = NP_LOGIC_TUNN;
	np->dflt  = PRSR_PROTO_PAYLOAD;
	/* header skip info */
	skip = &params->skip;
	skip->hdr_len_im = sizeof(struct vlan_hdr) * 2;
	skip->logic      = SKIP_LOGIC_IM;
	/* preceding protocols */
	params->n_pre = 1;
	/* Tunnel -> VLAN */
	params->pre[0].np_logic = NP_LOGIC_TUNN;
	params->pre[0].val      = ETH_P_8021AD;
	/* extract info */
	/* mac addresses */
	fld = &params->ext.fld[params->ext.n_fld++];
	fld->len    = offsetof(struct ethhdr, h_proto);
	fld->off    = offsetof(struct ethhdr, h_dest);
	fld->fv_idx = offsetof(struct pp_fv, second.l2.h_dst);
	fld->mask   = 0;
	/* External VLAN ID */
	fld = &params->ext.fld[params->ext.n_fld++];
	fld->len    = FIELD_SIZEOF(struct vlan_hdr, h_vlan_TCI);
	fld->off    = (sizeof(struct ethhdr) +
		       offsetof(struct vlan_hdr, h_vlan_TCI));
	fld->fv_idx = offsetof(struct pp_fv, second.l2.ext_vlan);
	fld->mask   = 0;
	/* Internal VLAN ID + ethertype */
	fld = &params->ext.fld[params->ext.n_fld++];
	fld->len    = sizeof(struct vlan_hdr);
	fld->off    = sizeof(struct ethhdr) + sizeof(struct vlan_hdr) +
		      offsetof(struct vlan_hdr, h_vlan_TCI);
	fld->fv_idx = offsetof(struct pp_fv, second.l2.int_vlan);
	fld->mask   = 0;
	__proto_pr_debug(info);
	pr_debug("done\n");

	return 0;
}

/**
 * @brief Init pppoe protocol info
 */
static s32 __proto_pppoe_init(struct protocol_info *info)
{
	struct prsr_l2_proto_params *params;
	s32 n;

	if (unlikely(ptr_is_null(info)))
		return -EINVAL;

	params      = &info->l2;
	info->id       = PRSR_PROTO_PPPOE;
	info->prof_id  = PRSR_PROFILE_COMMON;
	info->name     = params->name; /* a reference to the name */
	n = strscpy(params->name, "PPPoE", sizeof(params->name));
	/* parsing info */
	params->hw.ethtype  = ETH_P_PPP_SES;
	params->hw.hdr_len  = sizeof(struct pppoe_hdr);
	params->hw.fallback = false;
	params->hw.cmp_op   = L2_CMP_OP_EQ;
	/* L2 parsing requires the protocols to be in a specific order */
	params->hw_idx      = L2_ENT_PPPOE;
	/* extracting the pppoe session id */
	params->ext.fld[0].off = offsetof(struct pppoe_hdr, sid) +
				 FIELD_SIZEOF(struct ethhdr, h_proto);
	params->ext.fld[0].len    = FIELD_SIZEOF(struct pppoe_hdr, sid);
	params->ext.fld[0].fv_idx = offsetof(struct pp_fv, first.l2.pppoe_id);
	params->ext.fld[0].mask   = 0;
	params->ext.n_fld  = 1;
	__proto_pr_debug(info);
	pr_debug("done\n");

	return 0;
}

/**
 * @brief Init internal pppoe protocol profile info
 */
static s32 __proto_pppoe_int_init(struct protocol_info *info)
{
	struct prsr_up_layer_proto_params *params;
	struct field_ext_info *fld;
	struct skip_info      *skip;
	struct np_info        *np;
	s32 n;

	if (unlikely(ptr_is_null(info)))
		return -EINVAL;

	params = &info->up;
	info->id      = PRSR_PROTO_PPPOE_INT;
	info->prof_id = PRSR_PROFILE_COMMON;
	info->name    = params->name; /* a reference to the name */
	n = strscpy(params->name, "PPPoE INT", sizeof(params->name));

	/* parsing info */
	params->proto_id   = ETH_P_PPP_SES;
	params->hw_idx     = 45;
	params->hdr_len    = 4;
	params->entry_type = ETYPE_NONE;
	/* next protocol info */
	np = &params->np;
	np->off   = sizeof(struct pppoe_hdr)          * BITS_PER_BYTE;
	np->len   = FIELD_SIZEOF(struct ethhdr, h_proto) * BITS_PER_BYTE;
	np->logic = NP_LOGIC_TUNN;
	np->dflt  = PRSR_PROTO_PAYLOAD;
	/* header skip info */
	skip = &params->skip;
	skip->hdr_len_im = PPPOE_SES_HLEN;
	skip->logic      = SKIP_LOGIC_IM;
	/* preceding protocols */
	params->n_pre = 1;
	/* Tunnel -> PPPoE */
	params->pre[0].np_logic = NP_LOGIC_TUNN;
	params->pre[0].val      = ETH_P_PPP_SES;
	/* extract info */
	/* PPPoE session id */
	fld = &params->ext.fld[params->ext.n_fld++];
	fld->len    = FIELD_SIZEOF(struct pppoe_hdr, sid);
	fld->off    = offsetof(struct pppoe_hdr, sid);
	fld->fv_idx = offsetof(struct pp_fv, second.l2.pppoe_id);
	fld->mask   = 0;
	/* Ethertype */
	fld = &params->ext.fld[params->ext.n_fld++];
	fld->len    = FIELD_SIZEOF(struct ethhdr, h_proto);
	fld->off    = offsetof(struct pppoe_hdr, tag);
	fld->fv_idx = offsetof(struct pp_fv, second.l2.h_prot);
	fld->mask   = 0;
	__proto_pr_debug(info);
	pr_debug("done\n");

	return 0;
}

/**
 * @brief LLC SNAP header definition
 */
struct llc_snap_hdr {
	u8  dsap;
	u8  ssap;
	u8  ctrl;
	u8  oui[3];
} __packed;

/**
 * @brief Init snap protocol info
 */
static s32 __proto_snap_init(struct protocol_info *info)
{
	struct prsr_l2_proto_params *params;
	s32 n;

	if (unlikely(ptr_is_null(info)))
		return -EINVAL;

	params     = &info->l2;
	info->id      = PRSR_PROTO_SNAP;
	info->prof_id = PRSR_PROFILE_COMMON;
	info->name    = params->name; /* a reference to the name */
	n = strscpy(params->name, "SNAP", sizeof(params->name));
	/* parsing info */
	params->hw.ethtype  = ETH_P_802_3_MIN;
	params->hw.hdr_len  = sizeof(struct llc_snap_hdr);
	params->hw.fallback = false;
	params->hw.cmp_op   = L2_CMP_OP_LT;
	/* L2 parsing requires the protocols to be in a specific order */
	params->hw_idx      = L2_ENT_SNAP;
	__proto_pr_debug(info);
	pr_debug("done\n");

	return 0;
}

/**
 * @brief Init ptp protocol info
 */
static s32 __proto_ptp_init(struct protocol_info *info)
{
	struct prsr_up_layer_proto_params *params;
	struct skip_info *skip;
	struct np_info   *np;
	s32 n;

	if (unlikely(ptr_is_null(info)))
		return -EINVAL;

	params      = &info->up;
	info->id       = PRSR_PROTO_PTP;
	info->prof_id  = PRSR_PROFILE_COMMON;
	info->name     = params->name; /* a reference to the name */
	n = strscpy(params->name, "PTP", sizeof(params->name));
	/* parsing info */
	params->proto_id   = ETH_P_1588;
	params->hw_idx     = TABLE_ENTRY_INVALID_IDX;
	params->hdr_len    = 0;
	params->entry_type = ETYPE_NONE;
	/* set protocol as last ethertype */
	params->n_leth     = 1;
	params->leth[0].ethtype = ETH_P_1588;
	n = strscpy(params->leth[0].name, params->name,
		    sizeof(params->leth[0].name));
	/* next protocol info */
	np = &params->np;
	np->logic = NP_LOGIC_END;
	np->dflt  = PRSR_PROTO_PAYLOAD;
	/* header skip info */
	skip = &params->skip;
	skip->hdr_len_im  = 0;
	skip->logic       = SKIP_LOGIC_IM;
	__proto_pr_debug(info);
	pr_debug("done\n");

	return 0;
}

/**
 * @brief Init ip select info, this is not a real protocol and only
 *        used for ports w/o L2 where the parser need to detect
 *        whether the first header is IPv4 or IPv6
 */
static s32 __proto_ip_select_init(struct protocol_info *info)
{
	struct prsr_up_layer_proto_params *params;
	struct skip_info *skip;
	struct np_info *np;
	s32 n;

	if (unlikely(ptr_is_null(info)))
		return -EINVAL;

	params      = &info->up;
	info->id       = PRSR_PROTO_IP_SEL;
	info->prof_id  = PRSR_PROFILE_COMMON;
	info->name     = params->name; /* a reference to the name */
	n = strscpy(params->name, "IP Select", sizeof(params->name));
	/* parsing info */
	params->proto_id   = U16_MAX;
	params->hw_idx     = TABLE_ENTRY_INVALID_IDX;
	params->hdr_len    = 1;
	params->ignore     = true;
	params->entry_type = ETYPE_NONE;
	/* next protocol info
	 * both IPv4 and IPv6 has bits [0-3] specifying the protocol version,
	 * where '4' specify IPv4 and '6' specify IPv6, so we configure the
	 * IP select to extract these 4 bits to determine in if the packet
	 * has IPv4 or IPv6 as the first header
	 */
	np = &params->np;
	np->off   = IP_SELECT_NEXT_HDR_OFF;
	np->len   = IP_SELECT_NEXT_HDR_LEN;
	np->logic = NP_LOGIC_IP_SELECT;
	np->dflt  = PRSR_PROTO_PAYLOAD;
	/* no skipping! */
	skip = &params->skip;
	skip->hdr_len_im  = 0;
	skip->logic       = SKIP_LOGIC_IM;
	__proto_pr_debug(info);
	pr_debug("done\n");

	return 0;
}

/**
 * @brief Init ipv4 common protocol info
 */
static void __proto_ipv4_common_init(struct protocol_info *info, char *name)
{
	struct prsr_up_layer_proto_params *params;
	struct field_ext_info *fld;
	struct skip_info      *skip;
	struct np_info        *np;
	u32 n;

	if (unlikely(!(!ptr_is_null(info) &&
		       !ptr_is_null(name))))
		return;

	params      = &info->up;
	info->prof_id  = PRSR_PROFILE_COMMON;
	info->name     = params->name; /* a reference to the name */
	n = strscpy(params->name, name, sizeof(params->name));

	/* parsing info */
	params->proto_id   = ETH_P_IP;
	params->hw_idx     = TABLE_ENTRY_INVALID_IDX;
	params->hdr_len    = 10;
	params->entry_type = ETYPE_IPV4;
	/* next protocol info */
	np = &params->np;
	np->off   = offsetof(struct iphdr,  protocol) * BITS_PER_BYTE;
	np->len   = FIELD_SIZEOF(struct iphdr, protocol) * BITS_PER_BYTE;
	np->dflt  = PRSR_PROTO_PAYLOAD;
	/* header skip info */
	skip = &params->skip;
	skip->hdr_len_off = 4; /* we cannot sizeof bit fields */
	skip->hdr_len_sz  = 4; /* we cannot sizeof bit fields */
	skip->hdr_len_im  = 0;
	skip->logic       = SKIP_LOGIC_ALU;
	/* don't care as we use IM for skip logic */
	skip->op          = ALU_OP_4XLEN_PLUS_IM;
	skip->hdr_len_min = sizeof(struct iphdr);
	skip->hdr_len_max = 0xFF;
	/* extract info */
	/* only ToS and the protocol fields are required for classification so
	 * we configure to extract all fields up to the protocol and will use
	 * mask '0b0111111101' to mask everything other than the
	 * ToS and protocol
	 */
	fld = &params->ext.fld[params->ext.n_fld++];
	fld->off  = offsetof(struct iphdr, tos);
	fld->len  = offsetof(struct iphdr, check) -
		    offsetof(struct iphdr, tos);
	fld->mask = 0xfe;

	/* ip addresses*/
	fld = &params->ext.fld[params->ext.n_fld++];
	fld->off = offsetof(struct iphdr, saddr);
	fld->len = sizeof(struct iphdr) - offsetof(struct iphdr, saddr);
	fld->mask = 0;
}

/**
 * @brief Init ipv4 protocol info
 */
static s32 __proto_ipv4_init(struct protocol_info *info)
{
	struct prsr_up_layer_proto_params *params;
	u32 n;

	if (unlikely(ptr_is_null(info)))
		return -EINVAL;

	/* init common info */
	__proto_ipv4_common_init(info, "IPv4");

	params   = &info->up;
	info->id = PRSR_PROTO_IPV4;
	/* set protocols as last ethertype */
	params->n_leth     = 2;
	/* IPv4 as last ethertype */
	params->leth[0].ethtype = ETH_P_IP;
	n = strscpy(params->leth[0].name, "IPv4",
		    sizeof(params->leth[0].name));
	/* IPv4 over PPPoE as last ethertype */
	params->leth[1].ethtype = PPP_IP;
	n = strscpy(params->leth[1].name, "PPPoE IPv4",
		    sizeof(params->leth[1].name));
	/* next protocol info */
	params->np.logic = NP_LOGIC_IPV4;
	/* preceding protocols */
	params->n_pre = 1;
	/* IP Select -> IPv4 */
	params->pre[0].np_logic = NP_LOGIC_IP_SELECT;
	params->pre[0].val      = 4; /* no definition for IPv4 version value */
	/* extract info */
	/* set fv index value to the first IPv4 */
	params->ext.fld[0].fv_idx = offsetof(struct pp_fv, first.l3.v4);
	params->ext.fld[1].fv_idx = offsetof(struct pp_fv, first.l3.v4.saddr);

	__proto_pr_debug(info);
	pr_debug("done\n");

	return 0;
}

/**
 * @brief Init ipv4 internal protocol info
 */
static s32 __proto_ipv4_int_init(struct protocol_info *info)
{
	struct prsr_up_layer_proto_params *params;
	struct np_info *np;

	if (unlikely(ptr_is_null(info)))
		return -EINVAL;

	/* init common info */
	__proto_ipv4_common_init(info, "IPv4 INT");

	params      = &info->up;
	info->id    = PRSR_PROTO_IPV4_INT;
	/* next protocol info */
	np = &params->np;
	np->logic = NP_LOGIC_IPV4_2;
	/* preceding protocols */
	params->n_pre = 4;
	/* (DSlite) IPv4 -> IPv4 */
	params->pre[0].np_logic = NP_LOGIC_IPV4;
	params->pre[0].val      = IPPROTO_IPIP;
	/* (DSlite) IPv6 -> IPv4 */
	params->pre[1].np_logic = NP_LOGIC_IPV6;
	params->pre[1].val      = IPPROTO_IPIP;
	/* Tunnel -> IPv4 */
	params->pre[2].np_logic = NP_LOGIC_TUNN;
	params->pre[2].val      = ETH_P_IP;
	/* PPPoE internal -> IPv4 */
	params->pre[3].np_logic = NP_LOGIC_TUNN;
	params->pre[3].val      = PPP_IP;
	/* extract info */
	/* set fv index value to the first IPv4 */
	params->ext.fld[0].fv_idx = offsetof(struct pp_fv, second.l3.v4);
	params->ext.fld[1].fv_idx = offsetof(struct pp_fv, second.l3.v4.saddr);
	__proto_pr_debug(info);
	pr_debug("done\n");

	return 0;
}

/**
 * @brief Init ipv6 common protocol info
 */
static void __proto_ipv6_common_init(struct protocol_info *info, char *name)
{
	struct prsr_up_layer_proto_params *params;
	struct field_ext_info *fld;
	struct skip_info      *skip;
	struct np_info        *np;
	u32 n;

	if (unlikely(!(!ptr_is_null(info) && !ptr_is_null(name))))
		return;

	params      = &info->up;
	info->prof_id  = PRSR_PROFILE_COMMON;
	info->name     = params->name; /* a reference to the name */
	n = strscpy(params->name, name, sizeof(params->name));

	/* parsing info */
	params->proto_id   = ETH_P_IPV6;
	params->hw_idx     = TABLE_ENTRY_INVALID_IDX;
	params->hdr_len    = 7;
	params->entry_type = ETYPE_IPV6;
	/* next protocol info */
	np = &params->np;
	np->off   = offsetof(struct ipv6hdr,  nexthdr) * BITS_PER_BYTE;
	np->len   = FIELD_SIZEOF(struct ipv6hdr, nexthdr) * BITS_PER_BYTE;
	np->dflt  = PRSR_PROTO_PAYLOAD;
	/* header skip info - IPv6 has constant header size */
	skip = &params->skip;
	skip->hdr_len_im  = sizeof(struct ipv6hdr);
	skip->logic       = SKIP_LOGIC_IM;
	/* extract info */
	/* Version/TC and flow label fields */
	fld = &params->ext.fld[params->ext.n_fld++];
	fld->off       = 0; /* we cannot sizeof bit fields */
	fld->len       = offsetof(struct ipv6hdr, payload_len);
	fld->mask      = 0;
	/* ip addresses*/
	fld = &params->ext.fld[params->ext.n_fld++];
	fld->off       = offsetof(struct ipv6hdr, saddr);
	fld->len       = sizeof(struct ipv6hdr) -
			 offsetof(struct ipv6hdr, saddr);
	fld->mask      = 0;
}

/**
 * @brief Init ipv6 protocol info
 */
static s32 __proto_ipv6_init(struct protocol_info *info)
{
	struct prsr_up_layer_proto_params *params;
	u32 n;

	if (unlikely(ptr_is_null(info)))
		return -EINVAL;

	/* init common info */
	__proto_ipv6_common_init(info, "IPv6");

	params      = &info->up;
	info->id       = PRSR_PROTO_IPV6;
	/* set protocol as last ethertype */
	params->n_leth     = 2;
	/* IPv6 as last ethertype */
	params->leth[0].ethtype = params->proto_id;
	n = strscpy(params->leth[0].name, params->name,
		    sizeof(params->leth[0].name));
	/* IPv6 over PPPoE as last ethertype */
	params->leth[1].ethtype = PPP_IPV6;
	n = strscpy(params->leth[1].name, "PPPoE IPv6",
		    sizeof(params->leth[1].name));
	/* next protocol info */
	params->np.logic = NP_LOGIC_IPV6;
	/* preceding protocols */
	params->n_pre = 1;
	/* IP Select -> IPv6 */
	params->pre[0].np_logic = NP_LOGIC_IP_SELECT;
	params->pre[0].val      = 6; /* no definition for IPv6 version value */
	/* extracting info */
	params->ext.fld[0].fv_idx = offsetof(struct pp_fv, first.l3.v6);
	params->ext.fld[1].fv_idx = offsetof(struct pp_fv, first.l3.v6.saddr);

	__proto_pr_debug(info);
	pr_debug("done\n");

	return 0;
}

/**
 * @brief Init ipv6 internal protocol info
 */
static s32 __proto_ipv6_int_init(struct protocol_info *info)
{
	struct prsr_up_layer_proto_params *params;

	if (unlikely(ptr_is_null(info)))
		return -EINVAL;

	/* init common info */
	__proto_ipv6_common_init(info, "IPv6 INT");

	params      = &info->up;
	info->id       = PRSR_PROTO_IPV6_INT;
	/* next protocol info */
	params->np.logic = NP_LOGIC_IPV6_2;
	/* preceding protocols */
	params->n_pre = 4;
	/* (DSlite) IPv4 -> IPv6 */
	params->pre[0].np_logic = NP_LOGIC_IPV4;
	params->pre[0].val      = IPPROTO_IPV6;
	/* (DSlite) IPv6 -> IPv6 */
	params->pre[1].np_logic = NP_LOGIC_IPV6;
	params->pre[1].val      = IPPROTO_IPV6;
	/* Tunnel -> IPv6 */
	params->pre[2].np_logic = NP_LOGIC_TUNN;
	params->pre[2].val      = ETH_P_IPV6;
	/* PPPoE internal -> IPv6 */
	params->pre[3].np_logic = NP_LOGIC_TUNN;
	params->pre[3].val      = PPP_IPV6;
	/* extract info */
	params->ext.fld[0].fv_idx = offsetof(struct pp_fv, second.l3.v6);
	params->ext.fld[1].fv_idx = offsetof(struct pp_fv, second.l3.v6.saddr);
	__proto_pr_debug(info);
	pr_debug("done\n");

	return 0;
}

/**
 * @brief Init common ipv6 extension headers info
 */
static void __proto_ipv6_exten_common_init(struct protocol_info *info,
					   char *name)
{
	struct prsr_up_layer_proto_params *params;
	struct skip_info *skip;
	struct np_info   *np;
	u32 n;

	if (unlikely(!(!ptr_is_null(info) && !ptr_is_null(name))))
		return;

	params     = &info->up;
	info->prof_id = PRSR_PROFILE_COMMON;
	info->name    = params->name; /* a reference to the name */
	n = strscpy(params->name, name, sizeof(params->name));
	/* parsing info */
	params->proto_id   = U16_MAX;
	params->hw_idx     = TABLE_ENTRY_INVALID_IDX;
	params->hdr_len    = 2;
	params->entry_type = ETYPE_IPV6_EXT;
	params->ignore     = true;
	/* next protocol info */
	np = &params->np;
	np->off   = offsetof(struct ipv6_opt_hdr, nexthdr)  * BITS_PER_BYTE;
	np->len   = FIELD_SIZEOF(struct ipv6_opt_hdr, nexthdr) * BITS_PER_BYTE;
	np->dflt  = PRSR_PROTO_PAYLOAD;
	/* header skip info */
	skip = &params->skip;
	skip->hdr_len_off = offsetof(struct ipv6_opt_hdr, hdrlen) *
			    BITS_PER_BYTE;
	skip->hdr_len_sz  = FIELD_SIZEOF(struct ipv6_opt_hdr, hdrlen) *
			    BITS_PER_BYTE;
	skip->hdr_len_im  = 8;
	skip->logic       = SKIP_LOGIC_ALU;
	skip->op          = ALU_OP_8XLEN_PLUS_IM;
}

/**
 * @brief Init ipv6 hop, route and opts protocol info
 */
static u32 __proto_ipv6_exten_init(struct protocol_info *info)
{
	struct prsr_up_layer_proto_params *params;

	if (unlikely(ptr_is_null(info)))
		return -EINVAL;

	__proto_ipv6_exten_common_init(info, "IPv6 Extension");

	params = &info->up;
	info->id  = PRSR_PROTO_IPV6EXT;
	/* next protocol info */
	params->np.logic = NP_LOGIC_IPV6;
	/* preceding protocols */
	params->n_pre = 3;
	/* IPv6 -> HOP OPTS */
	params->pre[0].np_logic = NP_LOGIC_IPV6;
	params->pre[0].val      = IPPROTO_HOPOPTS;
	/* IPv6 -> Routing */
	params->pre[1].np_logic = NP_LOGIC_IPV6;
	params->pre[1].val      = IPPROTO_ROUTING;
	/* IPv6 -> Dest OPTS */
	params->pre[2].np_logic = NP_LOGIC_IPV6;
	params->pre[2].val      = IPPROTO_DSTOPTS;
	__proto_pr_debug(info);
	pr_debug("done\n");
	return 0;
}

/**
 * @brief Init internal ipv6 hop, route and opts protocol info
 */
static u32 __proto_ipv6_exten_int_init(struct protocol_info *info)
{
	struct prsr_up_layer_proto_params *params;

	if (unlikely(ptr_is_null(info)))
		return -EINVAL;

	__proto_ipv6_exten_common_init(info, "IPv6 Extension INT");

	params = &info->up;
	info->id  = PRSR_PROTO_IPV6EXT_INT;
	/* next protocol info */
	params->np.logic = NP_LOGIC_IPV6_2;
	/* preceding protocols */
	params->n_pre = 3;
	/* IPv6 Internal -> HOP OPTS */
	params->pre[0].np_logic = NP_LOGIC_IPV6_2;
	params->pre[0].val      = IPPROTO_HOPOPTS;
	/* IPv6 Internal -> Routing */
	params->pre[1].np_logic = NP_LOGIC_IPV6_2;
	params->pre[1].val      = IPPROTO_ROUTING;
	/* IPv6 Internal -> Dest OPTS */
	params->pre[2].np_logic = NP_LOGIC_IPV6_2;
	params->pre[2].val      = IPPROTO_DSTOPTS;
	__proto_pr_debug(info);
	pr_debug("done\n");
	return 0;
}

/**
 * @brief Init ipv6 common frag extension protocol info
 */
static void __proto_ipv6_frag_common_init(struct protocol_info *info,
					  char *name)
{
	struct prsr_up_layer_proto_params *params;
	struct skip_info *skip;
	struct np_info   *np;
	u32 n;

	if (unlikely(!(!ptr_is_null(info) && !ptr_is_null(name))))
		return;

	params     = &info->up;
	info->prof_id = PRSR_PROFILE_COMMON;
	info->name    = params->name; /* a reference to the name */
	n = strscpy(params->name, name, sizeof(params->name));
	/* parsing info */
	params->proto_id   = U16_MAX;
	params->hw_idx     = TABLE_ENTRY_INVALID_IDX;
	params->hdr_len    = 4;
	params->entry_type = ETYPE_IPV6_FRAG;
	params->ignore     = true;
	/* next protocol info */
	np = &params->np;
	np->off   = offsetof(struct frag_hdr, nexthdr)  * BITS_PER_BYTE;
	np->len   = FIELD_SIZEOF(struct frag_hdr, nexthdr) * BITS_PER_BYTE;
	np->dflt  = PRSR_PROTO_PAYLOAD;
	/* header skip info - IPv6 has constant header size */
	skip = &params->skip;
	skip->hdr_len_im  = sizeof(struct frag_hdr);
	skip->logic       = SKIP_LOGIC_IM;
}

/**
 * @brief Init ipv6 frag extension protocol info
 */
static u32 __proto_ipv6_frag_init(struct protocol_info *info)
{
	struct prsr_up_layer_proto_params *params;

	if (unlikely(ptr_is_null(info)))
		return -EINVAL;

	__proto_ipv6_frag_common_init(info, "IPv6 Frag");
	params     = &info->up;
	info->id      = PRSR_PROTO_IPV6FRAG;
	/* next protocol info */
	params->np.logic = NP_LOGIC_IPV6;
	/* preceding protocols */
	params->n_pre = 1;
	/* IPv6 -> Fragment */
	params->pre[0].np_logic = NP_LOGIC_IPV6;
	params->pre[0].val      = IPPROTO_FRAGMENT;
	__proto_pr_debug(info);
	pr_debug("done\n");
	return 0;
}

/**
 * @brief Init internal ipv6 frag extension protocol info
 */
static u32 __proto_ipv6_frag_int_init(struct protocol_info *info)
{
	struct prsr_up_layer_proto_params *params;

	if (unlikely(ptr_is_null(info)))
		return -EINVAL;

	__proto_ipv6_frag_common_init(info, "IPv6 Frag INT");
	params     = &info->up;
	info->id      = PRSR_PROTO_IPV6FRAG_INT;
	/* next protocol info */
	params->np.logic = NP_LOGIC_IPV6_2;
	/* preceding protocols */
	params->n_pre = 1;
	/* IPv6 Internal -> Fragment */
	params->pre[0].np_logic = NP_LOGIC_IPV6_2;
	params->pre[0].val      = IPPROTO_FRAGMENT;
	__proto_pr_debug(info);
	pr_debug("done\n");
	return 0;
}

/**
 * @brief Init icmp protocol info
 */
static s32 __proto_icmpv4_init(struct protocol_info *info)
{
	struct prsr_up_layer_proto_params *params;
	struct field_ext_info *fld;
	struct skip_info      *skip;
	struct np_info        *np;
	s32 n;

	if (unlikely(ptr_is_null(info)))
		return -EINVAL;

	params     = &info->up;
	info->id      = PRSR_PROTO_ICMP;
	info->prof_id = PRSR_PROFILE_COMMON;
	info->name    = params->name; /* a reference to the name */
	n = strscpy(params->name, "ICMP", sizeof(params->name));
	/* parsing info */
	params->proto_id   = IPPROTO_ICMP;
	params->hw_idx     = TABLE_ENTRY_INVALID_IDX;
	params->entry_type = ETYPE_OTHER_L4;
	/* next protocol info */
	np = &params->np;
	np->logic = NP_LOGIC_END;
	np->dflt  = PRSR_PROTO_PAYLOAD;
	/* header skip info */
	skip = &params->skip;
	skip->hdr_len_im = sizeof(struct icmphdr);
	skip->logic      = SKIP_LOGIC_IM;
	/* preceding protocols */
	params->n_pre = 2;
	/* IPv4 -> ICMP */
	params->pre[0].np_logic = NP_LOGIC_IPV4;
	params->pre[0].val      = IPPROTO_ICMP;
	/* IPv4 internal -> ICMP */
	params->pre[1].np_logic = NP_LOGIC_IPV4_2;
	params->pre[1].val      = IPPROTO_ICMP;
	/* extract info */
	/* extracting type and code fields */
	fld = &params->ext.fld[params->ext.n_fld++];
	fld->off    = offsetof(struct icmphdr, type);
	fld->len    = offsetof(struct icmphdr, checksum);
	fld->fv_idx = offsetof(struct pp_fv, first.l4.icmp);
	fld->mask   = 0;
	__proto_pr_debug(info);
	pr_debug("done\n");

	return 0;
}

/**
 * @brief Init icmpv6 protocol info
 */
static s32 __proto_icmpv6_init(struct protocol_info *info)
{
	struct prsr_up_layer_proto_params *params;
	struct field_ext_info *fld;
	struct skip_info      *skip;
	struct np_info        *np;
	s32 n;

	if (unlikely(ptr_is_null(info)))
		return -EINVAL;

	params     = &info->up;
	info->id      = PRSR_PROTO_ICMPV6;
	info->prof_id = PRSR_PROFILE_COMMON;
	info->name    = params->name; /* a reference to the name */
	n = strscpy(params->name, "ICMPv6", sizeof(params->name));
	/* parsing info */
	params->proto_id   = IPPROTO_ICMPV6;
	params->hw_idx     = TABLE_ENTRY_INVALID_IDX;
	params->entry_type = ETYPE_OTHER_L4;
	/* next protocol info */
	np = &params->np;
	np->logic = NP_LOGIC_END;
	np->dflt  = PRSR_PROTO_PAYLOAD;
	/* header skip info */
	skip = &params->skip;
	skip->hdr_len_im  = sizeof(struct icmp6hdr);
	skip->logic       = SKIP_LOGIC_IM;
	/* preceding protocols */
	params->n_pre = 1;
	/* IPv6 -> ICMPv6 */
	params->pre[0].np_logic = NP_LOGIC_IPV6;
	params->pre[0].val      = IPPROTO_ICMPV6;
	/* IPv6 Internal -> ICMPv6 */
	params->pre[0].np_logic = NP_LOGIC_IPV6_2;
	params->pre[0].val      = IPPROTO_ICMPV6;
	/* extract info */
	/* extracting type and code fields */
	fld = &params->ext.fld[params->ext.n_fld++];
	fld->off    = offsetof(struct icmp6hdr, icmp6_type);
	fld->len    = offsetof(struct icmp6hdr, icmp6_cksum);
	fld->fv_idx = offsetof(struct pp_fv,    first.l4.icmp);
	fld->mask   = 0;
	__proto_pr_debug(info);
	pr_debug("done\n");

	return 0;
}

/**
 * @brief Init esp common protocol info
 */
static void __proto_esp_common_init(struct protocol_info *info, char *name)
{
	struct prsr_up_layer_proto_params *params;
	struct field_ext_info *fld;
	struct skip_info      *skip;
	struct np_info        *np;
	u32 n;

	if (unlikely(!(!ptr_is_null(info) && !ptr_is_null(name))))
		return;

	params        = &info->up;
	info->prof_id = PRSR_PROFILE_COMMON;
	info->name    = params->name; /* a reference to the name */
	n             = strscpy(params->name, name, sizeof(params->name));

	/* parsing info */
	params->proto_id   = IPPROTO_ESP;
	params->hw_idx     = TABLE_ENTRY_INVALID_IDX;
	params->hdr_len    = 4;
	params->fallback   = false;
	params->ignore     = false;
	params->entry_type = ETYPE_OTHER_L4;
	/* next protocol info */
	np        = &params->np;
	np->logic = NP_LOGIC_END;
	np->dflt  = PRSR_PROTO_PAYLOAD;
	/* header skip info */
	skip             = &params->skip;
	skip->hdr_len_im = sizeof(struct ip_esp_hdr);
	skip->logic      = SKIP_LOGIC_IM;
	/* extract info - spi field */
	fld       = &params->ext.fld[params->ext.n_fld++];
	fld->off  = offsetof(struct ip_esp_hdr, spi);
	fld->len  = FIELD_SIZEOF(struct ip_esp_hdr, spi);
	fld->mask = 0;
}

/**
 * @brief Init esp protocol info
 */
static s32 __proto_esp_init(struct protocol_info *info)
{
	struct prsr_up_layer_proto_params *params;

	if (unlikely(ptr_is_null(info)))
		return -EINVAL;

	/* init common info */
	__proto_esp_common_init(info, "ESP");

	params   = &info->up;
	info->id = PRSR_PROTO_ESP;
	/* preceding protocols */
	params->n_pre = 2;
	/* IPv4 -> ESP */
	params->pre[0].np_logic = NP_LOGIC_IPV4;
	params->pre[0].val      = IPPROTO_ESP;
	/* IPv6 -> ESP */
	params->pre[1].np_logic = NP_LOGIC_IPV6;
	params->pre[1].val      = IPPROTO_ESP;
	/* extract info */
	/* set the fv index to the first */
	params->ext.fld[0].fv_idx = offsetof(struct pp_fv, first.l4.esp);

	__proto_pr_debug(info);
	pr_debug("done\n");

	return 0;
}

/**
 * @brief Init esp internal protocol info
 */
static s32 __proto_esp_int_init(struct protocol_info *info)
{
	if (unlikely(ptr_is_null(info)))
		return -EINVAL;

	/* init common info */
	__proto_esp_common_init(info, "ESP INT");

	info->id = PRSR_PROTO_ESP_INT;
	/* preceding protocols */
	info->up.n_pre = 1;
	/* UDP -> ESP select */
	info->up.pre[0].np_logic = NP_LOGIC_UDP;
	info->up.pre[0].val      = ESP_UDP_DPORT;
	/* extract info */
	/* set the fv index to the second */
	info->up.ext.fld[0].fv_idx = offsetof(struct pp_fv, second.l4.esp);

	__proto_pr_debug(info);
	pr_debug("done\n");

	return 0;
}

/**
 * @brief Init tcp common protocol info
 */
static void __proto_tcp_common_init(struct protocol_info *info, char *name)
{
	struct prsr_up_layer_proto_params *params;
	struct field_ext_info *fld;
	struct skip_info      *skip;
	struct np_info        *np;
	u32 n;

	if (unlikely(!(!ptr_is_null(info) && !ptr_is_null(name))))
		return;

	params      = &info->up;
	info->prof_id  = PRSR_PROFILE_COMMON;
	info->name     = params->name; /* a reference to the name */
	n = strscpy(params->name, name, sizeof(params->name));

	/* parsing info */
	params->proto_id = IPPROTO_TCP;
	params->hw_idx   = TABLE_ENTRY_INVALID_IDX;
	params->hdr_len  = 14;
	params->fallback = false;
	params->ignore   = false;
	params->entry_type = ETYPE_TCP;
	/* next protocol info */
	np = &params->np;
	np->logic = NP_LOGIC_END;
	np->dflt  = PRSR_PROTO_PAYLOAD;
	/* header skip info */
	skip = &params->skip;
	/* cannot use offetof(struct tcphdr, doff) cause its bitfield */
	skip->hdr_len_off = (offsetof(struct tcphdr, ack_seq) +
			     FIELD_SIZEOF(struct tcphdr, ack_seq)) *
			    BITS_PER_BYTE;
	/* cannot use FIELD_SIZEOF(struct tcphdr, doff) cause its bitfield */
	skip->hdr_len_sz = 4;
	skip->logic      = SKIP_LOGIC_ALU;
	skip->op         = ALU_OP_4XLEN_PLUS_IM;
	skip->hdr_len_im = 0;
	skip->hdr_len_min = sizeof(struct tcphdr);
	skip->hdr_len_max = 0xFF;
	/* extract info */
	/* extracting source and destination ports */
	fld = &params->ext.fld[params->ext.n_fld++];
	fld->off  = offsetof(struct tcphdr, source);
	fld->len  = offsetof(struct tcphdr, seq) -
		    offsetof(struct tcphdr, source);
	fld->mask = 0;
}

/**
 * @brief Init tcp protocol info
 */
static s32 __proto_tcp_init(struct protocol_info *info)
{
	struct prsr_up_layer_proto_params *params;

	if (unlikely(ptr_is_null(info)))
		return -EINVAL;

	/* init common info */
	__proto_tcp_common_init(info, "TCP");

	params   = &info->up;
	info->id = PRSR_PROTO_TCP;
	/* preceding protocols */
	params->n_pre = 2;
	/* IPv4 -> TCP */
	params->pre[0].np_logic = NP_LOGIC_IPV4;
	params->pre[0].val      = IPPROTO_TCP;
	/* IPv6 -> TCP */
	params->pre[1].np_logic = NP_LOGIC_IPV6;
	params->pre[1].val      = IPPROTO_TCP;
	/* extract info */
	/* set the fv index to the second */
	params->ext.fld[0].fv_idx = offsetof(struct pp_fv, first.l4.tcp);

	__proto_pr_debug(info);
	pr_debug("done\n");

	return 0;
}

/**
 * @brief Init tcp internal protocol info
 */
static s32 __proto_tcp_int_init(struct protocol_info *info)
{
	struct prsr_up_layer_proto_params *params;

	if (unlikely(ptr_is_null(info)))
		return -EINVAL;

	/* init common info */
	__proto_tcp_common_init(info, "TCP INT");

	params      = &info->up;
	info->id       = PRSR_PROTO_TCP_INT;
	/* preceding protocols */
	params->n_pre = 2;
	/* IPv4 -> TCP */
	params->pre[0].np_logic = NP_LOGIC_IPV4_2;
	params->pre[0].val      = IPPROTO_TCP;
	/* IPv6 -> TCP */
	params->pre[1].np_logic = NP_LOGIC_IPV6_2;
	params->pre[1].val      = IPPROTO_TCP;
	/* extract info */
	/* set the fv index to the second */
	params->ext.fld[0].fv_idx = offsetof(struct pp_fv, second.l4.tcp);

	__proto_pr_debug(info);
	pr_debug("done\n");

	return 0;
}

/**
 * @brief Init tcp common protocol info
 */
static void __proto_udp_common_init(struct protocol_info *info, char *name)
{
	struct prsr_up_layer_proto_params *params;
	struct field_ext_info *fld;
	struct skip_info      *skip;
	struct np_info        *np;
	u32 n;

	if (unlikely(!(!ptr_is_null(info) && !ptr_is_null(name))))
		return;

	params      = &info->up;
	info->prof_id  = PRSR_PROFILE_COMMON;
	info->name     = params->name; /* a reference to the name */
	n = strscpy(params->name, name, sizeof(params->name));

	/* parsing info */
	params->proto_id   = IPPROTO_UDP;
	params->hw_idx     = TABLE_ENTRY_INVALID_IDX;
	params->hdr_len    = 4;
	params->entry_type = ETYPE_UDP;
	/* next protocol info */
	np = &params->np;
	np->dflt  = PRSR_PROTO_PAYLOAD;
	/* header skip info */
	skip = &params->skip;
	skip->hdr_len_off = offsetof(struct udphdr, dest) * BITS_PER_BYTE;
	skip->hdr_len_sz  = FIELD_SIZEOF(struct udphdr, dest) * BITS_PER_BYTE;
	skip->hdr_len_im  = sizeof(struct udphdr);
	skip->logic       = SKIP_LOGIC_LOOKUP;
	skip->op          = ALU_OP_IM;
	/* extract info */
	/* extracting source and destination ports */
	fld = &params->ext.fld[params->ext.n_fld++];
	fld->off = offsetof(struct udphdr, source);
	fld->len = offsetof(struct udphdr, len) -
		   offsetof(struct udphdr, source);
	fld->mask = 0;
}

/**
 * @brief Init udp protocol info
 */
static s32 __proto_udp_init(struct protocol_info *info)
{
	struct prsr_up_layer_proto_params *params;
	struct np_info *np;

	if (unlikely(ptr_is_null(info)))
		return -EINVAL;

	/* init common info */
	__proto_udp_common_init(info, "UDP");

	params      = &info->up;
	info->id       = PRSR_PROTO_UDP;
	/* next protocol info */
	np = &params->np;
	np->off   = offsetof(struct udphdr,  dest) * BITS_PER_BYTE;
	np->len   = FIELD_SIZEOF(struct udphdr, dest) * BITS_PER_BYTE;
	np->logic = NP_LOGIC_UDP;
	/* preceding protocols */
	params->n_pre = 2;
	/* IPv4 -> UDP */
	params->pre[0].np_logic = NP_LOGIC_IPV4;
	params->pre[0].val      = IPPROTO_UDP;
	/* IPv6 -> UDP */
	params->pre[1].np_logic = NP_LOGIC_IPV6;
	params->pre[1].val      = IPPROTO_UDP;
	/* extract info */
	/* set the fv index to the second */
	params->ext.fld[0].fv_idx = offsetof(struct pp_fv, first.l4.udp);

	__proto_pr_debug(info);
	pr_debug("done\n");

	return 0;
}

/**
 * @brief Init udp internal protocol info
 */
static s32 __proto_udp_int_init(struct protocol_info *info)
{
	struct prsr_up_layer_proto_params *params;
	struct np_info *np;

	if (unlikely(ptr_is_null(info)))
		return -EINVAL;

	/* init common info */
	__proto_udp_common_init(info, "UDP INT");

	params      = &info->up;
	info->id       = PRSR_PROTO_UDP_INT;
	/* next protocol info */
	np = &params->np;
	np->off   = offsetof(struct udphdr,  dest) * BITS_PER_BYTE;
	np->len   = FIELD_SIZEOF(struct udphdr, dest) * BITS_PER_BYTE;
	np->logic = NP_LOGIC_END;
	/* preceding protocols */
	params->n_pre = 2;
	/* IPv4 -> UDP */
	params->pre[0].np_logic = NP_LOGIC_IPV4_2;
	params->pre[0].val      = IPPROTO_UDP;
	/* IPv6 -> UDP */
	params->pre[1].np_logic = NP_LOGIC_IPV6_2;
	params->pre[1].val      = IPPROTO_UDP;
	/* extract info */
	/* set the fv index to the second */
	params->ext.fld[0].fv_idx = offsetof(struct pp_fv, second.l4.udp);

	__proto_pr_debug(info);
	pr_debug("done\n");

	return 0;
}

/**
 * @brief Init gre protocol info
 */
static s32 __proto_gre_init(struct protocol_info *info)
{
	struct prsr_up_layer_proto_params *params;
	struct skip_info *skip;
	struct np_info   *np;
	s32 n;

	if (unlikely(ptr_is_null(info)))
		return -EINVAL;

	params      = &info->up;
	info->id       = PRSR_PROTO_GRE;
	info->prof_id  = PRSR_PROFILE_COMMON;
	info->name     = params->name; /* a reference to the name */
	n = strscpy(params->name, "GRE", sizeof(params->name));
	/* parsing info */
	params->proto_id   = IPPROTO_GRE;
	params->hw_idx     = TABLE_ENTRY_INVALID_IDX;
	params->hdr_len    = 4;
	params->entry_type = ETYPE_OTHER_L4;
	/* next protocol info */
	np = &params->np;
	np->off   = offsetof(struct gre_base_hdr, protocol)  * BITS_PER_BYTE;
	np->len   = FIELD_SIZEOF(struct gre_base_hdr, protocol) * BITS_PER_BYTE;
	np->logic = NP_LOGIC_TUNN;
	np->dflt  = PRSR_PROTO_PAYLOAD;
	/* header skip info */
	skip = &params->skip;
	/* header calculation depends on the flags field */
	skip->hdr_len_off = offsetof(struct gre_base_hdr, flags) *
			    BITS_PER_BYTE;
	skip->hdr_len_sz  = 8;
	skip->logic       = SKIP_LOGIC_ALU;
	skip->op          = ALU_OP_4XC_4XK_4XS_PLUS_4;
	/* preceding protocols */
	params->n_pre = 2;
	/* IPv4 -> GRE */
	params->pre[0].np_logic = NP_LOGIC_IPV4;
	params->pre[0].val      = IPPROTO_GRE;
	/* IPv6 -> GRE */
	params->pre[1].np_logic = NP_LOGIC_IPV6;
	params->pre[1].val      = IPPROTO_GRE;
	__proto_pr_debug(info);
	pr_debug("done\n");

	return 0;
}

/**
 * @brief Init l2tp (v3) over ip protocol info
 */
static s32 __proto_l2tpv3_ip_init(struct protocol_info *info)
{
	struct prsr_up_layer_proto_params *params;
	struct field_ext_info *fld;
	struct skip_info *skip;
	struct np_info *np;
	s32 n;

	if (unlikely(ptr_is_null(info)))
		return -EINVAL;

	params = &info->up;
	info->id = PRSR_PROTO_L2TPV3_IP;
	info->prof_id = PRSR_PROFILE_COMMON;
	info->name = params->name;
	n = strscpy(params->name, "L2TP IP", sizeof(params->name));
	/* parsing info */
	params->proto_id = IPPROTO_L2TP;
	params->hw_idx = TABLE_ENTRY_INVALID_IDX;
	params->hdr_len = 4;
	params->entry_type = ETYPE_OTHER_L4;
	/* next protocol info - next protocol is always PPP */
	np = &params->np;
	np->logic = NP_LOGIC_END;
	np->dflt = PRSR_PROTO_PPP;
	/* header skip info */
	skip = &params->skip;
	skip->hdr_len_im = L2TPV3IP_HDR_LEN;
	skip->logic = SKIP_LOGIC_IM;
	/* preceding protocols */
	params->n_pre = 2;
	/* IPv4 -> L2TPv3 over IP */
	params->pre[0].np_logic = NP_LOGIC_IPV4;
	params->pre[0].val = IPPROTO_L2TP;
	/* IPv6 -> L2TPv3 over IP */
	params->pre[1].np_logic = NP_LOGIC_IPV6;
	params->pre[1].val = IPPROTO_L2TP;
	/* extract session id */
	fld = &params->ext.fld[params->ext.n_fld++];
	fld->fv_idx = offsetof(struct pp_fv, first.l4.l2tpoip);
	fld->len = L2TPV3IP_SESS_ID_SZ;
	fld->off = 0;
	fld->mask = 0;
	__proto_pr_debug(info);
	pr_debug("done\n");

	return 0;
}

/**
 * @brief Init l2tpv3 UDP protocol entry point.
 * Assumes there is no cookie present.
 */
static s32 __proto_l2tpv3_udp_init(struct protocol_info *info)
{
	struct prsr_up_layer_proto_params *params;
	struct field_ext_info *fld;
	struct np_info *np;
	s32 n;

	if (unlikely(ptr_is_null(info)))
		return -EINVAL;

	params = &info->up;
	info->id = PRSR_PROTO_L2TPV3_UDP;
	info->prof_id = PRSR_PROFILE_COMMON;
	info->name = params->name;
	n = strscpy(params->name, "L2TPv3 UDP", sizeof(params->name));
	/* parsing info */
	params->proto_id = U16_MAX;
	params->hw_idx = TABLE_ENTRY_INVALID_IDX;
	params->hdr_len = 8;
	params->entry_type = ETYPE_NONE;
	/* next protocol info - next protocol is always PPP */
	np = &params->np;
	np->logic = NP_LOGIC_END;
	np->dflt = PRSR_PROTO_PPP;
	/* header skip info */
	params->skip.logic = SKIP_LOGIC_IM;
	params->skip.hdr_len_im = L2TPV3_BASE_HDR_SIZE;
	/* preceding protocols */
	params->n_pre = 1;
	/* UDP -> L2TP select */
	params->pre[0].np_logic = NP_LOGIC_UDP;
	params->pre[0].val = L2TPV3_VER;
	/* extract the 32-bit session id */
	fld = &params->ext.fld[params->ext.n_fld++];
	fld->fv_idx = offsetof(struct pp_fv, tunn.l2tpv3oudp);
	fld->off = L2TPV3_TUNN_SESS_OFF;
	fld->len = L2TPV3_TUNN_SESS_LEN;
	fld->mask = 0;
	__proto_pr_debug(info);
	pr_debug("done\n");

	return 0;
}

/**
 * @brief Init l2tp select info
 * The purpose is just to distinguish between L2TP v3 or v2
 */
static s32 __proto_l2tp_select_init(struct protocol_info *info)
{
	struct prsr_up_layer_proto_params *params;
	struct np_info *np;
	s32 n;

	if (unlikely(ptr_is_null(info)))
		return -EINVAL;

	params = &info->up;
	info->id = PRSR_PROTO_L2TP_SEL;
	info->prof_id = PRSR_PROFILE_COMMON;
	info->name = params->name;
	n = strscpy(params->name, "L2TP Select", sizeof(params->name));
	/* parsing info */
	params->proto_id = U16_MAX;
	params->hw_idx = TABLE_ENTRY_INVALID_IDX;
	params->hdr_len = 2;
	params->entry_type = ETYPE_NONE;
	params->ignore = true;
	/* next protocol info. Next protocol is based on the L2TP version */
	np = &params->np;
	np->off = L2TP_SELECT_VER_OFF;
	np->len = L2TP_SELECT_VER_LEN;
	np->logic = NP_LOGIC_UDP;
	np->dflt = PRSR_PROTO_PAYLOAD;
	/* header skip info. Do not skip the header */
	params->skip.hdr_len_im = 0;
	params->skip.logic = SKIP_LOGIC_IM;
	/* preceding protocols */
	params->n_pre = 1;
	/* UDP -> L2TP select */
	params->pre[0].np_logic = NP_LOGIC_UDP;
	params->pre[0].val = L2TP_UDP_DPORT;
	__proto_pr_debug(info);
	pr_debug("done\n");

	return 0;
}

/**
 * @brief Init l2tpv2 protocol entry point
 * Helper parsing stage to check if the optional length field is present,
 * next protocol will continue parsing the header.
 */
static s32 __proto_l2tpv2_start_init(struct protocol_info *info)
{
	struct prsr_up_layer_proto_params *params;
	struct np_info *np;
	s32 n;

	if (unlikely(ptr_is_null(info)))
		return -EINVAL;

	params = &info->up;
	info->id = PRSR_PROTO_L2TPV2_START;
	info->prof_id = PRSR_PROFILE_COMMON;
	info->name = params->name;
	n = strscpy(params->name, "L2TPv2 start", sizeof(params->name));
	/* parsing info */
	params->proto_id = U16_MAX;
	params->hw_idx = TABLE_ENTRY_INVALID_IDX;
	params->hdr_len = 2;
	params->entry_type = ETYPE_NONE;
	params->ignore = true;
	/* next protocol info.
	 * Look at the first four bits of the header,
	 * those bits indicate if this is a control packet and if the optional
	 * length field is present (|T|L|x|x|)
	 */
	np = &params->np;
	np->off = 0;
	np->len = 4;
	np->logic = NP_LOGIC_UDP;
	np->dflt = PRSR_PROTO_PAYLOAD;
	/* header skip info */
	params->skip.hdr_len_im = 0;
	params->skip.logic = SKIP_LOGIC_IM;
	/* preceding protocols */
	params->n_pre = 1;
	/* L2TP select -> L2TPv2 start */
	params->pre[0].np_logic = NP_LOGIC_UDP;
	params->pre[0].val = L2TPV2_START;
	__proto_pr_debug(info);
	pr_debug("done\n");

	return 0;
}

/**
 * @brief Init l2tp v2 protocol with optional length field present
 */
static s32 __proto_l2tpv2_op_len_init(struct protocol_info *info)
{
	struct prsr_up_layer_proto_params *params;
	struct field_ext_info *fld;
	struct np_info *np;
	s32 n;

	if (unlikely(ptr_is_null(info)))
		return -EINVAL;

	params = &info->up;
	info->id = PRSR_PROTO_L2TPV2_LEN;
	info->prof_id = PRSR_PROFILE_COMMON;
	info->name = params->name;
	n = strscpy(params->name, "L2TPv2 len field", sizeof(params->name));
	/* parsing info */
	params->proto_id = U16_MAX;
	params->hw_idx = TABLE_ENTRY_INVALID_IDX;
	params->hdr_len = 8;
	params->entry_type = ETYPE_NONE;
	/* next protocol info - next protocol is always PPP */
	np = &params->np;
	np->logic = NP_LOGIC_END;
	np->dflt = PRSR_PROTO_PPP;
	/* header skip info, based on what the length field says */
	params->skip.hdr_len_off = L2TPV2_OPT_LEN_OFF * BITS_PER_BYTE;
	params->skip.hdr_len_sz = L2TPV2_OPT_LEN_LEN * BITS_PER_BYTE;
	params->skip.logic = SKIP_LOGIC_ALU;
	params->skip.hdr_len_im = 0;
	params->skip.op = ALU_OP_LEN_PLUS_IM;
	/* preceding protocols */
	params->n_pre = 1;
	/* L2TP v2 start -> L2TP v2 with length field */
	params->pre[0].np_logic = NP_LOGIC_UDP;
	params->pre[0].val = L2TPV2_LEN_PRESENT;
	/* extract tunnel id and session id */
	fld = &params->ext.fld[params->ext.n_fld++];
	fld->fv_idx = offsetof(struct pp_fv, tunn.l2tpoudp);
	/* tunnel and session ids are right after the optional length field */
	fld->off = L2TPV2_TUNN_SESS_OFF + L2TPV2_OPT_LEN_LEN;
	fld->len = L2TPV2_TUNN_SESS_LEN;
	fld->mask = 0;
	__proto_pr_debug(info);
	pr_debug("done\n");

	return 0;
}

/**
 * @brief Init l2tp v2 protocol with no length field.
 * The purpose of this helper parsing stage is to look at the other
 * optional fields in order to call the right parsing protocol that knows
 * how to calculate the header length.
 */
static s32 __proto_l2tpv2_no_op_len_init(struct protocol_info *info)
{
	struct prsr_up_layer_proto_params *params;
	struct np_info *np;
	s32 n;

	if (unlikely(ptr_is_null(info)))
		return -EINVAL;

	params = &info->up;
	info->id = PRSR_PROTO_L2TPV2_NO_LEN;
	info->prof_id = PRSR_PROFILE_COMMON;
	info->name = params->name;
	n = strscpy(params->name, "L2TPv2 no len field", sizeof(params->name));
	/* parsing info */
	params->proto_id = U16_MAX;
	params->hw_idx = TABLE_ENTRY_INVALID_IDX;
	params->hdr_len = 2;
	params->entry_type = ETYPE_NONE;
	params->ignore = true;
	/* next protocol info.
	 * Look at the first seven bits of the header (|T|L|x|x|S|x|O),
	 * Note that we only really care about S and O since we already know
	 * both T and L flags are clear.
	 */
	np = &params->np;
	np->off = 0;
	np->len = 7;
	np->logic = NP_LOGIC_IPV4_2;//NP_LOGIC_TUNN;
	np->dflt = PRSR_PROTO_PAYLOAD;
	/* header skip info */
	params->skip.hdr_len_im = 0;
	params->skip.logic = SKIP_LOGIC_IM;
	/* preceding protocols */
	params->n_pre = 1;
	/* L2TP v2 start -> L2TP v2 with no length field */
	params->pre[0].np_logic = NP_LOGIC_UDP;
	params->pre[0].val = L2TPV2_NO_LEN_PRESENT;
	__proto_pr_debug(info);
	pr_debug("done\n");

	return 0;
}

/**
 * @brief Init l2tp v2 protocol with no length info and no padding.
 * At this point we know the header flags look like:
 * |T|L|x|x|S|x|O|P|x|x|x|x|  Ver  |
 * |0|0|0|0|S|0|0|P|0|0|0|0|0|0|1|0|
 */
static s32 __proto_l2tpv2_no_op_pad_init(struct protocol_info *info)
{
	struct prsr_up_layer_proto_params *params;
	struct field_ext_info *fld;
	struct np_info *np;
	s32 n;

	if (unlikely(ptr_is_null(info)))
		return -EINVAL;

	params = &info->up;
	info->id = PRSR_PROTO_L2TPV2_NO_PAD;
	info->prof_id = PRSR_PROFILE_COMMON;
	info->name = params->name;
	n = strscpy(params->name, "L2TPv2 no padding",
		    sizeof(params->name));
	/* parsing info */
	params->proto_id = U16_MAX;
	params->hw_idx = TABLE_ENTRY_INVALID_IDX;
	params->hdr_len = 6;
	params->entry_type = ETYPE_NONE;
	/* next protocol info - next protocol is always PPP */
	np = &params->np;
	np->logic = NP_LOGIC_END;
	np->dflt = PRSR_PROTO_PPP;
	/* header skip info. This will be the size of the header with no
	 * optional fields, if the Sequence fields are present then add 4 extra
	 * bytes. We can achieve this by taking advantage of the header flags
	 * and using the ALU to calculate the header length as:
	 * header size = |S|x|O| flags + base_hdr_size
	 */
	params->skip.hdr_len_off = 4; // Start of |S|x|O| flags
	params->skip.hdr_len_sz = 3;
	params->skip.logic = SKIP_LOGIC_ALU;
	params->skip.hdr_len_im = L2TPV2_BASE_HDR_SIZE;
	params->skip.op = ALU_OP_LEN_PLUS_IM;
	/* preceding protocols */
	params->n_pre = 2;
	/* L2TP v2 without length field -> L2TP v2 without padding field */
	params->pre[0].np_logic = NP_LOGIC_IPV4_2;//NP_LOGIC_TUNN;
	params->pre[0].val = L2TPV2_NO_SEQ_NO_PAD;
	params->pre[1].np_logic = NP_LOGIC_IPV4_2;//NP_LOGIC_TUNN;
	params->pre[1].val = L2TPV2_SEQ_NO_PAD;
	/* extract tunnel id and session id */
	fld = &params->ext.fld[params->ext.n_fld++];
	fld->fv_idx = offsetof(struct pp_fv, tunn.l2tpoudp);
	/* tunnel and session ids are right after the version field */
	fld->off = L2TPV2_TUNN_SESS_OFF;
	fld->len = L2TPV2_TUNN_SESS_LEN;
	fld->mask = 0;
	__proto_pr_debug(info);
	pr_debug("done\n");

	return 0;
}

/**
 * @brief Init ppp protocol info
 */
static s32 __proto_ppp_init(struct protocol_info *info)
{
	struct prsr_up_layer_proto_params *params;
	struct np_info *np;
	s32 n;

	if (unlikely(ptr_is_null(info)))
		return -EINVAL;

	params = &info->up;
	info->id = PRSR_PROTO_PPP;
	info->prof_id = PRSR_PROFILE_COMMON;
	info->name = params->name;
	n = strscpy(params->name, "PPP", sizeof(params->name));
	/* parsing info */
	params->proto_id = L2TPV2_VER;
	params->hw_idx = TABLE_ENTRY_INVALID_IDX;
	params->hdr_len = PPP_HDRLEN;
	params->entry_type = ETYPE_NONE;
	params->ignore = true;
	/* next protocol info */
	np = &params->np;
	np->off = PPP_PROTO_OFF * BITS_PER_BYTE;
	np->len = PPP_PROTO_LEN * BITS_PER_BYTE;
	np->logic = NP_LOGIC_TUNN;
	np->dflt = PRSR_PROTO_IPV4_INT;
	/* header skip info */
	params->skip.hdr_len_im = PPP_HDR_LEN;
	params->skip.logic = SKIP_LOGIC_IM;
	/* preceding protocols */
	params->n_pre = 1;
	/* L2TP -> PPP */
	params->pre[0].np_logic = NP_LOGIC_TUNN;
	params->pre[0].val = U16_MAX; // Not used
	__proto_pr_debug(info);
	pr_debug("done\n");

	return 0;
}

/**
 * @brief Init udp vxlan protocol info
 */
static s32 __proto_vxlan_init(struct protocol_info *info)
{
	struct prsr_up_layer_proto_params *params;
	struct field_ext_info *fld;
	struct np_info        *np;
	s32 n;

	if (unlikely(ptr_is_null(info)))
		return -EINVAL;

	params      = &info->up;
	info->id       = PRSR_PROTO_VXLAN;
	info->prof_id  = PRSR_PROFILE_COMMON;
	info->name     = params->name; /* a reference to the name */
	n = strscpy(params->name, "VXLAN", sizeof(params->name));
	/* parsing info */
	params->proto_id   = VXLAN_UDP_DPORT;
	params->hw_idx     = TABLE_ENTRY_INVALID_IDX;
	params->hdr_len    = sizeof(struct vxlanhdr);
	params->entry_type = ETYPE_NONE;
	/* next protocol info - next protocol is always MAC header */
	np = &params->np;
	np->logic = NP_LOGIC_END;
	np->dflt  = PRSR_PROTO_MAC_INT;
	/* header skip info */
	params->skip.logic      = SKIP_LOGIC_IM;
	params->skip.hdr_len_im = sizeof(struct vxlanhdr);
	/* preceding protocols */
	params->n_pre = 1;
	/* UDP -> vxlan select */
	params->pre[0].np_logic = NP_LOGIC_UDP;
	params->pre[0].val      = VXLAN_UDP_DPORT;
	/* extract info */
	/* extract udp ports */
	fld = &params->ext.fld[params->ext.n_fld++];
	fld->fv_idx = offsetof(struct pp_fv, first.l4.udp);
	fld->off    = offsetof(struct udphdr, source);
	fld->len    = offsetof(struct udphdr, len) -
		      offsetof(struct udphdr, source);
	fld->mask   = 0;
	/* extract vni */
	fld = &params->ext.fld[params->ext.n_fld++];
	fld->fv_idx = offsetof(struct pp_fv, tunn.vxlan);
	fld->off    = sizeof(struct udphdr) +
		      offsetof(struct vxlanhdr, vx_vni);
	fld->len    = FIELD_SIZEOF(struct vxlanhdr, vx_vni);
	fld->mask   = 0;
	__proto_pr_debug(info);
	pr_debug("done\n");

	return 0;
}

/**
 * @brief Init udp geneve protocol info
 */
static s32 __proto_geneve_init(struct protocol_info *info)
{
	struct prsr_up_layer_proto_params *params;
	struct field_ext_info *fld;
	struct skip_info      *skip;
	struct np_info        *np;
	s32 n;

	if (unlikely(ptr_is_null(info)))
		return -EINVAL;

	params      = &info->up;
	info->id       = PRSR_PROTO_GENEVE;
	info->prof_id  = PRSR_PROFILE_COMMON;
	info->name     = params->name; /* a reference to the name */
	n = strscpy(params->name, "Geneve", sizeof(params->name));
	/* parsing info */
	params->proto_id   = GENEVE_UDP_DPORT;
	params->hw_idx     = TABLE_ENTRY_INVALID_IDX;
	params->hdr_len    = offsetof(struct genevehdr, options);
	params->entry_type = ETYPE_NONE;
	/* next protocol info - next protocol is always MAC header */
	np = &params->np;
	np->logic = NP_LOGIC_END;
	np->off   = offsetof(struct genevehdr, proto_type);
	np->len   = FIELD_SIZEOF(struct genevehdr, proto_type);
	np->dflt  = PRSR_PROTO_MAC_INT;
	/* header skip info */
	skip = &params->skip;
	skip->logic       = SKIP_LOGIC_ALU;
	skip->hdr_len_off = GENEVE_OPTS_OFF;
	skip->hdr_len_sz  = GENEVE_OPTS_LEN;
	skip->hdr_len_im  = offsetof(struct genevehdr, options);
	skip->op          = ALU_OP_4XLEN_PLUS_IM;
	/* preceding protocols */
	params->n_pre = 1;
	/* UDP -> geneve select */
	params->pre[0].np_logic = NP_LOGIC_UDP;
	params->pre[0].val      = GENEVE_UDP_DPORT;
	/* extract info */
	/* extract udp ports */
	fld = &params->ext.fld[params->ext.n_fld++];
	fld->fv_idx = offsetof(struct pp_fv, first.l4.udp);
	fld->off    = offsetof(struct udphdr, source);
	fld->len    = offsetof(struct udphdr, len) -
		      offsetof(struct udphdr, source);
	fld->mask   = 0;
	/* extract vni */
	fld = &params->ext.fld[params->ext.n_fld++];
	fld->fv_idx = offsetof(struct pp_fv, tunn.vxlan);
	fld->off    = sizeof(struct udphdr) +
		      offsetof(struct genevehdr, vni);
	fld->len    = offsetof(struct genevehdr, options) -
		      offsetof(struct genevehdr, vni);
	fld->mask   = 0;
	__proto_pr_debug(info);
	pr_debug("done\n");

	return 0;
}

/**
 * @brief Init sctp protocol info
 */
static s32 __proto_sctp_init(struct protocol_info *info)
{
	struct prsr_up_layer_proto_params *params;
	struct field_ext_info *fld;
	struct skip_info      *skip;
	struct np_info        *np;
	s32 n;

	if (unlikely(ptr_is_null(info)))
		return -EINVAL;

	params        = &info->up;
	info->id      = PRSR_PROTO_SCTP;
	info->prof_id = PRSR_PROFILE_COMMON;
	info->name    = params->name; /* a reference to the name */
	n = strscpy(params->name, "SCTP", sizeof(params->name));
	/* parsing info */
	params->proto_id   = IPPROTO_SCTP;
	params->hw_idx     = TABLE_ENTRY_INVALID_IDX;
	params->entry_type = ETYPE_OTHER_L4;
	/* next protocol info */
	np = &params->np;
	np->logic = NP_LOGIC_END;
	np->dflt  = PRSR_PROTO_PAYLOAD;
	/* header skip info */
	skip = &params->skip;
	skip->hdr_len_im = sizeof(struct sctphdr);
	skip->logic = SKIP_LOGIC_IM;
	/* preceding protocols */
	params->n_pre = 4;
	/* IPv4 -> SCTP */
	params->pre[0].np_logic = NP_LOGIC_IPV4;
	params->pre[0].val      = IPPROTO_SCTP;
	/* IPv4 Internal -> SCTP */
	params->pre[1].np_logic = NP_LOGIC_IPV4_2;
	params->pre[1].val      = IPPROTO_SCTP;
	/* IPv6 -> SCTP */
	params->pre[2].np_logic = NP_LOGIC_IPV6;
	params->pre[2].val      = IPPROTO_SCTP;
	/* IPv6 Internal -> SCTP */
	params->pre[3].np_logic = NP_LOGIC_IPV6_2;
	params->pre[3].val      = IPPROTO_SCTP;
	/* extract info */
	/* extract ports and vtag vtag */
	fld = &params->ext.fld[params->ext.n_fld++];
	fld->fv_idx = offsetof(struct pp_fv, tunn.sctp);
	fld->off    = offsetof(struct sctphdr, source);
	fld->len    = offsetof(struct sctphdr, checksum) -
		      offsetof(struct sctphdr, source);
	fld->mask   = 0;
	__proto_pr_debug(info);
	pr_debug("done\n");

	return 0;
}

/**
 * @brief Shortcut for adding new protocol to the profile protocols
 *        array and verifying we didn't exceeded the array size
 * @param init_func protocol init function the function should init
 *                  all the protocols parameters
 * @param protos profile protocols array
 * @param ret variable to set error code in case init function fails
 * @param i variable to update the number of protocols which are
 *          currently configured in the array
 */
#define INIT_PROTO(init_func, protos, ret, i) \
do {									 \
	if (i < ARRAY_SIZE(protos->proto))				 \
		ret |= init_func(&protos->proto[i++]);			 \
	else								 \
		prsr_err("Cannot call '%s', max protocols %u reached\n", \
			 #init_func, (u32)ARRAY_SIZE(protos->proto));	 \
} while (0)

/**
 * @brief Shortcut for adding new protocols merge info to the profile
 *        merge infos array and verifying we didn't exceeded the array
 *        size
 * @param _prof_id profile id
 * @param _prev previous protocol
 * @param _curr current protocol
 * @param _merged merged protocol
 * @param protos profile protocols array
 * @param ret variable to set error code in case init function fails
 * @param i variable to update the number of protocols which are
 *          currently configured in the array
 */
#define INIT_MERGE(_prof_id, _prev, _curr, _merged, protos, ret, i)	    \
do {									    \
	if (i < ARRAY_SIZE(protos->merge)) {				    \
		protos->merge[i].prof_id     = _prof_id;		    \
		protos->merge[i].info.prev   = _prev;			    \
		protos->merge[i].info.curr   = _curr;			    \
		protos->merge[i].info.merged = _merged;			    \
		i++;							    \
	} else {							    \
		prsr_err("Cannot add new merge info, max of %u reached\n",  \
			 (u32)ARRAY_SIZE(protos->merge));		    \
			 ret = -ENOSPC;					    \
	}								    \
} while (0)

s32 __prsr_protocols_init(struct protocols *protos)
{
	s32 ret = 0;
	u32 i   = 0;

	if (unlikely(!protos))
		return -EINVAL;

	/* layer 2 protocols init */
	INIT_PROTO(__proto_mac_init,       protos, ret, i);
	INIT_PROTO(__proto_ext_vlan_init,  protos, ret, i);
	INIT_PROTO(__proto_stag_init,      protos, ret, i);
	INIT_PROTO(__proto_vlan_init,      protos, ret, i);
	INIT_PROTO(__proto_pppoe_init,     protos, ret, i);
	INIT_PROTO(__proto_snap_init,      protos, ret, i);

	/* upper layers protocols init */
	/* idle must be first upper layer protocol */
	INIT_PROTO(__proto_idle_init,           protos, ret, i);
	/* payload must be the second upper layer protocol */
	INIT_PROTO(__proto_payload_init,        protos, ret, i);
	INIT_PROTO(__proto_no_parse_init,       protos, ret, i);
	/* other upper layers protocols starts here */
	INIT_PROTO(__proto_mac_int_init,        protos, ret, i);
	INIT_PROTO(__proto_vlan_int_init,       protos, ret, i);
	INIT_PROTO(__proto_q_in_q_init,         protos, ret, i);
	INIT_PROTO(__proto_pppoe_int_init,      protos, ret, i);
	INIT_PROTO(__proto_ptp_init,            protos, ret, i);
	INIT_PROTO(__proto_ipv4_init,           protos, ret, i);
	INIT_PROTO(__proto_ipv6_init,           protos, ret, i);
	INIT_PROTO(__proto_ip_select_init,      protos, ret, i);
	INIT_PROTO(__proto_icmpv4_init,         protos, ret, i);
	INIT_PROTO(__proto_icmpv6_init,         protos, ret, i);
	INIT_PROTO(__proto_ipv4_int_init,       protos, ret, i);
	INIT_PROTO(__proto_tcp_init,            protos, ret, i);
	INIT_PROTO(__proto_udp_init,            protos, ret, i);
	INIT_PROTO(__proto_esp_init,            protos, ret, i);
	INIT_PROTO(__proto_geneve_init,         protos, ret, i);
	INIT_PROTO(__proto_vxlan_init,          protos, ret, i);
	INIT_PROTO(__proto_tcp_int_init,        protos, ret, i);
	INIT_PROTO(__proto_udp_int_init,        protos, ret, i);
	INIT_PROTO(__proto_esp_int_init,        protos, ret, i);
	INIT_PROTO(__proto_ppp_init,            protos, ret, i);
	INIT_PROTO(__proto_l2tpv3_ip_init,      protos, ret, i);
	INIT_PROTO(__proto_l2tp_select_init,    protos, ret, i);
	INIT_PROTO(__proto_l2tpv2_start_init,   protos, ret, i);
	INIT_PROTO(__proto_l2tpv2_op_len_init,  protos, ret, i);
	INIT_PROTO(__proto_l2tpv2_no_op_pad_init, protos, ret, i);
	INIT_PROTO(__proto_l2tpv2_no_op_len_init, protos, ret, i);
	INIT_PROTO(__proto_l2tpv3_udp_init,     protos, ret, i);
	INIT_PROTO(__proto_ipv6_int_init,       protos, ret, i);
	INIT_PROTO(__proto_gre_init,            protos, ret, i);
	INIT_PROTO(__proto_sctp_init,           protos, ret, i);
	INIT_PROTO(__proto_ipv6_exten_init,     protos, ret, i);
	INIT_PROTO(__proto_ipv6_exten_int_init, protos, ret, i);
	INIT_PROTO(__proto_ipv6_frag_init,      protos, ret, i);
	INIT_PROTO(__proto_ipv6_frag_int_init,  protos, ret, i);

	protos->n_proto = i;

	/* merge protocols */
	i = 0;
	INIT_MERGE(PRSR_PROFILE_COMMON, PRSR_PROTO_MAC_INT,
		   PRSR_PROTO_VLAN_INT, PRSR_PROTO_VLAN_INT,
		   protos, ret, i);
	INIT_MERGE(PRSR_PROFILE_COMMON, PRSR_PROTO_MAC_INT,
		   PRSR_PROTO_QINQ, PRSR_PROTO_QINQ,
		   protos, ret, i);
	INIT_MERGE(PRSR_PROFILE_COMMON, PRSR_PROTO_UDP,
		   PRSR_PROTO_VXLAN, PRSR_PROTO_VXLAN,
		   protos, ret, i);
	INIT_MERGE(PRSR_PROFILE_COMMON, PRSR_PROTO_UDP,
		   PRSR_PROTO_GENEVE, PRSR_PROTO_GENEVE,
		   protos, ret, i);
	protos->n_merge = i;

	return ret;
}

