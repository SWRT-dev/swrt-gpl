/*
 * This file is part of ltrace.
 * Copyright (C) 2011,2012,2013,2015 Petr Machata, Red Hat Inc.
 * Copyright (C) 2007,2008 Juan Cespedes
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
#include <limits.h>

#include "type.h"
#include "sysdep.h"
#include "expr.h"
#include "lens.h"

struct arg_type_info *
type_get_simple(enum arg_type type)
{
#define HANDLE(T) {					\
		static struct arg_type_info t = { T };	\
	case T:						\
		return &t;				\
	}

	switch (type) {
	HANDLE(ARGTYPE_VOID)
	HANDLE(ARGTYPE_INT)
	HANDLE(ARGTYPE_UINT)
	HANDLE(ARGTYPE_LONG)
	HANDLE(ARGTYPE_ULONG)
	HANDLE(ARGTYPE_CHAR)
	HANDLE(ARGTYPE_SHORT)
	HANDLE(ARGTYPE_USHORT)
	HANDLE(ARGTYPE_FLOAT)
	HANDLE(ARGTYPE_DOUBLE)

#undef HANDLE

	case ARGTYPE_ARRAY:
	case ARGTYPE_STRUCT:
	case ARGTYPE_POINTER:
		assert(!"Not a simple type!");
	};
	abort();
}

struct arg_type_info *
type_get_voidptr(void)
{
	struct arg_type_info *void_info = type_get_simple(ARGTYPE_VOID);
	static struct arg_type_info *ret;
	if (ret == NULL) {
		static struct arg_type_info ptr_info;
		type_init_pointer(&ptr_info, void_info, 0);
		ret = &ptr_info;
	}
	return ret;
}

static void
type_init_common(struct arg_type_info *info, enum arg_type type)
{
	info->type = type;
	info->lens = NULL;
	info->own_lens = 0;
}

struct struct_field {
	struct arg_type_info *info;
	int own_info;
};

void
type_init_struct(struct arg_type_info *info)
{
	type_init_common(info, ARGTYPE_STRUCT);
	VECT_INIT(&info->u.entries, struct struct_field);
}

int
type_struct_add(struct arg_type_info *info,
		struct arg_type_info *field_info, int own)
{
	assert(info->type == ARGTYPE_STRUCT);
	assert(field_info->type != ARGTYPE_VOID);
	struct struct_field field = { field_info, own };
	return VECT_PUSHBACK(&info->u.entries, &field);
}

struct arg_type_info *
type_struct_get(struct arg_type_info *info, size_t idx)
{
	assert(info->type == ARGTYPE_STRUCT);
	return VECT_ELEMENT(&info->u.entries, struct struct_field, idx)->info;
}

size_t
type_struct_size(struct arg_type_info *info)
{
	assert(info->type == ARGTYPE_STRUCT);
	return vect_size(&info->u.entries);
}

static void
struct_field_dtor(struct struct_field *field, void *data)
{
	if (field->own_info) {
		type_destroy(field->info);
		free(field->info);
	}
}

static void
type_struct_destroy(struct arg_type_info *info)
{
	VECT_DESTROY(&info->u.entries, struct struct_field,
		     struct_field_dtor, NULL);
}

static int
layout_struct(struct process *proc, struct arg_type_info *info,
	      size_t *sizep, size_t *alignmentp, size_t *offsetofp)
{
	size_t sz = 0;
	size_t max_alignment = 0;
	size_t i;
	size_t offsetof_field = (size_t)-1;
	if (offsetofp != NULL)
		offsetof_field = *offsetofp;

	assert(info->type == ARGTYPE_STRUCT);
	for (i = 0; i < vect_size(&info->u.entries); ++i) {
		struct struct_field *field
			= VECT_ELEMENT(&info->u.entries,
				       struct struct_field, i);

		size_t alignment = type_alignof(proc, field->info);
		if (alignment == (size_t)-1)
			return -1;

		/* Add padding to SZ to align the next element.  */
		sz = align(sz, alignment);
		if (i == offsetof_field) {
			*offsetofp = sz;
			if (sizep == NULL && alignmentp == NULL)
				return 0;
		}

		size_t size = type_sizeof(proc, field->info);
		if (size == (size_t)-1)
			return -1;
		sz += size;

		if (alignment > max_alignment)
			max_alignment = alignment;
	}

	if (max_alignment > 0)
		sz = align(sz, max_alignment);

	if (sizep != NULL)
		*sizep = sz;

	if (alignmentp != NULL)
		*alignmentp = max_alignment;

	return 0;
}

