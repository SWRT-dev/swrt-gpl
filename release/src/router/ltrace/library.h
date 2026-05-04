/*
 * This file is part of ltrace.
 * Copyright (C) 2012,2013 Petr Machata, Red Hat Inc.
 * Copyright (C) 2006 Paul Gilliam, IBM Corporation
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

#ifndef LIBRARY_H
#define LIBRARY_H

#include <stdint.h>
#include <stdbool.h>

#if defined(HAVE_LIBDW)
# include <elfutils/libdwfl.h>
#endif

#include "dict.h"
#include "callback.h"
#include "forward.h"
#include "sysdep.h"

enum toplt {
	LS_TOPLT_NONE = 0,	/* PLT not used for this symbol. */
	LS_TOPLT_EXEC,		/* PLT for this symbol is executable. */
};

/* Dict interface.  */
size_t arch_addr_hash(const arch_addr_t *addr);
int arch_addr_eq(const arch_addr_t *addr1, const arch_addr_t *addr2);


/* For handling -l and for handling library export aliases (different symbol
 * name, same address)
 *
 * This structure needs to
 * - store (addr, name) tuples
 * - be searchable by addr (when populating)
 * - be searchable by name (when looking for aliases)
 * - be enumeratable (by activate_latent_in())
 */
struct library_exported_names {
	// I store the data in several structures to facilitate different types
	// of access
	struct dict names; // maps a name to an address
	struct dict addrs; // maps an address to a vect of names
};

struct library_symbol {
	struct library_symbol *next;
	struct library *lib;
	const char *name;
	arch_addr_t enter_addr;
	enum toplt plt_type;

	/* If this is non-NULL, this prototype is used instead of
	 * looking up one in LIB->protolib.  */
	struct prototype *proto;

	int own_name : 1;

	/* This is relevant for PLT symbols.  Latent PLT symbols are
	 * those that don't match any of the -e rules, but that might
	 * potentially become active if a library implementing them
	 * appears that matches a -l rule.  Ltrace core is responsible
	 * for clearing latent flag.  */
	int latent : 1;

	/* Delayed symbols are those for which a breakpoint shouldn't
	 * be enabled yet.  They are similar to latent symbols, but
	 * backend is responsible for clearing the delayed flag.  See
	 * proc_activate_delayed_symbol.  */
	int delayed : 1;

	struct arch_library_symbol_data arch;
	struct os_library_symbol_data os;
};

/* Init LIBSYM.  NAME will be freed when LIBSYM is destroyed if
 * OWN_NAME.  ARCH has to be initialized by a separate call.  */
int library_symbol_init(struct library_symbol *libsym,
			arch_addr_t addr, const char *name, int own_name,
			enum toplt type_of_plt);

/* Copy library symbol SYM into the area pointed-to by RETP.  Return 0
 * on success or a negative value on failure.  */
int library_symbol_clone(struct library_symbol *retp,
			 struct library_symbol *sym);

/* Destroy library symbol.  This essentially just frees name if it's
 * owned.  It doesn't free the memory associated with SYM pointer
 * itself.  Returns 0 on success or a negative value in case of an
 * error (which would be an out of memory condition).  */
void library_symbol_destroy(struct library_symbol *sym);

/* Compare two library symbols.  Returns a negative value, 0, or a
 * positive value, much like strcmp.  The function compares symbol
 * addresses, and if those are equal, it compares symbol names.  If
 * those are equal, too, the symbols are considered equal.  */
int library_symbol_cmp(struct library_symbol *a, struct library_symbol *b);

/* Set a name for library symbol.  This frees the old name, if
 * that is owned.  */
void library_symbol_set_name(struct library_symbol *libsym,
			     const char *name, int own_name);

/* A function that can be used as library_each_symbol callback.  Looks
 * for a symbol SYM for which library_symbol_cmp(SYM, STANDARD)
 * returns 0.  */
enum callback_status library_symbol_equal_cb(struct library_symbol *libsym,
					     void *standard);

/* A function that can be used as library_each_symbol callback.  Looks
 * for a symbol SYM for which strcmp(SYM->name, NAME) == 0.  */
enum callback_status library_symbol_named_cb(struct library_symbol *libsym,
					     void *name);

/* A function that can be used as library_each_symbol callback.  Looks
 * for a delayed symbol.  */
enum callback_status library_symbol_delayed_cb(struct library_symbol *libsym,
					       void *unused);

enum library_type {
	LT_LIBTYPE_MAIN,
	LT_LIBTYPE_DSO,
	LT_LIBTYPE_SYSCALL,
};

/* XXX we might consider sharing libraries across processes.  Things
 * like libc will be opened by every single process, no point cloning
 * these everywhere.  But for now, keep the ownership structure
 * simple.  */
struct library {
	struct library *next;

	/* Unique key. Two library objects are considered equal, if
	 * they have the same key.  */
	arch_addr_t key;

	/* Address where the library is mapped.  */
	arch_addr_t base;

