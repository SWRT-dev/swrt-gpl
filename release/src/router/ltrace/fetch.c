/*
 * This file is part of ltrace.
 * Copyright (C) 2011,2012 Petr Machata, Red Hat Inc.
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
#include <string.h>

#include "fetch.h"
#include "sysdep.h"
#include "value.h"
#include "type.h"

#ifdef ARCH_HAVE_FETCH_ARG
struct fetch_context *arch_fetch_arg_init(enum tof type, struct process *proc,
					  struct arg_type_info *ret_info);

struct fetch_context *arch_fetch_arg_clone(struct process *proc,
					   struct fetch_context *context);

int arch_fetch_arg_next(struct fetch_context *ctx, enum tof type,
			struct process *proc, struct arg_type_info *info,
			struct value *valuep);

int arch_fetch_retval(struct fetch_context *ctx, enum tof type,
		      struct process *proc, struct arg_type_info *info,
		      struct value *valuep);

void arch_fetch_arg_done(struct fetch_context *context);

# ifdef ARCH_HAVE_FETCH_PACK
int arch_fetch_param_pack_start(struct fetch_context *context,
				enum param_pack_flavor ppflavor);

void arch_fetch_param_pack_end(struct fetch_context *context);
# endif

#else
/* Fall back to gimme_arg.  */

long gimme_arg(enum tof type, struct process *proc, int arg_num,
	       struct arg_type_info *info);

struct fetch_context {
	int argnum;
};

struct fetch_context *
arch_fetch_arg_init(enum tof type, struct process *proc,
		    struct arg_type_info *ret_info)
{
	return calloc(sizeof(struct fetch_context), 1);
}

struct fetch_context *
arch_fetch_arg_clone(struct process *proc, struct fetch_context *context)
{
	struct fetch_context *ret = malloc(sizeof(*ret));
	if (ret == NULL)
		return NULL;
	return memcpy(ret, context, sizeof(*ret));
}

int
arch_fetch_arg_next(struct fetch_context *context, enum tof type,
		    struct process *proc,
		    struct arg_type_info *info, struct value *valuep)
{
	long l = gimme_arg(type, proc, context->argnum++, info);
	value_set_word(valuep, l);
	return 0;
}

int
arch_fetch_retval(struct fetch_context *context, enum tof type,
		  struct process *proc,
		  struct arg_type_info *info, struct value *valuep)
{
	long l = gimme_arg(type, proc, -1, info);
	value_set_word(valuep, l);
	return 0;
}

void
arch_fetch_arg_done(struct fetch_context *context)
{
	free(context);
}
#endif

#if !defined(ARCH_HAVE_FETCH_ARG) || !defined(ARCH_HAVE_FETCH_PACK)
int
arch_fetch_param_pack_start(struct fetch_context *context,
			    enum param_pack_flavor ppflavor)
{
	return 0;
}

void
arch_fetch_param_pack_end(struct fetch_context *context)
{
}
#endif

struct fetch_context *
fetch_arg_init(enum tof type, struct process *proc,
	       struct arg_type_info *ret_info)
{
	return arch_fetch_arg_init(type, proc, ret_info);
}

struct fetch_context *
fetch_arg_clone(struct process *proc, struct fetch_context *context)
{
	return arch_fetch_arg_clone(proc, context);
}

int
fetch_arg_next(struct fetch_context *context, enum tof type,
	       struct process *proc,
	       struct arg_type_info *info, struct value *valuep)
{
	return arch_fetch_arg_next(context, type, proc, info, valuep);
}

int
fetch_retval(struct fetch_context *context, enum tof type,
	     struct process *proc,
	     struct arg_type_info *info, struct value *valuep)
{
	return arch_fetch_retval(context, type, proc, info, valuep);
}

void
fetch_arg_done(struct fetch_context *context)
{
	return arch_fetch_arg_done(context);
}

int
fetch_param_pack_start(struct fetch_context *context,
		       enum param_pack_flavor ppflavor)
{
	return arch_fetch_param_pack_start(context, ppflavor);
}

void
fetch_param_pack_end(struct fetch_context *context)
{
	return arch_fetch_param_pack_end(context);
}
