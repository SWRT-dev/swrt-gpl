/* Copyright Dima Kogan <dima@secretsauce.net>
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of version 2 of the GNU General Public License as published by the
 * Free Software Foundation.
 *
 */
#include <stdio.h>
#include <elfutils/libdwfl.h>
#include <dwarf.h>
#include <inttypes.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "config.h"
#include "debug.h"
#include "dict.h"
#include "expr.h"
#include "filter.h"
#include "lens.h"
#include "lens_default.h"
#include "lens_enum.h"
#include "library.h"
#include "options.h"
#include "param.h"
#include "prototype.h"
#include "type.h"
#include "value.h"

#define complain(die, format, ...)					\
	debug(DEBUG_FUNCTION, "%s() die '%s' @ 0x%" PRIx64 ": " format, \
	      __func__, dwarf_diename(die), dwarf_dieoffset(die),	\
	      ##__VA_ARGS__)

#define NEXT_SIBLING(die)					\
	int res = dwarf_siblingof(die, die);			\
	if (res == 0) continue;     /* sibling exists    */	\
	if (res < 0)  return false; /* error             */	\
	break                       /* no sibling exists */

static struct arg_type_info *get_type(int *newly_allocated_info,
				      Dwarf_Die *type_die,
				      struct protolib *plib,
				      struct dict *type_dieoffset_hash);




// debugging functions to dump types that I already parsed
#ifdef DUMP_PROTOTYPES
static bool _dump_ltrace_tree(const struct arg_type_info *info, int indent)
{
	if (indent > 7) {
		fprintf(stderr, "%*s%p ...\n", indent*4, "", (void*)info);
		return true;
	}

	if (info == NULL) {
		fprintf(stderr, "%*s%p NULL\n", indent*4, "", (void*)info);
		return true;
	}

	switch (info->type) {
	case ARGTYPE_VOID:
		fprintf(stderr, "%*s%p void\n", indent*4, "", (void*)info);
		break;

	case ARGTYPE_INT:
	case ARGTYPE_UINT:
	case ARGTYPE_LONG:
	case ARGTYPE_ULONG:
	case ARGTYPE_CHAR:
	case ARGTYPE_SHORT:
	case ARGTYPE_USHORT:
	case ARGTYPE_FLOAT:
	case ARGTYPE_DOUBLE:
		fprintf(stderr, "%*s%p base\n", indent*4, "", (void*)info);
		break;

	case ARGTYPE_ARRAY:
		fprintf(stderr, "%*s%p array. elements not printed\n", indent*4,
			"", (void*)info);
		break;

	case ARGTYPE_POINTER:
		fprintf(stderr, "%*s%p pointer to...\n", indent*4,
			"", (void*)info);
		_dump_ltrace_tree(info->u.ptr_info.info, indent+1);
		break;

	case ARGTYPE_STRUCT:
		fprintf(stderr, "%*s%p struct...\n", indent*4,
			"", (void*)info);
		struct struct_field
		{
			struct arg_type_info *info;
			int own_info;
		} *elements = (struct struct_field*)info->u.entries.data;
		unsigned int i;
		for(i=0; i<info->u.entries.size; i++)
			_dump_ltrace_tree(elements[i].info, indent+1);
		break;

	default:
		fprintf(stderr, "%*s%p unknown type\n", indent*4,
			"", (void*)info);
		return false;;
	}

	return true;
}

static bool dump_ltrace_tree(const struct arg_type_info *info)
{
	return _dump_ltrace_tree(info, 0);
}
#endif


// pulls a numerical value out of a particular attribute in a die. Returns true
// if successful. The result is returned in *result. Note that this is cast to
// (uint64_t), regardless of the actual type of the input
static bool get_die_numeric(uint64_t *result,
			    Dwarf_Die *die, unsigned int attr_name)
{
	Dwarf_Attribute attr ;

	union {
		Dwarf_Word udata;
		Dwarf_Sword sdata;
		Dwarf_Addr addr;
		bool flag;
	} u;

	if (dwarf_attr(die, attr_name, &attr) == NULL)
		return false;

	unsigned int form = dwarf_whatform(&attr);

#define PROCESS_NUMERIC(type)				\
	if (dwarf_form ## type(&attr, &u.type) != 0)	\
		return false;				\
	*result = (uint64_t)u.type;			\
	return true


	switch (form) {
	case DW_FORM_addr:
		PROCESS_NUMERIC(addr);

	case DW_FORM_data1:
	case DW_FORM_data2:
	case DW_FORM_data4:
	case DW_FORM_data8:
	case DW_FORM_udata:
		PROCESS_NUMERIC(udata);

	case DW_FORM_sdata:
		PROCESS_NUMERIC(sdata);

	case DW_FORM_flag:
		PROCESS_NUMERIC(flag);

	default:
		complain(die, "Unknown numeric form %d for attr_name: %d",
			 form, attr_name);
		return false;
	}
#undef PROCESS_NUMERIC
}

static bool get_integer_base_type(enum arg_type *type, int byte_size,
				  bool is_signed)
{
	// not using a switch() here because sizeof(int) == sizeof(long) on some
	// architectures, and removing that case for those arches is a pain
	if (byte_size == sizeof(char)) {
		*type = ARGTYPE_CHAR;
		return true;
	}

	if (byte_size == sizeof(short)) {
		*type = is_signed ? ARGTYPE_SHORT : ARGTYPE_USHORT;
		return true;
	}

	if (byte_size == sizeof(int)) {
		*type = is_signed ? ARGTYPE_INT : ARGTYPE_UINT;
		return true;
	}

	if (byte_size == sizeof(long)) {
		*type = is_signed ? ARGTYPE_LONG : ARGTYPE_ULONG;
		return true;
	}

	return false;
}

// returns an ltrace ARGTYPE_XXX base type from the given die. If we dont
// support a particular type (or an error occurred), I regturn ARGTYPE_VOID
static enum arg_type get_base_type(Dwarf_Die *die)
{
	uint64_t encoding;
	if (!get_die_numeric(&encoding, die, DW_AT_encoding))
		return ARGTYPE_VOID;

	if (encoding == DW_ATE_void)
		return ARGTYPE_VOID;

	if (encoding == DW_ATE_signed_char || encoding == DW_ATE_unsigned_char)
		return ARGTYPE_CHAR;

	uint64_t byte_size;
	if (!get_die_numeric(&byte_size, die, DW_AT_byte_size))
		return ARGTYPE_VOID;

	if (encoding == DW_ATE_signed   ||
	    encoding == DW_ATE_unsigned ||
	    encoding == DW_ATE_boolean) {

		bool is_signed = (encoding == DW_ATE_signed);

		enum arg_type type;
		if (!get_integer_base_type(&type, (int)byte_size, is_signed)) {
			complain(die, "Unknown integer base type. "
				 "Using 'void'");
			return ARGTYPE_VOID;
		}
		return type;
	}

	if (encoding == DW_ATE_float) {
		switch (byte_size) {
		case sizeof(float):
			return ARGTYPE_FLOAT;

		case sizeof(double):
			return ARGTYPE_DOUBLE;

		default:
			// things like long doubles. ltrace has no support yet,
			// so I just say "void"
			return ARGTYPE_VOID;
		}
	}

#if 0
	if (encoding == DW_ATE_complex_float) {
		switch (byte_size) {
		case 2*sizeof(float):
			return ARGTYPE_FLOAT;

		case 2*sizeof(double):
			return ARGTYPE_DOUBLE;

		default:
			// things like long doubles. ltrace has no support yet,
			// so I just say "void"
			return ARGTYPE_VOID;
		}
	}
#endif

	// Unknown encoding. I just say void
	complain(die, "Unknown base type. Returning 'void'");
	return ARGTYPE_VOID;
}

static bool get_type_die(Dwarf_Die *type_die, Dwarf_Die *die)
{
	Dwarf_Attribute attr;
	return
		dwarf_attr(die, DW_AT_type, &attr) != NULL &&
		dwarf_formref_die(&attr, type_die) != NULL;
}



// type_dieoffset_hash dictionary callbacks
static size_t dwarf_die_hash(const void *x)
{
	return *(const Dwarf_Off*)x;
}
static int dwarf_die_eq(const void *a, const void *b)
{
	return *(const Dwarf_Off*)a == *(const Dwarf_Off*)b;
}


// returns a newly-allocated art_type_info*, or NULL on error
static struct arg_type_info *get_enum(Dwarf_Die *parent,
				      struct dict *type_dieoffset_hash)
{

#define CLEANUP_AND_RETURN_ERROR(ret) do {				\
		if (dupkey != NULL)					\
			free((void*)dupkey);				\
		if (value != NULL) {					\
			value_destroy(value);				\
			free(value);					\
		}							\
		if (lens != NULL) {					\
			lens_destroy(&lens->super);			\
			free(lens);					\
		}							\
		if (result != NULL) {					\
			type_destroy(result);				\
			free(result);					\
		}							\
		dict_erase (type_dieoffset_hash, &die_offset, NULL,	\
			    NULL, NULL);				\
		dict_insert(type_dieoffset_hash, &die_offset,		\
			    &(struct arg_type_info*){			\
				    type_get_simple(ARGTYPE_VOID)});	\
		return ret;						\
	} while (0)

	struct arg_type_info *result = NULL;
	struct enum_lens *lens = NULL;
	const char *dupkey = NULL;
	struct value *value = NULL;

	Dwarf_Off die_offset = dwarf_dieoffset(parent);

	result = calloc(1, sizeof(struct arg_type_info));
	if (result == NULL) {
		complain(parent, "alloc error");
		CLEANUP_AND_RETURN_ERROR(NULL);
	}

	if (dict_insert(type_dieoffset_hash, &die_offset, &result) != 0) {
		complain(parent, "Couldn't insert into cache dict");
		CLEANUP_AND_RETURN_ERROR(NULL);
	}

	uint64_t byte_size;
	if (!get_die_numeric(&byte_size, parent, DW_AT_byte_size)) {
		// No byte size given, assume 'int'
		result->type = ARGTYPE_INT;
	} else {
		if (!get_integer_base_type(&result->type,
					   (int)byte_size, true)) {
			complain(parent, "Unknown integer base type. "
				 "Using 'int'");
			result->type = ARGTYPE_INT;
		}
	}

	lens = calloc(1, sizeof(struct enum_lens));
	if (lens == NULL) {
		complain(parent, "alloc error");
		CLEANUP_AND_RETURN_ERROR(NULL);
	}
	lens_init_enum(lens);
	result->lens = &lens->super;
	result->own_lens = 1;

	Dwarf_Die die;
	if (dwarf_child(parent, &die) != 0) {
		// empty enum. we're done
		CLEANUP_AND_RETURN_ERROR(NULL);
	}


	while (1) {
		complain(&die, "enum element: 0x%02x/'%s'", dwarf_tag(&die),
			 dwarf_diename(&die));

		dupkey = NULL;
		value = NULL;

		if (dwarf_tag(&die) != DW_TAG_enumerator) {
			complain(&die, "Enums can have ONLY DW_TAG_enumerator "
				 "elements");
			CLEANUP_AND_RETURN_ERROR(NULL);
		}

		if (!dwarf_hasattr(&die, DW_AT_const_value)) {
			complain(&die, "Enums MUST have DW_AT_const_value "
				 "values");
			CLEANUP_AND_RETURN_ERROR(NULL);
		}

		const char *key = dwarf_diename(&die);
		if (key == NULL) {
			complain(&die, "Enums must have a DW_AT_name key");
			CLEANUP_AND_RETURN_ERROR(NULL);
		}
		dupkey = strdup(key);
		if (dupkey == NULL) {
			complain(&die, "Couldn't duplicate enum key");
			CLEANUP_AND_RETURN_ERROR(NULL);
		}

		value = calloc(1, sizeof(struct value));
		if (value == NULL) {
			complain(&die, "Couldn't alloc enum value");
			CLEANUP_AND_RETURN_ERROR(NULL);
		}

		value_init_detached(value, NULL, type_get_simple(result->type),
				    0);
		uint64_t enum_value;
		if (!get_die_numeric(&enum_value, &die, DW_AT_const_value)) {
			complain(&die, "Couldn't get enum value");
			CLEANUP_AND_RETURN_ERROR(NULL);
		}

		value_set_word(value, (long)enum_value);

		if (lens_enum_add(lens, dupkey, 1, value, 1)) {
			complain(&die, "Couldn't add enum element");
			CLEANUP_AND_RETURN_ERROR(NULL);
		}

		NEXT_SIBLING(&die);
	}

	return result;

#undef CLEANUP_AND_RETURN_ERROR
}

// returns a newly-allocated art_type_info*, or NULL on error
static struct arg_type_info *get_array(Dwarf_Die *parent,
				       struct protolib *plib,
				       struct dict *type_dieoffset_hash)
{

#define CLEANUP_AND_RETURN_ERROR(ret) do {				\
		if (length != NULL) {					\
			expr_destroy(length);				\
			free(length);					\
		}							\
		if (array_type != NULL && newly_allocated_array_type) {	\
			type_destroy(array_type);			\
			free(array_type);				\
		}							\
		if (result != NULL) {					\
			type_destroy(result);				\
			free(result);					\
		}							\
		dict_erase (type_dieoffset_hash, &die_offset,		\
			    NULL, NULL, NULL);				\
		dict_insert(type_dieoffset_hash, &die_offset,		\
			    &(struct arg_type_info*){			\
				    type_get_simple(ARGTYPE_VOID)});	\
		return ret;						\
	} while (0)


	struct arg_type_info *result = NULL;
	struct expr_node *length = NULL;
	struct arg_type_info *array_type = NULL;
	int newly_allocated_array_type = 0;

	Dwarf_Off die_offset = dwarf_dieoffset(parent);

	result = calloc(1, sizeof(struct arg_type_info));
	if (result == NULL) {
		complain(parent, "alloc error");
		CLEANUP_AND_RETURN_ERROR(NULL);
	}

	Dwarf_Die type_die;
	if (!get_type_die(&type_die, parent)) {
		complain(parent, "Array has unknown type");
		CLEANUP_AND_RETURN_ERROR(NULL);
	}

	if (dict_insert(type_dieoffset_hash, &die_offset, &result) != 0) {
		complain(parent, "Couldn't insert into cache dict");
		CLEANUP_AND_RETURN_ERROR(NULL);
	}
	array_type = get_type(&newly_allocated_array_type,
			      &type_die, plib, type_dieoffset_hash);
	if (array_type == NULL) {
		complain(parent, "Couldn't figure out array's type");
		CLEANUP_AND_RETURN_ERROR(NULL);
	}

	Dwarf_Die subrange;
	if (dwarf_child(parent, &subrange) != 0) {
		complain(parent,
			 "Array must have a DW_TAG_subrange_type child, "
			 "but has none");
		CLEANUP_AND_RETURN_ERROR(NULL);
	}

	Dwarf_Die next_subrange;
	if (dwarf_siblingof(&subrange, &next_subrange) <= 0) {
		complain(parent,
			 "Array must have exactly one DW_TAG_subrange_type "
			 "child");
		CLEANUP_AND_RETURN_ERROR(NULL);
	}

	if (dwarf_hasattr(&subrange, DW_AT_lower_bound)) {
		uint64_t lower_bound;
		if (!get_die_numeric(&lower_bound, &subrange,
				     DW_AT_lower_bound)) {
			complain(parent, "Couldn't read lower bound");
			CLEANUP_AND_RETURN_ERROR(NULL);
		}

		if (lower_bound != 0) {
			complain(parent,
				 "Array subrange has a nonzero lower bound. "
				 "Don't know what to do");
			CLEANUP_AND_RETURN_ERROR(NULL);
		}
	}

	uint64_t N;
	if (!dwarf_hasattr(&subrange, DW_AT_upper_bound)) {
		// no upper bound is defined. This is probably a variable-width
		// array, and I don't know how long it is. Let's say 0 to be
		// safe
		N = 0;
	}
	else
	{
		if (!get_die_numeric(&N, &subrange, DW_AT_upper_bound)) {
			complain(parent, "Couldn't read upper bound");
			CLEANUP_AND_RETURN_ERROR(NULL);
		}
		N++;
	}

	// I'm not checking the subrange type. It should be some sort of
	// integer, and I don't know what it would mean for it to be something
	// else
	length = calloc(1, sizeof(struct expr_node));
	if (length == NULL) {
		complain(&subrange, "Couldn't alloc length expr");
		CLEANUP_AND_RETURN_ERROR(NULL);
	}
	expr_init_const_word(length, N, type_get_simple(ARGTYPE_INT), 0);

	type_init_array(result, array_type, newly_allocated_array_type,
			length, 1);
	return result;

#undef CLEANUP_AND_RETURN_ERROR
}

