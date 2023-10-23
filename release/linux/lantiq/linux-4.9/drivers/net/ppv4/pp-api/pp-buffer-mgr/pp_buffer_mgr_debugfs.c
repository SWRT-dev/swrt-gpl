/*
 * pp_bm_debugfs.c
 * Description: Packet Processor bm Debug FS Interface
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018-2020 Intel Corporation
 */

#define pr_fmt(fmt) "[PP_BMGR_DBG]:%s:%d: " fmt, __func__, __LINE__

#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/debugfs.h>
#include "pp_common.h"
#include "pp_debugfs_common.h"
#include "pp_buffer_mgr.h"
#include "pp_buffer_mgr_internal.h"

static struct dentry *dbgfs;

void __dbg_conf_set_help(struct seq_file *f)
{
	seq_puts(f, "\"<max_policies> <max_pools>\"\n");
}

void __dbg_conf_set(char *cmd_buf, void *data)
{
	struct pp_bmgr_init_param cfg;

	if (sscanf(cmd_buf, "%u %u",
		   &cfg.max_policies,
		   &cfg.max_pools) != 2) {
		return;
	}
	if (bmgr_config_set(&cfg)) {
		pr_err("bmgr_config_set failed\n");
		return;
	}
}

PP_DEFINE_DEBUGFS(conf_set, __dbg_conf_set_help, __dbg_conf_set);

void __dbg_pool_add_help(struct seq_file *f)
{
	seq_puts(f,
		 "<flags> <num_buffs> <buff_size> <base addr - 0 allocating memory>\n");
}

void __dbg_pool_add(char *cmd_buf, void *data)
{
	struct pp_bmgr_pool_params cfg;
	u8     pool_id = PP_BM_INVALID_POOL_ID;

	if (sscanf(cmd_buf, "%hu %u %u %u",
		   &cfg.flags,
		   &cfg.num_buffers,
		   &cfg.size_of_buffer,
		   &cfg.base_addr_low) != 4) {
		pr_err("sscanf err\n");
		return;
	}

	cfg.base_addr_high = 0;

	if (pp_bmgr_pool_configure(&cfg, &pool_id)) {
		pr_err("pp_bmgr_pool_configure failed\n");
		return;
	} else {
		pr_info("Pool %d was created successfully\n", pool_id);
	}
}

PP_DEFINE_DEBUGFS(pool_add, __dbg_pool_add_help, __dbg_pool_add);

void __dbg_pool_rem_help(struct seq_file *f)
{
	seq_puts(f, "<pool_id>\n");
}

void __dbg_pool_rem(char *cmd_buf, void *data)
{
	int    str_ret;
	u8     pool_id;

	str_ret = kstrtou8(cmd_buf, 10, &pool_id);
	if (str_ret) {
		pr_err("kstrtou8 err %d\n", str_ret);
		return;
	}

	if (pp_bmgr_pool_pop_disable(pool_id)) {
		pr_err("pp_bmgr_pool_pop_disable failed\n");
		return;
	} else {
		pr_info("Pool %d pop was disabled successfully\n", pool_id);
	}

	if (pp_bmgr_pool_remove(pool_id)) {
		pr_err("pp_bmgr_pool_remove failed\n");
		return;
	} else {
		pr_info("Pool %d was removed successfully\n", pool_id);
	}
}

PP_DEFINE_DEBUGFS(pool_rem, __dbg_pool_rem_help, __dbg_pool_rem);

void pool_verify_help(struct seq_file *f)
{
	seq_puts(f, "<pool_id>\n");
}

void pool_verify(char *buf, void *data)
{
	int ret;
	u32 id;

	ret = kstrtouint(buf, 0, &id);
	if (ret) {
		pr_err("unable to parse %s, ret %d\n", buf, ret);
		return;
	}

	ret = pp_bmgr_pool_verification(id);
	if (ret)
		pr_info("Pool verification failed, ret %d\n", ret);
}

PP_DEFINE_DEBUGFS(pool_verify, pool_verify_help, pool_verify);

