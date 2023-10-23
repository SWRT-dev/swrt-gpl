/*
 * pp_port_mgr_sysfs.c
 * Description: PP port manager sysfs interface
 *              Creates module's sysfs files
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2019 Intel Corporation
 */

#include <linux/kernel.h>
#include <linux/types.h>
#include "pp_common.h"
#include "pp_sysfs_common.h"
#include "port_dist.h"
#include "pp_port_mgr.h"
#include "pp_port_mgr_internal.h"

/**
 * @brief Module's sysfs main directory
 */
static struct kobject *sysfs;

static ssize_t __ports_stats_show(char *buf, size_t sz)
{
	size_t n = 0;

	pp_stats_show(sizeof(struct pp_stats), PP_MAX_PORT,
		      pmgr_ports_stats_get, pmgr_ports_stats_show, NULL, buf,
		      sz, &n);
	return n;
}

static ssize_t __ports_pps_show(char *buf, size_t sz)
{
	size_t n = 0;

	pp_pps_show(sizeof(struct pp_stats), PP_MAX_PORT, pmgr_ports_stats_get,
		    pmgr_ports_stats_diff, pmgr_ports_stats_show, NULL, buf,
		    sz, &n);
	return n;
}

static ssize_t __ports_stats_reset(const char *buf, size_t sz)
{
	if (strncmp(buf, "0", strlen("0")))
		return -EINVAL;

	port_dist_reset_stat();

	return sz;
}

PP_DEFINE_SYSFS_ATTR(ports_stats, __ports_stats_show, __ports_stats_reset);
PP_DEFINE_SYSFS_ATTR(ports_pps, __ports_pps_show, __ports_stats_reset);

static struct attribute *attrs[] = {
	&PP_SYSFS_ATTR(ports_stats).attr,
	&PP_SYSFS_ATTR(ports_pps).attr,
	NULL
};

static struct attribute_group attrs_group = {
	.attrs = attrs,
};

s32 pmgr_sysfs_init(struct kobject *parent)
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

void pmgr_sysfs_clean(void)
{
	if (unlikely(!sysfs))
		return;

	if (attrs_group.attrs)
		sysfs_remove_group(sysfs, &attrs_group);
	sysfs = NULL;
	attrs_group.attrs = NULL;
}
