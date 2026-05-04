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

/* _GNU_SOURCE may be necessary for open_memstream visibility (see
 * configure.ac), and there's no harm defining it just in case.  */
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "memstream.h"

int
memstream_init(struct memstream *memstream)
{
#if HAVE_OPEN_MEMSTREAM
	memstream->stream = open_memstream(&memstream->buf,
					   &memstream->size);
#else
	memstream->stream = tmpfile();
	memstream->buf = NULL;
#endif
	return memstream->stream != NULL ? 0 : -1;
}

int
memstream_close(struct memstream *memstream)
{
#if !defined(HAVE_OPEN_MEMSTREAM) || !HAVE_OPEN_MEMSTREAM
	if (fseek(memstream->stream, 0, SEEK_END) < 0) {
	fail:
		fclose(memstream->stream);
		return -1;
	}
	memstream->size = ftell(memstream->stream);
	if (memstream->size == (size_t)-1)
		goto fail;
	memstream->buf = malloc(memstream->size);
	if (memstream->buf == NULL)
		goto fail;

	rewind(memstream->stream);
	if (fread(memstream->buf, 1, memstream->size, memstream->stream)
	    < memstream->size)
		goto fail;
#endif

	return fclose(memstream->stream) == 0 ? 0 : -1;
}

void
memstream_destroy(struct memstream *memstream)
{
	free(memstream->buf);
}
