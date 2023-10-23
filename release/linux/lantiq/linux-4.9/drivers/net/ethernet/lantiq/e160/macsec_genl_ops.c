// SPDX-License-Identifier: (BSD-3-Clause OR GPL-2.0-only)
/*
 * Driver for MACsec Engine inside Intel CHD GSWIP or GPHY
 *
 * Copyright 2019 - 2020 Intel Corporation
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <net/rtnetlink.h>
#include <net/macsec.h>

#include "macsec.h"

#include "registers/eip160s_ing_classifier_various_control_regs.h"
#include "registers/eip160s_egr_classifier_various_control_regs.h"

#define ENABLE_NETDEV_NOTIFIER	0

#define LGM_MAX_RX_SC		1

/* Allow all packets to be encrypted and sent */
#define DEFAULT_EGR_MATCH_MASK	0
/* Allow only packets with correct SCI and AN */
#define DEFAULT_ING_MATCH_MASK	0x03800000
/* Allow only MACsec tagged packets (Prio = 7) */
#define DEFAULT_ING_MATCH_MISC	0x00070200
/* Allow only untagged packets (Prio = 7) */
#define DEFAULT_EGR_MATCH_MISC	0x00070100
/* Allow only untagged packets (Prio = 15) */
#define AUTH_PKT_MATCH_MISC	0x000F0100

#define INVALID_RULE_NUM	0xFF

#define AUTH_PKT_FLOW_ID	0
#define SEC_PKT_FLOW_ID(x)	(1+x)

#define PRINT_DEBUG		1

/* The MISC_CONTROL register value after MACsec engine initialization, check
 * _macsec_cfg_ing_misc_flow_ctrl, _macsec_cfg_egr_misc_flow_ctrl for details
 */
#define ING_MISC_CTRL_INIT_VAL	0x01000A00
#define EGR_MISC_CTRL_INIT_VAL	0x02000A00

typedef struct
{
	u8 active;
	u8 sci[MACSEC_SCI_LEN_BYTE];
	u8 raw_tr_ids[MACSEC_MAX_AN];
	struct sa_match_par match_par;
	struct sam_flow_ctrl sam_fca;
} tx_sc_rule_t;

typedef struct
{
	u8 active;
	u8 sci[MACSEC_SCI_LEN_BYTE];
	u8 raw_tr_ids[MACSEC_MAX_AN];
} rx_sc_rule_t;

typedef struct
{
	u8 active;
	u8 enabled;
	u8 an;
	struct transform_rec raw_tr;
} tx_sa_rule_t;

typedef struct
{
	u8 active;
	u8 sc_id;
	u8 an;
	u8 enabled;
	struct transform_rec raw_tr;
	struct sa_match_par match_par;
	struct sam_flow_ctrl sam_fca;
} rx_sa_rule_t;

struct macsec_pdata
{
    bool initialized;
    tx_sa_rule_t tx_sa[MACSEC_MAX_SA_RULES];
    rx_sa_rule_t rx_sa[MACSEC_MAX_SA_RULES];
    rx_sc_rule_t rx_sc[LGM_MAX_RX_SC];
    tx_sc_rule_t tx_sc;
    struct e160_metadata dev;
};

typedef enum {
	INGRESS_DEVID=0,
	EGRESS_DEVID=1,
	MAX_MACSEC_DEVICE=2
} macsec_dev_type_t;

static inline struct macsec_pdata *get_macsec_pdata(const struct macsec_context
						    *ctx)
{
	switch (ctx->offload) {
	case MACSEC_OFFLOAD_MAC:
		if (!ctx->netdev)
			return NULL;
		return *(struct macsec_pdata **)netdev_priv(ctx->netdev);
	case MACSEC_OFFLOAD_PHY:
		if (!ctx->phydev || !ctx->phydev->priv)
			return NULL;
		return *(struct macsec_pdata **)ctx->phydev->priv;
	default:
		return NULL;
	}
}

static inline void set_macsec_pdata_ops(const struct macsec_context *ctx,
					struct macsec_pdata *pdata,
					const struct macsec_ops *ops)
{
	if (ctx->offload == MACSEC_OFFLOAD_MAC) {
		*(struct macsec_pdata **)netdev_priv(ctx->netdev) = pdata;
		ctx->netdev->macsec_ops = ops;
	} else if (ctx->offload == MACSEC_OFFLOAD_PHY) {
		*(struct macsec_pdata **)ctx->phydev->priv = pdata;
		ctx->phydev->macsec_ops = ops;
	}
}

static struct macsec_rx_sc *find_rx_sc(struct macsec_secy *secy, u8* sci)
{
	struct macsec_rx_sc *rx_sc;

	for_each_rxsc(secy, rx_sc) {
		if (!memcmp(&rx_sc->sci, sci, MACSEC_SCI_LEN_BYTE))
			return rx_sc;
	}

	return NULL;
}

static int update_trans_rec(macsec_dev_type_t dev_type, int sa_index,
			    struct macsec_secy *secy, u8 *sci,
			    struct nlattr **tb_sa,
			    struct transform_rec *raw_tr)
{
#if PRINT_DEBUG
	u8 i;
	char keystr[MACSEC_KEY_LEN_BYTE*2+1] = "\0";
	char scistr[MACSEC_SCI_LEN_BYTE*2+1] = "\0";
#endif

	if (!secy || !tb_sa || !raw_tr || !sci)
		return -EINVAL;

	raw_tr->sa_index = sa_index;

	if ((tb_sa[MACSEC_SA_ATTR_KEY]) && (nla_len(tb_sa[MACSEC_SA_ATTR_KEY]) != secy->key_len)) {
		pr_crit("macsec: bad key length: %d != %d\n",
			nla_len(tb_sa[MACSEC_SA_ATTR_KEY]), secy->key_len);
		return -EINVAL;
	}

	raw_tr->ccw.ca_type = (secy->key_len == MACSEC_CA128_KEY_LEN_BYTE) ? CA_AES_CTR_128 : CA_AES_CTR_256;
	if (tb_sa[MACSEC_SA_ATTR_AN]) {
		raw_tr->ccw.an = nla_get_u8(tb_sa[MACSEC_SA_ATTR_AN]);
	}

	raw_tr->ccw.sn_type = SN_32_BIT;
	raw_tr->pn_rc.seq1 = 0;

	if (tb_sa[MACSEC_SA_ATTR_KEY]) {
		nla_memcpy(raw_tr->cp.key, tb_sa[MACSEC_SA_ATTR_KEY], secy->key_len);
	}

	memcpy(&raw_tr->cp.scid, sci, MACSEC_SCI_LEN_BYTE);
	if (tb_sa[MACSEC_SA_ATTR_PN]) {
		raw_tr->pn_rc.seq0 = nla_get_u32(tb_sa[MACSEC_SA_ATTR_PN]);
	}
	raw_tr->pn_rc.mask = secy->replay_window;

	if (dev_type == EGRESS_DEVID) { /* TODO: Connect this parameters to API */
		raw_tr->sa_cw.next_sa_index = 0; /* Index of next SA (secy->tx_sc.encoding_sa) */;
		raw_tr->sa_cw.sa_expired_irq = 1;
		raw_tr->sa_cw.sa_index_valid = 0; /* Next SA index valid */
		raw_tr->sa_cw.flow_index = 1; /* sarule_num of flow rule */
		raw_tr->sa_cw.sa_ind_update_en = 1; /* SA index update enable */
	}

#if PRINT_DEBUG
	for (i = 0; i < secy->key_len; i++)
		sprintf(keystr + 2*i, "%02X", raw_tr->cp.key[i]);
	for (i = 0; i < MACSEC_SCI_LEN_BYTE; i++)
		sprintf(scistr + 2*i, "%02X", ((u8 *)&raw_tr->cp.scid)[i]);

	pr_debug("DEV=%d,\tTRANSFORM_ID=%d,\nCA_TYPE=%d,\tAN=%d,\tSN_TYPE=%d,\n"
		 "KEY=%s,\tSCI=%s,\nSEQ0=%d,\tSEQ1=%d,\tMASK=%d,\nSA_IND=%d,"
		 "\tSA_EXP_IRQ=%d,\tFLOW_IND=%d,\tSA_IND_UPD_EN=%d\n",
		 dev_type,
		 raw_tr->sa_index,
		 raw_tr->ccw.ca_type,
		 raw_tr->ccw.an,
		 raw_tr->ccw.sn_type,
		 keystr,
		 scistr,
		 raw_tr->pn_rc.seq0,
		 raw_tr->pn_rc.seq1,
		 raw_tr->pn_rc.mask,
		 raw_tr->sa_cw.next_sa_index,
		 raw_tr->sa_cw.sa_expired_irq,
		 raw_tr->sa_cw.flow_index,
		 raw_tr->sa_cw.sa_ind_update_en);
#endif

