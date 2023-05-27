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

/*
 * This file contains the code for handling uci config delta files
 */

#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "uci.h"
#include "uci_internal.h"

/* record a change that was done to a package */
void
uci_add_delta(struct uci_context *ctx, struct uci_list *list, int cmd, const char *section, const char *option, const char *value)
{
	struct uci_delta *h;
	int size = strlen(section) + 1;
	char *ptr;

	if (value)
		size += strlen(value) + 1;

	h = uci_alloc_element(ctx, delta, option, size);
	ptr = uci_dataptr(h);
	h->cmd = cmd;
	h->section = strcpy(ptr, section);
	if (value) {
		ptr += strlen(ptr) + 1;
		h->value = strcpy(ptr, value);
	}
	uci_list_add(list, &h->e.list);
}

void
uci_free_delta(struct uci_delta *h)
{
	if (!h)
		return;
	if ((h->section != NULL) &&
		(h->section != uci_dataptr(h))) {
		free(h->section);
		free(h->value);
	}
	uci_free_element(&h->e);
}

static void uci_delta_save(struct uci_context *ctx, FILE *f,
			const char *name, const struct uci_delta *h)
{
	const struct uci_element *e = &h->e;
	char prefix[2] = {0, 0};

	if (h->cmd <= __UCI_CMD_LAST)
		prefix[0] = uci_command_char[h->cmd];

	fprintf(f, "%s%s.%s", prefix, name, h->section);
	if (e->name)
		fprintf(f, ".%s", e->name);

	if (h->cmd == UCI_CMD_REMOVE && !h->value)
		fprintf(f, "\n");
	else {
		int i;

		fprintf(f, "='");
		for (i = 0; h->value[i]; i++) {
			unsigned char c = h->value[i];
			if (c != '\'')
				fputc(c, f);
			else
				fprintf(f, "'\\''");
		}
		fprintf(f, "'\n");
	}
}

int uci_set_savedir(struct uci_context *ctx, const char *dir)
{
	char *sdir;
	struct uci_element *e, *tmp;
	volatile bool exists = false;

	UCI_HANDLE_ERR(ctx);
	UCI_ASSERT(ctx, dir != NULL);

	/* Move dir to the end of ctx->delta_path */
	uci_foreach_element_safe(&ctx->delta_path, tmp, e) {
		if (!strcmp(e->name, dir)) {
			exists = true;
			uci_list_del(&e->list);
			break;
		}
	}
	if (!exists)
		e = uci_alloc_generic(ctx, UCI_TYPE_PATH, dir, sizeof(struct uci_element));
	uci_list_add(&ctx->delta_path, &e->list);

	sdir = uci_strdup(ctx, dir);
	if (ctx->savedir != uci_savedir)
		free(ctx->savedir);
	ctx->savedir = sdir;
	return 0;
}

int uci_add_delta_path(struct uci_context *ctx, const char *dir)
{
	struct uci_element *e;
	struct uci_list *savedir;

	UCI_HANDLE_ERR(ctx);
	UCI_ASSERT(ctx, dir != NULL);

	/* Duplicate delta path is not allowed */
	uci_foreach_element(&ctx->delta_path, e) {
		if (!strcmp(e->name, dir))
			UCI_THROW(ctx, UCI_ERR_DUPLICATE);
	}

	e = uci_alloc_generic(ctx, UCI_TYPE_PATH, dir, sizeof(struct uci_element));
	/* Keep savedir at the end of ctx->delta_path list */
	savedir = ctx->delta_path.prev;
	uci_list_insert(savedir->prev, &e->list);

	return 0;
}

char const uci_command_char[] = {
	[UCI_CMD_ADD] = '+',
	[UCI_CMD_REMOVE] = '-',
	[UCI_CMD_CHANGE] = 0,
	[UCI_CMD_RENAME] = '@',
	[UCI_CMD_REORDER] = '^',
	[UCI_CMD_LIST_ADD] = '|',
	[UCI_CMD_LIST_DEL] = '~'
};

