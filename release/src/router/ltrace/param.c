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

#include <assert.h>
#include <stdlib.h>

#include "param.h"
#include "type.h"
#include "value.h"
#include "expr.h"

void
param_init_type(struct param *param, struct arg_type_info *type, int own)
{
	param->flavor = PARAM_FLAVOR_TYPE;
	param->u.type.type = type;
	param->u.type.own_type = own;
}

void
param_init_stop(struct param *param)
{
	param->flavor = PARAM_FLAVOR_STOP;
}

void
param_init_pack(struct param *param, enum param_pack_flavor ppflavor,
		struct expr_node *args, size_t nargs, int own_args,
		struct param_enum *(*init)(struct value *cb_args,
					   size_t nargs,
					   struct value_dict *arguments),
		int (*next)(struct param_enum *context,
			    struct arg_type_info *infop,
			    int *insert_stop),
		enum param_status (*stop)(struct param_enum *ctx,
					  struct value *value),
		void (*done)(struct param_enum *))
{
	param->flavor = PARAM_FLAVOR_PACK;
	param->u.pack.args = args;
	param->u.pack.nargs = nargs;
	param->u.pack.own_args = own_args;
	param->u.pack.ppflavor = ppflavor;
	param->u.pack.init = init;
	param->u.pack.next = next;
	param->u.pack.stop = stop;
	param->u.pack.done = done;
}

struct param_enum *
param_pack_init(struct param *param, struct value_dict *fargs)
{
	struct value cb_args[param->u.pack.nargs];
	size_t i;

	/* For evaluation of argument expressions, we pass in this as
	 * a "current" value.  */
	struct arg_type_info *void_type = type_get_simple(ARGTYPE_VOID);
	struct value void_val;
	value_init_detached(&void_val, NULL, void_type, 0);

	struct param_enum *ret = NULL;
	for (i = 0; i < param->u.pack.nargs; ++i) {
		if (expr_eval(&param->u.pack.args[i], &void_val,
			      fargs, &cb_args[i]) < 0)
			goto release;
	}

	ret = param->u.pack.init(cb_args, param->u.pack.nargs, fargs);

release:
	while (i-- > 0)
		value_destroy(&cb_args[i]);
	return ret;
}

int
param_pack_next(struct param *param, struct param_enum *context,
		struct arg_type_info *infop, int *insert_stop)
{
	return param->u.pack.next(context, infop, insert_stop);
}

enum param_status
param_pack_stop(struct param *param,
		struct param_enum *context, struct value *value)
{
	return param->u.pack.stop(context, value);
}

void
param_pack_done(struct param *param, struct param_enum *context)
{
	return param->u.pack.done(context);
}

void
param_destroy(struct param *param)
{
	if (param == NULL)
		return;

	switch (param->flavor) {
	case PARAM_FLAVOR_TYPE:
		if (param->u.type.own_type) {
			type_destroy(param->u.type.type);
			free(param->u.type.type);
		}
		return;

	case PARAM_FLAVOR_PACK:
		if (param->u.pack.own_args) {
			size_t i;
			for (i = 0; i < param->u.pack.nargs; ++i)
				expr_destroy(&param->u.pack.args[i]);
			free(param->u.pack.args);
		}
		return;

	case PARAM_FLAVOR_STOP:
		return;
	}

	assert(!"Unknown value of param flavor!");
	abort();
}
