/*
 * This file is part of ltrace.
 * Copyright (C) 2012, 2014 Petr Machata, Red Hat Inc.
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

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ucontext.h>
#include <stdio.h>

#include "backend.h"
#include "fetch.h"
#include "type.h"
#include "ptrace.h"
#include "proc.h"
#include "value.h"
#include "ltrace-elf.h"

static int allocate_gpr(struct fetch_context *ctx, struct process *proc,
			struct arg_type_info *info, struct value *valuep,
			size_t off, bool is_hfa_type);

/* Floating point registers have the same width on 32-bit as well as
 * 64-bit PPC, but <ucontext.h> presents a different API depending on
 * whether ltrace is PPC32 or PPC64.
 *
 * This is PPC64 definition.  The PPC32 is simply an array of 33
 * doubles, and doesn't contain the terminating pad.  Both seem
 * compatible enough.  */
struct fpregs_t
{
	double fpregs[32];
	double fpscr;
	unsigned int _pad[2];
};

typedef uint32_t gregs32_t[48];
typedef uint64_t gregs64_t[48];

struct fetch_context {
	arch_addr_t stack_pointer;
	int greg;
	int freg;
	bool ret_struct;

	union {
		gregs32_t r32;
		gregs64_t r64;
	} regs;
	struct fpregs_t fpregs;
	int vgreg;
};

static bool
is_eligible_hfa(struct arg_type_info *info,
		struct arg_type_info **hfa_infop, size_t *hfa_countp)
{
	size_t hfa_count;
	struct arg_type_info *hfa_info = type_get_hfa_type(info, &hfa_count);

	if (hfa_info != NULL && hfa_count <= 8
	    && (hfa_info->type == ARGTYPE_FLOAT
		|| hfa_info->type == ARGTYPE_DOUBLE)) {

		if (hfa_infop != NULL)
			*hfa_infop = hfa_info;
		if (hfa_countp != NULL)
			*hfa_countp = hfa_count;
		return true;
	}

	return false;
}

static int
fetch_context_init(struct process *proc, struct fetch_context *context)
{
	context->greg = 3;
	context->freg = 1;

	if (proc->e_machine == EM_PPC)
		context->stack_pointer = proc->stack_pointer + 8;
	else
		context->stack_pointer = proc->stack_pointer
			+ STACK_FRAME_OVERHEAD;

	/* When ltrace is 64-bit, we might use PTRACE_GETREGS to
	 * obtain 64-bit as well as 32-bit registers.  But if we do it
	 * this way, 32-bit ltrace can obtain 64-bit registers.
	 *
	 * XXX this direction is not supported as of this writing, but
	 * should be eventually.  */
	if (proc->e_machine == EM_PPC64) {
		if (ptrace(PTRACE_GETREGS64, proc->pid, 0,
			   &context->regs.r64) < 0)
			return -1;
	} else {
#ifdef __powerpc64__
		if (ptrace(PTRACE_GETREGS, proc->pid, 0,
			  &context->regs.r64) < 0)
			return -1;
		unsigned i;
		for (i = 0; i < sizeof(context->regs.r64)/8; ++i)
			context->regs.r32[i] = context->regs.r64[i];
#else
		if (ptrace(PTRACE_GETREGS, proc->pid, 0,
			  &context->regs.r32) < 0)
			return -1;
#endif
	}

	if (ptrace(PTRACE_GETFPREGS, proc->pid, 0, &context->fpregs) < 0)
		return -1;

	return 0;
}

struct fetch_context *
arch_fetch_arg_init(enum tof type, struct process *proc,
		    struct arg_type_info *ret_info)
{
	struct fetch_context *context = malloc(sizeof(*context));
	if (context == NULL
	    || fetch_context_init(proc, context) < 0) {
		free(context);
		return NULL;
	}

	context->vgreg = context->greg;

	/* Aggregates or unions of any length, and character strings
	 * of length longer than 8 bytes, will be returned in a
	 * storage buffer allocated by the caller. The caller will
	 * pass the address of this buffer as a hidden first argument
	 * in r3, causing the first explicit argument to be passed in
	 * r4.
	 */

	context->ret_struct = false;

	if (ppc64_call_elf_abi == 2) {
		/* With ELFv2 ABI, aggregates that consist
		 * (recursively) only of members of the same
		 * floating-point or vector type, are passed in a
		 * series of floating-point resp. vector registers.
		 * Additionally, when returning any aggregate of up to
		 * 16 bytes, general-purpose registers are used.  */

		if (ret_info->type == ARGTYPE_STRUCT
		    && ! is_eligible_hfa(ret_info, NULL, NULL)
		    && type_sizeof(proc, ret_info) > 16) {

			context->ret_struct = true;
			context->greg++;
			context->stack_pointer += 8;
		}

	} else if (ret_info->type == ARGTYPE_STRUCT) {
		context->ret_struct = true;
		context->greg++;
	}

