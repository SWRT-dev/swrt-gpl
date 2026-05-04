/*
 * This file is part of ltrace.
 * Copyright (C) 2011,2012,2013,2014,2015 Petr Machata, Red Hat Inc.
 * Copyright (C) 1998,1999,2003,2007,2008,2009 Juan Cespedes
 * Copyright (C) 2006 Ian Wienand
 * Copyright (C) 2006 Steve Fink
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

#define _GNU_SOURCE

#include "config.h"

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <assert.h>

#include "common.h"
#include "output.h"
#include "expr.h"
#include "param.h"
#include "printf.h"
#include "prototype.h"
#include "zero.h"
#include "type.h"
#include "lens.h"
#include "lens_default.h"
#include "lens_enum.h"

/* Lifted from GCC: The ctype functions are often implemented as
 * macros which do lookups in arrays using the parameter as the
 * offset.  If the ctype function parameter is a char, then gcc will
 * (appropriately) warn that a "subscript has type char".  Using a
 * (signed) char as a subscript is bad because you may get negative
 * offsets and thus it is not 8-bit safe.  The CTYPE_CONV macro
 * ensures that the parameter is cast to an unsigned char when a char
 * is passed in.  When an int is passed in, the parameter is left
 * alone so we don't lose EOF.  */

#define CTYPE_CONV(CH) \
  (sizeof(CH) == sizeof(unsigned char) ? (int)(unsigned char)(CH) : (int)(CH))

struct locus
{
	const char *filename;
	int line_no;
};

static struct arg_type_info *parse_nonpointer_type(struct protolib *plib,
						   struct locus *loc,
						   char **str,
						   struct param **extra_param,
						   size_t param_num,
						   int *ownp, int *forwardp);
static struct arg_type_info *parse_type(struct protolib *plib,
					struct locus *loc,
					char **str, struct param **extra_param,
					size_t param_num, int *ownp,
					int *forwardp);
static struct arg_type_info *parse_lens(struct protolib *plib,
					struct locus *loc,
					char **str, struct param **extra_param,
					size_t param_num, int *ownp,
					int *forwardp);
static int parse_enum(struct protolib *plib, struct locus *loc,
		      char **str, struct arg_type_info **retp, int *ownp);

static int try_parse_kwd(char **str, const char *kwd);

struct prototype *list_of_functions = NULL;

static int
parse_arg_type(char **name, enum arg_type *ret)
{
#define KEYWORD(KWD, TYPE)					\
	do {							\
		if (try_parse_kwd(name, KWD) >= 0) {		\
			*ret = TYPE;				\
			return 0;				\
		}						\
	} while (0)

	KEYWORD("void", ARGTYPE_VOID);
	KEYWORD("int", ARGTYPE_INT);
	KEYWORD("uint", ARGTYPE_UINT);
	KEYWORD("long", ARGTYPE_LONG);
	KEYWORD("ulong", ARGTYPE_ULONG);
	KEYWORD("char", ARGTYPE_CHAR);
	KEYWORD("short", ARGTYPE_SHORT);
	KEYWORD("ushort", ARGTYPE_USHORT);
	KEYWORD("float", ARGTYPE_FLOAT);
	KEYWORD("double", ARGTYPE_DOUBLE);
	KEYWORD("array", ARGTYPE_ARRAY);
	KEYWORD("struct", ARGTYPE_STRUCT);

	/* Misspelling of int used in ltrace.conf that we used to
	 * ship.  */
	KEYWORD("itn", ARGTYPE_INT);
#undef KEYWORD

	return -1;
}

static void
skip_whitespace(char **str) {
	for (; isspace(CTYPE_CONV(**str)); ++*str)
		;
}

static char *
xstrndup(char *str, size_t len) {
	char *ret = (char *) malloc(len + 1);
	if (ret == NULL) {
		report_global_error("malloc: %s", strerror(errno));
		return NULL;
	}
	strncpy(ret, str, len);
	ret[len] = 0;
	return ret;
}

static char *
parse_ident(struct locus *loc, char **str)
{
	char *ident = *str;

	if (!isalpha(CTYPE_CONV(**str)) && **str != '_') {
		report_error(loc->filename, loc->line_no, "bad identifier");
		return NULL;
	}

	while (**str && (isalnum(CTYPE_CONV(**str)) || **str == '_')) {
		++(*str);
	}

	return xstrndup(ident, *str - ident);
}

