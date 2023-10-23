/*
 * Description: PP Modifier Recipes
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018-2020 Intel Corporation
 */

#define pr_fmt(fmt) "[PP_MODIFIER_RECIPES]: %s:%d: " fmt, __func__, __LINE__

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/if_pppox.h>
#include <linux/tcp.h>
#include <linux/udp.h>

#include "pp_common.h"
#include "modifier.h"
#include "modifier_internal.h"

/* supported recipes description */
static const char * const mod_rcp_str[MOD_RCPS_NUM + 2] = {
	[MOD_RCP_NO_MOD]                  = "No modification",
	[MOD_RCP_HDR_RPLC]                = "Header replacement",
	[MOD_RCP_IPV4_NAT]                = "IPv4 NAT",
	[MOD_RCP_IPV4_NAPT]               = "IPv4 NAPT",
	[MOD_RCP_IPV6_ROUTED]             = "IPv6 routed",
	[MOD_RCP_IPV4_IPV4_ENCAP]         = "ipv4 over ipv4 encapsulation",
	[MOD_RCP_IPV6_IPV4_ENCAP]         = "ipv4 over ipv6 encapsulation",
	[MOD_RCP_IPV6_IPV6_ENCAP]         = "ipv6 over ipv6 encapsulation",
	[MOD_RCP_IPV4_IPV6_ENCAP]         = "ipv6 over ipv4 encapsulation",
	[MOD_RCP_EOGRE_DECP_IPV4_NAPT]    = "EoGRE decap inner ipv4 NAPT",
	[MOD_RCP_IPOGRE_DECP_IPV4_NAPT]   = "IPoGRE decap inner ipv4 NAPT",
	[MOD_RCP_EOGRE_DECP_IPV6_ROUTED]  = "EoGRE decap inner ipv6 NAPT",
	[MOD_RCP_IPOGRE_DECP_IPV6_ROUTED] = "IPoGRE decap inner ipv6 NAPT",
	[MOD_RCP_DSLITE_DECP_NAPT]        = "DsLite decap NAPT",
	[MOD_RCP_6RD_DECP_ROUTED]         = "6RD decap",
	[MOD_RCP_ESP_V4_TUNNL_ENCAP]      = "ESP over ipv4 encapsulation",
	[MOD_RCP_ESP_V4_TUNNL_BPAS]       = "ESP over ipv4 bypass",
	[MOD_RCP_ESP_OUTER_IP_REM]        = "ESP outer ip removal",
	[MOD_RCP_L2TPUDP_DECP_IPV4_NAPT]  = "L2TP UDP decap inner ipv4 NAPT",
	[MOD_RCP_L2TPUDP_DECP_IPV6_ROUTED] = "L2TP UDP decap inner ipv6 NAPT",
	[MOD_RCP_L2TPIP_DECP_IPV4_NAPT]   = "L2TP IP decap inner ipv4 NAPT",
	[MOD_RCP_L2TPIP_DECP_IPV6_ROUTED] = "L2TP IP decap inner ipv6 NAPT",
	[MOD_RCP_CUSTOM]                  = "CUSTOM",
	[MOD_RCPS_NUM]                    = "NOP",
	[MOD_RCPS_NUM + 1]                = "INVALID",
};

/* modification flags description */
static const char * const mod_flag_str[MOD_FLAG_NUM + 1] = {
	[MOD_HDR_RPLC_BIT]          = "HDR RPLC",
	[MOD_INNER_PPPOE_BIT]       = "PPPoE INNER",
	[MOD_IPV4_NAT_BIT]          = "IPv4 NAT",
	[MOD_IPV6_HOPL_BIT]         = "IPv6 HOPL",
	[MOD_L4_NAPT_BIT]           = "L4 NAPT",
	[MOD_ICMP4_ID_BIT]          = "ICMPv4 ID",
	[MOD_EOGRE_DECP_BIT]        = "EoGRE decap",
	[MOD_EOGRE_V4_ENCP_BIT]     = "EoGREv4 encap",
	[MOD_EOGRE_V6_ENCP_BIT]     = "EoGREv6 encap",
	[MOD_IPOGRE_DECP_BIT]       = "IPoGRE decap",
	[MOD_IPOGRE_V4_ENCP_BIT]    = "IPoGREv4 encap",
	[MOD_IPOGRE_V6_ENCP_BIT]    = "IPoGREv6 encap",
	[MOD_DSLITE_DECP_BIT]       = "DsLite decap",
	[MOD_DSLITE_ENCP_BIT]       = "DsLite encap",
	[MOD_6RD_ENCP_BIT]          = "6RD encap",
	[MOD_6RD_DECP_BIT]          = "6RD decap",
	[MOD_ESP_V4_TUNNL_ENCP_BIT] = "ESPv4 tunnel encap",
	[MOD_ESP_V4_TUNNL_BPAS_BIT] = "ESPv4 tunnel bypass",
	[MOD_ESP_OUTER_IP_REM_BIT]  = "ESP outer ip removal",
	[MOD_L2TPUDP_IPV4_ENCP_BIT] = "L2TPUDPv4 encap",
	[MOD_L2TPUDP_IPV6_ENCP_BIT] = "L2TPUDPv6 encap",
	[MOD_L2TPUDP_DECP_BIT]      = "L2TPUDP decap",
	[MOD_L2TPIP_IPV4_ENCP_BIT]  = "L2TPIPv4 encap",
	[MOD_L2TPIP_IPV6_ENCP_BIT]  = "L2TPIPv6 encap",
	[MOD_L2TPIP_DECP_BIT]       = "L2TPIP decap",
	[MOD_FLAG_NUM]              = "INVALID",
};

#define MOD_HDR_RPLC_MSK          BIT(MOD_HDR_RPLC_BIT)
#define MOD_INNER_PPPOE_MSK       BIT(MOD_INNER_PPPOE_BIT)
#define MOD_IPV4_NAT_MSK          BIT(MOD_IPV4_NAT_BIT)
#define MOD_IPV6_HOPL_MSK         BIT(MOD_IPV6_HOPL_BIT)
#define MOD_L4_NAPT_MSK           BIT(MOD_L4_NAPT_BIT)
#define MOD_ICMP4_ID_MSK          BIT(MOD_ICMP4_ID_BIT)
#define MOD_EOGRE_DECP_MSK        BIT(MOD_EOGRE_DECP_BIT)
#define MOD_EOGRE_V4_ENCP_MSK     BIT(MOD_EOGRE_V4_ENCP_BIT)
#define MOD_EOGRE_V6_ENCP_MSK     BIT(MOD_EOGRE_V6_ENCP_BIT)
#define MOD_IPOGRE_DECP_MSK       BIT(MOD_IPOGRE_DECP_BIT)
#define MOD_IPOGRE_V4_ENCP_MSK    BIT(MOD_IPOGRE_V4_ENCP_BIT)
#define MOD_IPOGRE_V6_ENCP_MSK    BIT(MOD_IPOGRE_V6_ENCP_BIT)
#define MOD_DSLITE_DECP_MSK       BIT(MOD_DSLITE_DECP_BIT)
#define MOD_DSLITE_ENCP_MSK       BIT(MOD_DSLITE_ENCP_BIT)
#define MOD_6RD_ENCP_MSK          BIT(MOD_6RD_ENCP_BIT)
#define MOD_6RD_DECP_MSK          BIT(MOD_6RD_DECP_BIT)
#define MOD_ESP_V4_TUNNL_ENCP_MSK BIT(MOD_ESP_V4_TUNNL_ENCP_BIT)
#define MOD_ESP_V4_TUNNL_BPAS_MSK BIT(MOD_ESP_V4_TUNNL_BPAS_BIT)
#define MOD_ESP_OUTER_IP_REM_MSK  BIT(MOD_ESP_OUTER_IP_REM_BIT)
#define MOD_L2TPUDP_IPV4_ENCP_MSK BIT(MOD_L2TPUDP_IPV4_ENCP_BIT)
#define MOD_L2TPUDP_IPV6_ENCP_MSK BIT(MOD_L2TPUDP_IPV6_ENCP_BIT)
#define MOD_L2TPIP_IPV4_ENCP_MSK  BIT(MOD_L2TPIP_IPV4_ENCP_BIT)
#define MOD_L2TPIP_IPV6_ENCP_MSK  BIT(MOD_L2TPIP_IPV6_ENCP_BIT)
#define MOD_L2TPUDP_DECP_MSK      BIT(MOD_L2TPUDP_DECP_BIT)
#define MOD_L2TPIP_DECP_MSK       BIT(MOD_L2TPIP_DECP_BIT)

