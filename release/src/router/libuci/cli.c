/*
 * cli - Command Line Interface for the Unified Configuration Interface
 * Copyright (C) 2008 Felix Fietkau <nbd@openwrt.org>
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
#include "uci.h"

#define MAX_ARGS	4 /* max command line arguments for batch mode */

static const char *delimiter = " ";
static const char *appname;
static enum {
	CLI_FLAG_MERGE =    (1 << 0),
	CLI_FLAG_QUIET =    (1 << 1),
	CLI_FLAG_NOCOMMIT = (1 << 2),
	CLI_FLAG_BATCH =    (1 << 3),
	CLI_FLAG_SHOW_EXT = (1 << 4),
	CLI_FLAG_NOPLUGINS= (1 << 5),
} flags;

static FILE *input;

static struct uci_context *ctx;
enum {
	/* section cmds */
	CMD_GET,
	CMD_SET,
	CMD_ADD_LIST,
	CMD_DEL,
	CMD_RENAME,
	CMD_REVERT,
	CMD_REORDER,
	/* package cmds */
	CMD_SHOW,
	CMD_CHANGES,
	CMD_EXPORT,
	CMD_COMMIT,
	/* other cmds */
	CMD_ADD,
	CMD_IMPORT,
	CMD_HELP,
};

struct uci_type_list {
	unsigned int idx;
	const char *name;
	struct uci_type_list *next;
};

static struct uci_type_list *type_list = NULL;
static char *typestr = NULL;
static const char *cur_section_ref = NULL;

static int uci_cmd(int argc, char **argv);

static void
uci_reset_typelist(void)
{
	struct uci_type_list *type;
	while (type_list != NULL) {
			type = type_list;
			type_list = type_list->next;
			free(type);
	}
	if (typestr) {
		free(typestr);
		typestr = NULL;
	}
	cur_section_ref = NULL;
}

static char *
uci_lookup_section_ref(struct uci_section *s)
{
	struct uci_type_list *ti = type_list;
	int maxlen;

	if (!s->anonymous || !(flags & CLI_FLAG_SHOW_EXT))
		return s->e.name;

	/* look up in section type list */
	while (ti) {
		if (strcmp(ti->name, s->type) == 0)
			break;
		ti = ti->next;
	}
	if (!ti) {
		ti = malloc(sizeof(struct uci_type_list));
		memset(ti, 0, sizeof(struct uci_type_list));
		ti->next = type_list;
		type_list = ti;
		ti->name = s->type;
	}

	maxlen = strlen(s->type) + 1 + 2 + 10;
	if (!typestr) {
		typestr = malloc(maxlen);
	} else {
		typestr = realloc(typestr, maxlen);
	}
	sprintf(typestr, "@%s[%d]", ti->name, ti->idx);
	ti->idx++;
	return typestr;
}

static void uci_usage(void)
{
	fprintf(stderr,
		"Usage: %s [<options>] <command> [<arguments>]\n\n"
		"Commands:\n"
		"\tbatch\n"
		"\texport     [<config>]\n"
		"\timport     [<config>]\n"
		"\tchanges    [<config>]\n"
		"\tcommit     [<config>]\n"
		"\tadd        <config> <section-type>\n"
		"\tadd_list   <config>.<section>.<option>=<string>\n"
		"\tshow       [<config>[.<section>[.<option>]]]\n"
		"\tget        <config>.<section>[.<option>]\n"
		"\tset        <config>.<section>[.<option>]=<value>\n"
		"\tdelete     <config>[.<section[.<option>]]\n"
		"\trename     <config>.<section>[.<option>]=<name>\n"
		"\trevert     <config>[.<section>[.<option>]]\n"
		"\treorder    <config>.<section>=<position>\n"
		"\n"
		"Options:\n"
		"\t-c <path>  set the search path for config files (default: /etc/config)\n"
		"\t-d <str>   set the delimiter for list values in uci show\n"
		"\t-f <file>  use <file> as input instead of stdin\n"
		"\t-L         do not load any plugins\n"
		"\t-m         when importing, merge data into an existing package\n"
		"\t-n         name unnamed sections on export (default)\n"
		"\t-N         don't name unnamed sections\n"
		"\t-p <path>  add a search path for config change files\n"
		"\t-P <path>  add a search path for config change files and use as default\n"
		"\t-q         quiet mode (don't print error messages)\n"
		"\t-s         force strict mode (stop on parser errors, default)\n"
		"\t-S         disable strict mode\n"
		"\t-X         do not use extended syntax on 'show'\n"
		"\n",
		appname
	);
}

static void cli_perror(void)
{
	if (flags & CLI_FLAG_QUIET)
		return;

	uci_perror(ctx, appname);
}

