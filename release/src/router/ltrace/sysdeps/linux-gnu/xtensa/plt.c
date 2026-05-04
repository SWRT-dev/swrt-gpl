/*
 * This file is part of ltrace.
 * Copyright (C) 2014 Cadence Design Systems Inc.
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

#include <errno.h>
#include <error.h>
#include <gelf.h>
#include <inttypes.h>
#include <string.h>
#include <sys/ptrace.h>

#include "common.h"
#include "debug.h"
#include "proc.h"
#include "library.h"
#include "breakpoint.h"
#include "backend.h"
#include "trace.h"

static void
mark_as_resolved(struct process *proc, struct library_symbol *libsym,
		 GElf_Addr value)
{
	arch_addr_t addr = (arch_addr_t)(intptr_t)libsym->arch.resolved_addr;
	struct breakpoint *bp = insert_breakpoint_at(proc, addr, libsym);

	if (bp != NULL) {
		enable_breakpoint(proc, bp);
	}
	libsym->arch.type = XTENSA_PLT_RESOLVED;
	libsym->arch.resolved_addr = value;
}

static int
read_plt_slot_value(struct process *proc, arch_addr_t addr, GElf_Addr *valp)
{
	long l = ptrace(PTRACE_PEEKTEXT, proc->pid, addr);

	if (l < 0) {
		debug(DEBUG_EVENT, "ptrace .plt slot value @%p: %s",
		      addr, strerror(errno));
		return -1;
	}

	*valp = (GElf_Addr)l;
	return 0;
}

static int
unresolve_plt_slot(struct process *proc, arch_addr_t addr, GElf_Addr value)
{
	if (ptrace(PTRACE_POKETEXT, proc->pid, addr,
		   (void *)(intptr_t)value) < 0) {
		debug(DEBUG_EVENT, "failed to unresolve .plt slot @%p: %s",
		      addr, strerror(errno));
		return -1;
	}
	return 0;
}

int
arch_elf_init(struct ltelf *lte, struct library *lib)
{
	Elf_Scn *scn;
	GElf_Shdr shdr;
	GElf_Addr relplt_addr;
	GElf_Phdr phdr;
	GElf_Addr low, high;
	int has_loadable = 0;
	size_t i;

	for (i = 0; gelf_getphdr(lte->elf, i, &phdr) != NULL; ++i) {
		if (phdr.p_type == PT_LOAD) {
			if (has_loadable) {
				if (phdr.p_vaddr < low)
					low = phdr.p_vaddr;
				if (phdr.p_vaddr + phdr.p_memsz > high)
					high = phdr.p_vaddr + phdr.p_memsz;
			} else {
				has_loadable = 1;
				low = phdr.p_vaddr;
				high = phdr.p_vaddr + phdr.p_memsz;
			}
		}
	}
	lib->arch.loadable_sz = has_loadable ? high - low : 0;

	if (elf_load_dynamic_entry(lte, DT_JMPREL, &relplt_addr) < 0 ||
	    elf_get_section_covering(lte, relplt_addr, &scn, &shdr) < 0 ||
	    scn == NULL)
		return 0;

	if (elf_read_relocs(lte, scn, &shdr, &lte->plt_relocs) < 0) {
		fprintf(stderr, "Couldn't get .rel*.plt data: %s\n",
			elf_errmsg(-1));
		return -1;
	}
	return 0;
}

void
arch_elf_destroy(struct ltelf *lte)
{
}

int
arch_get_sym_info(struct ltelf *lte, const char *filename,
		  size_t sym_index, GElf_Rela *rela, GElf_Sym *sym)
{
	if (gelf_getsym(lte->dynsym, ELF64_R_SYM(rela->r_info), sym) == NULL)
		return -1;

	/* .rela.plt entries that reference locally defined functions point
	 * to their entry points directly, not to PLT entries.  Skip such
	 * symbols.  */
	if (sym->st_shndx != SHN_UNDEF) {
		const char *name = lte->dynstr + sym->st_name;
		debug(2, "symbol %s does not have plt entry", name);
		return 1;
	}

	return 0;
}

enum plt_status
arch_elf_add_plt_entry(struct process *proc, struct ltelf *lte,
		       const char *name, GElf_Rela *rela, size_t ndx,
		       struct library_symbol **ret)
{
	if (default_elf_add_plt_entry(proc, lte, name, rela, ndx, ret) < 0) {
		return PLT_FAIL;
	}

	/* All PLT library symbols are initially marked as delayed.  Some of
	 * them may reference weak symbols that are never loaded, sym2addr for
	 * such entries will return NULL.  All other symbols are activated
	 * after the dynlink is done.  */
	(*ret)->delayed = 1;
	return PLT_OK;
}