enum rcp_new_hdr_ip_type {
	RCP_NEW_HDR_V4,
	RCP_NEW_HDR_V6,
	RCP_NEW_HDR_NO_IP,
};

/**
 * @brief Supported modification definition
 */
struct supp_mod {
	ulong flags;  /*! modification flags */
	u8    recipe; /*! recipe id */
};

/**
 * @brief Array of all supported modification types
 */
static const struct supp_mod supp_mod_types[] = {
	{ /* no modification */
		.flags  = 0,
		.recipe = MOD_RCP_NO_MOD
	},
	{ /* Header replacement */
		.flags  = MOD_HDR_RPLC_MSK,
		.recipe = MOD_RCP_HDR_RPLC
	},
	{ /* IPv4 NAT */
		.flags  = MOD_IPV4_NAT_MSK,
		.recipe = MOD_RCP_IPV4_NAT
	},
	{ /* IPv4 NAPT */
		.flags  = MOD_IPV4_NAT_MSK | MOD_L4_NAPT_MSK,
		.recipe = MOD_RCP_IPV4_NAPT
	},
	{ /* IPv6 NAT */
		.flags  = MOD_IPV6_HOPL_MSK,
		.recipe = MOD_RCP_IPV6_ROUTED
	},
	/* ================================================================ */
	/*          IPv4(inner) over IPv6(outer) Encapsulation              */
	/* ================================================================ */
	{ /* DsLite tunnel encapsulation */
		.flags  = MOD_DSLITE_ENCP_MSK | MOD_IPV4_NAT_MSK |
			  MOD_L4_NAPT_MSK,
		.recipe = MOD_RCP_IPV6_IPV4_ENCAP
	},
	{ /* ETH IPv4 over GRE over IPv6 encapsulation */
		.flags  = MOD_EOGRE_V6_ENCP_MSK | MOD_IPV4_NAT_MSK |
			  MOD_L4_NAPT_MSK,
		.recipe = MOD_RCP_IPV6_IPV4_ENCAP
	},
	{ /* IPv4 over GRE over IPv6 encapsulation */
		.flags  = MOD_IPOGRE_V6_ENCP_MSK | MOD_IPV4_NAT_MSK |
			  MOD_L4_NAPT_MSK,
		.recipe = MOD_RCP_IPV6_IPV4_ENCAP
	},
	{ /* IPv4 over L2TP UDP over IPv6 encapsulation */
		.flags  = MOD_L2TPUDP_IPV6_ENCP_MSK | MOD_IPV4_NAT_MSK |
			  MOD_L4_NAPT_MSK,
		.recipe = MOD_RCP_IPV6_IPV4_ENCAP
	},
	{ /* IPv4 over L2TP IP over IPv6 encapsulation */
		.flags	= MOD_L2TPIP_IPV6_ENCP_MSK | MOD_IPV4_NAT_MSK |
			  MOD_L4_NAPT_MSK,
		.recipe = MOD_RCP_IPV6_IPV4_ENCAP
	},
	/* ================================================================ */
	/*          IPv6(inner) over IPv6(outer) Encapsulation              */
	/* ================================================================ */
	{ /* ETH IPv6 over GRE over IPv6 encapsulation */
		.flags  = MOD_EOGRE_V6_ENCP_MSK | MOD_IPV6_HOPL_MSK,
		.recipe = MOD_RCP_IPV6_IPV6_ENCAP
	},
	{ /* IPv6 over GRE over IPv6 encapsulation */
		.flags  = MOD_IPOGRE_V6_ENCP_MSK | MOD_IPV6_HOPL_MSK,
		.recipe = MOD_RCP_IPV6_IPV6_ENCAP
	},
	{ /* IPv6 over L2TP UDP over IPv6 encapsulation */
		.flags  = MOD_L2TPUDP_IPV6_ENCP_MSK | MOD_IPV6_HOPL_MSK,
		.recipe = MOD_RCP_IPV6_IPV6_ENCAP
	},
	{ /* IPv6 over L2TP IP over IPv6 encapsulation */
		.flags  = MOD_L2TPIP_IPV6_ENCP_MSK | MOD_IPV6_HOPL_MSK,
		.recipe = MOD_RCP_IPV6_IPV6_ENCAP
	},
	/* ================================================================ */
	/*          IPv6(inner) over IPv4(outer) Encapsulation              */
	/* ================================================================ */
	{ /* 6RD tunnel encapsulation */
		.flags	= MOD_6RD_ENCP_MSK | MOD_IPV6_HOPL_MSK,
		.recipe = MOD_RCP_IPV4_IPV6_ENCAP
	},
	{ /* IPv6 over GRE over IPv4 encapsulation */
		.flags  = MOD_IPOGRE_V4_ENCP_MSK | MOD_IPV6_HOPL_MSK,
		.recipe = MOD_RCP_IPV4_IPV6_ENCAP
	},
	{ /* ETH IPv6 over GRE over IPv4 encapsulation */
		.flags  = MOD_EOGRE_V4_ENCP_MSK | MOD_IPV6_HOPL_MSK,
		.recipe = MOD_RCP_IPV4_IPV6_ENCAP
	},
	{ /* IPv6 over L2TP UDP over IPv4 encapsulation */
		.flags  = MOD_L2TPUDP_IPV4_ENCP_MSK | MOD_IPV6_HOPL_MSK,
		.recipe = MOD_RCP_IPV4_IPV6_ENCAP
	},
	{ /* IPv6 over L2TP IP over IPv4 encapsulation */
		.flags	= MOD_L2TPIP_IPV4_ENCP_MSK | MOD_IPV6_HOPL_MSK,
		.recipe = MOD_RCP_IPV4_IPV6_ENCAP
	},
	/* ================================================================ */
	/*          IPv4(inner) over IPv4(outer) Encapsulation              */
	/* ================================================================ */
	{ /* IPv4 over GRE over IPv4 encapsulation */
		.flags  = MOD_IPOGRE_V4_ENCP_MSK | MOD_IPV4_NAT_MSK |
			  MOD_L4_NAPT_MSK,
		.recipe = MOD_RCP_IPV4_IPV4_ENCAP
	},
	{ /* ETH IPv4 over GRE over IPv4 encapsulation */
		.flags  = MOD_EOGRE_V4_ENCP_MSK | MOD_IPV4_NAT_MSK |
			  MOD_L4_NAPT_MSK,
		.recipe = MOD_RCP_IPV4_IPV4_ENCAP
	},
	{ /* IPv4 over L2TP UDP over IPv4 encapsulation */
		.flags  = MOD_L2TPUDP_IPV4_ENCP_MSK | MOD_IPV4_NAT_MSK |
			  MOD_L4_NAPT_MSK,
		.recipe = MOD_RCP_IPV4_IPV4_ENCAP
	},
	{ /* IPv4 over L2TP UDP over IPv4 encapsulation */
		.flags	= MOD_L2TPIP_IPV4_ENCP_MSK | MOD_IPV4_NAT_MSK |
			  MOD_L4_NAPT_MSK,
		.recipe = MOD_RCP_IPV4_IPV4_ENCAP
	},
	/* ================================================================ */
	/*         ESP tunnel mode over IPv4(outer) Encapsulation           */
	/* ================================================================ */
	{ /* No inner layers, new header has ipv4 */
		.flags  = MOD_ESP_V4_TUNNL_ENCP_MSK | MOD_HDR_RPLC_MSK,
		.recipe = MOD_RCP_ESP_V4_TUNNL_ENCAP
	},
	/* ================================================================ */
	/*         ESP tunnel mode over IPv4(outer) Bypass                  */
	/* ================================================================ */
	{ /* No inner layers, new header has no ip */
		.flags  = MOD_ESP_V4_TUNNL_BPAS_MSK | MOD_HDR_RPLC_MSK,
		.recipe = MOD_RCP_ESP_V4_TUNNL_BPAS
	},
	/* ================================================================ */
	/*         ESP decapsulation with 1st session                       */
	/* ================================================================ */
	{ /* esp w/o outer ip layer */
		.flags = MOD_ESP_OUTER_IP_REM_MSK,
		.recipe = MOD_RCP_ESP_OUTER_IP_REM
	},
	/* ================================================================ */
	/*                    GRE Decapsulation                             */
	/* ================================================================ */
	{ /* ETH IPv4 over GRE over IPv4/6 decapsulation */
		.flags  = MOD_EOGRE_DECP_MSK | MOD_IPV4_NAT_MSK |
			  MOD_L4_NAPT_MSK,
		.recipe = MOD_RCP_EOGRE_DECP_IPV4_NAPT
	},
	{ /* IPv4 over GRE over IPv4/6 decapsulation */
		.flags  = MOD_IPOGRE_DECP_MSK | MOD_IPV4_NAT_MSK |
			  MOD_L4_NAPT_MSK,
		.recipe = MOD_RCP_IPOGRE_DECP_IPV4_NAPT
	},
	{ /* ETH IPv6 over GRE over IPv4/6 decapsulation */
		.flags  = MOD_EOGRE_DECP_MSK | MOD_IPV6_HOPL_MSK,
		.recipe = MOD_RCP_EOGRE_DECP_IPV6_ROUTED
	},
	{ /* IPv6 over GRE over IPv4/6 decapsulation */
		.flags  = MOD_IPOGRE_DECP_MSK | MOD_IPV6_HOPL_MSK,
		.recipe = MOD_RCP_IPOGRE_DECP_IPV6_ROUTED
	},
	/* ================================================================ */
	/*                    DSLITE Decapsulation                          */
	/* ================================================================ */
	{ /* Dslite tunnel decapsulation */
		.flags  = MOD_DSLITE_DECP_MSK | MOD_IPV4_NAT_MSK |
			  MOD_L4_NAPT_MSK,
		.recipe = MOD_RCP_DSLITE_DECP_NAPT
	},
	/* ================================================================ */
	/*                    6RD Decapsulation                             */
	/* ================================================================ */
	{ /* 6RD inner IPv6 decapsulation */
		.flags  = MOD_6RD_DECP_MSK | MOD_IPV6_HOPL_MSK,
		.recipe = MOD_RCP_6RD_DECP_ROUTED
	},
	/* ================================================================ */
	/*                    L2TP Decapsulation                            */
	/* ================================================================ */
	{ /* Inner IPv6 over L2TP UDP  */
		.flags  = MOD_L2TPUDP_DECP_MSK | MOD_IPV6_HOPL_MSK,
		.recipe = MOD_RCP_L2TPUDP_DECP_IPV6_ROUTED
	},
	{ /* Inner IPv4 NAPT over L2TP UDP  */
		.flags  = MOD_L2TPUDP_DECP_MSK | MOD_IPV4_NAT_MSK |
			  MOD_L4_NAPT_MSK,
		.recipe = MOD_RCP_L2TPUDP_DECP_IPV4_NAPT
	},
	{ /* Inner IPv6 over L2TP IP  */
		.flags	= MOD_L2TPIP_DECP_MSK | MOD_IPV6_HOPL_MSK,
		.recipe = MOD_RCP_L2TPIP_DECP_IPV6_ROUTED
	},
	{ /* Inner IPv4 NAPT over L2TP IP  */
		.flags	= MOD_L2TPIP_DECP_MSK | MOD_IPV4_NAT_MSK |
			  MOD_L4_NAPT_MSK,
		.recipe = MOD_RCP_L2TPIP_DECP_IPV4_NAPT
	},
};

