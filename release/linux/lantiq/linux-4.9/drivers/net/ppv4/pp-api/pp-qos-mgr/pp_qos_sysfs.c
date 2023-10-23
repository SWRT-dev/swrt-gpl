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
#include "pp_qos_utils.h"

static ssize_t __queues_stats_show(char *buf, size_t sz)
{
	struct pp_qos_dev *qdev;
	size_t n = 0;

	qdev = pp_qos_dev_open(PP_QOS_INSTANCE_ID);
	if (unlikely(ptr_is_null(qdev)))
		return -EINVAL;

	pp_stats_show(sizeof(struct queue_stats), NUM_OF_NODES,
		      qos_queues_stats_get, qos_queues_stats_show, qdev, buf,
		      sz, &n);
	return n;
}

static ssize_t __queues_pps_show(char *buf, size_t sz)
{
	struct pp_qos_dev *qdev;
	size_t n = 0;

	qdev = pp_qos_dev_open(PP_QOS_INSTANCE_ID);
	if (unlikely(ptr_is_null(qdev)))
		return -EINVAL;

	pp_pps_show(sizeof(struct queue_stats), NUM_OF_NODES,
		    qos_queues_stats_get, qos_queues_stats_diff,
		    qos_queues_stats_show, qdev, buf, sz, &n);
	return n;
}

static ssize_t __queues_stats_reset(const char *buf, size_t sz)
{
	struct pp_qos_dev *qdev;

	if (strncmp(buf, "0", strlen("0")))
		return -EINVAL;

	qdev = pp_qos_dev_open(PP_QOS_INSTANCE_ID);
	if (unlikely(ptr_is_null(qdev)))
		return -EINVAL;

	pp_qos_stats_reset(qdev);

	return sz;
}

static ssize_t __tree_show(char *buf, size_t sz)
{
	struct pp_qos_dev *qdev;
	struct seq_file s = { 0 };

	qdev = pp_qos_dev_open(PP_QOS_INSTANCE_ID);
	if (unlikely(ptr_is_null(qdev)))
		return -EINVAL;

	s.buf = buf;
	s.size = sz;
	qos_dbg_tree_show_locked(qdev, true, &s);

	return s.count;
}

PP_DEFINE_SYSFS_ATTR(qos_queues_stats, __queues_stats_show,
		     __queues_stats_reset);
PP_DEFINE_SYSFS_ATTR(qos_queues_pps, __queues_pps_show, __queues_stats_reset);
PP_DEFINE_SYSFS_ATTR(qos_tree, __tree_show, NULL);

static struct attribute *attrs[] = {
	&PP_SYSFS_ATTR(qos_queues_stats).attr,
	&PP_SYSFS_ATTR(qos_queues_pps).attr,
	&PP_SYSFS_ATTR(qos_tree).attr,
	NULL
};

static struct attribute_group attrs_group = {
	.attrs = attrs,
};

s32 qos_sysfs_init(struct pp_qos_dev *qdev)
{
	s32 ret;

	/* save the parent dir so we can delete the files later */
	qdev->sysfs = qdev->init_params.sysfs;
	attrs_group.attrs = attrs;
	ret = sysfs_create_group(qdev->sysfs, &attrs_group);
	if (unlikely(ret)) {
		pr_err("Failed to create sysfs files, ret = %d\n", ret);
		attrs_group.attrs = NULL;
	}

	return ret;
}

void qos_sysfs_clean(struct pp_qos_dev *qdev)
{
	if (unlikely(!qdev->sysfs))
		return;

	if (attrs_group.attrs)
		sysfs_remove_group(qdev->sysfs, &attrs_group);
	qdev->sysfs = NULL;
	attrs_group.attrs = NULL;
}
