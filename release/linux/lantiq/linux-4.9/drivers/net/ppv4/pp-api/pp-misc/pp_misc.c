/*
 * pp_misc.c
 * Description: PP Misc Driver
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018-2020 Intel Corporation
 */

#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/slab.h>
#ifdef CONFIG_PPV4_LGM
#include <linux/skb_extension.h>
#include <linux/platform_data/intel_epu.h>
#include <linux/pktprs.h>
#endif
#include <linux/pp_qos_api.h>
#include <linux/pp_api.h>
#include "pp_common.h"
#include "pp_misc.h"
#include "infra.h"
#include "uc.h"
#include "checker.h"
#include "pp_desc.h"
#include "pp_misc_internal.h"
#include "pp_session_mgr.h"

#ifdef CONFIG_PPV4_LGM
/**
 * @struct pp_misc_db
 * @brief Packet Processor misc driver database
 */
struct pp_misc_db {
	/*! pp descriptor cookie cache */
	struct kmem_cache *pp_desc_cache;

	/*! handle to the PP cookie for attaching cookie on RX hook */
	s32                pp_cookie_hnd;

	/*! misc driver stats */
	struct misc_stats  stats;

	/*! network functions enabled bitmap */
	ulong              nf_en;

	/*! network functions */
	struct pp_nf_info  nf[PP_NF_NUM];

	/*! PP hw clock frequency in MHz */
	u32 hw_clk;

	/*! max prints for brief command */
	u32 dbg_print_cnt;

	/*! Ingore clock freq' updates */
	u32 dbg_ignore_clock_freq_updates;
};

static inline void __desc_free(skb_cookie c);

/* Driver Database */
static struct pp_misc_db *db;

static int pm_notifier(struct notifier_block *nb, unsigned long e, void *data);

static struct notifier_block nb = { .notifier_call =  pm_notifier};

s32 misc_stats_get(struct misc_stats *stats)
{
	atomic_t *db_cnt, *cnt;

	if (ptr_is_null(stats))
		return -EINVAL;

	cnt = (atomic_t *)stats;
	for_each_struct_mem(&db->stats, db_cnt, cnt++)
		atomic_set(cnt, atomic_read(db_cnt));

	return 0;
}

s32 misc_stats_reset(void)
{
	atomic_t *cnt;

	for_each_struct_mem(&db->stats, cnt)
		atomic_set(cnt, 0);

	return 0;
}

s32 __hw_clk_set(u32 clk_MHz)
{
	struct pp_qos_dev *qdev;
	s32 ret;

	/* silently do nothing */
	if (unlikely(db->hw_clk == clk_MHz))
		return 0;

	ret = chk_clock_set(clk_MHz);
	if (unlikely(ret))
		return ret;

	qdev = pp_qos_dev_open(PP_QOS_INSTANCE_ID);
	if (unlikely(ptr_is_null(qdev)))
		return -ENODEV;

	ret = qos_clock_set(qdev, clk_MHz);
	if (unlikely(ret))
		return ret;

	db->hw_clk = clk_MHz;
	return 0;
}

s32 pp_hw_clock_freq_get(u32 *clk_MHz)
{
	if (unlikely(ptr_is_null(clk_MHz)))
		return -EINVAL;

	*clk_MHz = db->hw_clk;
	return 0;
}

s32 pp_hw_clock_freq_set(u32 clk_MHz)
{
	s32 ret;

	if (unlikely(!pp_is_ready()))
		return -EPERM;

	if (unlikely(!clk_MHz)) {
		pr_err("Invalid clock %u\n", clk_MHz);
		return -EINVAL;
	}

	ret = __hw_clk_set(clk_MHz);
	if (unlikely(ret))
		/* changing clock failed, revert back */
		__hw_clk_set(db->hw_clk);

	return ret;
}
EXPORT_SYMBOL(pp_hw_clock_freq_set);

/**
 * @brief Get Packet Processor version
 * @param v pp_version type buffer to write version to
 * @param t version type
 * @return s32 0 on success, -EINVAL in case of null version
 *         buffer
 */
s32 pp_version_get(struct pp_version *v, enum pp_version_type t)
{
	if (unlikely(!pp_is_ready()))
		return -EPERM;
	if (unlikely(!v))
		return -EINVAL;

	memset(v, 0, sizeof(*v));
	switch (t) {
	case PP_VER_TYPE_DRV:
		v->major     = PP_VER_MAJOR;
		v->major_mid = PP_VER_MAJOR_MID;
		v->mid       = PP_VER_MID;
		v->minor_mid = PP_VER_MINOR_MID;
		v->minor     = PP_VER_MINOR;
		break;
	case PP_VER_TYPE_FW:
		/* TODO: get PP FW version */
		break;
	case PP_VER_TYPE_HW:
		v->major = infra_version_get();
		break;
	default:
		pr_err("Invalid PP version type %u\n", t);
		return -EINVAL;
	}

	return 0;
}
EXPORT_SYMBOL(pp_version_get);

s32 pp_version_show(char *buf, size_t sz, size_t *n, struct pp_version *drv,
		    struct pp_version *fw, struct pp_version *hw)
{
	if (unlikely(ptr_is_null(buf) || ptr_is_null(n) || ptr_is_null(drv) ||
		     ptr_is_null(fw) || ptr_is_null(hw)))
		return -EINVAL;

	pr_buf(buf, sz, *n, "  %-25s: \t%u.%u.%u.%u.%u\n", "PP driver version",
	       drv->major, drv->major_mid, drv->mid, drv->minor_mid,
	       drv->minor);

	pr_buf_cat(buf, sz, *n, "  %-25s: \t%u.%u.%u.%u.%u\n", "PP FW version",
		   fw->major, fw->major_mid, fw->mid, fw->minor_mid, fw->minor);

	pr_buf_cat(buf, sz, *n, "  %-25s: \t%u.%u.%u.%u.%u\n", "PP HW version",
		   hw->major, hw->major_mid, hw->mid, hw->minor_mid, hw->minor);

	return 0;
}

