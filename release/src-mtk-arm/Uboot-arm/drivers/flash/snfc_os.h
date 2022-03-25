/*
 * Copyright (c) 2012  MediaTek, Inc.
 * Author : Guochun.Mao@mediatek.com
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

/**
 * os related api and variable, include:
 *	1, data type define
 *	2, register read/write api
 *	3, time delay api
 *	4, log print api
 *	5, error type
 *	6, other api linux has supplied
 */

#ifndef __SNFC_OS_H__
#define __SNFC_OS_H__

#include <asm/io.h>
#include <common.h>
#include <asm/types.h>
#include <linux/err.h>

#define snfc_loge(fmt, arg...)	error(fmt, ##arg)

#define SNFC_DEBUG
#ifdef SNFC_DEBUG
#define snfc_logd(fmt, arg...)	debug(fmt, ##arg)
#define snfc_logi(fmt, arg...)	debug(fmt, ##arg)
#else
#define snfc_logd(fmt, arg...)	do {} while (0)
#define snfc_logi(fmt, arg...)	do {} while (0)
#endif

#define ERR_NOMEM	(-ENOMEM)
#define ERR_BUSY	(-EBUSY)
#define ERR_INVAL	(-EINVAL)
#define ERR_FAULT	(-EFAULT)

#define WRITEB(val, reg)	writeb(val, reg)
#define READB(reg)		readb(reg)
#define WRITEL(val, reg)	writel(val, reg)
#define READL(reg)		readl(reg)

#define os_invalidate_cache_range(addr, len) \
	invalidate_dcache_range((unsigned long)addr, (unsigned long)addr + len)

#ifndef BIT
#define BIT(n) (1UL << (n))
#endif

#endif /* __SNFC_OS_H__ */
