/*
 * This file is part of ltrace.
 * Copyright (C) 2014 Petr Machata, Red Hat, Inc.
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
#ifndef LTRACE_AARCH64_ARCH_H
#define LTRACE_AARCH64_ARCH_H

/* | 31                 21 | 20       5 | 4       0 | *
 * | 1 1 0 1 0 1 0 0 0 0 1 |    imm16   | 0 0 0 0 0 | */
#define BREAKPOINT_VALUE { 0xd4, 0x20, 0, 0 }
#define BREAKPOINT_LENGTH 4
#define DECR_PC_AFTER_BREAK 0

#define LT_ELFCLASS	ELFCLASS64
#define LT_ELF_MACHINE	EM_AARCH64

#define ARCH_HAVE_FETCH_ARG
#define ARCH_ENDIAN_BIG
#define ARCH_HAVE_SIZEOF
#define ARCH_HAVE_ALIGNOF

#endif /* LTRACE_AARCH64_ARCH_H */
