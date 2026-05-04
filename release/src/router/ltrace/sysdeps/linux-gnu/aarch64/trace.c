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
#include <sys/types.h>
#include <sys/wait.h>
#include <asm/ptrace.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "backend.h"
#include "proc.h"

void
get_arch_dep(struct process *proc)
{
}

int aarch64_read_gregs(struct process *proc, struct user_pt_regs *regs);

/* The syscall instruction is:
 * | 31                   21 | 20    5 | 4       0 |
 * | 1 1 0 1 0 1 0 0 | 0 0 0 |  imm16  | 0 0 0 0 1 | */
#define SVC_MASK  0xffe0001f
#define SVC_VALUE 0xd4000001

int
syscall_p(struct process *proc, int status, int *sysnum)
{
	if (WIFSTOPPED(status)
	    && WSTOPSIG(status) == (SIGTRAP | proc->tracesysgood)) {

		struct user_pt_regs regs;
		if (aarch64_read_gregs(proc, &regs) < 0) {
			fprintf(stderr, "syscall_p: "
				"Couldn't read registers of %d.\n", proc->pid);
			return -1;
		}

		errno = 0;
		unsigned long insn = (unsigned long) ptrace(PTRACE_PEEKTEXT,
							    proc->pid,
							    regs.pc - 4, 0);
		if (insn == -1UL && errno != 0) {
			fprintf(stderr, "syscall_p: "
				"Couldn't peek into %d: %s\n", proc->pid,
				strerror(errno));
			return -1;
		}

		insn &= 0xffffffffUL;
		if ((insn & SVC_MASK) == SVC_VALUE) {
			*sysnum = regs.regs[8];

			size_t d1 = proc->callstack_depth - 1;
			if (proc->callstack_depth > 0
			    && proc->callstack[d1].is_syscall
			    && proc->callstack[d1].c_un.syscall == *sysnum)
				return 2;

			return 1;
		}
	}

	return 0;
}
