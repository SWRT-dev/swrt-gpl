/*
 * This file is part of ltrace.
 * Copyright (C) 2006,2011,2012 Petr Machata, Red Hat Inc.
 * Copyright (C) 2006 Ian Wienand
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

#define ARCH_HAVE_DISABLE_BREAKPOINT 1
#define ARCH_HAVE_ENABLE_BREAKPOINT 1
#define ARCH_HAVE_FETCH_ARG
#define ARCH_HAVE_FETCH_PACK

#define BREAKPOINT_LENGTH 16
#define BREAKPOINT_VALUE {0}
#define DECR_PC_AFTER_BREAK 0
#define ARCH_ENDIAN_LITTLE
#define ARCH_HAVE_TRANSLATE_ADDRESS

#define LT_ELFCLASS   ELFCLASS64
#define LT_ELF_MACHINE EM_IA_64

// ia64 actually does use .opd, but we don't need to do the
// translation manually.
#undef ARCH_SUPPORTS_OPD
