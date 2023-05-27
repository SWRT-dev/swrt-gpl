/*
 * ucimap.c - Library for the Unified Configuration Interface
 * Copyright (C) 2008-2009 Felix Fietkau <nbd@openwrt.org>
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

/*
 * This file contains ucimap, an API for mapping UCI to C data structures
 */

#include <strings.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include "ucimap.h"
#include "uci_internal.h"

struct ucimap_alloc {
	void *ptr;
};

struct ucimap_alloc_custom {
	void *section;
	struct uci_optmap *om;
	void *ptr;
};

struct ucimap_fixup {
	struct ucimap_fixup *next;
	struct uci_sectionmap *sm;
	const char *name;
	enum ucimap_type type;
	union ucimap_data *data;
};

#define ucimap_foreach_option(_sm, _o) \
	if (!(_sm)->options_size) \
		(_sm)->options_size = sizeof(struct uci_optmap); \
	for (_o = &(_sm)->options[0]; \
		 ((char *)(_o)) < ((char *) &(_sm)->options[0] + \
			(_sm)->options_size * (_sm)->n_options); \
		 _o = (struct uci_optmap *) ((char *)(_o) + \
			(_sm)->options_size))


static inline bool
ucimap_is_alloc(enum ucimap_type type)
{
	return (type & UCIMAP_SUBTYPE) == UCIMAP_STRING;
}

static inline bool
ucimap_is_fixup(enum ucimap_type type)
{
	return (type & UCIMAP_SUBTYPE) == UCIMAP_SECTION;
}

static inline bool
ucimap_is_simple(enum ucimap_type type)
{
	return ((type & UCIMAP_TYPE) == UCIMAP_SIMPLE);
}

static inline bool
ucimap_is_list(enum ucimap_type type)
{
	return ((type & UCIMAP_TYPE) == UCIMAP_LIST);
}

static inline bool
ucimap_is_list_auto(enum ucimap_type type)
{
	return ucimap_is_list(type) && !!(type & UCIMAP_LIST_AUTO);
}

static inline bool
ucimap_is_custom(enum ucimap_type type)
{
	return ((type & UCIMAP_SUBTYPE) == UCIMAP_CUSTOM);
}

static inline void *
ucimap_section_ptr(struct ucimap_section_data *sd)
{
	return ((char *) sd - sd->sm->smap_offset);
}

static inline struct ucimap_section_data *
ucimap_ptr_section(struct uci_sectionmap *sm, void *ptr) {
	ptr = (char *) ptr + sm->smap_offset;
	return ptr;
}

static inline union ucimap_data *
ucimap_get_data(struct ucimap_section_data *sd, struct uci_optmap *om)
{
	void *data;

	data = (char *) ucimap_section_ptr(sd) + om->offset;
	return data;
}

int
ucimap_init(struct uci_map *map)
{
	map->fixup = NULL;
	map->sdata = NULL;
	map->fixup_tail = &map->fixup;
	map->sdata_tail = &map->sdata;
	return 0;
}

static void
ucimap_add_alloc(struct ucimap_section_data *sd, void *ptr)
{
	struct ucimap_alloc *a = &sd->allocmap[sd->allocmap_len++];
	a->ptr = ptr;
}

void
ucimap_free_section(struct uci_map *map, struct ucimap_section_data *sd)
{
	void *section;
	unsigned int i;

	section = ucimap_section_ptr(sd);
	if (sd->ref)
		*sd->ref = sd->next;

	if (sd->sm->free)
		sd->sm->free(map, section);

	for (i = 0; i < sd->allocmap_len; i++) {
		free(sd->allocmap[i].ptr);
	}

	if (sd->alloc_custom) {
		for (i = 0; i < sd->alloc_custom_len; i++) {
			struct ucimap_alloc_custom *a = &sd->alloc_custom[i];
			a->om->free(a->section, a->om, a->ptr);
		}
		free(sd->alloc_custom);
	}

	free(sd->allocmap);
	free(sd);
}

