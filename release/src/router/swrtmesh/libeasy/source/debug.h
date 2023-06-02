/*
 * debug.h - defines macros for debugging.
 *
 * Copyright (C) 2020 iopsys Software Solutions AB. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */
#ifndef EASYDEBUG_H
#define EASYDEBUG_H

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

#define WARN_ON(cond) \
({ \
	typeof(cond) __c = (cond); \
	if (__c) \
		pr_warn("%sL%d@%s: [%s] failed\n", __FILE__, __LINE__, __func__, #cond); \
	__c; \
})

#ifdef LIBEASY_TRACE_TIME
#define ENTER() clock_t _clk_end, _clk_begin; \
		_clk_begin = clock(); \
		pr_error("%s called\n", __func__)

#define EXIT(__res__) _clk_end = clock(); \
			pr_error("%s ret %d consume %u us\n\n", __func__, __res__, \
				 (unsigned int) _clk_end - _clk_begin)

#define EXITV() _clk_end = clock(); \
		pr_error("%s exit consume %u us\n\n", __func__, \
			 (unsigned int) _clk_end - _clk_begin)
#else
#define ENTER()
#define EXIT(__res__)
#define EXITV()
#endif


#ifdef __cplusplus
}
#endif
#endif /* EASYDEBUG_H */
