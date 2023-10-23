/*
 * pp_port_mgr_sysfs.c
 * Description: PP misc module sysfs interface
 *              Creates module's sysfs files
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2019-2020 Intel Corporation
 */

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/pp_qos_api.h>
#include "pp_common.h"
#include "pp_sysfs_common.h"
#include "pp_misc.h"
#include "pp_misc_internal.h"
#include "pp_session_mgr.h"

/**
 * @brief Module's sysfs main directory
 */
static struct kobject *sysfs;

static ssize_t __global_stats_show(char *buf, size_t sz)
{
	struct pp_qos_dev *qdev;
	size_t n = 0;

	qdev = pp_qos_dev_open(PP_QOS_INSTANCE_ID);
	if (unlikely(ptr_is_null(qdev)))
		return -EINVAL;

	pp_stats_show(sizeof(struct pp_global_stats), 1,
		      pp_global_stats_get, pp_global_stats_show, qdev, buf,
		      sz, &n);
	return n;
}

static ssize_t __global_pps_show(char *buf, size_t sz)
{
	struct pp_qos_dev *qdev;
	size_t n = 0;

	qdev = pp_qos_dev_open(PP_QOS_INSTANCE_ID);
	if (unlikely(ptr_is_null(qdev)))
		return -EINVAL;

	pp_pps_show(sizeof(struct pp_global_stats), 1, pp_global_stats_get,
		    pp_global_stats_diff, pp_global_stats_show, qdev, buf, sz,
		    &n);
	return n;
}

static ssize_t __brief_stats_max_cnt_set(const char *buf, size_t sz)
{
	u32 cnt;

	if (kstrtou32(buf, 10, &cnt))
		pr_info("failed to parse '%s'\n", buf);

	pr_info("Brief max lines: %u\n", cnt);
	pp_misc_set_brief_cnt(cnt);
	return sz;
}

static ssize_t __global_stats_reset(const char *buf, size_t sz)
{
	if (strncmp(buf, "0", strlen("0")))
		return -EINVAL;

	pp_global_stats_reset();
	return sz;
}

static ssize_t __brief_show(char *buf, size_t sz)
{
	struct smgr_stats stats;
	size_t n = 0;

	if (pp_smgr_stats_get(&stats))
		return -EINVAL;

	pp_pps_show(sizeof(struct pp_active_sess_stats),
		atomic_read(&stats.sess_open), pp_brief_stats_get,
		pp_brief_stats_diff, pp_brief_stats_show, NULL, buf, sz, &n);

	return n;
}

static ssize_t __driver_stats_show(char *buf, size_t sz)
{
	struct pp_qos_dev *qdev;
	size_t n = 0;

	qdev = pp_qos_dev_open(PP_QOS_INSTANCE_ID);
	if (unlikely(ptr_is_null(qdev)))
		return -EINVAL;

	pp_stats_show(sizeof(struct pp_driver_stats), 1,
		      pp_driver_stats_get, pp_driver_stats_show, qdev, buf,
		      sz, &n);
	return n;
}

static ssize_t __driver_stats_reset(const char *buf, size_t sz)
{
	if (strncmp(buf, "0", strlen("0")))
		return -EINVAL;

	pp_driver_stats_reset();
	return sz;
}

static ssize_t __version_show(char *buf, size_t sz)
{
	struct pp_version drv, fw, hw;
	size_t n = 0;
	s32 ret = 0;

	ret |= pp_version_get(&drv, PP_VER_TYPE_DRV);
	ret |= pp_version_get(&fw, PP_VER_TYPE_FW);
	ret |= pp_version_get(&hw, PP_VER_TYPE_HW);

	if (unlikely(ret)) {
		pr_err("Failed to get PP driver version\n");
		return ret;
	}

	ret = pp_version_show(buf, sz, &n, &drv, &fw, &hw);
	if (unlikely(ret)) {
		pr_err("Failed to show PP driver version\n");
		return ret;
	}

	return n;
}

static ssize_t __resources_show(char *buf, size_t sz)
{
	size_t n = 0;
	s32 ret;

	ret = pp_resource_stats_show(buf, sz, &n);
	if (unlikely(ret)) {
		pr_err("Failed to show PP resources status\n");
		return ret;
	}

	return n;
}

PP_DEFINE_SYSFS_ATTR(brief, __brief_show, __brief_stats_max_cnt_set);
PP_DEFINE_SYSFS_ATTR(global, __global_stats_show, __global_stats_reset);
PP_DEFINE_SYSFS_ATTR(pps, __global_pps_show, __global_stats_reset);
PP_DEFINE_SYSFS_ATTR(driver_stats, __driver_stats_show, __driver_stats_reset);
PP_DEFINE_SYSFS_ATTR(version, __version_show, NULL);
PP_DEFINE_SYSFS_ATTR(resources, __resources_show, NULL);

static struct attribute *attrs[] = {
	&PP_SYSFS_ATTR(brief).attr,
	&PP_SYSFS_ATTR(global).attr,
	&PP_SYSFS_ATTR(pps).attr,
	&PP_SYSFS_ATTR(driver_stats).attr,
	&PP_SYSFS_ATTR(version).attr,
	&PP_SYSFS_ATTR(resources).attr,
	NULL
};

static struct attribute_group attrs_group = {
	.attrs = attrs,
};

s32 pp_misc_sysfs_init(struct kobject *parent)
{
	s32 ret;

	/* save the parent dir so we can delete the files later */
	sysfs = parent;
	attrs_group.attrs = attrs;
	ret = sysfs_create_group(sysfs, &attrs_group);
	if (unlikely(ret)) {
		pr_err("Failed to create sysfs files, ret = %d\n", ret);
		attrs_group.attrs = NULL;
	}

	return ret;
}

void pp_misc_sysfs_clean(void)
{
	if (unlikely(!sysfs))
		return;

	if (attrs_group.attrs)
		sysfs_remove_group(sysfs, &attrs_group);
	sysfs = NULL;
	attrs_group.attrs = NULL;
}
