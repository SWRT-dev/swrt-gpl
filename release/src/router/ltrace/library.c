/*
 * This file is part of ltrace.
 * Copyright (C) 2011,2012,2013,2014 Petr Machata, Red Hat Inc.
 * Copyright (C) 2001,2009 Juan Cespedes
 * Copyright (C) 2006 Ian Wienand
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#include "config.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "library.h"
#include "callback.h"
#include "debug.h"
#include "dict.h"
#include "vect.h"
#include "backend.h" // for arch_library_symbol_init, arch_library_init

static void
library_exported_names_init(struct library_exported_names *names);

#ifndef OS_HAVE_LIBRARY_DATA
int
os_library_init(struct library *lib)
{
	return 0;
}

void
os_library_destroy(struct library *lib)
{
}

int
os_library_clone(struct library *retp, struct library *lib)
{
	return 0;
}
#endif

#ifndef ARCH_HAVE_LIBRARY_DATA
int
arch_library_init(struct library *lib)
{
	return 0;
}

void
arch_library_destroy(struct library *lib)
{
}

int
arch_library_clone(struct library *retp, struct library *lib)
{
	return 0;
}
#endif

#ifndef OS_HAVE_LIBRARY_SYMBOL_DATA
int
os_library_symbol_init(struct library_symbol *libsym)
{
	return 0;
}

void
os_library_symbol_destroy(struct library_symbol *libsym)
{
}

int
os_library_symbol_clone(struct library_symbol *retp,
			struct library_symbol *libsym)
{
	return 0;
}
#endif

#ifndef ARCH_HAVE_LIBRARY_SYMBOL_DATA
int
arch_library_symbol_init(struct library_symbol *libsym)
{
	return 0;
}

void
arch_library_symbol_destroy(struct library_symbol *libsym)
{
}

int
arch_library_symbol_clone(struct library_symbol *retp,
			  struct library_symbol *libsym)
{
	return 0;
}
#endif

size_t
arch_addr_hash(const arch_addr_t *addr)
{
	union {
		arch_addr_t addr;
		int ints[sizeof(arch_addr_t)
			 / sizeof(unsigned int)];
	} u = { .addr = *addr };

	size_t i;
	size_t h = 0;
	for (i = 0; i < sizeof(u.ints) / sizeof(*u.ints); ++i)
		h ^= dict_hash_int(&u.ints[i]);
	return h;
}

int
arch_addr_eq(const arch_addr_t *addr1, const arch_addr_t *addr2)
{
	return *addr1 == *addr2;
}

int
strdup_if(const char **retp, const char *str, int whether)
{
	if (whether && str != NULL) {
		str = strdup(str);
		if (str == NULL)
			return -1;
	}

	*retp = str;
	return 0;
}

static void
private_library_symbol_init(struct library_symbol *libsym,
			    arch_addr_t addr,
			    const char *name, int own_name,
			    enum toplt type_of_plt,
			    int latent, int delayed)
{
	libsym->next = NULL;
	libsym->lib = NULL;
	libsym->plt_type = type_of_plt;
	libsym->name = name;
	libsym->own_name = own_name;
	libsym->latent = latent;
	libsym->delayed = delayed;
	libsym->enter_addr = (void *)(uintptr_t)addr;
	libsym->proto = NULL;
}

static void
private_library_symbol_destroy(struct library_symbol *libsym)
{
	library_symbol_set_name(libsym, NULL, 0);
}

int
library_symbol_init(struct library_symbol *libsym,
		    arch_addr_t addr, const char *name, int own_name,
		    enum toplt type_of_plt)
{
	private_library_symbol_init(libsym, addr, name, own_name,
				    type_of_plt, 0, 0);

	if (os_library_symbol_init(libsym) < 0)
		/* We've already set libsym->name and own_name.  But
		 * we return failure, and the client code isn't
		 * supposed to call library_symbol_destroy in such
		 * case.  */
		return -1;

	if (arch_library_symbol_init(libsym) < 0) {
		os_library_symbol_destroy(libsym);
		return -1;
	}

	return 0;
}

