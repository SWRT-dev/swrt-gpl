/*
 * This file is part of ltrace.
 * Copyright (C) 2012,2013,2014 Petr Machata, Red Hat Inc.
 * Copyright (C) 2012 Edgar E. Iglesias, Axis Communications
 * Copyright (C) 2008,2009 Juan Cespedes
 * Copyright (C) 2006 Eric Vaitl, Cisco Systems, Inc.
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

#include <sys/ptrace.h>
#include <errno.h>
#include <err.h>
#include <gelf.h>
#include <inttypes.h>
#include <string.h>

#include "sysdeps/linux-gnu/trace.h"
#include "backend.h"
#include "breakpoint.h"
#include "common.h"
#include "debug.h"
#include "library.h"
#include "proc.h"
#include "vect.h"

#include "mips.h"

/**
   \addtogroup mips
   @{
 */

/*
 * The following holds for o32 only.  Ltrace currently doesn't support
 * n32 or n64 ABI's.
 *
 * MIPS ABI Supplement has this to say about MIPS-specific SHT_DYNAMIC
 * entries:
 *
 * - DT_PLTGOT This member holds the address of the .got section.
 *
 * - DT_MIPS_SYMTABNO This member holds the number of entries in the
 *   .dynsym section.
 *
 * - DT_MIPS_LOCAL_GOTNO This member holds the number of local GOT
 *   entries.
 *
 * - DT_MIPS_GOTSYM This member holds the index of the first dyamic
 *   symbol table entry that corresponds to an entry in the gobal
 *   offset table.
 *
 * In PLT-enabled binaries, we trace binaries as usual.
 *
 * In PLT-disabled binaries, the calls are done through addresses
 * fetched from GOT.  (For some background, see code in
 * ports/sysdeps/mips/dl-trampoline.c in glibc.  Also, keep in mind
 * that MIPS uses branch delay slots.)
 *
 * 00400660 <main>:
 *   [...]
 *   400670:	3c1c0042 	lui	gp,0x42
 *   400674:	279c8900 	addiu	gp,gp,-30464 <-- gp = some global pointer?
 *   [...]
 *   40068c:	8f828034 	lw	v0,-32716(gp) <-- address from GOT
 *   400690:	00200825 	move	at,at
 *   400694:	0040c821 	move	t9,v0
 *   400698:	0320f809 	jalr	t9    <-- jump at it
 *   40069c:	00200825 	move	at,at
 *
 * GOT initially contains addresses that point to .MIPS.stubs section,
 * where the jump leads:
 *
 *   400820:	8f998010 	lw	t9,-32752(gp) <-- gp = GOT[0]
 *   400824:	03e07821 	move	t7,ra         <-- t7 = caller's return addr
 *   400828:	0320f809 	jalr	t9
 *   40082c:	24180009 	li	t8,9          <-- t8 = .dynsym index
 *
 * After the first call, GOT entry is resolved and contains address of
 * the resolved symbol.  So .MIPS.stubs is avoided except on first
 * call, and possibly even then, if the binary is prelinked.  So we
 * have to do the same thing that PPC backend does (see
 * sysdeps/linux-gnu/ppc/plt.c for more details):
 *
 * - We scan .MIPS.stubs and decode the li t8,X instruction to figure
 *   out which stub corresponds to which symbol.  These stubs will act
 *   as PLT entries for us--that's where we put breakpoints.
 *
 * - After startup, we detect whether GOT entries corresponding to
 *   MIPS stubs really contain the addresses that we expect.  If they
 *   don't, presumably prelink has altered them, and they contain
 *   already resolved addresses.  Where that is the case, we remember
 *   that address, rewrite the GOT entry to point back to stub, and
 *   mark the breakpoint as "resolved".  If GOT still contains the
 *   stub address, we mark the breakpoint as "unresolved".
 *
 * - When a "resolved" breakpoint hits, we look up the resolved
 *   address, move the PC there, and continue the process.
 *
 * - When an "unresolved" breakpoint hits, we stop all threads,
 *   single-step through the whole dynamic linker, waiting for the GOT
 *   entry to be updated.  When it is, we remember that value, rewrite
 *   it back, mark the breakpoint as resolved, move the PC, and
 *   continue the process.
 *
 * - As an optimization, we could remember the PC at which the GOT was
 *   resolved, install there a breakpoint, and next time, instead of
 *   single-stepping, we could let the process hit that breakpoint.
 */

