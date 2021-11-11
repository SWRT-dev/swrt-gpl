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

static void uci_unload_plugin(struct uci_context *ctx, struct uci_plugin *p);

#include "uci_internal.h"
#include "list.c"

__private const char *uci_confdir = UCI_CONFDIR;
__private const char *uci_savedir = UCI_SAVEDIR;

/* exported functions */
struct uci_context *uci_alloc_context(void)
{
	struct uci_context *ctx;

	ctx = (struct uci_context *) malloc(sizeof(struct uci_context));
	if (!ctx)
		return NULL;

	memset(ctx, 0, sizeof(struct uci_context));
	uci_list_init(&ctx->root);
	uci_list_init(&ctx->delta_path);
	uci_list_init(&ctx->backends);
	uci_list_init(&ctx->hooks);
	uci_list_init(&ctx->plugins);
	ctx->flags = UCI_FLAG_STRICT | UCI_FLAG_SAVED_DELTA;

	ctx->confdir = (char *) uci_confdir;
	ctx->savedir = (char *) uci_savedir;

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
	uci_foreach_element_safe(&ctx->root, tmp, e) {
		uci_unload_plugin(ctx, uci_to_plugin(e));
	}
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
	static char error_info[128];
	int err;
	const char *format =
		"%s%s" /* prefix */
		"%s%s" /* function */
		"%s" /* error */
		"%s"; /* details */

	error_info[0] = 0;

	if (!ctx)
		err = UCI_ERR_INVAL;
	else
		err = ctx->err;

	if ((err < 0) || (err >= UCI_ERR_LAST))
		err = UCI_ERR_UNKNOWN;

	switch (err) {
	case UCI_ERR_PARSE:
		if (ctx->pctx) {
			snprintf(error_info, sizeof(error_info) - 1, " (%s) at line %d, byte %d", (ctx->pctx->reason ? ctx->pctx->reason : "unknown"), ctx->pctx->line, ctx->pctx->byte);
			break;
		}
		break;
	default:
		break;
	}
	if (dest) {
		err = asprintf(dest, format,
			(prefix ? prefix : ""), (prefix ? ": " : ""),
			(ctx->func ? ctx->func : ""), (ctx->func ? ": " : ""),
			uci_errstr[err],
			error_info);
		if (err < 0)
			*dest = NULL;
	} else {
		strcat(error_info, "\n");
		fprintf(stderr, format,
			(prefix ? prefix : ""), (prefix ? ": " : ""),
			(ctx->func ? ctx->func : ""), (ctx->func ? ": " : ""),
			uci_errstr[err],
			error_info);
	}
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
	struct uci_element *e;

	UCI_HANDLE_ERR(ctx);
	UCI_ASSERT(ctx, ctx->backend && ctx->backend->load);
	p = ctx->backend->load(ctx, name);
	uci_foreach_element(&ctx->hooks, e) {
		struct uci_hook *h = uci_to_hook(e);
		if (h->ops->load)
			h->ops->load(h->ops, p);
	}
	if (package)
		*package = p;

	return 0;
}

#ifdef UCI_PLUGIN_SUPPORT

__plugin int uci_add_backend(struct uci_context *ctx, struct uci_backend *b)
{
	struct uci_element *e;
	UCI_HANDLE_ERR(ctx);

	e = uci_lookup_list(&ctx->backends, b->e.name);
	if (e)
		UCI_THROW(ctx, UCI_ERR_DUPLICATE);

	e = uci_malloc(ctx, sizeof(struct uci_backend));
	memcpy(e, b, sizeof(struct uci_backend));

	uci_list_add(&ctx->backends, &e->list);
	return 0;
}

__plugin int uci_del_backend(struct uci_context *ctx, struct uci_backend *b)
{
	struct uci_element *e, *tmp;

	UCI_HANDLE_ERR(ctx);

	e = uci_lookup_list(&ctx->backends, b->e.name);
	if (!e || uci_to_backend(e)->ptr != b->ptr)
		UCI_THROW(ctx, UCI_ERR_NOTFOUND);
	b = uci_to_backend(e);

	if (ctx->backend && ctx->backend->ptr == b->ptr)
		ctx->backend = &uci_file_backend;

	uci_foreach_element_safe(&ctx->root, tmp, e) {
		struct uci_package *p = uci_to_package(e);

		if (!p->backend)
			continue;

		if (p->backend->ptr == b->ptr)
			UCI_INTERNAL(uci_unload, ctx, p);
	}

	uci_list_del(&b->e.list);
	free(b);

	return 0;
}

#endif

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

int uci_add_hook(struct uci_context *ctx, const struct uci_hook_ops *ops)
{
	struct uci_element *e;
	struct uci_hook *h;

	UCI_HANDLE_ERR(ctx);

	/* check for duplicate elements */
	uci_foreach_element(&ctx->hooks, e) {
		h = uci_to_hook(e);
		if (h->ops == ops)
			return UCI_ERR_INVAL;
	}

	h = uci_alloc_element(ctx, hook, "", 0);
	h->ops = ops;
	uci_list_init(&h->e.list);
	uci_list_add(&ctx->hooks, &h->e.list);

	return 0;
}

int uci_remove_hook(struct uci_context *ctx, const struct uci_hook_ops *ops)
{
	struct uci_element *e;

	uci_foreach_element(&ctx->hooks, e) {
		struct uci_hook *h = uci_to_hook(e);
		if (h->ops == ops) {
			uci_list_del(&e->list);
			return 0;
		}
	}
	return UCI_ERR_NOTFOUND;
}

int uci_load_plugin(struct uci_context *ctx, const char *filename)
{
	struct uci_plugin *p;
	const struct uci_plugin_ops *ops;
	void *dlh;

	UCI_HANDLE_ERR(ctx);
	dlh = dlopen(filename, RTLD_GLOBAL|RTLD_NOW);
	if (!dlh)
		UCI_THROW(ctx, UCI_ERR_NOTFOUND);

	ops = dlsym(dlh, "uci_plugin");
	if (!ops || !ops->attach || (ops->attach(ctx) != 0)) {
		if (!ops)
			fprintf(stderr, "No ops\n");
		else if (!ops->attach)
			fprintf(stderr, "No attach\n");
		else
			fprintf(stderr, "Other weirdness\n");
		dlclose(dlh);
		UCI_THROW(ctx, UCI_ERR_INVAL);
	}

	p = uci_alloc_element(ctx, plugin, filename, 0);
	p->dlh = dlh;
	p->ops = ops;
	uci_list_add(&ctx->plugins, &p->e.list);

	return 0;
}

static void uci_unload_plugin(struct uci_context *ctx, struct uci_plugin *p)
{
	if (p->ops->detach)
		p->ops->detach(ctx);
	dlclose(p->dlh);
	uci_free_element(&p->e);
}

int uci_load_plugins(struct uci_context *ctx, const char *pattern)
{
	glob_t gl;
	int i;

	if (!pattern)
		pattern = UCI_PREFIX "/lib/uci_*.so";

	memset(&gl, 0, sizeof(gl));
	glob(pattern, 0, NULL, &gl);
	for (i = 0; i < gl.gl_pathc; i++)
		uci_load_plugin(ctx, gl.gl_pathv[i]);

	return 0;
}