// returns a newly-allocated art_type_info*, or NULL on error
static struct arg_type_info *get_structure(Dwarf_Die *parent,
					   struct protolib *plib,
					   struct dict *type_dieoffset_hash)
{

#define CLEANUP_AND_RETURN_ERROR(ret) do {				\
		if (member_type != NULL && newly_allocated_member_type) { \
			type_destroy(member_type);			\
			free(member_type);				\
		}							\
		if (result != NULL) {					\
			type_destroy(result);				\
			free(result);					\
		}							\
		dict_erase (type_dieoffset_hash, &die_offset,		\
			    NULL, NULL, NULL);				\
		dict_insert(type_dieoffset_hash, &die_offset,		\
			    &(struct arg_type_info*){			\
				    type_get_simple(ARGTYPE_VOID)});	\
		return ret;						\
	} while (0)


	struct arg_type_info *result = NULL;
	struct arg_type_info *member_type = NULL;
	int newly_allocated_member_type = 0;

	Dwarf_Off die_offset = dwarf_dieoffset(parent);

	result = calloc(1, sizeof(struct arg_type_info));
	if (result == NULL) {
		complain(parent, "alloc error");
		CLEANUP_AND_RETURN_ERROR(NULL);
	}
	type_init_struct(result);
	if (dict_insert(type_dieoffset_hash, &die_offset, &result) != 0) {
		complain(parent, "Couldn't insert into cache dict");
		CLEANUP_AND_RETURN_ERROR(NULL);
	}

	Dwarf_Die die;
	if (dwarf_child(parent, &die) != 0) {
		// no elements; we're done
		return result;
	}

	while (1) {
		member_type = NULL;
		newly_allocated_member_type = 0;

		complain(&die, "member: 0x%02x", dwarf_tag(&die));

		if (dwarf_tag(&die) != DW_TAG_member) {
			complain(&die, "Structure can have ONLY DW_TAG_member");
			CLEANUP_AND_RETURN_ERROR(NULL);
		}

		Dwarf_Die type_die;
		if (!get_type_die(&type_die, &die)) {
			complain(&die, "Couldn't get type of element");
			CLEANUP_AND_RETURN_ERROR(NULL);
		}

		member_type = get_type(&newly_allocated_member_type,
				       &type_die, plib, type_dieoffset_hash);
		if (member_type == NULL) {
			complain(&die, "Couldn't parse type from DWARF data");
			CLEANUP_AND_RETURN_ERROR(NULL);
		}
		if (member_type->type == ARGTYPE_VOID) {
			complain(&die, "Struct member has void type. "
				 "Giving up on this structure");
			CLEANUP_AND_RETURN_ERROR(NULL);
		}
		if (type_struct_add(result, member_type,
				    newly_allocated_member_type) != 0) {
			complain(&die, "Couldn't add type to struct");
			CLEANUP_AND_RETURN_ERROR(NULL);
		}

		NEXT_SIBLING(&die);
	}

	return result;