	/* Absolute address of the entry point.  Useful for main
	 * binary, though I suppose the value might be useful for the
	 * dynamic linker, too (in case we ever want to do early
	 * process tracing).  */
	arch_addr_t entry;

	/* Address of PT_DYNAMIC segment.  */
	arch_addr_t dyn_addr;

	/* Symbols associated with the library.  This includes a
	 * symbols that don't have a breakpoint attached (yet).  */
	struct library_symbol *symbols;

	/* List of names that this library implements, and that match
	 * -l filter.  Each time a new library is mapped, its list of
	 * exports is examined, and corresponding PLT slots are
	 * enabled. This data structure also keeps track of export
	 * addresses to find symbols with different names, but same
	 * addresses */
	struct library_exported_names exported_names;

	/* Prototype library associated with this library.  */
	struct protolib *protolib;

	const char *soname;
	const char *pathname;

	enum library_type type;

	char own_soname : 1;
	char own_pathname : 1;
	bool should_activate_latent : 1;

	struct arch_library_data arch;
	struct os_library_data os;

#if defined(HAVE_LIBDW)
	Dwfl_Module *dwfl_module;
#endif
};

/* Init LIB.  */
int library_init(struct library *lib, enum library_type type);

/* Initialize RETP to a library identical to LIB.  Symbols are not
 * shared, but copied over.  Returns 0 on success and a negative value
 * in case of failure.  */
int library_clone(struct library *retp, struct library *lib);

/* Destroy library.  Doesn't free LIB itself.  Symbols are destroyed
 * and freed.  */
void library_destroy(struct library *lib);

/* Set library soname.  Frees the old name if necessary.  */
void library_set_soname(struct library *lib,
			const char *new_name, int own_name);

/* Set library pathname.  Frees the old name if necessary.  */
void library_set_pathname(struct library *lib,
			  const char *new_name, int own_name);

/* Iterate through list of symbols of library LIB.  See callback.h for
 * notes on this interface.  */
struct library_symbol *library_each_symbol
	(struct library *lib, struct library_symbol *start_after,
	 enum callback_status (*cb)(struct library_symbol *, void *),
	 void *data);

/* Add a new symbol SYM to LIB.  SYM is assumed owned, we need to
 * overwrite SYM->next.  */
void library_add_symbol(struct library *lib, struct library_symbol *sym);

/* A function that can be used as proc_each_library callback.  Looks
 * for a library with the name passed in DATA.  PROC is ignored.  */
enum callback_status library_named_cb(struct process *proc,
				      struct library *lib, void *name);

/* A function that can be used as proc_each_library callback.  Looks
 * for a library with given base.
 *
 * NOTE: The key is passed as a POINTER to arch_addr_t (that
 * because in general, arch_addr_t doesn't fit in void*).  */
enum callback_status library_with_key_cb(struct process *proc,
					 struct library *lib, void *keyp);

/* XXX this should really be in backend.h (as on pmachata/revamp
 * branch), or, on this branch, in common.h.  But we need
 * arch_addr_t (which should also be in backend.h, I reckon), so
 * stuff it here for the time being.  */
/* This function is implemented in the back end.  It is called for all
 * raw addresses as read from symbol tables etc.  If necessary on
 * given architecture, this function should translate the address
 * according to .opd or other indirection mechanism.  Returns 0 on
 * success and a negative value on failure.  */
struct ltelf;
int arch_translate_address(struct ltelf *lte,
			   arch_addr_t addr, arch_addr_t *ret);
/* This is the same function as arch_translate_address, except it's
 * used at the point that we don't have ELF available anymore.  */
int arch_translate_address_dyn(struct process *proc,
			       arch_addr_t addr, arch_addr_t *ret);


/* Pushes a name/address tuple to the list of a library's exports. Returns 0 on
 * success
 */
int library_exported_names_push(struct library_exported_names *names,
				uint64_t addr, char *name,
				int own_name );

/* Iterates through the a library's export list, reporting each symbol that is
 * an alias of the given 'aliasname' symbol. This 'aliasname' symbol itself is
 * NOT reported, so if this symbol is unique, the callback is not called at all.
 *
 * If we want to iterate through the whole alias list, set
 * name_start_after=NULL. If we want to start iterating immediately past a
 * particular symbol name, pass a pointer to this symbol name in
 * name_start_after. This must be a pointer in the internal dict, preferably
 * returned by an earlier call to this function
 *
 * If the callback fails at any point, a pointer to the failing key is returned.
 * On success, returns NULL. The returned pointer can be passed back to this
 * function in name_start_after to resume skipping this element
 */
const char** library_exported_names_each_alias(
	struct library_exported_names *names,
	const char *aliasname,
	const char **name_start_after,
	enum callback_status (*cb)(const char *,
				   void *),
	void *data);

/* Returns 0 if the exported names list does not contain a given name, or 1 if
 * it does */
int library_exported_names_contains(struct library_exported_names *names,
				    const char *queryname);

#endif /* LIBRARY_H */