s32 mod_recipe_select(unsigned long mod_bmap, u8 *rcp)
{
	u32 i;

	if (ptr_is_null(rcp))
		return -EINVAL;

	pr_debug("mod_bmap %#lx\n", mod_bmap);
	for (i = 0; i < ARRAY_SIZE(supp_mod_types); i++) {
		if (supp_mod_types[i].flags != mod_bmap)
			continue;

		*rcp = supp_mod_types[i].recipe;
		pr_debug("recipe %s(%u) selected\n",
			 mod_rcp_to_str(*rcp), *rcp);
		return 0;
	}

	pr_err("Modification %#lx isn't supported\n", mod_bmap);
	return -EPROTONOSUPPORT;
}

const char * const mod_rcp_to_str(u32 rcp_idx)
{
	if (rcp_idx == MOD_NO_RCP || rcp_idx == MOD_RCP_NOP)
		return mod_rcp_str[MOD_RCPS_NUM];

	if (unlikely(rcp_idx > MOD_RCP_LAST)) {
		return mod_rcp_str[MOD_RCPS_NUM + 1];
	}

	return mod_rcp_str[rcp_idx];
}

const char * const mod_flag_to_str(u32 flag)
{
	if (unlikely(flag > MOD_FLAG_LAST)) {
		pr_err("error: invalid flag id, %d\n", flag);
		return mod_flag_str[MOD_FLAG_NUM];
	}

	return mod_flag_str[flag];
}

static s32 mod_rcp_no_mod_set(s32 rcp_idx)
{
	struct mod_recipe rcp;
	s32 ret;

	if (unlikely(rcp_idx > MOD_RCP_LAST)) {
		pr_err("error: invalid recipe id, %d\n", rcp_idx);
		return -EINVAL;
	}

	pr_debug("Configuring recipe %s(%u)\n", mod_rcp_to_str(rcp_idx),
		 rcp_idx);

	memset(&rcp, 0, sizeof(rcp));
	rcp.index = rcp_idx;
	ret = mod_rcp_complete(&rcp);
	if (unlikely(ret)) {
		pr_err("Failed to create recipe %s(%d)\n",
		       mod_rcp_to_str(rcp_idx), rcp_idx);
	}

	return ret;
}

static s32 mod_rcp_hdr_rplc_set(s32 rcp_idx, bool pppoe_update,
				enum mod_eu_stw_mux_sel strip_l3_off)
{
	struct mod_recipe rcp;
	s32 ret = 0;
	s32 pkt_l3_len     = DPU_RCP_REG0_B0;
	s32 si_l3_ttl_diff = DPU_RCP_REG1_B0;
	s32 pppoe_diff     = DPU_RCP_REG1_B1;
	s32 pppoe_len      = DPU_RCP_REG1_B2;

	/* USE registers R12 - R15 with extra caution as they are
	 * the registers to compare to for the alu conditions
	 * R12 - in used by ALU0 for pppoe and l4 csum calculation and MUST
	 *       be zero in the time of the calculation
	 * R13 - not used
	 * R14 - not used
	 * R15 - not used
	 */

	if (unlikely(rcp_idx > MOD_RCP_LAST)) {
		pr_err("error: invalid recipe id, %d\n", rcp_idx);
		return -EINVAL;
	}

	pr_debug("Configuring recipe %s(%u)\n", mod_rcp_to_str(rcp_idx),
		 rcp_idx);
	memset(&rcp, 0, sizeof(rcp));
	rcp.index = rcp_idx;

	if (pppoe_update) {
		/*==========================================*/
		/* copy from sce mux to dpu registers (sce) */
		/*==========================================*/
		/* Fetch SI info: ttl, pppoe diff and pppoe length fields */
		ret |= mod_sce_mux_to_reg_cpy(&rcp, si_l3_ttl_diff,
					      SCE_SI_SEL_TTL);

		/*===========================================*/
		/* copy from packet to dpu registers (store) */
		/*===========================================*/
		/* store L3 total_len */
		ret |= mod_pkt_to_reg_cpy(&rcp, pkt_l3_len, strip_l3_off,
					  MOD_SI_PKT_OFF,
					  FIELD_SIZEOF(struct iphdr, tot_len));

		/*=========================*/
		/*      ALU operations     */
		/*=========================*/
		/* sync to the IP len field */
		ret |= mod_alu_synch(EU_ALU0, &rcp, strip_l3_off,
				     MOD_SI_PKT_OFF,
				     FIELD_SIZEOF(struct iphdr, tot_len));

		/* pppoe_len = pppoe_diff ? pppoe_diff + l3 len : l2_org_val */
		ret |= mod_alu_add_cond(EU_ALU0, &rcp, pppoe_diff, 1,
					pkt_l3_len, 2, pppoe_len,
					EU_ALU_L0_BIG_R2, EU_ALU_COND_MET);
	}

	/*============================================*/
	/* copy from si bce (sram) to packet (insrep) */
	/*============================================*/
	/* replace old header with new header */
	ret |= mod_hdr_rplc(&rcp, BCE_OFF_0, strip_l3_off);

	/*======================================================*/
	/* copy from dpu registers to packet new header (reps1) */
	/*======================================================*/
	if (pppoe_update)
		ret |= mod_reg_to_new_hdr_cpy(&rcp, pppoe_len,
					      EU_SI_SEL_PPPOE_OFF,
					      offsetof(struct pppoe_hdr,
						       length),
					      FIELD_SIZEOF(struct pppoe_hdr,
							   length));

