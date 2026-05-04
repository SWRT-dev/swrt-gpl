/*
 * This file is part of ltrace.
 * Copyright (C) 2013 Petr Machata, Red Hat Inc.
 * Copyright (C) 2002,2004,2008,2009 Juan Cespedes
 * Copyright (C) 2009 Juan Cespedes
 * Copyright (C) 2006 Ian Wienand
 * Copyright (C) 2001 IBM Poughkeepsie, IBM Corporation
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

#include "proc.h"
#include "common.h"

#if (!defined(PTRACE_PEEKUSER) && defined(PTRACE_PEEKUSR))
# define PTRACE_PEEKUSER PTRACE_PEEKUSR
#endif

#if (!defined(PTRACE_POKEUSER) && defined(PTRACE_POKEUSR))
# define PTRACE_POKEUSER PTRACE_POKEUSR
#endif

#ifdef __s390x__
#define PSW_MASK	0xffffffffffffffff
#define PSW_MASK31	0x7fffffff
#else
#define PSW_MASK	0x7fffffff
#endif

arch_addr_t
get_instruction_pointer(struct process *proc)
{
	long ret = ptrace(PTRACE_PEEKUSER, proc->pid, PT_PSWADDR, 0) & PSW_MASK;
#ifdef __s390x__
	if (proc->mask_32bit)
		ret &= PSW_MASK31;
#endif
	/* XXX double cast.  */
	return (arch_addr_t)ret;
}

void
set_instruction_pointer(struct process *proc, arch_addr_t addr)
{
#ifdef __s390x__
	if (proc->mask_32bit)
		/* XXX double cast.  */
		addr = (arch_addr_t)((uintptr_t)addr & PSW_MASK31);
#else
	/* XXX double cast.  */
	addr = (arch_addr_t)((uintptr_t)addr | ~PSW_MASK);
#endif
	ptrace(PTRACE_POKEUSER, proc->pid, PT_PSWADDR, addr);
}

arch_addr_t
get_stack_pointer(struct process *proc)
{
	long ret = ptrace(PTRACE_PEEKUSER, proc->pid, PT_GPR15, 0) & PSW_MASK;
#ifdef __s390x__
	if (proc->mask_32bit)
		ret &= PSW_MASK31;
#endif
	/* XXX double cast.  */
	return (arch_addr_t)ret;
}

arch_addr_t
get_return_addr(struct process *proc, arch_addr_t stack_pointer)
{
	long ret = ptrace(PTRACE_PEEKUSER, proc->pid, PT_GPR14, 0) & PSW_MASK;
#ifdef __s390x__
	if (proc->mask_32bit)
		ret &= PSW_MASK31;
#endif
	/* XXX double cast.  */
	return (arch_addr_t)ret;
}
