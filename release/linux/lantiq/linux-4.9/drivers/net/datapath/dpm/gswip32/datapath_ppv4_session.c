/*
 * Copyright (C) Intel Corporation
 * Author: Shao Guohua <guohua.shao@intel.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */
#include <net/datapath_api.h>
#include "../datapath.h"
#include "datapath_misc.h"
#include "datapath_ppv4_session.h"

#define LGM_DP_HARDCODE_TEST

#if IS_ENABLED(CONFIG_PPV4)
#define DP_PP_API_DUMMY 0
#else
#define DP_PP_API_DUMMY 1
#endif

#if DP_PP_API_DUMMY
s32 pp_port_add(u16 port_id, struct pp_port_cfg *cfg)
{
	return 0;
}

s32 pp_session_create(struct pp_sess_create_args *args, u32 *sess_id,
		      struct pp_request *req)
{
	static int local_sess_id;
	*sess_id = local_sess_id++;

	return 0;
}

s32 pp_hostif_add(struct pp_hostif_cfg *hif)
{
	return 0;
}

s32 pp_hostif_dflt_set(struct pp_hif_datapath *dp)
{
	return 0;
}

int cbm_gpid_lpid_map(struct cbm_gpid_lpid  *map)
{
	return 0;
}
#endif

void init_gpid_map_table(int inst)
{
	struct hal_priv *priv = HAL(inst);
	u32 i = 0;

	memset(priv->gp_dp_map, 0, (sizeof(priv->gp_dp_map) * MAX_GPID));

	for (i = 0; i < MAX_GPID; i++) {
		if (i >= 0 && i <= DP_DYN_GPID_START)
			priv->gp_dp_map[i].dpid = i;

		/* Rest all GPID > 16 map to DPID 15,
		 * Later Caller will change this
		 */
		if (i >= DP_SPL_GPID_START)
			priv->gp_dp_map[i].dpid =
				(12 + (i - DP_SPL_GPID_START));
	}
}

static void __mark_alloc_gpid(int inst, int base, int end, int dpid)
{
	int tmp;
	struct hal_priv *priv = HAL(inst);

	for (tmp = base; (tmp < end) && (tmp < MAX_GPID); tmp++) {
		priv->gp_dp_map[tmp].alloc_flags = 1;
		priv->gp_dp_map[tmp].dpid = dpid;
		priv->gp_dp_map[tmp].subif = -1;
		priv->gp_dp_map[tmp].ref_cnt = 0;
	}
}

/* This API will be used during DP_alloc_port/ext only */
int alloc_gpid(int inst, enum GPID_TYPE type, int gpid_num, int dpid)
{
	u32 base, match;
	struct hal_priv *priv = HAL(inst);
	int start = 0, end = 0;

	if (type < DP_DYN_GPID) {
		DP_DEBUG(DP_DBG_FLAG_DBG, "Can't alloc overlaps with LPID\n");
		return DP_FAILURE;
	}

	if (type == DP_DYN_GPID) {
		start = DP_DYN_GPID_START;
		end = DP_DYN_GPID_END;
	} else if (type == DP_SPL_GPID) {
		start = SPL_GPID_VIA_DPID(dpid);
		end = start + 1;
	}
	for (base = start; base < end; base++) {
		for (match = 0;
		     (match < gpid_num) && ((base + match) < end);
		     match++) {
			if (priv->gp_dp_map[base + match].alloc_flags)
				break;
		}

		if (match == gpid_num) {
			__mark_alloc_gpid(inst, base, (base + match), dpid);
			return base;
		}
	}

	return DP_FAILURE; /* Alloc GPID Failure */
}

int free_gpid(int inst, int base, int gpid_num)
{
	u32 tmp;
	struct hal_priv *priv = HAL(inst);

	/* Expecting base always greater than DP_DYN_GPID_START */
	if (base < DP_DYN_GPID_START)
		return DP_FAILURE;

	for (tmp = base; (tmp < MAX_GPID) && gpid_num; tmp++) {
		priv->gp_dp_map[tmp].alloc_flags = 0;
		priv->gp_dp_map[tmp].dpid = 0;
		priv->gp_dp_map[tmp].subif = 0;
		/* TODO: Dpid and vap reset*/
		gpid_num--;
	}

	return DP_SUCCESS;
}

