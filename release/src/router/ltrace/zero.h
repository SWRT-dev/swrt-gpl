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

#ifndef ZERO_H
#define ZERO_H

#include "forward.h"

/* This returns a pre-built "zero" node without argument.  Share, but
   don't free.  */
struct expr_node *expr_node_zero(void);

/* This builds a new "zero" node with EXPR as argument.  EXPR is owned
 * by the built node if OWN.  Returns NULL if something failed.  */
struct expr_node *build_zero_w_arg(struct expr_node *expr, int own);

#endif /* ZERO_H */
