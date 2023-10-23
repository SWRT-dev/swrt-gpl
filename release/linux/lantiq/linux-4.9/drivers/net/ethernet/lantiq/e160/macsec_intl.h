// SPDX-License-Identifier: (BSD-3-Clause OR GPL-2.0-only)
/*
 * Driver for MACsec Engine inside Intel CHD GSWIP or GPHY
 *
 * Copyright 2019 - 2020 Intel Corporation
 */

#ifndef _E160_MACSEC_INTL_H_
#define _E160_MACSEC_INTL_H_

/* Egress engine registers */
#include "registers/eip160s_egr_classifier_control_packet.h"
#include "registers/eip160s_egr_classifier_debug.h"
#include "registers/eip160s_egr_classifier_various_control_regs.h"
#include "registers/eip160s_egr_flow_control_page0.h"
#include "registers/eip160s_egr_oppe.h"
#include "registers/eip160s_egr_sam_enable_ctrl.h"
#include "registers/eip160s_egr_sam_rules_page0.h"
#include "registers/eip160s_egr_stat_control.h"
#include "registers/eip160s_egr_stat_global.h"
#include "registers/eip160s_egr_stat_sa_page0.h"
#include "registers/eip160s_egr_stat_vlan.h"
#include "registers/eip160s_egr_system_control.h"

/* Ingress engine registers */
#include "registers/eip160s_ing_classifier_control_packet.h"
#include "registers/eip160s_ing_classifier_debug.h"
#include "registers/eip160s_ing_classifier_various_control_regs.h"
#include "registers/eip160s_ing_flow_control_page0.h"
#include "registers/eip160s_ing_oppe.h"
#include "registers/eip160s_ing_sam_enable_ctrl.h"
#include "registers/eip160s_ing_sam_rules_page0.h"
#include "registers/eip160s_ing_stat_control.h"
#include "registers/eip160s_ing_stat_global.h"
#include "registers/eip160s_ing_stat_sa_page0.h"
#include "registers/eip160s_ing_stat_vlan.h"
#include "registers/eip160s_ing_system_control.h"

/* Advanced Interrupt Controller (AIC) engine registers */
#include "registers/eip201_egr_aic_registers.h"
#include "registers/eip201_ing_aic_registers.h"

/* Crypto Core engine registers */
#include "registers/eip62_egr_ccore_registers.h"
#include "registers/eip62_ing_ccore_registers.h"

/* Ingress Consistency Check engine registers */
#include "registers/eip160s_ing_cc_control.h"
#include "registers/eip160s_ing_cc_rules_page0.h"

#define MAX_RW_RETRY 100
#define FIELD_GET(reg, field) (((reg) & field ## _MASK) >> field ## _POS)
#define FIELD_SET(val, field) (((val) << field ## _POS) & field ## _MASK)
#define FIELD_REPLACE(src, val, field) (src = (((src) & (~field ## _MASK)) | FIELD_SET(val, field)))

/**********************
	Constant
 **********************/

/* Max number of sync retry */
#define IN_FLIGHT_BUSY_RETRY 10

/* Ethertype for MACSec frames. */
#define ETH_PROTO_MACSEC 0x88E5
/* Ethertype for MACSec frames - byte swapped of ETH_PROTO_MACSEC */
#define BYTE_SWAP_16BIT(A) ((((A) & 0xFF00) >> 8) | (((A) & 0x00FF) << 8))
#define ETH_PROTO_MACSEC_BYTE_SWAPPED BYTE_SWAP_16BIT(ETH_PROTO_MACSEC)

/* EIP-160 Signature */
#define EIP160_NR_CNR 0x5FA0u
/* EIP-62 Signature */
#define EIP62_NR_CNR 0xC13Eu
/* Secure Channel Id size */
#define MACSEC_SCI_LEN_WORD 2

/* Key size for 256-bit CA type */
#define MACSEC_CA256_KEY_LEN_BYTE 32
/* Key size for 128-bit CA type */
#define MACSEC_CA128_KEY_LEN_WORD 4
/* Key size for 256-bit CA type */
#define MACSEC_CA256_KEY_LEN_WORD 8

/* H-Key size */
#define MACSEC_HKEY_LEN_WORD 4
/* Context Salt size */
#define MACSEC_CSALT_LEN_WORD 3

/* Base Address of EIP_160S_IG_TR_PAGE0 */
#define EIP_160S_IG_TR_PAGE0_BASE 0x00000000u
/* Base Address of EIP_160S_EG_TR_PAGE0 */
#define EIP_160S_EG_TR_PAGE0_BASE 0x00000000u

/* CCW Fields */
/* Field SN - Sequence Number */
#define CCW_EIPX_TR_CCW_SN_POS 28
/* Field SN - Sequence Number */
#define CCW_EIPX_TR_CCW_SN_MASK 0x03000000u
/* Field AN - Association Number */
#define CCW_EIPX_TR_CCW_AN_POS 26
/* Field AN - Association Number */
#define CCW_EIPX_TR_CCW_AN_MASK 0x0C000000u
/* Field CA - Crypto algorithm */
#define CCW_EIPX_TR_CCW_CA_POS 17
/* Field CA - Crypto algorithm */
#define CCW_EIPX_TR_CCW_CA_MASK 0x000E0000u

/* SA Update Control Fields */
/* Field AN - Index of the next SA.  */
#define SA_UPDATE_SA_INDEX_POS 0
/* Field AN - Index of the next SA */
#define SA_UPDATE_SA_INDEX_MASK 0x00003FFFu
/* Field AN - SA expired summary register */
#define SA_UPDATE_SA_EXPIRED_IRQ_POS 14
/* Field AN - SA expired summary register */
#define SA_UPDATE_SA_EXPIRED_IRQ_MASK 0x00004000u
/* Field AN - Next SA index valid */
#define SA_UPDATE_SA_INDEX_VALID_POS 15
/* Field AN - Next SA index valid */
#define SA_UPDATE_SA_INDEX_VALID_MASK 0x00008000u
/* Field AN - Index of the flow control that holds the current SA */
#define SA_UPDATE_FLOW_INDEX_POS 16
/* Field AN - Index of the flow control that holds the current SA */
#define SA_UPDATE_FLOW_INDEX_MASK 0x7FFF0000u
/* Field AN - SA index update enable */
#define SA_UPDATE_SA_INDEX_UPDATE_EN_POS 31
/* Field AN - SA index update enable */
#define SA_UPDATE_SA_INDEX_UPDATE_EN_MASK 0x80000000u

/* TR context_size in words */
#define ING_MAX_TR_SIZE_WORDS 20
/* TR context_size in bytes */
#define ING_MAX_TR_SIZE_BYTES (ING_MAX_TR_SIZE_WORDS * 4)

/* TR context_size in words */
#define EGR_MAX_TR_SIZE_WORDS 24
/* TR context_size in bytes */
#define EGR_MAX_TR_SIZE_BYTES (EGR_MAX_TR_SIZE_WORDS * 4)

/* SAM params size in words */
#define IE_MAX_SAM_SIZE_WORDS 16
/* SAM params size in bytes */
#define IE_MAX_SAM_SIZE_BYTES (IE_MAX_SAM_SIZE_WORDS * 4)

/* Ingress Consistency Check params size in words */
#define ICC_MAX_MATCH_SIZE_WORDS 4
/* Ingress Consistency Check params size in bytes */
#define ICC_MAX_MATCH_SIZE_BYTES (ICC_MAX_MATCH_SIZE_WORDS * 4)

/* CPC params size in bytes */
#define IE_MAX_CPC_SIZE_BYTES 16
/* CPC rule for DA + EHT size in bytes */
#define IE_CPC_DA_ETH_SIZE_BYTES 8
/* CPC rule for EHT size in bytes */
#define IE_CPC_ETH_SIZE_BYTES 4

/* SAM FCA size in bytes */
#define IE_MAX_SAM_FCA_BYTES 4

