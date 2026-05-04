/*
 * This file is part of ltrace.
 * Copyright (C) 2006,2007,2011,2012,2013,2014 Petr Machata, Red Hat Inc.
 * Copyright (C) 2009 Juan Cespedes
 * Copyright (C) 1998,2001,2002,2003,2007,2008,2009 Juan Cespedes
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

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __powerpc__
#include <sys/ptrace.h>
#endif

#include "backend.h"
#include "breakpoint.h"
#include "debug.h"
#include "library.h"
#include "ltrace-elf.h"
#include "proc.h"

#ifndef ARCH_HAVE_TRANSLATE_ADDRESS
int
arch_translate_address_dyn(struct process *proc,
		       arch_addr_t addr, arch_addr_t *ret)
{
	*ret = addr;
	return 0;
}

struct ltelf;
int
arch_translate_address(struct ltelf *lte,
		       arch_addr_t addr, arch_addr_t *ret)
{
	*ret = addr;
	return 0;
}
#endif

void
breakpoint_on_hit(struct breakpoint *bp, struct process *proc)
{
	assert(bp != NULL);
	if (bp->cbs != NULL && bp->cbs->on_hit != NULL)
		(bp->cbs->on_hit)(bp, proc);
}

void
breakpoint_on_continue(struct breakpoint *bp, struct process *proc)
{
	assert(bp != NULL);
	if (bp->cbs != NULL && bp->cbs->on_continue != NULL)
		(bp->cbs->on_continue)(bp, proc);
	else
		continue_after_breakpoint(proc, bp);
}

void
breakpoint_on_retract(struct breakpoint *bp, struct process *proc)
{
	assert(bp != NULL);
	if (bp->cbs != NULL && bp->cbs->on_retract != NULL)
		(bp->cbs->on_retract)(bp, proc);
}

void
breakpoint_on_install(struct breakpoint *bp, struct process *proc)
{
	assert(bp != NULL);
	if (bp->cbs != NULL && bp->cbs->on_install != NULL)
		(bp->cbs->on_install)(bp, proc);
}

int
breakpoint_get_return_bp(struct breakpoint **ret,
			 struct breakpoint *bp, struct process *proc)
{
	assert(bp != NULL);
	if (bp->cbs != NULL && bp->cbs->get_return_bp != NULL)
		return (bp->cbs->get_return_bp)(ret, bp, proc);

	if ((*ret = create_default_return_bp(proc)) == NULL)
		return -1;

	return 0;
}

/*****************************************************************************/

struct breakpoint *
address2bpstruct(struct process *proc, arch_addr_t addr)
{
	assert(proc != NULL);
	assert(proc->breakpoints != NULL);
	assert(proc->leader == proc);
	debug(DEBUG_FUNCTION, "address2bpstruct(pid=%d, addr=%p)", proc->pid, addr);

	struct breakpoint *found;
	if (DICT_FIND_VAL(proc->breakpoints, &addr, &found) < 0)
		return NULL;
	return found;
}

#ifndef OS_HAVE_BREAKPOINT_DATA
int
os_breakpoint_init(struct process *proc, struct breakpoint *sbp)
{
	return 0;
}

void
os_breakpoint_destroy(struct breakpoint *sbp)
{
}

int
os_breakpoint_clone(struct breakpoint *retp, struct breakpoint *sbp)
{
	return 0;
}
#endif

#ifndef ARCH_HAVE_BREAKPOINT_DATA
int
arch_breakpoint_init(struct process *proc, struct breakpoint *sbp)
{
	return 0;
}

void
arch_breakpoint_destroy(struct breakpoint *sbp)
{
}

int
arch_breakpoint_clone(struct breakpoint *retp, struct breakpoint *sbp)
{
	return 0;
}
#endif

static void
breakpoint_init_base(struct breakpoint *bp,
		     arch_addr_t addr, struct library_symbol *libsym)
{
	bp->cbs = NULL;
	bp->addr = addr;
	memset(bp->orig_value, 0, sizeof(bp->orig_value));
	bp->enabled = 0;
	bp->libsym = libsym;
}

/* On second thought, I don't think we need PROC.  All the translation
 * (arch_translate_address in particular) should be doable using
 * static lookups of various sections in the ELF file.  We shouldn't
 * need process for anything.  */
int
breakpoint_init(struct breakpoint *bp, struct process *proc,
		arch_addr_t addr, struct library_symbol *libsym)
{
	breakpoint_init_base(bp, addr, libsym);
	if (os_breakpoint_init(proc, bp) < 0)
		return -1;
	if (arch_breakpoint_init(proc, bp) < 0) {
		os_breakpoint_destroy(bp);
		return -1;
	}
	return 0;
}

