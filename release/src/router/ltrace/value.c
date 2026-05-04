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

#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include "value.h"
#include "type.h"
#include "common.h"
#include "expr.h"
#include "backend.h"

static void
value_common_init(struct value *valp, struct process *inferior,
		  struct value *parent, struct arg_type_info *type,
		  int own_type)
{
	valp->type = type;
	valp->own_type = own_type;
	valp->inferior = inferior;
	memset(&valp->u, 0, sizeof(valp->u));
	valp->where = VAL_LOC_NODATA;
	valp->parent = parent;
	valp->size = (size_t)-1;
}

void
value_init(struct value *valp, struct process *inferior, struct value *parent,
	   struct arg_type_info *type, int own_type)
{
	assert(inferior != NULL);
	value_common_init(valp, inferior, parent, type, own_type);
}

void
value_init_detached(struct value *valp, struct value *parent,
		    struct arg_type_info *type, int own_type)
{
	value_common_init(valp, NULL, parent, type, own_type);
}

void
value_set_type(struct value *value, struct arg_type_info *type, int own_type)
{
	if (value->own_type) {
		type_destroy(value->type);
		free(value->type);
	}
	value->type = type;
	value->own_type = own_type;
}

void
value_take_type(struct value *value, struct arg_type_info **type,
		int *own_type)
{
	*type = value->type;
	*own_type = value->own_type;
	value->own_type = 0;
}

void
value_release(struct value *val)
{
	if (val == NULL)
		return;
	if (val->where == VAL_LOC_COPY) {
		free(val->u.address);
		val->where = VAL_LOC_NODATA;
	}
}

void
value_destroy(struct value *val)
{
	if (val == NULL)
		return;
	value_release(val);
	value_set_type(val, NULL, 0);
}

unsigned char *
value_reserve(struct value *valp, size_t size)
{
	value_release(valp);

	if (size <= sizeof(valp->u.value)) {
		valp->where = VAL_LOC_WORD;
		valp->u.value = 0;
	} else {
		valp->where = VAL_LOC_COPY;
		valp->u.address = calloc(size, 1);
		if (valp->u.address == 0)
			return NULL;
	}
	return value_get_raw_data(valp);
}

void
value_in_inferior(struct value *valp, arch_addr_t address)
{
	value_release(valp);
	valp->where = VAL_LOC_INFERIOR;
	valp->u.address = address;
}

int
value_reify(struct value *val, struct value_dict *arguments)
{
	if (val->where != VAL_LOC_INFERIOR)
		return 0;
	assert(val->inferior != NULL);

	size_t size = value_size(val, arguments);
	if (size == (size_t)-1)
		return -1;

	void *data;
	enum value_location_t nloc;
	if (size <= sizeof(val->u.value)) {
		data = &val->u.value;
		nloc = VAL_LOC_WORD;
	} else {
		data = malloc(size);
		if (data == NULL)
			return -1;
		nloc = VAL_LOC_COPY;
	}

	if (umovebytes(val->inferior, val->u.inf_address, data, size) < size) {
		if (nloc == VAL_LOC_COPY)
			free(data);
		return -1;
	}

	val->where = nloc;
	if (nloc == VAL_LOC_COPY)
		val->u.address = data;

	return 0;
}

unsigned char *
value_get_data(struct value *val, struct value_dict *arguments)
{
	if (value_reify(val, arguments) < 0)
		return NULL;
	return value_get_raw_data(val);
}

unsigned char *
value_get_raw_data(struct value *val)
{
	switch (val->where) {
	case VAL_LOC_INFERIOR:
		abort();
	case VAL_LOC_NODATA:
		return NULL;
	case VAL_LOC_COPY:
	case VAL_LOC_SHARED:
		return val->u.address;
	case VAL_LOC_WORD:
		return val->u.buf;
	}

	assert(!"Unexpected value of val->where");
	abort();
}

