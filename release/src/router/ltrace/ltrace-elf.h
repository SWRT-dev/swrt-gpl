/*
 * This file is part of ltrace.
 * Copyright (C) 2006,2010,2012,2013 Petr Machata, Red Hat Inc.
 * Copyright (C) 2010 Zachary T Welch
 * Copyright (C) 2001,2004,2007,2009 Juan Cespedes
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

#ifndef LTRACE_ELF_H
#define LTRACE_ELF_H

#include <gelf.h>
#include <stdlib.h>
#include <callback.h>

#include "forward.h"
#include "sysdep.h"
#include "vect.h"

/* XXX Ok, the original idea was to separate the low-level ELF data
 * from the abstract "struct library" object, but we use some of the
 * following extensively in the back end.  Not all though.  So what we
 * use should be move to struct library, and the rest of this
 * structure maybe could be safely hidden in .c.  How to integrate the
 * arch-specific bits into struct library is unclear as of now.  */
struct ltelf {
	int fd;
	Elf *elf;
	GElf_Ehdr ehdr;
	Elf_Data *dynsym;
	size_t dynsym_count;
	const char *dynstr;
	GElf_Addr plt_addr;
	GElf_Word plt_flags;
	size_t plt_size;
	Elf_Data *plt_data;

	/* Vector of GElf_Rela with PLT relocations.  */
	struct vect plt_relocs;

	Elf_Data *symtab;
	const char *strtab;
	const char *soname;
	size_t symtab_count;
	Elf_Data *opd;
	GElf_Addr *opd_addr;
	size_t opd_size;
	GElf_Addr dyn_addr;
	size_t dyn_sz;
	GElf_Addr bias;
	GElf_Addr entry_addr;
	GElf_Addr base_addr;
	struct arch_ltelf_data arch;
};

int ltelf_init(struct ltelf *lte, const char *filename);
void ltelf_destroy(struct ltelf *lte);

/* XXX is it possible to put breakpoints in VDSO and VSYSCALL
 * pseudo-libraries?  For now we assume that all libraries can be
 * opened via a filesystem.  BASE is ignored for ET_EXEC files.  */
int ltelf_read_library(struct library *lib, struct process *proc,
		       const char *filename, GElf_Addr bias);

/* Create a library object representing the main binary.  The entry
 * point address is stored to *ENTRYP.  */
struct library *ltelf_read_main_binary(struct process *proc, const char *path);

/* This is called for every PLT relocation R in ELF file LTE, that
 * ltrace is about to add to a library constructed in process PROC.
 * The corresponding PLT entry is for symbol called NAME, and it's
 * I-th relocation in the file.  *RET shall be initialized and
 * symbol(s) corresponding to the given PLT entry will be added to the
 * front.  Returns 0 for success, or a negative value for failures.
 *
 * This calls os_elf_add_plt_entry and arch_elf_add_plt_entry in the
 * background (see backend.h for documentation).  The arch callback is
 * called first.  If it returns PLT_DEFAULT, the os callback is called
 * next.  If that returns PLT_DEFAULT, default_elf_add_plt_entry is
 * called.  */
int elf_add_plt_entry(struct process *proc, struct ltelf *lte,
		      const char *name, GElf_Rela *rela, size_t idx,
		      struct library_symbol **ret);

/* Create a default PLT entry.  This can be used instead (or in
 * addition to) returning PLT_DEFAULT from arch_elf_add_plt_entry.
 * RET shall be initialized, the created symbol will be added to the
 * beginning of the linked list at *RET.  This function doesn't add
 * the symbol to LTE.  arch_elf_add_plt_entry has the chance to adjust
 * symbol internals to its liking, and then return either PLT_DEFAULT
 * or PLT_OK.  */
int default_elf_add_plt_entry(struct process *proc, struct ltelf *lte,
			      const char *a_name, GElf_Rela *rela, size_t ndx,
			      struct library_symbol **ret);

Elf_Data *elf_loaddata(Elf_Scn *scn, GElf_Shdr *shdr);

/* The following three look for sections based on various criteria.
 * They return 0 if there was no error, or a negative value if there
 * was.  If the section was found, it is returned in *TGT_SEC, and the
 * header is stored to TGT_SHDR.  If it wasn't found, *TGT_SEC is set
 * to NULL.  */
int elf_get_section_covering(struct ltelf *lte, GElf_Addr addr,
			     Elf_Scn **tgt_sec, GElf_Shdr *tgt_shdr);
int elf_get_section_type(struct ltelf *lte, GElf_Word type,
			 Elf_Scn **tgt_sec, GElf_Shdr *tgt_shdr);
int elf_get_section_named(struct ltelf *lte, const char *name,
			  Elf_Scn **tgt_sec, GElf_Shdr *tgt_shdr);

/* Iterate through all symbols in LTE.  See callback.h for notes on
 * iteration interfaces.  START_AFTER is 0 in initial call.  */
struct elf_each_symbol_t {
	unsigned restart;
	int status;
} elf_each_symbol(struct ltelf *lte, unsigned start_after,
		  enum callback_status (*cb)(GElf_Sym *symbol,
					     const char *name,
					     void *data),
		  void *data);

/* Read relocations from relocation section SCN with header SHDR and
 * add them to RELA_VEC, which is a vector of GElf_Rela.  Return 0 on
 * success, or a negative value on failure.  */
int elf_read_relocs(struct ltelf *lte, Elf_Scn *scn, GElf_Shdr *shdr,
		    struct vect *rela_vec);

/* Read a given DT_ TAG from LTE.  Value is returned in *VALUEP.
 * Returns 0 on success or a negative value on failure.  */
int elf_load_dynamic_entry(struct ltelf *lte, int tag, GElf_Addr *valuep);

/* Read, respectively, 1, 2, 4, or 8 bytes from Elf data at given
 * OFFSET, and store it in *RETP.  Returns 0 on success or a negative
 * value if there's not enough data.  */
int elf_read_u8(Elf_Data *data, GElf_Xword offset, uint8_t *retp);
int elf_read_u16(Elf_Data *data, GElf_Xword offset, uint16_t *retp);
int elf_read_u32(Elf_Data *data, GElf_Xword offset, uint32_t *retp);
int elf_read_u64(Elf_Data *data, GElf_Xword offset, uint64_t *retp);

/* Read at most 64-bit quantity recorded in an ULEB128 variable-length
 * encoding.  */
int elf_read_uleb128(Elf_Data *data, GElf_Xword offset, uint64_t *retp);

/* These are same as above, but update *OFFSET with the width
 * of read datum.  */
int elf_read_next_u8(Elf_Data *data, GElf_Xword *offset, uint8_t *retp);
int elf_read_next_u16(Elf_Data *data, GElf_Xword *offset, uint16_t *retp);
int elf_read_next_u32(Elf_Data *data, GElf_Xword *offset, uint32_t *retp);
int elf_read_next_u64(Elf_Data *data, GElf_Xword *offset, uint64_t *retp);
int elf_read_next_uleb128(Elf_Data *data, GElf_Xword *offset, uint64_t *retp);

/* Return whether there's AMOUNT more bytes after OFFSET in DATA.  */
int elf_can_read_next(Elf_Data *data, GElf_Xword offset, GElf_Xword amount);

void delete_symbol_chain(struct library_symbol *);
#if __WORDSIZE == 32
#define PRI_ELF_ADDR		PRIx32
#define GELF_ADDR_CAST(x)	(void *)(uint32_t)(x)
#else
#define PRI_ELF_ADDR		PRIx64
#define GELF_ADDR_CAST(x)	(void *)(x)
#endif

#endif