/* SA statistic size in words */
#define MAX_SA_STATS_SIZE_WORDS 32
/* SA statistic size in bytes */
#define MAX_SA_STATS_SIZE_BYTES (MAX_SA_STATS_SIZE_WORDS * 4)
/* Max Ingress SA counters */
#define MAX_ING_SA_COUNTERS 11
/* Max Egress SA counters */
#define MAX_EGR_SA_COUNTERS 4
/* Max VLAN counters for both Ingress & Egress */
#define MAX_VLAN_COUNTERS 5
/* Max Ingress GLOBAL counters */
#define MAX_ING_GLOBAL_COUNTERS 14
/* Max Egress GLOBAL counters */
#define MAX_EGR_GLOBAL_COUNTERS 5
/* Max VLAN user priorities */
#define MAX_VLAN_USER_PRIORITIES 8

/* Max entries in Control Packet Classification table */
#define MAX_CTRL_PKT_CLASS_ENTRIES 18

/* Max Counter sync retry count */
#define EIP160_MAX_SYNC_RETRY_COUNT 100

/* Ingress default CCW word for 32-bit PN & 128-bit AES type */
#define ING_32BPN_128KEY_DEF_CCW 0xD24BE06F
/* Ingress default CCW word for 32-bit PN & 256-bit AES type */
#define ING_32BPN_256KEY_DEF_CCW 0xD24FE06F
/* Ingress default CCW word for 64-bit PN & 128-bit AES type */
#define ING_64BPN_128KEY_DEF_CCW 0xE24BA0EF
/* Ingress default CCW word for 64-bit PN & 256-bit AES type */
#define ING_64BPN_256KEY_DEF_CCW 0xE24FA0EF

/* AN is variable field and should be inserted */
/* Egress default CCW word for 32-bit PN & 128-bit AES type */
#define EGR_32BPN_128KEY_DEF_CCW 0x924BE066
/* Egress default CCW word for 32-bit PN & 256-bit AES type */
#define EGR_32BPN_256KEY_DEF_CCW 0x924FE066
/* Egress default CCW word for 64-bit PN & 128-bit AES type */
#define EGR_64BPN_128KEY_DEF_CCW 0xA24BE066
/* Egress default CCW word for 64-bit PN & 256-bit AES type */
#define EGR_64BPN_256KEY_DEF_CCW 0xA24FE066

/* Default register values. */
/* Default value of SAM_NM_PARAMS reg. */
#define DEF_SAM_NM_PARAMS 0xE588007F

/* MACsec direction. */
enum msec_dir {
	/* Ingress (decryption) direction. */
	INGRESS_DIR = 0,
	/* Egress (encryption) direction. */
	EGRESS_DIR = 1,
};

/* Internal used for structure eip160_config reg. */
union _eip160_config {
	struct {
#if defined(__LITTLE_ENDIAN)
		u32 nr_of_sas: 8;
		u32 consist_checks: 8;
		u32 match_sci: 1;
		u32 reserved: 15;
#elif defined(__BIG_ENDIAN)
		u32 reserved: 15;
		u32 match_sci: 1;
		u32 consist_checks: 8;
		u32 nr_of_sas: 8;
#else
#error "__LITTLE_ENDIAN or __BIG_ENDIAN must be defined"
#endif
	};
	u32 raw_eip160_cfg;
} ;

/* Internal used for structure eip160_config2 reg. */
union _eip160_extcfg {
	struct {
#if defined(__LITTLE_ENDIAN)
		u32 sa_counters: 4;
		u32 vl_counters: 4;
		u32 global_counters: 6;
		u32 saturate_cntrs: 1;
		u32 auto_cntr_reset: 1;
		u32 sa_octet_ctrs: 2;
		u32 vl_octet_ctrs: 2;
		u32 glob_octet_ctrs: 4;
		u32 ingress_only: 1;
		u32 egress_only: 1;
		u32 reserved: 1;
		u32 tag_bypass: 1;
		u32 reserved2: 4;
#elif defined(__BIG_ENDIAN)
		u32	reserved2: 4;
		u32	tag_bypass: 1;
		u32	reserved: 1;
		u32	egress_only: 1;
		u32	ingress_only: 1;
		u32	glob_octet_ctrs: 4;
		u32	vl_octet_ctrs: 2;
		u32	sa_octet_ctrs: 2;
		u32	auto_cntr_reset: 1;
		u32	saturate_cntrs: 1;
		u32	global_counters: 6;
		u32	vl_counters: 4;
		u32	sa_counters: 4;
#else
#error "__LITTLE_ENDIAN or __BIG_ENDIAN must be defined"
#endif
	};
	u32 raw_eip160_extcfg;
};

/* Internal used for structure eip160_version, eip201_version, eip62_version. */
union _eip_eng_ver {
	struct {
#if defined(__LITTLE_ENDIAN)
		u32 eip_nr: 8;
		u32 eip_nr_complement: 8;
		u32 patch_level: 4;
		u32 minor_version: 4;
		u32 major_version: 4;
		u32 reserved: 4;
#elif defined(__BIG_ENDIAN)
		u32 reserved: 4;
		u32 major_version: 4;
		u32 minor_version: 4;
		u32 patch_level: 4;
		u32 eip_nr_complement: 8;
		u32 eip_nr: 8;
#else
#error "__LITTLE_ENDIAN or __BIG_ENDIAN must be defined"
#endif
	};
	u32 raw_engxxx_ver;
};

/* Internal used for structure eip62_config. */
union _eip62_config {
	struct {
#if defined(__LITTLE_ENDIAN)
		u32 minor_revision: 4;
		u32 major_revision: 4;
		u32 fpga_solution: 1;
		u32 gf_sboxes: 1;
		u32 lookup_sboxes: 1;
		u32 macsec_aes_only: 1;
		u32 aes_present: 1;
		u32 aes_fb: 1;
		u32 aes_speed: 4;
		u32 aes192: 1;
		u32 aes256: 1;
		u32 eop_param_bits: 3;
		u32 reserved: 4;
		u32 sec_offset: 1;
		u32 reserved2: 2;
		u32 ghash_present: 1;
		u32 ghash_speed: 1;
#elif defined(__BIG_ENDIAN)
		u32 ghash_speed: 1;
		u32 ghash_present: 1;
		u32 reserved2: 2;
		u32 sec_offset: 1;
		u32 reserved: 4;
		u32 eop_param_bits: 3;
		u32 aes256: 1;
		u32 aes192: 1;
		u32 aes_speed: 4;
		u32 aes_fb: 1;
		u32 aes_present: 1;
		u32 macsec_aes_only: 1;
		u32 lookup_sboxes: 1;
		u32 gf_sboxes: 1;
		u32 fpga_solution: 1;
		u32 major_revision: 4;
		u32 minor_revision: 4;
#else
#error "__LITTLE_ENDIAN or __BIG_ENDIAN must be defined"
#endif
	};
	u32 raw_eip62_cfg;
};

/* Internal structure used for eip201_config. */
union _eip201_config {
	struct {
#if defined(__LITTLE_ENDIAN)
		u32 number_of_inputs: 6;
		u32 reserved: 26;
#elif defined(__BIG_ENDIAN)
		u32 reserved: 26;
		u32 number_of_inputs: 6;
#else
#error "__LITTLE_ENDIAN or __BIG_ENDIAN must be defined"
#endif
	};
	u32 raw_aic_opt;
};

/* Internal used for structure update_sa_cw. */
union _update_sa_cw {
	struct {
#if defined(__LITTLE_ENDIAN)
		u32 next_sa_index: 14;
		u32 sa_expired_irq: 1;
		u32 sa_index_valid: 1;
		u32 flow_index: 15;
		u32 sa_ind_update_en: 1;
#elif defined(__BIG_ENDIAN)
		u32 sa_ind_update_en: 1;
		u32 flow_index: 15;
		u32 sa_index_valid: 1;
		u32 sa_expired_irq: 1;
		u32 next_sa_index: 14;
#else
#error "__LITTLE_ENDIAN or __BIG_ENDIAN must be defined"
#endif
	};
	u32 raw_sa_cw;
};

