/*
 * This file is part of ltrace.
 * Copyright (C) 2012, 2013 Petr Machata, Red Hat Inc.
 * Copyright (C) 1998,2004,2008,2009 Juan Cespedes
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

#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/ptrace.h>
#include <asm/ptrace.h>

#include "bits.h"
#include "common.h"
#include "proc.h"
#include "output.h"
#include "ptrace.h"
#include "regs.h"
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
	proc_archdep *a;

	if (!proc->arch_ptr)
		proc->arch_ptr = (void *)malloc(sizeof(proc_archdep));
	a = (proc_archdep *) (proc->arch_ptr);
	a->valid = (ptrace(PTRACE_GETREGS, proc->pid, 0, &a->regs) >= 0);
}

/* Returns 0 if not a syscall,
 *         1 if syscall entry, 2 if syscall exit,
 *         3 if arch-specific syscall entry, 4 if arch-specific syscall exit,
 *         -1 on error.
 */
int
syscall_p(struct process *proc, int status, int *sysnum)
{
	if (WIFSTOPPED(status)
	    && WSTOPSIG(status) == (SIGTRAP | proc->tracesysgood)) {
		uint32_t pc, ip;
		if (arm_get_register(proc, ARM_REG_PC, &pc) < 0
		    || arm_get_register(proc, ARM_REG_IP, &ip) < 0)
			return -1;

		pc = pc - 4;

		/* fetch the SWI instruction */
		unsigned insn = ptrace(PTRACE_PEEKTEXT, proc->pid,
				       (void *)pc, 0);

		if (insn == 0xef000000 || insn == 0x0f000000
		    || (insn & 0xffff0000) == 0xdf000000) {
			/* EABI syscall */
			uint32_t r7;
			if (arm_get_register(proc, ARM_REG_R7, &r7) < 0)
				return -1;
			*sysnum = r7;
		} else if ((insn & 0xfff00000) == 0xef900000) {
			/* old ABI syscall */
			*sysnum = insn & 0xfffff;
		} else {
			/* TODO: handle swi<cond> variations */
			/* one possible reason for getting in here is that we
			 * are coming from a signal handler, so the current
			 * PC does not point to the instruction just after the
			 * "swi" one. */
			output_line(proc, "unexpected instruction 0x%x at %p",
				    insn, pc);
			return 0;
		}
		if ((*sysnum & 0xf0000) == 0xf0000) {
			/* arch-specific syscall */
			*sysnum &= ~0xf0000;
			return ip ? 4 : 3;
		}
		/* ARM syscall convention: on syscall entry, ip is zero;
		 * on syscall exit, ip is non-zero */
		return ip ? 2 : 1;
	}
	return 0;
}

static arch_addr_t
arm_branch_dest(const arch_addr_t pc, const uint32_t insn)
{
	/* Bits 0-23 are signed immediate value.  */
	return pc + ((((insn & 0xffffff) ^ 0x800000) - 0x800000) << 2) + 8;
}

/* Addresses for calling Thumb functions have the bit 0 set.
   Here are some macros to test, set, or clear bit 0 of addresses.  */
/* XXX double cast */
#define IS_THUMB_ADDR(addr)	((uintptr_t)(addr) & 1)
#define MAKE_THUMB_ADDR(addr)	((arch_addr_t)((uintptr_t)(addr) | 1))
#define UNMAKE_THUMB_ADDR(addr) ((arch_addr_t)((uintptr_t)(addr) & ~1))

enum {
	COND_ALWAYS = 0xe,
	COND_NV = 0xf,
	FLAG_C = 0x20000000,
};