/* Are we in pure CPIC mode (the non-PIC ABI extension)?  */
static inline int
mips_elf_is_cpic(unsigned int elf_flags)
{
	return (elf_flags & (EF_MIPS_PIC | EF_MIPS_CPIC)) == EF_MIPS_CPIC;
}

/**
 * \param lte Structure containing link table entry information
 * \param ndx Index into .dynsym
 *
 * Called by read_elf when building the symbol table.
 */
GElf_Addr
arch_plt_sym_val(struct ltelf *lte, size_t ndx, GElf_Rela *rela)
{
	debug(1,"plt_addr %zx ndx %#zx",lte->arch.pltgot_addr, ndx);

	if (mips_elf_is_cpic(lte->ehdr.e_flags) && lte->plt_addr != 0) {
		/* Return a pointer into the PLT.  */
		return lte->plt_addr + 16 * 2 + (ndx * 16);
	} else {
		return rela->r_addend;
	}
}

/**
 * \param proc The process to work on.
 * \param sym The library symbol.
 * \return What is at the got table address
 *
 * The return value should be the address to put the breakpoint at.
 *
 * On the mips the library_symbol.enter_addr is the .got addr for the
 * symbol and the breakpoint.addr is the actual breakpoint address.
 *
 * Other processors use a plt, the mips is "special" in that is uses
 * the .got for both function and data relocations. Prior to program
 * startup, return 0.
 *
 * \warning MIPS relocations are lazy. This means that the breakpoint
 * may move after the first call. Ltrace dictionary routines don't
 * have a delete and symbol is one to one with breakpoint, so if the
 * breakpoint changes I just add a new breakpoint for the new address.
 */
void *
sym2addr(struct process *proc, struct library_symbol *sym)
{
        return sym->enter_addr;
}

/* Address of run time loader map, used for debugging.  */
#ifndef DT_MIPS_RLD_MAP
# define DT_MIPS_RLD_MAP         0x70000016
#endif
int
arch_find_dl_debug(struct process *proc, arch_addr_t dyn_addr,
		   arch_addr_t *ret)
{
	arch_addr_t rld_addr;
	int r;

	/* MIPS puts the address of the r_debug structure into the
	 * DT_MIPS_RLD_MAP entry instead of into the DT_DEBUG entry.  */
	r = proc_find_dynamic_entry_addr(proc, dyn_addr,
					 DT_MIPS_RLD_MAP, &rld_addr);
	if (r == 0) {
		if (umovebytes(proc, rld_addr,
			       ret, sizeof *ret) != sizeof *ret) {
			r = -1;
		}
	}
	return r;
}