static int
parse_int(struct locus *loc, char **str, long *ret)
{
	char *end;
	long n = strtol(*str, &end, 0);
	if (end == *str) {
		report_error(loc->filename, loc->line_no, "bad number");
		return -1;
	}

	*str = end;
	if (ret != NULL)
		*ret = n;
	return 0;
}

static int
check_nonnegative(struct locus *loc, long l)
{
	if (l < 0) {
		report_error(loc->filename, loc->line_no,
			     "expected non-negative value, got %ld", l);
		return -1;
	}
	return 0;
}

static int
check_int(struct locus *loc, long l)
{
	int i = l;
	if ((long)i != l) {
		report_error(loc->filename, loc->line_no,
			     "Number too large: %ld", l);
		return -1;
	}
	return 0;
}

static int
parse_char(struct locus *loc, char **str, char expected)
{
	if (**str != expected) {
		report_error(loc->filename, loc->line_no,
			     "expected '%c', got '%c'", expected, **str);
		return -1;
	}

	++*str;
	return 0;
}

static char *
parse_string_literal(struct locus *loc, char **str)
{
	if (parse_char(loc, str, '"') < 0)
		return NULL;

	size_t len = strcspn(*str, "\"\n\\");
	char terminator = (*str)[len];

	if (terminator == '\0' || terminator == '\n') {
		report_error(loc->filename, loc->line_no, "unmatched quote");
		return NULL;
	}

	if (terminator == '\\') {
		/* Strings are currently only used for filenames, where
		 * there's usually nothing to escape. Nevertheless, we reserve
		 * the backslash in case strings become used in other contexts
		 * where escaping is necessary. */
		report_error(loc->filename, loc->line_no,
			     "backslashes in string literals reserved for future use");
		return NULL;
	}

	char *result = malloc(len + 1);
	if (!result) {
		report_error(loc->filename, loc->line_no,
			     "malloc: %s", strerror(errno));
		return NULL;
	}
	memcpy(result, *str, len);
	result[len] = '\0';
	(*str) += len + 1;
	return result;
}

static struct expr_node *parse_argnum(struct locus *loc,
				      char **str, int *ownp, int zero);

static struct expr_node *
parse_zero(struct locus *loc, char **str, int *ownp)
{
	skip_whitespace(str);
	if (**str == '(') {
		++*str;
		int own;
		struct expr_node *arg = parse_argnum(loc, str, &own, 0);
		if (arg == NULL)
			return NULL;
		if (parse_char(loc, str, ')') < 0) {
		fail:
			expr_destroy(arg);
			free(arg);
			return NULL;
		}

		struct expr_node *ret = build_zero_w_arg(arg, own);
		if (ret == NULL)
			goto fail;
		*ownp = 1;
		return ret;

	} else {
		*ownp = 0;
		return expr_node_zero();
	}
}

static int
wrap_in_zero(struct expr_node **nodep)
{
	struct expr_node *n = build_zero_w_arg(*nodep, 1);
	if (n == NULL)
		return -1;
	*nodep = n;
	return 0;
}

/*
 * Input:
 *  argN   : The value of argument #N, counting from 1
 *  eltN   : The value of element #N of the containing structure
 *  retval : The return value
 *  N      : The numeric value N
 */
