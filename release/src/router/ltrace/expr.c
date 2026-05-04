/*
 * This file is part of ltrace.
 * Copyright (C) 2011,2012,2013 Petr Machata, Red Hat Inc.
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

#include <string.h>
#include <assert.h>
#include <errno.h>
#include <stdlib.h>

#include "expr.h"

static void
expr_init_common(struct expr_node *node, enum expr_node_kind kind)
{
	node->kind = kind;
	node->lhs = NULL;
	node->own_lhs = 0;
	memset(&node->u, 0, sizeof(node->u));
}

void
expr_init_self(struct expr_node *node)
{
	expr_init_common(node, EXPR_OP_SELF);
}

void
expr_init_named(struct expr_node *node,
		const char *name, int own_name)
{
	expr_init_common(node, EXPR_OP_NAMED);
	node->u.name.s = name;
	node->u.name.own = own_name;
}

void
expr_init_argno(struct expr_node *node, size_t num)
{
	expr_init_common(node, EXPR_OP_ARGNO);
	node->u.num = num;
}

void
expr_init_const(struct expr_node *node, struct value *val)
{
	expr_init_common(node, EXPR_OP_CONST);
	node->u.value = *val;
}

void
expr_init_const_word(struct expr_node *node, long l,
		     struct arg_type_info *type, int own_type)
{
	struct value val;
	value_init_detached(&val, NULL, type, own_type);
	value_set_word(&val, l);
	expr_init_const(node, &val);
}

void
expr_init_index(struct expr_node *node,
		struct expr_node *lhs, int own_lhs,
		struct expr_node *rhs, int own_rhs)
{
	expr_init_common(node, EXPR_OP_INDEX);
	node->lhs = lhs;
	node->own_lhs = own_lhs;
	node->u.node.n = rhs;
	node->u.node.own = own_rhs;
}

void
expr_init_up(struct expr_node *node, struct expr_node *lhs, int own_lhs)
{
	assert(lhs != NULL);
	expr_init_common(node, EXPR_OP_UP);
	node->lhs = lhs;
	node->own_lhs = own_lhs;
}

void
expr_init_cb1(struct expr_node *node,
	      int (*cb)(struct value *ret_value, struct value *value,
			struct value_dict *arguments, void *data),
	      struct expr_node *lhs, int own_lhs, void *data)
{
	expr_init_common(node, EXPR_OP_CALL1);
	node->lhs = lhs;
	node->own_lhs = own_lhs;
	node->u.call.u.cb1 = cb;
	node->u.call.data = data;
}

void
expr_init_cb2(struct expr_node *node,
	      int (*cb)(struct value *ret_value,
			struct value *lhs, struct value *rhs,
			struct value_dict *arguments, void *data),
	      struct expr_node *lhs, int own_lhs,
	      struct expr_node *rhs, int own_rhs, void *data)
{
	expr_init_common(node, EXPR_OP_CALL2);
	node->lhs = lhs;
	node->own_lhs = own_lhs;
	node->u.call.rhs = rhs;
	node->u.call.own_rhs = own_rhs;
	node->u.call.u.cb2 = cb;
	node->u.call.data = data;
}

static void
release_expr(struct expr_node *node, int own)
{
	if (own) {
		expr_destroy(node);
		free(node);
	}
}

void
expr_destroy(struct expr_node *node)
{
	if (node == NULL)
		return;

	switch (node->kind) {
	case EXPR_OP_ARGNO:
	case EXPR_OP_SELF:
		return;

	case EXPR_OP_CONST:
		value_destroy(&node->u.value);
		return;

	case EXPR_OP_NAMED:
		if (node->u.name.own)
			free((char *)node->u.name.s);
		return;

	case EXPR_OP_INDEX:
		release_expr(node->lhs, node->own_lhs);
		release_expr(node->u.node.n, node->u.node.own);
		return;

	case EXPR_OP_CALL2:
		release_expr(node->u.call.rhs, node->u.call.own_rhs);
		/* Fall through.  */
	case EXPR_OP_UP:
	case EXPR_OP_CALL1:
		release_expr(node->lhs, node->own_lhs);
		return;
	}

	assert(!"Invalid value of node kind");
	abort();
}

static int
expr_alloc_and_clone(struct expr_node **retpp, struct expr_node *node, int own)
{
	*retpp = node;
	if (own) {
		*retpp = malloc(sizeof **retpp);
		if (*retpp == NULL || expr_clone(*retpp, node) < 0) {
			free(*retpp);
			return -1;
		}
	}
	return 0;
}