void
breakpoint_set_callbacks(struct breakpoint *bp, struct bp_callbacks *cbs)
{
	if (bp->cbs != NULL)
		assert(bp->cbs == NULL);
	bp->cbs = cbs;
}

void
breakpoint_destroy(struct breakpoint *bp)
{
	if (bp == NULL)
		return;
	arch_breakpoint_destroy(bp);
	os_breakpoint_destroy(bp);
}

int
breakpoint_clone(struct breakpoint *retp, struct process *new_proc,
		 struct breakpoint *bp)
{
	struct library_symbol *libsym = NULL;
	if (bp->libsym != NULL) {
		int rc = proc_find_symbol(new_proc, bp->libsym, NULL, &libsym);
		assert(rc == 0);
	}

	breakpoint_init_base(retp, bp->addr, libsym);
	memcpy(retp->orig_value, bp->orig_value, sizeof(bp->orig_value));
	retp->enabled = bp->enabled;
	if (os_breakpoint_clone(retp, bp) < 0)
		return -1;
	if (arch_breakpoint_clone(retp, bp) < 0) {
		os_breakpoint_destroy(retp);
		return -1;
	}
	breakpoint_set_callbacks(retp, bp->cbs);
	return 0;
}

int
breakpoint_turn_on(struct breakpoint *bp, struct process *proc)
{
	bp->enabled++;
	if (bp->enabled == 1) {
		assert(proc->pid != 0);
		enable_breakpoint(proc, bp);
		breakpoint_on_install(bp, proc);
	}
	return 0;
}

int
breakpoint_turn_off(struct breakpoint *bp, struct process *proc)
{
	bp->enabled--;
	if (bp->enabled == 0)
		disable_breakpoint(proc, bp);
	assert(bp->enabled >= 0);
	return 0;
}

struct breakpoint *
create_default_return_bp(struct process *proc)
{
	struct breakpoint *bp = malloc(sizeof *bp);
	arch_addr_t return_addr = get_return_addr(proc, proc->stack_pointer);
	if (return_addr == 0 || bp == NULL
	    || breakpoint_init(bp, proc, return_addr, NULL) < 0) {
		free(bp);
		return NULL;
	}
	return bp;
}

struct breakpoint *
insert_breakpoint_at(struct process *proc, arch_addr_t addr,
		     struct library_symbol *libsym)
{
	debug(DEBUG_FUNCTION,
	      "insert_breakpoint_at(pid=%d, addr=%p, symbol=%s)",
	      proc->pid, addr, libsym ? libsym->name : "NULL");

	assert(addr != 0);

	struct breakpoint *bp = malloc(sizeof *bp);
	if (bp == NULL || breakpoint_init(bp, proc, addr, libsym) < 0) {
		free(bp);
		return NULL;
	}

	/* N.B. (and XXX): BP->addr might differ from ADDR.  On ARM
	 * this is a real possibility.  The problem here is that to
	 * create a return breakpoint ltrace calls get_return_addr and
	 * then insert_breakpoint_at.  So get_return_addr needs to
	 * encode all the information necessary for breakpoint_init
	 * into the address itself, so ADDR is potentially
	 * mangled.  */

	struct breakpoint *tmp = insert_breakpoint(proc, bp);
	if (tmp != bp) {
		breakpoint_destroy(bp);
		free(bp);
	}
	return tmp;
}

struct breakpoint *
insert_breakpoint(struct process *proc, struct breakpoint *bp)
{
	/* Only the group leader should be getting the breakpoints and
	 * thus have ->breakpoint initialized.  */
	struct process *leader = proc->leader;
	assert(leader != NULL);
	assert(leader->breakpoints != NULL);

	/* XXX what we need to do instead is have a list of
	 * breakpoints that are enabled at this address.  The
	 * following works if every breakpoint is the same and there's
	 * no extra data, but that doesn't hold anymore.  For now it
	 * will suffice, about the only realistic case where we need
	 * to have more than one breakpoint per address is return from
	 * a recursive library call.  */
	struct breakpoint *ext_bp = bp;
	if (DICT_FIND_VAL(leader->breakpoints, &bp->addr, &ext_bp) != 0) {
		if (proc_add_breakpoint(leader, bp) < 0)
			return NULL;
		ext_bp = bp;
	}

	if (breakpoint_turn_on(ext_bp, proc) < 0) {
		if (ext_bp != bp)
			proc_remove_breakpoint(leader, bp);
		return NULL;
	}

	return ext_bp;
}