	return 0;
}

static void convert_sci_to_mac_addr(u8 *sci, u8 *mac_addr)
{
	memcpy(mac_addr, sci, 6);
}

static int update_match_rec(macsec_dev_type_t dev_type, int flow_id,
			    u8 *sci, u8 an, struct sa_match_par *match_par)
{
#if PRINT_DEBUG
	u8 i;
	char sastr[6*2+1] = "\0", dastr[6*2+1] = "\0", scistr[MACSEC_SCI_LEN_BYTE*2+1] = "\0";
#endif

	if (!sci || !match_par)
		return -EINVAL;

	match_par->rule_index = SEC_PKT_FLOW_ID(flow_id);

	memset(match_par->da_mac, 0, 6);
	convert_sci_to_mac_addr(sci, match_par->sa_mac);
	memcpy(&match_par->scid, sci, MACSEC_SCI_LEN_BYTE);
	if (dev_type == INGRESS_DEVID) {
		match_par->mask_par.macsec_sci_mask = 1;
		match_par->mask_par.tci_an_mask = 0x3;
		match_par->misc_par.macsec_tci_an = an;
		match_par->misc_par.match_priority = 0x7;
		match_par->misc_par.tagged = 1;
	} else if (dev_type == EGRESS_DEVID) {
		match_par->mask_par.mac_sa_mask = 1;
		match_par->misc_par.match_priority = 0x7;
		match_par->misc_par.untagged = 1;
	}
	match_par->flow_index = SEC_PKT_FLOW_ID(flow_id);

#if PRINT_DEBUG
	for (i = 0; i < 6; i++)
	{
		sprintf(sastr + 2*i, "%02X", match_par->sa_mac[i]);
		sprintf(dastr + 2*i, "%02X", match_par->da_mac[i]);
	}
	for (i = 0; i < MACSEC_SCI_LEN_BYTE; i++)
		sprintf(scistr + 2*i, "%02X", ((u8 *)&match_par->scid)[i]);

	pr_debug("DEV=%d,\tMATCH_ID=%d,\nSRCMAC=%s,\tDSTMAC=%s,\tSCI=%s,\t"
		 "ETH=0x%04X\n\n",
		 dev_type,
		 match_par->rule_index,
		 sastr,
		 dastr,
		 scistr,
		 match_par->eth_type);
#endif

	return 0;
}

static int update_flow_rec(macsec_dev_type_t dev_type, int flow_id,
			   int sa_index, struct macsec_secy *secy,
			   struct sam_flow_ctrl *sam_fca)
{
	sam_fca->flow_index = SEC_PKT_FLOW_ID(flow_id);

	if (dev_type == INGRESS_DEVID) {
		sam_fca->flow_type = SAM_FCA_FLOW_INGRESS;
		sam_fca->dest_port = SAM_FCA_DPT_CONTROL;
		sam_fca->replay_protect = secy->replay_protect;
		switch (secy->validate_frames) {
		case MACSEC_VALIDATE_DISABLED:
			sam_fca->validate_frames = SAM_FCA_VALIDATE_DIS;
			break;
		case MACSEC_VALIDATE_CHECK:
			sam_fca->validate_frames = SAM_FCA_VALIDATE_CHECK;
			break;
		case MACSEC_VALIDATE_STRICT:
			sam_fca->validate_frames = SAM_FCA_VALIDATE_STRICT;
			break;
		default:
			return -EINVAL;
		}
	} else if (dev_type == EGRESS_DEVID) {
		sam_fca->flow_type = SAM_FCA_FLOW_EGRESS;
		sam_fca->dest_port = SAM_FCA_DPT_COMMON;
		sam_fca->include_sci = secy->tx_sc.send_sci;
		sam_fca->use_es = secy->tx_sc.end_station;
		sam_fca->use_scb = secy->tx_sc.scb;

		sam_fca->protect_frame = secy->protect_frames;
		sam_fca->conf_protect = secy->tx_sc.encrypt;
		sam_fca->conf_offset = 0;
	}
	sam_fca->drop_non_reserved = 0;
	sam_fca->flow_crypt_auth = 0;
	sam_fca->drop_action = SAM_FCA_DROP_AS_CRC;
	sam_fca->sa_in_use = 1;
	sam_fca->sa_index = sa_index;

#if PRINT_DEBUG
	pr_debug("DEV=%d,\tFLOW_ID=%d,\nFLOW=%d,\tDES PORT=%d,\tDROP NOT RES=%d,"
		 "\tFLOW_CRYPT_AUTH=%d,\tDROP_ACT=%d,\nSA IND=%d,\tINC_SCI=%d,"
		 "\tCONF_PROTECT=%d,\tREP_PROTECT=%d,\tSA_IN_USE=%d,\tVAL_TYPE=%d"
		 "\tFRAME_PR=%d,\nUSE_ES=%d,\tUSE_SCB=%d,\tTAG_BYPASS_SIZE=%d,"
		 "\tSA_IND_UPD=%d,\tCONF_OFFSET=%d\n\n",
		 dev_type,
		 sam_fca->flow_index,
		 sam_fca->flow_type,
		 sam_fca->dest_port,
		 sam_fca->drop_non_reserved,
		 sam_fca->flow_crypt_auth,
		 sam_fca->drop_action,
		 sam_fca->sa_index,
		 sam_fca->include_sci,
		 sam_fca->conf_protect,
		 sam_fca->replay_protect,
		 sam_fca->sa_in_use,
		 sam_fca->validate_frames,
		 sam_fca->protect_frame,
		 sam_fca->use_es,
		 sam_fca->use_scb,
		 sam_fca->tag_bypass_size,
		 sam_fca->sa_index_update,
		 sam_fca->conf_offset);
#endif

	return 0;
}

static int build_auth_packet_match_rec(struct sa_match_par *match_par)
{
	if (!match_par)
		return -EINVAL;

	memset(match_par, 0, sizeof(struct sa_match_par));
	match_par->flow_index = AUTH_PKT_FLOW_ID;
	match_par->mask_par.mac_etype_mask = 1; /*ETH_TYPE Match */
	match_par->eth_type = ETH_P_PAE;
	match_par->misc_par.match_priority = 15;
	match_par->misc_par.untagged = 1;

#if PRINT_DEBUG
	pr_debug("RULE=%d,\nETH=0x%04X\n",
		match_par->flow_index,
		match_par->eth_type);
#endif

	return 0;
}

static int build_auth_packet_flow_rec(macsec_dev_type_t dev_type,
				struct sam_flow_ctrl *sam_fca)
{
	memset(sam_fca, 0, sizeof(struct sam_flow_ctrl));

	sam_fca->flow_index = AUTH_PKT_FLOW_ID;
	sam_fca->flow_type = SAM_FCA_FLOW_BYPASS;
	if (dev_type == INGRESS_DEVID) {
		sam_fca->dest_port = SAM_FCA_DPT_UNCONTROL;
	}
	else if (dev_type == EGRESS_DEVID) {
		sam_fca->dest_port = SAM_FCA_DPT_COMMON;
	}

	return 0;
}

static char * sci_to_str(u8 * sci, char * scistr)
{
	u8 i;

	scistr[0] = 0;
	for (i = 0; i < MACSEC_SCI_LEN_BYTE; i++)
		sprintf(scistr + 2*i, "%02X", sci[i]);

	return scistr;
}

static u8 macsec_find_rx_sc(struct macsec_pdata *pdata, u8 * sci)
{
	u8 idx;
	char scistr[MACSEC_SCI_LEN_BYTE*2+1] = "\0";

	for (idx = 0; idx < LGM_MAX_RX_SC; idx++) {
		if (pdata->rx_sc[idx].active && (!memcmp(pdata->rx_sc[idx].sci, sci, MACSEC_SCI_LEN_BYTE))) {
			return idx;
		}
	}

	pr_debug("SC with SCI=%s is not found\n", sci_to_str(sci, scistr));
	return INVALID_RULE_NUM;
}

