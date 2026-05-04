/*
 * This file is part of ltrace.
 * Copyright (C) 2012,2013,2014 Petr Machata, Red Hat Inc.
 * Copyright (C) 2004,2008,2009 Juan Cespedes
 * Copyright (C) 2006 Paul Gilliam
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

#include <gelf.h>
#include <sys/ptrace.h>
#include <errno.h>
#include <inttypes.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include "proc.h"
#include "common.h"
#include "insn.h"
#include "library.h"
#include "breakpoint.h"
#include "linux-gnu/trace.h"
#include "backend.h"

/* There are two PLT types on 32-bit PPC: old-style, BSS PLT, and
 * new-style "secure" PLT.  We can tell one from the other by the
 * flags on the .plt section.  If it's +X (executable), it's BSS PLT,
 * otherwise it's secure.
 *
 * BSS PLT works the same way as most architectures: the .plt section
 * contains trampolines and we put breakpoints to those.  If not
 * prelinked, .plt contains zeroes, and dynamic linker fills in the
 * initial set of trampolines, which means that we need to delay
 * enabling breakpoints until after binary entry point is hit.
 * Additionally, after first call, dynamic linker updates .plt with
 * branch to resolved address.  That means that on first hit, we must
 * do something similar to the PPC64 gambit described below.
 *
 * With secure PLT, the .plt section doesn't contain instructions but
 * addresses.  The real PLT table is stored in .text.  Addresses of
 * those PLT entries can be computed, and apart from the fact that
 * they are in .text, they are ordinary PLT entries.
 *
 * 64-bit PPC is more involved.  Program linker creates for each
 * library call a _stub_ symbol named xxxxxxxx.plt_call.<callee>
 * (where xxxxxxxx is a hexadecimal number).  That stub does the call
 * dispatch: it loads an address of a function to call from the
 * section .plt, and branches.  PLT entries themselves are essentially
 * a curried call to the resolver.  When the symbol is resolved, the
 * resolver updates the value stored in .plt, and the next time
 * around, the stub calls the library function directly.  So we make
 * at most one trip (none if the binary is prelinked) through each PLT
 * entry, and correspondingly that is useless as a breakpoint site.
 *
 * Note the three confusing terms: stubs (that play the role of PLT
 * entries), PLT entries, .plt section.
 *
 * We first check symbol tables and see if we happen to have stub
 * symbols available.  If yes we just put breakpoints to those, and
 * treat them as usual breakpoints.  The only tricky part is realizing
 * that there can be more than one breakpoint per symbol.
 *
 * The case that we don't have the stub symbols available is harder.
 * The following scheme uses two kinds of PLT breakpoints: unresolved
 * and resolved (to some address).  When the process starts (or when
 * we attach), we distribute unresolved PLT breakpoints to the PLT
 * entries (not stubs).  Then we look in .plt, and for each entry
 * whose value is different than the corresponding PLT entry address,
 * we assume it was already resolved, and convert the breakpoint to
 * resolved.  We also rewrite the resolved value in .plt back to the
 * PLT address.
 *
 * When a PLT entry hits a resolved breakpoint (which happens because
 * we rewrite .plt with the original unresolved addresses), we move
 * the instruction pointer to the corresponding address and continue
 * the process as if nothing happened.
 *
 * When unresolved PLT entry is called for the first time, we need to
 * catch the new value that the resolver will write to a .plt slot.
 * We also need to prevent another thread from racing through and
 * taking the branch without ltrace noticing.  So when unresolved PLT
 * entry hits, we have to stop all threads.  We then single-step
 * through the resolver, until the .plt slot changes.  When it does,
 * we treat it the same way as above: convert the PLT breakpoint to
 * resolved, and rewrite the .plt value back to PLT address.  We then
 * start all threads again.
 *
 * As an optimization, we remember the address where the address was
 * resolved, and put a breakpoint there.  The next time around (when
 * the next PLT entry is to be resolved), instead of single-stepping
 * through half the dynamic linker, we just let the thread run and hit
 * this breakpoint.  When it hits, we know the PLT entry was resolved.
 *
 * Another twist comes from tracing slots corresponding to
 * R_PPC64_JMP_IREL relocations.  These have no dedicated PLT entry.
 * The calls are done directly from stubs, and the .plt entry
 * (actually .iplt entry, these live in a special section) is resolved
 * in advance before the binary starts.  Because there's no PLT entry,
 * we put the PLT breakpoints directly to the IFUNC resolver code, and
 * then would like them to behave like ordinary PLT slots, including
 * catching the point where these get resolved to unresolve them.  So
 * for the first call (which is the actual resolver call), we pretend
 * that this breakpoint is artificial and has no associated symbol,
 * and turn it on fully only after the first hit.  Ideally we would
 * trace that first call as well, but then the stepper, which tries to
 * catch the point where the slot is resolved, would hit the return
 * breakpoint and that's not currently handled well.
 *
 * On PPC32 with secure PLT, the address of IFUNC symbols in main
 * binary actually isn't of the resolver, but of a PLT slot.  We
 * therefore have to locate the corresponding PLT relocation (which is
 * of type R_PPC_IRELATIVE) and request that it be traced.  The addend
 * of that relocation is an address of resolver, and we request
 * tracing of the xyz.IFUNC symbol there.
 *
 * XXX TODO If we have hardware watch point, we might put a read watch
 * on .plt slot, and discover the offenders this way.  I don't know
 * the details, but I assume at most a handful (like, one or two, if
 * available at all) addresses may be watched at a time, and thus this
 * would be used as an amendment of the above rather than full-on
 * solution to PLT tracing on PPC.
 */