s32 pp_nf_set(enum pp_nf_type type, struct pp_nf_info *nf)
{
	struct pp_qos_queue_info q_info;
	struct pp_qos_dev *qdev;
	s32 ret = 0;
	u16 uc_qos_port;
	u16 uc_q;

	if (unlikely(!pp_is_ready()))
		return -EPERM;

	if (unlikely(!(!ptr_is_null(nf) &&
		       !ptr_is_null(db)))) {
		ret = -EINVAL;
		goto out;
	}

	if (unlikely(!PP_IS_NF_VALID(type))) {
		pr_err("invalid nf type %d\n", type);
		ret = -EINVAL;
		goto out;
	}

	if (unlikely(type != PP_NF_REASSEMBLY &&
		     !__pp_is_port_valid(nf->pid))) {
		pr_err("invalid nf port %hu\n", nf->pid);
		ret = -EINVAL;
		goto out;
	}

	/* get the nf physical queue */
	qdev = pp_qos_dev_open(PP_QOS_INSTANCE_ID);
	if (unlikely(ptr_is_null(qdev)))
		return -EINVAL;

	ret = pp_qos_queue_info_get(qdev, nf->q, &q_info);
	if (unlikely(ret)) {
		pr_err("Failed getting queue %u info\n", nf->q);
		goto out;
	}

	uc_qos_port = q_info.port_id;
	uc_q = q_info.physical_id;

	ret = pp_qos_queue_info_get(qdev, nf->cycl2_q, &q_info);
	if (unlikely(ret)) {
		pr_err("Failed getting queue %u info\n", nf->cycl2_q);
		goto out;
	}

	ret = uc_nf_set(type, nf->pid, uc_qos_port, q_info.physical_id,
			PMGR_HIF_DFLT_DP_IDX);
	if (unlikely(ret)) {
		pr_err("Failed Setting NF %d in uc\n", type);
		goto out;
	}

	/* update checker */
	ret = chk_nf_set(uc_q, type);
	if (unlikely(ret)) {
		pr_err("Failed Setting queue %hu for nf %d\n",
		       uc_q, type);
		goto out;
	}

	/* store nf info */
	set_bit(type, &db->nf_en);
	memcpy(&db->nf[type], nf, sizeof(*nf));

	/* Notify session manager of the new NF added */
	ret = smgr_nf_set(type, nf);
out:
	return ret;
}
EXPORT_SYMBOL(pp_nf_set);

s32 pp_nf_get(enum pp_nf_type type, struct pp_nf_info *nf)
{
	if (unlikely(!pp_is_ready()))
		return -EPERM;

	if (unlikely(ptr_is_null(nf) || ptr_is_null(db)))
		return -EINVAL;

	if (unlikely(!PP_IS_NF_VALID(type))) {
		pr_err("invalid nf type %d\n", type);
		return -EINVAL;
	}

	memcpy(nf, &db->nf[type], sizeof(*nf));

	return 0;
}
EXPORT_SYMBOL(pp_nf_get);

bool pp_misc_is_nf_en(enum pp_nf_type nf)
{
	return test_bit(nf, &db->nf_en);
}

struct pp_desc *pp_pkt_desc_get(struct sk_buff *skb)
{
	if (unlikely(!pp_is_ready()))
		return NULL;
	if (unlikely(ptr_is_null(skb)))
		return NULL;

	return ((struct pp_desc *)skb_cookie_get(skb, db->pp_cookie_hnd));
}
EXPORT_SYMBOL(pp_pkt_desc_get);

void pp_rx_pkt_hook(struct sk_buff *skb)
{
	struct pp_desc *desc = NULL;

	if (unlikely(!(db && db->pp_desc_cache)))
		return;
	if (unlikely(!skb)) {
		atomic_inc(&db->stats.rx_hook_null_skb_cnt);
		return;
	}
	atomic_inc(&db->stats.rx_hook_skb_cnt);

	/* Allocate PP cookie */
	desc = (struct pp_desc *)kmem_cache_alloc_node(db->pp_desc_cache,
						       GFP_ATOMIC,
						       NUMA_NO_NODE);
	if (unlikely(!desc)) {
		pr_err("Failed to allocate PP descriptor skb cookie of sz %u\n",
		       (u32)sizeof(*desc));
		return;
	}

	if (pp_desc_decode(desc, (struct pp_hw_desc *)skb->buf_base)) {
		pr_err("Failed to decode PP descriptor, skb 0x%p\n", skb);
		__desc_free((skb_cookie)desc);
		return;
	}

	/* Count last slow path packets received for debug */
	if (desc->lsp_pkt)
		atomic_inc(&db->stats.rx_hook_lspp_pkt_cnt);

	/* attach PP descriptor cookie to the SKB pointer */
	if (skb_cookie_attach(skb, db->pp_cookie_hnd, (skb_cookie)desc,
			      __desc_free)) {
		pr_err("Failed to attach PP descriptor cookie, skb 0x%p, cookie 0x%p\n",
		       skb, desc);
		__desc_free((skb_cookie)desc);
	}
}
EXPORT_SYMBOL(pp_rx_pkt_hook);

void pp_tx_pkt_hook(struct sk_buff *skb, u16 pid)
{
	struct pp_desc *desc;

	if (unlikely(!db))
		return;

	if (unlikely(!skb)) {
		atomic_inc(&db->stats.tx_hook_null_skb_cnt);
		return;
	}

	atomic_inc(&db->stats.tx_hook_skb_cnt);
	desc = pp_pkt_desc_get(skb);
	if (!desc)
		return;

	/* if this is the last slow path packet,
	 * signal the sync queue mechanism
	 */
	if (desc->lsp_pkt)
		smgr_sq_lspp_rcv(desc->ud.sess_id);
}
EXPORT_SYMBOL(pp_tx_pkt_hook);