static struct expr_node *
parse_argnum(struct locus *loc, char **str, int *ownp, int zero)
{
	struct expr_node *expr = malloc(sizeof(*expr));
	if (expr == NULL)
		return NULL;

	if (isdigit(CTYPE_CONV(**str))) {
		long l;
		if (parse_int(loc, str, &l) < 0
		    || check_nonnegative(loc, l) < 0
		    || check_int(loc, l) < 0)
			goto fail;

		expr_init_const_word(expr, l, type_get_simple(ARGTYPE_LONG), 0);

		if (zero && wrap_in_zero(&expr) < 0)
			goto fail;

		*ownp = 1;
		return expr;

	} else {
		char *const name = parse_ident(loc, str);
		if (name == NULL) {
		fail_ident:
			free(name);
			goto fail;
		}

		int is_arg = strncmp(name, "arg", 3) == 0;
		if (is_arg || strncmp(name, "elt", 3) == 0) {
			long l;
			char *num = name + 3;
			if (parse_int(loc, &num, &l) < 0
			    || check_int(loc, l) < 0)
				goto fail_ident;

			if (is_arg) {
				if (l == 0)
					expr_init_named(expr, "retval", 0);
				else
					expr_init_argno(expr, l - 1);
			} else {
				struct expr_node *e_up = malloc(sizeof(*e_up));
				struct expr_node *e_ix = malloc(sizeof(*e_ix));
				if (e_up == NULL || e_ix == NULL) {
					free(e_up);
					free(e_ix);
					goto fail_ident;
				}

				expr_init_up(e_up, expr_self(), 0);
				struct arg_type_info *ti
					= type_get_simple(ARGTYPE_LONG);
				expr_init_const_word(e_ix, l - 1, ti, 0);
				expr_init_index(expr, e_up, 1, e_ix, 1);
			}

		} else if (strcmp(name, "retval") == 0) {
			expr_init_named(expr, "retval", 0);

		} else if (strcmp(name, "zero") == 0) {
			struct expr_node *ret
				= parse_zero(loc, str, ownp);
			if (ret == NULL)
				goto fail_ident;
			free(expr);
			free(name);
			return ret;

		} else {
			report_error(loc->filename, loc->line_no,
				     "Unknown length specifier: '%s'", name);
			goto fail_ident;
		}

		if (zero && wrap_in_zero(&expr) < 0)
			goto fail_ident;

		free(name);
		*ownp = 1;
		return expr;
	}

fail:
	free(expr);
	return NULL;
}

static struct arg_type_info *
parse_typedef_name(struct protolib *plib, struct locus *loc, char **str)
{
	char *buf = parse_ident(loc, str);
	if (buf == NULL)
		return NULL;

	struct named_type *nt = protolib_lookup_type(plib, buf, true);
	free(buf);
	if (nt == NULL)
		return NULL;
	return nt->info;
}

static int
parse_typedef(struct protolib *plib, struct locus *loc, char **str)
{
	skip_whitespace(str);
	char *name = parse_ident(loc, str);

	/* Look through the typedef list whether we already have a
	 * forward of this type.  If we do, it must be forward
	 * structure.  */
	struct named_type *forward = protolib_lookup_type(plib, name, true);
	if (forward != NULL
	    && (forward->info->type != ARGTYPE_STRUCT
		|| !forward->forward)) {
		report_error(loc->filename, loc->line_no,
			     "Redefinition of typedef '%s'", name);
	err:
		free(name);
		return -1;
	}

	// Skip = sign
	skip_whitespace(str);
	if (parse_char(loc, str, '=') < 0)
		goto err;
	skip_whitespace(str);

	int fwd = 0;
	int own = 0;
	struct arg_type_info *info
		= parse_lens(plib, loc, str, NULL, 0, &own, &fwd);
	if (info == NULL)
		goto err;

	struct named_type this_nt;
	named_type_init(&this_nt, info, own);
	this_nt.forward = fwd;

	if (forward == NULL) {
		if (protolib_add_named_type(plib, name, 1, &this_nt) < 0) {
			named_type_destroy(&this_nt);
			goto err;
		}
		return 0;
	}

	/* If we are defining a forward, make sure the definition is a
	 * structure as well.  */
	if (this_nt.info->type != ARGTYPE_STRUCT) {
		report_error(loc->filename, loc->line_no,
			     "Definition of forward '%s' must be a structure.",
			     name);
		named_type_destroy(&this_nt);
		goto err;
	}

	/* Now move guts of the actual type over to the forward type.
	 * We can't just move pointers around, because references to
	 * forward must stay intact.  */
	assert(this_nt.own_type);
	type_destroy(forward->info);
	*forward->info = *this_nt.info;
	forward->forward = 0;
	free(this_nt.info);
	free(name);
	return 0;
}

/* Syntax: struct ( type,type,type,... ) */
static int
parse_struct(struct protolib *plib, struct locus *loc,
	     char **str, struct arg_type_info *info,
	     int *forwardp)
{
	skip_whitespace(str);

	if (**str == ';') {
		if (forwardp == NULL) {
			report_error(loc->filename, loc->line_no,
				     "Forward struct can be declared only "
				     "directly after a typedef.");
			return -1;
		}

		/* Forward declaration is currently handled as an
		 * empty struct.  */
		type_init_struct(info);
		*forwardp = 1;
		return 0;
	}

	if (parse_char(loc, str, '(') < 0)
		return -1;

	skip_whitespace(str); // Empty arg list with whitespace inside

	type_init_struct(info);

	while (1) {
		skip_whitespace(str);
		if (**str == 0 || **str == ')') {
			parse_char(loc, str, ')');
			return 0;
		}

		/* Field delimiter.  */
		if (type_struct_size(info) > 0)
			parse_char(loc, str, ',');

		skip_whitespace(str);
		int own;
		struct arg_type_info *field
			= parse_lens(plib, loc, str, NULL, 0, &own, NULL);
		if (field == NULL || type_struct_add(info, field, own)) {
			type_destroy(info);
			return -1;
		}
	}
}

