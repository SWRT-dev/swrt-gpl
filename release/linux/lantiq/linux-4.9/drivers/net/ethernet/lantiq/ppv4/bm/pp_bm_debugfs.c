/*
 * GPL LICENSE SUMMARY
 *
 *  Copyright(c) 2017 Intel Corporation.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of version 2 of the GNU General Public License as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 *  The full GNU General Public License is included in this distribution
 *  in the file called LICENSE.GPL.
 *
 *  Contact Information:
 *  Intel Corporation
 *  2200 Mission College Blvd.
 *  Santa Clara, CA  97052
 */

#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/platform_device.h>
#include <linux/debugfs.h>
#include "pp_bm_drv_internal.h"

#define PP_BM_DEBUGFS_DIR "ppv4_bm"

typedef int (*file_func)(struct seq_file *, void *);

static int dbg_dump_hw_stats(struct seq_file *f, void *unused);
static int dbg_dump_sw_stats(struct seq_file *f, void *unused);
static int dbg_pop(struct seq_file *f, void *unused);
static int dbg_push(struct seq_file *f, void *unused);
static int dbg_test_dma(struct seq_file *f, void *unused);
static int dbg_test_init(struct seq_file *f, void *unused);

struct bm_debugfs_file_info {
	const char	*name;
	file_func	func;
};

static struct bm_debugfs_file_info bm_debugfs_files[] = {
		{"hw_stats", dbg_dump_hw_stats},
		{"sw_stats", dbg_dump_sw_stats},
		{"pop", dbg_pop},
		{"push", dbg_push},
		{"test_dma", dbg_test_dma},
		{"test_init", dbg_test_init},
};

//!< debugfs dir
static struct dentry *bm_dir;
static u16 policy_pop_test;
static struct bmgr_buff_info buff_info;

static void dump_pools(struct seq_file *f, struct bmgr_driver_private *pdata)
{
	u8	idx;
	struct bmgr_pool_db_entry *pool;

	// Dump all pools
	seq_puts(f, "============= POOLS =============\n");
	for (idx = 0; idx < pdata->driver_db.num_pools; idx++) {
		pool = &pdata->driver_db.pools[idx];

		if (!pool->is_busy)
			continue;

		seq_printf(f, "Active pool %d:\n", idx);
		seq_printf(f, "\t\tNum allocated buffers: %d\n",
			pool->num_allocated_buffers);
		seq_printf(f, "\t\tNum deallocated buffers: %d\n",
			pool->num_deallocated_buffers);
		seq_printf(f, "\t\tInternal table address: %x\n",
			(phys_addr_t)pool->internal_pointers_tables);
		seq_printf(f, "\t\tNum buffers: %d\n",
			pool->pool_params.num_buffers);
		seq_printf(f, "\t\tSize of buffer: %d\n",
			pool->pool_params.size_of_buffer);
		seq_printf(f, "\t\tGroup id: %d\n",
			pool->pool_params.group_id);
		seq_printf(f, "\t\tBase address low: 0x%x\n",
			pool->pool_params.base_addr_low);
		seq_printf(f, "\t\tBase address high: 0x%x\n",
			pool->pool_params.base_addr_high);
		seq_printf(f, "\t\tFlags: %d\n",
			pool->pool_params.flags);
	}
}

static void dump_groups(struct seq_file *f, struct bmgr_driver_private *pdata)
{
	struct bmgr_group_db_entry	*group;
	u8	idx;
	u8	idx2;
	u32	num = 0;

	// Dump all groups
	seq_puts(f, "============= GROUPS ============\n");
	for (idx = 0; idx < pdata->driver_db.num_groups; idx++) {
		group = &pdata->driver_db.groups[idx];

		if (group->num_pools_in_group) {
			seq_printf(f, "Active group %d:\n", idx);
			seq_printf(f, "\t\tAvailable buffers: %d\n",
				group->available_buffers);
			seq_printf(f, "\t\tReserved buffers: %d\n",
				group->reserved_buffers);
			seq_puts(f, "\t\tPools in group:");
			num = group->num_pools_in_group;
			for (idx2 = 0; idx2 < num; idx2++) {
				if (group->pools[idx2])
					seq_printf(f, " %d", idx2);
			}
			seq_puts(f, "\n");
		}
	}
}