void __dbg_policy_add_help(struct seq_file *f)
{
	seq_puts(f, "<max_allowed> <min_guaranteed> <poolN id>"
		    "<poolN max_allow> Nx4 <num_pools>\n");
}

void __dbg_policy_add(char *cmd_buf, void *data)
{
	struct pp_bmgr_policy_params cfg;
	u16    policy_id = PP_BM_INVALID_POLICY_ID;

	if (sscanf(cmd_buf, "%u %u %hhu %u %hhu %u %hhu %u %hhu %u %hhu",
		   &cfg.max_allowed,
		   &cfg.min_guaranteed,
		   &cfg.pools_in_policy[0].pool_id,
		   &cfg.pools_in_policy[0].max_allowed,
		   &cfg.pools_in_policy[1].pool_id,
		   &cfg.pools_in_policy[1].max_allowed,
		   &cfg.pools_in_policy[2].pool_id,
		   &cfg.pools_in_policy[2].max_allowed,
		   &cfg.pools_in_policy[3].pool_id,
		   &cfg.pools_in_policy[3].max_allowed,
		   &cfg.num_pools_in_policy) != 11) {
		return;
	}

	if (pp_bmgr_policy_configure(&cfg, &policy_id)) {
		pr_err("pp_bmgr_policy_configure failed\n");
		return;
	} else {
		pr_info("Policy %d was created successfully\n", policy_id);
	}
}

PP_DEFINE_DEBUGFS(policy_add, __dbg_policy_add_help, __dbg_policy_add);

void __dbg_policy_rem_help(struct seq_file *f)
{
	seq_puts(f, "<policy_id>\n");
}

void __dbg_policy_rem(char *cmd_buf, void *data)
{
	int str_ret;
	u16 policy_id;

	str_ret = kstrtou16(cmd_buf, 10, &policy_id);
	if (str_ret) {
		pr_err("kstrtou16 err\n");
		return;
	}

	if (pp_bmgr_policy_remove(policy_id)) {
		pr_err("pp_bmgr_policy_remove failed\n");
		return;
	} else {
		pr_info("Policy %d was removed successfully\n", policy_id);
	}
}

PP_DEFINE_DEBUGFS(policy_rem, __dbg_policy_rem_help, __dbg_policy_rem);

void __dbg_policy_reset_help(struct seq_file *f)
{
	seq_puts(f, "<policy_id>\n");
}

static void policy_reset_cb(s32 ret)
{
	pr_info("Policy reset return value: %d\n", ret);
}

void __dbg_policy_reset(char *cmd_buf, void *data)
{
	int    str_ret;
	u16    policy_id;

	str_ret = kstrtou16(cmd_buf, 10, &policy_id);
	if (str_ret) {
		pr_err("kstrtou16 err\n");
		return;
	}

	if (pp_bmgr_policy_reset(policy_id, &policy_reset_cb)) {
		pr_err("pp_bmgr_policy_reset failed\n");
		return;
	} else {
		pr_info("Policy %d was reset successfully\n", policy_id);
	}
}

PP_DEFINE_DEBUGFS(policy_reset, __dbg_policy_reset_help, __dbg_policy_reset);

void __dbg_conf_get(struct seq_file *f)
{
	struct pp_bmgr_init_param cfg;

	pp_bmgr_config_get(&cfg);

	seq_puts(f, "+=========+==========+======+\n");
	seq_puts(f, "| Pools | Policies |\n");
	seq_puts(f, "+-------------------+\n");
	seq_printf(f, "|  %-2d   |    %-2d    |\n",
		   cfg.max_pools, cfg.max_policies);
	seq_puts(f, "+=========+==========+======+\n");
}

PP_DEFINE_DEBUGFS(conf_get, __dbg_conf_get, NULL);