static u8 macsec_allocate_rx_sc(struct macsec_pdata *pdata, u8 * sci)
{
	u8 idx;
	char scistr[MACSEC_SCI_LEN_BYTE*2+1] = "\0";

	for (idx = 0; idx < LGM_MAX_RX_SC; idx++) {
		if (!pdata->rx_sc[idx].active) {
			pdata->rx_sc[idx].active = 1;
			memcpy(pdata->rx_sc[idx].sci, sci, MACSEC_SCI_LEN_BYTE);

			pr_debug("Allocated RX SC=%d, sci=%s\n", idx,
				sci_to_str(sci, scistr));
			return idx;
		}
	}

	pr_crit("RX SC could not be allocated sci=%s", sci_to_str(sci, scistr));
	return INVALID_RULE_NUM;
}

static void macsec_free_rx_sc(struct macsec_pdata *pdata, u8 idx)
{
	pdata->rx_sc[idx].active = 0;

	pr_debug("Removed RX SC=%d\n", idx);
}

static u8 macsec_allocate_rx_rule(struct macsec_pdata *pdata)
{
	u8 rule_num;

	for (rule_num = 0; rule_num < MACSEC_MAX_SA_RULES; rule_num++) {
		if (!pdata->rx_sa[rule_num].active) {
			pdata->rx_sa[rule_num].active = 1;

			pr_debug("Allocated RX SA=%d\n", rule_num);
			return rule_num;
		}
	}

	pr_crit("RX SA could not be allocated");
	return INVALID_RULE_NUM;
}

static void macsec_free_rx_rule(struct macsec_pdata *pdata, u8 rule_num)
{
	pdata->rx_sa[rule_num].active = 0;

	pr_debug("Removed RX SA=%d\n", rule_num);
}

static int macsec_dump_cntrs(struct macsec_context *ctx, struct macsec_secy *secy)
{
	struct e160_metadata *e160_meta;
	struct macsec_pdata *pdata;
	struct macsec_rx_sc *rx_sc;
	struct pcpu_secy_stats * global_stats;
	struct pcpu_rx_sc_stats *rxsc_stats;
	struct pcpu_tx_sc_stats *txsc_stats;
	struct macsec_tx_sa_stats *txsa_stats;
	struct macsec_rx_sa_stats *rxsa_stats;
	struct egr_sa_stats egr_sa_ctrs;
	struct ing_sa_stats ing_sa_ctrs;
	struct ing_global_stats ing_global_ctrs;
	struct egr_global_stats egr_global_ctrs;
	struct sa_current_pn sa_pn;
	u8 rule_num, an, sc_id;

	pr_debug("Enter %s\n", __FUNCTION__);

	if (!ctx || !secy)
		return -EINVAL;

	pdata = get_macsec_pdata(ctx);
	if (!pdata) {
		return -EINVAL;
	}

	e160_meta = &pdata->dev;

	memset(&ing_global_ctrs, 0, sizeof(struct ing_global_stats));
	memset(&ing_global_ctrs, 0, sizeof(struct egr_global_stats));
	if (e160_get_ing_global_stats(e160_meta, &ing_global_ctrs)) {
		return -EIO;
	}
	if (e160_get_egr_global_stats(e160_meta, &egr_global_ctrs)) {
		return -EIO;
	}

	pr_debug("\nRX GLOBAL Counters:\n----------------\n");
	pr_debug("\t%40s:\t%llu\n", "TransformErrorPkts", ing_global_ctrs.TransformErrorPkts);
	pr_debug("\t%40s:\t%llu\n", "InPktsCtrl", ing_global_ctrs.InPktsCtrl);
	pr_debug("\t%40s:\t%llu\n", "InPktsTagged", ing_global_ctrs.InPktsTagged);
	pr_debug("\t%40s:\t%llu\n", "InPktsUntaggedMiss", ing_global_ctrs.InPktsUntaggedMiss);
	pr_debug("\t%40s:\t%llu\n", "InConsistCheckControlledNotPass", ing_global_ctrs.InConsistCheckControlledNotPass);
	pr_debug("\t%40s:\t%llu\n", "InConsistCheckUncontrolledNotPass", ing_global_ctrs.InConsistCheckUncontrolledNotPass);
	pr_debug("\t%40s:\t%llu\n", "InConsistCheckControlledPass", ing_global_ctrs.InConsistCheckControlledPass);
	pr_debug("\t%40s:\t%llu\n", "InConsistCheckUncontrolledPass", ing_global_ctrs.InConsistCheckUncontrolledPass);
	pr_debug("\t%40s:\t%llu\n", "InOverSizePkts", ing_global_ctrs.InOverSizePkts);
	pr_debug("\t%40s:\t%llu\n", "InPktsBadTag", ing_global_ctrs.InPktsBadTag);
	pr_debug("\t%40s:\t%llu\n", "InPktsNoSCI", ing_global_ctrs.InPktsNoSCI);
	pr_debug("\t%40s:\t%llu\n", "InPktsNoTag", ing_global_ctrs.InPktsNoTag);
	pr_debug("\t%40s:\t%llu\n", "InPktsUnknownSCI", ing_global_ctrs.InPktsUnknownSCI);
	pr_debug("\t%40s:\t%llu\n", "InPktsUntagged", ing_global_ctrs.InPktsUntagged);
	pr_debug("\nTX GLOBAL Counters:\n----------------\n");
	pr_debug("\t%40s:\t%llu\n", "TransformErrorPkts", egr_global_ctrs.TransformErrorPkts);
	pr_debug("\t%40s:\t%llu\n", "OutPktsCtrl", egr_global_ctrs.OutPktsCtrl);
	pr_debug("\t%40s:\t%llu\n", "OutPktsUnknownSA", egr_global_ctrs.OutPktsUnknownSA);
	pr_debug("\t%40s:\t%llu\n", "OutOverSizePkts", egr_global_ctrs.OutOverSizePkts);
	pr_debug("\t%40s:\t%llu\n", "OutPktsUntagged", egr_global_ctrs.OutPktsUntagged);

	rcu_read_lock_bh();
	global_stats = per_cpu_ptr(macsec_priv(secy->netdev)->stats, 0);
	u64_stats_update_begin(&global_stats->syncp);
	global_stats->stats.InPktsBadTag += ing_global_ctrs.InPktsBadTag;
	global_stats->stats.InPktsNoSCI += ing_global_ctrs.InPktsNoSCI;
	global_stats->stats.InPktsNoTag += ing_global_ctrs.InPktsNoTag;
	global_stats->stats.InPktsOverrun = 0;
	global_stats->stats.InPktsUnknownSCI += ing_global_ctrs.InPktsUnknownSCI;
	global_stats->stats.InPktsUntagged += ing_global_ctrs.InPktsUntagged;
	global_stats->stats.OutPktsTooLong += egr_global_ctrs.OutOverSizePkts;
	global_stats->stats.OutPktsUntagged += egr_global_ctrs.OutPktsUntagged;
	u64_stats_update_end(&global_stats->syncp);
	rcu_read_unlock_bh();

	for (rule_num = 0; rule_num < MACSEC_MAX_SA_RULES; rule_num++) {
		if (pdata->rx_sa[rule_num].active) {
			sc_id = pdata->rx_sa[rule_num].sc_id;
			an = pdata->rx_sa[rule_num].an;
			if (an >= MACSEC_MAX_AN) {
				pr_crit("Wrong AN value detected during RX stats parsing AN=%u",
					an);
				return -EINVAL;
			}

			memset(&ing_sa_ctrs, 0, sizeof(struct ing_sa_stats));
			ing_sa_ctrs.rule_index = rule_num;
			if (e160_get_ing_sa_stats(e160_meta, &ing_sa_ctrs)) {
				return -EIO;
			}

			memset(&sa_pn, 0, sizeof(struct sa_current_pn));
			sa_pn.sa_index = rule_num;
			if (e160_get_ing_sa_curr_pn(e160_meta, &sa_pn)) {
				return -EIO;
			}

			pr_debug("\nRX rule=%d SA Counters:\n----------------\n", rule_num);
			pr_debug("\t%40s:\t%llu\n", "InPktsUntaggedHit", ing_sa_ctrs.InPktsUntaggedHit);

			rx_sc = find_rx_sc(secy, pdata->rx_sc[sc_id].sci);
			if (rx_sc) {
				rcu_read_lock_bh();
				rxsc_stats = per_cpu_ptr(rx_sc->stats, 0);
				u64_stats_update_begin(&rxsc_stats->syncp);
				if (rx_sc->sa[an]) {
					rx_sc->sa[an]->next_pn = (u32)sa_pn.curr_pn;
					rxsa_stats = per_cpu_ptr(rx_sc->sa[an]->stats, 0);
					rxsa_stats->InPktsOK += ing_sa_ctrs.InPktsOK;
					rxsa_stats->InPktsInvalid += ing_sa_ctrs.InPktsInvalid;
					rxsa_stats->InPktsNotValid += ing_sa_ctrs.InPktsNotValid;
					rxsa_stats->InPktsNotUsingSA += ing_sa_ctrs.InPktsNotUsingSA;
					rxsa_stats->InPktsUnusedSA += ing_sa_ctrs.InPktsUnusedSA;
				} else {
					pr_warn("RX SA pointer is NULL for rule = %u, AN = %u", rule_num, an);
				}

				rxsc_stats->stats.InOctetsDecrypted += (__u64)ing_sa_ctrs.InOctetsDecrypted + ((__u64)ing_sa_ctrs.InOctetsDecrypted2<<32);
				rxsc_stats->stats.InPktsDelayed += ing_sa_ctrs.InPktsDelayed;
				rxsc_stats->stats.InPktsInvalid += ing_sa_ctrs.InPktsInvalid;
				rxsc_stats->stats.InPktsLate += ing_sa_ctrs.InPktsLate;
				rxsc_stats->stats.InPktsNotUsingSA += ing_sa_ctrs.InPktsNotUsingSA;
				rxsc_stats->stats.InPktsNotValid += ing_sa_ctrs.InPktsNotValid;
				rxsc_stats->stats.InPktsOK += ing_sa_ctrs.InPktsOK;
				rxsc_stats->stats.InPktsUnchecked += ing_sa_ctrs.InPktsUnchecked;
				rxsc_stats->stats.InPktsUnusedSA += ing_sa_ctrs.InPktsUnusedSA;
				u64_stats_update_end(&rxsc_stats->syncp);
				rcu_read_unlock_bh();
			} else {
				pr_warn("RX SC pointer is NULL for rule = %u SC_ID = %u", rule_num, sc_id);
			}
		}
	}

	rcu_read_lock_bh();
	txsc_stats = per_cpu_ptr(secy->tx_sc.stats, 0);
	u64_stats_update_begin(&txsc_stats->syncp);
	for (rule_num = 0; rule_num < MACSEC_MAX_SA_RULES; rule_num++) {
		if (pdata->tx_sa[rule_num].active) {
			an = pdata->tx_sa[rule_num].an;
			if (an >= MACSEC_MAX_AN) {
				pr_crit("Wrong AN value detected during TX stats parsing AN=%u",
					an);
				return -EINVAL;
			}

			memset(&egr_sa_ctrs, 0, sizeof(struct egr_sa_stats));
			egr_sa_ctrs.rule_index = rule_num;
			if (e160_get_egr_sa_stats(e160_meta, &egr_sa_ctrs)) {
				return -EIO;
			}

			memset(&sa_pn, 0, sizeof(struct sa_current_pn));
			sa_pn.sa_index = rule_num;
			if (e160_get_egr_sa_curr_pn(e160_meta, &sa_pn)) {
				return -EIO;
			}

			pr_debug("\nTX rule=%d SA Counters:\n----------------\n", rule_num);
			pr_debug("\t%40s:\t%llu\n", "OutPktsTooLong", egr_sa_ctrs.OutPktsTooLong);

			if (secy->tx_sc.sa[an]) {
				secy->tx_sc.sa[an]->next_pn = (u32)sa_pn.curr_pn;
				txsa_stats = per_cpu_ptr(secy->tx_sc.sa[an]->stats, 0);
				txsa_stats->OutPktsEncrypted += egr_sa_ctrs.OutPktsEncrypted;
			} else {
				pr_warn("TX SA pointer is NULL for rule = %u", rule_num);
			}

			txsc_stats->stats.OutOctetsEncrypted += (__u64)egr_sa_ctrs.OutOctetsEncrypted + ((__u64)egr_sa_ctrs.OutOctetsEncrypted2<<32);
			txsc_stats->stats.OutPktsEncrypted += egr_sa_ctrs.OutPktsEncrypted;

		}
	}
	u64_stats_update_end(&txsc_stats->syncp);
	rcu_read_unlock_bh();

	pr_debug("Exit %s\n", __FUNCTION__);
	return 0;
}

