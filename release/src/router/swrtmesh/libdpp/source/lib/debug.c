/*
 * debug.c - debug functions
 *
 * Copyright (C) 2023 IOPSYS Software Solutions AB. All rights reserved.
 *
 * See LICENSE file for license related information.
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <easy/easy.h>

void log_stderr(int level, const char *fmt, ...)
{
	va_list args;
	int dbgl;
	char *e = getenv("LIBDPP_DEBUG_LEVEL");

	dbgl = e ? atoi(e) : -1;
	if (dbgl < level)
		return;

	va_start(args, fmt);
	vfprintf(stderr, fmt, args);	/* Flawfinder: ignore */
	va_end(args);
}
