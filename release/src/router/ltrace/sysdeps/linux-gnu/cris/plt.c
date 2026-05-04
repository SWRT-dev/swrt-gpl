/*
 * This file is part of ltrace.
 * Copyright (C) 2010 Edgar E. Iglesias
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

#include <gelf.h>
#include "proc.h"
#include "common.h"
#include "library.h"

GElf_Addr arch_plt_sym_val(struct ltelf *lte, size_t ndx, GElf_Rela * rela)
{
	return lte->plt_addr + 0x20 + (ndx * 26);
}

void *sym2addr(struct process *proc, struct library_symbol *sym)
{
	return sym->enter_addr;
}