static inline int uci_parse_delta_tuple(struct uci_context *ctx, struct uci_ptr *ptr)
{
	struct uci_parse_context *pctx = ctx->pctx;
	char *str = pctx_cur_str(pctx), *arg;
	int c;

	UCI_INTERNAL(uci_parse_argument, ctx, ctx->pctx->file, &str, &arg);
	if (str && *str) {
		goto error;
	}
	for (c = 0; c <= __UCI_CMD_LAST; c++) {
		if (uci_command_char[c] == *arg)
			break;
	}
	if (c > __UCI_CMD_LAST)
		c = UCI_CMD_CHANGE;

	if (c != UCI_CMD_CHANGE)
		arg += 1;

	UCI_INTERNAL(uci_parse_ptr, ctx, ptr, arg);

	if (!ptr->section)
		goto error;
	if (ptr->flags & UCI_LOOKUP_EXTENDED)
		goto error;
	if (c != UCI_CMD_REMOVE && !ptr->value) {
		goto error;
	}

	switch(c) {
	case UCI_CMD_REORDER:
		if (!ptr->value || ptr->option)
			goto error;
		break;
	case UCI_CMD_RENAME:
		if (!ptr->value || !uci_validate_name(ptr->value))
			goto error;
		break;
	case UCI_CMD_LIST_ADD:
		if (!ptr->option)
			goto error;
		/* fall through */
	case UCI_CMD_LIST_DEL:
		if (!ptr->option)
			goto error;
	}

	return c;

error:
	UCI_THROW(ctx, UCI_ERR_INVAL);
	return 0;
}

static void uci_parse_delta_line(struct uci_context *ctx, struct uci_package *p)
{
	struct uci_element *e = NULL;
	struct uci_ptr ptr;
	int cmd;

	cmd = uci_parse_delta_tuple(ctx, &ptr);
	if (strcmp(ptr.package, p->e.name) != 0)
		goto error;

	if (ctx->flags & UCI_FLAG_SAVED_DELTA)
		uci_add_delta(ctx, &p->saved_delta, cmd, ptr.section, ptr.option, ptr.value);

	switch(cmd) {
	case UCI_CMD_REORDER:
		uci_expand_ptr(ctx, &ptr, true);
		if (!ptr.s)
			UCI_THROW(ctx, UCI_ERR_NOTFOUND);
		UCI_INTERNAL(uci_reorder_section, ctx, ptr.s, strtoul(ptr.value, NULL, 10));
		break;
	case UCI_CMD_RENAME:
		UCI_INTERNAL(uci_rename, ctx, &ptr);
		break;
	case UCI_CMD_REMOVE:
		UCI_INTERNAL(uci_delete, ctx, &ptr);
		break;
	case UCI_CMD_LIST_ADD:
		UCI_INTERNAL(uci_add_list, ctx, &ptr);
		break;
	case UCI_CMD_LIST_DEL:
		UCI_INTERNAL(uci_del_list, ctx, &ptr);
		break;
	case UCI_CMD_ADD:
	case UCI_CMD_CHANGE:
		UCI_INTERNAL(uci_set, ctx, &ptr);
		e = ptr.last;
		if (!ptr.option && e && (cmd == UCI_CMD_ADD))
			uci_to_section(e)->anonymous = true;
		break;
	}
	return;
error:
	UCI_THROW(ctx, UCI_ERR_PARSE);
}

