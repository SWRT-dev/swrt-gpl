/*
 * This file is part of ltrace.
 * Copyright (C) 2007, 2008, 2012 Petr Machata, Red Hat Inc.
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

#ifndef GLOB_H
#define GLOB_H

#include <sys/types.h>
#include <regex.h>

/* This is akin to regcomp(3), except it compiles a glob expression
 * passed in GLOB.  See glob(7) for more information about the syntax
 * supported by globcomp.  */
int globcomp(regex_t *preg, const char *glob, int cflags);

#endif /* GLOB_H */