#define PPC_PLT_STUB_SIZE 16
#if _CALL_ELF != 2
#define PPC64_PLT_STUB_SIZE 8
#else
#define PPC64_PLT_STUB_SIZE 4
#endif

static inline int
host_powerpc64()
{
#ifdef __powerpc64__
	return 1;
#else
	return 0;
#endif
}

static void
mark_as_resolved(struct library_symbol *libsym, GElf_Addr value)
{
	libsym->arch.type = PPC_PLT_RESOLVED;
	libsym->arch.resolved_value = value;
}

static void
ppc32_delayed_symbol(struct library_symbol *libsym)
{
	/* arch_dynlink_done is called on attach as well.  In that
	 * case some slots will have been resolved already.
	 * Unresolved PLT looks like this:
	 *
	 *    <sleep@plt>:	li      r11,0
	 *    <sleep@plt+4>:	b       "resolve"
	 *
	 * "resolve" is another address in PLTGOT (the same block that
	 * all the PLT slots are it).  When resolved, it looks either
	 * this way:
	 *
	 *    <sleep@plt>:	b       0xfea88d0 <sleep>
	 *
	 * Which is easy to detect.  It can also look this way:
	 *
	 *    <sleep@plt>:	li      r11,0
	 *    <sleep@plt+4>:	b       "dispatch"
	 *
	 * The "dispatch" address lies in PLTGOT as well.  In current
	 * GNU toolchain, "dispatch" address is the same as PLTGOT
	 * address.  We rely on this to figure out whether the address
	 * is resolved or not.  */

	uint32_t insn1 = libsym->arch.resolved_value >> 32;
	uint32_t insn2 = (uint32_t) libsym->arch.resolved_value;
	if ((insn1 & BRANCH_MASK) == B_INSN
	    || ((insn2 & BRANCH_MASK) == B_INSN
		/* XXX double cast  */
#ifdef __LITTLE_ENDIAN__
		&& (ppc_branch_dest(libsym->enter_addr + 4, insn1)
		    == (arch_addr_t) (long) libsym->lib->arch.pltgot_addr)))
#else
		&& (ppc_branch_dest(libsym->enter_addr + 4, insn2)
		    == (arch_addr_t) (long) libsym->lib->arch.pltgot_addr)))
#endif
	{
		mark_as_resolved(libsym, libsym->arch.resolved_value);
	}
}

void
arch_dynlink_done(struct process *proc)
{
	/* We may need to activate delayed symbols.  */
	struct library_symbol *libsym = NULL;
	while ((libsym = proc_each_symbol(proc, libsym,
					  library_symbol_delayed_cb, NULL))) {
		if (proc_read_64(proc, libsym->enter_addr,
				 &libsym->arch.resolved_value) < 0) {
			fprintf(stderr,
				"couldn't read PLT value for %s(%p): %s\n",
				libsym->name, libsym->enter_addr,
				strerror(errno));
				return;
		}

		if (proc->e_machine == EM_PPC)
			ppc32_delayed_symbol(libsym);

		if (proc_activate_delayed_symbol(proc, libsym) < 0)
			return;

		if (proc->e_machine == EM_PPC)
			/* XXX double cast  */
			libsym->arch.plt_slot_addr
				= (GElf_Addr) (uintptr_t) libsym->enter_addr;
	}
}

static bool
reloc_is_irelative(int machine, GElf_Rela *rela)
{
	bool irelative = false;
	if (machine == EM_PPC64) {
#ifdef __LITTLE_ENDIAN__
# ifdef R_PPC64_IRELATIVE
		irelative = GELF_R_TYPE(rela->r_info) == R_PPC64_IRELATIVE;
# endif
#else
# ifdef R_PPC64_JMP_IREL
		irelative = GELF_R_TYPE(rela->r_info) == R_PPC64_JMP_IREL;
# endif
#endif
	} else {
		assert(machine == EM_PPC);
#ifdef R_PPC_IRELATIVE
		irelative = GELF_R_TYPE(rela->r_info) == R_PPC_IRELATIVE;
#endif
	}
	return irelative;
}

GElf_Addr
arch_plt_sym_val(struct ltelf *lte, size_t ndx, GElf_Rela *rela)
{
	if (lte->ehdr.e_machine == EM_PPC && lte->arch.secure_plt) {
		assert(lte->arch.plt_stub_vma != 0);
		return lte->arch.plt_stub_vma + PPC_PLT_STUB_SIZE * ndx;

	} else if (lte->ehdr.e_machine == EM_PPC) {
		return rela->r_offset;

	/* Beyond this point, we are on PPC64, but don't have stub
	 * symbols.  */

	} else if (reloc_is_irelative(lte->ehdr.e_machine, rela)) {

		/* Put JMP_IREL breakpoint to resolver, since there's
		 * no dedicated PLT entry.  */

		assert(rela->r_addend != 0);
		/* XXX double cast */
		arch_addr_t res_addr
		  = (arch_addr_t) (uintptr_t) (rela->r_addend + lte->bias);
		if (arch_translate_address(lte, res_addr, &res_addr) < 0) {
			fprintf(stderr, "Couldn't OPD-translate IRELATIVE "
				"resolver address.\n");
			return 0;
		}
		/* XXX double cast */
		return (GElf_Addr) (uintptr_t) (res_addr - lte->bias);

	} else {
		/* We put brakpoints to PLT entries the same as the
		 * PPC32 secure PLT case does. */
		assert(lte->arch.plt_stub_vma != 0);
		return lte->arch.plt_stub_vma + PPC64_PLT_STUB_SIZE * ndx;
	}
}

