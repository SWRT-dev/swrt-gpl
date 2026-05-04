/*
 * This file is part of ltrace.
 *
 * Copyright (C) 2007 by Instituto Nokia de Tecnologia (INdT)
 *
 * Author: Anderson Lizardo <anderson.lizardo@indt.org.br>
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
 *
 * Modified from sysdeps/linux-gnu/breakpoint.c and added ARM Thumb support.
 */

#include <sys/ptrace.h>
#include <sys/types.h>

#include "config.h"
#include "breakpoint.h"
#include "debug.h"
#include "proc.h"

void
arch_enable_breakpoint(pid_t pid, struct breakpoint *sbp)
{
	unsigned int i, j;
	const unsigned char break_insn[] = BREAKPOINT_VALUE;
	const unsigned char thumb_break_insn[] = THUMB_BREAKPOINT_VALUE;

	debug(1, "arch_enable_breakpoint(%d,%p)", pid, sbp->addr);

	for (i = 0; i < 1 + ((BREAKPOINT_LENGTH - 1) / sizeof(long)); i++) {
		union _ { long l; unsigned char b[SIZEOF_LONG]; };
		union _ orig, current;
		unsigned char *bytes = current.b;
		for (j = 0; j < sizeof(long); j++) {
			orig.b[j] = sbp->orig_value[i * sizeof(long) + j];
		}
		current.l = ptrace(PTRACE_PEEKTEXT, pid, sbp->addr + i * sizeof(long), 0);

		debug(2, "current = 0x%lx, orig_value = 0x%lx, thumb_mode = %d",
		      current.l, orig.l, sbp->arch.thumb_mode);
		for (j = 0; j < sizeof(long) && i * sizeof(long) + j < BREAKPOINT_LENGTH; j++) {

			sbp->orig_value[i * sizeof(long) + j] = bytes[j];
			if (!sbp->arch.thumb_mode) {
				bytes[j] = break_insn[i * sizeof(long) + j];
			}
			else if (j < THUMB_BREAKPOINT_LENGTH) {
				bytes[j] = thumb_break_insn[i * sizeof(long) + j];
			}
		}
		ptrace(PTRACE_POKETEXT, pid, sbp->addr + i * sizeof(long),
		       (void *)current.l);
	}
}

void
arch_disable_breakpoint(pid_t pid, const struct breakpoint *sbp)
{
	unsigned int i, j;

	debug(1, "arch_disable_breakpoint(%d,%p)", pid, sbp->addr);

	for (i = 0; i < 1 + ((BREAKPOINT_LENGTH - 1) / sizeof(long)); i++) {
		union _ { long l; unsigned char b[SIZEOF_LONG]; };
		union _ orig, current;
		unsigned char *bytes = current.b;
		for (j = 0; j < sizeof(long); j++) {
			orig.b[j] = sbp->orig_value[i * sizeof(long) + j];
		}
		current.l = ptrace(PTRACE_PEEKTEXT, pid, sbp->addr + i * sizeof(long), 0);

		debug(2, "current = 0x%lx, orig_value = 0x%lx, thumb_mode = %d",
		      current.l, orig.l, sbp->arch.thumb_mode);
		for (j = 0; j < sizeof(long) && i * sizeof(long) + j < BREAKPOINT_LENGTH; j++) {
			bytes[j] = sbp->orig_value[i * sizeof(long) + j];
		}
		ptrace(PTRACE_POKETEXT, pid, sbp->addr + i * sizeof(long),
		       (void *)current.l);
	}
}

int
arch_breakpoint_init(struct process *proc, struct breakpoint *sbp)
{
	/* XXX double cast  */
	sbp->arch.thumb_mode = ((uintptr_t)sbp->addr) & 1;
	if (sbp->arch.thumb_mode)
		/* XXX double cast */
		sbp->addr = (arch_addr_t)((uintptr_t)sbp->addr & ~1);
	return 0;
}

void
arch_breakpoint_destroy(struct breakpoint *sbp)
{
}

int
arch_breakpoint_clone(struct breakpoint *retp, struct breakpoint *sbp)
{
	retp->arch.thumb_mode = sbp->arch.thumb_mode;
	return 0;
}
