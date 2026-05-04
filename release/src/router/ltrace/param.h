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

#ifndef PARAM_H
#define PARAM_H

#include "forward.h"

/* The structure param holds information about a parameter of a
 * function.  It's used to configure a function prototype.  There are
 * two flavors of parameters:
 *
 *  - simple types
 *  - parameter packs
 *
 * Parameter packs are used to describe various vararg constructs.
 * They themselves are parametrized by ltrace expressions.  Those will
 * typically be references to other arguments, but constants might
 * also make sense, and it principle, anything can be used.  */

enum param_flavor {
	PARAM_FLAVOR_TYPE,
	PARAM_FLAVOR_PACK,

	/* This is for emitting arguments in two bunches.  This is
	 * where we should stop emitting "left" bunch.  All that's
	 * after this parameter should be emitted in the "right"
	 * bunch.  */
	PARAM_FLAVOR_STOP,
};

enum param_pack_flavor {
	/* This parameter pack expands to a list of ordinary
	 * arguments.  For example if the last argument is sometimes
	 * ignored, that would be described by a PARAM_PACK_ARGS
	 * parameter pack.  ioctl or ptrace are two examples that
	 * would benefit from this.  */
	PARAM_PACK_ARGS,

	/* This parameter pack represents a vararg argument.  */
	PARAM_PACK_VARARGS,
};

enum param_status {
	PPCB_ERR = -1,	/* An error occurred.  */
	PPCB_STOP,	/* Stop fetching the arguments.  */
	PPCB_CONT,	/* Display this argument and keep going.  */
};

/* Each parameter enumerator defines its own context object.
 * Definitions of these are in respective .c files of each
 * enumerator.  */
struct param_enum;

struct param {
	enum param_flavor flavor;
	union {
		struct {
			struct arg_type_info *type;
			int own_type;
		} type;
		struct {
			struct expr_node *args;
			size_t nargs;
			int own_args;
			enum param_pack_flavor ppflavor;

			struct param_enum *(*init)(struct value *cb_args,
						   size_t nargs,
						   struct value_dict *arguments);
			int (*next)(struct param_enum *self,
				    struct arg_type_info *info,
				    int *insert_stop);
			enum param_status (*stop)(struct param_enum *self,
						  struct value *value);
			void (*done)(struct param_enum *self);
		} pack;
	} u;
};

/* Initialize simple type parameter.  TYPE is owned and released by
 * PARAM if OWN_TYPE.  */
void param_init_type(struct param *param,
		     struct arg_type_info *type, int own_type);

/* Initialize a stop.  */
void param_init_stop(struct param *param);

/* Initialize parameter pack PARAM.  ARGS is an array of expressions
 * with parameters.  ARGS is owned and released by the pack if
 * OWN_ARGS.  NARGS is number of ARGS.
 *
 * When the parameter pack should be expanded, those expressions are
 * evaluated and passed to the INIT callback.  This has to return a
 * non-NULL context object.
 *
 * The NEXT callback is then called repeatedly, and should initialize
 * its INFOP argument to a type of the next parameter in the pack.
 * When there are no more parameters in the pack, the NEXT callback
 * will set INFOP to a VOID parameter.  If the callback sets
 * INSERT_STOP to a non-zero value, a stop parameter shall be inserted
 * before this actual parameter.
 *
 * Core then uses the passed-in type to fetch the next argument, which
 * is in turn passed to STOP callback.  This callback then tells
 * ltrace core what to do next: whether there are more arguments, and
 * if not, whether this argument should be displayed.
 *
 * After the enumeration is ended, DONE callback is called.  */
void param_init_pack(struct param *param, enum param_pack_flavor ppflavor,
		     struct expr_node *args, size_t nargs, int own_args,
		     struct param_enum *(*init)(struct value *cb_args,
						size_t nargs,
						struct value_dict *arguments),
		     int (*next)(struct param_enum *self,
				 struct arg_type_info *infop,
				 int *insert_stop),
		     enum param_status (*stop)(struct param_enum *self,
					       struct value *value),
		     void (*done)(struct param_enum *self));

/* Start enumerating types in parameter pack.  This evaluates the
 * parameter the pack arguments and calls the init callback.  See the
 * documentation of param_init_pack for details.  */
struct param_enum *param_pack_init(struct param *param,
				   struct value_dict *fargs);

/* Ask for next type in enumeration.  See the documentation of
 * param_init_pack for details.  */
int param_pack_next(struct param *param, struct param_enum *self,
		    struct arg_type_info *infop, int *insert_stop);

/* Ask whether we should stop enumerating.  See the documentation of
 * param_init_pack for details.  */
enum param_status param_pack_stop(struct param *param,
				  struct param_enum *self, struct value *value);

/* Finish enumerating types in parameter pack.  See the documentation
 * of param_init_pack for details.  */
void param_pack_done(struct param *param, struct param_enum *self);

/* Destroy data held by PARAM, but not the PARAM pointer itself.  */
void param_destroy(struct param *param);

#endif /* PARAM_H */