void
arch_dynlink_done(struct process *proc)
{
	struct library_symbol *libsym = NULL;

	while ((libsym = proc_each_symbol(proc, libsym,
					  library_symbol_delayed_cb, NULL))) {
		assert(libsym->plt_type == LS_TOPLT_EXEC);

		if (read_plt_slot_value(proc, libsym->enter_addr,
					&libsym->arch.resolved_addr) == 0 &&
		    libsym->arch.resolved_addr) {
			GElf_Addr base =
				(GElf_Addr)(intptr_t)libsym->lib->base;
			GElf_Addr sz = libsym->lib->arch.loadable_sz;

			/* Some references may be resolved at this point, they
			 * will point outside the loadable area of their own
			 * library.  */
			if (libsym->arch.resolved_addr >= base &&
			    libsym->arch.resolved_addr - base < sz) {
				libsym->arch.type = XTENSA_PLT_UNRESOLVED;
				proc_activate_delayed_symbol(proc, libsym);
			} else {
				libsym->arch.type = XTENSA_PLT_RESOLVED;
			}
		}
	}
}

GElf_Addr arch_plt_sym_val(struct ltelf *lte, size_t ndx, GElf_Rela * rela)
{
	return rela->r_offset;
}

void *sym2addr(struct process *proc, struct library_symbol *sym)
{
	void *addr = NULL;
	long ret = ptrace(PTRACE_PEEKTEXT, proc->pid, sym->enter_addr, 0);

	switch (sym->plt_type) {
	case LS_TOPLT_NONE:
		addr = sym->enter_addr;

		/* Not every exported function starts with ENTRY instruction,
		 * e.g. _start does not.  Only skip first instruction if it's
		 * entry, otherwise don't do it: if the first instruction is
		 * FLIX or density it will break it or the following
		 * instruction.  */
		if ((ret & XTENSA_ENTRY_MASK) == XTENSA_ENTRY_VALUE) {
			addr += 3;
		}
		break;

	case LS_TOPLT_EXEC:

		/* OTOH every PLT entry starts with ENTRY.  Put initial
		 * breakpoint after it.  After symbol resolution put
		 * additional breakpoint at the first instruction.  */
		addr = (ret == -1 || ret == 0) ? NULL : (void *)(ret + 3);
		break;
	}
	return addr;
}

int
arch_library_symbol_init(struct library_symbol *libsym)
{
	libsym->arch.type = XTENSA_DEFAULT;
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
	retp->arch = libsym->arch;
	return 0;
}

static void
dl_plt_update_bp_on_hit(struct breakpoint *bp, struct process *proc)
{
	debug(DEBUG_PROCESS, "pid=%d dl_plt_update_bp_on_hit %s(%p)",
	      proc->pid, breakpoint_name(bp), bp->addr);
	struct process_stopping_handler *self = proc->arch.handler;
	assert(self != NULL);

	struct library_symbol *libsym = self->breakpoint_being_enabled->libsym;
	GElf_Addr value;
	if (read_plt_slot_value(proc, libsym->enter_addr, &value) < 0)
		return;

	unresolve_plt_slot(proc, libsym->enter_addr,
			   libsym->arch.resolved_addr);
	mark_as_resolved(proc, libsym, value);

	/* cb_on_all_stopped looks if HANDLER is set to NULL as a way
	 * to check that this was run.  It's an error if it
	 * wasn't.  */
	proc->arch.handler = NULL;

	breakpoint_turn_off(bp, proc);
}

