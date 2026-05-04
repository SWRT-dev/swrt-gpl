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

#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ptrace.h>
#include <asm/ptrace.h>

#include "proc.h"
#include "common.h"

static int xtensa_peek_user(struct process *proc, unsigned addr,
			    unsigned long *res)
{
	long retval;

	errno = 0;
	retval = ptrace(PTRACE_PEEKUSER, proc->pid, addr, 0);
	if (retval == -1 && errno) {
		fprintf(stderr, "%s: pid=%d, %s\n",
			__func__, proc->pid, strerror(errno));
		*res = 0;
		return 0;
	}
	*res = retval;
	return 1;
}

void *get_instruction_pointer(struct process *proc)
{
	unsigned long res;

	if (xtensa_peek_user(proc, REG_PC, &res))
		return (void *)res;
	else
		return NULL;
}

void set_instruction_pointer(struct process *proc, void *addr)
{
	ptrace(PTRACE_POKEUSER, proc->pid, REG_PC, addr);
}

void *get_stack_pointer(struct process *proc)
{
	unsigned long res;

	if (xtensa_peek_user(proc, REG_A_BASE + 1, &res))
		return (void *)res;
	else
		return NULL;
}

void *get_return_addr(struct process *proc, void *stack_pointer)
{
	unsigned long res;

	if (xtensa_peek_user(proc, REG_A_BASE, &res))
		/* Assume call8, mask the upper 2 bits. */
		return (void *)(0x3FFFFFFF & res);
	else
		return NULL;
}