static void dump_policies(struct seq_file *f, struct bmgr_driver_private *pdata)
{
	struct bmgr_policy_db_entry *policy;
	struct bmgr_pool_in_policy_info *pool_in_policy;
	u16	idx;
	u8	idx2;
	u32	num = 0;
	u32	pool_id = 0;
	u32	max_allowed = 0;

	// Dump all policies
	seq_puts(f, "============= POLICIES ==========\n");
	for (idx = 0; idx < pdata->driver_db.num_policies; idx++) {
		policy = &pdata->driver_db.policies[idx];

		if (!policy->is_busy)
			continue;

		seq_printf(f, "Active policy %d:\n", idx);
		seq_printf(f, "\t\tNum allocated buffers: %d\n",
			policy->num_allocated_buffers);
		seq_printf(f, "\t\tNum deallocated buffers: %d\n",
			policy->num_deallocated_buffers);
		seq_printf(f, "\t\tMax allowed: %d\n",
			policy->policy_params.max_allowed);
		seq_printf(f, "\t\tMin guaranteed: %d\n",
			policy->policy_params.min_guaranteed);
		seq_printf(f, "\t\tGroup id: %d\n",
			policy->policy_params.group_id);
		seq_printf(f, "\t\tFlags: %d\n",
			policy->policy_params.flags);
		seq_puts(f, "\t\tPools in policy:\n");
		num = policy->policy_params.num_pools_in_policy;
		for (idx2 = 0; idx2 < num; idx2++) {
			pool_in_policy =
				&policy->policy_params.pools_in_policy[idx2];
			pool_id = pool_in_policy->pool_id;
			max_allowed = pool_in_policy->max_allowed;

			seq_printf(f, "\t\t\t%d. id %d, max allowed %d\n",
				idx2, pool_id, max_allowed);
		}
	}
}

static void dump_db(struct seq_file *f, struct bmgr_driver_private *pdata)
{
	seq_puts(f, "=================================\n");
	seq_puts(f, "====== BUFFER MANAGER DUMP ======\n");
	seq_puts(f, "=================================\n");

	// Dump all DB general counters
	seq_puts(f, "======= GENERAL COUNTERS ========\n");
	seq_printf(f, "Number of active pools:    %d\n",
		pdata->driver_db.num_pools);
	seq_printf(f, "Number of active groups:   %d\n",
		pdata->driver_db.num_groups);
	seq_printf(f, "Number of active policies: %d\n",
		pdata->driver_db.num_policies);

	dump_pools(f, pdata);
	dump_groups(f, pdata);
	dump_policies(f, pdata);

	seq_puts(f, "=================================\n");
	seq_puts(f, "========== END OF DUMP ==========\n");
	seq_puts(f, "=================================\n");
}

static int dbg_dump_hw_stats(struct seq_file *f, void *unused)
{
	struct bmgr_driver_private *pdata;

	pdata = dev_get_drvdata(f->private);

	print_hw_stats();

	return 0;
}

static int dbg_dump_sw_stats(struct seq_file *f, void *unused)
{
	struct bmgr_driver_private *pdata;

	pdata = dev_get_drvdata(f->private);

	dump_db(f, pdata);

	return 0;
}

static int dbg_test_dma(struct seq_file *f, void *unused)
{
	struct bmgr_driver_private *pdata;

	pdata = dev_get_drvdata(f->private);

	bmgr_test_dma(8);

	return 0;
}

static int dbg_test_init(struct seq_file *f, void *unused)
{
	struct bmgr_driver_private	*pdata;
	struct bmgr_pool_params		pool_params;
	struct bmgr_policy_params	policy_params;
	u8				pool_id;
	u8				policy_id;
	u8				num_buffers = 10;
	u8				size_of_buffer = 64;
	void				*ptr = NULL;

	pdata = dev_get_drvdata(f->private);

	if (bmgr_driver_init() != 0) {
		seq_puts(f, "dbg_test_init(): bmgr_driver_init failed\n");
		return 0;
	}

	ptr = devm_kzalloc(f->private,
			   size_of_buffer * num_buffers,
			   GFP_KERNEL);
	if (!ptr)
		return 0;

	pool_params.base_addr_low = (u32)ptr;

	pool_params.base_addr_high = 0;
	pool_params.size_of_buffer = size_of_buffer;
	pool_params.num_buffers = num_buffers;
	pool_params.group_id = 0;
	pool_params.flags = 0;
	if (bmgr_pool_configure(&pool_params, &pool_id) != 0) {
		seq_puts(f, "dbg_test_init(): bmgr_pool_configure failed\n");
		return 0;
	}

	seq_printf(f, "dbg_test_init(): configured pool_id %d with %d buffers (of %d), group id 0, address 0x%x\n",
		pool_id, num_buffers, size_of_buffer, (u32)ptr);

	policy_params.flags = 0;
	policy_params.group_id = 0;
	policy_params.max_allowed = num_buffers / 2;
	policy_params.min_guaranteed = num_buffers / 5;
	policy_params.num_pools_in_policy = 1;
	policy_params.pools_in_policy[0].max_allowed = num_buffers;
	policy_params.pools_in_policy[0].pool_id = 0;
	if (bmgr_policy_configure(&policy_params, &policy_id) != 0) {
		seq_puts(f, "dbg_test_init(): bmgr_policy_configure failed\n");
		return 0;
	}

	seq_printf(f, "dbg_test_init(): configured policy_id %d with max allowed %d, min guaranteed %d, pool 0 (max allowed %d) group 0\n",
		policy_id, num_buffers / 2, num_buffers / 5, num_buffers);

	return 0;
}