static int e160_set_rule_state(struct e160_metadata *e160_meta,
				macsec_dev_type_t dev_type, u8 flow_id, bool state)
{
	struct sam_enable_ctrl ee_ctrl;

	memset(&ee_ctrl, 0, sizeof(struct sam_enable_ctrl));
	if (state) {
		ee_ctrl.sam_index_set = SEC_PKT_FLOW_ID(flow_id);
		ee_ctrl.set_enable = 1;
	} else {
		ee_ctrl.sam_index_clear = SEC_PKT_FLOW_ID(flow_id);
		ee_ctrl.clear_enable = 1;
	}

	pr_debug("SET STATE RULE=%d,\tDEV=%d,\tSTATE=%d\n",
		ee_ctrl.sam_index_set,
		dev_type,
		state);

	if (dev_type == INGRESS_DEVID) {
		if (e160_cfg_ing_sam_ena_ctrl(e160_meta, &ee_ctrl))
			return -EIO;
	}
	else if (dev_type == EGRESS_DEVID) {
		if (e160_cfg_egr_sam_ena_ctrl(e160_meta, &ee_ctrl))
			return -EIO;
	}

	return 0;
}

static int macsec_remove_rx_sa(struct macsec_pdata *pdata, u8 rule_num)
{
	struct e160_metadata *e160_meta = &pdata->dev;

	if (pdata->rx_sa[rule_num].enabled) {
		if (e160_set_rule_state(e160_meta, INGRESS_DEVID, rule_num, false)) {
			return -EIO;
		}
		pdata->rx_sa[rule_num].enabled = false;
		memset(&pdata->rx_sa[rule_num].raw_tr, 0, sizeof(struct transform_rec));
	}
	macsec_free_rx_rule(pdata, rule_num);

	return 0;
}

static void macsec_remove_rx_sc(struct macsec_pdata *pdata, u8 rx_sc_id)
{
	u8 an, rule_num;

	for (an=0; an < MACSEC_MAX_AN; an++) {
		rule_num = pdata->rx_sc[rx_sc_id].raw_tr_ids[an];
		if (pdata->rx_sa[rule_num].active) {
			macsec_remove_rx_sa(pdata, rule_num);
		}
	}
	macsec_free_rx_sc(pdata, rx_sc_id);
}

/* RX SA */
static int macsec_add_rxsa(struct macsec_context *ctx, struct macsec_secy *secy,
				struct nlattr **tb_rxsc, struct nlattr **tb_sa)
{
	struct e160_metadata *e160_meta;
	struct macsec_pdata *pdata;
	u8 rule_num, rx_sc_id, *sci, an;

	pr_debug("Enter %s\n", __FUNCTION__);

	if (!ctx || !secy || !tb_rxsc || !tb_sa)
		return -EINVAL;

	pdata = get_macsec_pdata(ctx);
	if (!pdata) {
		return -EINVAL;
	}

	e160_meta = &pdata->dev;

	sci = (u8 *)nla_data(tb_rxsc[MACSEC_RXSC_ATTR_SCI]);
	an = nla_get_u8(tb_sa[MACSEC_SA_ATTR_AN]);
	rx_sc_id = macsec_find_rx_sc(pdata, sci);
	if (rx_sc_id == INVALID_RULE_NUM) {
		return -EINVAL;
	}

	rule_num = macsec_allocate_rx_rule(pdata);
	if (rule_num == INVALID_RULE_NUM) {
		return -ENOMEM;
	}
	pdata->rx_sc[rx_sc_id].raw_tr_ids[an] = rule_num;
	pdata->rx_sa[rule_num].an = an;

	if (tb_sa[MACSEC_SA_ATTR_ACTIVE]) {
		pdata->rx_sa[rule_num].enabled = !!nla_get_u8(tb_sa[MACSEC_SA_ATTR_ACTIVE]);
	}

	/* Reset All structures */
	memset(&pdata->rx_sa[rule_num].raw_tr, 0, sizeof(struct transform_rec));
	memset(&pdata->rx_sa[rule_num].match_par, 0, sizeof(struct sa_match_par));
	memset(&pdata->rx_sa[rule_num].sam_fca, 0, sizeof(struct sam_flow_ctrl));
	if (update_trans_rec(INGRESS_DEVID, rule_num, secy, sci, tb_sa, &pdata->rx_sa[rule_num].raw_tr)) {
		return -EINVAL;
	}
	if (e160_cfg_ing_trans_rec(e160_meta, &pdata->rx_sa[rule_num].raw_tr)) {
		return -EIO;
	}

