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
 * This file contains the code for parsing uci config files
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <ctype.h>
#include <glob.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "uci.h"
#include "uci_internal.h"

#define LINEBUF	32

/*
 * Fetch a new line from the input stream and resize buffer if necessary
 */
__private void uci_getln(struct uci_context *ctx, size_t offset)
{
	struct uci_parse_context *pctx = ctx->pctx;
	char *p;
	size_t ofs;

	if (pctx->buf == NULL) {
		pctx->buf = uci_malloc(ctx, LINEBUF);
		pctx->bufsz = LINEBUF;
	}
	/* It takes 2 slots for fgets to read 1 char. */
	if (offset >= pctx->bufsz - 1) {
		pctx->bufsz *= 2;
		pctx->buf = uci_realloc(ctx, pctx->buf, pctx->bufsz);
	}

	ofs = offset;
	do {
		p = &pctx->buf[ofs];
		p[0] = 0;

		p = fgets(p, pctx->bufsz - ofs, pctx->file);
		if (!p || !*p)
			return;

		ofs += strlen(p);
		pctx->buf_filled = ofs;
		if (pctx->buf[ofs - 1] == '\n') {
			pctx->line++;
			return;
		}

		pctx->bufsz *= 2;
		pctx->buf = uci_realloc(ctx, pctx->buf, pctx->bufsz);
	} while (1);
}

/*
 * parse a character escaped by '\'
 * returns true if the escaped character is to be parsed
 * returns false if the escaped character is to be ignored
 */
static bool parse_backslash(struct uci_context *ctx)
{
	struct uci_parse_context *pctx = ctx->pctx;

	/* skip backslash */
	pctx->pos += 1;

	/* undecoded backslash at the end of line, fetch the next line */
	if (!pctx_cur_char(pctx) ||
	    pctx_cur_char(pctx) == '\n' ||
	    (pctx_cur_char(pctx) == '\r' &&
	     pctx_char(pctx, pctx_pos(pctx) + 1) == '\n' &&
	     !pctx_char(pctx, pctx_pos(pctx) + 2))) {
		uci_getln(ctx, pctx->pos);
		return false;
	}

	/* FIXME: decode escaped char, necessary? */
	return true;
}

/*
 * move the string pointer forward until a non-whitespace character or
 * EOL is reached
 */
static void skip_whitespace(struct uci_context *ctx)
{
	struct uci_parse_context *pctx = ctx->pctx;

	while (pctx_cur_char(pctx) && isspace(pctx_cur_char(pctx)))
		pctx->pos += 1;
}

static inline void addc(struct uci_context *ctx, size_t *pos_dest, size_t *pos_src)
{
	struct uci_parse_context *pctx = ctx->pctx;

	pctx_char(pctx, *pos_dest) = pctx_char(pctx, *pos_src);
	*pos_dest += 1;
	*pos_src += 1;
}

static int uci_increase_pos(struct uci_parse_context *pctx, size_t add)
{
	if (pctx->pos + add > pctx->buf_filled)
		return -EINVAL;

	pctx->pos += add;
	return 0;
}

/*
 * parse a double quoted string argument from the command line
 */
static void parse_double_quote(struct uci_context *ctx, size_t *target)
{
	struct uci_parse_context *pctx = ctx->pctx;
	char c;

	/* skip quote character */
	pctx->pos += 1;

	while (1) {
		c = pctx_cur_char(pctx);
		switch(c) {
		case '"':
			pctx->pos += 1;
			return;
		case 0:
			/* Multi-line str value */
			uci_getln(ctx, pctx->pos);
			if (!pctx_cur_char(pctx)) {
				uci_parse_error(ctx, "EOF with unterminated \"");
			}
			break;
		case '\\':
			if (!parse_backslash(ctx))
				continue;
			/* fall through */
		default:
			addc(ctx, target, &pctx->pos);
			break;
		}
	}
}

/*
 * parse a single quoted string argument from the command line
 */
static void parse_single_quote(struct uci_context *ctx, size_t *target)
{
	struct uci_parse_context *pctx = ctx->pctx;
	char c;
	/* skip quote character */
	pctx->pos += 1;

	while (1) {
		c = pctx_cur_char(pctx);
		switch(c) {
		case '\'':
			pctx->pos += 1;
			return;
		case 0:
			/* Multi-line str value */
			uci_getln(ctx, pctx->pos);
			if (!pctx_cur_char(pctx))
				uci_parse_error(ctx, "EOF with unterminated '");

			break;
		default:
			addc(ctx, target, &pctx->pos);
		}
	}
}