static void dump_pools(struct seq_file *f)
{
	u8     idx, flag;
	struct bmgr_pool_db_entry *pool;
	struct bmgr_driver_db *db = f->private;
	ulong flags;
	const char *flags_str[BMGR_NUM_FLAGS] = {
		"MIN GUARANTEE",
		"ISOLATED",
	};

	/* Dump all pools */
	seq_puts(f, "============= POOLS =============\n");
	for_each_set_bit(idx, db->pools_bmap, db->cfg.max_pools) {
		pool = &db->pools[idx];
		flags = pool->params.flags;

		seq_printf(f, "Active pool %d:\n", idx);
		seq_printf(f, "\t\tNum allocated buffers: %d\n",
			   pool->num_allocated_buffers);
		seq_printf(f, "\t\tNum deallocated buffers: %d\n",
			   pool->num_deallocated_buffers);
		seq_printf(f, "\t\tInternal table address: %p\n",
			   pool->internal_ptrs_virt);
		seq_printf(f, "\t\tNum buffers: %d\n",
			   pool->params.num_buffers);
		seq_printf(f, "\t\tSize of buffer: %d\n",
			   pool->params.size_of_buffer);
		seq_printf(f, "\t\tBase address : 0x%llx\n",
			   PP_BUILD_64BIT_WORD(pool->params.base_addr_low,
					       pool->params.base_addr_high));
		seq_printf(f, "\t\tFlags: %#x ", pool->params.flags);
		for_each_set_bit(flag, &flags, BMGR_NUM_FLAGS)
			seq_printf(f, "[%s]", flags_str[flag]);
		seq_puts(f, "\n");

		seq_printf(f, "\t\t%u Policies: %*pbl\n",
			   pool->policy_ref_cnt, PP_BM_MAX_POLICIES,
			   pool->policies_bmap);
	}
}

static void dump_groups(struct seq_file *f)
{
	struct bmgr_driver_db *db = f->private;
	struct bmgr_group_db_entry *group;
	u32 idx, num_policies, num_pools;

	/* Dump all groups */
	seq_puts(f, "============= GROUPS ============\n");

	for_each_set_bit(idx, db->groups_bmap, db->cfg.max_groups) {
		group = &db->groups[idx];
		num_policies =
			bitmap_weight(group->policies_bmap, PP_BM_MAX_POLICIES);
		num_pools =
			bitmap_weight(group->pools_bmap, PP_BM_MAX_POOLS);
		seq_printf(f, "Active group %d:\n", idx);
		seq_printf(f, "\t\tAvailable buffers: %d\n", group->available);
		seq_printf(f, "\t\tReserved buffers: %d\n", group->reserved);
		seq_printf(f, "\t\t%u Policies: %*pbl\n", num_policies,
			   PP_BM_MAX_POLICIES, group->policies_bmap);
		seq_printf(f, "\t\t%u Pools   : %*pbl\n", num_pools,
			   PP_BM_MAX_POOLS, group->pools_bmap);
		seq_puts(f, "\n");
	}
}

static void dump_policies(struct seq_file *f)
{
	struct bmgr_policy_db_entry *policy;
	struct pp_bmgr_pool_in_policy_info *pool_in_policy;
	u16    idx;
	u8     idx2;
	u32    num = 0;
	u32    pool_id = 0;
	u32    max_allowed = 0;
	struct bmgr_driver_db *db = f->private;

	/* Dump all policies */
	seq_puts(f, "============= POLICIES ==========\n");
	for_each_set_bit(idx, db->policies_bmap, db->cfg.max_policies) {
		policy = &db->policies[idx];

		seq_printf(f, "Active policy %d:\n", idx);
		seq_printf(f, "\t\tNum allocated buffers: %d\n",
			   policy->num_allocated_buffers);
		seq_printf(f, "\t\tNum deallocated buffers: %d\n",
			   policy->num_deallocated_buffers);
		seq_printf(f, "\t\tMax allowed: %d\n",
			   policy->params.max_allowed);
		seq_printf(f, "\t\tMin guaranteed: %d\n",
			   policy->params.min_guaranteed);
		seq_puts(f, "\t\tPools in policy:\n");
		num = policy->params.num_pools_in_policy;
		for (idx2 = 0; idx2 < num; idx2++) {
			pool_in_policy =
				&policy->params.pools_in_policy[idx2];
			pool_id = pool_in_policy->pool_id;
			max_allowed = pool_in_policy->max_allowed;

			seq_printf(f, "\t\t\t%d. id %d, max allowed %d\n",
				   idx2, pool_id, max_allowed);
		}
	}
}