void
library_symbol_destroy(struct library_symbol *libsym)
{
	if (libsym != NULL) {
		arch_library_symbol_destroy(libsym);
		os_library_symbol_destroy(libsym);
		private_library_symbol_destroy(libsym);
	}
}

int
library_symbol_clone(struct library_symbol *retp, struct library_symbol *libsym)
{
	/* Make lifetimes of name stored at original independent of
	 * the one at the clone.  */
	const char *name;
	if (strdup_if(&name, libsym->name, libsym->own_name) < 0)
		return -1;

	private_library_symbol_init(retp, libsym->enter_addr,
				    name, libsym->own_name, libsym->plt_type,
				    libsym->latent, libsym->delayed);

	if (os_library_symbol_clone(retp, libsym) < 0) {
	fail:
		private_library_symbol_destroy(retp);
		return -1;
	}

	if (arch_library_symbol_clone(retp, libsym) < 0) {
		os_library_symbol_destroy(retp);
		goto fail;
	}

	return 0;
}

int
library_symbol_cmp(struct library_symbol *a, struct library_symbol *b)
{
	if (a->enter_addr < b->enter_addr)
		return -1;
	if (a->enter_addr > b->enter_addr)
		return 1;
	if (a->name != NULL && b->name != NULL)
		return strcmp(a->name, b->name);
	if (a->name == NULL) {
		if (b->name == NULL)
			return 0;
		return -1;
	}
	return 1;
}

void
library_symbol_set_name(struct library_symbol *libsym,
			const char *name, int own_name)
{
	if (libsym->own_name)
		free((char *)libsym->name);
	libsym->name = name;
	libsym->own_name = own_name;
}

enum callback_status
library_symbol_equal_cb(struct library_symbol *libsym, void *u)
{
	struct library_symbol *standard = u;
	return library_symbol_cmp(libsym, standard) == 0 ? CBS_STOP : CBS_CONT;
}

enum callback_status
library_symbol_named_cb(struct library_symbol *libsym, void *name)
{
	return strcmp(libsym->name, name) == 0 ? CBS_STOP : CBS_CONT;
}

enum callback_status
library_symbol_delayed_cb(struct library_symbol *libsym, void *unused)
{
	return libsym->delayed ? CBS_STOP : CBS_CONT;
}

static void
private_library_init(struct library *lib, enum library_type type)
{
	lib->next = NULL;

	lib->key = 0;
	lib->base = 0;
	lib->entry = 0;
	lib->dyn_addr = 0;
	lib->protolib = NULL;

	lib->soname = NULL;
	lib->own_soname = 0;

	lib->pathname = NULL;
	lib->own_pathname = 0;

	lib->symbols = NULL;
	library_exported_names_init(&lib->exported_names);
	lib->should_activate_latent = false;
	lib->type = type;

#if defined(HAVE_LIBDW)
	lib->dwfl_module = NULL;
#endif
}

int
library_init(struct library *lib, enum library_type type)
{
	private_library_init(lib, type);

	if (os_library_init(lib) < 0)
		return -1;

	if (arch_library_init(lib) < 0) {
		os_library_destroy(lib);
		return -1;
	}

	return 0;
}





static void dtor_string(const char **tgt, void *data)
{
	free((char*)*tgt);
}
static int clone_vect(struct vect **to, const struct vect **from, void *data)
{
	*to = malloc(sizeof(struct vect));
	if (*to == NULL)
		return -1;

	return
		VECT_CLONE(*to, *from, const char*,
			   dict_clone_string,
			   dtor_string,
			   NULL);
}
static void dtor_vect(struct vect **tgt, void *data)
{
	VECT_DESTROY(*tgt, const char*, dtor_string, NULL);
	free(*tgt);
}

static void
library_exported_names_init(struct library_exported_names *names)
{
	DICT_INIT(&names->names,
		  const char*, uint64_t,
		  dict_hash_string, dict_eq_string, NULL);
	DICT_INIT(&names->addrs,
		  uint64_t, struct vect*,
		  dict_hash_uint64, dict_eq_uint64, NULL);
}