	return context;
}

struct fetch_context *
arch_fetch_arg_clone(struct process *proc,
		     struct fetch_context *context)
{
	struct fetch_context *clone = malloc(sizeof(*context));
	if (clone == NULL)
		return NULL;
	*clone = *context;
	return clone;
}

static int
allocate_stack_slot(struct fetch_context *ctx, struct process *proc,
		    struct arg_type_info *info, struct value *valuep,
		    bool is_hfa_type)
{
	size_t sz = type_sizeof(proc, info);
	if (sz == (size_t)-1)
		return -1;

	size_t a = type_alignof(proc, info);
	size_t off = 0;
	if (proc->e_machine == EM_PPC && a < 4) {
		a = 4;
	} else if (ppc64_call_elf_abi == 2) {
		if (proc->e_machine == EM_PPC64 && sz == 4 && is_hfa_type) {
			a = 4;
		} else
			a = 8;
	} else if (proc->e_machine == EM_PPC64 && a < 8) {
		a = 8;
	}

	/* XXX Remove the two double casts when arch_addr_t
	 * becomes integral type.  */
	uintptr_t tmp = align((uint64_t)(uintptr_t)ctx->stack_pointer, a);
	ctx->stack_pointer = (arch_addr_t)tmp;

	if (valuep != NULL)
		value_in_inferior(valuep, ctx->stack_pointer + off);
	ctx->stack_pointer += a;

	return 0;
}

static uint64_t
read_gpr(struct fetch_context *ctx, struct process *proc, int reg_num)
{
	if (proc->e_machine == EM_PPC)
		return ctx->regs.r32[reg_num];
	else
		return ctx->regs.r64[reg_num];
}

/* The support for little endian PowerPC is in upstream Linux and BFD,
 * and Unix-like Solaris, which we might well support at some point,
 * runs PowerPC in little endian as well.  This code moves SZ-sized
 * value to the beginning of W-sized BUF regardless of
 * endian.  */
static void
align_small_int(unsigned char *buf, size_t w, size_t sz)
{
	assert(w == 4 || w == 8);
	union {
		uint64_t i64;
		uint32_t i32;
		uint16_t i16;
		uint8_t i8;
		char buf[0];
	} u;
	memcpy(u.buf, buf, w);
	if (w == 4)
		u.i64 = u.i32;

	switch (sz) {
	case 1:
		u.i8 = u.i64;
		break;
	case 2:
		u.i16 = u.i64;
		break;
	case 4:
		u.i32 = u.i64;
	case 8:
		break;
	}

	memcpy(buf, u.buf, sz);
}

static int
allocate_gpr(struct fetch_context *ctx, struct process *proc,
	     struct arg_type_info *info, struct value *valuep,
	     size_t off, bool is_hfa_type)
{
	if (ctx->greg > 10)
		return allocate_stack_slot(ctx, proc, info, valuep, is_hfa_type);

	int reg_num = ctx->greg;

	size_t sz = type_sizeof(proc, info);
	if (sz == (size_t)-1)
		return -1;
	assert(sz == 1 || sz == 2 || sz == 4 || sz == 8);

	if (ppc64_call_elf_abi == 2) {
		/* Consume the stack slot corresponding to this arg.  */
		if ((sz + off) >= 8)
			ctx->greg++;

		if (is_hfa_type)
			ctx->stack_pointer += sz;
		else
			ctx->stack_pointer += 8;
	} else {
		ctx->greg++;
	}

	if (valuep == NULL)
		return 0;

	if (value_reserve(valuep, sz) == NULL)
		return -1;

	union {
		uint64_t i64;
		unsigned char buf[0];
	} u;

	u.i64 = read_gpr(ctx, proc, reg_num);
	if (proc->e_machine == EM_PPC)
		align_small_int(u.buf, 8, sz);
	memcpy(value_get_raw_data(valuep), u.buf + off, sz);
	return 0;
}

static int
allocate_float(struct fetch_context *ctx, struct process *proc,
	       struct arg_type_info *info, struct value *valuep,
	       size_t off, bool is_hfa_type)
{
	int pool = proc->e_machine == EM_PPC64 ? 13 : 8;
	if (ctx->freg <= pool) {
		union {
			double d;
			float f;
			char buf[0];
		} u = { .d = ctx->fpregs.fpregs[ctx->freg] };

		ctx->freg++;

		if (!is_hfa_type)
			ctx->vgreg++;

		if (proc->e_machine == EM_PPC64)
			allocate_gpr(ctx, proc, info, NULL, off, is_hfa_type);

		size_t sz = sizeof(double);
		if (info->type == ARGTYPE_FLOAT) {
			sz = sizeof(float);
			u.f = (float)u.d;
		}

		if (value_reserve(valuep, sz) == NULL)
			return -1;

		memcpy(value_get_raw_data(valuep), u.buf, sz);
		return 0;
	}
	return allocate_stack_slot(ctx, proc, info, valuep, is_hfa_type);
}