s32 pp_resource_stats_show(char *buf, size_t sz, size_t *n)
{
	struct pp_bmgr_policy_stats policy_stats;
	struct pp_bmgr_pool_params pool_cfg;
	struct pp_bmgr_pool_stats pool_stats;
	struct rpb_hw_stats rpb_hw_stats;
	struct pp_bmgr_init_param bmgr_cfg;
	u32 i, rpb_mem_sz, tmp;
	s32 ret;
	char pools[64];

	if (unlikely(ptr_is_null(buf) || ptr_is_null(n)))
		return -EINVAL;

	ret = rpb_hw_stats_get(&rpb_hw_stats);
	if (unlikely(ret)) {
		pr_err("rpb_hw_stats_get failed\n");
		return ret;
	}

	ret = rpb_memory_sz_get(&rpb_mem_sz);
	if (unlikely(ret)) {
		pr_err("rpb_memory_sz_get failed\n");
		return ret;
	}

	ret = pp_bmgr_config_get(&bmgr_cfg);
	if (unlikely(ret)) {
		pr_err("pp_bmgr_config_get failed\n");
		return ret;
	}

	pr_buf(buf, sz, *n, "\n");
	pr_buf_cat(buf, sz, *n,
		   "+----------------------------------------------------------------------------------------+\n");
	pr_buf_cat(buf, sz, *n, "| %-86s |\n",
		   "          Packet Processor Resources");
	pr_buf_cat(buf, sz, *n,
		   "+--------|-------------|-------------|---------------|---------------|-------------------+\n");

	/* Policies stats */
	pr_buf_cat(buf, sz, *n,
		   "| %-6s | %-11s | %-11s | %-13s | %-13s | %-17s |\n",
		   "Policy", "Null ctr", "Max Allowed", "Min Guarantee",
		   "Pools Mapping", "Allocated Buffers");
	pr_buf_cat(buf, sz, *n,
		   "+--------|-------------|-------------|---------------|---------------|-------------------+\n");
	for (i = 0; i < bmgr_cfg.max_policies; i++) {
		if (!pp_bmgr_is_policy_active(i))
			continue;

		pp_bmgr_policy_stats_get(&policy_stats, i);
		pr_buf(pools, sizeof(pools), tmp, "%*pbl", PP_BM_MAX_POOLS,
		       policy_stats.pools_bmap);

		pr_buf_cat(buf, sz, *n,
			   "| %-6d | %11d | %11d | %13d | %-13s | %17d |\n", i,
			   policy_stats.policy_null_ctr,
			   policy_stats.policy_max_allowed,
			   policy_stats.policy_min_guaranteed, pools,
			   policy_stats.policy_alloc_buff);
	}

	pr_buf_cat(buf, sz, *n,
		   "+--------+-------------+-------------+---------------+---------------+-------------------+\n");

	/* Pools stats */
	pr_buf_cat(buf, sz, *n, "| %-6s | %-11s | %-11s | %-13s |\n", "Pool",
		   "Buffer Size", "Num Buffers", "Used Buffers");
	pr_buf_cat(buf, sz, *n,
		   "+--------|-------------|-------------|---------------+\n");

	for (i = 0; i < bmgr_cfg.max_pools; i++) {
		ret = bm_pool_stats_get(&pool_stats, i);
		if (unlikely(ret))
			continue;

		ret = pp_bmgr_pool_conf_get(i, &pool_cfg);
		if (unlikely(ret))
			continue;

		pr_buf_cat(buf, sz, *n, "| %-6u | %11u | %11u | %13u |\n", i,
			   pool_cfg.size_of_buffer, pool_stats.pool_size,
			   pool_stats.pool_allocated_ctr);
	}

	pr_buf_cat(buf, sz, *n,
		   "+--------+-------------+-------------+---------------+\n");

	/* Pools stats */
	pr_buf_cat(buf, sz, *n, "| %-34s | %12u%% |\n",
		   "RPB Memory Utilization",
		   (rpb_hw_stats.total_bytes_used * 100) / rpb_mem_sz);
	pr_buf_cat(buf, sz, *n, "| %-34s | %13u |\n", "RPB Bytes Used",
		   rpb_hw_stats.total_bytes_used);
	pr_buf_cat(buf, sz, *n, "| %-34s | %13u |\n", "RPB Packets Used",
		   rpb_hw_stats.total_pkts_used);
	pr_buf_cat(buf, sz, *n,
		   "+------------------------------------+---------------+\n");

	return 0;
}

void pp_global_stats_reset(void)
{
	struct pp_qos_dev *qdev;

	port_dist_reset_stat();
	rpb_stats_reset();
	prsr_stats_reset();
	cls_stats_reset();
	chk_stats_reset();
	mod_stats_reset();
	rx_dma_stats_reset();
	qdev = pp_qos_dev_open(PP_QOS_INSTANCE_ID);
	pp_qos_stats_reset(qdev);
	smgr_sq_dbg_stats_get(NULL, true);
	uc_stats_reset();
}

void pp_driver_stats_reset(void)
{
	pp_smgr_stats_reset();
	pmgr_stats_reset();
	misc_stats_reset();
}

s32 pp_global_stats_diff(void *pre, u32 num_pre, void *post, u32 num_post,
			 void *delta, u32 num_delta, void *data)
{
	struct pp_global_stats *__pre, *__post, *__delta;

	if (unlikely(ptr_is_null(pre) || ptr_is_null(post) ||
		     ptr_is_null(delta)))
		return -EINVAL;

	__pre   = pre;
	__post  = post;
	__delta = delta;
	U32_STRUCT_DIFF(&__pre->chk_stats, &__post->chk_stats,
			&__delta->chk_stats);
	U32_STRUCT_DIFF(&__pre->rpb_stats, &__post->rpb_stats,
			&__delta->rpb_stats);
	U32_STRUCT_DIFF(&__pre->cls_stats, &__post->cls_stats,
			&__delta->cls_stats);
	U32_STRUCT_DIFF(&__pre->mod_stats, &__post->mod_stats,
			&__delta->mod_stats);
	U32_STRUCT_DIFF(&__pre->qos_stats, &__post->qos_stats,
			&__delta->qos_stats);
	U32_STRUCT_DIFF(&__pre->parser_stats, &__post->parser_stats,
			&__delta->parser_stats);
	U32_STRUCT_DIFF(&__pre->syncq_stats, &__post->syncq_stats,
			&__delta->syncq_stats);
	U64_STRUCT_DIFF(&__pre->port_dist_stats, &__post->port_dist_stats,
			&__delta->port_dist_stats);
	U64_STRUCT_DIFF(&__pre->rx_dma_stats, &__post->rx_dma_stats,
			&__delta->rx_dma_stats);
	U64_STRUCT_DIFF(&__pre->mcast_stats, &__post->mcast_stats,
			&__delta->mcast_stats);
	U64_STRUCT_DIFF(&__pre->reass_stats, &__post->reass_stats,
			&__delta->reass_stats);
	U64_STRUCT_DIFF(&__pre->frag_stats, &__post->frag_stats,
			&__delta->frag_stats);

	return 0;
}

s32 pp_global_stats_get(void *stats, u32 num_stats, void *data)
{
	struct pp_global_stats *__stats = stats;
	s32 ret;

	if (unlikely(ptr_is_null(stats) || ptr_is_null(data)))
		return -EINVAL;

	ret = port_dist_stats_get(&__stats->port_dist_stats);
	if (unlikely(ret))
		pr_err("port_dist_stats_get failed\n");

	ret = rpb_stats_get(&__stats->rpb_stats);
	if (unlikely(ret))
		pr_err("rpb_stats_get failed\n");

	ret = prsr_stats_get(&__stats->parser_stats);
	if (unlikely(ret))
		pr_err("prsr_stats_get failed\n");

	ret = cls_stats_get(&__stats->cls_stats);
	if (unlikely(ret))
		pr_err("cls_stats_get failed\n");

	ret = chk_stats_get(&__stats->chk_stats);
	if (unlikely(ret))
		pr_err("chk_stats_get failed\n");

	ret = mod_stats_get(&__stats->mod_stats);
	if (unlikely(ret))
		pr_err("mod_stats_get failed\n");

	ret = rx_dma_stats_get(&__stats->rx_dma_stats);
	if (unlikely(ret))
		pr_err("rx_dma_stats_get failed\n");

	ret = pp_qos_stats_get(data, &__stats->qos_stats);
	if (unlikely(ret))
		pr_err("pp_qos_stats_get failed\n");

	ret = uc_mcast_stats_get(&__stats->mcast_stats);
	if (unlikely(ret))
		pr_err("uc_mcast_stats_get failed\n");

	ret = uc_frag_stats_get(&__stats->frag_stats);
	if (unlikely(ret))
		pr_err("uc_frag_stats_get failed\n");

	ret = smgr_sq_dbg_stats_get(&__stats->syncq_stats, false);
	if (unlikely(ret))
		pr_err("smgr_sq_dbg_stats_get failed\n");

	ret = uc_reass_stats_get(&__stats->reass_stats);
	if (unlikely(ret))
		pr_err("uc_reassembly_stats_get failed\n");

	return 0;
}