void
ucimap_cleanup(struct uci_map *map)
{
	struct ucimap_section_data *sd, *sd_next;

	for (sd = map->sdata; sd; sd = sd_next) {
		sd_next = sd->next;
		ucimap_free_section(map, sd);
	}
}

static void *
ucimap_find_section(struct uci_map *map, struct ucimap_fixup *f)
{
	struct ucimap_section_data *sd;

	for (sd = map->sdata; sd; sd = sd->next) {
		if (sd->sm != f->sm)
			continue;
		if (strcmp(f->name, sd->section_name) != 0)
			continue;
		return ucimap_section_ptr(sd);
	}
	for (sd = map->pending; sd; sd = sd->next) {
		if (sd->sm != f->sm)
			continue;
		if (strcmp(f->name, sd->section_name) != 0)
			continue;
		return ucimap_section_ptr(sd);
	}
	return NULL;
}

static union ucimap_data *
ucimap_list_append(struct ucimap_list *list)
{
	if (unlikely(list->size <= list->n_items)) {
		/* should not happen */
		DPRINTF("ERROR: overflow while filling a list (size=%d)\n", list->size);
		return NULL;
	}
	return &list->item[list->n_items++];
}


static bool
ucimap_handle_fixup(struct uci_map *map, struct ucimap_fixup *f)
{
	void *ptr = ucimap_find_section(map, f);
	union ucimap_data *data;

	if (!ptr)
		return false;

	switch(f->type & UCIMAP_TYPE) {
	case UCIMAP_SIMPLE:
		f->data->ptr = ptr;
		break;
	case UCIMAP_LIST:
		data = ucimap_list_append(f->data->list);
		if (!data)
			return false;

		data->ptr = ptr;
		break;
	}
	return true;
}

void
ucimap_free_item(struct ucimap_section_data *sd, void *item)
{
	struct ucimap_alloc_custom *ac;
	struct ucimap_alloc *a;
	void *ptr = *((void **) item);
	unsigned int i;

	if (!ptr)
		return;

	*((void **)item) = NULL;
	for (i = 0, a = sd->allocmap; i < sd->allocmap_len; i++, a++) {
		if (a->ptr != ptr)
			continue;

		if (i != sd->allocmap_len - 1)
			a->ptr = sd->allocmap[sd->allocmap_len - 1].ptr;

		sd->allocmap_len--;
		return;
	}

	for (i = 0, ac = sd->alloc_custom; i < sd->alloc_custom_len; i++, ac++) {
		if (ac->ptr != ptr)
			continue;

		if (i != sd->alloc_custom_len - 1)
			memcpy(ac, &sd->alloc_custom[sd->alloc_custom_len - 1],
				sizeof(struct ucimap_alloc_custom));

		ac->om->free(ac->section, ac->om, ac->ptr);
		sd->alloc_custom_len--;
		return;
	}
}

int
ucimap_resize_list(struct ucimap_section_data *sd, struct ucimap_list **list, int items)
{
	struct ucimap_list *new;
	struct ucimap_alloc *a;
	unsigned int i;
	int offset = 0;
	int size = sizeof(struct ucimap_list) + items * sizeof(union ucimap_data);

	if (!*list) {
		new = calloc(1, size);
		if (!new)
			return -ENOMEM;

		ucimap_add_alloc(sd, new);
		goto set;
	}

	for (i = 0, a = sd->allocmap; i < sd->allocmap_len; i++, a++) {
		if (a->ptr != *list)
			continue;

		goto realloc;
	}
	return -ENOENT;

realloc:
	if (items > (*list)->size)
		offset = (items - (*list)->size) * sizeof(union ucimap_data);

	a->ptr = realloc(a->ptr, size);
	if (!a->ptr)
		return -ENOMEM;

	if (offset)
		memset((char *) a->ptr + offset, 0, size - offset);
	new = a->ptr;

set:
	new->size = items;
	*list = new;
	return 0;
}