/* Output Transform Record - Internal use for transform_rec. */
struct _raw_trans_rec {
	u8 sa_index;
	u32 raw_trans_rec[EGR_MAX_TR_SIZE_WORDS];
};

/* Clear Transform Record - Internal use for transform_rec. */
struct _clr_trans_rec {
	u8 sa_index;
};

/* HW Transform Record for CA type of GCM-AES-128 - Internal use for
transform_rec. */
struct _ing_tr_32bpn_128bak {
	u32 ccw;
	u32 ctx_id;
	u32 key[MACSEC_CA128_KEY_LEN_WORD];
	u32 hkey[MACSEC_CA128_KEY_LEN_WORD];
	u32 seq;
	u32 mask;
	u32 iv0;
	u32 iv1;
	u32 res0[6];
};

/* HW Transform Record for CA type of GCM-AES-256 - Internal use for
transform_rec. */
struct _ing_tr_32bpn_256bak {
	u32 ccw;
	u32 ctx_id;
	u32 key[MACSEC_CA256_KEY_LEN_WORD];
	u32 hkey[MACSEC_CA128_KEY_LEN_WORD];
	u32 seq;
	u32 mask;
	u32 iv0;
	u32 iv1;
	u32 res0[2];
};

/* HW Transform Record for CA type of GCM-AES-XPN-128 - Internal use for
transform_rec. */
struct _ing_tr_64bpn_128bak {
	u32 ccw;
	u32 ctx_id;
	u32 key[MACSEC_CA128_KEY_LEN_WORD];
	u32 hkey[MACSEC_CA128_KEY_LEN_WORD];
	u32 seq0;
	u32 seq1;
	u32 mask;
	u32 iv0;
	u32 iv1;
	u32 iv2;
	u32 res[4];
};

/* HW Transform Record for CA type of GCM-AES-XPN-256 - Internal use for
transform_rec. */
struct _ing_tr_64bpn_256bak {
	u32 ccw;
	u32 ctx_id;
	u32 key[MACSEC_CA256_KEY_LEN_WORD];
	u32 hkey[MACSEC_CA128_KEY_LEN_WORD];
	u32 seq0;
	u32 seq1;
	u32 mask;
	u32 iv0;
	u32 iv1;
	u32 iv2;
};

/* HW Transform Record for CA type of GCM-AES-128 - Internal use for
transform_rec. */
struct _egr_tr_32bpn_128bak {
	u32 ccw;
	u32 ctx_id;
	u32 key[MACSEC_CA128_KEY_LEN_WORD];
	u32 hkey[MACSEC_CA128_KEY_LEN_WORD];
	u32 seq;
	u32 iv0;
	u32 iv1;
	u32 res0[2];
	u32 upd_sa_cw;
	u32 res1[8];
};

/* HW Transform Record for CA type of GCM-AES-256 - Internal use for
transform_rec. */
struct _egr_tr_32bpn_256bak {
	u32 ccw;
	u32 ctx_id;
	u32 key[MACSEC_CA256_KEY_LEN_WORD];
	u32 hkey[MACSEC_CA128_KEY_LEN_WORD];
	u32 seq;
	u32 iv0;
	u32 iv1;
	u32 res0[2];
	u32 upd_sa_cw;
	u32 res1[4];
};

/* HW Transform Record for CA type of GCM-AES-XPN-128 - Internal use for
transform_rec. */
struct _egr_tr_64bpn_128bak {
	u32 ccw;
	u32 ctx_id;
	u32 key[MACSEC_CA128_KEY_LEN_WORD];
	u32 hkey[MACSEC_CA128_KEY_LEN_WORD];
	u32 seq0;
	u32 seq1;
	u32 res0;
	u32 is0;
	u32 is1;
	u32 is2;
	u32 iv0;
	u32 iv1;
	u32 res1;
	u32 upd_sa_cw;
	u32 res2[4];
};

/* HW Transform Record for CA type of GCM-AES-XPN-256 - Internal use for
transform_rec. */
struct _egr_tr_64bpn_256bak {
	u32 ccw;
	u32 ctx_id;
	u32 key[MACSEC_CA256_KEY_LEN_WORD];
	u32 hkey[MACSEC_CA128_KEY_LEN_WORD];
	u32 seq0;
	u32 seq1;
	u32 res0;
	u32 is0;
	u32 is1;
	u32 is2;
	u32 iv0;
	u32 iv1;
	u32 res1;
	u32 upd_sa_cw;
};

/* Internal used for structure sam_misc_par, sam_vlan_par. */
union _sam_misc_par {
	struct {
#if defined(__LITTLE_ENDIAN)
		u32 vlan_valid: 1;
		u32 qinq_found: 1;
		u32 stag_valid: 1;
		u32 qtag_valid: 1;
		u32 vlan_up: 3;
		u32 control_packet: 1;
		u32 untagged: 1;
		u32 tagged: 1;
		u32 bad_tag: 1;
		u32 kay_tag: 1;
		u32 source_port: 2;
		u32 reserved1: 2;
		u32 match_priority: 4;
		u32 reserved2: 4;
		u32 macsec_tci_an: 8;
#elif defined(__BIG_ENDIAN)
		u32 macsec_tci_an: 8;
		u32 reserved2: 4;
		u32 match_priority: 4;
		u32 reserved1: 2;
		u32 source_port: 2;
		u32 kay_tag: 1;
		u32 bad_tag: 1;
		u32 tagged: 1;
		u32 untagged: 1;
		u32 control_packet: 1;
		u32 vlan_up: 3;
		u32 qtag_valid: 1;
		u32 stag_valid: 1;
		u32 qinq_found: 1;
		u32 vlan_valid: 1;
#else
#error "__LITTLE_ENDIAN or __BIG_ENDIAN must be defined"
#endif
	};
	u32 raw_sam_misc;
};

/* Internal used for structure sam_mask_par. */
union _sam_mask_par {
	struct {
#if defined(__LITTLE_ENDIAN)
		u32 mac_sa_mask: 6;
		u32 mac_da_mask: 6;
		u32 mac_etype_mask: 1;
		u32 vlan_valid_mask: 1;
		u32 qinq_found_mask: 1;
		u32 stag_valid_mask: 1;
		u32 qtag_valid_mask: 1;
		u32 vlan_up_mask: 1;
		u32 vlan_id_mask: 1;
		u32 source_port_mask: 1;
		u32 ctrl_packet_mask: 1;
		u32 vlan_up_inner_mask: 1;
		u32 vlan_id_inner_mask: 1;
		u32 macsec_sci_mask: 1;
		u32 tci_an_mask: 8;
#elif defined(__BIG_ENDIAN)
		u32	tci_an_mask: 8;
		u32	macsec_sci_mask: 1;
		u32	vlan_id_inner_mask: 1;
		u32	vlan_up_inner_mask: 1;
		u32	ctrl_packet_mask: 1;
		u32	source_port_mask: 1;
		u32	vlan_id_mask: 1;
		u32	vlan_up_mask: 1;
		u32	qtag_valid_mask: 1;
		u32	stag_valid_mask: 1;
		u32	qinq_found_mask: 1;
		u32	vlan_valid_mask: 1;
		u32	mac_etype_mask: 1;
		u32 mac_da_mask: 6;
		u32 mac_sa_mask: 6;
#else
#error "__LITTLE_ENDIAN or __BIG_ENDIAN must be defined"
#endif
	};
	u32 raw_sam_mask;
};