#undef CLEANUP_AND_RETURN_ERROR
}

// Reads the type in the die and returns the corresponding arg_type_info*. If
// this was newly allocated on the heap, *newly_allocated_info = true. If an
// error occurred, returns NULL
static struct arg_type_info *get_type(int *newly_allocated_result,
				      Dwarf_Die *type_die,
				      struct protolib *plib,
				      struct dict *type_dieoffset_hash)
{

#define CLEANUP_AND_RETURN_ERROR(ret) do {				\
		if (pointee != NULL && newly_allocated_pointee) {	\
			type_destroy(pointee);				\
			free(pointee);					\
		}							\
		if (result != NULL && *newly_allocated_result) {	\
			type_destroy(result);				\
			free(result);					\
		}							\
		dict_erase (type_dieoffset_hash, &die_offset,		\
			    NULL, NULL, NULL);				\
		dict_insert(type_dieoffset_hash, &die_offset,		\
			    &(struct arg_type_info*){			\
				    type_get_simple(ARGTYPE_VOID)});	\
		return ret;						\
	} while (0)

#define DICT_INSERT_AND_CHECK(type_dieoffset_hash, die_offset, result)	\
	do {								\
		if (dict_insert(type_dieoffset_hash,			\
				die_offset, result) != 0) {		\
			complain(type_die,				\
				 "Couldn't insert into cache dict");	\
			CLEANUP_AND_RETURN_ERROR(NULL);			\
		}							\
	} while(0)

	struct arg_type_info *result = NULL;
	struct arg_type_info *pointee = NULL;
	int newly_allocated_pointee = 0;

	Dwarf_Off die_offset = dwarf_dieoffset(type_die);


	// by default, we say we allocated nothing. I set this to true later,
	// when I allocate memory
	*newly_allocated_result = 0;

	struct arg_type_info **found_type = dict_find(type_dieoffset_hash,
						      &die_offset);
	if (found_type != NULL) {
		complain(type_die, "Read pre-computed type");
		return *found_type;
	}

	const char *type_name = dwarf_diename(type_die);
	if (type_name != NULL) {

		struct named_type *already_defined_type =
			protolib_lookup_type(plib, type_name, true);

		if (already_defined_type != NULL) {
			complain(type_die,
				 "Type '%s' defined in a .conf file. "
				 "Using that instead of DWARF",
				 type_name);
			return already_defined_type->info;
		}
	}

	Dwarf_Die next_die;

	switch (dwarf_tag(type_die)) {
	case DW_TAG_base_type:
		complain(type_die, "Storing base type");
		result = type_get_simple(get_base_type(type_die));
		DICT_INSERT_AND_CHECK(type_dieoffset_hash, &die_offset, &result);
		return result;

	case DW_TAG_subroutine_type:
	case DW_TAG_inlined_subroutine:
		// function pointers are stored as void*. If ltrace tries to
		// dereference these, it'll get a segfault
		complain(type_die, "Storing subroutine type");
		result = type_get_simple(ARGTYPE_VOID);
		DICT_INSERT_AND_CHECK(type_dieoffset_hash, &die_offset, &result);
		return result;

	case DW_TAG_pointer_type:
		if (!get_type_die(&next_die, type_die)) {
			// the pointed-to type isn't defined, so I report a
			// void*
			complain(type_die, "Storing void-pointer type");
			result = type_get_voidptr();
			DICT_INSERT_AND_CHECK(type_dieoffset_hash, &die_offset, &result);
			return result;
		}

		complain(type_die, "Storing pointer type");

		*newly_allocated_result = 1;
		result = calloc(1, sizeof(struct arg_type_info));
		if (result == NULL) {
			complain(type_die, "alloc error");
			CLEANUP_AND_RETURN_ERROR(NULL);
		}

		/* Make it at least look like a pointer, so that
		 * e.g. alignment and sizeof can be computed when
		 * needed, and so that it doesn't look as if we're
		 * adding bare voids to structures.  */
		result->type = ARGTYPE_POINTER;
		result->u.ptr_info.info = NULL;

		/* Add it now so that recursive requests for this type
		 * don't end up spinning endlessly.  */
		DICT_INSERT_AND_CHECK(type_dieoffset_hash, &die_offset, &result);

		/* Now we can safely recurse.  */
		pointee = get_type(&newly_allocated_pointee,
				   &next_die, plib, type_dieoffset_hash);
		if (pointee == NULL)
			CLEANUP_AND_RETURN_ERROR(NULL);

		/* Update the stored type in-place.  */
		type_init_pointer(result, pointee, newly_allocated_pointee);
		complain(type_die, "Done storing pointer type.");

		return result;

	case DW_TAG_structure_type:
		complain(type_die, "Storing struct type");
		*newly_allocated_result = 1;

		result = get_structure(type_die, plib, type_dieoffset_hash);
		if (result == NULL)
			CLEANUP_AND_RETURN_ERROR(NULL);
		return result;


	case DW_TAG_typedef:
	case DW_TAG_const_type:
	case DW_TAG_volatile_type:
		// Various tags are simply pass-through, so I just keep going
		if (get_type_die(&next_die, type_die)) {
			complain(type_die, "Storing const/typedef type");

			result = get_type(newly_allocated_result, &next_die,
					  plib, type_dieoffset_hash);
			if (result == NULL)
				CLEANUP_AND_RETURN_ERROR(NULL);
		} else {
			// no type. Use 'void'. Normally I'd think this is
			// bogus, but stdio typedefs something to void
			result = type_get_simple(ARGTYPE_VOID);
			complain(type_die, "Storing void type");
		}
		DICT_INSERT_AND_CHECK(type_dieoffset_hash, &die_offset, &result);
		return result;

	case DW_TAG_enumeration_type:
		// We have an enumeration. This has a base type, but has a
		// particular lens to handle the enum
		*newly_allocated_result = 1;

		complain(type_die, "Storing enum int");
		result = get_enum(type_die, type_dieoffset_hash);
		if (result == NULL)
			CLEANUP_AND_RETURN_ERROR(NULL);
		return result;

	case DW_TAG_array_type:
		*newly_allocated_result = 1;

		complain(type_die, "Storing array");
		result = get_array(type_die, plib, type_dieoffset_hash);
		if (result == NULL)
			CLEANUP_AND_RETURN_ERROR(NULL);
		return result;

	case DW_TAG_union_type:
	{
		uint64_t sz;
		if (! get_die_numeric(&sz, type_die, DW_AT_byte_size)) {
			complain(type_die, "Can't determine type size");
			CLEANUP_AND_RETURN_ERROR(NULL);

		} else if (sz != (uint64_t) (unsigned long) sz) {
			complain(type_die, "Union size too big");
			CLEANUP_AND_RETURN_ERROR(NULL);

		} else {
			complain(type_die, "Storing union as byte array");

			*newly_allocated_result = 1;

			/* Allocate the array type and its element
			 * type in one bunch.  */
			result = calloc(2, sizeof *result);
			struct expr_node *len_expr = calloc(1, sizeof *len_expr);
			if (result == NULL || len_expr == NULL) {
				complain(type_die, "alloc error");
				free(result);
				free(len_expr);
				CLEANUP_AND_RETURN_ERROR(NULL);
			}

			result[1].type = ARGTYPE_CHAR;
			result[1].lens = &hex_lens;
			result[1].own_lens = 0;

			expr_init_const_word(len_expr, (unsigned long) sz,
					     type_get_simple(ARGTYPE_ULONG), 0);

			/* Don't own element type, own length.  */
			type_init_array(&result[0], &result[1], 0, len_expr, 1);
		}
		DICT_INSERT_AND_CHECK(type_dieoffset_hash, &die_offset, &result);
		return result;
	}

	default:
		complain(type_die, "Unknown type tag 0x%x. Returning void",
			 dwarf_tag(type_die));
		result = type_get_simple(ARGTYPE_VOID);
		DICT_INSERT_AND_CHECK(type_dieoffset_hash, &die_offset, &result);
		return result;
	}