static void
library_exported_names_destroy(struct library_exported_names *names)
{
	DICT_DESTROY(&names->names,
		     const char*, uint64_t,
		     dtor_string, NULL, NULL);
	DICT_DESTROY(&names->addrs,
		     uint64_t, struct vect*,
		     NULL, dtor_vect, NULL);
}

static int
library_exported_names_clone(struct library_exported_names *retp,
			     const struct library_exported_names *names)
{
	return (DICT_CLONE(&retp->names, &names->names,
			   const char*, uint64_t,
			   dict_clone_string, dtor_string,
			   NULL, NULL,
			   NULL) < 0  ||
		DICT_CLONE(&retp->addrs, &names->addrs,
			   uint64_t, struct vect*,
			   NULL, NULL,
			   clone_vect, dtor_vect,
			   NULL) < 0) ? -1 : 0;
}

int library_exported_names_push(struct library_exported_names *names,
				uint64_t addr, char *name,
				int own_name )
{
	// first, take ownership of the name, if it's not yet ours
	if (!own_name)
		name = strdup(name);
	if (name == NULL)
		return -1;

	// push to the name->addr map
	int result = DICT_INSERT(&names->names, &name, &addr);
	if (result > 0) {
		// This symbol is already present in the table. This library has
		// multiple copies of this symbol (probably corresponding to
		// different symbol versions). I should handle this gracefully
		// at some point, but for now I simply ignore later instances of
		// any particular symbol
		free(name);
		return 0;
	}

	if (result != 0)
		return result;

	// push to the addr->names map
	// I get the alias vector. If it doesn't yet exist, I make it
	struct vect *aliases;
	struct vect **paliases = DICT_FIND_REF(&names->addrs,
					       &addr, struct vect*);

	if (paliases == NULL) {
		aliases = malloc(sizeof(struct vect));
		if (aliases == NULL)
			return -1;
		VECT_INIT(aliases, const char*);
		result = DICT_INSERT(&names->addrs, &addr, &aliases);
		assert(result <= 0);
		if (result < 0)
			return result;
	}
	else
		aliases = *paliases;

	char *namedup = strdup(name);
	if (namedup == NULL)
		return -1;

	result = vect_pushback(aliases, &namedup);
	if (result != 0) {
		free(namedup);
		return result;
	}

	return 0;
}

struct library_exported_names_each_alias_context
{
	enum callback_status (*inner_cb)(const char *, void *);
	const char *origname;
	void *data;
};
static enum callback_status
library_exported_names_each_alias_cb(const char **name, void *data)
{
	struct library_exported_names_each_alias_context *context =
		(struct library_exported_names_each_alias_context*)data;

	// I do not report the original name we were asked about. Otherwise, any
	// time the caller asks for aliases of symbol "sym", I'll always report
	// "sym" along with any actual aliases
	if (strcmp(*name, context->origname) == 0)
		return CBS_CONT;

	return context->inner_cb(*name, context->data);
}

const char** library_exported_names_each_alias(
	struct library_exported_names *names,
	const char *aliasname,
	const char **name_start_after,
	enum callback_status (*cb)(const char *,
				   void *),
	void *data)
{
	// I have a symbol name. I look up its address, then get the list of
	// aliased names
	uint64_t *addr = DICT_FIND_REF(&names->names,
				       &aliasname, uint64_t);
	if (addr == NULL)
		return NULL;

	// OK. I have an address. Get the list of symbols at this address
	struct vect **aliases = DICT_FIND_REF(&names->addrs,
					      addr, struct vect*);
	assert(aliases != NULL);

	struct library_exported_names_each_alias_context context =
		{.inner_cb = cb,
		 .origname = aliasname,
		 .data = data};
	return VECT_EACH(*aliases, const char*, name_start_after,
			 library_exported_names_each_alias_cb, &context);
}

int library_exported_names_contains(struct library_exported_names *names,
				    const char *queryname)
{
	uint64_t *addr = DICT_FIND_REF(&names->names,
				       &queryname, uint64_t);
	return (addr == NULL) ? 0 : 1;
}