s32 pp_brief_stats_get(void *stats, u32 num_stats, void *data)
{
	struct pp_active_sess_stats *sess_stats;
	s32    ret;
	u32    n_bits, i, sess;
	ulong  *bmap;

	sess_stats = (struct pp_active_sess_stats *)stats;

	if (unlikely(ptr_is_null(stats)))
		return -EINVAL;

	/* allocate bitmap for all sessions */
	ret = smgr_sessions_bmap_alloc(&bmap, &n_bits);
	if (unlikely(ret))
		return ret;

	/* get all opened sessions */
	ret = smgr_open_sessions_bmap_get(bmap, n_bits);
	if (unlikely(ret))
		goto done;

	/* iterate over all sessions */
	i = 0;
	for_each_set_bit(sess, bmap, n_bits) {
		if (i == num_stats)
			break;
		sess_stats[i].id = sess;
		pp_session_stats_get(sess_stats[i].id, &sess_stats[i].stats);
		i++;
	}

done:
	kfree(bmap);
	return ret;
}

static struct pp_active_sess_stats
__find_stat_by_id(struct pp_active_sess_stats *stats, u32 num, u32 id)
{
	u32 i;
	struct pp_active_sess_stats tmp = {0};

	tmp.id = id;
	if (unlikely(ptr_is_null(stats)) || (!num))
		return tmp;

	for (i = 0 ; i < num ; i++)
		if (stats[i].id == id) {
			memcpy(&tmp.stats, &stats[i].stats, sizeof(tmp.stats));
			break;
		}

	return tmp;
}

s32 pp_brief_stats_diff(void *pre, u32 num_pre, void *post, u32 num_post,
			 void *delta, u32 num_delta, void *data)
{
	int i, j;
	struct pp_active_sess_stats *__pre, *__post, *__delta, sess_stats;

	if (unlikely(ptr_is_null(pre) || ptr_is_null(post) ||
		     ptr_is_null(delta)))
		return -EINVAL;

	__pre   = pre;
	__post  = post;
	__delta = delta;

	for (i = 0, j = 0; i < num_pre && j < num_delta; i++)
		if (__pre[i].id == __post[i].id) {
			U64_STRUCT_DIFF(&__pre[i].stats,
			&__post[i].stats,
			&__delta[j++].stats);
		} else {
			/* if not match, we will try to find session in the
			 * pre. If not exists, it means that this is a new
			 * session and we will copy counters and id
			 */
			sess_stats = __find_stat_by_id(__pre, num_pre,
			__post[i].id);
			U64_STRUCT_DIFF(&sess_stats, &__post[i].stats,
			&__delta[j].stats);
			__delta[j++].id = __post[i].id;
		}
	return 0;
}

s32 pp_brief_stats_show(char *buf, size_t sz, size_t *n, void *stats,
			     u32 num_stats, void *data)
{
	struct sess_db_info info;
	struct pp_active_sess_stats *sess_stats, temp;
	int    i, j;
	char   l3Type[10], l4Prot[10];

	sess_stats = (struct  pp_active_sess_stats *) stats;

	if (unlikely(ptr_is_null(buf) || ptr_is_null(n) || ptr_is_null(stats)))
		return -EINVAL;

	/* Sort the active sessions */
	for (i = 0; i < num_stats; i++)
		for (j = i ; j < num_stats ; j++)
			if (sess_stats[i].stats.packets <
				sess_stats[j].stats.packets) {
					memcpy(&temp, &sess_stats[i],
					sizeof(struct pp_active_sess_stats));
					memcpy(&sess_stats[i], &sess_stats[j],
					sizeof(struct pp_active_sess_stats));
					memcpy(&sess_stats[j], &temp,
					sizeof(struct pp_active_sess_stats));
				}

	pr_buf(buf, sz, *n,
	       " +--------+----------+--------+---------+----------+---------+---------+\n");
	pr_buf_cat(buf, sz, *n,
	       " | id     |  PPS     | Mbps   | In GPID | Out GPID | L3 Type | L4 Prot |\n");

	/* Print only active sessions */
	j = min(db->dbg_print_cnt, num_stats);
	for (i = 0; i < j; i++) {
		if (!sess_stats[i].stats.packets)
			break;

		if (smgr_session_info_get(sess_stats[i].id, &info))
			continue;
#ifdef CONFIG_DEBUG_FS
		if (PKTPRS_IS_IPV4(&info.rx, PKTPRS_HDR_LEVEL0))
			strcpy(l3Type, "IPv4");
		else if (PKTPRS_IS_IPV6(&info.rx, PKTPRS_HDR_LEVEL0))
			strcpy(l3Type, "IPv6");
		else
#endif
		{
			strcpy(l3Type, "N/A");
		}

#ifdef CONFIG_DEBUG_FS
		if (PKTPRS_IS_TCP(&info.rx, PKTPRS_HDR_LEVEL0))
			strcpy(l4Prot, "TCP");
		else if (PKTPRS_IS_UDP(&info.rx, PKTPRS_HDR_LEVEL0))
			strcpy(l4Prot, "UDP");
		else
#endif
		{
			strcpy(l4Prot, "N/A");
		}

		pr_buf_cat(buf, sz, *n,
	       		" +--------+----------+--------+---------+----------+---------+---------+\n");
		pr_buf_cat(buf, sz, *n,
		" | %-6u | %-8llu | %-6llu | %-7u | %-8u | %-8s| %-8s|\n",
		   sess_stats[i].id, sess_stats[i].stats.packets,
		   sess_stats[i].stats.bytes*8/1000/1000,
		   info.in_port, info.eg_port, l3Type, l4Prot);
	}

	pr_buf_cat(buf, sz, *n,
	       " +--------+----------+--------+---------+----------+---------+---------+\n");
	return 0;
}

void pp_misc_set_brief_cnt(u32 cnt)
{
	db->dbg_print_cnt = cnt ? cnt : 1;
}

s32 pp_driver_stats_diff(void *pre, u32 num_pre, void *post, u32 num_post,
			 void *delta, u32 num_delta, void *data)
{
	struct pp_driver_stats *__pre, *__post, *__delta;

	if (unlikely(ptr_is_null(pre) || ptr_is_null(post) ||
		     ptr_is_null(delta)))
		return -EINVAL;

	__pre   = pre;
	__post  = post;
	__delta = delta;

	U32_STRUCT_DIFF(&__pre->pmgr_stats, &__post->pmgr_stats,
			&__delta->pmgr_stats);
	U32_STRUCT_DIFF(&__pre->bmgr_stats, &__post->bmgr_stats,
			&__delta->bmgr_stats);
	U32_STRUCT_DIFF(&__pre->qos_stats, &__post->qos_stats,
			&__delta->qos_stats);
	U32_STRUCT_DIFF(&__pre->syncq_stats, &__post->syncq_stats,
			&__delta->syncq_stats);
	ATOMIC_STRUCT_DIFF(&__pre->smgr_stats, &__post->smgr_stats,
			   &__delta->smgr_stats);
	ATOMIC_STRUCT_DIFF(&__pre->misc_stats, &__post->misc_stats,
			   &__delta->misc_stats);

	return 0;
}