int
value_clone(struct value *retp, const struct value *val)
{
	*retp = *val;

	if (val->own_type) {
		retp->type = malloc(sizeof(struct arg_type_info));
		if (type_clone (retp->type, val->type) < 0) {
			free(retp->type);
			return -1;
		}
	}

	if (val->where == VAL_LOC_COPY) {
		assert(val->inferior != NULL);
		size_t size = type_sizeof(val->inferior, val->type);
		if (size == (size_t)-1) {
		fail:
			if (retp->own_type) {
				type_destroy(retp->type);
				free(retp->type);
			}
			return -1;
		}

		retp->u.address = malloc(size);
		if (retp->u.address == NULL)
			goto fail;

		memcpy(retp->u.address, val->u.address, size);
	}

	return 0;
}

size_t
value_size(struct value *val, struct value_dict *arguments)
{
	if (val->size != (size_t)-1)
		return val->size;

	if (val->type->type != ARGTYPE_ARRAY)
		return val->size = type_sizeof(val->inferior, val->type);

	struct value length;
	if (expr_eval(val->type->u.array_info.length, val,
		      arguments, &length) < 0)
		return (size_t)-1;

	size_t l;
	int o = value_extract_word(&length, (long *)&l, arguments);
	value_destroy(&length);

	if (o < 0)
		return (size_t)-1;

	size_t elt_size = type_sizeof(val->inferior,
				      val->type->u.array_info.elt_type);
	if (elt_size == (size_t)-1)
		return (size_t)-1;

	return val->size = elt_size * l;
}

int
value_init_element(struct value *ret_val, struct value *val, size_t element)
{
	size_t off = type_offsetof(val->inferior, val->type, element);
	if (off == (size_t)-1)
		return -1;

	struct arg_type_info *e_info = type_element(val->type, element);
	if (e_info == NULL)
		return -1;

	value_common_init(ret_val, val->inferior, val, e_info, 0);

	switch (val->where) {
	case VAL_LOC_COPY:
	case VAL_LOC_SHARED:
		ret_val->u.address = val->u.address + off;
		ret_val->where = VAL_LOC_SHARED;
		return 0;

	case VAL_LOC_WORD:
		ret_val->u.address = value_get_raw_data(val) + off;
		ret_val->where = VAL_LOC_SHARED;
		return 0;

	case VAL_LOC_INFERIOR:
		ret_val->u.inf_address = val->u.inf_address + off;
		ret_val->where = VAL_LOC_INFERIOR;
		return 0;

	case VAL_LOC_NODATA:
		assert(!"Can't offset NODATA.");
		abort();
	}
	abort();
}

int
value_init_deref(struct value *ret_val, struct value *valp)
{
	assert(valp->type->type == ARGTYPE_POINTER);

	/* Note: extracting a pointer value should not need value_dict
	 * with function arguments.  */
	long l;
	if (value_extract_word(valp, &l, NULL) < 0)
		return -1;

	/* We need "long" to be long enough to hold platform
	 * pointers.  */
	(void)sizeof(char[1 - 2*(sizeof(l) < sizeof(void *))]);

	value_common_init(ret_val, valp->inferior, valp,
			  valp->type->u.ptr_info.info, 0);
	ret_val->u.value = l; /* Set the address.  */
	ret_val->where = VAL_LOC_INFERIOR;
	return 0;
}

/* The functions value_extract_buf and value_extract_word assume that
 * data in VALUE is stored at the start of the internal buffer.  For
 * value_extract_buf in particular there's no other reasonable
 * default.  If we need to copy out four bytes, they need to be the
 * bytes pointed to by the buffer pointer.
 *
 * But actually the situation is similar for value_extract_word as
 * well.  This function is used e.g. to extract characters from
 * strings.  Those weren't stored by value_set_word, they might still
 * be in client for all we know.  So value_extract_word has to assume
 * that the whole of data is data is stored at the buffer pointer.
 *
 * This is a problem on big endian machines, where 2-byte quantity
 * carried in 4- or 8-byte long is stored at the end of that long.
 * (Though that quantity itself is still big endian.)  So we need to
 * make a little dance to shift the value to the right part of the
 * buffer.  */

