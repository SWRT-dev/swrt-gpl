/*
 * This file is part of ltrace.
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
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"

#include <sys/types.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/procfs.h>
#include <sys/reg.h>

#include "backend.h"
#include "expr.h"
#include "fetch.h"
#include "proc.h"
#include "ptrace.h"
#include "type.h"
#include "value.h"

struct fetch_context
{
	elf_gregset_t regs;
	elf_fpregset_t fpregs;

	int arg_num;
	arch_addr_t stack_pointer;
	struct value retval;
};

static int
fetch_register_banks(struct process *proc, struct fetch_context *context,
		     int floating)
{
	if (ptrace(PTRACE_GETREGS, proc->pid, 0, &context->regs) < 0)
		return -1;

	if (floating
	    && ptrace(PTRACE_GETFPREGS, proc->pid, 0, &context->fpregs) < 0)
		return -1;

	return 0;
}

struct fetch_context *
arch_fetch_arg_init(enum tof type, struct process *proc,
		    struct arg_type_info *ret_info)
{
	struct fetch_context *context = malloc(sizeof(*context));
	if (context == NULL)
		return NULL;

	assert(type != LT_TOF_FUNCTIONR && type != LT_TOF_SYSCALLR);
	if (fetch_register_banks(proc, context, type == LT_TOF_FUNCTION) < 0) {
	fail:
		free(context);
		return NULL;
	}

	context->arg_num = 0;
	context->stack_pointer = (arch_addr_t)context->regs[PT_USP] + 4;

	size_t sz = type_sizeof(proc, ret_info);
	if (sz == (size_t)-1)
		goto fail;

	if (ret_info->type == ARGTYPE_STRUCT && !(sz <= 4 || sz == 8)) {
		value_init(&context->retval, proc, NULL, ret_info, 0);

		if (value_pass_by_reference(&context->retval) < 0)
			goto fail;
		value_set_word(&context->retval, context->regs[PT_A1]);
	} else {
		value_init_detached(&context->retval, NULL, NULL, 0);
	}

	return context;
}

struct fetch_context *
arch_fetch_arg_clone(struct process *proc, struct fetch_context *context)
{
	struct fetch_context *ret = malloc(sizeof(*ret));
	if (ret == NULL)
		return NULL;
	*ret = *context;
	return ret;
}

int
arch_fetch_arg_next(struct fetch_context *context, enum tof type,
		    struct process *proc, struct arg_type_info *info,
		    struct value *valuep)
{
	size_t sz = type_sizeof(proc, info);
	if (sz == (size_t)-1)
		return -1;

	if (type == LT_TOF_SYSCALL) {
		int reg;

		switch (context->arg_num++) {
		case 0: reg = PT_D1; break;
		case 1: reg = PT_D2; break;
		case 2: reg = PT_D3; break;
		case 3: reg = PT_D4; break;
		case 4: reg = PT_D5; break;
		case 5: reg = PT_A0; break;
		default:
			assert(!"More than six syscall arguments???");
			abort();
		}
		value_set_word(valuep, context->regs[reg]);
	} else {
		size_t a = type_alignof(valuep->inferior, valuep->type);
		if (a < 4)
			a = 4;
		context->stack_pointer = (arch_addr_t)
			align((unsigned long)context->stack_pointer, a);
		if (sz < 4)
			context->stack_pointer += 4 - sz;

		value_in_inferior(valuep, context->stack_pointer);
		context->stack_pointer += sz;
	}

	return 0;
}

int
arch_fetch_retval(struct fetch_context *context, enum tof type,
		  struct process *proc, struct arg_type_info *info,
		  struct value *valuep)
{
	if (fetch_register_banks(proc, context, type == LT_TOF_FUNCTIONR) < 0)
		return -1;

	if (context->retval.type != NULL) {
		/* Struct return value was extracted when in fetch
		 * init.  */
		*valuep = context->retval;
		return 0;
	}

	size_t sz = type_sizeof(proc, info);
	if (sz == (size_t)-1)
		return -1;
	if (value_reserve(valuep, sz) == NULL)
		return -1;

	switch (info->type) {
	case ARGTYPE_VOID:
		return 0;

	case ARGTYPE_INT:
	case ARGTYPE_UINT:
	case ARGTYPE_LONG:
	case ARGTYPE_ULONG:
	case ARGTYPE_CHAR:
	case ARGTYPE_SHORT:
	case ARGTYPE_USHORT:
	case ARGTYPE_POINTER:
		{
			unsigned char *buf = value_get_raw_data(valuep);
			int reg = info->type == ARGTYPE_POINTER ? PT_A0 : PT_D0;
			unsigned char *val
				= (unsigned char *)&context->regs[reg];
			if (sz < 4) val += 4 - sz;
			memcpy(buf, val, sz);
		}
		return 0;

	case ARGTYPE_FLOAT:
	case ARGTYPE_DOUBLE:
		{
			union {
				long double ld;
				double d;
				float f;
				char buf[0];
			} u;

			unsigned long *reg = &context->fpregs.fpregs[0];
			memcpy (&u.ld, reg, sizeof (u.ld));
			if (valuep->type->type == ARGTYPE_FLOAT)
				u.f = (float)u.ld;
			else if (valuep->type->type == ARGTYPE_DOUBLE)
				u.d = (double)u.ld;
			else {
				assert(!"Unexpected floating type!");
				abort();
			}
			unsigned char *buf = value_get_raw_data (valuep);
			memcpy (buf, u.buf, sz);
		}
		return 0;

	case ARGTYPE_STRUCT:
		{
			unsigned char *buf = value_get_raw_data(valuep);
			unsigned char *val
				= (unsigned char *)&context->regs[PT_D0];

			assert(sz <= 4 || sz == 8);
			if (sz < 4) val += 4 - sz;
			memcpy(buf, val, sz <= 4 ? sz : 4);
			if (sz == 8)
				memcpy(buf + 4, &context->regs[PT_D1], 4);
		}
		return 0;

	default:
		assert(!"Unexpected m68k retval type!");
		abort();
	}

	abort();
}

void
arch_fetch_arg_done(struct fetch_context *context)
{
	if (context != NULL)
		free(context);
}