/* This entry point is called when ltelf is not available
 * anymore--during runtime.  At that point we don't have to concern
 * ourselves with bias, as the values in OPD have been resolved
 * already.  */
int
arch_translate_address_dyn(struct process *proc,
			   arch_addr_t addr, arch_addr_t *ret)
{
	if (proc->e_machine == EM_PPC64) {
#if _CALL_ELF != 2
		uint64_t value;
		if (proc_read_64(proc, addr, &value) < 0) {
			fprintf(stderr,
				"dynamic .opd translation of %p: %s\n",
				addr, strerror(errno));
			return -1;
		}
		/* XXX The double cast should be removed when
		 * arch_addr_t becomes integral type.  */
		*ret = (arch_addr_t)(uintptr_t)value;
		return 0;
#endif
	}

	*ret = addr;
	return 0;
}

int
arch_translate_address(struct ltelf *lte,
		       arch_addr_t addr, arch_addr_t *ret)
{
	if (lte->ehdr.e_machine == EM_PPC64
	    && !lte->arch.elfv2_abi) {
		/* XXX The double cast should be removed when
		 * arch_addr_t becomes integral type.  */
		GElf_Xword offset
			= (GElf_Addr)(uintptr_t)addr - lte->arch.opd_base;
		uint64_t value;
		if (elf_read_u64(lte->arch.opd_data, offset, &value) < 0) {
			fprintf(stderr, "static .opd translation of %p: %s\n",
				addr, elf_errmsg(-1));
			return -1;
		}
		*ret = (arch_addr_t)(uintptr_t)(value + lte->bias);
		return 0;
	}

	*ret = addr;
	return 0;
}

static int
load_opd_data(struct ltelf *lte, struct library *lib)
{
	Elf_Scn *sec;
	GElf_Shdr shdr;
	if (elf_get_section_named(lte, ".opd", &sec, &shdr) < 0
	    || sec == NULL) {
	fail:
		fprintf(stderr, "couldn't find .opd data\n");
		return -1;
	}

	lte->arch.opd_data = elf_rawdata(sec, NULL);
	if (lte->arch.opd_data == NULL)
		goto fail;

	lte->arch.opd_base = shdr.sh_addr + lte->bias;
	lte->arch.opd_size = shdr.sh_size;

	return 0;
}

void *
sym2addr(struct process *proc, struct library_symbol *sym)
{
	return sym->enter_addr;
}

static GElf_Addr
get_glink_vma(struct ltelf *lte, GElf_Addr ppcgot, Elf_Data *plt_data)
{
	Elf_Scn *ppcgot_sec = NULL;
	GElf_Shdr ppcgot_shdr;
	if (ppcgot != 0
	    && (elf_get_section_covering(lte, ppcgot,
					 &ppcgot_sec, &ppcgot_shdr) < 0
		|| ppcgot_sec == NULL))
		fprintf(stderr,
			"DT_PPC_GOT=%#"PRIx64", but no such section found\n",
			ppcgot);

	if (ppcgot_sec != NULL) {
		Elf_Data *data = elf_loaddata(ppcgot_sec, &ppcgot_shdr);
		if (data == NULL || data->d_size < 8 ) {
			fprintf(stderr, "couldn't read GOT data\n");
		} else {
			// where PPCGOT begins in .got
			size_t offset = ppcgot - ppcgot_shdr.sh_addr;
			assert(offset % 4 == 0);
			uint32_t glink_vma;
			if (elf_read_u32(data, offset + 4, &glink_vma) < 0) {
				fprintf(stderr, "couldn't read glink VMA"
					" address at %zd@GOT\n", offset);
				return 0;
			}
			if (glink_vma != 0) {
				debug(1, "PPC GOT glink_vma address: %#" PRIx32,
				      glink_vma);
				return (GElf_Addr)glink_vma;
			}
		}
	}

	if (plt_data != NULL) {
		uint32_t glink_vma;
		if (elf_read_u32(plt_data, 0, &glink_vma) < 0) {
			fprintf(stderr, "couldn't read glink VMA address\n");
			return 0;
		}
		debug(1, ".plt glink_vma address: %#" PRIx32, glink_vma);
		return (GElf_Addr)glink_vma;
	}

	return 0;
}

static int
nonzero_data(Elf_Data *data)
{
	/* We are not supposed to get here if there's no PLT.  */
	assert(data != NULL);

	unsigned char *buf = data->d_buf;
	if (buf == NULL)
		return 0;

	size_t i;
	for (i = 0; i < data->d_size; ++i)
		if (buf[i] != 0)
			return 1;
	return 0;
}

static enum callback_status
reloc_copy_if_irelative(GElf_Rela *rela, void *data)
{
	struct ltelf *lte = data;

	return CBS_STOP_IF(reloc_is_irelative(lte->ehdr.e_machine, rela)
			   && VECT_PUSHBACK(&lte->plt_relocs, rela) < 0);
}