// Currently pop one buffer from policies 0-3 in increament order
static int dbg_pop(struct seq_file *f, void *unused)
{
	struct bmgr_driver_private *pdata;

	pdata = dev_get_drvdata(f->private);

	memset(&buff_info, 0x0, sizeof(buff_info));

	buff_info.num_allocs = 1;
	buff_info.policy_id = policy_pop_test;

	bmgr_pop_buffer(&buff_info);

	policy_pop_test++;
	if (policy_pop_test == 4)
		policy_pop_test = 0;

	seq_printf(f, "pop buffer address 0x%x (high 0x%x) from policy %d pool %d\n",
		buff_info.addr_low[0], buff_info.addr_high[0],
		buff_info.policy_id, buff_info.pool_id[0]);

	return 0;
}

// Currently push only last pop-ed buffer
static int dbg_push(struct seq_file *f, void *unused)
{
	struct bmgr_driver_private *pdata;

	pdata = dev_get_drvdata(f->private);

	bmgr_push_buffer(&buff_info);

	seq_printf(f, "push buffer address 0x%x (high 0x%x) from policy %d pool %d\n",
		buff_info.addr_low[0], buff_info.addr_high[0],
		buff_info.policy_id, buff_info.pool_id[0]);

	return 0;
}

int bm_dbg_dev_init(struct platform_device *pdev)
{
	struct bmgr_driver_private *pdata;
	struct dentry *dent;
	int err;
	u32 num_files = sizeof(bm_debugfs_files)/sizeof(struct bm_debugfs_file_info);
	u16 ind = 0;

	if (!pdev) {
		dev_err(&pdev->dev, "Invalid platform device\n");
		return -ENODEV;
	}

	pdata = platform_get_drvdata(pdev);
	if (!pdata) {
		dev_err(&pdev->dev, "Invalid platform device data\n");
		return -ENODEV;
	}

	pdata->debugfs_info.dir = bm_dir;

	dent = debugfs_create_u32("reg_bar_offset", 0644,
				  pdata->debugfs_info.dir,
				  &pdata->debugfs_info.virt2phyoff);
	if (IS_ERR_OR_NULL(dent)) {
		err = (int)PTR_ERR(dent);
		dev_err(&pdev->dev,
			"debugfs_create_u32 failed creating reg_bar_offset with %d\n",
			err);
		return err;
	}

	dent = debugfs_create_u32("ddr_bar_offset", 0644,
				  pdata->debugfs_info.dir,
				  &pdata->debugfs_info.ddrvirt2phyoff);
	if (IS_ERR_OR_NULL(dent)) {
		err = (int)PTR_ERR(dent);
		dev_err(&pdev->dev,
			"debugfs_create_u32 failed creating ddr_bar_offset with %d\n",
			err);
		return err;
	}

	dent = debugfs_create_u32("fpga_bar_offset", 0644,
				  pdata->debugfs_info.dir,
				  &pdata->debugfs_info.fpga_offset);
	if (IS_ERR_OR_NULL(dent)) {
		err = (int)PTR_ERR(dent);
		dev_err(&pdev->dev,
			"debugfs_create_u32 failed creating fpga_offset with %d\n",
			err);
		return err;
	}

	for (ind = 0; ind < num_files; ind++) {
		dent = debugfs_create_devm_seqfile(&pdev->dev,
						   bm_debugfs_files[ind].name,
						   pdata->debugfs_info.dir,
						   bm_debugfs_files[ind].func);
		if (IS_ERR_OR_NULL(dent)) {
			err = (int)PTR_ERR(dent);
			dev_err(&pdev->dev,
				"debugfs_create_file failed creating %s with %d\n",
				bm_debugfs_files[ind].name,
				err);
			return err;
		}
	}

	return 0;
}

void bm_dbg_dev_clean(struct platform_device *pdev)
{
//	struct bmgr_driver_private *pdata;

//	if (pdev) {
//		pdata = platform_get_drvdata(pdev);
//		if (pdata)
//			debugfs_remove_recursive(pdata->dbg_info.dir);
//	}
}

int bm_dbg_module_init(void)
{
	int rc;
	struct dentry *dir;

	dir = debugfs_create_dir(PP_BM_DEBUGFS_DIR, NULL);
	if (IS_ERR_OR_NULL(dir)) {
		rc = (int)PTR_ERR(dir);
		pr_err("debugfs_create_dir failed with %d\n", rc);
		return rc;
	}

	bm_dir = dir;

	return 0;
}

void bm_dbg_module_clean(void)
{
	debugfs_remove_recursive(bm_dir); // NULL check occurs internally
}