static int
arm_get_next_pcs(struct process *proc,
		 const arch_addr_t pc, arch_addr_t next_pcs[2])
{
	uint32_t this_instr;
	uint32_t status;
	if (proc_read_32(proc, pc, &this_instr) < 0
	    || arm_get_register(proc, ARM_REG_CPSR, &status) < 0)
		return -1;

	/* In theory, we sometimes don't even need to add any
	 * breakpoints at all.  If the conditional bits of the
	 * instruction indicate that it should not be taken, then we
	 * can just skip it altogether without bothering.  We could
	 * also emulate the instruction under the breakpoint.
	 *
	 * Here, we make it as simple as possible (though We Accept
	 * Patches).  */
	int nr = 0;

	/* ARM can branch either relatively by using a branch
	 * instruction, or absolutely, by doing arbitrary arithmetic
	 * with PC as the destination.  */
	const unsigned cond = BITS(this_instr, 28, 31);
	const unsigned opcode = BITS(this_instr, 24, 27);

	if (cond == COND_NV)
		switch (opcode) {
			arch_addr_t addr;
		case 0xa:
		case 0xb:
			/* Branch with Link and change to Thumb.  */
			/* XXX double cast.  */
			addr = (arch_addr_t)
				((uint32_t)arm_branch_dest(pc, this_instr)
				 | (((this_instr >> 24) & 0x1) << 1));
			next_pcs[nr++] = MAKE_THUMB_ADDR(addr);
			break;
		}
	else
		switch (opcode) {
			uint32_t operand1, operand2, result = 0;
		case 0x0:
		case 0x1:			/* data processing */
		case 0x2:
		case 0x3:
			if (BITS(this_instr, 12, 15) != ARM_REG_PC)
				break;

			if (BITS(this_instr, 22, 25) == 0
			    && BITS(this_instr, 4, 7) == 9) {	/* multiply */
			invalid:
				fprintf(stderr,
				"Invalid update to pc in instruction.\n");
				break;
			}

			/* BX <reg>, BLX <reg> */
			if (BITS(this_instr, 4, 27) == 0x12fff1
			    || BITS(this_instr, 4, 27) == 0x12fff3) {
				enum arm_register reg = BITS(this_instr, 0, 3);
				/* XXX double cast: no need to go
				 * through tmp.  */
				uint32_t tmp;
				if (arm_get_register_offpc(proc, reg, &tmp) < 0)
					return -1;
				next_pcs[nr++] = (arch_addr_t)tmp;
				return 0;
			}

			/* Multiply into PC.  */
			if (arm_get_register_offpc
			    (proc, BITS(this_instr, 16, 19), &operand1) < 0)
				return -1;

			int c = (status & FLAG_C) ? 1 : 0;
			if (BIT(this_instr, 25)) {
				uint32_t immval = BITS(this_instr, 0, 7);
				uint32_t rotate = 2 * BITS(this_instr, 8, 11);
				operand2 = (((immval >> rotate)
					     | (immval << (32 - rotate)))
					    & 0xffffffff);
			} else {
				/* operand 2 is a shifted register.  */
				if (arm_get_shifted_register
				    (proc, this_instr, c, pc, &operand2) < 0)
					return -1;
			}

			switch (BITS(this_instr, 21, 24)) {
			case 0x0:	/*and */
				result = operand1 & operand2;
				break;

			case 0x1:	/*eor */
				result = operand1 ^ operand2;
				break;

			case 0x2:	/*sub */
				result = operand1 - operand2;
				break;

			case 0x3:	/*rsb */
				result = operand2 - operand1;
				break;

			case 0x4:	/*add */
				result = operand1 + operand2;
				break;

			case 0x5:	/*adc */
				result = operand1 + operand2 + c;
				break;

			case 0x6:	/*sbc */
				result = operand1 - operand2 + c;
				break;

			case 0x7:	/*rsc */
				result = operand2 - operand1 + c;
				break;

			case 0x8:
			case 0x9:
			case 0xa:
			case 0xb:	/* tst, teq, cmp, cmn */
				/* Only take the default branch.  */
				result = 0;
				break;

			case 0xc:	/*orr */
				result = operand1 | operand2;
				break;

			case 0xd:	/*mov */
				/* Always step into a function.  */
				result = operand2;
				break;

			case 0xe:	/*bic */
				result = operand1 & ~operand2;
				break;

			case 0xf:	/*mvn */
				result = ~operand2;
				break;
			}

			/* XXX double cast */
			next_pcs[nr++] = (arch_addr_t)result;
			break;

		case 0x4:
		case 0x5:		/* data transfer */
		case 0x6:
		case 0x7:
			/* Ignore if insn isn't load or Rn not PC.  */
			if (!BIT(this_instr, 20)
			    || BITS(this_instr, 12, 15) != ARM_REG_PC)
				break;

			if (BIT(this_instr, 22))
				goto invalid;

			/* byte write to PC */
			uint32_t base;
			if (arm_get_register_offpc
			    (proc, BITS(this_instr, 16, 19), &base) < 0)
				return -1;

			if (BIT(this_instr, 24)) {
				/* pre-indexed */
				int c = (status & FLAG_C) ? 1 : 0;
				uint32_t offset;
				if (BIT(this_instr, 25)) {
					if (arm_get_shifted_register
					    (proc, this_instr, c,
					     pc, &offset) < 0)
						return -1;
				} else {
					offset = BITS(this_instr, 0, 11);
				}

				if (BIT(this_instr, 23))
					base += offset;
				else
					base -= offset;
			}

			/* XXX two double casts.  */
			uint32_t next;
			if (proc_read_32(proc, (arch_addr_t)base, &next) < 0)
				return -1;
			next_pcs[nr++] = (arch_addr_t)next;
			break;

		case 0x8:
		case 0x9:		/* block transfer */
			if (!BIT(this_instr, 20))
				break;
			/* LDM */
			if (BIT(this_instr, 15)) {
				/* Loading pc.  */
				int offset = 0;
				enum arm_register rn = BITS(this_instr, 16, 19);
				uint32_t rn_val;
				if (arm_get_register(proc, rn, &rn_val) < 0)
					return -1;

				int pre = BIT(this_instr, 24);
				if (BIT(this_instr, 23)) {
					/* Bit U = up.  */
					unsigned reglist
						= BITS(this_instr, 0, 14);
					offset = bitcount(reglist) * 4;
					if (pre)
						offset += 4;
				} else if (pre) {
					offset = -4;
				}

				/* XXX double cast.  */
				arch_addr_t addr
					= (arch_addr_t)(rn_val + offset);
				uint32_t next;
				if (proc_read_32(proc, addr, &next) < 0)
					return -1;
				next_pcs[nr++] = (arch_addr_t)next;
			}
			break;

		case 0xb:		/* branch & link */
		case 0xa:		/* branch */
			next_pcs[nr++] = arm_branch_dest(pc, this_instr);
			break;

		case 0xc:
		case 0xd:
		case 0xe:		/* coproc ops */
		case 0xf:		/* SWI */
			break;
		}

	/* Otherwise take the next instruction.  */
	if (cond != COND_ALWAYS || nr == 0)
		next_pcs[nr++] = pc + 4;
	return 0;
}