int
arch_elf_init(struct ltelf *lte, struct library *lib)
{

	/* Check for ABIv2 in ELF header processor specific flag.  */
#ifndef EF_PPC64_ABI
	assert (! (lte->ehdr.e_flags & 3 ) == 2)
#else
	lte->arch.elfv2_abi = ((lte->ehdr.e_flags & EF_PPC64_ABI) == 2);
#endif

	if (lte->ehdr.e_machine == EM_PPC64
	    && !lte->arch.elfv2_abi
	    && load_opd_data(lte, lib) < 0)
		return -1;

	lte->arch.secure_plt = !(lte->plt_flags & SHF_EXECINSTR);

	/* For PPC32 BSS, it is important whether the binary was
	 * prelinked.  If .plt section is NODATA, or if it contains
	 * zeroes, then this library is not prelinked, and we need to
	 * delay breakpoints.  */
	if (lte->ehdr.e_machine == EM_PPC && !lte->arch.secure_plt)
		lib->arch.bss_plt_prelinked = nonzero_data(lte->plt_data);
	else
		/* For cases where it's irrelevant, initialize the
		 * value to something conspicuous.  */
		lib->arch.bss_plt_prelinked = -1;

	/* On PPC64 and PPC32 secure, IRELATIVE relocations actually
	 * relocate .iplt section, and as such are stored in .rela.dyn
	 * (where all non-PLT relocations are stored) instead of
	 * .rela.plt.  Add these to lte->plt_relocs.  */

	GElf_Addr rela, relasz;
	Elf_Scn *rela_sec;
	GElf_Shdr rela_shdr;
	if ((lte->ehdr.e_machine == EM_PPC64 || lte->arch.secure_plt)
	    && elf_load_dynamic_entry(lte, DT_RELA, &rela) == 0
	    && elf_load_dynamic_entry(lte, DT_RELASZ, &relasz) == 0
	    && elf_get_section_covering(lte, rela, &rela_sec, &rela_shdr) == 0
	    && rela_sec != NULL) {

		struct vect v;
		VECT_INIT(&v, GElf_Rela);
		int ret = elf_read_relocs(lte, rela_sec, &rela_shdr, &v);
		if (ret >= 0
		    && VECT_EACH(&v, GElf_Rela, NULL,
				 reloc_copy_if_irelative, lte) != NULL)
			ret = -1;

		VECT_DESTROY(&v, GElf_Rela, NULL, NULL);

		if (ret < 0)
			return ret;
	}

	if (lte->ehdr.e_machine == EM_PPC && lte->arch.secure_plt) {
		GElf_Addr ppcgot;
		if (elf_load_dynamic_entry(lte, DT_PPC_GOT, &ppcgot) < 0) {
			fprintf(stderr, "couldn't find DT_PPC_GOT\n");
			return -1;
		}
		GElf_Addr glink_vma = get_glink_vma(lte, ppcgot, lte->plt_data);

		size_t count = vect_size(&lte->plt_relocs);
		lte->arch.plt_stub_vma = glink_vma
			- (GElf_Addr) count * PPC_PLT_STUB_SIZE;
		debug(1, "stub_vma is %#" PRIx64, lte->arch.plt_stub_vma);

	} else if (lte->ehdr.e_machine == EM_PPC64) {
		GElf_Addr glink_vma;
		if (elf_load_dynamic_entry(lte, DT_PPC64_GLINK,
					   &glink_vma) < 0) {
			fprintf(stderr, "couldn't find DT_PPC64_GLINK\n");
			return -1;
		}

		/* The first glink stub starts at offset 32.  */
		lte->arch.plt_stub_vma = glink_vma + 32;

	} else {
		/* By exhaustion--PPC32 BSS.  */
		if (elf_load_dynamic_entry(lte, DT_PLTGOT,
					   &lib->arch.pltgot_addr) < 0) {
			fprintf(stderr, "couldn't find DT_PLTGOT\n");
			return -1;
		}
	}

	/* On PPC64, look for stub symbols in symbol table.  These are
	 * called: xxxxxxxx.plt_call.callee_name@version+addend.  */
	if (lte->ehdr.e_machine == EM_PPC64
	    && lte->symtab != NULL && lte->strtab != NULL) {

		/* N.B. We can't simply skip the symbols that we fail
		 * to read or malloc.  There may be more than one stub
		 * per symbol name, and if we failed in one but
		 * succeeded in another, the PLT enabling code would
		 * have no way to tell that something is missing.  We
		 * could work around that, of course, but it doesn't
		 * seem worth the trouble.  So if anything fails, we
		 * just pretend that we don't have stub symbols at
		 * all, as if the binary is stripped.  */

		size_t i;
		for (i = 0; i < lte->symtab_count; ++i) {
			GElf_Sym sym;
			if (gelf_getsym(lte->symtab, i, &sym) == NULL) {
				struct library_symbol *sym, *next;
			fail:
				for (sym = lte->arch.stubs; sym != NULL; ) {
					next = sym->next;
					library_symbol_destroy(sym);
					free(sym);
					sym = next;
				}
				lte->arch.stubs = NULL;
				break;
			}

			const char *name = lte->strtab + sym.st_name;

#define STUBN ".plt_call."
			if ((name = strstr(name, STUBN)) == NULL)
				continue;
			name += sizeof(STUBN) - 1;
#undef STUBN

			size_t len;
			const char *ver = strchr(name, '@');
			if (ver != NULL) {
				len = ver - name;

			} else {
				/* If there is "+" at all, check that
				 * the symbol name ends in "+0".  */
				const char *add = strrchr(name, '+');
				if (add != NULL) {
					assert(strcmp(add, "+0") == 0);
					len = add - name;
				} else {
					len = strlen(name);
				}
			}

			char *sym_name = strndup(name, len);
			struct library_symbol *libsym = malloc(sizeof(*libsym));
			if (sym_name == NULL || libsym == NULL) {
			fail2:
				free(sym_name);
				free(libsym);
				goto fail;
			}

			/* XXX The double cast should be removed when
			 * arch_addr_t becomes integral type.  */
			arch_addr_t addr = (arch_addr_t)
				(uintptr_t)sym.st_value + lte->bias;
			if (library_symbol_init(libsym, addr, sym_name, 1,
						LS_TOPLT_EXEC) < 0)
				goto fail2;
			libsym->arch.type = PPC64_PLT_STUB;
			libsym->next = lte->arch.stubs;
			lte->arch.stubs = libsym;
		}
	}

	return 0;
}