static void uci_show_value(struct uci_option *o)
{
	struct uci_element *e;
	bool sep = false;

	switch(o->type) {
	case UCI_TYPE_STRING:
		printf("%s\n", o->v.string);
		break;
	case UCI_TYPE_LIST:
		uci_foreach_element(&o->v.list, e) {
			printf("%s%s", (sep ? delimiter : ""), e->name);
			sep = true;
		}
		printf("\n");
		break;
	default:
		printf("<unknown>\n");
		break;
	}
}

static void uci_show_option(struct uci_option *o)
{
	printf("%s.%s.%s=",
		o->section->package->e.name,
		(cur_section_ref ? cur_section_ref : o->section->e.name),
		o->e.name);
	uci_show_value(o);
}

static void uci_show_section(struct uci_section *s)
{
	struct uci_element *e;
	const char *cname;
	const char *sname;

	cname = s->package->e.name;
	sname = (cur_section_ref ? cur_section_ref : s->e.name);
	printf("%s.%s=%s\n", cname, sname, s->type);
	uci_foreach_element(&s->options, e) {
		uci_show_option(uci_to_option(e));
	}
}

static void uci_show_package(struct uci_package *p)
{
	struct uci_element *e;

	uci_reset_typelist();
	uci_foreach_element( &p->sections, e) {
		struct uci_section *s = uci_to_section(e);
		cur_section_ref = uci_lookup_section_ref(s);
		uci_show_section(s);
	}
	uci_reset_typelist();
}

static void uci_show_changes(struct uci_package *p)
{
	struct uci_element *e;

	uci_foreach_element(&p->saved_delta, e) {
		struct uci_delta *h = uci_to_delta(e);
		char *prefix = "";
		char *op = "=";

		switch(h->cmd) {
		case UCI_CMD_REMOVE:
			prefix = "-";
			break;
		case UCI_CMD_LIST_ADD:
			op = "+=";
			break;
		default:
			break;
		}
		printf("%s%s.%s", prefix, p->e.name, h->section);
		if (e->name)
			printf(".%s", e->name);
		if (h->cmd != UCI_CMD_REMOVE)
			printf("%s%s", op, h->value);
		printf("\n");
	}
}

static int package_cmd(int cmd, char *tuple)
{
	struct uci_element *e = NULL;
	struct uci_ptr ptr;

	if (uci_lookup_ptr(ctx, &ptr, tuple, true) != UCI_OK) {
		cli_perror();
		return 1;
	}

	e = ptr.last;
	switch(cmd) {
	case CMD_CHANGES:
		uci_show_changes(ptr.p);
		break;
	case CMD_COMMIT:
		if (flags & CLI_FLAG_NOCOMMIT)
			return 0;
		if (uci_commit(ctx, &ptr.p, false) != UCI_OK)
			cli_perror();
		break;
	case CMD_EXPORT:
		uci_export(ctx, stdout, ptr.p, true);
		break;
	case CMD_SHOW:
		if (!(ptr.flags & UCI_LOOKUP_COMPLETE)) {
			ctx->err = UCI_ERR_NOTFOUND;
			cli_perror();
			return 1;
		}
		switch(e->type) {
			case UCI_TYPE_PACKAGE:
				uci_show_package(ptr.p);
				break;
			case UCI_TYPE_SECTION:
				uci_show_section(ptr.s);
				break;
			case UCI_TYPE_OPTION:
				uci_show_option(ptr.o);
				break;
			default:
				/* should not happen */
				return 1;
		}
		break;
	}

	uci_unload(ctx, ptr.p);
	return 0;
}

static int uci_do_import(int argc, char **argv)
{
	struct uci_package *package = NULL;
	char *name = NULL;
	int ret = UCI_OK;
	bool merge = false;

	if (argc > 2)
		return 255;

	if (argc == 2)
		name = argv[1];
	else if (flags & CLI_FLAG_MERGE)
		/* need a package to merge */
		return 255;

	if (flags & CLI_FLAG_MERGE) {
		if (uci_load(ctx, name, &package) != UCI_OK)
			package = NULL;
		else
			merge = true;
	}
	ret = uci_import(ctx, input, name, &package, (name != NULL));
	if (ret == UCI_OK) {
		if (merge) {
			ret = uci_save(ctx, package);
		} else {
			struct uci_element *e;
			/* loop through all config sections and overwrite existing data */
			uci_foreach_element(&ctx->root, e) {
				struct uci_package *p = uci_to_package(e);
				ret = uci_commit(ctx, &p, true);
			}
		}
	}

	if (ret != UCI_OK) {
		cli_perror();
		return 1;
	}

	return 0;
}