int get_dpid_from_gpid(int inst, int gpid)
{
	struct hal_priv *priv = HAL(inst);

	return priv->gp_dp_map[gpid].dpid;
}

bool is_mem_port(int alloc_flag)
{
	if (alloc_flag & DP_F_ACA)
		return true;

	return false;
}

bool is_stream_port(int alloc_flag)
{
	if ((alloc_flag & DP_F_FAST_ETH_LAN) ||
	    (alloc_flag & DP_F_FAST_ETH_WAN) ||
	    (alloc_flag & DP_F_GINT) ||
	    (alloc_flag & DP_F_GPON) ||
	    (alloc_flag & DP_F_EPON))
		return true;

	return false;
}

int get_subif_size(u32 vap_mask)
{
	u32 i;
	int num = 0;

	for (i = 0; i < sizeof(i); i++)
		if (vap_mask & (1 << i))
			num++;

	return num;
}

/* dp_add_pp_gpid: to configure normal GPID or special GPID
 * Note: try to get all GPID related configuration via dpid/vap
 *       if spl_gpid is 1, vap is not valid
 * If success, return DP_SUCCESS.
 * else return -1 /DP_FAILURE
 */
int dp_add_pp_gpid(int inst, int dpid, int vap, int gpid, int spl_gpid)
{
	struct pp_port_cfg cfg = {0};
	const struct ctp_assign *ctp_info;
	struct pmac_port_info *port_info;
	int i;
	struct hal_priv *priv;
	struct dp_subif_info *sif;

	priv = HAL(inst);
	port_info = get_dp_port_info(inst, dpid);
	ctp_info = get_ctp_assign(port_info->alloc_flags);
	if (!ctp_info) {
		pr_err("get_ctp_assign fail:0x%x for dpid=%d\n",
		       port_info->alloc_flags, dpid);
		return DP_SUCCESS;
	}
	sif = get_dp_port_subif(port_info, vap);
	sif->gpid = gpid;
	priv->gp_dp_map[gpid].ref_cnt++;
	if (priv->gp_dp_map[gpid].ref_cnt > 1) /* GPID already configued */
		return DP_SUCCESS;
	if (!is_stream_port(port_info->alloc_flags))
		cfg.rx.mem_port_en = 1;
	cfg.rx.flow_ctrl_en = 1;
	if (spl_gpid) {
		cfg.rx.parse_type = NO_PARSE;
		cfg.rx.cls.n_flds = 2;
		/* convert traffic class to PP classification ID */
		cfg.rx.cls.n_flds = 2;
		cfg.rx.cls.cp[0].stw_off = DP_CLaSS_OFFSET;
		cfg.rx.cls.cp[0].copy_size = DP_CLASS_SIZE;
		cfg.rx.cls.cp[1].stw_off = ctp_info->vap_offset;
		cfg.rx.cls.cp[1].copy_size = get_subif_size(ctp_info->vap_mask);
	} else {
		cfg.rx.parse_type = L2_PARSE;
		/* Note: for subif based GPID, no need to stw operation since
		 * it will be based on packet information, not stw data
		 */
		//return DP_FAILURE;
	}
	/* As told my hezi/Soma for the rx.policies_map setting
	 * streaming port: 0
	   WIFI: 2K, ingress policy
	   other mem port: all
	   But I don't really understand how PPv4 internal is using ???
	*/
#ifdef LGM_DP_HARDCODE_TEST
	/* In fact it is rx pool map, Not really policy map.
	 * index: bit 0-pool size 0, bit 1-pool size 1 and so on
	 * This hard-coded setting is just for Ethernet LAN/WAN CPU path purpose
	 */
	if (is_stream_port(port_info->alloc_flags)) /* Stream port all pool */
		cfg.rx.policies_map = 0;
	else /* CPU */
		cfg.rx.policies_map = 0xF;
#endif /* end of LGM_DP_HARDCODE_TEST */
	cfg.tx.max_pkt_size = sif->max_pkt_size;
	/* headroom_size: maybe multipe of 128, currently may 256 ? */
	cfg.tx.headroom_size = sif->headroom_size;
	/* for timestamp, IPSEC  */
	cfg.tx.tailroom_size = sif->tailroom_size;
	cfg.tx.min_pkt_len = sif->min_pkt_len;
	cfg.tx.pkt_only_en = 0;
	if (is_stream_port(port_info->alloc_flags))
		cfg.tx.seg_en = 0;
	if (spl_gpid) {
		cfg.tx.base_policy = port_info->policy_base;
		for (i = 0; i < port_info->policy_num; i++)
			cfg.tx.policies_map |= BIT(i);
	} else {
		cfg.tx.base_policy = sif->policy_base;
		for (i = 0; i < sif->policy_num; i++)
			cfg.tx.policies_map |= BIT(i);
	}

#ifdef LGM_DP_HARDCODE_TEST
/* packet information to stream port */
#define STREAM_MAX_PKT_ZIE  (1518 + 16) /* PMAC 16 bytes */
#define STREAM_HEADERROOM   32   /* maybe zero ok ? */
#define STREAM_TAILROOM     4  /* No need */

/* packet information to CPU */
#define CPU_MAX_PKT_ZIE  (1518 + 16 + 32) /*Hope no time tag ?: pmac + preL2 */
#define CPU_HEADERROOM   208
#define CPU_TAILROOM     320 /*aligned skb shar_info */

	if (is_stream_port(port_info->alloc_flags)) {
		if (cfg.tx.seg_en) {
			/* Headroom & tailroom must be 0 if seg_en is set */
			cfg.tx.headroom_size = 0;
			cfg.tx.tailroom_size = 0;
		} else {
			cfg.tx.headroom_size = STREAM_HEADERROOM;
			cfg.tx.tailroom_size = STREAM_TAILROOM;
		}
		cfg.tx.max_pkt_size = STREAM_MAX_PKT_ZIE;
		// PP_MIN_TX_PKT_LEN_64B
		cfg.tx.min_pkt_len = PP_MIN_TX_PKT_LEN_NONE;
	} else { /*to CPU */
		cfg.tx.max_pkt_size = CPU_MAX_PKT_ZIE;
		cfg.tx.headroom_size = CPU_HEADERROOM;
		cfg.tx.tailroom_size = CPU_TAILROOM;
		cfg.tx.min_pkt_len = PP_MIN_TX_PKT_LEN_NONE;
		cfg.tx.prel2_en = true;
	}
#endif  /* end of LGM_DP_HARDCODE_TEST */
	pr_info("add gpid=%d\n", gpid);
	pr_info("cfg.rx.cls.n_flds=%d\n", cfg.rx.cls.n_flds);
	pr_info("cfg.rx.mem_port_en=%d\n", cfg.rx.mem_port_en);
	pr_info("cfg.rx.flow_ctrl_en=%d\n", cfg.rx.flow_ctrl_en);
	pr_info("cfg.rx.policies_map=0x%X\n", cfg.rx.policies_map);
	pr_info("cfg.rx.parse_type=%d\n", cfg.rx.parse_type);

	pr_info("cfg.tx.max_pkt_size=%d\n", cfg.tx.max_pkt_size);
	pr_info("cfg.tx.headroom_size=%d\n", cfg.tx.headroom_size);
	pr_info("cfg.tx.tailroom_size=%d\n", cfg.tx.tailroom_size);
	pr_info("cfg.tx.min_pkt_len=%d\n", cfg.tx.min_pkt_len);
	pr_info("cfg.tx.base_policy=%d\n", cfg.tx.base_policy);
	pr_info("cfg.tx.policy_map=0x%X\n", cfg.tx.policies_map);
	pr_info("cfg.tx.pkt_only_en=%d\n", cfg.tx.pkt_only_en);
	pr_info("cfg.tx.seg_en=%d\n", cfg.tx.seg_en);

	if (pp_port_add(gpid, &cfg)) {
		pr_err("failed to create gpid: %d\n", gpid);
		return DP_FAILURE;
	}
	pr_info("GPID[%d] added ok\n", gpid);
	return DP_SUCCESS;
}

