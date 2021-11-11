/*
 * libuci - Library for the Unified Configuration Interface
 * Copyright (C) 2008 Felix Fietkau <nbd@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 2.1
 * as published by the Free Software Foundation
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 */

static void uci_list_set_pos(struct uci_list *head, struct uci_list *ptr, int pos)
{
	struct uci_list *new_head = head;
	struct uci_element *p = NULL;

	uci_list_del(ptr);
	uci_foreach_element(head, p) {
		if (pos-- <= 0)
			break;
		new_head = &p->list;
	}

	uci_list_add(new_head->next, ptr);
}

static inline void uci_list_fixup(struct uci_list *ptr)
{
	ptr->prev->next = ptr;
	ptr->next->prev = ptr;
}

/* 
 * uci_alloc_generic allocates a new uci_element with payload
 * payload is appended to the struct to save memory and reduce fragmentation
 */
__private struct uci_element *
uci_alloc_generic(struct uci_context *ctx, int type, const char *name, int size)
{
	struct uci_element *e;
	int datalen = size;
	void *ptr;

	ptr = uci_malloc(ctx, datalen);
	e = (struct uci_element *) ptr;
	e->type = type;
	if (name) {
		UCI_TRAP_SAVE(ctx, error);
		e->name = uci_strdup(ctx, name);
		UCI_TRAP_RESTORE(ctx);
	}
	uci_list_init(&e->list);
	goto done;

error:
	free(ptr);
	UCI_THROW(ctx, ctx->err);

done:
	return e;
}

__private void
uci_free_element(struct uci_element *e)
{
	if (e->name)
		free(e->name);
	if (!uci_list_empty(&e->list))
		uci_list_del(&e->list);
	free(e);
}

static struct uci_option *
uci_alloc_option(struct uci_section *s, const char *name, const char *value)
{
	struct uci_package *p = s->package;
	struct uci_context *ctx = p->ctx;
	struct uci_option *o;

	o = uci_alloc_element(ctx, option, name, strlen(value) + 1);
	o->type = UCI_TYPE_STRING;
	o->v.string = uci_dataptr(o);
	o->section = s;
	strcpy(o->v.string, value);
	uci_list_add(&s->options, &o->e.list);

	return o;
}

static inline void
uci_free_option(struct uci_option *o)
{
	struct uci_element *e, *tmp;

	switch(o->type) {
	case UCI_TYPE_STRING:
		if ((o->v.string != uci_dataptr(o)) &&
			(o->v.string != NULL))
			free(o->v.string);
		break;
	case UCI_TYPE_LIST:
		uci_foreach_element_safe(&o->v.list, tmp, e) {
			uci_free_element(e);
		}
		break;
	default:
		break;
	}
	uci_free_element(&o->e);
}

static struct uci_option *
uci_alloc_list(struct uci_section *s, const char *name)
{
	struct uci_package *p = s->package;
	struct uci_context *ctx = p->ctx;
	struct uci_option *o;

	o = uci_alloc_element(ctx, option, name, 0);
	o->type = UCI_TYPE_LIST;
	o->section = s;
	uci_list_init(&o->v.list);
	uci_list_add(&s->options, &o->e.list);

	return o;
}

/* Based on an efficient hash function published by D. J. Bernstein */
static unsigned int djbhash(unsigned int hash, char *str)
{
	int len = strlen(str);
	int i;

	/* initial value */
	if (hash == ~0)
		hash = 5381;

	for(i = 0; i < len; i++) {
		hash = ((hash << 5) + hash) + str[i];
	}
	return (hash & 0x7FFFFFFF);
}

