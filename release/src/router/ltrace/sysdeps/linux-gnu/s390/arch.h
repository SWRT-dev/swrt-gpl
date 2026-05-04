/*
 * This file is part of ltrace.
 * Copyright (C) 2013 Petr Machata, Red Hat Inc.
 * Copyright (C) 2001 IBM Poughkeepsie, IBM Corporation
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

#define BREAKPOINT_VALUE { 0x00, 0x01 }
#define BREAKPOINT_LENGTH 2
#define DECR_PC_AFTER_BREAK 2
#define ARCH_ENDIAN_BIG
#define ARCH_HAVE_FETCH_ARG
#define ARCH_HAVE_SIZEOF
#define ARCH_HAVE_ALIGNOF
#define ARCH_HAVE_ADD_PLT_ENTRY

#define LT_ELFCLASS	ELFCLASS32
#define LT_ELF_MACHINE	EM_S390

#ifdef __s390x__
#define LT_ELFCLASS2	ELFCLASS64
#define LT_ELF_MACHINE2	EM_S390
#endif
