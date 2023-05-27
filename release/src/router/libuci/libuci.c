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
 * This file contains some common code for the uci library
 */

#define _GNU_SOURCE
#include <sys/types.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <glob.h>
#include "uci.h"

static const char *uci_errstr[] = {
	[UCI_OK] =            "Success",
	[UCI_ERR_MEM] =       "Out of memory",
	[UCI_ERR_INVAL] =     "Invalid argument",
	[UCI_ERR_NOTFOUND] =  "Entry not found",
	[UCI_ERR_IO] =        "I/O error",
	[UCI_ERR_PARSE] =     "Parse error",
	[UCI_ERR_DUPLICATE] = "Duplicate entry",
	[UCI_ERR_UNKNOWN] =   "Unknown error",
};

#include "uci_internal.h"
#include "list.c"

__private const char *uci_confdir = UCI_CONFDIR;
__private const char *uci_savedir = UCI_SAVEDIR;

/* exported functions */
struct uci_context *uci_alloc_context(void)
{
	struct uci_context *ctx;

	ctx = (struct uci_context *) calloc(1, sizeof(struct uci_context));
	if (!ctx)
		return NULL;

	uci_list_init(&ctx->root);
	uci_list_init(&ctx->delta_path);
	uci_list_init(&ctx->backends);
	ctx->flags = UCI_FLAG_STRICT | UCI_FLAG_SAVED_DELTA;

	ctx->confdir = (char *) uci_confdir;
	ctx->savedir = (char *) uci_savedir;
	uci_add_delta_path(ctx, uci_savedir);

	uci_list_add(&ctx->backends, &uci_file_backend.e.list);
	ctx->backend = &uci_file_backend;

	return ctx;
}

void uci_free_context(struct uci_context *ctx)
{
	struct uci_element *e, *tmp;

	if (ctx->confdir != uci_confdir)
		free(ctx->confdir);
	if (ctx->savedir != uci_savedir)
		free(ctx->savedir);

	uci_cleanup(ctx);
	UCI_TRAP_SAVE(ctx, ignore);
	uci_foreach_element_safe(&ctx->root, tmp, e) {
		struct uci_package *p = uci_to_package(e);
		uci_free_package(&p);
	}
	uci_foreach_element_safe(&ctx->delta_path, tmp, e) {
		uci_free_element(e);
	}
	UCI_TRAP_RESTORE(ctx);
	free(ctx);

ignore:
	return;
}

int uci_set_confdir(struct uci_context *ctx, const char *dir)
{
	char *cdir;

	UCI_HANDLE_ERR(ctx);
	UCI_ASSERT(ctx, dir != NULL);

	cdir = uci_strdup(ctx, dir);
	if (ctx->confdir != uci_confdir)
		free(ctx->confdir);
	ctx->confdir = cdir;
	return 0;
}

__private void uci_cleanup(struct uci_context *ctx)
{
	struct uci_parse_context *pctx;

	if (ctx->buf) {
		free(ctx->buf);
		ctx->buf = NULL;
		ctx->bufsz = 0;
	}

	pctx = ctx->pctx;
	if (!pctx)
		return;

	ctx->pctx = NULL;
	if (pctx->package)
		uci_free_package(&pctx->package);

	if (pctx->buf)
		free(pctx->buf);

	free(pctx);
}

void
uci_perror(struct uci_context *ctx, const char *str)
{
	uci_get_errorstr(ctx, NULL, str);
}

void
uci_get_errorstr(struct uci_context *ctx, char **dest, const char *prefix)
{
	static char error_info[128] = { 0 };
	int err;

	err = ctx ? ctx->err : UCI_ERR_INVAL;
	if ((err < 0) || (err >= UCI_ERR_LAST))
		err = UCI_ERR_UNKNOWN;

	if (ctx && ctx->pctx && (err == UCI_ERR_PARSE)) {
		snprintf(error_info, sizeof(error_info) - 1, " (%s) at line %d, byte %zu",
			 (ctx->pctx->reason ? ctx->pctx->reason : "unknown"),
			 ctx->pctx->line, ctx->pctx->byte);
	}

	if (!dest) {
		strcat(error_info, "\n");
		fprintf(stderr, "%s%s%s%s%s%s",
			(prefix ? prefix : ""), (prefix ? ": " : ""),
			(ctx && ctx->func ? ctx->func : ""), (ctx && ctx->func ? ": " : ""),
			uci_errstr[err],
			error_info);
		return;
	}

	err = asprintf(dest, "%s%s%s%s%s%s",
		(prefix ? prefix : ""), (prefix ? ": " : ""),
		(ctx && ctx->func ? ctx->func : ""), (ctx && ctx->func ? ": " : ""),
		uci_errstr[err],
		error_info);

	if (err < 0)
		*dest = NULL;
}

int uci_list_configs(struct uci_context *ctx, char ***list)
{
	UCI_HANDLE_ERR(ctx);
	UCI_ASSERT(ctx, list != NULL);
	UCI_ASSERT(ctx, ctx->backend && ctx->backend->list_configs);
	*list = ctx->backend->list_configs(ctx);
	return 0;
}

int uci_commit(struct uci_context *ctx, struct uci_package **package, bool overwrite)
{
	struct uci_package *p;
	UCI_HANDLE_ERR(ctx);
	UCI_ASSERT(ctx, package != NULL);
	p = *package;
	UCI_ASSERT(ctx, p != NULL);
	UCI_ASSERT(ctx, p->backend && p->backend->commit);
	p->backend->commit(ctx, package, overwrite);
	return 0;
}

int uci_load(struct uci_context *ctx, const char *name, struct uci_package **package)
{
	struct uci_package *p;

	UCI_HANDLE_ERR(ctx);
	UCI_ASSERT(ctx, ctx->backend && ctx->backend->load);
	p = ctx->backend->load(ctx, name);
	if (package)
		*package = p;

	return 0;
}

int uci_set_backend(struct uci_context *ctx, const char *name)
{
	struct uci_element *e;

	UCI_HANDLE_ERR(ctx);
	UCI_ASSERT(ctx, name != NULL);
	e = uci_lookup_list(&ctx->backends, name);
	if (!e)
		UCI_THROW(ctx, UCI_ERR_NOTFOUND);
	ctx->backend = uci_to_backend(e);
	return 0;
}
