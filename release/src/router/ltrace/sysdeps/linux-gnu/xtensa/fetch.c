/*
 * This file is part of ltrace.
 * Copyright (C) 2014 Cadence Design Systems Inc.
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
#include <assert.h>
#include <elf.h>
#include <libelf.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "backend.h"
#include "fetch.h"
#include "library.h"
#include "proc.h"
#include "ptrace.h"
#include "type.h"
#include "value.h"

enum {
	MAX_REG_ARG_WORDS = 6,
	REG_ARG_BASE_REG = 2,

	MAX_RETURN_WORDS = 4,
	RETURN_BASE_REG = 10,
	SYSCALL_RETURN_BASE_REG = 2,
};

struct fetch_context {
	unsigned arg_word_idx;
	arch_addr_t sp;
	arch_addr_t ret_struct;

};

struct fetch_context *
arch_fetch_arg_init(enum tof type, struct process *proc,
		    struct arg_type_info *ret_info)
{
	struct fetch_context *ctx = malloc(sizeof(*ctx));
	size_t ret_sz = type_sizeof(proc, ret_info);
	unsigned long sp = ptrace(PTRACE_PEEKUSER, proc->pid,
				  (REG_A_BASE + 1), 0);

	if (ctx == NULL || sp == (size_t)-1) {
		free(ctx);
		return NULL;
	}

	ctx->arg_word_idx = 0;
	ctx->sp = (arch_addr_t)sp;
	ctx->ret_struct = NULL;

	if (ret_sz > MAX_RETURN_WORDS * sizeof(long)) {
		unsigned long a2 = ptrace(PTRACE_PEEKUSER, proc->pid,
					  (REG_A_BASE + 2), 0);
		ctx->ret_struct = (arch_addr_t)a2;
		++ctx->arg_word_idx;
	}

	return ctx;
}

struct fetch_context *
arch_fetch_arg_clone(struct process *proc,
		     struct fetch_context *ctx)
{
	struct fetch_context *clone = malloc(sizeof(*ctx));

	if (clone == NULL)
		return NULL;
	*clone = *ctx;
	return clone;
}

int
arch_fetch_arg_next(struct fetch_context *ctx, enum tof type,
		    struct process *proc,
		    struct arg_type_info *info, struct value *valuep)
{
	size_t sz = type_sizeof(proc, info);
	size_t al = type_alignof(proc, info);
	size_t words = (sz + sizeof(long) - 1) / sizeof(long);

	assert(sz != (size_t)-1);
	assert(al != (size_t)-1);

	if (al > sizeof(long)) {
		al /= sizeof(long);
		ctx->arg_word_idx = (ctx->arg_word_idx + al - 1) & ~(al - 1);
	}

	if (ctx->arg_word_idx + words <= MAX_REG_ARG_WORDS) {
		size_t i;
		unsigned char *data = value_reserve(valuep, sz);

		if (data == NULL)
			return -1;

		for (i = 0; i < words; ++i) {
			static const unsigned syscall_reg[] = {
				6, 3, 4, 5, 8, 9
			};
			unsigned regnr =
				(type == LT_TOF_FUNCTION ?
				 REG_ARG_BASE_REG + ctx->arg_word_idx + i :
				 syscall_reg[ctx->arg_word_idx + i]);
			unsigned long a = ptrace(PTRACE_PEEKUSER, proc->pid,
						 (REG_A_BASE + regnr), 0);
			size_t copy = sizeof(a) < sz ? sizeof(a) : sz;

			memcpy(data, &a, copy);
			data += sizeof(long);
			sz -= copy;
		}
		ctx->arg_word_idx += words;
		return 0;
	} else if (ctx->arg_word_idx < MAX_REG_ARG_WORDS) {
		ctx->arg_word_idx = MAX_REG_ARG_WORDS;
	}

	value_in_inferior(valuep, ctx->sp + sizeof(long) *
			  (ctx->arg_word_idx - MAX_REG_ARG_WORDS));
	ctx->arg_word_idx += words;

	return 0;
}

int
arch_fetch_retval(struct fetch_context *ctx, enum tof type,
		  struct process *proc, struct arg_type_info *info,
		  struct value *valuep)
{
	size_t sz = type_sizeof(proc, info);
	size_t words = (sz + sizeof(long) - 1) / sizeof(long);

	assert(sz != (size_t)-1);

	if (words <= MAX_RETURN_WORDS) {
		size_t i;
		unsigned char *data = value_reserve(valuep, sz);

		if (data == NULL)
			return -1;

		for (i = 0; i < words; ++i) {
			unsigned regnr = i +
				(type == LT_TOF_FUNCTIONR ?
				 RETURN_BASE_REG : SYSCALL_RETURN_BASE_REG);
			unsigned long a = ptrace(PTRACE_PEEKUSER, proc->pid,
						 (REG_A_BASE + regnr), 0);
			size_t copy = sizeof(a) < sz ? sizeof(a) : sz;

			memcpy(data, &a, copy);
			data += sizeof(long);
			sz -= copy;
		}
	} else {
		value_in_inferior(valuep, ctx->ret_struct);
	}
	return 0;
}

void
arch_fetch_arg_done(struct fetch_context *context)
{
	free(context);
}