/*
 * parse a string from the command line and detect the quoting style
 */
static void parse_str(struct uci_context *ctx, size_t *target)
{
	struct uci_parse_context *pctx = ctx->pctx;
	bool next = true;
	do {
		switch(pctx_cur_char(pctx)) {
		case '\'':
			parse_single_quote(ctx, target);
			break;
		case '"':
			parse_double_quote(ctx, target);
			break;
		case '#':
			pctx_cur_char(pctx) = 0;
			/* fall through */
		case 0:
			goto done;
		case ';':
			next = false;
			goto done;
		case '\\':
			if (!parse_backslash(ctx))
				continue;
			/* fall through */
		default:
			addc(ctx, target, &pctx->pos);
			break;
		}
	} while (pctx_cur_char(pctx) && !isspace(pctx_cur_char(pctx)));
done:

	/*
	 * if the string was unquoted and we've stopped at a whitespace
	 * character, skip to the next one, because the whitespace will
	 * be overwritten by a null byte here
	 */
	if (pctx_cur_char(pctx) && next)
		pctx->pos += 1;

	/* terminate the parsed string */
	pctx_char(pctx, *target) = 0;
}

/*
 * extract the next argument from the command line
 */
static int next_arg(struct uci_context *ctx, bool required, bool name, bool package)
{
	struct uci_parse_context *pctx = ctx->pctx;
	size_t val, ptr;

	skip_whitespace(ctx);
	val = ptr = pctx_pos(pctx);
	if (pctx_cur_char(pctx) == ';') {
		pctx_cur_char(pctx) = 0;
		pctx->pos += 1;
	} else {
		parse_str(ctx, &ptr);
	}
	if (!pctx_char(pctx, val)) {
		if (required)
			uci_parse_error(ctx, "insufficient arguments");
		goto done;
	}

	if (name && !uci_validate_str(pctx_str(pctx, val), name, package))
		uci_parse_error(ctx, "invalid character in name field");

done:
	return val;
}

int uci_parse_argument(struct uci_context *ctx, FILE *stream, char **str, char **result)
{
	int ofs_result;

	UCI_HANDLE_ERR(ctx);
	UCI_ASSERT(ctx, str != NULL);
	UCI_ASSERT(ctx, result != NULL);

	if (ctx->pctx && (ctx->pctx->file != stream))
		uci_cleanup(ctx);

	if (!ctx->pctx)
		uci_alloc_parse_context(ctx);

	ctx->pctx->file = stream;
	if (!*str) {
		ctx->pctx->pos = 0;
		uci_getln(ctx, 0);
	}

	ofs_result = next_arg(ctx, false, false, false);
	*result = pctx_str(ctx->pctx, ofs_result);
	*str = pctx_cur_str(ctx->pctx);

	return 0;
}

static int
uci_fill_ptr(struct uci_context *ctx, struct uci_ptr *ptr, struct uci_element *e)
{
	UCI_ASSERT(ctx, ptr != NULL);
	UCI_ASSERT(ctx, e != NULL);

	memset(ptr, 0, sizeof(struct uci_ptr));
	switch(e->type) {
	case UCI_TYPE_OPTION:
		ptr->o = uci_to_option(e);
		goto fill_option;
	case UCI_TYPE_SECTION:
		ptr->s = uci_to_section(e);
		goto fill_section;
	case UCI_TYPE_PACKAGE:
		ptr->p = uci_to_package(e);
		goto fill_package;
	default:
		UCI_THROW(ctx, UCI_ERR_INVAL);
	}

fill_option:
	ptr->option = ptr->o->e.name;
	ptr->s = ptr->o->section;
fill_section:
	ptr->section = ptr->s->e.name;
	ptr->p = ptr->s->package;
fill_package:
	ptr->package = ptr->p->e.name;

	ptr->flags |= UCI_LOOKUP_DONE;

	return 0;
}



/*
 * verify that the end of the line or command is reached.
 * throw an error if extra arguments are given on the command line
 */
static void assert_eol(struct uci_context *ctx)
{
	char *tmp;
	int ofs_tmp;

	skip_whitespace(ctx);
	ofs_tmp = next_arg(ctx, false, false, false);
	tmp = pctx_str(ctx->pctx, ofs_tmp);
	if (*tmp && (ctx->flags & UCI_FLAG_STRICT))
		uci_parse_error(ctx, "too many arguments");
}

