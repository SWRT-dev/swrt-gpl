/*
 * This file is part of ltrace.
 * Copyright (C) 2008,2009 Juan Cespedes
 * Copyright (C) 2006 Steve Fink
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
#include <stddef.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/ptrace.h>
#include <string.h>
#include <asm/ptrace_offsets.h>
#include <asm/rse.h>
#include <errno.h>

#include "proc.h"
#include "common.h"
#include "type.h"

/* What we think of as a bundle, ptrace thinks of it as two unsigned
 * longs */
union bundle_t {
	/* An IA64 instruction bundle has a 5 bit header describing the
	 * type of bundle, then 3 41 bit instructions
	 */
	struct {
		struct {
			unsigned long template:5;
			unsigned long slot0:41;
			unsigned long bot_slot1:18;
		} word0;
		struct {
			unsigned long top_slot1:23;
			unsigned long slot2:41;
		} word1;
	} bitmap;
	unsigned long code[2];
};

union cfm_t {
	struct {
		unsigned long sof:7;
		unsigned long sol:7;
		unsigned long sor:4;
		unsigned long rrb_gr:7;
		unsigned long rrb_fr:7;
		unsigned long rrb_pr:6;
	} cfm;
	unsigned long value;
};

int
syscall_p(struct process *proc, int status, int *sysnum)
{
	if (WIFSTOPPED(status)
	    && WSTOPSIG(status) == (SIGTRAP | proc->tracesysgood)) {
		long l = ptrace(PTRACE_PEEKUSER, proc->pid, PT_CR_IPSR, 0);
		if (l == -1 && errno)
			return -1;
		unsigned long slot = ((unsigned long)l >> 41) & 0x3;
		unsigned long ip =
		    ptrace(PTRACE_PEEKUSER, proc->pid, PT_CR_IIP, 0);

		/* r15 holds the system call number */
		unsigned long r15 =
		    ptrace(PTRACE_PEEKUSER, proc->pid, PT_R15, 0);
		unsigned long insn;

		union bundle_t bundle;

		/* On fault, the IP has moved forward to the next
		 * slot.  If that is zero, then the actual place we
		 * broke was in the previous bundle, so wind back the
		 * IP.
		 */
		if (slot == 0)
			ip = ip - 16;
		bundle.code[0] = ptrace(PTRACE_PEEKTEXT, proc->pid, ip, 0);
		bundle.code[1] = ptrace(PTRACE_PEEKTEXT, proc->pid, ip + 8, 0);

		unsigned long bot = 0UL | bundle.bitmap.word0.bot_slot1;
		unsigned long top = 0UL | bundle.bitmap.word1.top_slot1;

		/* handle the rollback, slot 0 is actually slot 2 of
		 * the previous instruction (see above) */
		switch (slot) {
		case 0:
			insn = bundle.bitmap.word1.slot2;
			break;
		case 1:
			insn = bundle.bitmap.word0.slot0;
			break;
		case 2:
			/* make sure we're shifting about longs */
			insn = 0UL | bot | (top << 18UL);
			break;
		default:
			printf("Ummm, can't find instruction slot?\n");
			exit(1);
		}

		/* We need to support both the older break instruction
		 * type syscalls, and the new epc type ones.
		 *
		 * Bit 20 of the break constant is encoded in the "i"
		 * bit (bit 36) of the instruction, hence you should
		 * see 0x1000000000.
		 *
		 *  An EPC call is just 0x1ffffffffff
		 */
		if (insn == 0x1000000000 || insn == 0x1ffffffffff) {
			*sysnum = r15;
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

void
get_arch_dep(struct process *proc)
{
}