static void
ucimap_add_fixup(struct ucimap_section_data *sd, union ucimap_data *data, struct uci_optmap *om, const char *str)
{
	struct ucimap_fixup *f, tmp;
	struct uci_map *map = sd->map;

	tmp.next = NULL;
	tmp.sm = om->data.sm;
	tmp.name = str;
	tmp.type = om->type;
	tmp.data = data;
	if (ucimap_handle_fixup(map, &tmp))
		return;

	f = malloc(sizeof(struct ucimap_fixup));
	if (!f)
		return;

	memcpy(f, &tmp, sizeof(tmp));
	f->next = NULL;
	*map->fixup_tail = f;
	map->fixup_tail = &f->next;
}

static void
ucimap_add_custom_alloc(struct ucimap_section_data *sd, struct uci_optmap *om, void *ptr)
{
	struct ucimap_alloc_custom *a = &sd->alloc_custom[sd->alloc_custom_len++];

	a->section = ucimap_section_ptr(sd);
	a->om = om;
	a->ptr = ptr;
}

static void
ucimap_add_value(union ucimap_data *data, struct uci_optmap *om, struct ucimap_section_data *sd, const char *str)
{
	union ucimap_data tdata = *data;
	char *eptr = NULL;
	long lval;
	char *s;
	int val;

	if (ucimap_is_list(om->type) && !ucimap_is_fixup(om->type)) {
		data = ucimap_list_append(data->list);
		if (!data)
			return;
	}

	switch(om->type & UCIMAP_SUBTYPE) {
	case UCIMAP_STRING:
		if ((om->data.s.maxlen > 0) &&
			(strlen(str) > (unsigned) om->data.s.maxlen))
			return;

		s = strdup(str);
		tdata.s = s;
		ucimap_add_alloc(sd, s);
		break;
	case UCIMAP_BOOL:
		if (!strcmp(str, "on"))
			val = true;
		else if (!strcmp(str, "1"))
			val = true;
		else if (!strcmp(str, "enabled"))
			val = true;
		else if (!strcmp(str, "off"))
			val = false;
		else if (!strcmp(str, "0"))
			val = false;
		else if (!strcmp(str, "disabled"))
			val = false;
		else
			return;

		tdata.b = val;
		break;
	case UCIMAP_INT:
		lval = strtol(str, &eptr, om->data.i.base);
		if (lval < INT_MIN || lval > INT_MAX)
			return;

		if (!eptr || *eptr == '\0')
			tdata.i = (int) lval;
		else
			return;
		break;
	case UCIMAP_SECTION:
		ucimap_add_fixup(sd, data, om, str);
		return;
	case UCIMAP_CUSTOM:
		break;
	}
	if (om->parse) {
		if (om->parse(ucimap_section_ptr(sd), om, data, str) < 0)
			return;
		if (ucimap_is_custom(om->type) && om->free) {
			if (tdata.ptr != data->ptr)
				ucimap_add_custom_alloc(sd, om, data->ptr);
		}
	}
	if (ucimap_is_custom(om->type))
		return;
	memcpy(data, &tdata, sizeof(union ucimap_data));
}


static void
ucimap_convert_list(union ucimap_data *data, struct uci_optmap *om, struct ucimap_section_data *sd, const char *str)
{
	char *s, *p;

	s = strdup(str);
	if (!s)
		return;

	ucimap_add_alloc(sd, s);

	do {
		while (isspace(*s))
			s++;

		if (!*s)
			break;

		p = s;
		while (*s && !isspace(*s))
			s++;

		if (isspace(*s)) {
			*s = 0;
			s++;
		}

		ucimap_add_value(data, om, sd, p);
	} while (*s);
}

static int
ucimap_parse_options(struct uci_map *map, struct uci_sectionmap *sm, struct ucimap_section_data *sd, struct uci_section *s)
{
	struct uci_element *e, *l;
	struct uci_option *o;
	union ucimap_data *data;

	uci_foreach_element(&s->options, e) {
		struct uci_optmap *om = NULL, *tmp;

		ucimap_foreach_option(sm, tmp) {
			if (strcmp(e->name, tmp->name) == 0) {
				om = tmp;
				break;
			}
		}
		if (!om)
			continue;

		data = ucimap_get_data(sd, om);
		o = uci_to_option(e);
		if ((o->type == UCI_TYPE_STRING) && ucimap_is_simple(om->type)) {
			ucimap_add_value(data, om, sd, o->v.string);
		} else if ((o->type == UCI_TYPE_LIST) && ucimap_is_list(om->type)) {
			uci_foreach_element(&o->v.list, l) {
				ucimap_add_value(data, om, sd, l->name);
			}
		} else if ((o->type == UCI_TYPE_STRING) && ucimap_is_list_auto(om->type)) {
			ucimap_convert_list(data, om, sd, o->v.string);
		}
	}

	return 0;
}