int
arch_elf_init(struct ltelf *lte, struct library *lib)
{
	/* FIXME: for CPIC we should really scan both GOT tables
	 * to pick up relocations to external functions.  Right now
	 * function pointers from the main binary to external functions
	 * can't be traced in CPIC mode.  */
	if (mips_elf_is_cpic(lte->ehdr.e_flags) && lte->plt_addr != 0) {
		return 0; /* We are already done.  */
	}

	Elf_Scn *scn;
	GElf_Shdr shdr;
	if (elf_get_section_type(lte, SHT_DYNAMIC, &scn, &shdr) < 0
	    || scn == NULL) {
	fail_dyn:
		fprintf(stderr, "Error reading SHT_DYNAMIC: %s\n",
			elf_errmsg(-1));
		return -1;
	}

	Elf_Data *data = elf_loaddata(scn, &shdr);
	if (data == NULL)
		goto fail_dyn;

	size_t j;
	for (j = 0; j < shdr.sh_size / shdr.sh_entsize; ++j) {
		GElf_Dyn dyn;
		if (gelf_getdyn(data, j, &dyn) == NULL)
			goto fail_dyn;

		if (dyn.d_tag == DT_PLTGOT)
			lte->arch.pltgot_addr = dyn.d_un.d_ptr;
		else if (dyn.d_tag == DT_MIPS_LOCAL_GOTNO)
			lte->arch.mips_local_gotno = dyn.d_un.d_val;
		else if (dyn.d_tag == DT_MIPS_GOTSYM)
			lte->arch.mips_gotsym = dyn.d_un.d_val;
	}

	if (lte->arch.mips_gotsym > lte->dynsym_count) {
		fprintf(stderr,
			"%s: DT_MIPS_GOTSYM larger than number of .dynsym's.\n",
			lib->soname);
		return -1;
	}

	/* In non-CPIC code, we need to tell ltrace about the
	 * .MIPS.stubs entries.
	 *
	 * From the subset of .dynsym that is covered by GOT, we take
	 * those symbols that are FUNC, UND, and have value != 0
	 * (i.e. either non-WEAK, or defined).  Those ought to have a
	 * stub associated.
	 */

	if (elf_get_section_named(lte, ".MIPS.stubs", &scn, &shdr) < 0) {
	fail_stubs:
		fprintf(stderr, "Error reading .MIPS.stubs: %s\n",
			elf_errmsg(-1));
		return -1;
	}

	if (scn == NULL) {
		/* Maybe we don't need any stubs?  */
		for (j = lte->arch.mips_gotsym; j < lte->dynsym_count; ++j) {
			GElf_Sym sym_mem, *sym = gelf_getsym(lte->dynsym, j, &sym_mem);
			if (sym == NULL) {
				fprintf(stderr,
					"%s: Error reading .dynsym: %s\n",
					lte->soname, elf_errmsg(-1));
				return -1;
			}
			if (GELF_ST_TYPE(sym->st_info) == STT_FUNC
			    && sym->st_shndx == SHN_UNDEF
			    && sym->st_value != 0) {
				/* Nope, we do need a stub for this
				 * symbol.  Warn the user.  */
				fprintf(stderr,
					"Can't trace undefined symbols in %s "
					"without .MIPS.stubs.\n", lte->soname);
				break;
			}
		}

		return 0;
	}

	data = elf_loaddata(scn, &shdr);
	if (data == NULL
	    || data->d_size % 16 != 0 /* Four insns per stub.  */)
		goto fail_stubs;

	/* Stubs seem to be ordered in reverse order of .dynsym's.
	 * Quite likely that's deliberate, as then we get to subtract
	 * some multiple of symbol index from some base, and get to
	 * the stub.  But to be sure, we'll scan the stubs.  */

	for (j = 0; j < data->d_size / 16; ++j) {
		uint32_t insn;
		if (elf_read_u32(data, j * 16 + 12, &insn) < 0)
			goto fail_stubs;

		if (insn == 0)
			continue;

		/* 0x2418XXXX encodes lbu 0,t8,XXXX or li t8,XXXX.  */
		if ((insn & 0xffff0000U) != 0x24180000U)
			goto fail_stubs;

		unsigned idx = insn & 0xffff;
		if (idx < lte->arch.mips_gotsym || idx >= lte->dynsym_count)
			goto fail_stubs;

		GElf_Sym sym_mem, *sym
			= gelf_getsym(lte->dynsym, idx, &sym_mem);
		if (sym == NULL)
			goto fail_stubs;

		/* The GOT is organized as follows: first, there are
		 * DT_MIPS_LOCAL_GOTNO local entries.  The rest are
		 * entries for symbols from .dynsym, starting with
		 * DT_MIPS_GOTSYM-th symbol.  After that there can be
		 * further entries.  */
		unsigned got_idx = idx - lte->arch.mips_gotsym
			+ lte->arch.mips_local_gotno;
		/* XXX Double cast.  */
		arch_addr_t got_entry_addr
			= (arch_addr_t) (uintptr_t) lte->arch.pltgot_addr
			+ got_idx * 4;

		GElf_Rela rela = {
			/* XXX double cast.  */
			.r_offset = (GElf_Addr) (uintptr_t) got_entry_addr,
			.r_info = GELF_R_INFO(idx, R_MIPS_NONE),
			.r_addend = j * 16 + shdr.sh_addr,
		};

		if (VECT_PUSHBACK(&lte->plt_relocs, &rela) < 0)
			goto fail_stubs;

		fprintf(stderr,
			"added stub entry for symbol %u at %#lx, GOT @%p\n",
			idx, (unsigned long) rela.r_addend, got_entry_addr);
	}

	/* Tell the generic code how many dynamic trace:able symbols
	 * we've got.  */
	// lte->relplt_count = lte->dynsym_count - lte->arch.mips_gotsym;
	return 0;
}