void
type_init_array(struct arg_type_info *info,
		struct arg_type_info *element_info, int own_info,
		struct expr_node *length_expr, int own_length)
{
	type_init_common(info, ARGTYPE_ARRAY);
	info->u.array_info.elt_type = element_info;
	info->u.array_info.own_info = own_info;
	info->u.array_info.length = length_expr;
	info->u.array_info.own_length = own_length;
}

static void
type_array_destroy(struct arg_type_info *info)
{
	if (info->u.array_info.own_info) {
		type_destroy(info->u.array_info.elt_type);
		free(info->u.array_info.elt_type);
	}
	if (info->u.array_info.own_length) {
		expr_destroy(info->u.array_info.length);
		free(info->u.array_info.length);
	}
}

void
type_init_pointer(struct arg_type_info *info,
		  struct arg_type_info *pointee_info, int own_info)
{
	type_init_common(info, ARGTYPE_POINTER);
	info->u.ptr_info.info = pointee_info;
	info->u.ptr_info.own_info = own_info;
}

static void
type_pointer_destroy(struct arg_type_info *info)
{
	if (info->u.ptr_info.own_info) {
		type_destroy(info->u.ptr_info.info);
		free(info->u.ptr_info.info);
	}
}

void
type_destroy(struct arg_type_info *info)
{
	if (info == NULL)
		return;

	switch (info->type) {
	case ARGTYPE_STRUCT:
		type_struct_destroy(info);
		break;

	case ARGTYPE_ARRAY:
		type_array_destroy(info);
		break;

	case ARGTYPE_POINTER:
		type_pointer_destroy(info);
		break;

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
		break;
	}

	if (info->own_lens) {
		lens_destroy(info->lens);
		free(info->lens);
	}
}

static int
type_alloc_and_clone(struct arg_type_info **retpp,
		     struct arg_type_info *info, int own)
{
	*retpp = info;
	if (own) {
		*retpp = malloc(sizeof **retpp);
		if (*retpp == NULL || type_clone(*retpp, info) < 0) {
			free(*retpp);
			return -1;
		}
	}
	return 0;
}

static enum callback_status
clone_struct_add_field(const struct struct_field *field, void *data)
{
	struct arg_type_info *retp = data;
	struct arg_type_info *info;
	if (type_alloc_and_clone(&info, field->info, field->own_info) < 0) {
	fail:
		if (info != field->info)
			free(info);
		return CBS_STOP;
	}

	if (type_struct_add(retp, info, field->own_info) < 0) {
		if (field->own_info)
			type_destroy(info);
		goto fail;
	}

	return CBS_CONT;
}

int
type_clone(struct arg_type_info *retp, const struct arg_type_info *info)
{
	switch (info->type) {
	case ARGTYPE_STRUCT:
		type_init_struct(retp);
		if (VECT_EACH_CST(&info->u.entries, struct struct_field, NULL,
				  clone_struct_add_field, retp) != NULL) {
			type_destroy(retp);
			return -1;
		}
		break;

	case ARGTYPE_ARRAY:;
		struct arg_type_info *elt_type;
		if (type_alloc_and_clone(&elt_type, info->u.array_info.elt_type,
					 info->u.array_info.own_info) < 0)
			return -1;

		assert(!info->u.array_info.own_length); // XXXXXXX
		type_init_array(retp, elt_type, info->u.array_info.own_info,
				info->u.array_info.length,
				info->u.array_info.own_length);
		break;

	case ARGTYPE_POINTER:;
		struct arg_type_info *ninfo;
		if (type_alloc_and_clone(&ninfo, info->u.ptr_info.info,
					 info->u.ptr_info.own_info) < 0)
			return -1;
		type_init_pointer(retp, ninfo, info->u.ptr_info.own_info);
		break;

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
		*retp = *info;
		break;
	}

	assert(!info->own_lens);
	retp->lens = info->lens;
	retp->own_lens = info->own_lens;
	return 0;
}

#ifdef ARCH_HAVE_SIZEOF
size_t arch_type_sizeof(struct process *proc, struct arg_type_info *arg);
#else
size_t
arch_type_sizeof(struct process *proc, struct arg_type_info *arg)
{
	/* Use default value.  */
	return (size_t)-2;
}
#endif

#ifdef ARCH_HAVE_ALIGNOF
size_t arch_type_alignof(struct process *proc, struct arg_type_info *arg);
#else
size_t
arch_type_alignof(struct process *proc, struct arg_type_info *arg)
{
	/* Use default value.  */
	return (size_t)-2;
}
#endif

