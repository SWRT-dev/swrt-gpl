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

#ifndef PRINTF_H
#define PRINTF_H

#include <stddef.h>
#include "forward.h"

/* Wrapper around param_init_pack with callbacks to handle
 * printf-style format strings.  ARG should be an expression that
 * evaluates to a pointer to a character array with the format string
 * contents.  */
void param_pack_init_printf(struct param *param,
			    struct expr_node *arg, int own_arg);

#endif /* PRINTF_H */