	/*=================================================*/
	/* complete recipe with nop command and 'last' bit */
	/*=================================================*/
	ret |= mod_rcp_complete(&rcp);
	if (unlikely(ret)) {
		pr_err("Failed to create recipe %s(%d)\n",
		       mod_rcp_to_str(rcp_idx), rcp_idx);
	}

	return ret;
}

/**
 * @brief Create recipe for IPv4 traffic with NAT changes in L3 and L4
 * @param rcp_idx recipe index to create
 * @param l3_off offset to the l3 offset field in the status word
 *               all prior layers will be trimmed, l3 NAT changes will
 *               be done on that layer, l4 NAT changes will done on
 *               the following tcp/udp header
 * @param l4_napt specify if l4 napt changes are needed in the
 *                    recipe
 * @param nhdr new header ip type
 * @param pppoe_encap specify if pppoe encapsulation should be supported
 * @return s32 0 on success, error code otherwise
 */
static s32 mod_rcp_ipv4_routed_set(s32 rcp_idx, enum mod_eu_stw_mux_sel l3_off,
				   bool l4_napt, enum rcp_new_hdr_ip_type nhdr,
				   bool pppoe_encap)
{
	struct mod_recipe rcp;
	s32 ret = 0;

	/* ================= */
	/* Map dpu registers */
	/* ================= */
	s32 si_l4_csum_delta   = DPU_RCP_REG0_B0;
	s32 si_l3_csum_delta   = DPU_RCP_REG0_B2;
	s32 si_l3_tot_len_diff = DPU_RCP_REG1_B0;
	s32 si_l3_tos          = DPU_RCP_REG1_B1;
	s32 si_nhdr_csum       = DPU_RCP_REG1_B2;
	s32 si_l3_ttl_diff     = DPU_RCP_REG2_B0;
	s32 pppoe_diff         = DPU_RCP_REG2_B1;
	s32 pppoe_len          = DPU_RCP_REG2_B2;
	s32 si_l4_ports        = DPU_RCP_REG3_B0;
	s32 pkt_l3_csum        = DPU_RCP_REG4_B0;
	s32 pkt_l3_ttl         = DPU_RCP_REG4_B3;
	s32 pkt_l4_csum        = DPU_RCP_REG5_B0;
	s32 pkt_l3_tot_len     = DPU_RCP_REG6_B0;
	s32 si_l4_csum_zero    = DPU_RCP_REG8_B0;

	/* USE registers R12 - R15 with extra caution as they are
	 * the registers to compare to for the alu conditions
	 * R12 - in used by ALU0 for pppoe and l4 csum calculation and MUST
	 *       be zero in the time of the calculation
	 * R13 - not used
	 * R14 - not used
	 * R15 - not used
	 */

	if (unlikely(rcp_idx > MOD_RCP_LAST)) {
		pr_err("error: invalid recipe id, %d\n", rcp_idx);
		return -EINVAL;
	}

	pr_debug("Configuring recipe %s(%u)\n", mod_rcp_to_str(rcp_idx),
		 rcp_idx);
	memset(&rcp, 0, sizeof(rcp));
	rcp.index = rcp_idx;

	/*==========================================*/
	/* copy from sce mux to dpu registers (sce) */
	/*==========================================*/
	/* Fetch SI info: L3 csum delta & L4 checksum delta */
	ret |= mod_sce_mux_to_reg_cpy(&rcp, si_l4_csum_delta,
				      SCE_SI_SEL_L4_CSUM_DELTA);

	/* Fetch SI info: total length diff, tos, and new header csum */
	ret |= mod_sce_mux_to_reg_cpy(&rcp, si_l3_tot_len_diff,
				      SCE_SI_SEL_TOT_LEN_DIFF);

	/* Fetch SI info: ttl, pppoe diff and pppoe length fields */
	ret |= mod_sce_mux_to_reg_cpy(&rcp, si_l3_ttl_diff, SCE_SI_SEL_TTL);

	/* Fetch SI info: L4 ports and l4 csum zero */
	if (l4_napt) {
		ret |= mod_sce_mux_to_reg_cpy(&rcp, si_l4_ports,
					      SCE_SI_SEL_NEW_SRC_PORT);
		ret |= mod_sce_mux_to_reg_cpy(&rcp, si_l4_csum_zero,
					      SCE_SI_SEL_L4_CSUM_ZERO);
	}

	/*===========================================*/
	/* copy from packet to dpu registers (store) */
	/*===========================================*/
	if (nhdr != RCP_NEW_HDR_NO_IP || pppoe_encap) {
		/* store L3 total_len */
		/* @TODO: can't store total len and id fields both in a
		 * single command since the next command should store
		 * the ttl field and the 'stall' functionality is not
		 * working as expected
		 */
		ret |= mod_pkt_to_reg_cpy(&rcp, pkt_l3_tot_len, l3_off, 0, 4);
	}

	/* store L3 TTL and L3 checksum */
	ret |= mod_pkt_to_reg_cpy(&rcp, pkt_l3_csum, l3_off,
				  offsetof(struct iphdr, ttl),
				  FIELD_SIZEOF(struct iphdr, ttl) +
				  FIELD_SIZEOF(struct iphdr, protocol) +
				  FIELD_SIZEOF(struct iphdr, check));

	/* store L4 checksum */
	if (l4_napt) {
		ret |= mod_pkt_to_reg_cpy(&rcp, pkt_l4_csum,
					  STW_NEXT_LAYER(l3_off),
					  MOD_SI_PKT_OFF,
					  FIELD_SIZEOF(struct udphdr, check));
	}

	/*=========================*/
	/*      ALU operations     */
	/*=========================*/

	/*  |-----------------------------------|------------------------|
	 *  |      ALU0                         |      ALU1              |
	 *  |-----------------------------------|------------------------|
	 *  | sync to L3 csum                   |                        |
	 *  | TTL decrement                     | sync to L4 dport (napt)|
	 *  | nhdr tot len (nhdr or pppoe_encap)| pkt l3 csum + si delta |
	 *  | pppoe pkt len (pppoe_encap)       | pkt l4 csum step 1     |
	 *  | ID inc (nhdr v4)                  | pkt l4 csum step 2     |
	 *  | csum ID (nhdr v4)                 |                        |
	 *  | csum tot len (nhdr v4)            |                        |
	 *  |-----------------------------------|------------------------|
	 */

	/* Synchronize ALU0 to latest fetched information of STORE command */
	ret |= mod_alu_synch(EU_ALU0, &rcp, l3_off,
			     offsetof(struct iphdr, check),
			     FIELD_SIZEOF(struct iphdr, check));
	if (l4_napt) {
		/* Synchronize ALU1 to L4 checksum field,
		 * The offset value is taken from the SI and it will be
		 * Different value for UDP and TCP
		 */
		ret |= mod_alu_synch(EU_ALU1, &rcp, STW_NEXT_LAYER(l3_off),
				     MOD_SI_PKT_OFF,
				     FIELD_SIZEOF(struct udphdr, check));
	} else {
		ret |= mod_alu_synch(EU_ALU1, &rcp, l3_off,
				     offsetof(struct iphdr, check),
				     FIELD_SIZEOF(struct iphdr, check));
	}

	/* decrement TTL */
	ret |= mod_alu_sub(EU_ALU0, &rcp, pkt_l3_ttl, si_l3_ttl_diff,
			   pkt_l3_ttl, FIELD_SIZEOF(struct iphdr, ttl));

	/* Recalculate L3 checksum */
	ret |= mod_alu_csum16_add(EU_ALU1, &rcp, si_l3_csum_delta, pkt_l3_csum,
				  pkt_l3_csum);

	/* Calculate new header total length */
	if (nhdr != RCP_NEW_HDR_NO_IP || pppoe_encap) {
		ret |= mod_alu_add(EU_ALU0, &rcp, si_l3_tot_len_diff, 1,
				   pkt_l3_tot_len, 2, pkt_l3_tot_len);

		/* pppoe_len = pppoe_diff ? pppoe_diff + l3 len : l2_org_val */
		if (pppoe_encap) {
			ret |= mod_alu_add_cond(EU_ALU0, &rcp, pppoe_diff, 1,
						pkt_l3_tot_len, 2, pppoe_len,
						EU_ALU_L0_BIG_R2,
						EU_ALU_COND_MET);
		}
	}

	if (nhdr == RCP_NEW_HDR_V4) {
		/* Increment the global id */
		ret |= mod_alu_inc(EU_ALU0, &rcp, MOD_GLB_EXT_IPV4_ID,
				   MOD_GLB_EXT_IPV4_ID);

		/* Update the external L3 csum due to global id */
		ret |= mod_alu_csum16_sub(EU_ALU0, &rcp, MOD_GLB_EXT_IPV4_ID,
					  si_nhdr_csum, si_nhdr_csum);

		/* Update the external L3 csum due to total len */
		ret |= mod_alu_csum16_sub(EU_ALU0, &rcp, pkt_l3_tot_len,
					  si_nhdr_csum, si_nhdr_csum);
	}

	/* Recalculate L4 checksum */
	if (l4_napt) {
		/* in general, the checksum calculation should be as follow:
		 *   udp: csum = csum ? csum + csum_delta : csum
		 *   tcp: csum = csum + csum_delta
		 * instead, we have csum_zero value which is set as follow
		 *   udp: csum_zero = csum_delta
		 *   tcp: csum_zero = 0
		 * and the calculation is as follow for both:
		 *   csum = csum + csum_delta
		 *   csum = (csum - csum_zero) ? csum : csum - csum_zero;
		 */
		ret |= mod_alu_csum16_add(EU_ALU1, &rcp, si_l4_csum_delta,
					  pkt_l4_csum, pkt_l4_csum);
		ret |= mod_alu_csum16_sub_cond(EU_ALU1, &rcp, si_l4_csum_zero,
					       pkt_l4_csum, pkt_l4_csum,
					       EU_ALU_RES_EQ_R2,
					       EU_ALU_COND_MET);
	}

	/*==========================================*/
	/* copy from dpu registers to packet (reps) */
	/*==========================================*/
	/* Replace L3 TOS */
	ret |= mod_reg_to_hdr_cpy(&rcp, si_l3_tos, l3_off,
				  offsetof(struct iphdr, tos),
				  FIELD_SIZEOF(struct iphdr, tos));

	/* Replace L3 TTL */
	ret |= mod_reg_to_hdr_cpy(&rcp, pkt_l3_ttl, l3_off,
				  offsetof(struct iphdr, ttl),
				  FIELD_SIZEOF(struct iphdr, ttl));

	/* Replace L3 CSUM */
	ret |= mod_reg_to_hdr_cpy(&rcp, pkt_l3_csum, l3_off,
				  offsetof(struct iphdr, check),
				  FIELD_SIZEOF(struct iphdr, check));

	if (l4_napt) {
		/* Replace L4 PORTS */
		ret |= mod_reg_to_hdr_cpy(&rcp, si_l4_ports,
					  STW_NEXT_LAYER(l3_off),
					  offsetof(struct udphdr, source),
					  FIELD_SIZEOF(struct udphdr, source) +
					  FIELD_SIZEOF(struct udphdr, dest));

		/* Replace L4 CSUM */
		ret |= mod_reg_to_hdr_cpy(&rcp, pkt_l4_csum,
					  STW_NEXT_LAYER(l3_off),
					  MOD_SI_PKT_OFF,
					  FIELD_SIZEOF(struct udphdr, check));
	}

	/*============================================*/
	/* copy from si bce (sram) to packet (insrep) */
	/*============================================*/
	/* replace old L2 with new header */
	ret |= mod_hdr_rplc(&rcp, BCE_OFF_16, l3_off);

	/* replace old L3 IPs with new IPs */
	ret |= mod_bce_ip_nat_cpy(&rcp, BCE_OFF_0, l3_off);

	/*======================================================*/
	/* copy from dpu registers to packet new header (reps1) */
	/*======================================================*/
	if (pppoe_encap)
		ret |= mod_reg_to_new_hdr_cpy(&rcp, pppoe_len,
					      EU_SI_SEL_PPPOE_OFF,
					      offsetof(struct pppoe_hdr,
						       length),
					      FIELD_SIZEOF(struct pppoe_hdr,
							   length));

	if (nhdr == RCP_NEW_HDR_V6) {
		/* Replace L3 payload length with inner total length */
		ret |= mod_reg_to_new_hdr_cpy(&rcp, pkt_l3_tot_len,
					      MOD_PKT_OFF_L3_EXT,
					      offsetof(struct ipv6hdr,
						       payload_len),
					      FIELD_SIZEOF(struct ipv6hdr,
							   payload_len));

	} else if (nhdr == RCP_NEW_HDR_V4) {
		/* Update ipv4 id */
		ret |= mod_reg_to_new_hdr_cpy(&rcp, MOD_GLB_EXT_IPV4_ID,
					      MOD_PKT_OFF_L3_EXT,
					      offsetof(struct iphdr, id),
					      FIELD_SIZEOF(struct iphdr, id));

		/* Update ipv4 total length */
		ret |= mod_reg_to_new_hdr_cpy(&rcp, pkt_l3_tot_len,
					      MOD_PKT_OFF_L3_EXT,
					      offsetof(struct iphdr, tot_len),
					      FIELD_SIZEOF(struct iphdr,
							   tot_len));

		/* Replace L3 CSUM */
		ret |= mod_reg_to_new_hdr_cpy(&rcp, si_nhdr_csum,
					      MOD_PKT_OFF_L3_EXT,
					      offsetof(struct iphdr, check),
					      FIELD_SIZEOF(struct iphdr,
							   check));
	}

	/*=================================================*/
	/* complete recipe with nop command and 'last' bit */
	/*=================================================*/
	ret |= mod_rcp_complete(&rcp);
	if (unlikely(ret)) {
		pr_err("Failed to create recipe %s(%d)\n",
		       mod_rcp_to_str(rcp_idx), rcp_idx);
	}

	return ret;
}

