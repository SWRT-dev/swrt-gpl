/*
 * This file is part of ltrace.
 * Copyright (C) 2011, 2012, 2013 Petr Machata, Red Hat Inc.
 * Copyright (C) 2009 Juan Cespedes
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

#ifndef OUTPUT_H
#define OUTPUT_H

#include "fetch.h"
#include "forward.h"

void output_line(struct process *proc, const char *fmt, ...);
void output_left(enum tof type, struct process *proc,
		 struct library_symbol *libsym);
void output_right(enum tof type, struct process *proc,
		  struct library_symbol *libsym,
		  struct timedelta *spent);

/* This function is for emitting lists of comma-separated strings.
 *
 * STREAM is where the output should be eventually sent.
 *
 * WRITER is the function to do the output.  It returns number of
 * characters written, or a negative value if there were errors (like
 * what fprintf does).  If WRITER returns 0, it means nothing was
 * written, and separator shouldn't therefore be emitted either.
 *
 * NEED_DELIMP serves for keeping state between calls.  It should
 * point to a variable that is initialized to 0.  DATA is passed
 * verbatim to the WRITER.
 *
 * Returns number of characters written to STREAM (including any
 * separators), or a negative value if there were errors.  */
int delim_output(FILE *stream, int *need_delimp,
		 int (*writer)(FILE *stream, void *data), void *data);

/* If C is positive, add it to *COUNTP.  Returns C.  */
int account_output(int *countp, int c);

void report_error(char const *file, unsigned line_no, const char *fmt, ...);
void report_warning(char const *file, unsigned line_no, const char *fmt, ...);
void report_global_error(const char *fmt, ...);

#endif /* OUTPUT_H */
