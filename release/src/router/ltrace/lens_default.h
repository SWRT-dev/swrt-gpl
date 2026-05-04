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

#ifndef LENS_DEFAULT_H
#define LENS_DEFAULT_H

#include "lens.h"

/* Default lens that does reasonable job for most cases.  */
extern struct lens default_lens;

/* A lens that doesn't output anything.  */
extern struct lens blind_lens;

/* A lens that formats integers in octal.  */
extern struct lens octal_lens;

/* A lens that formats integers in hexadecimal.  */
extern struct lens hex_lens;

/* A lens that formats integers as either "true" or "false".  */
extern struct lens bool_lens;

/* A lens that tries to guess whether the value is "large" (i.e. a
 * pointer, and should be formatted in hex), or "small" (and should be
 * formatted in decimal).  */
extern struct lens guess_lens;

/* A lens for strings.  */
extern struct lens string_lens;

/* A lens for bit vector.  */
extern struct lens bitvect_lens;

#endif /* LENS_DEFAULT_H */