static void
ucimap_add_section_list(struct uci_map *map, struct ucimap_section_data *sd)
{
	sd->ref = map->sdata_tail;
	*sd->ref = sd;
	map->sdata_tail = &sd->next;
}

static int
ucimap_add_section(struct ucimap_section_data *sd)
{
	int r;
	struct uci_map *map = sd->map;

	sd->next = NULL;
	r = sd->sm->add(map, ucimap_section_ptr(sd));
	if (r < 0) {
		ucimap_free_section(map, sd);
		return r;
	} else
		ucimap_add_section_list(map, sd);

	return 0;
}

#ifdef UCI_DEBUG
static const char *ucimap_type_names[] = {
	[UCIMAP_STRING] = "string",
	[UCIMAP_INT] = "integer",
	[UCIMAP_BOOL] = "boolean",
	[UCIMAP_SECTION] = "section",
	[UCIMAP_LIST] = "list",
};

static const char *
ucimap_get_type_name(int type)
{
	static char buf[32];
	const char *name;

	if (ucimap_is_list(type))
		return ucimap_type_names[UCIMAP_LIST];

	name = ucimap_type_names[type & UCIMAP_SUBTYPE];
	if (!name) {
		sprintf(buf, "Unknown (%d)", type & UCIMAP_SUBTYPE);
		name = buf;
	}

	return name;
}
#endif

static bool
ucimap_check_optmap_type(struct uci_sectionmap *sm, struct uci_optmap *om)
{
	int type;

	if (unlikely(sm->type_name != om->type_name) &&
	    unlikely(strcmp(sm->type_name, om->type_name) != 0)) {
		DPRINTF("Option '%s' of section type '%s' refereces unknown "
			"section type '%s', should be '%s'.\n",
			om->name, sm->type, om->type_name, sm->type_name);
		return false;
	}

	if (om->detected_type < 0)
		return true;

	if (ucimap_is_custom(om->type))
		return true;

	if (ucimap_is_list(om->type) !=
	    ucimap_is_list(om->detected_type))
		goto failed;

	if (ucimap_is_list(om->type))
		return true;

	type = om->type & UCIMAP_SUBTYPE;
	switch(type) {
	case UCIMAP_STRING:
	case UCIMAP_INT:
	case UCIMAP_BOOL:
		if (type != om->detected_type)
			goto failed;
		break;
	case UCIMAP_SECTION:
		goto failed;
	default:
		break;
	}
	return true;

failed:
	DPRINTF("Invalid type in option '%s' of section type '%s', "
		"declared type is %s, detected type is %s\n",
		om->name, sm->type,
		ucimap_get_type_name(om->type),
		ucimap_get_type_name(om->detected_type));
	return false;
}

static void
ucimap_count_alloc(struct uci_optmap *om, int *n_alloc, int *n_custom)
{
	if (ucimap_is_alloc(om->type))
		(*n_alloc)++;
	else if (ucimap_is_custom(om->type) && om->free)
		(*n_custom)++;
}