/**
 * @brief Creates recipe for ipv6 routed traffic
 * @param rcp_idx Recipe index to create
 * @param l3_off Offset to the L3 layer in the status word
 *               where HOP_LIMIT should be performed
 * @param nhdr New IP header. NO_IP for flows without encapsulation
 * @param pppoe_encap specify if pppoe encapsulation should be supported
 * @return s32 0 on success, error code otherwise
 */
static s32 mod_rcp_ipv6_routed_set(s32 rcp_idx, enum mod_eu_stw_mux_sel l3_off,
				   enum rcp_new_hdr_ip_type nhdr,
				   bool pppoe_encap)
{
	struct mod_recipe rcp;
	s32 ret = 0;

	/*===================*/
	/* Map dpu registers */
	/*===================*/
	s32 si_ttl_diff     = DPU_RCP_REG0_B0;
	s32 pppoe_diff      = DPU_RCP_REG0_B1;
	s32 pppoe_len       = DPU_RCP_REG0_B2;
	s32 si_tot_len_diff = DPU_RCP_REG1_B0;
	s32 si_nhdr_csum    = DPU_RCP_REG1_B2;
	s32 pkt_hop_limit   = DPU_RCP_REG2_B0;
	/* s32 pkt_nexthdr       = DPU_RCP_REG2_B1; */
	s32 pkt_payload_len = DPU_RCP_REG2_B2;
	s32 ext_l3_hdr_len  = DPU_RCP_REG3_B3;
	s32 stw_ext_l3_off  = DPU_RCP_REG5_B0;
	s32 stw_int_l3_off  = DPU_RCP_REG5_B1;
	// s32 stw_ext_l4_off  = DPU_RCP_REG5_B2;
	s32 stw_int_l4_off  = DPU_RCP_REG5_B3;
	/* DO NOT USE registers R12 and R14 as they are used as R2 for
	 * the alu conditions and MUST be zero
	 */

	if (unlikely(rcp_idx > MOD_RCP_LAST)) {
		pr_err("error: invalid recipe id, %d\n", rcp_idx);
		return -EINVAL;
	}

	pr_debug("Configuring recipe %s(%u)\n", mod_rcp_to_str(rcp_idx),
		 rcp_idx);
	memset(&rcp, 0, sizeof(rcp));
	rcp.index = rcp_idx;

	/*==========================================*/
	/* copy from sce mux to dpu registers (sce) */
	/*==========================================*/
	/* Fetch SI info: total length diff, tos, and new header csum */
	if (nhdr != RCP_NEW_HDR_NO_IP)
		ret |= mod_sce_mux_to_reg_cpy(&rcp, si_tot_len_diff,
					      SCE_SI_SEL_TOT_LEN_DIFF);

	/* Fetch SI info: hop limit diff, pppoe diff and pppoe length fields */
	ret |= mod_sce_mux_to_reg_cpy(&rcp, si_ttl_diff, SCE_SI_SEL_TTL);

	/* Fetch STW info: l3 and l4 offsets */
	if (pppoe_encap)
		ret |= mod_sce_mux_to_reg_cpy(&rcp, stw_ext_l3_off,
					      SCE_STW_SEL_EXT_L3_OFF);

	/*===========================================*/
	/* copy from packet to dpu registers (store) */
	/*===========================================*/
	/* store hop limit, next header (not used), and payload length
	 * we always fetch all 3 cause we anyway need the hop limit
	 */
	ret |= mod_pkt_to_reg_cpy(&rcp, pkt_hop_limit, l3_off,
				  offsetof(struct ipv6hdr, payload_len),
				  FIELD_SIZEOF(struct ipv6hdr, payload_len) +
				  FIELD_SIZEOF(struct ipv6hdr, nexthdr) +
				  FIELD_SIZEOF(struct ipv6hdr, hop_limit));

	/*=========================*/
	/*      ALU operations     */
	/*=========================*/
	/*  |------------------------------- |------------------------------|
	 *  |      ALU0                      |      ALU1                    |
	 *  |------------------------------- |------------------------------|
	 *  | sync                           |                              |
	 *  | HOPL decrement                 | sync (nhdr or pppoe)         |
	 *  | pkt v6 hdr len (nhdr or pppoe) | nhdr tot len (nhdr or pppoe) |
	 *  | ID inc (nhdr v4)               | nhdr pkt len (nhdr or pppoe) |
	 *  | csum ID (nhdr v4)              | pppoe len (nhdr or pppoe)    |
	 *  | csum pkt len (nhdr v4)         |                              |
	 *  |------------------------        |------------------------------|
	 */
	/* Synchronize ALU to latest fetched information of STORE command */
	ret |= mod_alu_synch(EU_ALU0, &rcp, l3_off,
			     offsetof(struct ipv6hdr, hop_limit),
			     FIELD_SIZEOF(struct ipv6hdr, hop_limit));

	if (nhdr != RCP_NEW_HDR_NO_IP || pppoe_encap)
		ret |= mod_alu_synch(EU_ALU1, &rcp, l3_off,
				     offsetof(struct ipv6hdr, hop_limit),
				     FIELD_SIZEOF(struct ipv6hdr, hop_limit));

	/* decrement hop limit */
	ret |= mod_alu_sub(EU_ALU0, &rcp, pkt_hop_limit, si_ttl_diff,
			   pkt_hop_limit,
			   FIELD_SIZEOF(struct ipv6hdr, hop_limit));

	/* Calculate new header total and/or pppoe length */
	if (nhdr != RCP_NEW_HDR_NO_IP || pppoe_encap) {
		/* add session total len diff */
		ret |= mod_alu_add(EU_ALU1, &rcp, si_tot_len_diff, 1,
				   pkt_payload_len, 2, pkt_payload_len);

		/* calc the inner v6 header length */
		ret |= mod_alu_sub(EU_ALU0, &rcp, stw_int_l4_off,
				   stw_int_l3_off, ext_l3_hdr_len, 1);

		/* add the inner v6 header length */
		ret |= mod_alu_add(EU_ALU1, &rcp, pkt_payload_len, 2,
				   ext_l3_hdr_len, 1, pkt_payload_len);

		/* pppoe_len = pppoe_diff ? pppoe_diff + l3 len : l2_org_val */
		if (pppoe_encap)
			ret |= mod_alu_add_cond(EU_ALU1, &rcp, pppoe_diff, 1,
						pkt_payload_len, 2, pppoe_len,
						EU_ALU_L0_BIG_R2,
						EU_ALU_COND_MET);
	}

	/* for V4 update csum, id */
	if (nhdr == RCP_NEW_HDR_V4) {
		/* Increment the global id */
		ret |= mod_alu_inc(EU_ALU0, &rcp, MOD_GLB_EXT_IPV4_ID,
				   MOD_GLB_EXT_IPV4_ID);

		/* Update the external L3 csum due to global id */
		ret |= mod_alu_csum16_sub(EU_ALU0, &rcp, MOD_GLB_EXT_IPV4_ID,
					si_nhdr_csum, si_nhdr_csum);

		/* Calculate external L3 csum due to total len.
		 * this MUST be in sync with ALU1 operations
		 * Note: This assumes the total length in the new ipv4
		 *       template header is zero
		 */
		ret |= mod_alu_csum16_sub(EU_ALU0, &rcp, pkt_payload_len,
					si_nhdr_csum, si_nhdr_csum);
	}

	/*==========================================*/
	/* copy from dpu registers to packet (reps) */
	/*==========================================*/
	/* Replace L3 hop limit */
	ret |= mod_reg_to_hdr_cpy(&rcp, pkt_hop_limit, l3_off,
				  offsetof(struct ipv6hdr, hop_limit),
				  FIELD_SIZEOF(struct ipv6hdr, hop_limit));

	/*============================================*/
	/* copy from si bce (sram) to packet (insrep) */
	/*============================================*/
	/* replace old L2 with new header */
	ret |= mod_hdr_rplc(&rcp, BCE_OFF_0, l3_off);

	/*======================================================*/
	/* copy from dpu registers to packet new header (reps1) */
	/*======================================================*/
	if (pppoe_encap)
		ret |= mod_reg_to_new_hdr_cpy(&rcp, pppoe_len,
					      EU_SI_SEL_PPPOE_OFF,
					      offsetof(struct pppoe_hdr,
						       length),
					      FIELD_SIZEOF(struct pppoe_hdr,
							   length));

	if (nhdr == RCP_NEW_HDR_V6) {
		/* Replace payload length with inner payload length */
		ret |= mod_reg_to_new_hdr_cpy(&rcp, pkt_payload_len,
					      MOD_PKT_OFF_L3_EXT,
					      offsetof(struct ipv6hdr,
						       payload_len),
					      FIELD_SIZEOF(struct ipv6hdr,
							   payload_len));

	} else if (nhdr == RCP_NEW_HDR_V4) {
		/* Update ipv4 id */
		ret |= mod_reg_to_new_hdr_cpy(&rcp, MOD_GLB_EXT_IPV4_ID,
					      MOD_PKT_OFF_L3_EXT,
					      offsetof(struct iphdr, id),
					      FIELD_SIZEOF(struct iphdr, id));

		/* Update ipv4 total length */
		ret |= mod_reg_to_new_hdr_cpy(&rcp, pkt_payload_len,
					      MOD_PKT_OFF_L3_EXT,
					      offsetof(struct iphdr, tot_len),
					      FIELD_SIZEOF(struct iphdr,
							   tot_len));

		/* Replace L3 CSUM */
		ret |= mod_reg_to_new_hdr_cpy(&rcp, si_nhdr_csum,
					      MOD_PKT_OFF_L3_EXT,
					      offsetof(struct iphdr, check),
					      FIELD_SIZEOF(struct iphdr,
							   check));
	}

	/*=================================================*/
	/* complete recipe with nop command and 'last' bit */
	/*=================================================*/
	ret |= mod_rcp_complete(&rcp);
	if (unlikely(ret)) {
		pr_err("Failed to create recipe %s(%d)\n",
		       mod_rcp_to_str(rcp_idx), rcp_idx);
	}

	return ret;
}