s32 pp_driver_stats_get(void *stats, u32 num_stats, void *data)
{
	struct pp_driver_stats *__stats = stats;
	s32 ret;

	if (unlikely(ptr_is_null(stats) || ptr_is_null(data)))
		return -EINVAL;

	ret = pmgr_stats_get(&__stats->pmgr_stats);
	if (unlikely(ret)) {
		pr_err("pmgr_stats_get failed\n");
		return ret;
	}

	ret = pp_smgr_stats_get(&__stats->smgr_stats);
	if (unlikely(ret)) {
		pr_err("pp_smgr_stats_get failed\n");
		return ret;
	}

	ret = smgr_sq_dbg_stats_get(&__stats->syncq_stats, false);
	if (unlikely(ret)) {
		pr_err("smgr_sq_dbg_stats_get failed\n");
		return ret;
	}

	ret = pp_bmgr_stats_get(&__stats->bmgr_stats);
	if (unlikely(ret)) {
		pr_err("pp_bmgr_stats_get failed\n");
		return ret;
	}

	ret = misc_stats_get(&__stats->misc_stats);
	if (unlikely(ret)) {
		pr_err("misc_stats_get failed\n");
		return ret;
	}

	ret = qos_drv_stats_get(data, &__stats->qos_stats);
	if (unlikely(ret)) {
		pr_err("qos_drv_stats_get failed\n");
		return ret;
	}

	return 0;
}

s32 pp_driver_stats_show(char *buf, size_t sz, size_t *n, void *stats,
			 u32 num_stats, void *data)
{
	struct pp_driver_stats *st = stats;
	struct qos_drv_stats *qos;
	struct smgr_stats    *smgr;
	struct smgr_sq_stats *synq;
	struct pmgr_stats    *pmgr;
	struct bmgr_stats    *bmgr;
	struct misc_stats    *misc;

	if (unlikely(ptr_is_null(buf) || ptr_is_null(n) || ptr_is_null(stats)))
		return -EINVAL;

	smgr = &st->smgr_stats;
	synq = &st->syncq_stats;
	pmgr = &st->pmgr_stats;
	bmgr = &st->bmgr_stats;
	misc = &st->misc_stats;
	qos  = &st->qos_stats;

	pr_buf(buf, sz, *n,
	       " +---------------------------------------------------------------------------------------+\n");
	pr_buf_cat(buf, sz, *n,
		   " |                          Packet Processor Driver Statistics                           |\n");
	pr_buf_cat(buf, sz, *n,
		   " +------------------------------+------------+------------------------------+------------+\n");

	pr_buf_cat(buf, sz, *n, " | %-28s | %-10u | %-28s | %-10u |\n",
		   "Open Sessions", atomic_read(&smgr->sess_open),
		   "Open Sessions High Watermark",
		   atomic_read(&smgr->sess_open_hi_wm));

	pr_buf_cat(buf, sz, *n, " | %-28s | %-10u | %-28s | %-10u |\n",
		   "Active Ports", pmgr->port_count, "Active Host IF",
		   pmgr->hif_dps_count);

	pr_buf_cat(buf, sz, *n, " | %-28s | %-10u | %-28s | %-10u |\n",
		   "Active Policies", bmgr->active_policies, "Active Pools",
		   bmgr->active_pools);

	pr_buf_cat(buf, sz, *n, " | %-28s | %-10u | %-28s | %-10u |\n",
		   "Active QOS Nodes", qos->active_nodes, "Active QOS Ports",
		   qos->active_ports);

	pr_buf_cat(buf, sz, *n, " | %-28s | %-10u | %-28s | %-10u |\n",
		   "Active QOS Queues", qos->active_queues,
		   "Active QOS Schedulers", qos->active_sched);

	pr_buf_cat(buf, sz, *n, " | %-28s | %-10u | %-28s | %-10u |\n",
		   "Active Sync Queues", synq->active,
		   "Free Sync Queues", synq->total - synq->active);

	pr_buf_cat(buf, sz, *n, " | %-28s | %-10u | %-28s | %-10u |\n",
		   "Session Create Requests",
		   atomic_read(&smgr->sess_create_req), "Sessions Created",
		   atomic_read(&smgr->sess_created));

	pr_buf_cat(buf, sz, *n, " | %-28s | %-10u | %-28s | %-10u |\n",
		   "Session Delete Requests",
		   atomic_read(&smgr->sess_delete_req), "Deleted Sessions",
		   atomic_read(&smgr->sess_deleted));

	pr_buf_cat(buf, sz, *n, " | %-28s | %-10u | %-28s | %-10u |\n",
		   "RX Hook SKBs", atomic_read(&misc->rx_hook_skb_cnt),
		   "TX Hook SKBs", atomic_read(&misc->tx_hook_skb_cnt));

	pr_buf_cat(buf, sz, *n, " | %-28s | %-10u | %-28s | %-10s |\n",
		   "RX Hook SKBs with LSPP Mark",
		   atomic_read(&misc->rx_hook_lspp_pkt_cnt), "", "");

	pr_buf_cat(buf, sz, *n,
		   " +------------------------------+------------+------------------------------+------------+\n");
	pr_buf_cat(buf, sz, *n,
		   " |                                         Errors                                        |\n");
	pr_buf_cat(buf, sz, *n,
		   " +------------------------------+------------+------------------------------+------------+\n");
	pr_buf_cat(buf, sz, *n, " | %-28s | %-10u | %-28s | %-10u |\n",
		   "Session Create Fail", atomic_read(&smgr->sess_create_fail),
		   "Session Delete Fail", atomic_read(&smgr->sess_delete_fail));
	pr_buf_cat(buf, sz, *n, " | %-28s | %-10u | %-28s | %-10u |\n",
		   "RX Hook NULL SKBs",
		   atomic_read(&misc->rx_hook_null_skb_cnt),
		   "TX Hook NULL SKBs",
		   atomic_read(&misc->tx_hook_null_skb_cnt));
	pr_buf_cat(buf, sz, *n, " | %-28s | %-10u | %-28s | %-10u |\n",
		   "Invalid Arguments", atomic_read(&smgr->invalid_args),
		   "Unsupported Protocols",
		   atomic_read(&smgr->sess_not_supported));
	pr_buf_cat(buf, sz, *n,
		   " +------------------------------+------------+------------------------------+------------+\n");
	pr_buf_cat(buf, sz, *n, "\n");

	return 0;
}