static enum callback_status
cb_keep_stepping_p(struct process_stopping_handler *self)
{
	struct process *proc = self->task_enabling_breakpoint;
	struct library_symbol *libsym = self->breakpoint_being_enabled->libsym;

	GElf_Addr value;
	if (read_plt_slot_value(proc, libsym->enter_addr, &value) < 0)
		return CBS_FAIL;

	/* In UNRESOLVED state, the resolved_addr in fact contains
	 * the PLT entry value.  */
	if (value == libsym->arch.resolved_addr) {
		/* Don't try to single-step over our own breakpoint infinitely.
		 * This may happen if we fail to detect resolved PLT entry.  */
		if (address2bpstruct(proc, get_instruction_pointer(proc))) {
			return CBS_FAIL;
		}
		return CBS_CONT;
	}

	debug(DEBUG_PROCESS, "pid=%d PLT got resolved to value %#"PRIx64,
	      proc->pid, value);

	/* The .plt slot got resolved!  We can migrate the breakpoint
	 * to RESOLVED and stop single-stepping.  */
	if (unresolve_plt_slot(proc, libsym->enter_addr,
			       libsym->arch.resolved_addr) < 0)
		return CBS_FAIL;

	/* Install breakpoint to the address where the change takes
	 * place.  If we fail, then that just means that we'll have to
	 * singlestep the next time around as well.  */
	struct process *leader = proc->leader;
	if (leader == NULL || leader->arch.dl_plt_update_bp != NULL)
		goto done;

	arch_addr_t addr = get_instruction_pointer(proc);
	struct breakpoint *dl_plt_update_bp =
		insert_breakpoint_at(proc, addr, NULL);
	if (dl_plt_update_bp == NULL)
		goto done;

	leader->arch.dl_plt_update_bp = dl_plt_update_bp;

	static struct bp_callbacks dl_plt_update_cbs = {
		.on_hit = dl_plt_update_bp_on_hit,
	};
	breakpoint_set_callbacks(dl_plt_update_bp, &dl_plt_update_cbs);

	/* Turn it off for now.  We will turn it on again when we hit
	 * the PLT entry that needs this.  */
	breakpoint_turn_off(dl_plt_update_bp, proc);

done:
	mark_as_resolved(proc, libsym, value);

	return CBS_STOP;
}

static void
cb_on_all_stopped(struct process_stopping_handler *self)
{
	/* Put that in for dl_plt_update_bp_on_hit to see.  */
	assert(self->task_enabling_breakpoint->arch.handler == NULL);
	self->task_enabling_breakpoint->arch.handler = self;

	linux_ptrace_disable_and_continue(self);
}

static void
xtensa_plt_bp_hit(struct breakpoint *bp, struct process *proc)
{
	struct library_symbol *libsym = bp->libsym;

	if (libsym->arch.type == XTENSA_PLT_RESOLVED) {
		arch_addr_t addr =
			(arch_addr_t)(intptr_t)libsym->arch.resolved_addr;

		set_instruction_pointer(proc, addr);
		ptrace(PTRACE_SINGLESTEP, proc->pid, NULL, NULL);
		return;
	}
}

static void
xtensa_plt_bp_continue(struct breakpoint *bp, struct process *proc)
{
	struct process *leader = proc->leader;
	void (*on_all_stopped)(struct process_stopping_handler *) = NULL;
	enum callback_status (*keep_stepping_p)
		(struct process_stopping_handler *) = NULL;

	if (bp->libsym->arch.type != XTENSA_PLT_UNRESOLVED) {
		continue_process(proc->pid);
		return;
	}

	if (leader != NULL && leader->arch.dl_plt_update_bp != NULL &&
	    breakpoint_turn_on(leader->arch.dl_plt_update_bp, proc) >= 0) {
		on_all_stopped = cb_on_all_stopped;
	} else {
		keep_stepping_p = cb_keep_stepping_p;
	}

	if (process_install_stopping_handler(proc, bp, on_all_stopped,
					     keep_stepping_p, NULL) < 0) {
		fprintf(stderr,	"%s: couldn't install event handler\n",
			__func__);
		continue_after_breakpoint(proc, bp);
	}
}

/* For some symbol types, we need to set up custom callbacks.
 */
int
arch_breakpoint_init(struct process *proc, struct breakpoint *bp)
{
	/* Artificial and entry-point breakpoints are plain.  */
	if (bp->libsym == NULL || bp->libsym->plt_type != LS_TOPLT_EXEC)
		return 0;

	static struct bp_callbacks cbs = {
		.on_hit = xtensa_plt_bp_hit,
		.on_continue = xtensa_plt_bp_continue,
	};
	breakpoint_set_callbacks(bp, &cbs);

	return 0;
}

void
arch_breakpoint_destroy(struct breakpoint *bp)
{
}

int
arch_breakpoint_clone(struct breakpoint *retp, struct breakpoint *sbp)
{
	retp->arch = sbp->arch;
	return 0;
}

int
arch_process_init(struct process *proc)
{
	proc->arch.dl_plt_update_bp = NULL;
	return 0;
}

void
arch_process_destroy(struct process *proc)
{
}

int
arch_process_clone(struct process *retp, struct process *proc)
{
	retp->arch = proc->arch;

	if (retp->arch.dl_plt_update_bp != NULL) {
		/* Point it to the corresponding breakpoint in RETP.
		 * It must be there, this part of PROC has already
		 * been cloned to RETP.  */
		retp->arch.dl_plt_update_bp
			= address2bpstruct(retp,
					   retp->arch.dl_plt_update_bp->addr);

		assert(retp->arch.dl_plt_update_bp != NULL);
	}

	return 0;
}

int
arch_process_exec(struct process *proc)
{
	return arch_process_init(proc);
}

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
