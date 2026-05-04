/*
 * This file is part of ltrace.
 * Copyright (C) 2012, 2013 Petr Machata, Red Hat Inc.
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

#ifndef PROTOTYPE_H
#define PROTOTYPE_H

#include <stdbool.h>

#include "forward.h"
#include "dict.h"
#include "vect.h"

/* Function prototype.  */
struct prototype {
	/* Vector of struct param.  */
	struct vect params;

	struct arg_type_info *return_info;
	int own_return_info : 1;
};

/* Initialize a prototype PROTO.  The name will be NAME, and the
 * corresponding string will be owned and freed on destroy if
 * OWN_NAME.  */
void prototype_init(struct prototype *proto);

/* Destroy PROTO (but don't free the memory block pointed-to by
 * PROTO).  */
void prototype_destroy(struct prototype *proto);

/* Add new parameter PARAM to PROTO.  The structure contents are
 * copied and PARAM pointer itself is not owned by PROTO.  */
int prototype_push_param(struct prototype *proto, struct param *param);

/* Return number of parameters of prototype.  */
size_t prototype_num_params(struct prototype *proto);

/* Destroy N-th parameter from PROTO.  N shall be smaller than the
 * number of parameters.  */
void prototype_destroy_nth_param(struct prototype *proto, size_t n);

/* Get N-th parameter of PROTO.  N shall be smaller than the number of
 * parameters.  */
struct param *prototype_get_nth_param(struct prototype *proto, size_t n);

/* Iterate through the parameters of PROTO.  See callback.h for notes
 * on iteration interfaces.  */
struct param *prototype_each_param
	(struct prototype *proto, struct param *start_after,
	 enum callback_status (*cb)(struct prototype *, struct param *, void *),
	 void *data);

/* For storing type aliases.  */
struct named_type {
	struct arg_type_info *info;
	int forward : 1;
	int own_type : 1;
};

/* Initialize a named type INFO, which, if OWN_TYPE, is destroyed when
 * named_type_destroy is called.  */
void named_type_init(struct named_type *named,
		     struct arg_type_info *info, int own_type);

void named_type_destroy(struct named_type *named);

/* One prototype library.  */
struct protolib {
	/* Other libraries to look through if the definition is not
	 * found here.  Note that due to the way imports are stored,
	 * there is no way to distinguish where exactly (at which
	 * place of the config file) the import was made.  */
	struct vect imports;

	/* Dictionary of name->struct prototype.  */
	struct dict prototypes;

	/* Dictionary of name->struct named_type.  */
	struct dict named_types;

	/* Reference count.  */
	unsigned refs;
};

/* Initialize PLIB.  */
void protolib_init(struct protolib *plib);

/* Destroy PLIB.  */
void protolib_destroy(struct protolib *plib);

/* Push IMPORT to PLIB.  Returns 0 on success or a negative value on
 * failure.  In particular, -2 is returned if mutual import is
 * detected.  */
int protolib_add_import(struct protolib *plib, struct protolib *import);

/* Add a prototype PROTO to PLIB.  Returns 0 on success or a negative
 * value on failure.  NAME is owned and released on PLIB destruction
 * if OWN_NAME.  */
int protolib_add_prototype(struct protolib *plib,
			   const char *name, int own_name,
			   struct prototype *proto);

/* Add a named type NAMED to PLIB.  Returns 0 on success or a negative
 * value on failure.  NAME is owned and released on PLIB destruction
 * if OWN_NAME.  NAMED _pointer_ is copied to PLIB.  */
int protolib_add_named_type(struct protolib *plib,
			    const char *name, int own_name,
			    struct named_type *named);

/* Lookup prototype named NAME in PLIB.  If none is found and IMPORTS
 * is true, look recursively in each of the imports.  Returns the
 * corresponding prototype, or NULL if none was found.  */
struct prototype *protolib_lookup_prototype(struct protolib *plib,
					    const char *name, bool imports);

/* Add a named type NAMED to PLIB.  Returns 0 on success or a negative
 * value on failure.  */
int protolib_add_type(struct protolib *plib, struct named_type *named);

/* Lookup type named NAME in PLIB.  If none is found and IMPORTS is
 * true, look recursively in each of the imports.  Returns the
 * corresponding type, or NULL if none was found.  */
