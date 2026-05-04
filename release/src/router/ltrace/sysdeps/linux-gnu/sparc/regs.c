/*
 * This file is part of ltrace.
 * Copyright (C) 2013 Petr Machata, Red Hat Inc.
 * Copyright (C) 2004,2008,2009 Juan Cespedes
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
#include "ptrace.h"
#include "proc.h"
#include "common.h"

void *
get_instruction_pointer(struct process *proc)
{
	proc_archdep *a = (proc_archdep *) (proc->arch_ptr);
	if (a->valid)
		return (void *)a->regs.pc;
	return (void *)-1;
}

void
set_instruction_pointer(struct process *proc, void *addr)
{
	proc_archdep *a = (proc_archdep *) (proc->arch_ptr);
	if (a->valid)
		a->regs.pc = (long)addr;
}

void *
get_stack_pointer(struct process *proc)
{
	proc_archdep *a = (proc_archdep *) (proc->arch_ptr);
	if (a->valid)
		return (void *)a->regs.u_regs[UREG_I5];
	return (void *)-1;
}

void *
get_return_addr(struct process *proc, void *stack_pointer)
{
	proc_archdep *a = (proc_archdep *) (proc->arch_ptr);
	unsigned int t;
	if (!a->valid)
		return (void *)-1;
	/* Work around structure returns */
	t = ptrace(PTRACE_PEEKTEXT, proc->pid, a->regs.u_regs[UREG_I6] + 8, 0);
	if (t < 0x400000)
		return (void *)a->regs.u_regs[UREG_I6] + 12;
	return (void *)a->regs.u_regs[UREG_I6] + 8;
}