/*
 * switch to a different config, either triggered by uci_load, or by a
 * 'package <...>' statement in the import file
 */
static void uci_switch_config(struct uci_context *ctx)
{
	struct uci_parse_context *pctx;
	struct uci_element *e;
	const char *name;

	pctx = ctx->pctx;
	name = pctx->name;

	/* add the last config to main config file list */
	if (pctx->package) {
		pctx->package->backend = ctx->backend;
		uci_list_add(&ctx->root, &pctx->package->e.list);

		pctx->package = NULL;
		pctx->section = NULL;
	}

	if (!name)
		return;

	/*
	 * if an older config under the same name exists, unload it
	 * ignore errors here, e.g. if the config was not found
	 */
	e = uci_lookup_list(&ctx->root, name);
	if (e)
		UCI_THROW(ctx, UCI_ERR_DUPLICATE);
	pctx->package = uci_alloc_package(ctx, name);
}

/*
 * parse the 'package' uci command (next config package)
 */
static void uci_parse_package(struct uci_context *ctx, bool single)
{
	struct uci_parse_context *pctx = ctx->pctx;
	int ofs_name;
	char *name;

	/* command string null-terminated by strtok */
	if (uci_increase_pos(pctx, strlen(pctx_cur_str(pctx)) + 1))
		uci_parse_error(ctx, "package without name");

	ofs_name = next_arg(ctx, true, true, true);
	assert_eol(ctx);
	name = pctx_str(pctx, ofs_name);
	if (single)
		return;

	ctx->pctx->name = name;
	uci_switch_config(ctx);
}

/*
 * parse the 'config' uci command (open a section)
 */
static void uci_parse_config(struct uci_context *ctx)
{
	struct uci_parse_context *pctx = ctx->pctx;
	struct uci_element *e;
	struct uci_ptr ptr;
	int ofs_name, ofs_type;
	char *name;
	char *type;

	if (!ctx->pctx->package) {
		if (!ctx->pctx->name)
			uci_parse_error(ctx, "attempting to import a file without a package name");

		uci_switch_config(ctx);
	}

	/* command string null-terminated by strtok */
	if (uci_increase_pos(pctx, strlen(pctx_cur_str(pctx)) + 1))
		uci_parse_error(ctx, "config without name");

	ofs_type = next_arg(ctx, true, false, false);
	type = pctx_str(pctx, ofs_type);
	if (!uci_validate_type(type))
		uci_parse_error(ctx, "invalid character in type field");

	ofs_name = next_arg(ctx, false, true, false);
	assert_eol(ctx);
	type = pctx_str(pctx, ofs_type);
	name = pctx_str(pctx, ofs_name);

	if (!name || !name[0]) {
		ctx->internal = !pctx->merge;
		UCI_NESTED(uci_add_section, ctx, pctx->package, type, &pctx->section);
	} else {
		uci_fill_ptr(ctx, &ptr, &pctx->package->e);
		e = uci_lookup_list(&pctx->package->sections, name);
		if (e) {
			ptr.s = uci_to_section(e);

			if ((ctx->flags & UCI_FLAG_STRICT) && strcmp(ptr.s->type, type))
				uci_parse_error(ctx, "section of different type overwrites prior section with same name");
		}

		ptr.section = name;
		ptr.value = type;

		ctx->internal = !pctx->merge;
		UCI_NESTED(uci_set, ctx, &ptr);
		pctx->section = uci_to_section(ptr.last);
	}
}

/*
 * parse the 'option' uci command (open a value)
 */
static void uci_parse_option(struct uci_context *ctx, bool list)
{
	struct uci_parse_context *pctx = ctx->pctx;
	struct uci_element *e;
	struct uci_ptr ptr;
	int ofs_name, ofs_value;
	char *name = NULL;
	char *value = NULL;

	if (!pctx->section)
		uci_parse_error(ctx, "option/list command found before the first section");

	/* command string null-terminated by strtok */
	if (uci_increase_pos(pctx, strlen(pctx_cur_str(pctx)) + 1))
		uci_parse_error(ctx, "option without name");

	ofs_name = next_arg(ctx, true, true, false);
	ofs_value = next_arg(ctx, false, false, false);
	assert_eol(ctx);
	name = pctx_str(pctx, ofs_name);
	value = pctx_str(pctx, ofs_value);

	uci_fill_ptr(ctx, &ptr, &pctx->section->e);
	e = uci_lookup_list(&pctx->section->options, name);
	if (e)
		ptr.o = uci_to_option(e);
	ptr.option = name;
	ptr.value = value;

	ctx->internal = !pctx->merge;
	if (list)
		UCI_NESTED(uci_add_list, ctx, &ptr);
	else
		UCI_NESTED(uci_set, ctx, &ptr);
}