	if (update_match_rec(INGRESS_DEVID, rule_num, sci, an, &pdata->rx_sa[rule_num].match_par)) {
		return -EINVAL;
	}
	if (e160_cfg_ing_sam_rule(e160_meta, &pdata->rx_sa[rule_num].match_par)) {
		return -EIO;
	}

	if (update_flow_rec(INGRESS_DEVID, rule_num, rule_num, secy, &pdata->rx_sa[rule_num].sam_fca)) {
		return -EINVAL;
	}
	if (e160_cfg_ing_sam_flow_ctrl(e160_meta, &pdata->rx_sa[rule_num].sam_fca)) {
		return -EIO;
	}

	if (pdata->rx_sa[rule_num].enabled) {
		if (e160_set_rule_state(e160_meta, INGRESS_DEVID, rule_num, true)) {
			return -EIO;
		}
	}

	pr_debug("Exit %s\n", __FUNCTION__);
	return 0;
}

static int macsec_upd_rxsa(struct macsec_context *ctx, struct macsec_secy *secy,
			   struct nlattr **tb_rxsc, struct nlattr **tb_sa)
{
	struct e160_metadata *e160_meta;
	struct macsec_pdata *pdata;
	u8 rule_num, rx_sc_id, *sci, an;

	pr_debug("Enter %s\n", __FUNCTION__);

	if (!ctx || !secy || !tb_rxsc || !tb_sa)
		return -EINVAL;

	pdata = get_macsec_pdata(ctx);
	if (!pdata) {
		return -EINVAL;
	}

	e160_meta = &pdata->dev;

	sci = (u8 *)nla_data(tb_rxsc[MACSEC_RXSC_ATTR_SCI]);
	an = nla_get_u8(tb_sa[MACSEC_SA_ATTR_AN]);
	rx_sc_id = macsec_find_rx_sc(pdata, sci);
	if (rx_sc_id == INVALID_RULE_NUM) {
		return -EINVAL;
	}
	rule_num = pdata->rx_sc[rx_sc_id].raw_tr_ids[an];

	if (pdata->rx_sa[rule_num].enabled) {
		if (e160_set_rule_state(e160_meta, INGRESS_DEVID, rule_num, false)) {
			return -EIO;
		}
	}

	if (tb_sa[MACSEC_SA_ATTR_ACTIVE]) {
		pdata->rx_sa[rule_num].enabled = !!nla_get_u8(tb_sa[MACSEC_SA_ATTR_ACTIVE]);
	}

	if (update_trans_rec(INGRESS_DEVID, rule_num, secy, sci, tb_sa, &pdata->rx_sa[rule_num].raw_tr)) {
		return -EINVAL;
	}
	if (update_match_rec(INGRESS_DEVID, rule_num, sci, an, &pdata->rx_sa[rule_num].match_par)) {
		return -EINVAL;
	}
	if (update_flow_rec(INGRESS_DEVID, rule_num, rule_num, secy, &pdata->rx_sa[rule_num].sam_fca)) {
		return -EINVAL;
	}

	if (pdata->rx_sa[rule_num].enabled) {
		if (e160_cfg_ing_trans_rec(e160_meta, &pdata->rx_sa[rule_num].raw_tr)) {
			return -EIO;
		}
		if (e160_cfg_ing_sam_flow_ctrl(e160_meta, &pdata->rx_sa[rule_num].sam_fca)) {
			return -EIO;
		}
		if (e160_cfg_ing_sam_rule(e160_meta, &pdata->rx_sa[rule_num].match_par)) {
			return -EIO;
		}
		if (e160_set_rule_state(e160_meta, INGRESS_DEVID, rule_num, true)) {
			return -EIO;
		}
	}

	pr_debug("Exit %s\n", __FUNCTION__);
	return 0;
}

static int macsec_del_rxsa(struct macsec_context *ctx, struct macsec_secy *secy,
			   struct nlattr **tb_rxsc, struct nlattr **tb_sa)
{
	struct macsec_pdata *pdata;
	u8 rule_num, rx_sc_id, *sci, an;

	pr_debug("Enter %s\n", __FUNCTION__);

	if (!ctx || !secy || !tb_rxsc || !tb_sa)
		return -EINVAL;

	pdata = get_macsec_pdata(ctx);
	if (!pdata) {
		return -EINVAL;
	}

	sci = (u8 *)nla_data(tb_rxsc[MACSEC_RXSC_ATTR_SCI]);
	an = nla_get_u8(tb_sa[MACSEC_SA_ATTR_AN]);
	rx_sc_id = macsec_find_rx_sc(pdata, sci);
	if (rx_sc_id == INVALID_RULE_NUM) {
		return -EINVAL;
	}
	rule_num = pdata->rx_sc[rx_sc_id].raw_tr_ids[an];
	macsec_remove_rx_sa(pdata, rule_num);

	pr_debug("Exit %s\n", __FUNCTION__);
	return 0;
}

/* RX SC */
static int macsec_add_rxsc(struct macsec_context *ctx, struct macsec_secy *secy,
			   struct nlattr **tb_rxsc)
{
	struct macsec_pdata *pdata;
	u8 rx_sc_id, *sci;

	pr_debug("Enter %s\n", __FUNCTION__);

	if (!ctx || !secy || !tb_rxsc)
		return -EINVAL;

	pdata = get_macsec_pdata(ctx);
	if (!pdata) {
		return -EINVAL;
	}

	sci = (u8 *)nla_data(tb_rxsc[MACSEC_RXSC_ATTR_SCI]);
	rx_sc_id = macsec_find_rx_sc(pdata, sci);
	if (rx_sc_id == INVALID_RULE_NUM) {
		rx_sc_id = macsec_allocate_rx_sc(pdata, sci);
		if (rx_sc_id == INVALID_RULE_NUM) {
			return -ENOMEM;
		}
	}

	pr_debug("Exit %s\n", __FUNCTION__);
	return 0;
}

static int macsec_upd_rxsc(struct macsec_context *ctx, struct macsec_secy *secy,
			   struct nlattr **tb_rxsc)
{
	pr_warn("WARNING: Command is not supported\n");

	return 0;
}

static int macsec_del_rxsc(struct macsec_context *ctx, struct macsec_secy *secy,
			   struct nlattr **tb_rxsc)
{
	struct macsec_pdata *pdata;
	u8 rx_sc_id, *sci;

	pr_debug("Enter %s\n", __FUNCTION__);

	if (!ctx || !secy || !tb_rxsc)
		return -EINVAL;

	pdata = get_macsec_pdata(ctx);
	if (!pdata) {
		return -EINVAL;
	}

	sci = (u8 *)nla_data(tb_rxsc[MACSEC_RXSC_ATTR_SCI]);
	rx_sc_id = macsec_find_rx_sc(pdata, sci);
	if (rx_sc_id == INVALID_RULE_NUM) {
		return -EINVAL;
	}
	macsec_remove_rx_sc(pdata, rx_sc_id);

	pr_debug("Exit %s\n", __FUNCTION__);
	return 0;
}

/* TX SA */
static int macsec_add_txsa(struct macsec_context *ctx, struct macsec_secy *secy,
			   struct nlattr **tb_sa)
{
	struct e160_metadata *e160_meta;
	struct macsec_pdata *pdata;
	u8 an, raw_tr_id, *sci;

	pr_debug("Enter %s\n", __FUNCTION__);

	if (!ctx || !secy || !tb_sa)
		return -EINVAL;

	pdata = get_macsec_pdata(ctx);
	if (!pdata) {
		return -EINVAL;
	}

	e160_meta = &pdata->dev;

	sci = (u8*)&secy->sci;
	an = nla_get_u8(tb_sa[MACSEC_SA_ATTR_AN]);
	raw_tr_id = pdata->tx_sc.raw_tr_ids[an];
	pdata->tx_sa[raw_tr_id].an = an;

	if (tb_sa[MACSEC_SA_ATTR_ACTIVE]) {
		pdata->tx_sa[raw_tr_id].enabled = !!nla_get_u8(tb_sa[MACSEC_SA_ATTR_ACTIVE]);
	}

