/*
 * This file is part of ltrace.
 * Copyright (C) 2010,2012,2013 Petr Machata, Red Hat Inc.
 * Copyright (C) 2011 Andreas Schwab
 * Copyright (C) 2002,2004,2008,2009 Juan Cespedes
 * Copyright (C) 2008 Luis Machado, IBM Corporation
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

#include <assert.h>
#include <elf.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "backend.h"
#include "breakpoint.h"
#include "common.h"
#include "insn.h"
#include "proc.h"
#include "ptrace.h"
#include "type.h"

#if (!defined(PTRACE_PEEKUSER) && defined(PTRACE_PEEKUSR))
# define PTRACE_PEEKUSER PTRACE_PEEKUSR
#endif

#if (!defined(PTRACE_POKEUSER) && defined(PTRACE_POKEUSR))
# define PTRACE_POKEUSER PTRACE_POKEUSR
#endif

void
get_arch_dep(struct process *proc)
{
#ifdef __powerpc64__
	proc->mask_32bit = (proc->e_machine == EM_PPC);
#endif
}

#define SYSCALL_INSN   0x44000002

/* Returns 1 if syscall, 2 if sysret, 0 otherwise. */
int
syscall_p(struct process *proc, int status, int *sysnum)
{
	if (WIFSTOPPED(status)
	    && WSTOPSIG(status) == (SIGTRAP | proc->tracesysgood)) {
		long pc = (long)get_instruction_pointer(proc);
#ifndef __LITTLE_ENDIAN__
		int insn =
		    (int)ptrace(PTRACE_PEEKTEXT, proc->pid, pc - sizeof(long),
				0);
#else
		int insn =
		    (int)ptrace(PTRACE_PEEKTEXT, proc->pid, pc - sizeof(int),
				0);
#endif

		if (insn == SYSCALL_INSN) {
			*sysnum =
			    (int)ptrace(PTRACE_PEEKUSER, proc->pid,
					sizeof(long) * PT_R0, 0);
			if (proc->callstack_depth > 0 &&
					proc->callstack[proc->callstack_depth - 1].is_syscall &&
					proc->callstack[proc->callstack_depth - 1].c_un.syscall == *sysnum) {
				return 2;
			}
			return 1;
		}
	}
	return 0;
}

/* The atomic skip code is mostly taken from GDB.  */

enum sw_singlestep_status
arch_sw_singlestep(struct process *proc, struct breakpoint *sbp,
		   int (*add_cb)(arch_addr_t, struct sw_singlestep_data *),
		   struct sw_singlestep_data *add_cb_data)
{
	arch_addr_t ip = get_instruction_pointer(proc);
	struct breakpoint *other = address2bpstruct(proc->leader, ip);

	debug(1, "arch_sw_singlestep pid=%d addr=%p %s(%p)",
	      proc->pid, ip, breakpoint_name(sbp), sbp->addr);

	/* If the original instruction was lwarx/ldarx, we can't
	 * single-step over it, instead we have to execute the whole
	 * atomic block at once.  */
	union {
		uint32_t insn;
		char buf[BREAKPOINT_LENGTH];
	} u;
	if (other != NULL) {
		memcpy(u.buf, sbp->orig_value, BREAKPOINT_LENGTH);
	} else if (proc_read_32(proc, ip, &u.insn) < 0) {
		fprintf(stderr, "couldn't read instruction at IP %p\n", ip);
		/* Do the normal singlestep.  */
		return SWS_HW;
	}

	if ((u.insn & LWARX_MASK) != LWARX_INSTRUCTION
	    && (u.insn & LWARX_MASK) != LDARX_INSTRUCTION)
		return SWS_HW;

	debug(1, "singlestep over atomic block at %p", ip);

	int insn_count;
	arch_addr_t addr = ip;
	for (insn_count = 0; ; ++insn_count) {
		addr += 4;
		unsigned long l = ptrace(PTRACE_PEEKTEXT, proc->pid, addr, 0);
		if (l == (unsigned long)-1 && errno)
			return SWS_FAIL;
		uint32_t insn;
#ifdef __powerpc64__
# ifdef __LITTLE_ENDIAN__
		insn = (uint32_t) l ;
# else
		insn = l >> 32;
# endif
#else
		insn = l;
#endif

		/* If a conditional branch is found, put a breakpoint
		 * in its destination address.  */
		if ((insn & BRANCH_MASK) == BC_INSN) {
			arch_addr_t branch_addr = ppc_branch_dest(addr, insn);
			debug(1, "pid=%d, branch in atomic block from %p to %p",
			      proc->pid, addr, branch_addr);
			if (add_cb(branch_addr, add_cb_data) < 0)
				return SWS_FAIL;
		}

		/* Assume that the atomic sequence ends with a
		 * stwcx/stdcx instruction.  */
		if ((insn & STWCX_MASK) == STWCX_INSTRUCTION
		    || (insn & STWCX_MASK) == STDCX_INSTRUCTION) {
			debug(1, "pid=%d, found end of atomic block %p at %p",
			      proc->pid, ip, addr);
			break;
		}

		/* Arbitrary cut-off.  If we didn't find the
		 * terminating instruction by now, just give up.  */
		if (insn_count > 16) {
			fprintf(stderr, "[%d] couldn't find end of atomic block"
				" at %p\n", proc->pid, ip);
			return SWS_FAIL;
		}
	}

	/* Put the breakpoint to the next instruction.  */
	addr += 4;
	if (add_cb(addr, add_cb_data) < 0)
		return SWS_FAIL;

	debug(1, "PTRACE_CONT");
	ptrace(PTRACE_CONT, proc->pid, 0, 0);
	return SWS_OK;
}

size_t
arch_type_sizeof(struct process *proc, struct arg_type_info *info)
{
	if (proc == NULL)
		return (size_t)-2;

	switch (info->type) {
	case ARGTYPE_VOID:
		return 0;

	case ARGTYPE_CHAR:
		return 1;

	case ARGTYPE_SHORT:
	case ARGTYPE_USHORT:
		return 2;

	case ARGTYPE_INT:
	case ARGTYPE_UINT:
		return 4;

	case ARGTYPE_LONG:
	case ARGTYPE_ULONG:
	case ARGTYPE_POINTER:
		return proc->e_machine == EM_PPC64 ? 8 : 4;

	case ARGTYPE_FLOAT:
		return 4;
	case ARGTYPE_DOUBLE:
		return 8;

	case ARGTYPE_ARRAY:
	case ARGTYPE_STRUCT:
		/* Use default value.  */
		return (size_t)-2;

	default:
		assert(info->type != info->type);
		abort();
		break;
	}
}

size_t
arch_type_alignof(struct process *proc, struct arg_type_info *info)
{
	if (proc == NULL)
		return (size_t)-2;

	switch (info->type) {
	default:
		assert(info->type != info->type);
		abort();
		break;

	case ARGTYPE_CHAR:
	case ARGTYPE_SHORT:
	case ARGTYPE_USHORT:
	case ARGTYPE_INT:
	case ARGTYPE_UINT:
	case ARGTYPE_LONG:
	case ARGTYPE_ULONG:
	case ARGTYPE_POINTER:
	case ARGTYPE_FLOAT:
	case ARGTYPE_DOUBLE:
		/* On both PPC and PPC64, fundamental types have the
		 * same alignment as size.  */
		return arch_type_sizeof(proc, info);

	case ARGTYPE_ARRAY:
	case ARGTYPE_STRUCT:
		/* Use default value.  */
		return (size_t)-2;
	}
}
