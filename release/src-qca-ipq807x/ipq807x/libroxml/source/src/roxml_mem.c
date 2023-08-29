/**
 * \file roxml_mem.c
 * \brief XML internal memory management module
 *
 * (C) Copyright 2014
 * Tristan Lelong <tristan.lelong@libroxml.net>
 *
 * SPDX-Licence-Identifier:	LGPL-2.1+
 * The author added a static linking exception, see License.txt.
 */

#include <stdlib.h>
#include "roxml_mem.h"

/** \brief head of memory manager */
memory_cell_t head_cell = { PTR_NONE, 0, NULL, 0, NULL, NULL };

ROXML_STATIC ROXML_INT inline void roxml_release_last(void *data)
{
	memory_cell_t *ptr = &head_cell;
	memory_cell_t *to_delete = NULL;

	while ((ptr->prev != NULL) && (ptr->prev->id != pthread_self()))
		ptr = ptr->prev;

	if (ptr->prev == NULL)
		return;

	to_delete = ptr->prev;

	if (to_delete->next) {
		to_delete->prev->next = to_delete->next;
		to_delete->next->prev = to_delete->prev;
	} else {
		if (to_delete->prev != &head_cell)
			head_cell.prev = to_delete->prev;
		else
			head_cell.prev = NULL;
		to_delete->prev->next = NULL;
	}

	if (PTR_IS_STAR(to_delete)) {
		int i = 0;
		for (i = 0; i < to_delete->occ; i++)
			free(((void **)(to_delete->ptr))[i]);
	}
	if (to_delete->type != PTR_NONE) {
		free(to_delete->ptr);
		to_delete->type = PTR_NONE;
		free(to_delete);
	}
}

ROXML_STATIC ROXML_INT inline void roxml_release_all(void *data)
{
	memory_cell_t *to_delete = NULL;

	head_cell.prev = NULL;

	while ((head_cell.next != NULL)) {
		to_delete = head_cell.next;
		if (to_delete->next)
			to_delete->next->prev = &head_cell;
		head_cell.next = to_delete->next;

		if (PTR_IS_STAR(to_delete)) {
			int i = 0;
			for (i = 0; i < to_delete->occ; i++)
				free(((void **)(to_delete->ptr))[i]);
		}
		free(to_delete->ptr);
		to_delete->ptr = NULL;
		to_delete->type = PTR_NONE;
		free(to_delete);
	}
}

ROXML_STATIC ROXML_INT inline void roxml_release_pointer(void *data)
{
	memory_cell_t *ptr = &head_cell;
	memory_cell_t *to_delete = NULL;

	while ((ptr->next != NULL) && (ptr->next->ptr != data))
		ptr = ptr->next;

	if (ptr->next == NULL)
		return;

	to_delete = ptr->next;
	if (to_delete->next) {
		to_delete->next->prev = ptr;
	} else {
		if (ptr == &head_cell)
			head_cell.prev = NULL;
		else
			head_cell.prev = to_delete->prev;
	}
	ptr->next = to_delete->next;
	if (PTR_IS_STAR(to_delete)) {
		int i = 0;
		for (i = 0; i < to_delete->occ; i++)
			free(((void **)(to_delete->ptr))[i]);
	}
	free(to_delete->ptr);
	to_delete->type = PTR_NONE;
	free(to_delete);
}

ROXML_API void roxml_release(void *data)
{
	if (data == RELEASE_LAST)
		roxml_release_last(data);
	else if (data == RELEASE_ALL)
		roxml_release_all(data);
	else
		roxml_release_pointer(data);

	if (head_cell.next == &head_cell)
		head_cell.next = NULL;
	if (head_cell.prev == &head_cell)
		head_cell.prev = NULL;
}

ROXML_INT void *roxml_malloc(int size, int num, int type)
{
	memory_cell_t *cell = &head_cell;
	while (cell->next != NULL)
		cell = cell->next;

	cell->next = malloc(sizeof(memory_cell_t));
	if (!cell->next)
		return NULL;

	cell->next->next = NULL;
	cell->next->prev = cell;
	cell = cell->next;
	cell->type = type;
	cell->id = pthread_self();
	cell->occ = size;
	cell->ptr = calloc(num, size);
	head_cell.prev = cell;

	return cell->ptr;
}