/* Internal use for extended parameters match. */
union _sam_extn_par {
	struct {
#if defined(__LITTLE_ENDIAN)
		u32 vlan_id_inner: 12;
		u32 vlan_up_inner: 3;
		u32 reserved1: 1;
		u32 flow_index: 8;
		u32 reserved2: 8;
#elif defined(__BIG_ENDIAN)
		u32	reserved2: 8;
		u32	flow_index: 8;
		u32	reserved1: 1;
		u32	vlan_up_inner: 3;
		u32	vlan_id_inner: 12;
#else
#error "__LITTLE_ENDIAN or __BIG_ENDIAN must be defined"
#endif
	};
	u32 raw_sam_extn;
};

/* Output Secure Association Matching (SAM) rule params - Internal use for
sa_match_par. */
struct _raw_sam_par {
	u8 rule_index;
	u32 raw_sam_par[IE_MAX_SAM_SIZE_WORDS];
};

/* Clear Secure Association Matching (SAM) rule params - Internal use for
sa_match_par. */
struct _clr_sam_par {
	u8 rule_index;
};

#if defined(SUPPORT_MACSEC_VLAN) && SUPPORT_MACSEC_VLAN
union _sam_cp_tag {
	struct {
#if defined(__LITTLE_ENDIAN)
		u32 map_tbl_pcp_0: 3;
		u32 map_tbl_pcp_1: 3;
		u32 map_tbl_pcp_2: 3;
		u32 map_tbl_pcp_3: 3;
		u32 map_tbl_pcp_4: 3;
		u32 map_tbl_pcp_5: 3;
		u32 map_tbl_pcp_6: 3;
		u32 map_tbl_pcp_7: 3;
		u32 def_up: 3;
		u32 stag_up_en: 1;
		u32 qtag_up_en: 1;
		u32 parse_qinq: 1;
		u32 parse_stag: 1;
		u32 parse_qtag: 1;
#elif defined(__BIG_ENDIAN)
		u32 parse_qtag: 1;
		u32 parse_stag: 1;
		u32 parse_qinq: 1;
		u32 qtag_up_en: 1;
		u32 stag_up_en: 1;
		u32 def_up: 3;
		u32 map_tbl_pcp_7: 3;
		u32 map_tbl_pcp_6: 3;
		u32 map_tbl_pcp_5: 3;
		u32 map_tbl_pcp_4: 3;
		u32 map_tbl_pcp_3: 3;
		u32 map_tbl_pcp_2: 3;
		u32 map_tbl_pcp_1: 3;
		u32 map_tbl_pcp_0: 3;
#else
#error "__LITTLE_ENDIAN or __BIG_ENDIAN must be defined"
#endif
	};
	u32 raw_ocp_tag;
};

union _sam_pp_tags {
	struct {
#if defined(__LITTLE_ENDIAN)
		u32 qtag_value: 16;
		u32 stag_value: 16;
#elif defined(__BIG_ENDIAN)
		u32 stag_value: 16;
		u32 qtag_value: 16;
#else
#error "__LITTLE_ENDIAN or __BIG_ENDIAN must be defined"
#endif
	};
	u32 raw_pp_tags;
};

union _sam_pp_tags2 {
	struct {
#if defined(__LITTLE_ENDIAN)
		u32 stag_value2: 16;
		u32 stag_value3: 16;
#elif defined(__BIG_ENDIAN)
		u32 stag_value3: 16;
		u32 stag_value2: 16;
#else
#error "__LITTLE_ENDIAN or __BIG_ENDIAN must be defined"
#endif
	};
	u32 raw_pp_tags2;
};

union _sam_cp_tag2 {
	struct {
#if defined(__LITTLE_ENDIAN)
		u32 map_tbl2_pcp_0: 3;
		u32 map_tbl2_pcp_1: 3;
		u32 map_tbl2_pcp_2: 3;
		u32 map_tbl2_pcp_3: 3;
		u32 map_tbl2_pcp_4: 3;
		u32 map_tbl2_pcp_5: 3;
		u32 map_tbl2_pcp_6: 3;
		u32 map_tbl2_pcp_7: 3;
		u32 reserved: 8;
#elif defined(__BIG_ENDIAN)
		u32 reserved: 8;
		u32 map_tbl2_pcp_7: 3;
		u32 map_tbl2_pcp_6: 3;
		u32 map_tbl2_pcp_5: 3;
		u32 map_tbl2_pcp_4: 3;
		u32 map_tbl2_pcp_3: 3;
		u32 map_tbl2_pcp_2: 3;
		u32 map_tbl2_pcp_1: 3;
		u32 map_tbl2_pcp_0: 3;
#else
#error "__LITTLE_ENDIAN or __BIG_ENDIAN must be defined"
#endif
	};
	u32 raw_icp_tag;
};
#endif

/* Internal structure for clearing SAM Flow Control Action params. */
struct _clr_sam_flow_ctrl {
	u8 flow_index;
};

/* Internal structure for SAM Flow Control Action of decryption orencryption. */
union _sam_fca_macsec {
	struct {
#if defined(__LITTLE_ENDIAN)
		u32 flow_type: 2;
		u32 dest_port: 2;
		u32 drop_non_reserved: 1;
		u32 flow_crypt_auth: 1;
		u32 drop_action: 2;
		u32 sa_index: 8;
		u32 rep_prot_fr: 1;
		u32 sa_in_use: 1;
		u32 include_sci: 1;
		u32 vlevel_es_scb: 2;
		u32 tag_bypass_size: 2;
		u32 sa_index_update: 1;
		u32 conf_offset: 7;
		u32 conf_protect: 1;
#elif defined(__BIG_ENDIAN)
		u32 conf_protect: 1;
		u32 conf_offset: 7;
		u32 sa_index_update: 1;
		u32 tag_bypass_size: 2;
		u32 vlevel_es_scb: 2;
		u32 include_sci: 1;
		u32 sa_in_use: 1;
		u32 rep_prot_fr: 1;
		u32 sa_index: 8;
		u32 drop_action: 2;
		u32 flow_crypt_auth: 1;
		u32 drop_non_reserved: 1;
		u32 dest_port: 2;
		u32 flow_type: 2;
#else
#error "__LITTLE_ENDIAN or __BIG_ENDIAN must be defined"
#endif
	};
	u32 raw_fca_macsec;
};

/* Internal structure for SAM Flow Control Action of bypass or drop. */
union _sam_fca_bod {
	struct {
#if defined(__LITTLE_ENDIAN)
		u32 flow_type: 2;
		u32 dest_port: 2;
		u32 drop_non_reserved: 1;
		u32 flow_crypt_auth: 1;
		u32 drop_action: 2;
		u32 sa_index: 8;
		u32 reserved: 1;
		u32 sa_in_use: 1;
		u32 reserved2: 14;
#elif defined(__BIG_ENDIAN)
		u32 reserved2: 14;
		u32 sa_in_use: 1;
		u32 reserved: 1;
		u32 sa_index: 8;
		u32 drop_action: 2;
		u32 flow_crypt_auth: 1;
		u32 drop_non_reserved: 1;
		u32 dest_port: 2;
		u32 flow_type: 2;
#else
#error "__LITTLE_ENDIAN or __BIG_ENDIAN must be defined"
#endif
	};
	u32 raw_fca_bod;
};

/* Internal structure for SAM Flow Control Action of crypt-authentication. */
union _sam_fca_crypt {
	struct {
#if defined(__LITTLE_ENDIAN)
		u32 reserved: 2;
		u32 dest_port: 2;
		u32 drop_non_reserved: 1;
		u32 flow_crypt_auth: 1;
		u32 drop_action: 2;
		u32 sa_index: 8;
		u32 iv: 2;
		u32 conf_offset_7: 1;
		u32 icv_append: 1;
		u32 icv_verify: 1;
		u32 reserved2: 3;
		u32 conf_offset: 7;
		u32 conf_protect: 1;
#elif defined(__BIG_ENDIAN)
		u32 conf_protect: 1;
		u32 conf_offset: 7;
		u32 reserved2: 3;
		u32 icv_verify: 1;
		u32 icv_append: 1;
		u32 conf_offset_7: 1;
		u32 iv: 2;
		u32 sa_index: 8;
		u32 drop_action: 2;
		u32 flow_crypt_auth: 1;
		u32 drop_non_reserved: 1;
		u32 dest_port: 2;
		u32 reserved: 2;
#else
#error "__LITTLE_ENDIAN or __BIG_ENDIAN must be defined"
#endif
	};
	u32 raw_fca_crypt;
};