/* fix up an unnamed section, e.g. after adding options to it */
__private void uci_fixup_section(struct uci_context *ctx, struct uci_section *s)
{
	unsigned int hash = ~0;
	struct uci_element *e;
	char buf[16];

	if (!s || s->e.name)
		return;

	/*
	 * Generate a name for unnamed sections. This is used as reference
	 * when locating or updating the section from apps/scripts.
	 * To make multiple concurrent versions somewhat safe for updating,
	 * the name is generated from a hash of its type and name/value
	 * pairs of its option, and it is prefixed by a counter value.
	 * If the order of the unnamed sections changes for some reason,
	 * updates to them will be rejected.
	 */
	hash = djbhash(hash, s->type);
	uci_foreach_element(&s->options, e) {
		struct uci_option *o;
		hash = djbhash(hash, e->name);
		o = uci_to_option(e);
		switch(o->type) {
		case UCI_TYPE_STRING:
			hash = djbhash(hash, o->v.string);
			break;
		default:
			break;
		}
	}
	sprintf(buf, "cfg%02x%04x", ++s->package->n_section, hash % (1 << 16));
	s->e.name = uci_strdup(ctx, buf);
}

static struct uci_section *
uci_alloc_section(struct uci_package *p, const char *type, const char *name)
{
	struct uci_context *ctx = p->ctx;
	struct uci_section *s;

	if (name && !name[0])
		name = NULL;

	s = uci_alloc_element(ctx, section, name, strlen(type) + 1);
	uci_list_init(&s->options);
	s->type = uci_dataptr(s);
	s->package = p;
	strcpy(s->type, type);
	if (name == NULL)
		s->anonymous = true;
	p->n_section++;

	uci_list_add(&p->sections, &s->e.list);

	return s;
}

static void
uci_free_section(struct uci_section *s)
{
	struct uci_element *o, *tmp;

	uci_foreach_element_safe(&s->options, tmp, o) {
		uci_free_option(uci_to_option(o));
	}
	if ((s->type != uci_dataptr(s)) &&
		(s->type != NULL))
		free(s->type);
	uci_free_element(&s->e);
}

__plugin struct uci_package *
uci_alloc_package(struct uci_context *ctx, const char *name)
{
	struct uci_package *p;

	p = uci_alloc_element(ctx, package, name, 0);
	p->ctx = ctx;
	uci_list_init(&p->sections);
	uci_list_init(&p->delta);
	uci_list_init(&p->saved_delta);
	return p;
}

__private void
uci_free_package(struct uci_package **package)
{
	struct uci_element *e, *tmp;
	struct uci_package *p = *package;

	if(!p)
		return;

	if (p->path)
		free(p->path);
	uci_foreach_element_safe(&p->sections, tmp, e) {
		uci_free_section(uci_to_section(e));
	}
	uci_foreach_element_safe(&p->delta, tmp, e) {
		uci_free_delta(uci_to_delta(e));
	}
	uci_foreach_element_safe(&p->saved_delta, tmp, e) {
		uci_free_delta(uci_to_delta(e));
	}
	uci_free_element(&p->e);
	*package = NULL;
}

static void
uci_free_any(struct uci_element **e)
{
	switch((*e)->type) {
	case UCI_TYPE_SECTION:
		uci_free_section(uci_to_section(*e));
		break;
	case UCI_TYPE_OPTION:
		uci_free_option(uci_to_option(*e));
		break;
	default:
		break;
	}
	*e = NULL;
}

__private struct uci_element *
uci_lookup_list(struct uci_list *list, const char *name)
{
	struct uci_element *e;

	uci_foreach_element(list, e) {
		if (!strcmp(e->name, name))
			return e;
	}
	return NULL;
}