void
arch_elf_destroy(struct ltelf *lte)
{
}

void
arch_symbol_ret(struct process *proc, struct library_symbol *libsym)
{
}

static int
read_got_entry(struct process *proc, GElf_Addr addr, GElf_Addr *valp)
{
	/* XXX double cast.  */
	arch_addr_t a = (arch_addr_t) (uintptr_t) addr;
	uint32_t l;
	if (proc_read_32(proc, a, &l) < 0) {
		fprintf(stderr, "ptrace read got entry @%#" PRIx64 ": %s\n",
			addr, strerror(errno));
		return -1;
	}

	*valp = (GElf_Addr) l;
	return 0;
}

struct mips_unresolve_data {
	struct mips_unresolve_data *self; /* A canary.  */
	GElf_Addr got_entry_value;
	GElf_Addr stub_addr;
};

void
arch_dynlink_done(struct process *proc)
{
	struct library_symbol *libsym = NULL;

	while ((libsym = proc_each_symbol(proc, libsym,
					  library_symbol_delayed_cb, NULL))) {
		GElf_Addr got_entry_value = 0;
		GElf_Addr resolved_value = libsym->arch.resolved_value;

		if (read_got_entry(proc, libsym->arch.got_entry_addr,
				   &got_entry_value) < 0) {
			perror("dynlink_done: read got entry");
			return;
		}

		if ((got_entry_value != resolved_value) &&
		    (got_entry_value != 0)) {
			libsym->arch.type = MIPS_PLT_NEED_UNRESOLVE;
			libsym->arch.data = malloc(sizeof *libsym->arch.data);
			if (libsym->arch.data == NULL) {
				perror("dynlink done: malloc unresolve data");
				return;
			}

			libsym->arch.data->self = libsym->arch.data;
			libsym->arch.data->got_entry_value = got_entry_value;
			libsym->arch.data->stub_addr = resolved_value;
		}

		if (proc_activate_delayed_symbol(proc, libsym) < 0)
			return;
	}
}

enum plt_status
arch_elf_add_plt_entry(struct process *proc, struct ltelf *lte,
                       const char *a_name, GElf_Rela *rela, size_t ndx,
                       struct library_symbol **ret)
{
	if (mips_elf_is_cpic(lte->ehdr.e_flags) && (lte->plt_addr != 0))
		return PLT_DEFAULT;

	GElf_Addr got_entry_addr = rela->r_offset + lte->bias;
	GElf_Addr stub_addr = rela->r_addend + lte->bias;

	debug(2, "PLT-less arch_elf_add_plt_entry %s = %#llx\n",
	      a_name, stub_addr);

	struct library_symbol *libsym = NULL;
	if (default_elf_add_plt_entry(proc, lte, a_name, rela, ndx,
				      &libsym) < 0) {
		fprintf(stderr, "%s: failed %s(%#llx): %s\n", __func__,
			a_name, stub_addr, strerror(errno));
		goto fail;
	}

	debug(2, "%s unresolved\n", libsym->name);
	libsym->arch.got_entry_addr = got_entry_addr;
	libsym->arch.resolved_value = stub_addr;
	libsym->arch.type = MIPS_PLT_UNRESOLVED;
	libsym->delayed = 1;

	*ret = libsym;
	return PLT_OK;

fail:
	if (libsym != NULL) {
		library_symbol_destroy(libsym);
		free(libsym);
	}
	return PLT_FAIL;
}

int
arch_library_symbol_init(struct library_symbol *libsym)
{
	/* We set type explicitly in the code above, where we have the
	 * necessary context.  */
	libsym->arch.type = MIPS_PLT_DEFAULT;
	libsym->arch.got_entry_addr = 0;
	return 0;
}

void
arch_library_symbol_destroy(struct library_symbol *libsym)
{
	if (libsym->arch.type == MIPS_PLT_NEED_UNRESOLVE) {
		assert(libsym->arch.data->self == libsym->arch.data);
		free(libsym->arch.data);
		libsym->arch.data = NULL;
	}
}

