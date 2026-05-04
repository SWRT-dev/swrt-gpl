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

#ifndef EXPR_H
#define EXPR_H

#include "value.h"
#include "value_dict.h"

/* Expressions serve as a way of encoding array lengths.  */

enum expr_node_kind {
	EXPR_OP_SELF,	/* reference to the variable in question  */
	EXPR_OP_NAMED,	/* value of named argument */
	EXPR_OP_ARGNO,	/* value of numbered argument */
	EXPR_OP_CONST,	/* constant value */
	EXPR_OP_INDEX,	/* A[B] */
	EXPR_OP_UP,	/* reference to containing structure */
	EXPR_OP_CALL1,	/* internal callback with one operand */
	EXPR_OP_CALL2,	/* internal callback with two operands */
};

struct expr_node {
	enum expr_node_kind kind;

	struct expr_node *lhs;
	int own_lhs;

	union {
		struct {
			const char *s;
			int own;
		} name;
		struct {
			struct expr_node *n;
			int own;
		} node;
		struct value value;
		size_t num;
		struct {
			union {
				int (*cb1)(struct value *ret_value,
					   struct value *lhs,
					   struct value_dict *arguments,
					   void *data);
				int (*cb2)(struct value *ret_value,
					   struct value *lhs,
					   struct value *rhs,
					   struct value_dict *arguments,
					   void *data);
			} u;
			void *data;
			struct expr_node *rhs;
			int own_rhs;
		} call;
	} u;
};

/* Expression of type self just returns the value in consideration.
 * For example, if what we seek is length of an array, then the value
 * representing that array is returned by the expression.  */
void expr_init_self(struct expr_node *node);

/* Expression that yields the value of an argument named NAME.  NAME
 * is owned if OWN_NAME.  */
void expr_init_named(struct expr_node *node,
		     const char *name, int own_name);

/* Expression that yields the value of an argument number NUM.  */
void expr_init_argno(struct expr_node *node, size_t num);

/* Constant expression always returns the same value VAL.  VAL is
 * copied into NODE and owned by it.  */
void expr_init_const(struct expr_node *node, struct value *val);
void expr_init_const_word(struct expr_node *node, long l,
			  struct arg_type_info *type, int own_type);

/* Expression LHS[RHS].  LHS and RHS are owned if, respectively,
 * OWN_LHS and OWN_RHS.  */
void expr_init_index(struct expr_node *node,
		     struct expr_node *lhs, int own_lhs,
		     struct expr_node *rhs, int own_rhs);

/* This expression returns the containing value of LHS (^LHS).  LHS is
 * owned if OWN_LHS.  */
void expr_init_up(struct expr_node *node, struct expr_node *lhs, int own_lhs);

/* Callback expression calls CB(eval(LHS), DATA).  LHS is owned if
 * OWN_LHS.  DATA is passed to callback verbatim.  */
void expr_init_cb1(struct expr_node *node,
		   int (*cb)(struct value *ret_value,
			     struct value *value,
			     struct value_dict *arguments,
			     void *data),
		   struct expr_node *lhs, int own_lhs, void *data);

/* Callback expression calls CB(eval(LHS), eval(RHS), DATA).  LHS and
 * RHS are owned if, respectively, OWN_LHS and OWN_RHS.  DATA is
 * passed to callback verbatim.  */
void expr_init_cb2(struct expr_node *node,
		   int (*cb)(struct value *ret_value,
			     struct value *lhs, struct value *rhs,
			     struct value_dict *arguments,
			     void *data),
		   struct expr_node *lhs, int own_lhs,
		   struct expr_node *rhs, int own_rhs, void *data);

/* Release the data inside NODE.  Doesn't free NODE itself.  */
void expr_destroy(struct expr_node *node);

/* Copy expression NODE into the area pointed to by RETP.  Return 0 on
 * success or a negative value on failure.  */
int expr_clone(struct expr_node *retp, const struct expr_node *node);

/* Evaluate the expression NODE in context of VALUE.  ARGUMENTS is a
 * dictionary of named and numbered values that NODE may use.  Returns
 * 0 in case of success or a negative value on error.  CONTEXT and
 * ARGUMENTS may be NULL, but then the expression mustn't need them
 * for evaluation.  */
int expr_eval(struct expr_node *node, struct value *context,
	      struct value_dict *arguments, struct value *ret_value);

/* Evaluate compile-time expression.  Returns 0 on success or negative
 * value on failure.  Computed value is passed back in *VALUEP.  */
int expr_eval_constant(struct expr_node *node, long *valuep);

/* Evaluate expression, whose result should fit into a word.  In order
 * to easily support all the structure and array accesses, we simply
 * operate on values represented by struct value.  But eventually we need
 * to be able to get out a word-size datum to use it as an index, a
 * length, etc.  */
int expr_eval_word(struct expr_node *node, struct value *context,
		   struct value_dict *arguments, long *ret_value);

/* Returns non-zero value if the expression is a compile-time
 * constant.  Currently this is only EXPR_OP_CONST, but eventually
 * things like sizeof or simple expressions might be allowed.  */
int expr_is_compile_constant(struct expr_node *node);

/* Returns a pre-computed expression "self".  */
struct expr_node *expr_self(void);

#endif /* EXPR_H */