/*
 * parse a complete input line, split up combined commands by ';'
 */
static void uci_parse_line(struct uci_context *ctx, bool single)
{
	struct uci_parse_context *pctx = ctx->pctx;
	char *word;

	/* Skip whitespace characters at the start of line */
	skip_whitespace(ctx);
	do {
		word = strtok(pctx_cur_str(pctx), " \t");
		if (!word)
			return;

		switch(word[0]) {
			case 0:
			case '#':
				return;
			case 'p':
				if ((word[1] == 0) || !strcmp(word + 1, "ackage"))
					uci_parse_package(ctx, single);
				else
					goto invalid;
				break;
			case 'c':
				if ((word[1] == 0) || !strcmp(word + 1, "onfig"))
					uci_parse_config(ctx);
				else
					goto invalid;
				break;
			case 'o':
				if ((word[1] == 0) || !strcmp(word + 1, "ption"))
					uci_parse_option(ctx, false);
				else
					goto invalid;
				break;
			case 'l':
				if ((word[1] == 0) || !strcmp(word + 1, "ist"))
					uci_parse_option(ctx, true);
				else
					goto invalid;
				break;
			default:
				goto invalid;
		}
		continue;
invalid:
		uci_parse_error(ctx, "invalid command");
	} while (1);
}

/* max number of characters that escaping adds to the string */
#define UCI_QUOTE_ESCAPE	"'\\''"

/*
 * escape an uci string for export
 */
static const char *uci_escape(struct uci_context *ctx, const char *str)
{
	const char *end;
	int ofs = 0;

	if (!ctx->buf) {
		ctx->bufsz = LINEBUF;
		ctx->buf = malloc(LINEBUF);

		if (!ctx->buf)
			return str;
	}

	while (1) {
		int len;

		end = strchr(str, '\'');
		if (!end)
			end = str + strlen(str);
		len = end - str;

		/* make sure that we have enough room in the buffer */
		while (ofs + len + (int) sizeof(UCI_QUOTE_ESCAPE) + 1 > ctx->bufsz) {
			ctx->bufsz *= 2;
			ctx->buf = uci_realloc(ctx, ctx->buf, ctx->bufsz);
		}

		/* copy the string until the character before the quote */
		memcpy(&ctx->buf[ofs], str, len);
		ofs += len;

		/* end of string? return the buffer */
		if (*end == 0)
			break;

		memcpy(&ctx->buf[ofs], UCI_QUOTE_ESCAPE, sizeof(UCI_QUOTE_ESCAPE));
		ofs += strlen(&ctx->buf[ofs]);
		str = end + 1;
	}

	ctx->buf[ofs] = 0;
	return ctx->buf;
}

/*
 * export a single config package to a file stream
 */
static void uci_export_package(struct uci_package *p, FILE *stream, bool header)
{
	struct uci_context *ctx = p->ctx;
	struct uci_element *s, *o, *i;

	if (header)
		fprintf(stream, "package %s\n", uci_escape(ctx, p->e.name));
	uci_foreach_element(&p->sections, s) {
		struct uci_section *sec = uci_to_section(s);
		fprintf(stream, "\nconfig %s", uci_escape(ctx, sec->type));
		if (!sec->anonymous || (ctx->flags & UCI_FLAG_EXPORT_NAME))
			fprintf(stream, " '%s'", uci_escape(ctx, sec->e.name));
		fprintf(stream, "\n");
		uci_foreach_element(&sec->options, o) {
			struct uci_option *opt = uci_to_option(o);
			switch(opt->type) {
			case UCI_TYPE_STRING:
				fprintf(stream, "\toption %s", uci_escape(ctx, opt->e.name));
				fprintf(stream, " '%s'\n", uci_escape(ctx, opt->v.string));
				break;
			case UCI_TYPE_LIST:
				uci_foreach_element(&opt->v.list, i) {
					fprintf(stream, "\tlist %s", uci_escape(ctx, opt->e.name));
					fprintf(stream, " '%s'\n", uci_escape(ctx, i->name));
				}
				break;
			default:
				fprintf(stream, "\t# unknown type for option '%s'\n", uci_escape(ctx, opt->e.name));
				break;
			}
		}
	}
	fprintf(stream, "\n");
}

