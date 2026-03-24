/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef __WAVE_LOGS__H__
#define __WAVE_LOGS__H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#ifdef CONFIG_ALLOW_SYSLOG
#include <syslog.h>
#endif /* CONFIG_ALLOW_SYSLOG */

#define __RED "\033[0;31m"
#define __BLUE "\033[0;34m"

extern int __log_level;
extern int __use_syslog;

int wv_open_syslog(const char *name);
int wv_close_syslog(void);

typedef enum {
	WV_LOG_INFO,
	WV_LOG_ERR,
	WV_LOG_BUG,
} wv_log_type;

void wv_log_msg(int lvl, wv_log_type type, const char *clr,
		const char *file, int line, const char *fmt, ...)
		__attribute__ ((format(printf,6,7)));

#define LOG(__lvl, fmt, ...) \
wv_log_msg(__lvl, WV_LOG_INFO, NULL, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

#define ELOG(fmt, ...) \
wv_log_msg(1, WV_LOG_ERR, __RED, __FILE__, __LINE__, "ERROR: " fmt, ##__VA_ARGS__)

#define BUG(fmt, ...) \
wv_log_msg(0, WV_LOG_BUG, __RED, __FILE__, __LINE__, "BUG: " fmt, ##__VA_ARGS__)

#define DLOG(fmt, ...) \
wv_log_msg(2, WV_LOG_INFO, __BLUE, __FILE__, __LINE__, "DEBUG: " fmt, ##__VA_ARGS__)

#define WV_TIMER_CALC_TIMESPEC_DIFF(__srt, __stp, __res) {			\
    if ((__stp.tv_nsec - __srt.tv_nsec) < 0) {					\
        __res.tv_sec = __stp.tv_sec - __srt.tv_sec - 1;				\
        __res.tv_nsec = __stp.tv_nsec - __srt.tv_nsec + 1000000000L;		\
    } else {									\
        __res.tv_sec = __stp.tv_sec - __srt.tv_sec;				\
        __res.tv_nsec = __stp.tv_nsec - __srt.tv_nsec;				\
    }										\
}

#define WV_TIMER_START \
{ \
	struct timespec __start_tm, __end_tm, __diff_tm; \
	clock_gettime(7, &__start_tm);

#define WV_TIMER_ACTION_TOOK_LONGER_THAN(secs, msecs, fmt, ...) \
	clock_gettime(7, &__end_tm); \
	WV_TIMER_CALC_TIMESPEC_DIFF(__start_tm, __end_tm, __diff_tm); \
	if (__diff_tm.tv_sec > secs || \
	    (__diff_tm.tv_sec == secs && __diff_tm.tv_nsec > (1000000 * msecs))) { \
		ELOG("timer stopped after %ld.%09ld seconds - " fmt, \
		     __diff_tm.tv_sec, __diff_tm.tv_nsec, ##__VA_ARGS__); \
	} \
}

#ifdef __cplusplus
}
#endif

#endif /* __WAVE_LOGS__H__ */