int dp_del_pp_gpid(int inst, int dpid, int vap, int gpid, int spl_gpid)
{
	return DP_SUCCESS;
}

/* dp_add_default_egress_sess: Add default egress session based on
 *                             special GPID, class/subif only
 * This API will be used only for CPU TX path to memory port for
 * policy/pool conversion
 */
int dp_add_default_egress_sess(struct dp_session *sess, int flag)
{
	int i;
	struct pp_sess_create_args args = {0};
	u32 sess_id = -1;
	int ret;
	struct pmac_port_info *port_info;

	args.in_port = sess->in_port;
	args.eg_port = sess->eg_port;
	args.fsqm_prio = 0;
	args.color = PP_COLOR_GREEN;
	args.flags = 0;
	args.dst_q = dp_get_q_logic_32(sess->inst, sess->qid);
	for (i = 0; i < ARRAY_SIZE(args.sgc); i++)
		args.sgc[i] = PP_SGC_INVALID;
	for (i = 0; i < ARRAY_SIZE(args.tbm); i++)
		args.tbm[i] = PP_TBM_INVALID;
	args.ud_sz = 0;
	args.tmp_ud_sz = 0; /* 1 means 1 template of UD,
			     * ie, 16 bytes
			     */
	args.ps_off = PP_INVALID_PS_OFF;
	args.cls.n_flds = 2;
	args.cls.fld_data[0] = sess->class;
	args.cls.fld_data[1] = sess->vap;
	args.in_pkt = NULL;
	args.eg_pkt = NULL;
	args.nhdr = NULL;
	args.hash.h1 = sess->h1;
	args.hash.h2 = sess->h2;
	args.hash.sig = sess->sig;

	ret = pp_session_create(&args, &sess_id, NULL);
	if (ret) {
		/* IF failure, call PPA API to add this session... */
		pr_err("session create failed. Call PPA to continue");
		return DP_FAILURE;
	}
	port_info = get_dp_port_info(sess->inst, sess->in_port);
	get_dp_port_subif(port_info, sess->vap)->dfl_sess[sess->class] =
		sess_id;
	pr_info("session id = %u\n", sess_id);
	return DP_SUCCESS;
}

