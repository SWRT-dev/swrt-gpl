/*
 * pp_sysfs_common.h
 * Description: PP sysfs common definitions
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018 Intel Corporation
 */

#ifndef __PP_SYSFS_COMMON_H_
#define __PP_SYSFS_COMMON_H_

#include <linux/kernel.h>       /* for scnprintf */
#include <linux/errno.h>
#include <linux/capability.h>
#include "pp_common.h"

#ifdef pr_fmt
#undef pr_fmt
#define pr_fmt(fmt) "[PP_SYSFS_COMMON]:%s:%d: " fmt, __func__, __LINE__
#endif

/**
 * @define Auto generated 'fops' structure name
 */
#define PP_SYSFS_ATTR(name)      (__sysfs_##name##_attr)

/**
 * @brief Define sysfs attribute for show and store function
 * @param name base name for internal usage
 * @param read_fn signature:
 *        void READ_FN(struct seq_file *f)
 *        '@param f' seq_file ptr
 * @param write_fn signature:
 *        void WRITE_FN(char *cmd_buf, void *data)
 *        '@param cmd_buf' buffer contains the command from user
 *        '@param data' private data
 */
#define PP_DEFINE_SYSFS_ATTR(name, show, store)                                \
	static ssize_t __sysfs_##name##_rd(                                    \
		struct device *dev, struct device_attribute *attr, char *buf)  \
	{                                                                      \
		ssize_t (*__show)(char *buf, size_t sz);                       \
									       \
		if (!capable(CAP_SYS_PACCT))                                   \
			return -EPERM;                                         \
									       \
		if (unlikely(ptr_is_null(dev) || ptr_is_null(attr) ||          \
			     ptr_is_null(buf)))                                \
			return -EPERM;                                         \
									       \
		__show = show;                                                 \
		return __show ? __show(buf, PAGE_SIZE) : -EPERM;               \
	}                                                                      \
	static ssize_t __sysfs_##name##_wr(struct device *dev,                 \
					   struct device_attribute *attr,      \
					   const char *buf, size_t sz)         \
	{                                                                      \
		ssize_t (*__store)(const char *buf, size_t sz);                \
									       \
		if (!capable(CAP_SYS_PACCT))                                   \
			return -EPERM;                                         \
		if (unlikely(ptr_is_null(dev) || ptr_is_null(attr) ||          \
			     ptr_is_null(buf)))                                \
			return -EPERM;                                         \
									       \
		__store = store;                                               \
		return __store ? __store(buf, sz) : -EPERM;                    \
	}                                                                      \
	static struct device_attribute PP_SYSFS_ATTR(name) =                   \
		__ATTR(name, 0644, __sysfs_##name##_rd,                        \
		       __sysfs_##name##_wr)

#endif /* __PP_SYSFS_COMMON_H_ */
