/*
 * hlist.h - stripped down version of hash list implementation using
 * singly linked list.
 * Doubly linked list is wastage of space for big hash-tables. If cost of
 * iterating a hash list is significant, it means the hash function is NOT
 * formulated well and should be revisited.
 *
 * Copyright (C) 2021 IOPSYS Software Solutions AB. All rights reserved.
 *
 * Author: anjan.chanda@iopsys.eu
 *
 */

#ifndef _HLIST_H
#define _HLIST_H

struct hlist_node {
	struct hlist_node *next;
};

struct hlist_head {
	struct hlist_node *first;
};

#define HLIST_HEAD_INIT(name) { &(name) }

#define HLIST_HEAD(name) struct hlist_head name = HLIST_HEAD_INIT(name)

static inline void INIT_HLIST_HEAD(struct hlist_head *h)
{
	h->first = NULL;
}

static inline void INIT_HLIST_NODE(struct hlist_node *n)
{
	n->next = NULL;
}

static inline int hlist_empty(const struct hlist_head *h)
{
	return !h->first;
}

static inline void __hlist_del(struct hlist_node *prev, struct hlist_node *n)
{
	prev->next = n->next;
	n->next = NULL;
}

static inline void hlist_del(struct hlist_node *n, struct hlist_head *h)
{
	struct hlist_node *p;

	if (h->first == n) {
		h->first = n->next;
		n->next = NULL;
		return;
	}

	for (p = h->first; p; p = p->next) {
		if (p->next == n)
			__hlist_del(p, n);
	}
}

static inline void _hlist_add(struct hlist_node *_new, struct hlist_head *h)
{
	_new->next = h->first;
	h->first = _new;
}

static inline void hlist_add_head(struct hlist_node *n, struct hlist_head *h)
{
	_hlist_add(n, h);
}

#define hlist_for_each(pos, head) \
	for (pos = (head)->first; pos ; pos = pos->next)

#define hlist_for_each_safe(pos, n, head) \
	for (pos = (head)->first; pos && ({ n = pos->next; 1; }); \
	     pos = n)

#define hlist_entry(ptr, type, member) container_of(ptr, type, member)

#define hlist_entry_safe(ptr, type, member) \
	({ typeof(ptr) ____ptr = (ptr); \
	   ____ptr ? hlist_entry(____ptr, type, member) : NULL; \
	})

#define hlist_for_each_entry(pos, head, member)                                \
	for (pos = hlist_entry_safe((head)->first, typeof(*(pos)), member);    \
	     pos;                                                              \
	     pos = hlist_entry_safe((pos)->member.next, typeof(*(pos)), member))

#define hlist_for_each_entry_safe(pos, n, head, member)                   \
	for (pos = hlist_entry_safe((head)->first, typeof(*pos), member); \
	     pos && ({ n = pos->member.next; 1; });                       \
	     pos = hlist_entry_safe(n, typeof(*pos), member))

#endif /* _HLIST_H */