/* Make a copy of INFO and set the *OWN bit if it's not already
 * owned.  */
static int
unshare_type_info(struct locus *loc, struct arg_type_info **infop, int *ownp)
{
	if (*ownp)
		return 0;

	struct arg_type_info *ninfo = malloc(sizeof(*ninfo));
	if (ninfo == NULL || type_clone(ninfo, *infop) < 0) {
		report_error(loc->filename, loc->line_no,
			     "malloc: %s", strerror(errno));
		free(ninfo);
		return -1;
	}
	*infop = ninfo;
	*ownp = 1;
	return 0;
}

static int
parse_string(struct protolib *plib, struct locus *loc,
	     char **str, struct arg_type_info **retp, int *ownp)
{
	struct arg_type_info *info = NULL;
	struct expr_node *length;
	int own_length;

	if (isdigit(CTYPE_CONV(**str))) {
		/* string0 is string[retval], length is zero(retval)
		 * stringN is string[argN], length is zero(argN) */
		long l;
		if (parse_int(loc, str, &l) < 0
		    || check_int(loc, l) < 0)
			return -1;

		struct expr_node *length_arg = malloc(sizeof(*length_arg));
		if (length_arg == NULL)
			return -1;

		if (l == 0)
			expr_init_named(length_arg, "retval", 0);
		else
			expr_init_argno(length_arg, l - 1);

		length = build_zero_w_arg(length_arg, 1);
		if (length == NULL) {
			expr_destroy(length_arg);
			free(length_arg);
			return -1;
		}
		own_length = 1;

	} else {
		skip_whitespace(str);
		if (**str == '[') {
			(*str)++;
			skip_whitespace(str);

			length = parse_argnum(loc, str, &own_length, 1);
			if (length == NULL)
				return -1;

			skip_whitespace(str);
			parse_char(loc, str, ']');

		} else if (**str == '(') {
			/* Usage of "string" as lens.  */
			++*str;

			skip_whitespace(str);
			info = parse_type(plib, loc, str, NULL, 0, ownp, NULL);
			if (info == NULL)
				return -1;

			length = NULL;
			own_length = 0;

			skip_whitespace(str);
			parse_char(loc, str, ')');

		} else {
			/* It was just a simple string after all.  */
			length = expr_node_zero();
			own_length = 0;
		}
	}

	/* String is a pointer to array of chars.  */
	if (info == NULL) {
		struct arg_type_info *info1 = malloc(sizeof(*info1));
		struct arg_type_info *info2 = malloc(sizeof(*info2));
		if (info1 == NULL || info2 == NULL) {
			free(info1);
			free(info2);
		fail:
			if (own_length) {
				assert(length != NULL);
				expr_destroy(length);
				free(length);
			}
			return -1;
		}
		type_init_array(info2, type_get_simple(ARGTYPE_CHAR), 0,
				length, own_length);
		type_init_pointer(info1, info2, 1);

		info = info1;
		*ownp = 1;
	}

	/* We'll need to set the lens, so unshare.  */
	if (unshare_type_info(loc, &info, ownp) < 0)
		/* If unshare_type_info failed, it must have been as a
		 * result of cloning attempt because *OWNP was 0.
		 * Thus we don't need to destroy INFO.  */
		goto fail;

	info->lens = &string_lens;
	info->own_lens = 0;

	*retp = info;
	return 0;
}

static int
build_printf_pack(struct locus *loc, struct param **packp, size_t param_num)
{
	if (packp == NULL) {
		report_error(loc->filename, loc->line_no,
			     "'format' type in unexpected context");
		return -1;
	}
	if (*packp != NULL) {
		report_error(loc->filename, loc->line_no,
			     "only one 'format' type per function supported");
		return -1;
	}

	*packp = malloc(sizeof(**packp));
	if (*packp == NULL)
		return -1;

	struct expr_node *node = malloc(sizeof(*node));
	if (node == NULL) {
		free(*packp);
		*packp = NULL;
		return -1;
	}

	expr_init_argno(node, param_num);

	param_pack_init_printf(*packp, node, 1);

	return 0;
}

