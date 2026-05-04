/*
 * This file is part of ltrace.
 * Copyright (C) 2012 Petr Machata, Red Hat Inc.
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

#include <asm/ptrace.h>
#include <sys/ptrace.h>
#include <sys/ucontext.h>
#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "backend.h"
#include "fetch.h"
#include "type.h"
#include "proc.h"
#include "value.h"

struct fetch_context {
	struct user_regs_struct regs;
	arch_addr_t stack_pointer;
	int greg;
	int freg;
};

static int
s390x(struct fetch_context *ctx)
{
	/* +--------+--------+--------+
	 * | PSW.31 | PSW.32 | mode   |
	 * +--------+--------+--------+
	 * | 0      | 0      | 24-bit | Not supported in Linux
	 * | 0      | 1      | 31-bit | s390 compatible mode
	 * | 1      | 1      | 64-bit | z/Architecture, "s390x"
	 * +--------+--------+--------+
	 * (Note: The leftmost bit is PSW.0, rightmost PSW.63.)
	 */

#ifdef __s390x__
	if ((ctx->regs.psw.mask & 0x180000000UL) == 0x180000000UL)
		return 1;
#endif
	return 0;
}

static int
fetch_register_banks(struct process *proc, struct fetch_context *ctx,
		     bool syscall_enter)
{
	ptrace_area parea;
	parea.len = sizeof(ctx->regs);
	parea.process_addr = (uintptr_t)&ctx->regs;
	parea.kernel_addr = 0;
	if (ptrace(PTRACE_PEEKUSR_AREA, proc->pid, &parea, NULL) < 0) {
		fprintf(stderr, "fetch_register_banks GPR: %s\n",
			strerror(errno));
		return -1;
	}

	if (syscall_enter)
		ctx->regs.gprs[2] = ctx->regs.orig_gpr2;

	return 0;
}

static int
fetch_context_init(struct process *proc, struct fetch_context *context,
		   bool syscall_enter)
{
	context->greg = 2;
	context->freg = 0;
	return fetch_register_banks(proc, context, syscall_enter);
}

struct fetch_context *
arch_fetch_arg_init(enum tof type, struct process *proc,
		    struct arg_type_info *ret_info)
{
	struct fetch_context *context = malloc(sizeof(*context));
	if (context == NULL
	    || fetch_context_init(proc, context, type == LT_TOF_SYSCALL) < 0) {
		fprintf(stderr, "arch_fetch_arg_init: %s\n",
			strerror(errno));
		free(context);
		return NULL;
	}

	context->stack_pointer = get_stack_pointer(proc)
		+ (s390x(context) ? 160 : 96);
	if (ret_info->type == ARGTYPE_STRUCT)
		++context->greg;

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
		    size_t sz)
{
	/* Note: here we shouldn't see large composite types, those
	 * are passed by reference, which is handled below.  Here we
	 * only deal with integers, floats, small structs, etc.  */

	size_t a;
	if (s390x(ctx)) {
		assert(sz <= 8);
		a = 8;
	} else {
		/* Note: double is 8 bytes.  */
		assert(sz <= 8);
		a = 4;
	}

	size_t off = sz < a ? a - sz : 0;
	value_in_inferior(valuep, ctx->stack_pointer + off);

	ctx->stack_pointer += sz > a ? sz : a;
	return 0;
}

static void
copy_gpr(struct fetch_context *ctx, struct value *valuep, int regno)
{
	value_set_word(valuep, ctx->regs.gprs[regno]);
}

static int
allocate_gpr(struct fetch_context *ctx, struct process *proc,
	     struct arg_type_info *info, struct value *valuep,
	     size_t sz)
{
	if (ctx->greg > 6)
		return allocate_stack_slot(ctx, proc, info, valuep, sz);

	copy_gpr(ctx, valuep, ctx->greg++);
	return 0;
}

static int
allocate_gpr_pair(struct fetch_context *ctx, struct process *proc,
		  struct arg_type_info *info, struct value *valuep,
		  size_t sz)
{
	assert(!s390x(ctx));
	assert(sz <= 8);

	if (ctx->greg > 5) {
		ctx->greg = 7;
		return allocate_stack_slot(ctx, proc, info, valuep, sz);
	}

	if (value_reserve(valuep, sz) == NULL)
		return -1;

	unsigned char *ptr = value_get_raw_data(valuep);
	union {
		struct {
			uint32_t a;
			uint32_t b;
		};
		unsigned char buf[8];
	} u;
	u.a = ctx->regs.gprs[ctx->greg++];
	u.b = ctx->regs.gprs[ctx->greg++];
	memcpy(ptr, u.buf, sz);

	return 0;
}

static int
allocate_fpr(struct fetch_context *ctx, struct process *proc,
	     struct arg_type_info *info, struct value *valuep,
	     size_t sz)
{
	int pool = s390x(ctx) ? 6 : 2;

	if (ctx->freg > pool)
		return allocate_stack_slot(ctx, proc, info, valuep, sz);

	if (value_reserve(valuep, sz) == NULL)
		return -1;

	memcpy(value_get_raw_data(valuep),
	       &ctx->regs.fp_regs.fprs[ctx->freg], sz);
	ctx->freg += 2;

	return 0;
}

int
arch_fetch_arg_next(struct fetch_context *ctx, enum tof type,
		    struct process *proc,
		    struct arg_type_info *info, struct value *valuep)
{
	size_t sz = type_sizeof(proc, info);
	if (sz == (size_t)-1)
		return -1;

	switch (info->type) {
	case ARGTYPE_VOID:
		value_set_word(valuep, 0);
		return 0;

	case ARGTYPE_STRUCT:
		if (type_get_fp_equivalent(info) != NULL)
			/* fall through */
	case ARGTYPE_FLOAT:
	case ARGTYPE_DOUBLE:
			return allocate_fpr(ctx, proc, info, valuep, sz);

		/* Structures<4 bytes on s390 and structures<8 bytes
		 * on s390x are passed in register.  On s390, long
		 * long and structures<8 bytes are passed in two
		 * consecutive registers (if two are available).  */

		if (sz <= (s390x(ctx) ? 8 : 4))
			return allocate_gpr(ctx, proc, info, valuep, sz);
		else if (sz <= 8)
			return allocate_gpr_pair(ctx, proc, info, valuep, sz);

		/* fall through */

	case ARGTYPE_ARRAY:
		if (value_pass_by_reference(valuep) < 0)
			return -1;
		/* fall through */

	case ARGTYPE_INT:
	case ARGTYPE_UINT:
	case ARGTYPE_LONG:
	case ARGTYPE_ULONG:
	case ARGTYPE_CHAR:
	case ARGTYPE_SHORT:
	case ARGTYPE_USHORT:
	case ARGTYPE_POINTER:
		return allocate_gpr(ctx, proc, info, valuep, sz);

	default:
		assert(info->type != info->type);
		abort();
	}
	return -1;
}

int
arch_fetch_retval(struct fetch_context *ctx, enum tof type,
		  struct process *proc, struct arg_type_info *info,
		  struct value *valuep)
{
	if (info->type == ARGTYPE_STRUCT) {
		if (value_pass_by_reference(valuep) < 0)
			return -1;
		copy_gpr(ctx, valuep, 2);
		return 0;
	}

	if (fetch_context_init(proc, ctx, false) < 0)
		return -1;
	return arch_fetch_arg_next(ctx, type, proc, info, valuep);
}

void
arch_fetch_arg_done(struct fetch_context *context)
{
	free(context);
}