static struct uci_element *
uci_lookup_ext_section(struct uci_context *ctx, struct uci_ptr *ptr)
{
	char *idxstr, *t, *section, *name;
	struct uci_element *e = NULL;
	struct uci_section *s;
	int idx, c;

	section = uci_strdup(ctx, ptr->section);
	name = idxstr = section + 1;

	if (section[0] != '@')
		goto error;

	/* parse the section index part */
	idxstr = strchr(idxstr, '[');
	if (!idxstr)
		goto error;
	*idxstr = 0;
	idxstr++;

	t = strchr(idxstr, ']');
	if (!t)
		goto error;
	if (t[1] != 0)
		goto error;
	*t = 0;

	t = NULL;
	idx = strtol(idxstr, &t, 10);
	if (t && *t)
		goto error;

	if (!*name)
		name = NULL;
	else if (!uci_validate_type(name))
		goto error;

	/* if the given index is negative, it specifies the section number from 
	 * the end of the list */
	if (idx < 0) {
		c = 0;
		uci_foreach_element(&ptr->p->sections, e) {
			s = uci_to_section(e);
			if (name && (strcmp(s->type, name) != 0))
				continue;

			c++;
		}
		idx += c;
	}

	c = 0;
	uci_foreach_element(&ptr->p->sections, e) {
		s = uci_to_section(e);
		if (name && (strcmp(s->type, name) != 0))
			continue;

		if (idx == c)
			goto done;
		c++;
	}
	e = NULL;
	goto done;

error:
	e = NULL;
	memset(ptr, 0, sizeof(struct uci_ptr));
	UCI_THROW(ctx, UCI_ERR_INVAL);
done:
	free(section);
	if (e)
		ptr->section = e->name;
	return e;
}

int
uci_lookup_next(struct uci_context *ctx, struct uci_element **e, struct uci_list *list, const char *name)
{
	UCI_HANDLE_ERR(ctx);

	*e = uci_lookup_list(list, name);
	if (!*e)
		UCI_THROW(ctx, UCI_ERR_NOTFOUND);

	return 0;
}

int
uci_lookup_ptr(struct uci_context *ctx, struct uci_ptr *ptr, char *str, bool extended)
{
	struct uci_element *e;

	UCI_HANDLE_ERR(ctx);
	UCI_ASSERT(ctx, ptr != NULL);

	if (str)
		UCI_INTERNAL(uci_parse_ptr, ctx, ptr, str);

	ptr->flags |= UCI_LOOKUP_DONE;

	/* look up the package first */
	if (ptr->p)
		e = &ptr->p->e;
	else
		e = uci_lookup_list(&ctx->root, ptr->package);

	if (!e) {
		UCI_INTERNAL(uci_load, ctx, ptr->package, &ptr->p);
		if (!ptr->p)
			goto notfound;
		ptr->last = &ptr->p->e;
	} else {
		ptr->p = uci_to_package(e);
		ptr->last = e;
	}

	if (!ptr->section && !ptr->s)
		goto complete;

	/* if the section name validates as a regular name, pass through
	 * to the regular uci_lookup function call */
	if (ptr->s) {
		e = &ptr->s->e;
	} else if (ptr->flags & UCI_LOOKUP_EXTENDED) {
		if (extended)
			e = uci_lookup_ext_section(ctx, ptr);
		else
			UCI_THROW(ctx, UCI_ERR_INVAL);
	} else {
		e = uci_lookup_list(&ptr->p->sections, ptr->section);
	}

	if (!e)
		goto abort;

	ptr->last = e;
	ptr->s = uci_to_section(e);

	if (ptr->option) {
		e = uci_lookup_list(&ptr->s->options, ptr->option);
		if (!e)
			goto abort;

		ptr->o = uci_to_option(e);
		ptr->last = e;
	}

complete:
	ptr->flags |= UCI_LOOKUP_COMPLETE;
abort:
	return 0;

notfound:
	UCI_THROW(ctx, UCI_ERR_NOTFOUND);
	return 0;
}

__private struct uci_element *
uci_expand_ptr(struct uci_context *ctx, struct uci_ptr *ptr, bool complete)
{
	UCI_ASSERT(ctx, ptr != NULL);

	if (!(ptr->flags & UCI_LOOKUP_DONE))
		UCI_INTERNAL(uci_lookup_ptr, ctx, ptr, NULL, 1);
	if (complete && !(ptr->flags & UCI_LOOKUP_COMPLETE))
		UCI_THROW(ctx, UCI_ERR_NOTFOUND);
	UCI_ASSERT(ctx, ptr->p != NULL);

	/* fill in missing string info */
	if (ptr->p && !ptr->package)
		ptr->package = ptr->p->e.name;
	if (ptr->s && !ptr->section)
		ptr->section = ptr->s->e.name;
	if (ptr->o && !ptr->option)
		ptr->option = ptr->o->e.name;

	if (ptr->o)
		return &ptr->o->e;
	if (ptr->s)
		return &ptr->s->e;
	if (ptr->p)
		return &ptr->p->e;
	else
		return NULL;
}