struct named_type *protolib_lookup_type(struct protolib *plib,
					const char *name, bool imports);

/* A cache of prototype libraries.  Can load prototype libraries on
 * demand.
 *
 * XXX ltrace should open one config per ABI, which maps long, int,
 * etc. to uint32_t etc.  It would also map char to either of
 * {u,}int8_t.  Other protolibs would have this as implicit import.
 * That would mean that the cache needs ABI tagging--each ABI should
 * have a separate prototype cache, because the types will potentially
 * differ between the ABI's.  protolib cache would then naturally be
 * stored in the ABI object, when this is introduced.  */
struct protolib_cache {
	/* Dictionary of filename->protolib*.  */
	struct dict protolibs;

	/* Fake module for implicit imports.  This is populated by all
	 * files coming from -F.  When -F is empty, it also contains
	 * either $HOME/.ltrace.conf, or /etc/ltrace.conf (whichever
	 * comes first).  */
	struct protolib imports;

	/* For tracking uses of cache during cache's own
	 * initialization.  */
	int bootstrap : 1;
};

/* Initialize CACHE.  Returns 0 on success or a negative value on
 * failure.  */
int protolib_cache_init(struct protolib_cache *cache,
			struct protolib *import);

/* Destroy CACHE.  */
void protolib_cache_destroy(struct protolib_cache *cache);

/* Get protolib corresponding to KEY from CACHE.  KEY would typically
 * be the soname of a library for which a protolib should be obtained.
 * If none has been loaded yet, load a new protolib, cache and return
 * it.  Returns NULL for failures.
 *
 * Protolibs are loaded from a config directory.  If -F contains
 * directory names, those are checked first.  Next, os_get_config_dirs
 * callback is used to get a list of directories to look into.  In the
 * first round, if ALLOW_PRIVATE, ltrace looks in user's private
 * directories.  If the config file wasn't found, the second round is
 * made through system directories.  In each directory, ltrace looks
 * and reads the file named KEY.conf.
 *
 * If the config file still wasn't found, an empty (but non-NULL)
 * protolib is provided instead.  That is augmented with the following
 * imports:
 *
 * - Legacy typedefs
 * - The IMPORT argument passed to protolib_cache_init, if non-NULL
 * - $HOME/.ltrace.conf if available
 * - @sysconfdir@/ltrace.conf if available
 * - Any configure _files_ passed in -F
 *
 * This function returns either the loaded protolib, or NULL when
 * there was an error.  */
struct protolib *protolib_cache_load(struct protolib_cache *cache,
				     const char *key, int own_key,
				     bool allow_private);

/* This is similar to protolib_cache_load, except that if a protolib
 * is not found NULL is returned instead of a default module.
 *
 * It returns 0 for success and a negative value for failure, and the
 * actual return value is passed via *RET.*/
int protolib_cache_maybe_load(struct protolib_cache *cache,
			      const char *key, int own_key,
			      bool allow_private,
			      struct protolib **ret);

/* This is similar to protolib_cache_load, but instead of looking for
 * the file to load in directories, the filename is given.  */
struct protolib *protolib_cache_file(struct protolib_cache *cache,
				     const char *filename, int own_filename);

/* This caches a default module.  This is what protolib_cache_load
 * calls if it fails to find the actual protolib.  Returns default
 * protolib or NULL if there was an error.  */
struct protolib *protolib_cache_default(struct protolib_cache *cache,
					const char *key, int own_key);

/* This is similar to protolib_cache_file, but the library to cache is
 * given in argument.  Returns 0 on success or a negative value on
 * failure.  PLIB is thereafter owned by CACHE.  */
int protolib_cache_protolib(struct protolib_cache *cache,
			    const char *filename, int own_filename,
			    struct protolib *plib);

/* Single global prototype cache.
 *
 * XXX Eventually each ABI should have its own cache.  The idea is
 * that there's one per-ABI config file that all others use for
 * elementary typedefs (long, char, size_t).  Ltrace then only deals
 * in fixed-width integral types (and pointers etc.).  */
extern struct protolib_cache g_protocache;

void init_global_config(void);

#endif /* PROTOTYPE_H */
