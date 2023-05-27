/*
 * blob.c - uci <-> blobmsg conversion layer
 * Copyright (C) 2012-2013 Felix Fietkau <nbd@openwrt.org>
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
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <libubox/blobmsg.h>
#include "uci.h"
#include "uci_blob.h"

static bool
uci_attr_to_blob(struct blob_buf *b, const char *str,
		 const char *name, enum blobmsg_type type)
{
	char *err;
	int intval;
	long long llval;

	switch (type) {
	case BLOBMSG_TYPE_STRING:
		blobmsg_add_string(b, name, str);
		break;
	case BLOBMSG_TYPE_BOOL:
		if (!strcmp(str, "true") || !strcmp(str, "1"))
			intval = 1;
		else if (!strcmp(str, "false") || !strcmp(str, "0"))
			intval = 0;
		else
			return false;

		blobmsg_add_u8(b, name, intval);
		break;
	case BLOBMSG_TYPE_INT32:
		intval = strtol(str, &err, 0);
		if (*err)
			return false;

		blobmsg_add_u32(b, name, intval);
		break;
	case BLOBMSG_TYPE_INT64:
		llval = strtoll(str, &err, 0);
		if (*err)
			return false;

		blobmsg_add_u64(b, name, llval);
		break;
	default:
		return false;
	}
	return true;
}

static void
uci_array_to_blob(struct blob_buf *b, struct uci_option *o,
		  enum blobmsg_type type)
{
	struct uci_element *e;
	char *str, *next, *word;

	if (o->type == UCI_TYPE_LIST) {
		uci_foreach_element(&o->v.list, e) {
			uci_attr_to_blob(b, e->name, NULL, type);
		}
		return;
	}

	str = strdup(o->v.string);
	next = str;

	while ((word = strsep(&next, " \t")) != NULL) {
		if (!*word)
			continue;

		uci_attr_to_blob(b, word, NULL, type);
	}

	free(str);
}

static int
__uci_element_to_blob(struct blob_buf *b, struct uci_element *e,
		      const struct uci_blob_param_list *p)
{
	const struct blobmsg_policy *attr = NULL;
	struct uci_option *o = uci_to_option(e);
	unsigned int types = 0;
	void *array;
	int i, ret = 0;

	for (i = 0; i < p->n_params; i++) {
		attr = &p->params[i];

		if (strcmp(attr->name, e->name) != 0)
			continue;

		if (attr->type > BLOBMSG_TYPE_LAST)
			continue;

		if (types & (1 << attr->type))
			continue;

		types |= 1 << attr->type;

		if (attr->type == BLOBMSG_TYPE_ARRAY) {
			int element_type = 0;

			if (p->info)
				element_type = p->info[i].type;

			if (!element_type)
				element_type = BLOBMSG_TYPE_STRING;

			array = blobmsg_open_array(b, attr->name);
			uci_array_to_blob(b, o, element_type);
			blobmsg_close_array(b, array);
			ret++;
			continue;
		}

		if (o->type == UCI_TYPE_LIST)
			continue;

		ret += uci_attr_to_blob(b, o->v.string, attr->name, attr->type);
	}
	return ret;
}

static int
__uci_to_blob(struct blob_buf *b, struct uci_section *s,
	      const struct uci_blob_param_list *p)
{
	struct uci_element *e;
	int ret = 0;

	uci_foreach_element(&s->options, e)
		ret += __uci_element_to_blob(b, e, p);

	return ret;
}

int
uci_to_blob(struct blob_buf *b, struct uci_section *s,
	    const struct uci_blob_param_list *p)
{
	int ret = 0;
	int i;

	ret += __uci_to_blob(b, s, p);
	for (i = 0; i < p->n_next; i++)
		ret += uci_to_blob(b, s, p->next[i]);

	return ret;
}

bool
uci_blob_diff(struct blob_attr **tb1, struct blob_attr **tb2,
	      const struct uci_blob_param_list *config, unsigned long *diff)
{
	bool ret = false;
	int i;

	for (i = 0; i < config->n_params; i++) {
		if (!tb1[i] && !tb2[i])
			continue;

		if (!!tb1[i] != !!tb2[i])
			goto mark;

		if (blob_len(tb1[i]) != blob_len(tb2[i]))
			goto mark;

		if (memcmp(tb1[i], tb2[i], blob_raw_len(tb1[i])) != 0)
			goto mark;

		continue;

mark:
		ret = true;
		if (diff)
			uci_bitfield_set(diff, i);
		else
			return ret;
	}

	return ret;
}


static bool
__uci_blob_check_equal(struct blob_attr *c1, struct blob_attr *c2,
		       const struct uci_blob_param_list *config)
{
	struct blob_attr **tb1, **tb2;

	if (!!c1 ^ !!c2)
		return false;

	if (!c1 && !c2)
		return true;

	tb1 = alloca(config->n_params * sizeof(struct blob_attr *));
	blobmsg_parse(config->params, config->n_params, tb1,
		blob_data(c1), blob_len(c1));

	tb2 = alloca(config->n_params * sizeof(struct blob_attr *));
	blobmsg_parse(config->params, config->n_params, tb2,
		blob_data(c2), blob_len(c2));

	return !uci_blob_diff(tb1, tb2, config, NULL);
}

bool
uci_blob_check_equal(struct blob_attr *c1, struct blob_attr *c2,
		     const struct uci_blob_param_list *config)
{
	int i;

	if (!__uci_blob_check_equal(c1, c2, config))
		return false;

	for (i = 0; i < config->n_next; i++) {
		if (!__uci_blob_check_equal(c1, c2, config->next[i]))
			return false;
	}

	return true;
}