/**
 * @brief Creates recipe for esp traffic
 * @param rcp_idx Recipe index to create
 * @param nhdr New IP header. NO_IP for flows without encapsulation
 * @return s32 0 on success, error code otherwise
 */
static s32 mod_rcp_esp_set(s32 rcp_idx, enum rcp_new_hdr_ip_type nhdr)
{
	struct mod_recipe rcp;
	s32 ret = 0;
	enum mod_eu_stw_mux_sel l3_off = EU_STW_SEL_L3_OFF0;

	/* ================= */
	/* Map dpu registers */
	/* ================= */
	s32 stw_pkt_len        = DPU_RCP_REG0_B0;
	s32 stw_off_l3         = DPU_RCP_REG1_B0;
	s32 si_l3_tot_len_diff = DPU_RCP_REG2_B0;
	s32 si_nhdr_csum       = DPU_RCP_REG2_B2;
	s32 pkt_l3_csum        = DPU_RCP_REG3_B0;
	s32 pkt_l3_tot_len     = DPU_RCP_REG4_B0;

	if (unlikely(rcp_idx > MOD_RCP_LAST)) {
		pr_err("error: invalid recipe id, %d\n", rcp_idx);
		return -EINVAL;
	}

	pr_debug("Configuring recipe %s(%u)\n", mod_rcp_to_str(rcp_idx),
		 rcp_idx);
	memset(&rcp, 0, sizeof(rcp));
	rcp.index = rcp_idx;

	/*==========================================*/
	/* copy from sce mux to dpu registers (sce) */
	/*==========================================*/
	/*======================*/
	/* STW info: packet len */
	/*======================*/
	ret |= mod_sce_mux_to_reg_cpy(&rcp, stw_pkt_len,
				      SCE_STW_SEL_PKT_LEN);
	/*======================*/
	/* STW info: offset L3  */
	/*======================*/
	ret |= mod_sce_mux_to_reg_cpy(&rcp, stw_off_l3,
				      SCE_STW_SEL_L3_OFF0);
	/*========================*/
	/* SI info: tot_len_diff  */
	/*========================*/
	ret |= mod_sce_mux_to_reg_cpy(&rcp, si_l3_tot_len_diff,
				      SCE_SI_SEL_TOT_LEN_DIFF);

	/*===========================================*/
	/* copy from packet to dpu registers (store) */
	/*===========================================*/
	if (nhdr == RCP_NEW_HDR_NO_IP) {
		/*====================*/
		/* PKT: L3 total_len  */
		/*====================*/
		ret |= mod_pkt_to_reg_cpy(&rcp, pkt_l3_tot_len, l3_off,
					  offsetof(struct iphdr, tot_len),
					  FIELD_SIZEOF(struct iphdr, tot_len));
		/*===================*/
		/* PKT: L3 checksum  */
		/*===================*/
		ret |= mod_pkt_to_reg_cpy(&rcp, pkt_l3_csum, l3_off,
					  offsetof(struct iphdr, check),
					  FIELD_SIZEOF(struct iphdr, check));
	}

	/*=========================*/
	/*      ALU operations     */
	/*=========================*/
	/*  |--------------------------------|--------------------------------|
	 *  |  ALU0 for RCP_NEW_HDR_NO_IP    |  ALU0 for RCP_NEW_HDR_V4       |
	 *  |--------------------------------|--------------------------------|
	 *  | sync                           | off_l3 = pkt_len - off_l3      |
	 *  | off_l3 = pkt_len - off_l3      | pkt_len = off_l3 + tot_len_diff|
	 *  | pkt_len = off_l3 + tot_len_diff| inc global_id                  |
	 *  | csum += old total length       | csum -= global_id              |
	 *  | csum -= pkt_len (new tot_len)  | csum -= pkt_len (new tot_len)  |
	 *  |                                |                                |
	 *  |--------------------------------|--------------------------------|
	 */
	if (nhdr == RCP_NEW_HDR_NO_IP) {
		/* Synchronize ALU to last fetched info of STORE command */
		ret |= mod_alu_synch(EU_ALU0, &rcp, l3_off,
				     offsetof(struct iphdr, check),
				     FIELD_SIZEOF(struct iphdr, check));
	}
	/*=======================================*/
	/* stw_off_l3 = stw_pkt_len - stw_off_l3 */
	/*=======================================*/
	ret |= mod_alu_sub(EU_ALU0, &rcp, stw_pkt_len, stw_off_l3,
			   stw_off_l3, 2);
	/*===============================================*/
	/* stw_pkt_len = stw_off_l3 - si_l3_tot_len_diff */
	/*===============================================*/
	ret |= mod_alu_add(EU_ALU0, &rcp, stw_off_l3, 2,
			   si_l3_tot_len_diff, 1, stw_pkt_len);
	if (nhdr == RCP_NEW_HDR_V4) {
		/*=======================*/
		/* MOD_GLB_EXT_IPV4_ID++ */
		/*=======================*/
		ret |= mod_alu_inc(EU_ALU0, &rcp, MOD_GLB_EXT_IPV4_ID,
				   MOD_GLB_EXT_IPV4_ID);
		/*=====================================*/
		/* si_nhdr_csum -= MOD_GLB_EXT_IPV4_ID */
		/*=====================================*/
		ret |= mod_alu_csum16_sub(EU_ALU0, &rcp, MOD_GLB_EXT_IPV4_ID,
					  si_nhdr_csum, si_nhdr_csum);
		/*=============================*/
		/* si_nhdr_csum -= stw_pkt_len */
		/*=============================*/
		ret |= mod_alu_csum16_sub(EU_ALU0, &rcp, stw_pkt_len,
					  si_nhdr_csum, si_nhdr_csum);
	} else if (nhdr == RCP_NEW_HDR_NO_IP) {
		/*===============================*/
		/* pkt_l3_csum += pkt_l3_tot_len */
		/*===============================*/
		ret |= mod_alu_csum16_add(EU_ALU0, &rcp, pkt_l3_tot_len,
					  pkt_l3_csum, pkt_l3_csum);
		/*============================*/
		/* pkt_l3_csum -= stw_pkt_len */
		/*============================*/
		ret |= mod_alu_csum16_sub(EU_ALU0, &rcp, stw_pkt_len,
					  pkt_l3_csum, pkt_l3_csum);
	}

	/*==========================================*/
	/* copy from dpu registers to packet (reps0) */
	/*==========================================*/
	if (nhdr == RCP_NEW_HDR_NO_IP) {
		/* Replace L3 total length */
		ret |= mod_reg_to_hdr_cpy(&rcp, stw_pkt_len, l3_off,
					  offsetof(struct iphdr, tot_len),
					  FIELD_SIZEOF(struct iphdr, tot_len));
		/* Replace L3 CSUM */
		ret |= mod_reg_to_hdr_cpy(&rcp, pkt_l3_csum, l3_off,
					  offsetof(struct iphdr, check),
					  FIELD_SIZEOF(struct iphdr, check));
	}

	/*============================================*/
	/* copy from si bce (sram) to packet (insrep) */
	/*============================================*/
	/* replace old L2 with new header */
	ret |= mod_hdr_rplc(&rcp, BCE_OFF_0, l3_off);

	/*======================================================*/
	/* copy from dpu registers to packet new header (reps1) */
	/*======================================================*/
	if (nhdr == RCP_NEW_HDR_V4) {
		/* Update ipv4 id */
		ret |= mod_reg_to_new_hdr_cpy(&rcp, MOD_GLB_EXT_IPV4_ID,
					      MOD_PKT_OFF_L3_EXT,
					      offsetof(struct iphdr, id),
					      FIELD_SIZEOF(struct iphdr, id));

		/* Update ipv4 total length */
		ret |= mod_reg_to_new_hdr_cpy(&rcp, stw_pkt_len,
					      MOD_PKT_OFF_L3_EXT,
					      offsetof(struct iphdr, tot_len),
					      FIELD_SIZEOF(struct iphdr,
							   tot_len));

		/* Replace L3 CSUM */
		ret |= mod_reg_to_new_hdr_cpy(&rcp, si_nhdr_csum,
					      MOD_PKT_OFF_L3_EXT,
					      offsetof(struct iphdr, check),
					      FIELD_SIZEOF(struct iphdr,
							   check));
	}

	/*=================================================*/
	/* complete recipe with nop command and 'last' bit */
	/*=================================================*/
	ret |= mod_rcp_complete(&rcp);
	if (unlikely(ret)) {
		pr_err("Failed to create recipe %s(%d)\n",
		       mod_rcp_to_str(rcp_idx), rcp_idx);
	}

	return ret;
}

