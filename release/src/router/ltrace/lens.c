/*
 * This file is part of ltrace.
 * Copyright (C) 2011 Petr Machata, Red Hat Inc.
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

#include "lens.h"
#include "lens_default.h"
#include "type.h"
#include "value.h"

int
format_argument(FILE *stream, struct value *value,
		struct value_dict *arguments)
{
	/* Find the closest enclosing parental value whose type has a
	 * lens assigned.  */
	struct value *parent;
	for (parent = value; (parent != NULL && parent->type != NULL
			      && parent->type->lens == NULL);
	     parent = parent->parent)
		;

	struct lens *lens = &default_lens;
	if (parent != NULL && parent->type != NULL
	    && parent->type->lens != NULL)
		lens = parent->type->lens;

	return lens_format(lens, stream, value, arguments);
}

int
lens_format(struct lens *lens, FILE *stream, struct value *value,
	    struct value_dict *arguments)
{
	assert(lens->format_cb != NULL);
	return lens->format_cb(lens, stream, value, arguments);
}

void
lens_destroy(struct lens *lens)
{
	if (lens != NULL
	    && lens->destroy_cb != NULL)
		lens->destroy_cb(lens);
}
