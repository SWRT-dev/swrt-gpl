/*
 * This file is part of ltrace.
 * Copyright (C) 2012,2013 Petr Machata, Red Hat Inc.
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

#include <alloca.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "common.h"
#include "callback.h"
#include "param.h"
#include "prototype.h"
#include "type.h"
#include "options.h"
#include "read_config_file.h"
#include "backend.h"

struct protolib_cache g_protocache;
static struct protolib legacy_typedefs;

void
prototype_init(struct prototype *proto)
{
	VECT_INIT(&proto->params, struct param);

	proto->return_info = NULL;
	proto->own_return_info = 0;
}

static void
param_destroy_cb(struct param *param, void *data)
{
	param_destroy(param);
}

void
prototype_destroy(struct prototype *proto)
{
	if (proto == NULL)
		return;
	if (proto->own_return_info) {
		type_destroy(proto->return_info);
		free(proto->return_info);
	}

	VECT_DESTROY(&proto->params, struct param, &param_destroy_cb, NULL);
}

int
prototype_push_param(struct prototype *proto, struct param *param)
{
	return VECT_PUSHBACK(&proto->params, param);
}

size_t
prototype_num_params(struct prototype *proto)
{
	return vect_size(&proto->params);
}

void
prototype_destroy_nth_param(struct prototype *proto, size_t n)
{
	assert(n < prototype_num_params(proto));
	VECT_ERASE(&proto->params, struct param, n, n+1,
		   &param_destroy_cb, NULL);
}

struct param *
prototype_get_nth_param(struct prototype *proto, size_t n)
{
	assert(n < prototype_num_params(proto));
	return VECT_ELEMENT(&proto->params, struct param, n);
}

struct each_param_data {
	struct prototype *proto;
	enum callback_status (*cb)(struct prototype *, struct param *, void *);
	void *data;
};

static enum callback_status
each_param_cb(struct param *param, void *data)
{
	struct each_param_data *cb_data = data;
	return (cb_data->cb)(cb_data->proto, param, cb_data->data);
}

struct param *
prototype_each_param(struct prototype *proto, struct param *start_after,
		     enum callback_status (*cb)(struct prototype *,
						struct param *, void *),
		     void *data)
{
	struct each_param_data cb_data = { proto, cb, data };
	return VECT_EACH(&proto->params, struct param, start_after,
			 &each_param_cb, &cb_data);
}

void
named_type_init(struct named_type *named,
		struct arg_type_info *info, int own_type)
{
	named->info = info;
	named->own_type = own_type;
	named->forward = 0;
}

void
named_type_destroy(struct named_type *named)
{
	if (named->own_type) {
		type_destroy(named->info);
		free(named->info);
	}
}

void
protolib_init(struct protolib *plib)
{
	DICT_INIT(&plib->prototypes, char *, struct prototype,
		  dict_hash_string, dict_eq_string, NULL);

	DICT_INIT(&plib->named_types, char *, struct named_type,
		  dict_hash_string, dict_eq_string, NULL);

	VECT_INIT(&plib->imports, struct protolib *);

	plib->refs = 0;
}

static void
destroy_prototype_cb(struct prototype *proto, void *data)
{
	prototype_destroy(proto);
}

static void
destroy_named_type_cb(struct named_type *named, void *data)
{
	named_type_destroy(named);
}

void
protolib_destroy(struct protolib *plib)
{
	assert(plib->refs == 0);

	VECT_DESTROY(&plib->imports, struct prototype *, NULL, NULL);

	DICT_DESTROY(&plib->prototypes, const char *, struct prototype,
		     dict_dtor_string, destroy_prototype_cb, NULL);

	DICT_DESTROY(&plib->named_types, const char *, struct named_type,
		     dict_dtor_string, destroy_named_type_cb, NULL);
}

static struct protolib **
each_import(struct protolib *plib, struct protolib **start_after,
	    enum callback_status (*cb)(struct protolib **, void *), void *data)
{
	assert(plib != NULL);
	return VECT_EACH(&plib->imports, struct protolib *,
			 start_after, cb, data);
}

static enum callback_status
is_or_imports(struct protolib **plibp, void *data)
{
	assert(plibp != NULL);
	assert(*plibp != NULL);
	struct protolib *import = data;
	if (*plibp == import
	    || each_import(*plibp, NULL, &is_or_imports, import) != NULL)
		return CBS_STOP;
	else
		return CBS_CONT;
}

int
protolib_add_import(struct protolib *plib, struct protolib *import)
{
	assert(plib != NULL);
	assert(import != NULL);
	if (is_or_imports(&import, plib) == CBS_STOP) {
		fprintf(stderr, "Recursive import rejected.\n");
		return -2;
	}

	return VECT_PUSHBACK(&plib->imports, &import) < 0 ? -1 : 0;
}

static int
bailout(const char *name, int own)
{
	int save_errno = errno;
	if (own)
		free((char *)name);
	errno = save_errno;
	return -1;
}

int
protolib_add_prototype(struct protolib *plib, const char *name, int own_name,
		       struct prototype *proto)
{
	assert(plib != NULL);
	if (strdup_if(&name, name, !own_name) < 0)
		return -1;
	if (DICT_INSERT(&plib->prototypes, &name, proto) < 0)
		return bailout(name, own_name);
	return 0;
}

int
protolib_add_named_type(struct protolib *plib, const char *name, int own_name,
			struct named_type *named)
{
	assert(plib != NULL);
	if (strdup_if(&name, name, !own_name) < 0)
		return -1;
	if (DICT_INSERT(&plib->named_types, &name, named) < 0)
		return bailout(name, own_name);
	return 0;
}

struct lookup {
	const char *name;
	struct dict *(*getter)(struct protolib *plib);
	bool imports;
	void *result;
};

static struct dict *
get_prototypes(struct protolib *plib)
{
	assert(plib != NULL);
	return &plib->prototypes;
}

static struct dict *
get_named_types(struct protolib *plib)
{
	assert(plib != NULL);
	return &plib->named_types;
}

static enum callback_status
protolib_lookup_rec(struct protolib **plibp, void *data)
{
	assert(plibp != NULL);
	assert(*plibp != NULL);
	struct lookup *lookup = data;
	struct dict *dict = (*lookup->getter)(*plibp);

	lookup->result = dict_find(dict, &lookup->name);
	if (lookup->result != NULL)
		return CBS_STOP;

	if (lookup->imports && each_import(*plibp, NULL, &protolib_lookup_rec,
					   lookup) != NULL) {
		assert(lookup->result != NULL);
		return CBS_STOP;
	}

	return CBS_CONT;
}

static void *
protolib_lookup(struct protolib *plib, const char *name,
		struct dict *(*getter)(struct protolib *),
		bool imports)
{
	assert(plib != NULL);
	struct lookup lookup = { name, getter, imports, NULL };
	if (protolib_lookup_rec(&plib, &lookup) == CBS_STOP)
		assert(lookup.result != NULL);
	else
		assert(lookup.result == NULL);
	return lookup.result;
}

struct prototype *
protolib_lookup_prototype(struct protolib *plib, const char *name, bool imports)
{
	assert(plib != NULL);
	return protolib_lookup(plib, name, &get_prototypes, imports);
}

struct named_type *
protolib_lookup_type(struct protolib *plib, const char *name, bool imports)
{
	assert(plib != NULL);
	return protolib_lookup(plib, name, &get_named_types, imports);
}

static void
destroy_protolib_cb(struct protolib **plibp, void *data)
{
	assert(plibp != NULL);

	if (*plibp != NULL
	    && --(*plibp)->refs == 0) {
		protolib_destroy(*plibp);
		free(*plibp);
	}
}

void
protolib_cache_destroy(struct protolib_cache *cache)
{
	DICT_DESTROY(&cache->protolibs, const char *, struct protolib *,
		     dict_dtor_string, destroy_protolib_cb, NULL);
}

struct load_config_data {
	struct protolib_cache *self;
	const char *key;
	struct protolib *result;
};

static struct protolib *
consider_config_dir(struct protolib_cache *cache,
		    const char *path, const char *key)
{
	size_t len = sizeof ".conf";
	char *buf = alloca(strlen(path) + 1 + strlen(key) + len);
	sprintf(buf, "%s/%s.conf", path, key);

	return protolib_cache_file(cache, buf, 0);
}

static enum callback_status
consider_confdir_cb(struct opt_F_t *entry, void *d)
{
	if (opt_F_get_kind(entry) != OPT_F_DIR)
		return CBS_CONT;
	struct load_config_data *data = d;

	data->result = consider_config_dir(data->self,
					   entry->pathname, data->key);
	return data->result != NULL ? CBS_STOP : CBS_CONT;
}

static int
load_dash_F_dirs(struct protolib_cache *cache,
		 const char *key, struct protolib **retp)
{
	struct load_config_data data = {cache, key};

	if (VECT_EACH(&opt_F, struct opt_F_t, NULL,
		      consider_confdir_cb, &data) == NULL)
		/* Not found.  That's fine.  */
		return 0;

	if (data.result == NULL)
		/* There were errors.  */
		return -1;

	*retp = data.result;
	return 0;
}