/* Match and consume KWD if it's next in stream, and return 0.
 * Otherwise return negative number.  */
static int
try_parse_kwd(char **str, const char *kwd)
{
	size_t len = strlen(kwd);
	if (strncmp(*str, kwd, len) == 0
	    && !isalnum(CTYPE_CONV((*str)[len]))
	    && (*str)[len] != '_') {
		(*str) += len;
		return 0;
	}
	return -1;
}

/* XXX EXTRA_PARAM and PARAM_NUM are a kludge to get in
 * backward-compatible support for "format" parameter type.  The
 * latter is only valid if the former is non-NULL, which is only in
 * top-level context.  */
static int
parse_alias(struct protolib *plib, struct locus *loc,
	    char **str, struct arg_type_info **retp, int *ownp,
	    struct param **extra_param, size_t param_num)
{
	/* For backward compatibility, we need to support things like
	 * stringN (which is like string[argN], string[N], and also
	 * bare string.  We might, in theory, replace this by
	 * preprocessing configure file sources with M4, but for now,
	 * "string" is syntax.  */
	if (strncmp(*str, "string", 6) == 0) {
		(*str) += 6;
		return parse_string(plib, loc, str, retp, ownp);

	} else if (try_parse_kwd(str, "format") >= 0
		   && extra_param != NULL) {
		/* For backward compatibility, format is parsed as
		 * "string", but it smuggles to the parameter list of
		 * a function a "printf" argument pack with this
		 * parameter as argument.  */
		if (parse_string(plib, loc, str, retp, ownp) < 0)
			return -1;

		return build_printf_pack(loc, extra_param, param_num);

	} else if (try_parse_kwd(str, "enum") >= 0) {

		return parse_enum(plib, loc, str, retp, ownp);

	} else {
		*retp = NULL;
		return 0;
	}
}

/* Syntax: array ( type, N|argN ) */
static int
parse_array(struct protolib *plib, struct locus *loc,
	    char **str, struct arg_type_info *info)
{
	skip_whitespace(str);
	if (parse_char(loc, str, '(') < 0)
		return -1;

	skip_whitespace(str);
	int own;
	struct arg_type_info *elt_info
		= parse_lens(plib, loc, str, NULL, 0, &own, NULL);
	if (elt_info == NULL)
		return -1;

	skip_whitespace(str);
	parse_char(loc, str, ',');

	skip_whitespace(str);
	int own_length;
	struct expr_node *length = parse_argnum(loc, str, &own_length, 0);
	if (length == NULL) {
		if (own) {
			type_destroy(elt_info);
			free(elt_info);
		}
		return -1;
	}

	type_init_array(info, elt_info, own, length, own_length);

	skip_whitespace(str);
	parse_char(loc, str, ')');
	return 0;
}

/* Syntax:
 *   enum (keyname[=value],keyname[=value],... )
 *   enum<type> (keyname[=value],keyname[=value],... )
 */
static int
parse_enum(struct protolib *plib, struct locus *loc, char **str,
	   struct arg_type_info **retp, int *ownp)
{
	/* Optional type argument.  */
	skip_whitespace(str);
	if (**str == '[') {
		parse_char(loc, str, '[');
		skip_whitespace(str);
		*retp = parse_nonpointer_type(plib, loc, str, NULL, 0, ownp, 0);
		if (*retp == NULL)
			return -1;

		if (!type_is_integral((*retp)->type)) {
			report_error(loc->filename, loc->line_no,
				     "integral type required as enum argument");
		fail:
			if (*ownp) {
				/* This also releases associated lens
				 * if any was set so far.  */
				type_destroy(*retp);
				free(*retp);
			}
			return -1;
		}

		skip_whitespace(str);
		if (parse_char(loc, str, ']') < 0)
			goto fail;

	} else {
		*retp = type_get_simple(ARGTYPE_INT);
		*ownp = 0;
	}

	/* We'll need to set the lens, so unshare.  */
	if (unshare_type_info(loc, retp, ownp) < 0)
		goto fail;

	skip_whitespace(str);
	if (parse_char(loc, str, '(') < 0)
		goto fail;