int
expr_clone(struct expr_node *retp, const struct expr_node *node)
{
	*retp = *node;

	switch (node->kind) {
		struct expr_node *nlhs;
		struct expr_node *nrhs;

	case EXPR_OP_ARGNO:
	case EXPR_OP_SELF:
		return 0;

	case EXPR_OP_CONST:
		return value_clone(&retp->u.value, &node->u.value);

	case EXPR_OP_NAMED:
		if (node->u.name.own
		    && (retp->u.name.s = strdup(node->u.name.s)) == NULL)
			return -1;
		return 0;

	case EXPR_OP_INDEX:
		if (expr_alloc_and_clone(&nlhs, node->lhs, node->own_lhs) < 0)
			return -1;

		if (expr_alloc_and_clone(&nrhs, node->u.node.n,
					 node->u.node.own) < 0) {
			if (nlhs != node->lhs) {
				expr_destroy(nlhs);
				free(nlhs);
			}
			return -1;
		}

		retp->lhs = nlhs;
		retp->u.node.n = nrhs;
		return 0;

	case EXPR_OP_CALL2:
		if (expr_alloc_and_clone(&nrhs, node->u.call.rhs,
					 node->u.call.own_rhs) < 0)
			return -1;
		retp->u.call.rhs = nrhs;
		/* Fall through.  */

	case EXPR_OP_UP:
	case EXPR_OP_CALL1:
		if (expr_alloc_and_clone(&nlhs, node->lhs, node->own_lhs) < 0) {
			if (node->kind == EXPR_OP_CALL2
			    && node->u.call.own_rhs) {
				expr_destroy(nrhs);
				free(nrhs);
				return -1;
			}
		}

		retp->lhs = nlhs;
		return 0;
	}

	assert(!"Invalid value of node kind");
	abort();
}

int
expr_is_compile_constant(struct expr_node *node)
{
	return node->kind == EXPR_OP_CONST;
}

static int
eval_up(struct expr_node *node, struct value *context,
	struct value_dict *arguments, struct value *ret_value)
{
	if (expr_eval(node->lhs, context, arguments, ret_value) < 0)
		return -1;
	struct value *parent = value_get_parental_struct(ret_value);
	if (parent == NULL) {
		value_destroy(ret_value);
		return -1;
	}
	*ret_value = *parent;
	return 0;
}

static int
eval_cb1(struct expr_node *node, struct value *context,
	 struct value_dict *arguments, struct value *ret_value)
{
	struct value val;
	if (expr_eval(node->lhs, context, arguments, &val) < 0)
		return -1;

	int ret = 0;
	if (node->u.call.u.cb1(ret_value, &val, arguments,
			       node->u.call.data) < 0)
		ret = -1;

	/* N.B. the callback must return its own value, or somehow
	 * clone the incoming argument.  */
	value_destroy(&val);
	return ret;
}

static int
eval_cb2(struct expr_node *node, struct value *context,
	 struct value_dict *arguments, struct value *ret_value)
{
	struct value lhs;
	if (expr_eval(node->lhs, context, arguments, &lhs) < 0)
		return -1;

	struct value rhs;
	if (expr_eval(node->u.call.rhs, context, arguments, &rhs) < 0) {
		value_destroy(&lhs);
		return -1;
	}

	int ret = 0;
	if (node->u.call.u.cb2(ret_value, &lhs, &rhs, arguments,
			       node->u.call.data) < 0)
		ret = -1;

	/* N.B. the callback must return its own value, or somehow
	 * clone the incoming argument.  */
	value_destroy(&lhs);
	value_destroy(&rhs);
	return ret;
}

int
eval_index(struct expr_node *node, struct value *context,
	   struct value_dict *arguments, struct value *ret_value)
{
	struct value lhs;
	if (expr_eval(node->lhs, context, arguments, &lhs) < 0)
		return -1;

	long l;
	if (expr_eval_word(node->u.node.n, context, arguments, &l) < 0) {
	fail:
		value_destroy(&lhs);
		return -1;
	}

	if (value_init_element(ret_value, &lhs, (size_t)l) < 0)
		goto fail;
	return 0;
}

int
expr_eval(struct expr_node *node, struct value *context,
	  struct value_dict *arguments, struct value *ret_value)
{
	switch (node->kind) {
		struct value *valp;
	case EXPR_OP_ARGNO:
		valp = val_dict_get_num(arguments, node->u.num);
		if (valp == NULL)
			return -1;
		*ret_value = *valp;
		return 0;

	case EXPR_OP_NAMED:
		valp = val_dict_get_name(arguments, node->u.name.s);
		if (valp == NULL)
			return -1;
		*ret_value = *valp;
		return 0;

	case EXPR_OP_SELF:
		*ret_value = *context;
		return 0;

	case EXPR_OP_CONST:
		*ret_value = node->u.value;
		return 0;

	case EXPR_OP_INDEX:
		return eval_index(node, context, arguments, ret_value);

	case EXPR_OP_UP:
		return eval_up(node, context, arguments, ret_value);

	case EXPR_OP_CALL1:
		return eval_cb1(node, context, arguments, ret_value);

	case EXPR_OP_CALL2:
		return eval_cb2(node, context, arguments, ret_value);
	}

	assert(!"Unknown node kind.");
	abort();
}

int
expr_eval_word(struct expr_node *node, struct value *context,
	       struct value_dict *arguments, long *ret_value)
{
	struct value val;
	if (expr_eval(node, context, arguments, &val) < 0)
		return -1;
	int ret = 0;
	if (value_extract_word(&val, ret_value, arguments) < 0)
		ret = -1;
	value_destroy(&val);
	return ret;
}

int
expr_eval_constant(struct expr_node *node, long *valuep)
{
	assert(expr_is_compile_constant(node));
	return expr_eval_word(node, NULL, NULL, valuep);
}

struct expr_node *
expr_self(void)
{
	static struct expr_node *nodep = NULL;
	if (nodep == NULL) {
		static struct expr_node node;
		expr_init_self(&node);
		nodep = &node;
	}
	return nodep;
}