int
arch_library_symbol_clone(struct library_symbol *retp,
                          struct library_symbol *libsym)
{
	retp->arch = libsym->arch;
	if (libsym->arch.type == MIPS_PLT_NEED_UNRESOLVE) {
		assert(libsym->arch.data->self == libsym->arch.data);
		retp->arch.data = malloc(sizeof *retp->arch.data);
		if (retp->arch.data == NULL)
			return -1;
		*retp->arch.data = *libsym->arch.data;
		retp->arch.data->self = retp->arch.data;
	}
	return 0;
}

static void
jump_to_entry_point(struct process *proc, struct breakpoint *bp)
{
	/* XXX The double cast should be removed when
	 * arch_addr_t becomes integral type.  */
	arch_addr_t rv = (arch_addr_t)
		(uintptr_t) bp->libsym->arch.resolved_value;
	set_instruction_pointer(proc, rv);
	ptrace(PTRACE_POKEUSER, proc->pid, off_t9, rv);
}

static int
unresolve_got_entry(struct process *proc, GElf_Addr addr, GElf_Addr value)
{
	uint32_t v32 = (uint32_t) value;
	uint32_t a32 = (uint32_t) addr;
	if (ptrace(PTRACE_POKETEXT, proc->pid, a32, v32) < 0) {
		fprintf(stderr, "failed to unresolve GOT entry: %s\n",
			strerror(errno));
		return -1;
	}
	return 0;
}

static void
mark_as_resolved(struct library_symbol *libsym, GElf_Addr value)
{
	libsym->arch.type = MIPS_PLT_RESOLVED;
	libsym->arch.resolved_value = value;
}

static void
dl_got_update_bp_on_hit(struct breakpoint *bp, struct process *proc)
{
	debug(DEBUG_PROCESS, "pid=%d dl_got_update_bp_on_hit %s(%p)",
	      proc->pid, breakpoint_name(bp), bp->addr);
	struct process_stopping_handler *self = proc->arch.handler;
	assert(self != NULL);

	struct library_symbol *libsym = self->breakpoint_being_enabled->libsym;
	GElf_Addr got_entry_value;
	if (read_got_entry(proc, libsym->arch.got_entry_addr,
			   &got_entry_value) < 0)
		return;

	if (unresolve_got_entry(proc, libsym->arch.got_entry_addr,
				libsym->arch.resolved_value) < 0)
		return;
	mark_as_resolved(libsym, got_entry_value);

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

	GElf_Addr got_entry_value;
	if (read_got_entry(proc, libsym->arch.got_entry_addr,
			   &got_entry_value) < 0)
		return CBS_FAIL;

	/* In UNRESOLVED state, the RESOLVED_VALUE in fact contains
	 * the PLT entry value.  */
	if (got_entry_value == libsym->arch.resolved_value)
		return CBS_CONT;
	debug(2, "%#" PRIx64 " resolved to %#" PRIx64 "\n",
		libsym->arch.got_entry_addr, got_entry_value);

	/* The GOT entry got resolved!  We can migrate the breakpoint
	 * to RESOLVED and stop single-stepping.  */
	if (unresolve_got_entry(proc, libsym->arch.got_entry_addr,
				libsym->arch.resolved_value) < 0)
		return CBS_FAIL;

	/* Install breakpoint to the address where the change takes
	 * place.  If we fail, then that just means that we'll have to
	 * singlestep the next time around as well.  */
	struct process *leader = proc->leader;
	if (leader == NULL || leader->arch.dl_got_update_bp != NULL)
		goto done;

	/* We need to install to the next instruction.  ADDR points to
	 * a store instruction, so moving the breakpoint one
	 * instruction forward is safe.  */
	arch_addr_t addr = get_instruction_pointer(proc) + 4;
	leader->arch.dl_got_update_bp = insert_breakpoint_at(proc, addr, NULL);
	if (leader->arch.dl_got_update_bp == NULL)
		goto done;

	static struct bp_callbacks dl_got_update_cbs = {
		.on_hit = dl_got_update_bp_on_hit,
	};
	leader->arch.dl_got_update_bp->cbs = &dl_got_update_cbs;

	/* Turn it off for now.  We will turn it on again when we hit
	 * the PLT entry that needs this.  */
	breakpoint_turn_off(leader->arch.dl_got_update_bp, proc);

done:
	mark_as_resolved(libsym, got_entry_value);
	return CBS_STOP;
}