int uci_export(struct uci_context *ctx, FILE *stream, struct uci_package *package, bool header)
{
	struct uci_element *e;

	UCI_HANDLE_ERR(ctx);
	UCI_ASSERT(ctx, stream != NULL);

	if (package)
		uci_export_package(package, stream, header);
	else {
		uci_foreach_element(&ctx->root, e) {
			uci_export_package(uci_to_package(e), stream, header);
		}
	}

	return 0;
}

int uci_import(struct uci_context *ctx, FILE *stream, const char *name, struct uci_package **package, bool single)
{
	struct uci_parse_context *pctx;
	UCI_HANDLE_ERR(ctx);

	/* make sure no memory from previous parse attempts is leaked */
	uci_cleanup(ctx);

	uci_alloc_parse_context(ctx);
	pctx = ctx->pctx;
	pctx->file = stream;
	if (package && *package && single) {
		pctx->package = *package;
		pctx->merge = true;
	}

	/*
	 * If 'name' was supplied, assume that the supplied stream does not contain
	 * the appropriate 'package <name>' string to specify the config name
	 * NB: the config file can still override the package name
	 */
	if (name) {
		UCI_ASSERT(ctx, uci_validate_package(name));
		pctx->name = name;
	}

	while (!feof(pctx->file)) {
		pctx->pos = 0;
		uci_getln(ctx, 0);
		UCI_TRAP_SAVE(ctx, error);
		if (pctx->buf[0])
			uci_parse_line(ctx, single);
		UCI_TRAP_RESTORE(ctx);
		continue;
error:
		if (ctx->flags & UCI_FLAG_PERROR)
			uci_perror(ctx, NULL);
		if ((ctx->err != UCI_ERR_PARSE) ||
			(ctx->flags & UCI_FLAG_STRICT))
			UCI_THROW(ctx, ctx->err);
	}

	if (!pctx->package && name)
		uci_switch_config(ctx);
	if (package)
		*package = pctx->package;
	if (pctx->merge)
		pctx->package = NULL;

	pctx->name = NULL;
	uci_switch_config(ctx);

	/* no error happened, we can get rid of the parser context now */
	uci_cleanup(ctx);

	return 0;
}


static char *uci_config_path(struct uci_context *ctx, const char *name)
{
	char *filename;

	UCI_ASSERT(ctx, uci_validate_package(name));
	filename = uci_malloc(ctx, strlen(name) + strlen(ctx->confdir) + 2);
	sprintf(filename, "%s/%s", ctx->confdir, name);

	return filename;
}

static void uci_file_commit(struct uci_context *ctx, struct uci_package **package, bool overwrite)
{
	struct uci_package *p = *package;
	FILE *f1, *f2 = NULL;
	char *volatile name = NULL;
	char *volatile path = NULL;
	char *filename = NULL;
	struct stat statbuf;
	volatile bool do_rename = false;
	int fd, sz;

	if (!p->path) {
		if (overwrite)
			p->path = uci_config_path(ctx, p->e.name);
		else
			UCI_THROW(ctx, UCI_ERR_INVAL);
	}

	sz = snprintf(NULL, 0, "%s/.%s.uci-XXXXXX", ctx->confdir, p->e.name);
	filename = alloca(sz + 1);
	snprintf(filename, sz + 1, "%s/.%s.uci-XXXXXX", ctx->confdir, p->e.name);

	/* open the config file for writing now, so that it is locked */
	f1 = uci_open_stream(ctx, p->path, NULL, SEEK_SET, true, true);

	/* flush unsaved changes and reload from delta file */
	UCI_TRAP_SAVE(ctx, done);
	if (p->has_delta) {
		if (!overwrite) {
			name = uci_strdup(ctx, p->e.name);
			path = uci_strdup(ctx, p->path);
			/* dump our own changes to the delta file */
			if (!uci_list_empty(&p->delta))
				UCI_INTERNAL(uci_save, ctx, p);

			/*
			 * other processes might have modified the config
			 * as well. dump and reload
			 */
			uci_free_package(&p);
			uci_cleanup(ctx);
			UCI_INTERNAL(uci_import, ctx, f1, name, &p, true);

			p->path = path;
			p->has_delta = true;
			*package = p;

			/* freed together with the uci_package */
			path = NULL;
		}

		/* flush delta */
		if (!uci_load_delta(ctx, p, true))
			goto done;
	}

	fd = mkstemp(filename);
	if (fd == -1)
		UCI_THROW(ctx, UCI_ERR_IO);

	if ((flock(fd, LOCK_EX) < 0) && (errno != ENOSYS))
		UCI_THROW(ctx, UCI_ERR_IO);

	if (lseek(fd, 0, SEEK_SET) < 0)
		UCI_THROW(ctx, UCI_ERR_IO);

	f2 = fdopen(fd, "w+");
	if (!f2)
		UCI_THROW(ctx, UCI_ERR_IO);

	uci_export(ctx, f2, p, false);

	fflush(f2);
	fsync(fileno(f2));
	uci_close_stream(f2);

	do_rename = true;

	UCI_TRAP_RESTORE(ctx);

done:
	free(name);
	free(path);
	uci_close_stream(f1);
	if (do_rename) {
		path = realpath(p->path, NULL);
		if (!path || stat(path, &statbuf) || chmod(filename, statbuf.st_mode) || rename(filename, path)) {
			unlink(filename);
			UCI_THROW(ctx, UCI_ERR_IO);
		}
		free(path);
	}
	if (ctx->err)
		UCI_THROW(ctx, ctx->err);
}