/* Internal structure for SAM Flow Control Action of crypt-authentication. */
union _sam_fca_cac {
	struct {
#if defined(__LITTLE_ENDIAN)
		u32 crypt_auth_bypass_len: 8;
		u32 reserved: 24;
#elif defined(__BIG_ENDIAN)
		u32 reserved: 24;
		u32 crypt_auth_bypass_len: 8;
#else
#error "__LITTLE_ENDIAN or __BIG_ENDIAN must be defined"
#endif
	};
	u32 raw_fca_cac;
};

/* Internal structure for use for sam_flow_ctrl. */
struct _sam_flow_ctrl {
	u8 flow_index;
	union {
		union _sam_fca_macsec dec_enc;
		union _sam_fca_bod bypass_drop;
		union _sam_fca_crypt crypt_auth;
	};
	union _sam_fca_cac cabp_len;
};

/* Internal used for structure sam_enable_ctrl. */
union _sam_ena_ctrl {
	struct {
#if defined(__LITTLE_ENDIAN)
		u32 sam_index_set: 8;
		u32 res2: 6;
		u32 set_enable: 1;
		u32 set_all: 1;
		u32 sam_index_clear: 8;
		u32 res1: 6;
		u32 clear_enable: 1;
		u32 clear_all: 1;
#elif defined(__BIG_ENDIAN)
		u32 clear_all: 1;
		u32 clear_enable: 1;
		u32 res1: 6;
		u32 sam_index_clear: 8;
		u32 set_all: 1;
		u32 set_enable: 1;
		u32 res2: 6;
		u32 sam_index_set: 8;
#else
#error "__LITTLE_ENDIAN or __BIG_ENDIAN must be defined"
#endif
	};
	u32 raw_sam_eec;
};

#if defined(EN_MSEC_DEBUG_ACCESS) && EN_MSEC_DEBUG_ACCESS
/* Internal structure used for count_debug1 */
union _debug_count1 {
	struct {
#if defined(__LITTLE_ENDIAN)
		u32 sa_inc_mask: 15;
		u32 reserved: 1;
		u32 sa_octet_inc: 15;
		u32 reserved2: 1;
#elif defined(__BIG_ENDIAN)
		u32 reserved2: 1;
		u32 sa_octet_inc: 15;
		u32 reserved: 1;
		u32 sa_inc_mask: 15;
#else
#error "__LITTLE_ENDIAN or __BIG_ENDIAN must be defined"
#endif
	};
	u32 raw_dbg_cnt1;
};

/* Internal structure used for count_debug2 */
union _debug_count2 {
	struct {
#if defined(__LITTLE_ENDIAN)
		u32 vlan_inc_mask: 15;
		u32 reserved: 1;
		u32 vlan_octet_inc: 15;
		u32 reserved2: 1;
#elif defined(__BIG_ENDIAN)
		u32 reserved2: 1;
		u32 vlan_octet_inc: 15;
		u32 reserved: 1;
		u32 vlan_inc_mask: 15;
#else
#error "__LITTLE_ENDIAN or __BIG_ENDIAN must be defined"
#endif
	};
	u32 raw_dbg_cnt2;
};

/* Internal structure used for count_debug4 */
union _debug_count4 {
	struct {
#if defined(__LITTLE_ENDIAN)
		u32 sa_test: 1;
		u32 vlan_test: 1;
		u32 global_test: 1;
		u32 reserved: 1;
		u32 sa_nr: 8;
		u32 vlan_up: 3;
		u32 reserved2: 1;
		u32 global_octet_inc: 15;
		u32 reserved3: 1;
#elif defined(__BIG_ENDIAN)
		u32 reserved3: 1;
		u32 global_octet_inc: 15;
		u32 reserved2: 1;
		u32 vlan_up: 3;
		u32 sa_nr: 8;
		u32 reserved: 1;
		u32 global_test: 1;
		u32 vlan_test: 1;
		u32 sa_test: 1;
#else
#error "__LITTLE_ENDIAN or __BIG_ENDIAN must be defined"
#endif
	};
	u32 raw_dbg_cnt4;
};
#endif

/* Internal structure used for cntrs_ctrl. */
union _cntrs_ctrl {
	struct {
#if defined(__LITTLE_ENDIAN)
		u32 reset_all: 1;
		u32 debug_access: 1;
		u32 saturate_cntrs: 1;
		u32 auto_cntr_reset: 1;
		u32 reset_summary: 1;
		u32 reserved: 27;
#elif defined(__BIG_ENDIAN)
		u32 reserved: 27;
		u32 reset_summary: 1;
		u32 auto_cntr_reset: 1;
		u32 saturate_cntrs: 1;
		u32 debug_access: 1;
		u32 reset_all: 1;
#else
#error "__LITTLE_ENDIAN or __BIG_ENDIAN must be defined"
#endif
	};
	u32 raw_cnt_ctrl;
};

/* Internal structure used for count_incen1. */
union _cntr_inc_en {
	struct {
#if defined(__LITTLE_ENDIAN)
		u32 sa_inc_en: 15;
		u32 reserved: 1;
		u32 vlan_inc_en: 15;
		u32 reserved2: 1;
#elif defined(__BIG_ENDIAN)
		u32 reserved2: 1;
		u32 vlan_inc_en: 15;
		u32 reserved: 1;
		u32 sa_inc_en: 15;
#else
#error "__LITTLE_ENDIAN or __BIG_ENDIAN must be defined"
#endif
	};
	u32 raw_cnt_inc_en;
};

/* Internal structure used for sec_fail_evnt. */
union _sec_fail_evnt {
	struct {
#if defined(__LITTLE_ENDIAN)
		u32 sa_secfail_mask: 15;
		u32 vlan_cntr_update: 1;
		u32 gate_cons_check: 1;
		u32 reserved: 15;
#elif defined(__BIG_ENDIAN)
		u32 reserved: 15;
		u32 gate_cons_check: 1;
		u32 vlan_cntr_update: 1;
		u32 sa_secfail_mask: 15;
#else
#error "__LITTLE_ENDIAN or __BIG_ENDIAN must be defined"
#endif
	};
	u32 raw_sec_fail;
};

/* Internal structure used for misc_ctrl. */
union _misc_ctrl {
	struct {
#if defined(__LITTLE_ENDIAN)
		u32 mc_latency_fix: 8;
		u32 static_bypass: 1;
		u32 nm_macsec_en: 1;
		u32 validate_frames: 2;
		u32 sectag_after_vlan: 1;
		u32 reserved: 11;
		u32 xform_rec_size: 2;
		/* Future extension, should be set to zero. */
		u32 mc_latency_fix_m: 6;
#elif defined(__BIG_ENDIAN)
		/* Future extension, should be set to zero. */
		u32 mc_latency_fix_m: 6;
		u32 xform_rec_size: 2;
		u32 reserved: 11;
		u32 sectag_after_vlan: 1;
		u32 validate_frames: 2;
		u32 nm_macsec_en: 1;
		u32 static_bypass: 1;
		u32 mc_latency_fix: 8;
#else
#error "__LITTLE_ENDIAN or __BIG_ENDIAN must be defined"
#endif
	};
	u32 raw_misc_ctrl;
};

/* Internal structure used for sanm_flow_ctrl per frame type */
union _sanm_fca_pft {
	struct {
#if defined(__LITTLE_ENDIAN)
		u32 utag_flow_type: 1;
		u32 utag_reserved: 1;
		u32 utag_dest_port: 2;
		u32 utag_drop_non_res: 1;
		u32 utag_reserved2: 1;
		u32 utag_drop_action: 2;

