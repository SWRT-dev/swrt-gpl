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

#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/ptrace.h>
#include <asm/ptrace.h>

#include "common.h"
#include "proc.h"

void
get_arch_dep(struct process *proc)
{
}

/* Returns 1 if syscall, 2 if sysret, 0 otherwise.  */
int syscall_p(struct process *proc, int status, int *sysnum)
{
	if (WIFSTOPPED(status)
	    && WSTOPSIG(status) == (SIGTRAP | proc->tracesysgood)) {
		/* get the user's pc */
		int pc = ptrace(PTRACE_PEEKUSER, proc->pid, REG_PC, 0);

		/* fetch the SWI instruction */
		int insn = ptrace(PTRACE_PEEKTEXT, proc->pid, pc - 3, 0);

		*sysnum = ptrace(PTRACE_PEEKUSER, proc->pid, SYSCALL_NR, 0);
		/* if it is a syscall, return 1 or 2 */
		if ((insn & XTENSA_SYSCALL_MASK) == XTENSA_SYSCALL_VALUE) {
			if ((proc->callstack_depth > 0)
			    && proc->callstack[proc->callstack_depth
					       - 1].is_syscall) {
				return 2;
			} else {
				return 1;
			}
		}
	}
	return 0;
}