static int
load_config(struct protolib_cache *cache,
	    const char *key, int private, struct protolib **retp)
{
	const char **dirs = NULL;
	if (os_get_config_dirs(private, &dirs) < 0
	    || dirs == NULL)
		return -1;

	for (; *dirs != NULL; ++dirs) {
		struct protolib *plib = consider_config_dir(cache, *dirs, key);
		if (plib != NULL) {
			*retp = plib;
			break;
		}
	}

	return 0;
}

static enum callback_status
import_legacy_file(char **fnp, void *data)
{
	struct protolib_cache *cache = data;
	struct protolib *plib = protolib_cache_file(cache, *fnp, 1);
	if (plib != NULL) {
		/* The cache now owns the file name.  */
		*fnp = NULL;
		if (protolib_add_import(&cache->imports, plib) < 0)
			return CBS_STOP;
	}

	return CBS_CONT;
}

static int
add_ltrace_conf(struct protolib_cache *cache)
{
	/* Look into private config directories for .ltrace.conf and
	 * into system config directories for ltrace.conf.  If it's
	 * found, add it to implicit import module.  */
	struct vect legacy_files;
	VECT_INIT(&legacy_files, char *);
	if (os_get_ltrace_conf_filenames(&legacy_files) < 0) {
		vect_destroy(&legacy_files, NULL, NULL);
		return -1;
	}

	int ret = VECT_EACH(&legacy_files, char *, NULL,
			    import_legacy_file, cache) == NULL ? 0 : -1;
	VECT_DESTROY(&legacy_files, char *, vect_dtor_string, NULL);
	return ret;
}