/* returns the number of changes that were successfully parsed */
static int uci_parse_delta(struct uci_context *ctx, FILE *stream, struct uci_package *p)
{
	struct uci_parse_context *pctx;
	volatile int changes = 0;

	/* make sure no memory from previous parse attempts is leaked */
	uci_cleanup(ctx);

	pctx = (struct uci_parse_context *) uci_malloc(ctx, sizeof(struct uci_parse_context));
	ctx->pctx = pctx;
	pctx->file = stream;

	while (!feof(pctx->file)) {
		pctx->pos = 0;
		uci_getln(ctx, 0);
		if (!pctx->buf[0])
			continue;

		/*
		 * ignore parse errors in single lines, we want to preserve as much
		 * delta as possible
		 */
		UCI_TRAP_SAVE(ctx, error);
		uci_parse_delta_line(ctx, p);
		UCI_TRAP_RESTORE(ctx);
		changes++;
error:
		continue;
	}

	/* no error happened, we can get rid of the parser context now */
	uci_cleanup(ctx);
	return changes;
}

/* returns the number of changes that were successfully parsed */
static int uci_load_delta_file(struct uci_context *ctx, struct uci_package *p, char *filename, FILE **f, bool flush)
{
	FILE *volatile stream = NULL;
	volatile int changes = 0;

	UCI_TRAP_SAVE(ctx, done);
	stream = uci_open_stream(ctx, filename, NULL, SEEK_SET, flush, false);
	UCI_TRAP_RESTORE(ctx);

	if (p)
		changes = uci_parse_delta(ctx, stream, p);

done:
	if (f)
		*f = stream;
	else
		uci_close_stream(stream);
	return changes;
}

/* returns the number of changes that were successfully parsed */
__private int uci_load_delta(struct uci_context *ctx, struct uci_package *p, bool flush)
{
	struct uci_element *e;
	char *filename = NULL;
	FILE *volatile f = NULL;
	volatile int changes = 0;

	if (!p->has_delta)
		return 0;

	uci_foreach_element(&ctx->delta_path, e) {
		if ((asprintf(&filename, "%s/%s", e->name, p->e.name) < 0) || !filename)
			UCI_THROW(ctx, UCI_ERR_MEM);

		changes += uci_load_delta_file(ctx, p, filename, NULL, false);
		free(filename);
	}

	if ((asprintf(&filename, "%s/%s", ctx->savedir, p->e.name) < 0) || !filename)
		UCI_THROW(ctx, UCI_ERR_MEM);
	UCI_TRAP_SAVE(ctx, done);
	f = uci_open_stream(ctx, filename, NULL, SEEK_SET, flush, false);
	UCI_TRAP_RESTORE(ctx);

	if (flush && f && (changes > 0)) {
		if (ftruncate(fileno(f), 0) < 0) {
			free(filename);
			uci_close_stream(f);
			UCI_THROW(ctx, UCI_ERR_IO);
		}
	}

done:
	free(filename);
	uci_close_stream(f);
	ctx->err = 0;
	return changes;
}

static void uci_filter_delta(struct uci_context *ctx, const char *name, const char *section, const char *option)
{
	struct uci_parse_context *pctx;
	struct uci_element *e, *tmp;
	struct uci_list list;
	char *filename = NULL;
	struct uci_ptr ptr;
	FILE *f = NULL;

	uci_list_init(&list);
	uci_alloc_parse_context(ctx);
	pctx = ctx->pctx;

	if ((asprintf(&filename, "%s/%s", ctx->savedir, name) < 0) || !filename)
		UCI_THROW(ctx, UCI_ERR_MEM);

	UCI_TRAP_SAVE(ctx, done);
	f = uci_open_stream(ctx, filename, NULL, SEEK_SET, true, false);
	pctx->file = f;
	while (!feof(f)) {
		enum uci_command c;
		bool match;

		pctx->pos = 0;
		uci_getln(ctx, 0);
		if (!pctx->buf[0])
			continue;

		c = uci_parse_delta_tuple(ctx, &ptr);
		match = true;
		if (section) {
			if (!ptr.section || (strcmp(section, ptr.section) != 0))
				match = false;
		}
		if (match && option) {
			if (!ptr.option || (strcmp(option, ptr.option) != 0))
				match = false;
		}

		if (!match && ptr.section) {
			uci_add_delta(ctx, &list, c,
				ptr.section, ptr.option, ptr.value);
		}
	}

	/* rebuild the delta file */
	rewind(f);
	if (ftruncate(fileno(f), 0) < 0)
		UCI_THROW(ctx, UCI_ERR_IO);
	uci_foreach_element_safe(&list, tmp, e) {
		struct uci_delta *h = uci_to_delta(e);
		uci_delta_save(ctx, f, name, h);
		uci_free_delta(h);
	}
	UCI_TRAP_RESTORE(ctx);

done:
	free(filename);
	uci_close_stream(pctx->file);
	uci_foreach_element_safe(&list, tmp, e) {
		uci_free_delta(uci_to_delta(e));
	}
	uci_cleanup(ctx);
}