	struct enum_lens *lens = malloc(sizeof(*lens));
	if (lens == NULL) {
		report_error(loc->filename, loc->line_no,
			     "malloc enum lens: %s", strerror(errno));
		return -1;
	}

	lens_init_enum(lens);
	(*retp)->lens = &lens->super;
	(*retp)->own_lens = 1;

	long last_val = 0;
	while (1) {
		skip_whitespace(str);
		if (**str == 0 || **str == ')') {
			parse_char(loc, str, ')');
			return 0;
		}

		/* Field delimiter.  XXX should we support the C
		 * syntax, where the enumeration can end in pending
		 * comma?  */
		if (lens_enum_size(lens) > 0)
			parse_char(loc, str, ',');

		skip_whitespace(str);
		char *key = parse_ident(loc, str);
		if (key == NULL) {
		err:
			free(key);
			goto fail;
		}

		if (**str == '=') {
			++*str;
			skip_whitespace(str);
			if (parse_int(loc, str, &last_val) < 0)
				goto err;
		}

		struct value *value = malloc(sizeof(*value));
		if (value == NULL)
			goto err;
		value_init_detached(value, NULL, *retp, 0);
		value_set_word(value, last_val);

		if (lens_enum_add(lens, key, 1, value, 1) < 0)
			goto err;

		last_val++;
	}

	return 0;
}

static struct arg_type_info *
parse_nonpointer_type(struct protolib *plib, struct locus *loc,
		      char **str, struct param **extra_param, size_t param_num,
		      int *ownp, int *forwardp)
{
	const char *orig_str = *str;
	enum arg_type type;
	if (parse_arg_type(str, &type) < 0) {
		struct arg_type_info *type;
		if (parse_alias(plib, loc, str, &type,
				ownp, extra_param, param_num) < 0)
			return NULL;
		else if (type != NULL)
			return type;

		*ownp = 0;
		if ((type = parse_typedef_name(plib, loc, str)) == NULL)
			report_error(loc->filename, loc->line_no,
				     "unknown type around '%s'", orig_str);
		return type;
	}

	/* For some types that's all we need.  */
	switch (type) {
	case ARGTYPE_VOID:
	case ARGTYPE_INT:
	case ARGTYPE_UINT:
	case ARGTYPE_LONG:
	case ARGTYPE_ULONG:
	case ARGTYPE_CHAR:
	case ARGTYPE_SHORT:
	case ARGTYPE_USHORT:
	case ARGTYPE_FLOAT:
	case ARGTYPE_DOUBLE:
		*ownp = 0;
		return type_get_simple(type);

	case ARGTYPE_ARRAY:
	case ARGTYPE_STRUCT:
		break;

	case ARGTYPE_POINTER:
		/* Pointer syntax is not based on keyword, so we
		 * should never get this type.  */
		assert(type != ARGTYPE_POINTER);
		abort();
	}

	struct arg_type_info *info = malloc(sizeof(*info));
	if (info == NULL) {
		report_error(loc->filename, loc->line_no,
			     "malloc: %s", strerror(errno));
		return NULL;
	}
	*ownp = 1;

	if (type == ARGTYPE_ARRAY) {
		if (parse_array(plib, loc, str, info) < 0) {
		fail:
			free(info);
			return NULL;
		}
	} else {
		assert(type == ARGTYPE_STRUCT);
		if (parse_struct(plib, loc, str, info, forwardp) < 0)
			goto fail;
	}

	return info;
}

static struct named_lens {
	const char *name;
	struct lens *lens;
} lenses[] = {
	{ "hide", &blind_lens },
	{ "octal", &octal_lens },
	{ "oct", &octal_lens },
	{ "bitvec", &bitvect_lens },
	{ "hex", &hex_lens },
	{ "bool", &bool_lens },
	{ "guess", &guess_lens },
};

static struct arg_type_info *
parse_type(struct protolib *plib, struct locus *loc, char **str,
	   struct param **extra_param, size_t param_num,
	   int *ownp, int *forwardp)
{
	struct arg_type_info *info
		= parse_nonpointer_type(plib, loc, str, extra_param,
					param_num, ownp, forwardp);
	if (info == NULL)
		return NULL;

	while (1) {
		skip_whitespace(str);
		if (**str == '*') {
			struct arg_type_info *outer = malloc(sizeof(*outer));
			if (outer == NULL) {
				if (*ownp) {
					type_destroy(info);
					free(info);
				}
				report_error(loc->filename, loc->line_no,
					     "malloc: %s", strerror(errno));
				return NULL;
			}
			type_init_pointer(outer, info, *ownp);
			*ownp = 1;
			(*str)++;
			info = outer;
		} else
			break;
	}
	return info;
}