s32 pp_hal_global_stats_show(char *buf, size_t sz, size_t *n, void *stats,
			     u32 num_stats, void *data)
{
	struct pp_global_stats      *st = stats;
	struct pp_stats             *port_dist;
	struct rpb_stats            *rpb;
	struct prsr_stats           *parser;
	struct cls_stats            *cls;
	struct chk_stats            *chk;
	struct mod_stats            *mod;
	struct rx_dma_stats         *rx_dma;
	struct pp_qos_stats         *qos;
	struct mcast_stats          *mcast;
	struct reassembly_stats     *reass;
	struct frag_stats           *frag;
	u64 reass_err = 0, *it;

	if (unlikely(ptr_is_null(buf) || ptr_is_null(n) || ptr_is_null(stats)))
		return -EINVAL;

	port_dist = &st->port_dist_stats;
	rpb       = &st->rpb_stats;
	parser    = &st->parser_stats;
	cls       = &st->cls_stats;
	chk       = &st->chk_stats;
	mod       = &st->mod_stats;
	rx_dma    = &st->rx_dma_stats;
	qos       = &st->qos_stats;
	mcast     = &st->mcast_stats;
	reass     = &st->reass_stats;
	frag      = &st->frag_stats;

	for_each_struct_mem(&reass->err, it)
		reass_err += *it;

	/* prints all statistics */
	pr_buf(buf, sz, *n,
	       "+---------------------------+-------------------------+----------------------+------------------------+\n");
	pr_buf_cat(buf, sz, *n,
		   "| Port Distributor          | RPB                     | Parser               | Classifier             |\n");
	pr_buf_cat(buf, sz, *n,
		   "+---------------------------+-------------------------+----------------------+------------------------+\n");
	pr_buf_cat(buf, sz, *n,
		   "| Rx            %10u  | Rx          %10u  | Rx       %10u  | Rx         %10u  |\n",
		   (u32)port_dist->packets, (u32)rpb->pkts_rcvd_from_port_dist,
		   parser->rx_valid_pkts,
		   cls->pkts_rcvd_from_parser + cls->pkts_rcvd_from_uc);
	if (pp_silicon_step_get() == PP_SSTEP_A) {
		pr_buf_cat(buf, sz, *n,
		   "|                           | Drop        %10u  | Tx       %10s  | Rx(Parser) %10u  |\n",
		   (u32)rpb->total_packets_dropped, "n/a",
		   cls->pkts_rcvd_from_parser);
	} else {
		pr_buf_cat(buf, sz, *n,
		   "|                           | Drop        %10u  | Tx       %10u  | Rx(Parser) %10u  |\n",
		   (u32)rpb->total_packets_dropped, parser->tx_pkts,
		   cls->pkts_rcvd_from_parser);
	}

	pr_buf_cat(buf, sz, *n,
		   "|                           |                         | Tx(Cls)  %10u  | Rx(uC)     %10u  |\n",
		   parser->tx_pkts_to_cls, cls->pkts_rcvd_from_uc);
	pr_buf_cat(buf, sz, *n,
		   "|                           |                         | Tx(uC)   %10u  | Search     %10u  |\n",
		   parser->tx_pkts_to_uc, cls->total_lu_requests);
	pr_buf_cat(buf, sz, *n,
		   "|                           |                         |                      | Match      %10u  |\n",
		   cls->total_lu_matches);
	pr_buf_cat(buf, sz, *n,
		   "+---------------------------+-------------------------+----------------------+------------------------+\n");

	pr_buf_cat(buf, sz, *n,
		   "+---------------------------+-------------------------+----------------------+------------------------+\n");
	pr_buf_cat(buf, sz, *n,
		   "| Checker                   | Modifier                | Rx-Dma               | QoS                    |\n");
	pr_buf_cat(buf, sz, *n,
		   "+---------------------------+-------------------------+----------------------+------------------------+\n");
	pr_buf_cat(buf, sz, *n,
		   "| Rx            %10u  | Rx          %10u  | Rx       %10u  | Rx         %10u  |\n",
		   chk->pkts_rcvd_from_cls,
		   mod->pkts_rcvd_from_chk + mod->pkts_rcvd_from_uc,
		   (u32)rx_dma->rx_pkts, (u32)qos->pkts_rcvd);

	if (pp_silicon_step_get() == PP_SSTEP_A) {
		pr_buf_cat(buf, sz, *n,
			"| Exception Cnt %10s  | Rx(Checker) %10u  | Tx       %10u  | Tx         %10u  |\n",
			"n/a", mod->pkts_rcvd_from_chk, (u32)rx_dma->tx_pkts,
			(u32)qos->pkts_transmit);
		pr_buf_cat(buf, sz, *n,
			"| Dflt session  %10s  | Rx(uC)      %10u  | Drop     %10u  | Drop       %10u  |\n",
			"n/a", mod->pkts_rcvd_from_uc, (u32)rx_dma->drop_pkts,
			(u32)qos->pkts_dropped);
	} else {
		pr_buf_cat(buf, sz, *n,
			"| Exception Cnt %10u  | Rx(Checker) %10u  | Tx       %10u  | Tx         %10u  |\n",
			chk->exception_pkt_cnt, mod->pkts_rcvd_from_chk,
			(u32)rx_dma->tx_pkts, (u32)qos->pkts_transmit);
		pr_buf_cat(buf, sz, *n,
			"| Dflt session  %10u  | Rx(uC)      %10u  | Drop     %10u  | Drop       %10u  |\n",
			chk->dflt_session_pkt_cnt, mod->pkts_rcvd_from_uc,
			(u32)rx_dma->drop_pkts, (u32)qos->pkts_dropped);
	}
	pr_buf_cat(buf, sz, *n,
		   "|                           | Modified    %10u  | Recycled %10u  |                        |\n",
		   mod->pkts_modified, (u32)rpb->total_packets_rx_dma_recycled);
	pr_buf_cat(buf, sz, *n,
		   "+---------------------------+-------------------------+----------------------+------------------------+\n");

	pr_buf_cat(buf, sz, *n,
		   "+---------------------------+-------------------------+-------------------------+\n");
	pr_buf_cat(buf, sz, *n,
		   "| Multicast                 | Reassembly              | Fragmentation           |\n");
	pr_buf_cat(buf, sz, *n,
		   "+---------------------------+-------------------------+-------------------------+\n");
	pr_buf_cat(buf, sz, *n,
		   "| Rx            %10llu  | Rx          %10llu  | Rx          %10llu  |\n",
		   mcast->rx_pkt, reass->rx_pkts, frag->rx_pkt);
	pr_buf_cat(buf, sz, *n,
		   "| Tx            %10llu  | Tx          %10llu  | Tx          %10llu  |\n",
		   mcast->tx_pkt, reass->tx_pkts, frag->tx_pkt);
	pr_buf_cat(buf, sz, *n,
		   "| Drop          %10llu  | Drop        %10llu  | Drop        %10llu  |\n",
		   mcast->drop_pkt, reass->err.dropped, frag->total_drops);
	pr_buf_cat(buf, sz, *n,
		   "| Mirror TX     %10llu  | Reassembled %10llu  | bmgr drops  %10llu  |\n",
		   mcast->mirror_tx_pkt, reass->reassembled, frag->bmgr_drops);
	pr_buf_cat(buf, sz, *n,
		   "| Mirror Drop   %10llu  | Accelerated %10llu  | df drops    %10llu  |\n",
		   mcast->mirror_drop_pkt, reass->accelerated, frag->df_drops);
	pr_buf_cat(buf, sz, *n,
		   "|                           | Diverted    %10llu  | max frags dr%10llu  |\n",
		   reass->diverted, frag->max_frags_drops);
	pr_buf_cat(buf, sz, *n,
		   "|                           | Starvation  %10llu  |                         |\n",
		   reass->err.frags_starv);
	pr_buf_cat(buf, sz, *n,
		   "|                           | Errors      %10llu  |                         |\n",
		   reass_err);
	pr_buf_cat(buf, sz, *n,
		   "+---------------------------+-------------------------+-------------------------+\n\n");

	return 0;
}

