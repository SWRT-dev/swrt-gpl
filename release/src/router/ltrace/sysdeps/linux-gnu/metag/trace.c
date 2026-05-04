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
#include <sys/wait.h>
#include <signal.h>
#include <sys/ptrace.h>
#include <linux/uio.h>
#include <asm/ptrace.h>
#include <assert.h>

#include "proc.h"
#include "common.h"

#define METAG_INSN_SIZE	4
#define N_UNITS 2
#define REG_SIZE 4

/* unit codes  */
enum metag_unitnum {
	METAG_UNIT_CT,       /* 0x0 */
	METAG_UNIT_D0,
	METAG_UNIT_D1,
	METAG_UNIT_A0,
	METAG_UNIT_A1,       /* 0x4 */
	METAG_UNIT_PC,
	METAG_UNIT_RA,
	METAG_UNIT_TR,
	METAG_UNIT_TT,       /* 0x8 */
	METAG_UNIT_FX,
	METAG_UNIT_MAX,
};

/**
    \param proc The process that had an event.

    Called by \c next_event() right after the return from wait.
 */
void
get_arch_dep(struct process *proc)
{

}

/**
    \param proc Process that had event.
    \param status From \c\ waitpid().
    \param sysnum 0-based syscall number.
    \return 1 if syscall, 2 if sysret, 0 otherwise.

    Called by \c next_event() after the call to get_arch_dep().

 */
int
syscall_p(struct process *proc, int status, int *sysnum)
{
	if (WIFSTOPPED(status)
	    && WSTOPSIG(status) == (SIGTRAP | proc->tracesysgood)) {
		struct user_gp_regs regs;
		struct iovec iov;

		/* Get GP registers.  */
		iov.iov_base = &regs;
		iov.iov_len = sizeof(regs);
		if (ptrace(PTRACE_GETREGSET, proc->pid, NT_PRSTATUS,
			   (long)&iov))
			return -1;

		/* Fetch the SWITCH instruction.  */
		unsigned int insn = ptrace(PTRACE_PEEKTEXT, proc->pid, regs.pc,
					   0);
		*sysnum = regs.dx[0][1];

		if (insn != 0xaf440001) {
			/* Check if we're returning from the system call.  */
			insn = ptrace(PTRACE_PEEKTEXT, proc->pid, regs.pc - 4,
				      0);
			if (insn == 0xaf440001)
				return 2;

			return 0;
		}

		if (*sysnum >= 0)
			return 1;
	}
	return 0;
}

/* 2-bit base unit (BU) mapping.  */
static enum metag_unitnum metag_bu_map[4] = {
	METAG_UNIT_A1,
	METAG_UNIT_D0,
	METAG_UNIT_D1,
	METAG_UNIT_A0,
};

static int
get_regval_from_unit(enum metag_unitnum unit, unsigned int reg,
		     struct user_gp_regs *regs)
{
	/*
	 * Check if reg has a sane value.
	 * We do have N_UNITS, each one having X registers
	 * and each register is REG_SIZE bytes.
	 */
	if ((unit == METAG_UNIT_A0) || (unit == METAG_UNIT_A1)) {
		if (reg >= ((sizeof(regs->ax)/N_UNITS/REG_SIZE)))
			goto bad_reg;
	} else if ((unit == METAG_UNIT_D0) || (unit == METAG_UNIT_D1)) {
		if (reg >= ((sizeof(regs->dx)/N_UNITS/REG_SIZE)))
			goto bad_reg;
	}

	switch(unit) {
	case METAG_UNIT_A1:
		return regs->ax[reg][1];
	case METAG_UNIT_D0:
		return regs->dx[reg][0];
	case METAG_UNIT_D1:
		return regs->dx[reg][1];
	case METAG_UNIT_A0:
		return regs->ax[reg][0];
	/* We really shouldn't be here.  */
	default:
		assert(unit != unit);
		abort();
	}
	return 0;

bad_reg:
	fprintf(stderr,
		"Reading from register %d of unit %d is not implemented.",
		reg, unit);
	return 0;

}

