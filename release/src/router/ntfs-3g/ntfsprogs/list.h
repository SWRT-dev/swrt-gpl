/*
 * list.h - Linked list implementation. Part of the Linux-NTFS project.
 *
 * Copyright (c) 2000-2002 Anton Altaparmakov and others
 *
 * This program/include file is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program/include file is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program (in the main directory of the Linux-NTFS
 * distribution in the file COPYING); if not, write to the Free Software
 * Foundation,Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _NTFS_LIST_H
#define _NTFS_LIST_H

/**
 * struct ntfs_list_head - Simple doubly linked list implementation.
 *
 * Copied from Linux kernel 2.4.2-ac18 into Linux-NTFS (with minor
 * modifications). - AIA
 *
 * Some of the internal functions ("__xxx") are useful when
 * manipulating whole lists rather than single entries, as
 * sometimes we already know the next/prev entries and we can
 * generate better code by using them directly rather than
 * using the generic single-entry routines.
 */
struct ntfs_list_head {
	struct ntfs_list_head *next, *prev;
};

#define NTFS_LIST_HEAD_INIT(name) { &(name), &(name) }

#define NTFS_LIST_HEAD(name) \
	struct ntfs_list_head name = NTFS_LIST_HEAD_INIT(name)

#define NTFS_INIT_LIST_HEAD(ptr) do { \
	(ptr)->next = (ptr); (ptr)->prev = (ptr); \
} while (0)

/**
 * __ntfs_list_add - Insert a new entry between two known consecutive entries.
 * @new:
 * @prev:
 * @next:
 *
 * This is only for internal list manipulation where we know the prev/next
 * entries already!
 */
static __inline__ void __ntfs_list_add(struct ntfs_list_head * new,
		struct ntfs_list_head * prev, struct ntfs_list_head * next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}

/**
 * ntfs_list_add - add a new entry
 * @new:	new entry to be added
 * @head:	list head to add it after
 *
 * Insert a new entry after the specified head.
 * This is good for implementing stacks.
 */
static __inline__ void ntfs_list_add(struct ntfs_list_head *new,
		struct ntfs_list_head *head)
{
	__ntfs_list_add(new, head, head->next);
}

/**
 * ntfs_list_add_tail - add a new entry
 * @new:	new entry to be added
 * @head:	list head to add it before
 *
 * Insert a new entry before the specified head.
 * This is useful for implementing queues.
 */
static __inline__ void ntfs_list_add_tail(struct ntfs_list_head *new,
		struct ntfs_list_head *head)
{
	__ntfs_list_add(new, head->prev, head);
}

/**
 * __ntfs_list_del -
 * @prev:
 * @next:
 *
 * Delete a list entry by making the prev/next entries point to each other.
 *
 * This is only for internal list manipulation where we know the prev/next
 * entries already!
 */
static __inline__ void __ntfs_list_del(struct ntfs_list_head * prev,
		struct ntfs_list_head * next)
{
	next->prev = prev;
	prev->next = next;
}

/**
 * ntfs_list_del - deletes entry from list.
 * @entry:	the element to delete from the list.
 *
 * Note: ntfs_list_empty on entry does not return true after this, the entry is
 * in an undefined state.
 */
static __inline__ void ntfs_list_del(struct ntfs_list_head *entry)
{
	__ntfs_list_del(entry->prev, entry->next);
}

/**
 * ntfs_list_del_init - deletes entry from list and reinitialize it.
 * @entry:	the element to delete from the list.
 */
static __inline__ void ntfs_list_del_init(struct ntfs_list_head *entry)
{
	__ntfs_list_del(entry->prev, entry->next);
	NTFS_INIT_LIST_HEAD(entry);
}

/**
 * ntfs_list_empty - tests whether a list is empty
 * @head:	the list to test.
 */
static __inline__ int ntfs_list_empty(struct ntfs_list_head *head)
{
	return head->next == head;
}

/**
 * ntfs_list_splice - join two lists
 * @list:	the new list to add.
 * @head:	the place to add it in the first list.
 */
static __inline__ void ntfs_list_splice(struct ntfs_list_head *list,
		struct ntfs_list_head *head)
{
	struct ntfs_list_head *first = list->next;

	if (first != list) {
		struct ntfs_list_head *last = list->prev;
		struct ntfs_list_head *at = head->next;

		first->prev = head;
		head->next = first;

		last->next = at;
		at->prev = last;
	}
}

/**
 * ntfs_list_entry - get the struct for this entry
 * @ptr:	the &struct ntfs_list_head pointer.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_struct within the struct.
 */
#define ntfs_list_entry(ptr, type, member) \
	((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))

/**
 * ntfs_list_for_each - iterate over a list
 * @pos:	the &struct ntfs_list_head to use as a loop counter.
 * @head:	the head for your list.
 */
#define ntfs_list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)

/**
 * ntfs_list_for_each_safe	-	iterate over a list safe against removal of list entry
 * @pos:	the &struct ntfs_list_head to use as a loop counter.
 * @n:		another &struct ntfs_list_head to use as temporary storage
 * @head:	the head for your list.
 */
#define ntfs_list_for_each_safe(pos, n, head) \
	for (pos = (head)->next, n = pos->next; pos != (head); \
		pos = n, n = pos->next)

#endif /* defined _NTFS_LIST_H */

