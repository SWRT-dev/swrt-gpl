/*
 * This file is part of ltrace.
 * Copyright (C) 2011, 2012 Petr Machata, Red Hat Inc.
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

#ifndef LENS_ENUM_H
#define LENS_ENUM_H

#include "lens.h"
#include "vect.h"

struct enum_lens {
	struct lens super;
	struct vect entries;
};

/* Init enumeration LENS.  */
void lens_init_enum(struct enum_lens *lens);

/* Push another member of the enumeration, named KEY, with given
 * VALUE.  If OWN_KEY, KEY is owned and released after the type is
 * destroyed.  KEY is typed as const char *, but note that if OWN_KEY,
 * this value will be freed.  If OWN_VALUE, then VALUE is owned and
 * destroyed by LENS.  */
int lens_enum_add(struct enum_lens *lens,
		  const char *key, int own_key,
		  struct value *value, int own_value);

/* Return number of enum elements of type INFO.  */
size_t lens_enum_size(struct enum_lens *lens);

#endif /* LENS_ENUM_H */
