/*
 * This file is part of ltrace.
 * Copyright (C) 2013 Petr Machata, Red Hat Inc.
 * Copyright (C) 1998,2004,2008 Juan Cespedes
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

#ifndef LTRACE_ARM_ARCH_H
#define LTRACE_ARM_ARCH_H

#include <libelf.h>

#define ARCH_HAVE_ENABLE_BREAKPOINT 1
#define ARCH_HAVE_DISABLE_BREAKPOINT 1

#define BREAKPOINT_VALUE { 0xf0, 0x01, 0xf0, 0xe7 }
#define BREAKPOINT_LENGTH 4
#define THUMB_BREAKPOINT_VALUE { 0x01, 0xde }
#define THUMB_BREAKPOINT_LENGTH 2
#define DECR_PC_AFTER_BREAK 0
#define ARCH_ENDIAN_LITTLE

#define LT_ELFCLASS	ELFCLASS32
#define LT_ELF_MACHINE	EM_ARM

#define ARCH_HAVE_SW_SINGLESTEP
#define ARCH_HAVE_FETCH_ARG
#define ARCH_HAVE_FETCH_PACK
#define ARCH_HAVE_SIZEOF
#define ARCH_HAVE_ALIGNOF
#define ARCH_HAVE_BREAKPOINT_DATA
struct arch_breakpoint_data {
	int thumb_mode;
};

#define ARCH_HAVE_LTELF_DATA
struct arch_ltelf_data {
	Elf_Data *jmprel_data;
};

#define ARCH_HAVE_LIBRARY_DATA
struct arch_library_data {
	unsigned int hardfp:1;
};

#endif /* LTRACE_ARM_ARCH_H */