/* Return the size in bytes of the complete Thumb instruction whose
 * first halfword is INST1.  */

static int
thumb_insn_size (unsigned short inst1)
{
  if ((inst1 & 0xe000) == 0xe000 && (inst1 & 0x1800) != 0)
	  return 4;
  else
	  return 2;
}

static int
thumb_get_next_pcs(struct process *proc,
		   const arch_addr_t pc, arch_addr_t next_pcs[2])
{
	uint16_t inst1;
	uint32_t status;
	if (proc_read_16(proc, pc, &inst1) < 0
	    || arm_get_register(proc, ARM_REG_CPSR, &status) < 0)
		return -1;

	int nr = 0;

	/* We currently ignore Thumb-2 conditional execution support
	 * (the IT instruction).  No branches are allowed in IT block,
	 * and it's not legal to jump in the middle of it, so unless
	 * we need to singlestep through large swaths of code, which
	 * we currently don't, we can ignore them.  */

	if ((inst1 & 0xff00) == 0xbd00)	{ /* pop {rlist, pc} */
		/* Fetch the saved PC from the stack.  It's stored
		 * above all of the other registers.  */
		const unsigned offset = bitcount(BITS(inst1, 0, 7)) * 4;
		uint32_t sp;
		uint32_t next;
		/* XXX two double casts */
		if (arm_get_register(proc, ARM_REG_SP, &sp) < 0
		    || proc_read_32(proc, (arch_addr_t)(sp + offset),
				    &next) < 0)
			return -1;
		next_pcs[nr++] = (arch_addr_t)next;
	} else if ((inst1 & 0xf000) == 0xd000) { /* conditional branch */
		const unsigned long cond = BITS(inst1, 8, 11);
		if (cond != 0x0f) { /* SWI */
			next_pcs[nr++] = pc + (SBITS(inst1, 0, 7) << 1);
			if (cond == COND_ALWAYS)
				return 0;
		}
	} else if ((inst1 & 0xf800) == 0xe000) { /* unconditional branch */
		next_pcs[nr++] = pc + (SBITS(inst1, 0, 10) << 1);
	} else if (thumb_insn_size(inst1) == 4) { /* 32-bit instruction */
		unsigned short inst2;
		if (proc_read_16(proc, pc + 2, &inst2) < 0)
			return -1;

		if ((inst1 & 0xf800) == 0xf000 && (inst2 & 0x8000) == 0x8000) {
			/* Branches and miscellaneous control instructions.  */

			if ((inst2 & 0x1000) != 0
			    || (inst2 & 0xd001) == 0xc000) {
				/* B, BL, BLX.  */

				const int imm1 = SBITS(inst1, 0, 10);
				const unsigned imm2 = BITS(inst2, 0, 10);
				const unsigned j1 = BIT(inst2, 13);
				const unsigned j2 = BIT(inst2, 11);

				int32_t offset
					= ((imm1 << 12) + (imm2 << 1));
				offset ^= ((!j2) << 22) | ((!j1) << 23);

				/* XXX double cast */
				uint32_t next = (uint32_t)(pc + offset);
				/* For BLX make sure to clear the low bits.  */
				if (BIT(inst2, 12) == 0)
					next = next & 0xfffffffc;
				/* XXX double cast */
				next_pcs[nr++] = (arch_addr_t)next;
				return 0;
			} else if (inst1 == 0xf3de
				   && (inst2 & 0xff00) == 0x3f00) {
				/* SUBS PC, LR, #imm8.  */
				uint32_t next;
				if (arm_get_register(proc, ARM_REG_LR,
						     &next) < 0)
					return -1;
				next -= inst2 & 0x00ff;
				/* XXX double cast */
				next_pcs[nr++] = (arch_addr_t)next;
				return 0;
			} else if ((inst2 & 0xd000) == 0x8000
				   && (inst1 & 0x0380) != 0x0380) {
				/* Conditional branch.  */
				const int sign = SBITS(inst1, 10, 10);
				const unsigned imm1 = BITS(inst1, 0, 5);
				const unsigned imm2 = BITS(inst2, 0, 10);
				const unsigned j1 = BIT(inst2, 13);
				const unsigned j2 = BIT(inst2, 11);

				int32_t offset = (sign << 20)
					+ (j2 << 19) + (j1 << 18);
				offset += (imm1 << 12) + (imm2 << 1);
				next_pcs[nr++] = pc + offset;
				if (BITS(inst1, 6, 9) == COND_ALWAYS)
					return 0;
			}
		} else if ((inst1 & 0xfe50) == 0xe810) {
			int load_pc = 1;
			int offset;
			const enum arm_register rn = BITS(inst1, 0, 3);

			if (BIT(inst1, 7) && !BIT(inst1, 8)) {
				/* LDMIA or POP */
				if (!BIT(inst2, 15))
					load_pc = 0;
				offset = bitcount(inst2) * 4 - 4;
			} else if (!BIT(inst1, 7) && BIT(inst1, 8)) {
				/* LDMDB */
				if (!BIT(inst2, 15))
					load_pc = 0;
				offset = -4;
			} else if (BIT(inst1, 7) && BIT(inst1, 8)) {
				/* RFEIA */
				offset = 0;
			} else if (!BIT(inst1, 7) && !BIT(inst1, 8)) {
				/* RFEDB */
				offset = -8;
			} else {
				load_pc = 0;
			}

			if (load_pc) {
				uint32_t addr;
				if (arm_get_register(proc, rn, &addr) < 0)
					return -1;
				arch_addr_t a = (arch_addr_t)(addr + offset);
				uint32_t next;
				if (proc_read_32(proc, a, &next) < 0)
					return -1;
				/* XXX double cast */
				next_pcs[nr++] = (arch_addr_t)next;
			}
		} else if ((inst1 & 0xffef) == 0xea4f
			   && (inst2 & 0xfff0) == 0x0f00) {
			/* MOV PC or MOVS PC.  */
			const enum arm_register rn = BITS(inst2, 0, 3);
			uint32_t next;
			if (arm_get_register(proc, rn, &next) < 0)
				return -1;
			/* XXX double cast */
			next_pcs[nr++] = (arch_addr_t)next;
		} else if ((inst1 & 0xff70) == 0xf850
			   && (inst2 & 0xf000) == 0xf000) {
			/* LDR PC.  */
			const enum arm_register rn = BITS(inst1, 0, 3);
			uint32_t base;
			if (arm_get_register(proc, rn, &base) < 0)
				return -1;

			int load_pc = 1;
			if (rn == ARM_REG_PC) {
				base = (base + 4) & ~(uint32_t)0x3;
				if (BIT(inst1, 7))
					base += BITS(inst2, 0, 11);
				else
					base -= BITS(inst2, 0, 11);
			} else if (BIT(inst1, 7)) {
				base += BITS(inst2, 0, 11);
			} else if (BIT(inst2, 11)) {
				if (BIT(inst2, 10)) {
					if (BIT(inst2, 9))
						base += BITS(inst2, 0, 7);
					else
						base -= BITS(inst2, 0, 7);
				}
			} else if ((inst2 & 0x0fc0) == 0x0000) {
				const int shift = BITS(inst2, 4, 5);
				const enum arm_register rm = BITS(inst2, 0, 3);
				uint32_t v;
				if (arm_get_register(proc, rm, &v) < 0)
					return -1;
				base += v << shift;
			} else {
				/* Reserved.  */
				load_pc = 0;
			}

			if (load_pc) {
				/* xxx double casts */
				uint32_t next;
				if (proc_read_32(proc,
						 (arch_addr_t)base, &next) < 0)
					return -1;
				next_pcs[nr++] = (arch_addr_t)next;
			}
		} else if ((inst1 & 0xfff0) == 0xe8d0
			   && (inst2 & 0xfff0) == 0xf000) {
			/* TBB.  */
			const enum arm_register tbl_reg = BITS(inst1, 0, 3);
			const enum arm_register off_reg = BITS(inst2, 0, 3);

			uint32_t table;
			if (tbl_reg == ARM_REG_PC)
				/* Regcache copy of PC isn't right yet.  */
				/* XXX double cast */
				table = (uint32_t)pc + 4;
			else if (arm_get_register(proc, tbl_reg, &table) < 0)
				return -1;

			uint32_t offset;
			if (arm_get_register(proc, off_reg, &offset) < 0)
				return -1;

			table += offset;
			uint8_t length;
			/* XXX double cast */
			if (proc_read_8(proc, (arch_addr_t)table, &length) < 0)
				return -1;

			next_pcs[nr++] = pc + 2 * length;

		} else if ((inst1 & 0xfff0) == 0xe8d0
			   && (inst2 & 0xfff0) == 0xf010) {
			/* TBH.  */
			const enum arm_register tbl_reg = BITS(inst1, 0, 3);
			const enum arm_register off_reg = BITS(inst2, 0, 3);

			uint32_t table;
			if (tbl_reg == ARM_REG_PC)
				/* Regcache copy of PC isn't right yet.  */
				/* XXX double cast */
				table = (uint32_t)pc + 4;
			else if (arm_get_register(proc, tbl_reg, &table) < 0)
				return -1;

			uint32_t offset;
			if (arm_get_register(proc, off_reg, &offset) < 0)
				return -1;

			table += 2 * offset;
			uint16_t length;
			/* XXX double cast */
			if (proc_read_16(proc, (arch_addr_t)table, &length) < 0)
				return -1;

			next_pcs[nr++] = pc + 2 * length;
		}
	}


	/* Otherwise take the next instruction.  */
	if (nr == 0)
		next_pcs[nr++] = pc + thumb_insn_size(inst1);
	return 0;
}