#undef DICT_INSERT_AND_CHECK
#undef CLEANUP_AND_RETURN_ERROR
}

// fills in *proto with a prototype. Returns true on success
static bool get_prototype(struct prototype *result,
			  Dwarf_Die *subroutine, struct protolib *plib,
			  struct dict *type_dieoffset_hash)
{

#define CLEANUP_AND_RETURN_ERROR(ret) do {				\
		if (argument_type != NULL && newly_allocated_argument_type) { \
			type_destroy(argument_type);			\
			free(argument_type);				\
		}							\
		prototype_destroy(result);				\
		return ret;						\
	} while (0)


	struct arg_type_info *argument_type = NULL;
	int newly_allocated_argument_type = 0;

	prototype_init(result);

	// First, look at the return type. This is stored in a DW_AT_type tag in
	// the subroutine DIE. If there is no such tag, this function returns
	// void
	Dwarf_Die return_type_die;
	if (!get_type_die(&return_type_die, subroutine)) {
		result->return_info = type_get_simple(ARGTYPE_VOID);
		result->own_return_info = 0;
	} else {
		int newly_allocated_return_type;
		result->return_info = get_type(&newly_allocated_return_type,
					       &return_type_die, plib,
					       type_dieoffset_hash);
		if (result->return_info == NULL) {
			complain(subroutine, "Couldn't get return type");
			CLEANUP_AND_RETURN_ERROR(false);
		}
		result->own_return_info = newly_allocated_return_type;
	}


	// Now look at the arguments
	Dwarf_Die arg_die;
	if (dwarf_child(subroutine, &arg_die) != 0) {
		// no args. We're done
		return true;
	}

	while (1) {
		if (dwarf_tag(&arg_die) == DW_TAG_formal_parameter) {

			complain(&arg_die, "arg: 0x%02x", dwarf_tag(&arg_die));

			argument_type = NULL;
			newly_allocated_argument_type = false;

			Dwarf_Die type_die;
			if (!get_type_die(&type_die, &arg_die)) {
				complain(&arg_die, "Couldn't get the argument "
					 "type die");
				CLEANUP_AND_RETURN_ERROR(false);
			}


			argument_type = get_type(&newly_allocated_argument_type,
						 &type_die, plib,
						 type_dieoffset_hash);
			if (argument_type==NULL) {
				complain(&arg_die, "Couldn't parse arg "
					 "type from DWARF data");
				CLEANUP_AND_RETURN_ERROR(false);
			}

			struct param param;
			param_init_type(&param, argument_type,
					newly_allocated_argument_type);
			if (prototype_push_param(result, &param) <0) {
				complain(&arg_die, "couldn't add argument to "
					 "the prototype");
				CLEANUP_AND_RETURN_ERROR(false);
			}

#ifdef DUMP_PROTOTYPES
			fprintf(stderr, "Adding argument:\n");
			dump_ltrace_tree(argument_type);
#endif
		}

		NEXT_SIBLING(&arg_die);
	}

	return true;
