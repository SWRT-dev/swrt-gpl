/*
 * This file is part of ltrace.
 * Copyright (C) 2011,2012,2013 Petr Machata, Red Hat Inc.
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

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "vect.h"

static void *
slot(struct vect *vec, size_t i)
{
	return ((unsigned char *)vec->data) + vec->elt_size * i;
}

static const void *
cslot(const struct vect *vec, size_t i)
{
	return ((const unsigned char *)vec->data) + vec->elt_size * i;
}

void
vect_init(struct vect *vec, size_t elt_size)
{
	*vec = (struct vect){ NULL, 0, 0, elt_size };
}

static int
copy_elt(void *tgt, const void *src, void *data)
{
	struct vect *target = data;
	memcpy(tgt, src, target->elt_size);
	return 0;
}

int
vect_clone(struct vect *target, const struct vect *source,
	   int (*clone)(void *tgt, const void *src, void *data),
	   void (*dtor)(void *elt, void *data),
	   void *data)
{
	vect_init(target, source->elt_size);
	if (vect_reserve(target, source->size) < 0)
		return -1;

	if (clone == NULL) {
		assert(dtor == NULL);
		clone = copy_elt;
		data = target;
	} else {
		assert(dtor != NULL);
	}

	size_t i;
	for (i = 0; i < source->size; ++i)
		if (clone(slot(target, i), cslot(source, i), data) < 0)
			goto fail;

	target->size = source->size;
	return 0;

fail:
	/* N.B. destroy the elements in opposite order.  */
	if (dtor != NULL)
		while (i-- != 0)
			dtor(slot(target, i), data);
	vect_destroy(target, NULL, NULL);
	return -1;
}

int
vect_reserve(struct vect *vec, size_t count)
{
	if (count > vec->allocated) {
		size_t na = vec->allocated != 0 ? 2 * vec->allocated : 4;
		while (na < count)
			na *= 2;
		void *n = realloc(vec->data, na * vec->elt_size);
		if (n == NULL)
			return -1;
		vec->data = n;
		vec->allocated = na;
	}
	assert(count <= vec->allocated);
	return 0;
}

size_t
vect_size(const struct vect *vec)
{
	return vec->size;
}

int
vect_empty(const struct vect *vec)
{
	return vec->size == 0;
}

int
vect_reserve_additional(struct vect *vec, size_t count)
{
	return vect_reserve(vec, vect_size(vec) + count);
}

int
vect_pushback(struct vect *vec, void *eltp)
{
	if (vect_reserve_additional(vec, 1) < 0)
		return -1;
	memcpy(slot(vec, vec->size++), eltp, vec->elt_size);
	return 0;
}

void
vect_erase(struct vect *vec, size_t start, size_t end,
	   void (*dtor)(void *emt, void *data), void *data)
{
	assert(start < vect_size(vec) || start == 0);
	assert(end <= vect_size(vec));

	/* First, destroy the elements that are to be erased.  */
	if (dtor != NULL) {
		size_t i;
		for (i = start; i < end; ++i)
			dtor(slot(vec, i), data);
	}

	/* Now move the tail forward and adjust size.  */
	memmove(slot(vec, start), slot(vec, end),
		slot(vec, vec->size) - slot(vec, end));
	vec->size -= end - start;
}

void
vect_popback(struct vect *vec,
	     void (*dtor)(void *emt, void *data), void *data)
{
	assert(vect_size(vec) > 0);
	vect_erase(vec, vect_size(vec)-1, vect_size(vec), dtor, data);
}

void
vect_destroy(struct vect *vec, void (*dtor)(void *emt, void *data), void *data)
{
	if (vec == NULL)
		return;

	vect_erase(vec, 0, vect_size(vec), dtor, data);
	assert(vect_size(vec) == 0);
	free(vec->data);
}

void *
vect_each(struct vect *vec, void *start_after,
	  enum callback_status (*cb)(void *, void *), void *data)
{
	size_t i = start_after == NULL ? 0
		: ((start_after - vec->data) / vec->elt_size) + 1;

	for (; i < vec->size; ++i) {
		void *slt = slot(vec, i);
		switch ((*cb)(slt, data)) {
		case CBS_FAIL:
			/* XXX handle me */
		case CBS_STOP:
			return slt;
		case CBS_CONT:
			break;
		}
	}

	return NULL;
}

void
vect_qsort(struct vect *vec, int (*compar)(const void *, const void *))
{
	qsort(vec->data, vec->size, vec->elt_size, compar);
}

const void *
vect_each_cst(const struct vect *vec, const void *start_after,
	      enum callback_status (*cb)(const void *, void *), void *data)
{
	return vect_each((struct vect *)vec, (void *)start_after,
			 (void *)cb, data);
}

void
vect_dtor_string(char **key, void *data)
{
	free(*key);
}