int
ucimap_parse_section(struct uci_map *map, struct uci_sectionmap *sm, struct ucimap_section_data *sd, struct uci_section *s)
{
	struct uci_optmap *om;
	char *section_name;
	void *section;
	int n_alloc = 2;
	int n_alloc_custom = 0;
	int err;

	sd->map = map;
	sd->sm = sm;

	ucimap_foreach_option(sm, om) {
		if (!ucimap_check_optmap_type(sm, om))
			continue;

		if (ucimap_is_list(om->type)) {
			union ucimap_data *data;
			struct uci_element *e;
			int n_elements = 0;
			int n_elements_alloc = 0;
			int n_elements_custom = 0;
			int size;

			data = ucimap_get_data(sd, om);
			uci_foreach_element(&s->options, e) {
				struct uci_option *o = uci_to_option(e);
				struct uci_element *tmp;

				if (strcmp(e->name, om->name) != 0)
					continue;

				if (o->type == UCI_TYPE_LIST) {
					uci_foreach_element(&o->v.list, tmp) {
						ucimap_count_alloc(om, &n_elements_alloc, &n_elements_custom);
						n_elements++;
					}
				} else if ((o->type == UCI_TYPE_STRING) &&
				           ucimap_is_list_auto(om->type)) {
					const char *data = o->v.string;
					do {
						while (isspace(*data))
							data++;

						if (!*data)
							break;

						n_elements++;
						ucimap_count_alloc(om, &n_elements_alloc, &n_elements_custom);

						while (*data && !isspace(*data))
							data++;
					} while (*data);

					/* for the duplicated data string */
					if (n_elements)
						n_alloc++;
				}
				break;
			}
			/* add one more for the ucimap_list */
			n_alloc += n_elements_alloc + 1;
			n_alloc_custom += n_elements_custom;
			size = sizeof(struct ucimap_list) +
				n_elements * sizeof(union ucimap_data);

			data->list = calloc(1, size);
			if (!data->list)
				goto error_mem;

			data->list->size = n_elements;
		} else {
			ucimap_count_alloc(om, &n_alloc, &n_alloc_custom);
		}
	}

	sd->allocmap = calloc(n_alloc, sizeof(struct ucimap_alloc));
	if (!sd->allocmap)
		goto error_mem;

	if (n_alloc_custom > 0) {
		sd->alloc_custom = calloc(n_alloc_custom, sizeof(struct ucimap_alloc_custom));
		if (!sd->alloc_custom)
			goto error_mem;
	}

	section_name = strdup(s->e.name);
	if (!section_name)
		goto error_mem;

	sd->section_name = section_name;

	sd->cmap = calloc(1, BITFIELD_SIZE(sm->n_options));
	if (!sd->cmap)
		goto error_mem;

	ucimap_add_alloc(sd, (void *)section_name);
	ucimap_add_alloc(sd, (void *)sd->cmap);
	ucimap_foreach_option(sm, om) {
		if (!ucimap_is_list(om->type))
			continue;

		ucimap_add_alloc(sd, ucimap_get_data(sd, om)->list);
	}

	section = ucimap_section_ptr(sd);
	err = sm->init(map, section, s);
	if (err)
		goto error;

	if (map->parsed) {
		err = ucimap_add_section(sd);
		if (err)
			return err;
	} else {
		ucimap_add_section_list(map, sd);
	}

	err = ucimap_parse_options(map, sm, sd, s);
	if (err)
		goto error;

	return 0;

error_mem:
	free(sd->alloc_custom);
	free(sd->allocmap);
	free(sd);
	return UCI_ERR_MEM;

error:
	ucimap_free_section(map, sd);
	return err;
}

static int
ucimap_fill_ptr(struct uci_ptr *ptr, struct uci_section *s, const char *option)
{
	struct uci_package *p = s->package;

	memset(ptr, 0, sizeof(struct uci_ptr));

	ptr->package = p->e.name;
	ptr->p = p;

	ptr->section = s->e.name;
	ptr->s = s;

	ptr->option = option;
	return uci_lookup_ptr(p->ctx, ptr, NULL, false);
}

void
ucimap_set_changed(struct ucimap_section_data *sd, void *field)
{
	void *section = ucimap_section_ptr(sd);
	struct uci_sectionmap *sm = sd->sm;
	struct uci_optmap *om;
	unsigned int ofs = (char *)field - (char *)section;
	int i = 0;

	ucimap_foreach_option(sm, om) {
		if (om->offset == ofs) {
			SET_BIT(sd->cmap, i);
			break;
		}
		i++;
	}
}

