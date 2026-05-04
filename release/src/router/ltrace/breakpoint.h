/*
 * This file is part of ltrace.
 * Copyright (C) 2012,2013,2014 Petr Machata, Red Hat Inc.
 * Copyright (C) 2009 Juan Cespedes
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

#ifndef BREAKPOINT_H
#define BREAKPOINT_H

/* XXX This is currently a very weak abstraction.  We would like to
 * much expand this to allow things like breakpoints on SDT probes and
 * such.
 *
 * In particular, we would like to add a tracepoint abstraction.
 * Tracepoint is a traceable feature--e.g. an exact address, a DWARF
 * symbol, an ELF symbol, a PLT entry, or an SDT probe.  Tracepoints
 * are named and the user can configure which of them he wants to
 * enable.  Realized tracepoints enable breakpoints, which are a
 * low-level realization of high-level tracepoint.
 *
 * Service breakpoints like the handling of dlopen would be a
 * low-level breakpoint, likely without tracepoint attached.
 *
 * So that's for sometimes.
 */

#include "sysdep.h"
#include "library.h"
#include "forward.h"

struct bp_callbacks {
	void (*on_hit)(struct breakpoint *bp, struct process *proc);
	void (*on_continue)(struct breakpoint *bp, struct process *proc);
	void (*on_install)(struct breakpoint *bp, struct process *proc);
	void (*on_retract)(struct breakpoint *bp, struct process *proc);

	/* Create a new breakpoint that should handle return from the
	 * function.  BP is the breakpoint that was just hit and for
	 * which we wish to find the corresponding return breakpoint.
	 * This returns 0 on success (in which case *RET will have
	 * been initialized to desired breakpoint object, or NULL if
	 * none is necessary) or a negative value on failure.  */
	int (*get_return_bp)(struct breakpoint **ret,
			     struct breakpoint *bp, struct process *proc);
};

struct breakpoint {
	struct bp_callbacks *cbs;
	struct library_symbol *libsym;
	void *addr;
	unsigned char orig_value[BREAKPOINT_LENGTH];
	int enabled;
	struct arch_breakpoint_data arch;
	struct os_breakpoint_data os;
};

/* Call ON_HIT handler of BP, if any is set.  */
void breakpoint_on_hit(struct breakpoint *bp, struct process *proc);

/* Call ON_CONTINUE handler of BP.  If none is set, call
 * continue_after_breakpoint.  */
void breakpoint_on_continue(struct breakpoint *bp, struct process *proc);

/* Call ON_RETRACT handler of BP, if any is set.  This should be
 * called before the breakpoints are destroyed.  The reason for a
 * separate interface is that breakpoint_destroy has to be callable
 * without PROC.  ON_DISABLE might be useful as well, but that would
 * be called every time we disable the breakpoint, which is too often
 * (a breakpoint has to be disabled every time that we need to execute
 * the instruction underneath it).  */
void breakpoint_on_retract(struct breakpoint *bp, struct process *proc);

/* Call ON_INSTALL handler of BP, if any is set.  This should be
 * called after the breakpoint is enabled for the first time, not
 * every time it's enabled (such as after stepping over a site of a
 * temporarily disabled breakpoint).  */
void breakpoint_on_install(struct breakpoint *bp, struct process *proc);

/* Call GET_RETURN_BP handler of BP, if any is set.  If none is set,
 * call CREATE_DEFAULT_RETURN_BP to obtain one.  */
int breakpoint_get_return_bp(struct breakpoint **ret,
			     struct breakpoint *bp, struct process *proc);

/* Initialize a breakpoint structure.  That doesn't actually realize
 * the breakpoint.  The breakpoint is initially assumed to be
 * disabled.  orig_value has to be set separately.  CBS may be
 * NULL.  */
int breakpoint_init(struct breakpoint *bp, struct process *proc,
		    arch_addr_t addr, struct library_symbol *libsym);

/* Make a clone of breakpoint BP into the area of memory pointed to by
 * RETP.  Symbols of cloned breakpoint are looked up in NEW_PROC.
 * Returns 0 on success or a negative value on failure.  */
int breakpoint_clone(struct breakpoint *retp, struct process *new_proc,
		     struct breakpoint *bp);

/* Set callbacks.  If CBS is non-NULL, then BP->cbs shall be NULL.  */
void breakpoint_set_callbacks(struct breakpoint *bp, struct bp_callbacks *cbs);

/* Destroy a breakpoint structure.   */
void breakpoint_destroy(struct breakpoint *bp);

/* Call enable_breakpoint the first time it's called.  Returns 0 on
 * success and a negative value on failure.  */
int breakpoint_turn_on(struct breakpoint *bp, struct process *proc);

/* Call disable_breakpoint when turned off the same number of times
 * that it was turned on.  Returns 0 on success and a negative value
 * on failure.  */
int breakpoint_turn_off(struct breakpoint *bp, struct process *proc);

/* Allocate and initialize a default return breakpoint.  Returns NULL
 * on failure.  */
struct breakpoint *create_default_return_bp(struct process *proc);

/* This allocates and initializes new breakpoint at ADDR, then calls
 * INSERT_BREAKPOINT.  Returns the new breakpoint or NULL if there are
 * errors.  */
struct breakpoint *insert_breakpoint_at(struct process *proc, arch_addr_t addr,
					struct library_symbol *libsym);

/* Check if there is a breakpoint on this address already.  If yes,
 * return that breakpoint instead (BP was not added).  If no, try to
 * PROC_ADD_BREAKPOINT and BREAKPOINT_TURN_ON.  If it all works,
 * return BP.  Otherwise return NULL.  */
struct breakpoint *insert_breakpoint(struct process *proc,
				     struct breakpoint *bp);

/* Name of a symbol associated with BP.  May be NULL.  */
const char *breakpoint_name(const struct breakpoint *bp);

/* A library that this breakpoint comes from.  May be NULL.  */
struct library *breakpoint_library(const struct breakpoint *bp);

/* Again, this seems to be several interfaces rolled into one:
 *  - breakpoint_disable
 *  - proc_remove_breakpoint
 *  - breakpoint_destroy
 * XXX */
void delete_breakpoint_at(struct process *proc, void *addr);
int delete_breakpoint(struct process *proc, struct breakpoint *bp);

/* XXX some of the following belongs to proc.h/proc.c.  */
struct breakpoint *address2bpstruct(struct process *proc, void *addr);
void disable_all_breakpoints(struct process *proc);
int breakpoints_init(struct process *proc);

#endif /* BREAKPOINT_H */
