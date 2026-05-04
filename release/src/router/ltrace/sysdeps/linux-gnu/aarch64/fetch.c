/*
 * This file is part of ltrace.
 * Copyright (C) 2014 Petr Machata, Red Hat, Inc.
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

#include <sys/ptrace.h>
#include <asm/ptrace.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "fetch.h"
#include "proc.h"
#include "type.h"
#include "value.h"

int aarch64_read_gregs(struct process *proc, struct user_pt_regs *regs);
int aarch64_read_fregs(struct process *proc, struct user_fpsimd_state *regs);


struct fetch_context
{
	struct user_pt_regs gregs;
	struct user_fpsimd_state fpregs;
	arch_addr_t nsaa;
	unsigned ngrn;
	unsigned nsrn;
	arch_addr_t x8;
};

static int
context_init(struct fetch_context *context, struct process *proc)
{
	if (aarch64_read_gregs(proc, &context->gregs) < 0
	    || aarch64_read_fregs(proc, &context->fpregs) < 0)
		return -1;

	context->ngrn = 0;
	context->nsrn = 0;
	/* XXX double cast */
	context->nsaa = (arch_addr_t) (uintptr_t) context->gregs.sp;
	context->x8 = 0;

	return 0;
}

struct fetch_context *
arch_fetch_arg_clone(struct process *proc, struct fetch_context *context)
{
	struct fetch_context *ret = malloc(sizeof(*ret));
	if (ret == NULL)
		return NULL;
	return memcpy(ret, context, sizeof(*ret));
}

static void
fetch_next_gpr(struct fetch_context *context, unsigned char *buf)
{
	uint64_t u = context->gregs.regs[context->ngrn++];
	memcpy(buf, &u, 8);
}

static int
fetch_gpr(struct fetch_context *context, struct value *value, size_t sz)
{
	if (sz < 8)
		sz = 8;

	unsigned char *buf = value_reserve(value, sz);
	if (buf == NULL)
		return -1;

	size_t i;
	for (i = 0; i < sz; i += 8)
		fetch_next_gpr(context, buf + i);

	return 0;
}

static void
fetch_next_sse(struct fetch_context *context, unsigned char *buf, size_t sz)
{
	__int128 u = context->fpregs.vregs[context->nsrn++];
	memcpy(buf, &u, sz);
}

static int
fetch_sse(struct fetch_context *context, struct value *value, size_t sz)
{
	unsigned char *buf = value_reserve(value, sz);
	if (buf == NULL)
		return -1;

	fetch_next_sse(context, buf, sz);
	return 0;
}

static int
fetch_hfa(struct fetch_context *context,
	  struct value *value, struct arg_type_info *hfa_t, size_t count)
{
	size_t sz = type_sizeof(value->inferior, hfa_t);
	unsigned char *buf = value_reserve(value, sz * count);
	if (buf == NULL)
		return -1;

	size_t i;
	for (i = 0; i < count; ++i) {
		fetch_next_sse(context, buf, sz);
		buf += sz;
	}
	return 0;
}

static int
fetch_stack(struct fetch_context *context, struct value *value,
	    size_t align, size_t sz)
{
	if (align < 8)
		align = 8;
	size_t amount = ((sz + align - 1) / align) * align;

	/* XXX double casts */
	uintptr_t sp = (uintptr_t) context->nsaa;
	sp = ((sp + align - 1) / align) * align;

	value_in_inferior(value, (arch_addr_t) sp);

	sp += amount;
	context->nsaa = (arch_addr_t) sp;

	return 0;
}

enum convert_method {
	CVT_ERR = -1,
	CVT_NOP = 0,
	CVT_BYREF,
};

enum fetch_method {
	FETCH_NOP,
	FETCH_STACK,
	FETCH_GPR,
	FETCH_SSE,
	FETCH_HFA,
};

struct fetch_script {
	enum convert_method c;
	enum fetch_method f;
	size_t sz;
	struct arg_type_info *hfa_t;
	size_t count;
};

static struct fetch_script
pass_arg(struct fetch_context const *context,
	 struct process *proc, struct arg_type_info *info)
{
	enum fetch_method cvt = CVT_NOP;

	size_t sz = type_sizeof(proc, info);
	if (sz == (size_t) -1)
		return (struct fetch_script) { CVT_ERR, FETCH_NOP, sz };

