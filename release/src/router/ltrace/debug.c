/*
 * This file is part of ltrace.
 * Copyright (C) 2012 Petr Machata, Red Hat Inc.
 * Copyright (C) 2003,2008,2009 Juan Cespedes
 * Copyright (C) 2006 Ian Wienand
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
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
#include <stdarg.h>

#include "common.h"
#include "backend.h"

void
debug_(int level, const char *file, int line, const char *fmt, ...) {
	char buf[1024];
	va_list args;

	if (!(options.debug & level)) {
		return;
	}
	va_start(args, fmt);
	vsnprintf(buf, 1024, fmt, args);
	va_end(args);

	output_line(NULL, "DEBUG: %s:%d: %s", file, line, buf);
	fflush(options.output);
}

static int
xwritedump(long *ptr, arch_addr_t addr, size_t count)
{
	size_t i;
	for (i = 0; i < count; ++i) {
		if (fprintf(stderr, "%p->%0*lx\n",
			    addr, 2 * (int)sizeof(long), ptr[i]) < 0)
			return -1;
		addr += sizeof(long);
	}

	return 0;
}

int
xinfdump(struct process *proc, arch_addr_t addr, size_t length)
{
	unsigned char buf[length];
	size_t got = umovebytes(proc, addr, buf, length);
	if (got == (size_t)-1)
		return -1;
	return xwritedump((long *)buf, addr, got / sizeof(long));
}