static enum callback_status
add_imports_cb(struct opt_F_t *entry, void *data)
{
	struct protolib_cache *self = data;
	if (opt_F_get_kind(entry) != OPT_F_FILE)
		return CBS_CONT;

	struct protolib *new_import
		= protolib_cache_file(self, entry->pathname, 0);

	if (new_import == NULL
	    || protolib_add_import(&self->imports, new_import) < 0)
		/* N.B. If new_import is non-NULL, it has been already
		 * cached.  We don't therefore destroy it on
		 * failures.  */
		return CBS_STOP;

	return CBS_CONT;
}

int
protolib_cache_init(struct protolib_cache *cache, struct protolib *import)
{
	DICT_INIT(&cache->protolibs, char *, struct protolib *,
		  dict_hash_string, dict_eq_string, NULL);
	protolib_init(&cache->imports);

	/* At this point the cache is consistent.  This is important,
	 * because next we will use it to cache files that we load
	 * due to -F.
	 *
	 * But we are about to construct the implicit import module,
	 * which means this module can't be itself imported to the
	 * files that we load now.  So remember that we are still
	 * bootstrapping.  */
	cache->bootstrap = 1;

	if (protolib_add_import(&cache->imports, &legacy_typedefs) < 0
	    || (import != NULL
		&& protolib_add_import(&cache->imports, import) < 0)
	    || add_ltrace_conf(cache) < 0
	    || VECT_EACH(&opt_F, struct opt_F_t, NULL,
			 add_imports_cb, cache) != NULL) {
		protolib_cache_destroy(cache);
		return -1;
	}

	cache->bootstrap = 0;
	return 0;
}

static enum callback_status
add_import_cb(struct protolib **importp, void *data)
{
	struct protolib *plib = data;
	if (protolib_add_import(plib, *importp) < 0)
		return CBS_STOP;
	else
		return CBS_CONT;
}

static struct protolib *
build_default_config(struct protolib_cache *cache, const char *key)
{
	struct protolib *new_plib = malloc(sizeof(*new_plib));
	if (new_plib == NULL) {
		fprintf(stderr, "Couldn't create config module %s: %s\n",
			key, strerror(errno));
		return NULL;
	}

	protolib_init(new_plib);

	/* If bootstrapping, copy over imports from implicit import
	 * module to new_plib.  We can't reference the implicit
	 * import module itself, because new_plib will become part of
	 * this same implicit import module itself.  */
	if ((cache->bootstrap && each_import(&cache->imports, NULL,
					     add_import_cb, new_plib) != NULL)
	    || (!cache->bootstrap
		&& protolib_add_import(new_plib, &cache->imports) < 0)) {

		fprintf(stderr,
			"Couldn't add imports to config module %s: %s\n",
			key, strerror(errno));
		protolib_destroy(new_plib);
		free(new_plib);
		return NULL;
	}

	return new_plib;
}

static void
attempt_to_cache(struct protolib_cache *cache,
		 const char *key, struct protolib *plib)
{
	if (protolib_cache_protolib(cache, key, 1, plib) == 0
	    || plib == NULL)
		/* Never mind failing to store a NULL.  */
		return;

	/* Returning a protolib that hasn't been cached would leak
	 * that protolib, but perhaps it's less bad then giving up
	 * outright.  At least print an error message.  */
	fprintf(stderr, "Couldn't cache prototype library for %s\n", key);
	free((void *) key);
}

