/*
 * This file is part of ltrace.
 * Copyright (C) 2012 Petr Machata, Red Hat Inc.
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

#if defined HAVE_LIBSUPC__ || defined HAVE_LIBSTDC__
# define USE_CXA_DEMANGLE
#endif
#if defined HAVE_LIBIBERTY || defined USE_CXA_DEMANGLE
# define USE_DEMANGLE
#endif

extern char *cplus_demangle(const char *mangled, int options);

const char *my_demangle(const char *function_name);

/* Options passed to cplus_demangle (in 2nd parameter). */

#define DMGL_NO_OPTS    0	/* For readability... */
#define DMGL_PARAMS     (1 << 0)	/* Include function args */
#define DMGL_ANSI       (1 << 1)	/* Include const, volatile, etc */
#define DMGL_JAVA       (1 << 2)	/* Demangle as Java rather than C++. */