static char *
ucimap_data_to_string(struct ucimap_section_data *sd, struct uci_optmap *om, union ucimap_data *data)
{
	static char buf[32];
	char *str = NULL;

	switch(om->type & UCIMAP_SUBTYPE) {
	case UCIMAP_STRING:
		str = data->s;
		break;
	case UCIMAP_INT:
		sprintf(buf, "%d", data->i);
		str = buf;
		break;
	case UCIMAP_BOOL:
		sprintf(buf, "%d", !!data->b);
		str = buf;
		break;
	case UCIMAP_SECTION:
		if (data->ptr)
			str = (char *) ucimap_ptr_section(om->data.sm, data->ptr)->section_name;
		else
			str = "";
		break;
	case UCIMAP_CUSTOM:
		break;
	default:
		return NULL;
	}

	if (om->format) {
		if (om->format(ucimap_section_ptr(sd), om, data, &str) < 0)
			return NULL;

		if (!str)
			str = "";
	}
	return str;
}

int
ucimap_store_section(struct uci_map *map, struct uci_package *p, struct ucimap_section_data *sd)
{
	struct uci_sectionmap *sm = sd->sm;
	struct uci_section *s = NULL;
	struct uci_optmap *om;
	struct uci_element *e;
	struct uci_ptr ptr;
	int i = 0;
	int ret;

	uci_foreach_element(&p->sections, e) {
		if (!strcmp(e->name, sd->section_name)) {
			s = uci_to_section(e);
			break;
		}
	}
	if (!s)
		return UCI_ERR_NOTFOUND;

	ucimap_foreach_option(sm, om) {
		union ucimap_data *data;

		i++;
		data = ucimap_get_data(sd, om);
		if (!TEST_BIT(sd->cmap, i - 1))
			continue;

		ucimap_fill_ptr(&ptr, s, om->name);
		if (ucimap_is_list(om->type)) {
			struct ucimap_list *list = data->list;
			bool first = true;
			int j;

			for (j = 0; j < list->n_items; j++) {
				ptr.value = ucimap_data_to_string(sd, om, &list->item[j]);
				if (!ptr.value)
					continue;

				if (first) {
					ret = uci_set(s->package->ctx, &ptr);
					first = false;
				} else {
					ret = uci_add_list(s->package->ctx, &ptr);
				}
				if (ret)
					return ret;
			}
		} else {
			ptr.value = ucimap_data_to_string(sd, om, data);
			if (!ptr.value)
				continue;

			ret = uci_set(s->package->ctx, &ptr);
			if (ret)
				return ret;
		}

		CLR_BIT(sd->cmap, i - 1);
	}

	return 0;
}

void
ucimap_parse(struct uci_map *map, struct uci_package *pkg)
{
	struct uci_element *e;
	struct ucimap_section_data *sd, **sd_tail;
	struct ucimap_fixup *f;
	unsigned int i;

	sd_tail = map->sdata_tail;
	map->parsed = false;
	map->sdata_tail = &map->pending;
	uci_foreach_element(&pkg->sections, e) {
		struct uci_section *s = uci_to_section(e);

		for (i = 0; i < map->n_sections; i++) {
			struct uci_sectionmap *sm = map->sections[i];
			struct ucimap_section_data *sd;

			if (strcmp(s->type, map->sections[i]->type) != 0)
				continue;

			if (sm->alloc) {
				sd = sm->alloc(map, sm, s);
				if (!sd)
					continue;
				memset(sd, 0, sizeof(struct ucimap_section_data));
			} else {
				sd = calloc(1, sm->alloc_len);
				if (!sd)
					continue;
				sd = ucimap_ptr_section(sm, sd);
			}

			ucimap_parse_section(map, sm, sd, s);
		}
	}
	if (!map->parsed) {
		map->parsed = true;
		map->sdata_tail = sd_tail;
	}

	f = map->fixup;
	while (f) {
		struct ucimap_fixup *next = f->next;
		ucimap_handle_fixup(map, f);
		free(f);
		f = next;
	}
	map->fixup_tail = &map->fixup;
	map->fixup = NULL;

	sd = map->pending;
	while (sd) {
		struct ucimap_section_data *next = sd->next;
		ucimap_add_section(sd);
		sd = next;
	}
	map->pending = NULL;
}