/* dp_add_hostif: create hostif for CPU RX path
 * This API is for normal CPU RX path traffic per GPID
 * If exception session table full, dp_add_hostif will fail to add.
 * But this API itseld still can return success.
 */
int dp_add_hostif(int inst, int dpid, int vap)
{
	struct pp_hostif_cfg hif = {0};
	int i, ret;
	struct pmac_port_info *port_info;
	/* cpu0: 0(high) 1(low)
	 * cpu1: 2(high) 3(low)
	 * cpu2: 4(high) 5(low)
	 * cpu3: 6(high) 7(low)
	 */
	struct pmac_port_info *cpu_info;

	port_info = get_dp_port_info(inst, dpid);
	cpu_info = get_dp_port_info(inst, 0);

	hif.cls.port = get_dp_port_subif(port_info, vap)->gpid;
	hif.dp.color = PP_COLOR_GREEN;  //??? enough
	for (i = 0; i < ARRAY_SIZE(hif.dp.sgc); i++)
		hif.dp.sgc[i] = PP_SGC_INVALID;
	for (i = 0; i < ARRAY_SIZE(hif.dp.tbm); i++)
		hif.dp.tbm[i] = PP_TBM_INVALID;

	/* low priority */
	/* need check GSWIP implementation ? */
	hif.cls.tc_bitmap = BIT(0) | BIT(1);
	/*collect all CPU low priority queue/port */
	for (i = 0; (i < ARRAY_SIZE(hif.dp.eg)) && (i < MAX_SUBIFS); i++) {
		struct dp_subif_info *sif;

		sif = get_dp_port_subif(cpu_info, 2 * i + 1);
		if (sif->flags) { /* valid VAP */
			hif.dp.eg[i].qos_q = dp_get_q_logic_32(inst, sif->qid);
			hif.dp.eg[i].pid = sif->gpid;
		} else {
			hif.dp.eg[i].qos_q = PP_QOS_INVALID_ID;
			hif.dp.eg[i].pid = PP_PORT_INVALID;
		}
	}
	pr_info("dpid=%d vap=%u\n", dpid, vap);
	pr_info("hif.cls.port=%u\n", hif.cls.port);
	pr_info("hif.cls.tc_bitmap=0x%x\n", hif.cls.tc_bitmap);
	pr_info("hif.dp.color=%d\n", hif.dp.color);
	for (i = 0; i < ARRAY_SIZE(hif.dp.eg); i++) {
		pr_info("hif.dp.eg[%d].pid=%u\n", i, hif.dp.eg[i].pid);
		pr_info("hif.dp.eg[%d].qos_q=%u\n", i, hif.dp.eg[i].qos_q);
	}
	ret = pp_hostif_add(&hif);
	if (ret)
		pr_err("hostif_add fail:dpid/gpid=%u/%u vap/tc=%d/%u\n",
		       dpid, hif.cls.port, vap, hif.cls.tc_bitmap);

	/* high priority */
	/* need check GSWIP implementation ? */
	hif.cls.tc_bitmap = BIT(2) | BIT(3);
	/*collect all CPU low priority queue/port */
	for (i = 0; (i < ARRAY_SIZE(hif.dp.eg)) && (i < MAX_SUBIFS); i++) {
		struct dp_subif_info *sif;

		sif = get_dp_port_subif(cpu_info, 2 * i);
		if (sif->flags) { /* Valid VAP */
			hif.dp.eg[i].qos_q = dp_get_q_logic_32(inst, sif->qid);
			hif.dp.eg[i].pid = sif->gpid;
		} else {
			hif.dp.eg[i].qos_q = PP_QOS_INVALID_ID;
			hif.dp.eg[i].pid  = PP_PORT_INVALID;
		}
	}
	pr_info("dpid=%d vap=%u\n", dpid, vap);
	pr_info("hif.cls.port=%u\n", hif.cls.port);
	pr_info("hif.cls.tc_bitmap=0x%x\n", hif.cls.tc_bitmap);
	pr_info("hif.dp.color=%u\n", hif.dp.color);
	for (i = 0; i < ARRAY_SIZE(hif.dp.eg); i++) {
		pr_info("hif.dp.eg[%d].pid=%u\n", i, hif.dp.eg[i].pid);
		pr_info("hif.dp.eg[%d].qos_q=%u\n", i, hif.dp.eg[i].qos_q);
	}
	ret = pp_hostif_add(&hif);
	if (ret)
		pr_err("hostif_add fail:dpid/gpid=%d/%d vap/tc=%d/%d\n",
		       dpid, hif.cls.port, vap, hif.cls.tc_bitmap);

	return DP_SUCCESS;
}

