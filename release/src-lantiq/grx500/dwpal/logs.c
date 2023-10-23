/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#include "dwpal_logs.h"

#include <stdarg.h>
#include <time.h>
#ifdef CONFIG_ALLOW_SYSLOG
#include <syslog.h>
#endif /* CONFIG_ALLOW_SYSLOG */

#define __RST "\033[0m"

int __gen_log_level = 0;
int __gen_use_syslog = 0;

#ifdef CONFIG_ALLOW_SYSLOG
static int __opened_syslog = 0;

static int gen_syslog_priority(gen_log_type type)
{
	switch (type) {
	case GEN_LOG_INFO:
		return LOG_INFO;
	case GEN_LOG_ERR:
		return LOG_ERR;
	case GEN_LOG_BUG:
		return LOG_CRIT;
	}
	return LOG_INFO;
}

int gen_open_syslog(const char *name)
{
	if (__opened_syslog)
		return 1;

	openlog(name, LOG_PID | LOG_NDELAY, LOG_DAEMON);
	__opened_syslog = 1;
	return 0;
}

int gen_close_syslog(void)
{
	if (!__opened_syslog)
		return 1;

	closelog();
	__opened_syslog = 0;
	return 0;
}
#else /* CONFIG_ALLOW_SYSLOG */
int gen_open_syslog(const char *name)
{
	(void)name;
	return 1;
}

int gen_close_syslog(void)
{
	return 1;
}
#endif /* CONFIG_ALLOW_SYSLOG */


#ifdef CONFIG_NO_LOGS
void gen_log_msg(int lvl, gen_log_type type, const char *clr,
		const char *file, int line, const char *fmt, ...) {}
#else /* CONFIG_NO_LOGS */
void gen_log_msg(int lvl, gen_log_type type, const char *clr,
		const char *file, int line, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	if (__gen_log_level >= lvl) {
#ifdef CONFIG_ALLOW_SYSLOG
		if (__gen_use_syslog)
			vsyslog(gen_syslog_priority(type), fmt, ap);
		else
#endif /* CONFIG_ALLOW_SYSLOG */
		{
			struct timespec ts;
			clock_gettime(7 ,&ts);

			(void)type;
			printf("%ld.%06u [%s:%d]: ",
			       ts.tv_sec, (unsigned int)(ts.tv_nsec / 1000),
			       file, line);
			if (clr)
				printf("%s", clr);
			vprintf(fmt, ap);
			if (clr)
				printf(__RST);
			printf("\n");
		}
	}
	va_end(ap);
}
#endif /* CONFIG_NO_LOGS */