union word_data {
	uint8_t u8;
	uint16_t u16;
	uint32_t u32;
	uint64_t u64;
	long l;
	unsigned char buf[0];
} u;

void
value_set_word(struct value *value, long word)
{
	size_t sz = type_sizeof(value->inferior, value->type);
	assert(sz != (size_t)-1);
	assert(sz <= sizeof(value->u.value));

	value->where = VAL_LOC_WORD;

	union word_data u = {};

	switch (sz) {
	case 0:
		u.l = 0;
		break;
	case 1:
		u.u8 = word;
		break;
	case 2:
		u.u16 = word;
		break;
	case 4:
		u.u32 = word;
		break;
	case 8:
		u.u64 = word;
		break;
	default:
		assert(sz != sz);
		abort();
	}

	value->u.value = u.l;
}

static int
value_extract_buf_sz(struct value *value, unsigned char *tgt, size_t sz,
		     struct value_dict *arguments)
{
	unsigned char *data = value_get_data(value, arguments);
	if (data == NULL)
		return -1;

	memcpy(tgt, data, sz);
	return 0;
}

int
value_extract_word(struct value *value, long *retp,
		   struct value_dict *arguments)
{
	size_t sz = type_sizeof(value->inferior, value->type);
	if (sz == (size_t)-1)
		return -1;
	assert(sz <= sizeof(value->u.value));

	if (sz == 0) {
		*retp = 0;
		return 0;
	}

	union word_data u = {};
	if (value_extract_buf_sz(value, u.buf, sz, arguments) < 0)
		return -1;

	switch (sz) {
	case 1:
		*retp = (long)u.u8;
		return 0;
	case 2:
		*retp = (long)u.u16;
		return 0;
	case 4:
		*retp = (long)u.u32;
		return 0;
	case 8:
		*retp = (long)u.u64;
		return 0;
	default:
		assert(sz != sz);
		abort();
	}
}

int
value_extract_buf(struct value *value, unsigned char *tgt,
		  struct value_dict *arguments)
{
	size_t sz = type_sizeof(value->inferior, value->type);
	if (sz == (size_t)-1)
		return -1;

	return value_extract_buf_sz(value, tgt, sz, arguments);
}

struct value *
value_get_parental_struct(struct value *val)
{
	struct value *parent;
	for (parent = val->parent; parent != NULL; parent = parent->parent)
		if (parent->type->type == ARGTYPE_STRUCT)
			return parent;
	return NULL;
}

int
value_is_zero(struct value *val, struct value_dict *arguments)
{
	unsigned char *data = value_get_data(val, arguments);
	if (data == NULL)
		return -1;
	size_t sz = type_sizeof(val->inferior, val->type);
	if (sz == (size_t)-1)
		return -1;

	int zero = 1;
	size_t j;
	for (j = 0; j < sz; ++j) {
		if (data[j] != 0) {
			zero = 0;
			break;
		}
	}
	return zero;
}

int
value_equal(struct value *val1, struct value *val2,
	    struct value_dict *arguments)
{
	size_t sz1 = type_sizeof(val1->inferior, val1->type);
	size_t sz2 = type_sizeof(val2->inferior, val2->type);
	if (sz1 == (size_t)-1 || sz2 == (size_t)-1)
		return -1;
	if (sz1 != sz2)
		return 0;

	unsigned char *data1 = value_get_data(val1, arguments);
	unsigned char *data2 = value_get_data(val2, arguments);
	if (data1 == NULL || data2 == NULL)
		return -1;
	return memcmp(data1, data2, sz1) == 0 ? 1 : 0;
}

int
value_pass_by_reference(struct value *value)
{
	assert(value != NULL);
	assert(value->type->type == ARGTYPE_STRUCT);

	struct arg_type_info *new_info = calloc(sizeof(*new_info), 1);
	if (new_info == NULL)
		return -1;

	int own;
	struct arg_type_info *orig;
	value_take_type(value, &orig, &own);
	type_init_pointer(new_info, orig, own);
	new_info->lens = orig->lens;
	value_set_type(value, new_info, 1);

	return 0;
}