static int
metag_next_pcs(struct process *proc, uint32_t pc, uint32_t *newpc)
{
	uint32_t inst;
	int nr = 0, imm, reg_val;
	unsigned int unit = 0, reg;
	struct user_gp_regs regs;
	struct iovec iov;

	inst = ptrace(PTRACE_PEEKTEXT, proc->pid, pc, 0);

	if (inst == 0xa0fffffe) { /* NOP (Special branch instruction) */
		newpc[nr++] = pc + 4;
	} else if ((inst & 0xff000000) == 0xa0000000) {
		/* Matching 0xA 0x0 for opcode for B #S19 or B<cc> #S19.
		 *
		 * Potential Targets:
		 * - pc + #S19 * METAG_INSN_SIZE if R=1 or <CC> true
		 * - pc + 4  */
		imm = ((inst << 8) >> 13) * METAG_INSN_SIZE;
		newpc[nr++] = pc + imm;
		newpc[nr++] = pc + 4;
	} else if ((inst & 0xff000000) == 0xac000000) {
		/* Matching 0xA 0xC for opcode.
		 * JUMP BBx.r,#X16 or CALL BBx.r,#X16
		 *
		 * pc = reg + #x16 (aligned)  */
		imm = (inst >> 3) & 0xffff;
		reg = (inst >> 19) & 0x1f;
		unit = metag_bu_map[inst & 0x3];
		iov.iov_base = &regs;
		iov.iov_len = sizeof(regs);
		if (ptrace(PTRACE_GETREGSET, proc->pid,
			   NT_PRSTATUS, (long)&iov))
			goto ptrace_fail;

		reg_val = get_regval_from_unit(unit, reg, &regs);
		newpc[nr++] = (reg_val + imm) & -METAG_INSN_SIZE;
	} else if ((inst & 0xff000000) == 0xab000000) {
		/* Matching 0xA 0xB for opcode.
		 *
		 * CALLR BBx.r,#S19  */
		imm = ((inst << 8) >> 13) * METAG_INSN_SIZE;
		newpc[nr++] = pc + imm;
	} else if ((inst & 0xff0001e0) == 0xa30000a0) {
		/*
		 * Matching 0xA 0x3 for opcode and then
		 * Ud (bit 8-5) = 0x5 = METAG_UNIT_PC
		 *
		 * Potential MOV PC,.. or SWAP<cc> PC,.. or SWAP<cc> ..,PC
		 */

		iov.iov_base = &regs;
		iov.iov_len = sizeof(regs);
		if (ptrace(PTRACE_GETREGSET, proc->pid,
			   NT_PRSTATUS, (long)&iov))
			goto ptrace_fail;

		/*
		 * Maybe PC is the source register for a SWAP?
		 * bit9 = 1 and bit13-10(Us) == METAG_UNIT_PC
		 */
		if (((inst >> 9 ) & 0x1) &&
		    (((inst >> 10) & 0xf) == METAG_UNIT_PC)) {
			/* PC will get its value from the
			 * destination register.  */
			reg = (inst >> 14) & 0x1f;
			unit = (inst >> 5) & 0xf;
		} else { /* PC is the destination register. 
			  * Find the source register.  */
			reg = (inst >> 19) & 0x1f;
			unit = (inst >> 10) & 0xf;
		}

		switch(unit) {
		case METAG_UNIT_D0:
		case METAG_UNIT_D1:
		case METAG_UNIT_A0:
		case METAG_UNIT_A1:
			reg_val = get_regval_from_unit(unit, reg, &regs);
			break;
		case METAG_UNIT_PC:
			reg_val = regs.pc;
			break;
		default:
			goto unhandled;
		}
		newpc[nr++] = reg_val;
		/* In case it is a conditional instruction.  */
		newpc[nr++] = pc + 4;
	} else if ((inst & 0xff00001f) == 0xc600000a){
		/* Matching 0xC 0x{4,6} for opcode
		 * and UD == 0x5 == METAG_UNIT_PC
		 *
		 * GETD PC, [A0.r + #S6] or
		 * GETD PC, [A0.r + A0.r]  */
		unit = metag_bu_map[(inst >> 5) & 0x3];
		iov.iov_base = &regs;
		iov.iov_len = sizeof(regs);
		reg = (inst >> 14) & 0x1f;
		imm = (inst << 18) >> 5; /* sign-extend it */
		if (ptrace(PTRACE_GETREGSET, proc->pid,
			   NT_PRSTATUS, (long)&iov))
			goto ptrace_fail;
		reg_val = get_regval_from_unit(unit, reg, &regs) + imm;
		/* See where reg_val actually points to.  */
		newpc[nr++] = ptrace(PTRACE_PEEKTEXT, proc->pid, reg_val, 0);
	} else if (((inst & 0xfe0001e0) == 0x840000a0) || /* ADDcc R, A, R */
		   ((inst & 0xfe00003f) == 0x8600002a) || /* ADD R, A, #X8 */
		   ((inst & 0xfe0001e0) == 0x8c0000a0) || /* SUBcc R, A, R */
		   ((inst & 0xfe00003f) == 0x8e00002a) || /* SUB R, A, #X8 */
		   ((inst & 0xf40001e0) == 0x040000a0) || /* ADDcc R, D, D */
		   ((inst & 0xfe00003f) == 0x0600002a) || /* ADD R, D, #X8 */
		   ((inst & 0xf40001e0) == 0x140000a0) || /* SUBcc R, D, D */
		   ((inst & 0xf600003f) == 0x1600002a)) { /* SUB R, D, #X8 */

		/* bits4-1(Ud) == METAG_UNIT_PC */

		int src1, src2, pc_src1 = 0, pc_src2 = 0, is_aunit = 0;
		int umask = 0, optype = 0;

		/* Look for O2R bit */
		if ((((inst >> 24) & 0x6) == 0x4) && (inst & 0x1))
			goto unhandled;

		iov.iov_base = &regs;
		iov.iov_len = sizeof(regs);
		if (ptrace(PTRACE_GETREGSET, proc->pid,
			   NT_PRSTATUS, (long)&iov))
			goto ptrace_fail;

		/* Figure out unit for source registers based on the opcode.  */
		switch((inst >> 28) & 0xf) {
		case 0: /* ADD<cc> Rx.r, Dx.r, De.r|#X8 */
		case 1: /* SUB<cc> Rx.r, Dx.r, De.r|#X8 */
			unit = METAG_UNIT_D0 + ((inst >> 24) & 0x1);
			is_aunit = 0;
			umask = 0x1f;
			optype = (inst >> 28) & 0x1;
			break;
		case 8:
			unit = METAG_UNIT_A0 + ((inst >> 24) & 0x1);
			is_aunit = 1;
			umask = 0xf;
			optype = (inst >> 27) & 0x1;
			break;
		}

		/* Get pc bits (if any).  */
		if (is_aunit) {
			pc_src1 = (inst >> 18) & 0x1;
			pc_src2 = (inst >> 13) & 0x1;
		}

		/* Determine ADD|SUB format. Immediate or register ?  */
		if ((inst >> 25) & 0x1) { /* ADD|SUB cc PC, X, #imm8 */
			src2 = (inst >> 6) & 0xff; /* so we can share code.  */
			reg = (inst >> 14) & umask;
			if (pc_src1)	/* This can only be true for AU ops.  */
				src1 = regs.pc;
			else		/* This covers both AU an DU ops.  */
				src1 = get_regval_from_unit(unit, reg, &regs);
		} else { /* ADD|SUB cc PC, X, X */
			if (pc_src1)
				src1 = regs.pc;
			else
				src1 = get_regval_from_unit(unit, (inst >> 14)
							    & umask, &regs);
			if (pc_src2)
				src2 = regs.pc;
			else
				src2 = get_regval_from_unit(unit, (inst >> 9)
							    & umask, &regs);
		}

		/* Construct the new PC.  */
		if (optype)
			/* SUB */
			newpc[nr++] = src1 - src2;
		else 	/* ADD */
			newpc[nr++] = src1 + src2;
		/* Conditional instruction so PC may not change.  */
		newpc[nr++] = pc + 4;
	} else {
		newpc[nr++] = pc + 4;
	}

	if (nr <= 0 || nr > 2)
		goto fail;
	if (nr == 2 && newpc[1] == 0)
		goto fail;
	
	return nr;

ptrace_fail:
	fprintf(stderr, "Failed to read the registers pid=%d @ pc=0x%08x\n",
		proc->pid, pc);
	return 0;
unhandled:
	fprintf(stderr, "Unhandled instruction: pc=0x%08x, inst=0x%08x\n",
		pc, inst);
	return 0;
fail:
	fprintf(stderr, "nr=%d pc=0x%08x\n", nr, pc);
	fprintf(stderr, "newpc=0x%08x 0x%08x\n", newpc[0], newpc[1]);
	return 0;

}

