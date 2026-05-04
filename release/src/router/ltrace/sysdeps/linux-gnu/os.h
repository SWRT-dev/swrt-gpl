/*
 * This file is part of ltrace.
 * Copyright (C) 2012 Petr Machata
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

#define OS_HAVE_PROCESS_DATA
struct os_process_data {
	arch_addr_t debug_addr;
	int debug_state;
};

#define OS_HAVE_LIBRARY_SYMBOL_DATA
struct os_library_symbol_data {
	unsigned is_ifunc : 1;
};

#define OS_HAVE_BREAKPOINT_DATA
struct os_breakpoint_data {
	/* For breakpoints that track return from IFUNC functions, we
	 * keep here the IFUNC symbol itself.  */
	struct library_symbol *ret_libsym;
};

#define OS_HAVE_ADD_FUNC_ENTRY