#undef CLEANUP_AND_RETURN_ERROR
}


static enum callback_status
any_filter_matches_function_cb(const char *name, void *data)
{
	struct library *lib = (struct library*)data;

	return CBS_STOP_IF(
		filter_matches_symbol(options.plt_filter,    name, lib) ||
		filter_matches_symbol(options.static_filter, name, lib) ||
		filter_matches_symbol(options.export_filter, name, lib));
}
static bool any_filter_matches_function(const char *function_name,
					struct library *lib,
					Dwarf_Die *die)
{
	// I give up if this function is not wanted AND if none of its aliased
	// names are wanted
	if (any_filter_matches_function_cb(function_name, lib) == CBS_STOP)
		return true;

	// prototype not found. Is it aliased?
	if (library_exported_names_each_alias(&lib->exported_names,
					      function_name, NULL,
					      any_filter_matches_function_cb,
					      lib) != NULL)
		return true;

	complain(die, "Prototype not requested by any filter");
	return false;

}
static bool import_subprogram_name(struct protolib *plib, struct library *lib,
				   struct dict *type_dieoffset_hash,
				   Dwarf_Die *die, const char *function_name)
{
	if (!any_filter_matches_function( function_name, lib, die)) {
		complain(die, "Prototype not requested by any filter");
		return true;
	}

	complain(die, "subroutine_type: 0x%02x; function '%s'",
		 dwarf_tag(die), function_name);

	struct prototype *proto_already_there =
		protolib_lookup_prototype(plib, function_name, false);

	if (proto_already_there != NULL) {
		complain(die, "Prototype already exists. Skipping");
		return true;
	}

	struct prototype proto;
	if (!get_prototype(&proto, die, plib, type_dieoffset_hash)) {
		complain(die, "couldn't get prototype");
		return false;
	}

	const char *function_name_dup = strdup(function_name);
	if (function_name_dup == NULL) {
		complain(die, "couldn't strdup");
		prototype_destroy(&proto);
		return false;
	}
	protolib_add_prototype(plib, function_name_dup, 1, &proto);
	return true;
}