enum sw_singlestep_status
arch_sw_singlestep(struct process *proc, struct breakpoint *sbp,
		   int (*add_cb)(arch_addr_t, struct sw_singlestep_data *),
		   struct sw_singlestep_data *add_cb_data)
{
	const arch_addr_t pc = get_instruction_pointer(proc);

	uint32_t cpsr;
	if (arm_get_register(proc, ARM_REG_CPSR, &cpsr) < 0)
		return SWS_FAIL;

	const unsigned thumb_p = BIT(cpsr, 5);
	arch_addr_t next_pcs[2] = {};
	if ((thumb_p ? &thumb_get_next_pcs
	     : &arm_get_next_pcs)(proc, pc, next_pcs) < 0)
		return SWS_FAIL;

	int i;
	for (i = 0; i < 2; ++i) {
		/* XXX double cast.  */
		arch_addr_t target
			= (arch_addr_t)(((uintptr_t)next_pcs[i]) | thumb_p);
		if (next_pcs[i] != 0 && add_cb(target, add_cb_data) < 0)
			return SWS_FAIL;
	}

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
	case ARGTYPE_LONG:
	case ARGTYPE_ULONG:
	case ARGTYPE_POINTER:
		return 4;

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
	}
}

size_t
arch_type_alignof(struct process *proc, struct arg_type_info *info)
{
	return arch_type_sizeof(proc, info);
}