void
delete_breakpoint_at(struct process *proc, arch_addr_t addr)
{
	debug(DEBUG_FUNCTION, "delete_breakpoint_at(pid=%d, addr=%p)",
	      proc->pid, addr);

	struct process *leader = proc->leader;
	assert(leader != NULL);

	struct breakpoint *bp = NULL;
	DICT_FIND_VAL(leader->breakpoints, &addr, &bp);
	assert(bp != NULL);

	if (delete_breakpoint(proc, bp) < 0) {
		fprintf(stderr, "Couldn't turn off the breakpoint %s@%p\n",
			breakpoint_name(bp), bp->addr);
	}
}

int
delete_breakpoint(struct process *proc, struct breakpoint *bp)
{
	struct process *leader = proc->leader;
	assert(leader != NULL);

	if (breakpoint_turn_off(bp, proc) < 0)
		return -1;

	if (bp->enabled == 0) {
		proc_remove_breakpoint(leader, bp);
		breakpoint_destroy(bp);
		free(bp);
	}

	return 0;
}

const char *
breakpoint_name(const struct breakpoint *bp)
{
	assert(bp != NULL);
	return bp->libsym != NULL ? bp->libsym->name : NULL;
}

struct library *
breakpoint_library(const struct breakpoint *bp)
{
	assert(bp != NULL);
	return bp->libsym != NULL ? bp->libsym->lib : NULL;
}

static enum callback_status
disable_bp_cb(arch_addr_t *addr, struct breakpoint **bpp, void *data)
{
	struct process *proc = data;
	debug(DEBUG_FUNCTION, "disable_bp_cb(pid=%d)", proc->pid);
	if ((*bpp)->enabled)
		disable_breakpoint(proc, *bpp);
	return CBS_CONT;
}

void
disable_all_breakpoints(struct process *proc)
{
	debug(DEBUG_FUNCTION, "disable_all_breakpoints(pid=%d)", proc->pid);
	assert(proc->leader == proc);
	DICT_EACH(proc->breakpoints, arch_addr_t, struct breakpoint *,
		  NULL, disable_bp_cb, proc);
}

static void
entry_breakpoint_on_hit(struct breakpoint *bp, struct process *proc)
{
	if (proc == NULL || proc->leader == NULL)
		return;
	delete_breakpoint_at(proc, bp->addr);
	process_hit_start(proc);
}

int
entry_breakpoint_init(struct process *proc,
		      struct breakpoint *bp, arch_addr_t addr,
		      struct library *lib)
{
	assert(addr != 0);
	int err = breakpoint_init(bp, proc, addr, NULL);
	if (err < 0)
		return err;

	static struct bp_callbacks entry_callbacks = {
		.on_hit = entry_breakpoint_on_hit,
	};
	bp->cbs = &entry_callbacks;
	return 0;
}

int
breakpoints_init(struct process *proc)
{
	debug(DEBUG_FUNCTION, "breakpoints_init(pid=%d)", proc->pid);

	/* XXX breakpoint dictionary should be initialized
	 * outside.  Here we just put in breakpoints.  */
	assert(proc->breakpoints != NULL);

	/* Only the thread group leader should hold the breakpoints.  */
	assert(proc->leader == proc);

	/* N.B. the following used to be conditional on this, and
	 * maybe it still needs to be.  */
	assert(proc->filename != NULL);

	struct library *lib = ltelf_read_main_binary(proc, proc->filename);
	struct breakpoint *entry_bp = NULL;
	int bp_state = 0;
	int result = -1;
	switch ((int)(lib != NULL)) {
	fail:
		switch (bp_state) {
		case 2:
			proc_remove_library(proc, lib);
			proc_remove_breakpoint(proc, entry_bp);
		case 1:
			breakpoint_destroy(entry_bp);
		}
		library_destroy(lib);
		free(entry_bp);
	case 0:
		return result;
	}

	entry_bp = malloc(sizeof(*entry_bp));
	if (entry_bp == NULL
	    || (entry_breakpoint_init(proc, entry_bp,
				      lib->entry, lib)) < 0) {
		fprintf(stderr,
			"Couldn't initialize entry breakpoint for PID %d.\n"
			"Some tracing events may be missed.\n", proc->pid);
		free(entry_bp);

	} else {
		++bp_state;

		if ((result = proc_add_breakpoint(proc, entry_bp)) < 0)
			goto fail;
		++bp_state;

		if ((result = breakpoint_turn_on(entry_bp, proc)) < 0)
			goto fail;
	}
	proc_add_library(proc, lib);

	proc->callstack_depth = 0;
	return 0;
}