/*
 * This function returns the filename by returning the string
 * after the last '/' character. By checking for a non-'\0'
 * character afterwards, directories are ignored (glob marks
 * those with a trailing '/'
 */
static inline char *get_filename(char *path)
{
	char *p;

	p = strrchr(path, '/');
	p++;
	if (!*p)
		return NULL;
	return p;
}

static char **uci_list_config_files(struct uci_context *ctx)
{
	char **configs;
	glob_t globbuf;
	int size, j, skipped;
	size_t i;
	char *buf;
	char *dir;

	dir = uci_malloc(ctx, strlen(ctx->confdir) + 1 + sizeof("/*"));
	sprintf(dir, "%s/*", ctx->confdir);
	if (glob(dir, GLOB_MARK, NULL, &globbuf) != 0) {
		free(dir);
		UCI_THROW(ctx, UCI_ERR_NOTFOUND);
	}

	size = sizeof(char *) * (globbuf.gl_pathc + 1);
	skipped = 0;
	for(i = 0; i < globbuf.gl_pathc; i++) {
		char *p;

		p = get_filename(globbuf.gl_pathv[i]);
		if (!p) {
			skipped++;
			continue;
		}

		size += strlen(p) + 1;
	}

	configs = uci_malloc(ctx, size - skipped);
	buf = (char *) &configs[globbuf.gl_pathc + 1 - skipped];
	j = 0;
	for(i = 0; i < globbuf.gl_pathc; i++) {
		char *p;

		p = get_filename(globbuf.gl_pathv[i]);
		if (!p)
			continue;

		if (!uci_validate_package(p))
			continue;

		configs[j++] = buf;
		strcpy(buf, p);
		buf += strlen(buf) + 1;
	}
	free(dir);
	globfree(&globbuf);
	return configs;
}

static struct uci_package *uci_file_load(struct uci_context *ctx,
					 const char *volatile name)
{
	struct uci_package *package = NULL;
	char *filename;
	bool confdir;
	FILE *volatile file = NULL;

	switch (name[0]) {
	case '.':
		/* relative path outside of /etc/config */
		if (name[1] != '/')
			UCI_THROW(ctx, UCI_ERR_NOTFOUND);
		/* fall through */
	case '/':
		/* absolute path outside of /etc/config */
		filename = uci_strdup(ctx, name);
		name = strrchr(name, '/') + 1;
		confdir = false;
		break;
	default:
		/* config in /etc/config */
		filename = uci_config_path(ctx, name);
		confdir = true;
		break;
	}

	UCI_TRAP_SAVE(ctx, done);
	file = uci_open_stream(ctx, filename, NULL, SEEK_SET, false, false);
	ctx->err = 0;
	UCI_INTERNAL(uci_import, ctx, file, name, &package, true);
	UCI_TRAP_RESTORE(ctx);

	if (package) {
		package->path = filename;
		package->has_delta = confdir;
		uci_load_delta(ctx, package, false);
	}

done:
	uci_close_stream(file);
	if (ctx->err) {
		free(filename);
		UCI_THROW(ctx, ctx->err);
	}
	return package;
}

__private UCI_BACKEND(uci_file_backend, "file",
	.load = uci_file_load,
	.commit = uci_file_commit,
	.list_configs = uci_list_config_files,
);