		u32 tag_flow_type: 1;
		u32 tag_reserved: 1;
		u32 tag_dest_port: 2;
		u32 tag_drop_non_res: 1;
		u32 tag_reserved2: 1;
		u32 tag_drop_action: 2;

		u32 btag_flow_type: 1;
		u32 btag_reserved: 1;
		u32 btag_dest_port: 2;
		u32 btag_drop_non_res: 1;
		u32 btag_reserved2: 1;
		u32 btag_drop_action: 2;

		u32 ktag_flow_type: 1;
		u32 ktag_reserved: 1;
		u32 ktag_dest_port: 2;
		u32 ktag_drop_non_res: 1;
		u32 ktag_reserved2: 1;
		u32 ktag_drop_action: 2;
#elif defined(__BIG_ENDIAN)
		u32 ktag_drop_action: 2;
		u32 ktag_reserved2: 1;
		u32 ktag_drop_non_res: 1;
		u32 ktag_dest_port: 2;
		u32 ktag_reserved: 1;
		u32 ktag_flow_type: 1;

		u32 btag_drop_action: 2;
		u32 btag_reserved2: 1;
		u32 btag_drop_non_res: 1;
		u32 btag_dest_port: 2;
		u32 btag_reserved: 1;
		u32 btag_flow_type: 1;

		u32 tag_drop_action: 2;
		u32 tag_reserved2: 1;
		u32 tag_drop_non_res: 1;
		u32 tag_dest_port: 2;
		u32 tag_reserved: 1;
		u32 tag_flow_type: 1;

		u32 utag_drop_action: 2;
		u32 utag_reserved2: 1;
		u32 utag_drop_non_res: 1;
		u32 utag_dest_port: 2;
		u32 utag_reserved: 1;
		u32 utag_flow_type: 1;
#else
#error "__LITTLE_ENDIAN or __BIG_ENDIAN must be defined"
#endif
	};
	u32 raw_sanm_fc;
};

/* Ingresss Per-SA counters threshold summary. Bit assigned input/output
	is mapped to this structure in which the member InOctetsDecryptedValidated
	is mapped to LS bit 0, InPktsUncheckedHitDropReserved to LS bit 2 and so on.
	*/
struct ing_psa_cntrs {
	/* Total input octets decrypted (aka validated). */
	u8 InOctetsDecryptedValidated: 1;
	/* Reserved. */
	u8 reserved: 1;
	/* Total input packets Unchecked (aks HitDropReserved). */
	u8 InPktsUncheckedHitDropReserved: 1;
	/* Total input packets Delayed. */
	u8 InPktsDelayed: 1;
	/* Total input packets Late. */
	u8 InPktsLate: 1;
	/* Total input packets Decrypted. */
	u8 InPktsOK: 1;
	/* Total input packets Invalid. */
	u8 InPktsInvalid: 1;
	/* Total input packets NotValid. */
	u8 InPktsNotValid: 1;
	/* Total input packets NotUsingSA. */
	u8 InPktsNotUsingSA: 1;
	/* Total input packets UnusedSA. */
	u8 InPktsUnusedSA: 1;
	/* Total input packets those are not MACsec tagged. */
	u8 InPktsUntaggedHit: 1;
};

/* Egresss Per-SA counters threshold summary. Bit assigned input/output
	is mapped to this structure in which the member OutOctetsEncryptedProtected
	is mapped to LS bit 0, OutPktsEncryptedProtectedHitDropReserved to LS bit 2
	and so on. */
struct egr_psa_cntrs {
	/* Total output octets encrypted (aka protected). */
	u8 OutOctetsEncryptedProtected: 1;
	/* Reserved. */
	u8 reserved: 1;
	/* Total output packets encrypted (aks HitDropReserved). */
	u8 OutPktsEncryptedProtectedHitDropReserved: 1;
	/* Total output packets those exceed MTU size after encrypted. */
	u8 OutPktsTooLong: 1;
};

#if defined(SUPPORT_MACSEC_VLAN) && SUPPORT_MACSEC_VLAN
/* Per-VLAN counters summary. */
struct vlan_cntr_sum0 {
	/* Threshold summary for VLAN counter 0 (i.e (-i)InOctetsVL
	or (-e)OutOctetsVL) for all UP values, meaning LS bit 0 is counter 0
	for UP=0, LS bit 1 is counter 0 for UP=1 etc. */
	u8 vlan_cntr_0: 8;
	/* Reserved. */
	u8 reserved: 8;
	/* Threshold summary for VLAN counter 2 (i.e (-i)InPktsVL or
	(-e)OutPktsVL) for all UP values, meaning LS bit 0 is counter 2 for
	UP=0, LS bit 1 is counter 2 for UP=1 etc. */
	u8 vlan_cntr_2: 8;
	/* Threshold summary for VLAN counter 3 (i.e (-i)InDroppedPktsVL or
	(-e)OutDroppedPktsVL) for all UP values, meaning LS bit 0 is counter 3 for
	UP=0, LS bit 1 is counter 3 for UP=1 etc. */
	u8 vlan_cntr_3: 8;
};

/* Per-VLAN counters summary */
struct vlan_cntr_sum1 {
	/* Threshold summary for VLAN counter 4 (i.e (-i)InOverSizePktsVL or
	(-e)OutOverSizePktsVL) or (-e)OutOctetsVL) for all UP values, meaning LS
	bit 0 is counter 4 for UP=0, LS bit 1 is counter 4 for UP=1 etc. */
	u8 vlan_cntr_4: 8;
};

/* Ingresss VLAN counters threshold summary. Bit assigned input/output
	is mapped to this structure in which the member InOctetsVL is mapped to LS
	bit 0, InPktsVL to LS bit 2 and so on. */
struct ing_vlan_cntrs {
	/* Total decrypted octets. */
	u32 InOctetsVL: 1;
	/* Reserved. */
	u32 reserved: 1;
	/* Total decrypted packets. */
	u32 InPktsVL: 1;
	/* Total dropped packets. */
	u32 InDroppedPktsVL: 1;
	/* Total OverSize packets. */
	u32 InOverSizePktsVL: 1;
};

/* Egresss VLAN counters threshold summary. Bit assigned input/output
	is mapped to this structure in which the member OutOctetsVL is mapped to LS
	bit 0, OutPktsVL to LS bit 2 and so on. */
struct egr_vlan_cntrs {
	/* Total encrypted octets. */
	u8 OutOctetsVL: 1;
	/* Reserved. */
	u8 reserved: 1;
	/* Total encrypted packets. */
	u8 OutPktsVL: 1;
	/* Total dropped packets. */
	u8 OutDroppedPktsVL: 1;
	/* Total OverSize packets. */
	u8 OutOverSizePktsVL: 1;
};
#endif

/* Ingress Global counters threshold summary. Bit assigned input/output
	is mapped to this structure in which the member TransformErrorPkts is mapped
	to LS bit 0, InPktsCtrl to LS bit 1 and so on. */
struct ing_global_cntrs {
	/* Total transform errors. */
	u8 TransformErrorPkts: 1;
	/* Total input control frames. */
	u8 InPktsCtrl: 1;
	/* Total input 'Strict' validated non MACsec tagged frames. */
	u8 InPktsNoTag: 1;
	/* Total input non 'Strict' validated non MACsec tagged frames. */
	u8 InPktsUntagged: 1;
	/* Total input MACsec tagged frames. */
	u8 InPktsTagged: 1;
	/* Total input (non MACsec tagged) bad-tagged frames. */
	u8 InPktsBadTag: 1;
	/* Total input (non MACsec tagged) SA not found frames. */
	u8 InPktsUntaggedMiss: 1;
	/* Total input (MACsec tagged) but without SCI frames. */
	u8 InPktsNoSCI: 1;
	/* Total input (MACsec tagged) but with unknown SCI frames. */
	u8 InPktsUnknownSCI: 1;
	/* Total input Ingress CC failed Controlled frames. */
	u8 InConsistCheckControlledNotPass: 1;
	/* Total input Ingress CC failed Uncontrolled frames. */
	u8 InConsistCheckUncontrolledNotPass: 1;
	/* Total input Ingress CC passed Controlled frames. */
	u8 InConsistCheckControlledPass: 1;
	/* Total input Ingress CC passed Uncontrolled frames. */
	u8 InConsistCheckUncontrolledPass: 1;
	/* Total input non-VLAN frames decrypted. */
	u8 InOverSizePkts: 1;
};

