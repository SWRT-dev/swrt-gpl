/*
 * This file is part of ltrace.
 * Copyright (C) 2010 Edgar E. Iglesias
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

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/ptrace.h>
#include <asm/ptrace.h>
#include <elf.h>

#include "proc.h"
#include "common.h"

#if (!defined(PTRACE_PEEKUSER) && defined(PTRACE_PEEKUSR))
# define PTRACE_PEEKUSER PTRACE_PEEKUSR
#endif

#if (!defined(PTRACE_POKEUSER) && defined(PTRACE_POKEUSR))
# define PTRACE_POKEUSER PTRACE_POKEUSR
#endif

void get_arch_dep(struct process *proc)
{
}

/* Returns 1 if syscall, 2 if sysret, 0 otherwise.
 */
#define SYSCALL_INSN   0xe93d
int syscall_p(struct process *proc, int status, int *sysnum)
{
	if (WIFSTOPPED(status)
	    && WSTOPSIG(status) == (SIGTRAP | proc->tracesysgood)) {
		long pc = (long)get_instruction_pointer(proc);
		unsigned int insn =
		    (int)ptrace(PTRACE_PEEKTEXT, proc->pid, pc - sizeof(long),
				0);

		if ((insn >> 16) == SYSCALL_INSN) {
			*sysnum =
			    (int)ptrace(PTRACE_PEEKUSER, proc->pid,
					sizeof(long) * PT_R9, 0);
			if (proc->callstack_depth > 0
			    && proc->callstack[proc->callstack_depth -
					       1].is_syscall) {
				return 2;
			}
			return 1;
		}
	}
	return 0;
}

long gimme_arg(enum tof type, struct process *proc, int arg_num,
	       struct arg_type_info *info)
{
	int pid = proc->pid;

	if (arg_num == -1) {	/* return value */
		return ptrace(PTRACE_PEEKUSER, pid, PT_R10 * 4, 0);
	} else if (arg_num < 6) {
		int pt_arg[6] =
			{
				PT_ORIG_R10, PT_R11, PT_R12, PT_R13, PT_MOF,
				PT_SRP
			};
		return ptrace(PTRACE_PEEKUSER, pid, pt_arg[arg_num] * 4, 0);
	} else {
		return ptrace(PTRACE_PEEKDATA, pid,
			      proc->stack_pointer + 4 * (arg_num - 6), 0);
	}
	return 0;
}