s32 mod_rcp_init(void)
{
	s32 ret;

	/* config no-modification recipe */
	ret = mod_rcp_no_mod_set(MOD_RCP_NO_MOD);
	if (unlikely(ret))
		goto err;

	/* config recipe L2 replacing only */
	ret = mod_rcp_hdr_rplc_set(MOD_RCP_HDR_RPLC, true, EU_STW_SEL_L3_OFF0);
	if (unlikely(ret))
		goto err;

	/* config recipe IPV4 NAT without L4 modification
	 * [l2][ipv4][l4] => ipv4 layer offset is first
	 */
	ret = mod_rcp_ipv4_routed_set(MOD_RCP_IPV4_NAT, EU_STW_SEL_L3_OFF0,
				      false, RCP_NEW_HDR_NO_IP, true);
	if (unlikely(ret))
		goto err;

	/* config recipe IPV4 NAT with UDP modification
	 * [l2][ipv4][tcp/udp] => ipv4 layer offset is first
	 * for tcp/udp we also specify we need l4 nat
	 */
	ret = mod_rcp_ipv4_routed_set(MOD_RCP_IPV4_NAPT, EU_STW_SEL_L3_OFF0,
				      true, RCP_NEW_HDR_NO_IP, true);
	if (unlikely(ret))
		goto err;

	/* config recipe IPV6 decrement hop limit modification */
	ret = mod_rcp_ipv6_routed_set(MOD_RCP_IPV6_ROUTED, EU_STW_SEL_L3_OFF0,
				      RCP_NEW_HDR_NO_IP, true);
	if (unlikely(ret))
		goto err;

	/* config recipe tunnel v4 in v4 encapsulation */
	ret = mod_rcp_ipv4_routed_set(MOD_RCP_IPV4_IPV4_ENCAP,
				      EU_STW_SEL_L3_OFF0, true, RCP_NEW_HDR_V4,
				      true);
	if (unlikely(ret))
		goto err;

	/* config recipe tunnel v4 in v6 encapsulation with nat
	 * changes in the inner v4 and udp/tcp
	 */
	ret = mod_rcp_ipv4_routed_set(MOD_RCP_IPV6_IPV4_ENCAP,
				      EU_STW_SEL_L3_OFF0, true, RCP_NEW_HDR_V6,
				      true);
	if (unlikely(ret))
		goto err;

	/* config recipe tunnel v6 in v6 encapsulation with inner
	 * routed ipv6 and udp/tcp
	 */
	ret = mod_rcp_ipv6_routed_set(MOD_RCP_IPV6_IPV6_ENCAP,
				      EU_STW_SEL_L3_OFF0, RCP_NEW_HDR_V6, true);
	if (unlikely(ret))
		goto err;

	/* config recipe tunnel v6 in v4 encapsulation */
	ret = mod_rcp_ipv6_routed_set(MOD_RCP_IPV4_IPV6_ENCAP,
				      EU_STW_SEL_L3_OFF0, RCP_NEW_HDR_V4, true);
	if (unlikely(ret))
		goto err;

	/* config recipe eogre v4 in v6/v4 decapsulation with nat
	 * changes in the inner v4 and udp/tcp
	 * [l2][ipv4/6][gre][l2][ipv4][tcp/udp] => inner ipv4 offset is fourth
	 * for tcp/udp we also specify we need l4 nat
	 */
	ret = mod_rcp_ipv4_routed_set(MOD_RCP_EOGRE_DECP_IPV4_NAPT,
				      EU_STW_SEL_L3_OFF3, true,
				      RCP_NEW_HDR_NO_IP, false);
	if (unlikely(ret))
		goto err;

	/* config recipe ipogre v4 in v6/v4 decapsulation with nat
	 * changes in the inner v4 and udp/tcp
	 * [l2][ipv4/6][gre][ipv4][tcp/udp] => inner ipv4 offset is third
	 * for tcp/udp we also specify we need l4 nat
	 */
	ret = mod_rcp_ipv4_routed_set(MOD_RCP_IPOGRE_DECP_IPV4_NAPT,
				      EU_STW_SEL_L3_OFF2, true,
				      RCP_NEW_HDR_NO_IP, false);
	if (unlikely(ret))
		goto err;

	/* config recipe eogre v6 in v6/v4 decapsulation with hop limit
	 * decrement in the inner v6
	 * [l2][ipv4/6][gre][l2][ipv6][tcp/udp] => inner ipv6 offset is fourth
	 */
	ret = mod_rcp_ipv6_routed_set(MOD_RCP_EOGRE_DECP_IPV6_ROUTED,
				      EU_STW_SEL_L3_OFF3, RCP_NEW_HDR_NO_IP,
				      false);
	if (unlikely(ret))
		goto err;

	/* config recipe ipogre v6 in v6/v4 decapsulation with hop limit
	 * decrement in the inner v6
	 * [l2][ipv4/6][gre][ipv6][tcp/udp] => inner ipv6 offset is third
	 */
	ret = mod_rcp_ipv6_routed_set(MOD_RCP_IPOGRE_DECP_IPV6_ROUTED,
				      EU_STW_SEL_L3_OFF2, RCP_NEW_HDR_NO_IP,
				      false);
	if (unlikely(ret))
		goto err;

	/* config recipe dslite decapsulation with nat
	 * changes in the inner L3 and udp/tcp
	 * [l2][ipv6][ipv4][tcp/udp] => inner L3 offset is second
	 * for tcp/udp we also specify we need l4 nat
	 */
	ret = mod_rcp_ipv4_routed_set(MOD_RCP_DSLITE_DECP_NAPT,
				      EU_STW_SEL_L3_OFF1, true,
				      RCP_NEW_HDR_NO_IP, false);
	if (unlikely(ret))
		goto err;

	/* config recipe 6RD decapsulation
	 * [l2][ipv4][ipv6][tcp/udp] => inner L3 offset is second
	 * for tcp/udp we also specify we need l4 nat
	 */
	ret = mod_rcp_ipv6_routed_set(MOD_RCP_6RD_DECP_ROUTED,
				      EU_STW_SEL_L3_OFF1, RCP_NEW_HDR_NO_IP,
				      false);
	if (unlikely(ret))
		goto err;

	/* config recipe ESP over ipv4 encapsulation */
	ret = mod_rcp_esp_set(MOD_RCP_ESP_V4_TUNNL_ENCAP, RCP_NEW_HDR_V4);
	if (unlikely(ret))
		goto err;

	/* config recipe ESP over ipv4 bypass */
	ret = mod_rcp_esp_set(MOD_RCP_ESP_V4_TUNNL_BPAS, RCP_NEW_HDR_NO_IP);
	if (unlikely(ret))
		goto err;

	/* recipe for esp w/o outer ip header at egress */
	ret = mod_rcp_hdr_rplc_set(MOD_RCP_ESP_OUTER_IP_REM, false,
				   EU_STW_SEL_L3_OFF1);
	if (unlikely(ret))
		goto err;

	/* config recipe L2TP UDP in v6/v4 decapsulation with nat
	 * changes in the inner v4 and udp/tcp
	 * [l2][ipv4/6][udp][l2tp][ppp][ipv4][tcp/udp] => inner ipv4
	 * offset is fourth. For tcp/udp we also specify we need l4 nat
	 */
	ret = mod_rcp_ipv4_routed_set(MOD_RCP_L2TPUDP_DECP_IPV4_NAPT,
				      EU_STW_SEL_L3_OFF3, true,
				      RCP_NEW_HDR_NO_IP, false);
	if (unlikely(ret))
		goto err;

	/* config recipe L2TP UDP in v6/v4 decapsulation with hop limit
	 * decrement in the inner v6
	 * [l2][ipv4/6][udp][l2tp][ppp][ipv6][tcp/udp] => inner ipv6
	 * offset is fourth
	 */
	ret = mod_rcp_ipv6_routed_set(MOD_RCP_L2TPUDP_DECP_IPV6_ROUTED,
				      EU_STW_SEL_L3_OFF3, RCP_NEW_HDR_NO_IP,
				      false);
	if (unlikely(ret))
		goto err;

	/* config recipe L2TP IP in v6/v4 decapsulation with nat
	 * changes in the inner v4 and udp/tcp
	 * [l2][ipv4/6][l2tp][ppp][ipv4][tcp/udp] => inner ipv4
	 * offset is third. For tcp/udp we also specify we need l4 nat
	 */
	ret = mod_rcp_ipv4_routed_set(MOD_RCP_L2TPIP_DECP_IPV4_NAPT,
				      EU_STW_SEL_L3_OFF2, true,
				      RCP_NEW_HDR_NO_IP, false);
	if (unlikely(ret))
		goto err;

	/* config recipe L2TP IP in v6/v4 decapsulation with hop limit
	 * decrement in the inner v6
	 * [l2][ipv4/6][l2tp][ppp][ipv6][tcp/udp] => inner ipv6
	 * offset is third
	 */
	ret = mod_rcp_ipv6_routed_set(MOD_RCP_L2TPIP_DECP_IPV6_ROUTED,
				      EU_STW_SEL_L3_OFF2, RCP_NEW_HDR_NO_IP,
				      false);
	if (unlikely(ret))
		goto err;

	return 0;

err:
	pr_err("failed to init modifier recipes\n");
	return ret;
}