	if (!pdata->tx_sc.active) {
		pdata->tx_sc.active = 1;
		memset(&pdata->tx_sc.match_par, 0, sizeof(struct sa_match_par));
		memset(&pdata->tx_sc.sam_fca, 0, sizeof(struct sam_flow_ctrl));

		if (update_match_rec(EGRESS_DEVID, 0, sci, 0, &pdata->tx_sc.match_par)) {
			return -EINVAL;
		}
		secy->tx_sc.encoding_sa = an;
		if (update_flow_rec(EGRESS_DEVID, 0, raw_tr_id, secy, &pdata->tx_sc.sam_fca)) {
			return -EINVAL;
		}
		if (e160_cfg_egr_sam_rule(e160_meta, &pdata->tx_sc.match_par)) {
			return -EIO;
		}
		if (e160_cfg_egr_sam_flow_ctrl(e160_meta, &pdata->tx_sc.sam_fca)) {
			return -EIO;
		}
	}
	pdata->tx_sa[raw_tr_id].active = 1;
	memset(&pdata->tx_sa[raw_tr_id].raw_tr, 0, sizeof(struct transform_rec));
	if (update_trans_rec(EGRESS_DEVID, raw_tr_id, secy, sci, tb_sa, &pdata->tx_sa[raw_tr_id].raw_tr)) {
		return -EINVAL;
	}
	if (e160_cfg_egr_trans_rec(e160_meta, &pdata->tx_sa[raw_tr_id].raw_tr)) {
		return -EIO;
	}

	if (pdata->tx_sa[raw_tr_id].enabled && (secy->tx_sc.encoding_sa == an)) {
		if (e160_set_rule_state(e160_meta, EGRESS_DEVID, 0, true)) {
			return -EIO;
		}
	}

	pr_debug("Exit %s\n", __FUNCTION__);
	return 0;
}

static int macsec_upd_txsa(struct macsec_context *ctx, struct macsec_secy *secy,
			   struct nlattr **tb_sa)
{
	struct e160_metadata *e160_meta;
	struct macsec_pdata *pdata;
	u8 an, raw_tr_id, *sci;

	pr_debug("Enter %s\n", __FUNCTION__);

	if (!ctx || !secy || !tb_sa)
		return -EINVAL;

	pdata = get_macsec_pdata(ctx);
	if (!pdata) {
		return -EINVAL;
	}

	e160_meta = &pdata->dev;

	sci = (u8*)&secy->sci;
	an = nla_get_u8(tb_sa[MACSEC_SA_ATTR_AN]);
	raw_tr_id = pdata->tx_sc.raw_tr_ids[an];

	if (pdata->tx_sa[raw_tr_id].enabled && (secy->tx_sc.encoding_sa == an)) {
		if (e160_set_rule_state(e160_meta, EGRESS_DEVID, 0, false)) {
			return -EIO;
		}
	}

	if (tb_sa[MACSEC_SA_ATTR_ACTIVE]) {
		pdata->tx_sa[raw_tr_id].enabled = !!nla_get_u8(tb_sa[MACSEC_SA_ATTR_ACTIVE]);
	}

	if (update_trans_rec(EGRESS_DEVID, raw_tr_id, secy, sci, tb_sa, &pdata->tx_sa[raw_tr_id].raw_tr)) {
		return -EINVAL;
	}
	if (e160_cfg_egr_trans_rec(e160_meta, &pdata->tx_sa[raw_tr_id].raw_tr)) {
		return -EIO;
	}

	if (pdata->tx_sa[raw_tr_id].enabled && (secy->tx_sc.encoding_sa == an)) {
		if (e160_set_rule_state(e160_meta, EGRESS_DEVID, 0, true)) {
			return -EIO;
		}
	}

	pr_debug("Exit %s\n", __FUNCTION__);
	return 0;
}

static int macsec_del_txsa(struct macsec_context *ctx, struct macsec_secy *secy,
			   struct nlattr **tb_sa)
{
	struct e160_metadata *e160_meta;
	struct macsec_pdata *pdata;
	u8 an, raw_tr_id;

	pr_debug("Enter %s\n", __FUNCTION__);

	if (!ctx || !secy || !tb_sa)
		return -EINVAL;

	pdata = get_macsec_pdata(ctx);
	if (!pdata) {
		return -EINVAL;
	}

	e160_meta = &pdata->dev;

	an = nla_get_u8(tb_sa[MACSEC_SA_ATTR_AN]);
	raw_tr_id = pdata->tx_sc.raw_tr_ids[an];
	pdata->tx_sa[raw_tr_id].active = 0;
	pdata->tx_sa[raw_tr_id].enabled = false;

	if (secy->tx_sc.encoding_sa == an) {
		if (e160_set_rule_state(e160_meta, EGRESS_DEVID, 0, false)) {
			return -EIO;
		}
	}

	pr_debug("Exit %s\n", __FUNCTION__);
	return 0;
}

static int macsec_change_link(struct macsec_context *ctx, struct macsec_secy *secy)
{
	struct e160_metadata *e160_meta;
	struct macsec_pdata *pdata;
	u8 raw_tr_id;

	pr_debug("Enter %s\n", __FUNCTION__);

	if (!ctx || !secy)
		return -EINVAL;

	pdata = get_macsec_pdata(ctx);
	if (!pdata) {
		return -EINVAL;
	}

	e160_meta = &pdata->dev;

	/* Update Egress flow record according to new TX SA */
	if (update_flow_rec(EGRESS_DEVID, 0, pdata->tx_sc.raw_tr_ids[secy->tx_sc.encoding_sa], secy, &pdata->tx_sc.sam_fca)) {
		return -EINVAL;
	}
	if (e160_cfg_egr_sam_flow_ctrl(e160_meta, &pdata->tx_sc.sam_fca)) {
		return -EIO;
	}

	raw_tr_id = pdata->tx_sc.raw_tr_ids[secy->tx_sc.encoding_sa];
	if (e160_set_rule_state(e160_meta, EGRESS_DEVID, 0, pdata->tx_sa[raw_tr_id].enabled)) {
		return -EIO;
	}

	pr_debug("Exit %s\n", __FUNCTION__);
	return 0;
}

