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

#include <stdint.h>

__attribute__((noinline, optimize(3))) void
atomic_add(uint32_t *a, uint32_t b)
{
	__asm__ volatile("lwarx 9,0,%0\n"
			 "add 9,9,%2\n"
			 "stwcx. 9,0,%0\n"
			 "bne- atomic_add\n"
			 : "=r"(a)
			 : "0"(a), "r"(b)
			 : "%r9");
}

uint32_t a = 0;

__attribute__((optimize(0))) int
main(int argc, char **argv)
{
	atomic_add(&a, 5);
	atomic_add(&a, 10);
	atomic_add(&a, 15);
	return a;
}