static int
allocate_hfa(struct fetch_context *ctx, struct process *proc,
	     struct arg_type_info *info, struct value *valuep,
	     enum arg_type hfa_type, size_t hfa_count)
{
	size_t sz = type_sizeof(proc, info);
	if (sz == (size_t)-1)
		return -1;

	/* There are two changes regarding structure return types:
	 * * heterogeneous float/vector structs are returned in
	 *   (multiple) FP/vector registers, instead of via implicit
	 *   reference.
	 * * small structs (up to 16 bytes) are return in one or two
	 *   GPRs, instead of via implicit reference.
	 *
	 * Other structures (larger than 16 bytes, not heterogeneous)
	 * are still returned via implicit reference (i.e. a pointer
	 * to memory where to return the struct being passed in r3).
	 * Of course, whether or not an implicit reference pointer is
	 * present will shift the remaining arguments, so you need to
	 * get this right for ELFv2 in order to get the arguments
	 * correct.
	 *
	 * If an actual parameter is known to correspond to an HFA
	 * formal parameter, each element is passed in the next
	 * available floating-point argument register starting at fp1
	 * until the fp13. The remaining elements of the aggregate are
	 * passed on the stack.
	 */
	size_t slot_off = 0;

	unsigned char *buf = value_reserve(valuep, sz);
	if (buf == NULL)
		return -1;

	struct arg_type_info *hfa_info = type_get_simple(hfa_type);
	size_t hfa_sz = type_sizeof(proc, hfa_info);

	while (hfa_count > 0 && ctx->freg <= 13) {
		struct value tmp;
		value_init(&tmp, proc, NULL, hfa_info, 0);
		int rc = allocate_float(ctx, proc, hfa_info,
					&tmp, slot_off, true);
		if (rc == 0)
			memcpy(buf, value_get_data(&tmp, NULL), hfa_sz);
		value_destroy(&tmp);

		if (rc < 0)
			return -1;

		slot_off += hfa_sz;
		buf += hfa_sz;
		hfa_count--;
		if (slot_off == 8) {
			slot_off = 0;
			ctx->vgreg++;
		}
	}
	if (hfa_count == 0)
		return 0;

	/* if no remaining FP, GPR corresponding to slot is used
	 * Mostly it is in part of r10.  */
	if (ctx->vgreg == 10) {
		while (ctx->vgreg <= 10) {
			struct value tmp;
			value_init(&tmp, proc, NULL, hfa_info, 0);
			union {
				uint64_t i64;
				unsigned char buf[0];
			} u;

			u.i64 = read_gpr(ctx, proc, ctx->vgreg);

			memcpy(buf, u.buf + slot_off, hfa_sz);
			slot_off += hfa_sz;
			buf += hfa_sz;
			hfa_count--;
			ctx->stack_pointer += hfa_sz;
			if (slot_off >= 8 ) {
				slot_off = 0;
				ctx->vgreg++;
			}
			value_destroy(&tmp);
		}
	}

	/* Remaining values are on stack */
	while (hfa_count > 0) {
		struct value tmp;
		value_init(&tmp, proc, NULL, hfa_info, 0);

		value_in_inferior(&tmp, ctx->stack_pointer);
		memcpy(buf, value_get_data(&tmp, NULL), hfa_sz);
		ctx->stack_pointer += hfa_sz;
		buf += hfa_sz;
		hfa_count--;
	}
	return 0;
}