	switch (info->type) {
	case ARGTYPE_VOID:
		return (struct fetch_script) { cvt, FETCH_NOP, sz };

	case ARGTYPE_STRUCT:
	case ARGTYPE_ARRAY:;
		size_t count;
		struct arg_type_info *hfa_t = type_get_hfa_type(info, &count);
		if (hfa_t != NULL && count <= 4) {
			if (context->nsrn + count <= 8)
				return (struct fetch_script)
					{ cvt, FETCH_HFA, sz, hfa_t, count };
			return (struct fetch_script)
				{ cvt, FETCH_STACK, sz, hfa_t, count };
		}

		if (sz <= 16) {
			size_t count = sz / 8;
			if (context->ngrn + count <= 8)
				return (struct fetch_script)
					{ cvt, FETCH_GPR, sz };
		}

		cvt = CVT_BYREF;
		sz = 8;
		/* Fall through.  */

	case ARGTYPE_POINTER:
	case ARGTYPE_INT:
	case ARGTYPE_UINT:
	case ARGTYPE_LONG:
	case ARGTYPE_ULONG:
	case ARGTYPE_CHAR:
	case ARGTYPE_SHORT:
	case ARGTYPE_USHORT:
		if (context->ngrn < 8 && sz <= 8)
			return (struct fetch_script) { cvt, FETCH_GPR, sz };
		/* We don't support types wider than 8 bytes as of
		 * now.  */
		assert(sz <= 8);

		return (struct fetch_script) { cvt, FETCH_STACK, sz };

	case ARGTYPE_FLOAT:
	case ARGTYPE_DOUBLE:
		if (context->nsrn < 8) {
			/* ltrace doesn't support float128.  */
			assert(sz <= 8);
			return (struct fetch_script) { cvt, FETCH_SSE, sz };
		}

		return (struct fetch_script) { cvt, FETCH_STACK, sz };
	}

	assert(! "Failed to allocate argument.");
	abort();
}

static int
convert_arg(struct value *value, struct fetch_script how)
{
	switch (how.c) {
	case CVT_NOP:
		return 0;
	case CVT_BYREF:
		return value_pass_by_reference(value);
	case CVT_ERR:
		return -1;
	}

	assert(! "Don't know how to convert argument.");
	abort();
}

static int
fetch_arg(struct fetch_context *context,
	  struct process *proc, struct arg_type_info *info,
	  struct value *value, struct fetch_script how)
{
	if (convert_arg(value, how) < 0)
		return -1;

	switch (how.f) {
	case FETCH_NOP:
		return 0;

	case FETCH_STACK:
		if (how.hfa_t != NULL && how.count != 0 && how.count <= 8)
			context->nsrn = 8;
		return fetch_stack(context, value,
				   type_alignof(proc, info), how.sz);

	case FETCH_GPR:
		return fetch_gpr(context, value, how.sz);

	case FETCH_SSE:
		return fetch_sse(context, value, how.sz);

	case FETCH_HFA:
		return fetch_hfa(context, value, how.hfa_t, how.count);
	}

	assert(! "Don't know how to fetch argument.");
	abort();
}

struct fetch_context *
arch_fetch_arg_init(enum tof type, struct process *proc,
		    struct arg_type_info *ret_info)
{
	struct fetch_context *context = malloc(sizeof *context);
	if (context == NULL || context_init(context, proc) < 0) {
	fail:
		free(context);
		return NULL;
	}

	/* There's a provision in ARMv8 parameter passing convention
	 * for returning types that, if passed as first argument to a
	 * function, would be passed on stack.  For those types, x8
	 * contains an address where the return argument should be
	 * placed.  The callee doesn't need to preserve the value of
	 * x8, so we need to fetch it now.
	 *
	 * To my knowledge, there are currently no types where this
	 * holds, but the code is here, utterly untested.  */

	struct fetch_script how = pass_arg(context, proc, ret_info);
	if (how.c == CVT_ERR)
		goto fail;
	if (how.c == CVT_NOP && how.f == FETCH_STACK) {
		/* XXX double cast.  */
		context->x8 = (arch_addr_t) (uintptr_t) context->gregs.regs[8];
		/* See the comment above about the assert.  */
		assert(! "Unexpected: first argument passed on stack.");
		abort();
	}

	return context;
}

int
arch_fetch_arg_next(struct fetch_context *context, enum tof type,
		    struct process *proc, struct arg_type_info *info,
		    struct value *value)
{
	return fetch_arg(context, proc, info, value,
			 pass_arg(context, proc, info));
}

int
arch_fetch_retval(struct fetch_context *context, enum tof type,
		  struct process *proc, struct arg_type_info *info,
		  struct value *value)
{
	if (context->x8 != 0) {
		value_in_inferior(value, context->x8);
		return 0;
	}

	if (context_init(context, proc) < 0)
		return -1;

	return fetch_arg(context, proc, info, value,
			 pass_arg(context, proc, info));
}

void
arch_fetch_arg_done(struct fetch_context *context)
{
	if (context != NULL)
		free(context);
}

size_t
arch_type_sizeof(struct process *proc, struct arg_type_info *arg)
{
	return (size_t) -2;
}

size_t
arch_type_alignof(struct process *proc, struct arg_type_info *arg)
{
	return (size_t) -2;
}