static int uci_do_package_cmd(int cmd, int argc, char **argv)
{
	char **configs = NULL;
	char **p;

	if (argc > 2)
		return 255;

	if (argc == 2)
		return package_cmd(cmd, argv[1]);

	if ((uci_list_configs(ctx, &configs) != UCI_OK) || !configs) {
		cli_perror();
		return 1;
	}

	for (p = configs; *p; p++) {
		package_cmd(cmd, *p);
	}

	return 0;
}

static int uci_do_add(int argc, char **argv)
{
	struct uci_package *p = NULL;
	struct uci_section *s = NULL;
	int ret;

	if (argc != 3)
		return 255;

	ret = uci_load(ctx, argv[1], &p);
	if (ret != UCI_OK)
		goto done;

	ret = uci_add_section(ctx, p, argv[2], &s);
	if (ret != UCI_OK)
		goto done;

	ret = uci_save(ctx, p);

done:
	if (ret != UCI_OK)
		cli_perror();
	else if (s)
		fprintf(stdout, "%s\n", s->e.name);

	return ret;
}

static int uci_do_section_cmd(int cmd, int argc, char **argv)
{
	struct uci_element *e;
	struct uci_ptr ptr;
	int ret = UCI_OK;

	if (argc != 2)
		return 255;

	if (uci_lookup_ptr(ctx, &ptr, argv[1], true) != UCI_OK) {
		cli_perror();
		return 1;
	}

	if (ptr.value && (cmd != CMD_SET) && (cmd != CMD_ADD_LIST) && (cmd != CMD_RENAME) && (cmd != CMD_REORDER))
		return 1;

	e = ptr.last;
	switch(cmd) {
	case CMD_GET:
		if (!(ptr.flags & UCI_LOOKUP_COMPLETE)) {
			ctx->err = UCI_ERR_NOTFOUND;
			cli_perror();
			return 1;
		}
		switch(e->type) {
		case UCI_TYPE_SECTION:
			printf("%s\n", ptr.s->type);
			break;
		case UCI_TYPE_OPTION:
			uci_show_value(ptr.o);
			break;
		default:
			break;
		}
		/* throw the value to stdout */
		break;
	case CMD_RENAME:
		ret = uci_rename(ctx, &ptr);
		break;
	case CMD_REVERT:
		ret = uci_revert(ctx, &ptr);
		break;
	case CMD_SET:
		ret = uci_set(ctx, &ptr);
		break;
	case CMD_ADD_LIST:
		ret = uci_add_list(ctx, &ptr);
		break;
	case CMD_REORDER:
		if (!ptr.s) {
			ctx->err = UCI_ERR_NOTFOUND;
			cli_perror();
			return 1;
		}
		ret = uci_reorder_section(ctx, ptr.s, strtoul(ptr.value, NULL, 10));
		break;
	case CMD_DEL:
		ret = uci_delete(ctx, &ptr);
		break;
	}

	/* no save necessary for get */
	if ((cmd == CMD_GET) || (cmd == CMD_REVERT))
		return 0;

	/* save changes, but don't commit them yet */
	if (ret == UCI_OK)
		ret = uci_save(ctx, ptr.p);

	if (ret != UCI_OK) {
		cli_perror();
		return 1;
	}

	return 0;
}

static int uci_batch_cmd(void)
{
	char *argv[MAX_ARGS + 2];
	char *str = NULL;
	int ret = 0;
	int i, j;

	for(i = 0; i <= MAX_ARGS; i++) {
		if (i == MAX_ARGS) {
			fprintf(stderr, "Too many arguments\n");
			return 1;
		}
		argv[i] = NULL;
		if ((ret = uci_parse_argument(ctx, input, &str, &argv[i])) != UCI_OK) {
			cli_perror();
			i = 0;
			break;
		}
		if (!argv[i][0])
			break;
		argv[i] = strdup(argv[i]);
		if (!argv[i]) {
			perror("uci");
			return 1;
		}
	}
	argv[i] = NULL;

	if (i > 0) {
		if (!strcasecmp(argv[0], "exit"))
			return 254;
		ret = uci_cmd(i, argv);
	} else
		return 0;

	for (j = 0; j < i; j++) {
		if (argv[j])
			free(argv[j]);
	}

	return ret;
}

static int uci_batch(void)
{
	int ret = 0;

	flags |= CLI_FLAG_BATCH;
	while (!feof(input)) {
		struct uci_element *e, *tmp;

		ret = uci_batch_cmd();
		if (ret == 254)
			return 0;
		else if (ret == 255)
			fprintf(stderr, "Unknown command\n");

		/* clean up */
		uci_foreach_element_safe(&ctx->root, tmp, e) {
			uci_unload(ctx, uci_to_package(e));
		}
	}
	flags &= ~CLI_FLAG_BATCH;

	return 0;
}

