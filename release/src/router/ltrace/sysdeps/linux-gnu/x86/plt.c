/*
 * This file is part of ltrace.
 * Copyright (C) 2013,2014 Petr Machata, Red Hat Inc.
 * Copyright (C) 2004,2008,2009 Juan Cespedes
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
#include <stdbool.h>

#include "proc.h"
#include "common.h"
#include "library.h"
#include "trace.h"

static GElf_Addr
x86_plt_offset(uint32_t i)
{
	/* Skip the first PLT entry, which contains a stub to call the
	 * resolver.  */
	return (i + 1) * 16;
}

GElf_Addr
arch_plt_sym_val(struct ltelf *lte, size_t ndx, GElf_Rela *rela)
{
	uint32_t i = *VECT_ELEMENT(&lte->arch.plt_map, uint32_t, ndx);
	return x86_plt_offset(i) + lte->plt_addr;
}

void *
sym2addr(struct process *proc, struct library_symbol *sym)
{
	return sym->enter_addr;
}

enum plt_status
arch_elf_add_plt_entry(struct process *proc, struct ltelf *lte,
		       const char *a_name, GElf_Rela *rela, size_t ndx,
		       struct library_symbol **ret)
{
	bool irelative = false;
	if (lte->ehdr.e_machine == EM_X86_64) {
#ifdef R_X86_64_IRELATIVE
		irelative = GELF_R_TYPE(rela->r_info) == R_X86_64_IRELATIVE;
#endif
	} else {
		assert(lte->ehdr.e_machine == EM_386);
#ifdef R_386_IRELATIVE
		irelative = GELF_R_TYPE(rela->r_info) == R_386_IRELATIVE;
#endif
	}

	if (irelative)
		return linux_elf_add_plt_entry_irelative(proc, lte, rela,
							 ndx, ret);

	return PLT_DEFAULT;
}

int
arch_elf_init(struct ltelf *lte, struct library *lib)
{
	VECT_INIT(&lte->arch.plt_map, unsigned int);

	if (vect_reserve(&lte->arch.plt_map, vect_size(&lte->plt_relocs)) < 0) {
		arch_elf_destroy(lte);
		return -1;
	}

	{
		unsigned int i, sz = vect_size(&lte->plt_relocs);
		for (i = 0; i < sz; ++i)
			vect_pushback (&lte->arch.plt_map, &i);
	}

	/* IRELATIVE slots may make the whole situation a fair deal
	 * more complex.  On x86{,_64}, the PLT slots are not
	 * presented in the order of the corresponding relocations,
	 * but in the order it which these symbols are in the symbol
	 * table.  That's static symbol table, which may be stripped
	 * off, not dynsym--that doesn't contain IFUNC symbols at all.
	 * So we have to decode each PLT entry to figure out what
	 * entry it corresponds to.  We need to interpret the PLT
	 * table to figure this out.
	 *
	 * On i386, the PLT entry format is as follows:
	 *
	 *	8048300:   ff 25 0c a0 04 08       jmp    *0x804a00c
	 *	8048306:   68 20 00 00 00          push   $0x20
	 *	804830b:   e9 e0 ff ff ff          jmp    80482f0 <_init+0x30>
	 *
	 * For PIE binaries it is the following:
	 *
	 *	    410:   ff a3 10 00 00 00       jmp    *0x10(%ebx)
	 *	    416:   68 00 00 00 00          push   $0x0
	 *	    41b:   e9 d0 ff ff ff          jmp    3f0 <_init+0x30>
	 *
	 * On x86_64, it is:
	 *
	 *	 400420:   ff 25 f2 0b 20 00       jmpq   *0x200bf2(%rip)        # 601018 <_GLOBAL_OFFSET_TABLE_+0x18>
	 *	 400426:   68 00 00 00 00          pushq  $0x0
	 *	 40042b:   e9 e0 ff ff ff          jmpq   400410 <_init+0x18>
	 *
         * On i386, the argument to push is an offset of relocation to
	 * use.  The first PLT slot has an offset of 0x0, the second
	 * 0x8, etc.  On x86_64, it's directly the index that we are
	 * looking for.
	 */

	/* Here we scan the PLT table and initialize a map of
	 * relocation->slot number in lte->arch.plt_map.  */

	unsigned int i, sz = vect_size(&lte->plt_relocs);
	for (i = 0; i < sz; ++i) {

		GElf_Addr offset = x86_plt_offset(i);

		uint8_t byte;
		if (elf_read_next_u8(lte->plt_data, &offset, &byte) < 0
		    || byte != 0xff
		    || elf_read_next_u8(lte->plt_data, &offset, &byte) < 0
		    || (byte != 0xa3 && byte != 0x25))
			continue;

		/* Skip immediate argument in the instruction.  */
		offset += 4;

		uint32_t reloc_arg;
		if (elf_read_next_u8(lte->plt_data, &offset, &byte) < 0
		    || byte != 0x68
		    || elf_read_next_u32(lte->plt_data,
					 &offset, &reloc_arg) < 0)
			continue;

		if (lte->ehdr.e_machine == EM_386) {
			if (reloc_arg % 8 != 0)
				continue;
			reloc_arg /= 8;
		}

		*VECT_ELEMENT(&lte->arch.plt_map, unsigned int, reloc_arg) = i;
	}

	return 0;
}

void
arch_elf_destroy(struct ltelf *lte)
{
	VECT_DESTROY(&lte->arch.plt_map, uint32_t, NULL, NULL);
}