/* We need to support alignments that are not power of two.  E.g. long
 * double on x86 has alignment of 12.  */
size_t
align(size_t sz, size_t alignment)
{
	assert(alignment != 0);

	if ((sz % alignment) != 0)
		sz = ((sz / alignment) + 1) * alignment;

	return sz;
}

size_t
type_sizeof(struct process *proc, struct arg_type_info *type)
{
	size_t arch_size = arch_type_sizeof(proc, type);
	if (arch_size != (size_t)-2)
		return arch_size;

	switch (type->type) {
		size_t size;
	case ARGTYPE_CHAR:
		return sizeof(char);

	case ARGTYPE_SHORT:
	case ARGTYPE_USHORT:
		return sizeof(short);

	case ARGTYPE_INT:
	case ARGTYPE_UINT:
		return sizeof(int);

	case ARGTYPE_LONG:
	case ARGTYPE_ULONG:
		return sizeof(long);

	case ARGTYPE_FLOAT:
		return sizeof(float);

	case ARGTYPE_DOUBLE:
		return sizeof(double);

	case ARGTYPE_STRUCT:
		if (layout_struct(proc, type, &size, NULL, NULL) < 0)
			return (size_t)-1;
		return size;

	case ARGTYPE_POINTER:
		return sizeof(void *);

	case ARGTYPE_ARRAY:
		if (expr_is_compile_constant(type->u.array_info.length)) {
			long l;
			if (expr_eval_constant(type->u.array_info.length,
					       &l) < 0)
				return -1;

			struct arg_type_info *elt_ti
				= type->u.array_info.elt_type;

			size_t elt_size = type_sizeof(proc, elt_ti);
			if (elt_size == (size_t)-1)
				return (size_t)-1;

			return ((size_t)l) * elt_size;

		} else {
			/* Flexible arrays don't count into the
			 * sizeof.  */
			return 0;
		}

	case ARGTYPE_VOID:
		return 0;
	}

	abort();
}

#undef alignof
#define alignof(field,st) ((size_t) ((char*) &st.field - (char*) &st))

size_t
type_alignof(struct process *proc, struct arg_type_info *type)
{
	size_t arch_alignment = arch_type_alignof(proc, type);
	if (arch_alignment != (size_t)-2)
		return arch_alignment;

	struct { char c; char C; } cC;
	struct { char c; short s; } cs;
	struct { char c; int i; } ci;
	struct { char c; long l; } cl;
	struct { char c; void* p; } cp;
	struct { char c; float f; } cf;
	struct { char c; double d; } cd;

	static size_t char_alignment = alignof(C, cC);
	static size_t short_alignment = alignof(s, cs);
	static size_t int_alignment = alignof(i, ci);
	static size_t long_alignment = alignof(l, cl);
	static size_t ptr_alignment = alignof(p, cp);
	static size_t float_alignment = alignof(f, cf);
	static size_t double_alignment = alignof(d, cd);

	switch (type->type) {
		size_t alignment;
	case ARGTYPE_LONG:
	case ARGTYPE_ULONG:
		return long_alignment;
	case ARGTYPE_CHAR:
		return char_alignment;
	case ARGTYPE_SHORT:
	case ARGTYPE_USHORT:
		return short_alignment;
	case ARGTYPE_FLOAT:
		return float_alignment;
	case ARGTYPE_DOUBLE:
		return double_alignment;
	case ARGTYPE_POINTER:
		return ptr_alignment;

	case ARGTYPE_ARRAY:
		return type_alignof(proc, type->u.array_info.elt_type);

	case ARGTYPE_STRUCT:
		if (layout_struct(proc, type, NULL, &alignment, NULL) < 0)
			return (size_t)-1;
		return alignment;

	default:
		return int_alignment;
	}
}

size_t
type_offsetof(struct process *proc, struct arg_type_info *type, size_t emt)
{
	assert(type->type == ARGTYPE_STRUCT
	       || type->type == ARGTYPE_ARRAY);

	switch (type->type) {
		size_t alignment;
		size_t size;
	case ARGTYPE_ARRAY:
		alignment = type_alignof(proc, type->u.array_info.elt_type);
		if (alignment == (size_t)-1)
			return (size_t)-1;

		size = type_sizeof(proc, type->u.array_info.elt_type);
		if (size == (size_t)-1)
			return (size_t)-1;

		return emt * align(size, alignment);

	case ARGTYPE_STRUCT:
		if (layout_struct(proc, type, NULL, NULL, &emt) < 0)
			return (size_t)-1;
		return emt;

	default:
		abort();
	}
}