static int uci_cmd(int argc, char **argv)
{
	int cmd = 0;

	if (!strcasecmp(argv[0], "batch") && !(flags & CLI_FLAG_BATCH))
		return uci_batch();
	else if (!strcasecmp(argv[0], "show"))
		cmd = CMD_SHOW;
	else if (!strcasecmp(argv[0], "changes"))
		cmd = CMD_CHANGES;
	else if (!strcasecmp(argv[0], "export"))
		cmd = CMD_EXPORT;
	else if (!strcasecmp(argv[0], "commit"))
		cmd = CMD_COMMIT;
	else if (!strcasecmp(argv[0], "get"))
		cmd = CMD_GET;
	else if (!strcasecmp(argv[0], "set"))
		cmd = CMD_SET;
	else if (!strcasecmp(argv[0], "ren") ||
	         !strcasecmp(argv[0], "rename"))
		cmd = CMD_RENAME;
	else if (!strcasecmp(argv[0], "revert"))
		cmd = CMD_REVERT;
	else if (!strcasecmp(argv[0], "reorder"))
		cmd = CMD_REORDER;
	else if (!strcasecmp(argv[0], "del") ||
	         !strcasecmp(argv[0], "delete"))
		cmd = CMD_DEL;
	else if (!strcasecmp(argv[0], "import"))
		cmd = CMD_IMPORT;
	else if (!strcasecmp(argv[0], "help"))
		cmd = CMD_HELP;
	else if (!strcasecmp(argv[0], "add"))
		cmd = CMD_ADD;
	else if (!strcasecmp(argv[0], "add_list"))
		cmd = CMD_ADD_LIST;
	else
		cmd = -1;

	switch(cmd) {
		case CMD_ADD_LIST:
		case CMD_GET:
		case CMD_SET:
		case CMD_DEL:
		case CMD_RENAME:
		case CMD_REVERT:
		case CMD_REORDER:
			return uci_do_section_cmd(cmd, argc, argv);
		case CMD_SHOW:
		case CMD_EXPORT:
		case CMD_COMMIT:
		case CMD_CHANGES:
			return uci_do_package_cmd(cmd, argc, argv);
		case CMD_IMPORT:
			return uci_do_import(argc, argv);
		case CMD_ADD:
			return uci_do_add(argc, argv);
		case CMD_HELP:
			uci_usage();
			return 0;
		default:
			return 255;
	}
}

int main(int argc, char **argv)
{
	int ret;
	int c;

	flags = CLI_FLAG_SHOW_EXT;
	appname = argv[0];
	input = stdin;
	ctx = uci_alloc_context();
	if (!ctx) {
		fprintf(stderr, "Out of memory\n");
		return 1;
	}

	while((c = getopt(argc, argv, "c:d:f:LmnNp:P:sSqX")) != -1) {
		switch(c) {
			case 'c':
				uci_set_confdir(ctx, optarg);
				break;
			case 'd':
				delimiter = optarg;
				break;
			case 'f':
				input = fopen(optarg, "r");
				if (!input) {
					perror("uci");
					return 1;
				}
				break;
			case 'L':
				flags |= CLI_FLAG_NOPLUGINS;
				break;
			case 'm':
				flags |= CLI_FLAG_MERGE;
				break;
			case 's':
				ctx->flags |= UCI_FLAG_STRICT;
				break;
			case 'S':
				ctx->flags &= ~UCI_FLAG_STRICT;
				ctx->flags |= UCI_FLAG_PERROR;
				break;
			case 'n':
				ctx->flags |= UCI_FLAG_EXPORT_NAME;
				break;
			case 'N':
				ctx->flags &= ~UCI_FLAG_EXPORT_NAME;
				break;
			case 'p':
				uci_add_delta_path(ctx, optarg);
				break;
			case 'P':
				uci_add_delta_path(ctx, ctx->savedir);
				uci_set_savedir(ctx, optarg);
				flags |= CLI_FLAG_NOCOMMIT;
				break;
			case 'q':
				flags |= CLI_FLAG_QUIET;
				break;
			case 'X':
				flags &= ~CLI_FLAG_SHOW_EXT;
				break;
			default:
				uci_usage();
				return 0;
		}
	}
	if (optind > 1)
		argv[optind - 1] = argv[0];
	argv += optind - 1;
	argc -= optind - 1;

	if (argc < 2) {
		uci_usage();
		return 0;
	}

	if (!(flags & CLI_FLAG_NOPLUGINS))
		uci_load_plugins(ctx, NULL);

	ret = uci_cmd(argc - 1, argv + 1);
	if (input != stdin)
		fclose(input);

	if (ret == 255)
		uci_usage();

	uci_free_context(ctx);

	return ret;
}
