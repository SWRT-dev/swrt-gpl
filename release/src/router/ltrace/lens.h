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

#ifndef LENS_H
#define LENS_H

#include <stdio.h>
#include "forward.h"

struct lens {
	/* Format VALUE into STREAM.  The dictionary of all arguments
	 * is given for purposes of evaluating array lengths and other
	 * dynamic expressions.  Returns number of characters
	 * outputted, -1 in case of failure.  */
	int (*format_cb)(struct lens *lens, FILE *stream, struct value *value,
			 struct value_dict *arguments);

	/* Erase any memory allocated for LENS.  Keep the LENS pointer
	 * itself intact.  */
	void (*destroy_cb)(struct lens *lens);
};

/* Extracts a lens from VALUE and calls lens_format on that.  */
int format_argument(FILE *stream, struct value *value,
		    struct value_dict *arguments);

/* Calls format callback associated with LENS.  */
int lens_format(struct lens *lens, FILE *stream, struct value *value,
		struct value_dict *arguments);

/* Calls destroy callback associated with LENS.  */
void lens_destroy(struct lens *lens);

#endif /* LENS_H */