int
protolib_cache_maybe_load(struct protolib_cache *cache,
			  const char *key, int own_key, bool allow_private,
			  struct protolib **retp)
{
	if (DICT_FIND_VAL(&cache->protolibs, &key, retp) == 0) {
		if (*retp != NULL && own_key)
			free((void *) key);
		return 0;
	}

	if (strdup_if(&key, key, !own_key) < 0) {
		fprintf(stderr, "Couldn't cache %s: %s\n",
			key, strerror(errno));
		return -1;
	}

	*retp = NULL;
	if (load_dash_F_dirs(cache, key, retp) < 0
	    || (*retp == NULL && allow_private
		&& load_config(cache, key, 1, retp) < 0)
	    || (*retp == NULL
		&& load_config(cache, key, 0, retp) < 0))
	{
		fprintf(stderr,
			"Error occurred when attempting to load a prototype "
			"library for %s.\n", key);
		if (!own_key)
			free((void *) key);
		return -1;
	}

	if (*retp != NULL)
		attempt_to_cache(cache, key, *retp);
	else if (!own_key)
		free((void *) key);

	return 0;
}

struct protolib *
protolib_cache_load(struct protolib_cache *cache,
		    const char *key, int own_key, bool allow_private)
{
	struct protolib *plib;
	if (protolib_cache_maybe_load(cache, key, own_key,
				      allow_private, &plib) < 0)
		return NULL;

	if (plib == NULL)
		plib = protolib_cache_default(cache, key, own_key);

	return plib;
}

struct protolib *
protolib_cache_default(struct protolib_cache *cache,
		       const char *key, int own_key)
{
	if (strdup_if(&key, key, !own_key) < 0) {
		fprintf(stderr, "Couldn't cache default %s: %s\n",
			key, strerror(errno));
		return NULL;
	}

	struct protolib *plib = build_default_config(cache, key);

	/* Whatever came out of this (even NULL), store it in
	 * the cache.  */
	attempt_to_cache(cache, key, plib);

	return plib;
}

struct protolib *
protolib_cache_file(struct protolib_cache *cache,
		    const char *filename, int own_filename)
{
	{
		struct protolib *plib;
		if (DICT_FIND_VAL(&cache->protolibs, &filename, &plib) == 0)
			return plib;
	}

	FILE *stream = fopen(filename, "r");
	if (stream == NULL)
		return NULL;

	if (strdup_if(&filename, filename, !own_filename) < 0) {
		fprintf(stderr, "Couldn't cache %s: %s\n",
			filename, strerror(errno));
		fclose(stream);
		return NULL;
	}

	struct protolib *new_plib = build_default_config(cache, filename);
	if (new_plib == NULL
	    || read_config_file(stream, filename, cache, new_plib) < 0) {
		fclose(stream);
		if (own_filename)
			free((char *) filename);
		if (new_plib != NULL) {
			protolib_destroy(new_plib);
			free(new_plib);
		}
		return NULL;
	}

	attempt_to_cache(cache, filename, new_plib);
	fclose(stream);
	return new_plib;
}

int
protolib_cache_protolib(struct protolib_cache *cache,
			const char *filename, int own_filename,
			struct protolib *plib)
{
	if (strdup_if(&filename, filename, !own_filename) < 0) {
		fprintf(stderr, "Couldn't cache %s: %s\n",
			filename, strerror(errno));
		return -1;
	}

	int rc = DICT_INSERT(&cache->protolibs, &filename, &plib);
	if (rc < 0 && own_filename)
		free((char *) filename);
	if (rc == 0 && plib != NULL)
		plib->refs++;
	return rc;
}

static void
destroy_global_config(void)
{
	protolib_cache_destroy(&g_protocache);
	protolib_destroy(&legacy_typedefs);
}

void
init_global_config(void)
{
	protolib_init(&legacy_typedefs);

	struct arg_type_info *ptr_info = type_get_voidptr();
	static struct named_type voidptr_type;
	named_type_init(&voidptr_type, ptr_info, 0);

	/* Build legacy typedefs first.  This is used by
	 * protolib_cache_init call below.  */
	if (protolib_add_named_type(&legacy_typedefs, "addr", 0,
				    &voidptr_type) < 0
	    || protolib_add_named_type(&legacy_typedefs, "file", 0,
				       &voidptr_type) < 0) {
		fprintf(stderr,
			"Couldn't initialize aliases `addr' and `file'.\n");

		exit(1);
	}

	if (protolib_cache_init(&g_protocache, NULL) < 0) {
		fprintf(stderr, "Couldn't init prototype cache\n");
		exit(1);
	}

	atexit(destroy_global_config);
}
