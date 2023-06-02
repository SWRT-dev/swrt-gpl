/*
 * debug.c - for debug prints to stderr
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
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "easy.h"

void LIBEASY_API log_stderr(int level, const char *fmt, ...)
{
	va_list args;
	int iop_debug;
	char *e = getenv("LIBWIFI_DEBUG_LEVEL");

	iop_debug = e ? atoi(e) : -1;
	if (iop_debug < level)
		return;

	va_start(args, fmt);
	vfprintf(stderr, fmt, args);	/* Flawfinder: ignore */
	va_end(args);
}