s32 pp_global_stats_show(char *buf, size_t sz, size_t *n, void *stats,
			 u32 num_stats, void *data)
{
	struct pp_global_stats      *st = stats;
	struct pp_stats             *port_dist;
	struct rpb_stats            *rpb;
	struct prsr_stats           *parser;
	struct cls_stats            *cls;
	struct chk_stats            *chk;
	struct mod_stats            *mod;
	struct rx_dma_stats         *rx_dma;
	struct pp_qos_stats         *qos;
	struct mcast_stats          *mcast;
	struct smgr_sq_stats        *syncq;
	struct frag_stats           *frag;

	if (unlikely(ptr_is_null(buf) || ptr_is_null(n) || ptr_is_null(stats)))
		return -EINVAL;

	port_dist = &st->port_dist_stats;
	rpb       = &st->rpb_stats;
	parser    = &st->parser_stats;
	cls       = &st->cls_stats;
	chk       = &st->chk_stats;
	mod       = &st->mod_stats;
	rx_dma    = &st->rx_dma_stats;
	qos       = &st->qos_stats;
	mcast     = &st->mcast_stats;
	syncq     = &st->syncq_stats;
	frag      = &st->frag_stats;

	/* prints all statistics */
	pr_buf(buf, sz, *n,
	       "+-----------------------------------------------------------------------------------------------------+\n");
	pr_buf_cat(buf, sz, *n,
		   "|           Packet Processor Statistics                                                               |\n");
	pr_buf_cat(buf, sz, *n,
		   "+---------------------------+----------------------+---------------------------+----------------------+\n");

	pr_buf_cat(buf, sz, *n, "| %-25s | %-20llu | %-25s | %-20llu |\n",
		   "PP Rx Packets", port_dist->packets, "QoS Rx Packets",
		   qos->pkts_rcvd);
	pr_buf_cat(buf, sz, *n, "| %-25s | %-20llu | %-25s | %-20llu |\n",
		   "PP Tx Packets", rx_dma->tx_pkts, "QoS Tx Packets",
		   qos->pkts_transmit);
	pr_buf_cat(buf, sz, *n, "| %-25s | %-20llu | %-25s | %-20llu |\n",
		   "PP Rx Bytes", port_dist->bytes, "QoS Rx Bytes",
		   qos->bytes_rcvd);
	pr_buf_cat(buf, sz, *n, "| %-25s | %-20s | %-25s | %-20llu |\n", "", "",
		   "QoS Tx Bytes", qos->bytes_transmit);
	pr_buf_cat(buf, sz, *n, "| %-25s | %-20llu | %-25s | %-20llu |\n",
		   "PP Total Packets Drop",
		   rx_dma->drop_pkts + rpb->total_packets_dropped,
		   "QoS Total Packets Drop", qos->pkts_dropped);

	pr_buf_cat(buf, sz, *n,
		   "+---------------------------+----------------------+---------------------------+----------------------+\n");
	pr_buf_cat(buf, sz, *n, "| %-25s | %-20u | %-25s | %-20llu |\n",
		   "Accelerated Packets", chk->accelerated_pkts,
		   "TX Fragmented Packets", frag->tx_pkt);
	pr_buf_cat(buf, sz, *n, "| %-25s | %-20u | %-25s | %-20u |\n",
		   "Non-Accelerated Packets",
		   chk->pkts_rcvd_from_cls - chk->accelerated_pkts,
		   "TX   Syncq Packets", syncq->packets_accepted);
	pr_buf_cat(buf, sz, *n, "| %-25s | %-20llu | %-25s | %-20u |\n",
		   "RX Multicast Packets", mcast->rx_pkt,
		   "Drop Syncq Packets", syncq->packets_dropped);
	pr_buf_cat(buf, sz, *n, "| %-25s | %-20llu | %-25s | %-20llu |\n",
		   "TX Multicast Packets", mcast->tx_pkt,
		   "Drop Multicast Packets", mcast->drop_pkt);
	pr_buf_cat(buf, sz, *n,
		   "+---------------------------+----------------------+---------------------------+----------------------+\n");
	pr_buf_cat(buf, sz, *n, "| %-99s |\n", "          Drop Reasons");
	pr_buf_cat(buf, sz, *n,
		   "+---------------------------+----------------------+---------------------------+----------------------+\n");

	pr_buf_cat(buf, sz, *n, "| %-25s | %-20llu | %-25s | %-20llu |\n",
		   "Early Drop (RPB)", rpb->total_packets_dropped,
		   "QoS WRED Drop", qos->wred_pkts_dropped);
	pr_buf_cat(buf, sz, *n, "| %-25s | %-20llu | %-25s | %-20llu |\n",
		   "Null Buffer Drop", rx_dma->hw.bmgr_pkt_drop,
		   "QoS Codel Drop", qos->codel_pkts_dropped);
	pr_buf_cat(buf, sz, *n, "| %-25s | %-20llu | %-25s | %-20llu |\n",
		   "PPRS Drop", rx_dma->hw.pre_pp_pkt_drop, "Parser Drop",
		   rx_dma->hw.parser_pkt_drop + rx_dma->hw.parser_uc_pkt_drop +
			   rx_dma->hw.parser_pkt_drop);
	pr_buf_cat(buf, sz, *n, "| %-25s | %-20llu | %-25s | %-20llu |\n",
		   "Classifier Drop",
		   rx_dma->hw.hw_classifier_pkt_drop +
			   rx_dma->hw.classifier_uc_pkt_drop,
		   "Checker Drop", rx_dma->hw.checker_pkt_drop);
	pr_buf_cat(buf, sz, *n, "| %-25s | %-20llu | %-25s | %-20llu |\n",
		   "Modifier Drop", rx_dma->hw.robustness_violation_drop,
		   "No Policy Drop", rx_dma->hw.bmgr_no_match_pkt_drop);
	pr_buf_cat(buf, sz, *n, "| %-25s | %-20llu | %-25s | %-20llu |\n",
		   "FSQM Drop",
		   rx_dma->hw.fsqm_pkt_len_max_drop +
			   rx_dma->hw.fsqm_null_ptr_counter,
		   "Zero Len Drop", rx_dma->hw.zero_len_drop);
	pr_buf_cat(buf, sz, *n,
		   "+---------------------------+----------------------+---------------------------+----------------------+\n");
	pr_buf_cat(buf, sz, *n, "| %-99s |\n", "          Error Packets");
	pr_buf_cat(buf, sz, *n,
		   "+---------------------------+----------------------+---------------------------+----------------------+\n");
	pr_buf_cat(buf, sz, *n, "| %-25s | %-20u | %-25s | %-20u |\n",
		   "IP Length", parser->ip_len_err, "No Payload",
		   parser->payload_err);
	pr_buf_cat(buf, sz, *n, "| %-25s | %-20u | %-25s | %-20u |\n",
		   "Protocols Overflow", parser->proto_overflow,
		   "Protocol Error",
		   parser->proto_len_err + parser->hdr_len_err);
	pr_buf_cat(buf, sz, *n, "| %-25s | %-20u | %-25s | %-20u |\n",
		   "Unsupported Last ETH Type", parser->last_eth_err,
		   "RPB Error", parser->rpb_err);
	pr_buf_cat(buf, sz, *n,
		   "+---------------------------+----------------------+---------------------------+----------------------+\n");

	return 0;
}

