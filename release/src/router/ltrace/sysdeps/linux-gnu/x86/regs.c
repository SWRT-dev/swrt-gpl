/*
 * This file is part of ltrace.
 * Copyright (C) 2012,2013 Petr Machata, Red Hat Inc.
 * Copyright (C) 1998,2002,2004,2008,2009 Juan Cespedes
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
#include <sys/reg.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "backend.h"
#include "proc.h"

#if (!defined(PTRACE_PEEKUSER) && defined(PTRACE_PEEKUSR))
# define PTRACE_PEEKUSER PTRACE_PEEKUSR
#endif

#if (!defined(PTRACE_POKEUSER) && defined(PTRACE_POKEUSR))
# define PTRACE_POKEUSER PTRACE_POKEUSR
#endif

#ifdef __x86_64__
# define XIP (8 * RIP)
# define XSP (8 * RSP)
#else
# define XIP (4 * EIP)
# define XSP (4 * UESP)
#endif

static arch_addr_t
conv_32(arch_addr_t val)
{
	/* XXX Drop the multiple double casts when arch_addr_t
	 * becomes integral.  */
	return (arch_addr_t)(uintptr_t)(uint32_t)(uintptr_t)val;
}

void *
get_instruction_pointer(struct process *proc)
{
	long int ret = ptrace(PTRACE_PEEKUSER, proc->pid, XIP, 0);
	if (proc->e_machine == EM_386)
		ret &= 0xffffffff;
	return (void *)ret;
}

void
set_instruction_pointer(struct process *proc, arch_addr_t addr)
{
	if (proc->e_machine == EM_386)
		addr = conv_32(addr);
	ptrace(PTRACE_POKEUSER, proc->pid, XIP, addr);
}

void *
get_stack_pointer(struct process *proc)
{
	long sp = ptrace(PTRACE_PEEKUSER, proc->pid, XSP, 0);
	if (sp == -1 && errno) {
		fprintf(stderr, "Couldn't read SP register: %s\n",
			strerror(errno));
		return NULL;
	}

	/* XXX Drop the multiple double casts when arch_addr_t
	 * becomes integral.  */
	arch_addr_t ret = (arch_addr_t)(uintptr_t)sp;
	if (proc->e_machine == EM_386)
		ret = conv_32(ret);
	return ret;
}

void *
get_return_addr(struct process *proc, void *sp)
{
	long a = ptrace(PTRACE_PEEKTEXT, proc->pid, sp, 0);
	if (a == -1 && errno) {
		fprintf(stderr, "Couldn't read return value: %s\n",
			strerror(errno));
		return NULL;
	}

	/* XXX Drop the multiple double casts when arch_addr_t
	 * becomes integral.  */
	arch_addr_t ret = (arch_addr_t)(uintptr_t)a;
	if (proc->e_machine == EM_386)
		ret = conv_32(ret);
	return ret;
}
