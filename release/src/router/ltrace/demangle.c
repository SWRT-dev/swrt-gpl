/*
 * This file is part of ltrace.
 * Copyright (C) 2012,2013 Petr Machata, Red Hat Inc.
 * Copyright (C) 1998,1999,2003,2004,2008,2009 Juan Cespedes
 * Copyright (C) 2006 Ian Wienand
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

#include "config.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "demangle.h"
#include "dict.h"
#include "debug.h"

#ifdef USE_DEMANGLE

/*****************************************************************************/

static struct dict *name_cache = NULL;

const char *
my_demangle(const char *function_name) {
#ifdef USE_CXA_DEMANGLE
	extern char *__cxa_demangle(const char *, char *, size_t *, int *);
#endif

	debug(DEBUG_FUNCTION, "my_demangle(name=%s)", function_name);

	if (name_cache == NULL) {
		name_cache = malloc(sizeof(*name_cache));
		if (name_cache != NULL)
			DICT_INIT(name_cache, char *, const char *,
				  dict_hash_string, dict_eq_string, NULL);
	}

	const char *tmp = NULL;
	if (name_cache != NULL
	    && DICT_FIND_VAL(name_cache, &function_name, &tmp) == 0)
		return tmp;

#ifdef HAVE_LIBIBERTY
	tmp = cplus_demangle(function_name,
					 DMGL_ANSI | DMGL_PARAMS);
#elif defined USE_CXA_DEMANGLE
	int status = 0;
	tmp = __cxa_demangle(function_name, NULL, NULL, &status);
#endif
	if (name_cache == NULL || tmp == NULL) {
	fail:
		if (tmp == NULL)
			return function_name;
		return tmp;
	}

	const char *fn_copy = strdup(function_name);
	if (fn_copy == NULL)
		goto fail;

	if (DICT_INSERT(name_cache, &fn_copy, &tmp) < 0) {
		free((char *)fn_copy);
		goto fail;
	}

	return tmp;
}

#endif