static void
cb_on_all_stopped(struct process_stopping_handler *self)
{
	/* Put that in for dl_got_update_bp_on_hit to see.  */
	assert(self->task_enabling_breakpoint->arch.handler == NULL);
	self->task_enabling_breakpoint->arch.handler = self;

	linux_ptrace_disable_and_continue(self);
}

static void
mips_stub_bp_continue(struct breakpoint *bp, struct process *proc)
{
	switch (bp->libsym->arch.type) {
		struct process *leader;
		void (*on_all_stopped)(struct process_stopping_handler *);
		enum callback_status (*keep_stepping_p)
			(struct process_stopping_handler *);

	case MIPS_PLT_RESOLVED:
		debug(2, "hit resolved BP %s\n", bp->libsym->name);
		jump_to_entry_point(proc, bp);
		continue_process(proc->pid);
		return;

	case MIPS_PLT_UNRESOLVED:
		on_all_stopped = NULL;
		keep_stepping_p = NULL;
		leader = proc->leader;

		if (leader != NULL && leader->arch.dl_got_update_bp != NULL
		    && breakpoint_turn_on(leader->arch.dl_got_update_bp,
					  proc) >= 0)
			on_all_stopped = cb_on_all_stopped;
		else
			keep_stepping_p = cb_keep_stepping_p;

		if (process_install_stopping_handler
		    (proc, bp, on_all_stopped, keep_stepping_p, NULL) < 0) {
			fprintf(stderr,	"mips_stub_bp_continue: "
				"couldn't install event handler\n");
			continue_after_breakpoint(proc, bp);
		}
		return;

	case MIPS_PLT_DEFAULT:
	case MIPS_PLT_NEED_UNRESOLVE:
		/* We should never get here.  */
		break;
	}

	assert(bp->libsym->arch.type != bp->libsym->arch.type);
	abort();
}

static void
mips_stub_bp_retract(struct breakpoint *bp, struct process *proc)
{
	/* This should not be an artificial breakpoint.  */
	struct library_symbol *libsym = bp->libsym;
	assert(libsym != NULL);

	/* Restore resolved .got entry before detaching */
	unresolve_got_entry(proc, libsym->arch.got_entry_addr,
				libsym->arch.resolved_value);
}

static void
mips_stub_bp_install(struct breakpoint *bp, struct process *proc)
{
	/* This should not be an artificial breakpoint.  */
	struct library_symbol *libsym = bp->libsym;
	assert(libsym != NULL);

	if (libsym->arch.type == MIPS_PLT_NEED_UNRESOLVE) {
		/* Re-route the got-entry to the stub and save resolved address
		   for the break-point handler */
		GElf_Addr got_entry_value = libsym->arch.data->got_entry_value;

		if (unresolve_got_entry(proc, libsym->arch.got_entry_addr,
					   libsym->arch.data->stub_addr) < 0)
			return;
		free(libsym->arch.data);
		mark_as_resolved(libsym, got_entry_value);
	}
}

/* For some symbol types, we need to set up custom callbacks.  */
int
arch_breakpoint_init(struct process *proc, struct breakpoint *bp)
{
	/* Artificial, entry-point breakpoints are plain.  Breakpoints
	 * for PLT-ful symbols are also plain.  */
	if (bp->libsym == NULL || bp->libsym->plt_type != LS_TOPLT_EXEC
	    || bp->libsym->arch.type == MIPS_PLT_DEFAULT)
		return 0;

	debug(2, "new STUB breakpoint at %p\n", bp->addr);
	static struct bp_callbacks cbs = {
		.on_continue = mips_stub_bp_continue,
		.on_retract = mips_stub_bp_retract,
		.on_install = mips_stub_bp_install,
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
	proc->arch.dl_got_update_bp = NULL;
	proc->arch.handler = NULL;
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

	if (retp->arch.dl_got_update_bp != NULL) {
		/* Point it to the corresponding breakpoint in RETP.
		 * It must be there, this part of PROC has already
		 * been cloned to RETP.  */
		retp->arch.dl_got_update_bp
			= address2bpstruct(retp,
					   retp->arch.dl_got_update_bp->addr);

		assert(retp->arch.dl_got_update_bp != NULL);
	}

	return 0;
}

int
arch_process_exec(struct process *proc)
{
	return arch_process_init(proc);
}

/**@}*/
