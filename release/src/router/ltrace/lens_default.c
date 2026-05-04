/*
 * This file is part of ltrace.
 * Copyright (C) 2011,2012,2013 Petr Machata, Red Hat Inc.
 * Copyright (C) 1998,2004,2007,2008,2009 Juan Cespedes
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

#define _XOPEN_SOURCE /* For wcwidth from wchar.h.  */

#include <ctype.h>
#include <stdlib.h>
#include <assert.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

#include "bits.h"
#include "expr.h"
#include "lens_default.h"
#include "options.h"
#include "output.h"
#include "type.h"
#include "value.h"
#include "zero.h"

#define READER(NAME, TYPE)						\
	static int							\
	NAME(struct value *value, TYPE *ret, struct value_dict *arguments) \
	{								\
		union {							\
			TYPE val;					\
			unsigned char buf[0];				\
		} u;							\
		if (value_extract_buf(value, u.buf, arguments) < 0)	\
			return -1;					\
		*ret = u.val;						\
		return 0;						\
	}

READER(read_float, float)
READER(read_double, double)

#undef READER

#define HANDLE_WIDTH(BITS)						\
	do {								\
		long l;							\
		if (value_extract_word(value, &l, arguments) < 0)	\
			return -1;					\
		int##BITS##_t i = l;					\
		uint64_t v = (uint64_t)(uint##BITS##_t)i;		\
		switch (format) {					\
		case INT_FMT_unknown:					\
			if (l < -10000 || l > 10000)			\
		case INT_FMT_x:						\
			return fprintf(stream, "%#"PRIx64, v);		\
		case INT_FMT_i:						\
		case INT_FMT_default:					\
			return fprintf(stream, "%"PRIi##BITS, i);	\
		case INT_FMT_u:						\
			return fprintf(stream, "%"PRIu64, v);		\
		case INT_FMT_o:						\
			return fprintf(stream, "0%"PRIo64, v);		\
		}							\
	} while (0)

enum int_fmt_t
{
	INT_FMT_i,
	INT_FMT_u,
	INT_FMT_o,
	INT_FMT_x,
	INT_FMT_unknown,
	INT_FMT_default,
};

static int
format_integer(FILE *stream, struct value *value, enum int_fmt_t format,
	       struct value_dict *arguments)
{
	switch (type_sizeof(value->inferior, value->type)) {

	case 1: HANDLE_WIDTH(8);
	case 2: HANDLE_WIDTH(16);
	case 4: HANDLE_WIDTH(32);
	case 8: HANDLE_WIDTH(64);

	default:
		assert(!"unsupported integer width");
		abort();

	case -1:
		return -1;
	}
}

#undef HANDLE_WIDTH

static int
acc_fprintf(int *countp, FILE *stream, const char *format, ...)
{
	va_list pa;
	va_start(pa, format);
	int i = account_output(countp, vfprintf(stream, format, pa));
	va_end(pa);

	return i;
}

static int
print_char(FILE *stream, int c)
{
	const char *fmt;
	switch (c) {
	case -1:
		fmt = "EOF";
		break;
	case 0:
		fmt = "\\0";
		break;
	case '\a':
		fmt = "\\a";
		break;
	case '\b':
		fmt = "\\b";
		break;
	case '\t':
		fmt = "\\t";
		break;
	case '\n':
		fmt = "\\n";
		break;
	case '\v':
		fmt = "\\v";
		break;
	case '\f':
		fmt = "\\f";
		break;
	case '\r':
		fmt = "\\r";
		break;
	case '\\':
		fmt = "\\\\";
		break;
	default:
		if (isprint(c) || c == ' ')
			fmt = "%c";
		else
			fmt = "\\%03o";
	}

	return fprintf(stream, fmt, c);
}

static int
format_char(FILE *stream, struct value *value, struct value_dict *arguments)
{
	long lc;
	if (value_extract_word(value, &lc, arguments) < 0)
		return -1;
	return print_char(stream, (int) lc);
}

static int
format_naked(FILE *stream, struct value *value,
	     struct value_dict *arguments,
	     int (*what)(FILE *, struct value *, struct value_dict *))
{
	int written = 0;
	if (acc_fprintf(&written, stream, "'") < 0
	    || account_output(&written,
			      what(stream, value, arguments)) < 0
	    || acc_fprintf(&written, stream, "'") < 0)
		return -1;

	return written;
}

static int
format_double(FILE *stream, double value, enum int_fmt_t format)
{
	if (format == INT_FMT_x)
		return fprintf(stream, "%a", value);
	else
		return fprintf(stream, "%f", value);
}

static int
format_floating(FILE *stream, struct value *value, struct value_dict *arguments,
		enum int_fmt_t format)
{
	switch (value->type->type) {
		float f;
		double d;
	case ARGTYPE_FLOAT:
		if (read_float(value, &f, arguments) < 0)
			return -1;
		return format_double(stream, f, format);
	case ARGTYPE_DOUBLE:
		if (read_double(value, &d, arguments) < 0)
			return -1;
		return format_double(stream, d, format);
	default:
		abort();
	}
}

struct format_argument_data
{
	struct value *value;
	struct value_dict *arguments;
};

static int
format_argument_cb(FILE *stream, void *ptr)
{
	struct format_argument_data *data = ptr;
	return format_argument(stream, data->value, data->arguments);
}

static int
format_struct(FILE *stream, struct value *value, struct value_dict *arguments)
{
	int written = 0;
	if (acc_fprintf(&written, stream, "{ ") < 0)
		return -1;

	int need_delim = 0;
	size_t i;
	for (i = 0; i < type_struct_size(value->type); ++i) {
		struct value element;
		if (value_init_element(&element, value, i) < 0)
			return -1;

		struct format_argument_data data = { &element, arguments };
		int o = delim_output(stream, &need_delim,
				     format_argument_cb, &data);
		value_destroy(&element);
		if (o < 0)
			return -1;

		written += o;
	}
	if (acc_fprintf(&written, stream, " }") < 0)
		return -1;
	return written;
}

static const char null_message[] = "nil";
int
format_pointer(FILE *stream, struct value *value, struct value_dict *arguments)
{
	if (value_is_zero(value, arguments))
		return fprintf(stream, null_message);

	/* The following is for detecting recursion.  We keep track of
	 * the values that were already displayed.  Each time a
	 * pointer should be dereferenced, we compare its value to the
	 * value of each of the pointers dereferenced so far.  If one
	 * of them matches, instead of recursing, we just printf which
	 * superstructure this pointer recurses to.  */
	static struct vect pointers = {};
	if (pointers.elt_size == 0)
		VECT_INIT(&pointers, struct value *);

	/* Trim number of expanded structures of the same type.  Even
	 * for non-recursive structure, we don't want to expand all of
	 * it if it's huge.  */
	size_t i;
	size_t len = vect_size(&pointers);
	assert(value->type->type == ARGTYPE_POINTER);
	struct arg_type_info *pointee = value->type->u.ptr_info.info;
	if (pointee->type == ARGTYPE_STRUCT) {
		size_t depth = 0;
		for (i = 0; i < len; ++i) {
			struct value *old
				= *VECT_ELEMENT(&pointers, struct value *, i);
			assert(old->type->type == ARGTYPE_POINTER);
			struct arg_type_info *old_pointee
				= old->type->u.ptr_info.info;
			if (old_pointee == pointee)
				depth++;
		}
		if (depth >= options.arraylen)
			return fprintf(stream, "...");
	}

	for (i = len; i-- > 0 ;) {
		struct value **old = VECT_ELEMENT(&pointers, struct value *, i);
		int rc = value_equal(value, *old, arguments);
		if (rc < 0)
			return -1;
		if (rc > 0) {
			size_t reclevel = len - i - 1;
			char buf[reclevel + 1];
			memset(buf, '^', sizeof buf);
			buf[reclevel] = 0;
			return fprintf(stream, "recurse%s", buf);
		}
	}

	/* OK, not a recursion.  Remember this value for tracking.  */
	if (VECT_PUSHBACK(&pointers, &value) < 0)
		return -1;

	struct value element;
	int o;
	if (value_init_deref(&element, value) < 0) {
		o = -1;
		goto done;
	}
	o = format_argument(stream, &element, arguments);
	value_destroy(&element);

done:
	VECT_POPBACK(&pointers, struct value *, NULL, NULL);
	return o;
}

/*
 * LENGTH is an expression whose evaluation will yield the actual
 *    length of the array.
 *
 * MAXLEN is the actual maximum length that we care about
 *
 * BEFORE if LENGTH>MAXLEN, we display ellipsis.  We display it before
 *    the closing parenthesis if BEFORE, otherwise after it.
 *
 * OPEN, CLOSE, DELIM are opening and closing parenthesis and element
 *    delimiter.
 */
static int
format_array(FILE *stream, struct value *value, struct value_dict *arguments,
	     struct expr_node *length, size_t maxlen, int before,
	     const char *open, const char *close, const char *delim)
{
	/* We need "long" to be long enough to cover the whole address
	 * space.  */
	(void)sizeof(char[1 - 2*(sizeof(long) < sizeof(void *))]);
	long l;
	if (expr_eval_word(length, value, arguments, &l) < 0)
		return -1;
	size_t len = (size_t)l;

	int written = 0;
	if (acc_fprintf(&written, stream, "%s", open) < 0)
		return -1;

	size_t i;
	for (i = 0; i < len && i <= maxlen; ++i) {
		if (i == maxlen) {
			if (before && acc_fprintf(&written, stream, "...") < 0)
				return -1;
			break;
		}

		if (i > 0 && acc_fprintf(&written, stream, "%s", delim) < 0)
			return -1;

		struct value element;
		if (value_init_element(&element, value, i) < 0)
			return -1;
		int o = format_argument(stream, &element, arguments);
		value_destroy(&element);
		if (o < 0)
			return -1;
		written += o;
	}
	if (acc_fprintf(&written, stream, "%s", close) < 0)
		return -1;
	if (i == maxlen && !before && acc_fprintf(&written, stream, "...") < 0)
		return -1;

	return written;
}

static int
toplevel_format_lens(struct lens *lens, FILE *stream,
		     struct value *value, struct value_dict *arguments,
		     enum int_fmt_t int_fmt)
{
	switch (value->type->type) {
	case ARGTYPE_VOID:
		return fprintf(stream, "<void>");

	case ARGTYPE_SHORT:
	case ARGTYPE_INT:
	case ARGTYPE_LONG:
		return format_integer(stream, value, int_fmt, arguments);

	case ARGTYPE_USHORT:
	case ARGTYPE_UINT:
	case ARGTYPE_ULONG:
		if (int_fmt == INT_FMT_i || int_fmt == INT_FMT_default)
			int_fmt = INT_FMT_u;
		return format_integer(stream, value, int_fmt, arguments);

	case ARGTYPE_CHAR:
		if (int_fmt == INT_FMT_default)
			return format_naked(stream, value, arguments,
					    &format_char);
		return format_integer(stream, value, int_fmt, arguments);

	case ARGTYPE_FLOAT:
	case ARGTYPE_DOUBLE:
		return format_floating(stream, value, arguments, int_fmt);

	case ARGTYPE_STRUCT:
		return format_struct(stream, value, arguments);

	case ARGTYPE_POINTER:
		if (value_is_zero(value, arguments))
			return fprintf(stream, null_message);
		if (value->type->u.ptr_info.info->type != ARGTYPE_VOID)
			return format_pointer(stream, value, arguments);
		return format_integer(stream, value, INT_FMT_x, arguments);

	case ARGTYPE_ARRAY:
		return format_array(stream, value, arguments,
				    value->type->u.array_info.length,
				    options.arraylen, 1, "[ ", " ]", ", ");
	}
	abort();
}

static int
default_lens_format_cb(struct lens *lens, FILE *stream,
		       struct value *value, struct value_dict *arguments)
{
	return toplevel_format_lens(lens, stream, value, arguments,
				    INT_FMT_default);
}

struct lens default_lens = {
	.format_cb = default_lens_format_cb,
};


static int
blind_lens_format_cb(struct lens *lens, FILE *stream,
		     struct value *value, struct value_dict *arguments)
{
	return 0;
}

struct lens blind_lens = {
	.format_cb = blind_lens_format_cb,
};


static int
octal_lens_format_cb(struct lens *lens, FILE *stream,
		     struct value *value, struct value_dict *arguments)
{
	return toplevel_format_lens(lens, stream, value, arguments, INT_FMT_o);
}

struct lens octal_lens = {
	.format_cb = octal_lens_format_cb,
};


static int
hex_lens_format_cb(struct lens *lens, FILE *stream,
		   struct value *value, struct value_dict *arguments)
{
	return toplevel_format_lens(lens, stream, value, arguments, INT_FMT_x);
}

struct lens hex_lens = {
	.format_cb = hex_lens_format_cb,
};


static int
dec_lens_format_cb(struct lens *lens, FILE *stream,
		   struct value *value, struct value_dict *arguments)
{
	return toplevel_format_lens(lens, stream, value, arguments, INT_FMT_u);
}

struct lens dec_lens = {
	.format_cb = dec_lens_format_cb,
};


static int
guess_lens_format_cb(struct lens *lens, FILE *stream,
		     struct value *value, struct value_dict *arguments)
{
	return toplevel_format_lens(lens, stream, value, arguments,
				    INT_FMT_unknown);
}

struct lens guess_lens = {
	.format_cb = guess_lens_format_cb,
};


static int
bool_lens_format_cb(struct lens *lens, FILE *stream,
		    struct value *value, struct value_dict *arguments)
{
	switch (value->type->type) {
	case ARGTYPE_VOID:
	case ARGTYPE_FLOAT:
	case ARGTYPE_DOUBLE:
	case ARGTYPE_STRUCT:
	case ARGTYPE_POINTER:
	case ARGTYPE_ARRAY:
		return toplevel_format_lens(lens, stream, value,
					    arguments, INT_FMT_default);

		int zero;
	case ARGTYPE_SHORT:
	case ARGTYPE_INT:
	case ARGTYPE_LONG:
	case ARGTYPE_USHORT:
	case ARGTYPE_UINT:
	case ARGTYPE_ULONG:
	case ARGTYPE_CHAR:
		if ((zero = value_is_zero(value, arguments)) < 0)
			return -1;
		if (zero)
			return fprintf(stream, "false");
		else
			return fprintf(stream, "true");
	}
	abort();
}

struct lens bool_lens = {
	.format_cb = bool_lens_format_cb,
};

static int
redispatch_as_array(struct lens *lens, FILE *stream,
		    struct value *value, struct value_dict *arguments,
		    int (*cb)(struct lens *, FILE *,
			      struct value *, struct value_dict *))
{
	struct arg_type_info info[2];
	type_init_array(&info[1], value->type->u.ptr_info.info, 0,
			expr_node_zero(), 0);
	type_init_pointer(&info[0], &info[1], 0);
	info->lens = lens;
	info->own_lens = 0;
	struct value tmp;
	if (value_clone(&tmp, value) < 0)
		return -1;
	value_set_type(&tmp, info, 0);
	int ret = cb(lens, stream, &tmp, arguments);
	type_destroy(&info[0]);
	type_destroy(&info[1]);
	value_destroy(&tmp);
	return ret;
}

static int
format_wchar(FILE *stream, struct value *value, struct value_dict *arguments)
{
	long l;
	if (value_extract_word(value, &l, arguments) < 0)
		return -1;
	wchar_t wc = (wchar_t) l;
	char buf[MB_CUR_MAX + 1];

	int c = wctomb(buf, wc);
	if (c < 0)
		return -1;
	if (c == 1)
		return print_char(stream, buf[0]);

	buf[c] = 0;
	if (fprintf(stream, "%s", buf) < 0)
		return -1;

	c = wcwidth(wc);
	return c >= 0 ? c : 0;
}

static int
string_lens_format_cb(struct lens *lens, FILE *stream,
		      struct value *value, struct value_dict *arguments)
{
	switch (value->type->type) {
	case ARGTYPE_POINTER:
		/* This should really be written as either "string",
		 * or, if lens, then string(array(char, zero)*).  But
		 * I suspect people are so used to the char * C idiom,
		 * that string(char *) might actually turn up.  So
		 * let's just support it.  */
		switch ((int) value->type->u.ptr_info.info->type)
		case ARGTYPE_CHAR:
		case ARGTYPE_SHORT:
		case ARGTYPE_USHORT:
		case ARGTYPE_INT:
		case ARGTYPE_UINT:
		case ARGTYPE_LONG:
		case ARGTYPE_ULONG:
			return redispatch_as_array(lens, stream, value,
						   arguments,
						   &string_lens_format_cb);

		/* Otherwise dispatch to whatever the default for the
		 * pointee is--most likely this will again be us.  */
		/* Fall through.  */
	case ARGTYPE_VOID:
	case ARGTYPE_FLOAT:
	case ARGTYPE_DOUBLE:
	case ARGTYPE_STRUCT:
		return toplevel_format_lens(lens, stream, value,
					    arguments, INT_FMT_default);

	case ARGTYPE_SHORT:
	case ARGTYPE_INT:
	case ARGTYPE_LONG:
	case ARGTYPE_USHORT:
	case ARGTYPE_UINT:
	case ARGTYPE_ULONG:
		if (value->parent != NULL && value->type->lens == NULL)
			return format_wchar(stream, value, arguments);
		else
			return format_naked(stream, value, arguments,
					    &format_wchar);

	case ARGTYPE_CHAR:
		return format_char(stream, value, arguments);

	case ARGTYPE_ARRAY:
		return format_array(stream, value, arguments,
				    value->type->u.array_info.length,
				    options.strlen, 0, "\"", "\"", "");
	}
	abort();
}

struct lens string_lens = {
	.format_cb = string_lens_format_cb,
};

static int
out_bits(FILE *stream, size_t low, size_t high)
{
	if (low == high)
		return fprintf(stream, "%zd", low);
	else
		return fprintf(stream, "%zd-%zd", low, high);
}

static int
bitvect_lens_format_cb(struct lens *lens, FILE *stream,
		       struct value *value, struct value_dict *arguments)
{
	unsigned char *data = value_get_data(value, arguments);
	if (data == NULL)
		return -1;
	size_t sz = type_sizeof(value->inferior, value->type);
	if (sz == (size_t)-1)
		return -1;

	size_t i;
	unsigned char buf[sz];
	switch ((int)value->type->type) {
		union bitvect_integral_64
		{
			uint8_t u8;
			uint16_t u16;
			uint32_t u32;
			uint64_t u64;
			unsigned char buf[0];
		} bv;

	case ARGTYPE_POINTER:
		return format_pointer(stream, value, arguments);

	case ARGTYPE_STRUCT:
	case ARGTYPE_ARRAY:
		break;

	default:
		assert(sz <= sizeof(bv));
		memmove(bv.buf, data, sz);

		if (sz == 1)
			bv.u64 = bv.u8;
		else if (sz == 2)
			bv.u64 = bv.u16;
		else if (sz == 4)
			bv.u64 = bv.u32;

		for (i = 0; i < sz; ++i) {
			buf[i] = bv.u64 & 0xff;
			bv.u64 >>= 8;
		}
		data = buf;
	}

	size_t bits = 0;
	for (i = 0; i < sz; ++i)
		bits += bitcount(data[i]);

	/* If there's more 1's than 0's, show inverse.  */
	unsigned neg = bits > sz * 4 ? 0xff : 0x00;

	int o = 0;
	if (acc_fprintf(&o, stream, "%s<", &"~"[neg == 0x00]) < 0)
		return -1;

	size_t bitno = 0;
	ssize_t low = -1;
	for (i = 0; i < sz; ++i) {
		unsigned char m;
		unsigned char d = data[i] ^ neg;
		for (m = 0x01; m != 0; m <<= 1) {
			int bit = !!(m & d);
			if (low < 0) {
				if (bit) {
					if (low == -2
					    && acc_fprintf(&o, stream, ",") < 0)
						return -1;
					low = bitno;
				}
			} else if (!bit) {
				if (account_output(&o, out_bits(stream, low,
								bitno-1)) < 0)
					return -1;
				low = -2;
			}
			bitno++;
		}
	}
	if (low >= 0 && account_output(&o, out_bits(stream, low, bitno-1)) < 0)
		return -1;

	if (fputc('>', stream) < 0)
		return -1;
	o += 1;

	return o;
}

struct lens bitvect_lens = {
	.format_cb = bitvect_lens_format_cb,
};
