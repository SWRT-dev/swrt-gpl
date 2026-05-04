/*
 * This file is part of ltrace.
 * Copyright (C) 2014 Cadence Design Systems Inc.
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

#include "config.h"

#include <sys/ptrace.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

#include "common.h"
#include "backend.h"
#include "sysdep.h"
#include "breakpoint.h"
#include "proc.h"
#include "library.h"

void
arch_enable_breakpoint(pid_t pid, struct breakpoint *sbp)
{
	static unsigned char break_insn[] = BREAKPOINT_VALUE;
	static unsigned char density_break_insn[] = DENSITY_BREAKPOINT_VALUE;
	unsigned char *bytes;
	long a;

	debug(DEBUG_PROCESS,
	      "arch_enable_breakpoint: pid=%d, addr=%p, symbol=%s",
	      pid, sbp->addr, breakpoint_name(sbp));

	errno = 0;
	a = ptrace(PTRACE_PEEKTEXT, pid, sbp->addr, 0);
	if (a == -1 && errno) {
		fprintf(stderr, "enable_breakpoint"
			" pid=%d, addr=%p, symbol=%s: %s\n",
			pid, sbp->addr, breakpoint_name(sbp),
			strerror(errno));
		return;
	}
	bytes = (unsigned char *)&a;
	memcpy(sbp->orig_value, bytes, BREAKPOINT_LENGTH);
	if (is_density(bytes)) {
		memcpy(bytes, density_break_insn, DENSITY_BREAKPOINT_LENGTH);
	} else {
		memcpy(bytes, break_insn, BREAKPOINT_LENGTH);
	}
	a = ptrace(PTRACE_POKETEXT, pid, sbp->addr, a);
	if (a == -1) {
		fprintf(stderr, "enable_breakpoint"
			" pid=%d, addr=%p, symbol=%s: %s\n",
			pid, sbp->addr, breakpoint_name(sbp),
			strerror(errno));
		return;
	}
}