int
library_clone(struct library *retp, struct library *lib)
{
	const char *soname = NULL;
	const char *pathname;

	/* Make lifetimes of strings stored at original independent of
	 * those at the clone.  */
	if (strdup_if(&soname, lib->soname, lib->own_soname) < 0
	    || strdup_if(&pathname, lib->pathname, lib->own_pathname) < 0) {
		if (lib->own_soname)
			free((char *)soname);
		return -1;
	}

	private_library_init(retp, lib->type);
	library_set_soname(retp, soname, lib->own_soname);
	library_set_pathname(retp, pathname, lib->own_pathname);

	retp->key = lib->key;
	retp->should_activate_latent = lib->should_activate_latent;

	/* Clone symbols.  */
	{
		struct library_symbol *it;
		struct library_symbol **nsymp = &retp->symbols;
		for (it = lib->symbols; it != NULL; it = it->next) {
			*nsymp = malloc(sizeof(**nsymp));
			if (*nsymp == NULL
			    || library_symbol_clone(*nsymp, it) < 0) {
				free(*nsymp);
				*nsymp = NULL;
			fail:
				/* Release what we managed to allocate.  */
				library_destroy(retp);
				return -1;
			}

			(*nsymp)->lib = retp;
			nsymp = &(*nsymp)->next;
		}
		*nsymp = NULL;
	}

	/* Clone exported names.  */
	if (library_exported_names_clone(&retp->exported_names,
					 &lib->exported_names) != 0)
		goto fail;

	if (os_library_clone(retp, lib) < 0)
		goto fail;

	if (arch_library_clone(retp, lib) < 0) {
		os_library_destroy(retp);
		goto fail;
	}

#if defined(HAVE_LIBDW)
	/* Wipe DWFL_MODULE, leave it to proc_add_library to
	 * initialize.  */
	lib->dwfl_module = NULL;
#endif

	return 0;
}

void
library_destroy(struct library *lib)
{
	if (lib == NULL)
		return;

	arch_library_destroy(lib);
	os_library_destroy(lib);

	library_set_soname(lib, NULL, 0);
	library_set_pathname(lib, NULL, 0);

	struct library_symbol *sym;
	for (sym = lib->symbols; sym != NULL; ) {
		struct library_symbol *next = sym->next;
		library_symbol_destroy(sym);
		free(sym);
		sym = next;
	}

	/* Release exported names.  */
	library_exported_names_destroy(&lib->exported_names);
}

void
library_set_soname(struct library *lib, const char *new_name, int own_name)
{
	if (lib->own_soname)
		free((char *)lib->soname);
	lib->soname = new_name;
	lib->own_soname = own_name;
}

void
library_set_pathname(struct library *lib, const char *new_name, int own_name)
{
	if (lib->own_pathname)
		free((char *)lib->pathname);
	lib->pathname = new_name;
	lib->own_pathname = own_name;
}

struct library_symbol *
library_each_symbol(struct library *lib, struct library_symbol *start_after,
		    enum callback_status (*cb)(struct library_symbol *, void *),
		    void *data)
{
	struct library_symbol *it = start_after == NULL ? lib->symbols
		: start_after->next;

	while (it != NULL) {
		struct library_symbol *next = it->next;

		switch ((*cb)(it, data)) {
		case CBS_FAIL:
			/* XXX handle me  */
		case CBS_STOP:
			return it;
		case CBS_CONT:
			break;
		}

		it = next;
	}

	return NULL;
}

void
library_add_symbol(struct library *lib, struct library_symbol *first)
{
	struct library_symbol *last;
	for (last = first; last != NULL; ) {
		last->lib = lib;
		if (last->next != NULL)
			last = last->next;
		else
			break;
	}

	assert(last->next == NULL);
	last->next = lib->symbols;
	lib->symbols = first;
}

enum callback_status
library_named_cb(struct process *proc, struct library *lib, void *name)
{
	if (name == lib->soname
	    || strcmp(lib->soname, (char *)name) == 0)
		return CBS_STOP;
	else
		return CBS_CONT;
}

enum callback_status
library_with_key_cb(struct process *proc, struct library *lib, void *keyp)
{
	return lib->key == *(arch_addr_t *)keyp ? CBS_STOP : CBS_CONT;
}
