/*
 * pp_logger.h
 * Description: Packet Processor logger
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018 Intel Corporation
 */

#ifndef __PP_LOGGER_H__
#define __PP_LOGGER_H__

/**
 * @brief PP logger log levels
 */
enum log_level {
	LOGGER_LEVEL_FIRST,
	LOGGER_ERROR = LOGGER_LEVEL_FIRST,
	LOGGER_WARN,
	LOGGER_INFO,
	LOGGER_DEBUG,
	LOGGER_ALL,
	LOGGER_LEVEL_NUM
};

/**
 * @brief PP logger components
 */
enum logger_comp {
	LOGGER_COMP_FIRST,
	LOGGER_COMP_LOGGER = LOGGER_COMP_FIRST,
	LOGGER_COMP_INFRA,
	LOGGER_COMP_NUM
};

/**
 * Print error message
 * @param comp PP component
 * @note Function name and line are added
 * @note used by PP drivers for logging messages to the console
 */
#define pp_log_err(comp, fmt, ...)  \
	logger_print(comp, LOGGER_ERROR, __func__, __LINE__, \
	fmt, ##__VA_ARGS__)

/**
 * Print warning message
 * @param comp PP component
 * @note Function name and line are added
 * @note used by PP drivers for logging messages to the console
 */
#define pp_log_warn(comp, fmt, ...) \
	logger_print(comp, LOGGER_WARN, __func__, __LINE__, \
	fmt, ##__VA_ARGS__)

/**
 * Print info message
 * @param comp PP component
 * @note Function name and line are added
 * @note used by PP drivers for logging messages to the console
 */
#define pp_log_info(comp, fmt, ...) \
	logger_print(comp, LOGGER_INFO, __func__, __LINE__, \
	fmt, ##__VA_ARGS__)

/**
 * Print debug message
 * @param comp PP component
 * @note Function name and line are added
 * @note used by PP drivers for logging messages to the console
 */
#define pp_log_dbg(comp, fmt, ...)  \
	logger_print(comp, LOGGER_DEBUG, __func__, __LINE__, \
		fmt, ##__VA_ARGS__)

/**
 * Module init function <br>
 * 1. Create and initialized the database <br>
 * 2. Create logger sysfs folder <br>
 * @param level log level to set
 * @param dbgfs debugfs parent folder
 * @return int 0 on success, non-zero value otherwise
 */
int logger_init(int level, struct dentry *dbgfs);

/**
 * Module exit function, clean all resources
 * @return void
 */
void logger_exit(void);

/**
 * Print log message to the console using regular printk
 * infrastructure. <br>
 * All prints will be printed with a prefix: <br>
 * [COMP][Level]: [func](line)
 * @note It safe to use this API always, in case logger isn't
 *       active, all messages will be printed without apply
 *       logger rules.
 * @param c logger component
 * @param l log message level
 * @param f function name
 * @param ln line number
 * @param fmt format string
 *
 * @return s32 Number of characters printed
 */
s32 logger_print(enum logger_comp c, enum log_level l,
		 const char *f, u32 ln, const char *fmt, ...);

/**
 * Enable/Disable logger components.
 * @param c component
 * @param en enable/disable (1/0)
 *
 * @return s32 0 on success, non-zero value in case of logger
 *         isn't ready or component isn't valid.
 */
s32 logger_comp_set(enum logger_comp c, u32 en);

/**
 * Get logger components status
 * @param c component
 * @param en pointer to store status
 * @return s32 0 for success, non-zero otherwise
 */
s32 logger_comp_get(enum logger_comp c, u32 *en);

/**
 * Set logger log messages maximum level. <br>
 * @note ONLY messages LOWER than the configured log level will
 *       be printed, same as printk log levels :)
 * @param l level to configure
 * @return s32 0 on success, non-zero value in case the logger
 *         isn't ready or level isn't valid
 */
s32 logger_level_set(enum log_level l);

/**
 * Get logger current log messages level. <br>
 * @param l pointer to store the level
 * @return s32 0 on success, non-zero value otherwise
 */
s32 logger_level_get(enum log_level *l);

#endif /* __PP_LOGGER_H__ */
