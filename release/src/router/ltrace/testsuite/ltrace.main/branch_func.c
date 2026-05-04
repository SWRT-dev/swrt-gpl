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

/* This is specially made to produce tail calls.  We then trace them
 * and see if ltrace handles it well, or whether its internal stack
 * overflows.  */

__attribute__((noinline, optimize(3))) int
func3(int i)
{
	return i + 1;
}

__attribute__((noinline, optimize(3))) int
func2(int i)
{
	return func3(i * 3);
}

__attribute__((noinline, optimize(3))) int
func1(int i)
{
	return func2(i + 2);
}

__attribute__((optimize(0))) int
main(int argc, char **argv)
{
	int counter = 0;
	int i;
	for (i = 0; i < 100; ++i)
		counter += func1(i);
	return counter;
}
