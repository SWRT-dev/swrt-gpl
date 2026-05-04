/*
 * This file is part of ltrace.
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

#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include "ptrace.h"
#include "proc.h"
#include "common.h"

void
get_arch_dep(struct process *proc)
{
	proc_archdep *a;
	if (!proc->arch_ptr)
		proc->arch_ptr = (void *)malloc(sizeof(proc_archdep));
	a = (proc_archdep *) (proc->arch_ptr);
	a->valid = (ptrace(PTRACE_GETREGS, proc->pid, &a->regs, 0) >= 0);
}

/* Returns syscall number if `pid' stopped because of a syscall.
 * Returns -1 otherwise
 */
int
syscall_p(struct process *proc, int status, int *sysnum)
{
	if (WIFSTOPPED(status)
	    && WSTOPSIG(status) == (SIGTRAP | proc->tracesysgood)) {
		void *ip = get_instruction_pointer(proc);
		unsigned int insn;
		if (ip == (void *)-1)
			return 0;
		insn = ptrace(PTRACE_PEEKTEXT, proc->pid, ip, 0);
		if ((insn & 0xc1f8007f) == 0x81d00010) {
			*sysnum = ((proc_archdep *) proc->arch_ptr)->regs.u_regs[UREG_G0];
			if (proc->callstack_depth > 0 &&
					proc->callstack[proc->callstack_depth - 1].is_syscall &&
					proc->callstack[proc->callstack_depth - 1].c_un.syscall == *sysnum) {
				return 2;
			} else if (*sysnum >= 0) {
				return 1;
			}
		}
	}
	return 0;
}

long
gimme_arg(enum tof type, struct process *proc, int arg_num,
	  struct arg_type_info *info)
{
	proc_archdep *a = (proc_archdep *) proc->arch_ptr;
	if (!a->valid) {
		fprintf(stderr, "Could not get child registers\n");
		exit(1);
	}
	if (arg_num == -1)	/* return value */
		return a->regs.u_regs[UREG_G7];

	if (type == LT_TOF_FUNCTION || type == LT_TOF_SYSCALL || arg_num >= 6) {
		if (arg_num < 6)
			return ((int *)&a->regs.u_regs[UREG_G7])[arg_num];
		return ptrace(PTRACE_PEEKTEXT, proc->pid,
			      proc->stack_pointer + 64 * (arg_num + 1));
	} else if (type == LT_TOF_FUNCTIONR)
		return a->func_arg[arg_num];
	else if (type == LT_TOF_SYSCALLR)
		return a->sysc_arg[arg_num];
	else {
		fprintf(stderr, "gimme_arg called with wrong arguments\n");
		exit(1);
	}
	return 0;
}