static struct arg_type_info *
parse_lens(struct protolib *plib, struct locus *loc,
	   char **str, struct param **extra_param,
	   size_t param_num, int *ownp, int *forwardp)
{
	struct lens *lens = NULL; // N.B. unowned, should not be released.
	size_t i;
	for (i = 0; i < sizeof(lenses)/sizeof(*lenses); ++i)
		if (try_parse_kwd(str, lenses[i].name) == 0) {
			lens = lenses[i].lens;
			break;
		}

	int has_args = 1;
	struct arg_type_info *info;
	if (lens != NULL) {
		skip_whitespace(str);

		/* Octal lens gets special treatment, because of
		 * backward compatibility.  */
		if (lens == &octal_lens && **str != '(') {
			has_args = 0;
			info = type_get_simple(ARGTYPE_INT);
			*ownp = 0;
		} else if (parse_char(loc, str, '(') < 0) {
			report_error(loc->filename, loc->line_no,
				     "expected type argument after the lens");
			return NULL;
		}
	}

	if (has_args) {
		skip_whitespace(str);
		info = parse_type(plib, loc, str, extra_param, param_num,
				  ownp, forwardp);
		if (info == NULL)
			return NULL;
	}

	if (lens != NULL && has_args) {
		skip_whitespace(str);
		parse_char(loc, str, ')');
	}

	/* We can't modify shared types.  Make a copy if we have a
	 * lens.  */
	if (lens != NULL && unshare_type_info(loc, &info, ownp) < 0)
		return NULL;

	if (lens != NULL) {
		info->lens = lens;
		info->own_lens = 0;
	}

	return info;
}

static int
param_is_void(struct param *param)
{
	return param->flavor == PARAM_FLAVOR_TYPE
		&& param->u.type.type->type == ARGTYPE_VOID;
}

static struct arg_type_info *
get_hidden_int(void)
{
	static struct arg_type_info info, *pinfo = NULL;
	if (pinfo != NULL)
		return pinfo;

	info = *type_get_simple(ARGTYPE_INT);
	info.lens = &blind_lens;
	pinfo = &info;

	return pinfo;
}

static enum callback_status
void_to_hidden_int(struct prototype *proto, struct param *param, void *data)
{
	struct locus *loc = data;
	if (param_is_void(param)) {
		report_warning(loc->filename, loc->line_no,
			       "void parameter assumed to be 'hide(int)'");

		static struct arg_type_info *type = NULL;
		if (type == NULL)
			type = get_hidden_int();
		param_destroy(param);
		param_init_type(param, type, 0);
	}
	return CBS_CONT;
}

static void
parse_import(struct protolib_cache *cache, struct protolib *plib,
             struct locus *loc, char **str)
{
	skip_whitespace(str);

	char *file_name = parse_string_literal(loc, str);
	if (!file_name)
		return;

	skip_whitespace(str);
	if (parse_char(loc, str, ';') < 0) {
		free(file_name);
		return;
	}

	struct protolib *imported;

	if (protolib_cache_maybe_load(cache, file_name, 1, true, &imported) < 0) {
		free(file_name);
		return;
	}

	if (!imported) {
		report_error(loc->filename, loc->line_no,
			     "\"%s.conf\" not found", file_name);
		free(file_name);
		return;
	}

	if (protolib_add_import(plib, imported) < 0) {
		report_error(loc->filename, loc->line_no,
			     "import failed");
	}
}

