/*
 * This file is part of ltrace.
 * Copyright (C) 2013 Petr Machata, Red Hat Inc.
 * Copyright (C) 2002,2008,2009 Juan Cespedes
 * Copyright (C) 2009 Juan Cespedes
 * Copyright (C) 2008 Luis Machado, IBM Corporation
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
#include <asm/ptrace.h>
#include <errno.h>
#include <error.h>

#include "proc.h"
#include "common.h"

#if (!defined(PTRACE_PEEKUSER) && defined(PTRACE_PEEKUSR))
# define PTRACE_PEEKUSER PTRACE_PEEKUSR
#endif

#if (!defined(PTRACE_POKEUSER) && defined(PTRACE_POKEUSR))
# define PTRACE_POKEUSER PTRACE_POKEUSR
#endif

void *
get_instruction_pointer(struct process *proc)
{
	return (void *)ptrace(PTRACE_PEEKUSER, proc->pid, sizeof(long)*PT_NIP, 0);
}

void
set_instruction_pointer(struct process *proc, void *addr)
{
	if (ptrace(PTRACE_POKEUSER, proc->pid, sizeof(long)*PT_NIP, addr) != 0)
		error(0, errno, "set_instruction_pointer");
}

void *
get_stack_pointer(struct process *proc)
{
	return (void *)ptrace(PTRACE_PEEKUSER, proc->pid, sizeof(long)*PT_R1, 0);
}

void *
get_return_addr(struct process *proc, void *stack_pointer)
{
	return (void *)ptrace(PTRACE_PEEKUSER, proc->pid, sizeof(long)*PT_LNK, 0);
}
