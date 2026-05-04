/*
 * This file is part of ltrace.
 * Copyright (C) 2014 Petr Machata, Red Hat, Inc.
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

#include <sys/ptrace.h>
#include <asm/ptrace.h>
#include <linux/uio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "backend.h"
#include "proc.h"

#define PC_OFF (32 * 4)

int
aarch64_read_gregs(struct process *proc, struct user_pt_regs *regs)
{
	*regs = (struct user_pt_regs) {};
	struct iovec iovec;
	iovec.iov_base = regs;
	iovec.iov_len = sizeof *regs;
	return ptrace(PTRACE_GETREGSET, proc->pid, NT_PRSTATUS, &iovec) < 0
		? -1 : 0;
}

int
aarch64_write_gregs(struct process *proc, struct user_pt_regs *regs)
{
	struct iovec iovec;
	iovec.iov_base = regs;
	iovec.iov_len = sizeof *regs;
	return ptrace(PTRACE_SETREGSET, proc->pid, NT_PRSTATUS, &iovec) < 0
		? -1 : 0;
}

int
aarch64_read_fregs(struct process *proc, struct user_fpsimd_state *regs)
{
	*regs = (struct user_fpsimd_state) {};
	struct iovec iovec;
	iovec.iov_base = regs;
	iovec.iov_len = sizeof *regs;
	return ptrace(PTRACE_GETREGSET, proc->pid, NT_FPREGSET, &iovec) < 0
		? -1 : 0;
}

arch_addr_t
get_instruction_pointer(struct process *proc)
{
	struct user_pt_regs regs;
	if (aarch64_read_gregs(proc, &regs) < 0) {
		fprintf(stderr, "get_instruction_pointer: "
			"Couldn't read registers of %d.\n", proc->pid);
		return 0;
	}

	/* 
	char buf[128];
	sprintf(buf, "cat /proc/%d/maps", proc->pid);
	system(buf);
	*/

	/* XXX double cast */
	return (arch_addr_t) (uintptr_t) regs.pc;
}

void
set_instruction_pointer(struct process *proc, arch_addr_t addr)
{
	struct user_pt_regs regs;
	if (aarch64_read_gregs(proc, &regs) < 0) {
		fprintf(stderr, "get_instruction_pointer: "
			"Couldn't read registers of %d.\n", proc->pid);
		return;
	}

	/* XXX double cast */
	regs.pc = (uint64_t) (uintptr_t) addr;

	if (aarch64_write_gregs(proc, &regs) < 0) {
		fprintf(stderr, "get_instruction_pointer: "
			"Couldn't write registers of %d.\n", proc->pid);
		return;
	}
}

arch_addr_t
get_stack_pointer(struct process *proc)
{
	struct user_pt_regs regs;
	if (aarch64_read_gregs(proc, &regs) < 0) {
		fprintf(stderr, "get_stack_pointer: "
			"Couldn't read registers of %d.\n", proc->pid);
		return 0;
	}

	/* XXX double cast */
	return (arch_addr_t) (uintptr_t) regs.sp;
}

arch_addr_t
get_return_addr(struct process *proc, arch_addr_t stack_pointer)
{
	struct user_pt_regs regs;
	if (aarch64_read_gregs(proc, &regs) < 0) {
		fprintf(stderr, "get_return_addr: "
			"Couldn't read registers of %d.\n", proc->pid);
		return 0;
	}

	/* XXX double cast */
	return (arch_addr_t) (uintptr_t) regs.regs[30];
}
