/*
 * This file is part of ltrace.
 * Copyright (C) 2004,2009 Juan Cespedes
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

#undef PTRACE_GETREGS
#undef PTRACE_SETREGS
#undef PTRACE_GETFPREGS
#undef PTRACE_SETFPREGS
#include <sys/ptrace.h>
#ifndef PTRACE_SUNDETACH
#define PTRACE_SUNDETACH 11
#endif
#undef PT_DETACH
#undef PTRACE_DETACH
#define PT_DETACH PTRACE_SUNDETACH
#define PTRACE_DETACH PTRACE_SUNDETACH

#include <asm/ptrace.h>

typedef struct {
	int valid;
	struct pt_regs regs;
	unsigned int func_arg[6];
	unsigned int sysc_arg[6];
} proc_archdep;