enum sw_singlestep_status
arch_sw_singlestep(struct process *proc, struct breakpoint *bp,
		   int (*add_cb)(arch_addr_t, struct sw_singlestep_data *),
		   struct sw_singlestep_data *add_cb_data)
{
	arch_addr_t pc = get_instruction_pointer(proc);
	uint32_t newpcs[2];
	int nr;

	nr = metag_next_pcs(proc, (uint32_t)pc, newpcs);

	while (nr-- > 0) {
		arch_addr_t baddr = (arch_addr_t) newpcs[nr];
		if (dict_find(proc->leader->breakpoints, &baddr) != NULL) {
			fprintf(stderr, "skip %p %p\n", baddr, add_cb_data);
			continue;
		}

		if (add_cb(baddr, add_cb_data) < 0)
			return SWS_FAIL;
	}

	ptrace(PTRACE_SYSCALL, proc->pid, 0, 0);
	return SWS_OK;
}

long
gimme_arg(enum tof type, struct process *proc, int arg_num,
	  struct arg_type_info *info)
{
	long ret;
	struct user_gp_regs regs;
	struct iovec iov;

	/* get GP registers */
	iov.iov_base = &regs;
	iov.iov_len = sizeof(regs);
	if (ptrace(PTRACE_GETREGSET, proc->pid, NT_PRSTATUS, (long)&iov))
		return 0;

	debug(2, "type %d arg %d arg",type, arg_num);
	if (type == LT_TOF_FUNCTION || type == LT_TOF_SYSCALL) {
		if (arg_num < 6) {
			/* Args go backwards starting from D1Ar1 (D1.3) */
			ret = ((unsigned long *)&regs.dx[3][1])[-arg_num];
			debug(2,"ret = %#lx",ret);
			return ret;
		} else {
			return 0;
		}
	}
	if (arg_num >= 0) {
		fprintf(stderr,"args on return?");
	}
	if (type == LT_TOF_FUNCTIONR || type == LT_TOF_SYSCALLR) {
		return regs.dx[0][0]; /* D0Re0 (D0.0) */
	}

	fprintf(stderr, "gimme_arg called with wrong arguments\n");

	return 0;
}
