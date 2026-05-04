/*
 * This file is part of ltrace.
 *
 * Copyright (C) 2013 Imagination Technologies Ltd.
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

#include "config.h"

#include <sys/types.h>
#include <sys/ptrace.h>
#include <linux/uio.h>
#include <asm/ptrace.h>

#include "proc.h"
#include "common.h"

arch_addr_t
get_instruction_pointer(struct process *proc)
{
	struct user_gp_regs regs;
	struct iovec iov;

	iov.iov_base = &regs;
	iov.iov_len = sizeof(regs);
	if (ptrace(PTRACE_GETREGSET, proc->pid, NT_PRSTATUS, (long)&iov))
		return (void *)-1;

	return (void *)regs.pc; /* PC */
}

void
set_instruction_pointer(struct process *proc, arch_addr_t addr)
{
	struct user_gp_regs regs;
	struct iovec iov;

	iov.iov_base = &regs;
	iov.iov_len = sizeof(regs);
	if (ptrace(PTRACE_GETREGSET, proc->pid, NT_PRSTATUS, (long)&iov))
		return;

	regs.pc = (unsigned long)addr;

	iov.iov_base = &regs;
	iov.iov_len = sizeof(regs);
	ptrace(PTRACE_SETREGSET, proc->pid, NT_PRSTATUS, (long)&iov);
}

arch_addr_t
get_stack_pointer(struct process *proc)
{
	struct user_gp_regs regs;
	struct iovec iov;

	iov.iov_base = &regs;
	iov.iov_len = sizeof(regs);
	if (ptrace(PTRACE_GETREGSET, proc->pid, NT_PRSTATUS, (long)&iov))
		return (void *)-1;

	return (void *)regs.ax[0][0]; /* A0StP (A0.0) */
}

arch_addr_t
get_return_addr(struct process *proc, void *stack_pointer)
{
	struct user_gp_regs regs;
	struct iovec iov;

	iov.iov_base = &regs;
	iov.iov_len = sizeof(regs);
	if (ptrace(PTRACE_GETREGSET, proc->pid, NT_PRSTATUS, (long)&iov))
		return (void *)-1;

	return (void *)regs.dx[4][1]; /* D1RtP (D1.4) */
}
