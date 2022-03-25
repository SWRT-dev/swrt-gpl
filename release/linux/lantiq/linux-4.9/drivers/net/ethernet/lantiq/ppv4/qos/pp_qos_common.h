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
#ifndef PP_QOS_COMMON_H
#define PP_QOS_COMMON_H

#ifdef __KERNEL__
#include <linux/bitops.h>
#include <linux/types.h>
#include <linux/list.h>
#include <linux/device.h>
#include <linux/string.h>
#include <linux/vmalloc.h>
#include <linux/spinlock.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/stringify.h>
#else
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "list.h"
#endif

/* Logs */
#ifndef __KERNEL__
#define BIT(x) ((uint32_t) (1U << (x)))
#define QOS_LOG_CRIT(format, arg...)	printf(format, ##arg)
#define QOS_LOG_ERR(format, arg...)	printf(format, ##arg)
#define QOS_LOG_INFO(format, arg...)	printf(format, ##arg)
#define QOS_LOG_DEBUG(format, arg...)	printf(format, ##arg)
#else
extern struct device *cur_dev;
#define QOS_LOG_CRIT(format, arg...) \
do { \
	if (cur_dev) \
		dev_crit(cur_dev, "%s:%d: " format, __func__, __LINE__, \
			##arg); \
	else \
		pr_crit("%s:%d: " format, __func__, __LINE__, \
			##arg); \
} while (0)
#define QOS_LOG_ERR(format, arg...) \
do { \
	if (cur_dev) \
		dev_err(cur_dev, "%s:%d: " format, __func__, __LINE__, \
			##arg); \
	else \
		pr_err("%s:%d: " format, __func__, __LINE__, ##arg); \
} while (0)
#define QOS_LOG_INFO(format, arg...) \
do { \
	if (cur_dev) \
		dev_info(cur_dev, "%s:%d: " format, __func__, __LINE__, \
			##arg); \
	else \
		pr_info("%s:%d: " format, __func__, __LINE__, ##arg); \
} while (0)
#define QOS_LOG_DEBUG(format, arg...) \
do { \
	if (cur_dev) \
		dev_dbg(cur_dev, "%s:%d: " format, __func__, __LINE__, \
			##arg); \
	else \
		pr_debug("%s:%d: " format, __func__, __LINE__, ##arg); \
} while (0)
#define QOS_LOG_API_DEBUG(format, arg...) \
do { \
	if (cur_dev) \
		dev_dbg(cur_dev, "[API]%s:%d: " format, __func__, __LINE__, \
			##arg); \
	else \
		pr_debug("[API]%s:%d: " format, __func__, __LINE__, ##arg); \
} while (0)
#endif

#ifndef __KERNEL__
#define max(a, b)		\
	({ typeof(a) _a = (a);	\
	typeof(b) _b = (b);	\
	_a > _b ? _a : _b; })

#define min(a, b)		\
	({ typeof(a) _a = (a);	\
	 typeof(b) _b = (b);	\
	 _a < _b ? _a : _b; })
#endif

/* Memory allocation */
#ifdef __KERNEL__
#define QOS_MALLOC(size)	vmalloc(size)
#define QOS_FREE(p)		vfree(p)
#else
#define QOS_MALLOC(size)	malloc(size)
#define QOS_FREE(p)		free(p)
#endif

/* Locking */
#ifdef __KERNEL__
#define LOCK spinlock_t
#define QOS_LOCK_INIT(qdev)	spin_lock_init(&qdev->lock)
#define QOS_LOCK(qdev)		spin_lock(&qdev->lock)
#define QOS_UNLOCK(qdev)	spin_unlock(&qdev->lock)
#define QOS_SPIN_IS_LOCKED(qdev) spin_is_locked(&qdev->lock)
#else
#define LOCK int
#define QOS_LOCK_INIT(qdev)	(qdev->lock = 0)
#define QOS_LOCK(qdev)		do {QOS_ASSERT(qdev->lock == 0,\
				"Lock already taken\n");\
				qdev->lock++; } while (0)
#define QOS_UNLOCK(qdev)	do {QOS_ASSERT(qdev->lock == 1,\
				"Lock not taken\n");\
				qdev->lock--; } while (0)
#define QOS_SPIN_IS_LOCKED(qdev) (qdev->lock)
#endif

/* MMIO */
#ifdef __KERNEL__
#else
#define __iomem
#define iowrite32(val, addr) (*((uint32_t *)(addr)) = (val))
#endif

/* TODO not clean
 * There might be other mapping on platform different
 * than the FPGA I am currently testing on
 */
#ifdef CONFIG_OF
#define TO_QOS_ADDR(x) (x)
#define FW_OK_OFFSET (0x25e10000U)
#else
#define FW_OK_OFFSET (0x6ffff8U)
#define PCI_DDR_BAR_START 0xd2000000
#define TO_QOS_ADDR(x) ((x) - PCI_DDR_BAR_START)
#endif

/* Sleep */
#ifdef __KERNEL__
#define qos_sleep(t) msleep((t))
#else
#define qos_sleep(t) usleep((t) * 1000)
#endif

/* Endianess */
#ifndef __KERNEL__
#define cpu_to_le32(x) (x)
#define le32_to_cpu(x) (x)
#endif

#ifdef QOS_CPU_UC_SAME_ENDIANESS
#define qos_u32_to_uc(val)	(val)
#define qos_u32_from_uc(val)  (val)
#else
#define qos_u32_to_uc(val)	(cpu_to_le32((val)))
#define qos_u32_from_uc(val)	(le32_to_cpu((val)))
#endif

/* Creating enums and enums strings */
#ifndef __KERNEL__
#define __stringify(x)		#x
#endif

#define GEN_ENUM(enumerator)    enumerator,
#define GEN_STR(str)		__stringify(str),

#endif /* PP_QOS_COMMON_H */
