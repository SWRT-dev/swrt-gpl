/*
 * This file is part of ltrace.
 * Copyright (C) 2012 Petr Machata, Red Hat Inc.
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

/* Instruction masks used during single-stepping of atomic
 * sequences.  This was lifted from GDB.  */
#define LWARX_MASK 0xfc0007fe
#define LWARX_INSTRUCTION 0x7c000028
#define LDARX_INSTRUCTION 0x7c0000A8
#define STWCX_MASK 0xfc0007ff
#define STWCX_INSTRUCTION 0x7c00012d
#define STDCX_INSTRUCTION 0x7c0001ad
#define BRANCH_MASK 0xfc000000
#define BC_MASK 0xfc000000
#define BC_INSN 0x40000000
#define B_INSN 0x48000000

static inline arch_addr_t
ppc_branch_dest(arch_addr_t addr, uint32_t insn)
{
	int immediate = ((insn & 0xfffc) ^ 0x8000) - 0x8000;
	int absolute = insn & 2;

	/* XXX drop the following double casts.  */
	if (absolute)
		return (arch_addr_t)(uintptr_t)immediate;
	else
		return addr + (uintptr_t)immediate;
}
