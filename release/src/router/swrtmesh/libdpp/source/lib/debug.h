/*
 * debug.h - debug utility functions
 *
 * Copyright (C) 2023 IOPSYS Software Solutions AB. All rights reserved.
 *
 * See LICENSE file for license related information.
 */
#ifndef LIBDPP_DEBUG_H
#define LIBDPP_DEBUG_H

#define ERR_LEVEL   (3)
#define WARN_LEVEL  (4)
#define INFO_LEVEL  (6)
#define DBG_LEVEL   (7)

#ifdef __cplusplus
extern "C" {
#endif

extern void log_stderr(int level, const char *fmt, ...);

#define pr_error(...)	log_stderr(ERR_LEVEL, __VA_ARGS__)
#define pr_warn(...)	log_stderr(WARN_LEVEL, __VA_ARGS__)
#define pr_info(...)	log_stderr(INFO_LEVEL, __VA_ARGS__)
#define pr_debug(...)	log_stderr(DBG_LEVEL, __VA_ARGS__)

#ifdef __cplusplus
}
#endif
#endif /* LIBDPP_DEBUG_H */