static void uci_add_element_list(struct uci_context *ctx, struct uci_ptr *ptr, bool internal)
{
	struct uci_element *e;
	struct uci_package *p;

	p = ptr->p;
	if (!internal && p->has_delta)
		uci_add_delta(ctx, &p->delta, UCI_CMD_LIST_ADD, ptr->section, ptr->option, ptr->value);

	e = uci_alloc_generic(ctx, UCI_TYPE_ITEM, ptr->value, sizeof(struct uci_option));
	uci_list_add(&ptr->o->v.list, &e->list);
}

int uci_rename(struct uci_context *ctx, struct uci_ptr *ptr)
{
	/* NB: UCI_INTERNAL use means without delta tracking */
	bool internal = ctx->internal;
	struct uci_element *e;
	struct uci_package *p;
	char *n;

	UCI_HANDLE_ERR(ctx);

	e = uci_expand_ptr(ctx, ptr, true);
	p = ptr->p;

	UCI_ASSERT(ctx, ptr->s);
	UCI_ASSERT(ctx, ptr->value);

	if (!internal && p->has_delta)
		uci_add_delta(ctx, &p->delta, UCI_CMD_RENAME, ptr->section, ptr->option, ptr->value);

	n = uci_strdup(ctx, ptr->value);
	if (e->name)
		free(e->name);
	e->name = n;

	if (e->type == UCI_TYPE_SECTION)
		uci_to_section(e)->anonymous = false;

	return 0;
}

int uci_reorder_section(struct uci_context *ctx, struct uci_section *s, int pos)
{
	struct uci_package *p = s->package;
	char order[32];

	UCI_HANDLE_ERR(ctx);

	uci_list_set_pos(&s->package->sections, &s->e.list, pos);
	if (!ctx->internal && p->has_delta) {
		sprintf(order, "%d", pos);
		uci_add_delta(ctx, &p->delta, UCI_CMD_REORDER, s->e.name, NULL, order);
	}

	return 0;
}

int uci_add_section(struct uci_context *ctx, struct uci_package *p, const char *type, struct uci_section **res)
{
	bool internal = ctx->internal;
	struct uci_section *s;

	UCI_HANDLE_ERR(ctx);
	UCI_ASSERT(ctx, p != NULL);
	s = uci_alloc_section(p, type, NULL);
	uci_fixup_section(ctx, s);
	*res = s;
	if (!internal && p->has_delta)
		uci_add_delta(ctx, &p->delta, UCI_CMD_ADD, s->e.name, NULL, type);

	return 0;
}

int uci_delete(struct uci_context *ctx, struct uci_ptr *ptr)
{
	/* NB: pass on internal flag to uci_del_element */
	bool internal = ctx->internal;
	struct uci_package *p;
	struct uci_element *e;

	UCI_HANDLE_ERR(ctx);

	e = uci_expand_ptr(ctx, ptr, true);
	p = ptr->p;

	UCI_ASSERT(ctx, ptr->s);

	if (!internal && p->has_delta)
		uci_add_delta(ctx, &p->delta, UCI_CMD_REMOVE, ptr->section, ptr->option, NULL);

	uci_free_any(&e);

	if (ptr->option)
		ptr->o = NULL;
	else if (ptr->section)
		ptr->s = NULL;

	return 0;
}

