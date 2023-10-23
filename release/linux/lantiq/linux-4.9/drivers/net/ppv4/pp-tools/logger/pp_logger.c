/*
 * pp_logger.c
 * Description: Packet Processor logger
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018 Intel Corporation
 */

#include <linux/kobject.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/slab.h>         /* kzalloc */
#include <linux/spinlock.h>
#include <linux/interrupt.h>    /* for in_irq */
#include <linux/types.h>
#include <linux/errno.h>
#include "pp_common.h"
#include "pp_logger.h"
#include "pp_logger_internal.h"

/**
 * PP Logger database
 * @kobj kernel object for sysfs
 * @level current log level
 * @comps bitmaps represent which component is enable
 * @lock spinlock for db locking
 * @init_done specify if logger init was done
 */
struct logger_db {
	enum log_level level;
	spinlock_t     lock;
	u8             init_done;
	DECLARE_BITMAP(comps, LOGGER_COMP_NUM);
};

/* Database */
static struct logger_db *db;

/* MUST be aligned with enum pp_logger_level */
const char *logger_level_str[LOGGER_LEVEL_NUM] = {
	"ERROR",
	"WARN",
	"INFO",
	"DEBUG",
	"ALL"
};

/* MUST be aligned with enum pp_logger_comp */
const char *logger_comp_str[LOGGER_COMP_NUM] = {
	"PP_LOGGER",
	"PP_INFRA",
};

/**
 * @brief Check if logger driver is in active state
 * @return u32 1 if logger driver is in active state, 0 oterwise
 */
static inline s32 __logger_is_active_quick(void)
{
	if (db && db->init_done)
		return 1;

	return 0;
}

/**
 * @brief Check if logger driver is in active state, print error
 *        message in case not
 * @return u32 1 if logger driver is in active state, 0 oterwise
 */
static inline s32 __logger_is_active(void)
{
	if (__logger_is_active_quick())
		return 1;

	pp_logger_err("PP logger wasn't initialized\n");
	return 0;
}

/**
 * @brief Acquire database lock
 * @return s32 0 on successfull acquiring, non-zero value
 *         otherwise
 */
static inline s32 __logger_lock(void)
{
	if (unlikely(!__logger_is_active())) {
		pp_logger_err("Failed to lock DB\n");
		return -EPERM;
	}

	WARN_ON(in_irq() || irqs_disabled());
	spin_lock_bh(&db->lock);
	return 0;
}

/**
 * @brief Release database lock
 * @return s32 0 on successfull release, non-zero value
 *         otherwise
 */
static inline s32 __logger_unlock(void)
{
	if (unlikely(!__logger_is_active())) {
		pp_logger_err("Failed to unlock DB\n");
		return -EPERM;
	}

	spin_unlock_bh(&db->lock);
	return 0;
}

s32 logger_print(enum logger_comp c, enum log_level l,
	const char *f, u32 ln, const char *fmt, ...)
{
	char msg[512] = { 0 };
	va_list args;
	s32 ret;

	if (unlikely(!__logger_is_comp_valid(c)))
		return 0;

	if (unlikely(!__logger_is_level_valid(l)))
		return 0;

	if (unlikely(!__logger_is_active_quick()))
		goto fallback;

	__logger_lock();
	if ((l > LOGGER_ERROR && !test_bit(c, db->comps)) ||
		(db->level <= l)) {
		__logger_unlock();
		return 0;
	}
	__logger_unlock();

fallback:
	va_start(args, fmt);
	ret = snprintf(msg, sizeof(msg)-1, "[%s:%s]:%s(%u): ",
		__logger_comp_str(c), __logger_level_str(l), f, ln);
	vsnprintf(msg + ret, sizeof(msg)-1-ret, fmt, args);
	pr_debug("%s", msg);
	va_end(args);

	return ret;
}

s32 logger_comp_set(enum logger_comp c, u32 en)
{
	if (unlikely(!__logger_is_comp_valid(c)))
		return -EINVAL;

	if (unlikely(!__logger_is_active()))
		return -EPERM;

	__logger_lock();
	if (en)
		set_bit(c, db->comps);
	else
		clear_bit(c, db->comps);
	__logger_unlock();

	pp_logger_dbg("PP logger component %s(%u) was %s\n",
		__logger_comp_str(c), c, en ? "enabled" : "disabled");

	return 0;
}

s32 logger_comp_get(enum logger_comp c, u32 *en)
{
	if (!en)
		return -EINVAL;

	if (unlikely(!__logger_is_comp_valid(c)))
		return -EINVAL;

	if (unlikely(!__logger_is_active()))
		return -EPERM;

	__logger_lock();
	*en = test_bit(c, db->comps);
	__logger_unlock();

	pp_logger_dbg("PP logger component %s(%u) is %s\n",
		__logger_comp_str(c), c, en ? "enabled" : "disabled");

	return 0;
}

s32 logger_level_set(enum log_level l)
{
	if (unlikely(!__logger_is_level_valid(l)))
		return -EINVAL;

	if (unlikely(!__logger_is_active()))
		return -EPERM;

	__logger_lock();
	db->level = l;
	__logger_unlock();

	pp_logger_dbg("PP logger level was set to %s(%u)\n",
		__logger_level_str(l), l);

	return 0;
}
EXPORT_SYMBOL(logger_level_set);

s32 logger_level_get(enum log_level *l)
{
	if (!l)
		return -EINVAL;

	if (unlikely(!__logger_is_active()))
		return -EPERM;

	__logger_lock();
	*l = db->level;
	__logger_unlock();

	pp_logger_dbg("PP logger level is %s(%u)\n",
		__logger_level_str(*l), *l);

	return 0;
}

static s32 __logger_init_db(enum log_level level)
{
	s32 ret;

	db = kzalloc(sizeof(*db), GFP_KERNEL);
	if (unlikely(!db)) {
		pp_logger_err("Failed to allocate database memory of size %u\n",
			sizeof(*db));
		return -ENOMEM;
	}
	/* init the db so we can start printing */
	spin_lock_init(&db->lock);
	bitmap_fill(db->comps, LOGGER_COMP_NUM);
	db->level       = LOGGER_DEBUG;
	db->init_done   = 1;
	ret = logger_level_set(level);
	if (unlikely(ret)) {
		pp_logger_err("Failed to set log level to %u\n", level);
		return ret;
	}

	return 0;
}

void logger_exit(void)
{
	pp_logger_dbg("start\n");
	logger_dbg_clean();
	if (likely(db)) {
		/* clean database */
		kfree(db);
		db = NULL;
	}
	pp_logger_dbg("done\n");
}

int logger_init(int level, struct dentry *dbgfs)
{
	int ret;

	ret = __logger_init_db((enum log_level) level);
	if (unlikely(ret)) {
		pp_logger_err("Failed to init database, ret = %d\n", ret);
		return ret;
	}

	ret = logger_dbg_init(dbgfs);
	if (unlikely(ret)) {
		pp_logger_err("Failed to init debug stuff, ret = %d\n", ret);
		return ret;
	}

	pp_logger_info("pp logger init done\n");
	return 0;
}