int uci_revert(struct uci_context *ctx, struct uci_ptr *ptr)
{
	char *volatile package = NULL;
	char *volatile section = NULL;
	char *volatile option = NULL;

	UCI_HANDLE_ERR(ctx);
	uci_expand_ptr(ctx, ptr, false);
	UCI_ASSERT(ctx, ptr->p->has_delta);

	/*
	 * - flush unwritten changes
	 * - save the package name
	 * - unload the package
	 * - filter the delta
	 * - reload the package
	 */
	UCI_TRAP_SAVE(ctx, error);
	UCI_INTERNAL(uci_save, ctx, ptr->p);

	/* NB: need to clone package, section and option names,
	 * as they may get freed on uci_free_package() */
	package = uci_strdup(ctx, ptr->p->e.name);
	if (ptr->section)
		section = uci_strdup(ctx, ptr->section);
	if (ptr->option)
		option = uci_strdup(ctx, ptr->option);

	uci_free_package(&ptr->p);
	uci_filter_delta(ctx, package, section, option);

	UCI_INTERNAL(uci_load, ctx, package, &ptr->p);
	UCI_TRAP_RESTORE(ctx);
	ctx->err = 0;

error:
	free(package);
	free(section);
	free(option);
	if (ctx->err)
		UCI_THROW(ctx, ctx->err);
	return 0;
}

int uci_save(struct uci_context *ctx, struct uci_package *p)
{
	FILE *volatile f = NULL;
	char *filename = NULL;
	struct uci_element *e, *tmp;
	struct stat statbuf;

	UCI_HANDLE_ERR(ctx);
	UCI_ASSERT(ctx, p != NULL);

	/*
	 * if the config file was outside of the /etc/config path,
	 * don't save the delta to a file, update the real file
	 * directly.
	 * does not modify the uci_package pointer
	 */
	if (!p->has_delta)
		return uci_commit(ctx, &p, false);

	if (uci_list_empty(&p->delta))
		return 0;

	if (stat(ctx->savedir, &statbuf) < 0) {
		if (stat(ctx->confdir, &statbuf) == 0) {
			mkdir(ctx->savedir, statbuf.st_mode);
		} else {
			mkdir(ctx->savedir, UCI_DIRMODE);
		}
	} else if ((statbuf.st_mode & S_IFMT) != S_IFDIR) {
		UCI_THROW(ctx, UCI_ERR_IO);
	}

	if ((asprintf(&filename, "%s/%s", ctx->savedir, p->e.name) < 0) || !filename)
		UCI_THROW(ctx, UCI_ERR_MEM);

	ctx->err = 0;
	UCI_TRAP_SAVE(ctx, done);
	f = uci_open_stream(ctx, filename, NULL, SEEK_END, true, true);
	UCI_TRAP_RESTORE(ctx);

	uci_foreach_element_safe(&p->delta, tmp, e) {
		struct uci_delta *h = uci_to_delta(e);
		uci_delta_save(ctx, f, p->e.name, h);
		uci_free_delta(h);
	}

done:
	uci_close_stream(f);
	free(filename);
	if (ctx->err)
		UCI_THROW(ctx, ctx->err);

	return 0;
}


