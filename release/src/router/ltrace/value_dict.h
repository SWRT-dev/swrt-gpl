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

#ifndef VALUE_DICT_H
#define VALUE_DICT_H

#include "forward.h"
#include "vect.h"

/* Value dictionary is used to store actual function arguments.  It
 * supports both numbered and named arguments.  */
struct value_dict
{
	struct vect numbered;
	struct vect named;
};

/* Initialize DICT.  */
void val_dict_init(struct value_dict *dict);

/* Clone SOURCE into TARGET.  Return 0 on success or a negative value
 * on failure.  */
int val_dict_clone(struct value_dict *target, struct value_dict *source);

/* Push next numbered value, VAL.  The value is copied over and the
 * dictionary becomes its owner, and is responsible for destroying it
 * later.  Returns 0 on success and a negative value on failure.  */
int val_dict_push_next(struct value_dict *dict, struct value *val);

/* Return count of numbered arguments.  */
size_t val_dict_count(struct value_dict *dict);

/* Push value VAL named NAME.  See notes at val_dict_push_next about
 * value ownership.  The name is owned and freed if OWN_NAME is
 * non-zero.  */
int val_dict_push_named(struct value_dict *dict, struct value *val,
			const char *name, int own_name);

/* Get NUM-th numbered argument, or NULL if there's not that much
 * arguments.  */
struct value *val_dict_get_num(struct value_dict *dict, size_t num);

/* Get argument named NAME, or NULL if there's no such argument.  */
struct value *val_dict_get_name(struct value_dict *dict, const char *name);

/* Destroy the dictionary and all the values in it.  Note that DICT
 * itself (the pointer) is not freed.  */
void val_dict_destroy(struct value_dict *dict);

#endif /* VALUE_DICT_H */