static int
read_plt_slot_value(struct process *proc, GElf_Addr addr, GElf_Addr *valp)
{
	/* On PPC64, we read from .plt, which contains 8 byte
	 * addresses.  On PPC32 we read from .plt, which contains 4
	 * byte instructions, but the PLT is two instructions, and
	 * either can change.  */
	uint64_t l;
	/* XXX double cast.  */
	if (proc_read_64(proc, (arch_addr_t)(uintptr_t)addr, &l) < 0) {
		debug(DEBUG_EVENT, "ptrace .plt slot value @%#" PRIx64": %s",
			addr, strerror(errno));
		return -1;
	}

	*valp = (GElf_Addr)l;
	return 0;
}

static int
unresolve_plt_slot(struct process *proc, GElf_Addr addr, GElf_Addr value)
{
	/* We only modify plt_entry[0], which holds the resolved
	 * address of the routine.  We keep the TOC and environment
	 * pointers intact.  Hence the only adjustment that we need to
	 * do is to IP.  */
	if (ptrace(PTRACE_POKETEXT, proc->pid, addr, value) < 0) {
		debug(DEBUG_EVENT, "failed to unresolve .plt slot: %s",
			strerror(errno));
		return -1;
	}
	return 0;
}

enum plt_status
arch_elf_add_func_entry(struct process *proc, struct ltelf *lte,
			const GElf_Sym *sym,
			arch_addr_t addr, const char *name,
			struct library_symbol **ret)
{
#ifndef PPC64_LOCAL_ENTRY_OFFSET
	assert(! lte->arch.elfv2_abi);
#else
	/* With ABIv2 st_other field contains an offset.  */
	 if (lte->arch.elfv2_abi)
		addr += PPC64_LOCAL_ENTRY_OFFSET(sym->st_other);
#endif

	int st_info = GELF_ST_TYPE(sym->st_info);

	if ((lte->ehdr.e_machine != EM_PPC && sym->st_other == 0)
	    || lte->ehdr.e_type == ET_DYN
	    || (st_info == STT_FUNC && ! sym->st_other))
		return PLT_DEFAULT;

	if (st_info == STT_FUNC) {
		/* Put the default symbol to the chain.
		 * The addr has already been updated with
		 * symbol offset  */
		char *full_name = strdup(name);
		if (full_name == NULL) {
			fprintf(stderr, "couldn't copy name of %s: %s\n",
			name, strerror(errno));
			free(full_name);
			return PLT_FAIL;
		}
		struct library_symbol *libsym = malloc(sizeof *libsym);
		if (libsym == NULL
		    || library_symbol_init(libsym, addr, full_name, 1,
					   LS_TOPLT_NONE) < 0) {
			free(libsym);
			delete_symbol_chain(libsym);
			libsym = NULL;
			fprintf(stderr, "Couldn't add symbol %s"
				"for tracing.\n", name);
		}
		full_name = NULL;
		libsym->next = *ret;
		*ret = libsym;
		return PLT_OK;
	}

	bool ifunc = false;
#ifdef STT_GNU_IFUNC
	ifunc = GELF_ST_TYPE(sym->st_info) == STT_GNU_IFUNC;
#endif
	if (! ifunc)
		return PLT_DEFAULT;

	size_t len = vect_size(&lte->plt_relocs);
	size_t i;
	for (i = 0; i < len; ++i) {
		GElf_Rela *rela = VECT_ELEMENT(&lte->plt_relocs, GElf_Rela, i);
		if (sym->st_value == arch_plt_sym_val(lte, i, rela)) {

			char *tmp_name = linux_append_IFUNC_to_name(name);
			struct library_symbol *libsym = malloc(sizeof *libsym);

			/* XXX double cast.  */
			arch_addr_t resolver_addr
				= (arch_addr_t) (uintptr_t) rela->r_addend;

			if (tmp_name == NULL || libsym == NULL
			    || 	library_symbol_init(libsym, resolver_addr,
						    tmp_name, 1,
						    LS_TOPLT_EXEC) < 0) {
			fail:
				free(tmp_name);
				free(libsym);
				return PLT_FAIL;
			}

			if (elf_add_plt_entry(proc, lte, name, rela,
					      i, ret) < 0) {
				library_symbol_destroy(libsym);
				goto fail;
			}

			libsym->proto = linux_IFUNC_prototype();
			libsym->next = *ret;
			*ret = libsym;
			return PLT_OK;
		}
	}

	*ret = NULL;
	return PLT_OK;
}

struct ppc_unresolve_data {
	struct ppc_unresolve_data *self; /* A canary.  */
	GElf_Addr plt_entry_addr;
	GElf_Addr plt_slot_addr;
	GElf_Addr plt_slot_value;
	bool is_irelative;
};

enum plt_status
arch_elf_add_plt_entry(struct process *proc, struct ltelf *lte,
		       const char *a_name, GElf_Rela *rela, size_t ndx,
		       struct library_symbol **ret)
{
	bool is_irelative = reloc_is_irelative(lte->ehdr.e_machine, rela);
	char *name;
	if (! is_irelative) {
		name = strdup(a_name);
	} else {
		GElf_Addr addr = lte->ehdr.e_machine == EM_PPC64
			? (GElf_Addr) rela->r_addend
			: arch_plt_sym_val(lte, ndx, rela);
		name = linux_elf_find_irelative_name(lte, addr);
	}

	if (name == NULL) {
	fail:
		free(name);
		return PLT_FAIL;
	}

