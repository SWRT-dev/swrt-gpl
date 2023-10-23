/*
 * pp_logger_internal.h
 * Description: Packet Processor Logger Internal Definitions
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018 Intel Corporation
 */

#ifndef __PP_LOGGER_INTERNAL_H__
#define __PP_LOGGER_INTERNAL_H__
#include <linux/spinlock.h>
#include <linux/debugfs.h>
#include <linux/bitmap.h>
#include "pp_logger.h"

/**
 * @brief Log prints
 */
#define pp_logger_err(fmt, ...) \
	pp_log_err(LOGGER_COMP_LOGGER, fmt, ##__VA_ARGS__)
#define pp_logger_warn(fmt, ...) \
	pp_log_warn(LOGGER_COMP_LOGGER, fmt, ##__VA_ARGS__)
#define pp_logger_info(fmt, ...) \
	pp_log_info(LOGGER_COMP_LOGGER, fmt, ##__VA_ARGS__)
#define pp_logger_dbg(fmt, ...) \
	pp_log_dbg(LOGGER_COMP_LOGGER, fmt, ##__VA_ARGS__)

/**
 * @brief Shortcut for iterating through logger components
 * @param c variable to use as loop iterator
 */
#define LOGGER_FOR_ALL_COMPS(c) \
	for (c = LOGGER_COMP_FIRST; c < LOGGER_COMP_NUM; c++)
/**
 * @brief Shortcut for iterating through logger log levels
 * @param l variable to use as loop iterator
 */
#define LOGGER_FOR_ALL_LEVELS(l) \
	for (l = LOGGER_LEVEL_FIRST; l < LOGGER_LEVEL_NUM; l++)

/**
 * @brief Shortcut to check if logger component is valid
 * @param c component to check
 */
#define LOGGER_IS_COMP_VALID(c) \
	(c >= LOGGER_COMP_FIRST && c < LOGGER_COMP_NUM)
/**
 * @brief Shortcut to check if logger level is valid
 * @param l level to check
 */
#define LOGGER_IS_LEVEL_VALID(l) \
	(l >= LOGGER_LEVEL_FIRST && l < LOGGER_LEVEL_NUM)

/* MUST be aligned with enum pp_logger_level */
extern const char *logger_level_str[LOGGER_LEVEL_NUM];
/* MUST be aligned with enum pp_logger_comp */
extern const char *logger_comp_str[LOGGER_COMP_NUM];

/**
 * @brief Convert pp logger level number into a string
 * @param l log level
 * @return const char* level in string format
 */
static inline const char *__logger_level_str(enum log_level l)
{
	if (likely(LOGGER_IS_LEVEL_VALID(l)))
		return logger_level_str[l];

	return "Invalid";
}

/**
 * @brief Convert pp logger component number into a string
 * @param c component enum pp_logger_comp
 * @return const char* component in string format
 */
static inline const char *__logger_comp_str(enum logger_comp c)
{
	if (likely(LOGGER_IS_COMP_VALID(c)))
		return logger_comp_str[c];

	return "Invalid";
}

/**
 * @brief Check if PP logger component is valid, print error
 *        message in case not
 * @note Use 'PP_LOGGER_IS_COMP_VALID' macro to avoid error
 *       print
 * @param c component to check
 * @return s32 1 in case component is valid, 0 otherwise
 */
static inline s32 __logger_is_comp_valid(enum logger_comp c)
{
	if (LOGGER_IS_COMP_VALID(c))
		return 1;

	pp_logger_err("Invalid pp logger component %u\n", c);
	return 0;
}

/**
 * @brief Check if PP logger level is valid, print error
 *        message in case not
 * @param l level to check
 * @return s32 1 in case level is valid, 0 otherwise
 */
static inline s32 __logger_is_level_valid(enum log_level l)
{
	if (LOGGER_IS_LEVEL_VALID(l))
		return 1;

	pp_logger_err("Invalid pp logger level %u\n", l);
	return 0;
}

#ifdef CONFIG_DEBUG_FS
/**
 * @brief Logger debug init
 * @param parent debugfs parent dir to create all files in
 * @return int 0 on success, non-zero value otherwise
 */
s32 logger_dbg_init(struct dentry *parent);

/**
 * @brief Logger debug clean
 * @param db logger database pointer
 * @return int 0 on success, non-zero value otherwise
 */
s32 logger_dbg_clean(void);
#else
static inline s32 logger_dbg_init(struct dentry *parent)
{
	return 0;
}

static inline s32 logger_dbg_clean(void)
{
	return 0;
}

#endif

#endif /* __PP_LOGGER_INTERNAL_H__ */
