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

#include <errno.h>

#include "zero.h"
#include "common.h"
#include "type.h"
#include "value.h"
#include "expr.h"

static int
zero_callback_max(struct value *ret_value, struct value *lhs,
		  struct value_dict *arguments,
		  size_t max, void *data)
{
	size_t i;
	for (i = 0; i < max; ++i) {
		struct value element;
		if (value_init_element(&element, lhs, i) < 0)
			return -1;

		int zero = value_is_zero(&element, arguments);

		value_destroy(&element);

		if (zero)
			break;
	}

	struct arg_type_info *long_type = type_get_simple(ARGTYPE_LONG);
	value_init_detached(ret_value, NULL, long_type, 0);
	value_set_word(ret_value, i);
	return 0;
}

/* LHS->zero(RHS).  Looks for a length of zero-terminated array, but
 * looks no further than first RHS bytes.  */
static int
zero_callback(struct value *ret_value, struct value *lhs,
	      struct value *rhs, struct value_dict *arguments, void *data)
{
	long l;
	if (value_extract_word(rhs, &l, arguments) < 0)
		return -1;
	if (l < 0)
		/* It might just be a positive value >2GB, but that's
		 * not likely.  */
		report_global_error("maximum array length seems negative");
	size_t max = (size_t)l;
	return zero_callback_max(ret_value, lhs, arguments, max, data);
}

/* LHS->zero.  Looks for a length of zero-terminated array, without
 * limit.  */
static int
zero1_callback(struct value *ret_value, struct value *lhs,
	       struct value_dict *arguments, void *data)
{
	return zero_callback_max(ret_value, lhs, arguments, (size_t)-1, data);
}

struct expr_node *
build_zero_w_arg(struct expr_node *expr, int own)
{
	struct expr_node *e_z = malloc(sizeof(*e_z));
	if (e_z == NULL)
		return NULL;

	expr_init_cb2(e_z, &zero_callback,
		      expr_self(), 0, expr, own, NULL);
	return e_z;
}

struct expr_node *
expr_node_zero(void)
{
	static struct expr_node *nodep = NULL;
	if (nodep == NULL) {
		static struct expr_node node;
		expr_init_cb1(&node, &zero1_callback,
			      expr_self(), 0, (void *)-1);
		nodep = &node;
	}
	return nodep;
}
