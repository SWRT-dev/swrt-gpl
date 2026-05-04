/*
 * This file is part of ltrace.
 * Copyright (C) 2013,2014 Petr Machata, Red Hat Inc.
 * Copyright (C) 2006 Eric Vaitl
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

#ifndef LTRACE_MIPS_ARCH_H
#define LTRACE_MIPS_ARCH_H

#include <stddef.h>
#include <gelf.h>

#ifdef __MIPSEL__
# define BREAKPOINT_VALUE { 0x0d, 0x00, 0x00, 0x00 }
# define ARCH_ENDIAN_LITTLE
#elif defined(__MIPSEB__)
# define BREAKPOINT_VALUE { 0x00, 0x00, 0x00, 0x0d }
# define ARCH_ENDIAN_BIG
#else
# error __MIPSEL__ or __MIPSEB__ must be defined
#endif

#define BREAKPOINT_LENGTH 4
#define DECR_PC_AFTER_BREAK 0

#define LT_ELFCLASS	ELFCLASS32
#define LT_ELF_MACHINE	EM_MIPS

#define ARCH_HAVE_LTELF_DATA
struct arch_ltelf_data {
	size_t pltgot_addr;
	size_t mips_local_gotno;
	size_t mips_gotsym;
};

#define ARCH_HAVE_FIND_DL_DEBUG
#define ARCH_HAVE_DYNLINK_DONE
#define ARCH_HAVE_ADD_PLT_ENTRY
#define ARCH_HAVE_SW_SINGLESTEP
#define ARCH_HAVE_SYMBOL_RET

#define ARCH_HAVE_LIBRARY_SYMBOL_DATA
enum mips_plt_type
{
	/* A symbol has associated PLT entry.  */
	MIPS_PLT_DEFAULT,

	/* The GOT entry contains unresolved value.  RESOLVED_ADDR
	 * then contains stub address.  */
	MIPS_PLT_UNRESOLVED,

	/* The GOT entry contained resolved value, and was unresolved.
	 * The original value was saved to RESOLVED_ADDR.  */
	MIPS_PLT_RESOLVED,

	/* Symbol needs to be unresolved after it's enabled.
	 * RESOLVED_ADDR is undefined, instead DATA is carried.  */
	MIPS_PLT_NEED_UNRESOLVE,
};

struct mips_unresolve_data;
struct arch_library_symbol_data {
	enum mips_plt_type type;
	union {
		GElf_Addr resolved_value;
		struct mips_unresolve_data *data;
	};
	GElf_Addr got_entry_addr;
};

#define ARCH_HAVE_BREAKPOINT_DATA
struct arch_breakpoint_data {
};

#define ARCH_HAVE_PROCESS_DATA
struct arch_process_data {
	/* Breakpoint that hits when the dynamic linker is about to
	 * update a GOT entry.  NULL before that address is known.  */
	struct breakpoint *dl_got_update_bp;

	/* PLT update breakpoint looks here for the handler.  */
	struct process_stopping_handler *handler;
};

#endif /* LTRACE_MIPS_ARCH_H */