	struct library_symbol *chain = NULL;
	if (lte->ehdr.e_machine == EM_PPC) {
		if (default_elf_add_plt_entry(proc, lte, name, rela, ndx,
					      &chain) < 0)
			goto fail;

		if (! lte->arch.secure_plt) {
			/* On PPC32 with BSS PLT, delay the symbol
			 * until dynamic linker is done.  */
			assert(!chain->delayed);
			chain->delayed = 1;
		}

	ok:
		*ret = chain;
		free(name);
		return PLT_OK;
	}

	/* PPC64.  If we have stubs, we return a chain of breakpoint
	 * sites, one for each stub that corresponds to this PLT
	 * entry.  */
	struct library_symbol **symp;
	for (symp = &lte->arch.stubs; *symp != NULL; ) {
		struct library_symbol *sym = *symp;
		if (strcmp(sym->name, name) != 0) {
			symp = &(*symp)->next;
			continue;
		}

		/* Re-chain the symbol from stubs to CHAIN.  */
		*symp = sym->next;
		sym->next = chain;
		chain = sym;
	}

	if (chain != NULL)
		goto ok;

	/* We don't have stub symbols.  Find corresponding .plt slot,
	 * and check whether it contains the corresponding PLT address
	 * (or 0 if the dynamic linker hasn't run yet).  N.B. we don't
	 * want read this from ELF file, but from process image.  That
	 * makes a difference if we are attaching to a running
	 * process.  */

	GElf_Addr plt_entry_addr = arch_plt_sym_val(lte, ndx, rela);
	GElf_Addr plt_slot_addr = rela->r_offset;

	assert(plt_slot_addr >= lte->plt_addr
	       || plt_slot_addr < lte->plt_addr + lte->plt_size);

	plt_entry_addr += lte->bias;
	plt_slot_addr += lte->bias;

	/* Should avoid to do read if dynamic linker hasn't run yet
	 * or allow -1 a valid return code.  */
	GElf_Addr plt_slot_value;
	int rc = read_plt_slot_value(proc, plt_slot_addr, &plt_slot_value);
	if (rc < 0 && !lte->arch.elfv2_abi)
		goto fail;

	struct library_symbol *libsym = malloc(sizeof(*libsym));
	if (libsym == NULL) {
		fprintf(stderr, "allocation for .plt slot: %s\n",
			strerror(errno));
	fail2:
		free(libsym);
		goto fail;
	}

	/* XXX The double cast should be removed when
	 * arch_addr_t becomes integral type.  */
	if (library_symbol_init(libsym,
				(arch_addr_t) (uintptr_t) plt_entry_addr,
				name, 1, LS_TOPLT_EXEC) < 0)
		goto fail2;
	libsym->arch.plt_slot_addr = plt_slot_addr;

	if (rc < 0 || (! is_irelative
		       && (plt_slot_value == plt_entry_addr
			   || plt_slot_value == 0))) {
		libsym->arch.type = PPC_PLT_UNRESOLVED;
		libsym->arch.resolved_value = plt_entry_addr;
	} else {
		/* Mark the symbol for later unresolving.  We may not
		 * do this right away, as this is called by ltrace
		 * core for all symbols, and only later filtered.  We
		 * only unresolve the symbol before the breakpoint is
		 * enabled.  */

		libsym->arch.type = PPC_PLT_NEED_UNRESOLVE;
		libsym->arch.data = malloc(sizeof *libsym->arch.data);
		if (libsym->arch.data == NULL)
			goto fail2;

		libsym->arch.data->self = libsym->arch.data;
		libsym->arch.data->plt_entry_addr = plt_entry_addr;
		libsym->arch.data->plt_slot_addr = plt_slot_addr;
		libsym->arch.data->plt_slot_value = plt_slot_value;
		libsym->arch.data->is_irelative = is_irelative;
	}

	*ret = libsym;
	return PLT_OK;
}

void
arch_elf_destroy(struct ltelf *lte)
{
	struct library_symbol *sym;
	for (sym = lte->arch.stubs; sym != NULL; ) {
		struct library_symbol *next = sym->next;
		library_symbol_destroy(sym);
		free(sym);
		sym = next;
	}
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
	if (read_plt_slot_value(proc, libsym->arch.plt_slot_addr, &value) < 0)
		return;

	/* On PPC64, we rewrite the slot value.  */
	if (proc->e_machine == EM_PPC64)
		unresolve_plt_slot(proc, libsym->arch.plt_slot_addr,
				   libsym->arch.resolved_value);
	/* We mark the breakpoint as resolved on both arches.  */
	mark_as_resolved(libsym, value);

	/* cb_on_all_stopped looks if HANDLER is set to NULL as a way
	 * to check that this was run.  It's an error if it
	 * wasn't.  */
	proc->arch.handler = NULL;

	breakpoint_turn_off(bp, proc);
}

static void
cb_on_all_stopped(struct process_stopping_handler *self)
{
	/* Put that in for dl_plt_update_bp_on_hit to see.  */
	assert(self->task_enabling_breakpoint->arch.handler == NULL);
	self->task_enabling_breakpoint->arch.handler = self;

	linux_ptrace_disable_and_continue(self);
}

