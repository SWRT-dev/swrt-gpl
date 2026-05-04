/*
 * This file is part of ltrace.
 * Copyright (C) 2011,2012,2013 Petr Machata, Red Hat Inc.
 * Copyright (C) 2010 Joe Damato
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

#ifndef COMMON_H
#define COMMON_H

#include <config.h>

#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>

#include "ltrace.h"
#include "defs.h"
#include "dict.h"
#include "sysdep.h"
#include "debug.h"
#include "ltrace-elf.h"
#include "read_config_file.h"
#include "proc.h"
#include "forward.h"

extern char * command;

extern int exiting;  /* =1 if we have to exit ASAP */

extern char *PLTs_initialized_by_here;

#include "options.h"
#include "output.h"

/* Events  */
extern Event * next_event(void);
extern void handle_event(Event * event);

extern pid_t execute_program(const char * command, char ** argv);

struct breakpoint;
struct library_symbol;

/* Format VALUE into STREAM.  The dictionary of all arguments is given
 * for purposes of evaluating array lengths and other dynamic
 * expressions.  Returns number of characters outputted, -1 in case of
 * failure.  */
int format_argument(FILE *stream, struct value *value,
		    struct value_dict *arguments);

/* Set *RET to either a duplicate of STR (if WHETHER), or STR
 * (otherwise).  Return 0 on success or a negative value on failure.
 * The duplication is not done if STR is NULL.  */
int strdup_if(const char **ret, const char *str, int whether);

#endif