static int macsec_load_static_rules(struct macsec_pdata *pdata)
{
	struct e160_metadata *e160_meta = &pdata->dev;
	u8 an;
	struct sa_match_par match_par;
	struct sam_flow_ctrl sam_fca;
	struct sam_enable_ctrl ee_ctrl;
	struct sanm_flow_ctrl nm_cp;

	for (an = 0; an < MACSEC_MAX_AN; an++ ) {
		pdata->tx_sc.raw_tr_ids[an] = an;
	}

	/* BYPASS Auth packet configuration */
	build_auth_packet_match_rec(&match_par);
	if (e160_cfg_ing_sam_rule(e160_meta, &match_par)
	    || e160_cfg_egr_sam_rule(e160_meta, &match_par))
		return -EIO;

	build_auth_packet_flow_rec(INGRESS_DEVID, &sam_fca);
	if (e160_cfg_ing_sam_flow_ctrl(e160_meta, &sam_fca))
		return -EIO;
	build_auth_packet_flow_rec(EGRESS_DEVID, &sam_fca);
	if (e160_cfg_egr_sam_flow_ctrl(e160_meta, &sam_fca))
		return -EIO;

	/* Enable flow rules */
	memset(&ee_ctrl, 0, sizeof(struct sam_enable_ctrl));
	ee_ctrl.sam_index_set = AUTH_PKT_FLOW_ID;
	ee_ctrl.set_enable = 1;
	if (e160_cfg_ing_sam_ena_ctrl(e160_meta, &ee_ctrl)
	    || e160_cfg_egr_sam_ena_ctrl(e160_meta, &ee_ctrl))
		return -EIO;

	/* Set mode:
	 *   strict: drop all packets which are not hitting any rules
	 *   linux:  allow untagged packets for (un)controlled ports,
	 *           RX tagged packets for uncontrolled port,
	 *           KaY tagged packets for uncontrolled port
	 * Use linux mode
	 */
	/* RX configuration */
	nm_cp.nctrl_pkt.dest_port = nm_cp.ctrl_pkt.dest_port
				  = SA_NM_FCA_DPT_CONTROL;
	nm_cp.nctrl_pkt.drop_action = nm_cp.ctrl_pkt.drop_action
				    = SA_NM_FCA_DROP_AS_CRC;
	nm_cp.nctrl_pkt.drop_non_reserved = nm_cp.ctrl_pkt.drop_non_reserved
					  = 0;

	nm_cp.pkt_type = SA_NM_SECY_UNTAGGED;
	nm_cp.nctrl_pkt.flow_type = SA_NM_FCA_FLOW_BYPASS;
	nm_cp.ctrl_pkt.flow_type = SA_NM_FCA_FLOW_BYPASS;
	if (e160_cfg_ing_sanm_flow_ctrl(e160_meta, &nm_cp))
		return -EIO;

	nm_cp.pkt_type = SA_NM_SECY_TAGGED;
	nm_cp.nctrl_pkt.flow_type = SA_NM_FCA_FLOW_BYPASS;
	nm_cp.ctrl_pkt.flow_type = SA_NM_FCA_FLOW_DROP;
	if (e160_cfg_ing_sanm_flow_ctrl(e160_meta, &nm_cp))
		return -EIO;

	nm_cp.pkt_type = SA_NM_SECY_BAD_TAGGED;
	nm_cp.nctrl_pkt.flow_type = SA_NM_FCA_FLOW_DROP;
	nm_cp.ctrl_pkt.flow_type = SA_NM_FCA_FLOW_DROP;
	if (e160_cfg_ing_sanm_flow_ctrl(e160_meta, &nm_cp))
		return -EIO;

	nm_cp.pkt_type = SA_NM_SECY_KAY_TAGGED;
	nm_cp.nctrl_pkt.flow_type = SA_NM_FCA_FLOW_BYPASS;
	nm_cp.ctrl_pkt.flow_type = SA_NM_FCA_FLOW_DROP;
	if (e160_cfg_ing_sanm_flow_ctrl(e160_meta, &nm_cp))
		return -EIO;

	/* TX configuration */
	nm_cp.nctrl_pkt.dest_port = nm_cp.ctrl_pkt.dest_port
				  = SA_NM_FCA_DPT_COMMON;
	nm_cp.nctrl_pkt.drop_action = nm_cp.ctrl_pkt.drop_action
				    = SA_NM_FCA_DROP_AS_CRC;

	nm_cp.pkt_type = SA_NM_SECY_UNTAGGED;
	nm_cp.nctrl_pkt.flow_type = SA_NM_FCA_FLOW_BYPASS;
	nm_cp.ctrl_pkt.flow_type = SA_NM_FCA_FLOW_BYPASS;
	if (e160_cfg_egr_sanm_flow_ctrl(e160_meta, &nm_cp))
		return -EIO;

	nm_cp.pkt_type = SA_NM_SECY_TAGGED;
	nm_cp.nctrl_pkt.flow_type = SA_NM_FCA_FLOW_DROP;
	nm_cp.ctrl_pkt.flow_type = SA_NM_FCA_FLOW_DROP;
	if (e160_cfg_egr_sanm_flow_ctrl(e160_meta, &nm_cp))
		return -EIO;

	nm_cp.pkt_type = SA_NM_SECY_BAD_TAGGED;
	nm_cp.nctrl_pkt.flow_type = SA_NM_FCA_FLOW_DROP;
	nm_cp.ctrl_pkt.flow_type = SA_NM_FCA_FLOW_DROP;
	if (e160_cfg_egr_sanm_flow_ctrl(e160_meta, &nm_cp))
		return -EIO;

	nm_cp.pkt_type = SA_NM_SECY_KAY_TAGGED;
	nm_cp.nctrl_pkt.flow_type = SA_NM_FCA_FLOW_BYPASS;
	nm_cp.ctrl_pkt.flow_type = SA_NM_FCA_FLOW_DROP;
	if (e160_cfg_egr_sanm_flow_ctrl(e160_meta, &nm_cp))
		return -EIO;

	return 0;
}

static int macsec_disable_all_rules(struct macsec_pdata *pdata)
{
	struct e160_metadata *e160_meta = &pdata->dev;
	struct sam_enable_ctrl ee_ctrl;
	u8 an, raw_tr_id, rx_sc_id;

	/* Disable flow rules */
	memset(&ee_ctrl, 0, sizeof(struct sam_enable_ctrl));
	ee_ctrl.sam_index_clear = AUTH_PKT_FLOW_ID;
	ee_ctrl.clear_enable = 1;
	if (e160_cfg_ing_sam_ena_ctrl(e160_meta, &ee_ctrl)) {
		return -EIO;
	}
	if (e160_cfg_egr_sam_ena_ctrl(e160_meta, &ee_ctrl)) {
		return -EIO;
	}

	/* Disable all TX SC rules */
	pdata->tx_sc.active = 0;
	if (e160_set_rule_state(e160_meta, EGRESS_DEVID, 0, false)) {
		return -EIO;
	}
	for (an=0; an < MACSEC_MAX_AN; an++) {
		raw_tr_id = pdata->tx_sc.raw_tr_ids[an];
		pdata->tx_sa[raw_tr_id].active = 0;
		pdata->tx_sa[raw_tr_id].enabled = false;
		memset(&pdata->tx_sa[raw_tr_id].raw_tr, 0, sizeof(struct transform_rec));
	}
	/* Disable all RX SC rules */
	for (rx_sc_id = 0; rx_sc_id < LGM_MAX_RX_SC; rx_sc_id++) {
		macsec_remove_rx_sc(pdata, rx_sc_id);
	}

	return 0;
}

#if ENABLE_NETDEV_NOTIFIER
static int macsec_reload_all_rules(struct macsec_pdata *pdata)
{
	struct e160_metadata *e160_meta = &pdata->dev;
	u8 an, raw_tr_id, rx_sc_id, rx_sc_enabled;

	/* Restore TX SC */
	if (pdata->tx_sc.active) {
		/* Restore TX match and flow rules */
		if (e160_cfg_egr_sam_rule(e160_meta, &pdata->tx_sc.match_par)) {
			return -EIO;
		}
		if (e160_cfg_egr_sam_flow_ctrl(e160_meta, &pdata->tx_sc.sam_fca)) {
			return -EIO;
		}

		/* Restore all TX SA rules */
		rx_sc_enabled = 0;
		for (an=0; an < MACSEC_MAX_AN; an++) {
			raw_tr_id = pdata->tx_sc.raw_tr_ids[an];

			if (pdata->tx_sa[raw_tr_id].active) {
				if (e160_cfg_egr_trans_rec(e160_meta, &pdata->tx_sa[raw_tr_id].raw_tr)) {
					return -EIO;
				}
			}
			if (pdata->tx_sa[raw_tr_id].enabled && !rx_sc_enabled) {
				rx_sc_enabled = 1;
			}
		}

		if (rx_sc_enabled) {
			if (e160_set_rule_state(e160_meta, EGRESS_DEVID, 0, true)) {
				return -EIO;
			}
		}
	}

	/* Restore RX SC */
	for (rx_sc_id = 0; rx_sc_id < LGM_MAX_RX_SC; rx_sc_id++) {
		if (pdata->rx_sc[rx_sc_id].active) {
			/* Restore all RX SA rules */
			for (an=0; an < MACSEC_MAX_AN; an++) {
				raw_tr_id = pdata->rx_sc[rx_sc_id].raw_tr_ids[an];

				if (pdata->rx_sa[raw_tr_id].active) {
					/* Restore TX match and flow rules */
					if (e160_cfg_ing_trans_rec(e160_meta, &pdata->rx_sa[raw_tr_id].raw_tr)) {
						return -EIO;
					}
					if (e160_cfg_ing_sam_rule(e160_meta, &pdata->rx_sa[raw_tr_id].match_par)) {
						return -EIO;
					}

					if (e160_cfg_ing_sam_flow_ctrl(e160_meta, &pdata->rx_sa[raw_tr_id].sam_fca)) {
						return -EIO;
					}
					if (e160_set_rule_state(e160_meta, INGRESS_DEVID, raw_tr_id, true)) {
						return -EIO;
					}
				}
			}
		}
	}

	return 0;
}
#endif

/* Attach MACsec to MAC and initialize SecY */
static int macsec_init(struct macsec_context *ctx, struct macsec_secy *secy)
{
	int ret;
	struct macsec_pdata *pdata;
	struct e160_metadata *e160_meta;

	pr_debug("Enter %s\n", __FUNCTION__);

	if (!ctx)
		return -EINVAL;

	pdata = get_macsec_pdata(ctx);
	if (!pdata) {
		return -EINVAL;
	}

	if (pdata->initialized)
		/* only 1 SecY is supported */
		return -EEXIST;

	ret = pdata->dev.prepare(ctx, NULL, NULL);
	if (ret < 0)
		return ret;

	e160_meta = &pdata->dev;

	/* MACsec module initialization */
	ret = e160_init_ing_dev(e160_meta);
	if (ret < 0) {
		pr_err("\nERROR: Init ING dev failed.\n");
		goto ERROR;
	}

	ret = e160_init_egr_dev(e160_meta);
	if (ret < 0) {
		pr_err("\nERROR: Init EGR dev failed.\n");
		goto ERROR;
	}

	ret = macsec_load_static_rules(pdata);
	if (ret < 0) {
		pr_err("\nERROR: Loading static rules failed.\n");
		goto ERROR;
	}

	pdata->initialized = true;
	pr_debug("Exit %s\n", __FUNCTION__);
	return 0;

ERROR:
	pdata->dev.unprepare(ctx);
	pr_debug("Exit %s\n", __FUNCTION__);
	return ret;
}