static enum callback_status
cb_keep_stepping_p(struct process_stopping_handler *self)
{
	struct process *proc = self->task_enabling_breakpoint;
	struct library_symbol *libsym = self->breakpoint_being_enabled->libsym;

	GElf_Addr value;
	if (read_plt_slot_value(proc, libsym->arch.plt_slot_addr, &value) < 0)
		return CBS_FAIL;

	/* In UNRESOLVED state, the RESOLVED_VALUE in fact contains
	 * the PLT entry value.  */
	if (value == libsym->arch.resolved_value)
		return CBS_CONT;

	debug(DEBUG_PROCESS, "pid=%d PLT got resolved to value %#"PRIx64,
	      proc->pid, value);

	/* The .plt slot got resolved!  We can migrate the breakpoint
	 * to RESOLVED and stop single-stepping.  */
	if (proc->e_machine == EM_PPC64
	    && unresolve_plt_slot(proc, libsym->arch.plt_slot_addr,
				  libsym->arch.resolved_value) < 0)
		return CBS_FAIL;

	/* Resolving on PPC64 consists of overwriting a doubleword in
	 * .plt.  That doubleword is than read back by a stub, and
	 * jumped on.  Hopefully we can assume that double word update
	 * is done on a single place only, as it contains a final
	 * address.  We still need to look around for any sync
	 * instruction, but essentially it is safe to optimize away
	 * the single stepping next time and install a post-update
	 * breakpoint.
	 *
	 * The situation on PPC32 BSS is more complicated.  The
	 * dynamic linker here updates potentially several
	 * instructions (XXX currently we assume two) and the rules
	 * are more complicated.  Sometimes it's enough to adjust just
	 * one of the addresses--the logic for generating optimal
	 * dispatch depends on relative addresses of the .plt entry
	 * and the jump destination.  We can't assume that the some
	 * instruction block does the update every time.  So on PPC32,
	 * we turn the optimization off and just step through it each
	 * time.  */
	if (proc->e_machine == EM_PPC)
		goto done;

	/* Install breakpoint to the address where the change takes
	 * place.  If we fail, then that just means that we'll have to
	 * singlestep the next time around as well.  */
	struct process *leader = proc->leader;
	if (leader == NULL || leader->arch.dl_plt_update_bp != NULL)
		goto done;

	/* We need to install to the next instruction.  ADDR points to
	 * a store instruction, so moving the breakpoint one
	 * instruction forward is safe.  */
	arch_addr_t addr = get_instruction_pointer(proc) + 4;
	leader->arch.dl_plt_update_bp = insert_breakpoint_at(proc, addr, NULL);
	if (leader->arch.dl_plt_update_bp == NULL)
		goto done;

	static struct bp_callbacks dl_plt_update_cbs = {
		.on_hit = dl_plt_update_bp_on_hit,
	};
	leader->arch.dl_plt_update_bp->cbs = &dl_plt_update_cbs;

	/* Turn it off for now.  We will turn it on again when we hit
	 * the PLT entry that needs this.  */
	breakpoint_turn_off(leader->arch.dl_plt_update_bp, proc);

done:
	mark_as_resolved(libsym, value);

	return CBS_STOP;
}

static void
jump_to_entry_point(struct process *proc, struct breakpoint *bp)
{
	/* XXX The double cast should be removed when
	 * arch_addr_t becomes integral type.  */
	arch_addr_t rv = (arch_addr_t)
		(uintptr_t)bp->libsym->arch.resolved_value;
	set_instruction_pointer(proc, rv);
}

static void
ppc_plt_bp_continue(struct breakpoint *bp, struct process *proc)
{
	/* If this is a first call through IREL breakpoint, enable the
	 * symbol so that it doesn't look like an artificial
	 * breakpoint anymore.  */
	if (bp->libsym == NULL) {
		assert(bp->arch.irel_libsym != NULL);
		bp->libsym = bp->arch.irel_libsym;
		bp->arch.irel_libsym = NULL;
	}

	switch (bp->libsym->arch.type) {
		struct process *leader;
		void (*on_all_stopped)(struct process_stopping_handler *);
		enum callback_status (*keep_stepping_p)
			(struct process_stopping_handler *);

	case PPC_DEFAULT:
		assert(proc->e_machine == EM_PPC);
		assert(bp->libsym != NULL);
		assert(bp->libsym->lib->arch.bss_plt_prelinked == 0);
		/* Fall through.  */

	case PPC_PLT_IRELATIVE:
	case PPC_PLT_UNRESOLVED:
		on_all_stopped = NULL;
		keep_stepping_p = NULL;
		leader = proc->leader;

		if (leader != NULL && leader->arch.dl_plt_update_bp != NULL
		    && breakpoint_turn_on(leader->arch.dl_plt_update_bp,
					  proc) >= 0)
			on_all_stopped = cb_on_all_stopped;
		else
			keep_stepping_p = cb_keep_stepping_p;

		if (process_install_stopping_handler
		    (proc, bp, on_all_stopped, keep_stepping_p, NULL) < 0) {
			fprintf(stderr,	"ppc_plt_bp_continue: "
				"couldn't install event handler\n");
			continue_after_breakpoint(proc, bp);
		}
		return;

	case PPC_PLT_RESOLVED:
		if (proc->e_machine == EM_PPC) {
			continue_after_breakpoint(proc, bp);
			return;
		}

#if _CALL_ELF == 2
		continue_after_breakpoint(proc, bp);
#else
		jump_to_entry_point(proc, bp);
		continue_process(proc->pid);
#endif
		return;

	case PPC64_PLT_STUB:
	case PPC_PLT_NEED_UNRESOLVE:
		/* These should never hit here.  */
		break;
	}

	assert(bp->libsym->arch.type != bp->libsym->arch.type);
	abort();
}