/* dp_add_dflt_hostif: create default hostif
 * This API is for default setting in case not match any exception sessions
 */
int dp_add_dflt_hostif(struct dp_dflt_hostif *hostif, int flag)
{
	struct pp_hif_datapath dp = {0};
	int i;

	if (!hostif) {
		pr_err("hostif NULL\n");
		return DP_FAILURE;
	}
	/* only allowed one queue for pp_hostif_dflt_set */
	dp.eg[0].qos_q = dp_get_q_logic_32(hostif->inst, hostif->qid);
	dp.eg[0].pid = hostif->gpid;
	dp.color = PP_COLOR_GREEN;

	for (i = 1; i < ARRAY_SIZE(dp.eg); i++) {
		dp.eg[i].qos_q = PP_QOS_INVALID_ID;
		dp.eg[i].pid = PP_PORT_INVALID;
	}
	for (i = 0; i < ARRAY_SIZE(dp.sgc); i++)
		dp.sgc[i] = PP_SGC_INVALID;
	for (i = 0; i < ARRAY_SIZE(dp.tbm); i++)
		dp.tbm[i] = PP_TBM_INVALID;

	return pp_hostif_dflt_set(&dp);
}

int dp_subif_pp_set(int inst, int portid, int vap,
		    struct subif_platform_data *data, u32 flags)
{
	struct pmac_port_info *port_info;
	int gpid, num = vap;

	port_info = get_dp_port_info(inst, portid);
	if ((port_info->alloc_flags & DP_F_GPON) ||
	    port_info->alloc_flags & DP_F_GPON)
		gpid = port_info->gpid_base;
	else {
		if (vap >= port_info->gpid_num)
			num = port_info->gpid_num;
		gpid = port_info->gpid_base + num;
	}
	pr_info("dp_subif_pp_set=%d\n", gpid);
	if (dp_add_pp_gpid(inst, portid, vap, gpid, 0) == DP_FAILURE) {
		pr_err("dp_add_pp_gpid for dport/vap=%d/%d\n", portid, vap);
		return -1;
	}
	get_dp_port_subif(port_info, vap)->gpid = gpid;

	/* need increase GPID reference counter and store DPID
	 * Also need update to pmac_port_info array
	 */
	dp_add_hostif(inst, portid, vap);
	return 0;
}
