/*
 * Description: PP Checker sysfs interface
 *              Creates module's sysfs files
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2019 Intel Corporation
 */

#define pr_fmt(fmt) "[CHK_SYSFS]:%s:%d: " fmt, __func__, __LINE__

#include <linux/kernel.h>
#include <linux/types.h>
#include "pp_common.h"
#include "pp_sysfs_common.h"
#include "checker.h"
#include "checker_internal.h"

/**
 * @brief Module's sysfs main directory
 */
static struct kobject *sysfs;

static ssize_t __exceptions_stats_show(char *buf, size_t sz)
{
	size_t n = 0;

	pp_stats_show(sizeof(struct pp_stats), CHK_NUM_EXCEPTION_SESSIONS,
		      chk_exceptions_stats_get, chk_exceptions_stats_show, NULL,
		      buf, sz, &n);
	return n;
}

static ssize_t __exceptions_pps_show(char *buf, size_t sz)
{
	size_t n = 0;

	pp_pps_show(sizeof(struct pp_stats), CHK_NUM_EXCEPTION_SESSIONS,
		    chk_exceptions_stats_get, chk_exceptions_stats_diff,
		    chk_exceptions_stats_show, NULL, buf, sz, &n);
	return n;
}

PP_DEFINE_SYSFS_ATTR(exceptions_stats, __exceptions_stats_show, NULL);
PP_DEFINE_SYSFS_ATTR(exceptions_pps, __exceptions_pps_show, NULL);

static struct attribute *attrs[] = {
	&PP_SYSFS_ATTR(exceptions_stats).attr,
	&PP_SYSFS_ATTR(exceptions_pps).attr,
	NULL
};

static struct attribute_group attrs_group = {
	.attrs = attrs,
};

s32 chk_sysfs_init(struct pp_chk_init_param *init_param)
{
	s32 ret;

	/* save the parent dir so we can delete the files later */
	sysfs = init_param->sysfs;
	attrs_group.attrs = attrs;
	ret = sysfs_create_group(sysfs, &attrs_group);
	if (unlikely(ret)) {
		pr_err("Failed to create sysfs files, ret = %d\n", ret);
		attrs_group.attrs = NULL;
	}

	return ret;
}

void chk_sysfs_clean(void)
{
	if (unlikely(!sysfs))
		return;

	if (attrs_group.attrs)
		sysfs_remove_group(sysfs, &attrs_group);
	sysfs = NULL;
	attrs_group.attrs = NULL;
}