#ifdef CONFIG_DEBUG_FS
u32 ignore_clk_updates_get(void)
{
	return db->dbg_ignore_clock_freq_updates;
}

void ignore_clk_updates_set(u32 ignore_updates)
{
	db->dbg_ignore_clock_freq_updates = ignore_updates;
}
#endif

/**
 * @brief PP SKB cookie destructor
 * @param c skb cookie to free
 */
static void __desc_free(skb_cookie c)
{
	if (unlikely(!c)) {
		pr_err("Got null cookie to destruct\n");
		return;
	}

	kmem_cache_free(db->pp_desc_cache, (void *)c);
}

/**
 * @brief Init pp misc driver database
 * @param init_param
 * @return s32 0 on success
 */
static s32 __pp_misc_db_init(struct pp_misc_init_param *init_param)
{
	db = kzalloc(sizeof(*db), GFP_KERNEL);
	if (unlikely(!db)) {
		pr_err("Failed to allocate misc db memory of size %u\n",
		       (u32)sizeof(*db));
		return -ENOMEM;
	}

	db->pp_cookie_hnd = SKBEXT_INVALID_COOKIE_HND;

	/* allocate PP descriptor cookie cache pool */
	db->pp_desc_cache =
		kmem_cache_create("pp-cookie", sizeof(struct pp_desc), 0,
				  SLAB_HWCACHE_ALIGN | SLAB_PANIC, NULL);
	if (unlikely(!db->pp_desc_cache)) {
		pr_err("Allocating PP cookie cache failed\n");
		kfree(db);
		return -ENOMEM;
	}

	db->hw_clk = init_param->hw_clk;
	/* Limit max printout to 10 lines by default */
	db->dbg_print_cnt = 10;
	/* Do not ignore updates by default */
	db->dbg_ignore_clock_freq_updates = 0;
	return 0;
}

static u32 convert_event_to_hw_clk(unsigned long e)
{
	if (e == EPU_ADP_DFS_LVL_H)
		return HIGH_CLOCK;
	else if (e == EPU_ADP_DFS_LVL_M)
		return MED_CLOCK;
	else if (e == EPU_ADP_DFS_LVL_L)
		return LOW_CLOCK;

	pr_err("Invalid event received [%lu]\n", e);
	return HIGH_CLOCK;
}

/**
 * @brief Called to update upon clock freq' changes
 * @param nb notifier block
 * @param e new clock EPU_ADP_DFS_LVL_H | EPU_ADP_DFS_LVL_M | EPU_ADP_DFS_LVL_L
 * @param data not relevant
 * @return s32
 */
static int pm_notifier(struct notifier_block *nb, unsigned long e, void *data)
{
	u32 hw_clk;

	/* in case we would like to ignore updates, then just return */
	if (db->dbg_ignore_clock_freq_updates) {
		pr_info("DBG: PPv4 driver ignores clock freq update\n");
		return 0;
	}

	hw_clk = convert_event_to_hw_clk(e);
	if (pp_hw_clock_freq_set(hw_clk))
		pr_err("Failed to run pp_hw_clock_freq_set(%u)\n", hw_clk);
	return 0;
}
#endif /* CONFIG_PPV4_LGM */

s32 pp_misc_init(struct pp_misc_init_param *init_param)
{
	s32 ret;

	if (!init_param->valid)
		return -EINVAL;

#ifdef CONFIG_PPV4_LGM
	pr_debug("hw_clk %u, base addr %#lx\n", init_param->hw_clk,
		 (ulong)init_param->ppv4_base);

	/* database */
	ret = __pp_misc_db_init(init_param);
	if (unlikely(ret)) {
		pr_err("Failed to init database, ret %d\n", ret);
		return ret;
	}

	/* SI fields encoding/decoding init */
	pp_si_init();

	/* Descriptor fields decoding init */
	pp_desc_init();

	/* PP skb cookie */
	ret = register_skb_cookie(&db->pp_cookie_hnd, PP_DESC_COOKIE_NAME);
	if (unlikely(ret)) {
		pr_err("Failed registering PP skb cookie, ret %d\n", ret);
		return ret;
	}

	ret = pp_misc_sysfs_init(init_param->sysfs);
	if (unlikely(ret)) {
		pr_err("Failed to init sysfs stuff, ret %d\n", ret);
		return ret;
	}

	ret = epu_adp_lvl_notify_register(&nb);
	if (unlikely(ret))
		pr_err("Failed to epu_adp_lvl_notify_register\n");

#endif /* CONFIG_PPV4_LGM */

	/* debug */
	ret = pp_misc_dbg_init(init_param);
	if (unlikely(ret)) {
		pr_err("Failed to init debugfs, ret %d\n", ret);
		return ret;
	}

	return 0;
}

void pp_misc_exit(void)
{
	/* clean debug stuff */
	pp_misc_dbg_clean();

#ifdef CONFIG_PPV4_LGM
	pp_misc_sysfs_clean();

	if (db) {
		/* unregister cookie */
		unregister_skb_cookie(db->pp_cookie_hnd);
		/* destroy cookies caches */
		kmem_cache_destroy(db->pp_desc_cache);
		/* clean database */
		kfree(db);
		db = NULL;
	}
	epu_adp_lvl_notify_unregister(&nb);
#endif

	pr_debug("done\n");
}
