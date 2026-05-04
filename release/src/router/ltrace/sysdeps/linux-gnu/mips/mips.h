/*
 * This file is part of ltrace.
 * Copyright (C) 2014 Petr Machata, Red Hat Inc.
 * Copyright (C) 2010 Arnaud Patard, Mandriva SA
 * Copyright (C) 2006 Eric Vaitl, Cisco Systems, Inc.
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

#ifndef MIPS_h
#define  MIPS_h
// asm/ptrace.h for these offsets.
#define off_v0 2
#define off_pc 64
#define off_a0 4
#define off_a3 7
#define off_t9 25
#define off_lr 31
#define off_fpr0 32
#define off_sp 29

#endif //  MIPS_h
