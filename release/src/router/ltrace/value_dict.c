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

#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "value_dict.h"
#include "value.h"

struct named_value
{
	const char *name;
	struct value value;
	int own_name;
};

void
val_dict_init(struct value_dict *dict)
{
	VECT_INIT(&dict->numbered, struct value);
	VECT_INIT(&dict->named, struct named_value);
}

static int
value_clone_cb(struct value *tgt, const struct value *src, void *data)
{
	return value_clone(tgt, src);
}

static void
value_dtor(struct value *val, void *data)
{
	value_destroy(val);
}

static int
named_value_clone(struct named_value *tgt,
		  const struct named_value *src, void *data)
{
	tgt->name = strdup(src->name);
	if (tgt->name == NULL)
		return -1;
	tgt->own_name = 1;
	if (value_clone(&tgt->value, &src->value) < 0) {
		free((char *)tgt->name);
		return -1;
	}
	return 0;
}

static void
named_value_dtor(struct named_value *named, void *data)
{
	if (named->own_name)
		free((char *)named->name);
	value_destroy(&named->value);
}

int
val_dict_clone(struct value_dict *target, struct value_dict *source)
{
	if (VECT_CLONE(&target->numbered, &source->numbered, struct value,
		       value_clone_cb, value_dtor, NULL) < 0)
		return -1;

	if (VECT_CLONE(&target->named, &source->named, struct named_value,
		       named_value_clone, named_value_dtor, NULL) < 0) {
		VECT_DESTROY(&target->numbered, struct value, value_dtor, NULL);
		return -1;
	}

	return 0;
}

int
val_dict_push_next(struct value_dict *dict, struct value *val)
{
	return VECT_PUSHBACK(&dict->numbered, val);
}

int
val_dict_push_named(struct value_dict *dict, struct value *val,
		    const char *name, int own_name)
{
	if (own_name && (name = strdup(name)) == NULL)
		return -1;
	struct named_value element = { name, *val, own_name };
	if (VECT_PUSHBACK(&dict->named, &element) < 0) {
		if (own_name)
			free((char *)name);
		return -1;
	}
	return 0;
}

size_t
val_dict_count(struct value_dict *dict)
{
	return vect_size(&dict->numbered);
}

struct value *
val_dict_get_num(struct value_dict *dict, size_t num)
{
	assert(num < vect_size(&dict->numbered));
	return VECT_ELEMENT(&dict->numbered, struct value, num);
}

struct value *
val_dict_get_name(struct value_dict *dict, const char *name)
{
	size_t i;
	for (i = 0; i < vect_size(&dict->named); ++i) {
		struct named_value *element
			= VECT_ELEMENT(&dict->named, struct named_value, i);
		if (strcmp(element->name, name) == 0)
			return &element->value;
	}
	return NULL;
}

void
val_dict_destroy(struct value_dict *dict)
{
	if (dict == NULL)
		return;

	VECT_DESTROY(&dict->numbered, struct value, value_dtor, NULL);
	VECT_DESTROY(&dict->named, struct named_value, named_value_dtor, NULL);
}