#if 0
/* Egress Global counters threshold summary. Bit assigned input/output
	is mapped to this structure in which the member TransformErrorPkts is mapped
	to LS bit 0, OutPktsCtrl to LS bit 1 and so on.  */
struct egr_global_cntrs {
	/* Total transform errors.	*/
	u32 TransformErrorPkts: 1;
	/* Total output control frames. */
	u32 OutPktsCtrl: 1;
	/* Total output frames for those SA not found. */
	u32 OutPktsUnknownSA: 1;
	/* Total output frames without protection. */
	u32 OutPktsUntagged: 1;
	/* Total output non-VLAN frames encrypted. */
	u32 OutOverSizePkts: 1;
};
#endif

/* Internal structure used for outer VLAN match in cp_match_par. */
union _cpc_vlan_par {
	struct {
#if defined(__LITTLE_ENDIAN)
		u32 reserved: 16;
		u32 vlan_id: 12;
		u32 vlan_up: 3;
		u32 disable_up: 1;
#elif defined(__BIG_ENDIAN)
		u32 disable_up: 1;
		u32 vlan_up: 3;
		u32 vlan_id: 12;
		u32 reserved: 16;
#else
#error "__LITTLE_ENDIAN or __BIG_ENDIAN must be defined"
#endif
	};
	u32  raw_cpc_vlan;
};

/* Internal structure used for cp_match_par. */
union _cp_match_mode {
	struct {
#if defined(__LITTLE_ENDIAN)
		u32 vlan_sel_0_7: 8;
		u32 e_type_sel_0_9: 10;
		u32 reserved: 3;
		u32 e_type_sel_10_17: 8;
		u32 reserved2: 3;
#elif defined(__BIG_ENDIAN)
		u32 reserved2: 3;
		u32 e_type_sel_10_17: 8;
		u32 reserved: 3;
		u32 e_type_sel_0_9: 10;
		u32 vlan_sel_0_7: 8;
#else
#error "__LITTLE_ENDIAN or __BIG_ENDIAN must be defined"
#endif
	};
	u32 raw_cpm_mode;
};

/* Internal structure used for cpm_mode_ena. */
union _cp_match_ena {
	struct {
#if defined(__LITTLE_ENDIAN)
		u32 mac_da_0_7: 8;
		u32 e_type_0_7: 8;
		u32 combined_8_9: 2;
		u32 dmac_range: 1;
		u32 dmac_const_44: 1;
		u32 dmac_const_48: 1;
		u32 e_type_10_17:8;
		u32 reserved: 3;
#elif defined(__BIG_ENDIAN)
		u32 reserved: 3;
		u32 e_type_10_17:8;
		u32 dmac_const_48: 1;
		u32 dmac_const_44: 1;
		u32 dmac_range: 1;
		u32 combined_8_9: 2;
		u32 e_type_0_7: 8;
		u32 mac_da_0_7: 8;
#else
#error "__LITTLE_ENDIAN or __BIG_ENDIAN must be defined"
#endif
	};
	u32 raw_cpm_enable;
};

/* Ingress Consistency Check match's misc parameters. Bit assigned
	input/output is mapped to this structure in which the member sai_hit mapped
	to LS bit 0, vlan_valid to LS bit 1 and so on. */
struct iccm_misc_par {
	/* SA entry matched for this packet. */
	u8 sai_hit: 1;
	/* Packet is tagged as 802.1Q or 802.1s VLAN packet. */
	u8 vlan_valid: 1;
	/* First Ether-type in packet > cp_etype_max_len. */
	u8 etype_valid: 1;
	/* Packet was classified as control packet. */
	u8 ctrl_packet: 1;
	/* Enable sai_hit field compare. */
	u8 sa_hit_mask: 1;
	/* Enable sa_or_rule_nr field compare with SA entry number (the
	rule_nr_mask must be 0b). */
	u8 sa_nr_mask: 1;
	/* Enable vlan_valid field compare. */
	u8 vlan_valid_mask: 1;
	/* Enable vlan_id field compare (do not set to 1b if no VLAN packet
	is expected here). */
	u8 vlan_id_mask: 1;
	/* Enable etype_valid field compare. */
	u8 etype_valid_mask: 1;
	/* Enable payload_e_type field compare. */
	u8 payl_etype_mask: 1;
	/* Enable ctrl_packet field compare. */
	u8 ctrl_packet_mask: 1;
	/* Enable sa_or_rule_nr field compare with SA matching rule number
	(the sa_nr_mask must be 0b). */
	u8 rule_nr_mask: 1;
	/* Priority of this entry for determining the match action on a
	match, 0 = lowest, 7 = highest. */
	u8 match_priority: 3;
	/* Packet match action, 0b = drop, 1b = pass. */
	u8 match_action: 1;
	/* Compare value for SA matching rule number or SA entry number,
	depending on which flag is set (rule_nr_mask or sa_nr_mask). */
	u8 sa_or_rule_nr: 8;
	/* Enable parsed inner VLAN User Priority value compare with
	vlan_up_inner field. */
	u8 vlan_up_inner_mask: 1;
	/* Enable parsed inner VLAN ID value compare with vlan_up_inner
	field. */
	u8 vlan_id_inner_mask: 1;
	/* SA matching rule matched for this packet. */
	u8 rule_hit: 1;
	/* Enable rule_hit field compare. */
	u8 rule_hit_mask: 1;
};

/* Internal structure used for misc params of icc_match_par. */
union _iccm_misc_par {
	struct {
#if defined(__LITTLE_ENDIAN)
		u32 sai_hit: 1;
		u32 vlan_valid: 1;
		u32 etype_valid: 1;
		u32 ctrl_packet: 1;
		u32 sa_hit_mask: 1;
		u32 sa_nr_mask: 1;
		u32 vlan_valid_mask: 1;
		u32 vlan_id_mask: 1;
		u32 etype_valid_mask: 1;
		u32 payl_etype_mask: 1;
		u32 ctrl_packet_mask: 1;
		u32 rule_nr_mask: 1;
		u32 match_priority: 3;
		u32 match_action: 1;
		u32 sa_or_rule_nr: 8;
		u32 vlan_up_inner_mask: 1;
		u32 vlan_id_inner_mask: 1;
		u32 rule_hit: 1;
		u32 rule_hit_mask: 1;
		u32 reserved: 4;
#elif defined(__BIG_ENDIAN)
		u32 reserved: 4;
		u32 rule_hit_mask: 1;
		u32 rule_hit: 1;
		u32 vlan_id_inner_mask: 1;
		u32 vlan_up_inner_mask: 1;
		u32 sa_or_rule_nr: 8;
		u32 match_action: 1;
		u32 match_priority: 3;
		u32 rule_nr_mask: 1;
		u32 ctrl_packet_mask: 1;
		u32 payl_etype_mask: 1;
		u32 etype_valid_mask: 1;
		u32 vlan_id_mask: 1;
		u32 vlan_valid_mask: 1;
		u32 sa_nr_mask: 1;
		u32 sa_hit_mask: 1;
		u32 ctrl_packet: 1;
		u32 etype_valid: 1;
		u32 vlan_valid: 1;
		u32 sai_hit: 1;
#else
#error "__LITTLE_ENDIAN or __BIG_ENDIAN must be defined"
#endif
	};
	u32 raw_iccm_misc;
};