static void __dump_db(struct seq_file *f)
{
	struct bmgr_driver_db *db = f->private;

	seq_puts(f, "=================================\n");
	seq_puts(f, "====== BUFFER MANAGER DUMP ======\n");
	seq_puts(f, "=================================\n");

	/* Dump all DB general counters */
	seq_puts(f, "======= GENERAL COUNTERS ========\n");
	seq_printf(f, "%u Active pools   : %*pbl\n",
		   db->num_pools, db->cfg.max_pools, db->pools_bmap);
	seq_printf(f, "%u Active policies: %*pbl\n",
		   db->num_policies, db->cfg.max_policies, db->policies_bmap);
	seq_printf(f, "%u Active groups  : %*pbl\n",
		   db->num_groups, db->cfg.max_groups, db->groups_bmap);

	dump_pools(f);
	dump_groups(f);
	dump_policies(f);

	seq_puts(f, "=================================\n");
	seq_puts(f, "========== END OF DUMP ==========\n");
	seq_puts(f, "=================================\n");
}

void __dbg_dump_hw_stats(struct seq_file *f)
{
	struct  bmgr_driver_db *db = f->private;
	struct  bmgr_cfg_regs cfg_regs;
	struct  pp_bmgr_pool_stats pool_stats;
	struct  pp_bmgr_policy_stats policy_stats;
	struct  bmgr_group_stats group_stats;
	struct  pp_bmgr_policy_params *policy_params;
	bool    pool_isolated;
	u16     i, j, n;
	char    pools[64], group[32];

	bmgr_get_cfg_regs(&cfg_regs);

	seq_printf(f, "\nControl = 0x%x\n",
		   cfg_regs.ctrl_reg);
	seq_printf(f, "Pool Min Grant Bit Mask = 0x%x\n",
		   cfg_regs.min_grant_reg);
	seq_printf(f, "Pool Enable = 0x%x\n",
		   cfg_regs.pool_enable_reg);
	seq_printf(f, "Pool FIFO Reset = 0x%x\n",
		   cfg_regs.pool_fifo_reset_reg);
	seq_printf(f, "OCP Master Burst Size = 0x%x\n",
		   cfg_regs.ocpm_burst_size_reg);
	seq_printf(f, "OCP Master Number Of Bursts = 0x%x\n",
		   cfg_regs.ocpm_num_bursts_reg);
	seq_printf(f, "Status = 0x%x\n",
		   cfg_regs.status_reg);

	seq_puts(f, "\n+===========================================================================================================+\n");
	seq_puts(f, "|------------------------------------------  Pool Configuration  -------------------------------------------+\n");
	seq_puts(f, "+======+======+================+===========+================+===============+====================+==========+\n");
	seq_puts(f, "| Pool | Size | FIFO Base Addr | FIFO Size | Prog Empty Thr | Prog Full Thr | Ext FIFO Base Addr | Isolated |\n");
	seq_puts(f, "+------+------+----------------+-----------+----------------+---------------+--------------------+----------+\n");
	for_each_set_bit(i, db->pools_bmap, db->cfg.max_pools) {
		pool_isolated =
			db->pools[i].params.flags &
			POOL_ISOLATED;

		pp_bmgr_pool_stats_get(&pool_stats, i);
		seq_printf(f, "|%-6d|%-6d|   0x%8x   |%-11d|%-16d|%-15d|     0x%8x     |  %s   |\n",
			   i,
			   pool_stats.pool_size,
			   pool_stats.pcu_fifo_base_addr,
			   pool_stats.pcu_fifo_size,
			   pool_stats.pcu_fifo_prog_empty,
			   pool_stats.pcu_fifo_prog_full,
			   (u32)(pool_stats.ext_fifo_base_addr),
			   pool_isolated ? "True " : "False");
	}
	seq_puts(f, "+======+======+================+===========+================+===============+====================+==========+\n");

	seq_puts(f, "\n+=============================================================================================================+\n");
	seq_puts(f, "|----------------------------------------------  Pool Statistics  --------------------------------------------+\n");
	seq_puts(f, "+======+==========+==============+=========+===========+============+==========+==========+=========+=========+\n");
	seq_puts(f, "| Pool | FIFO Occ | Ext FIFO Occ |  Alloc  |    Pop    |    Push    | Burst wr | Burst rd | Wm lthr | Wm lctr |\n");
	seq_puts(f, "+------+----------+--------------+---------+-----------+------------+----------+----------+---------+---------+\n");
	for_each_set_bit(i, db->pools_bmap, db->cfg.max_pools) {
		pp_bmgr_pool_stats_get(&pool_stats, i);
		seq_printf(f, "|%-6d|%-10d|%-14d|%-9d|%-11d|%-12d|%-10d|%-10d|%-9d|%-9d|\n",
			   i,
			   pool_stats.pcu_fifo_occupancy,
			   pool_stats.ext_fifo_occupancy,
			   pool_stats.pool_allocated_ctr,
			   pool_stats.pool_pop_ctr,
			   pool_stats.pool_push_ctr,
			   pool_stats.pool_ddr_burst_write_ctr,
			   pool_stats.pool_ddr_burst_read_ctr,
			   pool_stats.pool_watermark_low_thr,
			   pool_stats.pool_watermark_low_ctr);
	}
	seq_puts(f, "+=============================================================================================================+\n");

	seq_puts(f, "\n+==============================================+\n");
	seq_puts(f, "|--------------- Group Statistics -------------+\n");
	seq_puts(f, "+=======+===================+==================+\n");
	seq_puts(f, "| Group | Available Buffers | Reserved Buffers |\n");
	seq_puts(f, "+-------+-------------------+------------------+\n");
	for_each_set_bit(i, db->groups_bmap, db->cfg.max_groups) {
		bmgr_group_stats_get(&group_stats, i);
		seq_printf(f, "|%-7d|%-19d|%-18d|\n",
			   i,
			   group_stats.grp_avail_buff,
			   group_stats.grp_rsrvd_buff);
	}
	seq_puts(f, "+=======+===================+==================+\n\n");

	seq_puts(f, "+==========================================================================================================+\n");
	seq_puts(f, "|------------------------------------------  Policy Statistics  -------------------------------------------+\n");
	seq_puts(f, "+========+==========+=============+================+===================+===============+===================+\n");
	seq_puts(f, "| Policy | Null ctr | Max Allowed | Min Guaranteed | Group Association | Pools Mapping | Allocated Buffers |\n");
	seq_puts(f, "+--------+----------+-------------+----------------+-------------------+---------------+-------------------+\n");
	for_each_set_bit(i, db->policies_bmap, db->cfg.max_policies) {
		pp_bmgr_policy_stats_get(&policy_stats, i);

		pr_buf(pools, sizeof(pools), n, "%*pbl", PP_BM_MAX_POOLS,
		       policy_stats.pools_bmap);

		if (policy_stats.policy_grp_association ==
		    PP_BM_INVALID_GROUP_ID)
			pr_buf(group, sizeof(group), n, "Invalid");
		else
			pr_buf(group, sizeof(group), n, "%u",
			       policy_stats.policy_grp_association);

		seq_printf(f, "|%-8d|%-10d|%-13d|%-16d|%-19s|%-15s|%-19u|\n",
			   i,
			   policy_stats.policy_null_ctr,
			   policy_stats.policy_max_allowed,
			   policy_stats.policy_min_guaranteed,
			   group,
			   pools,
			   policy_stats.policy_alloc_buff);
	}
	seq_puts(f, "+==========================================================================================================+\n");

	seq_puts(f, "\n+=================================================+\n");
	seq_puts(f, "|------------  Policy/Pool Statistics  -----------+\n");
	seq_puts(f, "+========+======+=============+===================+\n");
	seq_puts(f, "| Policy | Pool | Max Allowed | Allocated Buffers |\n");
	seq_puts(f, "+--------+------+-------------+-------------------+\n");
	for_each_set_bit(i, db->policies_bmap, db->cfg.max_policies) {
		policy_params = &db->policies[i].params;

		pp_bmgr_policy_stats_get(&policy_stats, i);
		for (j = 0; j < policy_params->num_pools_in_policy; j++) {
			seq_printf(f, "|%-8d|%-6d|%-13d|%-19d|\n",
				   i, policy_params->pools_in_policy[j].pool_id,
				   policy_stats.policy_max_allowed_per_pool[j],
				   policy_stats.policy_alloc_buff_per_pool[j]);
		}
	}
	seq_puts(f, "+========+======+=============+===================+\n");
}

