/*
 * This file is part of ltrace.
 * Copyright (C) 2014 Cadence Design Systems Inc.
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

#ifdef __XTENSA_EL__

# define ARCH_ENDIAN_LITTLE

# define BREAKPOINT_VALUE { 0x00, 0x41, 0x00 }
# define DENSITY_BREAKPOINT_VALUE { 0x2d, 0xf1 }

# define XTENSA_OP0_MASK 0xf
# define XTENSA_DENSITY_FIRST 0x8
# define XTENSA_DENSITY_LAST 0xe
# define XTENSA_SYSCALL_MASK 0xffffff
# define XTENSA_SYSCALL_VALUE 0x005000
# define XTENSA_ENTRY_MASK 0xff
# define XTENSA_ENTRY_VALUE 0x36

#elif defined(__XTENSA_EB__)

# define ARCH_ENDIAN_BIG

# define BREAKPOINT_VALUE { 0x00, 0x14, 0x00 }
# define DENSITY_BREAKPOINT_VALUE { 0xd2, 0x1f }

# define XTENSA_OP0_MASK 0xf0
# define XTENSA_DENSITY_FIRST 0x80
# define XTENSA_DENSITY_LAST 0xe0
# define XTENSA_SYSCALL_MASK 0xffffff00
# define XTENSA_SYSCALL_VALUE 0x00050000
# define XTENSA_ENTRY_MASK 0xff000000
# define XTENSA_ENTRY_VALUE 0x63000000

#else
# error __XTENSA_EL__ or __XTENSA_EB__ must be defined
#endif

#define BREAKPOINT_LENGTH 3
#define DENSITY_BREAKPOINT_LENGTH 2

#define DECR_PC_AFTER_BREAK 0

#define LT_ELFCLASS	ELFCLASS32
#define LT_ELF_MACHINE	EM_XTENSA

static inline int is_density(const void *p)
{
	const unsigned char *bytes = p;
	return (bytes[0] & XTENSA_OP0_MASK) >= XTENSA_DENSITY_FIRST &&
		(bytes[0] & XTENSA_OP0_MASK) < XTENSA_DENSITY_LAST;
}

#define ARCH_HAVE_LTELF_DATA
struct arch_ltelf_data {
};

enum xtensa_plt_type {
	XTENSA_DEFAULT,
	XTENSA_PLT_UNRESOLVED,
	XTENSA_PLT_RESOLVED,
};

#define ARCH_HAVE_LIBRARY_DATA
struct arch_library_data {
	GElf_Addr loadable_sz;
};

#define ARCH_HAVE_LIBRARY_SYMBOL_DATA
struct arch_library_symbol_data {
	enum xtensa_plt_type type;
	GElf_Addr resolved_addr;
};

#define ARCH_HAVE_BREAKPOINT_DATA
struct arch_breakpoint_data {
};

#define ARCH_HAVE_PROCESS_DATA
struct arch_process_data {
	/* Breakpoint that hits when the dynamic linker is about to
	 * update a .plt slot.  NULL before that address is known.  */
	struct breakpoint *dl_plt_update_bp;

	/* PLT update breakpoint looks here for the handler.  */
	struct process_stopping_handler *handler;
};

#define ARCH_HAVE_ADD_PLT_ENTRY
#define ARCH_HAVE_DYNLINK_DONE
#define ARCH_HAVE_ENABLE_BREAKPOINT
#define ARCH_HAVE_GET_SYMINFO
#define ARCH_HAVE_FETCH_ARG