static int
allocate_argument(struct fetch_context *ctx, struct process *proc,
		  struct arg_type_info *info, struct value *valuep)
{
	/* Floating point types and void are handled specially.  */
	switch (info->type) {
	case ARGTYPE_VOID:
		value_set_word(valuep, 0);
		return 0;

	case ARGTYPE_FLOAT:
	case ARGTYPE_DOUBLE:
		return allocate_float(ctx, proc, info, valuep,
				      8 - type_sizeof(proc,info), false);

	case ARGTYPE_STRUCT:
		if (proc->e_machine == EM_PPC) {
			if (value_pass_by_reference(valuep) < 0)
				return -1;
		} else if (ppc64_call_elf_abi == 2) {
			struct arg_type_info *hfa_info;
			size_t hfa_count;
			if (is_eligible_hfa(info, &hfa_info, &hfa_count)) {
				return allocate_hfa(ctx, proc, info, valuep,
						hfa_info->type, hfa_count);
			}
		} else {
			/* PPC64: Fixed size aggregates and unions passed by
			 * value are mapped to as many doublewords of the
			 * parameter save area as the value uses in memory.
			 * [...] The first eight doublewords mapped to the
			 * parameter save area correspond to the registers r3
			 * through r10.  */
		}
		/* fall through */
	case ARGTYPE_CHAR:
	case ARGTYPE_SHORT:
	case ARGTYPE_USHORT:
	case ARGTYPE_INT:
	case ARGTYPE_UINT:
	case ARGTYPE_LONG:
	case ARGTYPE_ULONG:
	case ARGTYPE_POINTER:
		break;

	case ARGTYPE_ARRAY:
		/* Arrays decay into pointers.  XXX Fortran?  */
	default:
		assert(info->type != info->type);
		abort();
	}

	unsigned width = proc->e_machine == EM_PPC64 ? 8 : 4;

	/* For other cases (integral types and aggregates), read the
	 * eightbytes comprising the data.  */
	size_t sz = type_sizeof(proc, valuep->type);
	if (sz == (size_t)-1)
		return -1;

	size_t slots = (sz + width - 1) / width;  /* Round up.  */
	unsigned char *buf = value_reserve(valuep, slots * width);
	if (buf == NULL)
		return -1;
	struct arg_type_info *long_info = type_get_simple(ARGTYPE_LONG);

	unsigned char *ptr = buf;
	while (slots-- > 0) {
		struct value val;
		value_init(&val, proc, NULL, long_info, 0);

		/* Floating point registers [...] are used [...] to
		   pass [...] one member aggregates passed by value
		   containing a floating point value[.]  Note that for
		   one member aggregates, "containing" extends to
		   aggregates within aggregates ad infinitum.  */
		int rc;
		struct arg_type_info *fp_info
			= type_get_fp_equivalent(valuep->type);
		if (fp_info != NULL)
			rc = allocate_float(ctx, proc, fp_info, &val,
					8-type_sizeof(proc,info), false);
		else
			rc = allocate_gpr(ctx, proc, long_info, &val,
					0, false);

		if (rc >= 0) {
			memcpy(ptr, value_get_data(&val, NULL), width);
			ptr += width;
		}
		value_destroy(&val);

		/* Bail out if we failed or if we are dealing with
		 * FP-equivalent.  Those don't need the adjustments
		 * made below.  */
		if (rc < 0 || fp_info != NULL)
			return rc;
	}

#ifndef __LITTLE_ENDIAN__
	/* Small values need post-processing.  */
	if (sz < width) {
		switch (info->type) {
		default:
			abort();

		/* Simple integer types (char, short, int, long, enum)
		 * are mapped to a single doubleword. Values shorter
		 * than a doubleword are sign or zero extended as
		 * necessary.  */
		case ARGTYPE_CHAR:
		case ARGTYPE_SHORT:
		case ARGTYPE_INT:
		case ARGTYPE_USHORT:
		case ARGTYPE_UINT:
			align_small_int(buf, width, sz);
			break;

		/* Single precision floating point values are mapped
		 * to the second word in a single doubleword.
		 *
		 * An aggregate or union smaller than one doubleword
		 * in size is padded so that it appears in the least
		 * significant bits of the doubleword.  */
		case ARGTYPE_FLOAT:
		case ARGTYPE_ARRAY:
		case ARGTYPE_STRUCT:
			memmove(buf, buf + width - sz, sz);
			break;
		}
	}
#endif

	return 0;
}

int
arch_fetch_arg_next(struct fetch_context *ctx, enum tof type,
		    struct process *proc,
		    struct arg_type_info *info, struct value *valuep)
{
	return allocate_argument(ctx, proc, info, valuep);
}

int
arch_fetch_retval(struct fetch_context *ctx, enum tof type,
		  struct process *proc, struct arg_type_info *info,
		  struct value *valuep)
{
	if (fetch_context_init(proc, ctx) < 0)
		return -1;

	if (ctx->ret_struct) {
		assert(info->type == ARGTYPE_STRUCT);

		uint64_t addr = read_gpr(ctx, proc, 3);
		value_init(valuep, proc, NULL, info, 0);

		valuep->where = VAL_LOC_INFERIOR;
		/* XXX Remove the double cast when arch_addr_t
		 * becomes integral type. */
		valuep->u.address = (arch_addr_t)(uintptr_t)addr;
		return 0;
	}

	return allocate_argument(ctx, proc, info, valuep);
}

void
arch_fetch_arg_done(struct fetch_context *context)
{
	free(context);
}
