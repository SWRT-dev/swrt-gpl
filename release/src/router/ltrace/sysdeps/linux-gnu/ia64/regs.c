/*
 * This file is part of ltrace.
 * Copyright (C) 2011,2012,2013 Petr Machata, Red Hat Inc.
 * Copyright (C) 2008,2009 Juan Cespedes
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

#include "config.h"

#include <sys/types.h>
#include <sys/ptrace.h>
#include <errno.h>

#include <asm/ptrace_offsets.h>
#include <asm/rse.h>

#include <stddef.h>
#include "proc.h"
#include "common.h"

void *
get_instruction_pointer(struct process *proc)
{
	unsigned long ip = ptrace(PTRACE_PEEKUSER, proc->pid, PT_CR_IIP, 0);
	unsigned long slot =
	    (ptrace(PTRACE_PEEKUSER, proc->pid, PT_CR_IPSR, 0) >> 41) & 3;

	return (void *)(ip | slot);
}

void
set_instruction_pointer(struct process *proc, void *addr)
{

	unsigned long newip = (unsigned long)addr;
	unsigned long slot = (unsigned long)addr & 0xf;
	unsigned long psr = ptrace(PTRACE_PEEKUSER, proc->pid, PT_CR_IPSR, 0);

	psr &= ~(3UL << 41);
	psr |= (slot & 0x3) << 41;

	newip &= ~0xfUL;

	ptrace(PTRACE_POKEUSER, proc->pid, PT_CR_IIP, (long)newip);
	ptrace(PTRACE_POKEUSER, proc->pid, PT_CR_IPSR, psr);
}

void *
get_stack_pointer(struct process *proc)
{
	long l = ptrace(PTRACE_PEEKUSER, proc->pid, PT_R12, 0);
	if (l == -1 && errno)
		return NULL;
	return (void *)l;
}

void *
get_return_addr(struct process *proc, void *stack_pointer)
{
	long l = ptrace(PTRACE_PEEKUSER, proc->pid, PT_B0, 0);
	if (l == -1 && errno)
		return NULL;
	return (void *)l;
}