static bool import_subprogram_die(struct protolib *plib, struct library *lib,
				  struct dict *type_dieoffset_hash,
				  Dwarf_Die *die)
{
	// If there is a linkage name, I use it (this is required for C++ code,
	// in particular).
	//
	// I use the plain name regardless, since sometimes the exported symbol
	// corresponds to the plain name, NOT the linkage name. For instance I
	// see this on my Debian/sid amd64 box. In its libc, the linkage name of
	// __nanosleep is __GI___nanosleep, but the export is __nanosleep
	const char *function_name;
	Dwarf_Attribute attr;

	if (dwarf_attr(die, DW_AT_linkage_name, &attr) != NULL &&
	    (function_name = dwarf_formstring(&attr)) != NULL &&
	    !import_subprogram_name(plib, lib, type_dieoffset_hash, die,
				    function_name)) {
		return false;
	}

	if ((function_name = dwarf_diename(die)) != NULL &&
	    !import_subprogram_name(plib, lib, type_dieoffset_hash, die,
				    function_name)) {
		return false;
	}

	return true;
}

static bool process_die_compileunit(struct protolib *plib, struct library *lib,
				    struct dict *type_dieoffset_hash,
				    Dwarf_Die *parent)
{
	complain(parent, "Processing compile unit");
	Dwarf_Die die;
	if (dwarf_child(parent, &die) != 0) {
		// no child nodes, so nothing to do
		return true;
	}

	while (1) {
		if (dwarf_tag(&die) == DW_TAG_subprogram)
			if (!import_subprogram_die(plib, lib, type_dieoffset_hash,
						   &die))
				complain(&die, "Error importing subprogram. "
					 "Skipping");

		NEXT_SIBLING(&die);
	}

	return true;
}

