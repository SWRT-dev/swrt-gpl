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

#ifndef MEMSTREAM_H
#define MEMSTREAM_H

#include <stdio.h>

/* Portability wrapper for platforms that don't have
 * open_memstream.  */

struct memstream
{
	FILE *stream;
	char *buf;
	size_t size;
};

int memstream_init(struct memstream *memstream);
int memstream_close(struct memstream *memstream);
void memstream_destroy(struct memstream *memstream);

#endif /* MEMSTREAM_H */