struct arg_type_info *
type_element(struct arg_type_info *info, size_t emt)
{
	assert(info->type == ARGTYPE_STRUCT
	       || info->type == ARGTYPE_ARRAY);

	switch (info->type) {
	case ARGTYPE_ARRAY:
		return info->u.array_info.elt_type;

	case ARGTYPE_STRUCT:
		assert(emt < type_struct_size(info));
		return type_struct_get(info, emt);

	default:
		abort();
	}
}

size_t
type_aggregate_size(struct arg_type_info *info)
{
	assert(info->type == ARGTYPE_STRUCT
	       || info->type == ARGTYPE_ARRAY);

	switch (info->type) {
		long ret;
	case ARGTYPE_ARRAY:
		if (expr_eval_constant(info->u.array_info.length, &ret) < 0)
			return (size_t)-1;
		return (size_t)ret;

	case ARGTYPE_STRUCT:
		return type_struct_size(info);

	default:
		abort();
	}
}

int
type_is_integral(enum arg_type type)
{
	switch (type) {
	case ARGTYPE_INT:
	case ARGTYPE_UINT:
	case ARGTYPE_LONG:
	case ARGTYPE_ULONG:
	case ARGTYPE_CHAR:
	case ARGTYPE_SHORT:
	case ARGTYPE_USHORT:
		return 1;

	case ARGTYPE_VOID:
	case ARGTYPE_FLOAT:
	case ARGTYPE_DOUBLE:
	case ARGTYPE_ARRAY:
	case ARGTYPE_STRUCT:
	case ARGTYPE_POINTER:
		return 0;
	}
	abort();
}

int
type_is_signed(enum arg_type type)
{
	assert(type_is_integral(type));

	switch (type) {
	case ARGTYPE_CHAR:
		return CHAR_MIN != 0;

	case ARGTYPE_SHORT:
	case ARGTYPE_INT:
	case ARGTYPE_LONG:
		return 1;

	case ARGTYPE_UINT:
	case ARGTYPE_ULONG:
	case ARGTYPE_USHORT:
		return 0;

	case ARGTYPE_VOID:
	case ARGTYPE_FLOAT:
	case ARGTYPE_DOUBLE:
	case ARGTYPE_ARRAY:
	case ARGTYPE_STRUCT:
	case ARGTYPE_POINTER:
		abort();
	}
	abort();
}

struct arg_type_info *
type_get_fp_equivalent(struct arg_type_info *info)
{
	/* Extract innermost structure.  Give up early if any
	 * component has more than one element.  */
	while (info->type == ARGTYPE_STRUCT) {
		if (type_struct_size(info) != 1)
			return NULL;
		info = type_element(info, 0);
	}

	switch (info->type) {
	case ARGTYPE_CHAR:
	case ARGTYPE_SHORT:
	case ARGTYPE_INT:
	case ARGTYPE_LONG:
	case ARGTYPE_UINT:
	case ARGTYPE_ULONG:
	case ARGTYPE_USHORT:
	case ARGTYPE_VOID:
	case ARGTYPE_ARRAY:
	case ARGTYPE_POINTER:
		return NULL;

	case ARGTYPE_FLOAT:
	case ARGTYPE_DOUBLE:
		return info;

	case ARGTYPE_STRUCT:
		abort();
	}
	abort();
}

struct arg_type_info *
type_get_hfa_type(struct arg_type_info *info, size_t *countp)
{
	assert(info != NULL);
	if (info->type != ARGTYPE_STRUCT
	    && info->type != ARGTYPE_ARRAY)
		return NULL;

	size_t n = type_aggregate_size(info);
	if (n == (size_t)-1)
		return NULL;

	struct arg_type_info *ret = NULL;
	*countp = 0;

	while (n-- > 0) {
		struct arg_type_info *emt = type_element(info, n);

		size_t emt_count = 1;
		if (emt->type == ARGTYPE_STRUCT || emt->type == ARGTYPE_ARRAY)
			emt = type_get_hfa_type(emt, &emt_count);
		if (emt == NULL)
			return NULL;
		if (ret == NULL) {
			if (emt->type != ARGTYPE_FLOAT
			    && emt->type != ARGTYPE_DOUBLE)
				return NULL;
			ret = emt;
		}
		if (emt->type != ret->type)
			return NULL;
		*countp += emt_count;
	}
	return ret;
}