int uci_add_list(struct uci_context *ctx, struct uci_ptr *ptr)
{
	/* NB: UCI_INTERNAL use means without delta tracking */
	bool internal = ctx->internal;
	struct uci_option *prev = NULL;
	const char *value2 = NULL;

	UCI_HANDLE_ERR(ctx);

	uci_expand_ptr(ctx, ptr, false);
	UCI_ASSERT(ctx, ptr->s);
	UCI_ASSERT(ctx, ptr->value);

	if (ptr->o) {
		switch (ptr->o->type) {
		case UCI_TYPE_STRING:
			/* we already have a string value, convert that to a list */
			prev = ptr->o;
			value2 = ptr->value;
			ptr->value = ptr->o->v.string;
			break;
		case UCI_TYPE_LIST:
			uci_add_element_list(ctx, ptr, internal);
			return 0;
		default:
			UCI_THROW(ctx, UCI_ERR_INVAL);
			break;
		}
	}

	ptr->o = uci_alloc_list(ptr->s, ptr->option);
	if (prev) {
		uci_add_element_list(ctx, ptr, true);
		uci_free_option(prev);
		ptr->value = value2;
	}
	uci_add_element_list(ctx, ptr, internal);

	return 0;
}

int uci_set(struct uci_context *ctx, struct uci_ptr *ptr)
{
	/* NB: UCI_INTERNAL use means without delta tracking */
	bool internal = ctx->internal;

	UCI_HANDLE_ERR(ctx);
	uci_expand_ptr(ctx, ptr, false);
	UCI_ASSERT(ctx, ptr->value);
	UCI_ASSERT(ctx, ptr->s || (!ptr->option && ptr->section));
	if (!ptr->option && ptr->value[0]) {
		UCI_ASSERT(ctx, uci_validate_type(ptr->value));
	}

	if (!ptr->o && ptr->s && ptr->option) {
		struct uci_element *e;
		e = uci_lookup_list(&ptr->s->options, ptr->option);
		if (e)
			ptr->o = uci_to_option(e);
	}
	if (!ptr->value[0]) {
		/* if setting a nonexistant option/section to a nonexistant value,
		 * exit without errors */
		if (!(ptr->flags & UCI_LOOKUP_COMPLETE))
			return 0;

		return uci_delete(ctx, ptr);
	} else if (!ptr->o && ptr->option) { /* new option */
		ptr->o = uci_alloc_option(ptr->s, ptr->option, ptr->value);
		ptr->last = &ptr->o->e;
	} else if (!ptr->s && ptr->section) { /* new section */
		ptr->s = uci_alloc_section(ptr->p, ptr->value, ptr->section);
		ptr->last = &ptr->s->e;
	} else if (ptr->o && ptr->option) { /* update option */
		if ((ptr->o->type == UCI_TYPE_STRING) &&
			!strcmp(ptr->o->v.string, ptr->value))
			return 0;
		uci_free_option(ptr->o);
		ptr->o = uci_alloc_option(ptr->s, ptr->option, ptr->value);
		ptr->last = &ptr->o->e;
	} else if (ptr->s && ptr->section) { /* update section */
		char *s = uci_strdup(ctx, ptr->value);

		if (ptr->s->type == uci_dataptr(ptr->s)) {
			ptr->last = NULL;
			ptr->last = uci_realloc(ctx, ptr->s, sizeof(struct uci_section));
			ptr->s = uci_to_section(ptr->last);
			uci_list_fixup(&ptr->s->e.list);
		} else {
			free(ptr->s->type);
		}
		ptr->s->type = s;
	} else {
		UCI_THROW(ctx, UCI_ERR_INVAL);
	}

	if (!internal && ptr->p->has_delta)
		uci_add_delta(ctx, &ptr->p->delta, UCI_CMD_CHANGE, ptr->section, ptr->option, ptr->value);

	return 0;
}

int uci_unload(struct uci_context *ctx, struct uci_package *p)
{
	UCI_HANDLE_ERR(ctx);
	UCI_ASSERT(ctx, p != NULL);

	uci_free_package(&p);
	return 0;
}

