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

#include <string.h>
#include <stdint.h>

#include "uci.h"

void uci_parse_section(struct uci_section *s, const struct uci_parse_option *opts,
		       int n_opts, struct uci_option **tb)
{
	struct uci_element *e;

	memset(tb, 0, n_opts * sizeof(*tb));

	uci_foreach_element(&s->options, e) {
		struct uci_option *o = uci_to_option(e);
		int i;

		for (i = 0; i < n_opts; i++) {
			if (tb[i])
				continue;

			if (strcmp(opts[i].name, o->e.name) != 0)
				continue;

			if (opts[i].type != o->type)
				continue;

			/* match found */
			tb[i] = o;
			break;
		}
	}
}

//-----------------------------------------------------------------------------
// MurmurHashNeutral2, by Austin Appleby

// Same as MurmurHash2, but endian- and alignment-neutral.
static uint32_t hash_murmur2(uint32_t h, const void * key, int len)
{
	const unsigned char * data = key;
	const uint32_t m = 0x5bd1e995;
	const int r = 24;

	while(len >= 4)
	{
		unsigned int k;

		k  = data[0];
		k |= data[1] << 8;
		k |= data[2] << 16;
		k |= data[3] << 24;

		k *= m;
		k ^= k >> r;
		k *= m;

		h *= m;
		h ^= k;

		data += 4;
		len -= 4;
	}

	switch(len)
	{
	case 3: h ^= data[2] << 16;
		/* fall through */
	case 2: h ^= data[1] << 8;
		/* fall through */
	case 1: h ^= data[0];
	        h *= m;
	}

	h ^= h >> 13;
	h *= m;
	h ^= h >> 15;

	return h;
}

static uint32_t uci_hash_list(uint32_t h, const struct uci_list *list)
{
	const struct uci_element *e;

	uci_foreach_element(list, e) {
		h = hash_murmur2(h, e->name, strlen(e->name) + 1);
	}
	return h;
}

uint32_t uci_hash_options(struct uci_option **tb, int n_opts)
{
	uint32_t h = 0xdeadc0de;
	int i;

	for (i = 0; i < n_opts; i++) {
		const struct uci_option *o = tb[i];

		if (!tb[i])
			continue;

		h = hash_murmur2(h, o->e.name, strlen(o->e.name) + 1);
		h = hash_murmur2(h, &o->type, sizeof(o->type));

		switch (tb[i]->type) {
		case UCI_TYPE_STRING:
			h = hash_murmur2(h, o->v.string, strlen(o->v.string) + 1);
			break;
		case UCI_TYPE_LIST:
			h = uci_hash_list(h, &o->v.list);
			break;
		}
	}

	return h;
}