static void
parse_prototype(struct protolib *plib, struct locus *loc, char **str)
{
	skip_whitespace(str);

	struct prototype fun;
	prototype_init(&fun);

	struct param *extra_param = NULL;
	char *proto_name = NULL;
	int own;
	fun.return_info = parse_lens(plib, loc, str, NULL, 0, &own, NULL);
	if (fun.return_info == NULL) {
	err:
		debug(3, " Skipping line %d", loc->line_no);

		if (extra_param != NULL) {
			param_destroy(extra_param);
			free(extra_param);
		}

		prototype_destroy(&fun);
		free(proto_name);
		return;
	}
	fun.own_return_info = own;
	debug(4, " return_type = %d", fun.return_info->type);

	skip_whitespace(str);
	proto_name = parse_ident(loc, str);
	if (proto_name == NULL)
		goto err;

	skip_whitespace(str);
	if (parse_char(loc, str, '(') < 0)
		goto err;
	debug(3, " name = %s", proto_name);

	int have_stop = 0;

	while (1) {
		skip_whitespace(str);
		if (**str == ')')
			break;

		if (**str == '+') {
			if (have_stop == 0) {
				struct param param;
				param_init_stop(&param);
				if (prototype_push_param(&fun, &param) < 0) {
				oom:
					report_error(loc->filename,
						     loc->line_no,
						     "%s", strerror(errno));
					goto err;
				}
				have_stop = 1;
			}
			(*str)++;
		}

		int own;
		size_t param_num = prototype_num_params(&fun) - have_stop;
		struct arg_type_info *type
			= parse_lens(plib, loc, str, &extra_param,
				     param_num, &own, NULL);
		if (type == NULL) {
			report_error(loc->filename, loc->line_no,
				     "unknown argument type");
			goto err;
		}

		struct param param;
		param_init_type(&param, type, own);
		if (prototype_push_param(&fun, &param) < 0)
			goto oom;

		skip_whitespace(str);
		if (**str == ',') {
			(*str)++;
			continue;
		} else if (**str == ')') {
			continue;
		} else {
			if ((*str)[strlen(*str) - 1] == '\n')
				(*str)[strlen(*str) - 1] = '\0';
			report_error(loc->filename, loc->line_no,
				     "syntax error around \"%s\"", *str);
			goto err;
		}
	}

	/* We used to allow void parameter as a synonym to an argument
	 * that shouldn't be displayed.  But backends really need to
	 * know the exact type that they are dealing with.  The proper
	 * way to do this these days is to use the hide lens.
	 *
	 * So if there are any voids in the parameter list, show a
	 * warning and assume that they are ints.  If there's a sole
	 * void, assume the function doesn't take any arguments.  The
	 * latter is conservative, we can drop the argument
	 * altogether, instead of fetching and then not showing it,
	 * without breaking any observable behavior.  */
	if (prototype_num_params(&fun) == 1
	    && param_is_void(prototype_get_nth_param(&fun, 0))) {
		if (0)
			/* Don't show this warning.  Pre-0.7.0
			 * ltrace.conf often used this idiom.  This
			 * should be postponed until much later, when
			 * extant uses are likely gone.  */
			report_warning(loc->filename, loc->line_no,
				       "sole void parameter ignored");
		prototype_destroy_nth_param(&fun, 0);
	} else {
		prototype_each_param(&fun, NULL, void_to_hidden_int, loc);
	}

	if (extra_param != NULL) {
		prototype_push_param(&fun, extra_param);
		free(extra_param);
		extra_param = NULL;
	}

	if (protolib_add_prototype(plib, proto_name, 1, &fun) < 0) {
		report_error(loc->filename, loc->line_no,
			     "couldn't add prototype: %s",
			     strerror(errno));
		goto err;
	}
}

static void
process_line(struct protolib_cache *cache, struct protolib *plib,
             struct locus *loc, char *buf)
{
	char *str = buf;

	debug(3, "Reading line %d of `%s'", loc->line_no, loc->filename);
	skip_whitespace(&str);

	/* A comment or empty line.  */
	if (*str == ';' || *str == 0 || *str == '\n' || *str == '#')
		return;

	if (try_parse_kwd(&str, "function") >= 0) {
		parse_prototype(plib, loc, &str);
		return;
	}

	if (try_parse_kwd(&str, "import") >= 0) {
		parse_import(cache, plib, loc, &str);
		return;
	}

	if (try_parse_kwd(&str, "typedef") >= 0) {
		parse_typedef(plib, loc, &str);
		return;
	}

	parse_prototype(plib, loc, &str);
}

int
read_config_file(FILE *stream, const char *path,
                 struct protolib_cache *cache, struct protolib *plib)
{
	debug(DEBUG_FUNCTION, "Reading config file `%s'...", path);

	struct locus loc = { path, 0 };
	char *line = NULL;
	size_t len = 0;
	while (getline(&line, &len, stream) >= 0) {
		loc.line_no++;
		process_line(cache, plib, &loc, line);
	}

	free(line);
	return 0;
}
