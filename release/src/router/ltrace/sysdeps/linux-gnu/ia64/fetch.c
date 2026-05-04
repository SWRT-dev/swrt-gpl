/*
 * This file is part of ltrace.
 * Copyright (C) 2012,2013 Petr Machata, Red Hat Inc.
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

#include <stdlib.h>
#include <assert.h>
#include <sys/rse.h>
#include <ptrace.h>
#include <string.h>
#include <errno.h>

#include "backend.h"
#include "fetch.h"
#include "type.h"
#include "proc.h"
#include "value.h"

struct fetch_context {
	arch_addr_t stack_pointer;
	struct pt_all_user_regs regs;
	enum param_pack_flavor ppflavor;

	/* Return values larger than 256 bits (except HFAs of up to 8
	 * elements) are returned in a buffer allocated by the
	 * caller. A pointer to the buffer is passed to the called
	 * procedure in r8. This register is not guaranteed to be
	 * preserved by the called procedure.  */
	unsigned long r8;

	int slot_n;
	int flt;
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

static int
fetch_context_init(struct process *proc, struct fetch_context *context)
{
	context->slot_n = 0;
	context->flt = 8;
	if (ptrace(PTRACE_GETREGS, proc->pid, 0, &context->regs) < 0)
		return -1;
	context->stack_pointer = (void *)(context->regs.gr[12] + 16);
	context->ppflavor = PARAM_PACK_ARGS;

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
	context->r8 = context->regs.gr[8];

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

int
allocate_stack_slot(struct fetch_context *ctx, struct process *proc,
		    struct arg_type_info *info, struct value *valuep)
{
	size_t al = type_alignof(proc, info);
	size_t sz = type_sizeof(proc, info);
	if (al == (size_t)-1 || sz == (size_t)-1)
		return -1;

	errno = 0;
	long value = ptrace(PTRACE_PEEKDATA, proc->pid, ctx->stack_pointer, 0);
	if (value == -1 && errno != 0)
		return -1;
	ctx->stack_pointer += 8;
	value_set_word(valuep, value);

	return 0;
}

static int
allocate_reg(struct fetch_context *ctx, struct process *proc,
	     struct arg_type_info *info, struct value *valuep)
{
	if (ctx->slot_n >= 8)
		return allocate_stack_slot(ctx, proc, info, valuep);

	int reg_num = ctx->slot_n++;
	if (ctx->slot_n == 8)
		ctx->flt = 16;
	if (valuep == NULL)
		return 0;

	/* This would normally be brought over from asm/ptrace.h, but
	 * when we do, we get namespace conflicts between asm/fpu.h
	 * and libunwind.  */
	enum { PT_AUR_BSP = 17 };

	union cfm_t cfm = { .value = ctx->regs.cfm };
	unsigned long *bsp = (unsigned long *)ctx->regs.ar[PT_AUR_BSP];
	unsigned long idx = -cfm.cfm.sof + reg_num;
	unsigned long *ptr = ia64_rse_skip_regs(bsp, idx);
	errno = 0;
	long ret = ptrace(PTRACE_PEEKDATA, proc->pid, ptr, 0);
	if (ret == -1 && errno != 0)
		return -1;

	value_set_word(valuep, ret);
	return 0;
}

static int
copy_aggregate_part(struct fetch_context *ctx, struct process *proc,
		    unsigned char *buf, size_t size)
{
	size_t slots = (size + 7) / 8;
	struct arg_type_info *long_info = type_get_simple(ARGTYPE_LONG);
	while (slots-- > 0) {
		size_t chunk_sz = size > 8 ? 8 : size;
		size -= 8;

		struct value tmp;
		value_init(&tmp, proc, NULL, long_info, 0);
		int rc = allocate_reg(ctx, proc, long_info, &tmp);
		if (rc >= 0) {
			memcpy(buf, value_get_data(&tmp, NULL), chunk_sz);
			buf += 8;
		}
		value_destroy(&tmp);
		if (rc < 0)
			return -1;
	}
	return 0;
}

static int
allocate_arg(struct fetch_context *ctx, struct process *proc,
	     struct arg_type_info *info, struct value *valuep)
{
	size_t sz = type_sizeof(proc, info);
	size_t align = type_alignof(proc, info);
	if (sz == (size_t)-1 || align == (size_t)-1)
		return -1;

	unsigned char *buf = value_reserve(valuep, sz);
	if (buf == NULL)
		return -1;

	assert(align == 0 || align == 1 || align == 2 || align == 4
	       || align == 8 || align == 16);

	/* For aggregates with an external alignment of 16 bytes, the
	 * Next Even policy is used.  128-bit integers use the Next
	 * Even policy as well.  */
	if (align == 16 && ctx->slot_n % 2 != 0)
		allocate_reg(ctx, proc, info, NULL);

	int rc= copy_aggregate_part(ctx, proc, buf, sz);

	return rc;
}

/* Stolen from David Mosberger's utrace tool, which he released under
   the GPL
   (http://www.gelato.unsw.edu.au/archives/linux-ia64/0104/1405.html) */
static inline double
fpreg_to_double (struct ia64_fpreg *fp) {
	double result;
	asm ("ldf.fill %0=%1" : "=f"(result) : "m"(*fp));
	return result;
}

static int
allocate_float(struct fetch_context *ctx, struct process *proc,
	       struct arg_type_info *info, struct value *valuep,
	       int take_slot)
{
	/* The actual parameter is passed in the next available
	 * floating-point parameter register, if one is
	 * available. Floating-point parameter registers are allocated
	 * as needed from the range f8-f15, starting with f8.  */
	/* Any register parameters corresponding to a
	 * variable-argument specification are passed in GRs.  */
	if (ctx->flt > 15 || ctx->ppflavor == PARAM_PACK_VARARGS)
		/* If all available floating-point parameter registers
		 * have been used, the actual parameter is passed in
		 * the appropriate general register(s).  */
		return allocate_reg(ctx, proc, info, valuep);

	union {
		double d;
		float f;
		char buf[0];
	} u = { .d = fpreg_to_double(&ctx->regs.fr[ctx->flt++]) };
	if (take_slot)
		allocate_reg(ctx, proc, info, NULL);

	if (info->type == ARGTYPE_FLOAT)
		u.f = u.d;
	else
		assert(info->type == ARGTYPE_DOUBLE);

	if (value_reserve(valuep, sizeof(u)) == NULL)
		return -1;
	memmove(value_get_raw_data(valuep), u.buf, sizeof(u));

	return 0;
}

static int
allocate_hfa(struct fetch_context *ctx, struct process *proc,
	     struct arg_type_info *info, struct value *valuep,
	     enum arg_type hfa_type, size_t hfa_count)
{
	size_t sz = type_sizeof(proc, info);
	if (sz == (size_t)-1)
		return -1;

	/* If an actual parameter is known to correspond to an HFA
	 * formal parameter, each element is passed in the next
	 * available floating-point argument register, until the eight
	 * argument registers are exhausted. The remaining elements of
	 * the aggregate are passed in output GRs, according to the
	 * normal conventions.
	 *
	 * Because HFAs are mapped to parameter slots as aggregates,
	 * single-precision HFAs will be allocated with two
	 * floating-point values in each parameter slot, but only one
	 * value per register.
	 *
	 * It is possible for the first of two values in a parameter
	 * slot to occupy the last available floating- point parameter
	 * register. In this case, the second value is passed in its
	 * designated GR, but the half of the GR that would have
	 * contained the first value is undefined.  */

	size_t slot_off = 0;

	unsigned char *buf = value_reserve(valuep, sz);
	if (buf == NULL)
		return -1;

	struct arg_type_info *hfa_info = type_get_simple(hfa_type);
	size_t hfa_sz = type_sizeof(proc, hfa_info);

	/* Pass in register the part that we can.  */
	while (ctx->flt <= 15 && hfa_count > 0) {
		struct value tmp;
		value_init(&tmp, proc, NULL, hfa_info, 0);
		int rc = allocate_float(ctx, proc, hfa_info, &tmp, 0);
		if (rc >= 0) {
			memcpy(buf, value_get_data(&tmp, NULL), hfa_sz);
			slot_off += hfa_sz;
			buf += hfa_sz;
			hfa_count--;

			/* Scratch each fully used slot.  */
			while (slot_off >= 8) {
				if (allocate_reg(ctx, proc, info, NULL) < 0)
					rc = -1;
				slot_off -= 8;
			}
		}
		value_destroy(&tmp);
		if (rc < 0)
			return -1;
	}

	/* If we have half-slot opened (the case where odd
	 * ARGTYPE_FLOAT member fits into the last floating point
	 * register, and the following even member does not), finish
	 * it first.  */
	struct arg_type_info *long_info = type_get_simple(ARGTYPE_LONG);
	if (slot_off != 0 && hfa_count > 0) {
		struct value tmp;
		value_init(&tmp, proc, NULL, long_info, 0);
		int rc = allocate_reg(ctx, proc, long_info, &tmp);
		if (rc >= 0) {
			unsigned char *data = value_get_data(&tmp, NULL);
			memcpy(buf, data, 8 - slot_off);
			buf += 8 - slot_off;
			hfa_count--;
		}
		value_destroy(&tmp);
		if (rc < 0) {
			return -1;
		}
	}

	/* The rest is passed in registers and on stack.  */
	size_t rest = hfa_count * hfa_sz;
	return copy_aggregate_part(ctx, proc, buf, rest);
}

static int
allocate_ret(struct fetch_context *ctx, struct process *proc,
	     struct arg_type_info *info, struct value *valuep)
{
	size_t sz = type_sizeof(proc, info);
	if (sz == (size_t)-1)
		return -1;

	/* Homogeneous floating-point aggregates [...] are returned in
	 * floating-point registers, provided the array or structure
	 * contains no more than eight individual values.  The
	 * elements of the aggregate are placed in successive
	 * floating-point registers, beginning with f8.  */
	if (info->type == ARGTYPE_STRUCT || info->type == ARGTYPE_ARRAY) {
		size_t hfa_size;
		struct arg_type_info *hfa_info
			= type_get_hfa_type(info, &hfa_size);
		if (hfa_info != NULL && hfa_size <= 8)
			return allocate_hfa(ctx, proc, info, valuep,
					    hfa_info->type, hfa_size);
	}

	/* Integers and pointers are passed in r8.  128-bit integers
	 * are passed in r8 and r9.  Aggregates of up to 256 bits [32
	 * bytes] are passed in registers r8...r11.  */
	if (sz <= 32) {
		unsigned char *buf = value_reserve(valuep, sz);
		if (buf == NULL)
			return -1;
		memcpy(buf, ctx->regs.gr + 8, sz);
		return 0;
	}

	if (value_pass_by_reference(valuep) < 0)
		return -1;
	value_set_word(valuep, ctx->r8);
	return 0;
}

int
arch_fetch_arg_next(struct fetch_context *ctx, enum tof type,
		    struct process *proc,
		    struct arg_type_info *info, struct value *valuep)
{
	switch (info->type) {
		struct arg_type_info *hfa_info;
		size_t hfa_size;

	case ARGTYPE_VOID:
		value_set_word(valuep, 0);
		return 0;

	case ARGTYPE_FLOAT:
	case ARGTYPE_DOUBLE:
		return allocate_float(ctx, proc, info, valuep, 1);

	case ARGTYPE_STRUCT:
		hfa_info = type_get_hfa_type(info, &hfa_size);
		if (hfa_info != NULL)
			return allocate_hfa(ctx, proc, info, valuep,
					    hfa_info->type, hfa_size);
		/* Fall through.  */
	case ARGTYPE_CHAR:
	case ARGTYPE_SHORT:
	case ARGTYPE_USHORT:
	case ARGTYPE_INT:
	case ARGTYPE_UINT:
	case ARGTYPE_LONG:
	case ARGTYPE_ULONG:
	case ARGTYPE_POINTER:
		return allocate_arg(ctx, proc, info, valuep);

	case ARGTYPE_ARRAY:
		/* Arrays decay into pointers.  XXX Fortran?  */
	default:
		assert(info->type != info->type);
		abort();
	}
}

int
arch_fetch_retval(struct fetch_context *ctx, enum tof type,
		  struct process *proc, struct arg_type_info *info,
		  struct value *valuep)
{
	if (fetch_context_init(proc, ctx) < 0)
		return -1;

	switch (info->type) {
	case ARGTYPE_VOID:
	case ARGTYPE_FLOAT:
	case ARGTYPE_DOUBLE:
		/* The rules for returning those types are the same as
		 * for passing them in arguments.  */
		return arch_fetch_arg_next(ctx, type, proc, info, valuep);

	case ARGTYPE_CHAR:
	case ARGTYPE_SHORT:
	case ARGTYPE_USHORT:
	case ARGTYPE_INT:
	case ARGTYPE_UINT:
	case ARGTYPE_LONG:
	case ARGTYPE_ULONG:
	case ARGTYPE_POINTER:
	case ARGTYPE_STRUCT:
		return allocate_ret(ctx, proc, info, valuep);

	case ARGTYPE_ARRAY:
		/* Arrays decay into pointers.  XXX Fortran?  */
		assert(info->type != ARGTYPE_ARRAY);
		abort();
	}
	assert("unhandled type");
	abort();
	return arch_fetch_arg_next(ctx, type, proc, info, valuep);
}

void
arch_fetch_arg_done(struct fetch_context *context)
{
	free(context);
}

int
arch_fetch_param_pack_start(struct fetch_context *context,
			    enum param_pack_flavor ppflavor)
{
	context->ppflavor = ppflavor;
	return 0;
}

void
arch_fetch_param_pack_end(struct fetch_context *context)
{
	context->ppflavor = PARAM_PACK_ARGS;
}
