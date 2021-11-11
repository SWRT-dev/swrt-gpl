/*
 * ucimap-example - sample code for the ucimap library
 * Copyright (C) 2008-2009 Felix Fietkau <nbd@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ucimap.h>
#include "list.h"

struct list_head ifs;

struct uci_network {
	struct ucimap_section_data map;
	struct list_head list;
	struct list_head alias;

	const char *name;
	const char *proto;
	const char *ifname;
	unsigned char *ipaddr;
	int test;
	bool enabled;
	struct ucimap_list *aliases;
};

struct uci_alias {
	struct ucimap_section_data map;
	struct list_head list;

	const char *name;
	struct uci_network *interface;
};

static int
network_parse_ip(void *section, struct uci_optmap *om, union ucimap_data *data, const char *str)
{
	unsigned char *target;
	int tmp[4];
	int i;

	if (sscanf(str, "%d.%d.%d.%d", &tmp[0], &tmp[1], &tmp[2], &tmp[3]) != 4)
		return -1;

	target = malloc(4);
	if (!target)
		return -1;

	data->ptr = target;
	for (i = 0; i < 4; i++)
		target[i] = (char) tmp[i];

	return 0;
}

static int
network_format_ip(void *section, struct uci_optmap *om, union ucimap_data *data, char **str)
{
	static char buf[16];
	unsigned char *ip = (unsigned char *) data->ptr;

	if (ip) {
		sprintf(buf, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
		*str = buf;
	} else {
		*str = NULL;
	}

	return 0;
}

static void
network_free_ip(void *section, struct uci_optmap *om, void *ptr)
{
	free(ptr);
}

static int
network_init_interface(struct uci_map *map, void *section, struct uci_section *s)
{
	struct uci_network *net = section;

	INIT_LIST_HEAD(&net->list);
	INIT_LIST_HEAD(&net->alias);
	net->name = s->e.name;
	net->test = -1;
	return 0;
}

static int
network_init_alias(struct uci_map *map, void *section, struct uci_section *s)
{
	struct uci_alias *alias = section;

	INIT_LIST_HEAD(&alias->list);
	alias->name = s->e.name;
	return 0;
}

static int
network_add_interface(struct uci_map *map, void *section)
{
	struct uci_network *net = section;

	list_add_tail(&net->list, &ifs);

	return 0;
}

static int
network_add_alias(struct uci_map *map, void *section)
{
	struct uci_alias *a = section;

	if (a->interface)
		list_add_tail(&a->list, &a->interface->alias);

	return 0;
}

static struct ucimap_section_data *
network_allocate(struct uci_map *map, struct uci_sectionmap *sm, struct uci_section *s)
{
	struct uci_network *p = malloc(sizeof(struct uci_network));
	memset(p, 0, sizeof(struct uci_network));
	return &p->map;
}

struct my_optmap {
	struct uci_optmap map;
	int test;
};

static struct uci_sectionmap network_interface;
static struct uci_sectionmap network_alias;

static struct my_optmap network_interface_options[] = {
	{
		.map = {
			UCIMAP_OPTION(struct uci_network, proto),
			.type = UCIMAP_STRING,
			.name = "proto",
			.data.s.maxlen = 32,
		}
	},
	{
		.map = {
			UCIMAP_OPTION(struct uci_network, ifname),
			.type = UCIMAP_STRING,
			.name = "ifname"
		}
	},
	{
		.map = {
			UCIMAP_OPTION(struct uci_network, ipaddr),
			.type = UCIMAP_CUSTOM,
			.name = "ipaddr",
			.parse = network_parse_ip,
			.format = network_format_ip,
			.free = network_free_ip,
		}
	},
	{
		.map = {
			UCIMAP_OPTION(struct uci_network, enabled),
			.type = UCIMAP_BOOL,
			.name = "enabled",
		}
	},
	{
		.map = {
			UCIMAP_OPTION(struct uci_network, test),
			.type = UCIMAP_INT,
			.name = "test"
		}
	},
	{
		.map = {
			UCIMAP_OPTION(struct uci_network, aliases),
			.type = UCIMAP_LIST | UCIMAP_SECTION | UCIMAP_LIST_AUTO,
			.data.sm = &network_alias
		}
	}
};

static struct uci_sectionmap network_interface = {
	UCIMAP_SECTION(struct uci_network, map),
	.type = "interface",
	.alloc = network_allocate,
	.init = network_init_interface,
	.add = network_add_interface,
	.options = &network_interface_options[0].map,
	.n_options = ARRAY_SIZE(network_interface_options),
	.options_size = sizeof(struct my_optmap)
};

static struct uci_optmap network_alias_options[] = {
	{
		UCIMAP_OPTION(struct uci_alias, interface),
		.type = UCIMAP_SECTION,
		.data.sm = &network_interface
	}
};

static struct uci_sectionmap network_alias = {
	UCIMAP_SECTION(struct uci_alias, map),
	.type = "alias",
	.options = network_alias_options,
	.init = network_init_alias,
	.add = network_add_alias,
	.n_options = ARRAY_SIZE(network_alias_options),
};

static struct uci_sectionmap *network_smap[] = {
	&network_interface,
	&network_alias,
};

static struct uci_map network_map = {
	.sections = network_smap,
	.n_sections = ARRAY_SIZE(network_smap),
};


int main(int argc, char **argv)
{
	struct uci_context *ctx;
	struct uci_package *pkg;
	struct list_head *p;
	struct uci_network *net;
	struct uci_alias *alias;
	bool set = false;
	int i;

	INIT_LIST_HEAD(&ifs);
	ctx = uci_alloc_context();
	ucimap_init(&network_map);

	if ((argc >= 2) && !strcmp(argv[1], "-s")) {
		uci_set_savedir(ctx, "./test/save");
		set = true;
	}

	uci_set_confdir(ctx, "./test/config");
	uci_load(ctx, "network", &pkg);

	ucimap_parse(&network_map, pkg);

	list_for_each(p, &ifs) {
		const unsigned char *ipaddr;
		int n_aliases = 0;

		net = list_entry(p, struct uci_network, list);
		ipaddr = net->ipaddr;
		if (!ipaddr)
			ipaddr = (const unsigned char *) "\x00\x00\x00\x00";

		printf("New network section '%s'\n"
			"	type: %s\n"
			"	ifname: %s\n"
			"	ipaddr: %d.%d.%d.%d\n"
			"	test: %d\n"
			"	enabled: %s\n",
			net->name,
			net->proto,
			net->ifname,
			ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3],
			net->test,
			(net->enabled ? "on" : "off"));

		if (net->aliases->n_items > 0) {
			printf("Configured aliases:");
			for (i = 0; i < net->aliases->n_items; i++) {
				alias = net->aliases->item[i].ptr;
				printf(" %s", alias->name);
			}
			printf("\n");
		}
		list_for_each_entry(alias, &net->alias, list) {
			n_aliases++;
			for (i = 0; i < net->aliases->n_items; i++) {
				if (alias == net->aliases->item[i].ptr)
					goto next_alias;
			}
			printf("New alias: %s\n", alias->name);
next_alias:
			continue;
		}
		if (set && !strcmp(net->name, "lan")) {
			ucimap_free_item(&net->map, &net->ipaddr);
			ucimap_set_changed(&net->map, &net->ipaddr);
			ucimap_resize_list(&net->map, &net->aliases, n_aliases);
			net->aliases->n_items = 0;
			list_for_each_entry(alias, &net->alias, list) {
				net->aliases->item[net->aliases->n_items++].ptr = alias;
			}
			ucimap_set_changed(&net->map, &net->aliases);
			ucimap_store_section(&network_map, pkg, &net->map);
			uci_save(ctx, pkg);
		}
	}


	ucimap_cleanup(&network_map);
	uci_free_context(ctx);

	return 0;
}
