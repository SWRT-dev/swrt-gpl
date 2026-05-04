/*
 * This file is part of ltrace.
 * Copyright (C) 2011,2012,2013 Petr Machata, Red Hat Inc.
 * Copyright (C) 2006 Ian Wienand
 * Copyright (C) 2004 Juan Cespedes
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
#ifndef LTRACE_X86_ARCH_H
#define LTRACE_X86_ARCH_H

#include "vect.h"

#define BREAKPOINT_VALUE {0xcc}
#define BREAKPOINT_LENGTH 1
#define DECR_PC_AFTER_BREAK 1
#define ARCH_HAVE_FETCH_ARG
#define ARCH_HAVE_SIZEOF
#define ARCH_HAVE_ALIGNOF
#define ARCH_ENDIAN_LITTLE

#define ARCH_HAVE_ADD_PLT_ENTRY

#define ARCH_HAVE_LTELF_DATA
struct arch_ltelf_data {
	struct vect plt_map;
};

#ifdef __x86_64__
#define LT_ELFCLASS	ELFCLASS64
#define LT_ELF_MACHINE	EM_X86_64
#endif
#define LT_ELFCLASS2	ELFCLASS32
#define LT_ELF_MACHINE2	EM_386

#endif /* LTRACE_X86_ARCH_H */