/* When a process is in a PLT stub, it may have already read the data
 * in .plt that we changed.  If we detach now, it will jump to PLT
 * entry and continue to the dynamic linker, where it will SIGSEGV,
 * because zeroth .plt slot is not filled in prelinked binaries, and
 * the dynamic linker needs that data.  Moreover, the process may
 * actually have hit the breakpoint already.  This functions tries to
 * detect both cases and do any fix-ups necessary to mend this
 * situation.  */
static enum callback_status
detach_task_cb(struct process *task, void *data)
{
	struct breakpoint *bp = data;

	if (get_instruction_pointer(task) == bp->addr) {
		debug(DEBUG_PROCESS, "%d at %p, which is PLT slot",
		      task->pid, bp->addr);
		jump_to_entry_point(task, bp);
		return CBS_CONT;
	}

	/* XXX There's still a window of several instructions where we
	 * might catch the task inside a stub such that it has already
	 * read destination address from .plt, but hasn't jumped yet,
	 * thus avoiding the breakpoint.  */

	return CBS_CONT;
}

static void
ppc_plt_bp_retract(struct breakpoint *bp, struct process *proc)
{
	/* On PPC64, we rewrite .plt with PLT entry addresses.  This
	 * needs to be undone.  Unfortunately, the program may have
	 * made decisions based on that value */
	if (proc->e_machine == EM_PPC64
	    && bp->libsym != NULL
	    && bp->libsym->arch.type == PPC_PLT_RESOLVED) {
		each_task(proc->leader, NULL, detach_task_cb, bp);
		unresolve_plt_slot(proc, bp->libsym->arch.plt_slot_addr,
				   bp->libsym->arch.resolved_value);
	}
}

static void
ppc_plt_bp_install(struct breakpoint *bp, struct process *proc)
{
	/* This should not be an artificial breakpoint.  */
	struct library_symbol *libsym = bp->libsym;
	if (libsym == NULL)
		libsym = bp->arch.irel_libsym;
	assert(libsym != NULL);

	if (libsym->arch.type == PPC_PLT_NEED_UNRESOLVE) {
		/* Unresolve the .plt slot.  If the binary was
		 * prelinked, this makes the code invalid, because in
		 * case of prelinked binary, the dynamic linker
		 * doesn't update .plt[0] and .plt[1] with addresses
		 * of the resover.  But we don't care, we will never
		 * need to enter the resolver.  That just means that
		 * we have to un-un-resolve this back before we
		 * detach.  */

		struct ppc_unresolve_data *data = libsym->arch.data;
		libsym->arch.data = NULL;
		assert(data->self == data);

		GElf_Addr plt_slot_addr = data->plt_slot_addr;
		GElf_Addr plt_slot_value = data->plt_slot_value;
		GElf_Addr plt_entry_addr = data->plt_entry_addr;

		if (unresolve_plt_slot(proc, plt_slot_addr,
				       plt_entry_addr) == 0) {
			if (! data->is_irelative) {
				mark_as_resolved(libsym, plt_slot_value);
			} else {
				libsym->arch.type = PPC_PLT_IRELATIVE;
				libsym->arch.resolved_value = plt_entry_addr;
			}
		} else {
			fprintf(stderr, "Couldn't unresolve %s@%p.  Will not"
				" trace this symbol.\n",
				breakpoint_name(bp), bp->addr);
			proc_remove_breakpoint(proc, bp);
		}

		free(data);
	}
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

int
arch_library_symbol_init(struct library_symbol *libsym)
{
	/* We set type explicitly in the code above, where we have the
	 * necessary context.  This is for calls from ltrace-elf.c and
	 * such.  */
#if _CALL_ELF == 2
	libsym->arch.type = PPC_PLT_UNRESOLVED;
#else
	libsym->arch.type = PPC_DEFAULT;
#endif
	return 0;
}

void
arch_library_symbol_destroy(struct library_symbol *libsym)
{
	if (libsym->arch.type == PPC_PLT_NEED_UNRESOLVE) {
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
	if (libsym->arch.type == PPC_PLT_NEED_UNRESOLVE) {
		assert(libsym->arch.data->self == libsym->arch.data);
		retp->arch.data = malloc(sizeof *retp->arch.data);
		if (retp->arch.data == NULL)
			return -1;
		*retp->arch.data = *libsym->arch.data;
		retp->arch.data->self = retp->arch.data;
	}
	return 0;
}

/* For some symbol types, we need to set up custom callbacks.  XXX we
 * don't need PROC here, we can store the data in BP if it is of
 * interest to us.  */
int
arch_breakpoint_init(struct process *proc, struct breakpoint *bp)
{
	bp->arch.irel_libsym = NULL;

	/* Artificial and entry-point breakpoints are plain.  */
	if (bp->libsym == NULL || bp->libsym->plt_type != LS_TOPLT_EXEC)
		return 0;

	/* On PPC, secure PLT and prelinked BSS PLT are plain.  */
	if (proc->e_machine == EM_PPC
	    && bp->libsym->lib->arch.bss_plt_prelinked != 0)
		return 0;

	/* On PPC64, stub PLT breakpoints are plain.  */
	if (proc->e_machine == EM_PPC64
	    && bp->libsym->arch.type == PPC64_PLT_STUB)
		return 0;

	static struct bp_callbacks cbs = {
		.on_continue = ppc_plt_bp_continue,
		.on_retract = ppc_plt_bp_retract,
		.on_install = ppc_plt_bp_install,
	};
	breakpoint_set_callbacks(bp, &cbs);

	/* For JMP_IREL breakpoints, make the breakpoint look
	 * artificial by hiding the symbol.  */
	if (bp->libsym->arch.type == PPC_PLT_IRELATIVE) {
		bp->arch.irel_libsym = bp->libsym;
		bp->libsym = NULL;
	}

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
