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

#ifndef VALUE_H
#define VALUE_H

#include "forward.h"
#include "sysdep.h"

/* Values are objects that capture data fetched from an inferior.
 * Typically a value is attached to a single inferior where it was
 * extracted from, but it is possible to create a detached value.
 * Each value is typed.  Values support a number of routines, such as
 * dereferencing if the value is of pointer type, array or structure
 * access, etc.
 *
 * A value can be uninitialized, abstract or reified.  Abstract values
 * are just references into inferior, no transfer has taken place yet.
 * Reified values have been copied out of the corresponding inferior,
 * or otherwise set to some value.  */

enum value_location_t {
	VAL_LOC_NODATA = 0,	/* Uninitialized.  */
	VAL_LOC_INFERIOR,	/* Value is in the inferior process.  */
	VAL_LOC_COPY,		/* Value was copied out of the inferior.  */
	VAL_LOC_SHARED,		/* Like VAL_LOC_COPY, but don't free.  */
	VAL_LOC_WORD,		/* Like VAL_LOC_COPY, but small enough.  */
};

struct value {
	struct arg_type_info *type;
	struct process *inferior;
	struct value *parent;
	size_t size;
	union {
		void *address;  /* VAL_LOC_COPY, VAL_LOC_SHARED */
		arch_addr_t inf_address;  /* VAL_LOC_INFERIOR */
		long value;     /* VAL_LOC_WORD */
		unsigned char buf[0];
	} u;
	enum value_location_t where;
	int own_type;
};

/* Initialize VALUE.  INFERIOR must not be NULL.  PARENT is parental
 * value, in case of compound types.  It may be NULL.  TYPE is a type
 * of the value.  It may be NULL if the type is not yet known.  If
 * OWN_TYPE, the passed-in type is owned and released by value.  */
void value_init(struct value *value, struct process *inferior,
		struct value *parent, struct arg_type_info *type,
		int own_type);

/* Initialize VALUE.  This is like value_init, except that inferior is
 * NULL.  VALP is initialized as a detached value, without assigned
 * process.  You have to be careful not to use VAL_LOC_INFERIOR
 * values if the value is detached.  */
void value_init_detached(struct value *value, struct value *parent,
			 struct arg_type_info *type, int own_type);

/* Set TYPE.  This releases old type if it was owned.  TYPE is owned
 * and released if OWN_TYPE.  */
void value_set_type(struct value *value,
		    struct arg_type_info *type, int own_type);

/* Transfers the ownership of VALUE's type, if any, to the caller.
 * This doesn't reset the VALUE's type, but gives up ownership if
 * there was one.  Previous ownership is passed in OWN_TYPE.  */
void value_take_type(struct value *value,
		     struct arg_type_info **type, int *own_type);

/* Release the data held by VALP, if any, but not the type.  */
void value_release(struct value *valp);

/* Value resides in inferior, on given ADDRESS.  */
void value_in_inferior(struct value *valp, arch_addr_t address);

/* Destroy the value.  This is like value_release, but it additionally
 * frees the value type, if it's own_type.  It doesn't free the VAL
 * pointer itself.  */
void value_destroy(struct value *val);

/* Set the data held by VALP to VALUE.  This also sets the value's
 * where to VAL_LOC_WORD.  */
void value_set_word(struct value *valp, long value);

/* Set the data held by VALP to a buffer of size SIZE.  This buffer
 * may be allocated by malloc.  Returns NULL on failure.  */
unsigned char *value_reserve(struct value *valp, size_t size);

/* Access ELEMENT-th field of the compound value VALP, and store the
 * result into the value RET_VAL.  Returns 0 on success, or negative
 * value on failure.  */
int value_init_element(struct value *ret_val, struct value *valp, size_t element);

/* De-reference pointer value, and store the result into the value
 * RET_VAL.  Returns 0 on success, or negative value on failure.  */
int value_init_deref(struct value *ret_val, struct value *valp);

/* If value is in inferior, copy it over to ltrace.  Return 0 for
 * success or negative value for failure.  */
int value_reify(struct value *val, struct value_dict *arguments);

/* Return a pointer to the data of the value.  This copies the data
 * from the inferior to the tracer.  Returns NULL on failure.  */
unsigned char *value_get_data(struct value *val, struct value_dict *arguments);

/* Return a pointer to the raw data of the value.  This shall not be
 * called on a VAL_LOC_INFERIOR value.  */
unsigned char *value_get_raw_data(struct value *val);

/* Copy value VAL into the area pointed-to by RETP.  Return 0 on
 * success or a negative value on failure.  */
int value_clone(struct value *retp, const struct value *val);

/* Give a size of given value.  Return (size_t)-1 for error.  This is
 * a full size of the value.  In particular for arrays, it returns
 * actual length of the array, as computed by the length
 * expression.  */
size_t value_size(struct value *val, struct value_dict *arguments);

/* Extract at most word-sized datum from the value.  Return 0 on
 * success or negative value on failure.  */
int value_extract_word(struct value *val, long *retp,
		       struct value_dict *arguments);

/* Copy contents of VAL to DATA.  The buffer must be large enough to
 * hold all the data inside.  */
int value_extract_buf(struct value *val, unsigned char *data,
		      struct value_dict *arguments);

/* Find the most enclosing parental value that is a struct.  Return
 * NULL when there is no such parental value.  */
struct value *value_get_parental_struct(struct value *val);

/* Determine whether this is all-zero value.  Returns >0 if it is, ==0
 * if it isn't, <0 on error.  */
int value_is_zero(struct value *val, struct value_dict *arguments);

/* Compare two values for byte-by-byte equality.  Returns >0 if they
 * are equal, ==0 if they are not, and <0 on error.  */
int value_equal(struct value *val1, struct value *val2,
		struct value_dict *arguments);

/* Convert a structure type to pointer to that structure type.  */
int value_pass_by_reference(struct value *value);

#endif /* VALUE_H */