/* Internal structure used for inner VLAN info of icc_match_par. */
union _iccm_inr_vlan {
	struct {
#if defined(__LITTLE_ENDIAN)
		u32 inr_vlan_id: 12;
		u32 inr_vlan_up: 3;
		u32 reserved: 17;
#elif defined(__BIG_ENDIAN)
		u32 reserved: 17;
		u32 inr_vlan_up: 3;
		u32 inr_vlan_id: 12;
#else
#error "__LITTLE_ENDIAN or __BIG_ENDIAN must be defined"
#endif
	};
	u32 raw_iccm_ivl;
};

/* Internal structure used for outer VLAN, payload eth-type of icc_match_par. */
union _iccm_ovlan_etht {
	struct {
#if defined(__LITTLE_ENDIAN)
		u32 vlan_id: 12;
		u32 reserved: 4;
		u32 payload_e_type: 16;
#elif defined(__BIG_ENDIAN)
		u32 payload_e_type: 16;
		u32 reserved: 4;
		u32 vlan_id: 12;
#else
#error "__LITTLE_ENDIAN or __BIG_ENDIAN must be defined"
#endif
	};
	u32 raw_iccm_ovl_eth;
};

/* Internal structure used for ICC flow control of non match packets of
	Control and non-Control frames. */
union _icc_nm_fc {
	struct {
#if defined(__LITTLE_ENDIAN)
		u32 reserved: 14;
		u32 nm_act_cp: 1;
		u32 nm_act_ncp: 1;
		u32 reserved2: 16;
#elif defined(__BIG_ENDIAN)
		u32 reserved2: 16;
		u32 nm_act_ncp: 1;
		u32 nm_act_cp: 1;
		u32 reserved: 14;
#else
#error "__LITTLE_ENDIAN or __BIG_ENDIAN must be defined"
#endif
	};
	u32 raw_icc_nm_fc;
};

/* Internal structure used for ICC flow control of ether-type field compare
	value for the etype_valid comparison. */
union _icc_etype_mlen {
	struct {
#if defined(__LITTLE_ENDIAN)
		u32 reserved: 16;
		u32 etype_max_len: 16;
#elif defined(__BIG_ENDIAN)
		u32 etype_max_len: 16;
		u32 reserved: 16;
#else
#error "__LITTLE_ENDIAN or __BIG_ENDIAN must be defined"
#endif
	};
	u32 raw_icc_eth_len;
};

/* Internal structure used for ccm_ena_ctrl. */
union _ccm_ena_ctrl {
	struct {
#if defined(__LITTLE_ENDIAN)
		u32 cc_index_set: 8;
		u32 reserved: 6;
		u32 set_enable: 1;
		u32 set_all: 1;
		u32 cc_index_clear: 8;
		u32 reserved2: 6;
		u32 clear_enable: 1;
		u32 clear_all: 1;
#elif defined(__BIG_ENDIAN)
		u32 clear_all: 1;
		u32 clear_enable: 1;
		u32 reserved2: 6;
		u32 cc_index_clear: 8;
		u32 set_all: 1;
		u32 set_enable: 1;
		u32 reserved: 6;
		u32 cc_index_set: 8;
#else
#error "__LITTLE_ENDIAN or __BIG_ENDIAN must be defined"
#endif
	};
	u32 raw_icc_eec;
};

#if 0
#if defined(EN_MSEC_DEBUG_ACCESS) && EN_MSEC_DEBUG_ACCESS
/* Context Status provides the context status of the currently active
   frame. Bit assigned output is mapped to this structure in which the member
   e0 mapped to LS bit 0, e1 to LS bit 1 and so on. */
struct eip62_ctx_stat {
	/* Frame length error. */
	u8 e0: 1;
	/* Token error. */
	u8 e1: 1;
	/* Frame input length error. */
	u8 e2: 1;
	/* Counter overflow. */
	u8 e3: 1;
	/* Token configuration error. */
	u8 e4: 1;
	/* Invalid command/algorithm/mode/combination. */
	u8 e5: 1;
	/* Prohibited algorithm. */
	u8 e6: 1;
	/* (-i)Packet number is zero. */
	u8 e7: 1;
	/* Sequence number threshold overflow. */
	u8 e8: 1;
	/* Authentication failed. */
	u8 e9: 1;
	/* (-i)Sequence number check failed. (-e)Sequence number rollover
	detected. */
	u8 e10: 1;
};
#endif
#endif

#if 0
/* Crypto Cores's interrupt status. Bit assigned output is mapped to
	this structure in which the member input_error mapped to LS bit 0,
	processing_error to LS bit 2 and so on. */
struct eip62_intrp_stat {
	/* Input data does not properly receive all data (e2). */
	u8 input_error: 1;
	/* Reserved. */
	u8 reserved: 1;
	/* A error occurred while processing the frame or parsing the token.
	Combination of errors e0, e1, e3 and e4. */
	u8 processing_error: 1;
	/* An invalid context has been fetched. Combination of errors e5 and
	e6. */
	u8 context_error: 1;
	/* If a sequence number exceeds the programmed sequence number
	threshold (refer to 0) due to an outbound sequence number increment this
	interrupt is triggered (e8). */
	u8 seq_threshold: 1;
	/* If a sequence number roll-over (increment from maximum to zero)
	due to an outbound sequence number increment this interrupt is triggered
	(e10 egress only). */
	u8 seq_rollover: 1;
	/* Reserved. */
	u16 reserved2: 9;
	/* Actual value of the output pin, read-only. */
	u8 interrupt_pin: 1;
};

/* Crypto Cores's interrupt enable control. Bit assigned input is mapped
	to this structure in which the member input_error mapped to LS bit 0,
	processing_error to LS bit 2 and so on. */
struct eip62_intrp_ctrl {
	/* Enable the input_error interrupt. */
	u8 input_error_en: 1;
	/* Enable the output_error interrupt. */
	u8 output_error_en: 1;
	/* Enable the processing_error interrupt. */
	u8 processing_error_en: 1;
	/* Enable the context_error interrupt. */
	u8 context_error_en: 1;
	/* Enable the seq_threshold interrupt. */
	u8 seq_threshold_en: 1;
	/* Enable the seq_rollover interrupt. */
	u8 seq_rollover_en: 1;
	/* Reserved. */
	u8 reserved3: 8;
	/* Enable the fatal_error interrupt. */
	u8 fatal_error_en: 1;
	/* Set to 1b if the interrupt signal must appear on the EIP-62 IRQ
	output. */
	u8 interrupt_pin_en: 1;
};
#endif

#if 0
/* Interrupt bit assignments in Advanced Interrupt Controller (EIP201).
   Bit assigned input/output is mapped to this structure in which the member
   drop_class mapped to LS bit 0, drop_pp to LS bit 1 and so on. */
struct eip201_intrpts {
	/* Packet drop pulse from classification logic. */
	u8 drop_class: 1;
	/* Packet drop pulse from post-processor logic. */
	u8 drop_pp: 1;
	/* Packet drop pulse from MTU checking logic. */
	u8 drop_mtu: 1;
	/* Interrupt pulse from EIP-62 MACsec crypto-engine core. */
	u8 eng_irq: 1;
	/* Interrupt pulse from statistics module (SA statistics). */
	u8 stat_sa_thr: 1;
	/* Interrupt pulse from statistics module (global statistics). */
	u8 stat_global_thr: 1;
	/* Interrupt pulse from statistics module (VLAN statistics). */
	u8 stat_vlan_thr: 1;
	/* (-i)Packet drop pulse from consistency checking logic. */
	u8 drop_cc: 1;
	/* (-e)Pulse from post processor. */
	u8 sa_pn_thr: 1;
	/* (-e)Pulse from classifier. */
	u8 sa_expired: 1;
};
#endif

int derive_aes_hkey(const u8 *key, u8 *output, u32 keylen);

#endif /* _E160_MACSEC_INTL_H_ */