static void import(struct protolib *plib, struct library *lib,
		   Dwfl_Module *dwfl_module)
{
	// A map from DIE addresses (Dwarf_Off) to type structures (struct
	// arg_type_info*). This is created and filled in at the start of each
	// import, and deleted when the import is complete
	struct dict type_dieoffset_hash;

	dict_init(&type_dieoffset_hash, sizeof(Dwarf_Off),
		  sizeof(struct arg_type_info*),
		  dwarf_die_hash, dwarf_die_eq, NULL);

	Dwarf_Addr bias;
	Dwarf_Die *die = NULL;
	while ((die = dwfl_module_nextcu(dwfl_module, die, &bias)) != NULL) {
		if (dwarf_tag(die) == DW_TAG_compile_unit)
			process_die_compileunit(plib, lib,
						&type_dieoffset_hash, die);
		else
			complain(die, "A DW_TAG_compile_unit die expected. "
				 "Skipping this one");
	}

	dict_destroy(&type_dieoffset_hash, NULL, NULL, NULL);
}

bool import_DWARF_prototypes(struct library *lib)
{
	struct protolib *plib = lib->protolib;

	debug(DEBUG_FUNCTION, "Importing DWARF prototypes from '%s'",
	      lib->soname);
	if (plib == NULL) {

		const char *soname_dup = strdup(lib->soname);
		if (soname_dup == NULL) {
			fprintf(stderr, "couldn't strdup");
			return false;
		}

		plib = protolib_cache_default(&g_protocache, soname_dup, 1);
		if (plib == NULL) {
			fprintf(stderr, "Error loading protolib %s: %s.\n",
				lib->soname, strerror(errno));
		}
	}

	import(plib, lib, lib->dwfl_module);
	lib->protolib = plib;

	return true;
}