PP_DEFINE_DEBUGFS(hw_stats, __dbg_dump_hw_stats, NULL);

void __dbg_dump_sw_stats(struct seq_file *f)
{
	__dump_db(f);
	seq_printf(f, "\nbm successful pop  requests  : %-10u\n",
		   bmgr_get_succ_pop_req());
	seq_printf(f, "bm successful push requests  : %-10u\n",
		   bmgr_get_succ_push_req());
	seq_printf(f, "bm failed     pop  requests  : %-10u\n",
		   bmgr_get_fail_pop_req());
	seq_printf(f, "bm failed     push requests  : %-10u\n\n",
		   bmgr_get_fail_push_req());
}

PP_DEFINE_DEBUGFS(sw_stats, __dbg_dump_sw_stats, NULL);

void pop_help(struct seq_file *f)
{
	seq_puts(f, "\"<policy id> <buff count (0 for all)>\"\n");
}

#define BM_MAX_BUFFER_POPS 1000

void pop(char *cmd_buf, void *data)
{
	u32 policy, pool, n, i;
	u64 buff = 0;

	if (sscanf(cmd_buf, "%u %u", &policy, &n) != 2)
		return;

	if (!n)
		n = BM_MAX_BUFFER_POPS;

	for (i = 0; i < n; i++) {
		pp_bmgr_pop_buffer(policy, &pool, &buff);
		if (!buff)
			break;

		pr_info("POP ==> %#llx (pool %u)\n", buff, pool);
	}
	pr_info("POP total of %u buffers\n", i);
}