/* Detach MACsec from MAC and uninitialize SecY */
static int macsec_exit(struct macsec_context *ctx, struct macsec_secy *secy)
{
	int ret;
	struct macsec_pdata *pdata;

	pr_debug("Enter %s\n", __FUNCTION__);

	if (!ctx)
		return -EINVAL;

	pdata = get_macsec_pdata(ctx);
	if (!pdata) {
		return -EINVAL;
	}

	if (!pdata->initialized)
		return 0;

	ret = macsec_disable_all_rules(pdata);
	pdata->dev.unprepare(ctx);
	pdata->initialized = false;

	if (ret < 0) {
		pr_err("\nERROR: API macsec_disable_all_rules failed.\n");
		return ret;
	}

	pr_debug("Exit %s\n", __FUNCTION__);

	return 0;
}

#if ENABLE_NETDEV_NOTIFIER
static int lgm_chk_init_state(struct e160_metadata *e160_meta)
{
	int ret;
	u32 _module_base, _module_offset, _absreg_addr;
	u32 _ig_misc_ctrl, _eg_misc_ctrl;

	_module_base = ING_CLASSIFIER_VARIOUS_CONTROL_REGS_MODULE_BASE;
	_module_offset = EIPI_MISC_CONTROL;
	_absreg_addr = _module_base + _module_offset;

	ret = IG_REG_READ(e160_meta, _absreg_addr, &_ig_misc_ctrl);
	if (ret < 0) {
		pr_crit("ERROR: Ing Misc Control word get failed\n");
		return ret;
	}

	_module_base = EGR_CLASSIFIER_VARIOUS_CONTROL_REGS_MODULE_BASE;
	_module_offset = EIPE_MISC_CONTROL;
	_absreg_addr = _module_base + _module_offset;

	ret = EG_REG_READ(e160_meta, _absreg_addr, &_eg_misc_ctrl);
	if (ret < 0) {
		pr_crit("ERROR: Egr Misc Control word get failed\n");
		return ret;
	}

	if ((_ig_misc_ctrl == ING_MISC_CTRL_INIT_VAL) &&
		(_eg_misc_ctrl == EGR_MISC_CTRL_INIT_VAL)){
		return 1; /* Engine is in initialized state */
	}

	/* Engine is in non-initialized state */
	return 0;
}

static int lgm_macsec_reinit(struct macsec_context *ctx)
{
	int ret;
	struct macsec_pdata *pdata;

	if (!ctx)
		return -EINVAL;

	pdata = get_macsec_pdata(ctx);
	if (!pdata) {
		return -EINVAL;
	}

	/* Check MACsec linux device is created and operational */
	if (pdata->initialized) {

		/* Check if HW engine is initialized of after reset */
		ret = lgm_chk_init_state(&pdata->dev);
		if (ret < 0) {
			return -EIO;
		}

		/* If HW engine is not initialized do HW reinit */
		if (ret == 0) {
			pdata->initialized = false;
			if (macsec_init(ctx, NULL)) {
				return -1;
			}

			if (macsec_reload_all_rules(pdata)) {
				return -1;
			}
			pr_info("Macsec HW engine reinitialization completed\n");
		} else {
			pr_warn("MACsec HW engine is already initialized\n");
		}
	}

	return 0;
}
#endif

static const struct macsec_ops macsec_ops = {
	.add_secy = macsec_init,
	.del_secy = macsec_exit,
	.add_rxsc = macsec_add_rxsc,
	.del_rxsc = macsec_del_rxsc,
	.upd_rxsc = macsec_upd_rxsc,
	.add_txsa = macsec_add_txsa,
	.del_txsa = macsec_del_txsa,
	.upd_txsa = macsec_upd_txsa,
	.add_rxsa = macsec_add_rxsa,
	.del_rxsa = macsec_del_rxsa,
	.upd_rxsa = macsec_upd_rxsa,
	.dump = macsec_dump_cntrs,
	.change_link = macsec_change_link,
};

int e160_register(const struct e160_metadata *pdev)
{
	struct macsec_pdata *pdata;

	if (!pdev
	    || !pdev->prepare || !pdev->unprepare
	    || !pdev->ig_irq_ena || !pdev->ig_irq_dis
	    || !pdev->ig_reg_rd || !pdev->ig_reg_wr
	    || !pdev->eg_irq_ena || !pdev->eg_irq_dis
	    || !pdev->eg_reg_rd || !pdev->eg_reg_wr) {
		pr_err("%s: one of the required pointer is NULL.\n", __FUNCTION__);
		return -EINVAL;
	}

	if ((pdev->ctx.offload == MACSEC_OFFLOAD_MAC && !pdev->ctx.netdev)
	    || (pdev->ctx.offload == MACSEC_OFFLOAD_PHY
	        && (!pdev->ctx.phydev || !pdev->ctx.phydev->priv))) {
		pr_err("%s: no netdev or phydev.\n", __FUNCTION__);
		return -EINVAL;
	}

	pdata = kzalloc(sizeof(*pdata), GFP_KERNEL);
	if (!pdata) {
		pr_err("%s: out of memory.\n", __FUNCTION__);
		return -ENOMEM;
	}

	pdata->dev = *pdev;

	set_macsec_pdata_ops(&pdev->ctx, pdata, &macsec_ops);

	return 0;
}
EXPORT_SYMBOL(e160_register);

void e160_unregister(const struct e160_metadata *pdev)
{
	struct macsec_pdata *pdata;
	struct macsec_context *ctx;

	if (!pdev)
		return;

	pdata = get_macsec_pdata(&pdev->ctx);
	if (!pdata || pdata->dev.ctx.netdev != pdev->ctx.netdev)
		return;

	ctx = &pdata->dev.ctx;
	macsec_exit(ctx, NULL);
	set_macsec_pdata_ops(ctx, NULL, NULL);

	kzfree(pdata);
}
EXPORT_SYMBOL(e160_unregister);

#if ENABLE_NETDEV_NOTIFIER
/* NewLink will have NETDEV_REGISTER notification
 * DelLink will have NETDEV_UNREGISTER notification
 * I/f UP will have NETDEV_UP notification
 * I/f DOWN will have NETDEV_DOWN notification
 */
static int lgm_macsec_notify(struct notifier_block *this,
			     unsigned long event, void *ptr)
{
	struct net_device *real_dev = netdev_notifier_info_to_dev(ptr);

	/* Check if this is a MACsec master net device */
	if (is_macsec_master(real_dev)) {
		struct macsec_dev *m;
		struct macsec_rxh_data *rxd;

		switch (event) {
		case NETDEV_UP:
			pr_debug("MACsec master NETDEV_UP\n");
			rxd = rtnl_dereference(real_dev->rx_handler_data);
			list_for_each_entry(m, &rxd->secys, secys) {
				struct macsec_context ctx = {0};

				ctx.offload = m->hw_offload;
				switch (ctx.offload) {
				case MACSEC_OFFLOAD_PHY:
					ctx.phydev = m->real_dev->phydev;
					break;
				case MACSEC_OFFLOAD_MAC:
					ctx.netdev = m->real_dev;
					break;
				default:
					continue;
				}

				lgm_macsec_reinit(&ctx);
			}
			break;

		default:
			break;
		}
	}

	return NOTIFY_OK;
}

static struct notifier_block lgm_macsec_notifier = {
	.notifier_call = lgm_macsec_notify,
};

int lgm_macsec_driv_init(void)
{
	if (register_netdevice_notifier(&lgm_macsec_notifier)) {
		pr_crit("Error in register_netdevice_notifier\n");
		return -1;
	}

	pr_info("lgm_macsec_driv_init success\n");

	return 0;
}

void lgm_macsec_driv_uninit(void)
{
	unregister_netdevice_notifier(&lgm_macsec_notifier);

	pr_info("lgm_macsec_driv_uninit success\n");
}
#endif

MODULE_AUTHOR("Govindaiah Mudepalli");
MODULE_DESCRIPTION("EIP160S MACsec API");
MODULE_LICENSE("GPL v2");