PP_DEFINE_DEBUGFS(pop, pop_help, pop);

void push_help(struct seq_file *f)
{
	seq_puts(f, "\"<buff addr> <pool id> <policy id>\"\n");
}

void push(char *cmd_buf, void *data)
{
	u32 policy, pool;
	u64 buff;

	if (sscanf(cmd_buf, "%llx %u %u", &buff, &pool, &policy) != 3)
		return;

	pr_info("<== PUSH %#llx (pool %u, policy %u)\n", buff, pool, policy);
	pp_bmgr_push_buffer(policy, pool, buff);
}

PP_DEFINE_DEBUGFS(push, push_help, push);

static struct debugfs_file bm_debugfs_files[] = {
	{"hw_stats", &PP_DEBUGFS_FOPS(hw_stats)},
	{"sw_stats", &PP_DEBUGFS_FOPS(sw_stats)},
	{"pop", &PP_DEBUGFS_FOPS(pop)},
	{"push", &PP_DEBUGFS_FOPS(push)},
	{"conf_get", &PP_DEBUGFS_FOPS(conf_get)},
	{"conf_set", &PP_DEBUGFS_FOPS(conf_set)},
	{"pool_add", &PP_DEBUGFS_FOPS(pool_add)},
	{"pool_rem", &PP_DEBUGFS_FOPS(pool_rem)},
	{"pool_verify", &PP_DEBUGFS_FOPS(pool_verify)},
	{"policy_add", &PP_DEBUGFS_FOPS(policy_add)},
	{"policy_rem", &PP_DEBUGFS_FOPS(policy_rem)},
	{"policy_reset", &PP_DEBUGFS_FOPS(policy_reset)},
};

int bm_dbg_init(struct bmgr_driver_db *db, struct dentry *parent)
{
	return pp_debugfs_create(parent, "buff_mgr", &dbgfs, bm_debugfs_files,
				 ARRAY_SIZE(bm_debugfs_files), db);
}

void bm_dbg_clean(void)
{
	debugfs_remove_recursive(dbgfs);
}
